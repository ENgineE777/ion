#pragma once

#include "NGUIWidget.h"
#include "NGUIFont.h"

class NGUIImageListDrawer : public NGUIWidget
{		
public:	
		
	PROP_START

	FLOAT_PROP(NGUIImageListDrawer, url_fade_in_time, 0.0f, "Texture", "URL_fade_in")		

	ENUM_PROP(NGUIImageListDrawer, tileType, 0, "Texture", "tileType")
		ENUM_ITEM("clamp", 0)
		ENUM_ITEM("wrap", 1)
		ENUM_ITEM("mirror", 2)
	ENUM_END(NGUIImageListDrawer, tileType)			

	vector<TextureEntry::TextureData*> tex_array;	

	ENTITY_CONSTRUCT(NGUIImageListDrawer,NGUIWidget)
	ENTITY_SCRIPTDEC
	
	virtual void RegisterProperties();
	virtual void Init();	
	virtual void RenderWidget();
	virtual void MethodsToScript(const char* script_class_name);
			
	void NILDScriptAddDrawCallScript(int index, float x, float y, float alpha, float rotate);		
	void NILDScriptAddDrawCallScript2(int index, float x, float y, float wdt, float hgt, float alpha, float rotate);		
	void NILDScriptAddDrawCallScript3(int index, float x, float y, float wdt, float hgt, float alpha, float rotate, float u, float v, float du, float dv);
	
	void ScriptAddDrawCallTex(TextureInstance* tex, float x, float y, float wdt, float hgt, float alpha, float rotate,
							  float u, float v, float du, float dv);				

	float NILDGetTextureDurationScript(int index);
	void  NILDSetTextureScript(int index, std::string& name);	
	void  NILDSetTextureURLScript(int index, std::string& url);
	void  NILDGetTextureFileNameScript(int index, std::string& name);
	void  NILDResizeTextureListScript(int count);
	int   NILDGetTextureListSizeScript();

	bool NILDIsTextureLoading(int index);	
	void NILDPauseTextureScript(int index, bool pause);
	void NILDRestartTextureScript(int index);		
	int  NILDGetTextureWidthScript(int index);
	int  NILDGetTextureHeightScript(int index);
	
	virtual bool AllowMouseCover();			

#ifdef EDITOR
	virtual Color GetDebugColor();		
#endif	

	void FixTextures(TextureInstance* tex, TextureInstance* prev_tex);

	virtual void Release();	
};

