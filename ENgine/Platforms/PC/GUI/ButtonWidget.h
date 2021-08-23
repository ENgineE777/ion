
#pragma once

#include "BaseWidget.h"
#include "Uxtheme.h"

class DEF_EXPORT ButtonWidget : public BaseWidget
{
	bool use_image;
	HBITMAP image;
	HBITMAP push_image;
	HBITMAP howered_image;
	HBITMAP howered_push_image;

	bool is_pushed;
	bool is_pushable;

	bool is_howered;
    bool bEnable;

    int nFlag;

public:

	HTHEME hTheme;

	ButtonWidget(int id, BaseWidget* parent, bool use_image, const char* txt, float x, float y, float w, float h,EAutoSizeType sizerType = eAutoSizeNone);
	virtual ~ButtonWidget();

	virtual void SetImage(const char* image_name);
	virtual void SetPushedImage(const char* image_name);
	virtual void SetHoweredImage(const char* image_name);
	virtual void SetHoweredPushedImage(const char* image_name);
	virtual void ProcessWidget(dword msg, WPARAM wParam, LPARAM lParam);
	virtual void SetText(const char* txt);
    virtual void SetFlag(int flag){nFlag = flag;}
    virtual int  GetFlag() const {return nFlag;}
	
	virtual void NotifyMouseOver(BaseWidget* widget);
	virtual void OnMouseLeave();

	bool IsPushable();
	void SetPushable(bool set);

	bool IsPushed();
	void SetPushed(bool set);
    void SetEnable(bool bSet);

	virtual void Draw();

};
