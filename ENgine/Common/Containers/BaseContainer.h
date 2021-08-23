
#pragma once

#include "Common/Common.h"

class BaseContainer
{
protected:
	char file[256];
	int  line;
	void* Alloc(int size);
	void Dealloc(void* ptr);
};