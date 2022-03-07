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
  @file CSMAPI_enums.h
*/

#pragma once

#ifndef BIT
#define BIT(x) (1 << x)
#endif

/// @brief Directions
///
enum Directions
{
	/// Left
	DIRECT_LEFT = 0,

	/// Up
	DIRECT_UP = 1,

	/// Right
	DIRECT_RIGHT = 2,

	/// Down
	DIRECT_DOWN = 3,

	/// Center
	DIRECT_CENTER = 4,

	/// Right-Up
	DIRECT_RIGHT_UP = 5,

	/// Down-Right
	DIRECT_DOWN_RIGHT = 6,

	/// Number of possible directions
	DIRECT_COUNT
};

/// Key input types.
///
enum KeyInputType
{
	/// gKey and gKeyTrg
	KIT_Key       = 0,

	/// gKey2 and gKeyTrg2
	KIT_Key2      = 1,

	/// gKey_JOY and gKeyTrg_JOY
	KIT_KeyJoypad = 2
};

/// @brief A table of keybind descriptors.
///
/// Contains all keybind descriptors.
///
/// @see GetKeybind
enum CSM_KEYBIND_DESC
{
	/// Jump button
	CSM_KEYBIND_DESC_JUMP      = 0,
	/// Shoot button
	CSM_KEYBIND_DESC_SHOOT     = 1,
	/// Next Weapon button
	CSM_KEYBIND_DESC_ARMS_NEXT = 2,
	/// Previous Weapon button
	CSM_KEYBIND_DESC_ARMS_PREV = 3,
	/// Inventory button
	CSM_KEYBIND_DESC_INVENTORY = 4,
	/// Map button
	CSM_KEYBIND_DESC_MAP       = 5,
	/// "OK" button
	CSM_KEYBIND_DESC_OK        = 6,
	/// "Cancel" button
	CSM_KEYBIND_DESC_CANCEL    = 7,
	/// Left button
	CSM_KEYBIND_DESC_LEFT      = 8,
	/// Up button
	CSM_KEYBIND_DESC_UP        = 9,
	/// Right button
	CSM_KEYBIND_DESC_RIGHT     = 10,
	/// Down button
	CSM_KEYBIND_DESC_DOWN      = 11,
};

/// @brief A table of input descriptors.
///
/// Contains all input descriptors.
///
/// @see GetInput
enum CSM_KEY_DESC
{
	/// Contains input flags from KEYBIND.
	///
	/// @note Input flags are set when a button is pressed.
	/// @note Input flags are cleared when a button is released.
	///
	/// @see KEYBIND
	CSM_KEY_DESC_KEY        = 0,

	/// Contains input flags from KEYBIND.
	/// 
	/// @note Input flags are set when a button is pressed.
	/// @note Input flags are cleared one frame after the button is pressed.
	///
	/// @see KEYBIND
	CSM_KEY_DESC_KEYTRG     = 1,

	/// Contains input flags from KEYBIND2.
	///
	/// @note Input flags are set when a button is pressed.
	/// @note Input flags are cleared when a button is released.
	///
	/// @see KEYBIND2
	CSM_KEY_DESC_KEY2       = 2,

	/// Contains input flags from KEYBIND2.
	/// 
	/// @note Input flags are set when a button is pressed.
	/// @note Input flags are cleared one frame after the button is pressed.
	///
	/// @see KEYBIND2
	CSM_KEY_DESC_KEYTRG2    = 3,

	/// Contains input flags from KEYBIND_JOYSTICK.
	/// 
	/// @note Input flags are set when a button is pressed.
	/// @note Input flags are cleared when a button is released.
	///
	/// @see KEYBIND_JOYSTICK
	CSM_KEY_DESC_KEY_JOY    = 4,

	/// Contains input flags from KEYBIND_JOYSTICK.
	/// 
	/// @note Input flags are set when a button is pressed.
	/// @note Input flags are cleared one frame after the button is pressed.
	///
	/// @see KEYBIND_JOYSTICK
	CSM_KEY_DESC_KEYTRG_JOY = 5
};

/// @brief A table of input flags.
///
/// Contains input flags for #CSM_KEY_DESC_KEY.
///
/// @see GetInput
/// @see CSM_KEY_DESC_KEY
enum KEYBIND
{
	/// Left arrow key
	KEY_LEFT =		0x00000001,
	/// Right arrow key
	KEY_RIGHT =		0x00000002,
	/// Up arrow key
	KEY_UP =		0x00000004,
	/// Down arrow key
	KEY_DOWN =		0x00000008,
	/// "Map" button
	KEY_MAP =		0x00000010,
	/// X key
	KEY_X =			0x00000020,
	/// Z key
	KEY_Z =			0x00000040,
	/// "Next Weapon" button
	KEY_ARMS =		0x00000080,
	/// "Previous weapon" button
	KEY_ARMSREV =	0x00000100,
	/// Shift key
	KEY_SHIFT =		0x00000200,
	/// F1 key
	KEY_F1 =		0x00000400,
	/// F2 key
	KEY_F2 =		0x00000800,
	/// KeyItem Button
	KEY_ITEM =		0x00001000,
	/// Escape button
	KEY_ESCAPE =	0x00008000,
	/// Forward slash key
	KEY_ALT_LEFT =	0x00010000,
	/// Period key
	KEY_ALT_DOWN =	0x00020000,
	/// Comma key
	KEY_ALT_RIGHT =	0x00040000,
	/// Colon key
	KEY_ALT_UP =	0x00180000,
	/// L key
	KEY_L =			0x00080000,
	/// Plus key
	KEY_PLUS =		0x00100000,
	/// Left mouse button
	KEY_LMB =       0x00200000,
	/// Right mouse button
	KEY_RMB =       0x00400000,
	/// Mouse scroll wheel (down)
	KEY_SCRL_DOWN = 0x00800000,
	/// Mouse scroll wheel (up)
	KEY_SCRL_UP =   0x01000000,
	/// Maximum key flags
	KEY_MAX =       0x02000000
};

/// @brief A table of input flags.
///
/// Contains input flags for #CSM_KEY_DESC_KEY2.
///
/// @see GetInput
/// @see CSM_KEY_DESC_KEY2
enum KEYBIND2
{
	/// 0 key
	KEY2_NUM_0  = 0x00000001,
	/// 1 key
	KEY2_NUM_1  = 0x00000002,
	/// 2 key
	KEY2_NUM_2  = 0x00000004,
	/// 3 key
	KEY2_NUM_3  = 0x00000008,
	/// 4 key
	KEY2_NUM_4  = 0x00000010,
	/// 5 key
	KEY2_NUM_5  = 0x00000020,
	/// 6 key
	KEY2_NUM_6  = 0x00000040,
	/// 7 key
	KEY2_NUM_7  = 0x00000080,
	/// 8 key
	KEY2_NUM_8  = 0x00000100,
	/// 9 key
	KEY2_NUM_9  = 0x00000200,
	/// Tab key
	KEY2_TAB    = 0x00000400,
	/// F3 key
	KEY2_F3		= 0x00000800,
	/// F4 key
	KEY2_F4		= 0x00001000,
	/// F5 key
	KEY2_F5		= 0x00002000,
	/// F6 key
	KEY2_F6		= 0x00004000,
	/// F7 key
	KEY2_F7		= 0x00008000,
	/// F8 key
	KEY2_F8		= 0x00010000,
	/// F9 key
	KEY2_F9		= 0x00020000,
	/// T key
	KEY2_T		= 0x00040000,
	/// Page up key
	KEY2_PG_UP  = 0x00080000,
	/// Page down key
	KEY2_PG_DN  = 0x00100000
};

/// @brief A table of joystick input flags.
///
/// Contains input flags for #CSM_KEY_DESC_KEY_JOY.
///
/// @see GetInput
/// @see CSM_KEY_DESC_KEY_JOY
enum KEYBIND_JOYSTICK
{
	/// Joystick "A" button
	KEY_JOY_A              = 0x01000000,
	/// Joystick "B" button
	KEY_JOY_B              = 0x02000000,
	/// Joystick "X" button
	KEY_JOY_X              = 0x04000000,
	/// Joystick "Y" button
	KEY_JOY_Y              = 0x08000000,
	/// Joystick "Left shoulder" button
	KEY_JOY_LEFT_SHOULDER  = 0x10000000,
	/// Joystick "Right shoulder" button
	KEY_JOY_RIGHT_SHOULDER = 0x20000000,
	/// Joystick "Start" button
	KEY_JOY_START          = 0x40000000,
	/// Joystick "Select" button
	KEY_JOY_SELECT         = 0x80000000,
};

/// @brief A table of query flags.
///
/// Contains flags used to get & set client information.
///
/// @see CSM_NET_CLIENT::LoadClientByIndex
/// @see CSM_NET_CLIENT::LoadClientByPointer
/// @see CSM_NET_CLIENT::LoadClientByGhostID
enum CSM_NET_CLIENT_QUERY : unsigned long long int
{
	/// Everything.
	CSM_NCQ_EVERYTHING     = 0xFFFFFFFFFFFFFFFF,

	/// [#CSM_NET_CLIENT::net_name] Get/set the net name. 
	CSM_NCQ_NAME           = 0x0000000000000001,

	/// [#CSM_NET_CLIENT::player] Get/set the player data. 
	CSM_NCQ_PLAYER         = 0x0000000000000002,

	/// [#CSM_NET_CLIENT::is_being_carried] Get/set being carried. 
	///
	/// If the client is being carried and the interface has it set to false, then the client will be dismounted.
	CSM_NCQ_BEING_CARRIED  = 0x0000000000000004,

	/// [#CSM_NET_CLIENT::is_carrying] Get/set carrying someone. 
	///
	/// If the client is carrying someone and the interface has it set to false, then the carried client will be dismounted.
	CSM_NCQ_IS_CARRYING    = 0x0000000000000008,

	/// [#CSM_NET_CLIENT::is_dead] Get/set being dead. 
	///
	/// If the client is dead and the interface has it set to false, then the client will be revived will 3 health.
	/// If the client is not dead and the interface has it set to true, then the client will be killed.
	CSM_NCQ_DEAD           = 0x0000000000000010,

	/// [#CSM_NET_CLIENT::gameFlags] Get/set the client's game flags. 
	CSM_NCQ_GAMEFLAGS      = 0x0000000000000020,

	/// [#CSM_NET_CLIENT::key] Get/set the client's input flags. 
	CSM_NCQ_KEY            = 0x0000000000000040,

	/// [#CSM_NET_CLIENT::keyTrg] Get/set the client's input flags. 
	CSM_NCQ_KEYTRG         = 0x0000000000000080,

	/// [#CSM_NET_CLIENT::currentWeapon] Get/set the client's current weapon. 
	CSM_NCQ_CURRENT_WEAPON = 0x0000000000000100,

	/// [#CSM_NET_CLIENT::carrying_ghost_id] Get/set who the client is carrying. 
	///
	/// The carried client will be set according to CSM_NET_CLIENT::carrying_ghost_id.
	CSM_NCQ_CARRYING       = 0x0000000000000200,

	/// [#CSM_NET_CLIENT::carried_by_ghost_id] Get/set who is carrying the client. 
	///
	/// The current carrier of the client will be set according to CSM_NET_CLIENT::carried_by_ghost_id.
	CSM_NCQ_CARRIER        = 0x0000000000000400,

	/// [#CSM_NET_CLIENT::spur_charge] Get/set the client's spur charge amount. 
	CSM_NCQ_SPUR_CHARGE    = 0x0000000000000800,

	/// [#CSM_NET_CLIENT::spur_max] Get/set whether the client has maximum spur charge. 
	CSM_NCQ_SPUR_MAX       = 0x0000000000001000,
};

/// Game flags for g_GameFlags
///
enum GameFlags
{
	/// Enable calling ACT (ex. ActMyChar())
	///
	/// Essentially freezes the game if not set
	GF_EnableActing = 0x0001,

	/// Enable player control
	///
	/// Does not allow the player to move whilst set. Also, disables drawing the HUD
	GF_EnableControl = 0x0002,

	/// Disable utitlies
	///
	/// Disables inventory & minimap
	GF_DisableInventory = 0x0004,

	/// Credits are running
	///
	/// Credits are running; draws movie black bars & the ending illustration.
	GF_CreditsRunning = 0x0008,
};

/// Internal cond flags for gMC (gMC.cond)
///
enum GMC_cond
{
	/// Player is hidden & disabled
	gmc_cond_disabled = 0x0002,
	/// Player is not drawn
	gmc_cond_hidden   = 0x0100
};

/// Internal flags for gMC (gMC.flag)
///
enum GMC_flag
{
	/// Player is colliding with something to the left of them
	gmc_flag_collide_left = 0x0001,

	/// Player is hitting their head on something
	gmc_flag_collide_ceiling = 0x0002,

	/// Player is colliding with something to the right of them
	gmc_flag_collide_right = 0x0004,

	/// Player is on a solid floor
	gmc_flag_collide_floor = 0x0008,

	/// Slope
	gmc_flag_collide_triangleA = 0x0002,

	/// Slope
	gmc_flag_collide_triangleB = 0x0002,

	/// Slope
	gmc_flag_collide_triangleC = 0x0002,

	/// Slope
	gmc_flag_collide_triangleD = 0x0002,

	/// Slope
	gmc_flag_collide_triangleE = 0x0028,

	/// Slope
	gmc_flag_collide_triangleF = 0x0028,

	/// Slope
	gmc_flag_collide_triangleG = 0x0018,

	/// Slope
	gmc_flag_collide_triangleH = 0x0018,

	/// Slope
	gmc_flag_on_water_tile = 0x0100,

	/// Slope
	gmc_flag_on_damage_tile = 0x0400,

	/// Slope
	gmc_flag_on_damagew_tile = 0x0D00,

	/// Wind / water current moving left
	gmc_flag_in_current_left = 0x1000,

	/// Wind / water current moving up
	gmc_flag_in_current_up = 0x2000,

	/// Wind / water current moving right
	gmc_flag_in_current_right = 0x4000,

	/// Wind / water current moving down
	gmc_flag_in_current_down = 0x8000,

	/// Any wind / water current
	gmc_flag_in_current = 0xF000
};

/// Internal equip flags for gMC (gMC.equip)
///
enum GMC_equipflags
{
	/// Booster v0.8
	gmc_eflag_booster_08     = 0x00000001,

	/// Map System
	gmc_eflag_map_system     = 0x00000002,

	/// Arms Barrier
	gmc_eflag_arms_barrier   = 0x00000004,

	/// Turbocharge
	gmc_eflag_turbocharge    = 0x00000008,

	/// Curly's Air Tank
	gmc_eflag_air_tank       = 0x00000010,

	/// Booster v2.0
	gmc_eflag_booster_2      = 0x00000020,

	/// Mimiga Mask
	gmc_eflag_mimiga_mask    = 0x00000040,

	/// Whimsical Star
	gmc_eflag_whimsical_star = 0x00000080,

	/// Mikumaru Timer
	gmc_eflag_nikumaru_timer = 0x00000100,
};

/// Internal flags for collision' sake
///
enum NPC_collision
{
	/// Colliding with something to the left of them
	npc_collide_left  = 0x0001,

	/// Colliding with something above them
	npc_collide_up    = 0x0002,

	/// Colliding with something to the right of them
	npc_collide_right = 0x0004,

	/// Colliding with something below them
	npc_collide_down  = 0x0008,
};

/// Map flags for NPCs (npc->bits)
///
enum NPC_flags
{
	/// Pushes quote out
	npc_solidSoft    = 0x000001,
	
	/// Ignores tile 44 (No NPC)
	npc_ignore44     = 0x000002,
	
	/// Can't get hit
	npc_invulnerable = 0x000004,
	
	/// Doesn't collide with anything
	npc_ignoreSolid  = 0x000008,
	
	/// Quote bounces on the top
	npc_bouncy       = 0x000010,
	
	/// Can be shot
	npc_shootable    = 0x000020,
	
	/// Essentially acts as level tiles
	npc_solidHard    = 0x000040,
	
	/// Rear and top don't hurt
	npc_rearTop      = 0x000080,
	
	/// Run event when touched (Option 1)
	npc_eventTouch   = 0x000100,
	
	/// Run event when killed
	npc_eventDie     = 0x000200,
	
	/// Only appear when flag is set
	npc_appearSet    = 0x000800,
	
	/// Spawn facing to the right (Option 2)
	npc_altDir       = 0x001000,
	
	/// Run event when interacted with
	npc_interact     = 0x002000,
	
	/// Hide when flag is set
	npc_hideSet      = 0x004000,
	
	/// Show #Damage taken
	npc_showDamage   = 0x008000,
	
	/// This NPC is an AnimNPC and should be cleaned up on death
	npc_deprecated1  = 0x010000,
	
	/// Auto focus target
	npc_autoFocus    = 0x020000,
	
	/// Don't send net updates
	npc_dontUpdate   = 0x040000,
	
	/// Build special update packets
	npc_deprecated2  = 0x080000,
	
	/// This NPC is a ghost
	npc_ghost        = 0x100000,

	/// NPC Option 3
	npc_option3      = 0x200000,

	/// Max flags
	npc_max_flags    = 0x2FFFFF,

	// Helper enums
	/// NPC Option 1
	npc_option1      = 0x000100,

	/// NPC Option 2
	npc_option2      = 0x001000
};

/// Internal flags for NPCs (npc->flag)
///
enum NPC_int_flag
{
	/// Collide left
	npc_flag_collide_left      = 0x0001,
	/// Collide up
	npc_flag_collide_ceiling   = 0x0002,
	/// Collide right
	npc_flag_collide_right     = 0x0004,
	/// Collide floor
	npc_flag_collide_floor     = 0x0008,
	/// Collide wall
	npc_flag_collide_wall      = 0x0005,
	/// Collide slope
	npc_flag_collide_triangleA = 0x0002,
	/// Collide slope
	npc_flag_collide_triangleB = 0x0002,
	/// Collide slope
	npc_flag_collide_triangleC = 0x0002,
	/// Collide slope
	npc_flag_collide_triangleD = 0x0002,
	/// Collide slope
	npc_flag_collide_triangleE = 0x0028,
	/// Collide slope
	npc_flag_collide_triangleF = 0x0028,
	/// Collide slope
	npc_flag_collide_triangleG = 0x0018,
	/// Collide slope
	npc_flag_collide_triangleH = 0x0018,
	/// In water
	npc_flag_on_water_tile     = 0x0100
};

/// Internal cond flags for NPCs (npc->cond)
///
enum NPC_cond
{
	/// NPC is not disabled
	npc_cond_npc_alive = 0x0080
};

/// List of surface IDs
///
typedef enum Surface_Ids
{
	/// Title.png
	SURFACE_ID_TITLE = 0,

	/// Pixel logo
	SURFACE_ID_PIXEL = 1,

	/// Current level tileset
	SURFACE_ID_LEVEL_TILESET = 2,

	/// Fade surface
	SURFACE_ID_FADE = 6,

	/// ItemImage.png
	SURFACE_ID_ITEM_IMAGE = 8,

	/// Map System Surface
	SURFACE_ID_MAP = 9,

	/// Last screen grab
	SURFACE_ID_SCREEN_GRAB = 10,

	/// Arms.png
	SURFACE_ID_ARMS = 11,

	/// ArmsImage.png
	SURFACE_ID_ARMS_IMAGE = 12,

	/// Room Name Surface
	SURFACE_ID_ROOM_NAME = 13,

	/// StageImage.png
	SURFACE_ID_STAGE_ITEM = 14,

	/// Loading.png
	SURFACE_ID_LOADING = 15,

	/// MyChar.png <i color="color:#990000;">(deprecated)</i>
	SURFACE_ID_MY_CHAR = 16,

	/// Bullet.png
	SURFACE_ID_BULLET = 17,

	/// Caret.png
	SURFACE_ID_CARET = 19,

	/// Npc/NpcSym.png
	SURFACE_ID_NPC_SYM = 20,

	/// Current level spritesheet (Npc)
	SURFACE_ID_LEVEL_SPRITESET_1 = 21,

	/// Current level spritesheet (Boss)
	SURFACE_ID_LEVEL_SPRITESET_2 = 22,

	/// Npc/NpcRegu.png
	SURFACE_ID_NPC_REGU = 23,

	/// Textbox stuff
	SURFACE_ID_TEXT_BOX = 26,

	/// Face.png
	SURFACE_ID_FACE = 27,

	/// Current level background
	SURFACE_ID_LEVEL_BACKGROUND = 28,

	/// ValueView
	SURFACE_ID_VALUE_VIEW = 29,

	/// TextLine1
	SURFACE_ID_TEXT_LINE1 = 30,

	/// TextLine2
	SURFACE_ID_TEXT_LINE2 = 31,

	/// TextLine3
	SURFACE_ID_TEXT_LINE3 = 32,

	/// TextLine4
	SURFACE_ID_TEXT_LINE4 = 33,

	/// TextLine5
	SURFACE_ID_TEXT_LINE5 = 34,

	/// ?
	SURFACE_ID_CREDIT_CAST = 35,

	/// Currently loaded credits image
	SURFACE_ID_CREDITS_IMAGE = 36,

	/// casts.png
	SURFACE_ID_CASTS = 37,

	/// Screengrab 2
	SURFACE_ID_SCREEN_GRAB2 = 38,

	/// GUI
	SURFACE_ID_GUI = 39,

	/// Character's face color mask <i color="color:#990000;">(deprecated)</i>
	SURFACE_ID_FACE_COLOR_MASK = 41,

	/// ChatTopics.png
	SURFACE_ID_CHAT_TOPICS = 42,

	/// Controls.png
	SURFACE_ID_CONTROLS = 43,

	/// Net name surface
	SURFACE_ID_NET_NAME_PLANE = 44,

	/// DemoEditor.png
	SURFACE_ID_DEMO_EDITOR = 45,

	/// HelpImages.png
	SURFACE_ID_HELP_IMAGES = 46,
		
	/// TeamSpawnDoors.png
	SURFACE_ID_TEAM_SPAWN_DOORS = 47,

	/// Used for <IMG and <IMF
	SURFACE_ID_SCREEN_IMAGE = 48,

	/// Screen grab 3
	SURFACE_ID_SCREEN_GRAB3 = 49,

	/// Number of static surfaces
	SURFACE_ID_MAX_UNDYNAMIC = 49,

	/// Start of allocatable surface indexing
	SURFACE_ID_ALLOCATABLE_SURFACE_START = 50, // These surfaces can be used for anything. Just make sure to initialize it with MakeGeneric_Generic first.

	/// End of allocatable surface indexing
	SURFACE_ID_ALLOCATABLE_SURFACE_END = 850, // These surfaces can be used for anything. Just make sure to initialize it with MakeGeneric_Generic first.

	/// Used for drawing directly to the screen with CacheSurface
	SURFACE_ID_RENDERER = 851,

	/// Used for drawing directly to the renderer's hardware texture
	SURFACE_ID_RENDERER_TEXTURE = 852,

	/// Debug charset
	SURFACE_ID_DEBUGCHARSET,

	/// Number of surfaces
	SURFACE_ID_MAX
} Surface_Ids;

/// Tiletype flags. Useful for checking a map tile's attributes.
///
/// @see CompareTileTypeXY
/// @see CompareTileTypeNPC
/// @see CompareTileType
/// @see GetTileFlagsXY
/// @see GetTileFlagsNPC
enum TileType
{
	/// Tile is drawn after NPCs and the player.
	TT_FOREGROUND    = 0x00000001,

	/// Tile is drawn before NPCs and the player.
	TT_BACKGROUND    = 0x00000002,

	/// Water tile.
	TT_WATER         = 0x00000004,

	/// Tile damages the player.
	TT_DAMAGE        = 0x00000008,

	/// Tile is destroyable.
	TT_DESTROYABLE   = 0x00000010,

	/// Tile does not allow NPCs inside of it.
	TT_NO_NPC        = 0x00000020,

	/// Tile does not allow the player inside of it.
	TT_NO_PLAYER     = 0x00000040,

	/// Tile is shootable.
	TT_SHOOTABLE     = 0X00000080,

	/// Tile is completely solid.
	TT_SOLID         = (TT_NO_NPC | TT_NO_PLAYER),

	/// Tile is visible.
	TT_VISIBLE       = (TT_FOREGROUND | TT_BACKGROUND),

	/// TIle is a right-facing large ceiling slope.
	TT_SLOPE_CEIL_0  = 0x00000100,

	/// TIle is a right-facing small ceiling slope.
	TT_SLOPE_CEIL_1  = 0x00000200,

	/// TIle is a left-facing large ceiling slope.
	TT_SLOPE_CEIL_2  = 0x00000400,

	/// TIle is a left-facing small ceiling slope.
	TT_SLOPE_CEIL_3  = 0x00000800,

	/// Tile is a right-facing large floor slope.
	TT_SLOPE_FLOOR_0 = 0x00001000,

	/// Tile is a right-facing small floor slope.
	TT_SLOPE_FLOOR_1 = 0x00002000,

	/// Tile is a left-facing large floor slope.
	TT_SLOPE_FLOOR_2 = 0x00004000,

	/// Tile is a left-facing small floor slope.
	TT_SLOPE_FLOOR_3 = 0x00008000,

	/// Tile is a slope.
	TT_ANY_SLOPE     = (TT_SLOPE_CEIL_0 | TT_SLOPE_CEIL_1 | TT_SLOPE_CEIL_2 | TT_SLOPE_CEIL_3 | TT_SLOPE_FLOOR_0 | TT_SLOPE_FLOOR_1 | TT_SLOPE_FLOOR_2 | TT_SLOPE_FLOOR_3),
	
	/// Tile pushes the player left.
	TT_MOVE_LEFT     = 0x00010000,

	/// Tile pushes the player right.
	TT_MOVE_RIGHT    = 0x00020000,

	/// Tile pushes the player up.
	TT_MOVE_UP       = 0x00040000,

	/// Tile pushes the player down.
	TT_MOVE_DOWN     = 0x00080000,

	/// Tile is solid for players not on the yellow team.
	TT_YELLOW_TEAM   = 0x00100000,

	/// Tile is solid for players not on the green team.
	TT_GREEN_TEAM    = 0x00200000,

	/// Tile is solid for players not on the red team.
	TT_RED_TEAM      = 0x00400000,

	/// Tile is solid for players not on the blue team.
	TT_BLUE_TEAM     = 0x00800000,

	/// Tile moves the player.
	TT_ANY_MOVE      = (TT_MOVE_LEFT | TT_MOVE_RIGHT | TT_MOVE_UP | TT_MOVE_DOWN),

	/// Tile is team-locked.
	TT_ANY_TEAM      = (TT_YELLOW_TEAM | TT_GREEN_TEAM | TT_RED_TEAM | TT_BLUE_TEAM)
};

/// Controls what the game is currently doing.
///
/// @see BaseModeInstance
enum GameProcessMode
{
	/// Continue the game from a mode.
	GPM_CONTINUE         = -1,

	/// Closing the game.
	GPM_CLOSE_GAME       = 0,

	/// Showing the opening sequence.
	GPM_OPENING_SEQUENCE = 1,

	/// Showing the title screen.
	GPM_TITLE_SCREEN     = 2,

	/// This is where all the singleplayer gameplay happens.
	///
	/// Switching to this mode automatically loads a game.
	GPM_ACTION           = 3,

	/// This is where all the multiplayer gameplay happens.
	///
	/// If the game is not connected to a server, then this mode will immediately display a 'disconnected' error.
	GPM_NET_ACITON       = 9,

	/// Showing the current net lobby we're in.
	///
	/// If the game is not connected to a server, then this mode will immediately display a 'disconnected' error.
	GPM_NET_LOBBY        = 10,

	/// Showing the 'CONNECTING...' screen.
	///
	/// If the game is not connected to a server, then this mode will immediately display a 'disconnected' error.
	GPM_NET_CONNECTING   = 11,

	/// Showing the download screen.
	///
	/// If the game is not connected to a server, then this mode will immediately display a 'disconnected' error.
	GPM_NET_MANIFEST     = 12
};

/// Determine which client to use.
///
/// @see SafeClientInterface::Set
enum SCI_SetType
{
	/// Automatically determine which client type to use.
	SCI_ST_AUTO          = 0b00011,

	/// ONLY use server-sided clients.
	///
	/// If the client parameter is set to a client-sided ghost, then it will attempt to find the server-sided client matching the ghost's client id.
	/// If the client parameter is NULL, it will use the local variables that make up singleplayer CaveStory (gMC, gKey, gKeyTrg, etc.)
	/// If the client parameter is NULL AND the set flags specify SCI_ST_ONLY_CLIENTS, then it will attempt to use the local player's server-sided client.
	SCI_ST_SERVER        = 0b00001,

	/// ONLY use client-sided clients.
	///
	/// If the client parameter is set to a server-sided client, then it will attempt to find the client-sided ghost matching the client's ghost id.
	/// If the client parameter is NULL, it will use the local variables that make up singleplayer CaveStory (gMC, gKey, gKeyTrg, etc.)
	/// If the client parameter is NULL AND the set flags specify SCI_ST_ONLY_CLIENTS, then it will attempt to use the local player's client-sided client.
	SCI_ST_CLIENT        = 0b00010,

	/// ONLY use client objects. Do NOT use local player variables.
	SCI_ST_ONLY_CLIENTS  = 0b00100,

	/// Instead of determining the best suitable candidate, just use the inputted client parameter.
	/// Ignores all other flags that determine type of client
	SCI_ST_EXACT         = 0b01000,

	/// Ensure that a MYCHAR is assigned, no matter what.
	SCI_ST_ENSURE_PLAYER = 0b10000
};

/// The TextScript's target.
///
enum TextScriptTarget
{
	/// Target everyone in the server.
	TS_TARGET_EVERYONE      = 0,

	/// Target the event executor.
	TS_TARGET_EXECUTOR      = 1,

	/// Target the event executor's team. If they are not in one, then target them directly.
	TS_TARGET_EXECUTOR_TEAM = 2,
	
	/// Target singleplayer components.
	TS_TARGET_SINGLEPLAYER  = 3,

	/// How many targets there can be
	TS_TARGETCOUNT          = 3	
};

/// Player state flags
///
enum PLAYER_STATE_FLAGS : unsigned long long int
{
	/// If set, then this state will not process air.
	PSF_NO_AIR        = BIT(0),

	/// If set, then players cannot use weapons whilst in this state.
	PSF_NO_WEAPONS    = BIT(1),

	/// If set, then the camera will not follow the player.
	PSF_NO_CAMERA     = BIT(2),

	/// If set, then this player cannot be spectated.
	PSF_NO_SPECTATE   = BIT(3),

	/// If set, then this player's hud cannot be drawn.
	PSF_NO_HUD        = BIT(4),

	/// If set, then this player cannot pickup drops.
	PSF_NO_PICKUPS    = BIT(5),

	/// If set, the camera will be centered onto the player.
	PSF_CENTER_CAMERA = BIT(6),

	/// If set, then the act function will be called immediately after the state is set.
	PSF_INIT          = BIT(7),

	/// If set, then the act function will be called before the player's state is changed.
	PSF_FREE          = BIT(8),

	/// If set, then this player will not be affected by collision.
	PSF_NOCLIP        = BIT(9),

	/// If set, then this player cannot be focused by NPCs or bosses.
	PSF_NOTARGET      = BIT(10),

	/// If set, then this player is invulnerable.
	PSF_GOD           = BIT(11),

	/// If set, then this state is a 'MYCHAR->unit' state.
	PSF_UNIT          = BIT(12),

	/// If set, then the bubble from the Ironhead boss will be drawn around the player.
	PSF_PUT_BUBBLE    = BIT(13),
};

/// Playerstate transmit flags
///
/// Please keep the array in MC_Actions.cpp up to date with this one!!
enum PLAYER_STATE_TRANSMIT_FLAGS
{
	/// Transmit 'count1' field.
	PSTF_COUNT1     = BIT(0),

	/// Transmit 'count2' field.
	PSTF_COUNT2     = BIT(1),

	/// Transmit 'count3' field.
	PSTF_COUNT3     = BIT(2),

	/// Transmit 'count4' field.
	PSTF_COUNT4     = BIT(3),

	/// Transmit 'act_no' field.
	PSTF_ACT_NO     = BIT(4),

	/// Transmit 'act_wait' field.
	PSTF_ACT_WAIT   = BIT(5),

	/// Transmit 'pNpc' field.
	PSTF_NPC_PTR    = BIT(6),

	/// Transmit 'pOther' and 'pOtherClient' fields.
	PSTF_MYCHAR_PTR = BIT(7),

	/// Transmit 'obj.x' field.
	PSTF_OBJ_X      = BIT(8),

	/// Transmit 'obj.y' field.
	PSTF_OBJ_Y      = BIT(9),

	/// Transmit 'obj.xm' field.
	PSTF_OBJ_XM     = BIT(10),

	/// Transmit 'obj.ym' field.
	PSTF_OBJ_YM     = BIT(11),

	/// Transmit 'obj.tgt_x' field.
	PSTF_OBJ_TGT_X  = BIT(12),

	/// Transmit 'obj.tgt_y' field.
	PSTF_OBJ_TGT_Y  = BIT(13),

	/// The amount of flags there are
	PLAYER_TRANSMIT_FLAG_COUNT = 14
};

/// All possible cursor types for BaseModeInstance::SetCursor
enum CSMP_CURSOR_TYPE
{
	// Default green arrow
	CSMP_CURSOR_NORMAL,
	// Normal system arrow
	CSMP_CURSOR_ARROW,
	// System I-beam (for text editing)
	CSMP_CURSOR_IBEAM,
	// System Wait (Hourglass on XP systems, loading circle on others)
	CSMP_CURSOR_WAIT,
	// System Crosshair
	CSMP_CURSOR_CROSSHAIR,
	// System wait arrow
	CSMP_CURSOR_WAITARROW,
	// System double arrow pointing northwest and southeast (\)
	CSMP_CURSOR_SIZENWSE,
	// System double arrow pointing northeast and southwest (/)
	CSMP_CURSOR_SIZENESW,
	// System double arrow pointing west and east (-)
	CSMP_CURSOR_SIZEWE,
	// System double arrow pointing north and south (|)
	CSMP_CURSOR_SIZENS,
	// System four pointed arrow pointing north, south, east and west
	CSMP_CURSOR_SIZEALL,
	// Slashed circle / crossbones
	CSMP_CURSOR_NO,
	// System Hand
	CSMP_CURSOR_HAND,

	// Number of cursors
	CSMP_NUM_CURSORS
};

/// A list of output types for a player state's "ANIMATE" function.
///
enum PlayerStateAnimator_OutputType
{
	// 'Animate' function will return a 'PlayerAnimFrame::*'.
	PSAOT_PLAYER_FRAME     = 0,
	// 'Animate' function will return a 'PlayerAnimation::*'.
	PSAOT_PLAYER_ANIMATION = 1,
	// 'Animate' function will return a zero; All character animation will be handled by the state.
	PSAOT_CUSTOM           = 2
};

/// A list of possible player death types.
///
enum PlayerDeathType
{
	// Default, generic death type. Announces "<name> died!".
	DEATHTYPE_UNKNOWN = 0,

	// Player killed themselves. Announces "<name> killed themselves!"
	DEATHTYPE_SUICIDE = 1,

	// Player died by another player. Announces "<playername> was killed by <killername>!"
	DEATHTYPE_KILLED_BY_PLAYER = 2,

	// Player was killed by an NPC. Announces "<name> was killed by <NPC_NAME>!"
	DEATHTYPE_KILLED_BY_NPC = 3,

	// Player fell out of bounds of the map. Announces "<name> fell to a clusmy, painful death."
	DEATHTYPE_OUT_OF_BOUNDS = 4,
	
	// Player drowned. Announces "<name> drowned!"
	DEATHTYPE_DROWNED = 5,

	// The number of death types.
	DEATHTYPE_COUNT
};

/// A list of position types for #CSM_Npc_GetPosition .
///
enum NpcPositionType
{
	// The draw position of the sprite, converted to magnified screen coordinates.
	NPT_DRAW_POSITION = 0,

	// The top left position of the sprite in subpixels. This position is not relative to the camera frame position.
	NPT_SUB_DRAW_POSITION = 1
};

/// Internal update nodes for NPCs (npc->update_nodes)
///
enum NPC_update_nodes : unsigned long long int
{
	npc_nodes_damage     = 0x0000000000000001, //   int: +4
	npc_nodes_flag       = 0x0000000000000002, //   int: +4
	npc_nodes_x          = 0x0000000000000004, //   int: +4
	npc_nodes_y          = 0x0000000000000008, //   int: +4
	npc_nodes_xm         = 0x0000000000000010, //   int: +4
	npc_nodes_ym         = 0x0000000000000020, //   int: +4
	npc_nodes_life       = 0x0000000000000040, //   int: +4
	npc_nodes_exp        = 0x0000000000000080, //   int: +4
	npc_nodes_size       = 0x0000000000000100, //   int: +4
	npc_nodes_direct     = 0x0000000000000200, //   int: +4
	npc_nodes_code_event = 0x0000000000000400, //   int: +4
	npc_nodes_surf       = 0x0000000000000800, //   int: +4
	npc_nodes_act_no     = 0x0000000000001000, //   int: +4
	npc_nodes_act_wait   = 0x0000000000002000, //   int: +4
	npc_nodes_ani_no     = 0x0000000000004000, //   int: +4
	npc_nodes_ani_wait   = 0x0000000000008000, //   int: +4
	npc_nodes_bits       = 0x0000000000010000, //  uint: +4
	npc_nodes_count1     = 0x0000000000020000, //   int: +4
	npc_nodes_count2     = 0x0000000000040000, //   int: +4
	npc_nodes_count3     = 0x0000000000080000, //   int: +4
	npc_nodes_count4     = 0x0000000000100000, //   int: +4
	npc_nodes_shock      = 0x0000000000200000, // uchar: +1
	npc_nodes_pNpc       = 0x0000000000400000, //   int: +4
	npc_nodes_focus      = 0x0000000000800000, //   int: +4
	npc_nodes_code_flag  = 0x0000000001000000, //   int: +4
	npc_nodes_focus2     = 0x0000000002000000, //   int: +4
	npc_nodes_tgt_x      = 0x0000000004000000, //   int: +4
	npc_nodes_tgt_y      = 0x0000000008000000, //   int: +4

	NPC_NODES_ALL           = 0x000000000FFFFFFF,
	NPC_NODES_COUNT         = 28,
	NPC_NODES_MAX_BYTE_SIZE = 109
};

/// Internal transmit nodes for NPCs (npc->transmit_nodes)
///
enum NPC_transmit_nodes : unsigned long long int
{
	npc_transmit_nodes_flag          = 0x0000000000000001, //   int: +4
	npc_transmit_nodes_x             = 0x0000000000000002, //   int: +4
	npc_transmit_nodes_y             = 0x0000000000000004, //   int: +4
	npc_transmit_nodes_xm            = 0x0000000000000008, //   int: +4
	npc_transmit_nodes_ym            = 0x0000000000000010, //   int: +4
	npc_transmit_nodes_code_flag     = 0x0000000000000020, //   int: +4
	npc_transmit_nodes_code_event    = 0x0000000000000040, //   int: +4
	npc_transmit_nodes_surf          = 0x0000000000000080, //   int: +4
	npc_transmit_nodes_hit_voice     = 0x0000000000000100, //   int: +4
	npc_transmit_nodes_destroy_voice = 0x0000000000000200, //   int: +4
	npc_transmit_nodes_life          = 0x0000000000000400, // short: +2
	npc_transmit_nodes_exp           = 0x0000000000000800, //   int: +4
	npc_transmit_nodes_direct        = 0x0000000000001000, // uchar: +1
	npc_transmit_nodes_bits          = 0x0000000000002000, //  uint: +4
	npc_transmit_nodes_ani_wait      = 0x0000000000004000, //   int: +4
	npc_transmit_nodes_ani_no        = 0x0000000000008000, //   int: +4
	npc_transmit_nodes_count1        = 0x0000000000010000, //   int: +4
	npc_transmit_nodes_count2        = 0x0000000000020000, //   int: +4
	npc_transmit_nodes_count3        = 0x0000000000040000, //   int: +4
	npc_transmit_nodes_count4        = 0x0000000000080000, //   int: +4
	npc_transmit_nodes_act_no        = 0x0000000000100000, //   int: +4
	npc_transmit_nodes_act_wait      = 0x0000000000200000, //   int: +4
	npc_transmit_nodes_shock         = 0x0000000000400000, // uchar: +1
	npc_transmit_nodes_damage        = 0x0000000000800000, //   int: +4
	npc_transmit_nodes_pNpc          = 0x0000000001000000, //   int: +4
	npc_transmit_nodes_focus         = 0x0000000002000000, // netid: +...
	npc_transmit_nodes_focus2        = 0x0000000004000000, // netid: +...
	npc_transmit_nodes_xm2           = 0x0000000008000000, //   int: +4
	npc_transmit_nodes_ym2           = 0x0000000010000000, //   int: +4
	npc_transmit_nodes_zone          = 0x0000000020000000, //   int: +4
	npc_transmit_nodes_rect_left     = 0x0000000040000000, //   int: +4
	npc_transmit_nodes_rect_top      = 0x0000000080000000, //   int: +4
	npc_transmit_nodes_rect_right    = 0x0000000100000000, //   int: +4
	npc_transmit_nodes_rect_bottom   = 0x0000000200000000, //   int: +4
	npc_transmit_nodes_tgt_x         = 0x0000000400000000, //   int: +4
	npc_transmit_nodes_tgt_y         = 0x0000000800000000, //   int: +4

	NPC_TRANSMIT_NODES_ALL           = 0x0000000FFFFFFFFF,
	NPC_TRANSMIT_NODES_COUNT         = 36,
	NPC_TRANSMIT_NODES_MAX_BYTE_SIZE = 130 + (sizeof(NetClientId) * 2)
};

/// Boss types.
///
enum CSM_BossTypes
{
	/// Do not use
	CSM_BOSS_NONE = 0,

	/// Omega boss
	CSM_BOSS_OMGEA = 1,

	/// Balfrog boss
	CSM_BOSS_BALFROG = 2,

	/// The top-left green dot (stage 1)
	CSM_BOSS_MOSTER_X_TOP_LEFT = 3,

	/// The top-right green dot (stage 1)
	CSM_BOSS_MOSTER_X_TOP_RIGHT = 4,

	/// The bottom-left green dot (stage 1)
	CSM_BOSS_MOSTER_X_BOTTOM_LEFT = 5,

	/// The bottom-right green dot (stage 1)
	CSM_BOSS_MOSTER_X_BOTTOM_RIGHT = 6,

	/// Monster X boss (stage 2)
	CSM_BOSS_MOSTER_X = 7,

	/// Island Core boss
	CSM_BOSS_CORE = 8,

	/// Ironhead boss
	CSM_BOSS_IRONHEAD = 9,

	/// The twin dragons (This boss has a shared health pool)
	CSM_BOSS_TWINS = 10,

	/// Undead island core boss
	CSM_BOSS_UNDEAD_CORE = 11,

	/// Heavy press boss
	CSM_BOSS_HEAVY_PRESS = 12,

	/// Ballos (Stage w/ bouncing ballos)
	CSM_BOSS_BALLOS_BOUNCING = 13,

	/// Ballos (Stage w/ Platforms)
	CSM_BOSS_BALLOS_PLATFORMS = 14,

	/// The number of boss types.
	CSM_BOSSTYPE_COUNT
};