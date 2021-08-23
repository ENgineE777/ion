#pragma once

#include "NGUIWidget.h"
#include "NGUIFont.h"

#ifdef EDITOR

#include "Platforms/PC/GUI/ComboBoxWidget.h"
extern void EditorGUITexrtureEnumerator(ComboBoxWidget* widget);
extern void StartEditImageDrawerGUITexture(PropertiesOwner* owner);

#endif

class NGUIImageDrawer : public NGUIWidget
{		
public:	
			
	TextureEntry::Slice slice;

	PROP_START

	BOOL_PROP(NGUIImageDrawer, textureUseEntity, false, "Texture", "UseEntity2")

	STRING_ENUM_PROP(NGUIImageDrawer, textureEntityName, "", "Texture", "Texture", true, EditorGUITexrtureEnumerator)

#ifdef EDITOR
	CALLBACK_PROP(NGUIImageDrawer, texEdit, StartEditImageDrawerGUITexture, "Texture", "Edit")
#endif

	FLOAT_PROP(NGUIImageDrawer, url_fade_in_time, 0.0f, "Texture", "URL_fade_in")			

	static RequestListiner net_listiner;

	ENTITY_CONSTRUCT(NGUIImageDrawer,NGUIWidget)
	ENTITY_SCRIPTDEC

	virtual void RegisterProperties();
	virtual void Init();		
	virtual void RenderWidget();	
	virtual void MethodsToScript(const char* script_class_name);
			
	void NIDScriptAddDrawCallScript(float x, float y, float alpha, float rotate);		
	void NIDScriptAddDrawCallScript2(float x, float y, float wdt, float hgt, float alpha, float rotate);	
	
	void ScriptAddDrawCallTex(float x, float y, float wdt, float hgt, float alpha, float rotate);					

	float NIDGetTextureDurationScript();
	void  NIDSetTextureScript(std::string& name);	
	void  NIDSetTextureURLScript(std::string& url);
	void  NIDGetTextureFileNameScript(std::string& name);	

	bool NIDIsTextureLoading();	
	void NIDPauseTextureScript(bool pause);
	void NIDRestartTextureScript();		
	int  NIDGetTextureWidthScript();
	int  NIDGetTextureHeightScript();
	
	virtual bool AllowMouseCover();			

	void NIDSetScriptTextureUV(float u, float v, float du, float dv);

#ifdef EDITOR
	virtual Color GetDebugColor();		
#endif	

	void FixTextures(TextureInstance* tex, TextureInstance* prev_tex);

	virtual void Release();	
};

