#pragma once

#include "Common/Common.h"
#include "Core/Network/NetworkDataBuffer.h"

class ChatCommandSlot
{
public:
	ChatCommandSlot() : used(false), free(true) {}

	bool used;
	bool free;
	std::string command;
};

class ChatCommandPool
{
public:
	ChatCommandPool(int size);

	ChatCommandSlot* GetFreeSlot();
	ChatCommandSlot* GetUsedSlot();

private:
	std::vector<ChatCommandSlot> slots_;
};
