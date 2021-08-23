
#include "NGUIImageListDrawer.h"
#include "Core/Network/NetworkService.h"

ENTITY(NGUIImageListDrawer)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUIImageListDrawer,"NGUIImageListDrawer","NGUI", "ImageListDrawer")

void NGUIImageListDrawer::RegisterProperties()
{
	NGUIWidget::RegisterProperties();	

	PROP_REG( url_fade_in_time )		
	PROP_REG(tileType)
}

void NGUIImageListDrawer::Init()
{		
	NGUIWidget::Init();
}	

void NGUIImageListDrawer::RenderWidget()
{
	float delta_time = core->GetDeltaTime();

	for (int i=0;i<tex_array.size();i++)
	{			
		TextureEntry::TextureData* tex = tex_array[i];

		if (tex->texture)
		{
			if (tex->texture->IsReady())
			{
				tex->tex_loading = 0;

				if (tex->fade_in_time > 0.0f)
				{
					tex->fade_in_time -= delta_time;
					
					if (tex->fade_in_time < 0.0f)
					{
						tex->fade_in_time = -1.0f;
					}
				}
			}
        }
	}
}

void NGUIImageListDrawer::FixTextures(TextureInstance* tex, TextureInstance* prev_tex)
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

CALLWRAPPERARG5(NGUIImageListDrawer, NILDScriptAddDrawCallScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(float,arg2),CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4),CALLWRAPPERGETPARAM(float,arg5), CALLWRAPPERNORET())
CALLWRAPPERARG7(NGUIImageListDrawer, NILDScriptAddDrawCallScript2, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETPARAM(float,arg2),CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4),CALLWRAPPERGETPARAM(float,arg5),CALLWRAPPERGETPARAM(float,arg6),CALLWRAPPERGETPARAM(float,arg7), CALLWRAPPERNORET())
CALLWRAPPERARG11(NGUIImageListDrawer, NILDScriptAddDrawCallScript3, CALLWRAPPERGETPARAM(int,arg1),CALLWRAPPERGETPARAM(float,arg2),CALLWRAPPERGETPARAM(float,arg3),CALLWRAPPERGETPARAM(float,arg4),CALLWRAPPERGETPARAM(float,arg5),CALLWRAPPERGETPARAM(float,arg6),CALLWRAPPERGETPARAM(float,arg7),CALLWRAPPERGETPARAM(float,arg8),CALLWRAPPERGETPARAM(float,arg9),CALLWRAPPERGETPARAM(float,arg10),CALLWRAPPERGETPARAM(float,arg11), CALLWRAPPERNORET())

CALLWRAPPERARG2(NGUIImageListDrawer, NILDSetTextureScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(NGUIImageListDrawer, NILDSetTextureURLScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())
CALLWRAPPERARG2(NGUIImageListDrawer, NILDGetTextureFileNameScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETADRESS(string,arg2), CALLWRAPPERNORET())	
CALLWRAPPERARG1(NGUIImageListDrawer, NILDResizeTextureListScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERNORET())
CALLWRAPPER(NGUIImageListDrawer, NILDGetTextureListSizeScript, CALLWRAPPERRET(int))
CALLWRAPPERARG1(NGUIImageListDrawer, NILDIsTextureLoading, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERRET(bool))

CALLWRAPPERARG1(NGUIImageListDrawer, NILDRestartTextureScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG2(NGUIImageListDrawer, NILDPauseTextureScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERGETPARAM(bool,arg2), CALLWRAPPERNORET())	
CALLWRAPPERARG1(NGUIImageListDrawer, NILDGetTextureDurationScript,CALLWRAPPERGETPARAM(int,arg1),  CALLWRAPPERRET(float))

CALLWRAPPERARG1(NGUIImageListDrawer, NILDGetTextureWidthScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERRET(int))
CALLWRAPPERARG1(NGUIImageListDrawer, NILDGetTextureHeightScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERRET(int))

void NGUIImageListDrawer::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);

	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void Draw(int index, float x, float y, float alpha, float rotate)", CALLWRAPPERNAMEAS(NILDScriptAddDrawCallScript));
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void Draw(int index, float x, float y, float wdt, float hgt, float alpha, float rotate)", CALLWRAPPERNAMEAS(NILDScriptAddDrawCallScript2));	
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void Draw(int index, float x, float y, float wdt, float hgt, float alpha, float rotate, float u, float v, float du, float dv)", CALLWRAPPERNAMEAS(NILDScriptAddDrawCallScript3));	
	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTexture(int index, string&in texture)", CALLWRAPPERNAMEAS(NILDSetTextureScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTextureURL(int index, string&in url)", CALLWRAPPERNAMEAS(NILDSetTextureURLScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetTextureFileName(int index, string&out name)", CALLWRAPPERNAMEAS(NILDGetTextureFileNameScript));		
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void ResizeList(int count)", CALLWRAPPERNAMEAS(NILDResizeTextureListScript));		
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int  GetListSize()", CALLWRAPPERNAMEAS(NILDGetTextureListSizeScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool IsTextureLoading(int index)", CALLWRAPPERNAMEAS(NILDIsTextureLoading));				

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void RestartTexture(int index)", CALLWRAPPERNAMEAS(NILDRestartTextureScript));	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void PauseTexture(int index, bool pause)", CALLWRAPPERNAMEAS(NILDPauseTextureScript));		
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "float GetTextureDuration(int index)", CALLWRAPPERNAMEAS(NILDGetTextureDurationScript));	

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int GetTextureWidth(int index)", CALLWRAPPERNAMEAS(NILDGetTextureWidthScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "int GetTextureHeight(int index)", CALLWRAPPERNAMEAS(NILDGetTextureHeightScript));			
}

void NGUIImageListDrawer::NILDScriptAddDrawCallScript(int index, float set_x, float set_y, float alpha, float rotate)
{
	if (index<0 || index>=tex_array.size()) return;

    if (!tex_array[index]->texture) return;
    if (!tex_array[index]->texture->IsReady()) return;
    
	float da = 1.0f;
	
	if (tex_array[index]->fade_in_time > 0.0f)
	{
		da = 1.0f - tex_array[index]->fade_in_time / url_fade_in_time;
	}

	ScriptAddDrawCallTex(tex_array[index]->texture, set_x, set_y, width, height, alpha * da, rotate, 0.0f, 0.0f, 1.0f, 1.0f);
}

void NGUIImageListDrawer::NILDScriptAddDrawCallScript2(int index, float set_x, float set_y, float wdt, float hgt, float alpha, float rotate)
{
	if (index<0 || index>=tex_array.size()) return;

    if (!tex_array[index]->texture) return;
    if (!tex_array[index]->texture->IsReady()) return;
    
	float da = 1.0f;
	
	if (tex_array[index]->fade_in_time > 0.0f)
	{
		da = 1.0f - tex_array[index]->fade_in_time / url_fade_in_time;
	}

	ScriptAddDrawCallTex(tex_array[index]->texture, set_x, set_y, wdt, hgt, alpha * da, rotate, 0.0f, 0.0f, 1.0f, 1.0f);
}

void NGUIImageListDrawer::NILDScriptAddDrawCallScript3(int index, float set_x, float set_y, float wdt, float hgt, float alpha, float rotate, float u, float v, float du, float dv)
{
	if (index<0 || index>=tex_array.size()) return;

    if (!tex_array[index]->texture) return;
    if (!tex_array[index]->texture->IsReady()) return;
    
	float da = 1.0f;
	
	if (tex_array[index]->fade_in_time > 0.0f)
	{
		da = 1.0f - tex_array[index]->fade_in_time / url_fade_in_time;
	}
            
	ScriptAddDrawCallTex(tex_array[index]->texture, set_x, set_y, wdt, hgt, alpha * da, rotate, u, v, du, dv);
}

void NGUIImageListDrawer::ScriptAddDrawCallTex(TextureInstance* tex, float set_x, float set_y, float wdt, float hgt, float alpha, float rotate, float u, float v, float du, float dv)
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
	
	AddDrawCall(tex, ConvertHorz(set_x ), ConvertVert(set_y ),
			    ConvertHorz(wdt), ConvertVert(hgt), clr, u, v, du, dv);
}

void NGUIImageListDrawer::NILDPauseTextureScript(int index, bool pause)
{
	if (index >= tex_array.size())
	{
		return;
	}

	if (tex_array[index]->texture)
	{
		tex_array[index]->texture->Pause(pause);
	}

}

void NGUIImageListDrawer::NILDRestartTextureScript(int index)
{
	if (index >= tex_array.size())
	{
		return;
	}

	if (tex_array[index]->texture)
	{
		tex_array[index]->texture->Restart();
	}
}

int NGUIImageListDrawer::NILDGetTextureWidthScript(int index)
{
	if (index<0 || index>=tex_array.size()) return -1;

	if (tex_array[index]->texture)
	{
		return tex_array[index]->texture->GetWidth();
	}

	return -1;
}

int NGUIImageListDrawer::NILDGetTextureHeightScript(int index)
{
	if (index<0 || index>=tex_array.size()) return -1;

	if (tex_array[index]->texture)
	{
		return tex_array[index]->texture->GetHeight();
	}

	return -1;
}

void NGUIImageListDrawer::NILDSetTextureScript(int index, std::string& txt)
{
	if (index<0 || index>= tex_array.size())
	{
		return;
	}

	SetTextureData(tex_array[index], txt.c_str()); 	
}

void NGUIImageListDrawer::NILDGetTextureFileNameScript(int index, std::string& name)
{
	if (index<0 || index>= tex_array.size())
	{
		return;
	}

#ifdef IOS      
    name = string("../Library/Caches/") + tex_array[index]->tex_name;
#else
	name = tex_array[index]->tex_name;
#endif    
}

float NGUIImageListDrawer::NILDGetTextureDurationScript(int index)
{
	if (index<0 || index>=tex_array.size()) return -1.0f;

	
	if (tex_array[index]->texture)
	{
		return tex_array[index]->texture->GetDuration();
	}

	return -1.0f;	
}

void NGUIImageListDrawer::NILDSetTextureURLScript(int index, std::string& url)
{
	if (index<0 || index>=tex_array.size()) return;
	
	SetTextureDataURL(tex_array[index], url.c_str(), url_fade_in_time);		
}

void NGUIImageListDrawer::NILDResizeTextureListScript(int count)
{	
	int prev_count = tex_array.size();	

	/*for (int i=0; i<prev_count;i++)
	{
		while (tex_array[i].tex_loading)
		{
			ThreadWorker::Sleep(100);
			core->Network()->Work(0, 0);
		}
	}*/

	if (count < prev_count)
	{		
		for (int i=count; i<prev_count;i++)
		{
			while ((tex_array[i]->tex_loading == 1))
			{
				ThreadWorker::Sleep(100);				

				((NetworkService*)en_core.Network())->Work(0, 0);
				en_core.Render()->Work(0,0);

				if (tex_array[i]->texture && tex_array[i]->texture->IsReady())
				{
					tex_array[i]->tex_loading = 0;
				}
			}

			FixTextures(null, tex_array[i]->texture);						
		}		
	}	

	tex_array.resize(count);

	if (count > prev_count)
	{
		for (int i=prev_count; i<count;i++)
		{
			tex_array[i] = new TextureEntry::TextureData();
		}
	}
}

int NGUIImageListDrawer::NILDGetTextureListSizeScript()
{
	return tex_array.size();
}

bool NGUIImageListDrawer::NILDIsTextureLoading(int index)
{
	if (index<0 || index>=tex_array.size()) return false;	
   
	if (tex_array[index]->texture)
	{
		return !tex_array[index]->texture->IsReady();
	}

	return (tex_array[index]->tex_loading == 1);
}

bool NGUIImageListDrawer::AllowMouseCover()
{
	return false;	
}

#ifdef EDITOR
Color NGUIImageListDrawer::GetDebugColor()
{
	return COLOR_GREEN;	
}
#endif

void NGUIImageListDrawer::Release()
{	
	for (int i=0;i<tex_array.size();i++)
	{
		RELEASE(tex_array[i]->texture)
		delete tex_array[i];
	}

	NGUIWidget::Release();
}