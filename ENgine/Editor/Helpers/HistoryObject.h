#pragma once
#if defined(PC) || defined(OSUNIX)
#include <vector>
class IHistoryObject
{
protected:
    double GetDeltaTime();
public:
    virtual void MoveBack() = 0;
    virtual void MoveForward() = 0;
    virtual void ClearHistory() = 0;
};
typedef std::vector<IHistoryObject*> THistory;

class HistoryBuffer
{
public:
    static HistoryBuffer* GetInstance() {return &gBuffer;}

    void           AddToHistory(IHistoryObject* pObject){vHistoryBack.push_back(pObject);vHistoryForward.clear();}  
    void           MoveForward();
    void           MoveBackward();
    void           Clear();
private:
    static HistoryBuffer gBuffer;
    THistory  vHistoryBack;
    THistory  vHistoryForward;

    void ClearHistory(THistory& vHistory);
};


template<typename Type>
class PropertiesHistoryObject: public IHistoryObject
{
protected:
    std::vector<Type>  vHistory;
    unsigned           nCurrentIndex;
    Type               CurrentData;
    double             nTimeDelta;
    double             nLastTime; 
public:
    virtual void MoveBack();
    virtual void MoveForward();
    virtual void ClearHistory(){nCurrentIndex = 0;vHistory.clear();}

    virtual void Apply(const Type& data) = 0;

    void AddToHistory(const Type& data);
    void SetCurrentHistoryData(const Type& data){CurrentData = data;}

    PropertiesHistoryObject():nCurrentIndex(0),nTimeDelta(0),nLastTime(0){}
    PropertiesHistoryObject(const Type& currentData):nCurrentIndex(0),CurrentData(currentData),nTimeDelta(0),nLastTime(0){}
    PropertiesHistoryObject(const Type& currentData,double delta):nCurrentIndex(0),CurrentData(currentData),nTimeDelta(delta),nLastTime(0){}
private:
    void   AddToHistory(const Type& data,double nCurTime);

};
template<typename Type>
void PropertiesHistoryObject<Type>::MoveBack()
{
    if(nCurrentIndex > 0)
        Apply(vHistory[--nCurrentIndex]);
}

template<typename Type>
void PropertiesHistoryObject<Type>::MoveForward()
{
    if(nCurrentIndex  + 1 < vHistory.size())
        Apply(vHistory[++nCurrentIndex]);
    else
    {
        if(nCurrentIndex  < vHistory.size())++nCurrentIndex;
        Apply(CurrentData);
    }
}
#define HISTORY_SIZE 35

template<typename Type>
void PropertiesHistoryObject<Type>::AddToHistory(const Type& data,double nCurTime)
{
    if(vHistory.size() > HISTORY_SIZE)
    {
        for(unsigned i = 0;i < vHistory.size() - 1;++i)
            vHistory[i] = vHistory[i + 1];
        vHistory[vHistory.size() - 1] = data;
    }
    else
        vHistory.push_back(data);
    ++nCurrentIndex;
    HistoryBuffer::GetInstance()->AddToHistory(this);
    nLastTime = nCurTime;
}
template<typename Type>
void PropertiesHistoryObject<Type>::AddToHistory(const Type& data)
{
    if(nTimeDelta == 0)AddToHistory(data,0);
    else
    {
        double nCurTime  = GetDeltaTime();
        if(nLastTime == 0)
        {
         //   core->TraceTo("Debug","Add to history Time = %f delta = %f wait time  = %f",nCurTime,nCurTime - nLastTime,nTimeDelta);
            AddToHistory(data,nCurTime);
        }
        else
            if(nCurTime - nLastTime > nTimeDelta)
            {
              //  core->TraceTo("Debug","Add to history Time = %f delta = %f wait time  = %f",nCurTime,nCurTime - nLastTime,nTimeDelta);
                AddToHistory(data,nCurTime);
            }
    }
}
#endif