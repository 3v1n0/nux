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


#ifndef NMEMORYHOOK_H
#define NMEMORYHOOK_H

#include "NMemoryAllocatorInterface.h"

NAMESPACE_BEGIN

/// This class is the interface to implement a memory hook. 
/*!  A memory hook is a layer that will redirect all allocation/free calls.
When you implement a hook, you need to :
- Derivate from Gear::MemHook
- Your constructor will hook/unhook itself in the memory manager
- Implement all virtual functions you want to override. 
- In your function, if you want to "continue" the call chain downward, just call your inherited function.

The GEAR_ALLOC macro calls all hooks in the chain (if they are enabled) starting with
the last one (the last declared) and finishing with the physical allocation

For example, say you only want to add a hook to add a 32 bit value in front of ALL buffers.
You virtual Alloc() function will look like:
/code
void* MyInheritedHook::Alloc{HookId OpId, unsigned Size, NMemoryAllocatorInterface& Allocator, etc...)
{
return MemoryHook::Alloc(OpId, Size + sizeof(long), NMemoryAllocatorInterface& Allocator, etc...);
}                    
/endcode            
*/
class MemoryHook
{
public:
    MemoryHook()
    {
        //initialize the hook to zero to properly destroy the hooks in their destructors
        m_pNextHook = 0;
    }

    virtual ~MemoryHook() {}

    virtual void* Alloc(
        int                         OpId,
        unsigned                    Size,
        NMemoryAllocatorInterface&  Allocator,
        const void*                 pParent,
        const char*                 pId,
        const char*                 pObjType,
        t_u32                       LineNo,
        const TCHAR*                 pFileName)
    {
        return m_pNextHook->Alloc(OpId, Size, Allocator, pParent, pId, pObjType, LineNo, pFileName);
    }

    virtual void* AllocAligned(
        int                 OpId, 
        unsigned            Size, 
        unsigned            Alignment,
        NMemoryAllocatorInterface&       Allocator, 
        const void*         pParent, 
        const char*         pId,
        const char*         pObjType,
        t_u32                 LineNo,
        const TCHAR*         pFileName)
    {
        return m_pNextHook->AllocAligned(OpId, Size, Alignment, Allocator, pParent, pId, pObjType, LineNo, pFileName);
    }

    virtual void* Realloc(  void*               pOldPtr,
        unsigned            NewMemSizeInByte, 
        NMemoryAllocatorInterface&       Allocator, 
        t_u32                 LineNo,
        const TCHAR*         pFileName)
    {
        return m_pNextHook->Realloc(pOldPtr, NewMemSizeInByte, Allocator, LineNo, pFileName);
    }

    virtual void* ReallocAligned(   void*           pOldPtr,
        unsigned        NewMemSizeInByte, 
        unsigned        Alignment,
        NMemoryAllocatorInterface&   Allocator,
        t_u32             LineNo,
        const TCHAR*     pFileName)
    {
        return m_pNextHook->ReallocAligned(pOldPtr, NewMemSizeInByte, Alignment, Allocator, LineNo, pFileName);
    }
    virtual void Free(      int                 OpId,
        void*               pData, 
        NMemoryAllocatorInterface&       Allocator,
        t_u32                 LineNo,
        const TCHAR*         pFileName)
    {
        m_pNextHook->Free(OpId, pData, Allocator, LineNo, pFileName);
    }

    virtual void* GetMarker(MemStackInterface&  StackAllocator, 
        t_u32                 LineNo, 
        const TCHAR*         pFileName)
    {
        return m_pNextHook->GetMarker(StackAllocator, LineNo, pFileName);
    }
    virtual void FreeMarker(void*               pMarker, 
        MemStackInterface&  StackAllocator, 
        t_u32                 LineNo, 
        const TCHAR*         pFileName)
    {
        m_pNextHook->FreeMarker(pMarker, StackAllocator, LineNo, pFileName);
    }

    virtual t_u32 GetAllocatedSize(void*          pData,                                             
        NMemoryAllocatorInterface&   Allocator, 
        t_u32*            pRetHeaderSize) const
    {
        return m_pNextHook->GetAllocatedSize(pData, Allocator, pRetHeaderSize);
    }

    virtual unsigned GetDebugOverHead(unsigned alignment) const
    {
        return m_pNextHook->GetDebugOverHead(alignment);
    }

    virtual void PushTagContext(const char* pTag) const
    {
        m_pNextHook->PushTagContext(pTag);
    }

    virtual void PopTagContext() const
    {
        m_pNextHook->PopTagContext();
    }

    virtual void LogSnapshot(const char* pLabel) const
    {
        m_pNextHook->LogSnapshot(pLabel);
    }

    virtual void AllocatorDestruction(NMemoryAllocatorInterface& Allocator)
    {
        m_pNextHook->AllocatorDestruction(Allocator);
    }

    virtual unsigned GetWastedMemory(const void* pData) const
    {
        return m_pNextHook->GetWastedMemory(pData);
    }

    void Hook();
    void UnHook();

protected:
    class MemoryHook*   m_pNextHook;
}; //class MemoryHook

/// This is the default hook, which is the final hook in the chain and that forwards
/// the memory call to the actual allocator.  For internal use only.
class MemoryHookDefault : public MemoryHook
{
public:

    virtual void* Alloc(    int                 /*OpId*/, 
        unsigned            Size, 
        NMemoryAllocatorInterface&       Allocator, 
        const void*         /*pParent*/, 
        const char*         /*pId*/,
        const char*         /*pObjType*/,
        t_u32               /*LineNo*/,
        const TCHAR*         /*pFileName*/)
    {
        void* ptr = Allocator.Alloc(Size);
        nuxAssertMsg( ptr != NULL, TEXT("Allocation Failed!"));
        return ptr;
    }

    virtual void* AllocAligned(    
        int                 /*OpId*/, 
        unsigned            Size, 
        unsigned            alignment,
        NMemoryAllocatorInterface&       Allocator, 
        const void*         /*pParent*/, 
        const char*         /*pId*/,
        const char*         /*pObjType*/,
        t_u32               /*LineNo*/,
        const TCHAR*         /*pFileName*/)
    {
        void* ptr = Allocator.AllocAligned(Size, alignment);
        nuxAssertMsg( ptr != NULL, TEXT("Allocation Failed!"));
        return ptr;
    }

    virtual void* Realloc(  void*               pOldPtr,
        unsigned            NewMemSizeInByte, 
        NMemoryAllocatorInterface&       Allocator, 
        t_u32                 /*LineNo*/,
        const TCHAR*         /*FileName*/)
    {
        void* ptr = Allocator.Realloc(pOldPtr, NewMemSizeInByte);
        nuxAssertMsg( ptr != NULL, TEXT("Allocation Failed!"));
        return ptr;
    }

    virtual void* ReallocAligned(   void*           pOldPtr,
        unsigned        NewMemSizeInByte,
        unsigned        Alignment,
        NMemoryAllocatorInterface&   Allocator,
        t_u32             /*LineNo*/,
        const TCHAR*     /*pFileName*/)
    {
        void* ptr = Allocator.ReallocAligned(pOldPtr, NewMemSizeInByte, Alignment);
        nuxAssertMsg(ptr != NULL, TEXT("Allocation Failed!"));
        return ptr;
    }

    virtual void Free(      int                 /*OpId*/,
        void*               pData,
        NMemoryAllocatorInterface&       Allocator,
        t_u32               /*LineNo*/,
        const TCHAR*         /*pFileName*/)
    {
        Allocator.Free(pData);
    }

    virtual void* GetMarker(MemStackInterface&  StackAllocator, 
        t_u32                 /*LineNo*/, 
        const TCHAR*         /*pFileName*/)
    {
        return StackAllocator.GetMarker();
    }

    virtual void FreeMarker(void*               pMarker, 
        MemStackInterface&  StackAllocator, 
        t_u32                 /*LineNo*/, 
        const TCHAR*         /*pFileName*/)
    {
        StackAllocator.FreeMarker(pMarker);
    }

    virtual t_u32 GetAllocatedSize(   void*           pData,                                                 
        NMemoryAllocatorInterface&   Allocator, 
        t_u32*            pRetHeaderSize) const
    {
        return Allocator.GetAllocatedSize(pData, pRetHeaderSize);
    }

    virtual unsigned GetDebugOverHead(unsigned /*alignment*/) const
    {
        return 0;
    }

    virtual void PushTagContext(const char*) const
    {                   
    }

    virtual void PopTagContext() const
    {
    }

    virtual void LogSnapshot(const char*) const
    {                   
    }

    virtual void AllocatorDestruction(NMemoryAllocatorInterface& )
    {
    }

};

/// This global object is the memory hook manager.
/// Its primary role is to return the first hook in the chain so that we can pass through
/// all our hooks
/// \sa MemoryHook
class MemHook
{
    INL_DECLARE_GLOBAL_OBJECT(MemHook, NGlobalSingletonInitializer)

public:
    MemoryHook* Hook(MemoryHook* pNewMemoryHook)    
    { 
        nuxAssert(pNewMemoryHook);
        MemoryHook* pRetVal = m_pRootHook;
        m_pRootHook = pNewMemoryHook;
        return pRetVal;
    }
    void UnHook(MemoryHook* pOldHook, MemoryHook* pNewHook)
    {
        // If you get this assert, it's because you don't hook in the exact reverse order
        // you hooked
        nuxAssert(pOldHook == m_pRootHook);
        INL_UNUSED(pOldHook); //to remove warning in release mode
        m_pRootHook = pNewHook;                                
    }

    MemoryHook* GetTopHook(void) 
    {
        return m_pRootHook;
    }                
    MemoryHook*         m_pRootHook;
    MemoryHookDefault   m_DefaultHook;
};

NAMESPACE_END

#endif // NMEMORYHOOK_H

