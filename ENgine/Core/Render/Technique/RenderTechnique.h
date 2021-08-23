
#pragma once

#include "Core/Render/Textures/Texture.h"
#include "Common/ClassDeclarant.h"

class VertexShader;
class PixelShader;

class RenderTechniqueExtension;

#ifdef IOS
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#endif

#if defined(ANDROID) || defined(OSUNIX)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#ifdef OSX
#import	<OpenGL/gl.h>
#import	<OpenGL/glu.h>
#endif

class DEF_EXPORT RenderTechnique
{	
protected:
	VertexShader* vshader;
	PixelShader* pshader;
	
	
public:	
	
#ifdef OPGL
	GLuint program;
#endif
	
	RenderTechnique();
	virtual ~RenderTechnique() {};
	virtual bool Init();

	void SetPSVS();
		
	virtual void ApplyParams();
	virtual void PostApplyParams();

    virtual void ActivateParams();
    virtual void DeactivateParams();
	
    virtual void ActivateVertexShaderParams();
    virtual void ActivatePixelShaderParams();

    virtual void DeactivateVertexShaderParams();
    virtual void DeactivatePixelShaderParams();

	virtual void UpdateShaderConstants();    
    //virtual void UpdateTextures();

	virtual void LoadVS() = 0;
	virtual void LoadPS() = 0;	

	virtual void ReCreate();

	virtual bool VS_ParamExist(const char* param);
	virtual void VS_SetVector4(const char* param, const Vector4& v);
	virtual void VS_SetVector4Array(const char* param, int num, float* v);	
	virtual void VS_SetMatrix(const char* param, const Matrix& mat);
	virtual void VS_SetMatrixArray(const char* param, int num, float* mat);	
	
	virtual bool PS_ParamExist(const char* param);
	virtual void PS_SetTexture(const char* param, Texture* texture);
	virtual void PS_SetTexture(const char* param, int slot, Texture* texture);

	virtual void PS_SetVector4(const char* param, const Vector4& v);
    virtual void PS_SetVector4Array(const char* param, int num, float* v);	
    virtual void PS_SetMatrix(const char* param, const Matrix& mat);
    virtual void PS_SetMatrixArray(const char* param, int num, float* mat);	
};

ClassDeclarantBegin(RTDecl, RenderTechnique)
ClassDeclarantEnd(RTDecl, RenderTechnique)

#define RTECHNIQUE(className, name)\
DESC_BEGIN(RTDecl,className,name,0)\
DESC_END_WO_DESC(RTDecl,className,name,0)
