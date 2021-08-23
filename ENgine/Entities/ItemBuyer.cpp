
#include "ItemBuyer.h"

ENTITY(ItemBuyer)
ENTITY_PARAMS(ItemBuyer,"ItemBuyer","Online", "Item Buyer")

void ItemBuyer::ItemBuyListiner::OnItemBuySuccess(void* data)
{
	((ItemBuyer*)data)->PurchaseSuccesed();
}

void ItemBuyer::ItemBuyListiner::OnItemBuyFail(void* data)
{
	((ItemBuyer*)data)->PurchaseFailed();
}

void ItemBuyer::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( id )		
	PROP_REG( id_android )

	PROP_REG( funcSuccessName )
	PROP_REG( funcFailName )
}

void ItemBuyer::Init()
{			
}

CALLWRAPPER(ItemBuyer, StartPurchase, CALLWRAPPERNORET())	
CALLWRAPPERARG1(ItemBuyer, SetIosIdScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	
CALLWRAPPERARG1(ItemBuyer, SetDroidIdScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())	

bool ItemBuyer::Start()
{
	ScriptFunction::Find(&callback_ok, Scene()->ScriptModule(), funcSuccessName.c_str());
	ScriptFunction::Find(&callback_fail, Scene()->ScriptModule(), funcFailName.c_str());

	return true;
}

void ItemBuyer::Finish()
{
	callback_ok.Reset();
	callback_fail.Reset();
}

void ItemBuyer::MethodsToScript(const char* script_class_name)
{
	SceneEntity::MethodsToScript(script_class_name);

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void StartPurchase()", CALLWRAPPERNAMEAS(StartPurchase));

	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetIosId(string&in name)", CALLWRAPPERNAMEAS(SetIosIdScript));
	core->Script()->Machine()->RegisterObjectMethod(script_class_name, "void SetDroidId(string&in name)", CALLWRAPPERNAMEAS(SetDroidIdScript));	
}

void ItemBuyer::StartPurchase()
{
	if (!id[0]) return;

	if (en_core.GetCurrentPlatform() == platform_ios)
	{
		en_core.OnlineUserStats()->StartItemBuy(&listiner, this, id.c_str());
	}
	else
	if (en_core.GetCurrentPlatform() == platform_android)
	{
		en_core.OnlineUserStats()->StartItemBuy(&listiner, this, id_android.c_str());
	}
	else	
	{
		en_core.OnlineUserStats()->StartItemBuy(&listiner, this, id.c_str());
	}
}

void ItemBuyer::SetIosIdScript(const string& txt)
{
	id = txt;
}

void ItemBuyer::SetDroidIdScript(const string& txt)
{
	id_android = txt;
}

void ItemBuyer::PurchaseSuccesed()
{
	core->Script()->Machine()->Call(&callback_ok);	
}

void ItemBuyer::PurchaseFailed()
{
	core->Script()->Machine()->Call(&callback_fail);	
}