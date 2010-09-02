/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the applicable version of the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of both the GNU Lesser General Public 
 * License version 3 along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#include "../NKernel.h"
#include "../Math/MathUtility.h"
#include "../DataStruct/NList.h"
#include "NMemoryDebugHook.h"

// optional dependency
#ifdef GEAR_CFG_USE_MEMPAGEHOOK_STUB        
#include GEAR_INCLUDE_MEMPAGEHOOK_STUB

// When the mempagehook is used, lets use the its functions
#define __GEAR_GET_ID(pData)        GEAR_MEM_PAGE_HOOK_FIND_ALLOCATOR(pData).GetId()
#define __GEAR_VALIDATE(pData)      GEAR_MEM_PAGE_HOOK_VALIDATE(pData)
#else
// When the mempagehook is not used, lets use the the default behavior
#define __GEAR_GET_ID(pData)            NMemoryAllocatorInterface::VOID_ALLOC_ID
#define __GEAR_VALIDATE(pData)          true
#endif


NAMESPACE_BEGIN

//NOTE on Optimization : It is REALY important that these 2 functions do not get optimized. 
//                       Since they have no end result, they will be emptied by the compiler if optimization is on.
#pragma optimize("", off)
bool IsBadReadPointer(void* ptr, t_u32 memSize) 
{
    bool isBad = true;
    t_u8* ptr8 = (t_u8*)ptr;
    t_u8 val;
#ifdef _WIN32
    __try
#endif
    {        
        for(t_u32 i=0; i < memSize; i++)
            val = *(ptr8+i);
        isBad = false;
    }    
#ifdef _WIN32
    __except (EXCEPTION_EXECUTE_HANDLER) 
    {
    }
#endif
    return isBad;
}

bool IsBadWritePointer(void* ptr, t_u32 memSize)
{
    bool isBad = true;
    t_u8* ptr8 = (t_u8*)ptr;
    t_u8 val;
#ifdef _WIN32
    __try
#endif
    {        
        for(t_u32 i=0; i < memSize; i++)
        {
            val = *(ptr8+i);
            *(ptr8+i) = val;
        }
        isBad = false;
    }    
#ifdef _WIN32
    __except (EXCEPTION_EXECUTE_HANDLER) 
    {
    }
#endif

    return isBad;
}
#pragma optimize("", on)


///Returns the amount of bytes equal to value in the current buffer
t_uaddress GetBufferPatternLength(const t_u8 *startOfBuffer, t_u8 value, t_u32 bufferSize)
{
    const t_u8* endOfBuffer = startOfBuffer + bufferSize;
    const t_u8* rounded = (const t_u8*) INL_ADDRESS_TO_PTR(RoundUp(INL_PTR_TO_ADDRESS(startOfBuffer), sizeof(t_word)));
    const t_u8* pIndex = startOfBuffer;

    //we do the first bytes up until alignment
    while(pIndex < rounded && pIndex < endOfBuffer && (*pIndex == value))
    {
        pIndex++;
    }

    if(*pIndex == value && pIndex < endOfBuffer) //we go till the end!
    {
        //we check word by word which is between 1.2 and 2 times faster than byte per byte
        t_word fullFillValue = value << 24 | value << 16 | value << 8 | value;
        pIndex = rounded;
        rounded = (const t_u8*) INL_ADDRESS_TO_PTR(RoundDown(INL_PTR_TO_ADDRESS(endOfBuffer), sizeof(t_word)));

        while((*(t_word*)pIndex) == fullFillValue && pIndex < rounded)
            pIndex+= sizeof(t_word);

        //we do the last part
        while(*pIndex == value && (char*)pIndex < (char*)endOfBuffer)
            pIndex++;
    }

    return static_cast<t_uaddress>(DiffPointer((void*)pIndex, (void*)startOfBuffer));
}

///Returns the amount of bytes equal to value in the current buffer, starting from the end of the buffer
t_uaddress GetBufferPatternLengthReverse(const t_u8 *startOfBuffer, t_u8 value, t_u32 bufferSize)
{
    const t_u8* endOfBuffer = startOfBuffer + bufferSize;
    const t_u8* rounded = (const t_u8*) INL_ADDRESS_TO_PTR(RoundDown(INL_PTR_TO_ADDRESS(endOfBuffer), sizeof(t_word)));
    const t_u8 *pIndex = endOfBuffer - 1;

    //We perform the first check on unaligned bytes
    while(pIndex >= rounded && *pIndex == value && pIndex >= startOfBuffer)
    {
        pIndex--;
    }

    if(*pIndex == value && pIndex >= startOfBuffer)
    {
        //we check word by word which is between 1.2 and 2 times faster than byte per byte
        t_word fullFillValue = value << 24 | value << 16 | value << 8 | value;

        pIndex = rounded - sizeof(t_word);
        rounded = (const t_u8*) INL_ADDRESS_TO_PTR(RoundUp(INL_PTR_TO_ADDRESS(startOfBuffer), sizeof(t_word)));

        while((*(t_word*)pIndex) == fullFillValue && pIndex >= rounded)
            pIndex-= sizeof(t_word);

        //we do the last part
        pIndex += sizeof(t_word)-1;
        while(*pIndex == value && pIndex >= startOfBuffer)
            pIndex--;
    }

    return static_cast<t_uaddress>(DiffPointer((void*)endOfBuffer, (void*)pIndex)-1);
}

INL_IMPLEMENT_GLOBAL_OBJECT(MemDebug)

// hold values for debugging specific allocation number or address
static t_u32 BreakOnAllocationNumber = 0;
// hold values for debugging specific allocation number or address
static t_uaddress BreakOnAllocationAdress = 0;

#define INL_DEFAULT_MEM_ALIGN           16

    // Definitions
#define FILL_VALUE                      0xFA        // Value used to fill the memory on alloc
#define FULL_FILL_VALUE                 0xFAFAFAFA  // 32bit value used to fill the memory on alloc
#define WIPE_VALUE                      0xFE        // Value used to crush the memory after free
#define STATUS_ALLOC                    0x01        // Value to identify a bloc allocated
#define STATUS_FREE                     0x00        // Value to identify a freed block

#define DBG_HDR_SIZE                    (t_u32)RoundUp(sizeof(dbg_header), INL_DEFAULT_MEM_ALIGN)
#define DBG_TRAILER_SIZE                sizeof(BOTTOM_FLAG)
#define DBG_OVERHEAD_SIZE               (DBG_HDR_SIZE + DBG_TRAILER_SIZE)

    //Status management functions
    // the first bit is for the status,
    // the 2-3 bits are for alloc type
    //the last 5 bits are for the alignment
#define SET_STATUS(pHdr, status)       pHdr->Status = ((pHdr->Status & 0xfe) | (status & 0x01))
#define GET_STATUS(pHdr)               (pHdr->Status & 0x01)

#define SET_ALLOCATION_TYPE(pHdr, allocType) pHdr->Status = ((pHdr->Status & 0xF9) | ((((t_u8)allocType) & 0x03) << 1))
#define GET_ALLOCATION_TYPE(pHdr)      ((pHdr->Status & 0x06)>> 1)

#define SET_ALIGNMENT(pHdr, Alignment)  pHdr->Status = ((pHdr->Status & 0x07) | ((((t_u8)GetLowerPowerOfTwoExponent(Alignment)) & 0x1F) << 3))
#define GET_ALIGNMENT(pHdr)             (1 << ((pHdr->Status & 0xf8) >> 3))

#define COMPUTE_CHECKSUM(pHdr)         (pHdr->FrontFlag + pHdr->Size + pHdr->LineNo + pHdr->Status + pHdr->AllocId)
    // Delimiter in the front of the buffer
    static const t_u32 FRONT_FLAG  = 0xB123456B;
    // Delimiter at the end of the buffer
    static const t_u32 BOTTOM_FLAG = 0xE123456E;

void MemDebug::Constructor(void) 
{    
    m_SequenceNoServer      = 0;    // Sequence start with 0
    m_LeakThresholdSeqNo    = 0;    // By default threshold = 0

    // Clear tables
    for(unsigned i = 0; i < INL_NB_ALLOCATOR_MAX; i++)
    {        
        m_AllocatorTbl[i].FailureDelay    = 0;
        m_AllocatorTbl[i].FailureCount    = 0;
    }

    // Register 
    if(1)
        m_pNextHook = INL_GLOBAL_OBJECT_INSTANCE(MemHook).Hook(this);

    OnAllocCallback = NULL;
    OnFreeCallback = NULL;
}

void MemDebug::Destructor(void)
{
    VerifyLeakAll();

    // Unregister 
    if(m_pNextHook)
    {
        INL_GLOBAL_OBJECT_INSTANCE(MemHook).UnHook(this, m_pNextHook);
        m_pNextHook = 0;
    }
}

void MemDebug::AllocatorDestruction(NMemoryAllocatorInterface& Allocator)
{
    NMemoryAllocatorInterface::id AllocId = Allocator.GetId();
    nuxAssert(AllocId < INL_NB_ALLOCATOR_MAX);

    VerifyLeak(AllocId);
    m_AllocatorTbl[AllocId].AllocatedList.Clear();

    MemoryHook::AllocatorDestruction(Allocator);
}

void* MemDebug::Alloc
    ( 
    int             OpId,
    unsigned        Size, 
    NMemoryAllocatorInterface&   Allocator, 
    const void*     pParent, 
    const char*     pId,
    const char*     pObjType,
    t_u32             LineNo,
    const TCHAR*     pFileName
    )
{
    //we need to, at least, satisfy the allocator alignment
    return AllocAligned(OpId, Size, INL_DEFAULT_MEM_ALIGN, Allocator, pParent, pId, pObjType, LineNo, pFileName);
}

void* MemDebug::AllocAligned
    ( 
    int             OpId,
    unsigned        Size, 
    unsigned        Alignment,
    NMemoryAllocatorInterface&   Allocator, 
    const void*     pParent, 
    const char*     pId,
    const char*     pObjType,
    t_u32             LineNo,
    const TCHAR*     pFileName
    )
{
    void* pData = 0;

    // Break on specific allocation number.
    // To set this value, you can either put a breakpoint in MemDebug::Constructor and set it in memory 
    // OR set it in the code and recompile.
    if (BreakOnAllocationNumber && (BreakOnAllocationNumber == m_SequenceNoServer))
    {
        INL_HARDWARE_BREAK;
    }

    if(!VerifyAllocFailure(Allocator.GetId()))
    {
        if(Alignment <= INL_DEFAULT_MEM_ALIGN)
        {
            pData = MemoryHook::Alloc(  OpId, 
                CalculateDebugSize(Size, Alignment),
                Allocator,
                pParent,
                pId,
                pObjType,
                LineNo,
                pFileName);
        }else
        {
            pData = MemoryHook::AllocAligned(  OpId, 
                CalculateDebugSize(Size, Alignment),
                Alignment,
                Allocator,
                pParent,
                pId,
                pObjType,
                LineNo,
                pFileName);               
        }
        if(pData) 
        {   
            pData = SecondStageAlloc(   pData, 
                OpId,
                Size, 
                Alignment, //Alignment
                Allocator.GetId(),                                                                                          
                LineNo, 
                pFileName,
                m_SequenceNoServer++);
        }
    }

    // Break on specific allocation address.
    // To set this value, you can either put a breakpoint in MemDebug::Constructor and set it in memory 
    // OR set it in the code and recompile.
    if (BreakOnAllocationAdress && (BreakOnAllocationAdress == t_uaddress(pData))) 
    {
        INL_HARDWARE_BREAK;
    }

    if (OnAllocCallback != NULL)
    {
        (*OnAllocCallback)(pData, Size);
    }
    return(pData);
}

void* MemDebug::Realloc
    ( 
    void*           pOldPtr, 
    unsigned        Size,
    NMemoryAllocatorInterface&   Allocator, 
    t_u32             LineNo,
    const TCHAR*     pFileName
    )
{
    //we need to, at least, satisfies the allocator alignment
    return ReallocAligned(pOldPtr, Size, INL_DEFAULT_MEM_ALIGN, Allocator, LineNo, pFileName);
}

void* MemDebug::ReallocAligned
    ( 
    void*           pOldPtr, 
    unsigned        Size,
    unsigned        Alignment,
    NMemoryAllocatorInterface&   Allocator, 
    t_u32             LineNo,
    const TCHAR*     pFileName
    )
{
    void*           pResult = 0;

    int Ret = 0;

    if(pOldPtr)
    {
        Ret = VerifyBlock(pOldPtr, Allocator.GetId());
    }

    // If the current block is valid...
    if(Ret == 0)
    {

        // Find our header
        dbg_header* pHeader = NULL;

        if(pOldPtr)
        {
            pHeader = SubstractPointer<dbg_header*>(pOldPtr, DBG_HDR_SIZE);

            // Allocate a new block...
            // Break on specific allocation number.
            // To set this value, you can either put a breakpoint in MemDebug::Constructor and set it in memory 
            // OR set it in the code and recompile.
            if (BreakOnAllocationNumber && (BreakOnAllocationNumber == pHeader->SequenceNo))
            {
                INL_HARDWARE_BREAK;
            }
        }

        if(Size)
        {
            if(!VerifyAllocFailure(Allocator.GetId()))
            {
                if(Alignment  <= (unsigned)(1 << GetLowerPowerOfTwoExponent(INL_DEFAULT_MEM_ALIGN)))
                {
                    pResult = MemoryHook::Alloc(INL_MEMOP_ALLOC, 
                        CalculateDebugSize(Size,Alignment),
                        Allocator,
                        0,
                        0,
                        0,
                        LineNo,
                        pFileName);   
                }else
                {
                    pResult = MemoryHook::AllocAligned(INL_MEMOP_ALLOC, 
                        CalculateDebugSize(Size,Alignment),
                        Alignment,
                        Allocator,
                        0,
                        0,
                        0,
                        LineNo,
                        pFileName);   
                }


                if(pResult) 
                {   
                    pResult = SecondStageAlloc(pResult, 
                        INL_MEMOP_ALLOC,
                        Size, 
                        Alignment,
                        Allocator.GetId(),                                                                                          
                        LineNo, 
                        pFileName,
#if GEAR_CFG_MEM_DEBUG_ALLOCNUMBER_INCREMENT_ON_REALLOC == GEAR_ENABLE
                        m_SequenceNoServer++);
#else
                        pHeader->SequenceNo);
#endif
                }
            }

        }
        // Break on specific allocation address.
        // To set this value, you can either put a breakpoint in MemDebug::Constructor and set it in memory 
        // OR set it in the code and recompile.
        if (BreakOnAllocationAdress && (BreakOnAllocationAdress == t_uaddress(pResult))) 
        {
            INL_HARDWARE_BREAK;
        }

        if(pOldPtr)
        {
            if(Size && pResult)
            {
                // Copy the data
                Memcpy(pResult, pOldPtr, Min(Size, (unsigned) pHeader->Size));
            }

            // Free the old one
            Free(INL_MEMOP_FREE, pOldPtr, Allocator, LineNo, pFileName);
        }
    }

    return(pResult);
}

//
// VerifyAllocFailure 
// Returns true when we must simulate an allocation failure
//
bool MemDebug::VerifyAllocFailure
    (
    NMemoryAllocatorInterface::id        AllocId
    )
{    
    nuxAssert(AllocId < INL_NB_ALLOCATOR_MAX);

    NScopeLock Scope(&m_Lock);
    if(m_AllocatorTbl[AllocId].FailureDelay > 0)
    {
        // Decrease the delay before verifying if failure occur
        m_AllocatorTbl[AllocId].FailureDelay--;
    }
    else
    {
        // Delay expired, How many fail must be done?
        if(m_AllocatorTbl[AllocId].FailureCount > 0) 
        {
            m_AllocatorTbl[AllocId].FailureCount--;
            return(true);
        }
    }

    return(false);
}

t_u32 MemDebug::CalculateDebugSize(t_u32 Size, t_u32 Alignment) const
{
    return Size + RoundUp(DBG_HDR_SIZE, Alignment)  + DBG_TRAILER_SIZE;
}

void* MemDebug::SecondStageAlloc(
                                 void*                   pData,
                                 int                     optId,
                                 unsigned                Size, 
                                 unsigned                Alignment,
                                 NMemoryAllocatorInterface::id        AllocId, 
                                 t_u32                   LineNo,
                                 const TCHAR*            pFileName,
                                 t_u32                   sequenceNo)
{
    nuxAssert(pData);
    nuxAssert(IsAligned((t_uaddress)pData, 4));


    dbg_header* pHeader = (dbg_header*) pData;

    if(DBG_HDR_SIZE != RoundUp(DBG_HDR_SIZE, Alignment)) //we need to offset everything
    {
        void* offsetData = AddPointer<void*>(pData, RoundUp(DBG_HDR_SIZE, Alignment));
        pHeader = SubstractPointer<dbg_header*>(offsetData, DBG_HDR_SIZE);  
    }

    /* Initialize the header */
    pHeader->m_Node.Clear();
    pHeader->FrontFlag  = FRONT_FLAG;           // Front Flag to identify where block starts
    pHeader->Size       = Size;                 // Block user size
    pHeader->LineNo     = (t_u16) LineNo;         // Line where it was allocated
    // Block status
    SET_STATUS(pHeader, STATUS_ALLOC);         // Alloc or freed?
    SET_ALLOCATION_TYPE(pHeader, optId);       // Alloc, new or new[]
    SET_ALIGNMENT(pHeader, Alignment);          //Alignment, 0 means we don't care

    pHeader->AllocId    = AllocId;              // Allocator Identifier
    pHeader->pFileName  = pFileName;            // Name of File where block has been allocated
    pHeader->SequenceNo = sequenceNo; // Get a sequential alloc number

    // Compute the checksum on header
    pHeader->Checksum     = COMPUTE_CHECKSUM(pHeader);

    // Fill buffer with traceable value...
    Memset(AddPointer<void*>(pHeader, DBG_HDR_SIZE),
        FILL_VALUE, 
        Size);

    /* Set bottom flag */
    Memcpy(AddPointer<void*>(pHeader, DBG_HDR_SIZE + Size),
        &BOTTOM_FLAG, 
        DBG_TRAILER_SIZE);

    // Enter the critical section to ad this buffer at the end of the 
    // double linked-list of allocated blocks.
    m_Lock.Lock();

    m_AllocatorTbl[AllocId].AllocatedList.PushBack(*pHeader);

    m_Lock.Unlock();

    /* Move right after the current header (to user zone) */
    return(AddPointer<void*>(pHeader, DBG_HDR_SIZE)); 
}    


void MemDebug::Free
    (
    int                 OpId,
    void*               pData, 
    NMemoryAllocatorInterface&       Allocator,
    t_u32               LineNo,
    const TCHAR*        pFileName
    )
{    
    if(pData && VerifyBlock(pData, Allocator.GetId()) == 0)
    {
        NScopeLock Scope(&m_Lock);
        FreeBlock(OpId, pData, Allocator, LineNo, pFileName);
    }
}

t_u32 MemDebug::GetAllocatedSize
    (
    void*           pData,     
    NMemoryAllocatorInterface&   Allocator, 
    t_u32*            pRetHeaderSize    
    ) const
{ 
    t_u32 Result = 0;

    if(pData)
    {
        dbg_header* pHeader = SubstractPointer<dbg_header*>(pData, DBG_HDR_SIZE);

        void * ptr = (void*)pHeader;
        t_u32 alignment = GET_ALIGNMENT(pHeader);

        if(alignment > DBG_HDR_SIZE)
        {
            ptr = SubstractPointer<void*>(pData, alignment);
        }

        Result = MemoryHook::GetAllocatedSize(ptr, Allocator, pRetHeaderSize);

        if(Result)
        {
            if(pRetHeaderSize)
                *pRetHeaderSize += Max(DBG_HDR_SIZE, alignment);
        }
    }

    return Result;
}

unsigned MemDebug::GetDebugOverHead(unsigned alignment) const
{
    return CalculateDebugSize(0, alignment) + MemoryHook::GetDebugOverHead(alignment);
}


void MemDebug::FreeBlock
    (
    int                 OpId,
    void*               pData, 
    NMemoryAllocatorInterface&       Allocator,
    t_u32               LineNo,
    const TCHAR*        pFileName
    )
{
    nuxAssert(pData);

    dbg_header* pHeader = SubstractPointer<dbg_header*>(pData, DBG_HDR_SIZE);       

    if(GET_STATUS(pHeader) == STATUS_ALLOC)
    {
        if (OnFreeCallback != NULL)
        {
            (*OnFreeCallback)(pData, pHeader->Size);
        }

        m_AllocatorTbl[Allocator.GetId()].AllocatedList.Remove(*pHeader);

        //Check if the Block was deleted with the correct operator

        if(GET_ALLOCATION_TYPE(pHeader) != OpId - (INL_MEMOP_FREE - INL_MEMOP_ALLOC))
        {
            nuxDebugMsg(TEXT("%s(%u): Error detected on Buffer #%u [0x%08lx]. allocated with %s, but released with %s\n%s(%u): Released with %s\n"),
                IsBadReadPointer((void*)pHeader->pFileName, sizeof(pHeader->pFileName)) ? TEXT("<Bad Pointer>") : ANSI_TO_TCHAR(pHeader->pFileName),
                pHeader->LineNo,
                pHeader->SequenceNo,
                (size_t) pData,
                ((GET_ALLOCATION_TYPE(pHeader) == INL_MEMOP_ALLOC) ? TEXT("Alloc") : ((GET_ALLOCATION_TYPE(pHeader) == INL_MEMOP_NEW)? TEXT("New") : TEXT("New[]"))),
                ((OpId == INL_MEMOP_FREE) ? TEXT("Free") : ((OpId == INL_MEMOP_DELETE) ? TEXT("Delete") : TEXT("Delete[]"))),
                pFileName,
                LineNo,
                ((OpId == INL_MEMOP_FREE) ? TEXT("Free") : ((OpId == INL_MEMOP_DELETE) ? TEXT("Delete") : TEXT("Delete[]")))
                );

            INL_HARDWARE_BREAK;
        }

        SET_STATUS(pHeader, STATUS_FREE);
        pHeader->pFileName      = pFileName;
        pHeader->LineNo         = (t_u16) LineNo;

        /* Recompute the checksum on header */
        pHeader->Checksum       = COMPUTE_CHECKSUM(pHeader);

        Memset(pData, WIPE_VALUE, pHeader->Size);           

        //we get the "real" pointer we allocated
        pData = SubstractPointer<void*>(pData, RoundUp(DBG_HDR_SIZE, GET_ALIGNMENT(pHeader)));

        MemoryHook::Free(OpId, pData, Allocator, LineNo, pFileName);

    }
    else 
    {
        NFileName filename = pFileName;
        nuxDebugMsg(TEXT("Buffer freed 2 times (0x%08lx), %s, LINE %u\n"), (size_t) pData, filename.GetCleanFilename().GetTCharPtr(), LineNo);
        INL_HARDWARE_BREAK;
    }
}

// When freeing a marker, we must also free all the block linked after
void MemDebug::FreeMarker(
    void*               pMarker, 
    MemStackInterface&  StackAllocator,
    t_u32               LineNo,
    const TCHAR*         pFileName
    )
{
    nuxAssert(pMarker);

    NListNoDyn<dbg_header>*    pRoot =  &m_AllocatorTbl[StackAllocator.GetId()].AllocatedList;

    NScopeLock Scope(&m_Lock);
    while(pRoot->Back() >= pMarker)
    {   
        void* pData = AddPointer<void*>(pRoot->Back(), DBG_HDR_SIZE);
        int   Ret   = VerifyBlock(pData, StackAllocator);
        if(Ret == 0)
        {        

            FreeBlock(GET_ALLOCATION_TYPE(pRoot->Back()) + (INL_MEMOP_FREE - INL_MEMOP_ALLOC) , pData, StackAllocator, LineNo, pFileName);
        }
    }

    // Free the real memory
    MemoryHook::FreeMarker(pMarker, StackAllocator, LineNo, pFileName);
}

void MemDebug::SetAllocFailure
    (   
    const NMemoryAllocatorInterface&     Allocator, 
    unsigned                Delay,
    unsigned                NbFailure
    )
{
    NMemoryAllocatorInterface::id     Id = Allocator.GetId();
    nuxAssert(Id < INL_NB_ALLOCATOR_MAX);

    NScopeLock Scope(&m_Lock);

    m_AllocatorTbl[Id].FailureDelay = Delay;
    m_AllocatorTbl[Id].FailureCount = NbFailure;    
}

unsigned MemDebug::VerifyLeak(NMemoryAllocatorInterface::id AllocId, bool Verbose)
{
    dbg_header* pHeader;
    void*		pMemFault;
    unsigned    Result = 0;

    nuxAssert(AllocId < INL_NB_ALLOCATOR_MAX);

    NScopeLock Scope(&m_Lock);

    NListNoDyn<dbg_header>::DIterator Iterator(m_AllocatorTbl[AllocId].AllocatedList);

    while(!Iterator.Empty())
    {
        pHeader = (dbg_header*) Iterator.Current();
        pMemFault = AddPointer<void*>(pHeader, DBG_HDR_SIZE);

        if(pHeader->SequenceNo >= m_LeakThresholdSeqNo)
        {
            if(Verbose)
            {
                nuxDebugMsg(TEXT("%s(%u): Memory leak detected of %d bytes at address 0x%08x. No. %d\n"),
                    IsBadReadPointer((void*)pHeader->pFileName, sizeof(pHeader->pFileName)) ? TEXT("<Bad Pointer>") : pHeader->pFileName,
                    pHeader->LineNo, 
                    pHeader->Size,
                    pMemFault,
                    pHeader->SequenceNo);                               
            }

            Result++;
        }

        ++Iterator;        
    }    

    return(Result);
}

unsigned MemDebug::VerifyLeakAll(bool Verbose)      
{
    unsigned Result = 0;

    for(unsigned i = 0; i < INL_NB_ALLOCATOR_MAX; i++)
    {
        Result += VerifyLeak((NMemoryAllocatorInterface::id) i, Verbose);
    }

    return Result;
}


int MemDebug::VerifyBuffer(NMemoryAllocatorInterface::id AllocId, bool Verbose)
{
    void*       pBuf;
    int         Result = 0;

    nuxAssert(AllocId < INL_NB_ALLOCATOR_MAX);

    NScopeLock Scope(&m_Lock);

    NListNoDyn<dbg_header>::DIterator Iterator(m_AllocatorTbl[AllocId].AllocatedList);

    while((!Iterator.Empty()) && (Result == 0))
    {
        pBuf = AddPointer<void*>(Iterator.Current(), DBG_HDR_SIZE);
        Result = VerifyBlock(pBuf, AllocId, Verbose);
        ++Iterator;
    }    

    return Result;
}

unsigned MemDebug::VerifyBufferAll(bool Verbose)
{
    unsigned Result = 0;

    for(unsigned i = 0; i < INL_NB_ALLOCATOR_MAX; i++)
    {
        if(VerifyBuffer((NMemoryAllocatorInterface::id) i, Verbose) != 0)
        {
            Result++;
        }
    }

    return Result;
}

int MemDebug::VerifyPointer(void* pBuf)
{
    if(!__GEAR_VALIDATE(pBuf))
        return Invalid;

    return VerifyBlock(pBuf, __GEAR_GET_ID(pBuf), false);
}

int MemDebug::VerifyBlock(void* pBuf, NMemoryAllocatorInterface::id AllocId, bool Verbose)
{
    dbg_header* pHeader;
    int Ret = 0;

    if(pBuf)
    {
        if(IsAligned((t_uaddress)pBuf, 4))
        {
            pHeader = SubstractPointer<dbg_header*>(pBuf, DBG_HDR_SIZE);

            if(pHeader->Checksum == COMPUTE_CHECKSUM(pHeader))
            {
                if((AllocId == NMemoryAllocatorInterface::VOID_ALLOC_ID) || (pHeader->AllocId == AllocId))
                {
                    /* Compare bottom flag */
                    if(Memcmp(AddPointer<void*>(pBuf, pHeader->Size), 
                        &BOTTOM_FLAG, 
                        DBG_TRAILER_SIZE))
                    {
                        if(Verbose) 
                        {
                            nuxDebugMsg(TEXT("%s(%u): Overflow detected on Buffer #%u [0x%08lx]. Data overwritten at address 0x%08lx\n"),
                                IsBadReadPointer((void*)pHeader->pFileName, sizeof(pHeader->pFileName)) ? TEXT("<Bad Pointer>"):pHeader->pFileName,
                                pHeader->LineNo,
                                pHeader->SequenceNo,
                                (size_t) pBuf,
                                pHeader->Size + (size_t) pBuf);
                            INL_HARDWARE_BREAK;
                        }

                        Ret = Overflow;                
                    }
                } 
                else 
                {
                    if(Verbose) 
                    {
                        nuxDebugMsg(TEXT("%s(%u): MemDebug called with wrong allocator on Buffer #%u [0x%08lx] (Id=%u instead of Id=%u)\n"),
                            IsBadReadPointer((void*)pHeader->pFileName, sizeof(pHeader->pFileName)) ? TEXT("<Bad Pointer>"):pHeader->pFileName,
                            pHeader->LineNo,
                            pHeader->SequenceNo,
                            (size_t) pBuf,
                            AllocId,
                            pHeader->AllocId);
                        INL_HARDWARE_BREAK;
                    }

                    Ret = WrongAllocator;
                }
            } 
            else 
            {
                if((pHeader->AllocId == AllocId) && (GET_STATUS(pHeader) == STATUS_FREE))
                {
                    if(Verbose) 
                    {
                        nuxDebugMsg(TEXT("%s(%u): MemDebug called with a freed buffer #%u [0x%08lx] <double free?>\n"),
                            IsBadReadPointer((void*)pHeader->pFileName, sizeof(pHeader->pFileName)) ? TEXT("<Bad Pointer>"):pHeader->pFileName,
                            pHeader->LineNo,
                            pHeader->SequenceNo,
                            (size_t) pBuf);
                        INL_HARDWARE_BREAK;
                    }

                    Ret = DoubleFree;
                }
                else
                {
                    if(Verbose) 
                    {
                        nuxDebugMsg(TEXT("MemDebug detects a corrupted header for buffer #%u [0x%08lx] - Checksum: 0x%08X <memory spill?>\n"),
                            pHeader->SequenceNo,
                            (size_t) pBuf,
                            (t_u32)COMPUTE_CHECKSUM(pHeader));
                        INL_HARDWARE_BREAK;
                    }

                    Ret = Corrupted;
                }
            }
        }         
        else 
        {
            if(Verbose) 
            {
                nuxDebugMsg(TEXT("MemDebug detects an unaligned pointer for buffer [0x%08lx]\n"),
                    (size_t) pBuf);
                INL_HARDWARE_BREAK;
            }
            Ret = Unaligned;
        }
    } 
    else 
    {
        if(Verbose) 
        {
            nuxDebugMsg(TEXT("MemDebug called with a NULL pointer\n"));
            INL_HARDWARE_BREAK;
        }
        Ret = Invalid;
    }

    return(Ret);
}

unsigned MemDebug::GetWastedMemory(const void* pData) const
{
    dbg_header*     pHeader;

    if (!pData || !IsAligned((t_uaddress)pData,4))
        return 0;

    pHeader = SubstractPointer<dbg_header*>((void*)pData, DBG_HDR_SIZE);
    nuxAssert(pHeader->Checksum == COMPUTE_CHECKSUM(pHeader));

    t_uaddress returnValue = GetBufferPatternLengthReverse( (const t_u8*)pData, FILL_VALUE, pHeader->Size);

    //just to make sure we don't have some silly overflow on 64-bit addresses and we can safely cast the result
    nuxAssert(returnValue < 0xFFFFFFFF);

    return (unsigned)returnValue;
}

t_u32 MemDebug::GetAllocationNumber(const void *pData) const
{
    if (!pData || !IsAligned((t_uaddress)pData,4))
        return 0;

    // Find our header
    dbg_header* pHeader = SubstractPointer<dbg_header*>((void*)pData, DBG_HDR_SIZE);
    return pHeader->SequenceNo;
}

void MemDebug::SetOnAllocCallback(MemDebug::OnAllocCallbackType callback)
{
    NScopeLock Scope(&m_Lock);
    OnAllocCallback = callback;
}

void MemDebug::SetOnFreeCallback(MemDebug::OnFreeCallbackType callback)
{
    NScopeLock Scope(&m_Lock);
    OnFreeCallback = callback;
}

NAMESPACE_END

//////////////////////////////////////////////////////////////////////////
// Undefs section
// This way, we avoid side-effects when compiling onefiletorulethemall.cpp 
#undef COMPUTE_CHECKSUM
#undef DBG_HDR_SIZE
#undef DBG_OVERHEAD_SIZE
#undef DBG_TRAILER_SIZE
#undef FILL_VALUE
#undef FULL_FILL_VALUE
#undef GET_ALIGNMENT
#undef GET_ALLOCATION_TYPE
#undef GET_STATUS
#undef SET_ALIGNMENT
#undef SET_ALLOCATION_TYPE
#undef SET_STATUS
#undef STATUS_ALLOC
#undef STATUS_FREE
#undef WIPE_VALUE
#undef __GEAR_GET_ID
#undef __GEAR_VALIDATE
//////////////////////////////////////////////////////////////////////////
