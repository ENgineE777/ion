
#include "SceneEntityDesc.h"

EntityDedeclarator* EntityDedeclarator::first = 0;

EntityDedeclarator::EntityDedeclarator()
{	
	if (first)
	{
		EntityDedeclarator* d;
		for(d = first; d->next; d = d->next)
		{
		}

		d->next = this;		
	}
	else
	{
		first = this;		
	}

	next = null;			
	flag = false;
};

EntityDedeclarator* EntityDedeclarator::GetFirst()
{
	return first;
};