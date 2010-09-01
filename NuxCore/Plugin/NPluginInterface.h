#ifndef NPLUGININTERFACE_H
#define NPLUGININTERFACE_H

#include "NPlugin.h"

NAMESPACE_BEGIN

class NPluginInterface 
{
public:
    NPluginInterface(){}
	virtual ~NPluginInterface(){}

	virtual int Activate()=0;
	virtual int Execute()=0;
	virtual int Destroy()=0;
};


NAMESPACE_END

#endif // NPLUGININTERFACE_H


