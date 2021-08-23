
#include "NGUITexture.h"

ENTITY(NGUITexture)
ENTITY_PARAMS(NGUITexture,"NGUITexture","Scene", "Texture")

#ifdef EDITOR
void StartEditGUITexture(PropertiesOwner* owner)
{
	NGUITexture* tex = (NGUITexture*)owner;
	TextureEntry::StartEditGUITexture(&tex->slice);
}
#endif

void NGUITexture::RegisterProperties()
{
	SceneEntity::RegisterProperties();

#ifdef EDITOR
	PROP_REG(texEdit)
#endif
}

void NGUITexture::Init()
{					
	RegisterInGroup("NGUITexture");
}

void NGUITexture::ReInit()
{
	RELEASE(slice.tex.texture)
	slice.tex.texture = en_core.Render()->LoadTexture(slice.tex.tex_name, false, _FL_);
}

void NGUITexture::LoadProperties(IStreamLoader* loader)
{
	SceneEntity::LoadProperties(loader);

    TextureEntry::Load(loader, &slice);
}

void NGUITexture::SaveProperties(IStreamSaver* saver)
{
	SceneEntity::SaveProperties(saver);

	TextureEntry::Save(saver, &slice);	
}

void NGUITexture::Copy(PropertiesOwner* propHolder)
{
	NGUITexture* src = (NGUITexture*)propHolder;
	TextureEntry::Copy(&slice, &src->slice);
	
	SceneEntity::Copy(propHolder);
}

#ifdef EDITOR

void NGUITexture::AddUser(NGUIWidget* user)
{
	for (int i=0; i<users.size(); i++)
	{
		if (users[i] == user)
		{
			return;
		}
	}

	users.push_back(user);
}

void NGUITexture::DelUser(NGUIWidget* user)
{
	for (int i=0; i<users.size(); i++)
	{
		if (users[i] == user)
		{
			users.erase(users.begin() + i);
			return;
		}
	}
}

void NGUITexture::SetEditMode(bool ed)
{
	SceneEntity::SetEditMode(ed);

	if (!ed)
	{		
		TextureEntry::StopEditGUITexture();
	}
	else
	{
		TextureEntry::StartEditGUITexture(&slice);
	}
}


bool NGUITexture::UpdateEditorWidgets()
{				
	TextureEntry::UpdateWidgets(null);	

	return SceneEntity::UpdateEditorWidgets();	
}

#endif

void NGUITexture::Release()
{	
#ifdef EDITOR
	for (int i=0; i<users.size(); i++)
	{
		//users[i]->tex_ = null;
	}
	users.clear();
#endif

	UnregisterInGroup("NGUITexture");
	
	RELEASE(slice.tex.texture)

	SceneEntity::Release();
}