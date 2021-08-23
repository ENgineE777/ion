
#include "Core/Sound/SoundOgg.h"
#include "Core/Sound/SoundResourceOgg.h"
#include "Core/Core.h"

static size_t read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	SoundOgg::MemoryStream* stream = (SoundOgg::MemoryStream*)datasource;

	size_t length = nmemb;
	if (length > stream->size - stream->offset)
	{
		length = stream->size - stream->offset;
	}

	if (length > 0)
	{
		memcpy(ptr, stream->buffer + stream->offset, length);
		stream->offset += length;
	}
	return length;
}

static int seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	return -1;
}

static int close_func(void *datasource)
{
	return 0;
}

static ov_callbacks cb = {
	read_func,
	seek_func,
	close_func,
	NULL,
};

void SoundOgg::Init(void* scn, SoundResourceOgg* rs, Vector* set_pos, bool set_looped, bool set_autodelete, byte* buffer, int buffer_len, const char* sourceFile, long line)
{	
	strcpy(cppFile,sourceFile);	
	cppLine = line;

	scene = scn;

	res = rs;

	service = res->service;
	
	SetAttributes(0);

	if (set_pos)
	{
		is3D = true;
		SetPosition(*set_pos,0.0f);
	}
	else
	{
		is3D = false;
		SetPosition(0.0f,0.0f);
	}

	AutoDelete(set_autodelete);	
	SetLooped(set_looped);
	
	min_d = 5.0f;
	max_d = 15.0f;
	out_volume = 0;	

	volume = 100;

	stream.offset = 0;
	stream.size = buffer_len;
	stream.buffer = buffer;

#ifdef OPENAL_ON

	if (ov_open_callbacks(&stream, &vf, NULL, 0, cb) != 0)
	{	
		core->TraceTo("Sound", "Failed to open ogg file");		
	}

	int channels = ov_info(&vf, 0)->channels;
	if (channels != 1 && channels != 2)
	{
		core->TraceTo("Sound", "Unsupported ogg file - %i channels. Only mono or stereo supported", channels);		
		return;
	}

	format = (channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16);
	freq = ov_info(&vf, 0)->rate;

	alGenSources(1, &source);

	alGenBuffers(4, buffers);

#endif

	PrepareBuffers();

	playing = false;
}

void SoundOgg::PrepareBuffers()
{
#ifdef OPENAL_ON
	alSourceStop(source);

	int queued;
	alGetSourcei(source,AL_BUFFERS_QUEUED,&queued);

	while (queued--)
	{
		ALuint bufferI;
		alSourceUnqueueBuffers(source,1,&bufferI);
	}

	stream.offset = 0;	

	for (int i=0; i<4; i++)
	{
		int decoded = res->Decode(temp_buffer, 8 * 1024, &vf);

		if (decoded)
		{
			alBufferData(buffers[i], format, temp_buffer, decoded, freq);
			alSourceQueueBuffers(source, 1, &buffers[i]);			
		}
	}
#endif
}

bool SoundOgg::IsPlaying()
{		
	return playing;
}

void SoundOgg::Pause(bool enable)
{	
#ifdef OPENAL_ON
	if (playing != !enable)
	{
		playing = !enable;	
		
		if (playing)
		{
			alSourcePlay(source);
		}
		else
		{
			alSourceStop(source);
		}
	}
#endif
}

void SoundOgg::AutoDelete(bool enable)
{
	autodelete = enable;
}

bool SoundOgg::IsAutoDelete()
{
	return autodelete;
}

void SoundOgg::SetLooped(bool loop)
{
	looped = true;//loop;
}

void SoundOgg::SetAttributes(int set_volume,int freq)
{
	volume = set_volume;
}

void SoundOgg::Set3DAttributes(float min, float max, int outvol)
{
	min_d = min;
	max_d = max;
	out_volume = outvol;
}

void SoundOgg::SetPosition(const Vector& set_pos, const Vector& set_vel)
{	
	pos = set_pos;
	vel = set_vel;	
}	

void SoundOgg::Release()
{	
#ifdef OPENAL_ON
	if (vf.datasource)
	{
		ov_clear(&vf);
	}

	if (buffers[0])
	{
		alDeleteBuffers(4, buffers);
	}

	if (source)
	{
		alDeleteSources(1, &source);
	}

	res->DecRef(this);
	delete this;
#endif
}

bool SoundOgg::IsActive()
{	
	return true;
}

void SoundOgg::CalculateVolume(const Matrix& listiner_tr)
{
#ifdef OPENAL_ON
	int state;

	//if (stream.offset < stream.size)
	{
		ALint processed;

		alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
		//AL_CHECK();

		// unqueue and refill processed buffers
		while (processed-- > 0)
		{
			ALuint buf;

			alSourceUnqueueBuffers(source, 1, &buf);
			//AL_CHECK();

			size_t decoded = res->Decode(temp_buffer, 8 * 1024, &vf);

			if (decoded)
			{
				alBufferData(buf, format, temp_buffer, decoded, freq);
				//AL_CHECK();

				alSourceQueueBuffers(source, 1, &buf);
				//AL_CHECK();
			}
		}

		alGetSourcei(source, AL_SOURCE_STATE, &state);
		//AL_CHECK();

		// if source has been stopped playing, then resume it
		if (state != AL_PLAYING)
		{
			alSourcePlay(source);
		}
		
		if (state != AL_PLAYING)
		{						
			alGetSourcei(source, AL_BUFFERS_QUEUED, &processed);
						
			if (processed)
			{
				alSourcePlay(source);
			}
			else
			{
				if (state == AL_STOPPED)
				{
					if (looped)
					{	
						PrepareBuffers();
						alSourcePlay(source);
					}
					else
					{
						playing = false;
					}					
				}				
			}
		}	
	}
#endif
	/*Vector ps = pos * listiner_tr;

	float k=1.0f;

	if (is3D)
	{
		float r = ps.Length();		

		if (r>max_d)
		{
			k=0.0f;
		}
		else
		if (r>min_d)
		{
			k = (max_d - r)/(max_d-min_d);
		}
	}
	
	ps.Normalize();*/
}