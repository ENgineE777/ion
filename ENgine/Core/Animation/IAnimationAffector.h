#pragma once

#include "Common/Common.h"

class IAnimationInstance;
class affecorDeclaratorBase;

#ifdef ANIM_AFFECT_PROP

#include "AffectorProperties.h"
#include "Utils/ClassDeclarant.h"
class IAnimationAffector : public IPropertyHolder

#else

class IAnimationAffector

#endif

{	
	IAnimationInstance* anim;

public:	
	
	IAnimationAffector()
	{
		m_pDesc = 0;
		anim = 0;
	}
	IAnimationAffector(affecorDeclaratorBase*  Desc)
	{
		m_pDesc = Desc;
		anim = 0;
	};

	virtual ~IAnimationAffector() {};

	virtual IAnimationInstance* Owner() { return anim; };
	virtual void SetOwner(IAnimationInstance* anm) { anim = anm; };
	virtual IAnimationInstance* GetOwner() { return anim; };
	virtual void TransformBone(int index, Matrix* parentMat, Matrix* curMat) = 0;	
	virtual void Update(float dt) {};
private:
	affecorDeclaratorBase*  m_pDesc;
};

#ifdef ANIM_AFFECT_PROP

ClassDeclarantBegin(affecorDeclarator,IAnimationAffector)
ClassDeclarantEnd(affecorDeclarator,IAnimationAffector)
	
#define AFFECTOR_DECL(className, name, level)\
DESC_BEGIN(affecorDeclarator,className, name, level)\
DESC_END(affecorDeclarator,className, name, level)

#endif


