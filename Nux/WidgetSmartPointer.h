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


#ifndef WIDGETSMARTPOINTER_H
#define WIDGETSMARTPOINTER_H

namespace nux
{
// // AtomicInt
// typedef long AtomicInt;
//
// inline void atomicIncrement (AtomicInt& i)
// {
//     InterlockedIncrement (&i);
// }
//
// inline bool atomicDecrement (AtomicInt& i)
// {
//     return InterlockedDecrement (&i) == 0;
// }

  inline bool isZero (NThreadSafeCounter &i)
  {
    return i == 0;
  }

///////////////////////////////////////////////////////
// handy helpers

  struct RefCounts
  {
    RefCounts () : totalRefs_ (1), strongRefs_ (1)
    {
    }

    NThreadSafeCounter totalRefs_;
    NThreadSafeCounter strongRefs_;
  };

  enum Null
  {
    null = 0
  };

///////////////////////////////////////////////////////
// forward definitions

  template <typename T>
  class WeakWSPtr;

  template <typename T>
  class WSPtr;

//! A smart pointer class. Implemented as an intrusive smart pointer.
  template <typename T>
  class WSPtr
  {
  public:
    //! Constructor
    WSPtr()
      :   ptr_ (0)
      , refCounts_ (0)
      , deletewhenrefcounthitzero_ (true)
    {
    }

    //! Copy constructor
    WSPtr (const WSPtr<T>& other)
      :   ptr_ (0)
      , refCounts_ (0)
      , deletewhenrefcounthitzero_ (true)
    {
      ptr_ = other.ptr_;
      refCounts_ = other.refCounts_;
      deletewhenrefcounthitzero_ = other.deletewhenrefcounthitzero_;

      if (ptr_ != 0)
      {
        refCounts_->strongRefs_.Increment();
        refCounts_->totalRefs_.Increment();
      }
    }

    //! Copy constructor
    /*!
        @param other Parameter with a type derived from T.
    */
    template <typename O>
    WSPtr (const WSPtr<O>& other)
      :   ptr_ (0)
      , refCounts_ (0)
      , deletewhenrefcounthitzero_ (true)
    {
      if (other.ptr_ && other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) )
      {
        ptr_ = (T *) other.ptr_;
        refCounts_ = other.refCounts_;
        deletewhenrefcounthitzero_ = other.deletewhenrefcounthitzero_;
      }

      if (ptr_ != 0)
      {
        refCounts_->strongRefs_.Increment();
        refCounts_->totalRefs_.Increment();
      }
    }

    //! Construction with a base pointer of type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param deletewhenrefcounthitzero if True, the pointer is deleted when the ref count reach 0 (to be deprecated).
    */
    explicit WSPtr (T *ptr, bool deletewhenrefcounthitzero = true)
      :   ptr_ (0)
      ,   refCounts_ (0)
      ,   deletewhenrefcounthitzero_ (true)
    {
      if (ptr != 0)
      {
        ptr_ = ptr;
        refCounts_ = new RefCounts;
        deletewhenrefcounthitzero_ = deletewhenrefcounthitzero;;
      }
    }

    //! Construction with a base pointer of type O that inherits from type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param deletewhenrefcounthitzero if True, the pointer is deleted when the ref count reach 0 (to be deprecated).
    */
    template <typename O>
    explicit WSPtr (O *ptr, bool deletewhenrefcounthitzero = true)
      :   ptr_ (0)
      , refCounts_ (0)
      , deletewhenrefcounthitzero_ (true)
    {
      if (ptr != 0)
      {
        if (ptr->Type().IsDerivedFromType (T::StaticObjectType) )
        {
          ptr_ = (T *) ptr;
          refCounts_ = new RefCounts;
          deletewhenrefcounthitzero_ = deletewhenrefcounthitzero;
        }
        else
        {
          // This is possible but check why!
          // It can happen when doing something like this:
          //      Layout* layout = this;
          // where this is a Baseobject.
          nuxAssert (0);
        }
      }
    }

    //! Assignment of a smart pointer of type T.
    /*!
        @param other Smart pointer of type T.
    */
    WSPtr &operator = (const WSPtr<T>& other)
    {
      if (ptr_ != other.ptr_)
      {
        ReleaseReference ();

        ptr_ = other.ptr_;
        refCounts_ = other.refCounts_;
        deletewhenrefcounthitzero_ = other.deletewhenrefcounthitzero_;

        if (ptr_ != 0)
        {
          refCounts_->strongRefs_.Increment();
          refCounts_->totalRefs_.Increment();
        }
      }

      return *this;
    }

    //! Assignment of a smart pointer of type O that inherits from type T.
    /*!
        @param other Smart pointer of type O.
    */
    template <typename O>
    WSPtr &operator = (const WSPtr<O>& other)
    {
      if (other.ptr_ && other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) )
      {
        if (ptr_ != other.ptr_)
        {
          ReleaseReference ();

          ptr_ = other.ptr_;
          refCounts_ = other.refCounts_;
          deletewhenrefcounthitzero_ = other.deletewhenrefcounthitzero_;

          if (ptr_ != 0)
          {
            refCounts_->strongRefs_.Increment();
            refCounts_->totalRefs_.Increment();
          }
        }
      }
      else
      {
        ReleaseReference ();
      }

      return *this;
    }

    ~WSPtr ()
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

    //! Swap the content of 2 smart pointers.
    /*!
        @param other Smart pointer to swap with.
    */
    void Swap (WSPtr<T>& other)
    {
      std::swap (ptr_, other.ptr_);
      std::swap (refCounts_, other.refCounts_);
    }

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

    bool operator < (WSPtr<T> other) const
    {
      return (ptr_ < other.ptr_);
    }

    bool operator > (WSPtr<T> other) const
    {
      return (ptr_ > other.ptr_);
    }

    bool operator != (T *ptr) const
    {
      return (ptr_ != ptr);
    }

    bool operator == (T *ptr) const
    {
      return (ptr_ == ptr);
    }

    template<typename U>
    bool operator != (U *ptr) const
    {
      if (ptr && (!ptr->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return (ptr_ != ptr);
    }

    template<typename U>
    bool operator == (U *ptr) const
    {
      if (ptr && (!ptr->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return (ptr_ == ptr);
    }

    template<typename U>
    bool operator != (const WSPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ != other.ptr_;
    }

    template<typename U>
    bool operator == (const WSPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ == other.ptr_;
    }

    template<typename U>
    bool operator != (const WeakWSPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ != other.ptr_;
    }

    template<typename U>
    bool operator == (const WeakWSPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ == other.ptr_;
    }

    void Release()
    {
      ReleaseReference();
    }

  private:
    WSPtr (T *ptr, RefCounts *refCounts) : ptr_ (ptr), refCounts_ (refCounts)
    {
    }

    void ReleaseReference()
    {
      if (ptr_ == 0)
      {
        return;
      }

      {
        refCounts_->strongRefs_.Decrement();
        bool release = refCounts_->strongRefs_ == 0;

        if (release && deletewhenrefcounthitzero_)
        {
          delete ptr_;
        }

        ptr_ = 0;
      }

      {
        refCounts_->totalRefs_.Decrement();
        bool release = refCounts_->totalRefs_ == 0;

        if (release)
        {
          delete refCounts_;
        }

        refCounts_ = 0;
      }
    }

    T *ptr_;
    RefCounts *refCounts_;
    bool deletewhenrefcounthitzero_;

    template <typename U>
    friend WSPtr<U> Create ();

    template <typename U, typename P1>
    friend WSPtr<U> Create (P1 p1);

    template <typename U, typename P1, typename P2>
    friend WSPtr<U> Create (P1 p1, P2 p2);

    template <typename U, typename P1, typename P2, typename P3>
    friend WSPtr<U> Create (P1 p1, P2 p2, P3 p3);

    template <typename U, typename P1, typename P2, typename P3, typename P4>
    friend WSPtr<U> Create (P1 p1, P2 p2, P3 p3, P4 p4);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5>
    friend WSPtr<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    friend WSPtr<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
    friend WSPtr<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);

    template <typename U>
    friend WSPtr<U> WrapWSPtr (U *u);

    template <typename O>
    friend class WSPtr;

    template <typename O>
    friend class WeakWSPtr;

    //     template<typename T, typename U>
    //     friend bool operator == (const WSPtr<T>& a, const WSPtr<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator != (const WSPtr<T>& a, const WSPtr<U>& b);

    //     template<typename T>
    //     friend bool operator == (const WSPtr<T>& a, T*);

    template<typename U>
    friend bool operator == (U *, const WSPtr<U>& a);

    //     template<typename T>
    //     friend bool operator != (const WSPtr<T>& a, T*);

    template<typename U>
    friend bool operator != (U *, const WSPtr<U>& a);

    //     template<typename T, typename U>
    //     friend bool operator == (const WSPtr<T>& a, const WeakWSPtr<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator == (const WeakWSPtr<T>& a, const WSPtr<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator != (const WSPtr<T>& a, const WeakWSPtr<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator != (const WeakWSPtr<T>& a, const WSPtr<U>& b);

    template <typename U, typename F>
    friend WSPtr<U> staticCast (const WSPtr<F>& from);

    template <typename U, typename F>
    friend WSPtr<U> constCast (const WSPtr<F>& from);

    template <typename U, typename F>
    friend WSPtr<U> dynamicCast (const WSPtr<F>& from);

    template <typename U, typename F>
    friend WSPtr<U> checkedCast (const WSPtr<F>& from);

    template <typename U, typename F>
    friend WSPtr<U> queryCast (const WSPtr<F>& from);
  };


//! A weak smart pointer class. Implemented as an intrusive smart pointer.
  /*!
      A weak smart pointer is built from a smart pointer or another weak smart pointer. It increments and decrements
      the total reference count of an pointer. Even is the original pointer is destroyed, weak smart pointers still point
      to the RefCounts pointers of the original pointer and can use it to check if the pointer is still valid or not.
  */
  template <typename T>
  class WeakWSPtr
  {
  public:
    //! Constructor
    WeakWSPtr () : ptr_ (0), refCounts_ (0)
    {
    }

    //! Copy constructor
    /*!
        @param other Parameter with type T.
    */
    WeakWSPtr (const WeakWSPtr<T>& other)
    {
      ptr_ = other.ptr_;
      refCounts_ = other.refCounts_;

      if (ptr_ != 0)
      {
        refCounts_->totalRefs_.Increment();
      }
    }

    //! Copy constructor
    /*!
        @param other Parameter with a type derived from T.
    */
    template <typename O>
    WeakWSPtr (const WeakWSPtr<O>& other)
      :   ptr_ (0)
      ,   refCounts_ (0)
    {
      if (other.ptr_ && other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) )
      {
        ptr_ = other.ptr_;
        refCounts_ = other.refCounts_;

        if (ptr_ != 0)
        {
          refCounts_->totalRefs_.Increment();
        }
      }
    }

//     //! Construction from a smart pointer of type T.
//     /*!
//         @param other Maintains a weak smart pointer reference to this parameter.
//     */
//     WeakWSPtr (const WSPtr<T>& other)
//         :   ptr_(0)
//         ,   refCounts_(0)
//     {
//         if(other.ptr_)
//         {
//             ptr_ = other.ptr_;
//             refCounts_ = other.refCounts_;
//
//             if (ptr_ != 0)
//             {
//                 refCounts_->totalRefs_.Increment();
//             }
//         }
//     }

    //! Construction from a smart pointer of type O that inherits from type T.
    /*!
        @param other Maintains a weak smart pointer reference to this parameter.
    */
    template <typename O>
    WeakWSPtr (const WSPtr<O>& other)
      :   ptr_ (0)
      ,   refCounts_ (0)
    {
      if (other.ptr_ && other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) )
      {
        ptr_ = other.ptr_;
        refCounts_ = other.refCounts_;

        if (ptr_ != 0)
        {
          refCounts_->totalRefs_.Increment();
        }
      }
    }

    //! Assignment of a weak smart pointer of type T.
    /*!
        @param other Weak smart pointer of type T.
    */
    WeakWSPtr &operator = (const WeakWSPtr<T>& other)
    {
      if (get () != other.get () )
      {
        ReleaseReference ();

        ptr_ = other.ptr_;
        refCounts_ = other.refCounts_;

        if (ptr_ != 0)
        {
          refCounts_->totalRefs_.Increment();
        }
      }

      return *this;
    }

    //! Assignment of a weak smart pointer of Type O that inherits from type T.
    /*!
        @param other Weak smart pointer of type O.
    */
    template <typename O>
    WeakWSPtr &operator = (const WeakWSPtr<O>& other)
    {
      if (other.ptr_ && other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) )
      {
        if (get () != other.get () )
        {
          ReleaseReference ();

          ptr_ = other.ptr_;
          refCounts_ = other.refCounts_;

          if (ptr_ != 0)
          {
            refCounts_->totalRefs_.Increment();
          }
        }
      }
      else
      {
        ReleaseReference();
      }

      return *this;
    }

    //! Assignment of a smart pointer of Type O that inherits from type T.
    /*!
        @param other Maintains a weak smart pointer reference to this parameter.
    */
    template <typename O>
    WeakWSPtr &operator = (const WSPtr<O>& other)
    {
      if (other.ptr_ && other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) )
      {
        if (get () != other.ptr_)
        {
          ReleaseReference ();

          ptr_ = other.ptr_;
          refCounts_ = other.refCounts_;

          if (ptr_ != 0)
          {
            refCounts_->totalRefs_.Increment();
          }
        }
      }
      else
      {
        ReleaseReference();
      }

      return *this;
    }

    ~WeakWSPtr ()
    {
      ReleaseReference ();
    }

    T &operator * () const
    {
      nuxAssert (!isZero (refCounts_->strongRefs_) && ptr_ != 0);

      return *get ();
    }

    T *operator -> () const
    {
      nuxAssert (!isZero (refCounts_->strongRefs_) && ptr_ != 0);

      return get ();
    }

    void Swap (WeakWSPtr<T>& other)
    {
      std::swap (ptr_, other.ptr_);
      std::swap (refCounts_, other.refCounts_);
    }

    bool operator < (T *ptr) const
    {
      return (ptr_ < ptr);
    }

    bool operator > (T *ptr) const
    {
      return (ptr_ > ptr);
    }

    bool operator < (WeakWSPtr<T> other) const
    {
      return (ptr_ < other.ptr_);
    }

    bool operator > (WeakWSPtr<T> other) const
    {
      return (ptr_ > other.ptr_);
    }

    bool operator != (T *ptr) const
    {
      return (ptr_ != ptr);
    }

    bool operator == (T *ptr) const
    {
      return (ptr_ == ptr);
    }

    template<typename U>
    bool operator != (U *ptr) const
    {
      if (ptr && (!ptr->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return (ptr_ != ptr);
    }

    template<typename U>
    bool operator == (U *ptr) const
    {
      if (ptr && (!ptr->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return (ptr_ == ptr);
    }

    template<typename U>
    bool operator != (const WeakWSPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ != other.ptr_;
    }

    template<typename U>
    bool operator == (const WeakWSPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ == other.ptr_;
    }

    template<typename U>
    bool operator != (const WSPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ != other.ptr_;
    }

    template<typename U>
    bool operator == (const WSPtr<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ == other.ptr_;
    }

    bool IsNull () const
    {
      return get () == 0;
    }

    bool IsValid () const
    {
      return get () != 0;
    }

    void Release()
    {
      ReleaseReference();
    }

  private:
    WeakWSPtr (T *ptr, RefCounts *refCounts) : ptr_ (ptr), refCounts_ (refCounts)
    {
    }

    T *get () const
    {
      if (refCounts_ == 0 || isZero (refCounts_->strongRefs_) )
      {
        return 0;
      }

      return ptr_;
    }

//     bool isNull () const
//     {
//         return get () == 0;
//     }

    void ReleaseReference ()
    {
      if (ptr_ == 0)
      {
        return;
      }

      refCounts_->totalRefs_.Decrement();
      bool release = refCounts_->totalRefs_ == 0;

      if (release)
      {
        delete refCounts_;
      }

      refCounts_ = 0;
      ptr_ = 0;
    }

    T *ptr_;
    RefCounts *refCounts_;

    template <typename O>
    friend class WeakWSPtr;

    template <typename O>
    friend class SmartPtr;

    //     template<typename T, typename U>
    //     friend bool operator == (const WeakWSPtr<T>& a, const WeakWSPtr<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator != (const WeakWSPtr<T>& a, const WeakWSPtr<U>& b);

    //     template<typename T>
    //     friend bool operator == (const WeakWSPtr<T>& a, T*);

    template<typename U>
    friend bool operator == (U *, const WeakWSPtr<U>& a);

    //     template<typename T>
    //     friend bool operator != (const WeakWSPtr<T>& a, T*);

    template<typename U>
    friend bool operator != (U *, const WeakWSPtr<U>& a);

    //     template<typename T, typename U>
    //     friend bool operator == (const WSPtr<T>& a, const WeakWSPtr<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator == (const WeakWSPtr<T>& a, const WSPtr<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator != (const WSPtr<T>& a, const WeakWSPtr<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator != (const WeakWSPtr<T>& a, const WSPtr<U>& b);

    template <typename U, typename F>
    friend WeakWSPtr<U> staticCast (const WeakWSPtr<F>& from);

    template <typename U, typename F>
    friend WeakWSPtr<U> constCast (const WeakWSPtr<F>& from);

    template <typename U, typename F>
    friend WeakWSPtr<U> dynamicCast (const WeakWSPtr<F>& from);

    template <typename U, typename F>
    friend WeakWSPtr<U> checkedCast (const WeakWSPtr<F>& from);

    template <typename U, typename F>
    friend WeakWSPtr<U> queryCast (const WeakWSPtr<F>& from);
  };

///////////////////////////////////////////////////////
// globals

// template<typename T, typename U>
// inline bool operator == (const WSPtr<T>& a, const WSPtr<U>& b)
// {
//     return a.ptr_ == b.ptr_;
// }
//
// template<typename T, typename U>
// inline bool operator == (const WeakWSPtr<T>& a, const WeakWSPtr<U>& b)
// {
//     return a.get () == b.get ();
// }
//
// template<typename T, typename U>
// inline bool operator == (const WSPtr<T>& a, const WeakWSPtr<U>& b)
// {
//     return a.ptr_ == b.get ();
// }
//
// template<typename T, typename U>
// inline bool operator == (const WeakWSPtr<T>& a, const WSPtr<U>& b)
// {
//     return a.get () == b.ptr_;
// }
//
// template<typename T, typename U>
// inline bool operator != (const WSPtr<T>& a, const WSPtr<U>& b)
// {
//     return a.ptr_ != b.ptr_;
// }
//
// template<typename T, typename U>
// inline bool operator != (const WeakWSPtr<T>& a, const WeakWSPtr<U>& b)
// {
//     return a.get () != b.get ();
// }
//
// template<typename T, typename U>
// inline bool operator != (const WSPtr<T>& a, const WeakWSPtr<U>& b)
// {
//     return a.ptr_ != b.get ();
// }
//
// template<typename T, typename U>
// inline bool operator != (const WeakWSPtr<T>& a, const WSPtr<U>& b)
// {
//     return a.get () != b.ptr_;
// }

// template<typename T>
// inline bool operator == (const WSPtr<T>& a, T* ptr)
// {
//     return a.ptr_ == ptr;
// }

  template<typename T>
  inline bool operator == (T *ptr, const WSPtr<T>& a)
  {
    return a.ptr_ == ptr;
  }

// template<typename T>
// inline bool operator != (const WSPtr<T>& a, T* ptr)
// {
//     return a.ptr_ != ptr;
// }

  template<typename T>
  inline bool operator != (T *ptr, const WSPtr<T>& a)
  {
    return a.ptr_ != ptr;
  }

// template<typename T>
// inline bool operator == (const WeakWSPtr<T>& a, T* ptr)
// {
//     return a.ptr_ == ptr;
// }

  template<typename T>
  inline bool operator == (T *ptr, const WeakWSPtr<T>& a)
  {
    return a.ptr_ == ptr;
  }

// template<typename T>
// inline bool operator != (const WeakWSPtr<T>& a, T* ptr)
// {
//     return a.ptr_ != ptr;
// }

  template<typename T>
  inline bool operator != (T *ptr, const WeakWSPtr<T>& a)
  {
    return a.ptr_ != ptr;
  }

///////////////////////////////////////////////////////
// creation functions

  template <typename T>
  WSPtr<T> Create ()
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T;
      return WSPtr<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1>
  WSPtr<T> Create (P1 p1)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1);
      return WSPtr<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1, typename P2>
  WSPtr<T> Create (P1 p1, P2 p2)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1, p2);
      return WSPtr<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1, typename P2, typename P3>
  WSPtr<T> Create (P1 p1, P2 p2, P3 p3)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1, p2, p3);
      return WSPtr<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1, typename P2, typename P3, typename P4>
  WSPtr<T> Create (P1 p1, P2 p2, P3 p3, P4 p4)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1, p2, p3, p4);
      return WSPtr<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5>
  WSPtr<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1, p2, p3, p4, p5);
      return WSPtr<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
  WSPtr<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1, p2, p3, p4, p5, p6);
      return WSPtr<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
  WSPtr<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1, p2, p3, p4, p5, p6, p7);
      return WSPtr<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T>
  WSPtr<T> WrapWSPtr (T *t)
  {
    if (t == 0)
    {
      return WSPtr<T> ();
    }

    try
    {
      RefCounts *rc = new RefCounts;

      return WSPtr<T> (t, rc);
    }
    catch (...)
    {
      delete t;
      throw;
    }
  }

///////////////////////////////////////////////////////
// casts

  template <typename U, typename F>
  WSPtr<U> staticCast (const WSPtr<F>& from)
  {
    if (from.ptr_ == 0)
    {
      return WSPtr<U>();
    }

    U *ptr = static_cast <U *> (from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->strongRefs_.Increment();
      refCounts->totalRefs_.Increment();
    }

    return WSPtr<U> (ptr, refCounts);
  }

  template <typename T, typename F>
  WSPtr<T> constCast (const WSPtr<F>& from)
  {
    if (from.ptr_ == 0)
    {
      return WSPtr<T>();
    }

    T *ptr = const_cast <T *> (from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->strongRefs_.Increment();
      refCounts->totalRefs_.Increment();
    }

    return WSPtr<T> (ptr, refCounts);
  }

  template <typename T, typename F>
  WSPtr<T> dynamicCast (const WSPtr<F>& from)
  {
    if (from.ptr_ == 0)
    {
      return WSPtr<T>();
    }

    T *ptr = &dynamic_cast <T &> (*from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->strongRefs_.Increment();
      refCounts->totalRefs_.Increment();
    }

    return WSPtr<T> (ptr, refCounts);
  }

  template <typename T, typename F>
  WSPtr<T> queryCast (const WSPtr<F>& from)
  {
    T *ptr = dynamic_cast <T *> (from.ptr_);

    if (ptr == 0)
    {
      return WSPtr<T>();
    }

    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->strongRefs_.Increment();
      refCounts->totalRefs_.Increment();
    }

    return WSPtr<T> (ptr, refCounts);
  }

  template <typename T, typename F>
  WSPtr<T> checkedCast (const WSPtr<F>& from)
  {
    if (from.ptr_ == 0)
    {
      return WSPtr<T>();
    }

    nuxAssert (dynamic_cast<T *> (from.ptr_) != 0);

    T *ptr = static_cast <T *> (from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->strongRefs_.Increment();
      refCounts->totalRefs_.Increment();
    }

    return WSPtr<T> (ptr, refCounts);
  }

  template <typename U, typename F>
  WeakWSPtr<U> staticCast (const WeakWSPtr<F>& from)
  {
    if (from.get () == 0)
    {
      return WeakWSPtr<U>();
    }

    U *ptr = static_cast <U *> (from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->totalRefs_.Increment();
    }

    return WeakWSPtr<U> (ptr, refCounts);
  }

  template <typename T, typename F>
  WeakWSPtr<T> constCast (const WeakWSPtr<F>& from)
  {
    if (from.get () == 0)
    {
      return WeakWSPtr<T>();
    }

    T *ptr = const_cast <T *> (from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->totalRefs_.Increment();
    }

    return WeakWSPtr<T> (ptr, refCounts);
  }

  template <typename T, typename F>
  WeakWSPtr<T> dynamicCast (const WeakWSPtr<F>& from)
  {
    if (from.get () == 0)
    {
      return WeakWSPtr<T>();
    }

    T *ptr = &dynamic_cast <T &> (*from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->totalRefs_.Increment();
    }

    return WeakWSPtr<T> (ptr, refCounts);
  }

  template <typename T, typename F>
  WeakWSPtr<T> queryCast (const WeakWSPtr<F>& from)
  {
    T *ptr = dynamic_cast <T *> (from.get () );

    if (ptr == 0)
    {
      return WeakWSPtr<T>();
    }

    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->totalRefs_.Increment();
    }

    return WeakWSPtr<T> (ptr, refCounts);
  }

  template <typename T, typename F>
  WeakWSPtr<T> checkedCast (const WeakWSPtr<F>& from)
  {
    if (from.get () == 0)
    {
      return WeakWSPtr<T>();
    }

    nuxAssert (dynamic_cast<T *> (from.ptr_) != 0);

    T *ptr = static_cast <T *> (from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->totalRefs_.Increment();
    }

    return WeakWSPtr<T> (ptr, refCounts);
  }

///////////////////////////////////////////////////////
// std specializations

  template <typename T>
  inline void swap (WSPtr<T>& t1, WSPtr<T>& t2)
  {
    t1.swap (t2);
  }

  template <typename T>
  inline void swap (WeakWSPtr<T>& t1, WeakWSPtr<T>& t2)
  {
    t1.swap (t2);
  }

}

#endif // WIDGETSMARTPOINTER_H
