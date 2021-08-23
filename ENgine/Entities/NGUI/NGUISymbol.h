#pragma once

#include "NGUIWidget.h"

class NGUISymbolInstance;

class NGUISymbol : public NGUIWidget
{		
public:		

	bool type_registered;
	char class_symbol[128];	
	char class_symbol_inst[128];

	ENTITY_CONSTRUCT(NGUISymbol,NGUIWidget)
	ENTITY_SCRIPTDEC

    virtual void SymbolTypesToScript(NGUIWidget* child);
    virtual void SymbolTypesToScript();	

    virtual void EmbedingToScript();

	virtual void Init();
	virtual void ReInit();
	virtual bool Start();
	virtual void RenderWidget();	
	virtual void MethodsToScript(const char* script_class_name);		
			
	virtual bool AllowMouseCover();	

	virtual bool CheckClassName(const char* clsName);
	
#if defined(PC) || defined(OSUNIX)
	virtual Color GetDebugColor();		
	virtual void Release();
    void CopyProperties(NGUIWidget* pDesc);

    static void GetMembersForAutoComplete(bool bSymbol,std::vector<string>& vMembers);
    static void GetSignatureForAutoComplete(bool bSymbol,const string& sName,const string& sMethod,std::vector<string>& vSignatures);
#endif
};

