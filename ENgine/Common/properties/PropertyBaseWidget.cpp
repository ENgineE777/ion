
#include "Core/Core.h"

#ifdef EDITOR

HashMap<BaseWidget> PropertyBaseWidgetsHolder::widgets(_FL_);

BaseWidget* PropertyBaseWidgetsHolder::GetWidget(const char* owner_name, const char* prop_name)
{
	char name[512];
	String::Printf(name, 512, "%s_%s", owner_name, prop_name);

	return widgets.Get(name);
}

void PropertyBaseWidgetsHolder::AddWidget(BaseWidget* widget, const char* owner_name, const char* prop_name)
{
	char name[512];
	String::Printf(name, 512, "%s_%s", owner_name, prop_name);
	widget->Show(false);

	widgets.Add(name, widget);
}

PropertyBaseWidget::Task::Task()
{
	pProperty = NULL;
	bActive = false;
	pOwner = NULL;
}

void PropertyBaseWidget::Task::ApplyChanges(float dt, int level)
{
	if(pProperty)
	{
		if(pOwner)
            pOwner->Apply();
		core->DelTask(this,(TaskHolder::Task)&PropertyBaseWidget::Task::ApplyChanges);
		bActive = false;
	}
}

void PropertyBaseWidget::Task::Start(float delay)
{
	if(pProperty)
	{
		if(!bActive)
		{
			core->AddTask(this,this, (TaskHolder::Task)&PropertyBaseWidget::Task::ApplyChanges, 0, delay);
			bActive = true;
		}
		else
		{
			core->DelTask(this,(TaskHolder::Task)&PropertyBaseWidget::Task::ApplyChanges);
			core->AddTask(this,this, (TaskHolder::Task)&PropertyBaseWidget::Task::ApplyChanges, 0, delay);
		}
	}
}
void PropertyBaseWidget::Task::Init(PropertyBase* prop, PropertyBaseWidget* owner)
{
	pProperty = prop;
	pOwner = owner;
}
	
float PropertyBaseWidget::apply_property_delay = 0.85f;
float PropertyBaseWidget::property_label_width = 140.f;

PropertyBaseWidget::PropertyBaseWidget(BaseWidget* parent) : LabelWidget(2000, parent, "", false, 0, 0, 200,18,BaseWidget::eAutoSizeWidth),owner(null)
{

}



#endif