
#include "NGUISymbolInstance.h"

ENTITY(NGUISymbolInstance)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS_WO_EMBEDING(NGUISymbolInstance,"NGUISymbolInstance","NGUI", "SymbolInstance")

void NGUISymbolInstance::RegisterProperties()
{
	NGUIWidget::RegisterProperties();	

	PROP_REG(symbol_name)
	PROP_REG(useInstance)
}

void NGUISymbolInstance::Init()
{	
	NGUIWidget::Init();

	is_symbol = false;
	wgt_symbol = null;
	childs_are_proxy = false;
    type_registered = false;
	bAbstarct = false;
}
	
void NGUISymbolInstance::ReInit()
{	

    if (!childs_are_proxy)
    {
        int count =  (int)childs.size() - (int)childs_names.size();
        std::vector<NGUIWidget*> vToDelete;
        for (int i = 0; i < count; i++)vToDelete.push_back(childs[i]);
        for (int i = 0; i < count; i++)DeleteAllChilds(vToDelete[i]);		
    }
    childs.clear();

#ifdef EDITOR
	if (ed_rollout)
		get_symbol_nameprop_inst()->SetSkipWidget(ed_rollout, true);
#endif

	SceneEntity* wgt = NGUIService::instance->FindSymbolWidget(Scene(), symbol_name.c_str());	

	if (wgt)
	{		
		wgt_symbol = (NGUISymbol*)wgt;		

		if (!useInstance || (useInstance && core->IsEditorMode()))
		{
			childs_are_proxy = false;
			InstanceSymbol(wgt_symbol, this);			
		}
	}
	else
	{
		if(wgt_symbol)
			wgt_symbol->Release();
		wgt_symbol = NULL;
#ifdef EDITOR
		wgt_symbol = static_cast<NGUISymbol*>(core->Scene()->CreateEntity(Scene(),"NGuiSymbol",false));
		if (wgt_symbol)
		{
			wgt_symbol->SetName(symbol_name.c_str());
			bAbstarct = true;
		}
#endif // EDITOR

	}

	NGUIWidget::ReInit();
	UpdateChildsPos();
}

bool NGUISymbolInstance::Start()
{
	if (!wgt_symbol)
	{
		core->TraceTo("SceneErr", "Error: instance %s linked to symbol %s that do not exist", GetName(), symbol_name.c_str());
		return false;
	}

	return NGUIWidget::Start();
}

void NGUISymbolInstance::SGetSymbolNameScript(string& name)
{
	name = symbol_name;
}

void NGUISymbolInstance::InstanceSymbol(NGUIWidget* symbol, NGUIWidget* parent)
{
	for (int i=0;i<symbol->childs_names.size();i++)
	{
        SceneEntity* wgt_chld = null;
		
		if (symbol->childs.size() == 0)
		{
            wgt_chld = Scene()->Find(symbol->childs_names[i].data, "NGUIWidget");
		}
		else
		{
            if(i < symbol->childs.size())
                wgt_chld = symbol->childs[i];
		}

		if (wgt_chld)
		{
			NGUIWidget* wgt_tmp = (NGUIWidget*)wgt_chld->Desc()->Create(Scene());						

			wgt_tmp->Init();
			//wgt_tmp->TieValues();						
			wgt_chld->Copy(wgt_tmp);		
			wgt_tmp->SetName(wgt_chld->GetName());
            wgt_tmp->SetClassName(wgt_chld->className);
			wgt_tmp->ReInit();		

			parent->AddChild(wgt_tmp);

            InstanceSymbol((NGUIWidget*)wgt_chld, wgt_tmp);
		}
	}
}

void NGUISymbolInstance::DeleteAllChilds(NGUIWidget* entity)
{		
	for (int i=0;i<entity->childs.size();i++)
	{
	/*	if (entity->childs[i]->CheckClassName("NGUISymbolInstance"))
		{
			core->DelTasks(entity->childs[i]);			
			entity->childs[i]->Release();
		}
		else*/
		{
			DeleteAllChilds(entity->childs[i]);		
		}

		i--;
	}

	core->DelTasks(entity);			
	entity->Release();
}

void NGUISymbolInstance::Release()
{		
    int count =  (int)childs.size() - (int)childs_names.size();
    std::vector<NGUIWidget*> vToDelete;
    for (int i = 0; i < count; i++)vToDelete.push_back(childs[i]);
    for (int i = 0; i < count; i++)DeleteAllChilds(vToDelete[i]);	
	if(bAbstarct)
	{
		if(wgt_symbol)
			wgt_symbol->Release();
	}
	NGUIWidget::Release();
}

void NGUISymbolInstance::EmbedingToScript()
{
}

void NGUISymbolInstance::PropertiesToScript(const char* script_class_name)
{
}


CALLWRAPPERARG1(NGUISymbolInstance, SInstDelInstance, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUISymbolInstance, SInstResizeInstances, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERNORET())
CALLWRAPPER(NGUISymbolInstance, SInstGetInstancesCount, CALLWRAPPERRET(int))
CALLWRAPPERARG1(NGUISymbolInstance, SGetSymbolNameScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())

void NGUISymbolInstance::SelfToScript()
{
    if (type_registered)
        return;
    if(wgt_symbol)
    {
	    wgt_symbol->SymbolTypesToScript();
	    char decl[128];
	    char class_symbol_inst[128];
	    String::Copy(class_symbol_inst, 128, "SymbolInst");
	    String::Cat(class_symbol_inst, 128, wgt_symbol->GetName());
        //String::Copy(script_class_name, 128, class_symbol_inst);
	    if (useInstance)
	    {				
            char class_symbol[128];
            String::Copy(class_symbol, 128, "Symbol");
            String::Cat(class_symbol, 128, wgt_symbol->GetName());
		    String::Printf(decl,128,"%s %s", class_symbol, GetName());
		    core->Script()->Machine()->RegisterGlobalProperty(decl, this);
	    }
	    else
	    {		
		    String::Printf(decl,128,"%s %s", class_symbol_inst, GetName());
		    core->Script()->Machine()->RegisterGlobalProperty(decl, this);
	    }
        childs_are_proxy = true;
    }
    type_registered = true;
}

NGUIWidget* NGUISymbolInstance::AddInstance()
{
 	NGUIWidget* wgt_tmp_prnt = (NGUIWidget*)Desc()->Create(Scene());
			
	wgt_tmp_prnt->Init();
	//wgt_tmp_prnt->TieValues();		
	Copy(wgt_tmp_prnt);
	wgt_tmp_prnt->ReInit();	

	InsertChild(wgt_tmp_prnt,childs.size() - childs_names.size());

	InstanceSymbol(wgt_symbol, wgt_tmp_prnt);

	if (Scene()->IsRunning())
	{
		wgt_tmp_prnt->Start();
	}

	wgt_tmp_prnt->UpdateChildsPos();
    return wgt_tmp_prnt;
}

void NGUISymbolInstance::SInstDelInstance(int index)
{
	if (index < 0 || index >= childs.size())
	{
		return;
	}
	
	DeleteAllChilds(childs[index]);		
}

void NGUISymbolInstance::SInstResizeInstances(int count)
{
	if (count < 0)
	{
		return;
	}

	if (count > childs.size())
	{
		int num = count - childs.size();

		for (int i=0;i<num;i++)
		{
			AddInstance();			
		}
	}
	else
	if (count < childs.size())
	{		
		for (int i=childs.size() - 1;i>=count;i--)
		{
			DeleteAllChilds(childs[i]);			
		}
	}
}

int NGUISymbolInstance::SInstGetInstancesCount()
{
	return childs.size();
}

void NGUISymbolInstance::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);	
}

bool NGUISymbolInstance::AllowMouseCover()
{
	return false;	
}

#if defined(PC) || defined(OSUNIX)
Color NGUISymbolInstance::GetDebugColor()
{
	return COLOR_GREEN;
}

void NGUISymbolInstance::CopyProperties(NGUIWidget* symbol)
{
	if(symbol)
	{
		width = symbol->width;
		height = symbol->height;
		alpha = symbol->alpha;
		overColor = symbol->overColor;
	}
}

#endif