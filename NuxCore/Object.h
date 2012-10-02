// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2010-2011 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef NUXCORE_OBJECT_H
#define NUXCORE_OBJECT_H

#include <string>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <boost/utility.hpp>
#include "ObjectType.h"
#include "Property.h"
#include "PropertyTraits.h"

#define NUX_FILE_LINE_PROTO     const char* __Nux_FileName__=__FILE__, int __Nux_LineNumber__ = __LINE__
#define NUX_FILE_LINE_DECL      const char* __Nux_FileName__, int __Nux_LineNumber__
#define NUX_FILE_LINE_PARAM     __Nux_FileName__, __Nux_LineNumber__
#define NUX_TRACKER_LOCATION    __FILE__, __LINE__

#define OnDestroyed object_destroyed


namespace nux
{
  template <typename T>
  class ObjectPtr;

  template <typename T>
  class ObjectWeakPtr;

  class ObjectStats
  {
    NUX_DECLARE_GLOBAL_OBJECT (ObjectStats, GlobalSingletonInitializer);
  public:
    typedef std::list<void*> AllocationList;
    AllocationList _allocation_list;
    int _total_allocated_size;  //! Total allocated memory size in bytes.
    int _number_of_objects;     //! Number of allocated objects;
  };

#define GObjectStats NUX_GLOBAL_OBJECT_INSTANCE(nux::ObjectStats)

//! Base class of heap allocated objects.
  /*!
      Trackable does not implement reference counting. It only defines the API. It is up
      to the class that inherit from Trackable to implement the reference counting.
  */
  class Trackable: public nux::Introspectable, public sigc::trackable, public boost::noncopyable
  {
  public:
    NUX_DECLARE_ROOT_OBJECT_TYPE (Trackable);
    //! Test if object reference is owned.
    /*
        @return True if the object reference is owned.
    */
    bool OwnsTheReference();

    //! Test if object was allocated dynamically.
    /*
        @return True if the object was allocated dynamically.
    */
    bool IsHeapAllocated();

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

        @return True if the object has been referenced.
    */
    virtual bool Reference();

    //! Decrease the reference count.
    /*!
        @return True if the object has been destroyed
    */
    virtual bool UnReference();

    //! Mark the object as owned.
    /*!
        If this object is not owned, calling SinkReference() as the same
        effect as calling Reference().

        @return True if the object was not owned previously
    */
    virtual bool SinkReference();

    //! Destroy and object that has a floating reference.
    /*!
        @return True if the object has been destroyed
    */
    virtual bool Dispose();

    //! Return the size of allocated for this object.
    /*!
        @return The size allocated for this object.
    */
    virtual int GetObjectSize ();

    static std::new_handler set_new_handler (std::new_handler handler);
    static void *operator new (size_t size);

#if (__GNUC__ < 4 && __GNUC_MINOR__ < 4)
    static void *operator new (size_t size, void *ptr);
#endif

    static void operator delete (void *ptr);

  protected:
    Trackable();
    virtual ~Trackable() = 0;
    void SetOwnedReference (bool b);
    int _heap_allocated;

  private:
    static std::new_handler _new_current_handler;

    bool _owns_the_reference;
    int _size_of_this_object;
  };

//! The base class of Nux objects.
  class Object: public Trackable
  {
  public:
    NUX_DECLARE_OBJECT_TYPE (BaseObject, Trackable);

    //! Constructor
    Object (bool OwnTheReference = true, NUX_FILE_LINE_PROTO);
    //! Increase reference count.
    /*!
        @return True if the object has successfully referenced.
    */
    bool Reference();

    //! Decrease reference count.
    /*!
        @return True if the object reference count has reached 0 and the
        object has been destroyed.
    */
    bool UnReference();

    //! Mark the object as owned.
    /*!
        If this object is not owned, calling SinkReference() as the same
        effect as calling Reference().

        @return True if the object was not owned previously
    */
    virtual bool SinkReference();

    //! Destroy and object that has a floating reference.
    /*!
        @return True is the object has been destroyed
    */
    virtual bool Dispose();

    //! Get the reference count of this object.
    /*!
        @return The reference count of this object.
    */
    int GetReferenceCount() const;

    //! Get the number of ObjectPtr holding this object.
    /*!
        @return The number of ObjectPtr holding this object.
    */
    int ObjectPtrCount() const;

    //! Signal emitted immediately before the object is destroyed.
    sigc::signal <void, Object *> object_destroyed;

    std::string GetAllocationLocation() const;
    std::string GetTypeName() const;

  protected:
    //! Private destructor.
    /*
        Private destructor. Ensure that Object cannot be created on the stack
        (only on the heap), but objects that inherits from Object can stil be
        created on the stack or on the heap.  (MEC++ item27)
    */
    virtual ~Object();

  private:
    //! Destroy the object.
    void Destroy();

    Object (const Object &);
    Object &operator = (const Object &);

    NThreadSafeCounter* reference_count_;
    //!< Number of ObjectPtr hosting the object.
    NThreadSafeCounter* objectptr_count_;

    std::string allocation_location_;
    std::string allocation_stacktrace_;

    template <typename T>
    friend class ObjectPtr;

    template <typename T>
    friend class ObjectWeakPtr;
    friend class ObjectStats;
  };

}

#endif // NUXOBJECT_H

