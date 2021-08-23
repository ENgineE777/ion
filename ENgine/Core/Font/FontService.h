#pragma once

#include "Common/TaskHolder.h"
#include "Common/Containers/HashMap.h"

#include "Core/Font/FontInstance.h"
#include "Core/Font/FontRes.h"

class FontService : public TaskHoldersOwner
{
	friend class FontRes;
	FT_Library library;
	HashMap<FontRes> fonts;
	ThreadLocker locker;

	struct FontVertex
	{
		Vector pos;
		Vector2 uv;		
	};

	static VertexBuffer*    vbuffer;	
	static RenderTechnique* tech_nfont;

public: 
		
	FontService();
	virtual bool Init();		
	virtual void Work(float dt, int level);	
	virtual FontInstance* CreateFont(const char* file_name, bool is_bold, bool is_italic, int height, float row_gap, float outline_thin);
	virtual void DeleteRes(FontRes* res);
	virtual void Release();

	void Reset();
	void Restore();
};
