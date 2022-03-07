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
  @file ModConfigResource.h
*/

#if defined(_CAVESTORY_MULTIPLAYER) || !defined(_CAVEEDITOR)
#include <CSMAPI_begincode.h>
#endif

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* g_PXMOD_MUSIC_TYPE_EXTENSION_LIST[] = { "", "org", "ptcop", "wav", "mp3", "ogg" };

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define PXMOD_HEADER_STRING		"PXMOD"
#define PXMOD_VERSION			16

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum PXMOD_BUILD_TYPE : unsigned char
{
	PXMOD_BUILD_DEBUG   = 0,
	PXMOD_BUILD_RELEASE = 1,

	PXMOD_BUILD_COUNT
};

enum PXMOD_DIRECTIONS : unsigned char
{
	PXMOD_DIRECT_LEFT       = 0,
	PXMOD_DIRECT_UP         = 1,
	PXMOD_DIRECT_RIGHT      = 2,
	PXMOD_DIRECT_DOWN       = 3,
	PXMOD_DIRECT_CENTER     = 4,
	PXMOD_DIRECT_RIGHT_UP   = 5,
	PXMOD_DIRECT_DOWN_RIGHT = 6,

	// This should always be at the bottom of this enum.
	PXMOD_DIRECT_COUNT
};

enum PXMOD_BK_TYPE : unsigned char
{
	PXMOD_BK_FIXED             = 0,
	PXMOD_BK_MOVE_SLOW         = 1,
	PXMOD_BK_FOLLOW_FOREGROUND = 2,
	PXMOD_BK_HIDE              = 3,
	PXMOD_BK_SCROLL_FAST       = 5,
	PXMOD_BK_SCROLL_GRAVITY    = 6,
	PXMOD_BK_SCROLL_LAYERED    = 7,

	PXMOD_BK_COUNT             = 7
};

enum PXMOD_BOSS_TYPE : unsigned char
{
	PXMOD_BOSS_NONE           = 0,
	PXMOD_BOSS_OMEGA          = 1,
	PXMOD_BOSS_BALFROG        = 2,
	PXMOD_BOSS_WEAPON_X       = 3,
	PXMOD_BOSS_CORE           = 4,
	PXMOD_BOSS_IRONHEAD       = 5,
	PXMOD_BOSS_DRAGON_SISTERS = 6,
	PXMOD_BOSS_UNDEAD_CORE    = 7,
	PXMOD_BOSS_HEAVY_PRESS    = 8,
	PXMOD_BOSS_BALLOS         = 9,

	PXMOD_BOSS_COUNT          = 9
};

enum PXMOD_BULLET_FLAGS : unsigned short
{
	PXMOD_BULLET_FLAG_IGNORE_SOLID          = 0x004,
	PXMOD_BULLET_FLAG_DONT_DESTROY_SOLID    = 0x008,
	PXMOD_BULLET_FLAG_PROJECTILE            = 0x010,
	PXMOD_BULLET_FLAG_BREAK_BLOCKS          = 0x020,
	PXMOD_BULLET_FLAG_PIERCE_BLOCKS         = 0x040
};

enum PXMOD_MUSIC_TYPE : unsigned char
{
	PXMOD_MUSIC_TYPE_INVALID = 0,
	PXMOD_MUSIC_TYPE_ORGANYA = 1,
	PXMOD_MUSIC_TYPE_PTCOP   = 2,
	PXMOD_MUSIC_TYPE_WAV     = 3,
	PXMOD_MUSIC_TYPE_MP3     = 4,
	PXMOD_MUSIC_TYPE_OGG     = 5,

	PXMOD_MUSIC_TYPE_COUNT   = 6
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct PXMOD_BACKGROUND_STRUCT
{
	char name[16];
};

struct PXMOD_SPRITESHEET_STRUCT
{
	char name[16];
};

struct PXMOD_TILESET_STRUCT
{
	char name[16];
};

struct PXMOD_STAGE_STRUCT
{
	/// Map file name
	char file[32];

	/// Tileset
	int tileset;

	/// Background type
	PXMOD_BK_TYPE bkType;

	/// Background
	int background;

	/// NPC Spritesheet file
	int npc_sheet;

	/// Boss Spritesheet file
	int boss_sheet;
	
	/// Area index
	int area_index;

	/// Boss number
	PXMOD_BOSS_TYPE boss_no;

	/// Map name
	char name[34];

	/// Force camera to lock onto the horizontal center of this stage
	bool focus_center_x;

	/// Force camera to lock onto the vertical center of this stage
	bool focus_center_y;
};

struct PXMOD_VERSION_STRUCT
{
	unsigned char data[4];

	inline int operator[](int idx) { return (int)data[idx % sizeof(data)]; }

	PXMOD_VERSION_STRUCT()
	{
		data[0] = 1;
		data[1] = 0;
		data[2] = 0;
		data[3] = 0;
	}

	PXMOD_VERSION_STRUCT(int season, int major, int minor, int patch)
	{
		data[0] = season;
		data[1] = major;
		data[2] = minor;
		data[3] = patch;
	}
};

struct PXMOD_AREA_STRUCT
{
	/// Area name
	char name[32];

	/// Which stage index will be shown in the background of the lobby
	int stage_idx;

	/// If true, then the server will NOT send any of the maps under this area to clients.
	bool singleplayer_only;

	/// Whether this area will be kept when compiling mod for release
	bool keep_on_release;
};

struct PXMOD_MUSIC_STRUCT
{
	/// File name
	char name[16];

	/// The type of music
	PXMOD_MUSIC_TYPE type;
};

struct PXMOD_BULLET_STRUCT
{
	/// Flags for this bullet
	unsigned short flags;

	/// The damage this bullet deals
	int damage;

	/// How many times this bullet can hit something before it despawns
	int hit_life;

	/// How far this bullet can go before it despawns, in ticks
	int range;

	/// The size of this bullet's bounding box
	struct
	{
		int w;
		int h;
	} bullet_size;

	/// The size of this bullet's collision
	struct
	{
		int w;
		int h;
	} collision_size;

	/// The sprite location for this bullet
	struct
	{
		int left;
		int right;
		int top;
		int bottom;
	} sprite;

	/// The name of this bullet. This is not saved in the release build.
	char name[64];
};

struct PXMOD_WEAPON_STRUCT
{
	/// Bullet info for every level
	struct WEAPON_LEVEL_INFO
	{
		/// The index of the bullet to use for this gun
		int bullet_index;

		/// How much EXP is required to go to the next level
		int exp;

		/// The gun's view
		struct
		{
			int left;
			int top;
			int right;
			int bottom;
		} view;
	} level_info[3];

	struct WEAPON_DISPLAY_INFO
	{
		int left;
		int top;
		int right;
		int bottom;

		int hold_offset_x;
		int hold_offset_y;
	} display_info;

	/// Name of the weapon
	char name[64];
};

struct PXMOD_NPC_ACT_STRUCT
{
	char name[64];

	int act_no;
	unsigned long long int nodes;
	unsigned long long int transmit_nodes;
};

struct PXMOD_NPC_STRUCT
{
	char name[64];

	unsigned long long int nodes;
	unsigned long long int transmit_nodes;
	unsigned int bits;
	unsigned short life;
	unsigned char surf;
	unsigned char hit_voice;
	unsigned char destroy_voice;
	unsigned char size;
	long exp;
	long damage;

	struct
	{
		unsigned char front;
		unsigned char back;
		unsigned char top;
		unsigned char bottom;
	} hit;

	struct
	{
		unsigned char front;
		unsigned char back;
		unsigned char top;
		unsigned char bottom;
	} view;

	struct
	{
		int left;
		int top;
		int right;
		int bottom;
	} sprite;

	int spritesheet_index;

	bool useNewColMethod;
	bool canBreakTile47;

	char category[64];
	char option1[256];
	char option2[256];
	char option3[256];
	char shortname1[8];
	char shortname2[8];

	struct
	{
		PXMOD_NPC_ACT_STRUCT* list;
		unsigned int count;
		unsigned int highest_act_no;
	} acts;
};

struct PXMOD_ITEM_STRUCT
{
	char name[64];
	char desc[2048];
	int sprite_index;
	char set_flag_count;
	int set_flag_list[8];
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct PXMOD_HEADER_STRUCT
{
	char title[128];
	char author[128];
	PXMOD_VERSION_STRUCT version;
	PXMOD_BUILD_TYPE build_type;

	struct
	{
		bool allow_carrying_players;
		bool allow_agility;
		bool allow_fishing;
	} gameplay;

	struct
	{
		bool draw_hp_bar;
		bool draw_hp;
		bool draw_weapons;
		bool draw_ammo;
		bool draw_exp_bar;
		bool draw_level;
	} hud;

	struct
	{
		bool enabled;
		unsigned short surface_id;
		struct
		{
			int left;
			int top;
			int right;
			int bottom;
		} sprite_rect;
	} collectables;

	struct
	{
		unsigned short drown_core_flag;
		unsigned short drown_core;
		unsigned short everyone_died;
		unsigned short drown;
		unsigned short death;
	} events;

	struct
	{
		unsigned int title;
		unsigned int netmenu;
	} music;
};

struct PXMOD_START_CHUNK
{
	unsigned int map;
	unsigned short x;
	unsigned short y;
	unsigned short event;
	unsigned char hp;
	unsigned char max_hp;
	PXMOD_DIRECTIONS direct;
	unsigned int flags;
};

struct PXMOD_TITLE_CHUNK
{
	unsigned int map;
	unsigned short event;
	unsigned short x;
	unsigned short y;
};

struct PXMOD_BACKGROUND_CHUNK
{
	PXMOD_BACKGROUND_STRUCT* list;
	unsigned int count;
};

struct PXMOD_SPRITESHEET_CHUNK
{
	PXMOD_SPRITESHEET_STRUCT* list;
	unsigned int count;
};

struct PXMOD_TILESET_CHUNK
{
	PXMOD_TILESET_STRUCT* list;
	unsigned int count;
};

struct PXMOD_STAGE_CHUNK
{
	PXMOD_STAGE_STRUCT* list;
	unsigned short count;
};

struct PXMOD_AREA_CHUNK
{
	PXMOD_AREA_STRUCT* list;
	unsigned int count;
};

struct PXMOD_MUSIC_CHUNK
{
	PXMOD_MUSIC_STRUCT* list;
	unsigned int count;
};

struct PXMOD_BULLET_CHUNK
{
	PXMOD_BULLET_STRUCT* list;
	unsigned short count;
};

struct PXMOD_WEAPON_CHUNK
{
	PXMOD_WEAPON_STRUCT* list;
	unsigned int count;
};

struct PXMOD_NPC_CHUNK
{
	PXMOD_NPC_STRUCT* list;
	unsigned int count;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief An interface for the 'PXMOD' file format.
///
/// Stores all assets for a mod.
#if defined(_CAVESTORY_MULTIPLAYER) || defined(CAVESTORYMULTIPLAYERBASEDLL_EXPORTS)
class CAVESTORY_MOD_API ModConfigResource
#else
class ModConfigResource
#endif
{
public: // Public map info

	/// The path of the mod -- This has no trailing slash, and should only contain forward slashes! ex: "H:/CaveStory/MyMod"
	///
	char mModPath[260];

	/// The header for this mod.
	///
	PXMOD_HEADER_STRUCT mHeader;

	/// Game start chunk.
	///
	PXMOD_START_CHUNK mStart;

	/// Titlescreen intro chunk.
	///
	PXMOD_TITLE_CHUNK mTitle;

	/// Stores all the backgrounds.
	///
	PXMOD_BACKGROUND_CHUNK mBackgrounds;

	/// Stores all the spritesheets.
	///
	PXMOD_SPRITESHEET_CHUNK mSpritesheets;

	/// Stores all the tilesets.
	///
	PXMOD_TILESET_CHUNK mTilesets;

	/// Stores all the stage information.
	///
	PXMOD_STAGE_CHUNK mStages;

	/// Stores all the map area information. If a map is in an area and the player saves in it & starts a netlobby with that save file selected,
	/// then the netlobby stage assigned to the map we saved in's area will be used in the lobby.
	PXMOD_AREA_CHUNK mAreas;

	/// Stores all the music.
	///
	PXMOD_MUSIC_CHUNK mMusic;

	/// Stores all the weapon information.
	///
	PXMOD_WEAPON_CHUNK mWeapons;

	/// Stores all the bullet information.
	///
	PXMOD_BULLET_CHUNK mBullets;

	/// Stores all the NPC information.
	///
	PXMOD_NPC_CHUNK mNpcs;

public: // Public constructor

	/// Constructor.
	///
	ModConfigResource();

	/// Deconstructor.
	///
	~ModConfigResource();

public: // Map manipulation

	/// @brief Create a mod out of a data path.
	///
	/// @param pModPath The mod path
	/// @param bAutoScan Scan the mod path for assets
	/// @param pModTitle The title of the mod
	/// @param pModAuthor The author of the mod
	/// @param pVersion The version of the mod
	///
	/// @returns Returns true if the mod could be created
	bool CreateMod(const char* pModPath, bool bAutoScan = true, const char* pModTitle = "My Mod", const char* pModAuthor = "Me", PXMOD_VERSION_STRUCT pVersion = PXMOD_VERSION_STRUCT());

	/// @brief Load a mod from a file on the disc.
	///
	/// @param pModPath The path of the mod's data folder.
	/// @param bOnlyValidate If set to true, then this function will only validate if the mod is valid.
	/// @param bForceRelease If this is set to true, then this will load / validate the release version of mods ONLY. If set to false, it will prefer debug-version of PXMODs over release-versions.
	/// @param bShowError Whether to show errors in console or not.
	///
	/// @returns Returns true if the mod could be loaded, false otherwise.
	bool Load(const char* pModPath, bool bOnlyValidate = false, bool bForceRelease = false, bool bShowError = true);

	/// @brief Save the mod.
	///
	/// @param bRelease Save the mod as 'release'.
	///
	/// @returns Returns true if the map could be saved, false otherwise.
	bool Save(bool bRelease = false);

public: // General cleanup

	/// @brief Clear & free all buffers.
	void Reset();

public: // Header setting

	/// @brief Set the title of the mod.
	///
	/// @param title The new title of the mod.
	void SetTitle(const char* title);

	/// @brief Set the author of the mod.
	///
	/// @param author The new author of the mod.
	void SetAuthor(const char* author);
	
	/// @brief Set the version of the mod.
	///
	/// @param pNewVersion The new version of this mod.
	void SetVersion(const PXMOD_VERSION_STRUCT& pNewVersion);

public: // Header gettings

	/// @brief Get the title of the mod.
	///
	/// @returns Returns the title of this mod.
	const char* GetTitle();

	/// @brief Get the mod's path name.
	///
	/// @return Returns the path name for this mod.
	const char* GetInternalName();

	/// @brief Get the author of the mod.
	///
	/// @returns Returns the author of this mod.
	const char* GetAuthor();

	/// @brief Get the version of the mod.
	///
	/// @returns Returns the mod's version
	PXMOD_VERSION_STRUCT GetVersion();

public: // Background editing

	/// @brief Add a background.
	///
	/// @param pFileName The background's file name. ex: "Green" will be expanded to "{data path}/bkGreen.png".
	///
	/// @returns Returns the index of the newly added background, or -1 on error.
	int AddBackground(const char* pFileName);

	/// @brief Remove a background.
	///
	/// @param pFileName The background's file name. ex: "Green" will be expanded to "{data path}/bkGreen.png".
	///
	/// @returns Returns true if the background was removed.
	bool RemoveBackground(const char* pFileName);

	/// @brief Find the index of a background.
	///
	/// @param pFileName The background's file name. ex: "Green" will be expanded to "{data path}/bkGreen.png".
	///
	/// @returns Returns the index of the background, or -1 if it wasn't found.
	int FindBackground(const char* pFileName);

public: // Tileset editing

	/// @brief Add a tileset.
	///
	/// @param pFileName The tileset's file name. ex: "Cave" will be expanded to "{data path}/Stage/PrtCave.png".
	///
	/// @returns Returns the index of the newly added tileset, or -1 on error.
	int AddTileset(const char* pFileName);

	/// @brief Remove a tileset.
	///
	/// @param pFileName The tileset's file name. ex: "Cave" will be expanded to "{data path}/Stage/PrtCave.png".
	///
	/// @returns Returns true if the tileset was removed.
	bool RemoveTileset(const char* pFileName);

	/// @brief Find the index of a tileset.
	///
	/// @param pFileName The tileset's file name. ex: "Cave" will be expanded to "{data path}/Stage/PrtCave.png".
	///
	/// @returns Returns the index of the tileset, or -1 if it wasn't found.
	int FindTileset(const char* pFileName);

public: // Spritesheet editing

	/// @brief Add a spritesheet.
	///
	/// @param pFileName The spritesheet's file name. ex: "Press" will be expanded to "{data path}/Npc/NpcPress.png".
	///
	/// @returns Returns the index of the newly added spritesheet, or -1 on error.
	int AddSpritesheet(const char* pFileName);

	/// @brief Remove a spritesheet.
	///
	/// @param pFileName The spritesheet's file name. ex: "Press" will be expanded to "{data path}/Npc/NpcPress.png".
	///
	/// @returns Returns true if the spritesheet was removed.
	bool RemoveSpritesheet(const char* pFileName);

	/// @brief Find the index of a spritesheet.
	///
	/// @param pFileName The spritesheet's file name. ex: "Press" will be expanded to "{data path}/Npc/NpcPress.png".
	///
	/// @returns Returns the index of the spritesheet, or -1 if it wasn't found.
	int FindSpritesheet(const char* pFileName);

public: // Stage editing

	/// @brief Add a stage.
	///
	/// @param pFileName The map's file name. ex: "Mimi"
	/// @param pMapName The actual string map name.
	/// @param pTileset The tileset to use for this stage.
	/// @param pNpcSheet The NPC spritesheet for this stage.
	/// @param pBossSheet The boss spritesheet for this stage.
	/// @param iBkType The background type for this stage.
	/// @param iBossType The boss type for this stage.
	///
	/// @returns Returns the index of the newly added stage, or -1 on error.
	int AddStage(const char* pFileName, const char* pMapName, const char* pTileset, const char* pNpcSheet, const char* pBossSheet, const char* pBackground, PXMOD_BK_TYPE iBkType, PXMOD_BOSS_TYPE iBossType);
	
	/// @brief Set a stage at an index.
	///
	/// @param iIndex The index of the stage.
	/// @param pFileName The stage's file name. ex: "Mimi"
	/// @param pMapName The actual string stage name.
	/// @param pTileset The tileset to use for this stage.
	/// @param pNpcSheet The NPC spritesheet for this stage.
	/// @param pBossSheet The boss spritesheet for this stage.
	/// @param iBkType The background type for this stage.
	/// @param iBossType The boss type for this stage.
	///
	/// @returns Returns the index of the newly added stage, or -1 on error.
	int SetStage(int iIndex, const char* pFileName, const char* pMapName, const char* pTileset, const char* pNpcSheet, const char* pBossSheet, const char* pBackground, PXMOD_BK_TYPE iBkType, PXMOD_BOSS_TYPE iBossType);

	/// @brief Remove a stage.
	///
	/// @param pFileName The map's file name. ex: "Mimi"
	///
	/// @returns Returns true if the stage was removed.
	bool RemoveStage(const char* pFileName);

	/// @brief Find the index of a stage.
	///
	/// @param pFileName The stage's file name. ex: "Mimi"
	///
	/// @returns Returns the index of the stage, or -1 if it wasn't found.
	int FindStage(const char* pFileName);

public: // Spritesheet editing

	/// @brief Add an area.
	///
	/// @param pAreaName The name of the new area.
	///
	/// @returns Returns the index of the newly added area, or -1 on error.
	int AddArea(const char* pAreaName);

	/// @brief Remove an area.
	///
	/// @param pAreaName The name of the area to delete.
	///
	/// @returns Returns true if the area was removed.
	bool RemoveArea(const char* pAreaName);

	/// @brief Find the index of an area.
	///
	/// @param pAreaName The name of the area to find.
	///
	/// @returns Returns the index of the area, or -1 if it wasn't found.
	int FindArea(const char* pAreaName);

public: // Music editing

	/// @brief Add music.
	///
	/// @param pMusicName The name of the music to add.
	///
	/// @returns Returns the index of the newly added music, or -1 on error.
	int AddMusic(const char* pMusicName);

	/// @brief Remove music.
	///
	/// @param pMusicName The name of the music to delete.
	///
	/// @returns Returns true if the music was removed.
	bool RemoveMusic(const char* pMusicName);

	/// @brief Find the index of music.
	///
	/// @param pMusicName The name of the music to find.
	///
	/// @returns Returns the index of the music, or -1 if it wasn't found.
	int FindMusic(const char* pMusicName);

public: // Weapon editing

	/// @brief Add a weapon.
	///
	/// @param pWeaponName The name of the weapon.
	///
	/// @returns Returns the index of the newly added weapon, or -1 on error.
	int AddWeapon(const char* pWeaponName);

	/// @brief Remove a weapon.
	///
	/// @param pWeaponName The name of the weapon to delete.
	///
	/// @returns Returns true if the weapon was removed.
	bool RemoveWeapon(const char* pWeaponName);

	/// @brief Find the index of a weapon.
	///
	/// @param pWeaponName The name of the weapon to find.
	///
	/// @returns Returns the index of the weapon, or -1 if it wasn't found.
	int FindWeapon(const char* pWeaponName);

public: // Bullet editing

	/// @brief Add a bullet.
	///
	/// @param pBulletName The name of the bullet. Not kept in release build.
	///
	/// @returns Returns the index of the newly added bullet, or -1 on error.
	int AddBullet(const char* pBulletNmae);

	/// @brief Remove a bullet.
	///
	/// @param pBulletName The name of the bullet to delete.
	///
	/// @returns Returns true if the bullet was removed.
	bool RemoveBullet(const char* pBulletName);

	/// @brief Find the index of a bullet.
	///
	/// @param pBulletName The name of the bullet to find.
	///
	/// @returns Returns the index of the bullet, or -1 if it wasn't found.
	int FindBullet(const char* pBulletName);

public: // NPC editing

	/// @brief Add an NPC.
	///
	/// @param pNpcName The name of the NPC. Not kept in release build.
	///
	/// @returns Returns the index of the newly added NPC, or -1 on error.
	int AddNPC(const char* pNpcName);

	/// @brief Remove an NPC.
	///
	/// @param pNpcName The name of the NPC to delete.
	///
	/// @returns Returns true if the NPC was removed.
	bool RemoveNPC(const char* pNpcName);

	/// @brief Find the index of an NPC.
	///
	/// @param pNpcName The name of the NPC to find.
	///
	/// @returns Returns the index of the NPC, or -1 if it wasn't found.
	int FindNPC(const char* pNpcName);

public: // Act editing

	/// @brief Add an NPC act.
	///
	/// @param iNpcIndex The index of the NPC.
	/// @param iActNo The act number.
	/// @param pActName The name of the act.
	/// @param iNodeFlags The node flags to use in this act.
	/// @param iTransmitNodeFlags The transmitnode flags to use in this act.
	///
	/// @returns Returns the index of the newly added NPC, or -1 on error.
	int AddNpcAct(int iNpcIndex, int iActNo, const char* pActName, unsigned long long int iNodeFlags, unsigned long long int iTransmitNodeFlags);

	/// @brief Remove an NPC act.
	///
	/// @param iNpcIndex The index of the NPC.
	/// @param iActNo The act number.
	///
	/// @returns Returns true if the NPC was removed.
	bool RemoveNpcAct(int iNpcIndex, int iActNo);

public: // Getting defaults methods

	/// @brief Get the default NPC for slot iIndex. Returns NULL if iIndex is out of range.
	///
	/// @param iIndex The index of the NPC slot.
	///
	/// @return Returns a pointer to a static PXMOD_NPC_STRUCT on success, otherwise returns NULL.
	static const PXMOD_NPC_STRUCT* GetDefaultNpc(int iIndex);
	
	/// @brief Get the default weapon for slot iIndex. Returns NULL if iIndex is out of range.
	///
	/// @param iIndex The index of the weapon slot.
	///
	/// @return Returns a pointer to a static PXMOD_WEAPON_STRUCT on success, otherwise returns NULL.
	static const PXMOD_WEAPON_STRUCT* GetDefaultWeapon(int iIndex);
	
	/// @brief Get the default bullet for slot iIndex. Returns NULL if iIndex is out of range.
	///
	/// @param iIndex The index of the bullet slot.
	///
	/// @return Returns a pointer to a static PXMOD_BULLET_STRUCT on success, otherwise returns NULL.
	static const PXMOD_BULLET_STRUCT* GetDefaultBullet(int iIndex);

public: // Defaulting

	/// @brief Reset weapons for this mod to the default Cavestory weapons.
	///
	void DefaultWeapons();
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

inline const char* ModConfigResource::GetTitle()
{
	return mHeader.title;
}

inline const char* ModConfigResource::GetAuthor()
{
	return mHeader.author;
}

inline PXMOD_VERSION_STRUCT ModConfigResource::GetVersion()
{
	return mHeader.version;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#if defined(_CAVESTORY_MULTIPLAYER) || !defined(_CAVEEDITOR)
extern CAVESTORY_MOD_API ModConfigResource gCaveMod;

#include <CSMAPI_endcode.h>
#endif