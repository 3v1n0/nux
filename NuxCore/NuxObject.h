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


#ifndef NUXOBJECT_H
#define NUXOBJECT_H

NAMESPACE_BEGIN

//! Base class of heap allocated objects.
class NuxTrackable
{
public:
    bool OwnsTheReference();
    bool IsHeapAllocated() const;
    bool IsDynamic() const;
    virtual void Ref();
    virtual void Unref();

    static std::new_handler set_new_handler(std::new_handler handler);
    static void* operator new(size_t size);

#if (__GNUC__ < 4 && __GNUC_MINOR__ < 4)
    static void* operator new(size_t size, void *ptr);
#endif

    static void operator delete(void *ptr);

protected:
    NuxTrackable();
    virtual ~NuxTrackable() = 0;
    void SetOwnedReference(bool b);

private:
    NuxTrackable(const NuxTrackable&);
    NuxTrackable& operator=(const NuxTrackable&);

    class AllocationList : public std::list<void*>
    {
    public:
        AllocationList();
        ~AllocationList();
    };

    static AllocationList allocation_list_;
    static std::new_handler current_handler_;

    bool m_owns_the_reference;

    template<typename T> friend class Pointer;
};

//! The base class of Nux objects.
class NuxObject: public NuxTrackable
{
public:
    //! Constructor
    NuxObject();
    //! Increase reference count.
    void Ref();
    //! Decrease reference count.
    void Unref();
    //! Destroy the object.
    void Destroy();
    
    //! Returns true if the object was allocated on the heap.
    /*!
        @return True if the object was allocated on the heap.
    */
    bool IsHeapAllocated();

protected:
    //! Private destructor.
    /*
        Private destructor. Ensure that object inheriting from NuxObject can only be allocated on the heap.
        (MEC++ item27)
    */
    virtual ~NuxObject();

private:
    NuxObject(const NuxObject&);
    NuxObject& operator = (const NuxObject&);

    NThreadSafeCounter m_reference_count; //!< Reference counter.
};

NAMESPACE_END

#endif // #ifndef NUXOBJECT_H

