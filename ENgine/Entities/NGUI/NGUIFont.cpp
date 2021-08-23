
#include "NGUIFont.h"
#include "NGUIWidget.h"

ENTITY(NGUIFont)
ENTITY_PARAMS(NGUIFont,"NGUIFont","Scene", "Font")

void NGUIFont::RegisterProperties()
{
	SceneEntity::RegisterProperties();

	PROP_REG(font)
}

void NGUIFont::Init()
{							
    RegisterInGroup("NGUIFont");
}

void NGUIFont::ReInit()
{
	RELEASE(font.font)
	font.font = en_core.Font()->CreateFont(font.name, font.bold, font.italic, font.height / (float)en_core.Render()->Display()->GetReferenceHeight() * (float)en_core.Render()->Display()->GetHeight(), font.gap, font.outline);

#ifdef EDITOR
	for (int i=0; i<users.size(); i++)
	{
		users[i]->font = font.font;
	}	
#endif
}

FontInstance* NGUIFont::GetFont()
{
	return font.font;	
}

#ifdef EDITOR

void NGUIFont::AddUseFont(NGUIWidget* user)
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

void NGUIFont::DelUseFont(NGUIWidget* user)
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
#endif

void NGUIFont::Release()
{	
#ifdef EDITOR
	for (int i=0; i<users.size(); i++)
	{
		users[i]->font = null;
	}
	users.clear();
#endif

	UnregisterInGroup("NGUIFont");

	RELEASE(font.font)

	SceneEntity::Release();
}