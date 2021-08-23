
#include "Core/Sound/SoundResourceOgg.h"
#include "Core/Sound/SoundOgg.h"
#include "Core/Sound/SoundService.h"
#include "Core/Core.h"

SoundResourceOgg::SoundResourceOgg(const char* fl_name,SoundService* serv, bool is3d) : SoundResource(fl_name, serv, is3d)
{	
	fbuffer = null;
}

bool SoundResourceOgg::Load()
{			
	fbuffer = core->Files()->LoadFile(GetFileName());

	if (!fbuffer) return false;		

	return true;
}

void SoundResourceOgg::Release()
{
	if (fbuffer) fbuffer->Release();
}

ISound* SoundResourceOgg::CreateReference(void* scene, Vector* pos, bool looped, bool autodelete, const char * sourceFile, long line)
{	
	SoundOgg* snd = new SoundOgg();
	snd->Init(scene,this,pos,looped,autodelete, fbuffer->GetBuffer(), fbuffer->GetSize(), sourceFile, line);	

	AddRef(snd);

	return snd;
}

ISoundStream* SoundResourceOgg::CreateReference(void* scene,bool looped, bool autodelete, const char * sourceFile, long line)
{	
	SoundStream* snd = new SoundStream();
	snd->Init(scene,this,looped,autodelete, sourceFile, line);	

	AddRef(snd);

	return snd;
}

int SoundResourceOgg::Decode(char* out_buffer, int buffer_len, OggVorbis_File* vf)
{
	char* buffer = out_buffer;
	size_t left = buffer_len;

#ifdef OPENAL_ON

	while (left != 0)
	{
#ifdef PC
		long decoded = ov_read(vf, buffer, left, NULL, 2, 1, null);
#endif


#ifdef ANDROID
		long decoded = ov_read(vf, buffer, left, NULL);
#endif		

#ifdef IOS
		long decoded = ov_read(vf, buffer, left, NULL);
#endif		
        
		if (decoded > 0)
		{
			buffer += decoded;
			left -= decoded;
		}
		else if (decoded == 0)
		{
			break;
		}
		else
		{
			core->TraceTo("Sound", "Ogg decoding error, will try to resume decoding");
		}
	}
#endif

	return buffer_len - left;
}
