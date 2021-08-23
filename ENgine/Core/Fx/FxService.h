#pragma  once

#include "Common\Common.h"
#include <hash_map>

class CFxRes;
class Fx;

class FxService: public TaskHoldersOwner
{	
public:

	FxService();
	~FxService() {};

	bool Init();
	void Release();

	Fx* CreateFx( const char* _filename);
	void ReleaseFx(Fx*& _p);
	const CFxRes* GetRes(const char* _fileName);
	void RelaaseRes(const CFxRes*& _res);

	bool m_debugMode:1;

private:
	void Update(float dt, int level);
	void Draw(float dt, int level);
	void DebugRender();

	std::vector<Fx*> m_Fxes;
	std::hash_map<std::string, CFxRes*> m_resources;

};
