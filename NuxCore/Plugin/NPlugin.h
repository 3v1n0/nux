#ifndef NPLUGIN_H
#define NPLUGIN_H

#define INL_DECLARE_PLUGIN(x) extern "C"{__declspec(dllexport) NPluginInterface * MakePlugin(){ return new x;}}
#define INL_SET_PLUGIN_TYPE(x) extern "C"{__declspec(dllexport) TCHAR * GetPluginType(){ return x;}}
#define INL_SET_PLUGIN_NAME(x) extern "C"{__declspec(dllexport) TCHAR * GetPluginName(){ return x;}}

NAMESPACE_BEGIN

class NPluginInterface;

typedef NPluginInterface * (*PLUGIN_FACTORYFUNC)();
typedef TCHAR * (*PLUGIN_TEXTFUNC)();

class NPlugin
{
public:
    NPlugin() { ClearMembers(); }

    ~NPlugin();

    NPluginInterface * MakeNewInstance();

    void SetFileName(const TCHAR * nm);
    TCHAR * GetName() { return pluginName; }
    TCHAR * GetType() { return pluginType; }
    void SetName(const TCHAR * nm);		
    void SetType(const TCHAR * nm);

private:
    void ClearMembers()
    {
        pluginType=NULL;
        pluginName=NULL;
        filename=NULL;
        dllHandle=NULL;
    }

    TCHAR * filename;
    TCHAR * pluginType;
    TCHAR * pluginName;
    HINSTANCE dllHandle;
    PLUGIN_FACTORYFUNC funcHandle;		
};

NAMESPACE_END

#endif // NPLUGIN_H
