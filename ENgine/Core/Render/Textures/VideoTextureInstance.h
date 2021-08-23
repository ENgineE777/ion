#pragma once
#include "TextureInstance.h"

class TextureRes;
class Texture;

class DEF_EXPORT VideoTextureInstance : public TextureInstance 
{	
    Texture* video_texture;

public:	
	
	VideoTextureInstance(TextureRes* rs);
	
	virtual void SetLooped(bool looped);
	void Work(float dt, int level);
	
	virtual void Pause(bool pause);	
	virtual void Restart();
	virtual float GetDuration();
	virtual float GetCurTime();

	virtual int GetWidth();
	virtual int GetHeight();
	virtual void* GetData();

	virtual bool IsVideo();

	virtual void SetAdress(TextureAddress adress);

	/*
#ifdef OPGL
	virtual GLuint GetInternalTexture();
	virtual GLenum GetInternalTarget();
#endif
	*/

	virtual void Release();

	static int instCounter;
};
