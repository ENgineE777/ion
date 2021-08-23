#pragma  once

#include "Common\Common.h"

struct eEmiterType
{
	enum Enum
	{
		type2d,
		type3d
	};
};

class CFxRes
{
	friend class FxService;
	friend class FxEditor;
	friend class FxResEdit;
public:
	
	struct SEmiterDesc
	{
		char TextureName[256];
		bool Looped;
		float LifeTime;
		dword ParticlesCount;
		dword SpawnRate;
		float SpawnTime;
		float SizeMaxX, SizeMinX;
		float SizeMaxY, SizeMinY;
		float LifeTimeMin;
		float LifeTimeMax;
		Matrix Transform;
		Vector PositionMin;
		Vector PositionMax;
		Vector VelocityMin;
		Vector VelocityMax;
		Vector AccelerationMin;
		Vector	AccelerationMax;
		float Fade;
		unsigned long Color[4];
		float scaleSizePerSec;

		eEmiterType::Enum type;


		SEmiterDesc()
		{
			memset(this, 0, sizeof(SEmiterDesc) );
		}
	};

	typedef std::vector<SEmiterDesc> Desc_t;
	const Desc_t& GetDescs() const { return m_Descs; }
	const char* GetFileName() const { return m_fileName; }
	eEmiterType::Enum GetType() const { return m_type;}

	
protected:
	inline CFxRes();
	inline ~CFxRes();

	bool Load( const char* _fileName);
	bool Save( const char* _fileName);
	
	void Release();

	inline void AddRef();
	inline void DecRef();
	inline int GetRef() const;
	
	int m_refCcount;
	Desc_t m_Descs;
	char  m_fileName[256];
	eEmiterType::Enum m_type;
};


int CFxRes::GetRef() const
{
	return m_refCcount;
}

void CFxRes::AddRef()
{
	m_refCcount++;
}

void CFxRes::DecRef()
{
	m_refCcount--;

	if (m_refCcount == 0)
	{
		Release();
	}
}


CFxRes::~CFxRes() 
{
	Release();
}

CFxRes::CFxRes() 
{
	m_refCcount = 0;
}