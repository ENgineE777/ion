
#include "ItemRestore.h"

ENTITY(ItemRestore)
ENTITY_PARAMS(ItemRestore,"ItemRestore","Online", "Item Restore")

void ItemRestore::ItemRestoreListiner::OnItemRestoreSuccess(void* data, const char* id)
{
	((ItemRestore*)data)->RestoreSuccesed();
}

void ItemRestore::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( id )		
	PROP_REG( funcSuccessName )	
}

void ItemRestore::Init()
{		
}

bool ItemRestore::Start()
{
	ScriptFunction::Find(&callback_ok, Scene()->ScriptModule(), funcSuccessName.c_str());	

	return true;
}

void ItemRestore::Finish()
{
	callback_ok.Reset();	
}

CALLWRAPPER(ItemRestore, StartRestore, CALLWRAPPERNORET())	

void ItemRestore::MethodsToScript(const char* script_class_name)
{
	SceneEntity::MethodsToScript(script_class_name);

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void StartRestore()", CALLWRAPPERNAMEAS(StartRestore));
}

void ItemRestore::StartRestore()
{
	if (!id[0]) return;

	en_core.OnlineUserStats()->StartRestorePurchases(&listiner, this);
}

void ItemRestore::RestoreSuccesed()
{
	core->Script()->Machine()->Call(&callback_ok);	
}