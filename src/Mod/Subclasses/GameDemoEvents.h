/// Do NOT include in the docs / mod api source code -- for internal use

#pragma once

struct DEMO_PLAYERSTATE_FIELD
{
#ifdef _DEBUG
	const char* field_name;
#endif

	// size to be written & read (in bits)
	int file_size;

	// size of the variable (in bytes)
	int size;

	// The change flag that controls this field
	int change_flag;

	// The offset of the field in PXDEMO_PLAYERSTATE_STRUCT
	int offset;

	// Get the value
	void*(*GetValue)(PXDEMO_PLAYERSTATE_STRUCT*, void*);

	// Set the value
	void*(*SetValue)(PXDEMO_PLAYERSTATE_STRUCT*, void*);
};

extern DEMO_PLAYERSTATE_FIELD gDemoPlayerstateFieldList[PXDPSC_COUNT];

struct PXDEMO_EVENT_INFO
{
	int(*ReadOrWrite)(bool bWrite, int iVersion, GAME_DEMO_STATE* pDemo, SharedBitStream* pStream, PXDEMO_EVENT_STRUCT* pEvent);
	int(*RunEvent)(GAME_DEMO_STATE* pDemo, PXDEMO_EVENT_STRUCT* pEvent);
	int(*CombineDemos)(int iFrameOffset, GAME_DEMO_STATE* pDemo, GAME_DEMO_STATE* pSource, PXDEMO_EVENT_STRUCT* pEvent, PXDEMO_EVENT_STRUCT* pSourceEvent);
};

extern PXDEMO_EVENT_INFO gDemoEventList[PXDET_MAX_EVENTS];