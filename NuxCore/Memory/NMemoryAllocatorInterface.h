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


#ifndef NMEMORYALLOCATORINTERFACE_H
#define NMEMORYALLOCATORINTERFACE_H

NAMESPACE_BEGIN

class NMemoryAllocatorInterface
{
    INL_DISABLE_OBJECT_COPY(NMemoryAllocatorInterface);

public:
    NMemoryAllocatorInterface();
    virtual ~NMemoryAllocatorInterface();

    //! Allocate a bloc of memory. The returned block is at least Size bytes.
    /*!
        @param  Size The size of the bloc to allocate.
        @return Pointer to the memory block allocated or 0 if there is not enough memory.
    */
    virtual void* Alloc(t_uint32 Size) = 0;

    //! Allocate an aligned bloc of memory. The returned block is at least Size bytes.
    /*!
        @param  Size The size of the bloc to allocate.
        @param  Alignment Memory alignment requested.
        @return Pointer to the memory block allocated or 0 if there is not enough memory.
    */
    virtual void* AllocAligned(t_uint32 Size, t_u8 Alignment) = 0;

    //! Realloc a bloc of memory. The returned block is at least Size bytes.
    /*!
        @param  pOldPtr Pointer to the block previously allocated with this allocator.
        @param  Size The size of the bloc to allocate.
        @return Pointer to the memory block allocated or 0 if there is not enough memory.
    */
    virtual void* Realloc(void* pOldPtr, t_uint32 Size) = 0;

    //! Allocate an aligned bloc of memory. The returned block is at least Size bytes.
    /*!
        @param  pOldPtr Pointer to the block previously allocated with this allocator.
        @param  Size The size of the bloc to allocate.
        @param  Alignment Memory alignment requested.
        @return Pointer to the memory block allocated or 0 if there is not enough memory.
    */
    virtual void* ReallocAligned(void* pOldPtr, t_uint32 Size, t_u8 Alignment) = 0;

    //! Free a block of memory.
    /*!
        @param  pData Pointer to the block previously allocated with this allocator.
    */
    virtual void Free(void* pData) = 0;

    //! Maximum size that can be allocated by this allocator
    /*!
        @return The maximum size that can be allocated by this allocator on the current system.
    */
    virtual t_u32 GetMaxAllocationSize() const = 0;

    //! Returns the size of a memory block allocated in the heap
    /*!
        @param pData            Pointer to the allocated memory block.
        @param pRetHeaderSize   [Output] [Optional] When specified, contains the size of the data added 
                                by the allocator in front of the block allocated.
        @return                 Return total size allocated in memory including including the header & padding at the end (for alignment purpose).
    */
    virtual t_u32 GetAllocatedSize(void* pData, t_u32* pRetHeaderSize) const = 0;

    //! Minimum alignment guaranteed by this allocator
    /*!
        @return the minimum alignment guaranteed by this allocator.
    */
    virtual t_u32 GetAlignment() const = 0;

    /*!
        @return The name of the allocator.
    */
    virtual const TCHAR* GetName() const = 0;

    /*!
        Returns the Allocator's version
    */
    virtual const char* GetVersion(void) const;

    /*!
        Returns the minimum overhead that can be added to an allocated chunk
    */
    virtual t_u32         GetMinOverhead(void) const;

    /*!
        Returns the maximum overhead that can be added to an allocated chunk
    */
    virtual t_u32         GetMaxOverhead(void) const;

    typedef t_u8          id;         //!< An allocator identifier. Max 255 allocators in the system (U8)
    enum {VOID_ALLOC_ID = 0xFF};

    //! Get the allocator identifier
    /*!
        @return The identifier of the allocator.
    */
    id GetId() const;
    //!
    /*!
        @param Id The identifier of the allocator.
    */
    void SetId(id Id);

    operator id() const;

private:
    // unique identifier for the memory allocator
    t_uint32 m_Identifier;
    t_uint32 m_AllocatedMemory;

};

inline NMemoryAllocatorInterface::NMemoryAllocatorInterface()
:   m_Identifier(0xFFFFFFFF)
,   m_AllocatedMemory(0)
{

}

inline NMemoryAllocatorInterface::~NMemoryAllocatorInterface()
{

}

inline NMemoryAllocatorInterface::id NMemoryAllocatorInterface::GetId() const
{
    return m_Identifier;
}

inline void NMemoryAllocatorInterface::SetId(NMemoryAllocatorInterface::id Id)
{
    m_Identifier = Id;
}

inline const char* NMemoryAllocatorInterface::GetVersion(void) const
{
    return "1.0.0";
}

inline t_u32 NMemoryAllocatorInterface::GetMinOverhead(void) const
{
    return 0;
}

inline t_u32 NMemoryAllocatorInterface::GetMaxOverhead(void) const
{
    return 0;
}

inline NMemoryAllocatorInterface::operator NMemoryAllocatorInterface::id() const                 
{
    return(m_Identifier);    
}

/// The MemStackInterface class behave exactly like the MemInterface interface, with addition of markers
/*! Stack allocators should inherit from this class instead of MemStackInterface class.
*/
class MemStackInterface : public NMemoryAllocatorInterface
{
    INL_DISABLE_OBJECT_COPY(MemStackInterface);

public:       

    /// User must provide the associated page allocator
    MemStackInterface();

    /// Set A marker in the allocator. All allocation done after will be freed when FreeMarker will be called.
    /*! Pure virtual to be implemented by derived allocators. 
    \return A marker value that will be passed in parameters in FreeMarker to free everything allocated since 
    the call to GetMarker().
    */
    virtual void* GetMarker(void)             = 0;

    /// Free all blocks allocated by this allocator since the call to GetMarker() that returned the given value.
    /*! Pure virtual to be implemented by derived allocators. 
    \param pMarker A marker value returned by GetMarker(). All allocation since this call will be deleted.
    */
    virtual void FreeMarker(void* pMarker)   = 0;

    /// Returns the amount of allocated memory in bytes
    virtual t_u32 GetCurrentAllocatedMemory(void) const = 0;
};

inline MemStackInterface::MemStackInterface() 
{

}

NAMESPACE_END

#endif // NMEMORYALLOCATORINTERFACE_H
