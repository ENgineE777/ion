
#include "NGUIWidget.h"
#include "Editor/Helpers/Gizmo.h"
#include "Common/Utils.h"
#include "NGUISymbolInstance.h"

NGUIWidget* NGUIWidget::focusedWidget = null;
NGUIWidget* NGUIWidget::mscover_widget = null;
NGUIWidget* NGUIWidget::mscover_widget_not_covered = null;
float NGUIWidget::ms_pos_x = 0.0f;
float NGUIWidget::ms_pos_y = 0.0f;
float NGUIWidget::ms_prev_pos_x = 0.0f;
float NGUIWidget::ms_prev_pos_y = 0.0f;
int NGUIWidget::editor_offset_x = 0;
int NGUIWidget::editor_offset_y = 0;
int NGUIWidget::num_updates = 0;

NGUIWidget::RequestListiner NGUIWidget::net_listiner;

void NGUIWidget::RequestListiner::OnRequestFinish(int res, void* data, const char* file_name)
{
	TextureEntry::TextureData* tex = (TextureEntry::TextureData*)data;

	if (res == 0)
	{		
		RELEASE(tex->texture)
		tex->texture = en_core.Render()->LoadTexture(file_name, true, _FL_);	

		if (tex->texture)
		{
			if (tex->mode == 0)
			{
				tex->texture->SetAdress(Texture::addressClamp);				
			}
			else
			if (tex->mode == 1)
			{
				tex->texture->SetAdress(Texture::addressWrap);				
			}
			else
			{
				tex->texture->SetAdress(Texture::addressMirror);
			}		
		}
		else
		{
			tex->tex_loading = 0;
		}
	}
	else
	{
		tex->tex_loading = 2;
	}
        
    tex->net_req = null;	
}

void NGUIWidgetGetPropCount(asIScriptGeneric* gen) 
{
	NGUIWidget* wgt = (NGUIWidget*)gen->GetObject();

	*(int*)(gen->GetAddressOfReturnLocation()) = wgt->childs.size();
}

void NGUIWidgetGetPropByName(asIScriptGeneric* gen) 
{	
	string* child_name = (string*)(gen->GetArgAddress(0));

	NGUIWidget* wgt = (NGUIWidget*)gen->GetObject();

	const char* str = child_name->c_str();	
		
	void* ptr = null;

	for (int i=0;i<wgt->childs.size();i++)
	{
		SceneEntity* entity = wgt->childs[i];

		if (String::IsEqual(str, entity->GetName()))
		{
			ptr = entity;
			break;
		}
	}

	gen->SetReturnAddress(ptr);
}

void NGUIWidgetGetPropByIndex(asIScriptGeneric* gen) 
{	
	NGUIWidget* wgt = (NGUIWidget*)gen->GetObject();

	int index = gen->GetArgDWord(0);
	
	if (index < 0 || index >= wgt->childs.size())
	{
		gen->SetReturnAddress(NULL);		
	}
	else
	{
		gen->SetReturnAddress(wgt->childs[index]);		
	}
}

NGUIWidget::~NGUIWidget()
{
	/*if (text) 
	{
		text->Release();
		text = null;
	}*/
}

void NGUIWidget::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG( x )
	PROP_REG( y )
	PROP_REG( width )
	PROP_REG( height )

	PROP_REG( rot_x )
	PROP_REG( rot_y )
	PROP_REG( rot_z )

	PROP_REG( scaleChilds )

	PROP_REG( alignByParent )	

	PROP_REG( horzAlign )
	PROP_REG( vertAlign )

	PROP_REG( clipChilds )

	PROP_REG( loc_text )

	PROP_REG( text_align )
	PROP_REG( vtext_align )

	PROP_REG( overColor )
	PROP_REG( alpha )

	PROP_REG( anchor_x )
	PROP_REG( anchor_y )
	PROP_REG( abs_anchor )

	PROP_REG( cellMove )	

	PROP_REG( allow_mouse_cover )
}

void NGUIWidget::Init()
{		

	bUseVertTextAlignInDrawCall = true;

	SceneEntity::Init();

	is_symbol = false;
	weight = 0;
				
	childs.clear();
	
	String::Copy(script_class_name, 128, Desc()->GetName());

#ifdef EDITOR
	empty_tex = en_core.Render()->LoadTexture("Editor/empty_label.png", false, _FL_);	
#endif	

	texMask = null;

	cur_anchor_x = cur_anchor_y = 0.0f;
	scr_x = scr_y = -10000;
	scr_w = scr_h = 0.0f;
	scr_anchor_x = scr_anchor_y = 0.0f;
	scr_delta_anchor_x = scr_delta_anchor_y = 0.0f;

	pre_x = pre_y = 0.0f;	
	pre_anchor_x = pre_anchor_y = 0.0f;	

	focused = false;

	prev_w = -100000.0f;
	prev_h = -100000.0f;
	
	prev_x = -100000.0f;
	prev_y = -100000.0f;

	original_w = -100000.0f;
	original_h = -100000.0f;
	
	scale_w = 1.0f;
	scale_h = 1.0f;

	need_recalc = eRecalcPos | eRecalcTextLine;

	parent_alpha = 1.0f;	
	clr = COLOR_WHITE;
	need_calc_to_parent_space = false;
	
	parent = null;			

	max_depth = -10000000;
	min_depth =  10000000;
	
	need_sort = false;

	font = null;

	deactivate_childs = false;
    sParentName[0] = 0;

#if defined(PC) || defined(OSUNIX)
    parent_path[0] = 0;
#endif
}

void NGUIWidget::ReInit()
{	
	prev_w = -100000.0f;
	prev_h = -100000.0f;
	prev_x = -100000.0f;
	prev_y = -100000.0f;

	need_recalc = eRecalcPos | eRecalcTextLine;

	if (!String::IsEqual(Desc()->GetName(),"NGUISymbolInstance"))
        childs.clear();

	{
		for (int i=0; i<childs_names.size(); i++)
		{
			NGUIWidget* wgt = static_cast<NGUIWidget*>(Scene()->Find(childs_names[i].data, "NGUIWidget"));
            if (wgt)
            {
                AddChild(wgt);
            }
            else
            {
                core->TraceTo("Scene", "Error: child %s for %s not founded", childs_names[i].data, name);
            }
 		}
	}

#ifdef EDITOR
	bool need_update_rollout = false;
#endif

	for (int i=0;i<slices.size();i++)
	{
		TexSlice& slc = slices[i];

		TextureInstance* ti = slc.slice->tex.texture;		
		slc.slice->tex.texture = en_core.Render()->LoadTexture(slc.slice->tex.tex_name, false, _FL_);	
		RELEASE(ti)	
	
#ifdef EDITOR
	
		if (slc.prev_use_texture_entity != *slc.prop_bool)
		{
			slc.prev_use_texture_entity = *slc.prop_bool;

			if (ed_rollout)
			{
				slc.prop_entity_name->SetSkipWidget(ed_rollout, !slc.prev_use_texture_entity);
				slc.prop_tex_edit->SetSkipWidget(ed_rollout, slc.prev_use_texture_entity);				

				need_update_rollout = true;
			}
		}
	
		if (slc.prev_use_texture_entity)
		{
			if (slc.tex_entity)
			{
				slc.tex_entity->DelUser(this);
				slc.tex_entity = null;
			}		
		}		
#endif

		if (*slc.prop_bool)
		{
			NGUITexture* entity = (NGUITexture*)Scene()->Find(slc.prop_char_entity_name, "NGUITexture");

			if (entity)
			{
				slc.tex_entity = entity;

#ifdef EDITOR
				slc.tex_entity->AddUser(this);
#endif
			}
		}
		else
		{
			slc.tex_entity = null;
		}
	}


#ifdef EDITOR
	if (need_update_rollout)
	{
		ed_rollout->CalcPos();
		ed_rollout->Redraw();
	}
#endif

	/*if (parent)
	{
		if (!String::IsEqual(parent_name, parent->GetName()))
		{					
			float x_scr_x = scr_x + scr_delta_anchor_x;

			if (horzAlign == align_center)
			{
				x_scr_x -= en_core.Render()->Display()->GetWidth() * 0.5f;
			}
			else
			if (horzAlign == align_right)
			{
				x_scr_x = scr_x - scr_delta_anchor_x;
				x_scr_x = en_core.Render()->Display()->GetWidth() - x_scr_x;
			}
				
			x = Convert2AbsHorz(x_scr_x);

			float y_scr_y = scr_y + scr_delta_anchor_y;

			if (vertAlign == align_center)
			{
				y_scr_y -= en_core.Render()->Display()->GetHeight() * 0.5f;
			}			
			else
			if (vertAlign == align_bottom)
			{
				y_scr_y = scr_y - scr_delta_anchor_y;
				y_scr_y = en_core.Render()->Display()->GetHeight() - y_scr_y;
			}

			y = Convert2AbsVert(y_scr_y);

#ifdef EDITOR
			ReverseTieValues();
#endif

			parent->DelChild(this);
			need_calc_to_parent_space = true;
		}
		else
		{
			NGUIWidget* wgt = parent;

			parent->DelChild(this);
			wgt->AddChild(this);
		}		
	}
	else
	{
		if (NGUIService::instance && !is_symbol) NGUIService::instance->DelTopWidget(this);
	}*/



	CalcColors();
    
	x = (int)x;
	y = (int)y;

	width = (int)width;
	height = (int)height;

//#ifdef EDITOR
	UpdateChildsPos();
//#endif

    SetText(loc_text.c_str());

#ifdef PC
    DataToGizmo();
#endif	
}

void NGUIWidget::FixTextures(TextureInstance* tex, TextureInstance* prev_tex)
{
	for (int i=0;i<draws.size();i++)
	{
		if (draws[i].tex == prev_tex)
		{
			draws[i].tex = tex;			
		}
	}

	if (parent)
	{
		for (int i=0;i<parent->draws.size();i++)
		{
			if (parent->draws[i].tex == prev_tex)
			{
				parent->draws[i].tex = tex;
			}
		}
	}	
}

void NGUIWidget::SetTextureData(TextureEntry::TextureData* data, const char* name)
{
	if (String::IsEqual(data->tex_name, name))
	{
		if (data->texture)
		{
			/*if (tileType == 0)
			{
				data->texture->SetAdress(TextureBase::addressClamp);				
			}
			else
			if (tileType == 1)
			{
				tex.texture->SetAdress(TextureBase::addressWrap);				
			}
			else
			{
				tex.texture->SetAdress(TextureBase::addressMirror);				
			}*/
		}

		return;
	}

	if (data->net_req)
	{
		core->Network()->CancelRequaest(data->net_req);
		data->net_req = null; 
	}

	TextureInstance* prev_tex_inst = data->texture;

	data->texture = en_core.Render()->LoadTexture(name, false, _FL_);

	if (data->texture)
	{
		/*if (tileType == 0)
		{
			data->texture->SetAdress(TextureBase::addressClamp);				
		}
		else
		if (tileType == 1)
		{
			data->texture->SetAdress(TextureBase::addressWrap);				
		}
		else
		{
			data->texture->SetAdress(TextureBase::addressMirror);				
		}*/		
	}	

	String::Copy(data->tex_name, 256, name);
	
	FixTextures(data->texture, prev_tex_inst);	

	RELEASE(prev_tex_inst);
}

void NGUIWidget::LoadProperties(IStreamLoader* loader)
{
	SceneEntity::LoadProperties(loader);
	
	loader->Read("weight", weight);


    int count = 0;
    loader->Read("childs_count", count);
    if(count > 0)
        childs_names.clear();
    for (int i=0; i< count; i++)
    {
        if (loader->EnterBlock("childs"))
        {
            Str128 sName;
            loader->Read("name",sName.data, 128);
            childs_names.push_back(sName);

            loader->LeaveBlock();
        }
    }

	for (int i=0; i<slices.size(); i++)
	{
		TextureEntry::Load(loader, slices[i].slice);	
	}
#ifdef EDITOR
	if (loader->GetType() != IStreamSaver::binary)
	{
		loader->Read("parentName",sParentName,128);
	}
#endif
}

 bool NGUIWidget::HaveDifferences(PropertiesOwner* holder)
 {
     NGUIWidget* wgt_holder = (NGUIWidget*)holder;

     if(parent && weight != wgt_holder->weight)
         return true;

     if(childs_names.size() != wgt_holder->childs_names.size())
         return true;

     for(unsigned  i = 0;i < childs_names.size();++i)
     {
        if(!String::IsEqual(childs_names[i].data,wgt_holder->childs_names[i].data))
            return true;
     }

     for (int i=0; i<slices.size(); i++)
     {
         if (!slices[i].slice->IsEqual(*wgt_holder->slices[i].slice))
         {
             return true;
         }		
     }
     return SceneEntity::HaveDifferences(holder);
 }

void NGUIWidget::SaveProperties(IStreamSaver* saver)
{

    if (fabs(rot_z) < 0.15f)
        rot_z = 0.0f;

	SceneEntity::SaveProperties(saver);

	saver->Write("weight", weight);

	int count = childs_names.size();

	saver->Write("childs_count", count);

	for (int i=childs.size() - childs_names.size(); i<childs.size(); i++)
	{
		saver->MarkBeginBlock("childs");
		saver->Write("name", childs[i]->GetName());
		saver->MarkEndBlock("childs");
	}

	for (int i=0; i<slices.size(); i++)
	{
        TextureEntry::Save(saver, slices[i].slice);
	}
	
#ifdef EDITOR
	if (saver->GetType() != IStreamSaver::binary)
	{
		if (parent)
		{
			saver->Write("parentName",parent->GetName());
		}
		else
		{
			saver->Write("parentName","");
		}
	}
#endif	
}

void NGUIWidget::SaveDiffProperties(IStreamSaver* saver, PropertiesOwner* holder)
{
	SceneEntity::SaveDiffProperties(saver, holder);

	NGUIWidget* origin = (NGUIWidget*)holder;

    if(weight != origin->weight)
        saver->Write("weight", weight);

    bool bSaveChilds = false;

    if(childs_names.size() != origin->childs_names.size())
        bSaveChilds = true;
    else
    {
        for(unsigned  i = 0;i < childs_names.size();++i)
        {
            if(!String::IsEqual(childs_names[i].data,origin->childs_names[i].data))
            {
                bSaveChilds = true;
                break;
            }
        }
    }

    if(bSaveChilds)
    {
        int count = childs_names.size();

        saver->Write("childs_count", count);

        for (int i=childs.size() - childs_names.size(); i<childs.size(); i++)
        {
            saver->MarkBeginBlock("childs");
            saver->Write("name", childs[i]->GetName());
            saver->MarkEndBlock("childs");
        }
    }
    for (int i=0; i<slices.size(); i++)
		TextureEntry::SaveDiffProperties(saver, slices[i].slice, origin->slices[i].slice);	
}
 SceneEntity* NGUIWidget::Clone()
{
    SceneEntity* pClone = SceneEntity::Clone();
   ((NGUIWidget*)pClone)->childs_names.assign(childs_names.begin(),childs_names.end());
   return pClone;
}
void NGUIWidget::Copy(PropertiesOwner* propHolder)
{
	NGUIWidget* dest = (NGUIWidget*)propHolder;

	for (int i=0; i<slices.size(); i++)
		TextureEntry::Copy(slices[i].slice, dest->slices[i].slice);		
   
	SceneEntity::Copy(propHolder);

    dest->weight = weight;
    dest->activeOnStart = activeOnStart;
    dest->showOnStart = showOnStart;
}

bool NGUIWidget::Start()
{	
	CalcColors();

	focused = false;

	original_w = width;
	original_h = height;

	scale_w = 1.0f;
	scale_h = 1.0f;

	cur_scale_w = 1.0f;
	cur_scale_h = 1.0f;

	for (int i=0; i<childs.size();i++)
	{
		childs[i]->Start();
	}

	return true;
}

void NGUIWidget::Finish()
{
	focused = false;
}

void NGUIWidget::Show(bool set_show)
{
	/*if (show != set_show)
	{
		NotifyVisibilityChange(set_show);
	}*/

	SceneEntity::Show(set_show);
}

bool NGUIWidget::IsFocused()
{
	return focused;
}

void NGUIWidget::SetFocused(bool focus)
{
	if (focus && IsVisible())
	{
		if (focusedWidget)
		{
			NGUIWidget* tmp = focusedWidget;			
			focusedWidget = null;

            tmp->SetFocused(false);
		}
	}

	focused = focus;
	
	if (focused)
	{
		focusedWidget = this;
	}
	else
	{
		focusedWidget = null;
	}
}

bool NGUIWidget::NWgtIsFocused()
{
	return IsFocused();
}

void NGUIWidget::NWgtSetFocused(bool focus)
{
	SetFocused(focus);
}

void NGUIWidget::AddChild(NGUIWidget * child)
{
    child->parent = this;
    childs.push_back(child);	
#if defined(PC) || defined(OSUNIX)
    String::Copy(child->parent_path, 512, name);
#endif
}
void  NGUIWidget::InsertChild(NGUIWidget* child,unsigned nPos)
{
    child->parent = this;
    childs.insert(childs.begin() + nPos,child);
#if defined(PC) || defined(OSUNIX)
    String::Copy(child->parent_path, 512, name);
#endif
}
void NGUIWidget::DelChild(NGUIWidget* child)
{
    child->parent = null;
    for (unsigned j = 0;j < childs.size();++j)
    {
        if(childs[j] == child)
        {
            childs.erase(childs.begin() + j);
            break;
        }
    }
}
void NGUIWidget::CalcColors()
{
	clr = overColor;
	clr.a = alpha;	
	
	if (parent)
	{	
		parent_alpha = parent->clr.a;

		clr *= parent->clr;	
	}	
	else
	{
		parent_alpha = 1.0f;
	}	

	for (int i=0;i<(int)childs.size();i++)
	{
		childs[i]->CalcColors();
	}	
}

void NGUIWidget::CalcHorzPos()
{		
	if (abs_anchor)
	{
		cur_anchor_x = anchor_x * width;			
	}
	else
	{
		cur_anchor_x = anchor_x;			
	}	

	float prnt_x = 0;
	float prnt_w = 0;

	if (parent && alignByParent)
	{		
		prnt_w = parent->width;
	}
	else
	{
		prnt_w = en_core.Render()->Display()->GetReferenceWidth();		
	}

	if (horzAlign == align_left)
	{
		pre_x = x - cur_anchor_x + prnt_x;
		pre_anchor_x = x + prnt_x;			
	}
	else
	if (horzAlign == align_center)
	{
		pre_x = x - cur_anchor_x + prnt_x + ((prnt_w) * 0.5f);
		pre_anchor_x = x + prnt_x + ((prnt_w) * 0.5f);
	}
	else
	if (horzAlign == align_right)
	{
		pre_x = prnt_x + prnt_w - (x - cur_anchor_x) - width;
		pre_anchor_x = prnt_x + prnt_w - x;
	}
			
	//scr_x = (ConvertHorz(pre_x));	
	scr_anchor_x = (ConvertHorz(pre_anchor_x));		
	
#ifdef EDITOR
	if (gizmo)
	{
		scr_w = (ConvertHorz(width));
	}
	else
#endif
	{
		scr_w = (ConvertHorz((int)(width + 0.5f)));
	}	
}

void NGUIWidget::CalcVertPos()
{	
	//cur_scale_h = GetScaleY();	
	
	if (abs_anchor)
	{	
		cur_anchor_y = anchor_y * height;
	}
	else
	{	
		cur_anchor_y = anchor_y;
	}	

	float prnt_y = 0;
	float prnt_h = 0;

	if (parent && alignByParent)
	{
		//prnt_y = parent->pre_y;
		prnt_h = parent->height;
	}
	else
	{
		prnt_h = en_core.Render()->Display()->GetReferenceHeight();		
	}

	if (vertAlign == align_top)
	{
		pre_y = (y - cur_anchor_y) + prnt_y;
		pre_anchor_y = (y) + prnt_y;	
	}
	else
	if (vertAlign == align_center)
	{
		pre_y = (y - cur_anchor_y) + prnt_y + ((prnt_h) * 0.5f);
		pre_anchor_y = (y) + prnt_y + ((prnt_h) * 0.5f);
	}
	else
	if (vertAlign == align_bottom)
	{
		pre_y = prnt_y + prnt_h - (y - cur_anchor_y) - height;
		pre_anchor_y = prnt_y + prnt_h - (y);
	}
							
	//scr_y = (ConvertVert(pre_y));	
	scr_anchor_y = (ConvertVert(pre_anchor_y));

#ifdef EDITOR
	if (gizmo)
	{
		scr_h = (ConvertVert(height));
	}
	else
#endif
	{
		scr_h = (ConvertVert((int)(height + 0.5f)));			
	}
}

void NGUIWidget::CalcChildsGlobalMat()
{
	childs_global_mat = global_mat;
}

void NGUIWidget::UpdateChildsPos()
{		
	if (!IsVisible() && Scene()->IsRunning())
	{
		need_recalc |= eRecalcPos;
		return;
	}

#ifdef EDITOR
    SetParentPath();
#endif

    float prev_scr_w = scr_w;
    
	if (scaleChilds)
	{
		if (prev_w > -99000.0f)
		{					
			if (fabsf(prev_w - width) > 1.0f || fabsf(prev_h - height) > 1.0f)
			{				
				scale_w = width / original_w;
				scale_h = height / original_h;
	
				float scale_x = width / prev_w;
				float scale_y = height / prev_h;

				for (int i=0;i<childs.size();i++)
				{
					childs[i]->Scale(scale_x, scale_y);
				}

				//need_recalc_pos = true;
			}
		}

		prev_w = width;
		prev_h = height;
	}

	//need_recalc_pos = true;

	/*if (prev_w > -99000.0f)
	{					
		if (fabsf(prev_x - x) > 1.0f || fabsf(prev_y - y) > 1.0f)
		{							
			need_recalc_pos = true;
		}
	}*/

	prev_x = x;
	prev_y = y;
	
	//if (need_recalc_pos)
	{	
		CalcHorzPos();
		CalcVertPos();
		
		Matrix mat_rt;
		//mat_rt.RotateZ(RADIAN * rot_x);
		//mat_rt.RotateX(RADIAN * rot_x);
		mat_rt.RotateZ(RADIAN * rot_z);

		Matrix mat_trans;

#ifdef EDITOR
		if (gizmo)
		{
			mat_trans.Move(Vector(ConvertHorz(pre_anchor_x), ConvertVert(pre_anchor_y), 0.0f));
		}
		else
#endif
		{
			mat_trans.Move(Vector(ConvertHorz((int)(pre_anchor_x + 0.5)), ConvertVert((int)(pre_anchor_y + 0.5f)), 0.0f));
		}		

		Matrix mat_trans2;

#ifdef EDITOR
		if (gizmo)
		{
			mat_trans2.Move(Vector(ConvertHorz(-cur_anchor_x), ConvertVert(-cur_anchor_y), 0.0f));
		}
		else
#endif
		{
			mat_trans2.Move(Vector(ConvertHorz(-(int)(cur_anchor_x + 0.5f)), ConvertVert(-(int)(cur_anchor_y + 0.5f)), 0.0f));
		}
		

		local_mat = mat_trans2 * mat_rt * mat_trans;		

		if (parent)
		{
			global_mat = local_mat * parent->childs_global_mat;
		}
		else
		{
			global_mat = local_mat;
		}		

		Vector ps(0, 0, 0.0f);
		
		ps = ps * global_mat;

		scr_x = ps.x;
		scr_y = ps.y;

		CalcChildsGlobalMat();

		need_recalc ^= eRecalcNone;
	}

	num_updates++;

	for (int i=0;i<(int)childs.size();i++)
	{
		childs[i]->UpdateChildsPos();
	}
}

bool NGUIWidget::IsHitted(float mx, float my)
{
	return IsHitted(mx, my, 0, 0, scr_w, scr_h);
}

bool NGUIWidget::IsHitted(float mx, float my, float x, float y, float w, float h)
{
	Vector2 ancorns[4];
	Vector p1;

	for (int i=0; i < 4; i++)
	{
		if (i==0)
		{
			p1 = Vector(x - GetBorderHorz(), y - GetBorderVert(), 0.0f);			
		}
		else
		if (i==1)
		{
			p1 = Vector(x + w + GetBorderHorz(), y - GetBorderVert(), 0.0f);			
		}
		else
		if (i==2)
		{
			p1 = Vector(x + w + GetBorderHorz(), y + h + GetBorderVert(), 0.0f);		
		}
		else
		if (i==3)
		{
			p1 = Vector(x - GetBorderHorz(), y + h + GetBorderVert(), 0.0f);		
		}						
				
		p1 = p1 * global_mat;

		ancorns[i] = Vector2(p1.x,p1.y);
	}

	if (IsPointInsideTriangle(Vector2(mx, my), ancorns[0], ancorns[1], ancorns[2]) ||
		IsPointInsideTriangle(Vector2(mx, my), ancorns[0], ancorns[2], ancorns[3]))		
	{
		return true;
	}

	return false;
}

void NGUIWidget::CheckMouseCover()
{
	if (!IsVisible()) return;
	
	if (IsActive() && alpha > 0.05f)
	{
		if (AllowMouseCover())
		{
			if (IsHitted(ms_pos_x, ms_pos_y))			
			{                
				mscover_widget = this;
				mscover_widget_not_covered = null;
			}	
		}
		else
		{
			if (!mscover_widget)
			{
				if (IsHitted(ms_pos_x, ms_pos_y))				
				{					
					mscover_widget_not_covered = this;
				}
			}
		}
	}

	for (int i=0;i<(int)childs.size();i++)
	{
		childs[i]->CheckMouseCover();
	}
}

void NGUIWidget::NWgtScriptSetAlpha(float set_alpha)
{
	alpha = set_alpha;
	CalcColors();
}

float NGUIWidget::NWgtScriptGetAlpha()
{
	return alpha;
}

void NGUIWidget::NWgtScriptSetX(float set_x)
{
	if (fabs(x - set_x) < 0.1f) return;

	x = set_x;
		
	need_recalc |= eRecalcPos;
}

float NGUIWidget::NWgtScriptGetX()
{
	return x;
}

void NGUIWidget::NWgtScriptSetY(float set_y)
{
	if (fabs(y - set_y) < 0.1f) return;

	y = set_y;

	need_recalc |= eRecalcPos;
}

float NGUIWidget::NWgtScriptGetY()
{
	return y;
}

float NGUIWidget::NWgtScriptGetWidth()
{
	return width;
}

void NGUIWidget::NWgtScriptSetWidth(float w)
{
	if (fabs(width - w) < 0.1f) return;

	width = w;

#ifdef EDITOR
	if (gizmo)
	{
		scr_w = (ConvertHorz(width));
	}
	else
#endif
	{
		scr_w = (ConvertHorz((int)(width + 0.5f)));
	}
		
	need_recalc = eRecalcPos | eRecalcTextLine;
}

float NGUIWidget::NWgtScriptGetHeight()
{
	return height;
}

void NGUIWidget::NWgtScriptSetHeight(float h)
{
	if (fabs(height - h) < 0.1f) return;
	
	height = h;
	
#ifdef EDITOR
	if (gizmo)
	{
		scr_h = (ConvertVert(height));
	}
	else
#endif
	{
		scr_h = (ConvertVert((int)(height + 0.5f)));			
	}

	need_recalc |= eRecalcPos;
}

void NGUIWidget::NWgtScriptSetRotx(float rotx)
{
	if (fabs(rot_x - rotx) < 0.1f) return;

	rot_x = rotx;
	UpdateChildsPos();	
}

float NGUIWidget::NWgtScriptGetRotx()
{
	return rot_x;
}

void NGUIWidget::NWgtScriptSetRoty(float roty)
{
	if (fabs(rot_y - roty) < 0.1f) return;

	rot_y = roty;
	UpdateChildsPos();	
}

float NGUIWidget::NWgtScriptGetRoty()
{
	return rot_y;
}

void NGUIWidget::NWgtScriptSetRotz(float rotz)
{
	if (fabs(rot_z - rotz) < 0.1f) return;

	rot_z = rotz;
	UpdateChildsPos();
}

float NGUIWidget::NWgtScriptGetRotz()
{
	return rot_z;
}

void NGUIWidget::Render(float dt, int level)
{
	if (parent) return;	
	
	FullRender();

	ExecuteDrawCalls();	
}

void NGUIWidget::FullRender()
{
	if (!IsVisible()) return;

	if (!Scene()->IsRunning())
	{
		if (!edited)
		{
			if (!showOnStart) return;
		}
	}		

	if (need_recalc & eRecalcPos)
	{
		UpdateChildsPos();
	}

	if (need_recalc & eRecalcTextLine)
	{
		CalcLinesData();

		need_recalc ^= eRecalcTextLine;
	}

	RenderWidget();

	for (int i=0;i<(int)childs.size();i++)
	{
		childs[i]->FullRender();
	}	

#ifdef EDITOR
	if (NGUIService::instance->DebugColor())
	{
		AddDrawCall(empty_tex, 0, 0, scr_w, scr_h, GetDebugColor(), 0,0,1,1);
	}
#endif
}

void NGUIWidget::SetText(const char* txt)
{
	text = txt;    
    
	CalcLinesData();	
}

void NGUIWidget::NWgtSetTextScript(const string& txt)
{
	SetText(txt.c_str());
}

bool NGUIWidget::CheckClassName(const char* clsName)
{
	if (String::IsEqual(clsName, "NGUIWidget"))
	{
		return true;
	}

	return false;
}

bool NGUIWidget::AllowRegisterSelfToScript()
{
	return true;	
}

void NGUIWidget::MethodsToScript(const char* script_class_name)
{
	Embed2ScriptBaseClass();
	
	MethodsToScriptSub(script_class_name);
}

extern void CALLWRAPPERNAME(IsVisible)(asIScriptGeneric *gen);
extern void CALLWRAPPERNAME(Show)(asIScriptGeneric *gen);
extern void CALLWRAPPERNAME(IsActive)(asIScriptGeneric *gen);
extern void CALLWRAPPERNAME(Activate)(asIScriptGeneric *gen);
extern void CALLWRAPPERNAME(GetTypeScript)(asIScriptGeneric *gen);
extern void CALLWRAPPERNAME(SEGetData)(asIScriptGeneric *gen);
extern void CALLWRAPPERNAME(SESetData)(asIScriptGeneric *gen);

void NGUIWidgetCastGeneric(asIScriptGeneric *gen)
{
	char wgt_type[64];
	
	const char* func_decl = gen->GetFunction()->GetDeclaration();
	
	int len = strlen(func_decl);
	
	for (int i=0;i<len;i++)
	{
		if (func_decl[i] != '@')
		{
			wgt_type[i] = func_decl[i];			
		}
		else
		{
			wgt_type[i] = 0;
			break;
		}
	}
	
	const char* className = ((NGUIWidget*)gen->GetObject())->className;
	char className_inst[128];
	
	if (String::IsEqual("NGUISymbolInstance", className))
	{
		if (((NGUISymbolInstance*)gen->GetObject())->useInstance)
		{
			className = ((NGUISymbolInstance*)gen->GetObject())->wgt_symbol->class_symbol;
		}
		else
		{
			className = ((NGUISymbolInstance*)gen->GetObject())->wgt_symbol->class_symbol_inst;
		}		
	}
	
	if (String::IsEqual(wgt_type, className) || String::IsEqual(wgt_type, "NGUIWidget"))
	{
		gen->SetReturnAddress(gen->GetObject());
	}
	else
	{
		gen->SetReturnAddress(null);
	}
}

void NGUIWidget::FullMethodsToScript(const char* class_name)
{
	core->Script()->Machine()->RegisterObjectType(class_name, sizeof(NGUIWidget), asOBJ_REF| asOBJ_NOCOUNT);	

	core->Script()->Machine()->RegisterObjectMethod(class_name, "bool IsVisible()", CALLWRAPPERNAMEAS(IsVisible));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void Show(bool show)", CALLWRAPPERNAMEAS(Show));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "bool IsActive()", CALLWRAPPERNAMEAS(IsActive));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void Activate(bool Active)", CALLWRAPPERNAMEAS(Activate));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void GetType(string& name)", CALLWRAPPERNAMEAS(GetTypeScript));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void GetData(string& name)", CALLWRAPPERNAMEAS(SEGetData));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void SetData(string&in name)", CALLWRAPPERNAMEAS(SESetData));
	
	for (int i=0;i<Properties().size();i++)
	{
		Properties()[i]->EmbedingScript(class_name);
	}	

	MethodsToScriptSub(class_name);
}

CALLWRAPPER(NGUIWidget, NWgtGetAbsoluteX, CALLWRAPPERRET(float))
CALLWRAPPER(NGUIWidget, NWgtGetAbsoluteY, CALLWRAPPERRET(float))

CALLWRAPPERARG1(NGUIWidget, NWgtSetTextScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUIWidget, NWgtGetTextScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())

CALLWRAPPER(NGUIWidget, NWgtScriptGetX, CALLWRAPPERRET(float))
CALLWRAPPERARG1(NGUIWidget, NWgtScriptSetX, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())

CALLWRAPPER(NGUIWidget, NWgtScriptGetY, CALLWRAPPERRET(float))
CALLWRAPPERARG1(NGUIWidget, NWgtScriptSetY, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())

CALLWRAPPER(NGUIWidget, NWgtScriptGetWidth, CALLWRAPPERRET(float))
CALLWRAPPERARG1(NGUIWidget, NWgtScriptSetWidth, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())

CALLWRAPPER(NGUIWidget, NWgtScriptGetHeight, CALLWRAPPERRET(float))
CALLWRAPPERARG1(NGUIWidget, NWgtScriptSetHeight, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())

CALLWRAPPER(NGUIWidget, NWgtScriptGetAlpha, CALLWRAPPERRET(float))
CALLWRAPPERARG1(NGUIWidget, NWgtScriptSetAlpha, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())

CALLWRAPPER(NGUIWidget, NWgtScriptGetRotx, CALLWRAPPERRET(float))
CALLWRAPPERARG1(NGUIWidget, NWgtScriptSetRotx, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())

CALLWRAPPER(NGUIWidget, NWgtScriptGetRoty, CALLWRAPPERRET(float))
CALLWRAPPERARG1(NGUIWidget, NWgtScriptSetRoty, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())

CALLWRAPPER(NGUIWidget, NWgtScriptGetRotz, CALLWRAPPERRET(float))
CALLWRAPPERARG1(NGUIWidget, NWgtScriptSetRotz, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERNORET())

CALLWRAPPER(NGUIWidget, NWgtIsFocused, CALLWRAPPERRET(bool))
CALLWRAPPERARG1(NGUIWidget, NWgtSetFocused, CALLWRAPPERGETPARAM(bool,arg1), CALLWRAPPERNORET())


CALLWRAPPER(NGUIWidget, NWgtGetCurTextWidth, CALLWRAPPERRET(float))
CALLWRAPPER(NGUIWidget, NWgtGetCurTextHeight, CALLWRAPPERRET(float))
CALLWRAPPER(NGUIWidget, NWgtGetCurTextLinesCount, CALLWRAPPERRET(int))
CALLWRAPPERARG1(NGUIWidget, NWgtGetTextWidth, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(NGUIWidget, NWgtGetTextHeight, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERRET(float))
CALLWRAPPERARG1(NGUIWidget, NWgtGetTextLinesCount, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERRET(int))	
CALLWRAPPERARG3(NGUIWidget, NWgtSetColorScript, CALLWRAPPERGETPARAM(float,arg1), CALLWRAPPERGETPARAM(float,arg2), CALLWRAPPERGETPARAM(float,arg3), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUIWidget, NWgtSetColorStringScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPERARG3(NGUIWidget, NWgtGetColorScript, CALLWRAPPERGETADRESS(float,arg1), CALLWRAPPERGETADRESS(float,arg2), CALLWRAPPERGETADRESS(float,arg3), CALLWRAPPERNORET())
CALLWRAPPERARG1(NGUIWidget, NWgtGetNameScript, CALLWRAPPERGETADRESS(string,arg1), CALLWRAPPERNORET())
CALLWRAPPER(NGUIWidget, NWgtGetParentIndexScript, CALLWRAPPERRET(int))
CALLWRAPPERARG1(NGUIWidget, NWgtSetParentIndexScript, CALLWRAPPERGETPARAM(int,arg1), CALLWRAPPERNORET())	


void NGUIWidget::MethodsToScriptSub(const char* class_name)
{	
	core->Script()->Machine()->RegisterObjectMethod(class_name, "float GetAbsoluteX()", CALLWRAPPERNAMEAS(NWgtGetAbsoluteX));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "float GetAbsoluteY()", CALLWRAPPERNAMEAS(NWgtGetAbsoluteY));
	
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void SetText(string&in name)", CALLWRAPPERNAMEAS(NWgtSetTextScript));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void GetText(string& name)", CALLWRAPPERNAMEAS(NWgtGetTextScript));		

	core->Script()->Machine()->RegisterObjectMethod(class_name, "float get_x()", CALLWRAPPERNAMEAS(NWgtScriptGetX));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void set_x(float set_x)", CALLWRAPPERNAMEAS(NWgtScriptSetX));

	core->Script()->Machine()->RegisterObjectMethod(class_name, "float get_y()", CALLWRAPPERNAMEAS(NWgtScriptGetY));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void set_y(float set_y)", CALLWRAPPERNAMEAS(NWgtScriptSetY));	

	core->Script()->Machine()->RegisterObjectMethod(class_name, "float get_width()", CALLWRAPPERNAMEAS(NWgtScriptGetWidth));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void set_width(float w)", CALLWRAPPERNAMEAS(NWgtScriptSetWidth));

	core->Script()->Machine()->RegisterObjectMethod(class_name, "float get_height()", CALLWRAPPERNAMEAS(NWgtScriptGetHeight));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void set_height(float h)", CALLWRAPPERNAMEAS(NWgtScriptSetHeight));

	core->Script()->Machine()->RegisterObjectMethod(class_name, "float get_alpha()", CALLWRAPPERNAMEAS(NWgtScriptGetAlpha));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void set_alpha(float set_alpha)", CALLWRAPPERNAMEAS(NWgtScriptSetAlpha));

	core->Script()->Machine()->RegisterObjectMethod(class_name, "float get_rot_x()", CALLWRAPPERNAMEAS(NWgtScriptGetRotx));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void set_rot_x(float set_rot_x)", CALLWRAPPERNAMEAS(NWgtScriptSetRotx));

	core->Script()->Machine()->RegisterObjectMethod(class_name, "float get_rot_y()", CALLWRAPPERNAMEAS(NWgtScriptGetRoty));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void set_rot_y(float set_rot_y)", CALLWRAPPERNAMEAS(NWgtScriptSetRoty));

	core->Script()->Machine()->RegisterObjectMethod(class_name, "float get_rot_z()", CALLWRAPPERNAMEAS(NWgtScriptGetRotz));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void set_rot_z(float set_rot_z)", CALLWRAPPERNAMEAS(NWgtScriptSetRotz));


	core->Script()->Machine()->RegisterObjectMethod(class_name, "void SetFocused(bool set_focused)", CALLWRAPPERNAMEAS(NWgtSetFocused));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "bool IsFocused()", CALLWRAPPERNAMEAS(NWgtIsFocused));	
	
	core->Script()->Machine()->RegisterObjectMethod(class_name, "float GetCurTextWidth()", CALLWRAPPERNAMEAS(NWgtGetCurTextWidth));			
	core->Script()->Machine()->RegisterObjectMethod(class_name, "float GetCurTextHeight()", CALLWRAPPERNAMEAS(NWgtGetCurTextHeight));			
	core->Script()->Machine()->RegisterObjectMethod(class_name, "int GetCurTextLinesCount()", CALLWRAPPERNAMEAS(NWgtGetCurTextLinesCount));			

	core->Script()->Machine()->RegisterObjectMethod(class_name, "float GetTextWidth(string&in txt)", CALLWRAPPERNAMEAS(NWgtGetTextWidth));			
	core->Script()->Machine()->RegisterObjectMethod(class_name, "float GetTextHeight(string&in txt)", CALLWRAPPERNAMEAS(NWgtGetTextHeight));			
	core->Script()->Machine()->RegisterObjectMethod(class_name, "int GetTextLinesCount(string&in txt)", CALLWRAPPERNAMEAS(NWgtGetTextLinesCount));			

	core->Script()->Machine()->RegisterObjectMethod(class_name, "void SetColor(float r, float g, float b)", CALLWRAPPERNAMEAS(NWgtSetColorScript));	
    core->Script()->Machine()->RegisterObjectMethod(class_name, "void SetColor(string&in txt)", CALLWRAPPERNAMEAS(NWgtSetColorStringScript));	
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void GetColor(float& r, float& g, float& b)", CALLWRAPPERNAMEAS(NWgtGetColorScript));	

	core->Script()->Machine()->RegisterObjectMethod(class_name, "void GetName(string& nm)", CALLWRAPPERNAMEAS(NWgtGetNameScript));					

	core->Script()->Machine()->RegisterObjectMethod(class_name, "int GetIndexInParentChilds()", CALLWRAPPERNAMEAS(NWgtGetParentIndexScript));		
	core->Script()->Machine()->RegisterObjectMethod(class_name, "void SetIndexInParentChilds(int index)", CALLWRAPPERNAMEAS(NWgtSetParentIndexScript));

	core->Script()->Machine()->RegisterObjectMethod(class_name, "NGUIWidget@ GetChild(string&in name)", asFUNCTION(NGUIWidgetGetPropByName));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "NGUIWidget@ GetChild(int index)", asFUNCTION(NGUIWidgetGetPropByIndex));
	core->Script()->Machine()->RegisterObjectMethod(class_name, "int GetChildCount()", asFUNCTION(NGUIWidgetGetPropCount));	

	if (strcmp("NGUIWidget", class_name))
	{
		core->Script()->Machine()->RegisterObjectMethod(class_name, "NGUIWidget@ opImplCast()", asFUNCTION(NGUIWidgetCastGeneric));

		char str[128];
		asIScriptEngine* engine = ((ScriptMachine*)(core->Script()->Machine()))->Machine();

		if (engine->GetDefaultNamespace()[0])
		{
			sprintf(str, "%s::%s@ opCast()", engine->GetDefaultNamespace(), class_name);
		}
		else
		{
			sprintf(str, "%s@ opCast()", class_name);
		}
		
		core->Script()->Machine()->RegisterObjectMethod("NGUIWidget", str, asFUNCTION(NGUIWidgetCastGeneric));		
	}

	//core->Script()->Machine()->RegisterObjectMethod(class_name, "void GetData(string& name)", CALLWRAPPERNAMEAS(SEGetData));
	//core->Script()->Machine()->RegisterObjectMethod(class_name, "void SetData(string& name)", CALLWRAPPERNAMEAS(SESetData));
}
#if defined(PC) || defined(OSUNIX)

void NGUIWidget::GetMembersForAutoComplete(std::vector<string>& vMembers)
{
    vMembers.push_back("IsVisible");
    vMembers.push_back("Show");
    vMembers.push_back("IsActive");
    vMembers.push_back("Activate");
    vMembers.push_back("GetType");
    vMembers.push_back("GetData");
    vMembers.push_back("SetData");

    vMembers.push_back("GetAbsoluteX");
    vMembers.push_back("GetAbsoluteY");

    vMembers.push_back("SetText");
    vMembers.push_back("GetText");	

    vMembers.push_back("SetFocused");
    vMembers.push_back("IsFocused");

    vMembers.push_back("GetCurTextWidth");
    vMembers.push_back("GetCurTextHeight");
    vMembers.push_back("GetCurTextLinesCount");

    vMembers.push_back("GetTextWidth");
    vMembers.push_back("GetTextHeight");
    vMembers.push_back("GetTextLinesCount");

    vMembers.push_back("SetColor");
    vMembers.push_back("GetColor");

    vMembers.push_back("GetName");

    vMembers.push_back("GetIndexInParentChilds");
    vMembers.push_back("SetIndexInParentChilds");

    vMembers.push_back("GetChild");
    vMembers.push_back("GetChild");
    vMembers.push_back("GetChildCount");


    vMembers.push_back("x");
    vMembers.push_back("y");

    vMembers.push_back("height");
    vMembers.push_back("width");

    vMembers.push_back("rot_x");
    vMembers.push_back("rot_y");
    vMembers.push_back("rot_z");

    vMembers.push_back("scaleChilds");

    vMembers.push_back("alignByParent");

    vMembers.push_back("horz_align");


    vMembers.push_back("vert_align");


    vMembers.push_back( "weight");
    vMembers.push_back( "clip_childs");
    vMembers.push_back( "Text");

    vMembers.push_back( "text_align");


    vMembers.push_back( "vrt_text_align");


    vMembers.push_back( "overColor");
    vMembers.push_back( "alpha");

    vMembers.push_back( "anchor_x");
    vMembers.push_back( "anchor_y");
    vMembers.push_back( "AnchAbsUnits");

    vMembers.push_back( "CellMove");

    vMembers.push_back( "AllowMouseCover");

}
void NGUIWidget::GetSignatureForAutoComplete(const string& sMethod,std::vector<string>& vSignatures)
{
    const std::vector<string*>& vStrings  = g_vSignatures.GetList(sMethod);
    for(unsigned i = 0;i < vStrings.size();++i)
        vSignatures.push_back(*vStrings[i]);
}
void NGUIWidget::BuildSignatures()
{
    g_vSignatures["IsVisible"]="bool IsVisible()";
    g_vSignatures["Show"]="void Show(bool show)";
    g_vSignatures["IsActive"]="bool IsActive()";
    g_vSignatures["Activate"]="void Activate(bool Active)";
    g_vSignatures["GetType"]="void GetType(string& name)";
    g_vSignatures["GetData"]="void GetData(string& name)";
    g_vSignatures["SetData"]="void SetData(string&in name)";

    g_vSignatures["GetAbsoluteX"]="float GetAbsoluteX()";
    g_vSignatures["GetAbsoluteY"]="float GetAbsoluteY()";

    g_vSignatures["SetText"]="void SetText(string&in name)";
    g_vSignatures["GetText"]="void GetText(string& name)";	

    g_vSignatures["SetFocused"]="void SetFocused(bool set_focused)";
    g_vSignatures["IsFocused"]="bool IsFocused()";

    g_vSignatures["GetCurTextWidth"]="float GetCurTextWidth()";
    g_vSignatures["GetCurTextHeight"]="float GetCurTextHeight()";
    g_vSignatures["GetCurTextLinesCount"]="int GetCurTextLinesCount()";

    g_vSignatures["GetTextWidth"]="float GetTextWidth(string&in txt)";
    g_vSignatures["GetTextHeight"]="float GetTextHeight(string&in txt)";
    g_vSignatures["GetTextLinesCount"]="int GetTextLinesCount(string&in txt)";

    g_vSignatures["SetColor"]="void SetColor(float r, float g, float b)";
    g_vSignatures.Add("SetColor","void SetColor(string&in txt)");
    
    g_vSignatures["GetColor"]="void GetColor(float& r, float& g, float& b)";

    g_vSignatures["GetName"]="void GetName(string& nm)";

    g_vSignatures["GetIndexInParentChilds"]="int GetIndexInParentChilds()";
    g_vSignatures["SetIndexInParentChilds"]="void SetIndexInParentChilds(int index)";

    g_vSignatures["GetChild"]="NGUIWidget@ GetChild(string&in name)";
    g_vSignatures.Add("GetChild","NGUIWidget@ GetChild(int index)");
    g_vSignatures["GetChildCount"]="int GetChildCount()";
}
HashTable<string,string> NGUIWidget::g_vSignatures;

#endif

bool NGUIWidget::IsVisible()
{	
	if (parent)
	{
		if (!SceneEntity::IsVisible()) return false;
		return parent->IsVisible();
	}

	return SceneEntity::IsVisible();
}

bool NGUIWidget::IsActive()
{	
	if (parent)
	{		
		if (!SceneEntity::IsActive()) return false;

		if (parent->deactivate_childs)
		{
			return false;
		}

		return parent->IsActive();
	}

	return SceneEntity::IsActive();
}

float NGUIWidget::ConvertHorz(float sz)
{	
 	return sz * NGUIService::instance->h_mul_RefToScreen;
}

float NGUIWidget::ConvertVert(float sz)
{	
	return sz * NGUIService::instance->v_mul_RefToScreen;	
}

float NGUIWidget::Convert2AbsHorz(float sz)
{	
	return sz * NGUIService::instance->h_mul_ScreenToRef;
}

float NGUIWidget::Convert2AbsVert(float sz)
{	
 	return sz * NGUIService::instance->v_mul_ScreenToRef;
}

float NGUIWidget::NWgtGetAbsoluteX()
{	
	return scr_x * NGUIService::instance->h_mul_ScreenToRef;
}

float NGUIWidget::NWgtGetAbsoluteY()
{	
	return scr_y * NGUIService::instance->v_mul_ScreenToRef;
}

void NGUIWidget::NWgtGetTextScript(std::string& val)
{
	//val = text->Get();
	val = text;
}

#ifdef EDITOR

void NGUIWidget::SetGizmo(Gizmo* gz)
{
	if (gizmo)
	{
		gizmo->enabled = false;
		gizmo->type = 0;
	}
	
	gizmo = gz;
	
	if (gizmo)
	{
		gizmo->enabled = true;
		gizmo->type = 2;
	}
}

void NGUIWidget::DataToGizmo()
{
	if (!gizmo) return;

	gizmo->width = scr_w;
	gizmo->height = scr_h;
	gizmo->transform = global_mat;	

	Matrix mat_trans2;
	mat_trans2.Move(Vector(ConvertHorz(-cur_anchor_x), ConvertVert(-cur_anchor_y), 0.0f));
	mat_trans2.Inverse();

	gizmo->center_transform = mat_trans2 * global_mat;

	Matrix mat_rt;
	
	//mat_rt.RotateZ(RADIAN * rot_x);
	//mat_rt.RotateX(RADIAN * rot_x);
	mat_rt.RotateZ(RADIAN * rot_z);

	gizmo->rot_matrix = mat_rt;
}

void NGUIWidget::DataFromGizmo()
{	
	if (!gizmo) return;	
		
	float delta_x = 0.0f;
	float delta_y = 0.0f;
    Vector prev_angels(rot_x, rot_y, rot_z);

	if (gizmo->selAxis == 0)
	{			
		if (parent)
		{
			Matrix mat_rt;
			//mat_rt.RotateZ(RADIAN * rot_x);
			//mat_rt.RotateX(RADIAN * rot_x);
			mat_rt.RotateZ(RADIAN * rot_z);

			Matrix mat_trans;
			mat_trans.Move(Vector(ConvertHorz(pre_anchor_x), ConvertVert(pre_anchor_y), 0.0f));

			Matrix mat_trans2;
			mat_trans2.Move(Vector(ConvertHorz(-cur_anchor_x), ConvertVert(-cur_anchor_y), 0.0f));

			Matrix lc_mat = parent->childs_global_mat;
			lc_mat.Inverse();

			lc_mat = gizmo->transform * lc_mat;

			Matrix lc_mat2 = mat_trans2 * mat_rt;
			lc_mat2.Inverse();

			Matrix m1 = lc_mat2 * lc_mat;		

			delta_x = Convert2AbsHorz(m1._41) - pre_anchor_x;
			delta_y = Convert2AbsVert(m1._42) - pre_anchor_y;		
		}
		else
		{
			delta_x = Convert2AbsHorz(gizmo->transform._41 - global_mat._41);
			delta_y = Convert2AbsVert(gizmo->transform._42 - global_mat._42);		
		}
	
		if (horzAlign == 2)
		{
			delta_x *= -1.0f;
		}

		if (vertAlign == 4)
		{
			delta_y *= -1.0f;
		}

		x += delta_x;
		y += delta_y;		

		x = (int)x;
		y = (int)y;
	}
	else
	if (gizmo->selAxis == 9)
	{
		Vector angels;
		gizmo->rot_matrix.GetAngles(angels);
		
		rot_x = angels.x / RADIAN;		
		rot_y = angels.y / RADIAN;
		rot_z = angels.z / RADIAN;		



		if (parent)
		{
			Matrix mat_rt;
			//mat_rt.RotateZ(RADIAN * rot_x);
			//mat_rt.RotateX(RADIAN * rot_x);
			mat_rt.RotateZ(RADIAN * rot_z);

			Matrix mat_trans;
			mat_trans.Move(Vector(ConvertHorz(pre_anchor_x), ConvertVert(pre_anchor_y), 0.0f));

			Matrix mat_trans2;
			mat_trans2.Move(Vector(ConvertHorz(-cur_anchor_x), ConvertVert(-cur_anchor_y), 0.0f));

			Matrix lc_mat = parent->childs_global_mat;
			lc_mat.Inverse();

			lc_mat = gizmo->transform * lc_mat;

			Matrix lc_mat2 = mat_trans2 * mat_rt;
			lc_mat2.Inverse();

			Matrix m1 = lc_mat2 * lc_mat;		

			delta_x = Convert2AbsHorz(m1._41) - pre_anchor_x;
			delta_y = Convert2AbsVert(m1._42) - pre_anchor_y;		
		}
		else
		{
			delta_x = Convert2AbsHorz(gizmo->transform._41 - global_mat._41);
			delta_y = Convert2AbsVert(gizmo->transform._42 - global_mat._42);		
		}

		x += delta_x;
		y += delta_y;		

		x = (int)x;
		y = (int)y;
	}
	else
	if (gizmo->selAxis > 0)
	{					
		delta_x = Convert2AbsHorz(gizmo->width) - width;
		delta_y = Convert2AbsVert(gizmo->height) - height;	
				
		float k1 = 1.0f;
		float k2 = 1.0f;

		if (horzAlign == 2)		
		{
			k1 = -1.0f;
		}

		if (vertAlign == 4)		
		{
			k2 = -1.0f;
		}

		if (gizmo->selAxis == 1 || gizmo->selAxis == 4 || gizmo->selAxis == 8)
		{			
			delta_x *= -1.0f;

			x -= global_mat.Vx().x * delta_x * k1;
			y -= global_mat.Vx().y * delta_x * k2;							
		}

		if (gizmo->selAxis == 1 || gizmo->selAxis == 2 || gizmo->selAxis == 5)
		{			
			delta_y *= -1.0f;
			
			x -= global_mat.Vy().x * delta_y * k1;
			y -= global_mat.Vy().y * delta_y * k2;			
		}

		width += delta_x;
		height += delta_y;

		if (abs_anchor)
		{
			float dt_x = anchor_x * width - cur_anchor_x;			
			float dt_y = anchor_y * height - cur_anchor_y;			

			x += global_mat.Vx().x * dt_x * k1;
			y += global_mat.Vx().y * dt_x * k2;

			x += global_mat.Vy().x * dt_y * k1;
			y += global_mat.Vy().y * dt_y * k2;			
		}		

		x = (int)x;
		y = (int)y;

		width = (int)width;
		height = (int)height;
	}

    UpdateChildsPos();	
    if (fabs(delta_x) > 0.01f || fabs(delta_y) > 0.01f  || fabs(prev_angels.x - rot_x) > 0.01f ||  fabs(prev_angels.y - rot_y) > 0.01f ||  fabs(prev_angels.z - rot_z) > 0.01f)
	{
		ShowEditorWidgets(true);
        OnPropertiesChanged();
	}

}

bool NGUIWidget::Select2D(float mx, float my, SceneEntity* cur_slection)
{
	if (!IsVisible()) return false;
	
	NGUIWidget* sel = null;

	/*if (cur_slection)
	{
		if (cur_slection->CheckClassName("NGUIWidget"))
		{
			sel = (NGUIWidget*)cur_slection;
			if (sel->weight>weight) return false;
		}
	}*/

	for (int i=childs.size() - 1;i>=0;i--)
	{
		if (childs[i]->Select2D(mx, my, sel))
		{
			return false;
		}
	}

	Vector2 ancorns[4];
	Vector p1;

	for (int i=0; i < 4; i++)
	{
		if (i==0)
		{
			p1 = Vector(0, 0, 0.0f);			
		}
		else
		if (i==1)
		{
			p1 = Vector(scr_w, 0, 0.0f);			
		}
		else
		if (i==2)
		{
			p1 = Vector(scr_w, scr_h, 0.0f);		
		}
		else
		if (i==3)
		{
			p1 = Vector(0, scr_h, 0.0f);		
		}						
				
		p1 = p1 * global_mat;

		ancorns[i] = Vector2(p1.x,p1.y);
	}

	if (IsPointInsideTriangle(Vector2(mx, my), ancorns[0], ancorns[1], ancorns[2]) ||
		IsPointInsideTriangle(Vector2(mx, my), ancorns[0], ancorns[2], ancorns[3]))		
	{
		return true;
	}

	return false;
}

#endif

void NGUIWidget::AddDrawCall(Texture* tex, float set_scr_x, float set_scr_y, float wdt, float hgt, Color color, float u, float v, float du, float dv)
{
	if (!IsVisible()) return;

	draws.push_back(DrawCall());
	DrawCall* draw_call = &draws[draws.size()-1];
	
	draw_call->tex = tex;	
	draw_call->scr_x = set_scr_x;
	draw_call->scr_y = set_scr_y;
	draw_call->width = wdt;
	draw_call->height = hgt;	

	draw_call->color = color;		

	draw_call->depth = -1.0f;	

	draw_call->u = u;	
	draw_call->du = du;	

	draw_call->v = v;
	draw_call->dv = dv;
}

void NGUIWidget::AddDrawCallLevel(float depth, Texture* tex, float set_scr_x, float set_scr_y, float wdt, float hgt, Color color, float u, float v, float du, float dv)
{
	if (!IsVisible()) return;

	need_sort = true;
	draws.push_back(DrawCall());
	DrawCall* draw_call = &draws[draws.size()-1];	

	draw_call->tex = tex;	
	draw_call->scr_x = set_scr_x;
	draw_call->scr_y = set_scr_y;
	draw_call->width = wdt;
	draw_call->height = hgt;	

	draw_call->color = color;		

	draw_call->depth = depth;	

	if (depth>max_depth) max_depth = depth;
	if (depth<min_depth) min_depth = depth;	

	draw_call->u = u;	
	draw_call->du = du;	

	draw_call->v = v;
	draw_call->dv = dv;
}

void NGUIWidget::AddPrintCall(FontInstance* font, float set_scr_x, float set_scr_y, float set_scr_w, float set_scr_h, std::vector<FontRes::LineData>* line_indices, float font_scale, Color color, const char* txt_text)
{
	if (!IsVisible()) return;

	text_draws.push_back(PrintCall());
	PrintCall* text_draw = &text_draws[text_draws.size()-1];

	text_draw->font = font;	
	text_draw->scr_x = set_scr_x;	
	text_draw->scr_y = set_scr_y;	
	text_draw->scr_w = set_scr_w;	
	text_draw->scr_h = set_scr_h;	
	text_draw->font_scale = font_scale;
	text_draw->color = color;	
	text_draw->text= txt_text;	
	text_draw->line_indices = line_indices;
}

void NGUIWidget::NWgtSetColorScript(float r, float g, float b)
{
	overColor.r = r;
	overColor.g = g;
	overColor.b = b;

	CalcColors();
}
    
void NGUIWidget::NWgtSetColorStringScript(std::string& txt)
{
    if(txt[0] == '#' && txt.size() > 6)
    {
        string sRed,sGreen,sBlue;
        sRed.assign(txt.begin() + 1,txt.begin() + 3);
        sGreen.assign(txt.begin() + 3,txt.begin() + 5);
        sBlue.assign(txt.begin() + 5,txt.begin() + 7);
        char * p;
        float red = static_cast<float>(strtoul( sRed.c_str(), & p, 16 )) / 255.f; 
        if ( * p != 0 )return;
        float green =  static_cast<float>(strtoul( sGreen.c_str(), & p, 16 )) / 255.f; 
        if ( * p != 0 )return;
        float blue =  static_cast<float>(strtoul( sBlue.c_str(), & p, 16 )) / 255.f; 
        if ( * p != 0 )return;
        overColor.r = red;
        overColor.g = green;
        overColor.b = blue;
        CalcColors();
    }
}

void NGUIWidget::NWgtGetColorScript(float& r, float& g, float& b)
{
	r = overColor.r;
	g = overColor.g;
	b = overColor.b;	
}

float NGUIWidget::NWgtGetCurTextWidth()
{		
	return GetTextWidth(text);
}

float NGUIWidget::NWgtGetCurTextHeight()
{
	return GetTextHeight(text);	
}

int NGUIWidget::NWgtGetCurTextLinesCount()
{
	return line_indexes.size();
}

float NGUIWidget::GetTextWidth(std::string& txt)
{	
	if (!font) return -1.0f;
	
	return (int)(Convert2AbsHorz(font->GetWidth(txt.c_str())));
}

float NGUIWidget::GetTextHeight(std::string& txt)
{	
	if (!font) return -1.0f;

	ClipRect rect;

	rect.x1 = 0;
	rect.y1 = 0;		

	rect.x2 = scr_w;
	rect.y2 = scr_h;
    float h = Convert2AbsVert(font->GetHeight(txt.c_str(), &rect, 1.0f, (TextAlign)text_align));
    ROUND(h)
	return h;
}

int NGUIWidget::GetTextLinesCount(std::string& txt)
{
	std::vector<FontRes::LineData> line_indexes;

	if (font)
	{
		font->CalcLinesData(txt.c_str(), line_indexes, scr_w, 1.0f, (TextAlign)text_align);		
	}

	return line_indexes.size();
}

float NGUIWidget::NWgtGetTextWidth(std::string& txt)
{
	return GetTextWidth(txt);
}

float NGUIWidget::NWgtGetTextHeight(std::string& txt)
{
	return GetTextHeight(txt);
}

int NGUIWidget::NWgtGetTextLinesCount(std::string& txt)
{
	return GetTextLinesCount(txt);
}

void NGUIWidget::NWgtGetNameScript(std::string& nm)
{
	nm = name;
}

bool NGUIWidget::IsChild(NGUIWidget* wgt)
{
	if (parent)
	{
		if (parent == wgt)
		{
			return true;
		}

		return parent->IsChild(wgt);
	}

	return false;
}

bool NGUIWidget::IsSymbol()
{
	if (parent)
	{
		return parent->IsSymbol();			
	}	

	return is_symbol;
}

void NGUIWidget::CalcLinesData()
{
}

void NGUIWidget::Embed2ScriptBaseClass()
{		
	if (core->Script()->Machine()->GetTypeIdByDecl("NGUIWidget") == asINVALID_TYPE)
	{			
		FullMethodsToScript("NGUIWidget");				
	}
}

void NGUIWidget::ModifyTextClipRect(ClipRect& rect)
{
}

void NGUIWidget::ExecuteDrawCalls()
{	
	float range = 0.0f;
	
	if (fabs(max_depth - min_depth) > 0.05f) range = 1.0f / (max_depth - min_depth) * 0.9f;

	if (need_sort)
	{
		for (int i=0;i<draws.size()-1;i++)
		{
			for (int j=i+1;j<draws.size();j++)
			{
				if (draws[i].depth > draws[j].depth)
				{
					DrawCall tmp = draws[i];
					draws[i] = draws[j];
					draws[j] = tmp;			
				}
			}
		}

		need_sort = false;
	}

	bool need_clip = false;
	bool skip_draw = false;

	float clip_x =-10000000.0f;
	float clip_y =-10000000.0f;
	float clip_x2 = 10000000.0f;
	float clip_y2 = 10000000.0f;	

	GetClipRect(clip_x, clip_y, clip_x2, clip_y2);

	bool skip_render = false;

	if (clip_x>-900000.0f)
	{
		need_clip = true;

		RenderService::Rect rect;
		rect.left = clip_x;
		if (rect.left < 0) rect.left = 0;

		rect.right = clip_x2;
		if (rect.right < 0)
		{
			skip_render = true;			
		}

		if (rect.left > NGUIService::instance->cur_rwidth)
		{
			skip_render = true;			
		}

		if (rect.right > NGUIService::instance->cur_rwidth)
		{
			rect.right = NGUIService::instance->cur_rwidth;
		}

		rect.top = clip_y;
		if (rect.top < 0) rect.top = 0;
		
		rect.bottom = clip_y2;
		if (rect.bottom < 0)
		{
			skip_render = true;
		}

		if (rect.top > NGUIService::instance->cur_rheight)
		{
			skip_render = true;
		}

		if (rect.bottom > NGUIService::instance->cur_rheight)
		{
			rect.bottom = NGUIService::instance->cur_rheight;
		}

		if (!skip_render)
		{
			en_core.Render()->State()->SetScissorRect(rect);
			en_core.Render()->State()->SetScissor(true);
		}
	}	

	int count = draws.size();

	if (!skip_render)
	{
		for (int i=0;i<count;i++)
		{
			DrawCall* draw = &draws[i];

			//draw->scr_x += scr_x;
			//draw->scr_y += scr_y;
	
			float depth = -1.0f;

			/*if (draws[i].depth > -0.5f)
			{
				depth = (draw->depth - min_depth) * range + 0.05f;
			}*/

			//if (!need_sort)
			{
				//draw->scr_x += scr_x;
				//draw->scr_y += scr_y;
			}

			if (need_clip)
			{			
				if (!ClipSprite(draw->scr_x, draw->scr_y, draw->width, draw->height,				
					clip_x - scr_x, clip_x2 - scr_x, clip_y - scr_y, clip_y2 - scr_y, 0.0f, scr_delta_anchor_x, scr_delta_anchor_y))
				{					
					continue;
				}			
			}			

#ifdef EDITOR
			if (!Scene()->IsRunning())
			{
				draw->scr_x += editor_offset_x;
				draw->scr_y += editor_offset_y;				
			}			
#endif
			NGUIService::instance->AddSpriteCall(draw->tex, texMask, draw->scr_x, draw->scr_y, draw->width, draw->height, draw->color, global_mat, depth, draw->u, draw->v, draw->du, draw->dv);		
		}
	}

	max_depth = -10000000;
	min_depth =  10000000;

	draws.clear();			
	
	count = text_draws.size();
	
	if (count != 0 && !skip_render) 
	{			
		ClipRect rect;
		ClipRect clip_rect;

		ClipRect* clip_rect_ptr = null;

		if (need_clip)
		{
			clip_rect.x1 = clip_x;
			clip_rect.x2 = clip_x2;
			clip_rect.y1 = clip_y;
			clip_rect.y2 = clip_y2;
			
			ModifyTextClipRect(clip_rect);		

			RenderService::Rect rect;

			rect.left = clip_rect.x1;
			rect.right = clip_rect.x2;
			rect.top = clip_rect.y1;
			rect.bottom = clip_rect.y2;
			
			if (rect.left < 0) rect.left = 0;		
			if (rect.top < 0) rect.top = 0;			

			en_core.Render()->State()->SetScissorRect(rect);			

			clip_rect_ptr = &clip_rect;	
		}

		for (int i=0;i<count;i++)
		{		
			PrintCall* text_draw = &text_draws[i];

			rect.x1 = text_draw->scr_x * 0;// + scr_x;
			rect.y1 = text_draw->scr_y * 0;// + scr_y;		

			rect.x2 = rect.x1 + text_draw->scr_w;
			rect.y2 = rect.y1 + text_draw->scr_h;

			float s_y = rect.y1;
						
			if (clipChilds)
			{
				if (need_clip)
				{				
					clip_rect.x1 = MathMax(clip_x,rect.x1);
					clip_rect.x2 = MathMin(clip_x2,rect.x2);
					clip_rect.y1 = MathMax(clip_y,rect.y1);
					clip_rect.y2 = MathMin(clip_y2,rect.y2);
				}
				else
				{
					clip_rect = rect;
					clip_rect_ptr = &clip_rect;
				}				
			}
								
			{
				if(bUseVertTextAlignInDrawCall)
				{
					if (vtext_align == 1 || vtext_align == 2)
					{
						float hgt = text_draw->font->GetHeight(text_draw->text.c_str(), &rect, text_draw->font_scale, (TextAlign)text_align);

						if (vtext_align == 1)
						{
							s_y = rect.y1 + ((rect.y2 - rect.y1) - hgt) * 0.5f;					
						}
						else
							if (vtext_align == 2)
							{
								s_y = rect.y2 - hgt;
							}
					}
				}


#ifdef EDITOR
				if (!Scene()->IsRunning())
				{				
					s_y += editor_offset_y;				

					rect.x1 += editor_offset_x;
					rect.y1 += editor_offset_y;				

					rect.x2 += editor_offset_x;
					rect.y2 += editor_offset_y;				

					if (clip_rect_ptr)
					{
						clip_rect_ptr->x1 += editor_offset_x;
						clip_rect_ptr->y1 += editor_offset_y;				

						clip_rect_ptr->x2 += editor_offset_x;
						clip_rect_ptr->y2 += editor_offset_y;				
					}
				}			
#endif								
				Matrix transform;
				transform = global_mat;				

				transform._41 += transform.Vx().x * text_draw->scr_x;
				transform._42 += transform.Vx().y * text_draw->scr_x;
				transform._43 += transform.Vx().z * text_draw->scr_x;

				transform._41 += transform.Vy().x * (s_y + text_draw->scr_y);
				transform._42 += transform.Vy().y * (s_y + text_draw->scr_y);
				transform._43 += transform.Vy().z * (s_y + text_draw->scr_y);

				NGUIService::instance->AddPrintCall(text_draw->font, transform, &rect, text_draw->line_indices, text_draw->font_scale, text_draw->color, (TextAlign)text_align, text_draw->text.c_str());
			}
		}

		text_draws.clear();
	}

	if (need_clip) en_core.Render()->State()->SetScissor(false);

	for (int i=0;i<(int)sub_drawers.size();i++)
	{
		sub_drawers[i]->ExecuteDrawCalls();
	}

	if (!skip_render)
	{
		for (int i=0;i<(int)childs.size();i++)
		{
			if (childs[i]->IsVisible()) childs[i]->ExecuteDrawCalls();
		}	
	}
}

bool NGUIWidget::AllowMouseCover()
{
#ifdef PC
	if (core->IsEditorMode()) return true;
#endif

	return allow_mouse_cover;
}

#ifdef EDITOR
void NGUIWidget::SetEditMode(bool ed)
{
	SceneEntity::SetEditMode(ed);

	if (!ed)
	{		
		TextureEntry::StopEditGUITexture();
	}
	else
	{
		if (ed_rollout)
		{
			for (int i=0;i<slices.size();i++)
			{
				TexSlice& slc = slices[i];					
	
				slc.prop_entity_name->SetSkipWidget(ed_rollout, !slc.prev_use_texture_entity);
				slc.prop_tex_edit->SetSkipWidget(ed_rollout, slc.prev_use_texture_entity);								
			}		

			ed_rollout->CalcPos();
			ed_rollout->Redraw();
		}
	}
}

Color NGUIWidget::GetDebugColor()
{
	return COLOR_WHITE;
}

bool NGUIWidget::UpdateEditorWidgets()
{				
    TextureEntry::UpdateWidgets(this);	

    return SceneEntity::UpdateEditorWidgets();	
}
#endif

bool NGUIWidget::IsAffectChildDrawCall()
{
	return false;
}

void NGUIWidget::AffectChildDrawCall(float& x, float& y, float w, float h, float& a)
{	
}

void NGUIWidget::GetClipRect(float& clip_x, float& clip_y, float& clip_x2, float& clip_y2)
{
	if (clipChilds)
	{
		if (clip_x<scr_x) clip_x = scr_x;
		if (clip_x2>scr_x + scr_w) clip_x2 = scr_x + scr_w;

		if (clip_y<scr_y) clip_y = scr_y;
		if (clip_y2>scr_y + scr_h) clip_y2 = scr_y + scr_h;		
	}

	if (parent)
	{
		parent->GetClipRect(clip_x, clip_y, clip_x2, clip_y2);
	}
}

float NGUIWidget::GetBorderHorz()
{
	return 0.0f;
}

float NGUIWidget::GetBorderVert()
{
	return 0.0f;
}

NGUIWidget* NGUIWidget::GetRoot()
{
	if (parent)
	{
		return parent->GetRoot();
	}

	return this;
}

int NGUIWidget::NWgtGetParentIndexScript()
{
	if (parent)
	{
		for (int i = 0; i<parent->childs.size(); i++)
		{
			if (parent->childs[i] == this)
			{
				return i;
			}
		}
	}

	return -1;
}

void NGUIWidget::NWgtSetParentIndexScript(int index)
{	
	if (parent)
	{
		int prev_index = NWgtGetParentIndexScript();

		if (prev_index == index)
		{
			return;
		}

		parent->childs.erase(parent->childs.begin() + prev_index, parent->childs.begin() + prev_index + 1);

		parent->childs.insert(parent->childs.begin() + index, this);		
	}	
}

void NGUIWidget::GetTextureFileName(TextureEntry::TextureData* data, std::string& name)
{
	name = data->tex_name;
}

void NGUIWidget::SetTextureDataURL(TextureEntry::TextureData* data, const char* url, float url_fade_time)
{		
	data->wgt = this;

	if (!url[0])
	{		
		SetTextureData(data, url);
		return;
	}

	char file_name[128];

#ifdef PC	
	String::Printf(file_name, 1024, "%s/from_net", en_core.EN_Files()->GetAppDirectory());
	CreateDirectory(file_name, 0);
#endif
	
	GetMD5Hash(url, file_name);	

    if (String::IsEqual(data->tex_name, file_name))
	{
        return;
    }    	

	core->TraceTo("WidgetURL", "%s - %s", GetName(), url);
	
	//RELEASE(data->texture)

	String::Copy(data->tex_name, 1024, file_name);		

	if (data->net_req)
	{
        core->Network()->CancelRequaest(data->net_req);
		data->net_req = null;
	}

	TextureInstance* prev_tex_inst = data->texture;			

#ifdef IOS
    NSString *imageFilename = [NSString stringWithFormat:@"%s", data->tex_name];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *path = [NSString stringWithFormat:@"%@/%@", [paths objectAtIndex:0], imageFilename];
    
    core->TraceTo("app", path.UTF8String);
    
    data->texture = en_core.Render()->LoadTexture(path.UTF8String, true, _FL_);
#endif

#if defined(PC) || defined(OSUNIX)
	char file_name2[512];
	String::Printf(file_name2, 1024, "from_net/%s", file_name);	
    data->texture = en_core.Render()->LoadTexture(file_name2, true, _FL_);
#endif

#ifdef ANDROID
	data->texture = en_core.Render()->LoadTexture(data->tex_name, true, _FL_);
#endif

    data->tex_loading = 1;
    
	if (!data->texture)
	{
		data->net_req = core->Network()->StartRequest(&net_listiner, data, null, url, null, file_name);
	}

	if (url_fade_time > 0.01f)
	{
		data->fade_in_time = url_fade_time;
	}

	FixTextures(data->texture, prev_tex_inst);	

	RELEASE(prev_tex_inst);
}

float NGUIWidget::GetScaleX()
{
	if (parent)
	{
		return scale_w * parent->GetScaleX();
	}

	return scale_w;
}

float NGUIWidget::GetScaleY()
{
	if (parent)
	{
		return scale_h * parent->GetScaleY();
	}

	return scale_h;
}

void NGUIWidget::Scale(float scale_x, float scale_y)
{
	x *= scale_x;
	y *= scale_y;

	width *= scale_x;
	height *= scale_y;

	prev_w = width;
	prev_h = height;

	for (int i=0;i<childs.size();i++)
	{
		childs[i]->Scale(scale_x, scale_y);
	}
}

void NGUIWidget::NotifyVisibilityChange(bool vis)
{
//#ifndef EDITOR
	for (int i=0;i<(int)childs.size();i++)
	{
		childs[i]->NotifyVisibilityChange(vis);
	}	
//#endif
}

void NGUIWidget::Release()
{	
	if (focusedWidget==this)
	{
		focusedWidget = null;
	}

	if (mscover_widget==this)
	{
		mscover_widget = null;
	}
	
	if (mscover_widget_not_covered==this)
	{
		mscover_widget_not_covered = null;
	}

	for (int i=0; i<slices.size(); i++)
	{
		RELEASE(slices[i].slice->tex.texture)
		
		if (slices[i].tex_entity)
		{
#ifdef EDITOR
			slices[i].tex_entity->DelUser(this);
#endif
		}
	}

	for (int i=0;i<childs.size();i++)
	{
		childs[i]->parent = null;
	}

	if (focused)
	{
		NGUIWidget::focusedWidget = null;
	}

	if (NGUIWidget::mscover_widget == this)
	{
		NGUIWidget::mscover_widget = null;
	}

	if (NGUIWidget::mscover_widget_not_covered == this)
	{
		NGUIWidget::mscover_widget_not_covered = null;
	}

	if (parent)
		parent->DelChild(this);
		
#ifdef EDITOR
	RELEASE(empty_tex);	
#endif

	SceneEntity::Release();
}

void NGUIWidget::ScrollerWork(NGUIWidget* widget, ScrollerData& data, float dt, int scrollerType, bool allow_scroll)
{
    float drag_border = 300;
	float rubber_velocity_clip = 400;
	    
	if (widget->IsVisible())// && widget->IsActive())
	{
		if (!data.is_draged)
		{
			if (widget->IsHitted(ms_pos_x, ms_pos_y, 0, 0, widget->scr_w, widget->scr_h) && allow_scroll && data.refresh_offset == 0)
			{
				bool drag = false;
				
				drag = core->Controls()->CheckTouch(0, ControlsService::Activated, widget);									
					
				if (drag)
				{
					if (mscover_widget && mscover_widget != widget)
					{
						drag = mscover_widget->IsChild(widget);
					}
				}

				if ( drag )
				{
					data.is_draged = true;										

					data.time_accum = 0.0f;

					data.offset_accum = 0;
                    
                    data.rubber_time = -1.0f;

                    if (!data.need_auto)
                    {
                        data.need_check_dir = true;
                    }
                    
                    if (fabs(data.auto_delta) > 90.0f)
                    {
                        data.time_to_block_childs = 0.275f;
                    }
                    
                    data.auto_delta = 0.0f;
				}
			}
		}
		else
		{
			bool drag = core->Controls()->CheckTouchIgnoreOwner(0, ControlsService::Active);

			if (!allow_scroll)
			{
				drag = false;
			}

            float delta = 0.0f;
			
			if (scrollerType == 1)
			{
				delta = widget->Convert2AbsVert(ms_pos_y - ms_prev_pos_y);
			}
			else
			{
				delta = widget->Convert2AbsVert(ms_pos_x - ms_prev_pos_x);
			}

			if (data.need_check_dir && fabs(delta) > 0.5f)
			{
				float delta_x = fabs(ms_pos_x - ms_prev_pos_x);
				float delta_y = fabs(ms_pos_y - ms_prev_pos_y);

				if ((scrollerType == 1 && delta_x > delta_y) || (scrollerType == 0 && delta_x < delta_y))
				{
					drag = false;
				}				

				data.need_check_dir = false;
			}
            
            if (drag)
            {
                if (fabs(delta) != 0)
                {
                    if (SIGN(delta) != SIGN(data.velocity))
                    {
                        data.velocity = SIGN(delta) * fabs(data.velocity) * 0.6f;
                    }
                }
            
                float v = delta / dt;
                data.velocity += v * 0.1f;
                                                           
                data.offset_accum += delta;
                data.time_accum += dt;
            
                if (data.time_accum > 0.05f)
                {
                    float v = data.offset_accum / data.time_accum;

                    if (fabs(data.offset_accum) < 45.0f)
                    {
                        data.velocity = data.velocity * 0.15f + v * 0.85f;
                    
                        if (fabs(data.velocity) > 750.0f)
                        {
                            data.velocity = SIGN(data.velocity) * 750.0f;
                        }
                    }
                
                    data.offset_accum = 0.0f;
                    data.time_accum = 0.0f;
                }
                
                if (fabs(data.velocity) > 10000)
                {
                    data.velocity = SIGN(data.velocity) * 10000;
                }
            
                float k = 1.0f;
                
                if (data.offset > 0.0f)
                {
                    k = (1.0f - data.offset / drag_border) * 0.9f + 0.1f;
                }
                else
                if (data.offset < -data.scroll_size)
                {
                    k = (1.0f - (-data.scroll_size - data.offset) / drag_border) * 0.9f + 0.1f;
                }
                
				data.offset += delta * k;
			}
			else
			{
				data.is_draged = false;
                
				if (fabs(data.velocity) > 25 && data.offset < 0 && data.offset > -data.scroll_size)
				{
					float v  = data.velocity; 
					float target = data.offset + 0.8f * data.velocity - 5.0f;
					if (fabs(v) <= rubber_velocity_clip && (target > 0 || target < -data.scroll_size))
					{
						v = SIGN(data.velocity) * rubber_velocity_clip;
 					    data.auto_border = 55; 						
					}
					else 
					{
						data.auto_border = 200;
						
						if (fabs(target - data.auto_border) < data.auto_border || fabs(target + (data.scroll_size + data.auto_border)) < data.auto_border)
						{
							data.auto_border = 55;
						}						
					}

					if (!data.allow_rubber)
					{
						data.auto_border = 0; 
					}

					data.amplitude = 0.8 * v;
					data.target = (int)(data.offset + data.amplitude + 0.5f);
					data.need_auto = true;
					data.time_accum = 0.0f;
				}
                else
                {
                    data.amplitude = 0;
                }
			}
		}
	}
    
	if (!data.is_draged && data.need_auto)
	{
        if (fabs(data.velocity) > 0.1f)
        {
            int sign = SIGN(data.velocity);
            data.velocity -= sign * dt * 4200;
            
            if (sign != SIGN(data.velocity))
            {
                data.velocity = 0.0f;
            }
        }
        
		if (fabs(data.amplitude) > 0.01f)
		{            
			data.time_accum += dt;

            data.auto_delta = -data.amplitude * exp(-static_cast<double>(data.time_accum) * 2.0);
            
			if (fabs(data.auto_delta) > 0.5)
			{
                data.offset = data.target + data.auto_delta;
                
                if (data.offset > data.auto_border || data.offset < -data.scroll_size -data.auto_border || fabs(data.auto_delta) < 5.0f)
                {
                    data.velocity = 0.0f;
                    data.need_auto = false;
                }
            }
			else
			{
				data.velocity = 0.0f;
				data.need_auto = false;
                
                data.auto_delta = 0.0f;
            }
        }
		else
		{
			data.velocity = 0.0f;
			data.need_auto = false;
            
            data.auto_delta = 0.0f;
		}
	}

    if (data.offset > drag_border)
	{
		data.offset = drag_border;
		data.velocity = 0.0f;
        data.amplitude = 0;

        data.rubber_time = 1.0f;
	}
    
    //if (data.is_draged || (data.time_to_block_childs > 0.0f))
	if (data.time_to_block_childs > 0.0f)
    {
        data.time_to_block_childs -= dt;
        
        if (data.time_to_block_childs < 0.0f)
        {
            data.time_to_block_childs = -1.0f;
            core->Controls()->SetDisableFastActivated(false);
        }
        else
        {
            core->Controls()->SetDisableFastActivated(true);
        }
    }
    
	if (data.offset < -data.scroll_size-drag_border)
	{
		data.offset = -data.scroll_size-drag_border;
		data.velocity = 0.0f;
        data.amplitude = 0;
	}
    
    if (!data.is_draged && (data.offset > 0 || data.offset < -data.scroll_size) && !data.need_auto)
    {
        if (data.rubber_time < 0.0f)
        {
            if (data.offset > 0)
            {
                data.rubber_time = powf(data.offset / drag_border, 1.0/ 3.0f);
            }
            else
            {
                data.rubber_time = powf((-data.scroll_size - data.offset) / drag_border, 1.0/ 3.0f);
            }
        }
        else
        {
            if(data.refresh_offset == 0)
                data.rubber_time -= dt * 1.25f;
            else
            {
                 if (data.offset > 0 && data.offset > data.refresh_offset)
                     data.rubber_time -= dt * 1.25f;
                 else
                   if (data.offset < 0 &&  data.offset < -data.scroll_size - data.refresh_offset)
                           data.rubber_time -= dt * 1.25f;
            }
            if (data.rubber_time < 0.0f)
            {
                data.rubber_time = -1.0f;
                
                if (data.offset > 0)
                {
                    data.offset = 0.0f;
                }
                else
                {
                    data.offset = -data.scroll_size;
                }
            }
            else
            {
                float f = (data.rubber_time);
                
                if (data.offset > 0)
                {
                    data.offset = (f * f * f) * drag_border;
                }
                else
                {
                    data.offset = -data.scroll_size - (f * f * f) * drag_border;
                }
            }
        }
    }
    
	/*if (data.need_auto) 
	{
		en_core.Render()->Debug()->PrintText(10, 30, up_left, COLOR_GREEN, "Scrl: v = %4.3f, a = %4.3f, offset = %4.3f size =  %4.3f", data.velocity, data.amplitude, data.offset,data.scroll_size);
	}*/
}

#if defined(PC) || defined(OSUNIX)

void NGUIWidget::SetParentPath()
{
    parent_path[0] = 0;
    if (parent)
    {
        parent->SetParentPath();
        if(String::IsEmpty(parent->parent_path))
            String::Printf(parent_path, 2048, "%s",parent->GetName());
        else
            String::Printf(parent_path, 2048, "%s.%s", parent->parent_path, parent->GetName());
    }
}
const char* NGUIWidget::GetFullName()
{
    static string sFullName;
    sFullName.clear();
    SetParentPath();
    sFullName = parent_path; 
    if(!sFullName.empty())sFullName += '.';
    sFullName += name;
    return sFullName.c_str();
}
#endif