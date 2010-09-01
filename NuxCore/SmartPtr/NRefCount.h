#ifndef NREFCOUNT_H
#define NREFCOUNT_H

NAMESPACE_BEGIN

class NRefCount
{
public:
    NRefCount();
    virtual ~NRefCount();
    void Increment();
    void Decrement();
    t_u32 GetRefCount() const;
private:
    NThreadSafeCounter m_TSCounter;
    static NCriticalSection m_CriticalSection;
    
    template < class T > 
    friend class NSmartPtr;
    // or for gcc ???
    // template<class> friend class B; 
    // friend class B <class T>;
};

NAMESPACE_END

#endif // NREFCOUNT_H

