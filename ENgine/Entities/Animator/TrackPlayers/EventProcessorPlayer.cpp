
#include "EventProcessorPlayer.h"
#include "DeepBlack/World.h"

TRACKDESC(EventProcessorPlayer, "EventProcessor", 0)

void EventProcessorPlayer::Init()
{	
	AddStringDataTrack( Name , NULL, (TrackPlayer::ChangeKeyFunc)&EventProcessorPlayer::ExecuteFunc)	
}

void EventProcessorPlayer::ExecuteFunc(TrackStringPoint* func)
{	
	CWorld::instance()->getUpdateScene()->executeEvent(func->val);
}