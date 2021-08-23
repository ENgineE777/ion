#include "Core/Core.h"

RTDeclBase* RTDeclBase::first = NULL;

#ifdef OPGL
#include "Core/Render/Shaders/VertexShaderGLES.h"
#include "Core/Render/Shaders/PixelShaderGLES.h"
#endif

RenderTechnique::RenderTechnique()
{	
	vshader = null;
	pshader = null;

#ifdef OPGL
	program = -1;
#endif
}
	
bool RenderTechnique::Init()
{
	LoadVS();
	LoadPS();

#ifdef OPGL
	ReCreate();
#endif
	
	return true;
}

void RenderTechnique::ReCreate()
{
#ifdef OPGL

	/*if (program != -1)
	{
		glDeleteProgram(program);
	}*/

	program = glCreateProgram();
	
    glAttachShader(program, *((GLuint*)vshader->GetData()));
    glAttachShader(program, *((GLuint*)pshader->GetData()));
    glLinkProgram(program);
	
    // 3
    GLint linkSuccess;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
    
	if (linkSuccess == GL_FALSE)
	{
        GLchar messages[256];
        glGetProgramInfoLog(program, sizeof(messages), 0, &messages[0]);
        core->TraceTo("Render", messages);
	}
	
	((VertexShaderGLES*)vshader)->shader.program = program;
	((PixelShaderGLES*)pshader)->shader.program = program;
#endif
}

void RenderTechnique::SetPSVS()
{
	en_core.Render()->SetVertexShader(vshader);	
	en_core.Render()->SetPixelShader(pshader);		
}

void RenderTechnique::ApplyParams()
{
#ifdef OPGL	

	glUseProgram(program);

	/*char str[128];
	sprintf(str, "%s %s %i", vshader->GetFileName(), pshader->GetFileName(), program);

	bool err = false;

	for (GLint error = glGetError(); error; error = glGetError())
	{	
		core->Trace(str);
		err = true;	
	}

	while (err)
	{
		core->Trace("Recreate");

		vshader->Load();
		pshader->Load();

		ReCreate();

		glUseProgram(program);

		err = false;

		for (GLint error = glGetError(); error; error = glGetError())
		{	
			core->Trace(str);
			err = true;	
		}

		break;
	}*/

	en_core.Render()->SetVertexShader(vshader);	
	en_core.Render()->SetPixelShader(pshader);	
	
	((VertexShaderGLES*)vshader)->shader.program = program;
	((PixelShaderGLES*)pshader)->shader.program = program;

#else
	en_core.Render()->SetVertexShader(vshader);	
	en_core.Render()->SetPixelShader(pshader);	

	if (vshader) vshader->ActivateParams(true);
	if (pshader) pshader->ActivateParams(true);
#endif
}

void RenderTechnique::PostApplyParams()
{
	if (vshader) vshader->ActivateParams(false);
	if (pshader) pshader->ActivateParams(false);
}

void RenderTechnique::ActivateParams()
{
    if (vshader) vshader->ActivateParams(true);
    if (pshader) pshader->ActivateParams(true);
}

void RenderTechnique::ActivateVertexShaderParams()
{
    if (vshader) vshader->ActivateParams(true);
}
void RenderTechnique::ActivatePixelShaderParams()
{
    if (pshader) pshader->ActivateParams(true);
}

void RenderTechnique::DeactivateParams()
{/*
#ifdef PC
    if (vshader) vshader->ActivateParams(false);
#endif*/
    if (pshader) pshader->ActivateParams(false);
}

void RenderTechnique::DeactivateVertexShaderParams()
{
    if (vshader) vshader->ActivateParams(false);
}

void RenderTechnique::DeactivatePixelShaderParams()
{
    if (pshader) pshader->ActivateParams(false);
}

void RenderTechnique::UpdateShaderConstants()
{
	if (vshader) vshader->UpdateConstants();
	if (pshader) pshader->UpdateConstants();
	if (pshader) pshader->UpdateTextures();
}

bool RenderTechnique::VS_ParamExist(const char* param)
{
	if (vshader) return vshader->ParamExist(param);
	return false;
}

void RenderTechnique::VS_SetMatrix(const char* param, const Matrix& mat)
{
	if (vshader) vshader->SetMatrix(param, mat);
}

void RenderTechnique::VS_SetVector4(const char* param, const Vector4& v)
{
	if (vshader) vshader->SetVector4(param, v);
}

void RenderTechnique::VS_SetVector4Array(const char* param, int num, float* v)
{
	if (vshader) vshader->SetVector4Array(param, num, v);
}

void RenderTechnique::VS_SetMatrixArray(const char* param, int num, float* mat)
{
	if (vshader) vshader->SetMatrixArray(param, num, mat);
}

bool RenderTechnique::PS_ParamExist(const char* param)
{
	if (pshader) return pshader->ParamExist(param);
	return false;
}

void RenderTechnique::PS_SetTexture(const char* param, Texture* texture)
{
	if (pshader) pshader->SetTexture(param, texture);
}

void RenderTechnique::PS_SetTexture(const char* param, int slot, Texture* texture)
{
	if (pshader) pshader->SetTexture(param, slot, texture);
}

void RenderTechnique::PS_SetVector4(const char* param, const Vector4& v)
{
	if (pshader) pshader->SetVector4(param, v);
}

void RenderTechnique::PS_SetVector4Array(const char* param, int num, float* v)
{
    if (pshader) pshader->SetVector4Array(param, num, v);
}

void RenderTechnique::PS_SetMatrix(const char* param, const Matrix& mat)
{
    if (pshader) pshader->SetMatrix(param, mat);
}

void RenderTechnique::PS_SetMatrixArray(const char* param, int num, float* mat)
{
    if (pshader) pshader->SetMatrixArray(param, num, mat);
}