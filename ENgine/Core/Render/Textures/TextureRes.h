
#pragma once

#include "Common/Common.h"

class Texture;
class TextureInstance;

class TextureRes
{		
public:	
	enum Type
	{
		eTexture2d = 0,
		eVideo
	};

	struct TextureFrame
	{
		float time;
        char  name[256];
		Texture* texture;
	};

	float total_time;
	std::vector<TextureFrame> frames;

	TextureRes();
	virtual ~TextureRes();

	virtual void ReCreate();

	bool Load(const char* name, bool async, const char* file, int line);
	bool LoadFrame(const char* name, float time, bool async, const char* file, int line);	
	TextureInstance* CreateReference(const char* file, int line);	
	void AddRef();
	void DecRef();
    void Reset();
    void Restore();
	void Release();
	Type GetType() { return type; }
	const char* GetPath() { return path.data; }

private:
	int ref_count;	
	bool def_looped;
	bool need_stream;
	Type type;
	Str256 path;
};
