#pragma once

#include "NGUIWidget.h"
#include "NGUISymbol.h"

class NGUISymbolInstance : public NGUIWidget
{		
    bool type_registered;
	bool bAbstarct;
public:		
	 	
	bool childs_are_proxy;
	NGUISymbol* wgt_symbol;
	 
	PROP_START
	STRING_PROP(NGUISymbolInstance, symbol_name, "", "Properties", "symbol")
	BOOL_PROP(NGUISymbolInstance, useInstance, false, "Properties", "UseInstance")

	ENTITY_CONSTRUCT(NGUISymbolInstance,NGUIWidget)
	ENTITY_SCRIPTDEC	

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();	

	virtual bool Start();

	virtual void MethodsToScript(const char* script_class_name);		
	virtual void EmbedingToScript();
			
	virtual bool AllowMouseCover();	
	
	void DeleteAllChilds(NGUIWidget* entity);

	virtual void Release();	

	inline bool IsAbstarct()const {return bAbstarct;}

	NGUIWidget* AddInstance();
	void SInstDelInstance(int index);
	void SInstResizeInstances(int count);
	int  SInstGetInstancesCount();
 
	void SGetSymbolNameScript(string& name);

	void InstanceSymbol(NGUIWidget* symbol, NGUIWidget* parent);
  //  virtual bool CheckClassName(const char* clsName);

#if defined(PC) || defined(OSUNIX)
	void CopyProperties(NGUIWidget* symbol);
	virtual Color GetDebugColor();	
#endif
};

