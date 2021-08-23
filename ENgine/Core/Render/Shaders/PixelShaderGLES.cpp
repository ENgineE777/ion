
#include "PixelShaderGLES.h"
#include "Core/Core.h"

/*
void RecurseParams( CGparameter param )
{
	if (param)
	{
		core.Trace("%s", cgGetParameterName(param));
	}	
   
	if(!param)
      return;
   do {
      switch(cgGetParameterType(param))
         {
            case CG_STRUCT :
               RecurseParams(cgGetFirstStructParameter(param));
               break;
            case CG_ARRAY :
               {
                 int ArraySize = cgGetArraySize(param, 0);
                 int i;
                 for (i=0; i < ArraySize; ++i)
                    RecurseParams(cgGetArrayParameter(param, i));
               }
               break;
            default :
                 // Do stuff to param
         }
   } while((param = cgGetNextParameter(param)) != 0);
}

void RecurseParamsInProgram( CGprogram prog )
{
	RecurseParams( cgGetFirstParameter( prog, CG_PROGRAM ) );
}
*/


PixelShaderGLES::PixelShaderGLES(const char* name) : PixelShader(name)
{
	
}

bool PixelShaderGLES::Load()
{
	shader.CompileShader(GL_FRAGMENT_SHADER, GetFileName());
    return true;
}

void* PixelShaderGLES::GetData()
{
	return &shader.shader;
}

bool PixelShaderGLES::SetVector4(const char* param, const Vector4& v)
{
	return shader.SetVector4(param, v);
}

bool PixelShaderGLES::SetVector4Array(const char* param, int num, float* v)
{
	return shader.SetVector4Array(param, num, v);
}

bool PixelShaderGLES::SetMatrix(const char* param, const Matrix& mat)
{
	return shader.SetMatrix(param, mat);
}

bool PixelShaderGLES::SetMatrixArray(const char* param, int num, float* mat)
{
	return shader.SetMatrixArray(param, num, mat);
}

bool PixelShaderGLES::SetTexture(const char* param, Texture* texture)
{
	ShaderGLES::Param* p = shader.GetParam(param, 3);
	
	if (p->param == -1) return false;

	if (texture)
	{		
		glActiveTexture(GL_TEXTURE0);		
		
		//glBindTexture( ((TextureGLES*)texture)->GetTarget(), *(GLuint*)(texture->GetData()));
        glBindTexture( GL_TEXTURE_2D, *(GLuint*)(texture->GetData()));

		glUniform1i(p->param, 0); // Correct		
	}
	else
	{		
	}

	return true;
}

bool PixelShaderGLES::SetTexture(const char* param, int slot, Texture* texture)
{
	ShaderGLES::Param* p = shader.GetParam(param, 3);
	
	if (p->param == -1) return false;

	if (texture)
	{
		if (slot == 0)
		{
			glActiveTexture(GL_TEXTURE0);			
		}
		else
		if (slot == 1)
		{
			glActiveTexture(GL_TEXTURE1);			
		}
		else
		if (slot == 2)
		{
			glActiveTexture(GL_TEXTURE2);			
		}
		else
		if (slot == 3)
		{
			glActiveTexture(GL_TEXTURE3);			
		}		

		//glBindTexture(((TextureGLES*)texture)->GetTarget(), *(GLuint*)(texture->GetData()));
        glBindTexture( GL_TEXTURE_2D, *(GLuint*)(texture->GetData()));
        
		glUniform1i(p->param, slot);
				
	}
	else
	{		
	}

	return true;
}

void PixelShaderGLES::Release()
{
	glDeleteShader(shader.shader);
}