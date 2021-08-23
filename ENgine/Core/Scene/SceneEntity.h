#pragma once

#include "Common/ClassDeclarant.h"
#include "Common/Common.h"
#include "SceneEntityDesc.h"
#include "Common/properties/Properties.h"
#include "SceneTaskLevels.h"
#include "ISceneInstance.h"

class SceneManager;

#ifdef PC
class Gizmo;
#endif

class DEF_EXPORT SceneEntity : public PropertiesOwner
{		
public:	
	
	bool    edited;
	bool	active;
	bool	show;
	bool    allowTasks;
	Matrix  transform;
	Vector  scale;
	Matrix  sv_transform;
	Vector  sv_scale;
	char    className[32];
	char    name[64];
	char    assetName[64];
	
	bool	activeOnStart;
	bool	showOnStart;

	bool    edit_mode;
	
	EntityDedeclarator* desc;

	ISceneInstance* scene;	

	STRING_PROP(SceneEntity, custom_data, "", "Properties", "Data")

#ifdef PC
	Gizmo*   gizmo;
#endif

	SceneEntity(EntityDedeclarator* dsc, ISceneInstance* scn);

	virtual EntityDedeclarator* Desc() const { return desc; };
	ISceneInstance*  Scene() { return scene; };

	virtual void Activate(bool set_active);
	virtual bool IsActive();

	virtual void Show(bool set_show);
	virtual bool IsVisible();

	virtual void RegisterProperties();

	virtual void Init();
	virtual void ReInit();	

	virtual bool Start();
	virtual void Finish();

	void SEGetData(std::string& val);	
	void SESetData(std::string& val);	

	virtual void SetTransform(const Matrix& mat);
	void GetTransform(Matrix& mat);

	virtual void SetScale(const Vector& scl);
	void GetScale(Vector& scl);
	
	void SetUpdate(TaskHolder::Task task, int level, float freq = -1.0f);
	void DelUpdate(TaskHolder::Task task);
	
	void RegisterInGroup(const char* name);
	void UnregisterInGroup(const char* name);

	const char* GetName() const;
	void  SetName(const char* nm);
    void  SetClassName(const char* nm);

	virtual void SetEditMode(bool ed);

	void SetAsset(SceneEntity* assest);

	virtual bool CheckClassName(const char* clsName);	

	virtual bool AllowRegisterSelfToScript();
	virtual void SelfToScript();
	virtual void PropertiesToScript(const char* script_class_name);
	virtual void MethodsToScript(const char* script_class_name);
	virtual void EmbedingToScript();		

	void GetTypeScript(string& name);

	virtual void SetTransformPos(Vector& pos);
	virtual void GetTransformPos(Vector& pos);

    virtual SceneEntity* Clone();

#ifdef EDITOR
	virtual void OnPropertiesChanged();
	virtual void SetGizmo(Gizmo* gz);
	virtual void DataToGizmo();
	virtual void DataFromGizmo();
	virtual void GetBoundingBox(BoundingBox& bbox);	
	virtual bool Select2D(float mx, float my, SceneEntity* cur_slection);
#endif	

	virtual void Release();
};