#if defined(PC) || defined(OSUNIX)
#include "HistoryObject.h"
#include "Core/ICore.h"
HistoryBuffer HistoryBuffer::gBuffer;


void HistoryBuffer::MoveForward()
{
    if(!vHistoryForward.empty())
    {
        IHistoryObject * pObject = vHistoryForward.back();
        pObject->MoveForward();
        vHistoryBack.push_back(pObject);
        vHistoryForward.pop_back();
    }
}

void HistoryBuffer::MoveBackward()
{
    if(!vHistoryBack.empty())
    {
        IHistoryObject * pObject = vHistoryBack.back();
        pObject->MoveBack();
        vHistoryForward.push_back(pObject);
        vHistoryBack.pop_back();
    }
}

void HistoryBuffer::Clear()
{
    ClearHistory(vHistoryForward);
    ClearHistory(vHistoryBack);
}

void HistoryBuffer::ClearHistory(THistory& vHistory)
{
    if(!vHistory.empty())
    {
        for(unsigned i = 0;i < vHistory.size();++i)
            vHistory[i]->ClearHistory();
        vHistory.clear();
    }
}

double IHistoryObject::GetDeltaTime()
{
    return core->GetDeltaTime();
}


#endif