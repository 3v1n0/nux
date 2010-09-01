#ifndef NPLUGINMANAGER_H
#define NPLUGINMANAGER_H

#include "NPlugin.h"

NAMESPACE_BEGIN

class NPluginManager
{
	public:
		NPluginManager(){}
		~NPluginManager() { ClearPluginList(); }


		void GetPluginList(TCHAR * dirPath, bool addToList=false);

		NPluginInterface * MakeNewPluginInstance(int index)
		{
			return pluginRegister.at(index)->MakeNewInstance();
		}
		int GetNumPlugins()
		{
			return int(pluginRegister.size());
		}
		
		TCHAR * GetPluginName(int index)
		{
			return pluginRegister.at(index)->GetName();
		}
		TCHAR * KetPluginType(int index)
		{
			return pluginRegister.at(index)->GetType();
		}

	private:
		void ClearPluginList()
		{
			for (unsigned int i = 0; i < pluginRegister.size(); i++)
			{
				delete pluginRegister.at(i);		
			}	
			pluginRegister.clear();
		}
        std::vector<NPlugin*> pluginRegister;
};

NAMESPACE_END
#endif // NPLUGINMANAGER_H
