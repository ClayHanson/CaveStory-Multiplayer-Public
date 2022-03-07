#include <CavestoryModAPI.h>
#include <Subclasses/GameDemo.h>

#include "../../CaveStoryCharacterEditor/CustomPlayer.h"
#include "../../../CaveEdit/BitStream.h"
#include "GameDemoEvents.h"
#include "CaveNet/CaveNet.h"
#include "MyChar.h"
#include "MycParam.h"
#include "Main.h"
#include "MycHit.h"
#include "ValueView.h"
#include "Back.h"
#include "NpcHit.h"
#include "BulHit.h"
#include "Shoot.h"
#include "Caret.h"
#include "Flash.h"
#include "Map.h"
#include "BossLife.h"
#include "MapName.h"
#include "Flags.h"
#include "Fade.h"
#include "Game/System/Platform/Platform.h"
#include "Hooks.h"

#define PXDEMO_HEADER_STRING	"PXDEMO"
#define PXDEMO_VERSION			2
#define GAME_DEMO_CHECKSUM		0x71B381FC

#include <CSMAPI_begincode.h>

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GAME_DEMO_STATE* GAME_DEMO_STATE::first = NULL;
GAME_DEMO_STATE* gRecordingDemo         = NULL;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
	 FILE FORMAT FOR .PXDEMO:
	 * +-----+--------+--------------------------------------------------------------------------------------------------------------------------- -  -   -     -
	 * | VER |  TYPE  | DESCRIPTION
	 * +-----+--------+--------------------------------------------------------------------------------------------------------------------------- -  -   -     -
	 * |  1  | STRING | File Header ("PXDEMO")
	 * |  1  | UCHAR  | File Version
	 * |  1  | UINT   | Frame Object Count
	 * |  1  | UINT   | Player Count
	 * |  1  | BITS   | NPC flags
	 * |  1  | BITS   | Skip flags
	 * |     |        | >>==================================== CHUNK: Game_ Info ====================================<<
	 * |  1  | UINT   | CaveNet Version
	 * |  1  | UINT   | Length of "Mod Name" string
	 * |  1  | STRING | Mod Name
	 * |  1  | UINT   | Mod Version
	 * |  1  | UINT   | Simulation Frame Count
	 * |  1  | UINT   | Game flags
	 * |     |        | >>=================================== CHUNK: Player_ Info ===================================<<
	 * |  1  | UINT   | Length of "Client Name" string
	 * |  1  | STRING | Client Name
	 * |  1  | UINT   | Length of "Character Name" string
	 * |  1  | STRING | Character Name
	 * |  2  | BIT    | [If true, read the next three entries:] Player has custom color
	 * |  2  | UCHAR  | [DETERMINED] Red Color
	 * |  2  | UCHAR  | [DETERMINED] Green Color
	 * |  2  | UCHAR  | [DETERMINED] Blue Color
	 * |  1  | DETERM | (ARMS) Arms Info
	 * |  1  | DETERM | (PXDEMO_PLAYERSTATE_STRUCT) Player state
	 * |     |        | >>==================================== CHUNK: Frame Info ====================================<<
	 * |  1  | UINT   | Frame number
	 * |  1  | UINT   | Event count
	 * |     |        | >>==================================== CHUNK: Event Info ====================================<<
	 * |  1  | USHORT | Event Type
	 * |  1  | DETERM | Event Data
	 * +-----+--------+--------------------------------------------------------------------------------------------------------------------------- -  -   -     -
*/
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSM_GameDemo_Link(GAME_DEMO_STATE* pState)
{
	pState->mNext          = GAME_DEMO_STATE::first;
	GAME_DEMO_STATE::first = pState;
}

void CSM_GameDemo_Unlink(GAME_DEMO_STATE* pState)
{
	if (GAME_DEMO_STATE::first == pState)
	{
		GAME_DEMO_STATE::first = pState->mNext;
		return;
	}
	
	GAME_DEMO_STATE* prev = NULL;
	
	// Find ourself in the linkage
	for (GAME_DEMO_STATE* walk = GAME_DEMO_STATE::first; walk; walk = walk->mNext)
	{
		if (walk != pState)
		{
			prev = walk;
			continue;
		}

		if (prev)
			prev->mNext = pState->mNext;

		break;
	}
}

bool CSM_GameDemo_FirstLink(GAME_DEMO_STATE** pFirst)
{
	if (pFirst)
		*pFirst = GAME_DEMO_STATE::first;

	return GAME_DEMO_STATE::first != NULL;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void SET_PLAYER_STATE(MYCHAR* in, PXDEMO_PLAYERSTATE_STRUCT* out, SafeClientInterface* pInterface)
{
	// Set weapon data
	out->selectedArms		= *pInterface->mSelectedArms;
	out->weapon_code		= pInterface->mArms[*pInterface->mSelectedArms].code;
	out->weapon_ammo		= pInterface->mArms[*pInterface->mSelectedArms].num;
	out->weapon_max_ammo	= pInterface->mArms[*pInterface->mSelectedArms].max_num;
	out->weapon_level		= pInterface->mArms[*pInterface->mSelectedArms].level;
	out->weapon_exp         = pInterface->mArms[*pInterface->mSelectedArms].exp;

	// Configure the rest
	out->input_flags      = *pInterface->mKey;
	out->input_flags_trg  = *pInterface->mKeyTrg;
	out->cond             = in->cond;
	out->life             = in->life;
	out->max_life         = in->max_life;
	out->x                = in->x;
	out->y                = in->y;
	out->xm               = in->xm;
	out->ym               = in->ym;
	out->direct           = in->direct;
	out->ani_no           = in->ani_no;
	out->ani_wait         = in->ani_wait;
	out->equip            = in->equip;
	out->netanim          = in->netanim;
	out->star             = in->star;
	out->shock            = in->shock;
	out->ignore_water     = in->ignore_water;
	out->boost_sw         = in->boost_sw;
	out->boost_cnt        = in->boost_cnt;
	out->tgt_x            = in->tgt_x;
	out->tgt_y            = in->tgt_y;
	out->flag             = in->flag;
	out->up               = in->up;
	out->down             = in->down;
	out->air              = in->air;

	memcpy(&out->physics_normal, &in->physics_normal, sizeof(MYCHAR_PHYSICS));
	memcpy(&out->physics_underwater, &in->physics_underwater, sizeof(MYCHAR_PHYSICS));
}

static void SET_MYCHAR_STATE(PXDEMO_PLAYERSTATE_STRUCT* in, MYCHAR* out, SafeClientInterface* pInterface)
{
	// Set weapon data
	*pInterface->mSelectedArms                            = in->selectedArms;
	pInterface->mArms[*pInterface->mSelectedArms].code    = in->weapon_code;
	pInterface->mArms[*pInterface->mSelectedArms].num     = in->weapon_ammo;
	pInterface->mArms[*pInterface->mSelectedArms].max_num = in->weapon_max_ammo;
	pInterface->mArms[*pInterface->mSelectedArms].level   = in->weapon_level;
	pInterface->mArms[*pInterface->mSelectedArms].exp     = in->weapon_exp;

	// Configure the rest
	*pInterface->mKey     = in->input_flags;
	*pInterface->mKeyTrg  = in->input_flags_trg;
	out->cond             = in->cond;
	out->life             = in->life;
	out->max_life         = in->max_life;
	out->x                = in->x;
	out->y                = in->y;
	out->xm               = in->xm;
	out->ym               = in->ym;
	out->direct           = in->direct;
	out->ani_no           = in->ani_no;
	out->ani_wait         = in->ani_wait;
	out->equip            = in->equip;
	out->star             = in->star;
	out->shock            = in->shock;
	out->netanim          = in->netanim;
	out->ignore_water     = in->ignore_water;
	out->boost_sw         = in->boost_sw;
	out->boost_cnt        = in->boost_cnt;
	out->tgt_x            = in->tgt_x;
	out->tgt_y            = in->tgt_y;
	out->flag             = in->flag;
	out->up               = in->up;
	out->down             = in->down;
	out->air              = in->air;

	memcpy(&out->physics_normal, &in->physics_normal, sizeof(MYCHAR_PHYSICS));
	memcpy(&out->physics_underwater, &in->physics_underwater, sizeof(MYCHAR_PHYSICS));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
static const char* gEventClassNameList[] =
{
	"<INVALID>",
	"EVENT_SETPLAYERSTATE_STRUCT",
	"EVENT_SETSTAGE_STRUCT",
	"EVENT_SETRANDOMSEED_STRUCT",
	"EVENT_SETNPCSTATE_STRUCT",
	"EVENT_DOSCREENFADE_STRUCT",
	"EVENT_DOSCREENFADE_STRUCT",
	"EVENT_SETCAMERA_STRUCT",
	"EVENT_SETTIMER_STRUCT"
};

#define SPLIT "\xC3"			// "|-"
#define CONTI "\xB3"			// "|"
#define LINES "\xC4\xC4\xC4"	// "---"
#define BLANK "   "				// "   "
#define ENDLN "\xC0"			// "|_"
#define PINIT(WHITESPACE, TO_TEST) pFlagWS = WHITESPACE; iFlags = TO_TEST; iFlagCount = 0;
#define POUTF(FORMAT, ...) printf("%s" FORMAT, pFlagWS, __VA_ARGS__);
#define PADDC(FLAG) if (iFlags & FLAG) ++iFlagCount;
#define PDISP(FLAG) \
	if (iFlags & FLAG)\
	{\
		--iFlagCount;\
		if (iFlagCount)\
			printf("%s" SPLIT LINES "%s\r\n", pFlagWS, #FLAG);\
		else\
			printf("%s" ENDLN LINES "%s\r\n", pFlagWS, #FLAG);\
	}

void CSM_GameDemo_Dump(GAME_DEMO_STATE* pState)
{
	const char* pFlagWS = 0;
	int iFlagCount      = 0;
	int iFlags          = 0;

	printf("GameDemo<0x%08p>\r\n", pState);
	printf(SPLIT LINES "Mod:            \"%s\" (v%d.%d.%d.%d)\r\n", pState->mInfo.mod_name, ((unsigned char*)& pState->mInfo.mod_version)[0], ((unsigned char*)& pState->mInfo.mod_version)[1], ((unsigned char*)& pState->mInfo.mod_version)[2], ((unsigned char*)& pState->mInfo.mod_version)[3]);
	printf(SPLIT LINES "CaveNet:        v%s\r\n", CaveNet::GetVersionString(pState->mInfo.cavenet_version));
	printf(SPLIT LINES "Random Seed:    %d\r\n", pState->mInfo.random_seed);
	printf(SPLIT LINES "Frame Count:    %d / %d\r\n", pState->mInfo.play_frame, pState->mInfo.total_sim_frames);
	printf(SPLIT LINES "Game Flags\r\n");

	// Write the game flags
	PINIT(CONTI BLANK, pState->mInfo.game_flags);
	PADDC(GameFlags::GF_EnableActing); PADDC(GameFlags::GF_EnableControl); PADDC(GameFlags::GF_DisableInventory); PADDC(GameFlags::GF_CreditsRunning);
	PDISP(GameFlags::GF_EnableActing); PDISP(GameFlags::GF_EnableControl); PDISP(GameFlags::GF_DisableInventory); PDISP(GameFlags::GF_CreditsRunning);

	printf(SPLIT LINES "Player (%d)\r\n", pState->mPlayers.count);
	for (int i = 0; i < pState->mPlayers.count; i++)
	{
		PXDEMO_PLAYER_STRUCT* pPlayer = &pState->mPlayers.list[i];
		bool bLastEntry               = (i != pState->mPlayers.count - 1);

		if (bLastEntry)
		{
			printf(CONTI BLANK SPLIT LINES "PXDEMO_PLAYER_STRUCT # %d (\"%s\")\r\n", i, pPlayer->name);
			PINIT(CONTI BLANK CONTI BLANK, 0);
		}
		else
		{
			printf(CONTI BLANK ENDLN LINES "PXDEMO_PLAYER_STRUCT # %d (\"%s\")\r\n", i, pPlayer->name);
			PINIT(CONTI BLANK " " BLANK, 0);
		}

		POUTF(SPLIT LINES "Character Name:  %s\r\n", pPlayer->character->GetName());
		POUTF(ENDLN LINES "Character Color: (%d, %d, %d)\r\n", pPlayer->char_color.r, pPlayer->char_color.g, pPlayer->char_color.b);
	}

	printf(ENDLN LINES "Frames (%d)\r\n", pState->mFrames.count);
	for (int i = 0; i < pState->mFrames.count; i++)
	{
		PXDEMO_FRAME_STRUCT* pFrame = &pState->mFrames.list[i];
		bool bLastEntry             = (i != pState->mFrames.count - 1);

		if (bLastEntry)
		{
			printf(" " BLANK SPLIT LINES "PXDEMO_FRAME_STRUCT # %d\r\n", i);
			PINIT(" " BLANK CONTI BLANK, 0);
		}
		else
		{
			printf(" " BLANK ENDLN LINES "PXDEMO_FRAME_STRUCT # %d\r\n", i);
			PINIT(" " BLANK " " BLANK, 0);
		}

		POUTF(SPLIT LINES "Frame #: %d\r\n", pFrame->frame_no);
		POUTF(ENDLN LINES "Events (%d)\r\n", pFrame->event_count);
		for (int j = 0; j < pFrame->event_count; j++)
		{
			PXDEMO_EVENT_STRUCT* pEvent = &pFrame->event_list[j];
			bool bLastEvent             = (j == pFrame->event_count - 1);

			if (bLastEvent)
			{
				POUTF(" " BLANK ENDLN LINES "%s\r\n", gEventClassNameList[pEvent->type < 0 || pEvent->type >= (sizeof(gEventClassNameList) / sizeof(const char*)) ? 0 : pEvent->type]);
			}
			else
			{
				POUTF(" " BLANK SPLIT LINES "%s\r\n", gEventClassNameList[pEvent->type < 0 || pEvent->type >= (sizeof(gEventClassNameList) / sizeof(const char*)) ? 0 : pEvent->type]);
			}
		}
	}
}

#undef SPLIT
#undef CONTI
#undef LINES
#undef BLANK
#undef ENDLN
#undef DFLAG
#undef POUTF
#undef PINIT
#undef PADDC
#undef PDISP

#endif

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSM_GameDemo_Init(GAME_DEMO_STATE* pState)
{
	memset(pState, 0, sizeof(GAME_DEMO_STATE));
}

void CSM_GameDemo_Init_Relink(GAME_DEMO_STATE* pState)
{
	if (pState->mChecksum == GAME_DEMO_CHECKSUM)
	{
		GAME_DEMO_STATE* last_next = pState->mNext;

		// Zero-mem it
		memset(pState, 0, sizeof(GAME_DEMO_STATE));

		// Set the appropriate CRC checksum & the next pointer
		pState->mChecksum = GAME_DEMO_CHECKSUM;
		pState->mNext     = last_next;

		return;
	}

	// Zero-mem it
	memset(pState, 0, sizeof(GAME_DEMO_STATE));

	// Set the appropriate CRC checksum
	pState->mChecksum = GAME_DEMO_CHECKSUM;

	// Link this state
	CSM_GameDemo_Link(pState);
}

void CSM_GameDemo_Free(GAME_DEMO_STATE* pState)
{
	int i;

	if (pState->mInfo.mod_name)
	{
		// Free the mod's name
		free(pState->mInfo.mod_name);
		pState->mInfo.mod_name = NULL;
	}

	if (pState->mPlayers.list)
	{
		// Free player names
		for (i = 0; i < pState->mPlayers.count; i++)
		{
			PXDEMO_PLAYER_STRUCT* pPlayer = &pState->mPlayers.list[i];

			// Free this player's name
			free(pPlayer->name);
		}

		// Free the list
		free(pState->mPlayers.list);

		// Reset to default
		pState->mPlayers.list  = NULL;
		pState->mPlayers.count = 0;
	}
	
	if (pState->mFrames.list)
	{
		// Free player names
		for (i = 0; i < pState->mFrames.count; i++)
		{
			PXDEMO_FRAME_STRUCT* pFrame = &pState->mFrames.list[i];
			
			// Free this frame's eventlist
			free(pFrame->event_list);
		}

		// Free the list
		free(pState->mFrames.list);

		// Reset to default
		pState->mFrames.list  = NULL;
		pState->mFrames.count = 0;
	}

	// Unlink this demo from the active list
	CSM_GameDemo_Unlink(pState);

	// Reset everything else to default
	memset(pState, 0, sizeof(GAME_DEMO_STATE));
}

int CSM_GameDemo_Load(GAME_DEMO_STATE* pState, const char* pFileName)
{
	SharedBitStream stream;
	unsigned char version;
	char buffer[1024];
	char* ptr = NULL;

	// Read the entire file into the bitstream
	{
		FILE* fp = NULL;

		// Make sure we load the file
		if ((fp = fopen(pFileName, "rb")) == NULL)
		{
			printf("ERROR: " __FUNCTION__ "() - Failed to open gamedemo file \"%s\"\r\n", pFileName);

			return 1;
		}

		// Go to the end of the file
		fseek(fp, 0, SEEK_END);

		// Get the length of the file
		unsigned int FullSize = ftell(fp);

		// Go back to the mStart of the file
		fseek(fp, 0, SEEK_SET);

		// Allocate the bitstream
		stream.Clear();
		stream.Resize(FullSize);

		// Read the entire file into the buffer
		fread(stream.mBuffer, FullSize, 1, fp);
		stream.mBufferLen = FullSize;

		stream.SetCurPos(0);

		// Close the file
		fclose(fp);
	}

	// Check it
	if (memcmp(stream.mBuffer, PXDEMO_HEADER_STRING, 6))
	{
		printf("ERROR: " __FUNCTION__ "() - Invalid magic!\r\n");
		stream.Clear();
		return 2;
	}

	// Get the file version
	version = (unsigned char)stream.mBuffer[6];

	// Validate version
	if (version != PXDEMO_VERSION)
		printf("ERROR: " __FUNCTION__ "() - Demo version mismatch (expected: %d, got: %d) -- Attempting to read anyways\r\n", PXDEMO_VERSION, version);

	// Skip past the header & version
	stream.SetCurPos(56);

	// Read basic information
	pState->mFrames.count  = stream.ReadInt(sizeof(unsigned int) << 3);
	pState->mPlayers.count = stream.ReadInt(sizeof(unsigned int) << 3);

	// Read NPC flags
	stream.Read(pState->mInfo.flags, MAX_FLAGS);
	stream.Read(pState->mInfo.skip_flags, MAX_SKIP_FLAGS);

	// Allocate stuff
	pState->mFrames.list  = (!pState->mFrames.count ?		NULL : (PXDEMO_FRAME_STRUCT*)malloc(sizeof(PXDEMO_FRAME_STRUCT) * pState->mFrames.count));
	pState->mPlayers.list = (!pState->mPlayers.count ?		NULL : (PXDEMO_PLAYER_STRUCT*)malloc(sizeof(PXDEMO_PLAYER_STRUCT) * pState->mPlayers.count));
	
	// Zero-out the memory where applicable
	if (pState->mFrames.list)	memset(pState->mFrames.list,  0, sizeof(PXDEMO_FRAME_STRUCT)  * pState->mFrames.count);
	if (pState->mPlayers.list)	memset(pState->mPlayers.list, 0, sizeof(PXDEMO_PLAYER_STRUCT) * pState->mPlayers.count);

	// Read the demo information
	pState->mInfo.cavenet_version  = stream.ReadInt(sizeof(unsigned int) << 3);
	stream.ReadString(buffer); pState->mInfo.mod_name = strdup(buffer);
	pState->mInfo.mod_version      = stream.ReadInt(sizeof(unsigned int) << 3);
	pState->mInfo.total_sim_frames = stream.ReadInt(sizeof(unsigned int) << 3);
	pState->mInfo.game_flags       = stream.ReadInt(sizeof(unsigned int) << 3);

	// Read players
	for (int i = 0; i < pState->mPlayers.count; i++)
	{
		PXDEMO_PLAYER_STRUCT* pPlayer = &pState->mPlayers.list[i];
		pPlayer->last_state_event_idx = -1;
		pPlayer->last_state_frame_idx = -1;

		stream.ReadString(buffer); pPlayer->name = strdup(buffer);
		stream.ReadString(buffer);

		if (version >= 2)
		{
			pPlayer->use_color = stream.ReadFlag();
			if (pPlayer->use_color)
			{
				pPlayer->char_color.r = stream.ReadInt(sizeof(unsigned char) << 3);
				pPlayer->char_color.g = stream.ReadInt(sizeof(unsigned char) << 3);
				pPlayer->char_color.b = stream.ReadInt(sizeof(unsigned char) << 3);
			}
		}
		else
		{
			pPlayer->use_color = false;
		}

		// Find this character
		pPlayer->character_name = strdup(buffer);
		pPlayer->character      = CustomPlayer::subsitute;

		// Attempt to find it
		for (CustomPlayer* walk = CustomPlayer::first; walk; walk = walk->next)
		{
			// Skip mismatches
			if (stricmp(pPlayer->character_name, walk->GetName()))
				continue;

			// Found it
			pPlayer->character = walk;
			break;
		}
		
		// Read this player's state
		stream.Read(pPlayer->arms, sizeof(pPlayer->arms) << 3);
		stream.Read(&pPlayer->state, sizeof(PXDEMO_PLAYERSTATE_STRUCT) << 3);
	}

	// Read frames
	for (int i = 0; i < pState->mFrames.count; i++)
	{
		PXDEMO_FRAME_STRUCT* pFrame = &pState->mFrames.list[i];
		pFrame->frame_no            = stream.ReadInt(sizeof(unsigned int) << 3);
		pFrame->event_count         = stream.ReadInt(sizeof(unsigned int) << 3);
		pFrame->event_list          = (pFrame->event_count ? (PXDEMO_EVENT_STRUCT*)malloc(sizeof(PXDEMO_EVENT_STRUCT) * pFrame->event_count) : NULL);

		if (!pFrame->event_list)
			continue;

		memset(pFrame->event_list, 0, sizeof(PXDEMO_EVENT_STRUCT) * pFrame->event_count);

		// Read events
		for (int j = 0; j < pFrame->event_count; j++)
		{
			PXDEMO_EVENT_STRUCT* pEvent = &pFrame->event_list[j];
			pEvent->type                = (PXDEMO_EVENT_TYPE)stream.ReadInt(sizeof(unsigned short) << 3);

			if (pEvent->type <= 0 || pEvent->type >= PXDEMO_EVENT_TYPE::PXDET_MAX_EVENTS)
			{
				printf("Unknown event type %d\r\n", pEvent->type);

				// Unrecoverable
				stream.Clear();
				CSM_GameDemo_Free(pState);

				return 1;
			}

			gDemoEventList[pEvent->type].ReadOrWrite(false, version, pState, &stream, pEvent);
		}
	}

	stream.Clear();

	// Done
	return 0;
}

int CSM_GameDemo_Save(GAME_DEMO_STATE* pState, const char* pFileName)
{
	SharedBitStream stream;
	char buffer[1024];

	// Write header info
	stream.Write(PXDEMO_HEADER_STRING, strlen(PXDEMO_HEADER_STRING) << 3);
	stream.WriteInt(PXDEMO_VERSION, sizeof(unsigned char) << 3);

	// Write common info
	stream.WriteInt(pState->mFrames.count,  sizeof(unsigned int) << 3);
	stream.WriteInt(pState->mPlayers.count, sizeof(unsigned int) << 3);
	stream.Write(pState->mInfo.flags,       MAX_FLAGS);
	stream.Write(pState->mInfo.skip_flags,  MAX_SKIP_FLAGS);

	// Write game info
	stream.WriteInt(pState->mInfo.cavenet_version,	sizeof(unsigned int) << 3);
	stream.WriteString(pState->mInfo.mod_name);
	stream.WriteInt(pState->mInfo.mod_version,		sizeof(unsigned int) << 3);
	stream.WriteInt(pState->mInfo.total_sim_frames,	sizeof(unsigned int) << 3);
	stream.WriteInt(pState->mInfo.game_flags,		sizeof(unsigned int) << 3);

	// Write players
	for (int i = 0; i < pState->mPlayers.count; i++)
	{
		PXDEMO_PLAYER_STRUCT* pPlayer = &pState->mPlayers.list[i];

		stream.WriteString(pPlayer->name);
		stream.WriteString((pPlayer->character ? pPlayer->character_name : "Quote"));

		if (stream.WriteFlag(pPlayer->use_color))
		{
			stream.WriteInt(pPlayer->char_color.r, sizeof(unsigned char) << 3);
			stream.WriteInt(pPlayer->char_color.g, sizeof(unsigned char) << 3);
			stream.WriteInt(pPlayer->char_color.b, sizeof(unsigned char) << 3);
		}

		stream.Write(pPlayer->arms, sizeof(pPlayer->arms) << 3);
		stream.Write(&pPlayer->state, sizeof(PXDEMO_PLAYERSTATE_STRUCT) << 3);
	}

	// Read frames
	for (int i = 0; i < pState->mFrames.count; i++)
	{
		PXDEMO_FRAME_STRUCT* pFrame = &pState->mFrames.list[i];
		stream.WriteInt(pFrame->frame_no, sizeof(unsigned int) << 3);
		stream.WriteInt(pFrame->event_count, sizeof(unsigned int) << 3);

		// Read events
		for (int j = 0; j < pFrame->event_count; j++)
		{
			PXDEMO_EVENT_STRUCT* pEvent = &pFrame->event_list[j];

			// Write the event type
			stream.WriteInt(pEvent->type, sizeof(unsigned short) << 3);

			// Write the event data
			gDemoEventList[pEvent->type].ReadOrWrite(true, PXDEMO_VERSION, pState, &stream, pEvent);
		}
	}

	// Save the entire bitstream into a file
	{
		FILE* fp;

		// Attempt to open the file for writing
		if ((fp = fopen(pFileName, "wb")) == NULL)
		{
			printf("ERROR: " __FUNCTION__ "() - Failed to open file \"%s\"\r\n", pFileName);
			return false;
		}

		fwrite(stream.mBuffer, stream.mBufferLen, 1, fp);

		// We're finished with the file, so close it.
		fclose(fp);
	}

	stream.Clear();
	return 0;
}

bool CSM_GameDemo_CombineDemos(GAME_DEMO_STATE* pDst, GAME_DEMO_STATE* pSource, unsigned int iFrameOffset)
{
	int iStartPlayerCount = pDst->mPlayers.count;
	iFrameOffset          = CLAMP(iFrameOffset, 0, pDst->mInfo.total_sim_frames);

	// Append all players
	for (int i = 0; i < pSource->mPlayers.count; i++)
	{
		PXDEMO_PLAYER_STRUCT* pFrom = &pSource->mPlayers.list[i];
		PXDEMO_PLAYER_STRUCT* pTo   = CSM_GameDemo_Players_Add(pDst, pFrom->name, NULL);

		if (!pTo)
			continue;

		// Configure the players
		memcpy(&pTo->state, &pFrom->state, sizeof(PXDEMO_PLAYERSTATE_STRUCT));
		memcpy(pTo->arms, pFrom->arms, sizeof(pTo->arms));
		pTo->character            = pFrom->character;
		pTo->last_state_event_idx = -1;
		pTo->last_state_frame_idx = -1;
	}

	// Find all frames w/ frame indicies greater than iFrameOffset, and offset them by the source's total sim frame count.
	for (int i = 0; i < pDst->mFrames.count; i++)
	{
		PXDEMO_FRAME_STRUCT* pFrame = &pDst->mFrames.list[i];

		if (pFrame->frame_no >= iFrameOffset)
			pFrame->frame_no += pSource->mInfo.total_sim_frames;
	}

	// Increase the total frame count
	pDst->mInfo.total_sim_frames += pSource->mInfo.total_sim_frames;

	// Add the events to the destination
	for (int i = 0; i < pSource->mFrames.count; i++)
	{
		PXDEMO_FRAME_STRUCT* pFrame = &pSource->mFrames.list[i];

		// Iterate through this frame's events
		for (int j = 0; j < pFrame->event_count; j++)
		{
			PXDEMO_EVENT_STRUCT* pFrom = &pFrame->event_list[j];
			PXDEMO_EVENT_STRUCT* pTo   = CSM_GameDemo_Events_Add(pDst, pFrom->type, pFrame->frame_no);

			// Combine demos
			gDemoEventList[pFrom->type].CombineDemos(pFrame->frame_no, pDst, pSource, pTo, pFrom);
		}
	}

	// Done!
	return true;
}

bool CSM_GameDemo_Optimize(GAME_DEMO_STATE* pState, bool bSaveAndLoad)
{
	PROFILE TmpProfile;
	NPCHAR* TmpNpcTable;
	NPCHAR* TmpBossTable;

	// Save profile
	if (bSaveAndLoad)
	{
		SaveProfileIntoMem(&TmpProfile);
		TmpNpcTable  = new NPCHAR[NPC_MAX];
		TmpBossTable = new NPCHAR[BOSS_MAX];
		memcpy(TmpNpcTable, gNPC, sizeof(gNPC));
		memcpy(TmpBossTable, gBoss, sizeof(gBoss));
	}

	// Start
	CSM_GameDemo_LoadGamestate(pState);

	// Cache
	unsigned char* pOnScreenTable = 0;
	unsigned char* pPlayerSeenTbl = 0;
	int iStageCount               = 0;
	int iStageCurr                = 0;
	int iLastStage                = gStageNo;

	// Figure out how much to allocate
	for (int i = 0; i < pState->mFrames.count; i++)
	{
		PXDEMO_FRAME_STRUCT* pFrame = &pState->mFrames.list[i];

		for (int j = 0; j < pFrame->event_count; j++)
		{
			PXDEMO_EVENT_STRUCT* pEvent = &pFrame->event_list[j];

			// Skip non-NPC events
			if (pEvent->type == PXDEMO_EVENT_TYPE::PXDET_SET_STAGE)
				++iStageCount;
		}
	}

	// Cache variables
	pOnScreenTable = new unsigned char[((NPC_MAX + BOSS_MAX) * iStageCount) >> 3];
	pPlayerSeenTbl = (pState->mPlayers.count ? new unsigned char[(pState->mPlayers.count + 7) >> 3] : NULL);

	// Zero-mem it
	if (pOnScreenTable)
		memset(pOnScreenTable, 0, ((NPC_MAX + BOSS_MAX) * iStageCount) >> 3);

	if (pPlayerSeenTbl)
	{
		memset(pPlayerSeenTbl, 0, (pState->mPlayers.count + 7) >> 3);
		
		// First player is always visible
		pPlayerSeenTbl[0] |= (1 << 0);
	}
	
	// Go event by event and search for NPCs that we've seen
	for (int iFrameIdx = 0; iFrameIdx < pState->mFrames.count; iFrameIdx++)
	{
		PXDEMO_FRAME_STRUCT* pFrame = &pState->mFrames.list[iFrameIdx];

		if (pFrame->frame_no >= pState->mInfo.total_sim_frames)
			pState->mInfo.total_sim_frames = pFrame->frame_no + 1;

		// Catch up to this frame
		while (pState->mInfo.play_frame < pFrame->frame_no)
			CSM_GameDemo_Act(pState, false, true, false, false);

		// Reset the onscreen lookup table if we've switched stages
		if (iLastStage != gStageNo)
		{
			// Zero-mem
			iLastStage = gStageNo;
			++iStageCurr;
		}

		// Check NPCs
		for (int i = 0; i < NPC_MAX; i++)
		{
			NPCHAR* pNpc = &gNPC[i];

			// Mark this NPC as 'seen' if we see it this frame
			if (!(pOnScreenTable[(((i * iStageCount) + iStageCurr) / 8)] & (1 << (((i * iStageCount) + iStageCurr) % 8))) && IsNpcOnScreen(pNpc))
				pOnScreenTable[(((i * iStageCount) + iStageCurr) / 8)] |= (1 << (((i * iStageCount) + iStageCurr) % 8));
		}

		// Check bosses
		for (int i = 0; i < BOSS_MAX; i++)
		{
			NPCHAR* pNpc = &gBoss[i];

			// Mark this NPC as 'seen' if we see it this frame
			if (!(pOnScreenTable[((((NPC_MAX + i) * iStageCount) + iStageCurr) / 8)] & (1 << ((((NPC_MAX + i) * iStageCount) + iStageCurr) / 8))) && IsNpcOnScreen(pNpc))
				pOnScreenTable[((((NPC_MAX + i) * iStageCount) + iStageCurr) / 8)] |= (1 << ((((NPC_MAX + i) * iStageCount) + iStageCurr) / 8));
		}

		// Check NPCs
		for (int i = 0; i < pState->mPlayers.count; i++)
		{
			PXDEMO_PLAYER_STRUCT* pPlayer = &pState->mPlayers.list[i];

			if (pPlayerSeenTbl[(i / 8)] & (1 << (i % 8)))
				continue;

			gNOT_USED.x         = pPlayer->state.x;
			gNOT_USED.y         = pPlayer->state.y;
			gNOT_USED.ani_no    = pPlayer->state.ani_no;
			gNOT_USED.character = pPlayer->character;

			GUI_RECT pDrawRect;
			
			GetMyCharDrawRectangle(&gNOT_USED, &pDrawRect);

			// Mark this player as 'seen' if they're onscreen for any amount of time.
			if (pDrawRect.right() >= 0 && pDrawRect.left() <= PixelToScreenCoord(WINDOW_WIDTH) && pDrawRect.bottom() >= 0 && pDrawRect.top() <= PixelToScreenCoord(WINDOW_HEIGHT))
				pPlayerSeenTbl[(i / 8)] |= (1 << (i % 8));
		}
	}

	// Reset current stage index
	iStageCurr = -1;

	// Remove unused NPCs & bosses
	for (int i = 0; i < pState->mFrames.count; i++)
	{
		PXDEMO_FRAME_STRUCT* pFrame = &pState->mFrames.list[i];

		for (int j = 0; j < pFrame->event_count; j++)
		{
			PXDEMO_EVENT_STRUCT* pEvent = &pFrame->event_list[j];

			// If this is a stage change event, then increment the current stage index
			if (pEvent->type == PXDEMO_EVENT_TYPE::PXDET_SET_STAGE)
			{
				++iStageCurr;
			}
			else if (pEvent->type == PXDEMO_EVENT_TYPE::PXDET_SET_NPC_STATE)
			{
				// Skip NPCs we've actually seen
				if ((pOnScreenTable[((pEvent->data.npc_state.npc_idx * iStageCount) + iStageCurr) / 8] & (1 << (((pEvent->data.npc_state.npc_idx * iStageCount) + iStageCurr) % 8))) != 0 && !(gCaveMod.mNpcs.list[pEvent->data.npc_state.state.code_char].bits & NPC_flags::npc_dontUpdate))
					continue;

				// We never saw this NPC; remove it.
				LIST_ERASE(pState->mFrames.list[i].event_list, pState->mFrames.list[i].event_count, PXDEMO_EVENT_STRUCT, j);
				--pState->mFrames.list[i].event_count;
				--j;
			}
			else if (pEvent->type == PXDEMO_EVENT_TYPE::PXDET_SET_PLAYER_STATE)
			{
				// Skip players we've actually seen
				if ((pPlayerSeenTbl[pEvent->data.player_state.player_idx / 8] & (1 << (pEvent->data.player_state.player_idx % 8))) != 0)
					continue;

				// We never saw this player; remove it.
				LIST_ERASE(pState->mFrames.list[i].event_list, pState->mFrames.list[i].event_count, PXDEMO_EVENT_STRUCT, j);
				--pState->mFrames.list[i].event_count;
				--j;
			}
		}

		// If this frame is empty, then remove it.
		if (!pFrame->event_count)
		{
			if (pFrame->event_list)
				free(pFrame->event_list);

			LIST_ERASE(pState->mFrames.list, pState->mFrames.count, PXDEMO_FRAME_STRUCT, i);
			--pState->mFrames.count;
			--i;
		}
	}

	for (int i = 0; i < pState->mPlayers.count; i++)
	{
		PXDEMO_PLAYER_STRUCT* pPlayer = &pState->mPlayers.list[i];

		// Skip players we've seen before
		if ((pPlayerSeenTbl[i / 8] & (1 << (i % 8))) != 0)
			continue;

		// Free their name
		if (pPlayer->name)
			free(pPlayer->name);

		// Erase them from existance
		LIST_ERASE(pState->mPlayers.list, pState->mPlayers.count, PXDEMO_PLAYER_STRUCT, i);
		--pState->mPlayers.count;
		--i;
	}

	delete[] pOnScreenTable;
	delete[] pPlayerSeenTbl;

	// Load then delete the temporary save
	if (bSaveAndLoad)
	{
		LoadProfileFromMem(&TmpProfile);
		memcpy(gNPC, TmpNpcTable, sizeof(gNPC));
		memcpy(gBoss, TmpBossTable, sizeof(gBoss));
		delete[] TmpBossTable;
		delete[] TmpNpcTable;
	}

	// Done
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PXDEMO_PLAYER_STRUCT* CSM_GameDemo_Players_Add(GAME_DEMO_STATE* pState, const char* pName, SafeClientInterface* pInterface)
{
	PXDEMO_PLAYER_STRUCT* pReturn = 0;

	// If it already exists, then just return the existing object.
	if ((pReturn = CSM_GameDemo_Players_Find(pState, pName)) != NULL)
		return pReturn;

	// Resize array to fit a new entry
	if (!pState->mPlayers.list)
	{
		pState->mPlayers.count = 1;
		pState->mPlayers.list  = (PXDEMO_PLAYER_STRUCT*)malloc(sizeof(PXDEMO_PLAYER_STRUCT));
	}
	else
	{
		pState->mPlayers.count += 1;
		pState->mPlayers.list   = (PXDEMO_PLAYER_STRUCT*)realloc((void*)pState->mPlayers.list, sizeof(PXDEMO_PLAYER_STRUCT) * pState->mPlayers.count);
	}

	// Setup the new entry
	PXDEMO_PLAYER_STRUCT* pPlayer = &pState->mPlayers.list[pState->mPlayers.count - 1];

	// Configure it
	pPlayer->name                 = strdup(pName);
	pPlayer->character_name       = (pInterface && pInterface->mMC ? strdup(pInterface->mMC->character->GetName()) : strdup(CustomPlayer::subsitute ? CustomPlayer::subsitute->GetName() : "Quote"));
	pPlayer->character            = (pInterface && pInterface->mMC ? pInterface->mMC->character : NULL);
	pPlayer->last_state_event_idx = -1;
	pPlayer->last_state_frame_idx = -1;
	pPlayer->char_color.r         = pInterface->mMC->col_r;
	pPlayer->char_color.g         = pInterface->mMC->col_g;
	pPlayer->char_color.b         = pInterface->mMC->col_b;
	pPlayer->use_color            = pInterface->mMC->use_char_color;
	memset(&pPlayer->state, 0, sizeof(PXDEMO_PLAYERSTATE_STRUCT));

	if (pInterface)
		memcpy(&pPlayer->arms, pInterface->mArms, sizeof(pPlayer->arms));
	else
		memset(&pPlayer->arms, 0, sizeof(pPlayer->arms));

	// Configure the state
	if (pInterface && pInterface->mMC)
		SET_PLAYER_STATE(pInterface->mMC, &pPlayer->state, pInterface);

	return pPlayer;
}

bool CSM_GameDemo_Players_Remove(GAME_DEMO_STATE* pState, const char* pName)
{
	PXDEMO_PLAYER_STRUCT* pPlayer = 0;
	int iFoundIndex               = 0;

	// If it already exists, then just return the existing object.
	if ((pPlayer = CSM_GameDemo_Players_Find(pState, pName, &iFoundIndex)) == NULL)
		return false;

	// Free the player's name
	free(pPlayer->character_name);
	free(pPlayer->name);

	// Erase its presence from the list
	LIST_ERASE(pState->mPlayers.list, pState->mPlayers.count, PXDEMO_PLAYER_STRUCT, iFoundIndex);

	// Decrement the number of players
	--pState->mPlayers.count;

	// Deallocate the unused memory
	if (!pState->mPlayers.count)
	{
		// Deallocate the entire list
		free(pState->mPlayers.list);
		pState->mPlayers.list = NULL;
	}
	else
	{
		// Resize it down
		pState->mPlayers.list = (PXDEMO_PLAYER_STRUCT*)realloc((void*)pState->mPlayers.list, sizeof(PXDEMO_PLAYER_STRUCT) * pState->mPlayers.count);
	}

	return true;
}

PXDEMO_PLAYER_STRUCT* CSM_GameDemo_Players_Find(GAME_DEMO_STATE* pState, const char* pName, int* pIndex)
{
	// We have no list, so no players can be found.
	if (!pState->mPlayers.list)
		return NULL;

	// Search for the user
	for (int i = 0; i < pState->mPlayers.count; i++)
	{
		PXDEMO_PLAYER_STRUCT* pEntry = &pState->mPlayers.list[i];

		// If the names match, then return the entry.
		if (!strcmpi(pEntry->name, pName))
		{
			if (pIndex)
				*pIndex = i;

			return pEntry;
		}
	}

	// Didn't find it; return null.
	return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PXDEMO_EVENT_STRUCT* CSM_GameDemo_Events_Add(GAME_DEMO_STATE* pState, PXDEMO_EVENT_TYPE iType, int iFrameNo)
{
	PXDEMO_EVENT_STRUCT* pReturn = NULL;
	PXDEMO_FRAME_STRUCT* pFrame  = NULL;

	// Get a frame to add the event to.
	if ((pFrame = CSM_GameDemo_Frames_Add(pState, iFrameNo)) == NULL)
		return NULL;

	// Resize array to fit a new entry
	if (!pFrame->event_list)
	{
		pFrame->event_count = 1;
		pFrame->event_list  = (PXDEMO_EVENT_STRUCT*)malloc(sizeof(PXDEMO_EVENT_STRUCT));
	}
	else
	{
		pFrame->event_count += 1;
		pFrame->event_list   = (PXDEMO_EVENT_STRUCT*)realloc((void*)pFrame->event_list, sizeof(PXDEMO_EVENT_STRUCT) * pFrame->event_count);
	}

	// Setup the new entry
	PXDEMO_EVENT_STRUCT* pEvent = &pFrame->event_list[pFrame->event_count - 1];

	// Zero-mem it
	memset(pEvent, 0, sizeof(PXDEMO_EVENT_STRUCT));

	// Set the event type
	pEvent->type = iType;

	return pEvent;
}

bool CSM_GameDemo_Events_Remove(GAME_DEMO_STATE* pState, PXDEMO_EVENT_TYPE iType, int iFrameNo, int iStartIndex)
{
	PXDEMO_EVENT_STRUCT* pEvent = NULL;
	PXDEMO_FRAME_STRUCT* pFrame = NULL;
	int iFrameIndex             = 0;
	int iEventIndex             = 0;

	// If it already exists, then just return the existing object.
	if ((pEvent = CSM_GameDemo_Events_Find(pState, iType, iFrameNo, iStartIndex, &pFrame, &iFrameIndex, &iEventIndex)) == NULL)
		return false;

	// Erase its presence from the list
	LIST_ERASE(pFrame->event_list, pFrame->event_count, PXDEMO_EVENT_STRUCT, iEventIndex);

	// Decrement the number of players
	--pFrame->event_count;

	// Deallocate the unused memory
	if (!pFrame->event_count)
	{
		// Deallocate the entire list
		free(pFrame->event_list);
		pFrame->event_list = NULL;
	}
	else
	{
		// Resize it down
		pFrame->event_list = (PXDEMO_EVENT_STRUCT*)realloc((void*)pFrame->event_list, sizeof(PXDEMO_EVENT_STRUCT) * pFrame->event_count);
	}

	return true;
}

PXDEMO_EVENT_STRUCT* CSM_GameDemo_Events_Find(GAME_DEMO_STATE* pState, PXDEMO_EVENT_TYPE iType, int iFrameNo, int iStartIndex, PXDEMO_FRAME_STRUCT** pFramePtr, int* pFrameIndex, int* pEventIndex)
{
	PXDEMO_FRAME_STRUCT* pFrame = NULL;
	int iIndex                  = 0;

	// If it already exists, then just return the existing object.
	if ((pFrame = CSM_GameDemo_Frames_Find(pState, iFrameNo, &iIndex)) == NULL)
		return NULL;

	if (pFrameIndex)
		*pFrameIndex = iIndex;

	if (pFramePtr)
		*pFramePtr = pFrame;

	// Validate the range
	if (iStartIndex < 0 && iStartIndex >= pFrame->event_count)
		return NULL;

	// Search for the event
	for (int i = iStartIndex; i < pFrame->event_count; i++)
	{
		PXDEMO_EVENT_STRUCT* pEvent = &pFrame->event_list[i];

		// Found it
		if (pEvent->type == iType)
		{
			if (pEventIndex)
				*pEventIndex = i;

			return pEvent;
		}
	}

	// Couldn't find it
	return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PXDEMO_FRAME_STRUCT* CSM_GameDemo_Frames_Add(GAME_DEMO_STATE* pState, int iFrameNo)
{
	PXDEMO_FRAME_STRUCT* pReturn = NULL;

	// If it already exists, then just return the existing object.
	if ((pReturn = CSM_GameDemo_Frames_Find(pState, iFrameNo)) != NULL)
		return pReturn;

	if (iFrameNo > pState->mInfo.total_sim_frames)
		pState->mInfo.total_sim_frames = iFrameNo;

	// Resize array to fit a new entry
	if (!pState->mFrames.count)
	{
		pState->mFrames.count = 1;
		pState->mFrames.list  = (PXDEMO_FRAME_STRUCT*)malloc(sizeof(PXDEMO_FRAME_STRUCT));
	}
	else
	{
		pState->mFrames.count += 1;
		pState->mFrames.list   = (PXDEMO_FRAME_STRUCT*)realloc((void*)pState->mFrames.list, sizeof(PXDEMO_FRAME_STRUCT) * pState->mFrames.count);
	}

	// Find where to put it
	if (pState->mFrames.count == 1 || iFrameNo >= pState->mFrames.list[pState->mFrames.count - 2].frame_no)
	{
		// Setup the new entry
		pReturn = &pState->mFrames.list[pState->mFrames.count - 1];

		// Zero-mem it
		memset(pReturn, 0, sizeof(PXDEMO_FRAME_STRUCT));

		// Set the event type
		pReturn->frame_no = iFrameNo;

		return pReturn;
	}

	if (iFrameNo <= pState->mFrames.list[0].frame_no)
	{
		// Setup the new entry
		pReturn = &pState->mFrames.list[0];

		// Insert it
		LIST_INSERT(pState->mFrames.list, pState->mFrames.count, PXDEMO_FRAME_STRUCT, 0);

		// Zero-mem it
		memset(pReturn, 0, sizeof(PXDEMO_FRAME_STRUCT));

		// Set the event type
		pReturn->frame_no = iFrameNo;

		// Done
		return pReturn;
	}

	// Determine where to insert this element at
	int i;
	for (i = pState->mFrames.count - 1; i >= 0 && pState->mFrames.list[i - 1].frame_no > iFrameNo; i--);

	// Insert at array
	pReturn = &pState->mFrames.list[i];

	// Insert it
	LIST_INSERT(pState->mFrames.list, pState->mFrames.count, PXDEMO_FRAME_STRUCT, i);

	// Zero-mem it
	memset(pReturn, 0, sizeof(PXDEMO_FRAME_STRUCT));

	// Set the event type
	pReturn->frame_no = iFrameNo;

	return pReturn;
}

bool CSM_GameDemo_Frames_Remove(GAME_DEMO_STATE* pState, int iFrameNo)
{
	PXDEMO_FRAME_STRUCT* pFrame = NULL;
	int iIndex                  = 0;

	// If it already exists, then just return the existing object.
	if ((pFrame = CSM_GameDemo_Frames_Find(pState, iFrameNo, &iIndex)) == NULL)
		return false;

	// Free the event list
	free(pFrame->event_list);

	// Erase its presence from the list
	LIST_ERASE(pState->mFrames.list, pState->mFrames.count, PXDEMO_FRAME_STRUCT, iIndex);

	// Decrement the number of players
	--pState->mFrames.count;

	// Deallocate the unused memory
	if (!pState->mFrames.count)
	{
		// Deallocate the entire list
		free(pState->mFrames.list);
		pState->mFrames.list = NULL;
	}
	else
	{
		// Resize it down
		pState->mFrames.list = (PXDEMO_FRAME_STRUCT*)realloc((void*)pState->mFrames.list, sizeof(PXDEMO_FRAME_STRUCT) * pState->mFrames.count);
	}

	return true;
}

PXDEMO_FRAME_STRUCT* CSM_GameDemo_Frames_Find(GAME_DEMO_STATE* pState, int iFrameNo, int* pIndex)
{
	if (!pState->mFrames.count)
		return NULL;

	if (pState->mFrames.list[0].frame_no == iFrameNo)
	{
		if (pIndex)
			*pIndex = 0;

		return &pState->mFrames.list[0];
	}

	if (pState->mFrames.list[pState->mFrames.count - 1].frame_no == iFrameNo)
	{
		if (pIndex)
			*pIndex = pState->mFrames.count - 1;

		return &pState->mFrames.list[pState->mFrames.count - 1];
	}

	int L = 0;
	int R = pState->mFrames.count - 1;
	int m = 0;
	while (L < R)
	{
		m = int(floorf(float(L + R) / 2.f));
		if (pState->mFrames.list[m].frame_no < iFrameNo)
			L = m + 1;
		else if (pState->mFrames.list[m].frame_no > iFrameNo)
			R = m - 1;
		else
		{
			if (pIndex)
				*pIndex = m;

			return &pState->mFrames.list[m];
		}
	}

	// Try the alternative method
	L = 0;
	R = pState->mFrames.count - 1;
	while (L != R)
	{
		m = int(ceil(float(L + R) / 2.f));
		if (pState->mFrames.list[m].frame_no > iFrameNo)
			R = m - 1;
		else
			L = m;
	}

	if (pState->mFrames.list[L].frame_no == iFrameNo)
	{
		if (pIndex)
			*pIndex = L;

		return &pState->mFrames.list[L];
	}

	return NULL;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSM_GameDemo_LoadGamestate(GAME_DEMO_STATE* pState)
{
	rep_srand(pState->mInfo.random_seed);

	memcpy(gFlagNPC, pState->mInfo.flags, sizeof(gFlagNPC));
	memcpy(gSkipFlag, pState->mInfo.skip_flags, sizeof(gSkipFlag));

	pState->mInfo.play_frame = 0;
	g_GameFlags              = pState->mInfo.game_flags;
	CSM_GameDemo_Act(pState, false, true, false, false);

	if (pState->mPlayers.count)
	{
		// Load the local player's state into the default containers
		memcpy(gArmsData, pState->mPlayers.list[0].arms, sizeof(gArmsData));
		gSelectedArms = pState->mPlayers.list[0].state.selectedArms;
	}
}

void CSM_GameDemo_Act(GAME_DEMO_STATE* pState, bool bFreezeFrame, bool bDontDraw, bool bDrawHUD, bool bDrawFade)
{
	// Increment the frame #
	int iFrameNo                = (bFreezeFrame ? pState->mInfo.play_frame : pState->mInfo.play_frame++);
	PXDEMO_FRAME_STRUCT* pFrame = NULL;

	if (!bFreezeFrame && pState->mInfo.play_frame >= pState->mInfo.total_sim_frames)
	{
		bFreezeFrame = true;
		--pState->mInfo.play_frame;
	}

	// Process this frame
	if ((pFrame = CSM_GameDemo_Frames_Find(pState, iFrameNo)) != NULL)
	{
		// Process the events
		for (int i = 0; i < pFrame->event_count; i++)
		{
			PXDEMO_EVENT_STRUCT* pEvent = &pFrame->event_list[i];

			// Run the event
			gDemoEventList[pEvent->type].RunEvent(pState, pEvent);
		}
	}

	// Set selected arms
	int oldGKey      = gKey;
	int oldGKeyTrg   = gKeyTrg;
	int oldGameFlags = g_GameFlags;
	g_GameFlags      = pState->mInfo.game_flags;
	gSelectedArms    = 0;
	int Frame_X      = gFrame.x;
	int Frame_Y      = gFrame.y;

	if (!bFreezeFrame)
	{
		ActStar();
		ActNpChar();
		ActBossChar();
		ActValueView();
		CSM_Map_ActAnimatedTiles();
		ActBack();

		if (bDrawFade)
			ProcFade();
	}

	if (!bDontDraw)
	{
		CortBox(&grcFull, GetCortBoxColor(RGB(0, 0, 0x20)));

		if (Hooks::gOnDrawMapBack)
			Hooks::gOnDrawMapBack();

		PutBack(Frame_X, Frame_Y);
		PutStage_Back(Frame_X, Frame_Y);
		PutBossChar(Frame_X, Frame_Y);
		PutNpChar(Frame_X, Frame_Y);
		PutBullet(Frame_X, Frame_Y);
	}

	SafeClientInterface SCI;
	SafeClientInterface SCI_LOCAL(NULL, SCI_SetType::SCI_ST_EXACT);

	// Act for all players
	for (int i = pState->mPlayers.count - 1; i >= 0; i--)
	{
		PXDEMO_PLAYER_STRUCT* pPlayer = &pState->mPlayers.list[i];

		SCI.mArms         = pPlayer->arms;
		SCI.mSelectedArms = &pPlayer->state.selectedArms;
		SCI.mKey          = &pPlayer->state.input_flags;
		SCI.mKeyTrg       = &pPlayer->state.input_flags_trg;

		// Set the player state for this one
		SET_MYCHAR_STATE(&pPlayer->state, &gMC, &SCI_LOCAL);
		gMC.character = pPlayer->character;

		if (pPlayer->use_color)
		{
			gMC.use_char_color = true;
			gMC.col_r          = pPlayer->char_color.r;
			gMC.col_g          = pPlayer->char_color.g;
			gMC.col_b          = pPlayer->char_color.b;
		}
		else
			gMC.use_char_color = false;

		gSelectedArms                    = CLAMP(pPlayer->state.selectedArms, 0, ARMS_MAX - 1);
		gArmsData[gSelectedArms].code    = pPlayer->state.weapon_code;
		gArmsData[gSelectedArms].num     = pPlayer->state.weapon_ammo;
		gArmsData[gSelectedArms].max_num = pPlayer->state.weapon_max_ammo;
		gArmsData[gSelectedArms].level   = pPlayer->state.weapon_level;
		gArmsData[gSelectedArms].exp     = pPlayer->state.weapon_exp;

		// Animate the player
		if (!bFreezeFrame)
		{
			ActMyChar(TRUE);

			ResetMyCharFlag();
			HitMyCharMap();
			HitMyCharNpChar();
			if (CaveNet::Server::Config::FriendlyFire.ToBool(TRUE))
				HitMyCharBullet();

			HitMyCharBoss();

			if (i == 0)
			{
				HitNpCharMap();
				HitBossMap();
				HitBulletMap();
				HitNpCharBullet();
				HitBossBullet();
			}

			if (g_GameFlags & 2 && !CaveNet::Server::IsHosting())
				ShootBullet();

			if (i == 0)
			{
				ActBullet();
				ActCaret();
				MoveFrame3();
			}

			ActFlash(Frame_X, Frame_Y);
			AnimationMyChar(TRUE);

			if (gKeyTrg & gKeyArms)
				RotationArms();
			else if (gKeyTrg & gKeyArmsRev)
				RotationArmsRev();
		}

		if (!bDontDraw)
			PutMyChar(Frame_X, Frame_Y);

		// Store the new state
		SET_PLAYER_STATE(&gMC, &pPlayer->state, &SCI);
	}

	if (!bDontDraw)
	{
		PutStar(Frame_X, Frame_Y);
		PutMapDataVector(Frame_X, Frame_Y);

		if (Hooks::gOnDrawMapFront)
			Hooks::gOnDrawMapFront();

		PutStage_Front(Frame_X, Frame_Y);
		PutFront(Frame_X, Frame_Y);

		if (Hooks::gOnPreDrawHUD)
			Hooks::gOnPreDrawHUD();

		PutFlash();
		PutCaret(Frame_X, Frame_Y);

		if (bDrawHUD)
		{
			for (int i = 0; i < pState->mPlayers.count; i++)
			{
				PXDEMO_PLAYER_STRUCT* pPlayer = &pState->mPlayers.list[i];

				int iPlayerX = SubpixelToScreenCoord(pPlayer->state.x) - PixelToScreenCoord(12) - SubpixelToScreenCoord(Frame_X);
				int iPlayerY = SubpixelToScreenCoord(pPlayer->state.y) - PixelToScreenCoord(12) - SubpixelToScreenCoord(Frame_Y);

				PutText3(iPlayerX + (((gMC.rect.right - gMC.rect.left) * magnification) / 2), iPlayerY - (TEXT_HEIGHT_2S(BaseModeInstance::GetDefaultFont(), pPlayer->name)), pPlayer->name, RGB(255, 255, 255));
			}
		}

		PutValueView(Frame_X, Frame_Y);

		PutBossLife();

		if (bDrawFade)
			PutFade();

		if (bDrawHUD)
		{
			PutMapName(FALSE);
			PutTimeCounter(16, 8, !bFreezeFrame);
		}
		else if (!bFreezeFrame)
			ProcTimeCounter();

		if (bDrawHUD && ((gMC.npc_control && gMC.life) || (g_GameFlags & 2)))
		{
			PutMyLife(TRUE);
			PutArmsEnergy(TRUE);
			PutMyAir((WINDOW_WIDTH - 80) / 2, (WINDOW_HEIGHT - 32) / 2);
			PutActiveArmsList();
		}
	}
	else if (!bFreezeFrame)
		ProcTimeCounter();

	// Restore previous state
	gKey        = oldGKey;
	gKeyTrg     = oldGKeyTrg;
	g_GameFlags = oldGameFlags;
	if (gTS.current_event)
		StopTextScript();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSM_GameDemo_StartRecord()
{
	// Already a recording going on!
	if (gRecordingDemo)
		return;

	// Create a new recording demo
	gRecordingDemo = new GAME_DEMO_STATE;

	// Init!
	CSM_GameDemo_Init(gRecordingDemo);

	// Get navigation func pointers
	CaveNet::DataStructures::NetClient*(*GetClient)(int iIndex) = (CaveNet::Server::IsHosting() ? CaveNet::Server::ServerClientGroup::GetClient : (CaveNet::Client::IsConnected() ? CaveNet::Client::ClientClientGroup::GetClient : NULL));
	int(*GetClientCount)()                                      = (CaveNet::Server::IsHosting() ? CaveNet::Server::ServerClientGroup::GetClientCount : (CaveNet::Client::IsConnected() ? CaveNet::Client::ClientClientGroup::GetClientCount : NULL));

	// Make sure the game information is saved
	gRecordingDemo->mInfo.cavenet_version  = CAVENET_VERSION;
	gRecordingDemo->mInfo.mod_name         = strdup(gCaveMod.GetInternalName());
	gRecordingDemo->mInfo.mod_version      = *((int*)gCaveMod.mHeader.version.data);
	gRecordingDemo->mInfo.total_sim_frames = 0;
	gRecordingDemo->mInfo.game_flags       = g_GameFlags;
	gRecordingDemo->mInfo.random_seed      = rep_grand();

	memcpy(gRecordingDemo->mInfo.flags, gFlagNPC, sizeof(gFlagNPC));
	memcpy(gRecordingDemo->mInfo.skip_flags, gSkipFlag, sizeof(gSkipFlag));

	// Record the current stage
	SafeClientInterface pInterface;

	pInterface.Set(NULL);

	// Add ourselves
	CSM_GameDemo_Players_Add(gRecordingDemo, "", &pInterface);
	CSM_GameDemo_Record_PlayerState(gRecordingDemo, "", &pInterface);

	// Populate with clients
	if (GetClient && GetClientCount)
	{
		// Populate with REAL clients
		for (int i = 0; i < GetClientCount(); i++)
		{
			CaveNet::DataStructures::NetClient* pClient = GetClient(i);

			pInterface.Set(pClient);

			// Don't record the local player!
			if (pInterface.mIsOurUser || !pClient->GetPlayer())
				continue;

			// Add the client
			pClient->mCache.recorder_player = CSM_GameDemo_Players_Add(gRecordingDemo, pClient->mCache.net_name, &pInterface);
			CSM_GameDemo_Record_PlayerState(gRecordingDemo, pClient->mCache.net_name, &pInterface);
		}
	}

	CSM_GameDemo_Record_Stage(gRecordingDemo);
	CSM_GameDemo_Record_Camera(gRecordingDemo);
	CSM_GameDemo_Record_RandomSeed(gRecordingDemo);
	CSM_GameDemo_Record_Timer(gRecordingDemo);
}

void CSM_GameDemo_StopRecord(GAME_DEMO_STATE* pState)
{
	if (!gRecordingDemo)
		return;

	// Get navigation ptrs
	CaveNet::DataStructures::NetClient*(*GetClient)(int iIndex) = (CaveNet::Server::IsHosting() ? CaveNet::Server::ServerClientGroup::GetClient : (CaveNet::Client::IsConnected() ? CaveNet::Client::ClientClientGroup::GetClient : NULL));
	int(*GetClientCount)()                                      = (CaveNet::Server::IsHosting() ? CaveNet::Server::ServerClientGroup::GetClientCount : (CaveNet::Client::IsConnected() ? CaveNet::Client::ClientClientGroup::GetClientCount : NULL));

	if (GetClient && GetClientCount)
	{
		for (int i = 0; i < GetClientCount(); i++)
		{
			CaveNet::DataStructures::NetClient* pClient = GetClient(i);
			pClient->mCache.recorder_player             = NULL;
		}
	}

	// Set the name of the player
	free(gRecordingDemo->mPlayers.list[0].name);
	gRecordingDemo->mPlayers.list[0].name = strdup(CaveNet::Client::Config::NetName.ToString());

	// Copy the recorded demo into state.
	memcpy(pState, gRecordingDemo, sizeof(GAME_DEMO_STATE));

	// Free the demo
	delete gRecordingDemo;

	// Reset it
	gRecordingDemo = NULL;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_PlayerState(GAME_DEMO_STATE* pState, const char* pName, SafeClientInterface* pInterface)
{
	PXDEMO_PLAYER_STRUCT* pPlayer = NULL;
	int iIndex                    = 0;

	// Player isn't in the game demo.
	if ((pPlayer = CSM_GameDemo_Players_Find(pState, pName, &iIndex)) == NULL)
		return NULL;

	// Create the event
	PXDEMO_EVENT_STRUCT* pEvent = CSM_GameDemo_Events_Add(pState, PXDEMO_EVENT_TYPE::PXDET_SET_PLAYER_STATE, pState->mInfo.total_sim_frames);

	if (!pEvent)
		return NULL;

	// Configure the state
	pEvent->data.player_state.player_idx   = iIndex;
	pEvent->data.player_state.change_flags = PXDPSC_ALL_FLAGS;

	SET_PLAYER_STATE(pInterface->mMC, &pEvent->data.player_state.state, pInterface);

	pEvent->data.player_state.state.weapon_code     = pInterface->mArms[*pInterface->mSelectedArms].code;
	pEvent->data.player_state.state.weapon_ammo     = pInterface->mArms[*pInterface->mSelectedArms].num;
	pEvent->data.player_state.state.weapon_max_ammo = pInterface->mArms[*pInterface->mSelectedArms].max_num;
	pEvent->data.player_state.state.weapon_level    = pInterface->mArms[*pInterface->mSelectedArms].level;
	pEvent->data.player_state.state.weapon_exp      = pInterface->mArms[*pInterface->mSelectedArms].exp;
	pEvent->data.player_state.state.selectedArms    = *pInterface->mSelectedArms;

	memcpy(&pPlayer->state, &pEvent->data.player_state.state, sizeof(PXDEMO_PLAYERSTATE_STRUCT));

	// Check to see if we can set change flags for this
	if (pPlayer->last_state_frame_idx >= 0 && pPlayer->last_state_frame_idx < pState->mFrames.count && pPlayer->last_state_event_idx >= 0)
	{
		PXDEMO_FRAME_STRUCT* pFrame    = &pState->mFrames.list[pPlayer->last_state_frame_idx];
		PXDEMO_EVENT_STRUCT* pOldEvent = (pFrame->event_list && pPlayer->last_state_event_idx < pFrame->event_count ? &pFrame->event_list[pPlayer->last_state_event_idx] : NULL);

		// Check to see if the event is valid, first
		if (pOldEvent)
		{
			// Reset our change flags
			pEvent->data.player_state.change_flags = 0;

			// Initialize data buffer
			char DataBuffer1[PXDPSC_BIGGEST_TYPE + 1];
			char DataBuffer2[PXDPSC_BIGGEST_TYPE + 1];

			// Loop through all of the recorded fields for playerstate
			for (int i = 0; i < PXDPSC_COUNT; i++)
			{
				DEMO_PLAYERSTATE_FIELD* pField = &gDemoPlayerstateFieldList[i];

				// Skip matches
				if (!memcmp(pField->GetValue(&pOldEvent->data.player_state.state, DataBuffer1), pField->GetValue(&pEvent->data.player_state.state, DataBuffer2), pField->size))
					continue;

				// Got a mismatch! Add it to the change list.
				pEvent->data.player_state.change_flags |= pField->change_flag;
			}
		}
	}

	// Find the new indexes
	pPlayer->last_state_event_idx = CSM_GameDemo_Frames_Find(pState, pState->mInfo.total_sim_frames, &pPlayer->last_state_frame_idx)->event_count - 1;

	return pEvent;
}

PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_Stage(GAME_DEMO_STATE* pState)
{
	PXDEMO_EVENT_STRUCT* pEvent  = CSM_GameDemo_Events_Add(pState, PXDEMO_EVENT_TYPE::PXDET_SET_STAGE, pState->mInfo.total_sim_frames);
	pEvent->data.stage.stage_idx = gStageNo;

	// Record random seed, too
	CSM_GameDemo_Record_RandomSeed(pState);

	// And also record the NPCs
	for (int i = 0; i < NPC_MAX; i++)
		CSM_GameDemo_Record_NpcState(gRecordingDemo, &gNPC[i], i);
	for (int i = 0; i < BOSS_MAX; i++)
		CSM_GameDemo_Record_NpcState(gRecordingDemo, &gBoss[i], NPC_MAX + i);

	return pEvent;
}

PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_RandomSeed(GAME_DEMO_STATE* pState)
{
	PXDEMO_EVENT_STRUCT* pEvent   = CSM_GameDemo_Events_Add(pState, PXDEMO_EVENT_TYPE::PXDET_SET_RANDOM_SEED, pState->mInfo.total_sim_frames);
	pEvent->data.random_seed.seed = rep_grand();

	return pEvent;
}

PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_NpcState(GAME_DEMO_STATE* pState, NPCHAR* pNpc, int iIndex)
{
	PXDEMO_EVENT_STRUCT* pEvent        = CSM_GameDemo_Events_Add(pState, PXDEMO_EVENT_TYPE::PXDET_SET_NPC_STATE, pState->mInfo.total_sim_frames);
	pEvent->data.npc_state.npc_idx     = CLAMP(iIndex, 0, BOSS_MAX + NPC_MAX);
	pEvent->data.npc_state.random_seed = rep_grand();
	
	if (pNpc->cond & 0x80)
		memcpy(&pEvent->data.npc_state.state, pNpc, sizeof(NPCHAR));
	else
		memset(&pEvent->data.npc_state.state, 0, sizeof(NPCHAR));

	return pEvent;
}

PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_Fade(GAME_DEMO_STATE* pState, PXDEMO_FADE_TYPE iType, Directions iDirect)
{
	PXDEMO_EVENT_STRUCT* pEvent     = CSM_GameDemo_Events_Add(pState, PXDEMO_EVENT_TYPE::PXDET_DO_SCREEN_FADE, pState->mInfo.total_sim_frames);
	pEvent->data.screen_fade.direct = iDirect;
	pEvent->data.screen_fade.type   = iType;

	return pEvent;
}

PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_Camera(GAME_DEMO_STATE* pState)
{
	PXDEMO_EVENT_STRUCT* pEvent = CSM_GameDemo_Events_Add(pState, PXDEMO_EVENT_TYPE::PXDET_SET_CAMERA, pState->mInfo.total_sim_frames);
	pEvent->data.camera.x       = gFrame.x;
	pEvent->data.camera.y       = gFrame.y;
	pEvent->data.camera.wait    = gFrame.wait;
	pEvent->data.camera.quake   = gFrame.quake;
	pEvent->data.camera.quake2  = gFrame.quake2;
	pEvent->data.camera.target  = 0;

	// Determine what the target should be set to
	if (gFrame.tgt_x == &gMC.tgt_x && gFrame.tgt_y == &gMC.tgt_y)
		return pEvent;

	// Search for the clientgroups
	for (int i = 0; i < CaveNet::Client::ClientClientGroup::GetClientCount(); i++)
	{
		CaveNet::DataStructures::NetClient* pClient = CaveNet::Client::ClientClientGroup::GetClient(i);

		// Skip mismatches
		if (!pClient->GetPlayer() || gFrame.tgt_x != &pClient->GetPlayer()->m_Player.npc.x && gFrame.tgt_y != &pClient->GetPlayer()->m_Player.npc.y)
			continue;

		int iIndex = 0;

		// Got it!
		if (!CSM_GameDemo_Players_Find(pState, pClient->mCache.net_name, &iIndex))
			return pEvent;

		pEvent->data.camera.target = iIndex;
		return pEvent;
	}
	
	// Search for it in NPCs
	if (gFrame.tgt_x >= &gNPC[0].x && gFrame.tgt_x <= &gNPC[NPC_MAX].y)
	{
		// Found it!
		pEvent->data.camera.target = pState->mPlayers.count + (((intptr_t)gFrame.tgt_x - (intptr_t)gNPC) / sizeof(NPCHAR));
		return pEvent;
	}
	
	// Search for it in bosses
	if (gFrame.tgt_x >= &gBoss[0].x && gFrame.tgt_x <= &gBoss[BOSS_MAX].y)
	{
		// Found it!
		pEvent->data.camera.target = pState->mPlayers.count + NPC_MAX + (((intptr_t)gFrame.tgt_x - (intptr_t)gBoss) / sizeof(NPCHAR));
		return pEvent;
	}

	return pEvent;
}

PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_Timer(GAME_DEMO_STATE* pState)
{
	PXDEMO_EVENT_STRUCT* pEvent    = CSM_GameDemo_Events_Add(pState, PXDEMO_EVENT_TYPE::PXDET_SET_TIMER, pState->mInfo.total_sim_frames);
	pEvent->data.timer.time_passed = GetTimeCounter();

	return pEvent;
}

int CSM_GameDemo_Record_NewFrame(GAME_DEMO_STATE* pState)
{
	return ++pState->mInfo.total_sim_frames;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_endcode.h>