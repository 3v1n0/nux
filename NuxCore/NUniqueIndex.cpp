#include "NKernel.h"
#include "NUniqueIndex.h"

NAMESPACE_BEGIN

INL_IMPLEMENT_GLOBAL_OBJECT(NUniqueIndex);

NCriticalSection NUniqueIndex::m_CriticalSection;

void NUniqueIndex::Constructor()
{
}

void NUniqueIndex::Destructor()
{
}

t_u64 NUniqueIndex::GetUniqueIndex()
{
    NScopeLock Scope(&m_CriticalSection);
    m_UniqueIndex.Increment();
    return m_UniqueIndex.GetValue();
}

NAMESPACE_END

