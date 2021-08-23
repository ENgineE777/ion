
#include "NGUIMap.h"

ENTITY(NGUIMap)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS(NGUIMap,"NGUIMap","NGUI", "Map")

void NGUIMap::RegisterProperties()
{
	NGUIWidget::RegisterProperties();		
}

void NGUIMap::Init()
{		
	NGUIWidget::Init();	

	SetUpdate((TaskHolder::Task)&NGUIMap::CheckPress, lvl_pre5 + 3);

	longitude = 51.5007292f;
	latitude = -0.1268141f;	

	offset_x = 0.0f;
	offset_y = 0.0f;
	in_drag = false;
}
	
void NGUIMap::ReInit()
{	    
	//clipChilds = true;	
	in_drag = false;

	NGUIWidget::ReInit();
}

void NGUIMap::ReloadCache()
{
	int map_x = center_tile_x - 1;
	int map_y = center_tile_y - 1;

	for (int i=0; i<3; i++)
	{		
		for (int j=0; j<3; j++)
		{			
			char str[512];			
			
			//sprintf(str, "http://maps.googleapis.com/maps/api/staticmap?center=%4.8f,%4.8f&zoom=%i&scale=false&size=%ix%i&maptype=roadmap&format=jpg&key=AIzaSyDNOGOEkhdi5ItBOoaiPrnxEdMnfh2vh7U", longi, lati, zoom,(int)scr_w, (int)scr_h);
			sprintf(str, "http://tile.openstreetmap.org/%i/%i/%i.png",zoom, map_x + j, map_y + i);
			SetTextureDataURL(&table[i*3 + j], str, -1.0f);			
		}
	}
}

bool NGUIMap::Start()
{	
	zoom = 12;

	int n = pow(2, zoom);
	center_tile_x = n * ((longitude + 180) / 360);
	center_tile_y = n * (1 - (log(tan(latitude * RADIAN) + 1 / sin(latitude * RADIAN)) / PI)) / 2;
	
	//center_tile_x = 2500;
	//center_tile_y = 2049;

	ReloadCache();

	return NGUIWidget::Start();
}

void NGUIMap::CheckPress(float dt, int level)
{
	if (Scene()->IsRunning())
	{		
		if (((mscover_widget == this || mscover_widget_not_covered == this) && !in_drag))
		{
			if (IsActive() && IsVisible() && alpha > 0.995f)
			{
				bool res = false;

				res = core->Controls()->CheckTouch(0, ControlsService::ActivatedExclusive, this);				

				if (res)
				{
					in_drag = true;

					SetFocused(true);						
				}
			}
		}

		if (in_drag)
		{				
			if (core->Controls()->CheckTouch(0, ControlsService::ActiveExclusive, this))
			{
				offset_x += ms_pos_x - ms_prev_pos_x;
				offset_y += ms_pos_y - ms_prev_pos_y;		

				bool need_rebuild = false;

				if (abs(offset_x) > scr_w)
				{					
					for (int i=0; i<3; i++)
					{
						TextureEntry::TextureData data;

						if (offset_x>0)
						{							
							for (int j=0; j<3; j++)
							{			
								int index = j-1;

								if (index < 0)
								{
									index = 2;
								}

								table[i*3 + j].Copy(&data);
								table[i*3 + index].Copy(&table[i*3 + j]);
								data.Copy(&table[i*3 + index]);																
							}
						}
						else
						{			
							for (int j=2; j>=0; j--)
							{			
								int index = j+1;

								if (index > 2)
								{
									index = 0;
								}

								table[i*3 + j].Copy(&data);
								table[i*3 + index].Copy(&table[i*3 + j]);
								data.Copy(&table[i*3 + index]);																
							}							
						}
					}
						
					if (offset_x>0)
					{
						offset_x -= scr_w;
						center_tile_x -= 1;
					}
					else
					{
						offset_x += scr_w;
						center_tile_x += 1;						
					}	

					need_rebuild = true;
				}

				if (abs(offset_y) > scr_h)
				{					
					for (int j=0; j<3; j++)
					{
						TextureEntry::TextureData data;

						if (offset_x>0)
						{							
							for (int i=0; i<3; i++)
							{			
								int index = i-1;

								if (index < 0)
								{
									index = 2;
								}

								table[i*3 + j].Copy(&data);
								table[index*3 + j].Copy(&table[i*3 + j]);
								data.Copy(&table[index*3 + j]);																
							}
						}
						else
						{			
							for (int i=2; i>=0; i--)
							{			
								int index = i+1;

								if (index > 2)
								{
									index = 0;
								}

								table[i*3 + j].Copy(&data);
								table[index*3 + j].Copy(&table[i*3 + j]);
								data.Copy(&table[index*3 + j]);																															
							}							
						}
					}

					if (offset_y>0)
					{
						offset_y -= scr_w;
						center_tile_y -= 1;						
					}
					else
					{
						offset_y += scr_h;
						center_tile_y += 1;
					}	

					need_rebuild = true;
				}

				if (need_rebuild)
				{
					ReloadCache();
				}
			}
			else
			{
				in_drag = false;					
			}			
		}
	}
}

void NGUIMap::RenderWidget()
{
#ifdef EDITOR
	if (!Scene()->IsRunning())
	{
		Color clr = COLOR_GREEN;
		clr.a *= clr.a;
		AddDrawCall(NGUIService::instance->pFilledTexture, 0, 0, scr_w, scr_h, clr,0,0,1,1);
	}
#endif

	float x = offset_x - scr_w;
	float y = offset_y - scr_h;

	float dv1 = 24.5f / (scr_h + 48.0f) * 0;				
	float dv2 = 48.0f / (scr_h + 48.0f) * 0;

	for (int i=0; i<3; i++)
	{
		x = offset_x - scr_w;

		for (int j=0; j<3; j++)
		{
			if (i != 1 || j != 1)
			{
				//continue;
			}

			if (table[i*3 + j].texture)
			{
				table[i*3 + j].texture->SetFilters(Texture::filterPoint, Texture::filterPoint);
			}
			Color clr = COLOR_WHITE;
			
			//clr.g = 0.5f + (i + j) * 0.1f;			

			AddDrawCall(table[i*3 + j].texture, x, y, scr_w, scr_h, clr, 0, dv1, 1.0f, 1.0f - dv2);			
			x += scr_w;
		}

		y += scr_h;
	}
}

void NGUIMap::Release()
{		
	NGUIWidget::Release();
}