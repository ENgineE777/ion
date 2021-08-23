
#include "ShaderGLES.h"
#include "Core/Core.h"

ShaderGLES::ShaderGLES() : params(_FL_)
{
	shader = -1;
}

ShaderGLES::~ShaderGLES()
{
	params.Clear();
}

ShaderGLES::Param* ShaderGLES::GetParam(const char* param, int type)
{
	Param* p = params.Get(param);

	if (!p)
	{
		p = params.Add(param);

		p->type = type;
		
		if (type == 1)
		{
			p->param = glGetAttribLocation(program, param);
		}
		else
		if (type == 2)
		{
			p->param = glGetUniformLocation(program, param);
		}
		else
		if (type == 3)
		{
			p->param = glGetUniformLocation(program, param);			
		}

		
		if (p->param == -1)
		{
			//core->TraceTo("Render", "In shader %s absent param %s", param);
		}
	}

	return p;
}

bool ShaderGLES::SetVector4(const char* param, const Vector4& v)
{
	Param* p = GetParam(param, 2);

	if (p->param != -1)
	{					
		glUniform4fv(p->param, 1, (float*)&v.x);		

		return true;
	}

	return false;
}

bool ShaderGLES::SetVector4Array(const char* param, int num, float* v)
{
	Param* p = GetParam(param, 2);

	if (p->param != -1)
	{				
		glUniform4fv(p->param, num, v);				

		return true;
	}

	return false;
}

bool ShaderGLES::SetMatrix(const char* param, const Matrix& mat)
{
	Param* p = GetParam(param, 2);

	if (p->param != -1)
	{			
		glUniformMatrix4fv(p->param, 1, 0, (float*)&mat);		
		
		return true;
	}

	return false;
}

bool ShaderGLES::SetMatrixArray(const char* param, int num, float* mat)
{
	Param* p = GetParam(param, 2);

	if (p->param != -1)
	{		
		glUniformMatrix4fv(p->param, num, false, mat);				

		return true;
	}

	return false;	
}

void ShaderGLES::CompileShader(int type, const char* file_name)
{
	if (!en_core.Render()->ResoursesAreDead())
	{
		if (shader != -1)
		{		
			glDeleteShader(shader);
			shader = -1;
		}
	}

	//core->TraceTo("app", "before glCreateShader");
	shader = glCreateShader(type);
	//core->TraceTo("app", "after glCreateShader");
		
	char file_path[512];

	String::Copy(file_path, 511, "#platform/shaders/");	
	String::Cat(file_path, 511, file_name);

	IFileBuffer* fb = core->Files()->LoadFile(file_path);

	if (fb)
	{		
		const char* buff = (const char*)fb->GetBuffer();
		glShaderSource(shader, 1, (const char**)&buff, NULL);

		glCompileShader(shader);
	
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    
		if (logLength > 0)
		{
			GLchar *log = (GLchar *)malloc(logLength);
			glGetShaderInfoLog(shader, logLength, &logLength, log);
		
			if (type == GL_FRAGMENT_SHADER)
			{
				core->TraceTo("Render", "Pixel Shader %s compile log: %s", file_path, log);
			}
			else
			{
				core->TraceTo("Render", "Vertex Shader %s compile log: %s", file_path, log);
			}
		
			free(log);
		}
	
		int status = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == 0)
		{
			glDeleteShader(shader);
		}

		fb->Release();
	}
}