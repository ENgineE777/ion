
#pragma once

#include "Common/Common.h"

class ISceneInstance;
class SceneEntity;
class EntityDedeclarator;

class DEF_EXPORT EntityDedeclarator
{
	bool flag;

public:

	EntityDedeclarator();

	virtual SceneEntity* Create(ISceneInstance* scene) = 0;
	virtual const char* GetName() const
	{
		return "";
	};
	
	static EntityDedeclarator* GetFirst();	
	
	inline EntityDedeclarator* GetNext()
	{
		return next;
	};

	//properties	
	virtual bool  AllowMultipleNode() {return true;}	
	
	virtual bool  HasSize() {return true;}
	
	virtual bool  TempObject() {return false;}

	virtual bool  Is2D() {return false;}	

	virtual const char* GetNodePanelSheetName() const {return "";}
	virtual const char* GetNodePanelTexName() const {return "";}	

	virtual bool IsChecked() { return flag; };
	virtual void SetCheck(bool check) { flag=check; };

private:
	
	static EntityDedeclarator* first;
	EntityDedeclarator* next;		
};

#define ENTITY(nodeClass)\
class nodeClass##Decl : public EntityDedeclarator\
{\
public:\
	nodeClass##Decl(const char* name) : EntityDedeclarator()\
	{\
	};\
	virtual SceneEntity* Create(ISceneInstance* scene)\
	{\
		return new nodeClass (this, scene);\
	};


#define ENTITY_NOT_ALLOW_MULTIPLE_NODE\
	virtual bool  AllowMultipleNode() {return false;}

#define ENTITY_HASNT_SIZE\
	virtual bool  HasSize() {return false;}

#define ENTITY_IS_TEMP_OBJECT\
	virtual bool  TempObject() {return true;}

#define ENTITY_IS_2D_OBJECT\
	virtual bool  Is2D() {return true;}

#define ENTITY_TOOL_PANEL(FileName,ID)\
	virtual const char* GetToolPanelFileName() {return FileName;}\
	virtual const char* GetToolPanelID() {return ID;}

#define ENTITY_PARAMS(nodeClass,node_name,nodePanelSheetName, nodePanelTexName)\
	virtual const char* GetName() const {return node_name;}\
	virtual const char* GetNodePanelSheetName() const { return nodePanelSheetName; }\
	virtual const char* GetNodePanelTexName() const {return nodePanelTexName;}\
};\
nodeClass##Decl nodeClass##_decl("node");\
void nodeClass::SelfToScript()\
{\
	char str[256];\
	sprintf(str,"::"#nodeClass" %s", GetName());\
	core->Script()->Machine()->RegisterGlobalProperty (str, this);\
};\
extern void CALLWRAPPERNAME(IsVisible)(asIScriptGeneric *gen);\
extern void CALLWRAPPERNAME(Show)(asIScriptGeneric *gen);\
extern void CALLWRAPPERNAME(IsActive)(asIScriptGeneric *gen);\
extern void CALLWRAPPERNAME(Activate)(asIScriptGeneric *gen);\
extern void CALLWRAPPERNAME(SetTransformPos)(asIScriptGeneric *gen);\
extern void CALLWRAPPERNAME(GetTransformPos)(asIScriptGeneric *gen);\
extern void CALLWRAPPERNAME(GetTypeScript)(asIScriptGeneric *gen);\
extern void CALLWRAPPERNAME(SEGetData)(asIScriptGeneric *gen);\
extern void CALLWRAPPERNAME(SESetData)(asIScriptGeneric *gen);\
void nodeClass::PropertiesToScript(const char* script_class_name)\
{\
	core->Script()->Machine()->RegisterObjectType(script_class_name, sizeof(nodeClass), asOBJ_REF| asOBJ_NOCOUNT);\
	for (int i=0;i<(int)Properties().size();i++)\
	{\
		Properties()[i]->EmbedingScript(script_class_name);\
	}\
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool IsVisible()", CALLWRAPPERNAMEAS(IsVisible));\
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void Show(bool show)", CALLWRAPPERNAMEAS(Show));\
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "bool IsActive()", CALLWRAPPERNAMEAS(IsActive));\
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void Activate(bool Active)", CALLWRAPPERNAMEAS(Activate));\
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetTransformPos(Vector&in pos)", CALLWRAPPERNAMEAS(SetTransformPos));\
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetTransformPos(Vector&out pos)", CALLWRAPPERNAMEAS(GetTransformPos));\
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetType(string&out name)", CALLWRAPPERNAMEAS(GetTypeScript));\
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void GetData(string&out name)", CALLWRAPPERNAMEAS(SEGetData));\
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetData(string&in name)", CALLWRAPPERNAMEAS(SESetData));\
};

#define ENTITY_SCRIPTDEC \
virtual void SelfToScript();\
virtual void PropertiesToScript(const char* script_class_name);

#define ENTITY_PARAMS_WO_EMBEDING(nodeClass,node_name,nodePanelSheetName, nodePanelTexName)\
	virtual const char* GetName() const {return node_name;}\
	virtual const char* GetNodePanelSheetName() const { return nodePanelSheetName; }\
	virtual const char* GetNodePanelTexName() const {return nodePanelTexName;}\
};\
nodeClass##Decl nodeClass##_decl("node");

#define ENTITY_CONSTRUCT(nodeClass,parentClass)\
nodeClass(EntityDedeclarator* dsc, ISceneInstance* scene): parentClass(dsc, scene)\
{\
};

#define ENTITY_DEFC(nodeClass) ENTITY_CONSTRUCT(nodeClass,SceneEntity) \
ENTITY_SCRIPTDEC