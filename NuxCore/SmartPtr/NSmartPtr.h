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

NAMESPACE_BEGIN

template < class T >
class NSmartPtr
{
public:
    NSmartPtr();
    NSmartPtr(T* t);
    NSmartPtr(const NSmartPtr& rp);
    ~NSmartPtr();
    NSmartPtr& operator = (const NSmartPtr<T>& rp);
    NSmartPtr& operator = (const T* rp);

    bool operator == (const NSmartPtr<T>& rp) const;
    bool operator == (const T* ptr) const;
    bool operator != (const NSmartPtr<T>& rp) const;
    bool operator != (const T* ptr) const;
    bool operator < (const NSmartPtr<T>& rp) const;
    bool operator < (const T* ptr) const;
    bool operator > (const NSmartPtr<T>& rp) const;
    bool operator > (const T* ptr) const;

    T& operator* ();
    const T& operator* () const;
    T* operator-> ();
    const T* operator-> () const;

    bool operator! () const;
    bool IsValid () const;
    T* GetPtr();
    const T* GetPtr() const; 

private:
    void CheckReferenceCount();
    T* m_ptr;
};

template<class T> 
inline NSmartPtr<T>::NSmartPtr() 
:   m_ptr(NULL) 
{
}

template<class T> inline 
NSmartPtr<T>::NSmartPtr(T* t) 
:   m_ptr(t)              
{ 
    if(m_ptr) 
    {
        m_ptr->Increment(); 
    }
}

template<class T> 
inline NSmartPtr<T>::NSmartPtr(const NSmartPtr& rp) 
:   m_ptr(rp.m_ptr)  
{ 
    if(m_ptr) 
    {
        m_ptr->Increment();
    }
}

template<class T> 
inline NSmartPtr<T>::~NSmartPtr()                           
{ 
    if(m_ptr) 
    {
        m_ptr->Decrement();
        CheckReferenceCount();
    }
}

template<class T> 
inline void NSmartPtr<T>::CheckReferenceCount()
{
    if(m_ptr->GetRefCount() == 0)
    {
        delete m_ptr;
        m_ptr = 0;
    }
}

template<class T> 
inline NSmartPtr<T>& NSmartPtr<T>::operator = (const NSmartPtr& rp)
{
    // if the value to assign to the pointer is already set, just return this element
    if(rp.m_ptr == m_ptr) 
    {
        return *this;
    }

    // decrement ref count of RefCounted previously pointed to by this pointer
    if(m_ptr) 
    {
        m_ptr->Decrement();
        CheckReferenceCount();
    }

    // copy new pointer value
    m_ptr = rp.m_ptr;

    // increment ref count of RefCounted now pointed to by this pointer
    if(m_ptr) 
    {
        m_ptr->Increment();
    }

    return *this;
}

template<class T> 
inline bool NSmartPtr<T>::operator == (const NSmartPtr& rp) const
{
    return (m_ptr == rp.m_ptr);
}

template<class T> 
inline bool NSmartPtr<T>::operator == (const T* ptr) const
{
    return (m_ptr == ptr);
}

template<class T> 
inline bool NSmartPtr<T>::operator != (const NSmartPtr& rp) const
{
    return (m_ptr != rp.m_ptr);
}

template<class T> 
inline bool NSmartPtr<T>::operator != (const T* ptr) const
{
    return (m_ptr != ptr);
}

template<class T> 
inline bool NSmartPtr<T>::operator < (const NSmartPtr& rp) const
{
    return (m_ptr < rp.m_ptr);
}

template<class T> 
inline bool NSmartPtr<T>::operator < (const T* ptr) const
{
    return (m_ptr < ptr);
}

template<class T> 
inline bool NSmartPtr<T>::operator > (const NSmartPtr& rp) const
{
    return (m_ptr > rp.m_ptr);
}

template<class T> 
inline bool NSmartPtr<T>::operator > (const T* ptr) const
{
    return (m_ptr > ptr);
}

template<class T> 
inline T& NSmartPtr<T>::operator * () 
{ 
    return *m_ptr; 
}

template<class T> 
inline const T& NSmartPtr<T>::operator * () const 
{ 
    return *m_ptr; 
}

template<class T>
inline T* NSmartPtr<T>::operator -> () 
{ 
    return m_ptr; 
}

template<class T> 
inline const T* NSmartPtr<T>::operator -> () const   
{ 
    return m_ptr; 
}

template<class T> 
inline bool NSmartPtr<T>::operator ! () const	
{ 
    return (m_ptr == NULL); 
}

template<class T> 
inline bool NSmartPtr<T>::IsValid() const	
{ 
    return (m_ptr != NULL); 
}

template<class T> 
inline T* NSmartPtr<T>::GetPtr() 
{ 
    return m_ptr; 
}

template<class T> 
inline const T* NSmartPtr<T>::GetPtr() const 
{ 
    return m_ptr; 
}

NAMESPACE_END

#endif // NSMARTPTR_H


