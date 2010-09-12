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


#ifndef NMEMORY_H
#define NMEMORY_H

#define NUX_DEFAULT_ALIGNMENT 4

#define NUX_SAFE_DELETE(mem)    if(mem)             \
                                {                   \
                                    INLDELETE (mem);   \
                                    (mem) = 0;      \
                                }   
#define NUX_SAFE_DELETE_ARRAY(mem_array)    if(mem_array)               \
                                            {                           \
                                                INLDELETEARRAY (mem_array);  \
                                                (mem_array) = 0;        \
                                            }   

#define NUX_SAFE_FREE(mem)    if(mem)             \
{                   \
    std::free(mem);      \
    (mem) = 0;      \
}   

#if defined(NUX_OS_WINDOWS)
    #define NUX_SYS_MEMORY_MALLOC(size)                     malloc(size)
    #define NUX_SYS_MEMORY_MEM_ALIGN(align, size)           NUX_SYS_MEMORY_MALLOC(size)
    #define NUX_SYS_MEMORY_REALLOC(ptr, size)               realloc(ptr, size)
    #define NUX_SYS_MEMORY_REALLOC_ALIGN(ptr, size, align)  realloc(ptr, size)
    #define NUX_SYS_MEMORY_FREE(ptr)                        free(ptr)
    #define NUX_SYS_MEMORY_PTR_SIZE(ptr)                    _msize(ptr)
#elif defined(NUX_OS_LINUX)
    #define NUX_SYS_MEMORY_MALLOC(size)                     malloc(size)
    #define NUX_SYS_MEMORY_MEM_ALIGN(align, size)           NUX_SYS_MEMORY_MALLOC(size)
    #define NUX_SYS_MEMORY_REALLOC(ptr, size)               realloc(ptr, size)
    #define NUX_SYS_MEMORY_REALLOC_ALIGN(ptr, size, align)  realloc(ptr, size)
    #define NUX_SYS_MEMORY_FREE(ptr)                        free(ptr)
    #define NUX_SYS_MEMORY_PTR_SIZE(ptr)                    0
#elif NUX_PS3
    #define NUX_SYS_MEMORY_MALLOC(size)                     malloc(size)
    #define NUX_SYS_MEMORY_MEM_ALIGN(align, size)           memalign(align, size)
    #define NUX_SYS_MEMORY_REALLOC(ptr, size)               realloc(ptr, size)
    #define NUX_SYS_MEMORY_REALLOC_ALIGN(ptr, size, align)  reallocalign(ptr, size, align)
    #define NUX_SYS_MEMORY_FREE(ptr)                        free(ptr)
    #define NUX_SYS_MEMORY_PTR_SIZE(ptr)                    0
#endif

NAMESPACE_BEGIN

t_u32 Memcmp( const void* Buf1, const void* Buf2, t_u32 Count );

bool MemIsZero( const void* V, t_size Count );

void* Memmove( void* Dest, const void* Src, t_size Count );

void Memset( void* Dest, t_s32 C, t_size Count );

void Memzero( void* Dest, t_size Count );

void Memcpy( void* Dest, const void* Src, t_size Count );

void Memswap( void* Ptr1, void* Ptr2, t_size Size );

//! Check that the alignment is a power of two
bool IsMemoryAligned(void* data, t_u32 alignment);

void* Malloc(t_size Count, t_u32 Alignment = NUX_DEFAULT_ALIGNMENT);
void* Realloc(void* Original, t_size Count, t_u32 Alignment = NUX_DEFAULT_ALIGNMENT);

NAMESPACE_END

    inline void inlFree( void* Original )
    {
        return free( Original );
    }

    /// Memory operation defines
    #define NUX_MEMOP_ALLOC           1
    #define NUX_MEMOP_NEW             2
    #define NUX_MEMOP_NEWARRAY        3
    #define NUX_MEMOP_FREE            4
    #define NUX_MEMOP_DELETE          5
    #define NUX_MEMOP_DELETEARRAY     6


    #define NUX_NEW_EXPLICIT(Allocator, ClassName, Comment, ParentPtr, File, Line, FunctionName)    \
        new(Memory::MemHelperAlloc< ClassName >(NUX_MEMOP_NEW,                 \
                    1,                                                              \
                    Allocator,                                                      \
                    ParentPtr,                                                      \
                    Comment,                                                        \
                    #ClassName,                                                     \
                    File,                                                           \
                    Line,                                                           \
                    FunctionName)) ClassName



    #define NUX_NEW(Allocator, ClassName, Comment, ParentPtr)     NUX_NEW_EXPLICIT(Allocator, ClassName, Comment, ParentPtr, __FILE__, __LINE__, __FUNCTION__)
    #define inlNew(ClassName, Comment, ParentPtr) NUX_NEW(GetDefaultMemoryAllocator(), ClassName, Comment, ParentPtr)

    #define NUX_DELETE_EXPLICIT(ptrObject, File, Line, FunctionName)                \
        Memory::MemHelperDelete(NUX_MEMOP_DELETE, (ptrObject),                 \
                          File,                                                     \
                          Line,                                                     \
                          FunctionName);
    #define NUX_DELETE(ptrObject)     NUX_DELETE_EXPLICIT(ptrObject, __FILE__, __LINE__, __FUNCTION__)
    #define inlDelete(ptrObject) NUX_DELETE(ptrObject)



    #define NUX_ALLOC_EXPLICIT(Allocator, ObjectType, Count, Comment, ParentPtr, File, Line, FunctionName)      \
        Memory::MemHelperAlloc< ObjectType >(NUX_MEMOP_ALLOC,                                              \
                                              Count,                                                            \
                                              Allocator,                                                        \
                                              ParentPtr,                                                        \
                                              Comment,                                                          \
                                              #ObjectType,                                                      \
                                              File,                                                             \
                                              Line,                                                             \
                                              FunctionName)


    #define NUX_ALLOC(Allocator, ObjectType, Count, Comment, ParentPtr, File, Line, FunctionName) NUX_ALLOC_EXPLICIT(Allocator, ObjectType, Count, Comment, ParentPtr, File, Line, FunctionName)
    #define inlMMAlloc(Allocator, ObjectType, Count, Comment, ParentPtr) NUX_ALLOC(Allocator, ObjectType, Count, Comment, ParentPtr, __FILE__, __LINE__, __FUNCTION__)


    #define NUX_FREE_EXPLICIT(Allocator, Ptr, File, Line)       \
        Memory::MemHelperFastDelete( NUX_MEMOP_FREE,       \
                            Allocator,                          \
                            Ptr,                                \
                            File,                               \
                            Line);

    #define NUX_FREE(Allocator, Ptr)     NUX_FREE_EXPLICIT(Allocator, Ptr, __FILE__, __LINE__)
    #define inlMMFree(Allocator, Ptr)      NUX_FREE(Allocator, Ptr)

#endif // NMEMORY_H
