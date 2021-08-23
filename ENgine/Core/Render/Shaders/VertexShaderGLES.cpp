
#include "VertexShaderGLES.h"
#include "Core/Core.h"

VertexShaderGLES::VertexShaderGLES(const char* name) : VertexShader(name)
{				
}

bool VertexShaderGLES::Load()
{
	shader.CompileShader(GL_VERTEX_SHADER, GetFileName());
    return true;
}

void* VertexShaderGLES::GetData()
{
	return &shader.shader;
}

bool VertexShaderGLES::SetVector4(const char* param, const Vector4& v)
{
	return shader.SetVector4(param, v);
}

bool VertexShaderGLES::SetVector4Array(const char* param, int num, float* v)
{
	return shader.SetVector4Array(param, num, v);
}

bool VertexShaderGLES::SetMatrix(const char* param, const Matrix& mat)
{
	return shader.SetMatrix(param, mat);
}

bool VertexShaderGLES::SetMatrixArray(const char* param, int num, float* mat)
{
	return shader.SetMatrixArray(param, num, mat);
}

#if defined(ANDROID) || defined(IOS) || defined(OSUNIX)
bool VertexShaderGLES::SetArray(const char* param, int num, int type, int stride, byte* offset)
{
	ShaderGLES::Param* p = shader.GetParam(param, 1);

	if (p->param == -1) return false;
	
	glVertexAttribPointer(p->param, num, type, GL_FALSE, stride, offset);	
	glEnableVertexAttribArray(p->param);	
	
	return true;
}
#endif

#if /*defined(IOS) ||*/ defined(OSX)
bool VertexShaderGLES::SetArray(const char* param, int num, int type, int stride, int offset)
{
	Param* p = GetParam(param, 1);

	if (p->param == -1) return false;
	
	glVertexAttribPointer(p->param, num, type, GL_FALSE, stride, (GLvoid*)offset);
	glEnableVertexAttribArray(p->param);
	
	return true;
}
#endif

void VertexShaderGLES::Release()
{
	glDeleteShader(shader.shader);
}