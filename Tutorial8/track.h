#include "cho_array.h"

#ifndef TRACK_H_9283749823
#define TRACK_H_9283749823

enum TRACK_EVENT_TYPE : int32_t
	{ TRACK_EVENT_TYPE_ENEMY			= 0
	, TRACK_EVENT_TYPE_SOUND_START
	, TRACK_EVENT_TYPE_SOUND_STOP
	, TRACK_EVENT_TYPE_PARTICLE
	, TRACK_EVENT_TYPE_MODE_AUTO
	, TRACK_EVENT_TYPE_MODE_MANUAL
	, TRACK_EVENT_TYPE_COUNT
	, TRACK_EVENT_TYPE_INVALID			= -1
	};

struct STrackEvent {
						float										TimeStart							;
						float										Duration							;
						int32_t										PayloadId							;
						TRACK_EVENT_TYPE							Type								;
};

struct STrackEventTable {
						::cho::array_pod<float				>		EventTimeStart						;
						::cho::array_pod<float				>		EventDuration						;
						::cho::array_pod<int32_t			>		EventPayloadId						;
						::cho::array_pod<TRACK_EVENT_TYPE	>		EventType							;

						void										Clear								()									noexcept		{ ::cho::clear(EventTimeStart, EventDuration, EventPayloadId, EventType); }
						::cho::error_t								PushEvent							(const STrackEvent& eventToPush)					{
		if(-1 == EventTimeStart	.push_back(eventToPush.TimeStart	)) { ::cho::resize(EventType.size(), EventTimeStart, EventDuration, EventPayloadId, EventType); error_printf("Out of memory?"); return -1; }
		if(-1 == EventDuration	.push_back(eventToPush.Duration		)) { ::cho::resize(EventType.size(), EventTimeStart, EventDuration, EventPayloadId, EventType); error_printf("Out of memory?"); return -1; }
		if(-1 == EventPayloadId	.push_back(eventToPush.PayloadId	)) { ::cho::resize(EventType.size(), EventTimeStart, EventDuration, EventPayloadId, EventType); error_printf("Out of memory?"); return -1; }
		if(-1 == EventType		.push_back(eventToPush.Type			)) { ::cho::resize(EventType.size(), EventTimeStart, EventDuration, EventPayloadId, EventType); error_printf("Out of memory?"); return -1; }
		return 0;
	}
};

struct STrack {
						float										TimeStart							;
						float										Duration							;
						STrackEventTable							Table								;
};

#endif // TRACK_H_9283749823
