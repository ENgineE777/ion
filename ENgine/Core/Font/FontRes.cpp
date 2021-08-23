#include "Core/Core.h"
#include "Common/Utils.h"

//#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
//#include "stb_truetype.h"
//stbtt_fontinfo stb_font;

unsigned FontRes::vTextSeparatorsSize = 10;
char FontRes::vTextSeparators[] = {' ','-','.',',',':',';','_','+','/','\\'};

int GetPow2(int value)
{
	int val = 1;
	for (int i=0;i<15;i++)
	{
		if (value <= val) return val;
		val *= 2;
	}

	return 1024;
}

FontRes::FontRes(FT_Library set_library, const char* fl_name, int hgt, float set_outline_thin) : BaseResource(fl_name)
{
	tex_w = -1;
	tex_h = -1;
	
	library = set_library;
	height = hgt;	
	outline_thin = set_outline_thin;

	tex = null;		

#ifdef PC
	need_update_tex = false;
	tex_buffer = null;
#endif

	font_fb = null;
	face = null;	
}

void FontRes::SetRowGap(float set_gap)
{		
	cur_row_gap = set_gap * height;
}

int FontRes::GenerateCharHeight(wchar_t ch)
{		
	FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT );
	
	FT_Glyph glyph;
	
	FT_Get_Glyph( face->glyph, &glyph );
	
	if (outline_thin > 0.01f)
	{		
		FT_Glyph_StrokeBorder(&glyph, stroker, 0, 0);
	}

	FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
 	
	FT_Bitmap& bitmap=bitmap_glyph->bitmap;
	 		
	FT_Glyph glyph_stroke;	
	FT_BitmapGlyph bitmap_glyph_stroke; 	
	FT_Bitmap bitmap_stroke;

	if (outline_thin > 0.01f)
	{
		FT_Get_Glyph( face->glyph, &glyph_stroke );

		FT_Glyph_Stroke(&glyph_stroke, stroker, 0);
		FT_Glyph_To_Bitmap( &glyph_stroke, ft_render_mode_normal, 0, 1 );

		bitmap_glyph_stroke = (FT_BitmapGlyph)glyph_stroke; 	
		bitmap_stroke = bitmap_glyph_stroke->bitmap;	 		
	}

	int hgt = bitmap_glyph->top;	

    FT_Done_Glyph(glyph);
	
	if (outline_thin > 0.01f) FT_Done_Glyph(glyph_stroke);	

	return hgt;
}

FontRes::Glyph* FontRes::GenerateChar(wchar_t ch)
{	
	glyphs.push_back(Glyph());

	int index = glyphs.size() - 1;
	glyphList.InsertIndex(ch, index);	

	Glyph& set_glyph = glyphs[index];
	
	int fnt_ndex = glyphList.FontIndex( ch );

	if (fnt_ndex == 0)
	{
		core->TraceTo("FontGen", "%i - absent in %s", ch, GetFileName());
	}	

	FT_Load_Glyph( face, fnt_ndex, FT_LOAD_DEFAULT );	
 	
	FT_Glyph glyph;
	
	FT_Get_Glyph( face->glyph, &glyph );	
	
	if (outline_thin > 0.01f)
	{		
		FT_Glyph_StrokeBorder(&glyph, stroker, 0, 0);
	}
	
	FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
 	
	FT_Bitmap& bitmap=bitmap_glyph->bitmap;
	 			
	FT_Glyph glyph_stroke;	
	FT_BitmapGlyph bitmap_glyph_stroke; 	
	FT_Bitmap bitmap_stroke;

	if (outline_thin > 0.01f)
	{
		FT_Get_Glyph( face->glyph, &glyph_stroke );		

		FT_Glyph_Stroke(&glyph_stroke, stroker, 0);
		FT_Glyph_To_Bitmap( &glyph_stroke, ft_render_mode_normal, 0, 1 );

		bitmap_glyph_stroke = (FT_BitmapGlyph)glyph_stroke; 	
		bitmap_stroke = bitmap_glyph_stroke->bitmap;	 		
	}

	set_glyph.width = bitmap.width;
	set_glyph.height = bitmap.rows;
	set_glyph.x_offset = (float)(face->glyph->metrics.horiBearingX) * (1.0f/64.0f);

	set_glyph.x_advance = (float)(face->glyph->metrics.horiAdvance) * (1.0f/64.0f);
				
	set_glyph.y_offset = height - bitmap_glyph->top;	

	if (bitmap.width > 0 && bitmap.rows > 0 && cur_tex_y + bitmap.rows < tex_h && ch>=32)
	{		
		if (cur_tex_x + bitmap.width > 1023)
		{
			cur_tex_x = 1;
			cur_tex_y += cur_tex_row_height + 1;
			cur_tex_row_height = 0;
		}
		
		if (cur_tex_row_height < bitmap.rows) cur_tex_row_height = bitmap.rows;

		if (tex_w != -1)
		{
            
#ifndef PC
            int tx_w = bitmap.width;
            
            if (tx_w%2 == 1)
            {
                tx_w++;
            }
            
			byte* tex_buffer = (byte*)malloc(bitmap.rows * tx_w * 2);
#endif

			for (int j=0; j < bitmap.rows; j++)
			{			
				for (int i=0; i < bitmap.width; i++)
				{							
					if (outline_thin > 0.01f)
					{
						int k = bitmap.buffer[i + bitmap.width * j] - bitmap_stroke.buffer[i + bitmap.width * j];
						if (k<0) k = 0;
										
#ifdef PC
						tex_buffer[(cur_tex_x + i + tex_w * (j + cur_tex_y)) * 2 + 0] = k;
#else
						tex_buffer[(i + tx_w * j) * 2 + 0] = k;
#endif
					}
					else
					{
#ifdef PC
						tex_buffer[(cur_tex_x + i + tex_w * (j + cur_tex_y)) * 2 + 0] = 255;
#else
						tex_buffer[(i + tx_w * j) * 2 + 0] = 255;
#endif                        
					}

#ifdef PC
					tex_buffer[(cur_tex_x + i + tex_w * (j + cur_tex_y)) * 2 + 1] = bitmap.buffer[i + bitmap.width * j];	
#else
                    tex_buffer[(i + tx_w * j) * 2 + 1] = bitmap.buffer[i + bitmap.width * j];
#endif
				}			
			}

#ifndef PC
			tex->UpdateLod(0, 0, cur_tex_x, cur_tex_y, bitmap.width, bitmap.rows, tex_buffer, bitmap.width * bitmap.rows * 2);
			free(tex_buffer);
#endif
             
            
					
			set_glyph.u = (float)(cur_tex_x) / (float)(tex_w);
			set_glyph.v = (float)(cur_tex_y) / (float)(tex_h);

			set_glyph.du = (float)(bitmap.width) / (float)(tex_w);
			set_glyph.dv = (float)(bitmap.rows) / (float)(tex_h);

			set_glyph.skip = 0;		
		}

		cur_tex_x += bitmap.width + 1;
	}
	else
	{
		set_glyph.x_offset = height * 0.45f;
		set_glyph.skip = 1;
	}		

    FT_Done_Glyph(glyph);
	
	if (outline_thin > 0.01f) FT_Done_Glyph(glyph_stroke);	

#ifdef PC
	need_update_tex = true;
#endif

	return &set_glyph;
}

bool FontRes::Load()
{	
	if (!FontService::vbuffer)
	{
		FontService::vbuffer = en_core.Render()->CreateVertexBuffer(6 * 1000 * sizeof(FontService::FontVertex), sizeof(FontService::FontVertex), _FL_);			

		FontService::vbuffer->StartDesc();

		FontService::vbuffer->AddDescElem(VertexBuffer::position,VertexBuffer::float3, "position");		
		FontService::vbuffer->AddDescElem(VertexBuffer::texcoord,VertexBuffer::float2, "texCoord");		

		FontService::vbuffer->EndDesc();	

		FontService::tech_nfont = en_core.Render()->CreateRenderTechnique("NFont");
	}

	RELEASE(tex)

	tex_w = -1;
	tex_h = -1;
	tex = null;
				
	font_fb = core->Files()->LoadFile(GetFileName());
	    
	if (font_fb)
    {			
		if (FT_New_Memory_Face( library, font_fb->GetBuffer(), font_fb->GetSize(), 0, &face))
		{        			
			RELEASE(font_fb)

            return false;
		}		
    }
	else
	{
		return false;
	}	

	glyphList.Init(face);

	FT_Set_Char_Size( face, height << 6, height << 6, 108, 108);	
 
	FT_Select_Charmap( face, FT_ENCODING_UNICODE);	

	if (outline_thin > 0.01f)
	{
		FT_Stroker_New(library, &stroker);
		FT_Stroker_Set(stroker,
					   (int)(outline_thin * 64),
						FT_STROKER_LINECAP_ROUND,
						FT_STROKER_LINEJOIN_ROUND,0);
	}
	
	height = (int)(((float)GenerateCharHeight('A') + (float)GenerateCharHeight('B') + (float)GenerateCharHeight('C')) / 3.0f);		

	int nums = (int)(1024.0f / height);

	tex_w = 1024;
	tex_h = GetPow2((int)((400.0f / nums) * height));	

	if (tex_h > 2048)
	{
		tex_h = 2048;
	}

	cur_tex_x = 1;
	cur_tex_y = 1;
	cur_tex_row_height = 0;						
		
#ifdef PC
	tex_buffer = new byte[tex_w * tex_h * 2];	
    memset(tex_buffer,0,tex_w * tex_h * 2);
#endif		
		
	tex = en_core.Render()->CreateTexture(tex_w, tex_h, 1, Texture::fmt_a8_r8,_FL_);

        
	//tex = en_core.Render()->CreateTexture(tex_w, tex_h, 1, GL_LUMINANCE_ALPHA,_FL_);

	return true;
}

FontInstance* FontRes::CreateReference(float set_gap)
{
	AddRef();

	return new FontInstance(this, set_gap);
}

FontRes::Glyph* FontRes::CheckGlyph(wchar_t characterCode)
{
	int index = glyphList.GlyphListIndex(characterCode);

	if (index == -1)
	{
		return GenerateChar(characterCode);
	}	

	return &glyphs[index];
}

void FontRes::Print(Matrix& transform, ClipRect* rect, ClipRect* clip_rect, std::vector<LineData>& lines_data, float font_scale, Color color, TextAlign align,  const char* text)
{	
	//return;

	if (!tex) return;

	//en_core.Render()->Debug()->DrawSprite(tex, 0.0f, Vector2(tex->GetWidth(), tex->GetHeight()), up_left, COLOR_WHITE, 0.0f);

	int len = 0;
		
	len = strlen(text);	

	if (len == 0) return;

	RenderTechnique* tech = FontService::tech_nfont;
	
	Matrix tmp;
			
	en_core.Render()->SetRenderTechnique(tech);

	Vector4 params[4];

	transform._41 = (int)transform._41;
	transform._42 = (int)transform._42;
	transform._43 = (int)transform._43;

	tmp = transform;

	params[0] = Vector4(en_core.Render()->Display()->GetWidth(),
						en_core.Render()->Display()->GetHeight(),
						0.5f,
						0.0f);

	tech->VS_SetVector4("desc", params[0]);
	tech->VS_SetMatrix("transform", tmp);
	

	if (font_scale > 1.01f)
	{
		tex->SetFilters(Texture::filterLinear, Texture::filterLinear);
	}
	else
	{
		tex->SetFilters(Texture::filterPoint, Texture::filterPoint);
	}	

	en_core.Render()->SetVertexBuffer(0, FontService::vbuffer);				

	tech->PS_SetTexture("diffuseMap", tex);				
	tech->PS_SetVector4("color", (Vector4&)color);			

	float scr_x = 0;

#ifdef PC
	scr_x += 0.5f;
#endif

	float scr_y = 0;

#ifdef PC
	scr_y += 1.0f;
#endif
    
#ifndef PC
    scr_y += 2.5f;
#endif

	int dr_index = 0;		
	
	FontService::FontVertex* v = (FontService::FontVertex*)FontService::vbuffer->Lock();

	int w = 0;
	int bytes = 0;	

	int line = -1;
	int x_offset = 0;

	if (lines_data.size() != 0)
	{
		x_offset = lines_data[0].offset;
		line = 0;
	}	

	int next_white_space = 0;	

	for (int i=0;i<len;i++)
	{			
		if (line != -1)
		{
			if (lines_data[line].index <= i)
			{
				scr_x = 0;

#ifdef PC
				scr_x += 0.5f;
#endif				

				scr_y += (height + cur_row_gap) * font_scale;

				line++;				

				if (lines_data.size() == line)
				{
					line = -1;
					x_offset = 0;
				}
				else
				{
					x_offset = lines_data[line].offset;
				}
			}
		}
		
		if (!BuildUtf16fromUtf8(text[i], bytes, w))
		{
			continue;
		}

		if (w > 65000) continue;					

		if (w == 10)
		{
			continue;
		}
		else
		if (w == '\\')
		{
			if (i <len-1)
			{				
				if (text[i+1] == 'n')
				{
					i++;
					continue;
				}
			}
		}		
		
		Glyph* set_glyph = CheckGlyph(w);
		if (!set_glyph) continue;		
				
		if (set_glyph->skip == 0)
		{						
			float char_x = scr_x + x_offset + set_glyph->x_offset * font_scale;
			float char_y = scr_y + set_glyph->y_offset * font_scale - 0.5f;
			float char_w = set_glyph->width * font_scale;
			float char_h = set_glyph->height * font_scale;

			float char_u = set_glyph->u;
			float char_v = set_glyph->v;

			float char_du = set_glyph->du;		
			float char_dv = set_glyph->dv;

			if (clip_rect)
			{
				if (!ClipSprite(char_x, char_y, char_w, char_h,
								char_u, char_v, char_du, char_dv,
								clip_rect->x1, clip_rect->x2, clip_rect->y1, clip_rect->y2))
				{
					scr_x += set_glyph->x_advance * font_scale;
					continue;
				}
			}

			v[dr_index * 6 + 0].pos = Vector(char_x, char_y + char_h , 0.5f);
			v[dr_index * 6 + 1].pos = Vector(char_x, char_y, 0.5f);
			v[dr_index * 6 + 2].pos = Vector(char_x + char_w,  char_y, 0.5f);

			v[dr_index * 6 + 3].pos = Vector(char_x, char_y + char_h, 0.5f);
			v[dr_index * 6 + 4].pos = Vector(char_x + char_w,  char_y, 0.5f);
			v[dr_index * 6 + 5].pos = Vector(char_x + char_w, char_y + char_h, 0.5f);		
		
			v[dr_index * 6 + 0].uv = Vector2( char_u, char_v + char_dv);
			v[dr_index * 6 + 1].uv = Vector2( char_u, char_v);
			v[dr_index * 6 + 2].uv = Vector2( char_u + char_du, char_v);

			v[dr_index * 6 + 3].uv = Vector2( char_u, char_v + char_dv);
			v[dr_index * 6 + 4].uv = Vector2( char_u + char_du, char_v);
			v[dr_index * 6 + 5].uv = Vector2( char_u + char_du, char_v + char_dv);							
			
			dr_index++;
		}
		
		scr_x += set_glyph->x_advance * font_scale;        
        
		if (dr_index >= 998)
		{
			FontService::vbuffer->Unlock();			

#ifdef PC
			if (need_update_tex) UpdateTexture();
#endif			

			en_core.Render()->DrawPrimitive(RenderService::trianglesList, 0, 2 * dr_index);			

			dr_index = 0;
				
			v = (FontService::FontVertex*)FontService::vbuffer->Lock();
		}		
	}
	
	FontService::vbuffer->Unlock();

	if (dr_index > 0)
	{		
#ifdef PC
		if (need_update_tex) UpdateTexture();
#endif

		en_core.Render()->DrawPrimitive(RenderService::trianglesList, 0, 2 * dr_index);
	}
}

int FontRes::GetHeight()
{
	return height;
}

int FontRes::GetCharHeight()
{
	return height;
}

float FontRes::GetRowGap()
{
	return cur_row_gap;
}

int FontRes::GetWidth(const char* text, int startIndex, bool break_on_whitespace)
{	
	float scr_x = 0;

	int len = 0;
	
	len = strlen(text);	

	int w = 0;
	int bytes = 0;				

	for (int index = startIndex; index < len; index++)
	{		
		if (!BuildUtf16fromUtf8(text[index], bytes, w))
		{
			continue;
		}
			
		if (w > 65000) continue;		
		
		Glyph* set_glyph = CheckGlyph(w);
		if (!set_glyph) continue;
		
		if (w == 10)
		{
			break;
		}
		else
		if (w == '\\')
		{
			if (index <len-1)
			{
				int nex_chr = 0;

				nex_chr = text[index + 1];				

				if (nex_chr == 'n')
				{					
					break;
				}
			}			
		}				

		if (break_on_whitespace && index != startIndex)
		{
			if (IsSeparator(w))
			{				
				break;
			}
		}				

		scr_x += set_glyph->x_advance;
	}

	return scr_x;
}


int FontRes::GetHeight(const char* text, ClipRect* rect, float font_scale, TextAlign align)
{
    static std::vector<LineData> buff;    
    CalcLinesData(text,buff,rect->x2 - rect->x1, font_scale, align);
		
	if (buff.size() == 0)
	{
		return 0;
	}

	return (int)((buff.size() * (height + cur_row_gap) - cur_row_gap) * font_scale);
}

int FontRes::FindNextWhitespace(const char* text, int start_index, float scr_x, float x2, float font_scale)
{	
	int len = (int)strlen(text);	

	int w = 0;
	int bytes = 0;	

	for (int index = start_index; index < len; index++)
	{							
		if (text)
		{
			if (!BuildUtf16fromUtf8(text[index], bytes, w))
			{
				continue;
			}
		}
		else
		{
			w = text[index];
		}

		if (IsSeparator(w))
		{
			return index;		
		}

		/*if (chr == '\\')
		{
			if (index <len-1)
			{				
				if (text && text[index+1] == 'n')
				{
					return index;
				}
			}
		}*/

		if (w > 65000) continue;
				
		Glyph* set_glyph = CheckGlyph(w);
		if (!set_glyph) continue;		

		if (scr_x + set_glyph->x_advance * font_scale > x2)
		{
			return index;
		}
		
		scr_x += set_glyph->x_advance * font_scale;
	}

	return -1;
}

bool  FontRes::IsSeparator(const wchar_t  symbol)
{
    for(unsigned i = 0;i < vTextSeparatorsSize;++i)
        if(symbol == vTextSeparators[i])return true;
    return false;
}

void FontRes::CalcLinesData(const char* text, std::vector<LineData>& lines_data, int width, float font_scale, TextAlign align)
{	
	lines_data.clear();
	
	int len = 0;
		
	len = strlen(text);	

	if (len == 0) return;

	float scr_x = (int)0;

	LineData ldata;

#ifdef PC
	scr_x += 0.5f;
#endif
					
	int next_white_space = 0;

	next_white_space = FindNextWhitespace(text, 0, scr_x, width, font_scale);	
	
	int w = 0;
	int bytes = 0;	

	for (int i=0;i<len;i++)
	{						
		if (!BuildUtf16fromUtf8(text[i], bytes, w))
		{
			continue;
		}
 		
		if (w > 65000) continue;		

		if (next_white_space == i)
		{	
			if (!IsSeparator(w))
			{
				ldata.index = i;
				ldata.offset = 0;

				if (align == text_center)
				{
					ldata.offset = (width - scr_x) * 0.5f;
				}
				else
				if (align == text_right)
				{
					ldata.offset = (width - scr_x - 1.0f);
				}								

				lines_data.push_back(ldata);

				scr_x = (int)0;

#ifdef PC
				scr_x += 0.5f;
#endif								

				next_white_space = FindNextWhitespace(text, i + 1, scr_x, width, font_scale);				
			}
			else
			{
				int wdt = 0;

				wdt = GetWidth(text, i, true);													

				if (scr_x + wdt > width)
				{
					ldata.index = i+1;
					ldata.offset = 0;

					if (align == text_center)
					{
						ldata.offset = (width - scr_x) * 0.5f;
					}
					else
					if (align == text_right)
					{
						ldata.offset = (width - scr_x - 1.0f);
					}								

					lines_data.push_back(ldata);

					scr_x = (int)0;
#ifdef PC
					scr_x += 0.5f;
#endif					
					
					next_white_space = FindNextWhitespace(text, i + 1, scr_x, width, font_scale);

					continue;				
				}

				next_white_space = FindNextWhitespace(text, i + 1, scr_x, width, font_scale);
			}
		}

		bool need_continue = false;

		if (w == 10)
		{
			need_continue = true;
		}
		else
		if (w == '\\')
		{
			if (i <len-1)
			{				
				if (text && text[i+1] == 'n')
				{
					i++;
					need_continue = true;
				}
			}
		}

		if (need_continue)
		{		
			ldata.index = i+1;
			ldata.offset = 0;

			if (align == text_center)
			{
				ldata.offset = (width - scr_x) * 0.5f;
			}
			else
			if (align == text_right)
			{
				ldata.offset = (width - scr_x - 1.0f);
			}								

			lines_data.push_back(ldata);

			scr_x = (int)0;			

#ifdef PC
			scr_x += 0.5f;
#endif			

			next_white_space = FindNextWhitespace(text, i + 1, scr_x, width, font_scale);

			continue;			
		}

		Glyph* set_glyph = CheckGlyph(w);
		if (!set_glyph) continue;						
		
		scr_x += set_glyph->x_advance * font_scale;        
	}

	ldata.index = len + 10;
	ldata.offset = 0;

	if (align == text_center)
	{
		ldata.offset = (width - scr_x) * 0.5f;
	}
	else
	if (align == text_right)
	{
		ldata.offset = (width - scr_x - 1.0f);
	}								

	lines_data.push_back(ldata);
}

int FontRes::GetCharPosition(const char* text, float& pos_x, int width, int start_pos, float cur_pos_x, TextAlign align)
{
	float font_scale = 1.0f;
	
	int len = 0;
		
	len = strlen(text);	

	if (len == 0) return start_pos;

	float scr_x = (int)pos_x;

	float scr_wdt = pos_x + width;

#ifdef PC
	scr_x += 0.5f;
#endif
		
	if (cur_pos_x < scr_x)
	{
		pos_x = scr_x;

		return start_pos;
	}	

	int next_white_space = 0;

	next_white_space = FindNextWhitespace(text, start_pos, scr_x, scr_wdt, font_scale);	
	
	int w = 0;
	int bytes = 0;	

	for (int i=start_pos; i < len; i++)
	{								
		if (!BuildUtf16fromUtf8(text[i], bytes, w))
		{
			continue;
		}
 		
		if (w > 65000) continue;		

		if (next_white_space == i)
		{	
			if (w != ' ')
			{				
				pos_x = scr_x;

				return i;
			}
			else
			{
				int wdt = 0;

				wdt = GetWidth(text, i, true);													

				if (scr_x + wdt > scr_wdt)
				{
					pos_x = scr_x;

					return i;					
				}

				next_white_space = FindNextWhitespace(text, i + 1, scr_x, scr_wdt, font_scale);
			}
		}


		if (w == '\\')
		{
			if (i <len-1)
			{				
				if (text && text[i+1] == 'n')
				{
					i++;
					
					pos_x = scr_x;

					return i;

					//next_white_space = FindNextWhitespace(text, i + 1, scr_x, scr_wdt, font_scale);
					//continue;
				}									
			}			
		}

		Glyph* set_glyph = CheckGlyph(w);
		if (!set_glyph) continue;								
		
		float wdt = set_glyph->x_advance * font_scale;
				
		if (scr_x < cur_pos_x && cur_pos_x < scr_x + wdt)
		{			
			pos_x = scr_x + wdt;

			return i + 1;
		}

		scr_x += wdt;		
	}	
	
	pos_x = scr_x;

	return len;
}

FontRes::Glyph* FontRes::GetCharGlyph(int ch)
{
	return CheckGlyph(ch);
}

void FontRes::Reset()
{	
	RELEASE(tex)
}

void FontRes::Restore()
{
	tex = en_core.Render()->CreateTexture(tex_w, tex_h, 1, Texture::fmt_a8_r8,_FL_);

#ifdef PC
	need_update_tex = true;
#endif
}

#ifdef PC
void FontRes::UpdateTexture()
{
	need_update_tex = false;

	Texture::LockRect rect;

	tex->Lock(0, 0, rect);

	for(int j=0; j < tex_h;j++)
	{
		byte* rows = (byte*)&rect.ptr[j*rect.pitch];
		
		memcpy(rows, &tex_buffer[j * tex_w * 2], tex_w * 2);						
	}

	tex->Unlock(0, 0);
}
#endif

void FontRes::Release()
{	
#ifdef PC
	if (!en_core.IsExited())
#endif
	{	
		if (face)
		{
			FT_Done_Face(face);	
		}
	}

#ifdef PC
	if (tex_buffer)
	{
		free(tex_buffer);
	}
#endif

	BaseResource::Release();	

	RELEASE(font_fb)	
	RELEASE(tex)

	en_core.Font()->DeleteRes(this);

	delete this;
}

