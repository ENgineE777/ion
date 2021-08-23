
#include "Core/Sound/SoundResourceWav.h"
#include "Core/Sound/SoundService.h"
#include "Core/Core.h"

SoundResourceWav::SoundResourceWav(const char* fl_name,SoundService* serv, bool is3d) : SoundResource(fl_name, serv, is3d)
{	
	ulFrequency = 0;
	ulFormat = 0;
}

bool SoundResourceWav::Load()
{					
	return true;
}

void SoundResourceWav::Release()
{
}

ISound* SoundResourceWav::CreateReference(void* scene, Vector* pos, bool looped, bool autodelete, const char * sourceFile, long line)
{	
	int wave_id = 0;

	if (!wav_loader.OpenWaveFile(GetFileName(), &wave_id))
	{
		wav_loader.GetWaveFrequency(wave_id, &ulFrequency);
		//wav_loader.GetWaveALBufferFormat(wave_id, &alGetEnumValue, &ulFormat);		

		WAVEFORMATEX_LOCAL	wfex;

		wav_loader.GetWaveFormatExHeader(wave_id, &wfex);
		unsigned long ulBufferSize = wfex.nAvgBytesPerSec >> 2;

		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		ulBufferSize -= (ulBufferSize % wfex.nBlockAlign);

		wav_loader.SetWaveDataOffset(wave_id, 0);

		SoundWav* snd = new SoundWav();
		snd->Init(scene,this,pos,looped,autodelete, ulBufferSize, wave_id, sourceFile, line);	

		AddRef(snd);

		return snd;
	}	

	return null;
}

ISoundStream* SoundResourceWav::CreateReference(void* scene,bool looped, bool autodelete, const char * sourceFile, long line)
{	
	SoundStream* snd = new SoundStream();
	snd->Init(scene,this,looped,autodelete, sourceFile, line);	

	AddRef(snd);

	return snd;
}

int SoundResourceWav::Decode(int id, char* out_buffer, int buffer_len)
{
	unsigned long ulBytesWritten;

	wav_loader.ReadWaveData(id, out_buffer, buffer_len, &ulBytesWritten);

	return ulBytesWritten;	
}

void SoundResourceWav::Reset(int id)
{
	wav_loader.SetWaveDataOffset(id, 0);
}
