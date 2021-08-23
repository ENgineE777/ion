#pragma once
#include "ILocalizeService.h"

class LocalizeService: public ILocalizeService
{
    typedef HashTable<string,string>  TTags;
    struct Locale
    {
        static string sBuff;
        string  sName;
        TTags   vTags;
        const string&   GetString(const string& key );
        void            GetString(const string& key,string& sOut);
        Locale&  operator = (const Locale& other)
        {
            sName = other.sName;
            vTags = other.vTags;
            return *this;
        }
        Locale():vTags(128){}
        Locale(const char* name):vTags(128),sName(name){}
        Locale(const Locale& other)
        {
            sName = other.sName;
            vTags = other.vTags;
        }
    };
    typedef HashTable<string,Locale>  TLocales;

    TLocales                  vLocales;
    static string             sLocaleError;
public:		
	LocalizeService():vLocales(128){}
	virtual void            Init();
    virtual const string&   GetString(const string& key );
    virtual void            GetString(const string& key,string& sOut);

};