#ifndef NARRAY_H
#define NARRAY_H

//
// Base dynamic array.
//

NAMESPACE_BEGIN
class NDataArray
{
public:
    NDataArray()
        :	m_Data(NULL)
        ,   m_ArraySize(0)
        ,	m_ReservedSize(0)
    {}

protected:
    // Constructor
    NDataArray(t_u32 InNum, int ElementSize, DWORD Alignment)
        :   m_Data(NULL)
        ,   m_ArraySize(0)
        ,	m_ReservedSize(0)
    {
        Realloc(InNum, ElementSize, Alignment);
        m_ArraySize = InNum;
    }

public:
    ~NDataArray()
    {
        if(m_Data)
        {
            std::free(m_Data);
        }
        m_Data = NULL;
        m_ArraySize = m_ReservedSize = 0;
    }

    BYTE* BaseBegin() { return m_Data; }
    const BYTE* BaseBegin() const { return m_Data; }
    BYTE* BaseEnd(unsigned int elementSize) { return m_Data + (m_ArraySize * elementSize); }
    const BYTE* BaseEnd(unsigned int elementSize) const { return m_Data + (m_ArraySize * elementSize); }


    /*!
        Get a pointer to the first element of the array.
        @return Pointer to first element of the array or NULL if m_ReservedSize == 0.
    */
    void* GetArrayDataPtr()
    {
        if(m_ArraySize == 0)
            return 0;
        return m_Data;
    }

    /*!
        Get a constant pointer to the first element of the array.
        @return Constant pointer to first element of the array or NULL if m_ReservedSize == 0.
    */
    const void* GetArrayDataPtr() const
    {
        if(m_ArraySize == 0)
            return 0;
        return m_Data;
    }

    BOOL IsValidIndex(t_u32 i) const
    { 
        return (i >= 0) && (i < m_ArraySize);
    }
    inline t_u32 Num() const
    {
        nuxAssert(m_ArraySize >= 0);
        nuxAssert(m_ReservedSize >= m_ArraySize);
        return m_ArraySize;
    }

    inline t_u32 ReservedSize() const
    {
        return m_ReservedSize;
    }

    inline t_u32 Size() const
    {
        return Num();
    }

    void Swap(t_u32 index0, t_u32 index1, t_u32 ElementSize)
    {
        Memswap((BYTE*)m_Data+(ElementSize*index0),(BYTE*)m_Data+(ElementSize*index1),ElementSize);
    }

    void Remove(t_u32 Index, t_u32 Count, t_u32 ElementSize, t_u32 Alignment);

protected:
    void Realloc(t_u32 NewReservedSize, t_u32 ElementSize, t_u32 Alignment);

    BYTE* m_Data;
    t_u32 m_ArraySize;
    t_u32 m_ReservedSize;

};

template<class T, DWORD Alignment = INL_DEFAULT_ALIGNMENT> class NArray;
//
// Templated dynamic array, with optional alignment (defaults to the INL_DEFAULT_ALIGNMENT #define)
//
template< class T, DWORD Alignment > class NArray : public NDataArray
{
    typedef T* PtrType;
    typedef const T* ConstPtrType;

protected:

    PtrType Begin() {return (PtrType) NDataArray::BaseBegin();}
    ConstPtrType Begin() const {return (ConstPtrType) NDataArray::BaseBegin();}

    PtrType End() {return (PtrType) NDataArray::BaseEnd(sizeof(T));}
    ConstPtrType End() const {return (ConstPtrType) NDataArray::BaseEnd(sizeof(T));}

    /*!
        Copies data from one array into this array. 
        Uses the fast copy path if the data does not need a constructor.
        
        @param Source The source array to copy.
    */
    inline void Copy(const NArray<T,Alignment>& Other)
    {
        nuxAssertMsg(this != &Other, TEXT("[NArray::Copy] Cannot copy to self."));
        if(this != &Other)
        {
            if(Other.m_ArraySize > 0)
            {
                Empty();
                Reserve(Other.m_ArraySize);
                if(NTypeTraits<T>::NeedsConstructor)
                {
                    // Using in-place new to copy the element.
                    for(int Index = 0; Index < Other.m_ArraySize; Index++)
                    {
                        AddElement(T(Other(Index)));
                    }
                }
                else
                {
                    Memcpy(m_Data, &Other(0), sizeof(T) * Other.m_ArraySize);
                    m_ArraySize = Other.m_ArraySize;
                }
            }
            else
            {
                Empty();
            }
        }
    }

public:
    typedef T ElementType;
    NArray()
        :	NDataArray()
    {}
    NArray(t_u32 InNum)
        :	NDataArray(InNum, sizeof(T), Alignment)
    {}

    /*!
        Copy constructor.
        @param Other The source array to copy.
    */
    NArray(const NArray& Other) : NDataArray()
    {
        Copy(Other);
    }

    ~NArray()
    {
        //nuxAssert(m_ArraySize>=0);
        //nuxAssert(m_ReservedSize>=m_ArraySize);
        Remove(0, m_ArraySize);
    }

    /*!
        Get a pointer to the first element of the array.
        @return Pointer to first element of the array or NULL if ArrayMax==0.
    */
    T* PtrData()
    {
        return (T*)GetArrayDataPtr();
    }
    /*!
        Get a constant pointer to the first element of the array.
        @return Constant pointer to first element of the array or NULL if ArrayMax==0.
    */
    const T* PtrData() const
    {
        return (T*)GetArrayDataPtr();
    }
    /*!
        Get size of elements
    
        @return Size of elements in bytes.
    */
    DWORD GetElementSize() const
    {
        return sizeof(T);
    }

    inline int GetReservedInByte() const
    {
        return m_ReservedSize * sizeof(T);
    }

    inline int GetSizeInByte() const
    {
        return Num() * sizeof(T);
    }

    inline int GetNumAvailable() const
    {
        return m_ReservedSize - m_ArraySize;
    }


    /*!
        Remove an element at the end of array.
    */
    T Pop()
    {
        nuxAssert(m_ArraySize > 0);
        nuxAssert(m_ReservedSize >= m_ArraySize);
        T Result = ((T*)m_Data)[m_ArraySize - 1];
        Remove(m_ArraySize - 1);
        return Result;
    }
    /*!
        Add an element at the end of array.
    */
    void Push(const T& Item)
    {
        AddElement(Item);
    }

    //! Search for an item index.
    /*!
        Search for an item.
        @param Item The item to search for.
        @return Returns the item index inside the array if it is found. Otherwise, returns -1.
    */
    t_u32 FindItemIndex(const T& Item) const
    {
        for(t_u32 Index = 0; Index < m_ArraySize; Index++)
            if((*this)(Index) == Item)
                return Index;
        return INL_INVALID_INDEX;
    }

    //! Search for an item.
    /*!
        Search for an item.
        @param Item The item to search for.
        @return Returns a constant pointer to the item if it is found. Otherwise, returns NULL.
    */
    ConstPtrType FindItem(const T& Item) const
    {
        ConstPtrType it = Begin();
        ConstPtrType itEnd = End();

        for(; it != itEnd; it++)
        {
            if(*it == Item)
                return it;
        }
        return NULL;
    }

    //! Search for an item.
    /*!
        Search for an item.
        @param Item The item to search for.
        @return Returns a pointer to the item if it is found. Otherwise, returns NULL.
    */
    PtrType FindItem(const T& Item)
    {
        PtrType it = Begin();
        PtrType itEnd = End();

        for(; it != itEnd; it++)
        {
            if((*it) == Item)
                return it;
        }
        return NULL;
    }

    //! Add enough space in the array for Count elements.
    /*!
        The function adds memory location in the array for Count elements. If T needs a constructor
        and there isn't enough space in the array then a new array is created and the object already
        in the array are copied to the new array using T copy constructor. The new elements are not constructed though.
        @param Count Number of elements to add
        @return The index of the first added element.
    */
    int Add(t_u32 Count)
    {
        nuxAssert(Count > 0);
        const int Index = m_ArraySize;
        if(Size() + Count > ReservedSize())
        {
            int newsize = 0;
            newsize = Count + ReservedSize() + 32;

            Reserve(newsize);
        }
        m_ArraySize += Count;
        return Index;

    }

    // Add, Insert, Remove, Empty interface.
    int AddElement(const T& element)
    {
        int index = Add(1);
        if(NTypeTraits<T>::NeedsConstructor)
            new(&(*this)[index]) T(element);
        else
            (*this)[index] = element;
        return index;
    }

//     int AddItem(const T& element)
//     {
//         int index = Add(1);
//         if(NTypeTraits<T>::NeedsConstructor)
//             new(&(*this)[index]) T(element);
//         else
//             (*this)[index] = element;
//         return index;
//     }

    int AddUniqueElement(const T& Item)
    {
        for(t_u32 Index = 0; Index < m_ArraySize; Index++)
            if((*this)(Index) == Item)
                return Index;
        return AddElement(Item);
    }

    int AddZeroData(t_u32 Count)
    {
        return Add(Count);
    }

    void Insert(t_u32 Index, t_u32 Count = 1)
    {
        nuxAssert(Count>=0);
        nuxAssert(m_ArraySize >= 0);
        nuxAssert(m_ReservedSize >= m_ArraySize);
        nuxAssert(Index >= 0);
        nuxAssert(Index <= m_ArraySize);

        int OldNum = m_ArraySize;
        if(Size() + Count > ReservedSize())
        {
            int newsize = 0;
            if(ReservedSize() < 32)
                newsize = ReservedSize() + 32;
            else
                newsize = ReservedSize() * 1.5;

            // increase the size of the array
            Reserve(newsize);
            m_ArraySize += Count;
        }
        Memmove((BYTE*)m_Data + (Index + Count) * sizeof(T),
            (BYTE*)m_Data + Index * sizeof(T),
            (OldNum-Index) * sizeof(T));
    }

    void InsertZeroData(t_u32 Index, t_u32 Count = 1)
    {
        Insert(Index, Count);
    }

    t_u32 InsertItem(const T& Item, t_u32 Index)
    {
        // construct a copy in place at Index (this new operator will insert at 
        // Index, then construct that memory with Item)
        new(*this, Index)T(Item);
        return Index;
    }
    void Remove(t_u32 Index, t_u32 Count=1)
    {
        nuxAssert(Index >= 0);
        nuxAssert(Index <= m_ArraySize);
        nuxAssert(Index + Count <= m_ArraySize);
        if(NTypeTraits<T>::NeedsDestructor)
            for(t_u32 i = Index; i < Index + Count; i++)
                (&(*this)(i))->~T();
        NDataArray::Remove(Index, Count, sizeof(T), Alignment);

        if(	m_ReservedSize && (((m_ReservedSize-m_ArraySize) * sizeof(T) >= 4*4096) || (m_ArraySize == 0)) )
        {
            Reduce(m_ArraySize);
        }
    }

    /*!
        Remove the first occurrence of the given item.
    */
    t_u32 RemoveItemFirstOccurence(const T& Item)
    {
        t_u32 OriginalNum = m_ArraySize;
        for(t_u32 Index = 0; Index < m_ArraySize; Index++)
            if((*this)(Index) == Item)
                Remove(Index--);
        return OriginalNum - m_ArraySize;
    }

    //! Empty the array.
    /*!
        Call objects destructor if any. then resize the array to 0;
    */
    void Empty()
    {
        if(NTypeTraits<T>::NeedsDestructor)
            for(t_u32 i = 0; i < m_ArraySize; i++)
                (&(*this)(i))->~T();
        m_ArraySize = 0;
        Reduce(0);
    }

    //! Clear the array.
    /*!
        Call objects destructor if any.
    */
    void Clear(void)
    {
        if(NTypeTraits<T>::NeedsDestructor)
            for(t_u32 i = 0; i < m_ArraySize; i++)
                (&(*this)(i))->~T();
        m_ArraySize = 0;
    }

    /*!
        Appends the specified array to this array.
        Cannot append to self.
    */
    inline void Append(const NArray<T,Alignment>& Source)
    {
        nuxAssert(this != &Source);
        if(this != &Source && Source.Num() > 0)
        {
            Reserve(m_ArraySize + Source.m_ArraySize);

            if(NTypeTraits<T>::NeedsConstructor)
            {
                for(t_u32 Index = 0 ; Index < Source.m_ArraySize ; ++Index)
                {
                    AddElement(T(Source(Index)));
                }
            }
            else
            {
                Memcpy((BYTE*)m_Data + m_ArraySize * sizeof(T), Source.m_Data, sizeof(T) * Source.m_ArraySize);
                m_ArraySize += Source.m_ArraySize;
            }
        }
    }

    BOOL operator == (const NArray<T,Alignment>& OtherArray) const
    {
        if(Num() != OtherArray.Num())
            return 0;
        for(int Index = 0;Index < Num();Index++)
        {
            if(!((*this)(Index) == OtherArray(Index)))
                return 0;
        }
        return 1;
    }
    BOOL operator != (const NArray<T,Alignment>& OtherArray) const
    {
        if(Num() != OtherArray.Num())
            return 1;
        for(int Index = 0;Index < Num();Index++)
        {
            if(!((*this)(Index) == OtherArray(Index)))
                return 1;
        }
        return 0;
    }

    /*!
        Appends the specified array to this array.
        Cannot append to self.
    */
    NArray& operator += (const NArray& Other)
    {
        Append(Other);
        return *this;
    }

    /*!
        Copies the source array into this one. Uses the common copy method.
    
        @param Other the source array to copy
    */
    NArray& operator = (const NArray& Other)
    {
        Copy(Other);
        return *this;
    }

    T& operator() (t_u32 index)
    {
        nuxAssert(index >= 0);
        nuxAssert(index < m_ArraySize||(index == 0 && m_ArraySize==0));
        nuxAssert(m_ReservedSize >= m_ArraySize);
        return ((T*)m_Data)[index];
    }

    const T& operator() (t_u32 index) const
    {
        nuxAssert(index >= 0);
        nuxAssert(index < m_ArraySize||(index == 0 && m_ArraySize==0));
        nuxAssert(m_ReservedSize >= m_ArraySize);
        return ((T*)m_Data)[index];
    }

    T& operator [] (t_u32 index)
    {
        nuxAssert(index>=0);
        nuxAssert(index<m_ArraySize);
        nuxAssert(m_ReservedSize>=m_ArraySize);

        return ((T*)m_Data)[index];
    }

    const T& operator [] (t_u32 index) const
    {
        nuxAssert(index >= 0);
        nuxAssert(index < m_ArraySize);
        nuxAssert(m_ReservedSize >= m_ArraySize);

        return ((T*)m_Data)[index];
    }

    /*!
        Reserves memory such that the array can contain at least Number elements.
    */
    void Reserve(t_u32 NewReservedSize)
    {
        nuxAssert(NewReservedSize > 0);

        if(NewReservedSize < m_ReservedSize)
            return;

        if(!NTypeTraits<T>::NeedsConstructor)
        {
            NDataArray::Realloc(NewReservedSize, sizeof(T), Alignment);
        }
        else
        {
            PtrType NewData = (PtrType)std::malloc(NewReservedSize * sizeof(T));
            nuxAssertMsg(NewData != 0, TEXT("[NArray::Reserve] Out of Memory!"));

            PtrType itData = NewData;
            PtrType it = Begin();
            PtrType itEnd = End();

            // Copy object from the old array to the new array (Object copy constructor).
            while(it != itEnd)
            {
                new(itData) T(*it);
                ++it;
                ++itData;
            }  

            INL_SAFE_FREE(m_Data);

            m_Data = (BYTE*)NewData;
            m_ReservedSize = NewReservedSize;
        }
    }

    void Reduce(t_u32 NewReservedSize)
    {
        nuxAssert(NewReservedSize >= 0);
        nuxAssert(NewReservedSize >= m_ArraySize);
        nuxAssert(NewReservedSize <= m_ReservedSize);

        if(NewReservedSize < m_ArraySize)
            return;
        if(NewReservedSize >= m_ReservedSize)
            return;

        if(NewReservedSize == 0)
        {
            INL_SAFE_FREE(m_Data);
            m_ReservedSize = 0;
            m_ArraySize = 0;
        }

        if(!NTypeTraits<T>::NeedsConstructor)
        {
            NDataArray::Realloc(NewReservedSize, sizeof(T), Alignment);
        }
        else
        {
            PtrType NewData = (PtrType)std::malloc(NewReservedSize * sizeof(T));
            nuxAssertMsg(NewData != 0, TEXT("[NArray::Reserve] Out of Memory!"));

            PtrType itData = NewData;
            PtrType it = Begin();
            PtrType itEnd = End();

            // Copy object from the old array to the new array (Object copy constructor).
            while(it != itEnd)
            {
                new(itData) T(*it);
                ++it;
                ++itData;
            }  

            INL_SAFE_FREE(m_Data);

            m_Data = (BYTE*)NewData;
            m_ReservedSize = NewReservedSize;
        }
    }

    /*!
        Swap items at the given indices.
    */
    void SwapItems(t_u32 index0, t_u32 index1)
    {
        nuxAssert((index0 >= 0) && (index1 >= 0));
        nuxAssert((m_ArraySize > index0) && (m_ArraySize > index1));
        if(index0 != index1)
        {
            NDataArray::Swap(index0, index1, sizeof(T));
        }
    }

    // Iterator.
    class TIterator
    {
    public:
        TIterator(NArray<T,Alignment>& InArray) : Array(InArray), Index(-1) { ++*this; }
        void operator++()      { ++Index;                                           }
        void RemoveCurrent()   { Array.Remove(Index--); }
        int GetIndex()   const { return Index;                                      }
        operator BOOL() const { return Array.IsValidIndex(Index);                  }
        T& operator*()   const { return Array(Index);                               }
        T* operator->()  const { return &Array(Index);                              }
        T& GetCurrent()  const { return Array(Index);                             }
        T& GetPrev()     const { return Array(Index ? Index-1 : Array.Num()-1);   }
        T& GetNext()     const { return Array(Index<Array.Num()-1 ? Index+1 : 0); }
    private:
        NArray<T,Alignment>& Array;
        t_u32 Index;
    };
};

//
// Array operator news.
//
// template <class T, DWORD Alignment> void* operator new(size_t Size, NArray<T, Alignment>& Array)
// {
//     const int Index = Array.NDataArray::Add(1,sizeof(T), Alignment);
//     return &Array(Index);
// }
// template <class T, DWORD Alignment> void* operator new(size_t Size, NArray<T, Alignment>& Array, int Index)
// {
//     Array.NDataArray::Insert(Index,1,sizeof(T), Alignment);
//     return &Array(Index);
// }

NAMESPACE_END

#endif // NARRAY_H
