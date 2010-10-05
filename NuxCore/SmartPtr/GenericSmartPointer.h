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


#ifndef WIDGETSMARTPOINTER_H
#define WIDGETSMARTPOINTER_H

namespace nux   //NUX_NAMESPACE_BEGIN
{

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
  class GenericWeakSP;

  template <typename T>
  class GenericSP;

//! A smart pointer class. Implemented as an intrusive smart pointer.
  template <typename T>
  class GenericSP
  {
  public:
    //! Constructor
    GenericSP()
      :   ptr_ (0)
      , refCounts_ (0)
      , deletewhenrefcounthitzero_ (true)
    {
    }

    //! Copy constructor
    GenericSP (const GenericSP<T>& other)
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
    GenericSP (const GenericSP<O>& other)
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
    explicit GenericSP (T *ptr, bool deletewhenrefcounthitzero = true)
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
    explicit GenericSP (O *ptr, bool deletewhenrefcounthitzero = true)
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
    GenericSP &operator = (const GenericSP<T>& other)
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
    GenericSP &operator = (const GenericSP<O>& other)
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

    ~GenericSP ()
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
    void Swap (GenericSP<T>& other)
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

    bool operator < (GenericSP<T> other) const
    {
      return (ptr_ < other.ptr_);
    }

    bool operator > (GenericSP<T> other) const
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
    bool operator != (const GenericSP<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ != other.ptr_;
    }

    template<typename U>
    bool operator == (const GenericSP<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ == other.ptr_;
    }

    template<typename U>
    bool operator != (const GenericWeakSP<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ != other.ptr_;
    }

    template<typename U>
    bool operator == (const GenericWeakSP<U>& other) const
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
    GenericSP (T *ptr, RefCounts *refCounts) : ptr_ (ptr), refCounts_ (refCounts)
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
    friend GenericSP<U> Create ();

    template <typename U, typename P1>
    friend GenericSP<U> Create (P1 p1);

    template <typename U, typename P1, typename P2>
    friend GenericSP<U> Create (P1 p1, P2 p2);

    template <typename U, typename P1, typename P2, typename P3>
    friend GenericSP<U> Create (P1 p1, P2 p2, P3 p3);

    template <typename U, typename P1, typename P2, typename P3, typename P4>
    friend GenericSP<U> Create (P1 p1, P2 p2, P3 p3, P4 p4);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5>
    friend GenericSP<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    friend GenericSP<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
    friend GenericSP<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);

    template <typename U>
    friend GenericSP<U> WrapWSPtr (U *u);

    template <typename O>
    friend class GenericSP;

    template <typename O>
    friend class GenericWeakSP;

    //     template<typename T, typename U>
    //     friend bool operator == (const GenericSP<T>& a, const GenericSP<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator != (const GenericSP<T>& a, const GenericSP<U>& b);

    //     template<typename T>
    //     friend bool operator == (const GenericSP<T>& a, T*);

    template<typename U>
    friend bool operator == (U *, const GenericSP<U>& a);

    //     template<typename T>
    //     friend bool operator != (const GenericSP<T>& a, T*);

    template<typename U>
    friend bool operator != (U *, const GenericSP<U>& a);

    //     template<typename T, typename U>
    //     friend bool operator == (const GenericSP<T>& a, const GenericWeakSP<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator == (const GenericWeakSP<T>& a, const GenericSP<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator != (const GenericSP<T>& a, const GenericWeakSP<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator != (const GenericWeakSP<T>& a, const GenericSP<U>& b);

    template <typename U, typename F>
    friend GenericSP<U> staticCast (const GenericSP<F>& from);

    template <typename U, typename F>
    friend GenericSP<U> constCast (const GenericSP<F>& from);

    template <typename U, typename F>
    friend GenericSP<U> dynamicCast (const GenericSP<F>& from);

    template <typename U, typename F>
    friend GenericSP<U> checkedCast (const GenericSP<F>& from);

    template <typename U, typename F>
    friend GenericSP<U> queryCast (const GenericSP<F>& from);
  };


//! A weak smart pointer class. Implemented as an intrusive smart pointer.
  /*!
      A weak smart pointer is built from a smart pointer or another weak smart pointer. It increments and decrements
      the total reference count of an pointer. Even is the original pointer is destroyed, weak smart pointers still point
      to the RefCounts pointers of the original pointer and can use it to check if the pointer is still valid or not.
  */
  template <typename T>
  class GenericWeakSP
  {
  public:
    //! Constructor
    GenericWeakSP () : ptr_ (0), refCounts_ (0)
    {
    }

    //! Copy constructor
    /*!
        @param other Parameter with type T.
    */
    GenericWeakSP (const GenericWeakSP<T>& other)
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
    GenericWeakSP (const GenericWeakSP<O>& other)
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
//     GenericWeakSP (const GenericSP<T>& other)
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
    GenericWeakSP (const GenericSP<O>& other)
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
    GenericWeakSP &operator = (const GenericWeakSP<T>& other)
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
    GenericWeakSP &operator = (const GenericWeakSP<O>& other)
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
    GenericWeakSP &operator = (const GenericSP<O>& other)
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

    ~GenericWeakSP ()
    {
      ReleaseReference ();
    }

    T &operator * () const
    {
      nuxAssert ( (refCounts_->strongRefs_.GetValue() != 0) && (ptr_ != 0) );

      return *get ();
    }

    T *operator -> () const
    {
      nuxAssert ( (refCounts_->strongRefs_.GetValue() != 0) && (ptr_ != 0) );

      return get ();
    }

    void Swap (GenericWeakSP<T>& other)
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

    bool operator < (GenericWeakSP<T> other) const
    {
      return (ptr_ < other.ptr_);
    }

    bool operator > (GenericWeakSP<T> other) const
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
    bool operator != (const GenericWeakSP<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ != other.ptr_;
    }

    template<typename U>
    bool operator == (const GenericWeakSP<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ == other.ptr_;
    }

    template<typename U>
    bool operator != (const GenericSP<U>& other) const
    {
      if (other.ptr_ && (!other.ptr_->Type().IsDerivedFromType (T::StaticObjectType) ) )
        return false;

      return ptr_ != other.ptr_;
    }

    template<typename U>
    bool operator == (const GenericSP<U>& other) const
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
    GenericWeakSP (T *ptr, RefCounts *refCounts) : ptr_ (ptr), refCounts_ (refCounts)
    {
    }

    T *get () const
    {
      if (refCounts_ == 0 || (refCounts_->strongRefs_ == 0) )
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
    friend class GenericWeakSP;

    template <typename O>
    friend class SmartPtr;

    //     template<typename T, typename U>
    //     friend bool operator == (const GenericWeakSP<T>& a, const GenericWeakSP<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator != (const GenericWeakSP<T>& a, const GenericWeakSP<U>& b);

    //     template<typename T>
    //     friend bool operator == (const GenericWeakSP<T>& a, T*);

    template<typename U>
    friend bool operator == (U *, const GenericWeakSP<U>& a);

    //     template<typename T>
    //     friend bool operator != (const GenericWeakSP<T>& a, T*);

    template<typename U>
    friend bool operator != (U *, const GenericWeakSP<U>& a);

    //     template<typename T, typename U>
    //     friend bool operator == (const GenericSP<T>& a, const GenericWeakSP<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator == (const GenericWeakSP<T>& a, const GenericSP<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator != (const GenericSP<T>& a, const GenericWeakSP<U>& b);
    //
    //     template<typename T, typename U>
    //     friend bool operator != (const GenericWeakSP<T>& a, const GenericSP<U>& b);

    template <typename U, typename F>
    friend GenericWeakSP<U> staticCast (const GenericWeakSP<F>& from);

    template <typename U, typename F>
    friend GenericWeakSP<U> constCast (const GenericWeakSP<F>& from);

    template <typename U, typename F>
    friend GenericWeakSP<U> dynamicCast (const GenericWeakSP<F>& from);

    template <typename U, typename F>
    friend GenericWeakSP<U> checkedCast (const GenericWeakSP<F>& from);

    template <typename U, typename F>
    friend GenericWeakSP<U> queryCast (const GenericWeakSP<F>& from);
  };

///////////////////////////////////////////////////////
// globals

// template<typename T, typename U>
// inline bool operator == (const GenericSP<T>& a, const GenericSP<U>& b)
// {
//     return a.ptr_ == b.ptr_;
// }
//
// template<typename T, typename U>
// inline bool operator == (const GenericWeakSP<T>& a, const GenericWeakSP<U>& b)
// {
//     return a.get () == b.get ();
// }
//
// template<typename T, typename U>
// inline bool operator == (const GenericSP<T>& a, const GenericWeakSP<U>& b)
// {
//     return a.ptr_ == b.get ();
// }
//
// template<typename T, typename U>
// inline bool operator == (const GenericWeakSP<T>& a, const GenericSP<U>& b)
// {
//     return a.get () == b.ptr_;
// }
//
// template<typename T, typename U>
// inline bool operator != (const GenericSP<T>& a, const GenericSP<U>& b)
// {
//     return a.ptr_ != b.ptr_;
// }
//
// template<typename T, typename U>
// inline bool operator != (const GenericWeakSP<T>& a, const GenericWeakSP<U>& b)
// {
//     return a.get () != b.get ();
// }
//
// template<typename T, typename U>
// inline bool operator != (const GenericSP<T>& a, const GenericWeakSP<U>& b)
// {
//     return a.ptr_ != b.get ();
// }
//
// template<typename T, typename U>
// inline bool operator != (const GenericWeakSP<T>& a, const GenericSP<U>& b)
// {
//     return a.get () != b.ptr_;
// }

// template<typename T>
// inline bool operator == (const GenericSP<T>& a, T* ptr)
// {
//     return a.ptr_ == ptr;
// }

  template<typename T>
  inline bool operator == (T *ptr, const GenericSP<T>& a)
  {
    return a.ptr_ == ptr;
  }

// template<typename T>
// inline bool operator != (const GenericSP<T>& a, T* ptr)
// {
//     return a.ptr_ != ptr;
// }

  template<typename T>
  inline bool operator != (T *ptr, const GenericSP<T>& a)
  {
    return a.ptr_ != ptr;
  }

// template<typename T>
// inline bool operator == (const GenericWeakSP<T>& a, T* ptr)
// {
//     return a.ptr_ == ptr;
// }

  template<typename T>
  inline bool operator == (T *ptr, const GenericWeakSP<T>& a)
  {
    return a.ptr_ == ptr;
  }

// template<typename T>
// inline bool operator != (const GenericWeakSP<T>& a, T* ptr)
// {
//     return a.ptr_ != ptr;
// }

  template<typename T>
  inline bool operator != (T *ptr, const GenericWeakSP<T>& a)
  {
    return a.ptr_ != ptr;
  }

///////////////////////////////////////////////////////
// creation functions

  template <typename T>
  GenericSP<T> Create ()
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T;
      return GenericSP<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1>
  GenericSP<T> Create (P1 p1)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1);
      return GenericSP<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1, typename P2>
  GenericSP<T> Create (P1 p1, P2 p2)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1, p2);
      return GenericSP<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1, typename P2, typename P3>
  GenericSP<T> Create (P1 p1, P2 p2, P3 p3)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1, p2, p3);
      return GenericSP<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1, typename P2, typename P3, typename P4>
  GenericSP<T> Create (P1 p1, P2 p2, P3 p3, P4 p4)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1, p2, p3, p4);
      return GenericSP<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5>
  GenericSP<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1, p2, p3, p4, p5);
      return GenericSP<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
  GenericSP<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1, p2, p3, p4, p5, p6);
      return GenericSP<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
  GenericSP<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
  {
    RefCounts *rc = new RefCounts;

    try
    {
      T *t = new T (p1, p2, p3, p4, p5, p6, p7);
      return GenericSP<T> (t, rc);
    }
    catch (...)
    {
      delete rc;
      throw;
    }
  }

  template <typename T>
  GenericSP<T> WrapWSPtr (T *t)
  {
    if (t == 0)
    {
      return GenericSP<T> ();
    }

    try
    {
      RefCounts *rc = new RefCounts;

      return GenericSP<T> (t, rc);
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
  GenericSP<U> staticCast (const GenericSP<F>& from)
  {
    if (from.ptr_ == 0)
    {
      return GenericSP<U>();
    }

    U *ptr = static_cast <U *> (from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->strongRefs_.Increment();
      refCounts->totalRefs_.Increment();
    }

    return GenericSP<U> (ptr, refCounts);
  }

  template <typename T, typename F>
  GenericSP<T> constCast (const GenericSP<F>& from)
  {
    if (from.ptr_ == 0)
    {
      return GenericSP<T>();
    }

    T *ptr = const_cast <T *> (from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->strongRefs_.Increment();
      refCounts->totalRefs_.Increment();
    }

    return GenericSP<T> (ptr, refCounts);
  }

  template <typename T, typename F>
  GenericSP<T> dynamicCast (const GenericSP<F>& from)
  {
    if (from.ptr_ == 0)
    {
      return GenericSP<T>();
    }

    T *ptr = &dynamic_cast <T &> (*from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->strongRefs_.Increment();
      refCounts->totalRefs_.Increment();
    }

    return GenericSP<T> (ptr, refCounts);
  }

  template <typename T, typename F>
  GenericSP<T> queryCast (const GenericSP<F>& from)
  {
    T *ptr = dynamic_cast <T *> (from.ptr_);

    if (ptr == 0)
    {
      return GenericSP<T>();
    }

    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->strongRefs_.Increment();
      refCounts->totalRefs_.Increment();
    }

    return GenericSP<T> (ptr, refCounts);
  }

  template <typename T, typename F>
  GenericSP<T> checkedCast (const GenericSP<F>& from)
  {
    if (from.ptr_ == 0)
    {
      return GenericSP<T>();
    }

    nuxAssert (dynamic_cast<T *> (from.ptr_) != 0);

    T *ptr = static_cast <T *> (from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->strongRefs_.Increment();
      refCounts->totalRefs_.Increment();
    }

    return GenericSP<T> (ptr, refCounts);
  }

  template <typename U, typename F>
  GenericWeakSP<U> staticCast (const GenericWeakSP<F>& from)
  {
    if (from.get () == 0)
    {
      return GenericWeakSP<U>();
    }

    U *ptr = static_cast <U *> (from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->totalRefs_.Increment();
    }

    return GenericWeakSP<U> (ptr, refCounts);
  }

  template <typename T, typename F>
  GenericWeakSP<T> constCast (const GenericWeakSP<F>& from)
  {
    if (from.get () == 0)
    {
      return GenericWeakSP<T>();
    }

    T *ptr = const_cast <T *> (from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->totalRefs_.Increment();
    }

    return GenericWeakSP<T> (ptr, refCounts);
  }

  template <typename T, typename F>
  GenericWeakSP<T> dynamicCast (const GenericWeakSP<F>& from)
  {
    if (from.get () == 0)
    {
      return GenericWeakSP<T>();
    }

    T *ptr = &dynamic_cast <T &> (*from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->totalRefs_.Increment();
    }

    return GenericWeakSP<T> (ptr, refCounts);
  }

  template <typename T, typename F>
  GenericWeakSP<T> queryCast (const GenericWeakSP<F>& from)
  {
    T *ptr = dynamic_cast <T *> (from.get () );

    if (ptr == 0)
    {
      return GenericWeakSP<T>();
    }

    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->totalRefs_.Increment();
    }

    return GenericWeakSP<T> (ptr, refCounts);
  }

  template <typename T, typename F>
  GenericWeakSP<T> checkedCast (const GenericWeakSP<F>& from)
  {
    if (from.get () == 0)
    {
      return GenericWeakSP<T>();
    }

    nuxAssert (dynamic_cast<T *> (from.ptr_) != 0);

    T *ptr = static_cast <T *> (from.ptr_);
    RefCounts *refCounts = from.refCounts_;

    if (ptr != 0)
    {
      refCounts->totalRefs_.Increment();
    }

    return GenericWeakSP<T> (ptr, refCounts);
  }

///////////////////////////////////////////////////////
// std specializations

  template <typename T>
  inline void swap (GenericSP<T>& t1, GenericSP<T>& t2)
  {
    t1.swap (t2);
  }

  template <typename T>
  inline void swap (GenericWeakSP<T>& t1, GenericWeakSP<T>& t2)
  {
    t1.swap (t2);
  }

} //NUX_NAMESPACE_END

#endif // WIDGETSMARTPOINTER_H
