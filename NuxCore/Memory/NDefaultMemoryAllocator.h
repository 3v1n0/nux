#ifndef NDEFAULTMEMORYALLOCATOR_H
#define NDEFAULTMEMORYALLOCATOR_H

#include "../NKernel.h"

NAMESPACE_BEGIN

class NDefaultMemoryAllocator: public NMemoryAllocatorInterface
{
    INL_DECLARE_GLOBAL_OBJECT(NDefaultMemoryAllocator, NGlobalSingletonInitializer);
public:
        //! Allocate a bloc of memory. The returned block is at least Size bytes.
    /*!
        @param  Size The size of the bloc to allocate.
        @return Pointer to the memory block allocated or 0 if there is not enough memory.
    */
    virtual void* Alloc(t_uint32 Size);

    //! Allocate an aligned bloc of memory. The returned block is at least Size bytes.
    /*!
        @param  Size The size of the bloc to allocate.
        @param  Alignment Memory alignment requested.
        @return Pointer to the memory block allocated or 0 if there is not enough memory.
    */
    virtual void* AllocAligned(t_uint32 Size, t_u8 Alignment);

    //! Realloc a bloc of memory. The returned block is at least Size bytes.
    /*!
        @param  pOldPtr Pointer to the block previously allocated with this allocator.
        @param  Size The size of the bloc to allocate.
        @return Pointer to the memory block allocated or 0 if there is not enough memory.
    */
    virtual void* Realloc(void* pOldPtr, t_uint32 Size);

    //! Allocate an aligned bloc of memory. The returned block is at least Size bytes.
    /*!
        @param  pOldPtr Pointer to the block previously allocated with this allocator.
        @param  Size The size of the bloc to allocate.
        @param  Alignment Memory alignment requested.
        @return Pointer to the memory block allocated or 0 if there is not enough memory.
    */
    virtual void* ReallocAligned(void* pOldPtr, t_uint32 Size, t_u8 Alignment);

    //! Free a block of memory.
    /*!
        @param  pData Pointer to the block previously allocated with this allocator.
    */
    virtual void Free(void* pData);

    //! Maximum size that can be allocated by this allocator
    /*!
        @return The maximum size that can be allocated by this allocator on the current system.
    */
    virtual t_u32 GetMaxAllocationSize() const;

    //! Returns the size of a memory block allocated in the heap
    /*!
        @param pData            Pointer to the allocated memory block.
        @param pRetHeaderSize   [Output] [Optional] When specified, contains the size of the data added 
                                by the allocator in front of the block allocated.
        @return                 Return total size allocated in memory including including the header & padding at the end (for alignment purpose).
    */
    virtual t_u32 GetAllocatedSize(void* pData, t_u32* pRetHeaderSize) const;

    //! Minimum alignment guaranteed by this allocator
    /*!
        @return the minimum alignment guaranteed by this allocator.
    */
    virtual t_u32 GetAlignment() const {return 8;}

    /*!
        @return The name of the allocator.
    */
    virtual const TCHAR* GetName() const {return TEXT("Default Allocator");}
};

NAMESPACE_END

#endif // NDEFAULTMEMORYALLOCATOR_H

