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


#ifndef NUXCORE_OBJECTPTR_H
#define NUXCORE_OBJECTPTR_H

#include <sigc++/connection.h>
#include <sigc++/functors/mem_fun.h>

namespace nux
{
  template <typename T>
  class ObjectWeakPtr;

  template <typename T>
  class ObjectPtr;

  //! A smart pointer class. Implemented as an intrusive smart pointer.
  template <typename T>
  class ObjectPtr
  {
  public:
    //! Constructor
    ObjectPtr()
      : ptr_(NULL)
    {
    }

    //! Copy constructor
    ObjectPtr(ObjectPtr<T> const& other)
      : ptr_(other.ptr_)
    {
      if (ptr_)
      {
        ptr_->objectptr_count_->Increment();
        ptr_->Reference();
      }
    }

    //! Copy constructor
    /*!
        This method takes advantage of the nux type information using the
        virtual Type function.

        @param other Parameter with a type derived from T.
    */
    template <typename O>
    ObjectPtr(ObjectPtr<O> const& other)
      : ptr_(NULL)
    {
      if (other.ptr_ &&
          other.ptr_->Type().IsDerivedFromType(T::StaticObjectType))
      {
        ptr_ = static_cast<T*>(other.ptr_);
        ptr_->objectptr_count_->Increment();
        ptr_->Reference();
      }
    }

    //! Construction with a base pointer of type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param WarnMissuse If True, then ObjectPtr test is ptr is owned or
        not. If ptr is not owned and WarnMissuse is True, then Print a warning
        message. This is a debug feature to detect cases such as
        "ObjectPtr(ObjectA) myobject(ptr);", because the calling code will no
        longer have a reference on ptr.
    */
    explicit ObjectPtr(T *ptr, bool WarnMissuse = false)
      : ptr_(NULL)
    {
      if (ptr)
      {
        if (WarnMissuse && (!ptr->OwnsTheReference()))
        {
          nuxDebugMsg (TEXT ("[ObjectPtr::ObjectPtr] Warning: Constructing a smart pointer from an object with a floating reference.") );
        }

        ptr_ = ptr;
        ptr_->objectptr_count_->Increment();
        ptr_->Reference();
      }
    }

    //! Construction with a base pointer of type O that inherits from type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param WarnMissuse If True, then ObjectPtr test is ptr is owned or
        not. If ptr is not owned and WarnMissuse is True, then Print a warning
        message. This is a debug feature to detect cases such as
        "ObjectPtr(ObjectA) myobject(ptr);", because the calling code will no
        longer have a reference on ptr.
    */
    template <typename O>
    explicit ObjectPtr(O *ptr, bool WarnMissuse = false)
      : ptr_(NULL)
    {
      if (ptr &&
          ptr->Type().IsDerivedFromType(T::StaticObjectType))
      {
        if (WarnMissuse && (!ptr->OwnsTheReference()))
        {
          nuxDebugMsg (TEXT ("[ObjectPtr::ObjectPtr] Warning: Constructing a smart pointer from an object with a floating reference.") );
        }

        ptr_ = static_cast<T*>(ptr);
        ptr_->objectptr_count_->Increment();
        ptr_->Reference();
      }
    }

    //! Take ownership of ptr.
    void Adopt(T* ptr)
    {
      bool was_owned = false;
      if (ptr)
      {
        // If 'was_owned' is false, then ptr has a floating reference status. The next line:
        //    ObjectPtr<T> temp(ptr);
        // will not increase its reference count. It will only make the object "Owned".
        was_owned = ptr->OwnsTheReference();
      }

      // If 'was_owned' is true then the next line increases the reference count of ptr.
      // Otherwise, ptr just becomes "Owned" (it looses it floating reference status).
      ObjectPtr<T> temp(ptr);
      Swap(temp);

      // Now we want to release the reference that was added on construction,
      // but keep the smart pointer count.
      if (ptr_ && was_owned)
      {
        // ptr was already owned. Reduce the reference count that was added by the call to
        //    ObjectPtr<T> temp(ptr);
        ptr_->UnReference();
      }
    }


    //! Assignment of a smart pointer of type T.
    /*!
        @param other Smart pointer of type T.
    */
    ObjectPtr& operator=(T* ptr)
    {
        ObjectPtr<T> temp(ptr);
        Swap(temp);
        return *this;
    }

    //! Assignment of a smart pointer of type T.
    /*!
        @param other Smart pointer of type T.
    */
    ObjectPtr& operator=(ObjectPtr<T> const& other)
    {
        ObjectPtr<T> temp(other);
        Swap(temp);
        return *this;
    }

    //! Assignment of a smart pointer of type O that inherits from type T.
    /*!
        @param other Smart pointer of type O.
    */
    template <typename O>
    ObjectPtr& operator=(ObjectPtr<O> const& other)
    {
        ObjectPtr<T> temp(other);
        Swap(temp);
        return *this;
    }

    ~ObjectPtr()
    {
      ReleaseReference();
    }

    T& operator*() const
    {
      nuxAssert(ptr_ != 0);
      return *ptr_;
    }

    T* operator->() const
    {
      nuxAssert(ptr_ != 0);
      return ptr_;
    }

    //! Return the stored pointer.
    /*!
        Caller of this function should Reference the pointer if they intend to
        keep it.
        @param Return the stored pointer.
    */
    T* GetPointer () const
    {
      return ptr_;
    }

    //! Swap the content of 2 smart pointers.
    /*!
        @param other Smart pointer to swap with.
    */
    void Swap (ObjectPtr<T>& other)
    {
        std::swap(ptr_, other.ptr_);
    }

    operator bool() const
    {
        return bool(ptr_);
    }

    //! Test validity of the smart pointer.
    /*!
        Return True if the internal pointer is not null.
    */
    bool operator() () const
    {
        return bool(ptr_);
    }

    //! Test validity of the smart pointer.
    /*!
        Return True if the internal pointer is null.
    */
    bool IsNull() const
    {
        return !IsValid();
    }

    //! Test validity of the smart pointer.
    /*!
        Return True if the internal pointer is not null.
    */
    bool IsValid() const
    {
      return (ptr_ != NULL);
    }

    bool operator < (T *ptr) const
    {
      return (ptr_ < ptr);
    }

    bool operator > (T *ptr) const
    {
      return (ptr_ > ptr);
    }

    bool operator < (ObjectPtr<T> other) const
    {
      return (ptr_ < other.ptr_);
    }

    bool operator > (ObjectPtr<T> other) const
    {
      return (ptr_ > other.ptr_);
    }

    bool operator == (T *ptr) const
    {
      return ptr_ == ptr;
    }

    template <typename U>
    bool operator != (U other) const
    {
      return !(*this == other);
    }

    template <typename U>
    bool operator == (U* ptr) const
    {
      if (ptr && (!ptr->Type().IsDerivedFromType(T::StaticObjectType)))
        return false;

      return ptr_ == static_cast<T*>(ptr);
    }

    template <typename U>
    bool operator == (ObjectPtr<U> const& other) const
    {
      if (other.ptr_ &&
          (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ == static_cast<T*>(other.ptr_);
    }

    template <typename U>
    bool operator == (ObjectWeakPtr<U> const& other) const
    {
      if (other.ptr_ &&
          (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ == static_cast<T*>(other.ptr_);
    }

    //! Release the hosted pointer from this object.
    /*!
        Release the hosted pointer from this object. After this call, all the
        members are null.

        @return True if the hosted object was destroyed.
    */
    bool Release()
    {
      return ReleaseReference();
    }

  private:

    bool ReleaseReference()
    {
      if (!ptr_)
      {
        return false;
      }

      // Decrease the number of strong reference on the hosted pointer.
      ptr_->objectptr_count_->Decrement();
      bool destroyed = ptr_->UnReference();
      ptr_ = NULL;
      return destroyed;
    }

    T* ptr_;

    template <typename O>
    friend class ObjectPtr;

    template <typename O>
    friend class ObjectWeakPtr;
  };


//! A weak smart pointer class. Implemented as an intrusive smart pointer.
  /*!
      A weak smart pointer is built from a smart pointer or another weak smart
      pointer. It increments and decrements the total reference count of an
      pointer. Even is the original pointer is destroyed, weak smart pointers
      still point to the RefCounts pointers of the original pointer and can
      use it to check if the pointer is still valid or not.
  */
  template <typename T>
  class ObjectWeakPtr
  {
  public:
    //! Constructor
    ObjectWeakPtr()
      : ptr_(NULL)
    {
    }

    //! Construction with a base pointer of type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param WarnMissuse If True, then ObjectPtr test is ptr is owned or
        not. If ptr is not owned and WarnMissuse is True, then Print a warning
        message. This is a debug feature to detect cases such as
        "ObjectPtr(ObjectA) myobject(ptr);", because the calling code will no
        longer have a reference on ptr.
    */
    explicit ObjectWeakPtr(T* ptr)
      : ptr_(ptr)
    {
      ConnectListener();
    }

    //! Construction with a base pointer of type O that inherits from type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param WarnMissuse If True, then ObjectPtr test is ptr is owned or
        not. If ptr is not owned and WarnMissuse is True, then Print a warning
        message. This is a debug feature to detect cases such as
        "ObjectPtr(ObjectA) myobject(ptr);", because the calling code will no
        longer have a reference on ptr.
    */
    template <typename O>
    explicit ObjectWeakPtr(O* ptr, bool /* WarnMissuse */ = false)
      : ptr_(NULL)
    {
      if (ptr &&
          (ptr->Type().IsDerivedFromType(T::StaticObjectType)))
      {
        ptr_ = static_cast<T*>(ptr);
        ConnectListener();
      }
    }

    //! Copy constructor
    /*!
        @param other Parameter with type T.
    */
    ObjectWeakPtr(ObjectWeakPtr<T> const& other)
      : ptr_(other.ptr_)
    {
      ConnectListener();
    }

    //! Copy constructor
    /*!
        @param other Parameter with a type derived from T.
    */
    template <typename O>
    ObjectWeakPtr(const ObjectWeakPtr<O>& other)
      : ptr_(NULL)
    {
      if (other.ptr_ &&
          (other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
      {
        ptr_ = static_cast<T*>(other.ptr_);
        ConnectListener();
      }
    }

    //! Construction from a smart pointer of type O that inherits from type T.
    /*!
        @param other Maintains a weak smart pointer reference to this parameter.
    */
    template <typename O>
    ObjectWeakPtr(const ObjectPtr<O> &other)
      : ptr_(NULL)
    {
      if (other.ptr_ &&
          (other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
      {
        ptr_ = static_cast<T*>(other.ptr_);
        ConnectListener();
      }
    }

    //! Assignment of a weak smart pointer of type T.
    /*!
        @param other Weak smart pointer of type T.
    */
    ObjectWeakPtr& operator = (ObjectWeakPtr<T> const& other)
    {
      Disconnect();
      ptr_ = other.ptr_;
      ConnectListener();
      return *this;
    }

    // Warning: We are not sure that other.ptr_ is valid.
    // Warning: Cannot call other.ptr_->Type().IsDerivedFromType (T::StaticObjectType)
    //! Assignment of a weak smart pointer of Type O that inherits from type T.
    /*!
        @param other Weak smart pointer of type O.
    */
    template <typename O>
    ObjectWeakPtr &operator = (const ObjectWeakPtr<O>& other)
    {
      Disconnect();
      if (other.ptr_ &&
          other.ptr_->Type().IsDerivedFromType(T::StaticObjectType))
      {
        ptr_ = static_cast<T*>(other.ptr_);
        ConnectListener();
      }
      return *this;
    }

    //! Assignment of a smart pointer of Type O that inherits from type T.
    /*!
        @param other Maintains a weak smart pointer reference to this parameter.
    */
    template <typename O>
    ObjectWeakPtr &operator = (const ObjectPtr<O>& other)
    {
      Disconnect();
      if (other.ptr_ &&
          other.ptr_->Type().IsDerivedFromType(T::StaticObjectType))
      {
        ptr_ = static_cast<T*>(other.ptr_);
        ConnectListener();
      }
      return *this;
    }

    //! Construction with a base pointer of type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param WarnMissuse If True, then ObjectPtr test is ptr is owned or not. If ptr is not owned
        and WarnMissuse is True, then Print a warning message. This is a debug feature to detect cases such as
        "ObjectPtr(ObjectA) myobject(ptr);", because the calling code will no longer have a reference on ptr.
    */
    ObjectWeakPtr& operator = (T* ptr)
    {
      Disconnect();
      ptr_ = ptr;
      ConnectListener();
      return *this;
    }

    template <typename O>
    ObjectWeakPtr &operator = (O* ptr)
    {
      Disconnect();
      if (ptr &&
          ptr->Type().IsDerivedFromType(T::StaticObjectType))
      {
        ptr_ = static_cast<T*>(ptr);
        ConnectListener();
      }
      return *this;
    }

    ~ObjectWeakPtr()
    {
      Disconnect();
    }

    T& operator* () const
    {
      nuxAssert (ptr_ != 0);
      return *(const_cast<T*>(ptr_));
    }

    T* operator -> () const
    {
      nuxAssert (ptr_ != 0);
      return const_cast<T*>(ptr_);
    }

    bool operator < (T *ptr) const
    {
      return (ptr_ < ptr);
    }

    bool operator > (T *ptr) const
    {
      return (ptr_ > ptr);
    }

    bool operator < (ObjectWeakPtr<T> other) const
    {
      return (ptr_ < other.ptr_);
    }

    bool operator > (ObjectWeakPtr<T> other) const
    {
      return (ptr_ > other.ptr_);
    }

    template <typename U>
    bool operator != (U other) const
    {
      return !(*this == other);
    }
    bool operator == (T *ptr) const
    {
      return ptr_ == ptr;
    }

    template<typename U>
    bool operator == (U *ptr) const
    {
      if (ptr && (!ptr->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ == static_cast<T*>(ptr);
    }

    /*!
        @other  A weak pointer
        @return True is this weak pointer host the same pointer as the weak pointer passed as parameter.
    */
    template<typename U>
    bool operator == (const ObjectWeakPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ == static_cast<T*>(other.ptr_);
    }

    /*!
        @other  An object pointer
        @return True is this weak pointer host the same pointer as the object pointer passed as parameter.
    */
    template<typename U>
    bool operator == (const ObjectPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ == static_cast<T*>(other.ptr_);
    }

    //! Return true is the hosted pointer is not null or has not been destroyed.
    /*!
        @return True if the internal pointer is not null.
    */
    bool operator() () const
    {
      return bool(ptr_);
    }

    //! Return true is the hosted pointer is not null or has not been destroyed.
    /*!
        @return True if the internal pointer is not null or has not been destroyed.
    */
    bool IsValid() const
    {
      return (ptr_ != NULL);
    }

    //! Return true is the hosted pointer is null or has been destroyed.
    /*!
        @return True if the internal pointer is null or has been destroyed.
    */
    bool IsNull() const
    {
      return !IsValid();
    }

    //! Release the hosted pointer from this object.
    /*!
        Release the hosted pointer from this object. After this call, the following members are null:
          _reference_count
          _weak_reference_count
          ptr_
        This call decreases the count of weak reference before setting _weak_reference_count to null.

        @return True this was the last weak reference on the hosted object.
    */
    bool Release()
    {
        Disconnect();
        ptr_ = NULL;
        return false;
    }

    //! Return the stored pointer.
    /*!
        Caller of this function should Reference the pointer if they intend to keep it.
        @param Return the stored pointer.
    */
    T* GetPointer () const
    {
      return ptr_;
    }

  private:
    void Disconnect()
    {
      if (destroy_listener_.connected())
        destroy_listener_.disconnect();
    }

    void ConnectListener()
    {
      if (ptr_)
      {
        auto slot = sigc::mem_fun(this, &ObjectWeakPtr<T>::TargetDestroyed);
        destroy_listener_ = ptr_->object_destroyed.connect(slot);
      }
    }

    void TargetDestroyed(Object* /* ptr */)
    {
        ptr_ = NULL;
        // rese the connetion too
        destroy_listener_ = sigc::connection();
    }

    T* ptr_;
    sigc::connection destroy_listener_;

    template <typename O>
    friend class ObjectWeakPtr;

    template<typename U>
    friend bool operator == (U *, const ObjectWeakPtr<U>& a);

    template<typename U>
    friend bool operator != (U *, const ObjectWeakPtr<U>& a);

    template <typename U, typename F>
    friend ObjectWeakPtr<U> staticCast (const ObjectWeakPtr<F>& from);

    template <typename U, typename F>
    friend ObjectWeakPtr<U> constCast (const ObjectWeakPtr<F>& from);

    template <typename U, typename F>
    friend ObjectWeakPtr<U> dynamicCast (const ObjectWeakPtr<F>& from);

    template <typename U, typename F>
    friend ObjectWeakPtr<U> checkedCast (const ObjectWeakPtr<F>& from);

    template <typename U, typename F>
    friend ObjectWeakPtr<U> queryCast (const ObjectWeakPtr<F>& from);
  };

///////////////////////////////////////////////////////
// globals


  template<typename T>
  inline bool operator == (T *ptr, const ObjectPtr<T>& a)
  {
    return a.ptr_ == ptr;
  }

  template<typename T>
  inline bool operator != (T *ptr, const ObjectPtr<T>& a)
  {
    return a.ptr_ != ptr;
  }

  template<typename T>
  inline bool operator == (T *ptr, const ObjectWeakPtr<T>& a)
  {
    return a.ptr_ == ptr;
  }

  template<typename T>
  inline bool operator != (T *ptr, const ObjectWeakPtr<T>& a)
  {
    return a.ptr_ != ptr;
  }

}

#endif


