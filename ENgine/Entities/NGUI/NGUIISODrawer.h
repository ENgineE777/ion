#pragma once

#include "NGUIWidget.h"

class NGUIISODrawer : public NGUIWidget
{		
public:	

	float half_tileSizeW, half_tileSizeH;	
	float cur_tileSizeW, cur_tileSizeH;
	float cur_half_tileSizeW, cur_half_tileSizeH;							
				
	PROP_START

	INT_PROP(NGUIISODrawer, numTilesW, 20, "Properties", "num_TilesW")
	INT_PROP(NGUIISODrawer, numTilesH, 20, "Properties", "num_TilesH")

	INT_PROP(NGUIISODrawer, tileSizeW, 70, "Properties", "tileWidth")
	INT_PROP(NGUIISODrawer, tileSizeH, 36, "Properties", "tileHeight")	

	struct SpriteDef
	{	
		Vector2 left_point;
		Vector2 right_point;		
		float   width;
		float   lenght;
		float   dh;
		float   right_dw;
		Vector2 tex_size;
		Vector2 ceneter_pos;
		Vector  cache;
		
		char tex_name[128];
		int used;
		TextureInstance* tex;
	};
	
	TextureInstance* tex_cell;
	vector<SpriteDef> sprites_def;

	struct SpriteDraw
	{
		int   used;
		float i,j;
		int index;
		Vector2 offset;

		float cx, cy;
		float half_w, half_h;

		float weight;
		
		SpriteDraw()
		{
			i = j = 0;
			index = -1;
		};
	};

	struct DrawQuad
	{
		float x1, y1;		
		float x2, y2;
		float cx, cy;
		float half_w, half_h;
		vector<SpriteDraw> sprite_draws;
		vector<SpriteDraw> tmp_sprite_draws;
	};	
			
	float quad_sz_x; 
	float quad_sz_y;
	int num_quads_w,num_quads_h;
	vector<DrawQuad> quads;

	ENTITY_CONSTRUCT(NGUIISODrawer,NGUIWidget)
	ENTITY_SCRIPTDEC

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();
	virtual void RenderWidget();	
	virtual void MethodsToScript(const char* script_class_name);		
		
	void PauseTextureScript(int def_sprite, bool pause);
	void RestartTextureScript(int def_sprite);
	float GetTextureDurationScript(int def_sprite);

	virtual bool AllowMouseCover();	

#ifdef EDITOR
	virtual Color GetDebugColor();	
#endif	

	void GetCell(float x, float y, int& i, int& j);
	
	inline void GetPos(float i, float j, float& x, float& y)
	{
		x = -tileSizeW * numTilesW * 0.5f + tileSizeW * 0.5f * (i+j);
		y = tileSizeH * 0.5f * (i-j);
	};

	inline void GetCurPos(float i, float j, float& x, float& y)
	{
		x = -cur_tileSizeW * numTilesW * 0.5f + cur_tileSizeW * 0.5f * (i+j);
		y = cur_tileSizeH * 0.5f * (i-j);
	};

	inline int GetWeight(int ci, int cj)
	{
		return (ci + (numTilesH-1-cj)* numTilesH)*10;
	};

	virtual void Release();

	void AddSpriteDef(Vector2 tex_size, Vector2 left_point, Vector2 right_point, const char* tex_name);
	void AddSpriteDefScript(Vector2 tex_size, Vector2 left_point, Vector2 right_point, string& tex_name);
	void DrawSprite(float i, float j, int index, Vector2 offset, float weight);

	void ClearSpritesScript();	
	int  AddSpriteScript(float i, float j, int def_index, Vector2 offset, float weight);
	int  SetSpriteScript(int index, float i, float j, int def_index, Vector2 offset, float weight);
	void DelSpriteScript(int index);

	void AddTempSpriteScript(float i, float j, int def_index, Vector2 offset, float weight);	

	int  AddSpriteCall(float i, float j, int def_index, Vector2 offset, float weight, bool is_temp);
};

