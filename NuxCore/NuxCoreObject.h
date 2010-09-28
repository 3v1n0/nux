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

namespace nux { //NUX_NAMESPACE_BEGIN

template <typename T>
class IntrusiveSP;

template <typename T>
class IntrusiveWeakSP;

#if defined(NUX_DEBUG)
    #define NUX_FILE_LINE_PROTO     char* __Nux_FileName__=__FILE__, int __Nux_LineNumber__ = __LINE__
    #define NUX_FILE_LINE_DECL      char* __Nux_FileName__, int __Nux_LineNumber__
    #define NUX_FILE_LINE_PARAM     __Nux_FileName__, __Nux_LineNumber__
    #define NUX_TRACKER_LOCATION    __FILE__, __LINE__
#else
    #define NUX_FILE_LINE_PROTO     int __Nux_Dummy__ = 0xD0DECADE
    #define NUX_FILE_LINE_DECL      int __Nux_Dummy__
    #define NUX_FILE_LINE_PARAM     __Nux_Dummy__
    #define NUX_TRACKER_LOCATION    0xD0DECADE
#endif

//! Base class of heap allocated objects.
/*!
    NuxTrackable does not implement reference counting. It only defines the API. It is up
    to the class that inherit from NuxTrackable to implement the reference counting.
*/
class NuxTrackable
{
public:
    NUX_DECLARE_ROOT_OBJECT_TYPE(NuxTrackable);
    //! Test if object reference is owned.
    /*
        @return True if the object reference is owned.
    */
    bool OwnsTheReference();
    
    //! Test if object was allocated dynamically.
    /*
        @return True if the object was allocated dynamically.
    */
    bool IsHeapAllocated() const;
    
    //! Test if object was allocated dynamically.
    /*
        @return True if the object was allocated dynamically.
    */
    bool IsDynamic() const;
    
    //! Increase the reference count.
    /*
        Widget are typically created and added to containers. It is decided that when widgets are created, they should have a floating reference
        and their reference count is set to 1. 
        {
            Button* button = new Button();  // button ref_cout = 1, floating = true;
            container->AddButton(button);   // button has a floating reference; when container call button->ref() the ref count 
                                            // of button remains at 1 but the floating reference is set to false. From now on,
                                            // calling button->ref will always increase the ref count (since button no longer has a floating reference).
        }
            
        It is best to pair calls to ref() with unref() when it comes to widgets. So if a widget was not added to a container and so it still has a
        floating reference, then call Dispose(). Dispose does some sanity check; it verifies that:
             ref_count == 1
             floating == true
        If these conditions are verified, dispose will cause the object to be destroyed.
        Calling unref() on an object that has a floating reference will trigger a warning/error in order to invite the
        developer. The developer can either ref the object first before calling unref or simply not create the widget since it 
        does not appear to have been used.
        
        During development it often happen that one forget to dispose an object with a floating reference. 
        Assuming that all functions that receive a reference counted object properly call ref on the object and that the compiler
        can detect unused variables, then the developer should have a way to detect reference counted objects that are not owned.
        It is up to the developer to properly handle these objects.
    */
    virtual void Reference();
    
    //! Decrease the reference count.
    /*!
        @return True if the object has been destroyed
    */
    virtual bool UnReference();

    //! Mark the object as owned.
    /*!
        @return True if the object was not owned previously
    */
    virtual bool SinkReference();

    //! Destroy and object that has a floating reference.
    /*!
        If this object is not owned, calling SinkReference() as the same effect as calling Reference().
        @return True if the object has been destroyed
    */
    virtual bool Dispose();

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

    static AllocationList m_allocation_list;
    static std::new_handler m_new_current_handler;

    bool m_owns_the_reference;
    static int m_total_allocated_size;  //! Total allocated memory size in bytes.
    static int m_number_of_objects;     //! Number of allocated objects;
    int m_size_of_this_object;

    //template<typename T> friend class Pointer;
};

//! The base class of Nux objects.
class NuxCoreObject: public NuxTrackable
{
public:
    NUX_DECLARE_OBJECT_TYPE(BaseObject, NuxTrackable);

    //! Constructor
    NuxCoreObject(bool OwnTheReference = true, NUX_FILE_LINE_PROTO);
    //! Increase reference count.
    void Reference();
    //! Decrease reference count.
    /*!
        @return True if the object reference count has reached 0 and the object has been destroyed.
    */
    bool UnReference();

    //! Mark the object as owned.
    /*!
        @return True if the object was not owned previously
    */
    virtual bool SinkReference();

    //! Destroy and object that has a floating reference.
    /*!
        If this object is not owned, calling SinkReference() as the same effect as calling Reference().
        @return True is the object has been destroyed
    */
    virtual bool Dispose();

    //! Returns true if the object was allocated on the heap.
    /*!
        @return True if the object was allocated on the heap.
    */
    bool IsHeapAllocated();

    NThreadSafeCounter* m_reference_count; //!< Reference count.
    NThreadSafeCounter* m_weak_reference_count; //!< Weak reference count.

protected:
    //! Private destructor.
    /*
        Private destructor. Ensure that NuxCoreObject cannot be created on the stack (only on the heap), but objects that inherits
        from NuxCoreObject can stil be created on the stack or on the heap.
        (MEC++ item27)
    */
    virtual ~NuxCoreObject();

    void IncrementWeakCounter();
    void DecrementWeakCounter();

private:
    //! Destroy the object.
    void Destroy();

    NuxCoreObject(const NuxCoreObject&);
    NuxCoreObject& operator = (const NuxCoreObject&);


#if defined(NUX_DEBUG)
    NString m_allocation_file_name;
    int     m_allocation_line_number;
#endif

    template <typename T>
    friend class IntrusiveSP;

    template <typename T>
    friend class IntrusiveWeakSP;
};

} //NUX_NAMESPACE_END

#endif // NUXOBJECT_H

