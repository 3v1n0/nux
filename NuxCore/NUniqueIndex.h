#ifndef NUNIQUEINDEX_H
#define NUNIQUEINDEX_H

NAMESPACE_BEGIN

class NUniqueIndex
{
    INL_DECLARE_GLOBAL_OBJECT(NUniqueIndex, NGlobalSingletonInitializer);
public:
    t_u64 GetUniqueIndex();
private:
    NThreadSafeCounter m_UniqueIndex;
    static NCriticalSection m_CriticalSection;
};

NAMESPACE_END

#endif // NUNIQUEINDEX_H
