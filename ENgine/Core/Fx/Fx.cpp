#include "Emiter.h"
#include "FxRes.h"
#include "Core/core.h"
#include "Fx.h"

Fx::Fx(void)
:m_Res(NULL)
{
	

}


bool Fx::SetRes(const char* _resName)
{
	m_Res = core->Fx()->GetRes(_resName);
	OnChangeRes();		

	return true;
}

bool Fx::OnChangeRes()
{
	m_Active = false;
	

	int size = m_Emiters.size();
	for(int i = 0; i < size; ++i)
	{
		delete m_Emiters[i];
	}
	m_Emiters.swap(std::vector<CEmiter*>());

	if (!m_Res)
		return false;
	
	size = m_Res->GetDescs().size();
	for(int i = 0; i < size; ++i)
	{
		m_Emiters.push_back(new CEmiter());
		m_Emiters.back()->SetDesc(&m_Res->GetDescs()[i]);
	}

	return true;

}

void Fx::Release(void)
{
	if (m_Res)
		core->Fx()->RelaaseRes(m_Res);
	OnChangeRes();
	
}


void Fx::SetTransform(Matrix& m) 
{
	m_Transform = m;
	if (m_Emiters.size())
	{
		CEmiter** pLast = &m_Emiters.back();
		for(CEmiter** p = &m_Emiters.front(); p <= pLast; ++p)
		{
			(*p)->SetTransform(m_Transform);
		}
	}
}

void Fx::Update(const float dt) 
{
	if (!m_Res)
		return;

	if (m_Emiters.size())
	{
		CEmiter** pLast = &m_Emiters.back();
		for(CEmiter** p = &m_Emiters.front(); p <= pLast; ++p)
		{
			(*p)->Update(dt);
		}
	}

}

void Fx::Draw()
{
	if (!m_Res)
		return;

	if (m_Emiters.size())
	{
		CEmiter** pLast = &m_Emiters.back();
		for(CEmiter** p = &m_Emiters.front(); p <= pLast; ++p)
		{
			(*p)->Draw();
		}
	}
	
}

void Fx::ReStart()
{
	if (!m_Res)
		return;

	if (m_Emiters.size())
	{
		CEmiter** pLast = &m_Emiters.back();
		for(CEmiter** p = &m_Emiters.front(); p <= pLast; ++p)
		{
			(*p)->ReStart();
		}
	}

}

