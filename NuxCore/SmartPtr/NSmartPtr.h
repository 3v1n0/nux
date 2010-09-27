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


#ifndef NSMARTPTR_H
#define NSMARTPTR_H

namespace nux { //NUX_NAMESPACE_BEGIN

// forward definitions

template <typename T>
class NuxObjectWeakSP;

template <typename T>
class NuxObjectSP;

//! A smart pointer class. Implemented as an intrusive smart pointer.
template <typename T>
class NuxObjectSP
{
public:
    //! Constructor
    NuxObjectSP()
        :   ptr_ (0)
    {
        m_reference_count = 0;
        m_weak_reference_count = 0;
    }

    //! Copy constructor
    NuxObjectSP (const NuxObjectSP<T>& other)
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
    NuxObjectSP (const NuxObjectSP<O>& other)
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
        @param WarnMissuse If True, then NuxObjectSP test is ptr is owned or not. If ptr is not owned 
        and WarnMissuse is True, then Print a warning message. This is a debug feature to detect cases such as
        "NuxObjectSP(ObjectA) myobject(ptr);", because the calling code will no longer have a reference on ptr.
    */
    explicit NuxObjectSP(T* ptr, bool WarnMissuse = false)
        :   ptr_ (0)
    {
        m_reference_count = 0;
        m_weak_reference_count = 0;

        if(ptr != 0)
        {
            if(WarnMissuse && (ptr->OwnsTheReference() == false))
            {
                nuxDebugMsg(TEXT("[NuxObjectSP::NuxObjectSP] Warning: Constructing a smart pointer from an object with a floating reference."));
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
        @param WarnMissuse If True, then NuxObjectSP test is ptr is owned or not. If ptr is not owned 
        and WarnMissuse is True, then Print a warning message. This is a debug feature to detect cases such as
        "NuxObjectSP(ObjectA) myobject(ptr);", because the calling code will no longer have a reference on ptr.
    */
    template <typename O>
    explicit NuxObjectSP(O* ptr, bool WarnMissuse = false)
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
                    nuxDebugMsg(TEXT("[NuxObjectSP::NuxObjectSP] Warning: Constructing a smart pointer from an object with a floating reference."));
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
    NuxObjectSP& operator = (const NuxObjectSP<T>& other)
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
    NuxObjectSP& operator = (const NuxObjectSP<O>& other)
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

    ~NuxObjectSP ()
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
//     void Swap (NuxObjectSP<T>& other)
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

    bool operator < (NuxObjectSP<T> other) const
    {
        return (ptr_ < other.ptr_);
    }

    bool operator > (NuxObjectSP<T> other) const
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
    bool operator != (const NuxObjectSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return true;
        return (void*)ptr_ != (void*)other.ptr_;
    }

    template<typename U>
    bool operator == (const NuxObjectSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return false;
        return (void*)ptr_ == (void*)other.ptr_;
    }

    template<typename U>
    bool operator != (const NuxObjectWeakSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return true;
        return (void*)ptr_ != (void*)other.ptr_;
    }

    template<typename U>
    bool operator == (const NuxObjectWeakSP<U>& other) const
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
    friend NuxObjectSP<U> Create ();

    template <typename U, typename P1>
    friend NuxObjectSP<U> Create (P1 p1);

    template <typename U, typename P1, typename P2>
    friend NuxObjectSP<U> Create (P1 p1, P2 p2);

    template <typename U, typename P1, typename P2, typename P3>
    friend NuxObjectSP<U> Create (P1 p1, P2 p2, P3 p3);

    template <typename U, typename P1, typename P2, typename P3, typename P4>
    friend NuxObjectSP<U> Create (P1 p1, P2 p2, P3 p3, P4 p4);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5>
    friend NuxObjectSP<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    friend NuxObjectSP<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6);

    template <typename U, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
    friend NuxObjectSP<U> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7);

    template <typename U>
    friend NuxObjectSP<U> WrapWSPtr (U* u);

    template <typename O>
    friend class NuxObjectSP;

    template <typename O>
    friend class NuxObjectWeakSP;

    //     template<typename T, typename U>
    //     friend bool operator == (const NuxObjectSP<T>& a, const NuxObjectSP<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator != (const NuxObjectSP<T>& a, const NuxObjectSP<U>& b);

    //     template<typename T>
    //     friend bool operator == (const NuxObjectSP<T>& a, T*);

    template<typename U>
    friend bool operator == (U*, const NuxObjectSP<U>& a);

    //     template<typename T>
    //     friend bool operator != (const NuxObjectSP<T>& a, T*);

    template<typename U>
    friend bool operator != (U*, const NuxObjectSP<U>& a);

    //     template<typename T, typename U>
    //     friend bool operator == (const NuxObjectSP<T>& a, const NuxObjectWeakSP<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator == (const NuxObjectWeakSP<T>& a, const NuxObjectSP<U>& b);
    // 
    //     template<typename T, typename U>
    //     friend bool operator != (const NuxObjectSP<T>& a, const NuxObjectWeakSP<U>& b);
    // 
    //     template<typename T, typename U>
    //     friend bool operator != (const NuxObjectWeakSP<T>& a, const NuxObjectSP<U>& b);

    template <typename U, typename F>
    friend NuxObjectSP<U> staticCast (const NuxObjectSP<F>& from);

    template <typename U, typename F>
    friend NuxObjectSP<U> constCast (const NuxObjectSP<F>& from);

    template <typename U, typename F>
    friend NuxObjectSP<U> dynamicCast (const NuxObjectSP<F>& from);

    template <typename U, typename F>
    friend NuxObjectSP<U> checkedCast (const NuxObjectSP<F>& from);

    template <typename U, typename F>
    friend NuxObjectSP<U> queryCast (const NuxObjectSP<F>& from);
};


//! A weak smart pointer class. Implemented as an intrusive smart pointer.
/*!
    A weak smart pointer is built from a smart pointer or another weak smart pointer. It increments and decrements
    the total reference count of an pointer. Even is the original pointer is destroyed, weak smart pointers still point
    to the RefCounts pointers of the original pointer and can use it to check if the pointer is still valid or not.
*/  
template <typename T>
class NuxObjectWeakSP
{
public:
    //! Constructor
    NuxObjectWeakSP ()
        :   ptr_ (0)
    {
        m_reference_count = 0;
        m_weak_reference_count = 0;
    }

    //! Copy constructor
    /*!
        @param other Parameter with type T.
    */
    NuxObjectWeakSP (const NuxObjectWeakSP<T>& other)
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
    NuxObjectWeakSP (const NuxObjectWeakSP<O>& other)
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
//     NuxObjectWeakSP (const NuxObjectSP<T>& other)
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
    NuxObjectWeakSP (const NuxObjectSP<O>& other)
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
    NuxObjectWeakSP& operator = (const NuxObjectWeakSP<T>& other)
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
    NuxObjectWeakSP& operator = (const NuxObjectWeakSP<O>& other)
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
    NuxObjectWeakSP& operator = (const NuxObjectSP<O>& other)
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

    ~NuxObjectWeakSP ()
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

//     void Swap (NuxObjectWeakSP<T>& other)
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

    bool operator < (NuxObjectWeakSP<T> other) const
    {
        return (ptr_ < other.ptr_);
    }

    bool operator > (NuxObjectWeakSP<T> other) const
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
    bool operator != (const NuxObjectWeakSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return true;
        return ((void*)ptr_ != (void*)other.ptr_);
    }

    template<typename U>
    bool operator == (const NuxObjectWeakSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return false;
        return ((void*)ptr_ == (void*)other.ptr_);
    }

    template<typename U>
    bool operator != (const NuxObjectSP<U>& other) const
    {
        if(other.ptr_ && (!other.ptr_->Type().IsDerivedFromType(T::StaticObjectType)))
            return true;
        return ((void*)ptr_ != (void*)other.ptr_);
    }

    template<typename U>
    bool operator == (const NuxObjectSP<U>& other) const
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
    NuxObjectWeakSP (T* ptr/*, RefCounts* refCounts*/)
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
    friend class NuxObjectWeakSP;

    template <typename O>
    friend class SmartPtr;

    //     template<typename T, typename U>
    //     friend bool operator == (const NuxObjectWeakSP<T>& a, const NuxObjectWeakSP<U>& b);

    //     template<typename T, typename U>
    //     friend bool operator != (const NuxObjectWeakSP<T>& a, const NuxObjectWeakSP<U>& b);

    //     template<typename T>
    //     friend bool operator == (const NuxObjectWeakSP<T>& a, T*);

    template<typename U>
    friend bool operator == (U*, const NuxObjectWeakSP<U>& a);

    //     template<typename T>
    //     friend bool operator != (const NuxObjectWeakSP<T>& a, T*);

    template<typename U>
    friend bool operator != (U*, const NuxObjectWeakSP<U>& a);

    //     template<typename T, typename U>
    //     friend bool operator == (const NuxObjectSP<T>& a, const NuxObjectWeakSP<U>& b);
    // 
    //     template<typename T, typename U>
    //     friend bool operator == (const NuxObjectWeakSP<T>& a, const NuxObjectSP<U>& b);
    // 
    //     template<typename T, typename U>
    //     friend bool operator != (const NuxObjectSP<T>& a, const NuxObjectWeakSP<U>& b);
    // 
    //     template<typename T, typename U>
    //     friend bool operator != (const NuxObjectWeakSP<T>& a, const NuxObjectSP<U>& b);

    template <typename U, typename F>
    friend NuxObjectWeakSP<U> staticCast (const NuxObjectWeakSP<F>& from);

    template <typename U, typename F>
    friend NuxObjectWeakSP<U> constCast (const NuxObjectWeakSP<F>& from);

    template <typename U, typename F>
    friend NuxObjectWeakSP<U> dynamicCast (const NuxObjectWeakSP<F>& from);

    template <typename U, typename F>
    friend NuxObjectWeakSP<U> checkedCast (const NuxObjectWeakSP<F>& from);

    template <typename U, typename F>
    friend NuxObjectWeakSP<U> queryCast (const NuxObjectWeakSP<F>& from);
};

///////////////////////////////////////////////////////
// globals

// template<typename T, typename U>
// inline bool operator == (const NuxObjectSP<T>& a, const NuxObjectSP<U>& b)
// {
//     return a.ptr_ == b.ptr_;
// }
// 
// template<typename T, typename U>
// inline bool operator == (const NuxObjectWeakSP<T>& a, const NuxObjectWeakSP<U>& b)
// {
//     return a.get () == b.get ();
// }
// 
// template<typename T, typename U>
// inline bool operator == (const NuxObjectSP<T>& a, const NuxObjectWeakSP<U>& b)
// {
//     return a.ptr_ == b.get ();
// }
// 
// template<typename T, typename U>
// inline bool operator == (const NuxObjectWeakSP<T>& a, const NuxObjectSP<U>& b)
// {
//     return a.get () == b.ptr_;
// }
// 
// template<typename T, typename U>
// inline bool operator != (const NuxObjectSP<T>& a, const NuxObjectSP<U>& b)
// {
//     return a.ptr_ != b.ptr_;
// }
// 
// template<typename T, typename U>
// inline bool operator != (const NuxObjectWeakSP<T>& a, const NuxObjectWeakSP<U>& b)
// {
//     return a.get () != b.get ();
// }
// 
// template<typename T, typename U>
// inline bool operator != (const NuxObjectSP<T>& a, const NuxObjectWeakSP<U>& b)
// {
//     return a.ptr_ != b.get ();
// }
// 
// template<typename T, typename U>
// inline bool operator != (const NuxObjectWeakSP<T>& a, const NuxObjectSP<U>& b)
// {
//     return a.get () != b.ptr_;
// }

// template<typename T>
// inline bool operator == (const NuxObjectSP<T>& a, T* ptr)
// {
//     return a.ptr_ == ptr;
// }

template<typename T>
inline bool operator == (T* ptr, const NuxObjectSP<T>& a)
{
    return a.ptr_ == ptr;
}

// template<typename T>
// inline bool operator != (const NuxObjectSP<T>& a, T* ptr)
// {
//     return a.ptr_ != ptr;
// }

template<typename T>
inline bool operator != (T* ptr, const NuxObjectSP<T>& a)
{
    return a.ptr_ != ptr;
}

// template<typename T>
// inline bool operator == (const NuxObjectWeakSP<T>& a, T* ptr)
// {
//     return a.ptr_ == ptr;
// }

template<typename T>
inline bool operator == (T* ptr, const NuxObjectWeakSP<T>& a)
{
    return a.ptr_ == ptr;
}

// template<typename T>
// inline bool operator != (const NuxObjectWeakSP<T>& a, T* ptr)
// {
//     return a.ptr_ != ptr;
// }

template<typename T>
inline bool operator != (T* ptr, const NuxObjectWeakSP<T>& a)
{
    return a.ptr_ != ptr;
}

///////////////////////////////////////////////////////
// creation functions

template <typename T>
NuxObjectSP<T> Create ()
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T;
        return NuxObjectSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1>
NuxObjectSP<T> Create (P1 p1)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1);
        return NuxObjectSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1, typename P2>
NuxObjectSP<T> Create (P1 p1, P2 p2)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1, p2);
        return NuxObjectSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1, typename P2, typename P3>
NuxObjectSP<T> Create (P1 p1, P2 p2, P3 p3)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1, p2, p3);
        return NuxObjectSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1, typename P2, typename P3, typename P4>
NuxObjectSP<T> Create (P1 p1, P2 p2, P3 p3, P4 p4)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1, p2, p3, p4);
        return NuxObjectSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5>
NuxObjectSP<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1, p2, p3, p4, p5);
        return NuxObjectSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
NuxObjectSP<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1, p2, p3, p4, p5, p6);
        return NuxObjectSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
NuxObjectSP<T> Create (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
{
    RefCounts* rc = new RefCounts;

    try
    {
        T* t = new T (p1, p2, p3, p4, p5, p6, p7);
        return NuxObjectSP<T> (t, rc);
    }
    catch (...)
    {
        delete rc;
        throw;
    }
}

template <typename T>
NuxObjectSP<T> WrapWSPtr (T* t)
{
    if (t == 0)
    {
        return NuxObjectSP<T> ();
    }

    try
    {
        RefCounts* rc = new RefCounts;

        return NuxObjectSP<T> (t, rc);
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
NuxObjectSP<U> staticCast (const NuxObjectSP<F>& from)
{
    if (from.ptr_ == 0)
    {
        return NuxObjectSP<U>();
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

    return NuxObjectSP<U> (ptr, refCounts);
}

template <typename T, typename F>
NuxObjectSP<T> constCast (const NuxObjectSP<F>& from)
{
    if (from.ptr_ == 0)
    {
        return NuxObjectSP<T>();
    }

    T* ptr = const_cast <T*> (from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0) {
        //refCounts->strongRefs_.Increment();
        //refCounts->totalRefs_.Increment();
        from.m_reference_count->Increment();
        from.m_weak_reference_count->Increment();
    }

    return NuxObjectSP<T> (ptr, refCounts);
}

template <typename T, typename F>
NuxObjectSP<T> dynamicCast (const NuxObjectSP<F>& from)
{
    if (from.ptr_ == 0)
    {
        return NuxObjectSP<T>();
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

    return NuxObjectSP<T> (ptr, refCounts);
}

template <typename T, typename F>
NuxObjectSP<T> queryCast (const NuxObjectSP<F>& from)
{
    T* ptr = dynamic_cast <T*> (from.ptr_);

    if (ptr == 0)
    {
        return NuxObjectSP<T>();
    }

    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->strongRefs_.Increment();
        //refCounts->totalRefs_.Increment();
        from.m_reference_count->Increment();
        from.m_weak_reference_count->Increment();
    }

    return NuxObjectSP<T> (ptr, refCounts);
}

template <typename T, typename F>
NuxObjectSP<T> checkedCast (const NuxObjectSP<F>& from)
{
    if (from.ptr_ == 0)
    {
        return NuxObjectSP<T>();
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

    return NuxObjectSP<T> (ptr, refCounts);
}

template <typename U, typename F>
NuxObjectWeakSP<U> staticCast (const NuxObjectWeakSP<F>& from)
{
    if (from.get () == 0)
    {
        return NuxObjectWeakSP<U>();
    }

    U* ptr = static_cast <U*> (from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        refCounts->totalRefs_.Increment();
    }

    return NuxObjectWeakSP<U> (ptr, refCounts);
}

template <typename T, typename F>
NuxObjectWeakSP<T> constCast (const NuxObjectWeakSP<F>& from)
{
    if (from.get () == 0)
    {
        return NuxObjectWeakSP<T>();
    }

    T* ptr = const_cast <T*> (from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->totalRefs_.Increment();
        from.m_weak_reference_count->Increment();
    }

    return NuxObjectWeakSP<T> (ptr, refCounts);
}

template <typename T, typename F>
NuxObjectWeakSP<T> dynamicCast (const NuxObjectWeakSP<F>& from)
{
    if (from.get () == 0)
    {
        return NuxObjectWeakSP<T>();
    }

    T* ptr = &dynamic_cast <T&> (*from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->totalRefs_.Increment();
        from.m_weak_reference_count->Increment();
    }

    return NuxObjectWeakSP<T> (ptr, refCounts);
}

template <typename T, typename F>
NuxObjectWeakSP<T> queryCast (const NuxObjectWeakSP<F>& from)
{
    T* ptr = dynamic_cast <T*> (from.get ());

    if (ptr == 0)
    {
        return NuxObjectWeakSP<T>();
    }

    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->totalRefs_.Increment();
        from.m_weak_reference_count->Increment();
    }

    return NuxObjectWeakSP<T> (ptr, refCounts);
}

template <typename T, typename F>
NuxObjectWeakSP<T> checkedCast (const NuxObjectWeakSP<F>& from)
{
    if (from.get () == 0)
    {
        return NuxObjectWeakSP<T>();
    }

    nuxAssert(dynamic_cast<T*> (from.ptr_) != 0);

    T* ptr = static_cast <T*> (from.ptr_);
    RefCounts* refCounts = from.refCounts_;

    if (ptr != 0)
    {
        //refCounts->totalRefs_.Increment();
        from.m_weak_reference_count->Increment();
    }

    return NuxObjectWeakSP<T> (ptr, refCounts);
}

// ///////////////////////////////////////////////////////
// // std specializations
// 
// template <typename T> 
// inline void swap (NuxObjectSP<T>& t1, NuxObjectSP<T>& t2)
// {
//     t1.swap (t2);
// }
// 
// template <typename T> 
// inline void swap (NuxObjectWeakSP<T>& t1, NuxObjectWeakSP<T>& t2)
// {
//     t1.swap (t2);
// }


} //NUX_NAMESPACE_END

#endif // NSMARTPTR_H


