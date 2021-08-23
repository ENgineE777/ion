
#pragma once
#include "Core\Scene\SceneEntityDesc.h"

#ifdef EDITOR
struct DeclarationsDescriptor
{
private:
	string                            sName;
	std::vector<EntityDedeclarator*>  vDescriptors;  

	inline void Copy(const DeclarationsDescriptor& other)
	{
		sName = other.sName;
		vDescriptors.assign(other.vDescriptors.begin(),other.vDescriptors.end());
	}
	struct DescriptorsComparator 
	{
		bool operator() (const EntityDedeclarator* i,const EntityDedeclarator* j) { return String::CompareAlphapedOrder(i->GetName(),j->GetName());}
	};

public:


	DeclarationsDescriptor(const string& _sName):sName(_sName){}
	DeclarationsDescriptor(const DeclarationsDescriptor& other){Copy(other);}
	DeclarationsDescriptor& operator = (const DeclarationsDescriptor& other){Copy(other);return *this;}
	EntityDedeclarator* operator [] (const unsigned  index)const {return vDescriptors[index];}
	inline const char* GetName() const {return sName.c_str();}
	inline unsigned Size() const {return vDescriptors.size();}
	inline void Add(EntityDedeclarator* pDesc) {return vDescriptors.push_back(pDesc);}
	inline void Sort() 
	{
		if(vDescriptors.size() > 1)
			std::sort(vDescriptors.begin(),vDescriptors.end(),DescriptorsComparator());
	}
	struct Comparator 
	{
		bool operator() (const DeclarationsDescriptor& i,const DeclarationsDescriptor& j) { return String::CompareAlphapedOrder(i.GetName(),j.GetName());}
	};
};
typedef std::vector<DeclarationsDescriptor> TDeclarationsDescriptors;

#endif