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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "NuxCore.h"
#include "Object.h"
#include "SmartPtr/IntrusiveSP.h"

namespace nux
{

  NUX_IMPLEMENT_ROOT_OBJECT_TYPE (Trackable);
  NUX_IMPLEMENT_OBJECT_TYPE (Object);
  NUX_IMPLEMENT_GLOBAL_OBJECT (ObjectStats);

//   int ObjectStats::_total_allocated_size = 0;
//   int ObjectStats::_number_of_objects = 0;
//   ObjectStats::AllocationList ObjectStats::_allocation_list;

  ObjectStats::AllocationList::AllocationList()
  {

  }

  ObjectStats::AllocationList::~AllocationList()
  {

  }

  void ObjectStats::Constructor()
  {
    _total_allocated_size = 0;
    _number_of_objects = 0;
  }

  void ObjectStats::Destructor()
  {
    nuxDebugMsg (TEXT("[ObjectStats::Destructor] %d undeleted objects."), _number_of_objects);

#if defined(NUX_DEBUG)
    AllocationList::iterator it;
    for (it = _allocation_list.begin(); it != _allocation_list.end(); it++)
    {
      Object* obj = NUX_STATIC_CAST (Object*, (*it));
      nuxDebugMsg (TEXT("Undeleted object: Type %s, %s line %d"), obj->Type().m_Name, obj->m_allocation_file_name.GetTCharPtr(), obj->m_allocation_line_number);

    }
#endif
  }

  std::new_handler Trackable::m_new_current_handler = 0;

  Trackable::Trackable()
  {
    _owns_the_reference = false;
  }

  Trackable::~Trackable()
  {

  }

  void Trackable::Reference()
  {

  }

  bool Trackable::UnReference()
  {
    return false;
  }

  bool Trackable::SinkReference()
  {
    return false;
  }

  bool Trackable::Dispose()
  {
    return false;
  }

  bool Trackable::OwnsTheReference()
  {
    return _owns_the_reference;
  }

  void Trackable::SetOwnedReference (bool b)
  {
    if (_owns_the_reference == true)
    {
      nuxDebugMsg (TEXT ("[Trackable::SetOwnedReference] Do not change the ownership if is already set to true!") );
      return;
    }

    _owns_the_reference = b;
  }

  std::new_handler
  Trackable::set_new_handler (std::new_handler handler)
  {
    std::new_handler old_handler = m_new_current_handler;
    m_new_current_handler = handler;
    return old_handler;
  }

  void*
  Trackable::operator new (size_t size)
  {
    // Set the new_handler for this call
    std::new_handler global_handler  = std::set_new_handler (m_new_current_handler);

    // If allocation fails m_new_current_handler is called, if specified, otherwise the global new_handler is called.
    void *ptr;

    try
    {
      ptr = ::operator new (size);
      // Clear the object memory region to 0, so we can set _size_of_this_object and _heap_allocated.
      Memset (ptr, 0, size);

      GObjectStats._allocation_list.push_front (ptr);
      NUX_STATIC_CAST (Trackable *, ptr)->_size_of_this_object = size;
      NUX_STATIC_CAST (Trackable *, ptr)->_heap_allocated = true;
      GObjectStats._total_allocated_size += size;
      ++GObjectStats._number_of_objects;
    }
    catch (std::bad_alloc &)
    {
      std::set_new_handler (global_handler);
      throw;
    }

    //  Reset gloabal new_handler
    std::set_new_handler (global_handler);
    return ptr;
  }

#if (__GNUC__ < 4 && __GNUC_MINOR__ < 4)

  void *Trackable::operator new (size_t size, void *ptr)
  {
    return ::operator new (size, ptr);
  }

#endif

  void Trackable::operator delete (void *ptr)
  {
    ObjectStats::AllocationList::iterator i = std::find (GObjectStats._allocation_list.begin(), GObjectStats._allocation_list.end(), ptr);

    if (i != GObjectStats._allocation_list.end() )
    {
      GObjectStats._total_allocated_size -= NUX_STATIC_CAST (Trackable *, ptr)->_size_of_this_object;
      --GObjectStats._number_of_objects;
      GObjectStats._allocation_list.erase (i);
      ::operator delete (ptr);
    }
  }

  bool Trackable::IsHeapAllocated () const
  {
    return _heap_allocated;
    //return IsDynamic ();
  }

  bool Trackable::IsDynamic () const
  {
    // Get pointer to beginning of the memory occupied by this.
    const void *ptr = dynamic_cast<const void *> (this);

    // Search for ptr in allocation_list
    ObjectStats::AllocationList::iterator i = std::find (GObjectStats._allocation_list.begin(), GObjectStats._allocation_list.end(), ptr);
    return i != GObjectStats._allocation_list.end();
  }

//////////////////////////////////////////////////////////////////////

  Object::Object (bool OwnTheReference, NUX_FILE_LINE_DECL)
  {
// #if defined(NUX_DEBUG)
    m_allocation_file_name      = __Nux_FileName__;
    m_allocation_line_number    = __Nux_LineNumber__;
// #endif

    m_reference_count = new NThreadSafeCounter();
    m_weak_reference_count = new NThreadSafeCounter();
    _destroyed = new bool;
    *_destroyed = false;


    m_reference_count->Set (1);
    m_weak_reference_count->Set (1);

    SetOwnedReference (OwnTheReference);
  }

  Object::~Object()
  {
    *_destroyed = true;

    if (IsHeapAllocated ())
    {
      // If the object has properly been UnReference, it should have gone through Destroy(). if that is the case then
      // m_reference_count should be NULL or its value (returned by GetValue ()) should be equal to 0;
      // We can use this to detect when delete is called directly on an object.
      nuxAssertMsg((m_reference_count == 0) || (m_reference_count && (m_reference_count->GetValue () == 0)), TEXT("[Object::~Object] Invalid object destruction. Make sure to call UnReference or Dispose (if the object has never been referenced) on the object."));
      nuxAssertMsg((m_weak_reference_count == 0) || (m_weak_reference_count && (m_weak_reference_count->GetValue () > 0)), TEXT("[Object::~Object] Invalid value of the weak reference count pointer. Make sure to call UnReference or Dispose (if the object has never been referenced) on the object."));

      if ((m_reference_count == 0) && (m_weak_reference_count == 0))
      {
        delete _destroyed;
      }
      else
      {
        // There is a smart pointer holding a weak reference to this object. It is the responsibility
        // of the last smart pointer to delete '_destroyed'.
      }
    }
    else
    {
      delete m_reference_count;
      delete m_weak_reference_count;
      delete _destroyed;
    }
  }

  void Object::Reference()
  {
#if defined (NUX_DEBUG)
    if (!IsHeapAllocated ())
    {
      nuxAssertMsg (0, TEXT("[Object::Reference] Trying to reference an object that was not heap allocated."));
      return;
    }
#endif

    if (m_reference_count->GetValue() == 0)
    {
      nuxAssertMsg (0, TEXT("[Object::Reference] Trying to reference an object that has been delete."));
      return;
    }

    if (!OwnsTheReference() )
    {
      SetOwnedReference (true);
      // The ref count remains at 1. Exit the method.
      return;
    }

    m_reference_count->Increment();
    m_weak_reference_count->Increment();
  }

  bool Object::UnReference()
  {
#if defined (NUX_DEBUG)
    if (!IsHeapAllocated ())
    {
      nuxAssertMsg (0, TEXT("[Object::Reference] Trying to un-reference an object that was not heap allocated."));
      return false;
    }
#endif

    if (!OwnsTheReference() )
    {
      nuxAssertMsg (0, TEXT ("[Object::Unref] Never call Unref on an object with a floating reference. Call Dispose() instead.") );
      return false;
    }

    m_reference_count->Decrement();
    m_weak_reference_count->Decrement();

    if ( (m_reference_count->GetValue() == 0) && IsHeapAllocated () )
    {
      Destroy();
      return true;
    }

    return false;
  }

  bool Object::SinkReference()
  {
#if defined (NUX_DEBUG)
    if (!IsHeapAllocated ())
    {
      nuxAssertMsg (0, TEXT("[Object::Reference] Trying to sink an object that was not heap allocated."));
      return false;
    }
#endif

    if (!OwnsTheReference() )
    {
      SetOwnedReference (true);
      // The ref count remains at 1. Exit the method.
      return true;
    }

    return false;
  }

  bool Object::Dispose()
  {
#if defined (NUX_DEBUG)
    if (!IsHeapAllocated ())
    {
      nuxAssertMsg (0, TEXT("[Object::Reference] Trying to dispose an object that was not heap allocated."));
      return false;
    }
#endif

    if (!OwnsTheReference() && (m_reference_count->GetValue() == 1) )
    {
      m_reference_count->Decrement ();
      m_weak_reference_count->Decrement ();
      Destroy();
      return true;
    }

    nuxAssertMsg (0, TEXT ("[Object::Dispose] Trying to destroy and object that is still referenced") );
    return false;
  }

  void Object::Destroy()
  {
#if defined (NUX_DEBUG)
    if (!IsHeapAllocated ())
    {
      nuxAssertMsg (0, TEXT("[Object::Reference] Trying to destroy an object that was not heap allocated."));
      return;
    }
#endif

    nuxAssert (m_reference_count->GetValue() == 0);

    if ( (m_reference_count->GetValue() == 0) && (m_weak_reference_count->GetValue() == 0) )
    {
      delete m_reference_count;
      delete m_weak_reference_count;
      m_reference_count = 0;
      m_weak_reference_count = 0;
    }
    else
    {
      nuxAssertMsg (m_weak_reference_count, TEXT("[Object::Destroy] Invalid pointer for the weak reference count."));
      nuxAssertMsg ((m_weak_reference_count->GetValue() != 0), TEXT("[Object::Destroy] Invalid value of the weak reference count."));
      //nuxDebugMsg (TEXT("[Object::Destroy] There are weak references pending on this object. This is OK!"));
    }

    delete this;
  }

  void Object::IncrementWeakCounter()
  {
#if defined (NUX_DEBUG)
    if (!IsHeapAllocated ())
    {
      nuxAssertMsg (0, TEXT("[Object::Reference] Trying to increment weak counter on an object that was not heap allocated."));
      return;
    }
#endif

    m_weak_reference_count->Increment();
  }

  void Object::DecrementWeakCounter()
  {
#if defined (NUX_DEBUG)
    if (!IsHeapAllocated ())
    {
      nuxAssertMsg (0, TEXT("[Object::Reference] Trying to decrement weak counter on an object that was not heap allocated."));
      return;
    }
#endif

    m_weak_reference_count->Decrement();
  }

  int Object::GetReferenceCount () const
  {
    return m_reference_count->GetValue();
  }

  int Object::GetWeakReferenceCount () const
  {
    return m_weak_reference_count->GetValue();
  }

}

