
#include "NGUISymbol.h"
#include "NGUISymbolInstance.h"

extern void NGUIWidgetCastGeneric(asIScriptGeneric *gen);

void NGUISymbolInstanceGetProp(asIScriptGeneric* gen) 
{
    NGUISymbolInstance* symbol = (NGUISymbolInstance*)gen->GetObject();

    const char* str = &gen->GetFunction()->GetName()[4];	

    void* ptr = null;

    for (unsigned i=0;i<symbol->childs.size();i++)
    {
        SceneEntity* entity = symbol->childs[i];

        if (String::IsEqual(str, entity->GetName()))
        {
            ptr = entity;
            break;
        }
    }

    gen->SetReturnAddress(ptr);
}

void NGUISymbolInstanceSetProp(asIScriptGeneric* gen) 
{	
    DEBUG_BREAK
}

void NGUISymbolInstanceGetInst(asIScriptGeneric* gen)
{
    NGUISymbolInstance* symbol = (NGUISymbolInstance*)gen->GetObject();

    int index = gen->GetArgDWord(0);

    if (index < 0 || index >= (int)symbol->childs.size())
    {
        gen->SetReturnAddress(NULL);		
    }
    else
    {
        gen->SetReturnAddress(symbol->childs[index]);		
    }
}

void NGUISymbolAddInstance(asIScriptGeneric* gen)
{
    NGUISymbolInstance* symbol = (NGUISymbolInstance*)gen->GetObject();
    NGUIWidget* newWidget = symbol->AddInstance();
    gen->SetReturnAddress(newWidget);
}

ENTITY(NGUISymbol)
ENTITY_IS_2D_OBJECT
ENTITY_PARAMS_WO_EMBEDING(NGUISymbol,"NGUISymbol","NGUI", "Symbol")

//extern void CALLWRAPPERNAME(SInstAddInstance)(asIScriptGeneric *gen);
extern void CALLWRAPPERNAME(SInstDelInstance)(asIScriptGeneric *gen);
extern void CALLWRAPPERNAME(SInstResizeInstances)(asIScriptGeneric *gen);
extern void CALLWRAPPERNAME(SInstGetInstancesCount)(asIScriptGeneric *gen);
extern void CALLWRAPPERNAME(SGetSymbolNameScript)(asIScriptGeneric *gen);

void NGUISymbol::Init()
{	
	NGUIWidget::Init();	

	is_symbol = true;
	type_registered = false;
	NGUIService::instance->AddSymbolWidget(this);

	class_symbol[0] = 0;	
	class_symbol_inst[0] = 0;	
}
	
void NGUISymbol::ReInit()
{    
	NGUIWidget::ReInit();
}

bool NGUISymbol::Start()
{
	type_registered = false;
	return NGUIWidget::Start();
}

void NGUISymbol::RenderWidget()
{
#ifdef EDITOR
	AddDrawCall(NGUIService::instance->checker_tex, 0, 0, scr_w, scr_h, COLOR_WHITE, 0, 0, scr_w / 16.0f, scr_h / 16.0f);
#endif
}

void NGUISymbol::MethodsToScript(const char* script_class_name)
{
	NGUIWidget::MethodsToScript(script_class_name);				
}

bool NGUISymbol::CheckClassName(const char* clsName)
{
	if (String::IsEqual(clsName, "NGUISymbol"))
	{
		return true;
	}

	return false;
}

bool NGUISymbol::AllowMouseCover()
{
	return false;	
}

#if defined(PC) || defined(OSUNIX)
void NGUISymbol::Release()
{
	NGUIService::instance->DelSymbolWidget(this);
	NGUIWidget::Release();
}

Color NGUISymbol::GetDebugColor()
{
	return COLOR_GREEN;
}
#endif

void NGUISymbol::SymbolTypesToScript(NGUIWidget* child)
{
    if (child->childs.size() > 0)
    {
        char decl[128],tmp[128];

        String::Copy(tmp, 128, child->script_class_name);
        String::Printf(child->script_class_name, 128, "%s%s", child->Desc()->GetName(), child->GetName());

        if (core->Script()->Machine()->GetTypeIdByDecl(child->script_class_name) == asINVALID_TYPE)
        {	
            child->PropertiesToScript(child->script_class_name);
            child->MethodsToScript(child->script_class_name);				

			char str[128];
			sprintf(str, "%s@ opImplCast()", child->Desc()->GetName());
			
			core->Script()->Machine()->RegisterObjectMethod(child->script_class_name, str, asFUNCTION(NGUIWidgetCastGeneric));

            for (unsigned i=0;i<child->childs.size();i++)
            {
                if (String::IsEqual(child->childs[i]->Desc()->GetName(), "NGUISymbolInstance"))
                {
                    NGUISymbolInstance* inst = (NGUISymbolInstance*)child->childs[i];
                    if(inst->wgt_symbol)
                    {
                        inst->wgt_symbol->SymbolTypesToScript();
                        if (inst->useInstance)
                        {
                            String::Printf(decl, 128, "Symbol%s@ get_%s()", inst->wgt_symbol->GetName(), inst->GetName());
                            core->Script()->Machine()->RegisterObjectMethod(child->script_class_name, decl, asFUNCTION(NGUISymbolInstanceGetProp));

                            String::Printf(decl, 128, "void set_%s(Symbol%s@ value)", inst->GetName(), inst->wgt_symbol->GetName());
                            core->Script()->Machine()->RegisterObjectMethod(child->script_class_name, decl, asFUNCTION(NGUISymbolInstanceSetProp));

                        }
                        else
                        {
                            String::Printf(decl, 128, "SymbolInst%s@ get_%s()", inst->wgt_symbol->GetName(), inst->GetName());
                            core->Script()->Machine()->RegisterObjectMethod(child->script_class_name, decl, asFUNCTION(NGUISymbolInstanceGetProp));

                            String::Printf(decl, 128, "void set_%s(SymbolInst%s@ value)", inst->GetName(), inst->wgt_symbol->GetName());
                            core->Script()->Machine()->RegisterObjectMethod(child->script_class_name, decl, asFUNCTION(NGUISymbolInstanceSetProp));
                        }
                    }
                }
                else
                {
                    if (child->childs[i]->childs.size() > 0)
                    {
                        SymbolTypesToScript(child->childs[i]);

                        String::Printf(decl, 128, "%s%s@ get_%s()", child->childs[i]->Desc()->GetName(), child->childs[i]->GetName(), child->childs[i]->GetName());
                        core->Script()->Machine()->RegisterObjectMethod(child->script_class_name, decl, asFUNCTION(NGUISymbolInstanceGetProp));

                        String::Printf(decl, 128, "void set_%s(%s%s@ value)", child->childs[i]->GetName(), child->childs[i]->Desc()->GetName(), child->childs[i]->GetName());
                        core->Script()->Machine()->RegisterObjectMethod(child->script_class_name, decl, asFUNCTION(NGUISymbolInstanceSetProp));
                    }
                    else
                    {
                        String::Printf(decl, 128, "%s@ get_%s()", child->childs[i]->script_class_name, child->childs[i]->GetName());
                        core->Script()->Machine()->RegisterObjectMethod(child->script_class_name, decl, asFUNCTION(NGUISymbolInstanceGetProp));

                        String::Printf(decl, 128, "void set_%s(%s@ value)", child->childs[i]->GetName(), child->childs[i]->script_class_name);
                        core->Script()->Machine()->RegisterObjectMethod(child->script_class_name, decl, asFUNCTION(NGUISymbolInstanceSetProp));
                    }		
                }
            }
        }
        String::Copy(child->script_class_name, 128, tmp);	
    }
}
#if defined(PC) || defined(OSUNIX)
void NGUISymbol::GetMembersForAutoComplete(bool bSymbol, std::vector<string>& vMembers)
{
    if(bSymbol)
    {
        vMembers.push_back("AddInstance");
        vMembers.push_back("GetInstance");
        vMembers.push_back("DelInstance");
        vMembers.push_back("ResizeInstances");
        vMembers.push_back("GetInstancesCount");
    }
    else
        vMembers.push_back("GetSymbolName");
    NGUIWidget::GetMembersForAutoComplete(vMembers);
}
void NGUISymbol::GetSignatureForAutoComplete(bool bSymbol,const string& sName,const string& sMethod,std::vector<string>& vSignatures)
{
    NGUIWidget::GetSignatureForAutoComplete(sMethod,vSignatures);
    if(vSignatures.empty())
    {
        if(bSymbol)
        {
            char decl[128];
            char class_symbol_inst[128];
            String::Copy(class_symbol_inst, 128, "SymbolInst");
            String::Cat(class_symbol_inst, 128,  sName.c_str());
            if(sMethod == "AddInstance")
            {
                 String::Printf(decl, 128, "%s@ AddInstance()", class_symbol_inst);
                 vSignatures.push_back(decl);
            }
            else 
                if(sMethod == "GetInstance")
                {
                    String::Printf(decl, 128, "%s@ GetInstance(int index)", class_symbol_inst);
                    vSignatures.push_back(decl);

                }
                else 
                    if(sMethod == "DelInstance")vSignatures.push_back("void DelInstance(int index)");
                    else 
                        if(sMethod == "ResizeInstances")vSignatures.push_back("void ResizeInstances(int count)");
                        else 
                            if(sMethod == "GetInstancesCount")vSignatures.push_back("int GetInstancesCount()");
        }
        else
          if(sMethod == "GetSymbolName")vSignatures.push_back("void GetSymbolName(string& name)");
    }
}

#endif

void NGUISymbol::SymbolTypesToScript()
{	
    if (type_registered)
	{
        return;
	}

    for (unsigned i=0;i< childs.size();i++)
    {
        if (String::IsEqual(childs[i]->Desc()->GetName(), "NGUISymbolInstance"))
        {
             if(((NGUISymbolInstance*)childs[i])->wgt_symbol)
			 {
                ((NGUISymbolInstance*)childs[i])->wgt_symbol->SymbolTypesToScript();
			 }
             else
			 {
                 core->TraceTo("Script","Error in binding symbol instance %s methods to script machine, miss symbol widget !!! ",childs[i]->GetName());
			 }
        }
        else
		{
            SymbolTypesToScript(childs[i]);
		}
    }	

    char decl[128];
    
    String::Copy(class_symbol, 128, "Symbol");
    String::Cat(class_symbol, 128,  GetName());
    
    String::Copy(class_symbol_inst, 128, "SymbolInst");
    String::Cat(class_symbol_inst, 128,  GetName());	

    FullMethodsToScript(class_symbol_inst);

    core->Script()->Machine()->RegisterObjectMethod(class_symbol_inst, "void GetSymbolName(string& name)", CALLWRAPPERNAMEAS(SGetSymbolNameScript));

    for (unsigned i=0;i<childs.size();i++)
    {			
        NGUIWidget* owner = childs[i];

        if (String::IsEqual(childs[i]->Desc()->GetName(), "NGUISymbolInstance"))
        {
            NGUISymbolInstance* inst = (NGUISymbolInstance*)childs[i];
            if(inst->wgt_symbol)
            {
                inst->wgt_symbol->SelfToScript();
                if (inst->useInstance)
                {
                    String::Printf(decl, 128, "Symbol%s@ get_%s()", inst->wgt_symbol->GetName(), owner->GetName());
                    core->Script()->Machine()->RegisterObjectMethod(class_symbol_inst, decl, asFUNCTION(NGUISymbolInstanceGetProp));

                    String::Printf(decl, 128, "void set_%s(Symbol%s@ value)", owner->GetName(), inst->wgt_symbol->GetName());
                    core->Script()->Machine()->RegisterObjectMethod(class_symbol_inst, decl, asFUNCTION(NGUISymbolInstanceSetProp));
                }
                else
                {
                    String::Printf(decl, 128, "SymbolInst%s@ get_%s()", inst->wgt_symbol->GetName(), owner->GetName());
                    core->Script()->Machine()->RegisterObjectMethod(class_symbol_inst, decl, asFUNCTION(NGUISymbolInstanceGetProp));

                    String::Printf(decl, 128, "void set_%s(SymbolInst%s@ value)", owner->GetName(), inst->wgt_symbol->GetName());
                    core->Script()->Machine()->RegisterObjectMethod(class_symbol_inst, decl, asFUNCTION(NGUISymbolInstanceSetProp));
                }
            }
            else
                core->TraceTo("ScriptErr","Error in binding symbol instance %s methods to script machine, miss symbol widget !!! ",owner->GetName());

        }
        else
        {
            if (owner->childs.size() > 0)
            {
                char tmp[128];				
                String::Printf(tmp, 128, "%s%s", owner->Desc()->GetName(), owner->GetName());

                String::Printf(decl, 128, "%s::%s@ get_%s()", Scene()->GetName(), tmp, owner->GetName());
                core->Script()->Machine()->RegisterObjectMethod(class_symbol_inst, decl, asFUNCTION(NGUISymbolInstanceGetProp));

                String::Printf(decl, 128, "void set_%s(%s::%s@ value)", owner->GetName(), Scene()->GetName(), tmp);
                core->Script()->Machine()->RegisterObjectMethod(class_symbol_inst, decl, asFUNCTION(NGUISymbolInstanceSetProp));
            }
            else
            {
                String::Printf(decl, 128, "%s@ get_%s()", owner->script_class_name, owner->GetName());
                core->Script()->Machine()->RegisterObjectMethod(class_symbol_inst, decl, asFUNCTION(NGUISymbolInstanceGetProp));

                String::Printf(decl, 128, "void set_%s(%s@ value)", owner->GetName(), owner->script_class_name);
                core->Script()->Machine()->RegisterObjectMethod(class_symbol_inst, decl, asFUNCTION(NGUISymbolInstanceSetProp));
            }				
        }
    }

    FullMethodsToScript(class_symbol);	

    String::Printf(decl, 128, "%s@ GetInstance(int index)", class_symbol_inst);

    core->Script()->Machine()->RegisterObjectMethod(class_symbol, decl, asFUNCTION(NGUISymbolInstanceGetInst));		

    String::Printf(decl, 128, "%s@ AddInstance()", class_symbol_inst);

    core->Script()->Machine()->RegisterObjectMethod(class_symbol, decl, asFUNCTION(NGUISymbolAddInstance));

    core->Script()->Machine()->RegisterObjectMethod(class_symbol, "void DelInstance(int index)", CALLWRAPPERNAMEAS(SInstDelInstance));
    core->Script()->Machine()->RegisterObjectMethod(class_symbol, "void ResizeInstances(int count)", CALLWRAPPERNAMEAS(SInstResizeInstances));
    core->Script()->Machine()->RegisterObjectMethod(class_symbol, "int GetInstancesCount()", CALLWRAPPERNAMEAS(SInstGetInstancesCount));	

	asIScriptEngine* engine = ((ScriptMachine*)(core->Script()->Machine()))->Machine();
	
	if (engine->GetDefaultNamespace()[0])
	{
		sprintf(class_symbol, "%s::Symbol", engine->GetDefaultNamespace());
	}
	else
	{
		String::Copy(class_symbol, 128, "Symbol");
	}
	
	String::Cat(class_symbol, 128,  GetName());
	
	if (engine->GetDefaultNamespace()[0])
	{
		sprintf(class_symbol_inst, "%s::SymbolInst", engine->GetDefaultNamespace());
	}
	else
	{
		String::Copy(class_symbol_inst, 128, "SymbolInst");
	}
	
	String::Cat(class_symbol_inst, 128,  GetName());	

    type_registered = true;

}

void NGUISymbol::PropertiesToScript(const char* script_class_name)
{

}

void NGUISymbol::SelfToScript()
{
   SymbolTypesToScript();
}

void NGUISymbol::EmbedingToScript()
{

}

#if defined(PC) || defined(OSUNIX)
void NGUISymbol::CopyProperties(NGUIWidget* pDesc)
{
    if(pDesc)
    {
        width = pDesc->width;
        height = pDesc->height;
        rot_x = pDesc->rot_x;
        rot_y = pDesc->rot_y;
        rot_z = pDesc->rot_z;
        scaleChilds = pDesc->scaleChilds;
        alignByParent = pDesc->alignByParent;
        horzAlign = pDesc->horzAlign;
        vertAlign = pDesc->vertAlign;
        weight = pDesc->weight;
        clipChilds = pDesc->clipChilds;
        text_align = pDesc->text_align;
        vtext_align = pDesc->vtext_align;
        overColor = pDesc->overColor;
        alpha = pDesc->alpha;
        anchor_x = pDesc->anchor_x;
        anchor_y = pDesc->anchor_y;
        abs_anchor = pDesc->abs_anchor;
        cellMove = pDesc->cellMove;
        allow_mouse_cover = pDesc->allow_mouse_cover;
        childs_names.assign(pDesc->childs_names.begin(),pDesc->childs_names.end());

        for (int i=0;i<pDesc->childs.size();i++)
            AddChild(pDesc->childs[i]);

        CalcColors();
        UpdateChildsPos();
    }
}
#endif