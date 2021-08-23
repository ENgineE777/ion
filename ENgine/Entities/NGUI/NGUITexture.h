#pragma once
#include "Core/Core.h"
#include "TextureEntry.h"

class NGUIWidget;

#ifdef EDITOR
#include "Platforms/PC/GUI/ComboBoxWidget.h"
extern void StartEditGUITexture(PropertiesOwner* owner);
#endif

class NGUITexture : public SceneEntity
{		
public:
	
	TextureEntry::Slice slice;			

	PROP_START
#ifdef EDITOR
	CALLBACK_PROP(NGUITexture, texEdit, StartEditGUITexture, "Texture", "Edit")
#endif	
		
	ENTITY_DEFC(NGUITexture)	
		
	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();	
	virtual void LoadProperties(IStreamLoader* loader);
	virtual void SaveProperties(IStreamSaver* saver);
	virtual void Copy(PropertiesOwner* propHolder);

	virtual void Release();	

#ifdef EDITOR
	vector<NGUIWidget*> users;
	void AddUser(NGUIWidget* user);
	void DelUser(NGUIWidget* user);
	virtual void SetEditMode(bool ed);
	virtual bool UpdateEditorWidgets();
#endif
};
