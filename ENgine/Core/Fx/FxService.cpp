#include "Core\core.h"
#include "FxService.h"
#include "Common\Math\Vector.h"
#include <algorithm>
#include "FxRes.h"
#include "Fx.h"

FxService::FxService()
:m_debugMode(false)
{
}

bool FxService::Init()
{
	core->AddTask(this, this, (TaskHolder::Task)&FxService::Update, -10, -1.f);
	core->Render()->RegisterCallback("debug_render", this, this, (TaskHolder::Task)&FxService::Draw, 0);

	m_Fxes.reserve(25);
	return true;
}

void FxService::Update(float dt, int level)
{
	if (m_Fxes.size())
	{
		Fx** pLast = &m_Fxes.back();
		for(Fx** p = &m_Fxes.front(); p <= pLast; ++p)
		{
			(*p)->Update(dt);
		}
	}

	if (m_debugMode)
		DebugRender();
}


void FxService::Draw(float dt, int level)
{
	//FIX ME!!!
	/*core->Render()->Device()->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	
	core->Render()->Device()->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	core->Render()->Device()->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	core->Render()->Device()->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );*/

	if (m_Fxes.size())
	{
		Fx** pLast = &m_Fxes.back();
		for(Fx** p = &m_Fxes.front(); p <= pLast; ++p)
		{
			if ( (*p)->m_Res && (*p)->m_Res->GetType() == eEmiterType::type3d )
				(*p)->Draw();
		}
	}
}
void FxService::Release()
{
	if (m_Fxes.size())
	{
		Fx** pLast = &m_Fxes.back();
		for(Fx** p = &m_Fxes.front(); p <= pLast; ++p)
		{
			delete (*p);
		}
	}

	m_Fxes.swap(std::vector<Fx*>());
	
	for (stdext::hash_map<string, CFxRes*>::iterator it = m_resources.begin(); it != m_resources.end(); ++it)
	{
		delete it->second;
	}
	m_resources.clear();
}

void FxService::DebugRender()
{
}

Fx* FxService::CreateFx( const char* _filename)
{
	m_Fxes.push_back(new Fx());
	if (m_Fxes.back()->SetRes(_filename))
	{
		return m_Fxes.back();
	}
	else
	{
		delete m_Fxes.back();
		m_Fxes.pop_back();
		return NULL;

	}

	return NULL;
}

void FxService::ReleaseFx(class Fx*& _p)
{
	if (m_Fxes.size())
	{
		Fx** pLast = &m_Fxes.back();
		for(Fx** p = &m_Fxes.front(); p <= pLast; ++p)
		{
			if (*p == _p)
			{
				*p = m_Fxes.back();
				m_Fxes.pop_back();
				delete _p;
				break;

			}
		}
	}

	_p = NULL;
	
}

const CFxRes* FxService::GetRes(const char* _fileName)
{
	CFxRes* res = null; 
	std::hash_map<std::string, CFxRes*>::iterator it = m_resources.find(_fileName);
	if (it != m_resources.end() )
		res = it->second;
	if (res)
	{
		res->AddRef();
		return res;
	}
	else
	{
		CFxRes* newRes = new CFxRes();
		if (!newRes->Load(_fileName))
		{
			delete newRes;
			return NULL;
		}

		m_resources[_fileName] = newRes;
		newRes->AddRef();
		return newRes;
	}

	return NULL;
}

void FxService::RelaaseRes(const CFxRes*& _res)
{
	const_cast<CFxRes*>(_res)->DecRef();

	if (_res->GetRef() == 0)
	{
		CFxRes* res = null;
		std::hash_map<std::string, CFxRes*>::iterator it = m_resources.find(_res->GetFileName());
		if (it != m_resources.end())
			res = it->second;
		if (res == _res)
		{
			m_resources.erase(_res->GetFileName());
			delete _res;
			
		}
		else
			assert(!"fuck");
	}

	_res = NULL;
}