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


#ifndef NMEMORYALLOCATOR_H
#define NMEMORYALLOCATOR_H

//#include "NMemoryHook.h"

//
// C style memory allocation stubs that fall back to C runtime
//
#define inlSystemMalloc		malloc
#define inlSystemFree		free
#define inlSystemRealloc	realloc

/* Formal C++ memory allocator: plain, in-place, non-throwing see "#include <new>"
    // Plain
    void* operator new(std::size_t) throw (std::bad_alloc);
    void* operator new[](std::size_t) throw (std::bad_alloc);
    void operator delete(void*) throw();
    void operator delete[](void*) throw();

    // no-throw
    void* operator new(std::size_t, const std::nothrow_t&) throw();
    void* operator new[](std::size_t, const std::nothrow_t&) throw();
    void operator delete(void*, const std::nothrow_t&) throw();
    void operator delete[](void*, const std::nothrow_t&) throw();

    // in-place
    inline void* operator new(std::size_t, void* __p) throw() { return __p; }
    inline void* operator new[](std::size_t, void* __p) throw() { return __p; }
    inline void  operator delete  (void*, void*) throw() { }
    inline void  operator delete[](void*, void*) throw() { }
*/

NAMESPACE_BEGIN


class NMemoryAllocatorInterface;
inline NMemoryAllocatorInterface& GetDefaultMemoryAllocator(void)
{
    return INL_GLOBAL_OBJECT_INSTANCE(NDefaultMemoryAllocator);
}

namespace Memory
{

    template<typename T>
        T* MemHelperAlloc(int MemoryOpType,
        unsigned Count,
        NMemoryAllocatorInterface& Allocator,
        const void* pParent,
        const char* pComment,
        const char* pObjType,
        const char* pFile,
        t_u32 Line,
        const char* pFunctionName)
    { 
        INL_UNUSED(MemoryOpType);
        INL_UNUSED(pParent);
        INL_UNUSED(pComment);
        INL_UNUSED(pObjType);
        INL_UNUSED(Line);
        INL_UNUSED(pFile);
        INL_UNUSED(pFunctionName);

        NCriticalSection cs;
        //T* obj = (T*)Allocator.Alloc(sizeof(T) * (Count));
        T* obj = (T*)INL_GLOBAL_OBJECT_INSTANCE(MemHook).GetTopHook()->Alloc(MemoryOpType,
            Count,
            Allocator,
            pParent,
            pComment,
            pObjType,
            Line,
            pFile
            /*, pFunctionName*/);
        return obj;
    }

    template<typename T>
        inline void MemHelperDelete(int MemoryOpType, T* ptrObject, const char* pFile, t_u32 Line, const char* pFunctionName)
    {        
        if(ptrObject)
        {
            NCriticalSection cs;
            ptrObject->~T();
            //GetDefaultMemoryAllocator().Free(ptrObject);
            INL_GLOBAL_OBJECT_INSTANCE(MemHook).GetTopHook()->Free(MemoryOpType, ptrObject, GetDefaultMemoryAllocator(), Line, pFile);
        }
    }


    template<typename T>
        inline void MemHelperFastDelete(int MemoryOpType, NMemoryAllocatorInterface& Allocator, T* ptrObject, const char* pFile, t_u32 Line)
    {        
        if(ptrObject)
        {
            NCriticalSection cs;
            ptrObject->~T();
            Allocator.Free(ptrObject);
        }
    }

} // namespace Memory

NAMESPACE_END


#endif // NMEMORYALLOCATOR_H

