
#pragma once

#ifdef EDITOR

#include "PropertyBaseWidget.h"

class DEF_EXPORT PropertyFontWidget : public PropertyBaseWidget,public PropertiesHistoryObject<FontParams>
{
	static int used_id;
	int    id;	
    float  nNewValue;

    float*         pEditValue;
    TextBoxWidget* pEditBox;

    inline void SetTextBoxValue(TextBoxWidget* pBox,float nValue);
           void UpdateWidgets();

public:

	PropertyFontWidget(BaseWidget* parent);
	FontParams* value;

	LabelWidget*    prop_label_name;	
	ComboBoxWidget* prop_cb_name;

	LabelWidget*    prop_label_style;	
	ComboBoxWidget* prop_cb_style;	

	LabelWidget*    prop_label_height;	
	TextBoxWidget*  prop_ed_height;

	LabelWidget*    prop_label_outline;	
	TextBoxWidget*  prop_ed_outline;

	LabelWidget*    prop_label_gap;	
	TextBoxWidget*  prop_ed_gap;

	LabelWidget*    prop_label_color;	
	LabelWidget*    prop_ed_color;

	LabelWidget*    prop_label_use_shd;	
	CheckBoxWidget* prop_cb_use_shd;

	LabelWidget*    prop_label_shd_color;	
	LabelWidget*    prop_ed_shd_color;

	void InitControls();	
	void StartEdit(FontParams* vl, const char* name);

	void UpdateStyleWidget();

	virtual bool CheckChange();
    virtual void Apply(const FontParams& data);
    virtual void Apply();
    virtual void Show(bool show);
};

void PropertyFontWidget::SetTextBoxValue(TextBoxWidget* pBox,float nValue)
{
    char str[32];
    String::Printf(str,32, "%4.3f",nValue);
    pBox->SetText(str);
}

#endif