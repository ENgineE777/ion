#include "Core/Core.h"

#ifdef EDITOR
#include "Editor\Helpers\Gizmo.h"
#include "Editor\Editor.h"
#endif

CALLWRAPPER(SceneEntity, IsVisible, CALLWRAPPERRET(bool))
CALLWRAPPERARG1(SceneEntity, Show, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())
CALLWRAPPER(SceneEntity, IsActive, CALLWRAPPERRET(bool))
CALLWRAPPERARG1(SceneEntity, Activate, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(SceneEntity, SetTransformPos, CALLWRAPPERGETADRESS(Vector,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(SceneEntity, GetTransformPos, CALLWRAPPERGETADRESS(Vector,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(SceneEntity, GetTypeScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(SceneEntity, SEGetData, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(SceneEntity, SESetData, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())

SceneEntity::SceneEntity(EntityDedeclarator* dsc, ISceneInstance* scn)
{
	desc = dsc;

	scene = scn;
	edit_mode = false;
	transform.Identity();
	name[0] = 0;
	assetName[0] = 0;
	className[0] = 0;

	scale = 1.0;
#ifdef PC
	gizmo = null;
#endif

	activeOnStart = true;
	showOnStart = true;

	active = true;
	show = true;

	allowTasks = true;
}

void SceneEntity::Activate(bool set_active)
{
	active = set_active;
}

bool SceneEntity::IsActive()
{
	return active;
}

void SceneEntity::Show(bool set_show)
{
	show = set_show;
}

bool SceneEntity::IsVisible()
{
	if (!Scene()->IsRunning())
	{
		return showOnStart;
	}

	return show;
}

void SceneEntity::RegisterProperties()
{
	PROP_REG( custom_data )
}

void SceneEntity::Init()
{		
	
}

void SceneEntity::ReInit()
{
}

#ifdef EDITOR
void SceneEntity::OnPropertiesChanged()
{
	ReInit();
	DataToGizmo();
    ProjectInfo::Scene* pScene = core_ed.GetSceneInfo();
    if(pScene)
        pScene->OnPropertiesChanged(this);

}
#endif

bool SceneEntity::Start()
{
	return true;
}

void SceneEntity::Finish()
{

}

void SceneEntity::SEGetData(std::string& val)
{
	val = custom_data;
}

void SceneEntity::SESetData(std::string& val)
{
	custom_data.assign(val.c_str());
}

void SceneEntity::SetTransform(const Matrix& mat)
{
	transform = mat;
}

void SceneEntity::GetTransform(Matrix& mat)
{
	mat = transform;
}

void SceneEntity::SetScale(const Vector& scl)
{
	scale = scl;
}

void SceneEntity::GetScale(Vector& scl)
{
	scl = scale;
}

void SceneEntity::SetUpdate(TaskHolder::Task task, int level, float freq)
{
	if (!allowTasks || !Scene()) return;

	if (!Scene()->IsLoading())
	{
		core->AddTask(this,Scene(), task, level, freq);
	}
	else
	{
		Scene()->AddEntityUpdate(this,task, level, freq);
	}
}

void SceneEntity::DelUpdate(TaskHolder::Task task)
{
	if (!Scene()) return;

	if (!Scene()->IsLoading())
	{
		core->DelTask(this,task);
	}
	else
	{
		Scene()->DelEntityUpdate(this,task);
	}
}

void SceneEntity::RegisterInGroup(const char* name)
{
	Scene()->RegisterEntityInGroup(this, name);
}

void SceneEntity::UnregisterInGroup(const char* name)
{
	Scene()->UnregisterEntityInGroup(this, name);
}

const char* SceneEntity::GetName() const
{
	return name;
}

void SceneEntity::SetName(const char* nm)
{
	String::Copy(name,64,nm);	
}

void  SceneEntity::SetClassName(const char* nm)
{
    String::Copy(className,32,nm);	
}

void SceneEntity::SetEditMode(bool ed)
{
	edit_mode = ed;
}

void SceneEntity::SetAsset(SceneEntity* assest)
{
	String::Copy(assetName, 63, assest->GetName());
	assest->Copy(this);
	ReInit();
}

bool SceneEntity::CheckClassName(const char* clsName)
{
	if (String::IsEqual(clsName, className))
	{
		return true;
	}

	return false;
}

bool SceneEntity::AllowRegisterSelfToScript()
{	
	return true;
}

void SceneEntity::SelfToScript()
{	
}

void SceneEntity::PropertiesToScript(const char* script_class_name)
{	
}

void SceneEntity::MethodsToScript(const char* script_class_name)
{	
}

void SceneEntity::EmbedingToScript()
{
	if (!Desc()->IsChecked())
	{
		PropertiesToScript(Desc()->GetName());
		MethodsToScript(Desc()->GetName());
		Desc()->SetCheck(true);
	}

	//SelfToScript();
}

void SceneEntity::GetTypeScript(string& name)
{
	name = desc->GetName();
}

void SceneEntity::SetTransformPos(Vector& pos)
{
	transform.Pos() = pos;
}
void SceneEntity::GetTransformPos(Vector& pos)
{
	pos = transform.Pos();
}

#ifdef EDITOR

void SceneEntity::SetGizmo(Gizmo* gz)
{
	if (gizmo) gizmo->enabled = false;
	gizmo = gz;
	if (gizmo) gizmo->enabled = true;
}

void SceneEntity::DataToGizmo()
{
	if (!gizmo) return;
	gizmo->transform = transform;
}

void SceneEntity::DataFromGizmo()
{
	if (!gizmo) return;
	transform = gizmo->transform;
}

void SceneEntity::GetBoundingBox(BoundingBox& box)
{
	box.minV = -0.5f;
	box.maxV = -0.5f;
}

bool SceneEntity::Select2D(float mx, float my, SceneEntity* cur_slection)
{
	return false;
}

#endif

void SceneEntity::Release()
{	
	delete this;
}

SceneEntity* SceneEntity::Clone()
{
    SceneEntity* pClone = Desc()->Create(scene);
    pClone->allowTasks = false;
    pClone->Init();		
    Copy(pClone);
    pClone->SetName(name);
    String::Copy(pClone->className,32,className);
    return pClone;
}
