
#include "NGUIImageDrawer.h"
#include "Core/Network/NetworkService.h"

ENTITY(NGUIImageDrawer)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUIImageDrawer,"NGUIImageDrawer","NGUI", "ImageDrawer")

#ifdef EDITOR

void StartEditImageDrawerGUITexture(PropertiesOwner* owner)
{
	NGUIImageDrawer* drawer = (NGUIImageDrawer*)owner;
	TextureEntry::StartEditGUITexture(&drawer->slice);
}

#endif

void NGUIImageDrawer::RegisterProperties()
{
	NGUIWidget::RegisterProperties();	

	PROP_REG( textureUseEntity )
	PROP_REG( textureEntityName )
#ifdef EDITOR
	PROP_REG( texEdit )	
#endif

	PROP_REG( url_fade_in_time )
}

void NGUIImageDrawer::Init()
{			
	NGUIWidget::Init();	

	slices.push_back(TexSlice(&slice, &textureUseEntity,
#ifdef EDITOR
							  get_textureEntityNameprop_inst(), get_texEditprop_inst(),
#endif
							  textureEntityName.c_str()));
}	

void NGUIImageDrawer::RenderWidget()
{
	float delta_time = core->GetDeltaTime();

	if (slice.tex.texture)
    {
        if (slice.tex.texture->IsReady())
        {
			slice.tex.tex_loading = 0;

            if (slice.tex.fade_in_time > 0.0f)
            {
                slice.tex.fade_in_time -= delta_time;
				
                if (slice.tex.fade_in_time < 0.0f)
                {
                    slice.tex.fade_in_time = -1.0f;
                }
            }
        }
	}
}

void NGUIImageDrawer::FixTextures(TextureInstance* tex, TextureInstance* prev_tex)
{
	for (int i=0;i<draws.size();i++)
	{
		if (draws[i].tex == prev_tex)
		{
			draws[i].tex = tex;			
		}
	}

	if (parent)
	{
		for (int i=0;i<parent->draws.size();i++)
		{
			if (parent->draws[i].tex == prev_tex)
			{
				parent->draws[i].tex = tex;
			}
		}
	}	
}

CALLWRAPPERARG4(NGUIImageDrawer, NIDScriptAddDrawCallScript, CALLWRAPPERGETPARAM(float,arg1),CALLWRAPPERGETPARAM(float,arg2),CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG6(NGUIImageDrawer, NIDScriptAddDrawCallScript2, CALLWRAPPERGETPARAM(float,arg1),CALLWRAPPERGETPARAM(float,arg2),CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4),CALLWRAPPERGETPARAM(float,arg5),CALLWRAPPERGETPARAM(float,arg6), CALLWRAPPERNORET())

CALLWRAPPERARG1(NGUIImageDrawer, NIDSetTextureScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUIImageDrawer, NIDSetTextureURLScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUIImageDrawer, NIDGetTextureFileNameScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	
CALLWRAPPER(NGUIImageDrawer, NIDIsTextureLoading, CALLWRAPPERNORET())

CALLWRAPPER(NGUIImageDrawer, NIDRestartTextureScript, CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUIImageDrawer, NIDPauseTextureScript, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())	
CALLWRAPPER(NGUIImageDrawer, NIDGetTextureDurationScript, CALLWRAPPERRET(float))

CALLWRAPPER(NGUIImageDrawer, NIDGetTextureWidthScript, CALLWRAPPERRET(int))
CALLWRAPPER(NGUIImageDrawer, NIDGetTextureHeightScript, CALLWRAPPERRET(int))
	
CALLWRAPPERARG4(NGUIImageDrawer, NIDSetScriptTextureUV, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())	


void NGUIImageDrawer::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void Draw(float x, float y, float alpha, float rotate)", CALLWRAPPERNAMEAS(NIDScriptAddDrawCallScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void Draw(float x, float y, float wdt, float hgt, float alpha, float rotate)", CALLWRAPPERNAMEAS(NIDScriptAddDrawCallScript2));	
	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTexture(string&in texture)", CALLWRAPPERNAMEAS(NIDSetTextureScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTextureURL(string&in url)", CALLWRAPPERNAMEAS(NIDSetTextureURLScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetTextureFileName(string&out name)", CALLWRAPPERNAMEAS(NIDGetTextureFileNameScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool IsTextureLoading()", CALLWRAPPERNAMEAS(NIDIsTextureLoading));			
	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void RestartTexture()", CALLWRAPPERNAMEAS(NIDRestartTextureScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void PauseTexture(bool pause)", CALLWRAPPERNAMEAS(NIDPauseTextureScript));		
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "float GetTextureDuration()", CALLWRAPPERNAMEAS(NIDGetTextureDurationScript));	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int GetTextureWidth()", CALLWRAPPERNAMEAS(NIDGetTextureWidthScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int GetTextureHeight()", CALLWRAPPERNAMEAS(NIDGetTextureHeightScript));			

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTextureUV(float u, float v, float du, float dv)", CALLWRAPPERNAMEAS(NIDSetScriptTextureUV));
}

void NGUIImageDrawer::NIDSetScriptTextureUV(float u, float v, float du, float dv)
{
	if (slice.type == 0)
	{
		slice.rects[0].u = u;
		slice.rects[0].v = v;
		slice.rects[0].du = du;
		slice.rects[0].dv = dv;
	}
}

void NGUIImageDrawer::NIDScriptAddDrawCallScript(float set_x, float set_y, float alpha, float rotate)
{	
	if (!slice.tex.texture) return;
    if (!slice.tex.texture->IsReady()) return;
    
	float da = 1.0f;
	
	if (slice.tex.fade_in_time > 0.0f)
	{
		da = 1.0f - slice.tex.fade_in_time / url_fade_in_time;
	}

	ScriptAddDrawCallTex(set_x, set_y, width, height, alpha * da, rotate);
}

void NGUIImageDrawer::NIDScriptAddDrawCallScript2(float set_x, float set_y, float wdt, float hgt, float alpha, float rotate)
{
	if (!slice.tex.texture) return;
    if (!slice.tex.texture->IsReady()) return;
    
	float da = 1.0f;
	
	if (slice.tex.fade_in_time > 0.0f)
	{
		da = 1.0f - slice.tex.fade_in_time / url_fade_in_time;
	}

	ScriptAddDrawCallTex(set_x, set_y, wdt, hgt, alpha * da, rotate);
}

void NGUIImageDrawer::ScriptAddDrawCallTex(float set_x, float set_y, float wdt, float hgt, float alpha, float rotate)
{
	if (!IsVisible()) return;	

	if (parent) 
	{
		if (parent->IsAffectChildDrawCall())
		{
			parent->AffectChildDrawCall(set_x, set_y, wdt, hgt, alpha);
		}
	}

	clr.a = parent_alpha * alpha;
	
	if (slices[0].tex_entity)
	{
		TextureEntry::Draw(this, ConvertHorz(set_x), ConvertVert(set_y), ConvertHorz(wdt), ConvertVert(hgt), clr, &slices[0].tex_entity->slice);
	}
	else
	{
		TextureEntry::Draw(this, ConvertHorz(set_x), ConvertVert(set_y), ConvertHorz(wdt), ConvertVert(hgt), clr, &slice);
	}	
}

void NGUIImageDrawer::NIDPauseTextureScript(bool pause)
{	
	if (slice.tex.texture)
	{
		slice.tex.texture->Pause(pause);
	}
}

void NGUIImageDrawer::NIDRestartTextureScript()
{	
	if (slice.tex.texture)
	{
		slice.tex.texture->Restart();
	}
}

int NGUIImageDrawer::NIDGetTextureWidthScript()
{
	if (slice.tex.texture)
	{
		return slice.tex.texture->GetWidth();
	}

	return -1;
}

int NGUIImageDrawer::NIDGetTextureHeightScript()
{	
	if (slice.tex.texture)
	{
		return slice.tex.texture->GetHeight();
	}

	return -1;
}

void NGUIImageDrawer::NIDSetTextureScript(std::string& txt)
{	
	SetTextureData(&slice.tex, txt.c_str()); 	
}

void NGUIImageDrawer::NIDGetTextureFileNameScript(std::string& name)
{	
#ifdef IOS      
    name = string("../Library/Caches/") + slice.tex.tex_name;
#else
    name = slice.tex.tex_name;
#endif    
}

float NGUIImageDrawer::NIDGetTextureDurationScript()
{
	if (slice.tex.texture)
	{
		return slice.tex.texture->GetDuration();
	}

	return -1.0f;	
}

void NGUIImageDrawer::NIDSetTextureURLScript(std::string& url)
{
	SetTextureDataURL(&slice.tex, url.c_str(), url_fade_in_time);	
}

bool NGUIImageDrawer::NIDIsTextureLoading()
{	
	return (slice.tex.tex_loading == 1);
}

bool NGUIImageDrawer::AllowMouseCover()
{
	return false;	
}

#ifdef EDITOR
Color NGUIImageDrawer::GetDebugColor()
{
	return COLOR_GREEN;	
}
#endif

void NGUIImageDrawer::Release()
{	
	RELEASE(slice.tex.texture)	

	NGUIWidget::Release();
}