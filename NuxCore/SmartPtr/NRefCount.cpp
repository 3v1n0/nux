#include "../NKernel.h"
#include "NRefCount.h"

NAMESPACE_BEGIN

NCriticalSection NRefCount::m_CriticalSection;

NRefCount::NRefCount()
{

}

NRefCount::~NRefCount()
{

}

void NRefCount::Increment()
{
    NScopeLock Scope(&m_CriticalSection);
    m_TSCounter.Increment();
}

void NRefCount::Decrement()
{
    NScopeLock Scope(&m_CriticalSection);
    m_TSCounter.Decrement();
}

t_u32 NRefCount::GetRefCount() const
{
    NScopeLock Scope(&m_CriticalSection);
    return m_TSCounter.GetValue();
}

NAMESPACE_END
