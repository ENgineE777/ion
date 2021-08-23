
#include "NGUIISODrawer.h"

ENTITY(NGUIISODrawer)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUIISODrawer,"NGUIISODrawer","NGUI", "ISO Drawer")

void NGUIISODrawer::RegisterProperties()
{
	NGUIWidget::RegisterProperties();	

	PROP_REG( numTilesW )
	PROP_REG( numTilesH )		

	PROP_REG(tileSizeW)
	PROP_REG(tileSizeH)	
}

void NGUIISODrawer::Init()
{	
	NGUIWidget::Init();												

	tex_cell = null;
	//tex_cell = en_core.Render()->LoadTexture("Games/WildWestGold/textures/cells/cellPath.png", _FL_);

	num_quads_w = 0;
	num_quads_h = 0;
	quad_sz_x = 512.0f; 
	quad_sz_y = 512.0f;
}
	
void NGUIISODrawer::ReInit()
{
	NGUIWidget::ReInit();	

	half_tileSizeW = tileSizeW * 0.5f; 
    half_tileSizeH = tileSizeH * 0.5f;

	cur_tileSizeW = ConvertHorz(tileSizeW);
	cur_tileSizeH = ConvertVert(tileSizeH);
	
	cur_half_tileSizeW = cur_tileSizeW * 0.5f; 
	cur_half_tileSizeH = cur_tileSizeH * 0.5f;	

	num_quads_w = (int)(width / quad_sz_x) + 1;
	num_quads_h = (int)(height / quad_sz_y) + 1;

	quads.resize(num_quads_w * num_quads_h);

	for (int j=0;j<num_quads_h;j++)
	{
		for (int i=0;i<num_quads_w;i++)
		{
			DrawQuad& quad = quads[i + j * num_quads_w];
			
			quad.x1 = i * quad_sz_x;
			quad.y1 = j * quad_sz_y;			
			
			quad.x2 = quad.x1 + quad_sz_x;
			quad.y2 = quad.y1 + quad_sz_y;

			quad.half_w = quad_sz_x * 0.5f;
			quad.half_h = quad_sz_y * 0.5f;

			quad.cx = quad.x1 + quad.half_w;
			quad.cy = quad.y1 + quad.half_h;			
		}
	}	
}

void NGUIISODrawer::RenderWidget()
{	
	Color color = overColor;
	color.a = alpha;

	for (int i=0;i<sprites_def.size();i++)
	{
		sprites_def[i].used = 0;
	}

	if (parent)
	{
		color *= parent->overColor;
		color.a = alpha * parent->alpha;
	}		
	
	cur_tileSizeW = ConvertHorz(tileSizeW);
	cur_tileSizeH = ConvertVert(tileSizeH);
	
	cur_half_tileSizeW = cur_tileSizeW * 0.5f; 
	cur_half_tileSizeH = cur_tileSizeH * 0.5f;		
	
	float clamp_x1 = 0, clamp_y1 = 0;
	float clamp_x2 = NGUIService::instance->cur_rwidth, clamp_y2 = NGUIService::instance->cur_rheight;

	if (scr_x > clamp_x1)
	{
		clamp_x1 = scr_x;
	}

	if (scr_y > clamp_y1)
	{
		clamp_y1 = scr_y;
	}

	if (scr_x + scr_w < clamp_x2)
	{
		clamp_x2 = scr_x + scr_w;
	}

	if (scr_y + scr_h < clamp_y2)
	{
		clamp_y2 = scr_y + scr_h;
	}

	float half_w = (clamp_x2 - clamp_x1) * 0.5f;
	float half_h = (clamp_y2 - clamp_y1) * 0.5f;

	float cx = clamp_x1 + half_w;
	float cy = clamp_y1 + half_h;	

	int draw_count = 0;

	for (int j=0;j<num_quads_h;j++)
	{
		for (int i=0;i<num_quads_w;i++)
		{			
			DrawQuad& quad = quads[i + j * num_quads_w];						

			bool vis = false;

			float x1 = scr_x + ConvertHorz(quad.cx) * cur_scale_w;
			float y1 = scr_y + ConvertVert(quad.cy) * cur_scale_h;						

			if (fabs(x1 - cx) > half_w + ConvertHorz(quad.half_w) * cur_scale_w ||
				fabs(y1 - cy) > half_h + ConvertVert(quad.half_h) * cur_scale_h)
			{
				quad.tmp_sprite_draws.clear();
				continue;
			}

			for (int d=0;d<quad.sprite_draws.size();d++)
			{
				SpriteDraw& spr = quad.sprite_draws[d];

				if (spr.used == 0 ||
					fabs(scr_x + ConvertHorz(spr.cx * cur_scale_w) - cx) > half_w + ConvertHorz(spr.half_w * cur_scale_w) ||
					fabs(scr_y + ConvertVert(spr.cy * cur_scale_h) - cy) > half_h + ConvertVert(spr.half_h * cur_scale_h))
				{
					continue;
				}

				DrawSprite(spr.i, spr.j, spr.index, spr.offset, spr.weight);
				draw_count++;
			}

			for (int d = 0;d<quad.tmp_sprite_draws.size();d++)
			{
				SpriteDraw& spr = quad.tmp_sprite_draws[d];

				if (fabs(scr_x + ConvertHorz(spr.cx * cur_scale_w) - cx) > half_w + ConvertHorz(spr.half_w * cur_scale_w) ||
					fabs(scr_y + ConvertVert(spr.cy * cur_scale_h) - cy) > half_h + ConvertVert(spr.half_h * cur_scale_h))
				{
					continue;
				}

				DrawSprite(spr.i, spr.j, spr.index, spr.offset, spr.weight);
				draw_count++;
			}

			quad.tmp_sprite_draws.clear();
		}
	}

	//en_core.Render()->PrintText(10,10, up_left, COLOR_GREEN, "%i", draw_count);	
	
	if (tex_cell)
	{
		float x,y;

		int from_w = (float)numTilesW * 0.5f - 10;
		int from_h = (float)numTilesH * 0.5f - 10;

		for (int i = from_w; i<from_w + 20;i++)
		{
			for (int j = from_h; j<from_h + 20; j++)
			{			
				GetCurPos(i, j, x, y);			

				AddDrawCallLevel(10000, tex_cell, x + width * 0.5f, y + height * 0.5f - cur_half_tileSizeH, cur_tileSizeW, cur_tileSizeH, color, 0, 0, 1, 1);				
			}
		}
	}

	/*if (en_core.Render()->AllowStreamAtx())
	{
		for (int i=0;i<sprites_def.size();i++)
		{
			SpriteDef& def = sprites_def[i];

			if (def.used == 0)
			{
				RELEASE(def.tex)
			}
		}
	}*/

	/*{
		float x = Convert2AbsHorz(ms_pos_x - scr_x);
		float y = Convert2AbsVert(ms_pos_y - scr_y);

		int i = 0,j = 0;
		GetCell(x,y,i,j);

		DrawSprite(i, j, 0);
	}*/
}

void NGUIISODrawer::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);
					
	/*core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void AddSpriteDef(Vector2 tex_size, Vector2 left_point, Vector2 right_point, string&in tex_name)", asMETHOD(NGUIISODrawer, AddSpriteDefScript), asCALL_THISCALL);		

	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void ClearSprites()", asMETHOD(NGUIISODrawer,ClearSpritesScript), asCALL_THISCALL);		
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "int  AddSprite(float i, float j, int def_index, Vector2 offset, float weight)", asMETHOD(NGUIISODrawer,AddSpriteScript), asCALL_THISCALL);		
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "int  SetSprite(int index, float i, float j, int def_index, Vector2 offset, float weight)", asMETHOD(NGUIISODrawer,SetSpriteScript), asCALL_THISCALL);		
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void DelSprite(int index)", asMETHOD(NGUIISODrawer,DelSpriteScript), asCALL_THISCALL);		
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "void AddTempSprite(float i, float j, int def_index, Vector2 offset, float weight)", asMETHOD(NGUIISODrawer,AddTempSpriteScript), asCALL_THISCALL);		

	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "float PauseTexture(int def_sprite, bool pause)", asMETHOD(NGUIISODrawer,PauseTextureScript), asCALL_THISCALL);		
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "float RestartTexture(int def_sprite)", asMETHOD(NGUIISODrawer,RestartTextureScript), asCALL_THISCALL);	
	core->Script()->Machine()->RegisterObjectMethod(Desc()->GetName(), "float GetTextureDuration(int def_sprite)", asMETHOD(NGUIISODrawer,GetTextureDurationScript), asCALL_THISCALL);	
	*/	
}

void NGUIISODrawer::PauseTextureScript(int def_sprite, bool pause)
{
	if (def_sprite<0 || def_sprite>sprites_def.size()) return;

	if (sprites_def[def_sprite].tex)
	{
		sprites_def[def_sprite].tex->Pause(pause);
	}
}

void NGUIISODrawer::RestartTextureScript(int def_sprite)
{	
	if (def_sprite<0 || def_sprite>sprites_def.size()) return;

	if (sprites_def[def_sprite].tex)
	{
		sprites_def[def_sprite].tex->Restart();
	}
}

float NGUIISODrawer::GetTextureDurationScript(int def_sprite)
{
	if (def_sprite<0 || def_sprite>sprites_def.size()) return - 1.0f;

	if (sprites_def[def_sprite].tex)
	{
		return sprites_def[def_sprite].tex->GetDuration();
	}

	return -1.0f;
}

bool NGUIISODrawer::AllowMouseCover()
{
	return false;	
}

#ifdef EDITOR
Color NGUIISODrawer::GetDebugColor()
{
	return COLOR_GREEN;
}
#endif

void NGUIISODrawer::Release()
{	
	for (int i=0;i<sprites_def.size();i++)
	{
		RELEASE(sprites_def[i].tex)
	}

	NGUIWidget::Release();
}

void NGUIISODrawer::GetCell(float x, float y, int& i, int& j)
{
	float A = (x + tileSizeW*numTilesW*0.5)/ (tileSizeW*0.5f);
	float B = y / (tileSizeH*0.5f);

	float jf = (A - B)/2.f;
	float ifl = B + jf;
	
	j = jf;
	i = ifl;


	ifl = ( x + tileSizeW * numTilesW * 0.5f + (tileSizeW * 0.5f * y) / (tileSizeH * 0.5f) )  / tileSizeW   ;
	jf = (tileSizeH * 0.5f * ifl - y) / (tileSizeH * 0.5f);
	
	j = jf;
	i = ifl;
}

void NGUIISODrawer::AddSpriteDefScript(Vector2 tex_size, Vector2 left_point, Vector2 right_point, string& tex_name)
{
	AddSpriteDef(tex_size, left_point, right_point, tex_name.c_str());
}

void NGUIISODrawer::AddSpriteDef(Vector2 tex_size, Vector2 left_point, Vector2 right_point, const char* tex_name)
{
	sprites_def.push_back(SpriteDef());

	SpriteDef& sprite_def = sprites_def[sprites_def.size()-1];
	
	int cai, caj;
	int cci, ccj;

	sprite_def.left_point = left_point;
	sprite_def.right_point = right_point;
	String::Copy(sprite_def.tex_name, 128, tex_name);
	sprite_def.tex = null;
	sprite_def.used = 0;
	
	float dx,dy;
	GetCell(sprite_def.left_point.x, sprite_def.left_point.y, cai, caj);	
	GetPos(cai, caj, dx, dy);
	dx = sprite_def.left_point.x - dx;
	dy = sprite_def.left_point.y - dy;

	GetCell(sprite_def.right_point.x, sprite_def.right_point.y, cci, ccj);		
	GetPos(cci, ccj, sprite_def.right_point.x, sprite_def.right_point.y);	
	sprite_def.right_point.x += dx;
	sprite_def.right_point.y += dy;

	sprite_def.width = cci - cai + 1;
	sprite_def.lenght = ccj - caj + 1;

	sprite_def.tex_size = tex_size;	
	GetPos(cci, caj, sprite_def.ceneter_pos.x, sprite_def.ceneter_pos.y);	
	sprite_def.ceneter_pos.x += half_tileSizeW + dx;
	sprite_def.ceneter_pos.y += half_tileSizeH + dy;

	if (sprite_def.ceneter_pos.x > sprite_def.tex_size.x)
	{
		sprite_def.ceneter_pos.x = sprite_def.tex_size.x;
	}

	if (sprite_def.ceneter_pos.y > sprite_def.tex_size.y)
	{
		sprite_def.ceneter_pos.y = sprite_def.tex_size.y;
	}

	sprite_def.dh = sprite_def.tex_size.y - sprite_def.ceneter_pos.y;
	sprite_def.right_dw = sprite_def.tex_size.x - sprite_def.ceneter_pos.x - 0.5f * tileSizeW * sprite_def.lenght;

	sprite_def.cache.x = sprite_def.ceneter_pos.x/sprite_def.tex_size.x;
	sprite_def.cache.y = 1.0f/sprite_def.tex_size.x;
	sprite_def.cache.z = (half_tileSizeW)/sprite_def.tex_size.x;
}

void NGUIISODrawer::DrawSprite(float i, float j, int index, Vector2 offset, float weight)
{
	if (index == -1) return;

	float wgt = weight;

	SpriteDef& sprite = sprites_def[index];
			
	if (!sprite.tex)
	{
		if (sprite.tex_name[0])
		{
			sprite.tex = en_core.Render()->LoadTexture(sprite.tex_name, false, _FL_);
		}

		if (sprite.tex)
		{
			sprite.tex->SetAdress(Texture::addressClamp);
		}
		else
		{
			return;
		}
	}

	sprite.used = 1;
	

	float x = 0;
	float y = 0;
	
	GetCurPos(i, j, x, y);

	x += ConvertHorz(offset.x);
	y += ConvertVert(offset.y);

	float half_w = scr_w * 0.5f;
	float half_h = scr_h * 0.5f;	

	float w = ConvertHorz(sprite.ceneter_pos.x);
	float h = ConvertVert(sprite.tex_size.y);	

	if (weight < -0.0001f)
	{
		weight = GetWeight(i, j);
	}

	if (sprite.lenght == 1 && sprite.width == 1)
	{
		float tx_w = ConvertHorz(sprite.tex_size.x);
		float tx_h = ConvertVert(sprite.tex_size.y);	

		AddDrawCallLevel(weight, sprite.tex,
						 (x - w) * cur_scale_w + half_w, (y - h + sprite.dh) * cur_scale_h + half_h,
						 tx_w * cur_scale_w, tx_h * cur_scale_h, COLOR_WHITE, 0, 0, 1.0f, 1.0f);

		return;
	}	

	y -= ConvertVert(sprite.left_point.y);

	AddDrawCallLevel(weight, sprite.tex,
					(x - ConvertHorz(sprite.left_point.x)) * cur_scale_w + half_w, y * cur_scale_h + half_h,
					w * cur_scale_w, ConvertVert(sprite.tex_size.y) * cur_scale_h, COLOR_WHITE, 0, 0, sprite.cache.x, 1.0f);

	float pos_x = sprite.ceneter_pos.x;
		
	x += (sprite.width) * cur_half_tileSizeW;	

	w = cur_half_tileSizeW;

	for (int sub_j = j;sub_j<j+sprite.lenght;sub_j++)
	{						
		/*if (sub_j == j+sprite.lenght)
		{
			w = sprite.right_dw;		
			AddDrawCallLevel(GetWeight(i, sub_j), sprite.tex, x, y, w, h, COLOR_GREEN, rotate, pos_x * sprite.cache.y, 0, sprite.right_dw/sprite.tex_size.y, 1.0f);
		}
		else*/
		{
			if (pos_x * sprite.cache.y + sprite.cache.z > 1.0f) continue;

			if (weight < -0.0001f)
			{
				weight = GetWeight(i, sub_j);
			}

			AddDrawCallLevel(weight, sprite.tex, x * cur_scale_w + half_w, y * cur_scale_h + half_h, w * cur_scale_w, h * cur_scale_h, COLOR_WHITE, pos_x * sprite.cache.y, 0, sprite.cache.z, 1.0f);			
		}
		
		pos_x += half_tileSizeW;

		x += cur_half_tileSizeW;
	}
}

void NGUIISODrawer::ClearSpritesScript()
{
	for (int j=0;j<num_quads_h;j++)
	{
		for (int i=0;i<num_quads_w;i++)
		{			
			DrawQuad& quad = quads[i + j * num_quads_w];		

			quad.sprite_draws.clear();
			quad.tmp_sprite_draws.clear();
		}
	}	
}

int NGUIISODrawer::AddSpriteCall(float i, float j, int def_index, Vector2 offset, float weight, bool is_temp)
{
	float x,y;
	GetPos(i, j, x, y);

	byte quad_x = (width * 0.5f + x) / quad_sz_x;
	byte quad_y = (height * 0.5f + y) / quad_sz_y;

	if (quad_x < 0) quad_x = 0;
	if (quad_x>= num_quads_w) quad_x = num_quads_w - 1;
	
	if (quad_y < 0) quad_y = 0;
	if (quad_y>= num_quads_h) quad_y = num_quads_h - 1;

	DrawQuad& quad = quads[quad_x + quad_y * num_quads_w];	
	
	float x1 = width * 0.5f + x - sprites_def[def_index].left_point.x;
	float y1 = height * 0.5f + y - sprites_def[def_index].left_point.y;

	float x2 = x1 + sprites_def[def_index].tex_size.x;
	float y2 = y1 + sprites_def[def_index].tex_size.y;

	if (quad.x1 > x1) quad.x1 = x1;
	if (quad.y1 > y1) quad.y1 = y1;

	if (quad.x2 < x2) quad.x2 = x2;
	if (quad.y2 < y2) quad.y2 = y2;

	quad.half_w = (quad.x2 - quad.x1) * 0.5f;
	quad.half_h = (quad.y2 - quad.y1) * 0.5f;

	quad.cx = quad.x1 + quad.half_w;
	quad.cy = quad.y1 + quad.half_h;
		
	SpriteDraw* dr = null;
	word draw_index = -1;

	if (is_temp)
	{
		quad.tmp_sprite_draws.push_back(SpriteDraw());
		draw_index = quad.tmp_sprite_draws.size() - 1;
		dr = &quad.tmp_sprite_draws[draw_index];
	}
	else
	{
		for (int l = 0;l<quad.sprite_draws.size();l++)
		{
			if (quad.sprite_draws[l].used == 0)
			{
				dr = &quad.sprite_draws[l];
				draw_index = l;
			}
		}
	
		if (!dr)
		{
			quad.sprite_draws.push_back(SpriteDraw());
			draw_index = quad.sprite_draws.size() - 1;
			dr = &quad.sprite_draws[draw_index];
		}
	}
	
	dr->used = 1;

	dr->half_w = sprites_def[def_index].tex_size.x * 0.5f;
	dr->half_h = sprites_def[def_index].tex_size.y * 0.5f;

	dr->cx = x1 + dr->half_w;
	dr->cy = y1 + dr->half_h;
	dr->weight = weight;	

	dr->i = i;
	dr->j = j;

	dr->offset = offset;
	
	dr->index = def_index;
	
	int out_index = -1;

	if (!is_temp)
	{
		word* ptr_out_index = (word*)&out_index;
		ptr_out_index[0] = quad_x + quad_y * num_quads_w;		
		ptr_out_index[1] = draw_index;
	}
	
	return out_index;
}

int NGUIISODrawer::AddSpriteScript(float i, float j, int def_index, Vector2 offset, float weight)
{	
	if (def_index<0 || def_index>sprites_def.size()) return -1;
	
	return AddSpriteCall(i, j, def_index, offset, weight, false);	
}

int NGUIISODrawer::SetSpriteScript(int index, float i, float j, int def_index, Vector2 offset, float weight)
{	
	if (index == -1 || def_index<0 || def_index>sprites_def.size()) return -1;

	DelSpriteScript(index);

	return AddSpriteCall(i, j, def_index, offset, weight, false);
}

void NGUIISODrawer::DelSpriteScript(int index)
{
	if (index == -1) return;

	word* ptr_out_index = (word*)&index;
	word quad_index = ptr_out_index[0];		
	word draw_index = ptr_out_index[1];

	if (num_quads_w * num_quads_h < quad_index) return;

	quads[quad_index].sprite_draws[draw_index].used = 0;	
}

void NGUIISODrawer::AddTempSpriteScript(float i, float j, int def_index, Vector2 offset, float weight)
{
	if (def_index<0 || def_index>sprites_def.size()) return;

	AddSpriteCall(i, j, def_index, offset, weight, true);
}