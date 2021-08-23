#pragma once

#include "NGUIWidget.h"

class NGUIMap : public NGUIWidget
{		
	float longitude, latitude;

	TextureEntry::TextureData table[9];
	bool in_drag;
	int  offset_x;
	int  offset_y;

	int center_tile_x;
	int center_tile_y;

	int zoom;
	
public:		
	
	PROP_START		

	ENTITY_CONSTRUCT(NGUIMap,NGUIWidget)
	ENTITY_SCRIPTDEC	

	virtual void RegisterProperties();
	virtual void Init();
	virtual void ReInit();	

	void CheckPress(float dt, int level);
	void ReloadCache();

	virtual bool Start();	
	
	virtual void RenderWidget();

	virtual void Release();
};

