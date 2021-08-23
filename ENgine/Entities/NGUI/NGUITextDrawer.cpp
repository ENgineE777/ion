
#include "NGUITextDrawer.h"

ENTITY(NGUITextDrawer)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUITextDrawer,"NGUITextDrawer","NGUI", "TextDrawer")

void NGUITextDrawer::RegisterProperties()
{
	NGUIWidget::RegisterProperties();	

	PROP_REG( fontUseEntity )
	PROP_REG( fontEntityName )
	PROP_REG( fontDesc )				
	PROP_REG( fontScale )
}

void NGUITextDrawer::Init()
{	
	NGUIWidget::Init();								

	need_release_font = true;	
	font_entity = null;
}
	
void NGUITextDrawer::ReInit()
{
	NGUIWidget::ReInit();	
			
#ifdef EDITOR	
	if (need_release_font != !fontUseEntity)
	{
		if (ed_rollout)
		{
			SET_SKIP(fontEntityName, !fontUseEntity )
		
			SET_SKIP(fontDesc, fontUseEntity )			

			ed_rollout->CalcPos();
			ed_rollout->Redraw();
		}
	}	
#endif
	
	if (need_release_font)
	{
		RELEASE(font)
	}
#ifdef EDITOR
	else
	{
		if (font_entity)
		{
			font_entity->DelUseFont(this);
		}
	}
#endif

	need_release_font = !fontUseEntity;

	if (fontUseEntity)
	{
		NGUIFont* fnt = (NGUIFont*)Scene()->Find(fontEntityName.c_str(), "NGUIFont");
		
		if (fnt)
		{
#ifdef EDITOR
			font_entity = fnt;
			font_entity->AddUseFont(this);
#endif
			font = fnt->font.font;			
		}
		else
		{
			font = null;
		}
	}
	else
	{
		font = en_core.Font()->CreateFont(fontDesc.name, fontDesc.bold, fontDesc.italic, ConvertVert(fontDesc.height), fontDesc.gap, fontDesc.outline);		
		font_entity = null;
	}
}

CALLWRAPPERARG4(NGUITextDrawer, NTDScriptAddPrintCall, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETADRESS(string,arg3), CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERNORET())
CALLWRAPPERARG6(NGUITextDrawer, NTDScriptAddPrintCall2, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERGETADRESS(string,arg5), CALLWRAPPERGETPARAM(float,arg6), CALLWRAPPERNORET())
CALLWRAPPERARG7(NGUITextDrawer, NTDScriptAddPrintCall3, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERGETPARAM(float,arg4), CALLWRAPPERGETPARAM(float,arg5), CALLWRAPPERGETADRESS(string,arg6), CALLWRAPPERGETPARAM(float,arg7), CALLWRAPPERNORET())

CALLWRAPPERARG3(NGUITextDrawer, NTDSetFontColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())
CALLWRAPPERARG3(NGUITextDrawer, NTDSetFontShadowColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())

CALLWRAPPERARG1(NGUITextDrawer, NTDSetFontSize, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERNORET())	

void NGUITextDrawer::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);
	
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void Draw(float x, float y, string&in txt_text, float alpha)", CALLWRAPPERNAMEAS(NTDScriptAddPrintCall));		
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void Draw(float x, float y, float w, float h, string&in txt_text, float alpha)", CALLWRAPPERNAMEAS(NTDScriptAddPrintCall2));		
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void Draw(float x, float y, float w, float h, float font_scale, string&in txt_text, float alpha)", CALLWRAPPERNAMEAS(NTDScriptAddPrintCall3));		

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFontColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NTDSetFontColorScript));			
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFontShadowColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NTDSetFontShadowColorScript));

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetFontSize(int size)", CALLWRAPPERNAMEAS(NTDSetFontSize));		
}

void NGUITextDrawer::NTDScriptAddPrintCall(float set_x, float set_y, std::string& txt_text, float alpha)
{
	NTDScriptAddPrintCall2(set_x, set_y, width, height, txt_text, alpha);	
}

void NGUITextDrawer::NTDScriptAddPrintCall2(float set_x, float set_y, float w, float h, std::string& txt_text, float alpha)
{
	if (!IsVisible()) return;	
	if (!font) return;

	if (parent)
	{
		if (parent->IsAffectChildDrawCall())
		{
			parent->AffectChildDrawCall(set_x, set_y, width, height, alpha);
		}
	}

	fontDesc.color.a = alpha * parent_alpha;
	
	if (fontDesc.use_shadow)
	{
		fontDesc.shd_color.a = fontDesc.color.a;
	}

	float wdth = ConvertHorz(w*scale_w);
	float hgt = ConvertHorz(h*scale_h);

	float dlt_x = 0;
	float dlt_y = 0;

	if (abs_anchor)
	{
		dlt_x = -anchor_x * wdth;
		dlt_y = -anchor_y * hgt;
	}
	else
	{
		dlt_x = -ConvertHorz(anchor_x);
		dlt_y = -ConvertVert(anchor_y);
	}
	

	AddPrintCall(font, ConvertHorz(set_x*scale_w) + scr_delta_anchor_x + dlt_x, ConvertVert(set_y*scale_h) + scr_delta_anchor_y + dlt_y, wdth, hgt, null, fontScale, fontDesc.color, txt_text.c_str());	
}

void NGUITextDrawer::NTDScriptAddPrintCall3(float set_x, float set_y, float w, float h, float font_scale, std::string& txt_text, float alpha)
{
	if (!IsVisible()) return;
	if (!font) return;

	if (parent) 
	{
		if (parent->IsAffectChildDrawCall())
		{
			parent->AffectChildDrawCall(set_x, set_y, width, height, alpha);
		}
	}

	fontDesc.color.a = alpha * parent_alpha;
	
	if (fontDesc.use_shadow)
	{
		fontDesc.shd_color.a = fontDesc.color.a;
	}

	float wdth = ConvertHorz(w*scale_w);
	float hgt = ConvertHorz(h*scale_h);

	float dlt_x = 0;
	float dlt_y = 0;

	if (abs_anchor)
	{
		dlt_x = -anchor_x * wdth;
		dlt_y = -anchor_y * hgt;
	}
	else
	{
		dlt_x = -ConvertHorz(anchor_x);
		dlt_y = -ConvertVert(anchor_y);
	}

	AddPrintCall(font, ConvertHorz(set_x*scale_w) + scr_delta_anchor_x + dlt_x, ConvertVert(set_y*scale_h) + scr_delta_anchor_y + dlt_y, wdth, hgt, null, fontScale * font_scale, fontDesc.color, txt_text.c_str());	
}

void NGUITextDrawer::AddPrintCall(FontInstance* font, float scr_x, float scr_y, float scr_w, float scr_h, std::vector<int>* line_indices, float font_scale, Color color, const char* txt_text)
{	
	if (fontDesc.use_shadow)
	{
		NGUIWidget::AddPrintCall(font, scr_x, scr_y + 1, scr_w, scr_h, null, font_scale, fontDesc.shd_color, txt_text);
		NGUIWidget::AddPrintCall(font, scr_x, scr_y, scr_w, scr_h, null, font_scale, color, txt_text);		
	}
	else
	{
		NGUIWidget::AddPrintCall(font, scr_x, scr_y, scr_w, scr_h, null, font_scale, color, txt_text);
	}
}

void NGUITextDrawer::NTDSetFontColorScript(float r, float g, float b)
{
	fontDesc.color.r = r;
	fontDesc.color.g = g;
	fontDesc.color.b = b;

	CalcColors();
}

void NGUITextDrawer::NTDSetFontShadowColorScript(float r, float g, float b)
{	
	fontDesc.shd_color.r = r;
	fontDesc.shd_color.g = g;
	fontDesc.shd_color.b = b;
}

void NGUITextDrawer::NTDSetFontSize(int size)
{
	if (need_release_font)
	{
		RELEASE(font);
		font = en_core.Font()->CreateFont(fontDesc.name, fontDesc.bold, fontDesc.italic, ConvertVert(size), fontDesc.gap, fontDesc.outline);		
	}	
}

bool NGUITextDrawer::AllowMouseCover()
{
	return false;
}

#ifdef EDITOR
Color NGUITextDrawer::GetDebugColor()
{
	return COLOR_GREEN;	
}
#endif

void NGUITextDrawer::Release()
{	
	if (need_release_font)
	{
		RELEASE(font);
	}
#ifdef EDITOR
	else
	{
		if (font_entity)
		{
			font_entity->DelUseFont(this);
		}
	}
#endif
	
	NGUIWidget::Release();
}