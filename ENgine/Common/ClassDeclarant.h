#ifndef ClassDeclarant_H
#define ClassDeclarant_H

/* Sample of using

//Declare declarator
ClassDeclarant(declaratorName,baseClassName)

//Declaration of macros for registering class
#define DECLARATOR(className, name, level)	declaratorName<className> className##_Tmpl(name, level);

*/

#define ClassDeclarantBegin(declaratorName,className) class DEF_EXPORT declaratorName##Base\
{\
public:\
	declaratorName##Base(long lvl)\
	{\
		level = lvl;\
		if(first)\
		{\
			if(first->level > level)\
			{\
				next = first;\
				first = this;\
			}\
			else\
			{\
				declaratorName##Base * d;\
				for(d = first; d->next; d = d->next)\
				{\
					if(d->next->level > level)\
					{\
						next = d->next;\
						d->next = this;\
						return;\
					}\
				}\
				d->next = this;\
				next = 0;\
			}\
		}\
		else\
		{\
			first = this;\
			next = 0;\
		}\
	};\
	virtual ~declaratorName##Base() {};

#define ClassDeclarantEnd(declaratorName,className) \
	virtual className* Create() = 0;\
	inline const char* Name()\
	{\
		return dcl_name;\
	};\
	inline static declaratorName##Base* GetFirst()\
	{\
		return first;\
	};\
	inline declaratorName##Base* GetNext()\
	{\
		return next;\
	};\
	static declaratorName##Base* Find(const char* name)\
	{\
		for (declaratorName##Base* decl = declaratorName##Base::GetFirst(); decl != NULL; decl = decl->GetNext())\
		{\
		if (String::IsEqual(decl->Name(), name))\
			{\
				return decl;\
			}\
		}\
		return NULL;\
	}\
	static className* Create(const char* name)\
	{\
		declaratorName##Base* decl = Find(name);\
		if (decl)\
		{\
			return decl->Create();\
		}\
		return NULL;\
	}\
protected:\
	static declaratorName##Base* first;\
	declaratorName##Base* next;\
	const char* dcl_name;\
	long level;\
};

#define DESC_BEGIN(declaratorName,className, name, level)\
class declaratorName##className##Prop : public declaratorName##Base\
{\
public:\
	declaratorName##className##Prop(long lvl) : declaratorName##Base(lvl)\
{}


#define DESC_END(declaratorName,className, name, level) };\
	template<class T> class declaratorName##className : public declaratorName##className##Prop\
{\
public:\
	declaratorName##className(const char* nm, long lvl) : declaratorName##className##Prop(lvl)\
{\
	dcl_name = nm;\
}\
	virtual className* Create()\
{\
	/*return new T(this);*/\
	T* p;\
	NEW1(p,T,1,this);\
	return p;\
}\
};\
	declaratorName##className<className> className##_Tmpl(name, level);

#define DESC_END_WO_DESC(declaratorName,className, name, level) };\
	template<class T> class declaratorName##className : public declaratorName##className##Prop\
{\
public:\
	declaratorName##className(const char* nm, long lvl) : declaratorName##className##Prop(lvl)\
{\
	dcl_name = nm;\
}\
	virtual className* Create()\
{\
	return new T();\
}\
};\
	declaratorName##className<className> className##_Tmpl(name, level);


#endif



