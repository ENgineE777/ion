#pragma once
#include "Core/Core.h"

class NGUIWidget;

#ifdef EDITOR
#include "Platforms/PC/GUI/ComboBoxWidget.h"
extern void EditorFontEnumerator(ComboBoxWidget* widget);
#endif

class NGUIFont : public SceneEntity
{		
public:

	PROP_START
	FONT_PROP( NGUIFont, font, "Font", "Font" )

	ENTITY_DEFC(NGUIFont)	
	
	FontInstance* GetFont();

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();				
	virtual void Release();	

#ifdef EDITOR
	vector<NGUIWidget*> users;
	void AddUseFont(NGUIWidget* user);
	void DelUseFont(NGUIWidget* user);
#endif
};
