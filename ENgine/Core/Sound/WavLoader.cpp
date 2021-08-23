
#include "WavLoader.h"
#include "Core/Core.h"

typedef long HRESULT;
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)

#define WAVE_FORMAT_PCM     1

#define  WAVE_FORMAT_EXTENSIBLE                 0xFFFE

#pragma pack(push, 4)

struct WAVEFILEHEADER
{
	char			szRIFF[4];
	unsigned long	ulRIFFSize;
	char			szWAVE[4];
};

struct RIFFCHUNK
{
	char			szChunkName[4];
	unsigned long	ulChunkSize;
};

struct WAVEFMT
{
	unsigned short	usFormatTag;
	unsigned short	usChannels;
	unsigned long	ulSamplesPerSec;
	unsigned long	ulAvgBytesPerSec;
	unsigned short	usBlockAlign;
	unsigned short	usBitsPerSample;
	unsigned short	usSize;
	unsigned short  usReserved;
	unsigned long	ulChannelMask;
    GUID_LOCAL      guidSubFormat;
};

struct WAVEFORMAT_LOCAL
{
    word    wFormatTag;        /* format type */
    word    nChannels;         /* number of channels (i.e. mono, stereo, etc.) */
    dword   nSamplesPerSec;    /* sample rate */
    dword   nAvgBytesPerSec;   /* for buffer estimation */
    word    nBlockAlign;       /* block size of data */
};

struct PCMWAVEFORMAT_LOCAL
{
    WAVEFORMAT_LOCAL wf;
    word             wBitsPerSample;
};

#pragma pack(pop)

#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_LOW_FREQUENCY           0x8
#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_SIDE_LEFT               0x200
#define SPEAKER_SIDE_RIGHT              0x400
#define SPEAKER_TOP_CENTER              0x800
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_TOP_BACK_LEFT           0x8000
#define SPEAKER_TOP_BACK_CENTER         0x10000
#define SPEAKER_TOP_BACK_RIGHT          0x20000

#if _MSC_VER <= 1310 

int strncpy_s( char *strDest, size_t numberOfElements, const char *strSource, size_t count )
{
    strncpy( strDest, strSource, count );

    string STR;
    
    
    return 0;
}

#endif

WavLoader::WavLoader()
{
	memset(&m_WaveIDs, 0, sizeof(m_WaveIDs));
}

WavLoader::~WavLoader()
{
	long lLoop;

	for (lLoop = 0; lLoop < MAX_NUM_WAVEID; lLoop++)
	{
		if (m_WaveIDs[lLoop])
		{
			if (m_WaveIDs[lLoop]->pData)
				delete m_WaveIDs[lLoop]->pData;

			if (m_WaveIDs[lLoop]->file)
			{
				m_WaveIDs[lLoop]->file->Release();
			}

			delete m_WaveIDs[lLoop];
			m_WaveIDs[lLoop] = 0;
		}
	}
}


WAVERESULT WavLoader::LoadWaveFile(const char *szFilename, WAVEID *pWaveID)
{
	WAVERESULT wr = WR_OUTOFMEMORY;
	WAVEFILEINFO* pWaveInfo;

	pWaveInfo = new WAVEFILEINFO();
	if (pWaveInfo)
	{
		if (SUCCEEDED(wr = ParseFile(szFilename, pWaveInfo)))
		{
			// Allocate memory for sample data
			pWaveInfo->pData = new char[pWaveInfo->ulDataSize];
			if (pWaveInfo->pData)
			{
				// Seek to start of audio data				
				pWaveInfo->file->Seek( pWaveInfo->ulDataOffset, IFile::seek_set);

				// Read Sample Data
				if (pWaveInfo->file->Read(pWaveInfo->pData, pWaveInfo->ulDataSize) == pWaveInfo->ulDataSize)
				{
					long lLoop = 0;
					for (lLoop = 0; lLoop < MAX_NUM_WAVEID; lLoop++)
					{
						if (!m_WaveIDs[lLoop])
						{
							m_WaveIDs[lLoop] = pWaveInfo;
							*pWaveID = lLoop;
							wr = WR_OK;
							break;
						}
					}

					if (lLoop == MAX_NUM_WAVEID)
					{
						delete pWaveInfo->pData;
						wr = WR_OUTOFMEMORY;
					}
				}
				else
				{
					delete pWaveInfo->pData;
					wr = WR_BADWAVEFILE;
				}
			}
			else
			{
				wr = WR_OUTOFMEMORY;
			}

			pWaveInfo->file->Release();
			pWaveInfo->file = 0;
		}

		if (wr != WR_OK)
			delete pWaveInfo;
	}

	return wr;
}


WAVERESULT WavLoader::OpenWaveFile(const char *szFilename, WAVEID *pWaveID)
{
	WAVERESULT wr = WR_OUTOFMEMORY;
	WAVEFILEINFO* pWaveInfo;

	pWaveInfo = new WAVEFILEINFO();
	if (pWaveInfo)
	{
		if (SUCCEEDED(wr = ParseFile(szFilename, pWaveInfo)))
		{
			long lLoop = 0;
			for (lLoop = 0; lLoop < MAX_NUM_WAVEID; lLoop++)
			{
				if (!m_WaveIDs[lLoop])
				{
					m_WaveIDs[lLoop] = pWaveInfo;
					*pWaveID = lLoop;
					wr = WR_OK;
					break;
				}
			}

			if (lLoop == MAX_NUM_WAVEID)
				wr = WR_OUTOFMEMORY;
		}

		if (wr != WR_OK)
			delete pWaveInfo;
	}

	return wr;
}

WAVERESULT WavLoader::ReadWaveData(WAVEID WaveID, void *pData, unsigned long ulDataSize, unsigned long *pulBytesWritten)
{
	WAVEFILEINFO* pWaveInfo;
	WAVERESULT wr = WR_BADWAVEFILE;

	if (!pData || !pulBytesWritten || ulDataSize == 0)
		return WR_INVALIDPARAM;

	if (IsWaveID(WaveID))
	{
		pWaveInfo = m_WaveIDs[WaveID];
		if (pWaveInfo->file)
		{
			unsigned long ulOffset = pWaveInfo->file->GetCurrentPos();

			if ((ulOffset - pWaveInfo->ulDataOffset + ulDataSize) > pWaveInfo->ulDataSize)
				ulDataSize = pWaveInfo->ulDataSize - (ulOffset - pWaveInfo->ulDataOffset);

			*pulBytesWritten = (unsigned long)pWaveInfo->file->Read(pData, ulDataSize);

			wr = WR_OK;
		}
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

WAVERESULT WavLoader::SetWaveDataOffset(WAVEID WaveID, unsigned long ulOffset)
{
	WAVEFILEINFO* pWaveInfo;
	WAVERESULT wr = WR_INVALIDPARAM;

	if (IsWaveID(WaveID))
	{
		pWaveInfo = m_WaveIDs[WaveID];
		if (pWaveInfo->file)
		{
			// Seek into audio data
			pWaveInfo->file->Seek(pWaveInfo->ulDataOffset + ulOffset, IFile::seek_set);
			wr = WR_OK;
		}
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

WAVERESULT WavLoader::GetWaveDataOffset(WAVEID WaveID, unsigned long *pulOffset)
{
	WAVEFILEINFO* pWaveInfo;
	WAVERESULT wr = WR_INVALIDPARAM;

	if (IsWaveID(WaveID))
	{
		pWaveInfo = m_WaveIDs[WaveID];
		if ((pWaveInfo->file) && (pulOffset))
		{
			// Get current position
			*pulOffset = pWaveInfo->file->GetCurrentPos();
			*pulOffset -= pWaveInfo->ulDataOffset;
			wr = WR_OK;
		}
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

WAVERESULT WavLoader::ParseFile(const char *szFilename, WAVEFILEINFO* pWaveInfo)
{
	WAVEFILEHEADER	waveFileHeader;
	RIFFCHUNK		riffChunk;
	WAVEFMT			waveFmt;
	WAVERESULT		wr = WR_BADWAVEFILE;

	if (!szFilename || !pWaveInfo)
		return WR_INVALIDPARAM;

	memset(pWaveInfo, 0, sizeof(WAVEFILEINFO));

	// Open the wave file for reading
	pWaveInfo->file = core->Files()->OpenFile(szFilename, IFile::read);	
	
	if (pWaveInfo->file)
	{
		// Read Wave file header
		pWaveInfo->file->Read(&waveFileHeader, sizeof(WAVEFILEHEADER));
		if (String::IsEqual(waveFileHeader.szRIFF, 4, "RIFF", 4) && String::IsEqual(waveFileHeader.szWAVE, 4, "WAVE", 4))
		{
			while (pWaveInfo->file->Read(&riffChunk, sizeof(RIFFCHUNK)) == sizeof(RIFFCHUNK))
			{
				if (String::IsEqual(riffChunk.szChunkName, 4, "fmt ", 4))
				{
					if (riffChunk.ulChunkSize <= sizeof(WAVEFMT))
					{
						pWaveInfo->file->Read(&waveFmt, riffChunk.ulChunkSize);
					
						// Determine if this is a WAVEFORMATEX or WAVEFORMATEXTENSIBLE wave file
						if (waveFmt.usFormatTag == WAVE_FORMAT_PCM)
						{
							pWaveInfo->wfType = WF_EX;
							memcpy(&pWaveInfo->wfEXT.Format, &waveFmt, sizeof(PCMWAVEFORMAT_LOCAL));
						}
						else if (waveFmt.usFormatTag == WAVE_FORMAT_EXTENSIBLE)
						{
							pWaveInfo->wfType = WF_EXT;
							memcpy(&pWaveInfo->wfEXT, &waveFmt, sizeof(WAVEFORMATEXTENSIBLE));
						}
					}
					else
					{
						pWaveInfo->file->Seek(riffChunk.ulChunkSize, IFile::seek_cur);
					}
				}
				else if (String::IsEqual(riffChunk.szChunkName, 4, "data", 4))
				{
					pWaveInfo->ulDataSize = riffChunk.ulChunkSize;
					pWaveInfo->ulDataOffset = pWaveInfo->file->GetCurrentPos();
					pWaveInfo->file->Seek(riffChunk.ulChunkSize, IFile::seek_cur);					
				}
				else
				{
					pWaveInfo->file->Seek(riffChunk.ulChunkSize, IFile::seek_cur);					
				}

				// Ensure that we are correctly aligned for next chunk
				if (riffChunk.ulChunkSize & 1)
				{
					pWaveInfo->file->Seek(1, IFile::seek_cur);
				}	
			}

			if (pWaveInfo->ulDataSize && pWaveInfo->ulDataOffset && ((pWaveInfo->wfType == WF_EX) || (pWaveInfo->wfType == WF_EXT)))
			{
				wr = WR_OK;
			}
			else
			{
				pWaveInfo->file->Release();
				pWaveInfo->file = 0;
			}
		}
	}
	else
	{
		wr = WR_INVALIDFILENAME;
	}

	return wr;
}


WAVERESULT WavLoader::DeleteWaveFile(WAVEID WaveID)
{
	WAVERESULT wr = WR_OK;

	if (IsWaveID(WaveID))
	{
		if (m_WaveIDs[WaveID]->pData)
			delete m_WaveIDs[WaveID]->pData;

		if (m_WaveIDs[WaveID]->file) m_WaveIDs[WaveID]->file->Release();

		delete m_WaveIDs[WaveID];
		m_WaveIDs[WaveID] = 0;
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}


WAVERESULT WavLoader::GetWaveType(WAVEID WaveID, WAVEFILETYPE *wfType)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (!wfType)
		return WR_INVALIDPARAM;

	*wfType = m_WaveIDs[WaveID]->wfType;

	return WR_OK;
}


WAVERESULT WavLoader::GetWaveFormatExHeader(WAVEID WaveID, WAVEFORMATEX_LOCAL *wfex)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (!wfex)
		return WR_INVALIDPARAM;

	memcpy(wfex, &(m_WaveIDs[WaveID]->wfEXT.Format), sizeof(WAVEFORMATEX_LOCAL));

	return WR_OK;
}


WAVERESULT WavLoader::GetWaveFormatExtensibleHeader(WAVEID WaveID, WAVEFORMATEXTENSIBLE *wfext)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (m_WaveIDs[WaveID]->wfType != WF_EXT)
		return WR_NOTWAVEFORMATEXTENSIBLEFORMAT;

	if (!wfext)
		return WR_INVALIDPARAM;

	memcpy(wfext, &(m_WaveIDs[WaveID]->wfEXT), sizeof(WAVEFORMATEXTENSIBLE));

	return WR_OK;
}

WAVERESULT WavLoader::GetWaveData(WAVEID WaveID, void **lplpAudioData)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (!lplpAudioData)
		return WR_INVALIDPARAM;

	*lplpAudioData = m_WaveIDs[WaveID]->pData;

	return WR_OK;
}

WAVERESULT WavLoader::GetWaveSize(WAVEID WaveID, unsigned long *size)
{
	if (!IsWaveID(WaveID))
		return WR_INVALIDWAVEID;

	if (!size)
		return WR_INVALIDPARAM;

	*size = m_WaveIDs[WaveID]->ulDataSize;

	return WR_OK;
}


WAVERESULT WavLoader::GetWaveFrequency(WAVEID WaveID, unsigned long *pulFrequency)
{
	WAVERESULT wr = WR_OK;

	if (IsWaveID(WaveID))
	{
		if (pulFrequency)
			*pulFrequency = m_WaveIDs[WaveID]->wfEXT.Format.nSamplesPerSec;
		else
			wr = WR_INVALIDPARAM;
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

WAVERESULT WavLoader::GetWaveALBufferFormat(WAVEID WaveID, PFNALGETENUMVALUE pfnGetEnumValue, unsigned long *pulFormat)
{
	WAVERESULT wr = WR_OK;

	if (IsWaveID(WaveID))
	{
		if (pfnGetEnumValue && pulFormat)
		{
			*pulFormat = 0;

			if (m_WaveIDs[WaveID]->wfType == WF_EX)
			{
				if (m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 1)
				{
					switch (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample)
					{
					case 4:
						*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO_IMA4");
						break;
					case 8:
						*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO8");
						break;
					case 16:
						*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO16");
						break;
					}
				}
				else if (m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 2)
				{
					switch (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample)
					{
					case 4:
						*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO_IMA4");
						break;
					case 8:
						*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO8");
						break;
					case 16:
						*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO16");
						break;
					}
				}
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 4) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16))
					*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");
			}
			else if (m_WaveIDs[WaveID]->wfType == WF_EXT)
			{
				if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 1) &&
					((m_WaveIDs[WaveID]->wfEXT.dwChannelMask == SPEAKER_FRONT_CENTER) ||
					 (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT)) ||
					 (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == 0)))
				{
					switch (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample)
					{
					case 4:
						*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO_IMA4");
						break;
					case 8:
						*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO8");
						break;
					case 16:
						*pulFormat = pfnGetEnumValue("AL_FORMAT_MONO16");
						break;
					}
				}
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 2) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT)))
				{
					switch (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample)
					{
					case 4:
						*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO_IMA4");
						break;
					case 8:
						*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO8");
						break;
					case 16:
						*pulFormat = pfnGetEnumValue("AL_FORMAT_STEREO16");
						break;
					}
				}
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 2) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT)))
					*pulFormat =  pfnGetEnumValue("AL_FORMAT_REAR16");
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 4) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT)))
					*pulFormat = pfnGetEnumValue("AL_FORMAT_QUAD16");
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 6) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT)))
					*pulFormat = pfnGetEnumValue("AL_FORMAT_51CHN16");
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 7) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT|SPEAKER_BACK_CENTER)))
					*pulFormat = pfnGetEnumValue("AL_FORMAT_61CHN16");
				else if ((m_WaveIDs[WaveID]->wfEXT.Format.nChannels == 8) && (m_WaveIDs[WaveID]->wfEXT.Format.wBitsPerSample == 16) && (m_WaveIDs[WaveID]->wfEXT.dwChannelMask == (SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT|SPEAKER_FRONT_CENTER|SPEAKER_LOW_FREQUENCY|SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT|SPEAKER_SIDE_LEFT|SPEAKER_SIDE_RIGHT)))
					*pulFormat = pfnGetEnumValue("AL_FORMAT_71CHN16");
			}

			if (*pulFormat == 0)
				wr = WR_INVALIDWAVEFILETYPE;
		}
		else
		{
			wr = WR_INVALIDPARAM;
		}
	}
	else
	{
		wr = WR_INVALIDWAVEID;
	}

	return wr;
}

bool WavLoader::IsWaveID(WAVEID WaveID)
{
	bool bReturn = false;

	if ((WaveID >= 0) && (WaveID < MAX_NUM_WAVEID))
	{
		if (m_WaveIDs[WaveID])
			bReturn = true;
	}

	return bReturn;
}


char *WavLoader::GetErrorString(WAVERESULT wr, char *szErrorString, unsigned long nSizeOfErrorString)
{	
	switch (wr)
	{
		case WR_OK:
			strncpy_s(szErrorString, nSizeOfErrorString, "Success\n", nSizeOfErrorString-1);
			break;

		case WR_INVALIDFILENAME:
			strncpy_s(szErrorString, nSizeOfErrorString, "Invalid file name or file does not exist\n", nSizeOfErrorString-1);
			break;

		case WR_BADWAVEFILE:
			strncpy_s(szErrorString, nSizeOfErrorString, "Invalid Wave file\n", nSizeOfErrorString-1);
			break;

		case WR_INVALIDPARAM:
			strncpy_s(szErrorString, nSizeOfErrorString, "Invalid parameter passed to function\n", nSizeOfErrorString-1);
			break;

		case WR_FILEERROR:
			strncpy_s(szErrorString, nSizeOfErrorString, "File I/O error\n", nSizeOfErrorString-1);
			break;

		case WR_INVALIDWAVEID:
			strncpy_s(szErrorString, nSizeOfErrorString, "Invalid WAVEID\n", nSizeOfErrorString-1);
			break;

		case WR_NOTSUPPORTEDYET:
			strncpy_s(szErrorString, nSizeOfErrorString, "Function not supported yet\n", nSizeOfErrorString-1);
			break;

		case WR_WAVEMUSTBEMONO:
			strncpy_s(szErrorString, nSizeOfErrorString, "Input wave files must be mono\n", nSizeOfErrorString-1);
			break;

		case WR_WAVEMUSTBEWAVEFORMATPCM:
			strncpy_s(szErrorString, nSizeOfErrorString, "Input wave files must be in Wave Format PCM\n", nSizeOfErrorString-1);
			break;
		
		case WR_WAVESMUSTHAVESAMEBITRESOLUTION:
			strncpy_s(szErrorString, nSizeOfErrorString, "Input wave files must have the same Bit Resolution\n", nSizeOfErrorString-1);
			break;

		case WR_WAVESMUSTHAVESAMEFREQUENCY:
			strncpy_s(szErrorString, nSizeOfErrorString, "Input wave files must have the same Frequency\n", nSizeOfErrorString-1);
			break;

		case WR_WAVESMUSTHAVESAMEBITRATE:
			strncpy_s(szErrorString, nSizeOfErrorString, "Input wave files must have the same Bit Rate\n", nSizeOfErrorString-1);
			break;

		case WR_WAVESMUSTHAVESAMEBLOCKALIGNMENT:
			strncpy_s(szErrorString, nSizeOfErrorString, "Input wave files must have the same Block Alignment\n", nSizeOfErrorString-1);
			break;

		case WR_OFFSETOUTOFDATARANGE:
			strncpy_s(szErrorString, nSizeOfErrorString, "Wave files Offset is not within audio data\n", nSizeOfErrorString-1);
			break;

		case WR_INVALIDSPEAKERPOS:
			strncpy_s(szErrorString, nSizeOfErrorString, "Invalid Speaker Destinations\n", nSizeOfErrorString-1);
			break;

		case WR_OUTOFMEMORY:
			strncpy_s(szErrorString, nSizeOfErrorString, "Out of memory\n", nSizeOfErrorString-1);
			break;

		case WR_INVALIDWAVEFILETYPE:
			strncpy_s(szErrorString, nSizeOfErrorString, "Invalid Wave File Type\n", nSizeOfErrorString-1);
			break;

		case WR_NOTWAVEFORMATEXTENSIBLEFORMAT:
			strncpy_s(szErrorString, nSizeOfErrorString, "Wave file is not in WAVEFORMATEXTENSIBLE format\n", nSizeOfErrorString-1);
			break;

		default:
			strncpy_s(szErrorString, nSizeOfErrorString, "Undefined error\n", nSizeOfErrorString-1);
	}
	szErrorString[nSizeOfErrorString-1] = '\0';
	return szErrorString;
}