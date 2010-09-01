#include "../NKernel.h"
#include "NPluginInterface.h"
#include "NPluginManager.h"


NAMESPACE_BEGIN

void NPluginManager::GetPluginList(TCHAR * dirPath, bool addToList)
{		
	if(!addToList) ClearPluginList();

	WIN32_FIND_DATA fd;
	TCHAR fname[MAX_PATH];
	STRCPY_S(fname, MAX_PATH, dirPath);
    size_t len = _tcslen(fname);
	if(fname[len-1] == TEXT('/') || fname[len-1] == TEXT('\\'))
        STRCAT_S(fname, MAX_PATH, TEXT("*.dll"));
	else
        STRCAT_S(fname, MAX_PATH, TEXT("\\*.dll"));
	HANDLE hFind = FindFirstFile(fname, &fd); 

    NFileName Path = dirPath;
    Path.RemoveBackSlashAtEnd();
    Path.RemoveSlashAtEnd();

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		FindClose(hFind);
		return;
	}

	do 
	{ 
		HINSTANCE dllHandle = NULL;
		try
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{					
                NFileName FilePath = Path + INL_PATH_SEPARATOR_STRING + fd.cFileName;
				dllHandle = LoadLibrary(FilePath.GetTCharPtr());
				if (dllHandle != NULL) 
				{
					PLUGIN_FACTORYFUNC funcHandle;
					funcHandle = reinterpret_cast<PLUGIN_FACTORYFUNC>(GetProcAddress(dllHandle, "makePlugin"));
					if(funcHandle !=NULL)
					{							
						NPlugin * curPlugin = new NPlugin();
						curPlugin->SetFileName(FilePath.GetTCharPtr());

						PLUGIN_TEXTFUNC textFunc;
						textFunc = reinterpret_cast<PLUGIN_TEXTFUNC>(GetProcAddress(dllHandle, "getPluginType"));
						curPlugin->SetType(textFunc());
						textFunc = reinterpret_cast<PLUGIN_TEXTFUNC>(GetProcAddress(dllHandle, "getPluginName"));
						curPlugin->SetName(textFunc());

						pluginRegister.push_back(curPlugin);
					}
					FreeLibrary(dllHandle);
				}
			}
		}
		catch(...)
		{
			if (dllHandle != NULL) FreeLibrary(dllHandle);
		}
	} while (FindNextFile(hFind, &fd));
	FindClose(hFind); 
}

NAMESPACE_END
