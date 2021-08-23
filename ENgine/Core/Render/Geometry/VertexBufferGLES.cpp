
#include "VertexBufferGLES.h"
#include "Core/Render/Shaders/VertexShaderGLES.h"
#include "Core/Core.h"

VertexBufferGLES::VertexBufferGLES(int sz,int strd) : VertexBuffer(sz, strd)
{	
#if defined(ANDROID) || defined(IOS) || defined(OSUNIX)
	data = (byte*)malloc(size);
#endif

#if /*defined(IOS) ||*/ defined(OSX)
	vbo = 0;
	glGenBuffers(1,&vbo);
	
	glBindBuffer(GL_ARRAY_BUFFER,vbo);	
	glBufferData(GL_ARRAY_BUFFER,size,null,GL_DYNAMIC_DRAW);			

	if (glGetError() == GL_OUT_OF_MEMORY)
	{
		core.Trace("out of memory for vertex buffer with size - %i", sz);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0 ); 
#endif

	offset = 0;
	num_components = 0;
}

void VertexBufferGLES::ReCreate()
{
}

void VertexBufferGLES::StartDesc()
{
	offset = 0;
	num_components = 0;
}

void VertexBufferGLES::AddDescElem(ElemUsage usage, ElemType type, const char* name)
{
	String::Copy(components[num_components].name, 32, name);
	components[num_components].type = type;
	components[num_components].offset = offset;

	num_components++;

	if (type == float2) offset += sizeof(float) * 2;
	if (type == float3) offset += sizeof(float) * 3;
	if (type == float4) offset += sizeof(float) * 4;
	if (type == tp_color) offset += sizeof(dword);
	if (type == ubyte4) offset += sizeof(dword);
}

void VertexBufferGLES::EndDesc()
{
}

void* VertexBufferGLES::Lock()
{	
#if defined(ANDROID) || defined(IOS) || defined(OSUNIX)
	return data;
#endif


#if /*defined(IOS) ||*/ defined(OSX)
	glBindBuffer(GL_ARRAY_BUFFER,vbo);

#if defined(IOS)
	void* ptr = glMapBufferOES(GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES);	
#endif

#if defined(OSX)
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);	
#endif
	
	GLenum err = glGetError();

	if (err == GL_INVALID_OPERATION)
	{
		core.Trace("invalid operation when locking vertex buffer with size - %i", size);
	}
	else
	if (err == GL_OUT_OF_MEMORY)
	{
		core.Trace("out of memory for locking vertex buffer with size - %i", size);
	}	
	
	return ptr;
#endif
}

void VertexBufferGLES::Unlock()
{		
#if defined(IOS)
	//glUnmapBufferOES(GL_ARRAY_BUFFER);
#endif
	
#if defined(OSX)
	glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
}

void VertexBufferGLES::SetComponents(VertexShaderGLES* vshader)
{	
	for (int i = 0;i<num_components;i++)
	{
		int num_cpmonents = 4;
		int type = 0;

		if (components[i].type == float2)
		{
			num_cpmonents = 2;
			type = GL_FLOAT;
		}
		
		if (components[i].type == float3)
		{
			num_cpmonents = 3;
			type = GL_FLOAT;
		}

		if (components[i].type == float4)
		{
			num_cpmonents = 4;
			type = GL_FLOAT;
		}

		if (components[i].type == tp_color || components[i].type == ubyte4)
		{
			num_cpmonents = 4;
			type = GL_UNSIGNED_BYTE;
		}
		
#if defined(ANDROID) || defined(IOS) || defined(OSUNIX)
		vshader->SetArray(components[i].name, num_cpmonents,type,stride,data+components[i].offset);		
#endif

#if /*defined(IOS) ||*/ defined(OSX)
		vshader->SetArray(components[i].name, num_cpmonents,type,stride,components[i].offset);
#endif
	}
}

void* VertexBufferGLES::GetData()
{
#if /*defined(IOS) ||*/ defined(OSX)
	return &vbo;
#endif

	return null;
}

void VertexBufferGLES::Release()
{		
#if defined(ANDROID) || defined(IOS) || defined(OSUNIX)
	free(data);
#endif

#if /*defined(IOS) ||*/ defined(OSX)
	glDeleteBuffers(1,&vbo);
#endif

	delete this;
}
