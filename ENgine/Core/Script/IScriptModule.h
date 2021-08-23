
#pragma once

#include "Common/Common.h"

class IScriptFunction
{				
public:										

	virtual void Reset() = 0;
};

class IScriptModule
{
public:		

	virtual bool Find(IScriptFunction* sf, const char* desc) = 0;	

	virtual void BindImportFunctions() = 0;	

#if defined(PC) || defined(OSUNIX)
	virtual void SaveByteCode(const char* name) = 0;
#endif

	virtual void Release() = 0;
};