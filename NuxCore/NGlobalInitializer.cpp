#include "NKernel.h"

NAMESPACE_BEGIN

static NGlobalSingletonInitializer*      GGlobalInitializer   = 0;

static void SystemStart()
{
    static t_u8 StaticBuffer[sizeof(NGlobalSingletonInitializer)];
    // Placement new in our reserved buffer.
    GGlobalInitializer =  new(StaticBuffer) NGlobalSingletonInitializer();

    GLogDevice.AddOutputDevice( &INL_GLOBAL_OBJECT_INSTANCE(NOutputLogFile) );
    GLogDevice.AddOutputDevice( &INL_GLOBAL_OBJECT_INSTANCE(NOutputVisualDebugConsole) );
}

static void SystemShutdown()
{
    // Manually destroy initializer
    if(GGlobalInitializer)
        GGlobalInitializer->~NGlobalSingletonInitializer();
    GGlobalInitializer = 0;

}

bool NGlobalSingletonInitializer::m_bGlobalObjectsReady = false;
NGlobalSingletonInitializer::NGlobalSingletonInitializer()
{
    m_bGlobalObjectsReady = true;
}

NGlobalSingletonInitializer::~NGlobalSingletonInitializer()
{
    m_bGlobalObjectsReady = false;
}

bool NGlobalSingletonInitializer::Ready()
{
    return m_bGlobalObjectsReady;
}

int NGlobalInitializer::m_Count = 0;
NGlobalInitializer::NGlobalInitializer()
{
    if(m_Count++ == 0)
    {
        SystemStart();
    }
}

NGlobalInitializer::~NGlobalInitializer()
{
    if(--m_Count == 0)
    {
        SystemShutdown();
    }
}

NAMESPACE_END
