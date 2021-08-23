
#pragma once

class IAnimationEvents
{
public:	
	
	virtual ~IAnimationEvents() {};

	virtual void ProcessEvent(const char* event,const char* param) {};
	virtual void OnActiveDefaultLink(const char* destNode) {};
};
