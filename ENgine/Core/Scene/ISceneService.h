#pragma once

#include "Common/ClassDeclarant.h"
#include "Common/Common.h"
#include "Common/TaskHolder.h"

class DEF_EXPORT ISceneService : public TaskHoldersOwner
{		
public:

	virtual void Init() = 0;	
	virtual void MethodsToScript() {};
};

ClassDeclarantBegin(SSDecl, ISceneService)
ClassDeclarantEnd(SSDecl, ISceneService)

#define SCENESERVICE(className, name, level)\
	DESC_BEGIN(SSDecl,className,name,level)\
	DESC_END_WO_DESC(SSDecl,className,name,level)