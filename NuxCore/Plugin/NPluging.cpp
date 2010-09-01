#include "../NKernel.h"
#include "NPlugin.h"

NAMESPACE_BEGIN

NPlugin::~NPlugin()
{
    delete[] filename;	
    delete[] pluginType;
    delete[] pluginName;
    if(dllHandle) FreeLibrary(dllHandle);

    ClearMembers();
}

void NPlugin::SetFileName(const TCHAR * nm)
{
    size_t sz = _tcslen(nm)+1;
    filename = new TCHAR[sz];
    STRCPY_S(filename, sz, nm);
}

void NPlugin::SetName(const TCHAR * nm)
{
    size_t sz = _tcslen(nm)+1;
    pluginName = new TCHAR[sz];
    STRCPY_S(pluginName, sz, nm);
}

void NPlugin::SetType(const TCHAR * nm)
{
    size_t sz = _tcslen(nm)+1;
    pluginType = new TCHAR[sz];
    STRCPY_S(pluginType, sz, nm);
}


NPluginInterface * NPlugin::MakeNewInstance()
{
    if(!dllHandle) dllHandle = LoadLibrary(filename);

    if(dllHandle != NULL) 
    {				
        funcHandle = reinterpret_cast<PLUGIN_FACTORYFUNC>(GetProcAddress(dllHandle, "MakePlugin"));
        if(funcHandle !=NULL)
        {
            return funcHandle();			
        }		
    }
    return NULL;
}

NAMESPACE_END
