#pragma once
class ILocalizeService
{		
public:		
	virtual void            Init() = 0;
    virtual const string&   GetString(const string& key ) = 0;
    virtual void            GetString(const string& key,string& sOut) = 0;
};