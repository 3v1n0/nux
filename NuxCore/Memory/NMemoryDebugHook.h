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


#ifndef NMEMORYDEBUGHOOK_H
#define NMEMORYDEBUGHOOK_H

NAMESPACE_BEGIN

#define INL_NB_ALLOCATOR_MAX  127
/// MemDebug class is a layer that sits on memory allocators, and used to help debugging memory management
/*! The Memory Debugger is doing a lot of verification on every allocation/free action.
The idea is to allocate more than requested (32 bytes for the header + 4 trailing bytes = 36 bytes)
to keep information on the current memory block. This header contains:
- 2 Pointers for insertion in a double linked list
- The Size of the original block allocated
- The current Status of the block
- The Allocator ID that from which the block belong
- The filename & Line number in the file where the block has been allocated (and then freed)
- The Object type
- A header Checksum

The Memory Debugger keeps track of all the block allocated in a double linked list. That's the way
used to track leak at run-time. There is one distinct list per allocator, so it's possible to verify
leak for a specific allocator, or for all the system.
On allocation, the real size requested to the allocator is bigger to keep the header at the beginning
of the memory block. The size of the header is always a multiple of GEAR_HW_MEM_ALIGN.
The header is initialized, the checksum computed, and the buffer is filled with a known value. This will
be used when the block will be freed to evaluate the percentage of the block that has been used. This system
isn't 100% bullet proof, since there is a small percentage of cases where memory wasted will be detected
and will not be in reality (if the user write in the buffer the same pattern as used as filling value).
There is also 4 extra bytes added at the end of the buffer as a stopper, to ensure that no buffer overflow
occured. The pointer returned is then a value pointing right after the header, and this value is guaranteed
to be aligned.

During run time, a buffer integrity can be verified (Verify alignment, header checksum, allocator matching
and bottom flag integrity). A leak detection can be performed, that consist of simply listing all buffers
currently still allocated.
*/
class MemDebug : public MemoryHook
{
    /// MemDebug is a Global Object
    INL_DECLARE_GLOBAL_OBJECT(MemDebug, NGlobalSingletonInitializer);

public :
    typedef enum {  Invalid         = -1, 
        Unaligned       = -2, 
        Corrupted       = -3, 
        Overflow        = -4, 
        WrongAllocator  = -5, 
        DoubleFree      = -6 
    } Err;

    /// SetAllocFailure allows to simulate a failure in allocation.
    /*! This function available only when the debugger is enabled, allows to simulate allocation
    failure to test user code. The failure is set per Allocator. Any call to Alloc/Realloc/New will
    fail for [NbFailure] allocation in [Delay] calls. Failure sequence aren't queued, so any call
    to SetAllocFailure will change the previous one. To disable any failure simulation, just
    call SetAllocFailure(Allocator, 0, 0) (default params).
    \param Allocator    The allocator that will simulate an alloc fail
    \param Delay        The number of call To Alloc before the failure. 0 = Next call
    \param NbFailure    The number of call to Alloc that must fail in a row.
    \return             None
    \sa GEAR_ALLOC(), GEAR_REALLOC()
    */
    void SetAllocFailure(   const NMemoryAllocatorInterface&     Allocator, 
        unsigned                Delay = 0, 
        unsigned                NbFailure = 0);

    /// Returns the number of buffers still allocated in the specified allocator
    /*! \param AllocId  The allocator we're gonna verify
    \param Verbose  When true, will send a warning on the debug device if a leak is detected
    \return The number of buffer still allocated
    \sa VerifyLeakAll(), GEAR_ALLOC(), GEAR_FREE()
    */
    unsigned VerifyLeak(NMemoryAllocatorInterface::id AllocId, bool Verbose = true);

    /// Returns the number of buffers still allocated in all allocators
    /*! \param Verbose  When true, will send a warning on the debug device if a leak is detected
    \return The total number of buffer still allocated in all buffers
    \sa VerifyLeak(), GEAR_ALLOC(), GEAR_FREE()
    */
    unsigned VerifyLeakAll(bool Verbose = true);

    /// Verify buffer status for the specified allocator
    /*! Here's a list of problems detected on a problematics buffers:
    - Pointer position (Null & Alignment)
    - Double Free
    - Header integrity (Overflow problem slightly before the pointer)
    - Verify allocator matching (The allocator specified is the same as recorded when allocated)
    - Bottom flag (To detect overflow)
    \param AllocId  The allocator we're gonna verify
    \param Verbose  When true, will send a warning on the debug device if a problem is detected
    \return An error code (see MemDebug::Err) or 0 if the buffer is sound.
    \sa VerifyLeakAll(), GEAR_ALLOC(), GEAR_FREE()
    */
    int  VerifyBuffer(NMemoryAllocatorInterface::id AllocId, bool Verbose = true);

    /// Verify buffer status for all allocators
    /*! \param Verbose  When true, will send a warning on the debug device if a problem is detected
    \return The number of allocators that contains problematics buffers
    \sa VerifyBufferAll()
    */
    unsigned  VerifyBufferAll(bool Verbose = true);

    /// Verify buffer status for the specified allocator and the specified buffer pointed on by pBuf
    /*! Here's a list of problems detected on a problematics buffers:
    - Pointer position (Null & Alignment)
    - Double Free
    - Header integrity (Overflow problem slightly before the pointer)
    - Verify allocator matching (The allocator specified is the same as recorded when allocated)
    - Bottom flag (To detect overflow)
    \param pBuf Pointer to the memory block
    \return Block status (see MemDebug::Err) or 0 if the buffer is sound.
    */
    int VerifyPointer(void* pBuf);

    /// Returns the unused memory in an allocated memory block
    /*! When allocated, a block is filled with a known pattern. Starting from the end of the block, 
    we can trace back the pattern until the first byte changed, and evaluate the number of bytes
    "wasted"
    \param pData A pointer on a block of data allocated
    \return The number of bytes unused (wasted)
    */
    virtual unsigned GetWastedMemory(const void* pData) const;

    /// Returns the logagent identifier for the memory debugger
    static const char* GetLogId(void)
    { 
        return "MemoryDebugger"; 
    }

    /// Reset the Leak Threshold value. All allocations done before will not be considered as leak
    t_u32 ResetLeakThreshold(void)
    { 
        t_u32 oldValue = m_LeakThresholdSeqNo;
        m_LeakThresholdSeqNo = m_SequenceNoServer; 
        return oldValue;
    }

    /// Sets the leak threshold value. All allocations made before this sequence number won't be considered as leaks
    /// Set value to 0 to get all unfreed allocations
    void SetLeakThreshold(t_u32 seqNo)
    {
        m_LeakThresholdSeqNo = seqNo; 
    }

    virtual void* Alloc(    int          OpId, 
        unsigned            Size, 
        NMemoryAllocatorInterface&       Allocator, 
        const void*         pParent, 
        const char*         pId,
        const char*         pObjType,
        t_u32               LineNo,
        const TCHAR*        pFileName);

    virtual void* AllocAligned(int       OpId, 
        unsigned            Size, 
        unsigned            Alignment,
        NMemoryAllocatorInterface&       Allocator, 
        const void*         pParent, 
        const char*         pId,
        const char*         pObjType,
        t_u32               LineNo,
        const TCHAR*        pFileName);

    virtual void* Realloc(void*               pOldPtr,
        unsigned            NewMemSizeInByte, 
        NMemoryAllocatorInterface&       Allocator, 
        t_u32               LineNo,
        const TCHAR*         pFileName);

    virtual void* ReallocAligned(void*        pOldPtr,
        unsigned            NewMemSizeInByte, 
        unsigned            Alignment, 
        NMemoryAllocatorInterface&       Allocator, 
        t_u32               LineNo,
        const TCHAR*         pFileName);

    virtual void Free(int   OpId,
        void*               pData, 
        NMemoryAllocatorInterface&       Allocator,
        t_u32               LineNo,
        const TCHAR*         pFileName);
    virtual void FreeMarker(void*               pMarker, 
        MemStackInterface&  StackAllocator, 
        t_u32               LineNo, 
        const TCHAR*         pFileName);
    virtual t_u32 GetAllocatedSize(void* pData,                                         
        NMemoryAllocatorInterface&   Allocator, 
        t_u32*              pRetHeaderSize) const;

    virtual void AllocatorDestruction(NMemoryAllocatorInterface& Allocator);

    virtual unsigned GetDebugOverHead(unsigned alignment) const;
    /*
    /// We use the default behavior of theses virtual functions. It's intentionally NOT implemented
    virtual void* GetMarker(MemStackInterface&  StackAllocator, 
    t_u32                 LineNo, 
    const TCHAR*         pFileName);        
    */

    t_u32 GetAllocationNumber(const void *pData) const;

    // Callback used by the client for validation
    typedef void (*OnAllocCallbackType)(void * userAddr, size_t size);
    typedef void (*OnFreeCallbackType) (void * userAddr, size_t size);
    void SetOnAllocCallback(OnAllocCallbackType callback);
    void SetOnFreeCallback(OnFreeCallbackType callback);

private:
    void            FreeBlock(          int                 OpId,
        void*               pData,
        NMemoryAllocatorInterface&       Allocator,
        t_u32                 LineNo,
        const TCHAR*         pFileName);
    int             VerifyBlock(        void*               pBuf,
        NMemoryAllocatorInterface::id    AllocId,
        bool                Verbose = true);
    bool            VerifyAllocFailure( NMemoryAllocatorInterface::id    AllocId);
    t_u32             CalculateDebugSize( t_u32                 Size, t_u32 Alignment) const;
    void*           SecondStageAlloc(   void*               pBuffer,
        int                 OpId,
        unsigned            Size, 
        unsigned            Alignment,
        NMemoryAllocatorInterface::id    AllocId,                                                                                         
        t_u32                 LineNo,
        const TCHAR*         pFileName,
        t_u32                 sequenceNo);       
    void            VerifyMemoryWasted( const void* pBuf);

    class dbg_header
    {    
    public:
        t_u32             FrontFlag;      // Flag at the beginning of the packet
        NListNoDyn<dbg_header>::Node m_Node;
        //INL_NLISTNODYN_NODE(dbg_header); // Double linked list header
    public:
        t_u32             Size;           // Size of the user buffer
        t_u16             LineNo;         // Line number from allocator
        t_u8              Status;         // Status of block
        t_u8              AllocId;        // Allocator Identifier
        const TCHAR*      pFileName;      // Allocator's FileName
        t_u32             SequenceNo;     // Sequential number
        t_u32             Checksum;       // Header checksum
    };

    class data_per_allocator
    {       
    public:
        NListNoDyn<dbg_header>   AllocatedList;  // List of allocated block
        unsigned                FailureDelay;   // Delay before next failure
        unsigned                FailureCount;   // Number of successive failure
    };

    data_per_allocator  m_AllocatorTbl[INL_NB_ALLOCATOR_MAX];
    NCriticalSection    m_Lock;
    t_u32               m_SequenceNoServer;     // On every alloc we increase this counter
    t_u32               m_LeakThresholdSeqNo;   // All alloc before this value is not considered Leak

    OnAllocCallbackType     OnAllocCallback; // The pointer on client AllocCallback 
    OnFreeCallbackType      OnFreeCallback;  // The pointer on client FreeCallback
};

NAMESPACE_END

#endif // NMEMORYDEBUGHOOK_H
