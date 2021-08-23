#pragma once
#include "Core/Core.h"

class ItemBuyer : public SceneEntity
{
	class ItemBuyListiner : public OnlineUserStatsSystem::ItemBuyListiner
	{
	public: 		

		virtual void OnItemBuySuccess(void* data);
		virtual void OnItemBuyFail(void* data);
	};

	ItemBuyListiner listiner;

	ScriptFunction callback_ok;
	ScriptFunction callback_fail;

public:
		
	PROP_START
	STRING_PROP(ItemBuyer, id, "", "Properties", "id_ios")
	STRING_PROP(ItemBuyer, id_android, "", "Properties", "id_android")
	STRING_PROP(ItemBuyer, funcSuccessName, "", "Properties", "funcSuccessName")
	STRING_PROP(ItemBuyer, funcFailName, "", "Properties", "funcFailName")

	ENTITY_DEFC(ItemBuyer)

	virtual void RegisterProperties();
	virtual void Init();	
	virtual bool Start();
	virtual void Finish();
	virtual void MethodsToScript(const char* script_class_name);

	void SetIosIdScript(const string& txt);
	void SetDroidIdScript(const string& txt);

	void StartPurchase();
	void PurchaseSuccesed();
	void PurchaseFailed();
};
