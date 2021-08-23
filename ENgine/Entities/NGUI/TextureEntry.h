#pragma once
#include "Core/Core.h"

class NGUIWidget;

class TextureEntry
{		
public:
			
	struct TextureData
	{
		int   tex_loading;
		float fade_in_time;
		TextureInstance* texture;
        char  tex_name[1024];
		int   mode;
		int   filter;
		void* net_req;		
		NGUIWidget* wgt;

		TextureData()
		{
			tex_loading = 0;
			texture = null;
            tex_name[0] = 0;
			fade_in_time = -1.0f;
			mode = 0;
			filter = 1;
			net_req = null;
			wgt = null;
		}

		void Copy(TextureData* data)
		{
			data->tex_loading = tex_loading;
			data->texture = texture;
			String::Copy(data->tex_name, 1024, tex_name);
			data->fade_in_time = fade_in_time;
			data->mode = mode;
			data->filter = filter;
			data->net_req = net_req;
			data->wgt = wgt;
		}
	};	

	struct Rect
	{
		int x,y;
		int width, height;
		float u, v;
		float du, dv;
	};
		
	struct Slice
	{
		int type;
		int scale_type;
		int align_type;
		int width, height;
		int tileType;
		Rect rects[9];						
		Rect mask_rect;
		TextureData tex;
		int hack_scale;

		Slice()
		{
			type = -1;
			scale_type = 0;
			align_type = 0;
			width = height = 0;
			tileType = 0;		

			hack_scale = -1;

			mask_rect.u = 0.0f;
			mask_rect.v = 0.0f;

			mask_rect.du = 1.0f;
			mask_rect.dv = 1.0f;
		};

		bool IsEqual(const Slice& right)
		{
			if (type != right.type) return false;
			if (scale_type != right.scale_type) return false;
			if (align_type != right.align_type) return false;
			if (width != right.width) return false;
			if (height != right.height) return false;
			if (tileType != right.tileType) return false;
			if (fabs(tex.fade_in_time - right.tex.fade_in_time) > 0.01f) return false;
			if (tex.mode != right.tex.mode) return false;
			if (tex.filter != right.tex.filter) return false;			
			if (!String::IsEqual(tex.tex_name, right.tex.tex_name)) return false;

			int count = 1;

			if (type == -1)
			{
				count = 0;			
			}
			else
			if (type == 1 || type == 2)
			{
				count = 3;
			}
			else
			if (type == 3)
			{
				count = 9;
			}

			for (int i = 0; i<count; i++)
			{			
				Rect& rect = rects[i];
				const Rect& rect2 = right.rects[i];

				if (rect.x != rect2.x) return false;
				if (rect.y != rect2.y) return false;
				if (rect.width != rect2.width) return false;
				if (rect.height != rect2.height) return false;
				if (fabs(rect.u - rect2.u) > 0.01f) return false;
				if (fabs(rect.v - rect2.v) > 0.01f) return false;
				if (fabs(rect.du - rect2.du) > 0.01f) return false;
				if (fabs(rect.dv - rect2.dv) > 0.01f) return false;
			}			

			if (fabs(mask_rect.u - right.mask_rect.u) > 0.01f) return false;
			if (fabs(mask_rect.v - right.mask_rect.v) > 0.01f) return false;
			if (fabs(mask_rect.du - right.mask_rect.du) > 0.01f) return false;
			if (fabs(mask_rect.dv - right.mask_rect.dv) > 0.01f) return false;

			return true;
		}
	};	

	struct DrawRect
	{		
		float u, v;
		float du, dv;
	};

	struct DrawSlice
	{
		int num_rects;
		float x,y;
		DrawRect rects[9];		

		TextureInstance* texture;

		DrawSlice()
		{
			num_rects = 0;
			x = 0;
			y = 0;			
		};
	};			

	static Slice* cur_edited_slice;

#ifdef EDITOR
	static void StartEditGUITexture(Slice* slice);		
	static void StopEditGUITexture();
	static void UpdateWidgets(NGUIWidget* widget);
#endif

	static void Load(IStreamLoader* loader, Slice* slice);
	static void Save(IStreamSaver* saver, Slice* slice);
	static void SaveDiffProperties(IStreamSaver* saver, Slice* slice, Slice* origin_slice);	
    static bool HaveDifferences( Slice* slice, Slice* origin_slice);
	static void Copy(Slice* src, Slice* dest);
	static void Draw(NGUIWidget* widget, float x, float y, float w, float h, Color clr, Slice* slice,bool bTop = true);
};
