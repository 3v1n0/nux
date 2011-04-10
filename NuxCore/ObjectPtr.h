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


#ifndef INTRUSIVESP_H
#define INTRUSIVESP_H

namespace nux
{

// forward definitions

  template <typename T>
  class ObjectWeakPtr;

  template <typename T>
  class ObjectPtr;

  #define IntrusiveSP ObjectPtr
  #define IntrusiveWeakSP ObjectWeakPtr

//! A smart pointer class. Implemented as an intrusive smart pointer.
  template <typename T>
  class ObjectPtr
  {
  public:
    //! Constructor
    ObjectPtr()
      :   ptr_ (0)
    {
      _reference_count = 0;
      _weak_reference_count = 0;
      _objectptr_count = 0;
      _destroyed = 0;
    }

    //! Copy constructor
    ObjectPtr (const ObjectPtr<T>& other)
      :   ptr_ (0)
    {
      ptr_ = other.ptr_;

      if (ptr_ != 0)
      {
        _reference_count = other->_reference_count;
        _weak_reference_count = other->_weak_reference_count;
        _objectptr_count = other->_objectptr_count;
        _objectptr_count->Increment ();
        _destroyed = other->_destroyed;
        ptr_->Reference();
      }
    }

    //! Copy constructor
    /*!
        @param other Parameter with a type derived from T.
    */
    template <typename O>
    ObjectPtr (const ObjectPtr<O>& other)
      :   ptr_ (0)
    {
      _reference_count = 0;
      _weak_reference_count = 0;
      _objectptr_count = 0;
      _destroyed = 0;

      if (other.ptr_ && other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) )
      {
        ptr_ = (T *) other.ptr_;
        _reference_count = other->_reference_count;
        _weak_reference_count = other->_weak_reference_count;
        _objectptr_count = other->_objectptr_count;
        _objectptr_count->Increment ();
        _destroyed = other->_destroyed;
        ptr_->Reference();
      }
    }

    //! Construction with a base pointer of type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param WarnMissuse If True, then ObjectPtr test is ptr is owned or not. If ptr is not owned
        and WarnMissuse is True, then Print a warning message. This is a debug feature to detect cases such as
        "ObjectPtr(ObjectA) myobject(ptr);", because the calling code will no longer have a reference on ptr.
    */
    explicit ObjectPtr (T *ptr, bool WarnMissuse = false)
      :   ptr_ (0)
    {
      _reference_count = 0;
      _weak_reference_count = 0;
      _objectptr_count = 0;
      _destroyed = 0;

      if (ptr != 0)
      {
        if (WarnMissuse && (ptr->OwnsTheReference() == false) )
        {
          nuxDebugMsg (TEXT ("[ObjectPtr::ObjectPtr] Warning: Constructing a smart pointer from an object with a floating reference.") );
        }

        ptr_ = ptr;
        _reference_count = ptr->_reference_count;
        _weak_reference_count = ptr->_weak_reference_count;
        _objectptr_count = ptr->_objectptr_count;
        _objectptr_count->Increment ();
        _destroyed = ptr->_destroyed;
        ptr_->Reference();
      }
    }

    //! Construction with a base pointer of type O that inherits from type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param WarnMissuse If True, then ObjectPtr test is ptr is owned or not. If ptr is not owned
        and WarnMissuse is True, then Print a warning message. This is a debug feature to detect cases such as
        "ObjectPtr(ObjectA) myobject(ptr);", because the calling code will no longer have a reference on ptr.
    */
    template <typename O>
    explicit ObjectPtr (O *ptr, bool WarnMissuse = false)
      :   ptr_ (0)
    {
      _reference_count = 0;
      _weak_reference_count = 0;
      _objectptr_count = 0;
      _destroyed = 0;

      if (ptr != 0)
      {
        if (ptr->Type().IsDerivedFromType (T::StaticObjectType) )
        {
          if (WarnMissuse && (ptr->OwnsTheReference() == false) )
          {
            nuxDebugMsg (TEXT ("[ObjectPtr::ObjectPtr] Warning: Constructing a smart pointer from an object with a floating reference.") );
          }

          ptr_ = (T *) ptr;
          _reference_count = ptr->_reference_count;
          _weak_reference_count = ptr->_weak_reference_count;
          _objectptr_count = ptr->_objectptr_count;
          _objectptr_count->Increment ();
          _destroyed = ptr->_destroyed;
          ptr_->Reference();
        }
      }
    }

    //! Assignment of a smart pointer of type T.
    /*!
        @param other Smart pointer of type T.
    */
    ObjectPtr &operator = (const ObjectPtr<T>& other)
    {
      if (ptr_ != other.ptr_)
      {
        ReleaseReference ();

        ptr_ = other.ptr_;

        if (ptr_ != 0)
        {
          _reference_count = other->_reference_count;
          _weak_reference_count = other->_weak_reference_count;
          _objectptr_count = other->_objectptr_count;
          _objectptr_count->Increment ();
          _destroyed = other->_destroyed;
          ptr_->Reference();
        }
      }

      return *this;
    }

    //! Assignment of a smart pointer of type O that inherits from type T.
    /*!
        @param other Smart pointer of type O.
    */
    template <typename O>
    ObjectPtr &operator = (const ObjectPtr<O>& other)
    {
      if (other.ptr_ && other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) )
      {
        if (ptr_ != other.ptr_)
        {
          ReleaseReference ();

          ptr_ = other.ptr_;

          if (ptr_ != 0)
          {
            _reference_count = other->_reference_count;
            _weak_reference_count = other->_weak_reference_count;
            _objectptr_count = other->_objectptr_count;
            _objectptr_count->Increment ();
            _destroyed = other->_destroyed;
            ptr_->Reference();
          }
        }
      }
      else
      {
        ReleaseReference ();
      }

      return *this;
    }

    // WARNING: THIS FUNCTION IS MADE PUBLIC TEMPORARILY TO FACILITATE TRANSITION TO A PURE POINTER BASE USAGE IN NUX.
    // THIS FUNCTION WILL BE REMOVED!
    T *GetSPPointer() const
    {
      return ptr_;
    }

    ~ObjectPtr ()
    {
      ReleaseReference ();
    }

    T &operator * () const
    {
      nuxAssert (ptr_ != 0);
      return *ptr_;
    }

    T *operator -> () const
    {
      nuxAssert (ptr_ != 0);
      return ptr_;
    }

//     //! Swap the content of 2 smart pointers.
//     /*!
//         @param other Smart pointer to swap with.
//     */
//     void Swap (ObjectPtr<T>& other)
//     {
//         std::swap (ptr_, other.ptr_);
//         std::swap (refCounts_, other.refCounts_);
//     }

    //! Test validity of the smart pointer.
    /*!
        Return True if the internal pointer is not null.
    */
    bool operator () () const
    {
      return ptr_ != 0;
    }

    //! Test validity of the smart pointer.
    /*!
        Return True if the internal pointer is null.
    */
    bool IsNull() const
    {
      if (ptr_ == 0)
        return true;

      return false;
    }

    //! Test validity of the smart pointer.
    /*!
        Return True if the internal pointer is not null.
    */
    bool IsValid() const
    {
      if (ptr_ != 0)
        return true;

      return false;
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

    bool operator != (T *ptr) const
    {
      return ( (void *) ptr_ != (void *) ptr);
    }

    bool operator == (T *ptr) const
    {
      return ( (void *) ptr_ == (void *) ptr);
    }

    template<typename U>
    bool operator != (U *ptr) const
    {
      if (ptr && (!ptr->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return true;

      return ( (void *) ptr_ != (void *) ptr);
    }

    template<typename U>
    bool operator == (U *ptr) const
    {
      if (ptr && (!ptr->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ( (void *) ptr_ == (void *) ptr);
    }

    template<typename U>
    bool operator != (const ObjectPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return true;

      return (void *) ptr_ != (void *) other.ptr_;
    }

    template<typename U>
    bool operator == (const ObjectPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return (void *) ptr_ == (void *) other.ptr_;
    }

    template<typename U>
    bool operator != (const ObjectWeakPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return true;

      return (void *) ptr_ != (void *) other.ptr_;
    }

    template<typename U>
    bool operator == (const ObjectWeakPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return (void *) ptr_ == (void *) other.ptr_;
    }

    //! Release the hosted pointer from this object.
    /*!
        Release the hosted pointer from this object. After this call, the following members are null:
          _reference_count
          _weak_reference_count
          ptr_
        This call decreases the count of strong and weak references reference before setting _reference_count and _weak_reference_count to null.

        @return True if the hosted object was destroyed.
    */
    bool Release()
    {
      return ReleaseReference ();
    }

  private:

    bool ReleaseReference()
    {
      if (ptr_ == 0)
      {
        return false;
      }

      // Decrease the number of strong reference on the hosted pointer.
      _objectptr_count->Decrement ();

      bool delete_warning = (_reference_count->GetValue() == 1) && (_weak_reference_count->GetValue() == 1);

      // If delete_warning == true, then we know that the cal to Unref will delete the pointer ptr. Also, the pointer to
      // _reference_count and _weak_reference_count will be deleted.
      if (delete_warning)
      {
        nuxAssert (_objectptr_count->GetValue () == 0);
      }

      bool destroyed = ptr_->UnReference();
      // if destroyed is true and delete_warning is false, it means that the pointer was destroyed. But to to hanging Weak pointers,
      // _weak_reference_count and _weak_reference_count have not been deleted.

      if (delete_warning)
      {
        nuxAssert (destroyed);
      }

      if (destroyed && delete_warning)
      {
        // Everything has been destroyed and there are no hanging weak pointers. Reset the pointers for this object.
        _reference_count = 0;
        _weak_reference_count = 0;
        _objectptr_count = 0;
        _destroyed = 0;
        ptr_ = 0;
      }
      else if (destroyed && (delete_warning == false) )
      {
        // Only the ptr_ has been destroyed.There are still hanging weak pointers. Reset the pointers for this object.
        nuxAssert (_reference_count->GetValue() == 0);
        nuxAssert (_objectptr_count->GetValue () == 0);
        nuxAssert (_weak_reference_count->GetValue() >= 1);
        _reference_count = 0;
        _weak_reference_count = 0;
        _objectptr_count = 0;
        _destroyed = 0;
        ptr_ = 0;
      }
      else
      {
        // There are still pending references to ptr_. Reset the pointers for this object.
        // Notice how we do the same thing here as in the previous conditions. We end up setting
        // _reference_count, _weak_reference_count and ptr_ to 0.
        _reference_count = 0;
        _weak_reference_count = 0;
        _objectptr_count = 0;
        _destroyed = 0;
        ptr_ = 0;
      }

      return destroyed;
    }

    T *ptr_;
    NThreadSafeCounter *_reference_count;
    NThreadSafeCounter *_weak_reference_count;
    NThreadSafeCounter *_objectptr_count;
    bool *_destroyed;

    template <typename U>
    friend ObjectPtr<U> Create ();

    template <typename U, typename P1>
    friend ObjectPtr<U> Create (P1 p1);

    template <typename U, typename P1, typename P2>
    friend ObjectPtr<U> Create (P1 p1, P2 p2);

    template <typename U, typename P1, typename P2, typename P3>
    friend ObjectPtr<U> Create (P1 p1, P2 p2, P3 p3);

    template <typename U, typename P1, typename P2, typename P3, typename P4>
    friend ObjectPtr<U> Create (P1 p1, P2 p2, P3 p3, P4 p4);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5>
    friend ObjectPtr<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    friend ObjectPtr<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
    friend ObjectPtr<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);

    template <typename U>
    friend ObjectPtr<U> WrapWSPtr (U *u);

    template <typename O>
    friend class ObjectPtr;

    template <typename O>
    friend class ObjectWeakPtr;

    //     template<typename T, typename U>
    //     friend bool operator == (const ObjectPtr<T>& a, const ObjectPtr<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator != (const ObjectPtr<T>& a, const ObjectPtr<U>& b);

    //     template<typename T>
    //     friend bool operator == (const ObjectPtr<T>& a, T*);

    template<typename U>
    friend bool operator == (U *, const ObjectPtr<U>& a);

    //     template<typename T>
    //     friend bool operator != (const ObjectPtr<T>& a, T*);

    template<typename U>
    friend bool operator != (U *, const ObjectPtr<U>& a);

    //     template<typename T, typename U>
    //     friend bool operator == (const ObjectPtr<T>& a, const ObjectWeakPtr<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator == (const ObjectWeakPtr<T>& a, const ObjectPtr<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator != (const ObjectPtr<T>& a, const ObjectWeakPtr<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator != (const ObjectWeakPtr<T>& a, const ObjectPtr<U>& b);

    template <typename U, typename F>
    friend ObjectPtr<U> staticCast (const ObjectPtr<F>& from);

    template <typename U, typename F>
    friend ObjectPtr<U> constCast (const ObjectPtr<F>& from);

    template <typename U, typename F>
    friend ObjectPtr<U> dynamicCast (const ObjectPtr<F>& from);

    template <typename U, typename F>
    friend ObjectPtr<U> checkedCast (const ObjectPtr<F>& from);

    template <typename U, typename F>
    friend ObjectPtr<U> queryCast (const ObjectPtr<F>& from);
  };


//! A weak smart pointer class. Implemented as an intrusive smart pointer.
  /*!
      A weak smart pointer is built from a smart pointer or another weak smart pointer. It increments and decrements
      the total reference count of an pointer. Even is the original pointer is destroyed, weak smart pointers still point
      to the RefCounts pointers of the original pointer and can use it to check if the pointer is still valid or not.
  */
  template <typename T>
  class ObjectWeakPtr
  {
  public:
    //! Constructor
    ObjectWeakPtr ()
      :   ptr_ (0)
    {
      _reference_count = 0;
      _weak_reference_count = 0;
      _destroyed = 0;
    }

    //! Construction with a base pointer of type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param WarnMissuse If True, then ObjectPtr test is ptr is owned or not. If ptr is not owned
        and WarnMissuse is True, then Print a warning message. This is a debug feature to detect cases such as
        "ObjectPtr(ObjectA) myobject(ptr);", because the calling code will no longer have a reference on ptr.
    */
    explicit ObjectWeakPtr (T *ptr, bool WarnMissuse = false)
      :   ptr_ (0)
    {
      _reference_count = 0;
      _weak_reference_count = 0;
      _destroyed = 0;

      if (ptr != 0)
      {
        if (WarnMissuse && (ptr->OwnsTheReference () == false))
        {
          //nuxDebugMsg (TEXT ("[ObjectWeakPtr::ObjectWeakPtr] Warning: Constructing a weak smart pointer from an object with a floating reference.") );
        }

        ptr_ = ptr;
        _reference_count = ptr->_reference_count;
        _weak_reference_count = ptr->_weak_reference_count;
        _destroyed = ptr->_destroyed;
        ptr_->IncrementWeakCounter ();
      }
    }

    //! Construction with a base pointer of type O that inherits from type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param WarnMissuse If True, then ObjectPtr test is ptr is owned or not. If ptr is not owned
        and WarnMissuse is True, then Print a warning message. This is a debug feature to detect cases such as
        "ObjectPtr(ObjectA) myobject(ptr);", because the calling code will no longer have a reference on ptr.
    */
    template <typename O>
    explicit ObjectWeakPtr (O *ptr, bool WarnMissuse = false)
      :   ptr_ (0)
    {
      _reference_count = 0;
      _weak_reference_count = 0;
      _destroyed = 0;

      if (ptr != 0)
      {
        if (ptr->Type ().IsDerivedFromType (T::StaticObjectType))
        {
          if (WarnMissuse && (ptr->OwnsTheReference () == false))
          {
            //nuxDebugMsg (TEXT ("[ObjectWeakPtr::ObjectWeakPtr] Warning: Constructing a weak smart pointer from an object with a floating reference.") );
          }

          ptr_ = (T *) ptr;
          _reference_count = ptr->_reference_count;
          _weak_reference_count = ptr->_weak_reference_count;
          _destroyed = ptr->_destroyed;
          ptr_->IncrementWeakCounter ();
        }
      }
    }

    //! Copy constructor
    /*!
        @param other Parameter with type T.
    */
    ObjectWeakPtr (const ObjectWeakPtr<T> &other)
    {
      ptr_ = other.ptr_;
      _reference_count = other._reference_count;
      _weak_reference_count = other._weak_reference_count;
      _destroyed = other._destroyed;

      // Warning: We cannot count on ptr_ to be valid.
      // If _weak_reference_count is not null, then imcrement it.
      if (_weak_reference_count)
      {
        _weak_reference_count->Increment ();
      }
      else
      {
        // Sanity checks
        nuxAssert (_reference_count == 0);
        nuxAssert (_weak_reference_count == 0);
        nuxAssert (_destroyed == 0);
      }
    }

    // Warning: We are not sure that other.ptr_ is valid.
    // Warning: Cannot call other.ptr_->Type().IsDerivedFromType (T::StaticObjectType)
    /*template <typename O>
    ObjectWeakPtr (const ObjectWeakPtr<O>& other)*/

    //! Construction from a smart pointer of type O that inherits from type T.
    /*!
        @param other Maintains a weak smart pointer reference to this parameter.
    */
    template <typename O>
    ObjectWeakPtr (const ObjectPtr<O> &other)
      :   ptr_ (0)
    {
      _reference_count = 0;
      _weak_reference_count = 0;
      _destroyed = 0;

      if (other.ptr_ && other.ptr_->Type ().IsDerivedFromType (T::StaticObjectType))
      {
        ptr_ = other.ptr_;
        _reference_count = other._reference_count;
        _weak_reference_count = other._weak_reference_count;
        _destroyed = other._destroyed;

        if (ptr_ != 0)
        {
          _weak_reference_count->Increment ();
        }
        else
        {
          // Sanity checks
          nuxAssert (_reference_count == 0);
          nuxAssert (_weak_reference_count == 0);
          nuxAssert (_destroyed == 0);
        }
      }
    }

    //! Assignment of a weak smart pointer of type T.
    /*!
        @param other Weak smart pointer of type T.
    */
    ObjectWeakPtr& operator = (const ObjectWeakPtr<T> &other)
    {
      if (GetPointer () != other.GetPointer () ) // Avoid self assignment.
      {
        ReleaseReference ();

        ptr_ = other.ptr_;
        _reference_count = other._reference_count;
        _weak_reference_count = other._weak_reference_count;
        _destroyed = other._destroyed;

        // Warning: We cannot count on ptr_ to be valid.
        // If _weak_reference_count is not null, then imcrement it.
        if (_weak_reference_count)
        {
          _weak_reference_count->Increment ();
        }
        else
        {
          // Sanity checks
          nuxAssert (_reference_count == 0);
          nuxAssert (_weak_reference_count == 0);
          nuxAssert (_destroyed == 0);
        }       
      }

      return *this;
    }

    // Warning: We are not sure that other.ptr_ is valid.
    // Warning: Cannot call other.ptr_->Type().IsDerivedFromType (T::StaticObjectType)
    /*template <typename O>
    ObjectWeakPtr& operator = (const ObjectWeakPtr<O> &other)*/

    //! Assignment of a smart pointer of Type T.
    /*!
        @param other Maintains a smart pointer reference to this parameter.
    */
    ObjectWeakPtr& operator = (const ObjectPtr<T> &other)
    {
      if (GetPointer () != other.ptr_) // Avoid self assignment.
      {
        ReleaseReference ();

        ptr_ = other.ptr_;
        //refCounts_ = other.refCounts_;
        _reference_count = other._reference_count;
        _weak_reference_count = other._weak_reference_count;
        _destroyed = other._destroyed;

        if (ptr_ != 0)
        {
          _weak_reference_count->Increment ();
        }
      }
      else
      {
        ReleaseReference ();
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
      if (other.ptr_ && other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) )
      {
        if (GetPointer () != other.ptr_) // Avoid self assignment.
        {
          ReleaseReference ();

          ptr_ = other.ptr_;
          //refCounts_ = other.refCounts_;
          _reference_count = other._reference_count;
          _weak_reference_count = other._weak_reference_count;
          _destroyed = other._destroyed;

          if (ptr_ != 0)
          {
            //refCounts_->totalRefs_.Increment();
            _weak_reference_count->Increment();
          }
        }
      }
      else
      {
        ReleaseReference();
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
    ObjectWeakPtr& operator = (T *ptr)
    {
      if (ptr_ && _reference_count && (_reference_count->GetValue() != 0) )
      {
        ptr_->DecrementWeakCounter();
      }
      else if (_reference_count && _weak_reference_count)
      {
        _weak_reference_count->Decrement();
      }

      if (_reference_count && _weak_reference_count && (_reference_count->GetValue() == 0) && (_weak_reference_count->GetValue() == 0) )
      {
        if (!(*_destroyed))
        {
          // The object is between Object::Destroy() and Object::~Object()
          ptr_->_reference_count = 0;
          ptr_->_weak_reference_count = 0;
        }
        else
        {
          // The object has been destroyed and this is the last weak reference to it.
          delete _destroyed;
        }
        delete _reference_count;
        delete _weak_reference_count;
        
      }
      
      ptr_ = 0;
      _reference_count = 0;
      _weak_reference_count = 0;
      _destroyed = 0;

      if (ptr != 0)
      {
        if (ptr->OwnsTheReference() == false)
        {
          //nuxDebugMsg (TEXT ("[ObjectWeakPtr::operator = ()] Warning: Constructing a weak smart pointer from an object with a floating reference.") );
        }

        ptr_ = ptr;
        _reference_count = ptr->_reference_count;
        _weak_reference_count = ptr->_weak_reference_count;
        _destroyed = ptr->_destroyed;
        ptr_->IncrementWeakCounter();
      }

      return *this;
    }

    //! Construction with a base pointer of type O that inherits from type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param WarnMissuse If True, then ObjectPtr test is ptr is owned or not. If ptr is not owned
        and WarnMissuse is True, then Print a warning message. This is a debug feature to detect cases such as
        "ObjectPtr(ObjectA) myobject(ptr);", because the calling code will no longer have a reference on ptr.
    */
    template <typename O>
    ObjectWeakPtr &operator = (O *ptr)
    {
      if (ptr_ && _reference_count && (_reference_count->GetValue() != 0) )
      {
        ptr_->DecrementWeakCounter();
      }
      else if (_reference_count && _weak_reference_count)
      {
        _weak_reference_count->Decrement();
      }
      else
      {
        nuxAssertMsg (0, TEXT ("Could there be something wrong her?") );
      }

      if (_reference_count && _weak_reference_count && (_reference_count->GetValue() == 0) && (_weak_reference_count->GetValue() == 0) )
      {
        if (!(*_destroyed))
        {
          // The object is between Object::Destroy() and Object::~Object()
          ptr_->_reference_count = 0;
          ptr_->_weak_reference_count = 0;
        }
        else
        {
          // The object has been destroyed and this is the last weak reference to it.
          delete _destroyed;
        }
        delete _reference_count;
        delete _weak_reference_count;
      }

      ptr_ = 0;
      _reference_count = 0;
      _weak_reference_count = 0;
      _destroyed = 0;

      if (ptr != 0)
      {
        if (ptr->Type().IsDerivedFromType (T::StaticObjectType) )
        {
          if (ptr->OwnsTheReference() == false)
          {
            //nuxDebugMsg (TEXT ("[ObjectWeakPtr::operator = ()] Warning: Constructing a weak smart pointer from an object with a floating reference.") );
          }

          ptr_ = (T *) ptr;
          _reference_count = ptr->_reference_count;
          _weak_reference_count = ptr->_weak_reference_count;
          _destroyed = ptr->_destroyed;
          ptr_->IncrementWeakCounter();
        }
      }
    }

    ~ObjectWeakPtr ()
    {
      ReleaseReference ();
    }

    T &operator * () const
    {
      nuxAssert (_reference_count && (_reference_count->GetValue() != 0) && (ptr_ != 0) );

      return *(NUX_CONST_CAST (T*, GetPointer ()));
    }

    T *operator -> () const
    {
      nuxAssert (_reference_count && (_reference_count->GetValue() != 0) && (ptr_ != 0) );

      return NUX_CONST_CAST (T*, GetPointer ());
    }

//     void Swap (ObjectWeakPtr<T>& other)
//     {
//         std::swap (ptr_, other.ptr_);
//         std::swap (refCounts_, other.refCounts_);
//     }

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

    bool operator != (T *ptr) const
    {
      return ( (void *) ptr_ != (void *) ptr);
    }

    bool operator == (T *ptr) const
    {
      return ( (void *) ptr_ == (void *) ptr);
    }

    template<typename U>
    bool operator != (U *ptr) const
    {
      if (ptr && (!ptr->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return true;

      return ( (void *) ptr_ != (void *) ptr);
    }

    template<typename U>
    bool operator == (U *ptr) const
    {
      if (ptr && (!ptr->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ( (void *) ptr_ == (void *) ptr);
    }

    /*!
        @other  A weak pointer
        @return True is this weak pointer host a different pointer as the weak pointer passed as parameter.
    */
    template<typename U>
    bool operator != (const ObjectWeakPtr<U>& other) const
    {
      // Test if the object in other has been destroyed. If yes, then we can't call other.ptr_->Type().
      if (other._reference_count->GetValue () != 0)
      {
        if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
          return true;
      }

      return ( (void *) ptr_ != (void *) other.ptr_);
    }

    /*!
        @other  A weak pointer
        @return True is this weak pointer host the same pointer as the weak pointer passed as parameter.
    */
    template<typename U>
    bool operator == (const ObjectWeakPtr<U>& other) const
    {
      // Test if the object in other has been destroyed. If yes, then we can't call other.ptr_->Type().
      if (other._reference_count->GetValue () != 0)
      {
        if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType)))
          return false;
      }

      return ( (void *) ptr_ == (void *) other.ptr_);
    }

    /*!
        @other  An object pointer
        @return True is this weak pointer host a different pointer as the object pointer passed as parameter.
    */
    template<typename U>
    bool operator != (const ObjectPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return true;

      return ( (void *) ptr_ != (void *) other.ptr_);
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

      return ( (void *) ptr_ == (void *) other.ptr_);
    }

    //! Return true is the hosted pointer is not null or has not been destroyed.
    /*!
        @return True if the internal pointer is not null.
    */
    bool operator () () const
    {
      return GetPointer() != 0;
    }
    
    //! Return true is the hosted pointer is not null or has not been destroyed.
    /*!
        @return True if the internal pointer is not null or has not been destroyed.
    */
    bool IsValid() const
    {
      return GetPointer() != 0;
    }

    //! Return true is the hosted pointer is null or has been destroyed.
    /*!
        @return True if the internal pointer is null or has been destroyed.
    */
    bool IsNull() const
    {
      return GetPointer() == 0;
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
      return ReleaseReference ();
    }

    //! Return the stored pointer.
    /*!
        Caller of this function should Reference the pointer if they intend to keep it.
        @param Return the stored pointer.
    */
    const T* GetPointer () const
    {
      if ((_weak_reference_count == 0) || (_weak_reference_count->GetValue () == 0))
      {
        return 0;
      }

      if ((_reference_count == 0) || (_reference_count->GetValue () == 0))
      {
        return 0;
      }

      return ptr_;
    }

    //! Return the stored pointer.
    /*!
        Caller of this function should Reference the pointer if they intend to keep it.
        @param Return the stored pointer.
    */
    T *GetPointer() 
    {
      return NUX_CONST_CAST (T*, (const_cast< const ObjectWeakPtr* > (this))->GetPointer ());
    }

    int GetReferenceCount ()
    {
      if (_reference_count == 0)
      {
        return 0;
      }

      return _reference_count()->GetValue ();
    }

    int GetWeakReferenceCount ()
    {
      if (_weak_reference_count == 0)
      {
        return 0;
      }

      return _weak_reference_count->GetValue ();
    }

  private:
    bool ReleaseReference ()
    {
      if (ptr_ == 0)
      {
        return false;
      }

      nuxAssert (_weak_reference_count->GetValue() >= 1);

      _weak_reference_count->Decrement();
      bool delete_warning = (_weak_reference_count->GetValue() == 0);

      if (delete_warning)
      {
        if (!(*_destroyed))
        {
          // The object is between Object::Destroy() and Object::~Object()
          ptr_->_reference_count = 0;
          ptr_->_weak_reference_count = 0;
        }
        else
        {
          // The object has been destroyed and this is the last weak reference to it.
          delete _destroyed;
        }
        delete _reference_count;
        delete _weak_reference_count;
      }

      _reference_count = 0;
      _weak_reference_count = 0;
      ptr_ = 0;

      return delete_warning; 
    }

    T *ptr_;
    NThreadSafeCounter *_reference_count;
    NThreadSafeCounter *_weak_reference_count;
    bool               *_destroyed;

    template <typename O>
    friend class ObjectWeakPtr;

    template <typename O>
    friend class SmartPtr;

    //     template<typename T, typename U>
    //     friend bool operator == (const ObjectWeakPtr<T>& a, const ObjectWeakPtr<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator != (const ObjectWeakPtr<T>& a, const ObjectWeakPtr<U>& b);

    //     template<typename T>
    //     friend bool operator == (const ObjectWeakPtr<T>& a, T*);

    template<typename U>
    friend bool operator == (U *, const ObjectWeakPtr<U>& a);

    //     template<typename T>
    //     friend bool operator != (const ObjectWeakPtr<T>& a, T*);

    template<typename U>
    friend bool operator != (U *, const ObjectWeakPtr<U>& a);

    //     template<typename T, typename U>
    //     friend bool operator == (const ObjectPtr<T>& a, const ObjectWeakPtr<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator == (const ObjectWeakPtr<T>& a, const ObjectPtr<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator != (const ObjectPtr<T>& a, const ObjectWeakPtr<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator != (const ObjectWeakPtr<T>& a, const ObjectPtr<U>& b);

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

// template<typename T, typename U>
// inline bool operator == (const ObjectPtr<T>& a, const ObjectPtr<U>& b)
// {
//     return a.ptr_ == b.ptr_;
// }
//
// template<typename T, typename U>
// inline bool operator == (const ObjectWeakPtr<T>& a, const ObjectWeakPtr<U>& b)
// {
//     return a.GetPointer () == b.GetPointer ();
// }
//
// template<typename T, typename U>
// inline bool operator == (const ObjectPtr<T>& a, const ObjectWeakPtr<U>& b)
// {
//     return a.ptr_ == b.GetPointer ();
// }
//
// template<typename T, typename U>
// inline bool operator == (const ObjectWeakPtr<T>& a, const ObjectPtr<U>& b)
// {
//     return a.GetPointer () == b.ptr_;
// }
//
// template<typename T, typename U>
// inline bool operator != (const ObjectPtr<T>& a, const ObjectPtr<U>& b)
// {
//     return a.ptr_ != b.ptr_;
// }
//
// template<typename T, typename U>
// inline bool operator != (const ObjectWeakPtr<T>& a, const ObjectWeakPtr<U>& b)
// {
//     return a.GetPointer () != b.GetPointer ();
// }
//
// template<typename T, typename U>
// inline bool operator != (const ObjectPtr<T>& a, const ObjectWeakPtr<U>& b)
// {
//     return a.ptr_ != b.GetPointer ();
// }
//
// template<typename T, typename U>
// inline bool operator != (const ObjectWeakPtr<T>& a, const ObjectPtr<U>& b)
// {
//     return a.GetPointer () != b.ptr_;
// }

// template<typename T>
// inline bool operator == (const ObjectPtr<T>& a, T* ptr)
// {
//     return a.ptr_ == ptr;
// }

  template<typename T>
  inline bool operator == (T *ptr, const ObjectPtr<T>& a)
  {
    return a.ptr_ == ptr;
  }

// template<typename T>
// inline bool operator != (const ObjectPtr<T>& a, T* ptr)
// {
//     return a.ptr_ != ptr;
// }

  template<typename T>
  inline bool operator != (T *ptr, const ObjectPtr<T>& a)
  {
    return a.ptr_ != ptr;
  }

// template<typename T>
// inline bool operator == (const ObjectWeakPtr<T>& a, T* ptr)
// {
//     return a.ptr_ == ptr;
// }

  template<typename T>
  inline bool operator == (T *ptr, const ObjectWeakPtr<T>& a)
  {
    return a.ptr_ == ptr;
  }

// template<typename T>
// inline bool operator != (const ObjectWeakPtr<T>& a, T* ptr)
// {
//     return a.ptr_ != ptr;
// }

  template<typename T>
  inline bool operator != (T *ptr, const ObjectWeakPtr<T>& a)
  {
    return a.ptr_ != ptr;
  }

// ///////////////////////////////////////////////////////
// // creation functions
//
// template <typename T>
// ObjectPtr<T> Create ()
// {
//     RefCounts* rc = new RefCounts;
//
//     try
//     {
//         T* t = new T;
//         return ObjectPtr<T> (t, rc);
//     }
//     catch (...)
//     {
//         delete rc;
//         throw;
//     }
// }
//
// template <typename T, typename P1>
// ObjectPtr<T> Create (P1 p1)
// {
//     RefCounts* rc = new RefCounts;
//
//     try
//     {
//         T* t = new T (p1);
//         return ObjectPtr<T> (t, rc);
//     }
//     catch (...)
//     {
//         delete rc;
//         throw;
//     }
// }
//
// template <typename T, typename P1, typename P2>
// ObjectPtr<T> Create (P1 p1, P2 p2)
// {
//     RefCounts* rc = new RefCounts;
//
//     try
//     {
//         T* t = new T (p1, p2);
//         return ObjectPtr<T> (t, rc);
//     }
//     catch (...)
//     {
//         delete rc;
//         throw;
//     }
// }
//
// template <typename T, typename P1, typename P2, typename P3>
// ObjectPtr<T> Create (P1 p1, P2 p2, P3 p3)
// {
//     RefCounts* rc = new RefCounts;
//
//     try
//     {
//         T* t = new T (p1, p2, p3);
//         return ObjectPtr<T> (t, rc);
//     }
//     catch (...)
//     {
//         delete rc;
//         throw;
//     }
// }
//
// template <typename T, typename P1, typename P2, typename P3, typename P4>
// ObjectPtr<T> Create (P1 p1, P2 p2, P3 p3, P4 p4)
// {
//     RefCounts* rc = new RefCounts;
//
//     try
//     {
//         T* t = new T (p1, p2, p3, p4);
//         return ObjectPtr<T> (t, rc);
//     }
//     catch (...)
//     {
//         delete rc;
//         throw;
//     }
// }
//
// template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5>
// ObjectPtr<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
// {
//     RefCounts* rc = new RefCounts;
//
//     try
//     {
//         T* t = new T (p1, p2, p3, p4, p5);
//         return ObjectPtr<T> (t, rc);
//     }
//     catch (...)
//     {
//         delete rc;
//         throw;
//     }
// }
//
// template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
// ObjectPtr<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
// {
//     RefCounts* rc = new RefCounts;
//
//     try
//     {
//         T* t = new T (p1, p2, p3, p4, p5, p6);
//         return ObjectPtr<T> (t, rc);
//     }
//     catch (...)
//     {
//         delete rc;
//         throw;
//     }
// }
//
// template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
// ObjectPtr<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
// {
//     RefCounts* rc = new RefCounts;
//
//     try
//     {
//         T* t = new T (p1, p2, p3, p4, p5, p6, p7);
//         return ObjectPtr<T> (t, rc);
//     }
//     catch (...)
//     {
//         delete rc;
//         throw;
//     }
// }
//
// template <typename T>
// ObjectPtr<T> WrapWSPtr (T* t)
// {
//     if (t == 0)
//     {
//         return ObjectPtr<T> ();
//     }
//
//     try
//     {
//         RefCounts* rc = new RefCounts;
//
//         return ObjectPtr<T> (t, rc);
//     }
//     catch (...)
//     {
//         delete t;
//         throw;
//     }
// }
//
// ///////////////////////////////////////////////////////
// // casts
//
// template <typename U, typename F>
// ObjectPtr<U> staticCast (const ObjectPtr<F>& from)
// {
//     if (from.ptr_ == 0)
//     {
//         return ObjectPtr<U>();
//     }
//
//     U* ptr = static_cast <U*> (from.ptr_);
//     RefCounts* refCounts = from.refCounts_;
//
//     if (ptr != 0)
//     {
//         //refCounts->strongRefs_.Increment();
//         //refCounts->totalRefs_.Increment();
//         from._reference_count->Increment();
//         from._weak_reference_count->Increment();
//     }
//
//     return ObjectPtr<U> (ptr, refCounts);
// }
//
// template <typename T, typename F>
// ObjectPtr<T> constCast (const ObjectPtr<F>& from)
// {
//     if (from.ptr_ == 0)
//     {
//         return ObjectPtr<T>();
//     }
//
//     T* ptr = const_cast <T*> (from.ptr_);
//     RefCounts* refCounts = from.refCounts_;
//
//     if (ptr != 0) {
//         //refCounts->strongRefs_.Increment();
//         //refCounts->totalRefs_.Increment();
//         from._reference_count->Increment();
//         from._weak_reference_count->Increment();
//     }
//
//     return ObjectPtr<T> (ptr, refCounts);
// }
//
// template <typename T, typename F>
// ObjectPtr<T> dynamicCast (const ObjectPtr<F>& from)
// {
//     if (from.ptr_ == 0)
//     {
//         return ObjectPtr<T>();
//     }
//
//     T* ptr = &dynamic_cast <T&> (*from.ptr_);
//     RefCounts* refCounts = from.refCounts_;
//
//     if (ptr != 0)
//     {
//         //refCounts->strongRefs_.Increment();
//         //refCounts->totalRefs_.Increment();
//         from._reference_count->Increment();
//         from._weak_reference_count->Increment();
//     }
//
//     return ObjectPtr<T> (ptr, refCounts);
// }
//
// template <typename T, typename F>
// ObjectPtr<T> queryCast (const ObjectPtr<F>& from)
// {
//     T* ptr = dynamic_cast <T*> (from.ptr_);
//
//     if (ptr == 0)
//     {
//         return ObjectPtr<T>();
//     }
//
//     RefCounts* refCounts = from.refCounts_;
//
//     if (ptr != 0)
//     {
//         //refCounts->strongRefs_.Increment();
//         //refCounts->totalRefs_.Increment();
//         from._reference_count->Increment();
//         from._weak_reference_count->Increment();
//     }
//
//     return ObjectPtr<T> (ptr, refCounts);
// }
//
// template <typename T, typename F>
// ObjectPtr<T> checkedCast (const ObjectPtr<F>& from)
// {
//     if (from.ptr_ == 0)
//     {
//         return ObjectPtr<T>();
//     }
//
//     nuxAssert(dynamic_cast<T*> (from.ptr_) != 0);
//
//     T* ptr = static_cast <T*> (from.ptr_);
//     RefCounts* refCounts = from.refCounts_;
//
//     if (ptr != 0)
//     {
//         //refCounts->strongRefs_.Increment();
//         //refCounts->totalRefs_.Increment();
//         from._reference_count->Increment();
//         from._weak_reference_count->Increment();
//     }
//
//     return ObjectPtr<T> (ptr, refCounts);
// }
//
// template <typename U, typename F>
// ObjectWeakPtr<U> staticCast (const ObjectWeakPtr<F>& from)
// {
//     if (from.GetPointer () == 0)
//     {
//         return ObjectWeakPtr<U>();
//     }
//
//     U* ptr = static_cast <U*> (from.ptr_);
//     RefCounts* refCounts = from.refCounts_;
//
//     if (ptr != 0)
//     {
//         refCounts->totalRefs_.Increment();
//     }
//
//     return ObjectWeakPtr<U> (ptr, refCounts);
// }
//
// template <typename T, typename F>
// ObjectWeakPtr<T> constCast (const ObjectWeakPtr<F>& from)
// {
//     if (from.GetPointer () == 0)
//     {
//         return ObjectWeakPtr<T>();
//     }
//
//     T* ptr = const_cast <T*> (from.ptr_);
//     RefCounts* refCounts = from.refCounts_;
//
//     if (ptr != 0)
//     {
//         //refCounts->totalRefs_.Increment();
//         from._weak_reference_count->Increment();
//     }
//
//     return ObjectWeakPtr<T> (ptr, refCounts);
// }
//
// template <typename T, typename F>
// ObjectWeakPtr<T> dynamicCast (const ObjectWeakPtr<F>& from)
// {
//     if (from.GetPointer () == 0)
//     {
//         return ObjectWeakPtr<T>();
//     }
//
//     T* ptr = &dynamic_cast <T&> (*from.ptr_);
//     RefCounts* refCounts = from.refCounts_;
//
//     if (ptr != 0)
//     {
//         //refCounts->totalRefs_.Increment();
//         from._weak_reference_count->Increment();
//     }
//
//     return ObjectWeakPtr<T> (ptr, refCounts);
// }
//
// template <typename T, typename F>
// ObjectWeakPtr<T> queryCast (const ObjectWeakPtr<F>& from)
// {
//     T* ptr = dynamic_cast <T*> (from.GetPointer ());
//
//     if (ptr == 0)
//     {
//         return ObjectWeakPtr<T>();
//     }
//
//     RefCounts* refCounts = from.refCounts_;
//
//     if (ptr != 0)
//     {
//         //refCounts->totalRefs_.Increment();
//         from._weak_reference_count->Increment();
//     }
//
//     return ObjectWeakPtr<T> (ptr, refCounts);
// }
//
// template <typename T, typename F>
// ObjectWeakPtr<T> checkedCast (const ObjectWeakPtr<F>& from)
// {
//     if (from.GetPointer () == 0)
//     {
//         return ObjectWeakPtr<T>();
//     }
//
//     nuxAssert(dynamic_cast<T*> (from.ptr_) != 0);
//
//     T* ptr = static_cast <T*> (from.ptr_);
//     RefCounts* refCounts = from.refCounts_;
//
//     if (ptr != 0)
//     {
//         //refCounts->totalRefs_.Increment();
//         from._weak_reference_count->Increment();
//     }
//
//     return ObjectWeakPtr<T> (ptr, refCounts);
// }

// ///////////////////////////////////////////////////////
// // std specializations
//
// template <typename T>
// inline void swap (ObjectPtr<T>& t1, ObjectPtr<T>& t2)
// {
//     t1.swap (t2);
// }
//
// template <typename T>
// inline void swap (ObjectWeakPtr<T>& t1, ObjectWeakPtr<T>& t2)
// {
//     t1.swap (t2);
// }


}

#endif // INTRUSIVESP_H


