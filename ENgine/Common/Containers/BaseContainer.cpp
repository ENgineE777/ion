
#include "Common/Common.h"
#include "BaseContainer.h"
#include "Core/ICore.h"

void* BaseContainer::Alloc(int size)
{
	float f = ::sqrtf(1.0f);
	
	//return core->AllocMem(size, 1, file, line);	
	return malloc(size);	
}

void BaseContainer::Dealloc(void* ptr)
{
	//core->FreeMem(ptr);	
	free(ptr);
}