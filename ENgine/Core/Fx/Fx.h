#pragma once

#include "Common/Common.h"
#include "FxRes.h"

class CFxRes;
class CEmiter;

class DEF_EXPORT Fx 
{
	friend class FxService;
	friend class FxEditor;
	friend class FxResEdit;

public:
	bool SetRes( const char* _resName);
	inline Matrix& GetTransform();
	void SetTransform(Matrix &m);
	///call from user code only for 2d fx
	void Draw();


protected:

	Fx(void);
	inline ~Fx(void);
	bool OnChangeRes();
	void ReStart();
	void Release(void);
	void Update(const float dt);
	

	const CFxRes* m_Res;
	bool m_Active;
	float m_Life;
	Matrix m_Transform;
	std::vector<CEmiter*> m_Emiters;
};


Fx::~Fx(void)
{
	Release();
}

Matrix& Fx::GetTransform(){ return m_Transform; }
