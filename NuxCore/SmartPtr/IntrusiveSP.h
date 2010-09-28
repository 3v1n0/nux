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


#ifndef INTRUSIVESP_H
#define INTRUSIVESP_H

namespace nux { //NUX_NAMESPACE_BEGIN

// forward definitions

template <typename T>
class IntrusiveWeakSP;

template <typename T>
class IntrusiveSP;

//! A smart pointer class. Implemented as an intrusive smart pointer.
template <typename T>
class IntrusiveSP
{
public:
    //! Constructor
    IntrusiveSP()
        :   ptr_ (0)
    {
        m_reference_count = 0;
        m_weak_reference_count = 0;
    }

    //! Copy constructor
    IntrusiveSP (const IntrusiveSP<T>& other)
        :   ptr_ (0)
    {
        ptr_ = other.ptr_;
        if (ptr_ != 0)
        {
            m_reference_count = other->m_reference_count;
            m_weak_reference_count = other->m_weak_reference_count;
            ptr_->Reference();
        }
    }

    //! Copy constructor
    /*!
        @param other Parameter with a type derived from T.
    */
    template <typename O>
    IntrusiveSP (const IntrusiveSP<O>& other)
        :   ptr_ (0)
    {
        m_reference_count = 0;
        m_weak_reference_count = 0;

        if(other.ptr_ && other.ptr_->Type().IsDerivedFromType(T::StaticObjectType))
        {
            ptr_ = (T*)other.ptr_;
            m_reference_count = other->m_reference_count;
            m_weak_reference_count = other->m_weak_reference_count;
            ptr_->Reference();
        }
    }

    //! Construction with a base pointer of type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param WarnMissuse If True, then IntrusiveSP test is ptr is owned or not. If ptr is not owned 
        and WarnMissuse is True, then Print a warning message. This is a debug feature to detect cases such as
        "IntrusiveSP(ObjectA) myobject(ptr);", because the calling code will no longer have a reference on ptr.
    */
    explicit IntrusiveSP(T* ptr, bool WarnMissuse = false)
        :   ptr_ (0)
    {
        m_reference_count = 0;
        m_weak_reference_count = 0;

        if(ptr != 0)
        {
            if(WarnMissuse && (ptr->OwnsTheReference() == false))
            {
                nuxDebugMsg(TEXT("[IntrusiveSP::IntrusiveSP] Warning: Constructing a smart pointer from an object with a floating reference."));
            }
            ptr_ = ptr;
            m_reference_count = ptr->m_reference_count;
            m_weak_reference_count = ptr->m_weak_reference_count;
            ptr_->Reference();
        }
    }

    //! Construction with a base pointer of type O that inherits from type T.
    /*!
        @param ptr Start maintaining a reference count of the passed pointer.
        @param WarnMissuse If True, then IntrusiveSP test is ptr is owned or not. If ptr is not owned 
        and WarnMissuse is True, then Print a warning message. This is a debug feature to detect cases such as
        "IntrusiveSP(ObjectA) myobject(ptr);", because the calling code will no longer have a reference on ptr.
    */
    template <typename O>
    explicit IntrusiveSP(O* ptr, bool WarnMissuse = false)
        :   ptr_ (0)
    {
        m_reference_count = 0;
        m_weak_reference_count = 0;

        if(ptr != 0)
        {
            if(ptr->Type().IsDerivedFromType(T::StaticObjectType))
            {
                if(WarnMissuse && (ptr->OwnsTheReference() == false))
                {
                    nuxDebugMsg(TEXT("[IntrusiveSP::IntrusiveSP] Warning: Constructing a smart pointer from an object with a floating reference."));
                }
                ptr_ = (T*)ptr;
                m_reference_count = ptr->m_reference_count;
                m_weak_reference_count = ptr->m_weak_reference_count;
                ptr_->Reference();
            }
        }
    }

    //! Assignment of a smart pointer of type T.
    /*!
        @param other Smart pointer of type T.
    */
    IntrusiveSP& operator = (const IntrusiveSP<T>& other)
    {
        if (ptr_ != other.ptr_)
        {
            ReleaseReference ();

            ptr_ = other.ptr_;
            if (ptr_ != 0)
            {
                m_reference_count = other->m_reference_count;
                m_weak_reference_count = other->m_weak_reference_count;
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
    IntrusiveSP& operator = (const IntrusiveSP<O>& other)
    {
        if(other.ptr_ && other.ptr_->Type().IsDerivedFromType(T::StaticObjectType))
        {
            if (ptr_ != other.ptr_)
            {
                ReleaseReference ();

                ptr_ = other.ptr_;
                if (ptr_ != 0)
                {
                    m_reference_count = other->m_reference_count;
                    m_weak_reference_count = other->m_weak_reference_count;
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

    ~IntrusiveSP ()
    {
        ReleaseReference ();
    }

    T& operator * () const
    {
        nuxAssert(ptr_ != 0);
        return *ptr_;
    }

    T* operator -> () const
    {
        nuxAssert(ptr_ != 0);
        return ptr_;
    }

//     //! Swap the content of 2 smart pointers.
//     /*!
//         @param other Smart pointer to swap with.
//     */
//     void Swap (IntrusiveSP<T>& other)
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
        if(ptr_ == 0)
            return true;
        return false;
    }

    //! Test validity of the smart pointer.
    /*!
        Return True if the internal pointer is not null.
    */
    bool IsValid() const
    {
        if(ptr_ != 0)
            return true;
        return false;
    }

    bool operator < (T* ptr) const
    {
        return (ptr_ < ptr);
    }

    bool operator > (T* ptr) const
    {
        return (ptr_ > ptr);
    }

    bool operator < (IntrusiveSP<T> other) const
    {
        return (ptr_ < other.ptr_);
    }

    bool operator > (IntrusiveSP<T> other) const
    {
        return (ptr_ > other.ptr_);
    }

    bool operator != (T* ptr) const
    {
        return ((void*)ptr_ != (void*)ptr);
    }

    bool operator == (T* ptr) const
    {
        return ((void*)ptr_ == (void*)ptr);
    }

    template<typename U>
    bool operator != (U* ptr) const
    {
        if(ptr && (!ptr->Type().IsDerivedFromType(T::StaticObjectType)))
            return true;
        return ((void*)ptr_ != (void*)ptr);
    }

    template<typename U>
    bool operator == (U* ptr) const
    {
        if(ptr && (!ptr->Type().IsDerivedFromType(T::StaticObjectType)))
            return false;
        return ((void*)ptr_ == (void*)ptr);
    }

    template<typename U>
    bool operator != (const IntrusiveSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return true;
        return (void*)ptr_ != (void*)other.ptr_;
    }

    template<typename U>
    bool operator == (const IntrusiveSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return false;
        return (void*)ptr_ == (void*)other.ptr_;
    }

    template<typename U>
    bool operator != (const IntrusiveWeakSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return true;
        return (void*)ptr_ != (void*)other.ptr_;
    }

    template<typename U>
    bool operator == (const IntrusiveWeakSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return false;
        return (void*)ptr_ == (void*)other.ptr_;
    }

    void Release()
    {
        ReleaseReference();
    }

private:

    void ReleaseReference()
    {
        if(ptr_ == 0)
        {
            return;
        }

        bool DeleteWarning = (m_reference_count->GetValue() == 1) && (m_weak_reference_count->GetValue() == 1);
        // If DeleteWarning == true, then we know that the cal to Unref will delete the pointer ptr. Also, the pointer to 
        // m_reference_count and m_weak_reference_count will be deleted.
        bool Destroyed = ptr_->UnReference();
        // if Destroyed is true and DeleteWarning is false, it means that the pointer was destroyed. But to to hanging Weak pointers,
        // m_weak_reference_count and m_weak_reference_count have not been deleted.

        if(DeleteWarning)
        {
            nuxAssert(Destroyed);
        }

        if(Destroyed && DeleteWarning)
        {
            // Everything has been destroyed and there are no hanging weak pointers. Reset the pointers for this object.
            m_reference_count = 0;
            m_weak_reference_count = 0;
            ptr_ = 0;
        }
        else if(Destroyed && (DeleteWarning == false))
        {
            // Only the ptr_ has been destroyed.There are still hanging weak pointers. Reset the pointers for this object.
            nuxAssert(m_reference_count->GetValue() == 0);
            nuxAssert(m_weak_reference_count->GetValue() >= 1);
            m_reference_count = 0;
            m_weak_reference_count = 0;
            ptr_ = 0;
        }
        else
        {
            // There are still pending references to ptr_. Reset the pointers for this object.
            // Notice how we do the same thing here as in the previous conditions. We end up setting
            // m_reference_count, m_weak_reference_count and ptr_ to 0.
            m_reference_count = 0;
            m_weak_reference_count = 0;
            ptr_ = 0;
        }
    }

    T* ptr_;
    NThreadSafeCounter* m_reference_count;
    NThreadSafeCounter* m_weak_reference_count;

    template <typename U>
    friend IntrusiveSP<U> Create ();

    template <typename U, typename P1>
    friend IntrusiveSP<U> Create (P1 p1);

    template <typename U, typename P1, typename P2>
    friend IntrusiveSP<U> Create (P1 p1, P2 p2);

    template <typename U, typename P1, typename P2, typename P3>
    friend IntrusiveSP<U> Create (P1 p1, P2 p2, P3 p3);

    template <typename U, typename P1, typename P2, typename P3, typename P4>
    friend IntrusiveSP<U> Create (P1 p1, P2 p2, P3 p3, P4 p4);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5>
    friend IntrusiveSP<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    friend IntrusiveSP<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
    friend IntrusiveSP<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);

    template <typename U>
    friend IntrusiveSP<U> WrapWSPtr (U* u);

    template <typename O>
    friend class IntrusiveSP;

    template <typename O>
    friend class IntrusiveWeakSP;

    //     template<typename T, typename U>
    //     friend bool operator == (const IntrusiveSP<T>& a, const IntrusiveSP<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator != (const IntrusiveSP<T>& a, const IntrusiveSP<U>& b);

    //     template<typename T>
    //     friend bool operator == (const IntrusiveSP<T>& a, T*);

    template<typename U>
    friend bool operator == (U*, const IntrusiveSP<U>& a);

    //     template<typename T>
    //     friend bool operator != (const IntrusiveSP<T>& a, T*);

    template<typename U>
    friend bool operator != (U*, const IntrusiveSP<U>& a);

    //     template<typename T, typename U>
    //     friend bool operator == (const IntrusiveSP<T>& a, const IntrusiveWeakSP<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator == (const IntrusiveWeakSP<T>& a, const IntrusiveSP<U>& b);
    // 
    //     template<typename T, typename U>
    //     friend bool operator != (const IntrusiveSP<T>& a, const IntrusiveWeakSP<U>& b);
    // 
    //     template<typename T, typename U>
    //     friend bool operator != (const IntrusiveWeakSP<T>& a, const IntrusiveSP<U>& b);

    template <typename U, typename F>
    friend IntrusiveSP<U> staticCast (const IntrusiveSP<F>& from);

    template <typename U, typename F>
    friend IntrusiveSP<U> constCast (const IntrusiveSP<F>& from);

    template <typename U, typename F>
    friend IntrusiveSP<U> dynamicCast (const IntrusiveSP<F>& from);

    template <typename U, typename F>
    friend IntrusiveSP<U> checkedCast (const IntrusiveSP<F>& from);

    template <typename U, typename F>
    friend IntrusiveSP<U> queryCast (const IntrusiveSP<F>& from);
};


//! A weak smart pointer class. Implemented as an intrusive smart pointer.
/*!
    A weak smart pointer is built from a smart pointer or another weak smart pointer. It increments and decrements
    the total reference count of an pointer. Even is the original pointer is destroyed, weak smart pointers still point
    to the RefCounts pointers of the original pointer and can use it to check if the pointer is still valid or not.
*/  
template <typename T>
class IntrusiveWeakSP
{
public:
    //! Constructor
    IntrusiveWeakSP ()
        :   ptr_ (0)
    {
        m_reference_count = 0;
        m_weak_reference_count = 0;
    }

    //! Copy constructor
    /*!
        @param other Parameter with type T.
    */
    IntrusiveWeakSP (const IntrusiveWeakSP<T>& other)
    {
        ptr_ = other.ptr_;
        m_reference_count = other.m_reference_count;
        m_weak_reference_count = other.m_weak_reference_count;

        if (ptr_ != 0)
        {
            m_weak_reference_count->Increment();
        }
    }

    //! Copy constructor
    /*!
        @param other Parameter with a type derived from T.
    */
    template <typename O>
    IntrusiveWeakSP (const IntrusiveWeakSP<O>& other)
        :   ptr_(0)
    {
        m_reference_count = 0;
        m_weak_reference_count = 0;

        if(other.ptr_ && other.ptr_->Type().IsDerivedFromType(T::StaticObjectType))
        {
            ptr_ = other.ptr_;
            m_reference_count = other.m_reference_count;
            m_weak_reference_count = other.m_weak_reference_count;

            if (ptr_ != 0)
            {
                m_weak_reference_count->Increment();
            }
        }
    }

//     //! Construction from a smart pointer of type T.
//     /*!
//         @param other Maintains a weak smart pointer reference to this parameter.
//     */
//     IntrusiveWeakSP (const IntrusiveSP<T>& other)
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
    IntrusiveWeakSP (const IntrusiveSP<O>& other)
        :   ptr_(0)
    {
        m_reference_count = 0;
        m_weak_reference_count = 0;

        if(other.ptr_ && other.ptr_->Type().IsDerivedFromType(T::StaticObjectType))
        {
            ptr_ = other.ptr_;
            m_reference_count = other.m_reference_count;
            m_weak_reference_count = other.m_weak_reference_count;

            if (ptr_ != 0)
            {
                m_weak_reference_count->Increment();
            }
        }
    }

    //! Assignment of a weak smart pointer of type T.
    /*!
        @param other Weak smart pointer of type T.
    */
    IntrusiveWeakSP& operator = (const IntrusiveWeakSP<T>& other)
    {
        if (get () != other.get ()) // Avoid self assignment.
        {
            ReleaseReference ();

            ptr_ = other.ptr_;
            //refCounts_ = other.refCounts_;
            m_reference_count = other.m_reference_count;
            m_weak_reference_count = other.m_weak_reference_count;

            if (ptr_ != 0)
            {
                //refCounts_->totalRefs_.Increment();
                m_weak_reference_count->Increment();
            }
        }

        return *this;
    }

    //! Assignment of a weak smart pointer of Type O that inherits from type T.
    /*!
        @param other Weak smart pointer of type O.
    */
    template <typename O>
    IntrusiveWeakSP& operator = (const IntrusiveWeakSP<O>& other)
    {
        if(other.ptr_ && other.ptr_->Type().IsDerivedFromType(T::StaticObjectType))
        {
            if (get () != other.get ()) // Avoid self assignment.
            {
                ReleaseReference ();

                ptr_ = other.ptr_;
                //refCounts_ = other.refCounts_;
                m_reference_count = other.m_reference_count;
                m_weak_reference_count = other.m_weak_reference_count;

                if (ptr_ != 0)
                {
                    //refCounts_->totalRefs_.Increment();
                    m_weak_reference_count->Increment();
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
    IntrusiveWeakSP& operator = (const IntrusiveSP<O>& other)
    {
        if(other.ptr_ && other.ptr_->Type().IsDerivedFromType(T::StaticObjectType))
        {
            if(get () != other.ptr_) // Avoid self assignment.
            {
                ReleaseReference ();

                ptr_ = other.ptr_;
                //refCounts_ = other.refCounts_;
                m_reference_count = other.m_reference_count;
                m_weak_reference_count = other.m_weak_reference_count;

                if (ptr_ != 0)
                {
                    //refCounts_->totalRefs_.Increment();
                    m_weak_reference_count->Increment();
                }
            }
        }
        else
        {
            ReleaseReference();
        }
        return *this;
    }

    ~IntrusiveWeakSP ()
    {
        ReleaseReference ();
    }

    T& operator * () const
    {
        nuxAssert(m_reference_count && (m_reference_count->GetValue() != 0) && (ptr_ != 0));

        return *get ();
    }

    T* operator -> () const
    {
        nuxAssert(m_reference_count && (m_reference_count->GetValue() != 0) && (ptr_ != 0));

        return get ();
    }

//     void Swap (IntrusiveWeakSP<T>& other)
//     {
//         std::swap (ptr_, other.ptr_);
//         std::swap (refCounts_, other.refCounts_);
//     }

    bool operator < (T* ptr) const
    {
        return (ptr_ < ptr);
    }

    bool operator > (T* ptr) const
    {
        return (ptr_ > ptr);
    }

    bool operator < (IntrusiveWeakSP<T> other) const
    {
        return (ptr_ < other.ptr_);
    }

    bool operator > (IntrusiveWeakSP<T> other) const
    {
        return (ptr_ > other.ptr_);
    }

    bool operator != (T* ptr) const
    {
        return ((void*)ptr_ != (void*)ptr);
    }

    bool operator == (T* ptr) const
    {
        return ((void*)ptr_ == (void*)ptr);
    }

    template<typename U>
    bool operator != (U* ptr) const
    {
        if(ptr && (!ptr->Type().IsDerivedFromType(T::StaticObjectType)))
            return true;
        return ((void*)ptr_ != (void*)ptr);
    }

    template<typename U>
    bool operator == (U* ptr) const
    {
        if(ptr && (!ptr->Type().IsDerivedFromType(T::StaticObjectType)))
            return false;
        return ((void*)ptr_ == (void*)ptr);
    }

    template<typename U>
    bool operator != (const IntrusiveWeakSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return true;
        return ((void*)ptr_ != (void*)other.ptr_);
    }

    template<typename U>
    bool operator == (const IntrusiveWeakSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return false;
        return ((void*)ptr_ == (void*)other.ptr_);
    }

    template<typename U>
    bool operator != (const IntrusiveSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return true;
        return ((void*)ptr_ != (void*)other.ptr_);
    }

    template<typename U>
    bool operator == (const IntrusiveSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return false;
        return ((void*)ptr_ == (void*)other.ptr_);
    }

    //! Test validity of the smart pointer.
    /*!
        Return True if the internal pointer is not null.
    */
    bool operator () () const
    {
        return get() != 0;
    }

    bool IsNull() const
    {
        return get() == 0;
    }

    bool IsValid() const
    {
        return get() != 0;
    }

    void Release()
    {
        ReleaseReference();
    }

private:
    IntrusiveWeakSP (T* ptr/*, RefCounts* refCounts*/)
        :   ptr_ (ptr)
        //,   refCounts_ (refCounts)
    {
    }

    T* get() const
    {
        if((m_weak_reference_count == 0) || (m_weak_reference_count->GetValue() == 0))
        {
            return 0;
        }

        return ptr_;
    }

    void ReleaseReference ()
    {
        if (ptr_ == 0)
        {
            return;
        }

        nuxAssert(m_weak_reference_count->GetValue() >= 1);

        m_weak_reference_count->Decrement();
        bool DeleteWarning = (m_weak_reference_count->GetValue() == 0);
        if (DeleteWarning)
        {
            // There are no more reference to ptr_
            delete m_reference_count;
            delete m_weak_reference_count;
        }

        m_reference_count = 0;
        m_weak_reference_count = 0;
        ptr_ = 0;
    }

    T* ptr_;
    NThreadSafeCounter* m_reference_count;
    NThreadSafeCounter* m_weak_reference_count;

    template <typename O>
    friend class IntrusiveWeakSP;

    template <typename O>
    friend class SmartPtr;

    //     template<typename T, typename U>
    //     friend bool operator == (const IntrusiveWeakSP<T>& a, const IntrusiveWeakSP<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator != (const IntrusiveWeakSP<T>& a, const IntrusiveWeakSP<U>& b);

    //     template<typename T>
    //     friend bool operator == (const IntrusiveWeakSP<T>& a, T*);

    template<typename U>
    friend bool operator == (U*, const IntrusiveWeakSP<U>& a);

    //     template<typename T>
    //     friend bool operator != (const IntrusiveWeakSP<T>& a, T*);

    template<typename U>
    friend bool operator != (U*, const IntrusiveWeakSP<U>& a);

    //     template<typename T, typename U>
    //     friend bool operator == (const IntrusiveSP<T>& a, const IntrusiveWeakSP<U>& b);
    // 
    //     template<typename T, typename U>
    //     friend bool operator == (const IntrusiveWeakSP<T>& a, const IntrusiveSP<U>& b);
    // 
    //     template<typename T, typename U>
    //     friend bool operator != (const IntrusiveSP<T>& a, const IntrusiveWeakSP<U>& b);
    // 
    //     template<typename T, typename U>
    //     friend bool operator != (const IntrusiveWeakSP<T>& a, const IntrusiveSP<U>& b);

    template <typename U, typename F>
    friend IntrusiveWeakSP<U> staticCast (const IntrusiveWeakSP<F>& from);

    template <typename U, typename F>
    friend IntrusiveWeakSP<U> constCast (const IntrusiveWeakSP<F>& from);

    template <typename U, typename F>
    friend IntrusiveWeakSP<U> dynamicCast (const IntrusiveWeakSP<F>& from);

    template <typename U, typename F>
    friend IntrusiveWeakSP<U> checkedCast (const IntrusiveWeakSP<F>& from);

    template <typename U, typename F>
    friend IntrusiveWeakSP<U> queryCast (const IntrusiveWeakSP<F>& from);
};

///////////////////////////////////////////////////////
// globals

// template<typename T, typename U>
// inline bool operator == (const IntrusiveSP<T>& a, const IntrusiveSP<U>& b)
// {
//     return a.ptr_ == b.ptr_;
// }
// 
// template<typename T, typename U>
// inline bool operator == (const IntrusiveWeakSP<T>& a, const IntrusiveWeakSP<U>& b)
// {
//     return a.get () == b.get ();
// }
// 
// template<typename T, typename U>
// inline bool operator == (const IntrusiveSP<T>& a, const IntrusiveWeakSP<U>& b)
// {
//     return a.ptr_ == b.get ();
// }
// 
// template<typename T, typename U>
// inline bool operator == (const IntrusiveWeakSP<T>& a, const IntrusiveSP<U>& b)
// {
//     return a.get () == b.ptr_;
// }
// 
// template<typename T, typename U>
// inline bool operator != (const IntrusiveSP<T>& a, const IntrusiveSP<U>& b)
// {
//     return a.ptr_ != b.ptr_;
// }
// 
// template<typename T, typename U>
// inline bool operator != (const IntrusiveWeakSP<T>& a, const IntrusiveWeakSP<U>& b)
// {
//     return a.get () != b.get ();
// }
// 
// template<typename T, typename U>
// inline bool operator != (const IntrusiveSP<T>& a, const IntrusiveWeakSP<U>& b)
// {
//     return a.ptr_ != b.get ();
// }
// 
// template<typename T, typename U>
// inline bool operator != (const IntrusiveWeakSP<T>& a, const IntrusiveSP<U>& b)
// {
//     return a.get () != b.ptr_;
// }

// template<typename T>
// inline bool operator == (const IntrusiveSP<T>& a, T* ptr)
// {
//     return a.ptr_ == ptr;
// }

template<typename T>
inline bool operator == (T* ptr, const IntrusiveSP<T>& a)
{
    return a.ptr_ == ptr;
}

// template<typename T>
// inline bool operator != (const IntrusiveSP<T>& a, T* ptr)
// {
//     return a.ptr_ != ptr;
// }

template<typename T>
inline bool operator != (T* ptr, const IntrusiveSP<T>& a)
{
    return a.ptr_ != ptr;
}

// template<typename T>
// inline bool operator == (const IntrusiveWeakSP<T>& a, T* ptr)
// {
//     return a.ptr_ == ptr;
// }

template<typename T>
inline bool operator == (T* ptr, const IntrusiveWeakSP<T>& a)
{
    return a.ptr_ == ptr;
}

// template<typename T>
// inline bool operator != (const IntrusiveWeakSP<T>& a, T* ptr)
// {
//     return a.ptr_ != ptr;
// }

template<typename T>
inline bool operator != (T* ptr, const IntrusiveWeakSP<T>& a)
{
    return a.ptr_ != ptr;
}

///////////////////////////////////////////////////////
// creation functions

template <typename T>
IntrusiveSP<T> Create ()
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T;
        return IntrusiveSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1>
IntrusiveSP<T> Create (P1 p1)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1);
        return IntrusiveSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1, typename P2>
IntrusiveSP<T> Create (P1 p1, P2 p2)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1, p2);
        return IntrusiveSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1, typename P2, typename P3>
IntrusiveSP<T> Create (P1 p1, P2 p2, P3 p3)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1, p2, p3);
        return IntrusiveSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1, typename P2, typename P3, typename P4>
IntrusiveSP<T> Create (P1 p1, P2 p2, P3 p3, P4 p4)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1, p2, p3, p4);
        return IntrusiveSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5>
IntrusiveSP<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1, p2, p3, p4, p5);
        return IntrusiveSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
IntrusiveSP<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1, p2, p3, p4, p5, p6);
        return IntrusiveSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
IntrusiveSP<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1, p2, p3, p4, p5, p6, p7);
        return IntrusiveSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T>
IntrusiveSP<T> WrapWSPtr (T* t)
{
    if (t == 0)
    {
        return IntrusiveSP<T> ();
    }

    try
    {
        RefCounts* rc = new RefCounts;

        return IntrusiveSP<T> (t, rc);
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
IntrusiveSP<U> staticCast (const IntrusiveSP<F>& from)
{
    if (from.ptr_ == 0)
    {
        return IntrusiveSP<U>();
    }

    U* ptr = static_cast <U*> (from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->strongRefs_.Increment();
        //refCounts->totalRefs_.Increment();
        from.m_reference_count->Increment();
        from.m_weak_reference_count->Increment();
    }

    return IntrusiveSP<U> (ptr, refCounts);
}

template <typename T, typename F>
IntrusiveSP<T> constCast (const IntrusiveSP<F>& from)
{
    if (from.ptr_ == 0)
    {
        return IntrusiveSP<T>();
    }

    T* ptr = const_cast <T*> (from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0) {
        //refCounts->strongRefs_.Increment();
        //refCounts->totalRefs_.Increment();
        from.m_reference_count->Increment();
        from.m_weak_reference_count->Increment();
    }

    return IntrusiveSP<T> (ptr, refCounts);
}

template <typename T, typename F>
IntrusiveSP<T> dynamicCast (const IntrusiveSP<F>& from)
{
    if (from.ptr_ == 0)
    {
        return IntrusiveSP<T>();
    }

    T* ptr = &dynamic_cast <T&> (*from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->strongRefs_.Increment();
        //refCounts->totalRefs_.Increment();
        from.m_reference_count->Increment();
        from.m_weak_reference_count->Increment();
    }

    return IntrusiveSP<T> (ptr, refCounts);
}

template <typename T, typename F>
IntrusiveSP<T> queryCast (const IntrusiveSP<F>& from)
{
    T* ptr = dynamic_cast <T*> (from.ptr_);

    if (ptr == 0)
    {
        return IntrusiveSP<T>();
    }

    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->strongRefs_.Increment();
        //refCounts->totalRefs_.Increment();
        from.m_reference_count->Increment();
        from.m_weak_reference_count->Increment();
    }

    return IntrusiveSP<T> (ptr, refCounts);
}

template <typename T, typename F>
IntrusiveSP<T> checkedCast (const IntrusiveSP<F>& from)
{
    if (from.ptr_ == 0)
    {
        return IntrusiveSP<T>();
    }

    nuxAssert(dynamic_cast<T*> (from.ptr_) != 0);

    T* ptr = static_cast <T*> (from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->strongRefs_.Increment();
        //refCounts->totalRefs_.Increment();
        from.m_reference_count->Increment();
        from.m_weak_reference_count->Increment();
    }

    return IntrusiveSP<T> (ptr, refCounts);
}

template <typename U, typename F>
IntrusiveWeakSP<U> staticCast (const IntrusiveWeakSP<F>& from)
{
    if (from.get () == 0)
    {
        return IntrusiveWeakSP<U>();
    }

    U* ptr = static_cast <U*> (from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        refCounts->totalRefs_.Increment();
    }

    return IntrusiveWeakSP<U> (ptr, refCounts);
}

template <typename T, typename F>
IntrusiveWeakSP<T> constCast (const IntrusiveWeakSP<F>& from)
{
    if (from.get () == 0)
    {
        return IntrusiveWeakSP<T>();
    }

    T* ptr = const_cast <T*> (from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->totalRefs_.Increment();
        from.m_weak_reference_count->Increment();
    }

    return IntrusiveWeakSP<T> (ptr, refCounts);
}

template <typename T, typename F>
IntrusiveWeakSP<T> dynamicCast (const IntrusiveWeakSP<F>& from)
{
    if (from.get () == 0)
    {
        return IntrusiveWeakSP<T>();
    }

    T* ptr = &dynamic_cast <T&> (*from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->totalRefs_.Increment();
        from.m_weak_reference_count->Increment();
    }

    return IntrusiveWeakSP<T> (ptr, refCounts);
}

template <typename T, typename F>
IntrusiveWeakSP<T> queryCast (const IntrusiveWeakSP<F>& from)
{
    T* ptr = dynamic_cast <T*> (from.get ());

    if (ptr == 0)
    {
        return IntrusiveWeakSP<T>();
    }

    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->totalRefs_.Increment();
        from.m_weak_reference_count->Increment();
    }

    return IntrusiveWeakSP<T> (ptr, refCounts);
}

template <typename T, typename F>
IntrusiveWeakSP<T> checkedCast (const IntrusiveWeakSP<F>& from)
{
    if (from.get () == 0)
    {
        return IntrusiveWeakSP<T>();
    }

    nuxAssert(dynamic_cast<T*> (from.ptr_) != 0);

    T* ptr = static_cast <T*> (from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->totalRefs_.Increment();
        from.m_weak_reference_count->Increment();
    }

    return IntrusiveWeakSP<T> (ptr, refCounts);
}

// ///////////////////////////////////////////////////////
// // std specializations
// 
// template <typename T> 
// inline void swap (IntrusiveSP<T>& t1, IntrusiveSP<T>& t2)
// {
//     t1.swap (t2);
// }
// 
// template <typename T> 
// inline void swap (IntrusiveWeakSP<T>& t1, IntrusiveWeakSP<T>& t2)
// {
//     t1.swap (t2);
// }


} //NUX_NAMESPACE_END

#endif // INTRUSIVESP_H


