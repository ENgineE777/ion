#pragma once

#include "Common/Common.h"
#include "FxRes.h"

class Texture;

class CEmiter
{
	friend class Fx;
protected:

	void SetDesc( const CFxRes::SEmiterDesc* _desc);
	inline Matrix& GetTransform();
	inline void SetTransform(Matrix &m);
	void Update(const float dt);
	void Draw( );
	void ReStart();

	CEmiter(void);
	virtual ~CEmiter(void);

	void OnChangeDesc();
	void Release(void);


	struct vertex
	{
		Vector			xyz;
		dword			color;
		float			tu, tv;
	};
	

	struct SParticle
	{
		bool Active;
		float Life;
		float LifeInit;
		Vector Position;
		Vector Velocity;
		Vector	Acceleration;
		float sizeX;
		float sizeY;
		float alfa;

		void Draw();
	};

	const CFxRes::SEmiterDesc* m_Desc;
	bool m_Active;
	class TextureInstance* m_Texture;
	float m_LifeTime;
	float m_SpawnTimeCounter;
	Matrix m_Transform;
	std::vector<SParticle> m_Particles;

	class RenderTechnique* tech;
	class VertexBuffer* m_vbuffer;
};


Matrix& CEmiter::GetTransform(){ return m_Transform; }
void CEmiter::SetTransform(Matrix& m) { m_Transform = m; }


