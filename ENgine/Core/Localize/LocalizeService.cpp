 #include "Core/Core.h"

void LocalizeService::Init()
{
    /*IStreamLoader* loader = core->Files()->CreateStreamLoader("Loc/localize.loc", "Tags", IStreamLoader::xml);
    if(loader)
    {
        while (loader->EnterBlock("Tag"))
        {			
            char sKey[512],sValue[512];
            sKey[0] = sValue[0] = 0;
            loader->Read("key", sKey,512);	
            while (loader->EnterBlock("Locale"))
            {
                char sName[4];
                sName[0] = 0;
                loader->Read("name", sName,4);	
                loader->Read("value",sValue,512);	
                Locale* pLocale = vLocales.Find(sName);
                if(!pLocale)
                {
                    vLocales[sName] = Locale(sName);
                    pLocale = vLocales.Find(sName);
                }
                pLocale->vTags[sKey] = sValue;
                loader->LeaveBlock();
            }
            loader->LeaveBlock();
        }
        loader->Release();
    }*/
}

const string& LocalizeService::GetString(const string& key)
{
    Locale* pLocale = vLocales.Find(core->GetLocale());
    if(pLocale)
        return pLocale->GetString(key);
    else
      return sLocaleError;
}

void LocalizeService::GetString(const string& key,string& sOut)
{
    Locale* pLocale = vLocales.Find(core->GetLocale());
    if(pLocale)pLocale->GetString(key,sOut);
    else sOut = sLocaleError;
}

std::string LocalizeService::sLocaleError = "Locale_Error";


std::string LocalizeService::Locale::sBuff;
const string& LocalizeService::Locale::GetString(const string& key)
{
    sBuff.clear();
    string* pString = vTags.Find(key);
    if(pString)
        sBuff = *pString;
    else
        sBuff = "Locale_TAG_Error";
    return sBuff;
}

void LocalizeService::Locale::GetString(const string& key,string& sOut)
{
    sOut.clear();
    string* pString = vTags.Find(key);
    if(pString)
        sOut = *pString;
    else
        sOut = "Locale_TAG_Error";
}
