
#pragma once

#include <stdio.h>
#include "Common/Common.h"

#define MAX_NUM_WAVEID			1024

class IFile;

struct GUID_LOCAL
{
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
};

enum WAVEFILETYPE
{
	WF_EX  = 1,
	WF_EXT = 2
};

enum WAVERESULT
{
	WR_OK = 0,
	WR_INVALIDFILENAME					= - 1,
	WR_BADWAVEFILE						= - 2,
	WR_INVALIDPARAM						= - 3,
	WR_INVALIDWAVEID					= - 4,
	WR_NOTSUPPORTEDYET					= - 5,
	WR_WAVEMUSTBEMONO					= - 6,
	WR_WAVEMUSTBEWAVEFORMATPCM			= - 7,
	WR_WAVESMUSTHAVESAMEBITRESOLUTION	= - 8,
	WR_WAVESMUSTHAVESAMEFREQUENCY		= - 9,
	WR_WAVESMUSTHAVESAMEBITRATE			= -10,
	WR_WAVESMUSTHAVESAMEBLOCKALIGNMENT	= -11,
	WR_OFFSETOUTOFDATARANGE				= -12,
	WR_FILEERROR						= -13,
	WR_OUTOFMEMORY						= -14,
	WR_INVALIDSPEAKERPOS				= -15,
	WR_INVALIDWAVEFILETYPE				= -16,
	WR_NOTWAVEFORMATEXTENSIBLEFORMAT	= -17
};

struct WAVEFORMATEX_LOCAL
{
    word    wFormatTag;
    word    nChannels;
    dword   nSamplesPerSec;
    dword   nAvgBytesPerSec;
    word    nBlockAlign;
    word    wBitsPerSample;
    word    cbSize;
};

struct WAVEFORMATEXTENSIBLE
{
    WAVEFORMATEX_LOCAL   Format;
    
	union Samples
	{
        word wValidBitsPerSample;       /* bits of precision  */
        word wSamplesPerBlock;          /* valid if wBitsPerSample==0 */
        word wReserved;                 /* If neither applies, set to zero. */
    };
    
	dword           dwChannelMask;      /* which channels are */
                                        /* present in stream  */
    GUID_LOCAL      SubFormat;
};

struct WAVEFILEINFO
{
	WAVEFILETYPE	wfType;
	WAVEFORMATEXTENSIBLE wfEXT;		// For non-WAVEFORMATEXTENSIBLE wavefiles, the header is stored in the Format member of wfEXT
	char			*pData;
	unsigned long	ulDataSize;
	IFile			*file;
	unsigned long	ulDataOffset;
};

//typedef int (__cdecl *PFNALGETENUMVALUE)( const char *szEnumName );
typedef int (*PFNALGETENUMVALUE)( const char *szEnumName );

typedef int	WAVEID;

class WavLoader  
{
public:
	
	WavLoader();
	virtual ~WavLoader();

	WAVERESULT LoadWaveFile(const char *szFilename, WAVEID *WaveID);
	WAVERESULT OpenWaveFile(const char *szFilename, WAVEID *WaveID);
	WAVERESULT ReadWaveData(WAVEID WaveID, void *pData, unsigned long ulDataSize, unsigned long *pulBytesWritten);
	WAVERESULT SetWaveDataOffset(WAVEID WaveID, unsigned long ulOffset);
	WAVERESULT GetWaveDataOffset(WAVEID WaveID, unsigned long *pulOffset);
	WAVERESULT GetWaveType(WAVEID WaveID, WAVEFILETYPE *pwfType);
	WAVERESULT GetWaveFormatExHeader(WAVEID WaveID, WAVEFORMATEX_LOCAL *pWFEX);
	WAVERESULT GetWaveFormatExtensibleHeader(WAVEID WaveID, WAVEFORMATEXTENSIBLE *pWFEXT);
	WAVERESULT GetWaveData(WAVEID WaveID, void **ppAudioData);
	WAVERESULT GetWaveSize(WAVEID WaveID, unsigned long *pulDataSize);
	WAVERESULT GetWaveFrequency(WAVEID WaveID, unsigned long *pulFrequency);
	WAVERESULT GetWaveALBufferFormat(WAVEID WaveID, PFNALGETENUMVALUE pfnGetEnumValue, unsigned long *pulFormat);
	WAVERESULT DeleteWaveFile(WAVEID WaveID);

	char *GetErrorString(WAVERESULT wr, char *szErrorString, unsigned long nSizeOfErrorString);
	bool IsWaveID(WAVEID WaveID);

private:
	WAVERESULT ParseFile(const char *szFilename, WAVEFILEINFO* pWaveInfo);
	WAVEID InsertWaveID(WAVEFILEINFO* pWaveFileInfo);
	
	WAVEFILEINFO* m_WaveIDs[MAX_NUM_WAVEID];
};
