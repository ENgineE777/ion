
#include "IndexBufferGLES.h"
#include "Core/Core.h"

IndexBufferGLES::IndexBufferGLES(int sz) : IndexBuffer(sz)
{
#if defined(ANDROID) || defined(OSUNIX)
	data = (byte*)malloc(size);
#endif

	indexVBO = 0;
	glGenBuffers(1,&indexVBO);	

#if defined(IOS) || defined(OSX)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,size,null,GL_STATIC_DRAW);
	
	if (glGetError() == GL_OUT_OF_MEMORY)
	{
		core->TraceTo("render", "out of memory for index buffer with size - %i", sz);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
#endif
}

void IndexBufferGLES::ReCreate()
{
	glDeleteBuffers(1,&indexVBO);
	glGenBuffers(1,&indexVBO);
}

void* IndexBufferGLES::Lock()
{		
#if defined(ANDROID) || defined(OSUNIX)
	return data;
#endif

#if defined(IOS) || defined(OSX)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexVBO);

#if defined(IOS)
	void* ptr = glMapBufferOES(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY_OES);	
#endif
	
#if defined(OSX)
	void* ptr = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);	
#endif

	GLenum err = glGetError();

	if (err == GL_INVALID_OPERATION)
	{
		core->TraceTo("render", "invalid operation when locking index buffer with size - %i", size);
	}
	else
	if (err == GL_OUT_OF_MEMORY)
	{
		core->TraceTo("render", "out of memory for locking index buffer with size - %i", size);
	}	

	return ptr;
#endif
}

void IndexBufferGLES::Unlock()
{

#if defined(ANDROID) || defined(OSUNIX)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
	
	if (glGetError() == GL_OUT_OF_MEMORY)
	{
		core->TraceTo("render", "out of memory for index buffer with size - %i", size);
	}

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0 );
#endif
	
#if defined(IOS)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexVBO);			
	glUnmapBufferOES(GL_ELEMENT_ARRAY_BUFFER);
#endif
	
#if defined(OSX)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexVBO);			
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
#endif
}

void* IndexBufferGLES::GetData()
{
	return &indexVBO;
}

void IndexBufferGLES::Release()
{
#if defined(ANDROID) || defined(OSUNIX)
	free(data);
#endif

	glDeleteBuffers(1,&indexVBO);

	delete this;
}
