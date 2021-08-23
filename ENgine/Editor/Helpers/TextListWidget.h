#pragma  once

#ifdef EDITOR
#include "Platforms\PC\GUI\ListBoxWidget.h"

struct TextListWidget
{
public:
	enum TextType
	{
		eLogInfo = 0,
		eLogWarning = 1,
		eLogError = 2,
		eFindResult = 3,
	};  
private:
	struct LoggerString
	{
		TextType	 eType;
		string       sText;
		string       sData;
		LoggerString():eType(eLogInfo){};
		LoggerString(TextType type,const string& text):eType(type),sText(text){};
		LoggerString(TextType type,const string& text,const string& data):eType(type),sText(text),sData(data){};
	};
	std::vector<LoggerString> vLogs;
	ListBoxWidget*      pWidget;
	void Copy(const TextListWidget& other)
	{
		pWidget = other.pWidget;
		vLogs.assign(other.vLogs.begin(),other.vLogs.end());
	}
public:
	TextListWidget():pWidget(NULL){}
	TextListWidget(BaseWidget* pParent,const char* sName,unsigned nId){CreateWidget(pParent,sName,nId);}
	TextListWidget(const TextListWidget& other){Copy(other);}
	TextListWidget& operator = (const TextListWidget& other){Copy(other);return *this;}        
	void AddString(const string& str,TextType type)
	{
		vLogs.push_back(LoggerString(type,str));
		if(pWidget)
			pWidget->AddString(str.c_str(),type);
	}
	void AddString(const string& str,TextType type,string sData)
	{
		vLogs.push_back(LoggerString(type,str,sData));
		if(pWidget)
			pWidget->AddString(str.c_str(),type);
	}
	void OnWidgetDestroy(){pWidget = NULL;}
	string GetData(int index)
	{
		if(index < vLogs.size())
			return vLogs[index].sData;
		return "";
	}
	ListBoxWidget* CreateWidget(BaseWidget* pParent,const char* sName,unsigned nId)
	{
		pWidget = new ListBoxWidget(nId, pParent,sName, pParent->GetXOffset(), pParent->GetYOffset(), 100, 100, false, false, BaseWidget::eAutoSizeBoth);
		pWidget->Resize();
		for(unsigned i =0 ;i < vLogs.size();++i)
			pWidget->AddString(vLogs[i].sText.c_str(),vLogs[i].eType);
		return pWidget;
	}
	ListBoxWidget* Widget()const{ return pWidget;}
	void Clear()
	{
		vLogs.clear();
		if(pWidget)
			pWidget->ClearList();
	}
};
typedef HashTable<string,TextListWidget> TLoggerWidgets;

#endif