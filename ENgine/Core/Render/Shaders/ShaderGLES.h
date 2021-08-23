
#pragma once

#include "Shader.h"
#include "Common/Containers/HashMap.h"

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

class ShaderGLES
{	
public:
	
	struct Param
	{
		int type;
		GLuint param;
	};

	HashMap<Param> params;

public:
	
	GLuint shader;
	GLuint program;

	ShaderGLES();
	virtual ~ShaderGLES();

	virtual bool SetVector4(const char* param, const Vector4& v);
	virtual bool SetVector4Array(const char* param, int num, float* v);	
	virtual bool SetMatrix(const char* param, const Matrix& mat);
	virtual bool SetMatrixArray(const char* param, int num, float* mat);	

	virtual void UpdateConstants() {};	

	Param* GetParam(const char* param, int type);
	void CompileShader(int type, const char* file_name);
};