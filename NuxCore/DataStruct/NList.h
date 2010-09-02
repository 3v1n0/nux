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


#ifndef NLIST_H
#define NLIST_H

NAMESPACE_BEGIN

// This list requires that element T has a and object of type NListNoDyn::Node
// This list will not do any dynamic allocation for the Node.


template<typename T>
class NListNoDyn
{
public:
    class Node
    {
    public:
        Node()
        {
            Clear();
        }
        
        ~Node()
        {
            nuxAssert(!IsLinked());
        }

        void Clear()
        {
            m_Next = m_Prev = 0;
        }

        bool IsLinked() const
        {
            return (m_Prev || m_Next);
        }

        T* m_Next;
        T* m_Prev;
    };

    NListNoDyn()
        :   m_Head(0)
        ,   m_Tail(0)
    {
    }

    T* m_Head;
    T* m_Tail;

    inline void PushFront(T &NewNode)
    {
        nuxAssert(!NewNode.m_Node.IsLinked());
        if(Find(NewNode))
            return;

        NewNode.m_Node.m_Prev = 0;
        NewNode.m_Node.m_Next = m_Head;
        if(m_Head)
        {
            m_Head->m_Node.m_Prev = &NewNode;
            m_Head = &NewNode;
        }
        else
        {
            m_Head = &NewNode;
            m_Tail = &NewNode;
        }
    }

    inline void PushBack(T &NewNode)
    {
        nuxAssert(!NewNode.m_Node.IsLinked());
        if(Find(NewNode))
            return;

        NewNode.m_Node.m_Prev = m_Tail;
        NewNode.m_Node.m_Next = 0;
        if(m_Tail)
        {
            m_Tail->m_Node.m_Next = &NewNode;
            m_Tail = &NewNode;
        }
        else
        {
            m_Head = &NewNode;
            m_Tail = &NewNode;
        }
    }

    inline T* PopFront()
    {
        T* poped = m_Head;
        if(m_Head)
        {
            m_Head = m_Head->m_Node.m_Next;
            if(m_Head)
                m_Head->m_Node.m_Prev = 0;
            else
                m_Tail = 0;
            poped->m_Node.Clear();
        }
        return poped;
    }

    inline T* PopBack()
    {
        T* poped = m_Tail;
        if(m_Tail)
        {
            m_Tail = m_Tail->m_Node.m_Prev;
            if(m_Tail)
                m_Tail->m_Node.m_Next = 0;
            else
                m_Head = 0;
            poped->m_Node.Clear();
        }
        return poped;
    }

    inline bool Find(const T& ToFind)
    {
        T* pIndex = m_Head;
        while(pIndex && (pIndex != &ToFind)) 
            pIndex = pIndex->m_Node.m_Next;
        return(pIndex != 0);
    }

    inline void Remove(T &OldNode)
    {
        if(!Find(OldNode))
            return;

        // If OldNode have a previous node, her next pointer must point to OldNode next pointer.
        // Else, OldNode is the Head, and her next pointer must be the new Head.	
        if(OldNode.m_Node.m_Prev) 
            OldNode.m_Node.m_Prev->m_Node.m_Next = OldNode.m_Node.m_Next;
        else 
        {
            m_Head = OldNode.m_Node.m_Next;
            if(m_Head)
                m_Head->m_Node.m_Prev = 0;
            else
                m_Tail = 0;
        }

        // If, OldNode have a next node, her previous pointer must point to OldNode previous pointer.
        // Else, OldNode is the Tail, and her previous pointer must be the new Tail.
        if(OldNode.m_Node.m_Next) 
            OldNode.m_Node.m_Next->m_Node.m_Prev = OldNode.m_Node.m_Prev;
        else 
        {
            m_Tail = OldNode.m_Node.m_Prev;
            if(m_Tail)
                m_Tail->m_Node.m_Next = 0;
            else
                m_Head = 0;
        }
        OldNode.m_Node.Clear(); 
    }

    inline void Clear(void)
    {
        while(PopFront())
        {

        }
    }

    inline unsigned Size(void) const
    {
        unsigned int Result = 0;
        T*  pIndex = m_Head;
        while(pIndex)
        {
            pIndex = pIndex->m_Node.m_Next;
            ++Result;
        }
        return Result;
    }

    inline bool IsEmpty() const
    { 
        return (m_Head == 0); 
    }

    T* Front(void) const { return m_Head; }
    T* Back(void) const { return m_Tail; }

    /// Iterator is used to access a double linked list
    /*! An Iterator is always created from a root double linked list. By default, it internal index
    will always point on the first element of the double linked list (the head).
    */			
    class DIterator
    {
    public:	

        NListNoDyn* m_pList;
        T* m_pIndex;
        /// The only Constructor available accept a SRoot as parameter
        inline DIterator(NListNoDyn& List)
        { 
            m_pList = &List;
            Begin();
        }

        inline DIterator&  operator -- ()
        {
            nuxAssert(m_pIndex); 
            m_pIndex = m_pIndex->m_Node.m_pPrev;
            return *this;
        } 				

        inline DIterator   operator -- (int)
        { 
            nuxAssert(m_pIndex);
            DIterator Backup(*this);
            --*this; 
            return Backup;                       
        }				

        inline DIterator&  operator -= (int cnt)
        {
            nuxAssert(m_pIndex); 
            if(cnt < 0)
                cnt = -cnt;

            while(cnt && m_pIndex) 
            { 
                m_pIndex = m_pIndex->m_Node.m_pPrev; 
                cnt--;
            }

            return *this;
        } 				

        inline DIterator& operator ++ ()
        {
            nuxAssert(this->m_pIndex);
            this->m_pIndex = this->m_pIndex->m_Node.m_Next;
            return *this;
        } 				

        inline DIterator operator ++ (int)
        { 
            nuxAssert(m_pIndex);
            DIterator Backup(*this);
            ++*this; 
            return Backup;
        }

        inline DIterator& operator += (int cnt)
        {
            nuxAssert(m_pIndex); 
            if (cnt < 0)
                cnt = -cnt;

            while(cnt && m_pIndex) 
            {
                m_pIndex = m_pIndex->m_Node.m_Next;
                cnt--;
            }
            return *this;
        } 		

        inline bool operator == (const DIterator& Other) const
        {
            return m_pIndex == Other.m_pIndex;
        }	

        inline bool FindForward(const T& ToFind)
        {
            while(!Empty() && (m_pIndex != &ToFind))
            {
                ++(*this);
            }
            return(!Empty());
        }

        inline bool FindReverse(const T& ToFind)
        {
            while(!Empty() && (m_pIndex != &ToFind))
            {
                --(*this);
            }
            return(!Empty());
        }

        /// Removed the current element pointed by the Iterator internal index from the double linked list.
        /*! The function will assert if there is no current element (Current() returns null or Empty()
        returns true).            
        \return Returns a pointer on the removed element
        \sa Current(), Empty()
        */
        T* RemoveCurrent(void)
        {
            nuxAssert(m_pIndex);
            T* pTmp = m_pIndex;
            ++(*this);
            ((NListNoDyn*)m_pList)->Remove(*pTmp);
            return(pTmp);
        }

        inline void Begin(void)
        { 
            m_pIndex = m_pList->Front();  
        }				

        inline void End(void)
        { 
            m_pIndex = m_pList->Back();  
        }				

        inline bool Empty(void) const
        { 
            return (m_pIndex == 0); 
        }

        inline T* operator*(void) const
        { 
            return m_pIndex; 
        }

        inline T* Current(void) const
        { 
            return m_pIndex; 
        }

    }; // class DIterator
};

NAMESPACE_END

#define INL_NLISTNODYN_NODE(T) NListNoDyn<T>::Node m_Node;

#endif // NLIST_H
