
#include "TimeBomb.h"
//#include "PhysicBox.h"

#include "common/libjpeg/jpeglib.h"
#include "core/Render/Textures/Texture.h"

ENTITY(TimeBomb)
ENTITY_PARAMS(TimeBomb,"TimeBomb","Physics", "Time Bomb")

//Texture* read_JPEG_file (char * filename)
//{
	/*core->Trace("read_JPEG_file in");

	struct jpeg_decompress_struct d1;
	struct jpeg_error_mgr m1;

	d1.err = jpeg_std_error (&m1);
	jpeg_create_decompress(&d1);

#ifdef PC
	FILE *f = fopen(filename,"rb");
#endif	

#ifdef ANDROID
	char str[512];

	strcpy(str, core->Files()->GetBasePath());
	strcat(str, filename);

	FILE *f = fopen(str,"rb");
#endif		

	core->Trace("read_JPEG_file fopen");

	jpeg_stdio_src(&d1, f);
	jpeg_read_header(&d1, TRUE);

	int w,h;
	int n;
	w = d1.image_width;
	h = d1.image_height;
	n = d1.num_components;	

#ifdef PC
	Texture* tex = core->Render()->CreateTexture(w, h, 1, D3DFMT_A8R8G8B8,_FL_);	
#endif
        
#ifdef OPGL
	core->Trace("read_JPEG_file CreateTexture");
	byte* tex_buffer = (byte*)malloc(w * h * 4);
	Texture* tex = core->Render()->CreateTexture(w, h, 1, GL_RGBA,_FL_);			
#endif
	

#ifdef PC
	Texture::LockRect rect;
	tex->Lock(0, 0, rect);
#endif

	int n_s = w * n;
	unsigned char *pBuf;	

	pBuf = (unsigned char *)malloc(n_s * h);
	memset(pBuf, 0, n_s * h);
	int i,j;
	jpeg_start_decompress(&d1);
	i = 0;

	while (d1.output_scanline < d1.output_height)
	{	
#ifdef PC
		byte* rows = (byte*)&rect.ptr[i*rect.pitch];
#endif
    
#ifdef OPGL
		byte* rows = (byte*)&tex_buffer[i * w * 4];
#endif
		i += jpeg_read_scanlines(&d1,(JSAMPARRAY)&(pBuf),1);
							
		for (j = 0; j < w; j++)
		{
			rows[j * 4 + 0] = pBuf[j * n + 0];
			rows[j * 4 + 1] = pBuf[j * n + 1];				
			rows[j * 4 + 2] = pBuf[j * n + 2];				
			rows[j * 4 + 3] = 255;
		}
	}

#ifdef PC
	tex->Unlock(0, 0);
#endif

#ifdef OPGL        
	core->Trace("read_JPEG_file UpdateLod");
    tex->UpdateLod(0, 0, tex_buffer, 1024 * 1024 * 4);
	free(tex_buffer);
#endif
    
	jpeg_finish_decompress(&d1);
	jpeg_destroy_decompress(&d1);

	fclose(f);
	free(pBuf);

	core->Trace("read_JPEG_file finish");*/

  /*
	jpeg_decompress_struct cinfo;
	my_error_mgr jerr;
  	
	//FileBuffer* infile;
	FILE* infile;

	JSAMPARRAY buffer;
	int row_stride;  
	
	if ((infile = fopen(filename, "rb")) == NULL)
	{
		fprintf(stderr, "can't open %s\n", filename);
		return 0;
	}
	
	//infile = core->Files()->LoadFile(filename);

	//if (!infile)
	//{    
		//return 0;
	//}
	
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
  
	if (setjmp(jerr.setjmp_buffer))
	{	
		jpeg_destroy_decompress(&cinfo);

		fclose(infile);
		//infile->Release();
    
		return 0;
	}	
	
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);
	//jpeg_mem_src(&cinfo, infile->GetBuffer(), infile->GetSize());
	
	int res = jpeg_read_header(&cinfo, TRUE);

	
	res = jpeg_start_decompress(&cinfo);

	
  
	Texture* tex = core->Render()->CreateTexture(cinfo.output_width, cinfo.output_height, 1, D3DFMT_A8R8G8B8,_FL_);

	Texture::LockRect rect;

	tex->Lock(0, 0, rect);




	row_stride = cinfo.output_width * cinfo.output_components;

	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	
	while (cinfo.output_scanline < cinfo.output_height)
	{
		

		byte* rows = (byte*)&rect.ptr[cinfo.output_scanline*rect.pitch];

		res = jpeg_read_scanlines(&cinfo, buffer, 1);
		

		
		byte* src = (byte*)buffer;

		for(int i=0; i < cinfo.output_width; i++)
		{							
			rows[i * 4 + 0] = src[i * 3 + 0];
			rows[i * 4 + 1] = src[i * 3 + 1];				
			rows[i * 4 + 2] = src[i * 3 + 2];				
			rows[i * 4 + 3] = 255;				
		}		
	}

	tex->Unlock(0, 0);
	
	(void) jpeg_finish_decompress(&cinfo);
	
	jpeg_destroy_decompress(&cinfo);
	
	fclose(infile);
	//infile->Release();
	*/

//	return tex;
//}

void TimeBomb::Init()
{	
	PROP_REG( power )
	PROP_REG( radius )
	PROP_REG( delayTime )
	PROP_REG( file_name )	

	tex = null;

	SetUpdate((TaskHolder::Task)&TimeBomb::Work, lvl_work1);	
}

void TimeBomb::ReInit()
{
	RELEASE(tex)

	tex = core->Render()->LoadTexture(file_name, _FL_);
}

void TimeBomb::Start()
{
	time_to_blow = delayTime;
	blowed = false;
}

void TimeBomb::Finish()
{
}

void TimeBomb::Work(float dt, int level)
{
	core->Render()->DrawSprite(tex, 10.0f, 300.0f, up_left, COLOR_WHITE, 0.0f);

	if (Scene()->IsRunning())
	{
		if (!blowed)
		{
			time_to_blow -= dt;

			if (time_to_blow<0.0f)
			{
				blowed = true;

				/*SceneInstance::EntityGroup* group = Scene()->GetGroup("PhysBox");

				if (group)
				{
					for (int i=0;i<group->entities.size();i++)
					{
						Matrix tr;
						group->entities[i]->GetTransform(tr);
						Vector dir = tr.Pos() - transform.Pos();

						float dist = dir.Normalize();

						if (dist<radius)
						{
							float pow = 1.0f;
							
							if (dist > radius * 0.5)
							{
								pow = 1.0f - (dist - radius * 0.5) / (radius * 0.5);
							}
							
							((PhysicBox*)group->entities[i])->ApplyImpulse(dir * pow * power);
						}
					}
				}*/
			}

			//core->Render()->DrawCircle(0,transform,radius, 0xff00ff00);
			//core->Render()->DrawCircle(1,transform,radius, 0xff00ff00);
			//core->Render()->DrawCircle(2,transform,radius, 0xff00ff00);
		}

		return;
	}

	//core->Render()->DrawCircle(0,transform,radius, 0xff00ff00);
	//core->Render()->DrawCircle(1,transform,radius, 0xff00ff00);
	//core->Render()->DrawCircle(2,transform,radius, 0xff00ff00);
}

