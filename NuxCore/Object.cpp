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

#include "NuxCore.h"
#include "Object.h"
#include "ObjectPtr.h"
#include "Logger.h"

namespace nux
{
DECLARE_LOGGER(logger, "nux.core.object");

  NUX_IMPLEMENT_ROOT_OBJECT_TYPE (Trackable);
  NUX_IMPLEMENT_OBJECT_TYPE (Object);
  NUX_IMPLEMENT_GLOBAL_OBJECT (ObjectStats);

  void ObjectStats::Constructor()
  {
    _total_allocated_size= 0;
    _number_of_objects = 0;
  }

  void ObjectStats::Destructor()
  {
#if defined(NUX_DEBUG)
    if (_number_of_objects)
    {
      std::cerr << "[ObjectStats::Destructor] "
                << _number_of_objects << " undeleted objects.\n\t"
                << _allocation_list.size() << " items in allocation list.\n";
    }

    int index = 0;

#if defined(NUX_OS_WINDOWS)
    // Visual Studio does not support range based for loops.
    for (AllocationList::iterator ptr = _allocation_list.begin();
         ptr != _allocation_list.end(); ++ptr)
    {
      Object* obj = static_cast<Object*>(*ptr);

      std::stringstream sout;
      sout << "\t" << ++index << " Undeleted object: Type "
           << obj->GetTypeName() << ", "
           << obj->GetAllocationLocation() << "\n";

      OutputDebugString(sout.str().c_str());

      std::cerr << sout.str().c_str();
    }
#else
    for (auto ptr : _allocation_list)
    {
      Object* obj = static_cast<Object*>(ptr);
      std::cerr << "\t" << ++index << " Undeleted object: Type "
                << obj->GetTypeName() << ", "
                << obj->GetAllocationLocation() << "\n";
    }
#endif
#endif
  }

  std::new_handler Trackable::_new_current_handler = 0;

  Trackable::Trackable()
  {
    _heap_allocated = -1;

    _owns_the_reference = false;
  }

  Trackable::~Trackable()
  {
  }

  bool Trackable::Reference()
  {
    return false;
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
      LOG_DEBUG(logger) << "Do not change the ownership if is already set to true!" << "\n";
      return;
    }

    _owns_the_reference = b;
  }

  std::new_handler Trackable::set_new_handler (std::new_handler handler)
  {
    std::new_handler old_handler = _new_current_handler;
    _new_current_handler = handler;
    return old_handler;
  }

  void* Trackable::operator new (size_t size)
  {
    // Set the new_handler for this call
    std::new_handler global_handler  = std::set_new_handler (_new_current_handler);

    // If allocation fails _new_current_handler is called, if specified,
    // otherwise the global new_handler is called.
    void *ptr;

    try
    {
      ptr = ::operator new (size);

      GObjectStats._allocation_list.push_front (ptr);
      NUX_STATIC_CAST (Trackable *, ptr)->_size_of_this_object = size;
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
#ifdef NUX_DEBUG
    else
    {
      // Complain quite loudly as this should never happen.
      LOG_ERROR(logger) << "Attempting to delete a pointer we can't find.";
    }
#endif
  }

  bool Trackable::IsHeapAllocated()
  {
    if (_heap_allocated == -1)
    {
      _heap_allocated = IsDynamic();
    }

    return _heap_allocated;
  }

  bool Trackable::IsDynamic() const
  {
    // Get pointer to beginning of the memory occupied by this.
    const void *ptr = dynamic_cast<const void *> (this);

    // Search for ptr in allocation_list
#if defined(NUX_OS_WINDOWS) && !defined(NUX_VISUAL_STUDIO_2010)
    std::list<void*>::iterator i = std::find(GObjectStats._allocation_list.begin(),
      GObjectStats._allocation_list.end(), ptr);
#else
    auto i = std::find(GObjectStats._allocation_list.begin(),
                       GObjectStats._allocation_list.end(), ptr);
#endif
    return i != GObjectStats._allocation_list.end();
  }

  int Trackable::GetObjectSize ()
  {
    return _size_of_this_object;
  }

//////////////////////////////////////////////////////////////////////

  Object::Object(bool OwnTheReference, NUX_FILE_LINE_DECL)
    : reference_count_(new NThreadSafeCounter())
    , objectptr_count_(new NThreadSafeCounter())
  {
    reference_count_->Set(1);
    SetOwnedReference(OwnTheReference);
#ifdef NUX_DEBUG
    std::ostringstream sout;
    if (__Nux_FileName__)
      sout << __Nux_FileName__;
    else
      sout << "<unspecified file>";
    sout << ":" << __Nux_LineNumber__;
    allocation_location_ = sout.str();
#endif
  }

  Object::~Object()
  {
    if (IsHeapAllocated())
    {
      // If the object has properly been UnReference, it should have gone
      // through Destroy(). if that is the case then _reference_count should
      // be NULL or its value (returned by GetValue ()) should be equal to 0;
      // We can use this to detect when delete is called directly on an
      // object.
      if (reference_count_->GetValue() > 0)
      {
        LOG_WARN(logger) << "Invalid object destruction, still has "
                         << reference_count_->GetValue() << " references."
                         << "\nObject allocated at: " << GetAllocationLocation() << "\n";
      }
    }
    delete reference_count_;
    delete objectptr_count_;
  }

  bool Object::Reference()
  {
    if (!IsHeapAllocated())
    {
      LOG_WARN(logger) << "Trying to reference an object that was not heap allocated."
                       << "\nObject allocated at: " << GetAllocationLocation() << "\n";
      return false;
    }

    if (!OwnsTheReference())
    {
      SetOwnedReference(true);
      // The ref count remains at 1. Exit the method.
      return true;
    }
    reference_count_->Increment();
    return true;
  }

  bool Object::UnReference()
  {
    if (!IsHeapAllocated())
    {
      LOG_WARN(logger) << "Trying to un-reference an object that was not heap allocated."
                       << "\nObject allocated at: " << GetAllocationLocation() << "\n";
      return false;
    }

    if (objectptr_count_->GetValue() == reference_count_->GetValue())
    {
      // There are ObjectPtr's hosting this object. Release all of them to
      // destroy this object.  This prevent from calling UnReference () many
      // times and destroying the object when there are ObjectPtr's hosting
      // it.  This method should not be called directly in that case.
      LOG_WARN(logger) << "There are ObjectPtr hosting this object. "
                       << "Release all of them to destroy this object. "
                       << "\nObject allocated at: " << GetAllocationLocation() << "\n";
 #if defined(NUX_OS_LINUX)
      LOG_WARN(logger) << "UnReference occuring here: \n" << logging::Backtrace();
 #endif
      return false;
    }

    reference_count_->Decrement();

    if (reference_count_->GetValue() == 0)
    {
      Destroy();
      return true;
    }

    return false;
  }

  bool Object::SinkReference()
  {
    return Reference();
  }

  bool Object::Dispose()
  {
    // The intent of the Dispose call is to destroy objects with a float
    // reference (reference count is equal to 1 and the '_owns_the_reference'
    // flag is set to false). In Nux, only widgets object can have a floating
    // reference.  And widgets are only visible if added to the widget tree.
    // When an object with a floating reference is added to the widget tree,
    // it becomes "owned'. It looses it floating reference status but it still
    // has a reference count number of 1.  In practice, since widgets must be
    // added to the widget tree, there should never be a need to call Dispose
    // (except in a few cases).

    // Dispose() was designed to only destroy objects with floating
    // references, while UnReference() destroys objects that are "owned".
    // That is now relaxed. Dispose() calls UnReference().
    return UnReference();
  }

  void Object::Destroy()
  {
#ifdef NUX_DEBUG
    static int delete_depth = 0;
    ++delete_depth;
    std::string obj_type = GetTypeName();
    LOG_TRACE(logger) << "Depth: " << delete_depth << ", about to delete "
                      << obj_type << " allocated at " << GetAllocationLocation();
#endif
    // Weak smart pointers will clear their pointers when they get this signal.
    object_destroyed.emit(this);
    delete this;
#ifdef NUX_DEBUG
    LOG_TRACE(logger) << "Depth: " << delete_depth << ", delete successful for " << obj_type;
    --delete_depth;
#endif
  }

int Object::GetReferenceCount() const
{
  return reference_count_->GetValue();
}

int Object::ObjectPtrCount() const
{
  return objectptr_count_->GetValue();
}

std::string Object::GetAllocationLocation() const
{
  return allocation_location_;
}

std::string Object::GetTypeName() const
{
  return Type().name;
}

}

