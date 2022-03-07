#include <CavestoryModAPI.h>

#include "../../BitStream.h"
#include "GameDemo.h"
#include "GameDemoEvents.h"
#include "NpCharData.h"
#include "NpcHit.h"
#include "Fade.h"
#include "Main.h"
#include "MycParam.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define DEFINE_EVENT_FIELDS(EVENTNAME)							inline int _EVENT_READ_OR_WRITE_##EVENTNAME(bool bWrite, int iVersion, GAME_DEMO_STATE* pDemo, SharedBitStream* pStream, PXDEMO_EVENT_STRUCT* pEvent)
#define DEFINE_EVENT_PROCESS(EVENTNAME)							inline int _EVENT_PROCESS_##EVENTNAME(GAME_DEMO_STATE* pDemo, PXDEMO_EVENT_STRUCT* pEvent)
#define DEFINE_EVENT_COMBINE(EVENTNAME)							inline int _EVENT_COMBINE_EVENT_##EVENTNAME(int iFrameOffset, GAME_DEMO_STATE* pDemo, GAME_DEMO_STATE* pSource, PXDEMO_EVENT_STRUCT* pEvent, PXDEMO_EVENT_STRUCT* pSourceEvent)
#define IMPLEMENT_DEMO_EVENT(EVENTNAME)							{ _EVENT_READ_OR_WRITE_##EVENTNAME, _EVENT_PROCESS_##EVENTNAME, _EVENT_COMBINE_EVENT_##EVENTNAME }
#define DETERMINE_FUNC											void(SharedBitStream::*__PROC_FIELD)(void* val, unsigned long long int bitCount) = (bWrite ? &SharedBitStream::WriteInt2 : &SharedBitStream::ReadInt2);
#define INIT_DEMO_FUNC(EVENTNAME, DATANAME)						EVENTNAME* P_DATA                                             = &pEvent->data.##DATANAME;
#define INIT_DEMO_COMBINE_FUNC(EVENTNAME, DATANAME)				EVENTNAME* P_DATA                                             = &pEvent->data.##DATANAME;\
																EVENTNAME* P_DATA2                                            = &pSourceEvent->data.##DATANAME;
#define ADD_FIELD(SIZE, FIELDNAME)								(*pStream.*__PROC_FIELD)(&P_DATA->##FIELDNAME, SIZE);
#define EDATA(FIELDNAME)										(P_DATA->##FIELDNAME)
#define EDATA2(FIELDNAME)										(P_DATA2->##FIELDNAME)
#define SIZE_BOOL												1
#define SIZE_UCHAR												(sizeof(unsigned char) << 3)
#define SIZE_USHORT												(sizeof(unsigned short) << 3)
#define SIZE_UINT												(sizeof(unsigned int) << 3)
#define SIZE_ULONGLONG											(sizeof(unsigned long long) << 3)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DEFINE_EVENT_FIELDS(EVENT_NULL_STRUCT)
{
	return 0;
}

DEFINE_EVENT_PROCESS(EVENT_NULL_STRUCT)
{
	return 0;
}

DEFINE_EVENT_COMBINE(EVENT_NULL_STRUCT)
{
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DEFINE_EVENT_FIELDS(EVENT_SETPLAYERSTATE_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_SETPLAYERSTATE_STRUCT, player_state);
	DETERMINE_FUNC;

	//			Size (Bits)					Field Name
	ADD_FIELD( SIZE_UINT,					player_idx);
	ADD_FIELD( SIZE_UINT,					change_flags);

	char DataBuffer[PXDPSC_BIGGEST_TYPE + 1];

	for (int i = 0; i < PXDPSC_COUNT; i++)
	{
		DEMO_PLAYERSTATE_FIELD* pField = &gDemoPlayerstateFieldList[i];

		// Skip flags that we don't have
		if (!(EDATA(change_flags) & pField->change_flag))
			continue;

		memset(DataBuffer, 0, sizeof(DataBuffer));

		if (bWrite)
		{
			// Write it to the stream
			pStream->Write(pField->GetValue(&EDATA(state), DataBuffer), pField->file_size);
		}
		else
		{
			// Read it from the stream
			pStream->Read(DataBuffer, pField->file_size);

			pField->SetValue(&EDATA(state), DataBuffer);
		}
	}


	// Set new pointers

	return 0;
}

DEFINE_EVENT_PROCESS(EVENT_SETPLAYERSTATE_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_SETPLAYERSTATE_STRUCT, player_state);

	// Copy the new memory into the state
	char DataBuffer[PXDPSC_BIGGEST_TYPE + 1];

	for (int i = 0; i < PXDPSC_COUNT; i++)
	{
		DEMO_PLAYERSTATE_FIELD* pField = &gDemoPlayerstateFieldList[i];

		// Skip flags that we don't have
		if (!(EDATA(change_flags) & pField->change_flag))
			continue;

		pField->SetValue(&pDemo->mPlayers.list[EDATA(player_idx)].state, pField->GetValue(&EDATA(state), DataBuffer));
	}

	return 0;
}

DEFINE_EVENT_COMBINE(EVENT_SETPLAYERSTATE_STRUCT)
{
	INIT_DEMO_COMBINE_FUNC(EVENT_SETPLAYERSTATE_STRUCT, player_state);

	// Configure new with old
	CSM_GameDemo_Players_Find(pDemo, pSource->mPlayers.list[EDATA2(player_idx)].name, &EDATA(player_idx));
	EDATA(change_flags) = EDATA2(change_flags);

	// Copy the old data to the new event
	memcpy(&EDATA(state), &EDATA2(state), sizeof(PXDEMO_PLAYERSTATE_STRUCT));

	return 0;
}

//----------------//

DEFINE_EVENT_FIELDS(EVENT_SETSTAGE_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_SETSTAGE_STRUCT, stage);
	DETERMINE_FUNC;

	//			Size (Bits)		Field Name
	ADD_FIELD(	SIZE_USHORT,	stage_idx)

	return 0;
}

DEFINE_EVENT_PROCESS(EVENT_SETSTAGE_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_SETSTAGE_STRUCT, stage);

	// Copy the new memory into the state
	if (gStageNo != EDATA(stage_idx))
		TransferStage(EDATA(stage_idx), 0, 0, 0);

	for (int i = 0; i < NPC_MAX; i++)
		if (gNPC[i].code_char >= 0 && gNPC[i].code_char < gCaveMod.mNpcs.count && !(gCaveMod.mNpcs.list[gNPC[i].code_char].bits & NPC_flags::npc_dontUpdate))
			gNPC[i].cond = 0;

	memset(&gBoss, 0, sizeof(gBoss));

	return 0;
}

DEFINE_EVENT_COMBINE(EVENT_SETSTAGE_STRUCT)
{
	INIT_DEMO_COMBINE_FUNC(EVENT_SETSTAGE_STRUCT, stage);

	// Set new data
	memcpy(P_DATA, P_DATA2, sizeof(EVENT_SETSTAGE_STRUCT));

	// Done
	return 0;
}

//----------------//

DEFINE_EVENT_FIELDS(EVENT_SETRANDOMSEED_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_SETRANDOMSEED_STRUCT, random_seed);
	DETERMINE_FUNC;

	//			Size (Bits)		Field Name
	ADD_FIELD(	SIZE_UINT,		seed)

	return 0;
}

DEFINE_EVENT_PROCESS(EVENT_SETRANDOMSEED_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_SETRANDOMSEED_STRUCT, random_seed);

	// Set the random seed
	rep_srand(EDATA(seed));

	return 0;
}

DEFINE_EVENT_COMBINE(EVENT_SETRANDOMSEED_STRUCT)
{
	INIT_DEMO_COMBINE_FUNC(EVENT_SETRANDOMSEED_STRUCT, random_seed);

	// Set new data
	memcpy(P_DATA, P_DATA2, sizeof(EVENT_SETRANDOMSEED_STRUCT));

	// Done
	return 0;
}

//----------------//

DEFINE_EVENT_FIELDS(EVENT_SETNPCSTATE_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_SETNPCSTATE_STRUCT, npc_state);
	DETERMINE_FUNC;

	//			Size (Bits)		Field Name
	ADD_FIELD(	SIZE_USHORT,	npc_idx)
	ADD_FIELD(	SIZE_USHORT,	state.code_char)
	ADD_FIELD(	SIZE_UINT,		random_seed)

	if (!bWrite)
	{
		pEvent->data.npc_state.npc_idx         = pStream->ReadRangedInt(0, NPC_MAX + BOSS_MAX);
		pEvent->data.npc_state.state.code_char = pStream->ReadInt(sizeof(unsigned short) << 3);

		// Read NPC transmit data
		for (int i = 0; i < (sizeof(gNPCTransmitNodeArray) / sizeof(TransmitNodeInfo)); i++)
		{
			TransmitNodeInfo* pNode = &gNPCTransmitNodeArray[i];
			unsigned int iNodeValue = 0;

			// Skip transmit nodes that we don't have for this NPC
			if (EDATA(state).code_char < 0 || EDATA(state).code_char >= gCaveMod.mNpcs.count || !(gCaveMod.mNpcs.list[EDATA(state).code_char].transmit_nodes & pNode->node_flag))
				continue;

			// Use range first
			if (pNode->use_range)
			{
				// Read the ranged value
				iNodeValue = pStream->ReadRangedInt(pNode->range_min, pNode->range_max);
			}
			else
			{
				// Read the value
				iNodeValue = pStream->ReadInt(pNode->bit_size);
			}

			// Get special values
			if (pNode->node_flag == NPC_transmit_nodes::npc_transmit_nodes_pNpc)
			{
				EDATA(state).pNpc = GetPtrNpCharByIndex(iNodeValue);

				continue;
			}
			else if (pNode->node_flag == NPC_transmit_nodes::npc_transmit_nodes_focus)
			{
				EDATA(state).focus = &gMC; //pDemo->mPlayers.list[CLAMP(iNodeValue, 0, pDemo->mPlayers.count)].state;

				continue;
			}
			else if (pNode->node_flag == NPC_transmit_nodes::npc_transmit_nodes_focus2)
			{
				EDATA(state).focus2 = &gMC; //pDemo->mPlayers.list[CLAMP(iNodeValue, 0, pDemo->mPlayers.count)].state;

				continue;
			}

			// Set the value
			memcpy(pNode->GetFieldFunc(&EDATA(state)), &iNodeValue, pNode->size_of_field);
		}

		return 0;
	}

	pStream->WriteRangedInt(pEvent->data.npc_state.npc_idx,		0, NPC_MAX + BOSS_MAX);
	pStream->WriteInt(pEvent->data.npc_state.state.code_char,	sizeof(unsigned short) << 3);

	// Write NPC transmit data
	for (int i = 0; i < (sizeof(gNPCTransmitNodeArray) / sizeof(TransmitNodeInfo)); i++)
	{
		TransmitNodeInfo* pNode = &gNPCTransmitNodeArray[i];
		unsigned int iNodeValue = 0;

		// Skip transmit nodes that we don't have for this NPC
		if (!(gCaveMod.mNpcs.list[EDATA(state).code_char].transmit_nodes & pNode->node_flag))
			continue;

		// Get the value of this field
		memcpy_s(&iNodeValue, sizeof(unsigned int), pNode->GetFieldFunc(&EDATA(state)), pNode->size_of_field);

		// Get special values
		if (pNode->node_flag == NPC_transmit_nodes::npc_transmit_nodes_pNpc)
		{
			iNodeValue = GetPtrNpCharIndex(EDATA(state).pNpc);
		}
		else if (pNode->node_flag == NPC_transmit_nodes::npc_transmit_nodes_focus)
		{
			iNodeValue = -1;

			if (EDATA(state).client)
				CSM_GameDemo_Players_Find(pDemo, EDATA(state).client->mCache.net_name, (int*)&iNodeValue);
		}
		else if (pNode->node_flag == NPC_transmit_nodes::npc_transmit_nodes_focus2)
		{
			iNodeValue = -1;

			if (EDATA(state).client2)
				CSM_GameDemo_Players_Find(pDemo, EDATA(state).client2->mCache.net_name, (int*)&iNodeValue);
		}

		// Use range first
		if (pNode->use_range)
		{
			// Write the ranged value
			pStream->WriteRangedInt(iNodeValue, pNode->range_min, pNode->range_max);

			// Next
			continue;
		}

		// Write the value
		pStream->WriteInt(iNodeValue, pNode->bit_size);
	}

	return 0;
}

DEFINE_EVENT_PROCESS(EVENT_SETNPCSTATE_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_SETNPCSTATE_STRUCT, npc_state);

	// Get the NPC we're referring to
	NPCHAR* pNpc    = (EDATA(npc_idx) >= NPC_MAX ? &gBoss[EDATA(npc_idx) - NPC_MAX] : &gNPC[EDATA(npc_idx)]);
	pNpc->code_char = CLAMP(EDATA(state.code_char), 0, gCaveMod.mNpcs.count - 1);

	if (EDATA(state).code_char == 3)
	{
		pNpc->exp = 0;
		LoseNpChar(pNpc, TRUE);
	}

	srand(EDATA(random_seed));
	
	// Initialize the NPC
	SetUniqueParameter(pNpc);

	// If the NPC's character ID is set to 0, then it's nothing.
	if (pNpc->code_char == 0)
	{
		pNpc->cond = 0;
		return 0;
	}

	// Copy the NPC's data
	for (int i = 0; i < (sizeof(gNPCTransmitNodeArray) / sizeof(TransmitNodeInfo)); i++)
	{
		TransmitNodeInfo* pNode = &gNPCTransmitNodeArray[i];
		unsigned int iNodeValue = 0;

		// Skip transmit nodes that we don't have for this NPC
		if (!(gCaveMod.mNpcs.list[pNpc->code_char].transmit_nodes & pNode->node_flag))
			continue;

		memcpy(pNode->GetFieldFunc(pNpc), pNode->GetFieldFunc(&EDATA(state)), pNode->size_of_field);
	}

	// Set 'alive'
	pNpc->cond |= 0x80;
	pNpc->bits |= NPC_flags::npc_ghost;

	return 0;
}

DEFINE_EVENT_COMBINE(EVENT_SETNPCSTATE_STRUCT)
{
	INIT_DEMO_COMBINE_FUNC(EVENT_SETNPCSTATE_STRUCT, npc_state);

	// Set new data
	memcpy(P_DATA, P_DATA2, sizeof(EVENT_SETNPCSTATE_STRUCT));

	// Done
	return 0;
}

//----------------//

DEFINE_EVENT_FIELDS(EVENT_DOSCREENFADE_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_DOSCREENFADE_STRUCT, screen_fade);
	DETERMINE_FUNC;

	//			Size (Bits)		Field Name
	ADD_FIELD( SIZE_UCHAR,		type);

	// Only write & read  the direction if the fade type needs it!
	if (EDATA(type) != PXDEMO_FADE_TYPE::PXDFT_CLEAR_FADE)
		ADD_FIELD( SIZE_UCHAR,		direct);

	return 0;
}

DEFINE_EVENT_PROCESS(EVENT_DOSCREENFADE_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_DOSCREENFADE_STRUCT, screen_fade);

	// Perform a screen fade
	switch (EDATA(type))
	{
		case PXDEMO_FADE_TYPE::PXDFT_CLEAR_FADE:
		{
			ClearFade();
			break;
		}
		case PXDEMO_FADE_TYPE::PXDFT_FADE_IN:
		{
			StartFadeIn(EDATA(direct));
			break;
		}
		case PXDEMO_FADE_TYPE::PXDFT_FADE_OUT:
		{
			StartFadeOut(EDATA(direct));
			break;
		}
	}

	return 0;
}

DEFINE_EVENT_COMBINE(EVENT_DOSCREENFADE_STRUCT)
{
	INIT_DEMO_COMBINE_FUNC(EVENT_DOSCREENFADE_STRUCT, screen_fade);

	// Set new data
	memcpy(P_DATA, P_DATA2, sizeof(EVENT_DOSCREENFADE_STRUCT));

	// Done
	return 0;
}

//----------------//

DEFINE_EVENT_FIELDS(EVENT_SETCAMERA_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_SETCAMERA_STRUCT, camera);
	DETERMINE_FUNC;

	//			Size (Bits)		Field Name
	ADD_FIELD( SIZE_UINT,		x);
	ADD_FIELD( SIZE_UINT,		y);
	ADD_FIELD( SIZE_UINT,		target);
	ADD_FIELD( SIZE_UINT,		wait);

	return 0;
}

DEFINE_EVENT_PROCESS(EVENT_SETCAMERA_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_SETCAMERA_STRUCT, camera);

	gFrame.x      = EDATA(x);
	gFrame.y      = EDATA(y);
	gFrame.quake  = EDATA(quake);
	gFrame.quake2 = EDATA(quake2);
	gFrame.wait   = EDATA(wait);

	// Perform a screen fade
	if (EDATA(target) >= 0 && EDATA(target) < pDemo->mPlayers.count)
	{
		gFrame.tgt_x = &pDemo->mPlayers.list[EDATA(target)].state.x;
		gFrame.tgt_y = &pDemo->mPlayers.list[EDATA(target)].state.y;
	}
	else if (EDATA(target) >= pDemo->mPlayers.count && EDATA(target) < NPC_MAX + pDemo->mPlayers.count)
	{
		gFrame.tgt_x = &gNPC[EDATA(target) - pDemo->mPlayers.count].x;
		gFrame.tgt_y = &gNPC[EDATA(target) - pDemo->mPlayers.count].y;
	}
	else if (EDATA(target) >= pDemo->mPlayers.count + NPC_MAX && EDATA(target) < pDemo->mPlayers.count + NPC_MAX + BOSS_MAX)
	{
		gFrame.tgt_x = &gBoss[EDATA(target) - (pDemo->mPlayers.count + NPC_MAX)].x;
		gFrame.tgt_y = &gBoss[EDATA(target) - (pDemo->mPlayers.count + NPC_MAX)].y;
	}

	return 0;
}

DEFINE_EVENT_COMBINE(EVENT_SETCAMERA_STRUCT)
{
	INIT_DEMO_COMBINE_FUNC(EVENT_SETCAMERA_STRUCT, camera);

	// Set new data
	memcpy(P_DATA, P_DATA2, sizeof(EVENT_SETCAMERA_STRUCT));

	// Done
	return 0;
}

//----------------//

DEFINE_EVENT_FIELDS(EVENT_SETTIMER_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_SETTIMER_STRUCT, timer);
	DETERMINE_FUNC;

	//			Size (Bits)		Field Name
	ADD_FIELD( SIZE_UINT,		time_passed);

	return 0;
}

DEFINE_EVENT_PROCESS(EVENT_SETTIMER_STRUCT)
{
	INIT_DEMO_FUNC(EVENT_SETTIMER_STRUCT, timer);

	// Set time passed
	SetTimeCounter(EDATA(time_passed));

	return 0;
}

DEFINE_EVENT_COMBINE(EVENT_SETTIMER_STRUCT)
{
	INIT_DEMO_COMBINE_FUNC(EVENT_SETTIMER_STRUCT, timer);

	// Set new data
	memcpy(P_DATA, P_DATA2, sizeof(EVENT_SETTIMER_STRUCT));

	// Done
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PXDEMO_EVENT_INFO gDemoEventList[] =
{
	/* PXDET_INVALID_EVENT    */ IMPLEMENT_DEMO_EVENT(EVENT_NULL_STRUCT),
	/* PXDET_SET_PLAYER_STATE */ IMPLEMENT_DEMO_EVENT(EVENT_SETPLAYERSTATE_STRUCT),
	/* PXDET_SET_STAGE        */ IMPLEMENT_DEMO_EVENT(EVENT_SETSTAGE_STRUCT),
	/* PXDET_SET_RANDOM_SEED  */ IMPLEMENT_DEMO_EVENT(EVENT_SETRANDOMSEED_STRUCT),
	/* PXDET_SET_NPC_STATE    */ IMPLEMENT_DEMO_EVENT(EVENT_SETNPCSTATE_STRUCT),
	/* PXDET_DO_SCREEN_FADE   */ IMPLEMENT_DEMO_EVENT(EVENT_DOSCREENFADE_STRUCT),
	/* PXDET_SET_CAMERA       */ IMPLEMENT_DEMO_EVENT(EVENT_SETCAMERA_STRUCT),
	/* PXDET_SET_TIMER        */ IMPLEMENT_DEMO_EVENT(EVENT_SETTIMER_STRUCT),
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------