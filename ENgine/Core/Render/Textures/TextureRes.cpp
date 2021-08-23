
#include "TextureRes.h"
#include "TextureInstance.h"
#include "Texture.h"
#include "Core/Core.h"
#include "VideoTextureInstance.h"

TextureRes::TextureRes()
{
	ref_count = 0;
	total_time = 0.0f;
	def_looped = true;
	need_stream = false;
	type = eTexture2d;
}

TextureRes::~TextureRes()
{
}

void TextureRes::ReCreate()
{
	for (int i=0;i<frames.size();i++)
	{
		//frames[i].texture->Load();		
	}
}

bool TextureRes::Load(const char* name, bool async, const char* file, int line)
{
	char ext[16];
	String::ExtractExctention(name, ext, 15);
	String::Copy(path.data, 256, name);
	
	const char* formats = ".3gp,.mp4,.m4a,.aac,.ts,.flac,.mp3,.mid,.xmf,.mxmf,"
				".rtttl,.rtx,.ota,.imy,.ogg,.mkv,.wav,.webm";

	if (String::IsEqual(ext, "atx"))
	{
		async = false;

		IFileBuffer* fl = core->Files()->LoadFile(name);

		if (fl)
		{
			char path[256];
			String::ExtractPath(name, path, true);

			int len = fl->GetSize();
			byte* ptr = fl->GetBuffer(); 

			int index = 0;

			bool first_line = true;			

			while (index<len)
			{
				int sub_index = 0;
				char frame[128];

				while (ptr[index] != ' ')
				{
					frame[sub_index] = ptr[index];
					sub_index++;
					index++;
				}

				frame[sub_index] = 0;
				index++;

				sub_index = 0;
				char time[128];

				while (ptr[index] != '|')
				{
					time[sub_index] = ptr[index];
					sub_index++;
					index++;
				}

				time[sub_index] = 0;
				index++;
				
				float tm  = atof(time);

				char file_name[512];
				String::Copy(file_name, 511, path);
				String::Cat(file_name, 511, frame);

				if (first_line)
				{
					if (String::IsEqual(frame, "NeedStream"))
					{
						need_stream = (tm > 0.99f);// && en_core.Render()->AllowStreamAtx();		
					}
					else
					if (String::IsEqual(frame, "Looped"))
					{
						def_looped = (tm > 0.99f);						
					}
					else
					{
						if (need_stream)
						{
							TextureFrame frame;
							frame.time = total_time + tm;
							frame.texture = null;
							String::Copy(frame.name, 256, file_name);
	                        
							frames.push_back(frame);
                        
							total_time += tm;
						}
						else
						{							
							LoadFrame(file_name, 0.0f, async, file, line);
						}

						first_line = false;
					}
				}
				else
				{
					if (need_stream)
					{
						TextureFrame frame;
						frame.time = total_time + tm;
						frame.texture = null;
						String::Copy(frame.name, 256, file_name);
                    
						frames.push_back(frame);
	                    
						total_time += tm;
					}
					else
					{    
						LoadFrame(file_name, tm, async, file, line);
					}
				}				

				index+=2;
			}

			fl->Release();

			return true;
		}		
			
		return false;
	}
	else
    if (String::IsSubstr(formats, ext))
	{
		type = eVideo;
		return true;
	}

	return LoadFrame(name, 0.0f, async, file, line);
}

bool TextureRes::LoadFrame(const char* name, float time, bool async, const char* file, int line)
{
	RenderService::TextureData texture_data;
	
	String::Copy(texture_data.fl_name, 512, name);

	Texture* texture = null;

	if (texture_data.GetInfo())
	{
		texture = en_core.Render()->CreateTexture(texture_data.width, texture_data.height, 1, Texture::fmt_a8r8g8b8,_FL_);	

		if (!texture->Load(name, async))
		{
			RELEASE(texture)
		}
	}
    
	if (texture)
	{
		TextureFrame frame;
		frame.time = total_time + time;
		frame.texture = texture;
		
		frames.push_back(frame);

		total_time += time;
	
		return true;
	}

	return false;
}

TextureInstance* TextureRes::CreateReference(const char* file, int line)
{
	AddRef();
	
	TextureInstance* tex = null;
	if (type == eTexture2d)
	{
		tex = new TextureInstance(this);		

		tex->SetLooped(def_looped);
		
		if (frames.size()>1)
		{
			tex->RegisterWork();
		}
	}
	else
	if (type == eVideo)
	{
		if (VideoTextureInstance::instCounter < 2)
		{
			tex = new VideoTextureInstance(this);
		}
		else 
			core->TraceTo("Render", "TextureRes::CreateReference VideoTextureInstance::instCounter is limited to 2" );
		
	}
	return tex;
}

void TextureRes::AddRef()
{
	ref_count++;
}

void TextureRes::DecRef()
{
	ref_count--;

	if (ref_count == 0)
	{
		Release();
	}
}

void TextureRes::Reset()
{
#ifdef IOS
    for (int i=0;i<frames.size();i++)
	{
        if (frames[i].texture)
        {

            //((TextureGLES*)frames[i].texture)->Reset();
        }
	}
#endif
}

void TextureRes::Restore()
{
    for (int i=0;i<frames.size();i++)
	{
        if (frames[i].texture)
        {
            //frames[i].texture->Load();
        }
	}
}

void TextureRes::Release()
{	
	for (int i=0; i<(int)frames.size(); i++)
	{
		RELEASE(frames[i].texture)
	}

	frames.clear();	
	
	en_core.Render()->DeleteRes(this);

	delete this;
}
