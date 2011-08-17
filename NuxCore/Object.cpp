/*
 * Copyright 2010 Inalogic® Inc.
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

#include <iostream>
#include <sstream>

#include "NuxCore.h"
#include "Object.h"
#include "ObjectPtr.h"
#include "Logger.h"

namespace nux
{
namespace
{
logging::Logger logger("nux.core.object");
}

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
    for (auto ptr : _allocation_list)
    {
      Object* obj = reinterpret_cast<Object*>(ptr);
      std::cerr << "\t" << ++index << " Undeleted object: Type "
                << obj->Type().name << ", "
                << obj->GetAllocationLoation() << "\n";
    }
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
      LOG_DEBUG(logger) << "Do not change the ownership if is already set to true!";
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
  }

  bool Trackable::IsHeapAllocated()
  {
    if (_heap_allocated == -1)
    {
      _heap_allocated = (IsDynamic () ? 1 : 0);
    }

    return (_heap_allocated == 1 ? true : false);
  }

  bool Trackable::IsDynamic() const
  {
    // Get pointer to beginning of the memory occupied by this.
    const void *ptr = dynamic_cast<const void *> (this);

    // Search for ptr in allocation_list
    auto i = std::find(GObjectStats._allocation_list.begin(),
                       GObjectStats._allocation_list.end(), ptr);
    return i != GObjectStats._allocation_list.end();
  }

  int Trackable::GetObjectSize ()
  {
    return _size_of_this_object;
  }

//////////////////////////////////////////////////////////////////////

  Object::Object(bool OwnTheReference, NUX_FILE_LINE_DECL)
  {
    _allocation_file_name      = __Nux_FileName__;
    _allocation_line_number    = __Nux_LineNumber__;

    _reference_count      = new NThreadSafeCounter();
    _weak_reference_count = new NThreadSafeCounter();
    _objectptr_count     = new NThreadSafeCounter();
    _destroyed            = new bool(false);

    _reference_count->Set (1);
    _weak_reference_count->Set (1);

    SetOwnedReference (OwnTheReference);
  }

  Object::~Object()
  {
    *_destroyed = true;

    if (IsHeapAllocated ())
    {
      // If the object has properly been UnReference, it should have gone
      // through Destroy(). if that is the case then _reference_count should
      // be NULL or its value (returned by GetValue ()) should be equal to 0;
      // We can use this to detect when delete is called directly on an
      // object.
      if (_reference_count && _reference_count->GetValue() > 0)
      {
        LOG_ERROR(logger) << "Invalid object destruction, still has "
                          << _reference_count->GetValue() << " references."
                          << "\nObject allocated at: " << GetAllocationLoation();
      }
      if (_weak_reference_count && _weak_reference_count->GetValue() == 0)
      {
        LOG_ERROR(logger) << "Invalid value of the weak reference count pointer."
                          << "\nObject allocated at: " << GetAllocationLoation();
      }

      if ((_reference_count == 0) && (_weak_reference_count == 0))
      {
        delete _destroyed;
      }
      else
      {
        // There is a smart pointer holding a weak reference to this
        // object. It is the responsibility of the last smart pointer to
        // delete '_destroyed'.
      }
    }
    else
    {
      delete _reference_count;
      delete _weak_reference_count;
      delete _objectptr_count;
      delete _destroyed;
    }
  }

  bool Object::Reference()
  {
    if (!IsHeapAllocated ())
    {
      LOG_ERROR(logger) << "Trying to reference an object that was not heap allocated. "
                        << "\nObject allocated at: " << GetAllocationLoation();
      return false;
    }

    if (_reference_count->GetValue() == 0 || *_destroyed)
    {
      // If this happens, all bets are off, and this may crash.
      LOG_ERROR(logger) << "Trying to reference an object that has been deleted."
                        << "\nObject allocated at: " << GetAllocationLoation();
      return false;
    }

    if (!OwnsTheReference())
    {
      SetOwnedReference(true);
      // The ref count remains at 1. Exit the method.
      return true;
    }

    _reference_count->Increment();
    _weak_reference_count->Increment();
    return true;
  }

  bool Object::UnReference()
  {
    if (!IsHeapAllocated())
    {
      LOG_ERROR(logger) << "Trying to un-reference an object that was not heap allocated. "
                        << "\nObject allocated at: " << GetAllocationLoation();
      return false;
    }

    if (_objectptr_count->GetValue() == _reference_count->GetValue())
    {
      // There are ObjectPtr's hosting this object. Release all of them to
      // destroy this object.  This prevent from calling UnReference () many
      // times and destroying the object when there are ObjectPtr's hosting
      // it.  This method should not be called directly in that case.
      LOG_ERROR(logger) << "There are ObjectPtr hosting this object. "
                        << "Release all of them to destroy this object. "
                        << "\nObject allocated at: " << GetAllocationLoation();
      return false;
    }

    _reference_count->Decrement();
    _weak_reference_count->Decrement();

    if (_reference_count->GetValue() == 0)
    {
      Destroy();
      return true;
    }

    return false;
  }

  bool Object::SinkReference()
  {
    if (!IsHeapAllocated())
    {
      LOG_ERROR(logger) << "Trying to sink an object that was not heap allocated. "
                        << "\nObject allocated at: " << GetAllocationLoation();
      return false;
    }

    if (!OwnsTheReference())
    {
      SetOwnedReference(true);
      // The ref count remains at 1. Exit the method.
      return true;
    }

    return false;
  }

  bool Object::Dispose()
  {
    // The intent of the Dispose call is to destroy objects with a float reference (reference count is equal to 1 and
    // the '_owns_the_reference' flag is set to false). In Nux, only widgets object can have a floating reference.
    // And widgets are only visible if added to the widget tree. 
    // When an object with a floating reference is added to the widget tree, it becomes "owned'. It looses it
    // floating reference status but it still has a reference count number of 1.
    // In practice, since widgets must be added to the widget tree, there should never be a need to call Dispose
    // (except in a few cases).

    // Dispose() was designed to only destroy objects with floating references, while UnReference() destroys objects
    // that are "owned" . That is now relaxed. Dispose() calls UnReference().

    return UnReference();
  }

  void Object::Destroy()
  {
    if (!IsHeapAllocated ())
    {
      LOG_ERROR(logger) << "Trying to destroy an object that was not heap allocated. "
                        << "\nObject allocated at: " << GetAllocationLoation();
      return;
    }

    nuxAssert (_reference_count->GetValue() == 0);

    if ((_reference_count->GetValue() == 0) && (_weak_reference_count->GetValue() == 0) && (_objectptr_count->GetValue() == 0))
    {
      delete _reference_count;
      delete _weak_reference_count;
      delete _objectptr_count;
      _reference_count = 0;
      _weak_reference_count = 0;
      _objectptr_count = 0;
    }
    else
    {
      if ((_weak_reference_count == NULL) && (_objectptr_count->GetValue() == 0))
      {
        nuxDebugMsg (TEXT ("[Object::Destroy] Error on object allocated at %s [%d]:")
        , _allocation_file_name
        , _allocation_line_number);
        nuxAssertMsg (0, TEXT("[Object::Destroy] Invalid pointer for the weak reference count."));
      }

      if ((_weak_reference_count->GetValue() == 0) && (_objectptr_count->GetValue() == 0))
      {
        nuxDebugMsg (TEXT ("[Object::Destroy] Error on object allocated at %s [%d]:")
          , _allocation_file_name
          , _allocation_line_number);
        nuxAssertMsg (0, TEXT("[Object::Destroy] Invalid value of the weak reference count."));
      }
    }

    static int delete_depth = 0;
    ++delete_depth;
    object_destroyed.emit(this);
    const char* obj_type = this->Type().name;
    LOG_TRACE(logger) << "Depth: " << delete_depth << ", about to delete "
                      << obj_type << " allocated at " << GetAllocationLoation();
    delete this;
    LOG_TRACE(logger) << "Depth: " << delete_depth << ", delete successful for " << obj_type;
    --delete_depth;
  }

  void Object::IncrementWeakCounter()
  {
    if (!IsHeapAllocated ())
    {
      LOG_ERROR(logger) << "Trying to increment weak counter on an object that was not heap allocated. "
                        << "\nObject allocated at: " << GetAllocationLoation();
      return;
    }

    _weak_reference_count->Increment();
  }

  void Object::DecrementWeakCounter()
  {
    if (!IsHeapAllocated())
    {
      LOG_ERROR(logger) << "Trying to decrement weak counter on an object that was not heap allocated. "
                        << "\nObject allocated at: " << GetAllocationLoation();
      return;
    }

    _weak_reference_count->Decrement();
  }

  int Object::GetReferenceCount() const
  {
    return _reference_count->GetValue();
  }

  int Object::GetWeakReferenceCount() const
  {
    return _weak_reference_count->GetValue();
  }

std::string Object::GetAllocationLoation() const
{
  std::ostringstream sout;
  sout << _allocation_file_name
       << ":" << _allocation_line_number;
  return sout.str();
}

}

