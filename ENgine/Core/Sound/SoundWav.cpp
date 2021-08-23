
#include "Core/Sound/SoundWav.h"
#include "Core/Sound/SoundResourceWav.h"
#include "Core/Core.h"

void SoundWav::Init(void* scn, SoundResourceWav* rs, Vector* set_pos, bool set_looped, bool set_autodelete, int set_buffer_size, int id, const char* sourceFile, long line)
{	
	buffer_size = MathMin(32 * 1024, set_buffer_size);
	wave_id = id;

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

	
	//alGenSources(1, &source);

	//alGenBuffers(4, buffers);

	playing = false;
}

void SoundWav::PrepareBuffers()
{
	//alSourceStop(source);

	int queued;
	//alGetSourcei(source,AL_BUFFERS_QUEUED,&queued);

	while (queued--)
	{
		ALuint bufferI;
		//alSourceUnqueueBuffers(source,1,&bufferI);
	}

	res->Reset(wave_id);

	for (int i=0; i<4; i++)
	{
		int decoded = res->Decode(wave_id, buffer, buffer_size);

		if (decoded)
		{
			//alBufferData(buffers[i], res->ulFormat, buffer, decoded, res->ulFrequency);
			//alSourceQueueBuffers(source, 1, &buffers[i]);			
		}
	}
}

bool SoundWav::IsPlaying()
{		
	return playing;
}

void SoundWav::Pause(bool enable)
{	
	if (playing != !enable)
	{
		playing = !enable;	
		
		if (playing)
		{
			PrepareBuffers();
			//alSourcePlay(source);
		}
		else
		{
			//alSourceStop(source);
		}
	}
}

void SoundWav::AutoDelete(bool enable)
{
	autodelete = enable;
}

bool SoundWav::IsAutoDelete()
{
	return autodelete;
}

void SoundWav::SetLooped(bool loop)
{
	looped = loop;
}

void SoundWav::SetAttributes(int set_volume,int freq)
{
	volume = set_volume;
}

void SoundWav::Set3DAttributes(float min, float max, int outvol)
{
	min_d = min;
	max_d = max;
	out_volume = outvol;
}

void SoundWav::SetPosition(const Vector& set_pos, const Vector& set_vel)
{	
	pos = set_pos;
	vel = set_vel;	
}	

void SoundWav::Release()
{	
	if (buffers[0])
	{
		//alDeleteBuffers(4, buffers);
	}

	if (source)
	{
		//alDeleteSources(1, &source);
	}

	res->DecRef(this);
	delete this;
}

bool SoundWav::IsActive()
{	
	return true;
}

void SoundWav::CalculateVolume(const Matrix& listiner_tr)
{
	if (!playing) return;

	int state;

	//if (stream.offset < stream.size)
	{
		ALint processed = 0;

		//alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
		//AL_CHECK();

		// unqueue and refill processed buffers
		while (processed-- > 0)
		{
			ALuint buf = 0;

			//alSourceUnqueueBuffers(source, 1, &buf);
			//AL_CHECK();

			size_t decoded = res->Decode(wave_id, buffer, buffer_size);

			if (decoded)
			{
				//alBufferData(buf, res->ulFormat, buffer, decoded, res->ulFrequency);
				//AL_CHECK();

				//alSourceQueueBuffers(source, 1, &buf);
				//AL_CHECK();
			}
		}

		//alGetSourcei(source, AL_SOURCE_STATE, &state);
		//AL_CHECK();

		if (state != AL_PLAYING)
		{						
			//alGetSourcei(source, AL_BUFFERS_QUEUED, &processed);
						
			if (processed)
			{
				//alSourcePlay(source);
			}
			else
			{
				if (state == AL_STOPPED)
				{
					if (looped)
					{	
						PrepareBuffers();
						//alSourcePlay(source);
					}
					else
					{
						playing = false;
					}					
				}				
			}
		}		
	}

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