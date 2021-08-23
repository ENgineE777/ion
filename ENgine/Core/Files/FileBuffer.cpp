#include "Core/Core.h"
#include "Common/Utils.h"
#include "FileBuffer.h"

FileBuffer::FileBuffer()
{
	pBuffer = null;
	lenght = 0;
}

bool FileBuffer::Init(const char* name)
{
	pBuffer = NULL;
	lenght = 0;	

	IFile* file = core->Files()->OpenFile(name, IFile::read);
	
	if (file) 
	{						
		lenght = file->GetSize();	

		pBuffer = new byte[lenght + 1];
		file->Read(pBuffer, lenght);
		pBuffer[lenght] = 0;

		file->Release();

		return true;
	}	

	return false;
}

byte* FileBuffer::GetBuffer()
{
	return pBuffer;
}

unsigned long FileBuffer::GetSize()
{
	return lenght;
}

void FileBuffer::Release()
{
	delete []pBuffer;

	delete this;
}
