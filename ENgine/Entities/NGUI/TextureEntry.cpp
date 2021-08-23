
#include "TextureEntry.h"
#include "NGUIWidget.h"

#ifdef EDITOR
#include "SkinWindow.h"

TextureEntry::Slice* TextureEntry::cur_edited_slice = null;

void TextureEntry::StartEditGUITexture(Slice* slice)
{				
	cur_edited_slice = slice;

	SkinWindow::instance->PrepareForUse();
	SkinWindow::instance->SetImage(slice->tex.tex_name);

	SkinWindow::instance->img_wgt->texMode = slice->tex.mode;
	SkinWindow::instance->img_wgt->texFilter = slice->tex.filter;

	SkinWindow::instance->Show(true);

	SkinWindow::instance->img_wgt->cur_type = slice->type;

	SkinWindow::instance->img_wgt->scale_type = 0;
	SkinWindow::instance->img_wgt->vert_align = 0;
	SkinWindow::instance->img_wgt->horz_align = 0;
	SkinWindow::instance->img_wgt->slice_in_ref = false;

	if (slice->type == -1)
	{
		SkinWindow::instance->img_wgt->cur_type = 0;
		return;
	}

	if (slice->type == 0)
	{
		SkinWindow::instance->img_wgt->scale_type = slice->scale_type;

		if (slice->scale_type == 1)
		{
			SkinWindow::instance->img_wgt->vert_align = slice->align_type;
		}
		else
		if (slice->scale_type == 2)
		{
			SkinWindow::instance->img_wgt->horz_align = slice->align_type;
		}
	}
	else
	{
		SkinWindow::instance->img_wgt->slice_in_ref = slice->scale_type;
	}

	int start_rect = 0;
	int end_rect = 1;	
	
	SkinWindow::instance->img_wgt->SelectStartEndRect(start_rect, end_rect);
		
	int index = 0;

	for (int i = start_rect; i<end_rect; i++)
	{
		ImageWidget::Rect& img_rect = SkinWindow::instance->img_wgt->rects[i];
		Rect& rect =slice->rects[index];

		img_rect.x = rect.x;
		img_rect.y = rect.y;
		img_rect.width = rect.width;
		img_rect.height = rect.height;		

		index++;
	}		

	SkinWindow::instance->SetupWidgets();
}

void TextureEntry::StopEditGUITexture()
{
	cur_edited_slice = null;

	if (SkinWindow::instance)
	{		
		SkinWindow::instance->Show(false);
	}
}

void TextureEntry::UpdateWidgets(NGUIWidget* widget)
{
	if (SkinWindow::instance)
	{	
		SkinWindow::instance->Update();
	}

	if (SkinWindow::instance && SkinWindow::instance->wnd->IsVisible())
	{	
		if (!String::IsEqual(SkinWindow::instance->img_wgt->texName, cur_edited_slice->tex.tex_name))
		{			
			if (widget)
			{
				widget->SetTextureData(&cur_edited_slice->tex, SkinWindow::instance->img_wgt->texName);
			}
			else
			{
				RELEASE(cur_edited_slice->tex.texture)
				String::Copy(cur_edited_slice->tex.tex_name, 512, SkinWindow::instance->img_wgt->texName);
				cur_edited_slice->tex.texture = en_core.Render()->LoadTexture(cur_edited_slice->tex.tex_name, false, _FL_);
			}
		}

		cur_edited_slice->tex.mode = SkinWindow::instance->img_wgt->texMode;
		cur_edited_slice->tex.filter = SkinWindow::instance->img_wgt->texFilter;
		cur_edited_slice->width = SkinWindow::instance->img_wgt->img_width;
		cur_edited_slice->height = SkinWindow::instance->img_wgt->img_height;		

		cur_edited_slice->type = SkinWindow::instance->img_wgt->cur_type;

		if (cur_edited_slice->type == 0)
		{
			cur_edited_slice->scale_type = SkinWindow::instance->img_wgt->scale_type;

			if (cur_edited_slice->scale_type == 1)
			{
				cur_edited_slice->align_type = SkinWindow::instance->img_wgt->vert_align;
			}
			else
			if (cur_edited_slice->scale_type == 2)
			{
				cur_edited_slice->align_type = SkinWindow::instance->img_wgt->horz_align;
			}
		}
		else
		{
			cur_edited_slice->scale_type = SkinWindow::instance->img_wgt->slice_in_ref;
		}

		int start_rect = 0;
		int end_rect = 1;	
	
		SkinWindow::instance->img_wgt->SelectStartEndRect(start_rect, end_rect);
		
		int index = 0;

		for (int i = start_rect; i<end_rect; i++)
		{
			ImageWidget::Rect& img_rect = SkinWindow::instance->img_wgt->rects[i];
			Rect& rect = cur_edited_slice->rects[index];

			rect.x = img_rect.x;
			rect.y = img_rect.y;
			rect.width = img_rect.width;
			rect.height = img_rect.height;

			rect.u = (float)rect.x / cur_edited_slice->width;
			rect.v = 1.0f - (float)rect.y / cur_edited_slice->height;

			rect.du = (float)rect.width / cur_edited_slice->width;
			rect.dv = (float)rect.height / cur_edited_slice->height;

			index++;
		}		
	}
}
#endif

void TextureEntry::Load(IStreamLoader* loader, Slice* slice)
{
	if (loader->EnterBlock("slice"))
	{
		loader->Read("width", slice->width);
		loader->Read("height", slice->height);
		loader->Read("type", slice->type);
	
		loader->Read("scale_type", slice->scale_type);			
		loader->Read("align_type", slice->align_type);		
	
		loader->Read("texName", slice->tex.tex_name, 1024);		
		loader->Read("texMode", slice->tex.mode);		
		loader->Read("texFilter", slice->tex.filter);		

		int count = 1;


		if (slice->type == -1)
		{
			count = 0;			
		}
		else
		if (slice->type == 1 || slice->type == 2)
		{
			count = 3;
		}
		else
		if (slice->type == 3)
		{
			count = 9;
		}

		for (int i = 0; i<count; i++)
		{			
			Rect& rect = slice->rects[i];

			loader->EnterBlock("Rect");		

			loader->Read("x", rect.x);
			loader->Read("y", rect.y);
			loader->Read("width", rect.width);
			loader->Read("height", rect.height);
			
			loader->Read("u", rect.u);
			loader->Read("v", rect.v);
			loader->Read("du", rect.du);
			loader->Read("dv", rect.dv);
			
			loader->LeaveBlock();
		}

		loader->LeaveBlock();
	}

	if (slice->type == -1)
	{
		slice->type = 0;
	
		slice->rects[0].u = 0;
		slice->rects[0].v = 0;
		slice->rects[0].du = 1;
		slice->rects[0].dv = 1;	
	}
}

void TextureEntry::Save(IStreamSaver* saver, Slice* slice)
{
	saver->MarkBeginBlock("slice");

	saver->Write("width", slice->width);
	saver->Write("height", slice->height);
	saver->Write("type", slice->type);
	
	saver->Write("scale_type", slice->scale_type);
	saver->Write("align_type", slice->align_type);

	saver->Write("texName", slice->tex.tex_name);
	saver->Write("texMode", slice->tex.mode);
	saver->Write("texFilter", slice->tex.filter);		
	
	int count = 1;

	if (slice->type == -1)
	{
		count = 0;
	}
	else
	if (slice->type == 1 || slice->type == 2)
	{
		count = 3;
	}
	else
	if (slice->type == 3)
	{
		count = 9;
	}

	for (int i = 0; i<count; i++)
	{			
		Rect& rect = slice->rects[i];

		saver->MarkBeginBlock("Rect");		

		saver->Write("x", rect.x);
		saver->Write("y", rect.y);
		saver->Write("width", rect.width);
		saver->Write("height", rect.height);
		
		saver->Write("u", rect.u);
		saver->Write("v", rect.v);
		saver->Write("du", rect.du);
		saver->Write("dv", rect.dv);
			
		saver->MarkEndBlock("Rect");
	}

	saver->MarkEndBlock("slice");
}
bool TextureEntry::HaveDifferences( Slice* slice, Slice* origin_slice)
{
    if (slice->width != origin_slice->width)return true;
    if (slice->height != origin_slice->height)return true;
    if (slice->type != origin_slice->type)return true;
    if (slice->scale_type != origin_slice->scale_type)return true;
    if (slice->align_type != origin_slice->align_type)return true;
    if (!String::IsEqual(slice->tex.tex_name, origin_slice->tex.tex_name))return true;
    if (slice->tex.mode != origin_slice->tex.mode)return true;
    if (slice->tex.filter != origin_slice->tex.filter)return true;
    return false;
}
void TextureEntry::SaveDiffProperties(IStreamSaver* saver, Slice* slice, Slice* origin_slice)
{
    if(HaveDifferences(slice,origin_slice))
    {
        saver->MarkBeginBlock("slice");

        if (slice->width != origin_slice->width)
            saver->Write("width", slice->width);

        if (slice->height != origin_slice->height)
            saver->Write("height", slice->height);

        if (slice->type != origin_slice->type)
            saver->Write("type", slice->type);

        if (slice->scale_type != origin_slice->scale_type)
            saver->Write("scale_type", slice->scale_type);

        if (slice->align_type != origin_slice->align_type)
            saver->Write("align_type", slice->align_type);

        if (!String::IsEqual(slice->tex.tex_name, origin_slice->tex.tex_name))
            saver->Write("texName", slice->tex.tex_name);

        if (slice->tex.mode != origin_slice->tex.mode)
            saver->Write("texMode", slice->tex.mode);

        if (slice->tex.filter != origin_slice->tex.filter)
            saver->Write("texFilter", slice->tex.filter);		

        int count = 0;
        if (slice->type == -1)count = 0;
        else
        {
            if (slice->type == 1 || slice->type == 2) count = 3;
            else
            {
                if (slice->type == 3)count = 9;
            }
        }

        for (int i = 0; i<count; i++)
        {			
            Rect& rect = slice->rects[i];

            saver->MarkBeginBlock("Rect");

            saver->Write("x", rect.x);
            saver->Write("y", rect.y);
            saver->Write("width", rect.width);
            saver->Write("height", rect.height);

            saver->Write("u", rect.u);
            saver->Write("v", rect.v);
            saver->Write("du", rect.du);
            saver->Write("dv", rect.dv);

            saver->MarkEndBlock("Rect");
        }

        saver->MarkEndBlock("slice");
    }

}

void TextureEntry::Copy(Slice* src, Slice* dest)
{
	//if (src->type == -1) return;	

	dest->width = src->width;
	dest->height = src->height;
	dest->type = src->type;
	
	dest->scale_type = src->scale_type;			
	dest->align_type = src->align_type;		

	dest->tex.mode = src->tex.mode;
	dest->tex.filter = src->tex.filter;
	String::Copy(dest->tex.tex_name, 512, src->tex.tex_name);

	dest->tex.texture = en_core.Render()->LoadTexture(dest->tex.tex_name, false, _FL_);
	
	int count = 1;

	if (dest->type == 1 || dest->type == 2)
	{
		count = 3;
	}
	else
	if (dest->type == 3)
	{
		count = 9;
	}

	for (int i = 0; i<count; i++)
	{			
		dest->rects[i] = src->rects[i];		
	}
}

void TextureEntry::Draw(NGUIWidget* widget, float x, float y, float w, float h, Color clr, Slice* slice,bool bTop)
{
	/*int x = set_x;
	int y = set_y;
	int w = set_w;
	int h = set_h;*/
	

	TextureInstance* tex = slice->tex.texture;

	if (!tex) return;
	
	if (slice->tex.mode == 0)
	{
		tex->SetAdress(Texture::addressClamp);				
	}
	else
	if (slice->tex.mode == 1)
	{
		tex->SetAdress(Texture::addressWrap);				
	}
	else
	{
		tex->SetAdress(Texture::addressMirror);				
	}
	
	if (slice->tex.filter == 0)
	{
		tex->SetFilters(Texture::filterPoint, Texture::filterPoint);				
	}
	else
	{
		tex->SetFilters(Texture::filterLinear, Texture::filterLinear);	
	}	

	if (slice->type == -1)
	{
        if(bTop)
		    widget->AddDrawCall(tex, x, y, w, h, clr, 0, 0, 1.0f, 1.0f);
        else
            NGUIService::instance->AddTopSpriteCall(tex, x, y, w, h, clr,widget->global_mat,-1.0f,0, 0, 1.0f, 1.0f);
	}
	else
	if (slice->type == 0)
	{
		Rect& rect = slice->rects[0];

		if (slice->scale_type == 0)
		{
            if(bTop)
			    widget->AddDrawCall(tex, x, y, w, h, clr, rect.u, rect.v, rect.du, rect.dv);
            else
                NGUIService::instance->AddTopSpriteCall(tex, x, y, w, h, clr,widget->global_mat,-1.0f,rect.u, rect.v, rect.du, rect.dv);
		}
		else
		if (slice->scale_type == 1)
		{
			float scr_offset = 0;

			if (slice->align_type == 1)
			{
				scr_offset = (h - rect.height) * 0.5f;
			}
			else
			if (slice->align_type == 2)
			{
				scr_offset = h - rect.height;
			}
            if(bTop)
			    widget->AddDrawCall(tex, x, y + scr_offset, w, rect.height, clr, rect.u, rect.v, rect.du, rect.dv);
            else
                NGUIService::instance->AddTopSpriteCall(tex, x, y, w, h, clr,widget->global_mat,-1.0f,rect.u, rect.v, rect.du, rect.dv);
		}
		else
		if (slice->scale_type == 2)
		{
			float scr_offset = 0;

			if (slice->align_type == 1)
			{
				scr_offset = (w - rect.width) * 0.5f;
			}
			else
			if (slice->align_type == 2)
			{
				scr_offset = w - rect.width;
			}
			if(bTop)	
			    widget->AddDrawCall(tex, x + scr_offset, y, rect.width, h, clr, rect.u, rect.v, rect.du, rect.dv);
            else
                NGUIService::instance->AddTopSpriteCall(tex, x, y, w, h, clr,widget->global_mat,-1.0f,rect.u, rect.v, rect.du, rect.dv);
		}
	}
	else
	if (slice->type == 1)
	{		
		Rect* rect = &slice->rects[0];

		int w1 = (h * rect->width / rect->height + 0.5f);
		int w2 = (h * slice->rects[2].width / slice->rects[2].height + 0.5f);
        if(bTop)
		    widget->AddDrawCall(tex, x, y, w1, h, clr, rect->u, rect->v, rect->du, rect->dv);
        else
            NGUIService::instance->AddTopSpriteCall(tex, x, y, w1, h, clr,widget->global_mat,-1.0f,rect->u, rect->v, rect->du, rect->dv);
		rect = &slice->rects[1];
        if(bTop)
		    widget->AddDrawCall(tex, x + w1, y, w - (w1 + w2), h, clr, rect->u, rect->v, rect->du, rect->dv);
        else
            NGUIService::instance->AddTopSpriteCall(tex, x + w1, y, w - (w1 + w2), h, clr,widget->global_mat,-1.0f,rect->u, rect->v, rect->du, rect->dv);

		rect = &slice->rects[2];
        if(bTop)
		    widget->AddDrawCall(tex, x + w - w2, y, w2, h, clr, rect->u, rect->v, rect->du, rect->dv);
        else
            NGUIService::instance->AddTopSpriteCall(tex, x + w - w2, y,w2, h, clr,widget->global_mat,-1.0f,rect->u, rect->v, rect->du, rect->dv);
	}
	else
	if (slice->type == 2)
	{
		Rect* rect = &slice->rects[0];

		int h1 = (w * rect->height / rect->width + 0.5f);
		int h2 = (w * slice->rects[2].height / slice->rects[2].width + 0.5f);
        if(bTop)
		    widget->AddDrawCall(tex, x, y, w, h1, clr, rect->u, rect->v, rect->du, rect->dv);
        else
            NGUIService::instance->AddTopSpriteCall(tex, x, y,w, h1, clr,widget->global_mat,-1.0f,rect->u, rect->v, rect->du, rect->dv);

		rect = &slice->rects[1];
        if(bTop)
    		widget->AddDrawCall(tex, x, y + h1, w, h - h1 - h2, clr, rect->u, rect->v, rect->du, rect->dv);
        else
            NGUIService::instance->AddTopSpriteCall(tex, x, y + h1,w, h - h1 - h2, clr,widget->global_mat,-1.0f,rect->u, rect->v, rect->du, rect->dv);

		rect = &slice->rects[2];
        if(bTop)
    		widget->AddDrawCall(tex, x, y + h - h2, w, h2, clr, rect->u, rect->v, rect->du, rect->dv);
        else
            NGUIService::instance->AddTopSpriteCall(tex, x, y + h - h2,w, h2, clr,widget->global_mat,-1.0f,rect->u, rect->v, rect->du, rect->dv);
	}
	else
	if (slice->type == 3)
	{
		float h_mul = 1.0f;
		float v_mul = 1.0f;

		if (slice->scale_type)
		{
			h_mul = NGUIService::instance->h_mul_RefToScreen;
			v_mul = NGUIService::instance->v_mul_RefToScreen;
		}

		if (slice->hack_scale == 1)
		{
			h_mul = en_core.Render()->Display()->GetScreenDPI() / 449.0f;

			v_mul = h_mul;
		}

		Rect* rect = &slice->rects[0];
        if(bTop)
		    widget->AddDrawCall(tex, x, y,
								 rect->width * h_mul, rect->height * v_mul,
								 clr, rect->u, rect->v, rect->du, rect->dv);
        else
            NGUIService::instance->AddTopSpriteCall(tex, x, y,
                                                    rect->width * h_mul, rect->height * v_mul,
                                                    clr,widget->global_mat,-1.0f,rect->u, rect->v, rect->du, rect->dv);

		rect = &slice->rects[1];
        if(bTop)
		    widget->AddDrawCall(tex, x + slice->rects[0].width * h_mul, y,
								 w - slice->rects[0].width * h_mul - slice->rects[2].width * h_mul, rect->height * v_mul,
								 clr, rect->u, rect->v, rect->du, rect->dv);
        else
		    NGUIService::instance->AddTopSpriteCall(tex, x + slice->rects[0].width * h_mul, y,
							                    	 w - slice->rects[0].width * h_mul - slice->rects[2].width * h_mul, rect->height * v_mul,
					                    			 clr,widget->global_mat,-1.0f, rect->u, rect->v, rect->du, rect->dv);


		rect = &slice->rects[2];
        if(bTop)
		    widget->AddDrawCall(tex, x + w - rect->width * h_mul, y,
								 rect->width * h_mul, rect->height * v_mul,
								 clr, rect->u, rect->v, rect->du, rect->dv);
        else
		    NGUIService::instance->AddTopSpriteCall(tex, x + w - rect->width * h_mul, y,
							                        rect->width * h_mul, rect->height * v_mul,
								                    clr,widget->global_mat,-1.0f, rect->u, rect->v, rect->du, rect->dv);

		rect = &slice->rects[3];
        if(bTop)
		    widget->AddDrawCall(tex, x, y + slice->rects[0].height * v_mul,
								 rect->width * h_mul, h - slice->rects[0].height * v_mul - slice->rects[6].height * v_mul,
								 clr, rect->u, rect->v, rect->du, rect->dv);
        else
		    NGUIService::instance->AddTopSpriteCall(tex, x, y + slice->rects[0].height * v_mul,
								                    rect->width * h_mul, h - slice->rects[0].height * v_mul - slice->rects[6].height * v_mul,
								                    clr,widget->global_mat,-1.0f, rect->u, rect->v, rect->du, rect->dv);

		rect = &slice->rects[4];
        if(bTop)
		    widget->AddDrawCall(tex, x + slice->rects[0].width * h_mul, y + slice->rects[0].height * v_mul,
								 w - slice->rects[0].width * h_mul - slice->rects[2].width * h_mul , h - slice->rects[0].height * v_mul- slice->rects[6].height * v_mul,
								 clr, rect->u, rect->v, rect->du, rect->dv);
        else
		    NGUIService::instance->AddTopSpriteCall(tex, x + slice->rects[0].width * h_mul, y + slice->rects[0].height * v_mul,
								                    w - slice->rects[0].width * h_mul - slice->rects[2].width * h_mul , h - slice->rects[0].height * v_mul- slice->rects[6].height * v_mul,
								                    clr,widget->global_mat,-1.0f, rect->u, rect->v, rect->du, rect->dv);

		rect = &slice->rects[5];
        if(bTop)
		    widget->AddDrawCall(tex, x + w - rect->width * h_mul, y + slice->rects[0].height * v_mul,
								 rect->width * h_mul, h - slice->rects[0].height * v_mul - slice->rects[6].height * v_mul,
								 clr, rect->u, rect->v, rect->du, rect->dv);
        else
		    NGUIService::instance->AddTopSpriteCall(tex, x + w - rect->width * h_mul, y + slice->rects[0].height * v_mul,
								                    rect->width * h_mul, h - slice->rects[0].height * v_mul - slice->rects[6].height * v_mul,
								                    clr,widget->global_mat,-1.0f, rect->u, rect->v, rect->du, rect->dv);

		rect = &slice->rects[6];
        if(bTop)
		    widget->AddDrawCall(tex, x, y + h - rect->height * v_mul,
								 rect->width * h_mul, rect->height * v_mul,
								 clr, rect->u, rect->v, rect->du, rect->dv);
        else
		    NGUIService::instance->AddTopSpriteCall(tex, x, y + h - rect->height * v_mul,
								                    rect->width * h_mul, rect->height * v_mul,
								                    clr,widget->global_mat,-1.0f, rect->u, rect->v, rect->du, rect->dv);

		rect = &slice->rects[7];
        if(bTop)
    		widget->AddDrawCall(tex, x + slice->rects[0].width * h_mul, y + h - rect->height * v_mul,
								 w - slice->rects[0].width * h_mul - slice->rects[2].width * h_mul, rect->height * v_mul,
								 clr, rect->u, rect->v, rect->du, rect->dv);
        else
    		NGUIService::instance->AddTopSpriteCall(tex, x + slice->rects[0].width * h_mul, y + h - rect->height * v_mul,
								                    w - slice->rects[0].width * h_mul - slice->rects[2].width * h_mul, rect->height * v_mul,
								                    clr,widget->global_mat,-1.0f, rect->u, rect->v, rect->du, rect->dv);

		rect = &slice->rects[8];
        if(bTop)
    		widget->AddDrawCall(tex, x + w - rect->width * h_mul, y + h - rect->height * v_mul,
								 rect->width * h_mul, rect->height * v_mul,
								 clr, rect->u, rect->v, rect->du, rect->dv);
        else
    		NGUIService::instance->AddTopSpriteCall(tex, x + w - rect->width * h_mul, y + h - rect->height * v_mul,
								                    rect->width * h_mul, rect->height * v_mul,
								                    clr,widget->global_mat,-1.0f, rect->u, rect->v, rect->du, rect->dv);

	}
}



