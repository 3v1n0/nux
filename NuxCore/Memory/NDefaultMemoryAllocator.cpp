#include "../NKernel.h"


NAMESPACE_BEGIN

INL_IMPLEMENT_GLOBAL_OBJECT(NDefaultMemoryAllocator);

void NDefaultMemoryAllocator::Constructor()
{
    // Register the allocator in the page manager
    // Since this allocator doesn't mark pages, we have to set it as default
}

void NDefaultMemoryAllocator::Destructor()
{


}

void* NDefaultMemoryAllocator::Alloc(unsigned Size)
{
    void *ptr = INL_SYS_MEMORY_MALLOC(Size);
    return ptr;
}

// this is not supported on windows
// TODO-mm: check for support on other platforms
void* NDefaultMemoryAllocator::AllocAligned(t_uint32 Size, t_u8 Alignment)
{
    void *ptr = INL_SYS_MEMORY_MEM_ALIGN(Alignment, Size);
    INL_UNUSED(Alignment);
    // windows has no support for malloc with alignment.
#ifndef _WIN32
    nuxAssert((INL_PTR_TO_INT(ptr) % Alignment) == 0);
#endif
    return ptr;
}

void* NDefaultMemoryAllocator::Realloc(void* pOldPtr, t_uint32 Size)
{
    void *ptr = INL_SYS_MEMORY_REALLOC(pOldPtr, Size);
    return ptr;
}

void* NDefaultMemoryAllocator::ReallocAligned(void *pOldPtr, t_uint32 Size, t_u8 Alignment)
{
    void *ptr = INL_SYS_MEMORY_REALLOC_ALIGN(pOldPtr, Size, Alignment);
    INL_UNUSED(Alignment);
    // windows has no support for malloc with alignment.
#ifndef _WIN32
    nuxAssert((INL_PTR_TO_INT(ptr) % Alignment) == 0);
#endif
    return ptr;
}

t_u32 NDefaultMemoryAllocator::GetMaxAllocationSize() const
{
    return 0xFFFFFFFF;
}

void NDefaultMemoryAllocator::Free(void *pData)
{
    INL_SYS_MEMORY_FREE(pData);
}

t_u32 NDefaultMemoryAllocator::GetAllocatedSize(void* pData, t_u32* pRetHeaderSize) const
{        
    INL_UNUSED(pData);
    t_u32 size = (t_u32)INL_SYS_MEMORY_PTR_SIZE(pData);

    if(pRetHeaderSize)        
        *pRetHeaderSize = 0;

    return size;
}

NAMESPACE_END
