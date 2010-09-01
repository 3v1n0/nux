#include "../NKernel.h"

#include "NMemoryHook.h"

NAMESPACE_BEGIN

INL_IMPLEMENT_GLOBAL_OBJECT(MemHook)

// Singleton constructor 
void MemHook::Constructor()
{
    // The first hook will point to our default hook...
    m_pRootHook = &m_DefaultHook;
}

// Singleton destructor 
void MemHook::Destructor()
{
    // Check if all object that has hooked unregistered like they should...
    nuxAssert(m_pRootHook == &m_DefaultHook);
}    	

void MemoryHook::Hook()
{
    // Register 
    m_pNextHook = INL_GLOBAL_OBJECT_INSTANCE(MemHook).Hook(this);
}

void MemoryHook::UnHook()
{
    // Unregister 
    INL_GLOBAL_OBJECT_INSTANCE(MemHook).UnHook(this, m_pNextHook);
    m_pNextHook = 0;
}

NAMESPACE_END

