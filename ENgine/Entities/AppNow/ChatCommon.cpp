
#include "ChatCommon.h"

ChatCommandPool::ChatCommandPool(int size)
{
	slots_.resize(size);
}

ChatCommandSlot* ChatCommandPool::GetFreeSlot()
{
	for (int i = 0, size = slots_.size(); i < size; ++i)
	{
		if (slots_[i].free)
		{
			return &slots_[i];
		}
	}

	return null;
}

ChatCommandSlot* ChatCommandPool::GetUsedSlot()
{
	for (int i = 0, size = slots_.size(); i < size; ++i)
	{
		if (slots_[i].used)
		{
			return &slots_[i];
		}
	}

	return null;
}

