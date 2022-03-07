/*
  Cavestory Multiplayer API
  Copyright (C) 2021 Johnny Ledger

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
	 claim that you wrote the original software. If you use this software
	 in a product, an acknowledgment in the product documentation would be
	 appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
	 misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*!
  @file GameDemo.h
*/

#ifndef CAVESTORY_MOD_API_H_
#error "Please include CavestoryModAPI.h before including sub-classes."
#endif

#include <CSMAPI_begincode.h>

#pragma once

/// @ingroup Demos
/// @{
///

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief GameDemo Event Types
///
enum PXDEMO_EVENT_TYPE : unsigned char
{
	/// Invalid
	PXDET_INVALID_EVENT    = 0,

	/// Set the state of a player
	PXDET_SET_PLAYER_STATE = 1,

	/// Transfer to a new stage
	PXDET_SET_STAGE        = 2,

	/// Set the random seed
	PXDET_SET_RANDOM_SEED  = 3,

	/// Set an NPC
	PXDET_SET_NPC_STATE    = 4,

	/// Fade in or fade out the screen
	PXDET_DO_SCREEN_FADE   = 5,

	/// Set the game camera's details
	PXDET_SET_CAMERA       = 6,

	/// Set the game timer
	PXDET_SET_TIMER        = 7,

	/// How many events there can be
	PXDET_MAX_EVENTS
};

/// @brief Fade effect types
///
enum PXDEMO_FADE_TYPE : unsigned char
{
	/// Stop fading
	PXDFT_CLEAR_FADE = 0,

	/// Fade in
	PXDFT_FADE_IN    = 1,

	/// Fade out
	PXDFT_FADE_OUT   = 2,

	/// How many fade types there
	PXDFT_MAX_FADES
};

/// @brief Changed playerstate variables between frames
///
/// Lowers the filesize of demos considerably
///
enum PXDEMO_PLAYERSTATE_CHANGE : unsigned int
{
	/// cond
	PXDPSC_cond               = 0x00000001,

	/// x
	PXDPSC_x                  = 0x00000002,

	/// y
	PXDPSC_y                  = 0x00000004,

	/// xm
	PXDPSC_xm                 = 0x00000008,

	/// ym
	PXDPSC_ym                 = 0x00000010,

	/// life
	PXDPSC_life               = 0x00000020,

	/// max_life
	PXDPSC_max_life           = 0x00000040,

	/// input_flags
	PXDPSC_input_flags        = 0x00000080,

	/// input_flags_trg
	PXDPSC_input_flags_trg    = 0x00000100,

	/// weapon_code
	PXDPSC_weapon_code        = 0x00000200,

	/// weapon_ammo
	PXDPSC_weapon_ammo        = 0x00000400,

	/// weapon_max_ammo
	PXDPSC_weapon_max_ammo    = 0x00000800,

	/// weapon_level
	PXDPSC_weapon_level       = 0x00001000,

	/// weapon_exp
	PXDPSC_weapon_exp         = 0x00002000,

	/// selectedArms
	PXDPSC_selectedArms       = 0x00004000,

	/// direct
	PXDPSC_direct             = 0x00008000,

	/// ani_no
	PXDPSC_ani_no             = 0x00010000,

	/// ani_wait
	PXDPSC_ani_wait           = 0x00020000,

	/// equip
	PXDPSC_equip              = 0x00040000,

	/// star
	PXDPSC_star               = 0x00080000,

	/// shock
	PXDPSC_shock              = 0x00100000,

	/// unit
	PXDPSC_netanim            = 0x00200000,

	/// physics_normal
	PXDPSC_physics_normal     = 0x00400000,

	/// physics_underwater
	PXDPSC_physics_underwater = 0x00800000,

	/// ignore_water
	PXDPSC_ignore_water       = 0x01000000,

	/// boost_sw
	PXDPSC_boost_sw           = 0x02000000,

	/// boost_cnt
	PXDPSC_boost_cnt          = 0x04000000,

	/// Flags
	PXDPSC_flag               = 0x08000000,

	/// Up / down
	PXDPSC_updown             = 0x10000000,

	/// Count1
	PXDPSC_counts             = 0x20000000,

	/// Count2
	PXDPSC_act_wait           = 0x40000000,

	/// Count2
	PXDPSC_act_no             = 0x80000000,

	/// All flags combined
	PXDPSC_ALL_FLAGS          = 0xFFFFFFFF,

	/// Number of recordable variables for playerstate.
	PXDPSC_COUNT              = 34 + /* MYCHAR_PHYSICS */(8 * 2),

	/// Number of recordable variables (with unique change flags)
	PXDPSC_UNIQUE_COUNT       = PXDPSC_COUNT - (/* MYCHAR_PHYSICS */(8 * 2) + /* UP/DOWN */1),

	/// Biggest variable type
	PXDPSC_BIGGEST_TYPE       = sizeof(MYCHAR_PHYSICS)
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Game info
///
struct PXDEMO_DEMOINFO_STRUCT
{
	/// The version of CaveNet at the time
	///
	unsigned int cavenet_version;

	/// The loaded mod's internal name
	///
	char* mod_name;

	/// The version of the mod at the time
	///
	int mod_version;

	/// The frame length for this demo
	///
	int total_sim_frames;

	/// The current play frame
	///
	int play_frame;

	/// The random seed
	///
	int random_seed;

	/// NPC flags
	///
	unsigned char flags[1000];

	/// Skip flags
	///
	unsigned char skip_flags[8];

	/// Game flags
	///
	int game_flags;
};

/// @brief State of a player
///
struct PXDEMO_PLAYERSTATE_STRUCT
{
	/// Condition of the player
	///
	unsigned char cond;

	/// Current X position
	///
	int x;

	/// Current Y position
	///
	int y;

	/// Current X motion
	///
	int xm;
	
	/// Current Y motion
	///
	int ym;

	/// The life of this player
	///
	short life;

	/// The max life of this player
	///
	short max_life;

	/// Input flags
	///
	int input_flags;

	/// Trg input flags
	///
	int input_flags_trg;

	/// The held weapon index
	///
	int weapon_code;

	/// The held weapon's ammo
	///
	int weapon_ammo;
	
	/// The held weapon's max ammo
	///
	int weapon_max_ammo;

	/// The held weapon's max ammo
	///
	unsigned char weapon_level;

	/// The held weapon's exp
	///
	int weapon_exp;

	/// Selected weapon
	///
	int selectedArms;

	/// Direction
	///
	int direct;

	/// Animation number
	///
	int ani_no;

	/// Animation wait
	///
	int ani_wait;

	/// Equip flags
	///
	int equip;

	/// Amount of stars
	///
	short star;

	/// Player damage invulnerability counter
	///
	unsigned char shock;

	/// Player state
	///
	int netanim;

	/// Normal physics
	///
	MYCHAR_PHYSICS physics_normal;

	/// Underwater physics
	///
	MYCHAR_PHYSICS physics_underwater;

	/// Ignore water
	///
	bool ignore_water;

	/// Booster SW
	///
	signed char boost_sw;

	/// Booster count
	///
	int boost_cnt;

	/// Flags
	///
	unsigned int flag;

	/// Looking up
	///
	int up;

	/// Looking down
	///
	int down;

	/// a
	///
	signed char a;

	/// Target X
	///
	int tgt_x;

	/// Target Y
	///
	int tgt_y;

	/// Current air
	///
	int air;

	/// Count
	///
	int count1;

	/// Count
	///
	int count2;

	/// Count
	///
	int count3;

	/// Count
	///
	int count4;

	/// Act number
	///
	int act_no;

	/// Act wait
	///
	int act_wait;

};

/// @brief Player info
///
struct PXDEMO_PLAYER_STRUCT
{
	/// This player's name
	///
	char* name;

	/// This player's ARMS tables
	///
	ARMS arms[ARMS_MAX];

	/// This player's character
	///
	CustomPlayer* character;

	/// This player's character name
	///
	char* character_name;

	/// This player's state
	///
	PXDEMO_PLAYERSTATE_STRUCT state;

	/// This player's last state.
	///
	/// @warning Do NOT use this variable! It is used internally with the recording system.
	int last_state_frame_idx;

	/// The event index of this player's last state.
	///
	/// @warning Do NOT use this variable! It is used internally with the recording system.
	int last_state_event_idx;

	/// This character's color
	///
	GUI_COLOR char_color;

	/// Whether to use custom color or not.
	///
	bool use_color;
};

struct PXDEMO_EVENT_STRUCT;

/// @brief Frame info
///
struct PXDEMO_FRAME_STRUCT
{
	/// An allocated list of events to perform for this frame.
	///
	PXDEMO_EVENT_STRUCT* event_list;

	/// Size of event_list.
	///
	int event_count;

	/// This frame's number
	///
	int frame_no;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Event: Set player state
///
struct EVENT_SETPLAYERSTATE_STRUCT
{
	/// The player's index
	///
	int player_idx;

	/// The flags that determine what has changed between the last state and this one.
	///
	unsigned int change_flags;

	/// The player's new state
	///
	PXDEMO_PLAYERSTATE_STRUCT state;
};

/// @brief Event: Set current stage
///
struct EVENT_SETSTAGE_STRUCT
{
	/// The stage's index that we want to switch to
	///
	int stage_idx;
};

/// @brief Event: Set the random seed
///
struct EVENT_SETRANDOMSEED_STRUCT
{
	/// The new random seed
	///
	int seed;
};

/// @brief Event: Set the state of an NPC at the given slot
///
struct EVENT_SETNPCSTATE_STRUCT
{
	/// The NPC's slot index
	///
	unsigned int npc_idx;

	/// Random seed
	///
	unsigned int random_seed;

	/// The state of the NPC
	///
	NPCHAR state;
};

/// @brief Event: Do a screen fade in / out.
///
struct EVENT_DOSCREENFADE_STRUCT
{
	/// The fade type.
	///
	/// @see PXDEMO_FADE_TYPE
	PXDEMO_FADE_TYPE type;

	/// The direction of the fade.
	///
	/// @see Direction
	unsigned char direct;
};

/// @brief Event: Set camera details.
///
struct EVENT_SETCAMERA_STRUCT
{
	/// The X position of the camera.
	///
	int x;

	/// The Y position of the camera.
	///
	int y;

	/// The focus target for this camera.
	/// If this is between 0 and the number of players this demo has, then the camera should focus the specified player.
	/// If this is between NumOfPlayersThisDemoHas and NumOfPlayersThisDemoHas+NPC_MAX, then the camera should focus gNPC[camera.target - (1 + NumOfPlayersThisDemoHas)].
	/// If this is between NumOfPlayersThisDemoHas+NPC_MAX and NumOfPlayersThisDemoHas+NPC_MAX+BOSS_MAX, then it's gBoss[camera.target - (1 + NPC_MAX + NumOfPlayersThisDemoHas)].
	int target;

	/// Camera speed
	///
	int wait;

	/// Quake amount.
	///
	int quake;

	/// Quake amount (2).
	///
	int quake2;
};

/// @brief Event: Set game timer.
///
struct EVENT_SETTIMER_STRUCT
{
	/// The timer position
	///
	int time_passed;
};

/// A union containing all possibilities for an event.
///
union PXDEMO_EVENT_UNION
{
	/// PXDEMO_EVENT_TYPE::PXDET_SET_PLAYER_STATE
	///
	EVENT_SETPLAYERSTATE_STRUCT player_state;

	/// PXDEMO_EVENT_TYPE::PXDET_SET_STAGE
	///
	EVENT_SETSTAGE_STRUCT stage;

	/// PXDEMO_EVENT_TYPE::PXDET_SET_RANDOM_SEED
	///
	EVENT_SETRANDOMSEED_STRUCT random_seed;
	
	/// PXDEMO_EVENT_TYPE::PXDET_SET_NPC_STATE
	///
	EVENT_SETNPCSTATE_STRUCT npc_state;

	/// PXDEMO_EVENT_TYPE::PXDET_DO_SCREEN_FADE
	///
	EVENT_DOSCREENFADE_STRUCT screen_fade;

	/// PXDEMO_EVENT_TYPE::PXDET_SET_CAMERA
	///
	EVENT_SETCAMERA_STRUCT camera;

	/// PXDEMO_EVENT_TYPE::PXDET_SET_TIMER
	///
	EVENT_SETTIMER_STRUCT timer;
};

/// @brief Main event info struct
///
struct PXDEMO_EVENT_STRUCT
{
	/// Event type
	///
	PXDEMO_EVENT_TYPE type;

	/// A union containing all possible data for this event.
	///
	union PXDEMO_EVENT_UNION data;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Holds all the players.
///
struct PXDEMO_PLAYER_CHUNK
{
	/// An allocated list that holds the players.
	///
	PXDEMO_PLAYER_STRUCT* list;

	/// The size of list.
	///
	unsigned int count;
};

/// @brief Holds all frames.
///
struct PXDEMO_FRAME_CHUNK
{
	/// An allocated list that holds the frames.
	///
	PXDEMO_FRAME_STRUCT* list;

	/// The size of list.
	///
	unsigned int count;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Holds a game demo's state.
///
struct GAME_DEMO_STATE
{
	/// First demo state
	///
	static GAME_DEMO_STATE* first;

	/// The game details
	///
	PXDEMO_DEMOINFO_STRUCT mInfo;

	/// Players
	///
	PXDEMO_PLAYER_CHUNK mPlayers;

	/// Frames
	///
	PXDEMO_FRAME_CHUNK mFrames;

	/// Next demo state
	///
	GAME_DEMO_STATE* mNext;

	/// The checksum of this gamedemo
	///
	int mChecksum;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Register a demo to the active linkage list.
///
/// @param pState The state to be registered.
extern CAVESTORY_MOD_API void CSM_GameDemo_Link(
	GAME_DEMO_STATE* pState
);

/// @brief Unregister a demo from the active linkage list.
///
/// @param pState The state to be unregistered.
extern CAVESTORY_MOD_API void CSM_GameDemo_Unlink(
	GAME_DEMO_STATE* pState
);

/// @brief Get the first in the gamestate link.
///
/// @param pFirst A pointer to a GAME_DEMO_STATE* variable that should hold the value.
///
/// @return Returns true if there was an object in the linkage, false otherwise.
extern CAVESTORY_MOD_API bool CSM_GameDemo_FirstLink(
	GAME_DEMO_STATE** pFirst
);

/// @brief Initialize a GAME_DEMO_STATE object without linking it.
///
/// @param pState The state to initialize.
extern CAVESTORY_MOD_API void CSM_GameDemo_Init(
	GAME_DEMO_STATE* pState
);

/// @brief Initialize a GAME_DEMO_STATE object whilst also re-linking it.
///
/// @param pState The state to initialize.
extern CAVESTORY_MOD_API void CSM_GameDemo_Init_Relink(
	GAME_DEMO_STATE* pState
);

/// @brief Free a GAME_DEMO_STATE object. This should be called after initializing the object.
///
/// @param pState The state to free.
extern CAVESTORY_MOD_API void CSM_GameDemo_Free(
	GAME_DEMO_STATE* pState
);

/// @brief Load a game demo.
///
/// @param pState The state to load the demo into.
/// @param pFileName The name of the file to load into pState.
///
/// @return Returns 0 on success, otherwise returns a non-zero error code.
extern CAVESTORY_MOD_API int CSM_GameDemo_Load(
	GAME_DEMO_STATE* pState,
	const char* pFileName
);

/// @brief Save a game demo.
///
/// @param pState The state to save the demo into.
/// @param pFileName The output name of the file.
///
/// @return Returns 0 on success, otherwise returns a non-zero error code.
extern CAVESTORY_MOD_API int CSM_GameDemo_Save(
	GAME_DEMO_STATE* pState,
	const char* pFileName
);

/// @brief Combine game demos.
///
/// @param pDst The desired demo destination.
/// @param pSource The demo to insert.
/// @param iFrameOffset The frame index at which the demo should be inserted. Set to 0xFFFFFFFF to insert at the end of the demo.
///
/// @return Returns true on success.
extern CAVESTORY_MOD_API bool CSM_GameDemo_CombineDemos(
	GAME_DEMO_STATE* pDst,
	GAME_DEMO_STATE* pSource,
	unsigned int iFrameOffset = 0xFFFFFFFF
);

/// @brief Optimize a game demo.
///
/// @param pState The state to optimize.
/// @param bSaveAndLoad Save the game before optimizing the demo, and then load the save afterwards.
///
/// @return Returns true on success.
extern CAVESTORY_MOD_API bool CSM_GameDemo_Optimize(
	GAME_DEMO_STATE* pState,
	bool bSaveAndLoad
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Add a player to the demo state.
/// 
/// @param pState The gamedemo state to manipulate.
/// @param pName The name of the client.
/// @param pInterface A safe client interface. Can be NULL.
///
/// @return Returns a pointer to newly allocated PXDEMO_PLAYER_STRUCT on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_PLAYER_STRUCT* CSM_GameDemo_Players_Add(
	GAME_DEMO_STATE* pState,
	const char* pName,
	SafeClientInterface* pInterface
);

/// @brief Remove a player from the demo state.
///
/// @param pState The gamedemo state to manipulate.
/// @param pName The name of the player to remove.
///
/// @return Returns true on success.
extern CAVESTORY_MOD_API bool CSM_GameDemo_Players_Remove(
	GAME_DEMO_STATE* pState,
	const char* pName
);

/// @brief Find a player's object by name.
///
/// @param pState The gamedemo state to manipulate.
/// @param pName The name of the player to find.
/// @param pIndex A pointer to an int that will hold the index at which the entry was found at. Can be NULL.
///
/// @return Returns a pointer to the specified player's PXDEMO_PLAYER_STRUCT on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_PLAYER_STRUCT* CSM_GameDemo_Players_Find(
	GAME_DEMO_STATE* pState,
	const char* pName,
	int* pIndex = NULL
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Add an event to the demo.
/// 
/// @param pState The gamedemo state to manipulate.
/// @param iType The type of event to allocate.
/// @param iFrameNo The frame number to add the event to.
///
/// @return Returns a pointer to the newly allocated PXDEMO_EVENT_STRUCT on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_EVENT_STRUCT* CSM_GameDemo_Events_Add(
	GAME_DEMO_STATE* pState,
	PXDEMO_EVENT_TYPE iType,
	int iFrameNo
);

/// @brief Remove an event from the demo.
///
/// @param pState The gamedemo state to manipulate.
/// @param iType The type of event to remove.
/// @param iFrameNo The frame number to remove the event from.
/// @param iStartIndex The starting index in the frame's event list.
///
/// @return Returns true on success.
extern CAVESTORY_MOD_API bool CSM_GameDemo_Events_Remove(
	GAME_DEMO_STATE* pState,
	PXDEMO_EVENT_TYPE iType,
	int iFrameNo,
	int iStartIndex = 0
);

/// @brief Find an event.
///
/// @param pState The gamedemo state to manipulate.
/// @param iType The type of event to remove.
/// @param iFrameNo The frame number the event should belong to.
/// @param iStartIndex The starting index in the frame's event list.
/// @param pFramePtr  A pointer to the variable that will hold the frame that holds the event. Can be NULL.
/// @param pFrameIndex A pointer to an int that will hold the index at which the frame was found at. Can be NULL.
/// @param pEventIndex A pointer to an int that will hold the index at which the event was found at. Can be NULL.
///
/// @return Returns a pointer to the specified event's PXDEMO_EVENT_STRUCT on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_EVENT_STRUCT* CSM_GameDemo_Events_Find(
	GAME_DEMO_STATE* pState,
	PXDEMO_EVENT_TYPE iType,
	int iFrameNo,
	int iStartIndex = 0,
	PXDEMO_FRAME_STRUCT** pFramePtr = NULL,
	int* pFrameIndex = NULL,
	int* pEventIndex = NULL
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Add af rame to the demo.
/// 
/// @param pState The gamedemo state to manipulate.
/// @param iFrameNo The frame number.
///
/// @return Returns a pointer to the newly allocated PXDEMO_FRAME_STRUCT on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_FRAME_STRUCT* CSM_GameDemo_Frames_Add(
	GAME_DEMO_STATE* pState,
	int iFrameNo
);

/// @brief Remove a frame from the demo.
///
/// @param pState The gamedemo state to manipulate.
/// @param iFrameNo The frame number.
///
/// @return Returns true on success.
extern CAVESTORY_MOD_API bool CSM_GameDemo_Frames_Remove(
	GAME_DEMO_STATE* pState,
	int iFrameNo
);

/// @brief Find a frame.
///
/// @param pState The gamedemo state to manipulate.
/// @param iFrameNo The frame number to search for.
/// @param pIndex A pointer to an int that will hold the index at which the entry was found at. Can be NULL.
///
/// @return Returns a pointer to the specified frame's PXDEMO_FRAME_STRUCT on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_FRAME_STRUCT* CSM_GameDemo_Frames_Find(
	GAME_DEMO_STATE* pState,
	int iFrameNo,
	int* pIndex = NULL
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Load a demo's gamestate
///
/// @param pState The demo to simulate the game with.
extern CAVESTORY_MOD_API void CSM_GameDemo_LoadGamestate(
	GAME_DEMO_STATE* pState
);

/// @brief Act for a frame
/// 
/// @param pState The demo to simulate the game with.
/// @param bFreezeFrame If set to true, the frame counter does not advance, and nothing is acted.
/// @param bDontDraw Don't draw anything.
/// @param bDrawHUD Draw the HUD.
/// @param bDrawFade Draw the screen fade effect. If bDontDraw is set to 'true', then this is ignored.
extern CAVESTORY_MOD_API void CSM_GameDemo_Act(
	GAME_DEMO_STATE* pState,
	bool bFreezeFrame = false,
	bool bDontDraw = false,
	bool bDrawHUD = true,
	bool bDrawFade = true
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Begin recording a demo.
///
extern CAVESTORY_MOD_API void CSM_GameDemo_StartRecord();

/// @brief Stop recording a demo.
///
/// @param pState A pointer to the state to hold the recorded gamestate.
extern CAVESTORY_MOD_API void CSM_GameDemo_StopRecord(
	GAME_DEMO_STATE* pState
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Record a player's state.
///
/// @param pState The gamedemo state.
/// @param pName The name of the client.
/// @param pInterface An interface to the player's details.
///
/// @return Returns a pointer to the added event on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_PlayerState(
	GAME_DEMO_STATE* pState,
	const char* pName,
	SafeClientInterface* pInterface
);

/// @brief Record the current stage index.
///
/// @param pState The gamedemo state.
///
/// @return Returns a pointer to the added event on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_Stage(
	GAME_DEMO_STATE* pState
);

/// @brief Record the current random seed.
///
/// @param pState The gamedemo state.
///
/// @return Returns a pointer to the added event on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_RandomSeed(
	GAME_DEMO_STATE* pState
);

/// @brief Record the state of an NPC.
///
/// @param pState The gamedemo state.
/// @param pNpc The NPC to record the state of.
/// @param iIndex The NPC index.
///
/// @return Returns a pointer to the added event on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_NpcState(
	GAME_DEMO_STATE* pState,
	NPCHAR* pNpc,
	int iIndex
);

/// @brief Record a screen fade.
///
/// @param pState The gamedemo state.
/// @param iType The type of fade.
/// @param iDirect The direction of the fade.
///
/// @return Returns a pointer to the added event on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_Fade(
	GAME_DEMO_STATE* pState,
	PXDEMO_FADE_TYPE iType,
	Directions iDirect
);

/// @brief Record the game camera's current details.
///
/// @param pState The gamedemo state.
///
/// @return Returns a pointer to the added event on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_Camera(
	GAME_DEMO_STATE* pState
);

/// @brief Record the game's timer.
///
/// @param pState The gamedemo state.
///
/// @return Returns a pointer to the added event on success, otherwise returns NULL.
extern CAVESTORY_MOD_API PXDEMO_EVENT_STRUCT* CSM_GameDemo_Record_Timer(
	GAME_DEMO_STATE* pState
);

/// @brief Increment the simulated frame count.
/// 
/// @param pState The gamedemo state.
///
/// @return Returns the current simulated frame number.
extern CAVESTORY_MOD_API int CSM_GameDemo_Record_NewFrame(
	GAME_DEMO_STATE* pState
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// A pointer to the current recording demo. If NULL, then the game is not being recorded.
///
extern CAVESTORY_MOD_API GAME_DEMO_STATE* gRecordingDemo;

#include <CSMAPI_endcode.h>

/// @}
///