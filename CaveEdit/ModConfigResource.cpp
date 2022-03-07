#undef UNICODE
#include "stdafx.h"

#include "ModConfigResource.h"
#include "BitStream.h"

#include <Hasher.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <memory>
#include <direct.h>

#include "dirent.h"

#include <CaveNet_Config.h>
#include "CSMAPI_enums.h"

#ifdef _CAVESTORY_MULTIPLAYER
#include <CSMAPI_begincode.h>
#endif

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//#define PXMOD_EXPORT_DEFAULTS
#define PXMOD_DEBUG

#define LIST_INSERT(LISTV, LISTC, LISTTYPE, INDEX)		memmove(&LISTV[INDEX + 1], &LISTV[INDEX], (LISTC - INDEX - 1) * sizeof(LISTTYPE))
#define LIST_ERASE(LISTV, LISTC, LISTTYPE, INDEX)		memmove(&LISTV[INDEX], &LISTV[INDEX + 1], (LISTC - INDEX - 1) * sizeof(LISTTYPE))
#define CLAMP(a, b, c)									((a) < (b) ? (b) : ((a) > (c) ? (c) : (a)))
#define MAX(a, b)										((a) > (b) ? (a) : (b))
#define MIN(a, b)										((a) < (b) ? (a) : (b))
#define TEST_MUSIC_TYPE_FOR_WRITING(FILE_EXT, MUSIC_TYPE) \
	sprintf(path, "%s/Music/%s." #FILE_EXT, mModPath, pMusic->name); \
	if (DoesFileExist(path))\
	{\
		pMusic->type = PXMOD_MUSIC_TYPE::PXMOD_MUSIC_TYPE_##MUSIC_TYPE;\
		stream.WriteRangedInt(pMusic->type, 0, PXMOD_MUSIC_TYPE::PXMOD_MUSIC_TYPE_COUNT);\
		continue;\
	}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool DoesFileExist(const char* pFileName)
{
	FILE* fp;

	if ((fp = fopen(pFileName, "rb")) == NULL)
		return false;

	fclose(fp);
	return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
	 FILE FORMAT FOR .PXMOD:
	 * +-------+--------+--------------------------------------------------------------------------------------------------------------------------- -  -   -     -
	 * |  VER  |  TYPE  | DESCRIPTION
	 * +-------+--------+--------------------------------------------------------------------------------------------------------------------------- -  -   -     -
	 * |  1-R  | STRING | File mHeader ("PXMOD")
	 * |  1-R  | UCHAR  | File version
	 * |  2-R  | RANGED | Build type
	 * |  1-R  | RANGED | Length of "Mod Title" string
	 * |  1-R  | STRING | Mod Title
	 * |  1-R  | RANGED | Length of "Mod Author" string
	 * |  1-R  | STRING | Mod Author
	 * |  1-R  | RANGED | Mod Version 1
	 * |  1-R  | RANGED | Mod Version 2
	 * |  1-R  | RANGED | Mod Version 3
	 * |  1-R  | RANGED | Mod Version 4
	 * |  1-R  | UINT   | Background count
	 * |  1-R  | UINT   | Tileset count
	 * |  1-R  | UINT   | Spritesheet count
	 * |  1-R  | USHORT | Stage count
	 * |  2-R  | UINT   | Area count
	 * |  3-R  | UINT   | Music count
	 * |  4-R  | UINT   | Bullet count
	 * |  4-R  | UINT   | Weapon count
	 * |  5-R  | UINT   | NPC count
	 * |       |        | >>====================================== CHUNK: Events ======================================<<
	 * | 11-R  | RANGED | Core Drown Flag #
	 * | 11-R  | RANGED | Core Drown Event #
	 * | 11-R  | RANGED | 'Everyone Died' Event #
	 * | 11-R  | RANGED | Drown Event # (Singleplayer)
	 * | 11-R  | RANGED | Death Event # (Singleplayer)
	 * |       |        | >>====================================== CHUNK: Music_ ======================================<<
	 * | 12-R  | RANGED | Titlescreen Music Index
	 * | 12-R  | RANGED | NetMenu Music Index
	 * |       |        | >>===================================== CHUNK: Gameplay =====================================<<
	 * |  9-R  | BIT    | Allow Carrying Players
	 * |  9-R  | BIT    | Allow agility
	 * |  9-R  | BIT    | Allow fishing
	 * |       |        | >>======================================= CHUNK: HUD_ =======================================<<
	 * |  9-R  | BIT    | Draw HP bar
	 * |  9-R  | BIT    | Draw health (not present unless 'Draw HP bar' is '1' -- Default is '0')
	 * |  9-R  | BIT    | Draw weapons
	 * |  9-R  | BIT    | Draw ammo
	 * |  9-R  | BIT    | Draw EXP bar
	 * |  9-R  | BIT    | Draw level (not present unless 'Draw EXP bar' is '1' -- Default is '0')
	 * |       |        | >>=================================== CHUNK: Collectables ===================================<<
	 * |  9-R  | BIT    | Enabled (If this is '0', then the rest of this chunk is not present in the file, UNLESS this is a debug mod that we're loading.)
	 * |  9-R  | USHORT | Collectable Source Surface ID
	 * |  9-R  | UINT   | Collectable Source Image Left
	 * |  9-R  | UINT   | Collectable Source Image Top
	 * |  9-R  | UINT   | Collectable Source Image Right
	 * |  9-R  | UINT   | Collectable Source Image Bottom
	 * |       |        | >>==================================== CHUNK: Start Data ====================================<<
	 * |  1-R  | RANGED | Map Index
	 * |  1-R  | USHORT | X Position
	 * |  1-R  | USHORT | Y Position
	 * |  1-R  | RANGED | Event Number
	 * |  1-R  | USHORT | Health
	 * |  1-R  | USHORT | Max Health
	 * |  1-R  | RANGED | Direction
	 * |  1-R  | UINT   | Flags
	 * |       |        | >>==================================== CHUNK: Title Data ====================================<<
	 * |  1-R  | RANGED | Map Index
	 * |  1-R  | RANGED | Event Number
	 * |  1-R  | USHORT | X Position
	 * |  1-R  | USHORT | Y Position
	 * |       |        | >>================================= CHUNK: _Background Data =================================<<
	 * |  1-R  | RANGED | Size of "File name" string
	 * |  1-R  | STRING | File name
	 * |       |        | >>=================================== CHUNK: Tileset Data ===================================<<
	 * |  1-R  | RANGED | Size of "File name" string
	 * |  1-R  | STRING | File name
	 * |       |        | >>================================= CHUNK: Spritesheet Data =================================<<
	 * |  1-R  | RANGED | Size of "File name" string
	 * |  1-R  | STRING | File name
	 * |       |        | >>==================================== CHUNK: Stage Data ====================================<<
	 * |  1-R  | RANGED | Size of "File name" string
	 * |  1-R  | STRING | File name
	 * |  1-R  | RANGED | Size of "Map name" string
	 * |  1-R  | STRING | Map name
	 * |  1-R  | RANGED | Background Index
	 * |  1-R  | RANGED | Npc Spritesheet Index
	 * |  1-R  | RANGED | Boss Spritesheet Index
	 * |  1-R  | RANGED | Tileset Index
	 * |  2-R  | RANGED | Area Index
	 * |  1-R  | RANGED | Background Type
	 * |  1-R  | RANGED | Boss Number
	 * |  6-R  | BIT    | Force camera horizontal centering
	 * |  7-R  | BIT    | Force camera vertical centering
	 * |       |        | >>==================================== CHUNK: _Area Data ====================================<<
	 * |  2-D  | RANGED | Size of "Area Name" string
	 * |  2-D  | STRING | Area Name
	 * |  2-D  | BIT    | Keep this area on release
	 * |  2-R  | RANGED | Stage Index
	 * |  2-R  | BIT    | Area is singleplayer only
	 * |       |        | >>==================================== CHUNK: Music Data ====================================<<
	 * |  3-R  | RANGED | Size of "Music name" string
	 * |  3-R  | STRING | Music name
	 * |  3-R  | RANGED | Area Index
	 * | 15-R  | RANGED | Music Type
	 * |       |        | >>=================================== CHUNK: _Bullet Data ===================================<<
	 * |  4-R  | RANGED | Length of "Bullet Name" string
	 * |  4-R  | STRING | Bullet Name
	 * |  4-R  | USHORT | Flags
	 * |  4-R  | USHORT | Damage
	 * |  4-R  | USHORT | Hit Life
	 * |  4-R  | USHORT | Range
	 * |  4-R  | UINT   | Bounds Width
	 * |  4-R  | UINT   | Bounds Height
	 * |  4-R  | UINT   | Collision Width
	 * |  4-R  | UINT   | Collision Height
	 * |  4-R  | UINT   | Sprite left
	 * |  4-R  | UINT   | Sprite top
	 * |  4-R  | UINT   | Sprite right
	 * |  4-R  | UINT   | Sprite bottom
	 * |       |        | >>=================================== CHUNK: _Weapon Data ===================================<<
	 * |  4-R  | RANGED | Length of "Weapon Name" string
	 * |  4-R  | STRING | Weapon Name
	 * |  4-R  | UINT   | Display Left
	 * |  4-R  | UINT   | Display Top
	 * |  4-R  | UINT   | Display Right
	 * |  4-R  | UINT   | Display Bottom
	 * |  4-R  | UINT   | Draw Offset X
	 * |  4-R  | UINT   | Draw Offset Y
	 * |  4-R  | UINT   | Level 1 Bullet Index
	 * |  4-R  | UINT   | Level 1 Experience
	 * |  4-R  | UINT   | Level 1 Gun View Left
	 * |  4-R  | UINT   | Level 1 Gun View Top
	 * |  4-R  | UINT   | Level 1 Gun View Right
	 * |  4-R  | UINT   | Level 1 Gun View Bottom
	 * |  4-R  | UINT   | Level 2 Bullet Index
	 * |  4-R  | UINT   | Level 2 Experience
	 * |  4-R  | UINT   | Level 2 Gun View Left
	 * |  4-R  | UINT   | Level 2 Gun View Top
	 * |  4-R  | UINT   | Level 2 Gun View Right
	 * |  4-R  | UINT   | Level 2 Gun View Bottom
	 * |  4-R  | UINT   | Level 3 Bullet Index
	 * |  4-R  | UINT   | Level 3 Experience
	 * |  4-R  | UINT   | Level 3 Gun View Left
	 * |  4-R  | UINT   | Level 3 Gun View Top
	 * |  4-R  | UINT   | Level 3 Gun View Right
	 * |  4-R  | UINT   | Level 3 Gun View Bottom
	 * |       |        | >>===================================== CHUNK: NPC Data =====================================<<
	 * |  5-D  | RANGED | Length of "NPC Name" string	[ *!!!* Version 13+ includes this field in RELEASE MODE *!!!* ]
	 * |  5-D  | STRING | NPC Name						[ *!!!* Version 13+ includes this field in RELEASE MODE *!!!* ]
	 * |  8-D  | RANGED | Length of "Category Name" string
	 * |  8-D  | STRING | Category Name
	 * |  8-D  | RANGED | Length of "ShortHand Name 1" string
	 * |  8-D  | STRING | ShortHand Name 1
	 * |  8-D  | RANGED | Length of "ShortHand Name 2" string
	 * |  8-D  | STRING | ShortHand Name 2
	 * |  8-D  | RANGED | Length of "Option 1" string
	 * |  8-D  | STRING | Option 1
	 * |  8-D  | RANGED | Length of "Option 2" string
	 * |  8-D  | STRING | Option 2
	 * | 10-D  | RANGED | Length of "Option 3" string
	 * | 10-D  | STRING | Option 3
	 * |  5-D  | BITS   | Testing Spritesheet Index
	 * |  5-R  | ULLONG | Nodes
	 * |  5-R  | ULLONG | Transmit Nodes
	 * |  5-R  | UINT   | Default Bits
	 * |  5-R  | USHORT | Life
	 * |  5-R  | UCHAR  | Surface
	 * |  5-R  | UCHAR  | HitVoice
	 * |  5-R  | UCHAR  | DestroyVoice
	 * |  5-R  | UCHAR  | Size
	 * |  5-R  | UCHAR  | Hit Left
	 * |  5-R  | UCHAR  | Hit Right
	 * |  5-R  | UCHAR  | Hit Top
	 * |  5-R  | UCHAR  | Hit Bottom
	 * |  5-R  | UCHAR  | View Left
	 * |  5-R  | UCHAR  | View Right
	 * |  5-R  | UCHAR  | View Top
	 * |  5-R  | UCHAR  | View Bottom
	 * |  5-R  | UINT   | Sprite Left
	 * |  5-R  | UINT   | Sprite Top
	 * |  5-R  | UINT   | Sprite Right
	 * |  5-R  | UINT   | Sprite Bottom
	 * | 14-R  | UINT   | Act Count
	 * |       |        | >>===================================== CHUNK: ACT Data =====================================<<
	 * | 14-D  | RANGED | Length of "Act Name" string
	 * | 14-D  | STRING | Act Name
	 * | 14-D  | UINT   | Act Number
	 * | 14-R  | ULLONG | Nodes
	 * | 14-R  | ULLONG | Transmit Nodes
	 * +-------+--------+--------------------------------------------------------------------------------------------------------------------------- -  -   -     -
*/
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ModConfigResource::ModConfigResource()
{
	*mModPath = 0;

	memset(&mHeader,		0, sizeof(mHeader));
	memset(&mStart,			0, sizeof(mStart));
	memset(&mTitle,			0, sizeof(mTitle));
	memset(&mBackgrounds,	0, sizeof(mBackgrounds));
	memset(&mSpritesheets,	0, sizeof(mSpritesheets));
	memset(&mTilesets,		0, sizeof(mTilesets));
	memset(&mStages,		0, sizeof(mStages));
	memset(&mAreas,			0, sizeof(mAreas));
	memset(&mMusic,			0, sizeof(mMusic));
	memset(&mBullets,		0, sizeof(mBullets));
	memset(&mWeapons,		0, sizeof(mWeapons));
	memset(&mNpcs,			0, sizeof(mNpcs));
}

ModConfigResource::~ModConfigResource()
{
	Reset();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ModConfigResource::CreateMod(const char* pModPath, bool bAutoScan, const char* pModTitle, const char* pModAuthor, PXMOD_VERSION_STRUCT pVersion)
{
	unsigned int mod_path_length;
	char buffer[1024];
	char buffer2[1024];
	char parent_dir[512];
	char* iCache;

	// Reset memory
	Reset();

	// Set the mod path
	strcpy_s(mModPath, sizeof(mModPath), pModPath);

	// Get the length of the mod path, for easier & faster access
	mod_path_length = strlen(mModPath);

	// Make sure the mod path has no trailing slash.
	while (mModPath[mod_path_length - 1] == '/' || mModPath[mod_path_length - 1] == '\\')
		mModPath[--mod_path_length] = 0;

	// Make sure the mod path has no invalid slashes. It should ONLY contain forward slashes, for consistency.
	for (char* ptr = mModPath; *ptr != 0; ptr++)
		if (*ptr == '\\')
			*ptr = '/';

	struct stat info;
	
	// Build the path name
	strcpy(buffer, mModPath);
	strcat(buffer, "/");

	// Check to see if the path exists
	if (stat(buffer, &info))
	{
		strcpy(buffer, mModPath); _mkdir(buffer);
		strcpy(buffer, mModPath); strcat(buffer, "/Npc"); _mkdir(buffer);
		strcpy(buffer, mModPath); strcat(buffer, "/Stage"); _mkdir(buffer);
		strcpy(buffer, mModPath); strcat(buffer, "/Music"); _mkdir(buffer);
		strcpy(buffer, mModPath); strcat(buffer, "/Saves"); _mkdir(buffer);
		strcpy(buffer, mModPath); strcat(buffer, "/Demos"); _mkdir(buffer);
		strcpy(buffer, mModPath); strcat(buffer, "/Players"); _mkdir(buffer);
	}

	// Build the parent dir string.
	strcpy(parent_dir, mModPath);
	if (strrchr(parent_dir, '/') != NULL)
		*strrchr(parent_dir, '/') = 0;

	// Setup the mod config
	strcpy_s(mHeader.title,  sizeof(mHeader.title), pModTitle);
	strcpy_s(mHeader.author, sizeof(mHeader.author), pModAuthor);
	mHeader.version.data[0] = CLAMP(pVersion.data[0], 0, 9);
	mHeader.version.data[1] = CLAMP(pVersion.data[1], 0, 9);
	mHeader.version.data[2] = CLAMP(pVersion.data[2], 0, 9);
	mHeader.version.data[3] = CLAMP(pVersion.data[3], 0, 9);

	mHeader.events.drown_core_flag = 4000;
	mHeader.events.drown_core      = 1100;
	mHeader.events.everyone_died   = 33;
	mHeader.events.drown           = 41;
	mHeader.events.death           = 40;
	mHeader.music.title            = 24;
	mHeader.music.netmenu          = 42;

	mHeader.gameplay.allow_agility          = true;
	mHeader.gameplay.allow_fishing          = true;
	mHeader.gameplay.allow_carrying_players = true;
	mHeader.hud.draw_ammo                   = true;
	mHeader.hud.draw_exp_bar                = true;
	mHeader.hud.draw_hp                     = true;
	mHeader.hud.draw_hp_bar                 = true;
	mHeader.hud.draw_level                  = true;
	mHeader.hud.draw_weapons                = true;
	mHeader.collectables.enabled            = false;

	AddArea("Default Area");
	DefaultWeapons();

	if (!bAutoScan)
	{
		FILE* fp;

#define IF_COPY_DEF_FILE(FFFILE) sprintf_s(buffer, "%s/data/" ##FFFILE, parent_dir); sprintf(buffer2, "%s/" ##FFFILE, mModPath); if (CopyFileA(buffer, buffer2, false))
#define COPY_DEF_FILE(FFFILE) sprintf_s(buffer, "%s/data/" ##FFFILE, parent_dir); sprintf(buffer2, "%s/" ##FFFILE, mModPath); CopyFileA(buffer, buffer2, false);

		// Copy the files
		IF_COPY_DEF_FILE("bk0.png")               { AddBackground("0"); }
		IF_COPY_DEF_FILE("Npc/Npc0.png")          { AddSpritesheet("0"); }
		IF_COPY_DEF_FILE("Stage/Prt0.png")        { AddTileset("0"); }
		IF_COPY_DEF_FILE("Stage/PrtBarr.png")     { AddTileset("Barr"); COPY_DEF_FILE("Stage/Barr.pxa"); }
		COPY_DEF_FILE("Stage/0.pxa");
		COPY_DEF_FILE("Stage/NetLobbyNA.pxe");
		IF_COPY_DEF_FILE("Stage/NetLobbyNA.pxm")  { AddStage("NetLobbyNA", "NET_LOBBY_DEFAULT", "Barr", "0", "0", "0", PXMOD_BK_TYPE::PXMOD_BK_FIXED, PXMOD_BOSS_TYPE::PXMOD_BOSS_NONE); }
		COPY_DEF_FILE("Stage/NetLobbyNA.pxn");
		IF_COPY_DEF_FILE("Music/XXXX.org") { AddMusic("XXXX"); }
		COPY_DEF_FILE("Stage/NetLobbyNA.tsc");
		COPY_DEF_FILE("npc.tbl");
		COPY_DEF_FILE("StageSelect.tsc");
		COPY_DEF_FILE("Head.tsc");
		COPY_DEF_FILE("ArmsItem.tsc");
		COPY_DEF_FILE("Credit.tsc");

#undef COPY_DEF_FILE
#undef IF_COPY_DEF_FILE

		return true;
	}

	// Scan the directories
	DIR* dir;
	struct dirent* s_dir;
	char file_name_buffer[_MAX_PATH];

	// Scan for backgrounds
	{
		strcpy(buffer, mModPath);
		strcat(buffer, "/");

		if ((dir = opendir(buffer)) != NULL)
		{
			/* print all the files and directories */
			while ((s_dir = readdir(dir)) != NULL)
			{
				strcpy_s(file_name_buffer, sizeof(file_name_buffer), s_dir->d_name);

				// Terminate the string where the extension starts
				if ((iCache = strrchr(file_name_buffer, '.')) != NULL)
					*iCache = NULL;

				// Add the background if it's valid
				if (file_name_buffer[0] == 'b' && file_name_buffer[1] == 'k')
					AddBackground(file_name_buffer + 2);
			}

			closedir(dir);
		}
	}

	// Scan for NPC spritesheets
	{
		strcpy(buffer, mModPath);
		strcat(buffer, "/Npc/");

		if ((dir = opendir(buffer)) != NULL)
		{
			/* print all the files and directories */
			while ((s_dir = readdir(dir)) != NULL)
			{
				strcpy_s(file_name_buffer, sizeof(file_name_buffer), s_dir->d_name);

				// Terminate the string where the extension starts
				if ((iCache = strrchr(file_name_buffer, '.')) != NULL)
					* iCache = NULL;

				// Add the background if it's valid
				if (file_name_buffer[0] == 'N' && file_name_buffer[1] == 'p' && file_name_buffer[1] == 'c')
					AddSpritesheet(file_name_buffer + 3);
			}

			closedir(dir);
		}
	}

	// Scan for stages
	{
		strcpy(buffer, mModPath);
		strcat(buffer, "/Stage/");

		if ((dir = opendir(buffer)) != NULL)
		{
			/* print all the files and directories */
			while ((s_dir = readdir(dir)) != NULL)
			{
				strcpy_s(file_name_buffer, sizeof(file_name_buffer), s_dir->d_name);

				// Terminate the string where the extension starts
				if ((iCache = strrchr(file_name_buffer, '.')) != NULL)
					* iCache = NULL;

				// Add the background if it's valid
				if (
					s_dir->d_name[s_dir->d_namlen - 5] == '.' &&
					s_dir->d_name[s_dir->d_namlen - 4] == 'p' &&
					s_dir->d_name[s_dir->d_namlen - 3] == 'x' &&
					s_dir->d_name[s_dir->d_namlen - 2] == 'c' &&
					s_dir->d_name[s_dir->d_namlen - 1] == 'm'
					)
					AddStage(file_name_buffer, "Unknown", "0", "0", "0", "0", PXMOD_BK_TYPE::PXMOD_BK_FIXED, PXMOD_BOSS_TYPE::PXMOD_BOSS_NONE);
			}

			closedir(dir);
		}
	}

	// Scan for tilesets
	{
		strcpy(buffer, mModPath);
		strcat(buffer, "/Stage/");

		if ((dir = opendir(buffer)) != NULL)
		{
			/* print all the files and directories */
			while ((s_dir = readdir(dir)) != NULL)
			{
				strcpy_s(file_name_buffer, sizeof(file_name_buffer), s_dir->d_name);

				// Terminate the string where the extension starts
				if ((iCache = strrchr(file_name_buffer, '.')) != NULL)
					* iCache = NULL;

				// Add the background if it's valid
				if (
					s_dir->d_name[0] == 'P' &&
					s_dir->d_name[1] == 'r' &&
					s_dir->d_name[2] == 't' &&
					s_dir->d_name[s_dir->d_namlen - 4] == '.' &&
					s_dir->d_name[s_dir->d_namlen - 3] == 'p' &&
					s_dir->d_name[s_dir->d_namlen - 2] == 'n' &&
					s_dir->d_name[s_dir->d_namlen - 1] == 'g'
					)
					AddTileset(file_name_buffer + 3);
			}

			closedir(dir);
		}
	}

	// Scan for tilesets
	{
		strcpy(buffer, mModPath);
		strcat(buffer, "/Music/");

		if ((dir = opendir(buffer)) != NULL)
		{
			/* print all the files and directories */
			while ((s_dir = readdir(dir)) != NULL)
			{
				strcpy_s(file_name_buffer, sizeof(file_name_buffer), s_dir->d_name);

				// Terminate the string where the extension starts
				if ((iCache = strrchr(file_name_buffer, '.')) != NULL)
					* iCache = NULL;

				// Add the background if it's valid
				if (
					s_dir->d_name[s_dir->d_namlen - 4] == '.' &&
					s_dir->d_name[s_dir->d_namlen - 3] == 'o' &&
					s_dir->d_name[s_dir->d_namlen - 2] == 'r' &&
					s_dir->d_name[s_dir->d_namlen - 1] == 'g'
					)
					AddMusic(file_name_buffer);
			}

			closedir(dir);
		}
	}

	// Done!
	return true;
}

bool ModConfigResource::Load(const char* pModPath, bool bOnlyValidate, bool bForceRelease, bool bShowError)
{
	SharedBitStream stream;
	unsigned char version;
	unsigned int mod_path_length;
	unsigned int len;
	char tmp_mod_path[sizeof(mModPath)];
	char buffer[1024];
	char path[1024];
	int i, j;

	// Set the mod path
	strcpy_s(tmp_mod_path, sizeof(tmp_mod_path), pModPath);

	// Get the length of the mod path, for easier & faster access
	mod_path_length = strlen(tmp_mod_path);

	// Make sure the mod path has no trailing slash.
	while (tmp_mod_path[mod_path_length - 1] == '/' || tmp_mod_path[mod_path_length - 1] == '\\')
		tmp_mod_path[--mod_path_length] = 0;

	// Make sure the mod path has no invalid slashes. It should ONLY contain forward slashes, for consistency.
	for (char* ptr = tmp_mod_path; *ptr != 0; ptr++)
		if (*ptr == '\\')
			*ptr = '/';

	struct stat info;

	// Build the path name
	strcpy(buffer, tmp_mod_path);
	strcat(buffer, "/");

	// Check to see if the path exists
	if (stat(buffer, &info))
	{
#ifdef PXMOD_DEBUG
		if (bShowError)
			printf("ERROR: " __FUNCTION__ "() - Failed to find mod path \"%s\".\r\n", buffer);
#endif

		return false;
	}

	// Read the entire file into the bitstream
	{
		FILE* fp = NULL;

		// Try debug version
		if (fp == NULL && !bForceRelease)
		{
			// Build the path to the mod file
			sprintf_s(buffer, sizeof(buffer), "%s/dbg-mod.pxmod", tmp_mod_path);

			// Attempt to open the file for reading
			fp = fopen(buffer, "rb");
		}

		// Try release version
		if (fp == NULL)
		{
			// Build the path to the mod file
			sprintf_s(buffer, sizeof(buffer), "%s/mod.pxmod", tmp_mod_path);

			// Attempt to open the file for reading
			fp = fopen(buffer, "rb");
		}

		// Make sure we actually found a valid mod file
		if (fp == NULL)
		{
			if (bShowError)
				printf("ERROR: " __FUNCTION__ "() - Failed to open mod file \"%s\"\r\n", buffer);

			return false;
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
	if (memcmp(stream.mBuffer, PXMOD_HEADER_STRING, 5))
	{
		if (bShowError)
			printf("ERROR: " __FUNCTION__ "() - Invalid magic!\r\n");

		return false;
	}

	// Get the file version
	version = (unsigned char)stream.mBuffer[5];

	// Validate version
	if (version != PXMOD_VERSION && bShowError)
		printf("ERROR: " __FUNCTION__ "() - Mod version mismatch (expected: %d, got: %d) -- Attempting to read anyways\r\n", PXMOD_VERSION, version);

	// Stop here if we must
	if (bOnlyValidate)
	{
		stream.Clear();
		return true;
	}

	// Reset memory
	Reset();

	// Copy the tmp mod path into our member field
	strcpy_s(mModPath, sizeof(mModPath), tmp_mod_path);

	// Skip past the header & version
	stream.SetCurPos(48);

	// Read the mod info
	mHeader.build_type      = (version >= 2 ? (PXMOD_BUILD_TYPE)stream.ReadRangedInt(0, PXMOD_BUILD_TYPE::PXMOD_BUILD_COUNT) : PXMOD_BUILD_TYPE::PXMOD_BUILD_DEBUG);
	stream.ReadString(mHeader.title, 256);
	stream.ReadString(mHeader.author, 256);
	mHeader.version.data[0] = stream.ReadRangedInt(0, 9);
	mHeader.version.data[1] = stream.ReadRangedInt(0, 9);
	mHeader.version.data[2] = stream.ReadRangedInt(0, 9);
	mHeader.version.data[3] = stream.ReadRangedInt(0, 9);

	// Read the counts
	mBackgrounds.count  = stream.ReadInt(sizeof(unsigned int) << 3);
	mTilesets.count     = stream.ReadInt(sizeof(unsigned int) << 3);
	mSpritesheets.count = stream.ReadInt(sizeof(unsigned int) << 3);
	mStages.count       = stream.ReadInt(sizeof(unsigned short) << 3);
	mAreas.count        = (version >= 2 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
	mMusic.count        = (version >= 3 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
	mBullets.count      = (version >= 4 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
	mWeapons.count      = (version >= 4 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
	mNpcs.count         = (version >= 5 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);

	// Allocate stuff
	mBackgrounds.list  = (!mBackgrounds.count ?		NULL : (PXMOD_BACKGROUND_STRUCT*)malloc(sizeof(PXMOD_BACKGROUND_STRUCT) * mBackgrounds.count));
	mTilesets.list     = (!mTilesets.count ?		NULL : (PXMOD_TILESET_STRUCT*)malloc(sizeof(PXMOD_TILESET_STRUCT) * mTilesets.count));
	mSpritesheets.list = (!mSpritesheets.count ?	NULL : (PXMOD_SPRITESHEET_STRUCT*)malloc(sizeof(PXMOD_SPRITESHEET_STRUCT) * mSpritesheets.count));
	mStages.list       = (!mStages.count ?			NULL : (PXMOD_STAGE_STRUCT*)malloc(sizeof(PXMOD_STAGE_STRUCT) * mStages.count));
	mAreas.list        = (!mAreas.count ?			NULL : (PXMOD_AREA_STRUCT*)malloc(sizeof(PXMOD_AREA_STRUCT) * mAreas.count));
	mMusic.list        = (!mMusic.count ?			NULL : (PXMOD_MUSIC_STRUCT*)malloc(sizeof(PXMOD_MUSIC_STRUCT) * mMusic.count));
	mBullets.list      = (!mBullets.count ?			NULL : (PXMOD_BULLET_STRUCT*)malloc(sizeof(PXMOD_BULLET_STRUCT) * mBullets.count));
	mWeapons.list      = (!mWeapons.count ?			NULL : (PXMOD_WEAPON_STRUCT*)malloc(sizeof(PXMOD_WEAPON_STRUCT) * mWeapons.count));
	mNpcs.list         = (!mNpcs.count ?			NULL : (PXMOD_NPC_STRUCT*)malloc(sizeof(PXMOD_NPC_STRUCT) * mNpcs.count));

	// Zero-out the memory where applicable
	if (mBackgrounds.list)	memset(mBackgrounds.list,	0, sizeof(PXMOD_BACKGROUND_STRUCT) * mBackgrounds.count);
	if (mTilesets.list)		memset(mTilesets.list,		0, sizeof(PXMOD_TILESET_STRUCT) * mTilesets.count);
	if (mSpritesheets.list)	memset(mSpritesheets.list,	0, sizeof(PXMOD_SPRITESHEET_STRUCT) * mSpritesheets.count);
	if (mStages.list)		memset(mStages.list,		0, sizeof(PXMOD_STAGE_STRUCT) * mStages.count);
	if (mAreas.list)		memset(mAreas.list,			0, sizeof(PXMOD_AREA_STRUCT) * mAreas.count);
	if (mMusic.list)		memset(mMusic.list,			0, sizeof(PXMOD_MUSIC_STRUCT) * mMusic.count);
	if (mBullets.list)		memset(mBullets.list,		0, sizeof(PXMOD_BULLET_STRUCT) * mBullets.count);
	if (mWeapons.list)		memset(mWeapons.list,		0, sizeof(PXMOD_WEAPON_STRUCT) * mWeapons.count);
	if (mNpcs.list)			memset(mNpcs.list,			0, sizeof(PXMOD_NPC_STRUCT) * mNpcs.count);

	// Read the events
	mHeader.events.drown_core_flag = (version >= 11 ? stream.ReadRangedInt(0, 9999) : 4000);
	mHeader.events.drown_core      = (version >= 11 ? stream.ReadRangedInt(0, 9999) : 1100);
	mHeader.events.everyone_died   = (version >= 11 ? stream.ReadRangedInt(0, 9999) : 33);
	mHeader.events.drown           = (version >= 11 ? stream.ReadRangedInt(0, 9999) : 41);
	mHeader.events.death           = (version >= 11 ? stream.ReadRangedInt(0, 9999) : 40);

	// Read music indices
	mHeader.music.title   = (version >= 12 ? stream.ReadRangedInt(0, mMusic.count) : 24);
	mHeader.music.netmenu = (version >= 12 ? stream.ReadRangedInt(0, mMusic.count) : 42);

	// Read the gameplay elements
	mHeader.gameplay.allow_carrying_players = (version >= 9 ? stream.ReadFlag() : true);
	mHeader.gameplay.allow_agility          = (version >= 9 ? stream.ReadFlag() : true);
	mHeader.gameplay.allow_fishing          = (version >= 9 ? stream.ReadFlag() : true);

	// Read the HUD info
	mHeader.hud.draw_hp_bar  = (version >= 9 ? stream.ReadFlag() : true);
	mHeader.hud.draw_hp      = (version >= 9 ? (mHeader.hud.draw_hp_bar == true ? stream.ReadFlag() : false) : true);
	mHeader.hud.draw_weapons = (version >= 9 ? stream.ReadFlag() : true);
	mHeader.hud.draw_ammo    = (version >= 9 ? stream.ReadFlag() : true);
	mHeader.hud.draw_exp_bar = (version >= 9 ? stream.ReadFlag() : true);
	mHeader.hud.draw_level   = (version >= 9 ? (mHeader.hud.draw_exp_bar == true ? stream.ReadFlag() : false) : true);

	// Read the collectable data
	mHeader.collectables.enabled = (version >= 9 ? stream.ReadFlag() : false);
	if (mHeader.collectables.enabled || mHeader.build_type == PXMOD_BUILD_TYPE::PXMOD_BUILD_DEBUG)
	{
		mHeader.collectables.surface_id         = (version >= 9 ? stream.ReadInt(sizeof(unsigned short) << 3) : 0);
		mHeader.collectables.sprite_rect.left   = (version >= 9 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
		mHeader.collectables.sprite_rect.top    = (version >= 9 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
		mHeader.collectables.sprite_rect.right  = (version >= 9 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
		mHeader.collectables.sprite_rect.bottom = (version >= 9 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
	}

	// Read the starting map info
	mStart.map    = stream.ReadRangedInt(0, mStages.count);
	mStart.x      = stream.ReadInt(sizeof(unsigned short) << 3);
	mStart.y      = stream.ReadInt(sizeof(unsigned short) << 3);
	mStart.event  = stream.ReadRangedInt(0, 9999);
	mStart.hp     = stream.ReadInt(sizeof(unsigned short) << 3);
	mStart.max_hp = stream.ReadInt(sizeof(unsigned short) << 3);
	mStart.direct = (PXMOD_DIRECTIONS)stream.ReadRangedInt(0, PXMOD_DIRECTIONS::PXMOD_DIRECT_COUNT);
	mStart.flags  = stream.ReadInt(sizeof(unsigned int) << 3);

	// Read title info
	mTitle.map    = stream.ReadRangedInt(0, mStages.count);
	mTitle.event  = stream.ReadRangedInt(0, 9999);
	mTitle.x      = stream.ReadInt(sizeof(unsigned short) << 3);
	mTitle.y      = stream.ReadInt(sizeof(unsigned short) << 3);

	// Read resources
	for (i = 0; i < mBackgrounds.count; i++)  stream.ReadString(mBackgrounds.list[i].name, 256);
	for (i = 0; i < mTilesets.count; i++)     stream.ReadString(mTilesets.list[i].name, 256);
	for (i = 0; i < mSpritesheets.count; i++) stream.ReadString(mSpritesheets.list[i].name, 256);
	for (i = 0; i < mStages.count; i++)
	{
		PXMOD_STAGE_STRUCT* pStage = &mStages.list[i];

		stream.ReadString(pStage->file, (version >= 16 ? 32 : 16));
		stream.ReadString(pStage->name, sizeof(PXMOD_STAGE_STRUCT::name));

		pStage->background     = stream.ReadRangedInt(0, mBackgrounds.count);
		pStage->npc_sheet      = stream.ReadRangedInt(0, mSpritesheets.count);
		pStage->boss_sheet     = stream.ReadRangedInt(0, mSpritesheets.count);
		pStage->tileset        = stream.ReadRangedInt(0, mTilesets.count);
		pStage->area_index     = (version >= 2 ? stream.ReadRangedInt(0, mAreas.count) : 0);
		pStage->bkType         = (PXMOD_BK_TYPE)stream.ReadRangedInt(0, PXMOD_BK_TYPE::PXMOD_BK_COUNT);
		pStage->boss_no        = (PXMOD_BOSS_TYPE)stream.ReadRangedInt(0, PXMOD_BOSS_TYPE::PXMOD_BOSS_COUNT);
		pStage->focus_center_x = (version >= 6 ? stream.ReadFlag() : false);
		pStage->focus_center_y = (version >= 7 ? stream.ReadFlag() : false);
	}

	// Read areas
	for (i = 0; i < mAreas.count; i++)
	{
		PXMOD_AREA_STRUCT* pArea = &mAreas.list[i];

		// Read the debug information if we can
		if (mHeader.build_type == PXMOD_BUILD_TYPE::PXMOD_BUILD_DEBUG)
		{
			stream.ReadString(pArea->name, sizeof(PXMOD_AREA_STRUCT::name));
			pArea->keep_on_release = stream.ReadFlag();
		}
		else
		{
			pArea->keep_on_release = true;
		}

		// Read non-debug info
		pArea->stage_idx         = stream.ReadRangedInt(0, mStages.count);
		pArea->singleplayer_only = stream.ReadFlag();
	}

	// Read music
	for (i = 0; i < mMusic.count; i++)
	{
		PXMOD_MUSIC_STRUCT* pMusic = &mMusic.list[i];

		// Read info
		stream.ReadString(pMusic->name, sizeof(pMusic->name));

		// Determine the music type
		pMusic->type = (version >= 15 ? (PXMOD_MUSIC_TYPE)stream.ReadRangedInt(0, PXMOD_MUSIC_TYPE::PXMOD_MUSIC_TYPE_COUNT) : PXMOD_MUSIC_TYPE::PXMOD_MUSIC_TYPE_ORGANYA);
	}
	
	// Read bullets
	for (i = 0; i < mBullets.count; i++)
	{
		PXMOD_BULLET_STRUCT* pBullet = &mBullets.list[i];

		// Read info for this bullet
		stream.ReadString(pBullet->name, sizeof(pBullet->name));
		pBullet->flags            = stream.ReadInt(sizeof(unsigned short) << 3);
		pBullet->damage           = stream.ReadInt(sizeof(unsigned short) << 3);
		pBullet->hit_life         = stream.ReadInt(sizeof(unsigned short) << 3);
		pBullet->range            = stream.ReadInt(sizeof(unsigned short) << 3);
		pBullet->bullet_size.w    = stream.ReadInt(sizeof(unsigned int) << 3);
		pBullet->bullet_size.h    = stream.ReadInt(sizeof(unsigned int) << 3);
		pBullet->collision_size.w = stream.ReadInt(sizeof(unsigned int) << 3);
		pBullet->collision_size.h = stream.ReadInt(sizeof(unsigned int) << 3);
		pBullet->sprite.left      = stream.ReadInt(sizeof(unsigned int) << 3);
		pBullet->sprite.top       = stream.ReadInt(sizeof(unsigned int) << 3);
		pBullet->sprite.right     = stream.ReadInt(sizeof(unsigned int) << 3);
		pBullet->sprite.bottom    = stream.ReadInt(sizeof(unsigned int) << 3);
	}

	// Read weapons
	for (i = 0; i < mWeapons.count; i++)
	{
		PXMOD_WEAPON_STRUCT* pWeapon = &mWeapons.list[i];

		// Read basic info
		stream.ReadString(pWeapon->name, sizeof(pWeapon->name) - 1);

		// Read display info
		pWeapon->display_info.left          = stream.ReadInt(sizeof(unsigned int) << 3);
		pWeapon->display_info.top           = stream.ReadInt(sizeof(unsigned int) << 3);
		pWeapon->display_info.right         = stream.ReadInt(sizeof(unsigned int) << 3);
		pWeapon->display_info.bottom        = stream.ReadInt(sizeof(unsigned int) << 3);
		pWeapon->display_info.hold_offset_x = stream.ReadInt(sizeof(unsigned int) << 3);
		pWeapon->display_info.hold_offset_y = stream.ReadInt(sizeof(unsigned int) << 3);

		// Read level info
		for (j = 0; j < 3; j++)
		{
			pWeapon->level_info[j].bullet_index  = stream.ReadRangedInt(0, mBullets.count);
			pWeapon->level_info[j].exp           = stream.ReadInt(sizeof(unsigned int) << 3);
			pWeapon->level_info[j].view.left     = stream.ReadInt(sizeof(unsigned int) << 3);
			pWeapon->level_info[j].view.top      = stream.ReadInt(sizeof(unsigned int) << 3);
			pWeapon->level_info[j].view.right    = stream.ReadInt(sizeof(unsigned int) << 3);
			pWeapon->level_info[j].view.bottom   = stream.ReadInt(sizeof(unsigned int) << 3);
		}
	}

	// Read NPCs & bosses
	for (i = 0; i < mNpcs.count; i++)
	{
		PXMOD_NPC_STRUCT* pNpc = &mNpcs.list[i];

		// Read the name differently as of version 13+
		if (version >= 13 || (version < 13 && mHeader.build_type == PXMOD_BUILD_TYPE::PXMOD_BUILD_DEBUG))
			stream.ReadString(pNpc->name, sizeof(pNpc->name));

		if (mHeader.build_type == PXMOD_BUILD_TYPE::PXMOD_BUILD_DEBUG)
		{
			// Read the debug information
			if (version >= 8)
			{
				stream.ReadString(pNpc->category, sizeof(pNpc->category));
				stream.ReadString(pNpc->shortname1, sizeof(pNpc->shortname1));
				stream.ReadString(pNpc->shortname2, sizeof(pNpc->shortname2));
				stream.ReadString(pNpc->option1, sizeof(pNpc->option1));
				stream.ReadString(pNpc->option2, sizeof(pNpc->option2));

				if (version >= 10)
				{
					// Read the option 3
					stream.ReadString(pNpc->option3, sizeof(pNpc->option3));
				}
				else
				{
					// Null terminate the string otherwise
					*pNpc->option3 = 0;
				}
			}

			pNpc->spritesheet_index = stream.ReadInt(sizeof(unsigned int) << 3);
		}

		stream.Read(&pNpc->nodes,			sizeof(unsigned long long int) << 3);
		stream.Read(&pNpc->transmit_nodes,	sizeof(unsigned long long int) << 3);
		pNpc->bits           = stream.ReadInt(sizeof(unsigned int) << 3);
		pNpc->life           = stream.ReadInt(sizeof(unsigned short) << 3);
		pNpc->surf           = stream.ReadInt(sizeof(unsigned char) << 3);
		pNpc->hit_voice      = stream.ReadInt(sizeof(unsigned char) << 3);
		pNpc->destroy_voice  = stream.ReadInt(sizeof(unsigned char) << 3);
		pNpc->size           = stream.ReadInt(sizeof(unsigned char) << 3);
		pNpc->exp            = stream.ReadInt(sizeof(unsigned long) << 3);
		pNpc->damage         = stream.ReadInt(sizeof(unsigned long) << 3);
		pNpc->hit.front      = stream.ReadInt(sizeof(unsigned char) << 3);
		pNpc->hit.back       = stream.ReadInt(sizeof(unsigned char) << 3);
		pNpc->hit.top        = stream.ReadInt(sizeof(unsigned char) << 3);
		pNpc->hit.bottom     = stream.ReadInt(sizeof(unsigned char) << 3);
		pNpc->view.front     = stream.ReadInt(sizeof(unsigned char) << 3);
		pNpc->view.back      = stream.ReadInt(sizeof(unsigned char) << 3);
		pNpc->view.top       = stream.ReadInt(sizeof(unsigned char) << 3);
		pNpc->view.bottom    = stream.ReadInt(sizeof(unsigned char) << 3);
		pNpc->sprite.left    = stream.ReadInt(sizeof(unsigned int) << 3);
		pNpc->sprite.top     = stream.ReadInt(sizeof(unsigned int) << 3);
		pNpc->sprite.right   = stream.ReadInt(sizeof(unsigned int) << 3);
		pNpc->sprite.bottom  = stream.ReadInt(sizeof(unsigned int) << 3);

		pNpc->useNewColMethod = ((pNpc->bits & 0x004000) != 0 && ((pNpc->bits &= ~0x004000) & 0x004000) == 0) ? true : false;
		pNpc->canBreakTile47  = ((pNpc->bits & 0x001000) != 0 && ((pNpc->bits &= ~0x001000) & 0x001000) == 0) ? true : false;

		// Read acts
		pNpc->acts.count = (version >= 14 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);

		// Ignore acts if this NPC has none
		if (!pNpc->acts.count)
			continue;

		// Allocate the list
		pNpc->acts.highest_act_no = 0;
		pNpc->acts.list           = (PXMOD_NPC_ACT_STRUCT*)malloc(sizeof(PXMOD_NPC_ACT_STRUCT) * pNpc->acts.count);
		memset(pNpc->acts.list, 0, sizeof(PXMOD_NPC_ACT_STRUCT) * pNpc->acts.count);

		for (int j = 0; j < pNpc->acts.count; j++)
		{
			PXMOD_NPC_ACT_STRUCT* pAct = &pNpc->acts.list[j];

			// Read debug information
			if (mHeader.build_type == PXMOD_BUILD_TYPE::PXMOD_BUILD_DEBUG)
				stream.ReadString(pAct->name, sizeof(pAct->name));

			pAct->act_no = stream.ReadInt(sizeof(int) << 3);

			if (pAct->act_no > pNpc->acts.highest_act_no)
				pNpc->acts.highest_act_no = pAct->act_no;

			stream.Read(&pAct->nodes, sizeof(unsigned long long int) << 3);
			stream.Read(&pAct->transmit_nodes, sizeof(unsigned long long int) << 3);

			pAct->nodes          &= ~NPC_update_nodes::npc_nodes_act_no;
			pAct->transmit_nodes &= ~NPC_transmit_nodes::npc_transmit_nodes_act_no;
		}

		// Remove these nodes, since they will always be included in the packet if we have per-act_no stipulations
		pNpc->nodes          &= ~NPC_update_nodes::npc_nodes_act_no;
		pNpc->transmit_nodes &= ~NPC_transmit_nodes::npc_transmit_nodes_act_no;
	}
	
	// Clear the rest of the stream
	stream.Clear();

	// Do some special stuff here
	if (version < 2)
		AddArea("Default Area");

	if (version < 4)
		DefaultWeapons();

	// Done!
	return true;
}

bool ModConfigResource::Save(bool bRelease)
{
	SharedBitStream stream;
	char buffer[1024];
	char path[1024];
	int iBackgroundCount     = 0;
	int iSpritesheetCount    = 0;
	int iTilesetCount        = 0;
	int iAreaCount           = 0;
	int i, j;
	
	// Create a blank stage
	PXMOD_STAGE_STRUCT pBlankStage;
	memset(&pBlankStage, 0, sizeof(PXMOD_STAGE_STRUCT));

	struct TMP_PXMOD_RESOURCE_TABLE_STRUCT
	{
		bool stage_used;
		int area_index;
		int background_index;
		int spritesheet_index;
		int tileset_index;
	};

	// Force release save mode if we're already in release mode
	if (mHeader.build_type == PXMOD_BUILD_TYPE::PXMOD_BUILD_RELEASE)
		bRelease = true;

	// Allocate the resource lookup table
	unsigned int iResourceTableSize                     = MAX(mBackgrounds.count, MAX(mTilesets.count, MAX(mStages.count, MAX(mAreas.count, mSpritesheets.count))));
	TMP_PXMOD_RESOURCE_TABLE_STRUCT* pResourceUsedTable = (TMP_PXMOD_RESOURCE_TABLE_STRUCT*)malloc(sizeof(TMP_PXMOD_RESOURCE_TABLE_STRUCT) * iResourceTableSize);

	// Zero-mem the resource table
	memset(pResourceUsedTable, -1, sizeof(TMP_PXMOD_RESOURCE_TABLE_STRUCT) * iResourceTableSize);

	// Count areas
	for (int i = 0; i < mAreas.count; i++)
	{
		PXMOD_AREA_STRUCT* pArea = &mAreas.list[i];

		// Skip areas that aren't supposed to saved on release build
		if (bRelease && !pArea->keep_on_release)
			continue;

		// Set the index for this one
		pResourceUsedTable[i].area_index = iAreaCount++;
	}

	// Count stages
	for (int i = 0; i < mStages.count; i++)
		pResourceUsedTable[i].stage_used = (!bRelease || mAreas.list[mStages.list[i].area_index].keep_on_release);

	// Count backgrounds
	for (int i = 0; i < iResourceTableSize; i++)
	{
		PXMOD_BACKGROUND_STRUCT* pBackground   = (i >= mBackgrounds.count  ? NULL : &mBackgrounds.list[i]);
		PXMOD_TILESET_STRUCT* pTileset         = (i >= mTilesets.count     ? NULL : &mTilesets.list[i]);
		PXMOD_SPRITESHEET_STRUCT* pSpritesheet = (i >= mSpritesheets.count ? NULL : &mSpritesheets.list[i]);
		unsigned int iBGUse                    = !bRelease ? 1 : 0;
		unsigned int iTSUse                    = !bRelease ? 1 : 0;
		unsigned int iSSUse                    = !bRelease ? 1 : 0;

		// Stop here if there is nothing to check for
		if (!pBackground && !pTileset && !pSpritesheet)
			break;

		// Count how many times this background is used
		for (int j = 0; j < mStages.count; j++)
		{
			PXMOD_STAGE_STRUCT* pStage = &mStages.list[j];

			if (!pResourceUsedTable[j].stage_used)
				continue;

			// Skip stages that either don't have this background, or ones that aren't being kept in this compilation
			if (pBackground && pStage->background == i)
				++iBGUse;
				
			if (pTileset && pStage->tileset == i)
				++iTSUse;

			if (pSpritesheet && (pStage->npc_sheet == i || pStage->boss_sheet == i))
				++iSSUse;
		}

		// Count how many times this background is used
		if (pSpritesheet && !bRelease)
		{
			for (int j = 0; j < mNpcs.count; j++)
			{
				PXMOD_NPC_STRUCT* pNpc = &mNpcs.list[j];

				if (pNpc->spritesheet_index == i)
					++iSSUse;
			}
		}

		// Remove unused resources
		if (pBackground && iBGUse)
			pResourceUsedTable[i].background_index  = iBackgroundCount++;

		if (pTileset && iTSUse)
			pResourceUsedTable[i].tileset_index     = iTilesetCount++;
			
		if (pSpritesheet && iSSUse)
			pResourceUsedTable[i].spritesheet_index = iSpritesheetCount++;
	}

	// Determine the build type.
	PXMOD_BUILD_TYPE iBuildType = (bRelease ? PXMOD_BUILD_TYPE::PXMOD_BUILD_RELEASE : PXMOD_BUILD_TYPE::PXMOD_BUILD_DEBUG);

	// Write file info
	stream.Write(PXMOD_HEADER_STRING, strlen(PXMOD_HEADER_STRING) << 3);
	stream.WriteInt(PXMOD_VERSION, sizeof(unsigned char) << 3);

	stream.WriteRangedInt(iBuildType, 0, PXMOD_BUILD_TYPE::PXMOD_BUILD_COUNT);
	stream.WriteString(mHeader.title, 256);
	stream.WriteString(mHeader.author, 256);
	stream.WriteRangedInt(mHeader.version.data[0], 0, 9);
	stream.WriteRangedInt(mHeader.version.data[1], 0, 9);
	stream.WriteRangedInt(mHeader.version.data[2], 0, 9);
	stream.WriteRangedInt(mHeader.version.data[3], 0, 9);

	// Read the counts
	stream.WriteInt(iBackgroundCount,  sizeof(unsigned int) << 3);
	stream.WriteInt(iTilesetCount,     sizeof(unsigned int) << 3);
	stream.WriteInt(iSpritesheetCount, sizeof(unsigned int) << 3);
	stream.WriteInt(mStages.count,     sizeof(unsigned short) << 3);
	stream.WriteInt(iAreaCount,        sizeof(unsigned int) << 3);
	stream.WriteInt(mMusic.count,      sizeof(unsigned int) << 3);
	stream.WriteInt(mBullets.count,    sizeof(unsigned int) << 3);
	stream.WriteInt(mWeapons.count,    sizeof(unsigned int) << 3);
	stream.WriteInt(mNpcs.count,       sizeof(unsigned int) << 3);

	// Write the events
	stream.WriteRangedInt(mHeader.events.drown_core_flag,	0, 9999);
	stream.WriteRangedInt(mHeader.events.drown_core,		0, 9999);
	stream.WriteRangedInt(mHeader.events.everyone_died,		0, 9999);
	stream.WriteRangedInt(mHeader.events.drown,				0, 9999);
	stream.WriteRangedInt(mHeader.events.death,				0, 9999);

	// Write music pieces
	stream.WriteRangedInt(mHeader.music.title,   0, mMusic.count);
	stream.WriteRangedInt(mHeader.music.netmenu, 0, mMusic.count);

	// Write the gameplay elements
	stream.WriteFlag(mHeader.gameplay.allow_carrying_players);
	stream.WriteFlag(mHeader.gameplay.allow_agility);
	stream.WriteFlag(mHeader.gameplay.allow_fishing);

	// Read the HUD info
	if (stream.WriteFlag(mHeader.hud.draw_hp_bar))
		stream.WriteFlag(mHeader.hud.draw_hp);
	stream.WriteFlag(mHeader.hud.draw_weapons);
	stream.WriteFlag(mHeader.hud.draw_ammo);
	if (stream.WriteFlag(mHeader.hud.draw_exp_bar))
		stream.WriteFlag(mHeader.hud.draw_level);

	// Read the collectable data
	bool bCollectablesEnabled = stream.WriteFlag(mHeader.collectables.enabled);
	if (bCollectablesEnabled || !bRelease)
	{
		stream.WriteInt(mHeader.collectables.surface_id,			sizeof(unsigned short) << 3);
		stream.WriteInt(mHeader.collectables.sprite_rect.left,		sizeof(unsigned int) << 3);
		stream.WriteInt(mHeader.collectables.sprite_rect.top,		sizeof(unsigned int) << 3);
		stream.WriteInt(mHeader.collectables.sprite_rect.right,		sizeof(unsigned int) << 3);
		stream.WriteInt(mHeader.collectables.sprite_rect.bottom,	sizeof(unsigned int) << 3);
	}

	// Write the starting map info
	stream.WriteRangedInt(mStart.map, 0, mStages.count);
	stream.WriteInt(mStart.x,			sizeof(unsigned short) << 3);
	stream.WriteInt(mStart.y,			sizeof(unsigned short) << 3);
	stream.WriteRangedInt(mStart.event, 0, 9999);
	stream.WriteInt(mStart.hp,			sizeof(unsigned short) << 3);
	stream.WriteInt(mStart.max_hp,		sizeof(unsigned short) << 3);
	stream.WriteRangedInt(mStart.direct, 0, PXMOD_DIRECTIONS::PXMOD_DIRECT_COUNT);
	stream.WriteInt(mStart.flags,		sizeof(unsigned int) << 3);

	// Write title info
	stream.WriteRangedInt(mTitle.map,   0, mStages.count);
	stream.WriteRangedInt(mTitle.event, 0, 9999);
	stream.WriteInt(mTitle.x,			sizeof(unsigned short) << 3);
	stream.WriteInt(mTitle.y,			sizeof(unsigned short) << 3);

	// Write resources
	for (i = 0; i < mBackgrounds.count; i++)  { if (pResourceUsedTable[i].background_index  == -1) continue; stream.WriteString(mBackgrounds.list[i].name, 256); }
	for (i = 0; i < mTilesets.count; i++)     { if (pResourceUsedTable[i].tileset_index     == -1) continue; stream.WriteString(mTilesets.list[i].name, 256); }
	for (i = 0; i < mSpritesheets.count; i++) { if (pResourceUsedTable[i].spritesheet_index == -1) continue; stream.WriteString(mSpritesheets.list[i].name, 256); }
	for (i = 0; i < mStages.count; i++)
	{
		PXMOD_STAGE_STRUCT* pStage = &mStages.list[i];

		// Skip areas that aren't supposed to saved on release build
		if (!pResourceUsedTable[i].stage_used)
			pStage = &pBlankStage;

		stream.WriteString(pStage->file, sizeof(PXMOD_STAGE_STRUCT::file));
		stream.WriteString(pStage->name, sizeof(PXMOD_STAGE_STRUCT::name));

		stream.WriteRangedInt(pResourceUsedTable[pStage->background].background_index,	0, iBackgroundCount);
		stream.WriteRangedInt(pResourceUsedTable[pStage->npc_sheet].spritesheet_index,	0, iSpritesheetCount);
		stream.WriteRangedInt(pResourceUsedTable[pStage->boss_sheet].spritesheet_index,	0, iSpritesheetCount);
		stream.WriteRangedInt(pResourceUsedTable[pStage->tileset].tileset_index,		0, iTilesetCount);
		stream.WriteRangedInt(pResourceUsedTable[pStage->area_index].area_index,		0, iAreaCount);
		stream.WriteRangedInt(pStage->bkType,											0, PXMOD_BK_TYPE::PXMOD_BK_COUNT);
		stream.WriteRangedInt(pStage->boss_no,											0, PXMOD_BOSS_TYPE::PXMOD_BOSS_COUNT);
		stream.WriteFlag(pStage->focus_center_x);
		stream.WriteFlag(pStage->focus_center_y);
	}

	// Write the areas
	for (i = 0; i < mAreas.count; i++)
	{
		PXMOD_AREA_STRUCT* pArea = &mAreas.list[i];

		// Skip areas that aren't supposed to saved on release build
		if (pResourceUsedTable[i].area_index == -1)
			continue;

		// Write the debug information if we can
		if (!bRelease)
		{
			stream.WriteString(pArea->name, sizeof(PXMOD_AREA_STRUCT::name));
			stream.WriteFlag(pArea->keep_on_release);
		}

		// Write non-debug info
		stream.WriteRangedInt(pArea->stage_idx, 0, mStages.count);
		stream.WriteFlag(pArea->singleplayer_only);
	}

	// Write music
	for (i = 0; i < mMusic.count; i++)
	{
		PXMOD_MUSIC_STRUCT* pMusic = &mMusic.list[i];

		// Write info
		stream.WriteString(pMusic->name, sizeof(PXMOD_MUSIC_STRUCT::name));

		// Write type
		TEST_MUSIC_TYPE_FOR_WRITING(org, ORGANYA);
		TEST_MUSIC_TYPE_FOR_WRITING(ptcop, PTCOP);
		TEST_MUSIC_TYPE_FOR_WRITING(wav, WAV);
		TEST_MUSIC_TYPE_FOR_WRITING(mp3, MP3);
		TEST_MUSIC_TYPE_FOR_WRITING(ogg, OGG);

		// If none of the types above exist, then we're going to just set it as invalid
		pMusic->type = PXMOD_MUSIC_TYPE::PXMOD_MUSIC_TYPE_INVALID;
		stream.WriteRangedInt(pMusic->type, 0, PXMOD_MUSIC_TYPE::PXMOD_MUSIC_TYPE_COUNT);
	}
	
	// Write bullets
	for (i = 0; i < mBullets.count; i++)
	{
		PXMOD_BULLET_STRUCT* pBullet = &mBullets.list[i];

		// Read info for this bullet
		stream.WriteString(pBullet->name, sizeof(pBullet->name));
		stream.WriteInt(pBullet->flags, sizeof(unsigned short) << 3);
		stream.WriteInt(pBullet->damage, sizeof(unsigned short) << 3);
		stream.WriteInt(pBullet->hit_life, sizeof(unsigned short) << 3);
		stream.WriteInt(pBullet->range, sizeof(unsigned short) << 3);
		stream.WriteInt(pBullet->bullet_size.w, sizeof(unsigned int) << 3);
		stream.WriteInt(pBullet->bullet_size.h, sizeof(unsigned int) << 3);
		stream.WriteInt(pBullet->collision_size.w, sizeof(unsigned int) << 3);
		stream.WriteInt(pBullet->collision_size.h, sizeof(unsigned int) << 3);
		stream.WriteInt(pBullet->sprite.left, sizeof(unsigned int) << 3);
		stream.WriteInt(pBullet->sprite.top, sizeof(unsigned int) << 3);
		stream.WriteInt(pBullet->sprite.right, sizeof(unsigned int) << 3);
		stream.WriteInt(pBullet->sprite.bottom, sizeof(unsigned int) << 3);
	}

	// Read weapons
	for (i = 0; i < mWeapons.count; i++)
	{
		PXMOD_WEAPON_STRUCT* pWeapon = &mWeapons.list[i];

		// Read basic info
		stream.WriteString(pWeapon->name, sizeof(pWeapon->name) - 1);

		// Read display info
		stream.WriteInt(pWeapon->display_info.left, sizeof(unsigned int) << 3);
		stream.WriteInt(pWeapon->display_info.top, sizeof(unsigned int) << 3);
		stream.WriteInt(pWeapon->display_info.right, sizeof(unsigned int) << 3);
		stream.WriteInt(pWeapon->display_info.bottom, sizeof(unsigned int) << 3);
		stream.WriteInt(pWeapon->display_info.hold_offset_x, sizeof(unsigned int) << 3);
		stream.WriteInt(pWeapon->display_info.hold_offset_y, sizeof(unsigned int) << 3);

		// Read level info
		for (j = 0; j < 3; j++)
		{
			stream.WriteRangedInt(pWeapon->level_info[j].bullet_index, 0, mBullets.count);
			stream.WriteInt(pWeapon->level_info[j].exp, sizeof(unsigned int) << 3);
			stream.WriteInt(pWeapon->level_info[j].view.left, sizeof(unsigned int) << 3);
			stream.WriteInt(pWeapon->level_info[j].view.top, sizeof(unsigned int) << 3);
			stream.WriteInt(pWeapon->level_info[j].view.right, sizeof(unsigned int) << 3);
			stream.WriteInt(pWeapon->level_info[j].view.bottom, sizeof(unsigned int) << 3);
		}
	}

	// Write NPCs
	for (i = 0; i < mNpcs.count; i++)
	{
		PXMOD_NPC_STRUCT* pNpc = &mNpcs.list[i];
		
		// ALWAYS save the NPC's name.
		stream.WriteString(pNpc->name, sizeof(pNpc->name));

		if (!bRelease)
		{
			stream.WriteString(pNpc->category, sizeof(pNpc->category));
			stream.WriteString(pNpc->shortname1, sizeof(pNpc->shortname1));
			stream.WriteString(pNpc->shortname2, sizeof(pNpc->shortname2));
			stream.WriteString(pNpc->option1, sizeof(pNpc->option1));
			stream.WriteString(pNpc->option2, sizeof(pNpc->option2));
			stream.WriteString(pNpc->option3, sizeof(pNpc->option3));
			stream.WriteInt(pNpc->spritesheet_index, sizeof(unsigned int) << 3);
		}

		stream.Write(&pNpc->nodes,				sizeof(unsigned long long int) << 3);
		stream.Write(&pNpc->transmit_nodes,		sizeof(unsigned long long int) << 3);
		stream.WriteInt(pNpc->bits,				sizeof(unsigned int) << 3);
		stream.WriteInt(pNpc->life,				sizeof(unsigned short) << 3);
		stream.WriteInt(pNpc->surf,				sizeof(unsigned char) << 3);
		stream.WriteInt(pNpc->hit_voice,		sizeof(unsigned char) << 3);
		stream.WriteInt(pNpc->destroy_voice,	sizeof(unsigned char) << 3);
		stream.WriteInt(pNpc->size,				sizeof(unsigned char) << 3);
		stream.WriteInt(pNpc->exp,				sizeof(unsigned long) << 3);
		stream.WriteInt(pNpc->damage,			sizeof(unsigned long) << 3);
		stream.WriteInt(pNpc->hit.front,		sizeof(unsigned char) << 3);
		stream.WriteInt(pNpc->hit.back,			sizeof(unsigned char) << 3);
		stream.WriteInt(pNpc->hit.top,			sizeof(unsigned char) << 3);
		stream.WriteInt(pNpc->hit.bottom,		sizeof(unsigned char) << 3);
		stream.WriteInt(pNpc->view.front,		sizeof(unsigned char) << 3);
		stream.WriteInt(pNpc->view.back,		sizeof(unsigned char) << 3);
		stream.WriteInt(pNpc->view.top,			sizeof(unsigned char) << 3);
		stream.WriteInt(pNpc->view.bottom,		sizeof(unsigned char) << 3);
		stream.WriteInt(pNpc->sprite.left,		sizeof(unsigned int) << 3);
		stream.WriteInt(pNpc->sprite.top,		sizeof(unsigned int) << 3);
		stream.WriteInt(pNpc->sprite.right,		sizeof(unsigned int) << 3);
		stream.WriteInt(pNpc->sprite.bottom,	sizeof(unsigned int) << 3);
		stream.WriteInt(pNpc->acts.count,		sizeof(unsigned int) << 3);

		for (int j = 0; j < pNpc->acts.count; j++)
		{
			PXMOD_NPC_ACT_STRUCT* pAct = &pNpc->acts.list[j];

			if (!bRelease)
				stream.WriteString(pAct->name, sizeof(pAct->name));

			stream.WriteInt(pAct->act_no, sizeof(int) << 3);
			stream.Write(&pAct->nodes, sizeof(unsigned long long int) << 3);
			stream.Write(&pAct->transmit_nodes, sizeof(unsigned long long int) << 3);
		}
	}

	// Free the resource table
	free(pResourceUsedTable);

	// Save the entire bitstream into a file
	{
		FILE* fp;

		// Build the path to the mod file
		if (bRelease)
			sprintf_s(buffer, sizeof(buffer), "%s/mod.pxmod", mModPath);
		else
			sprintf_s(buffer, sizeof(buffer), "%s/dbg-mod.pxmod", mModPath);

		// Attempt to open the file for writing
		if ((fp = fopen(buffer, "wb")) == NULL)
		{
			printf("ERROR: " __FUNCTION__ "() - Failed to open file \"%s\"\r\n", mModPath);
			return false;
		}

		fwrite(stream.mBuffer, stream.mBufferLen, 1, fp);

		// We're finished with the file, so close it.
		fclose(fp);

		// EXPORT!!
#ifdef PXMOD_EXPORT_DEFAULTS
		if ((fp = fopen("test.txt", "wb")) != NULL)
		{
			fprintf(fp, "PXMOD_WEAPON_STRUCT pDefaultWeapons[] =\r\n{\r\n");

			for (int i = 0; i < mWeapons.count; i++)
			{
				PXMOD_WEAPON_STRUCT* pWeapon = &mWeapons.list[i];

				fprintf(fp, "\t{ {\r\n");
				for (int j = 0; j < 3; j++)
				{
					fprintf(fp, "\t\t{ %d, %d, { %d, %d, %d, %d } }%s",
						pWeapon->level_info[j].bullet_index,
						pWeapon->level_info[j].exp,
						pWeapon->level_info[j].view.left,
						pWeapon->level_info[j].view.top,
						pWeapon->level_info[j].view.right,
						pWeapon->level_info[j].view.bottom,
						(j == 2 ? "\r\n" : ",\r\n")
						);
				}
				fprintf(fp, "\t}, { %d, %d, %d, %d, %d, %d }, \"%s\" }%s",
					pWeapon->display_info.left,
					pWeapon->display_info.top,
					pWeapon->display_info.right,
					pWeapon->display_info.bottom,
					pWeapon->display_info.hold_offset_x,
					pWeapon->display_info.hold_offset_y,
					pWeapon->name,
					(i == mWeapons.count - 1 ? "\r\n" : ",\r\n")
				);
			}

			fprintf(fp, " };\r\n");

			//-----------------------

			fprintf(fp, "PXMOD_BULLET_STRUCT pDefaultBullets[] =\r\n{\r\n");

			for (int i = 0; i < mBullets.count; i++)
			{
				PXMOD_BULLET_STRUCT* pBullet = &mBullets.list[i];

				fprintf(fp, "\t{ %d, %d, %d, %d, { %d, %d }, { %d, %d }, { %d, %d, %d, %d }, \"%s\" }%s",
					pBullet->flags,
					pBullet->damage,
					pBullet->hit_life,
					pBullet->range,
					pBullet->bullet_size.w,
					pBullet->bullet_size.h,
					pBullet->collision_size.w,
					pBullet->collision_size.h,
					pBullet->sprite.left,
					pBullet->sprite.right,
					pBullet->sprite.top,
					pBullet->sprite.bottom,
					pBullet->name,
					(i == mBullets.count - 1 ? "\r\n" : ",\r\n")
				);
			}

			fprintf(fp, " };\r\n");

			//-----------------------

			fprintf(fp, "PXMOD_NPC_STRUCT pDefaultNPCs[] =\r\n{\r\n");

			for (int i = 0; i < mNpcs.count; i++)
			{
				PXMOD_NPC_STRUCT* pNpc = &mNpcs.list[i];

				fprintf(fp, "\t{ \"%s\", %llu, %llu, %d, %d, %d, %d, %d, %d, %d, %d, { %d, %d, %d, %d }, { %d, %d, %d, %d }, { %d, %d, %d, %d }, %d, false, false }%s",
					pNpc->name,
					pNpc->nodes,
					pNpc->transmit_nodes,
					pNpc->bits,
					pNpc->life,
					pNpc->surf,
					pNpc->hit_voice,
					pNpc->destroy_voice,
					pNpc->size,
					pNpc->exp,
					pNpc->damage,
					pNpc->hit.front,
					pNpc->hit.back,
					pNpc->hit.top,
					pNpc->hit.bottom,
					pNpc->view.front,
					pNpc->view.back,
					pNpc->view.top,
					pNpc->view.bottom,
					pNpc->sprite.left,
					pNpc->sprite.top,
					pNpc->sprite.right,
					pNpc->sprite.bottom,
					pNpc->spritesheet_index,
					(i == mNpcs.count - 1 ? "\r\n" : ",\r\n")
				);
			}

			fprintf(fp, " };\r\n");
			fclose(fp);
		}
#endif
	}

	stream.Clear();

	// Done!
	return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ModConfigResource::Reset()
{
	// Free memory
	if (mBackgrounds.list)  { free(mBackgrounds.list);  mBackgrounds.list  = NULL; }
	if (mSpritesheets.list) { free(mSpritesheets.list); mSpritesheets.list = NULL; }
	if (mTilesets.list)     { free(mTilesets.list);     mTilesets.list     = NULL; }
	if (mStages.list)       { free(mStages.list);       mStages.list       = NULL; }
	if (mAreas.list)        { free(mAreas.list);        mAreas.list        = NULL; }
	if (mBullets.list)      { free(mBullets.list);      mBullets.list      = NULL; }
	if (mWeapons.list)      { free(mWeapons.list);      mWeapons.list      = NULL; }
	if (mNpcs.list)
	{
		for (int i = 0; i < mNpcs.count; i++)
			if (mNpcs.list[i].acts.list)
				free(mNpcs.list[i].acts.list);

		free(mNpcs.list);
		mNpcs.list = NULL;
	}

	// Reset the mod path
	*mModPath = 0;

	// Clear all chunk information
	memset(&mHeader,       0, sizeof(mHeader));
	memset(&mStart,        0, sizeof(mStart));
	memset(&mTitle,        0, sizeof(mTitle));
	memset(&mBackgrounds,  0, sizeof(mBackgrounds));
	memset(&mSpritesheets, 0, sizeof(mSpritesheets));
	memset(&mTilesets,     0, sizeof(mTilesets));
	memset(&mStages,       0, sizeof(mStages));
	memset(&mAreas,        0, sizeof(mAreas));
	memset(&mBullets,      0, sizeof(mBullets));
	memset(&mWeapons,      0, sizeof(mWeapons));
	memset(&mNpcs,         0, sizeof(mNpcs));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ModConfigResource::SetTitle(const char* mTitle)
{
	strcpy_s(mHeader.title, sizeof(mHeader.title), mTitle);
}

void ModConfigResource::SetAuthor(const char* author)
{
	strcpy_s(mHeader.author, sizeof(mHeader.author), author);
}

void ModConfigResource::SetVersion(const PXMOD_VERSION_STRUCT& pNewVersion)
{
	mHeader.version.data[0] = pNewVersion.data[0];
	mHeader.version.data[1] = pNewVersion.data[1];
	mHeader.version.data[2] = pNewVersion.data[2];
	mHeader.version.data[3] = pNewVersion.data[3];
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* ModConfigResource::GetInternalName()
{
	char* iCache = NULL;

	return ((iCache = strrchr(mModPath, '/')) != NULL ? iCache + 1 : mModPath);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ModConfigResource::AddBackground(const char* pFileName)
{
	int iIndex = 0;

	char file[_MAX_PATH];
	sprintf(file, "%s/bk%s.png", mModPath, pFileName);

	// If it already exists, then just return the index at which it resides.
	if ((iIndex = FindBackground(file)) != -1)
		return iIndex;

	// Check for validity
	if (!DoesFileExist(file))
	{
		printf("ERROR: " __FUNCTION__ "() - Background \"%s\" does not exist.\r\n", file);
		return -1;
	}

	// Resize array to fit a new entity
	if (!mBackgrounds.list)
	{
		mBackgrounds.count = 1;
		mBackgrounds.list  = (PXMOD_BACKGROUND_STRUCT*)malloc(sizeof(PXMOD_BACKGROUND_STRUCT));
	}
	else
	{
		mBackgrounds.count += 1;
		mBackgrounds.list   = (PXMOD_BACKGROUND_STRUCT*)realloc((void*)mBackgrounds.list, sizeof(PXMOD_BACKGROUND_STRUCT) * mBackgrounds.count);
	}

	// Setup the new background
	PXMOD_BACKGROUND_STRUCT* pBackground = &mBackgrounds.list[mBackgrounds.count - 1];
	strcpy_s(pBackground->name, sizeof(pBackground->name), pFileName);

	return mBackgrounds.count - 1;
}

bool ModConfigResource::RemoveBackground(const char* pFileName)
{
	int iIndex = FindBackground(pFileName);

	// Don't remove invalid indicies
	if (iIndex == -1)
		return false;

	// Erase from our list
	LIST_ERASE(mBackgrounds.list, mBackgrounds.count, PXMOD_BACKGROUND_STRUCT, iIndex);

	// Fix stage pointers
	for (int i = 0; i < mStages.count; i++)
	{
		PXMOD_STAGE_STRUCT* pStage = &mStages.list[i];
		
		// Skip entries that don't point to us
		if (pStage->background != iIndex)
			continue;

		// Reset to a possible value
		pStage->background = 0;
	}

	// Shrink the list
	if (!--mBackgrounds.count)
	{
		free(mBackgrounds.list);
		mBackgrounds.list = NULL;
	}
	else
	{
		mBackgrounds.list = (PXMOD_BACKGROUND_STRUCT*)realloc((void*)mBackgrounds.list, sizeof(PXMOD_BACKGROUND_STRUCT) * mBackgrounds.count);
	}

	// Done!
	return true;
}

int ModConfigResource::FindBackground(const char* pFileName)
{
	for (int i = 0; i < mBackgrounds.count; i++)
	{
		// Mis-match
		if (stricmp(mBackgrounds.list[i].name, pFileName))
			continue;

		return i;
	}

	return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ModConfigResource::AddTileset(const char* pFileName)
{
	int iIndex = 0;

	char file[_MAX_PATH];
	sprintf(file, "%s/Stage/Prt%s.png", mModPath, pFileName);

	// If it already exists, then just return the index at which it resides.
	if ((iIndex = FindTileset(file)) != -1)
		return iIndex;

	// Check for validity
	if (!DoesFileExist(file))
	{
		printf("ERROR: " __FUNCTION__ "() - Map tileset \"%s\" does not exist.\r\n", file);
		return -1;
	}

	// Resize array to fit a new entity
	if (!mTilesets.list)
	{
		mTilesets.count = 1;
		mTilesets.list  = (PXMOD_TILESET_STRUCT*)malloc(sizeof(PXMOD_TILESET_STRUCT));
	}
	else
	{
		mTilesets.count += 1;
		mTilesets.list   = (PXMOD_TILESET_STRUCT*)realloc((void*)mTilesets.list, sizeof(PXMOD_TILESET_STRUCT) * mTilesets.count);
	}

	// Setup the new tileset
	PXMOD_TILESET_STRUCT* pTileset = &mTilesets.list[mTilesets.count - 1];
	strcpy_s(pTileset->name, sizeof(pTileset->name), pFileName);

	return mTilesets.count - 1;
}

bool ModConfigResource::RemoveTileset(const char* pFileName)
{
	int iIndex = FindTileset(pFileName);

	// Don't remove invalid indicies
	if (iIndex == -1)
		return false;

	// Erase from our list
	LIST_ERASE(mTilesets.list, mTilesets.count, PXMOD_TILESET_STRUCT, iIndex);

	// Fix stage pointers
	for (int i = 0; i < mStages.count; i++)
	{
		PXMOD_STAGE_STRUCT* pStage = &mStages.list[i];
		
		// Skip entries that don't point to us
		if (pStage->tileset != iIndex)
			continue;

		// Reset to a possible value
		pStage->tileset = 0;
	}

	// Shrink the list
	if (!--mTilesets.count)
	{
		free(mTilesets.list);
		mTilesets.list = NULL;
	}
	else
	{
		mTilesets.list = (PXMOD_TILESET_STRUCT*)realloc((void*)mTilesets.list, sizeof(PXMOD_TILESET_STRUCT) * mTilesets.count);
	}

	// Done!
	return true;
}

int ModConfigResource::FindTileset(const char* pFileName)
{
	for (int i = 0; i < mTilesets.count; i++)
	{
		// Mis-match
		if (stricmp(mTilesets.list[i].name, pFileName))
			continue;

		return i;
	}

	return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ModConfigResource::AddSpritesheet(const char* pFileName)
{
	int iIndex = 0;

	char file[_MAX_PATH];
	sprintf(file, "%s/Npc/Npc%s.png", mModPath, pFileName);

	// If it already exists, then just return the index at which it resides.
	if ((iIndex = FindSpritesheet(file)) != -1)
		return iIndex;

	// Check for validity
	if (!DoesFileExist(file))
	{
		printf("ERROR: " __FUNCTION__ "() - NPC Spritesheet \"%s\" does not exist.\r\n", file);
		return -1;
	}

	// Resize array to fit a new entity
	if (!mSpritesheets.list)
	{
		mSpritesheets.count = 1;
		mSpritesheets.list  = (PXMOD_SPRITESHEET_STRUCT*)malloc(sizeof(PXMOD_SPRITESHEET_STRUCT));
	}
	else
	{
		mSpritesheets.count += 1;
		mSpritesheets.list   = (PXMOD_SPRITESHEET_STRUCT*)realloc((void*)mSpritesheets.list, sizeof(PXMOD_SPRITESHEET_STRUCT) * mSpritesheets.count);
	}

	// Setup the new spritesheet
	PXMOD_SPRITESHEET_STRUCT* pSSheet = &mSpritesheets.list[mSpritesheets.count - 1];
	strcpy_s(pSSheet->name, sizeof(pSSheet->name), pFileName);

	return mSpritesheets.count - 1;
}

bool ModConfigResource::RemoveSpritesheet(const char* pFileName)
{
	int iIndex = FindSpritesheet(pFileName);

	// Don't remove invalid indicies
	if (iIndex == -1)
		return false;

	// Erase from our list
	LIST_ERASE(mSpritesheets.list, mSpritesheets.count, PXMOD_SPRITESHEET_STRUCT, iIndex);

	// Fix stage pointers
	for (int i = 0; i < mStages.count; i++)
	{
		PXMOD_STAGE_STRUCT* pStage = &mStages.list[i];
		
		// Multiple members in stage could point to us.
		if (pStage->npc_sheet == iIndex)
			pStage->npc_sheet = 0;

		if (pStage->boss_sheet == iIndex)
			pStage->boss_sheet = 0;
	}

	// Shrink the list
	if (!--mSpritesheets.count)
	{
		free(mSpritesheets.list);
		mSpritesheets.list = NULL;
	}
	else
	{
		mSpritesheets.list = (PXMOD_SPRITESHEET_STRUCT*)realloc((void*)mSpritesheets.list, sizeof(PXMOD_SPRITESHEET_STRUCT) * mSpritesheets.count);
	}

	// Done!
	return true;
}

int ModConfigResource::FindSpritesheet(const char* pFileName)
{
	for (int i = 0; i < mSpritesheets.count; i++)
	{
		// Mis-match
		if (stricmp(mSpritesheets.list[i].name, pFileName))
			continue;

		return i;
	}

	return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ModConfigResource::AddStage(const char* pFileName, const char* pMapName, const char* pTileset, const char* pNpcSheet, const char* pBossSheet, const char* pBackground, PXMOD_BK_TYPE iBkType, PXMOD_BOSS_TYPE iBossType)
{
	int iCache;

	// Resize array to fit a new entity
	if (!mStages.list)
	{
		mStages.count = 1;
		mStages.list  = (PXMOD_STAGE_STRUCT*)malloc(sizeof(PXMOD_STAGE_STRUCT));
	}
	else
	{
		mStages.count += 1;
		mStages.list   = (PXMOD_STAGE_STRUCT*)realloc((void*)mStages.list, sizeof(PXMOD_STAGE_STRUCT) * mStages.count);
	}

	memset(&mStages.list[mStages.count - 1], 0, sizeof(PXMOD_STAGE_STRUCT));

	// Add the zone
	PXMOD_STAGE_STRUCT* pStage = &mStages.list[mStages.count - 1];
	
	// Configure the new stage
	strcpy_s(pStage->file, sizeof(pStage->file), pFileName);
	strcpy_s(pStage->name, sizeof(pStage->name), pMapName);
	
	pStage->boss_no    = iBossType;
	pStage->bkType     = iBkType;
	pStage->background = ((iCache = FindBackground(pBackground))  == -1 ? AddBackground(pBackground) : iCache);
	pStage->npc_sheet  = ((iCache = FindSpritesheet(pNpcSheet))   == -1 ? AddSpritesheet(pNpcSheet)  : iCache);
	pStage->boss_sheet = ((iCache = FindSpritesheet(pBossSheet))  == -1 ? AddSpritesheet(pBossSheet) : iCache);
	pStage->tileset    = ((iCache = FindTileset(pTileset))        == -1 ? AddTileset(pTileset)       : iCache);

	if (pStage->background == -1) pStage->background = ((iCache = FindBackground("0"))  == -1 ? AddBackground("0")  : iCache);
	if (pStage->npc_sheet  == -1) pStage->npc_sheet  = ((iCache = FindSpritesheet("0")) == -1 ? AddSpritesheet("0") : iCache);
	if (pStage->boss_sheet == -1) pStage->boss_sheet = ((iCache = FindSpritesheet("0")) == -1 ? AddSpritesheet("0") : iCache);
	if (pStage->tileset    == -1) pStage->tileset    = ((iCache = FindTileset("0"))     == -1 ? AddTileset("0")     : iCache);

	return mStages.count - 1;
}

int ModConfigResource::SetStage(int iIndex, const char* pFileName, const char* pMapName, const char* pTileset, const char* pNpcSheet, const char* pBossSheet, const char* pBackground, PXMOD_BK_TYPE iBkType, PXMOD_BOSS_TYPE iBossType)
{
	if (iIndex < 0 || iIndex >= 1000)
		return -1;

	int iCache;

	// Resize array to fit a new entity
	if (iIndex >= mStages.count)
	{
		int Start     = mStages.count;
		mStages.count = iIndex + 1;

		if (!mStages.list)
		{
			mStages.list = (PXMOD_STAGE_STRUCT*)malloc(mStages.count * sizeof(PXMOD_STAGE_STRUCT));
		}
		else
		{
			mStages.list = (PXMOD_STAGE_STRUCT*)realloc((void*)mStages.list, mStages.count * sizeof(PXMOD_STAGE_STRUCT));
		}

		memset((void*)(((const char*)mStages.list) + (Start * sizeof(PXMOD_STAGE_STRUCT))), 0, (mStages.count - Start) * sizeof(PXMOD_STAGE_STRUCT));
	}

	memset(&mStages.list[iIndex], 0, sizeof(PXMOD_STAGE_STRUCT));

	// Add the zone
	PXMOD_STAGE_STRUCT* pStage = &mStages.list[iIndex];
	
	// Configure the new stage
	strcpy_s(pStage->file, sizeof(pStage->file), pFileName);
	strcpy_s(pStage->name, sizeof(pStage->name), pMapName);
	
	pStage->boss_no    = iBossType;
	pStage->bkType     = iBkType;
	pStage->background = ((iCache = FindBackground(pBackground))  == -1 ? AddBackground(pBackground) : iCache);
	pStage->npc_sheet  = ((iCache = FindSpritesheet(pNpcSheet))   == -1 ? AddSpritesheet(pNpcSheet)  : iCache);
	pStage->boss_sheet = ((iCache = FindSpritesheet(pBossSheet))  == -1 ? AddSpritesheet(pBossSheet) : iCache);
	pStage->tileset    = ((iCache = FindTileset(pTileset))        == -1 ? AddTileset(pTileset)       : iCache);

	if (pStage->background == -1) pStage->background = ((iCache = FindBackground("0"))  == -1 ? AddBackground("0")  : iCache);
	if (pStage->npc_sheet  == -1) pStage->npc_sheet  = ((iCache = FindSpritesheet("0")) == -1 ? AddSpritesheet("0") : iCache);
	if (pStage->boss_sheet == -1) pStage->boss_sheet = ((iCache = FindSpritesheet("0")) == -1 ? AddSpritesheet("0") : iCache);
	if (pStage->tileset    == -1) pStage->tileset    = ((iCache = FindTileset("0"))     == -1 ? AddTileset("0")     : iCache);

	return iIndex;
}

bool ModConfigResource::RemoveStage(const char* pFileName)
{
	int iIndex = FindStage(pFileName);

	// Don't remove invalid indicies
	if (iIndex == -1)
		return false;

	// Erase from our list
	LIST_ERASE(mStages.list, mStages.count, PXMOD_STAGE_STRUCT, iIndex);

	// Shrink the list
	if (!--mStages.count)
	{
		free(mStages.list);
		mStages.list = NULL;
	}
	else
	{
		mStages.list = (PXMOD_STAGE_STRUCT*)realloc((void*)mStages.list, sizeof(PXMOD_STAGE_STRUCT) * mStages.count);
	}

	// Done!
	return true;
}

int ModConfigResource::FindStage(const char* pFileName)
{
	for (int i = 0; i < mStages.count; i++)
	{
		// Mis-match
		if (stricmp(mStages.list[i].file, pFileName))
			continue;

		return i;
	}

	return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ModConfigResource::AddArea(const char* pAreaName)
{
	int iIndex = 0;
	char spBuffer[sizeof(PXMOD_AREA_STRUCT::name)];

	// Make sure we don't go over the name limit
	strcpy_s(spBuffer, sizeof(spBuffer), pAreaName);

	// If it already exists, then just return the index at which it resides.
	if ((iIndex = FindArea(spBuffer)) != -1)
		return iIndex;

	// Resize array to fit a new entity
	if (!mAreas.list)
	{
		mAreas.count = 1;
		mAreas.list  = (PXMOD_AREA_STRUCT*)malloc(sizeof(PXMOD_AREA_STRUCT));
	}
	else
	{
		mAreas.count += 1;
		mAreas.list   = (PXMOD_AREA_STRUCT*)realloc((void*)mAreas.list, sizeof(PXMOD_AREA_STRUCT) * mAreas.count);
	}

	// Setup the new area
	PXMOD_AREA_STRUCT* pArea = &mAreas.list[mAreas.count - 1];

	// Configure the area
	strcpy_s(pArea->name, sizeof(pArea->name), spBuffer);
	pArea->stage_idx         = 0;
	pArea->singleplayer_only = false;
	pArea->keep_on_release   = true;

	// Done!
	return mAreas.count - 1;
}

bool ModConfigResource::RemoveArea(const char* pAreaName)
{
	int iIndex = FindArea(pAreaName);

	// Don't remove invalid indicies
	if (iIndex == -1)
		return false;

	// Erase from our list
	LIST_ERASE(mAreas.list, mAreas.count, PXMOD_AREA_STRUCT, iIndex);

	// Shrink the list
	if (!--mAreas.count)
	{
		free(mAreas.list);
		mAreas.list = NULL;
	}
	else
	{
		mAreas.list = (PXMOD_AREA_STRUCT*)realloc((void*)mAreas.list, sizeof(PXMOD_AREA_STRUCT) * mAreas.count);
	}

	// Remove this area from all stages
	for (int i = 0; i < mStages.count; i++)
		mStages.list[i].area_index = (!mAreas.count ? 0 : CLAMP(mStages.list[i].area_index, 0, (mAreas.count - 1)));

	// Done!
	return true;
}

int ModConfigResource::FindArea(const char* pAreaName)
{
	// Find the area by name
	for (int i = 0; i < mAreas.count; i++)
	{
		if (!stricmp(mAreas.list[i].name, pAreaName))
			return i;
	}

	// Didn't find it
	return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ModConfigResource::AddMusic(const char* pMusicName)
{
	int iIndex = 0;

	// If it already exists, then just return the index at which it resides.
	if ((iIndex = FindMusic(pMusicName)) != -1)
		return iIndex;

	// Resize array to fit a new entity
	if (!mMusic.list)
	{
		mMusic.count = 1;
		mMusic.list  = (PXMOD_MUSIC_STRUCT*)malloc(sizeof(PXMOD_MUSIC_STRUCT));
	}
	else
	{
		mMusic.count += 1;
		mMusic.list   = (PXMOD_MUSIC_STRUCT*)realloc((void*)mMusic.list, sizeof(PXMOD_MUSIC_STRUCT) * mMusic.count);
	}

	// Setup the new area
	PXMOD_MUSIC_STRUCT* pMusic = &mMusic.list[mMusic.count - 1];

	// Configure the area
	strcpy_s(pMusic->name, sizeof(pMusic->name), pMusicName);

	// Done!
	return mMusic.count - 1;
}

bool ModConfigResource::RemoveMusic(const char* pMusicName)
{
	int iIndex = FindMusic(pMusicName);

	// Don't remove invalid indicies
	if (iIndex == -1)
		return false;

	// Erase from our list
	LIST_ERASE(mMusic.list, mMusic.count, PXMOD_MUSIC_STRUCT, iIndex);

	// Shrink the list
	if (!--mMusic.count)
	{
		free(mMusic.list);
		mMusic.list = NULL;
	}
	else
	{
		mMusic.list = (PXMOD_MUSIC_STRUCT*)realloc((void*)mMusic.list, sizeof(PXMOD_MUSIC_STRUCT) * mMusic.count);
	}

	// Done!
	return true;
}

int ModConfigResource::FindMusic(const char* pMusicName)
{
	// Find the area by name
	for (int i = 0; i < mMusic.count; i++)
	{
		if (!stricmp(mMusic.list[i].name, pMusicName))
			return i;
	}

	// Didn't find it
	return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ModConfigResource::AddWeapon(const char* pWeaponName)
{
	int iIndex = 0;

	// If it already exists, then just return the index at which it resides.
	//if ((iIndex = FindWeapon(pWeaponName)) != -1)
//		return iIndex;

	// Resize array to fit a new weapon
	if (!mWeapons.list)
	{
		mWeapons.count = 1;
		mWeapons.list  = (PXMOD_WEAPON_STRUCT*)malloc(sizeof(PXMOD_WEAPON_STRUCT));
	}
	else
	{
		mWeapons.count += 1;
		mWeapons.list   = (PXMOD_WEAPON_STRUCT*)realloc((void*)mWeapons.list, sizeof(PXMOD_WEAPON_STRUCT) * mWeapons.count);
	}

	// Setup the new weapon
	PXMOD_WEAPON_STRUCT* pWeapon = &mWeapons.list[mWeapons.count - 1];

	// Configure the weapon
	memset(pWeapon, 0, sizeof(PXMOD_WEAPON_STRUCT));
	strcpy_s(pWeapon->name, sizeof(pWeapon->name), pWeaponName);

	// Done!
	return mWeapons.count - 1;
}

bool ModConfigResource::RemoveWeapon(const char* pWeaponName)
{
	int iIndex = FindWeapon(pWeaponName);

	// Don't remove invalid indicies
	if (iIndex == -1)
		return false;

	// Erase from our list
	LIST_ERASE(mWeapons.list, mWeapons.count, PXMOD_WEAPON_STRUCT, iIndex);

	// Shrink the list
	if (!--mWeapons.count)
	{
		free(mWeapons.list);
		mWeapons.list = NULL;
	}
	else
	{
		mWeapons.list = (PXMOD_WEAPON_STRUCT*)realloc((void*)mWeapons.list, sizeof(PXMOD_WEAPON_STRUCT) * mWeapons.count);
	}

	// Done!
	return true;
}

int ModConfigResource::FindWeapon(const char* pWeaponName)
{
	// Find the area by name
	for (int i = 0; i < mWeapons.count; i++)
	{
		if (!stricmp(mWeapons.list[i].name, pWeaponName))
			return i;
	}

	// Didn't find it
	return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ModConfigResource::AddBullet(const char* pBulletName)
{
	int iIndex = 0;

	// If it already exists, then just return the index at which it resides.
	//if ((iIndex = FindBullet(pBulletName)) != -1)
		//return iIndex;

	// Resize array to fit a new weapon
	if (!mBullets.list)
	{
		mBullets.count = 1;
		mBullets.list  = (PXMOD_BULLET_STRUCT*)malloc(sizeof(PXMOD_BULLET_STRUCT));
	}
	else
	{
		mBullets.count += 1;
		mBullets.list   = (PXMOD_BULLET_STRUCT*)realloc((void*)mBullets.list, sizeof(PXMOD_BULLET_STRUCT) * mBullets.count);
	}

	// Setup the new weapon
	PXMOD_BULLET_STRUCT* pBullet = &mBullets.list[mBullets.count - 1];

	// Configure the weapon
	memset(pBullet, 0, sizeof(PXMOD_BULLET_STRUCT));
	strcpy_s(pBullet->name, sizeof(pBullet->name), pBulletName);

	// Done!
	return mBullets.count - 1;
}

bool ModConfigResource::RemoveBullet(const char* pBulletName)
{
	int iIndex = FindBullet(pBulletName);

	// Don't remove invalid indicies
	if (iIndex == -1)
		return false;

	// Erase from our list
	LIST_ERASE(mBullets.list, mBullets.count, PXMOD_BULLET_STRUCT, iIndex);

	// Fix weapon pointers
	for (int i = 0; i < mWeapons.count; i++)
	{
		PXMOD_WEAPON_STRUCT* pWeapon = &mWeapons.list[i];

		// Set them to possible values if they match us
		if (pWeapon->level_info[0].bullet_index == iIndex)
			pWeapon->level_info[0].bullet_index = 0;
		if (pWeapon->level_info[1].bullet_index == iIndex)
			pWeapon->level_info[1].bullet_index = 0;
		if (pWeapon->level_info[2].bullet_index == iIndex)
			pWeapon->level_info[2].bullet_index = 0;
	}

	// Shrink the list
	if (!--mBullets.count)
	{
		free(mBullets.list);
		mBullets.list = NULL;
	}
	else
	{
		mBullets.list = (PXMOD_BULLET_STRUCT*)realloc((void*)mBullets.list, sizeof(PXMOD_BULLET_STRUCT) * mBullets.count);
	}

	// Done!
	return true;
}

int ModConfigResource::FindBullet(const char* pBulletName)
{
	// Find the area by name
	for (int i = 0; i < mBullets.count; i++)
	{
		if (!stricmp(mBullets.list[i].name, pBulletName))
			return i;
	}

	// Didn't find it
	return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ModConfigResource::AddNPC(const char* pNpcName)
{
	int iIndex = 0;

	// Resize array to fit a new entity
	if (!mNpcs.list)
	{
		mNpcs.count = 1;
		mNpcs.list  = (PXMOD_NPC_STRUCT*)malloc(sizeof(PXMOD_NPC_STRUCT));
	}
	else
	{
		mNpcs.count += 1;
		mNpcs.list   = (PXMOD_NPC_STRUCT*)realloc((void*)mNpcs.list, sizeof(PXMOD_NPC_STRUCT) * mNpcs.count);
	}

	// Setup the new area
	PXMOD_NPC_STRUCT* pNpc = &mNpcs.list[mNpcs.count - 1];

	// Configure the area
	strcpy_s(pNpc->name, sizeof(pNpc->name), pNpcName);
	strcpy(pNpc->category, "N/A");
	strcpy(pNpc->shortname1, "Unkwn");
	strcpy(pNpc->shortname2, "Objct");
	strcpy(pNpc->option1, "");
	strcpy(pNpc->option2, "");
	strcpy(pNpc->option3, "");

	// Done!
	return mNpcs.count - 1;
}

bool ModConfigResource::RemoveNPC(const char* pNpcName)
{
	int iIndex = FindNPC(pNpcName);

	// Don't remove invalid indicies
	if (iIndex == -1)
		return false;

	if (mNpcs.list[iIndex].acts.list)
		free(mNpcs.list->acts.list);

	// Erase from our list
	LIST_ERASE(mNpcs.list, mNpcs.count, PXMOD_NPC_STRUCT, iIndex);

	// Shrink the list
	if (!--mNpcs.count)
	{
		free(mNpcs.list);
		mNpcs.list = NULL;
	}
	else
	{
		mNpcs.list = (PXMOD_NPC_STRUCT*)realloc((void*)mNpcs.list, sizeof(PXMOD_NPC_STRUCT) * mNpcs.count);
	}

	// Done!
	return true;
}

int ModConfigResource::FindNPC(const char* pNpcName)
{
	// Find the area by name
	for (int i = 0; i < mNpcs.count; i++)
	{
		if (!stricmp(mNpcs.list[i].name, pNpcName))
			return i;
	}

	// Didn't find it
	return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int ModConfigResource::AddNpcAct(int iNpcIndex, int iActNo, const char* pActName, unsigned long long int iNodeFlags, unsigned long long int iTransmitNodeFlags)
{
	if (iNpcIndex < 0 || iNpcIndex >= mNpcs.count)
		return -1;

	PXMOD_NPC_ACT_STRUCT* pAct = NULL;

	// Attempt to find it first
	for (int i = 0; i < mNpcs.list[iNpcIndex].acts.count; i++)
	{
		if (mNpcs.list[iNpcIndex].acts.list[i].act_no == iActNo)
		{
			pAct = &mNpcs.list[iNpcIndex].acts.list[i];
			break;
		}
	}

	if (!pAct)
	{
		// Resize array to fit a new entity
		if (!mNpcs.list[iNpcIndex].acts.list)
		{
			mNpcs.list[iNpcIndex].acts.count = 1;
			mNpcs.list[iNpcIndex].acts.list  = (PXMOD_NPC_ACT_STRUCT*)malloc(sizeof(PXMOD_NPC_ACT_STRUCT));
		}
		else
		{
			mNpcs.list[iNpcIndex].acts.count += 1;
			mNpcs.list[iNpcIndex].acts.list   = (PXMOD_NPC_ACT_STRUCT*)realloc((void*)mNpcs.list[iNpcIndex].acts.list, sizeof(PXMOD_NPC_ACT_STRUCT) * mNpcs.list[iNpcIndex].acts.count);
		}

		// Setup the new area
		pAct = &mNpcs.list[iNpcIndex].acts.list[mNpcs.list[iNpcIndex].acts.count - 1];
	}

	// Configure the area
	strcpy_s(pAct->name, sizeof(pAct->name), pActName);
	pAct->act_no         = iActNo;
	pAct->nodes          = iNodeFlags;
	pAct->transmit_nodes = iTransmitNodeFlags;

	// Done!
	return mNpcs.list[iNpcIndex].acts.count - 1;
}

bool ModConfigResource::RemoveNpcAct(int iNpcIndex, int iActNo)
{
	if (iNpcIndex < 0 || iNpcIndex >= mNpcs.count - 1)
		return false;

	PXMOD_NPC_ACT_STRUCT* pAct = NULL;
	int iIndex = 0;

	// Attempt to find it first
	for (iIndex = 0; iIndex < mNpcs.list[iNpcIndex].acts.count; iIndex++)
	{
		if (mNpcs.list[iNpcIndex].acts.list[iIndex].act_no == iActNo)
		{
			pAct = &mNpcs.list[iNpcIndex].acts.list[iIndex];
			break;
		}
	}

	if (iIndex == mNpcs.list[iNpcIndex].acts.count)
		return false;

	// Erase from our list
	LIST_ERASE(mNpcs.list[iNpcIndex].acts.list, mNpcs.list[iNpcIndex].acts.count, PXMOD_NPC_ACT_STRUCT, iIndex);

	// Shrink the list
	if (!--mNpcs.list[iNpcIndex].acts.count)
	{
		free(mNpcs.list[iNpcIndex].acts.list);
		mNpcs.list[iNpcIndex].acts.list = NULL;
	}
	else
	{
		mNpcs.list[iNpcIndex].acts.list = (PXMOD_NPC_ACT_STRUCT*)realloc((void*)mNpcs.list[iNpcIndex].acts.list, sizeof(PXMOD_NPC_ACT_STRUCT) * mNpcs.list[iNpcIndex].acts.count);
	}

	return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const PXMOD_NPC_STRUCT* ModConfigResource::GetDefaultNpc(int iIndex)
{
	static PXMOD_NPC_STRUCT gpzDefaultNPCs[] =
	{
		{"null", 0, 0, 262152, 1000, 20, 0, 0, 2, 0, 0, {4, 4, 3, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 1, false, false, "General", "", "", "", "", ""},
		{"Experience", 127166, 3275807, 0, 1, 20, 0, 0, 2, 1, 0, {4, 4, 3, 3}, {8, 8, 8, 8}, {16, 48, 32, 64}, 34, false, false, "General", "", "", "", "Weapn", "enrgy"},
		{"Behemoth", 4191231, 104854558, 32945, 8, 21, 52, 72, 3, 3, 1, {11, 11, 14, 8}, {16, 16, 16, 8}, {32, 0, 64, 24}, 2, false, false, "Egg1", "", "", "", "Enemy", "Behem"},
		{"Dead Enemy", 0, 104854558, 262152, 1, 20, 0, 0, 2, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 1, false, false, "Unsure", "", "", "", "?", ""},
		{"Smoke", 33554431, 104854558, 262144, 1, 20, 0, 0, 2, 0, 0, {4, 4, 4, 4}, {8, 8, 8, 8}, {64, 0, 80, 16}, 34, false, false, "Misc", "", "", "", "Smoke", ""},
		{"Critter (Green)", 33554431, 104854558, 163875, 4, 21, 51, 71, 2, 2, 2, {6, 6, 4, 5}, {8, 8, 8, 8}, {0, 48, 16, 64}, 2, false, false, "Egg1", "", "", "", "Enemy", "CrtHG"},
		{"Beetle (Fixed)", 33554431, 104854558, 163873, 4, 21, 53, 71, 2, 2, 2, {6, 5, 6, 6}, {8, 8, 8, 8}, {0, 80, 16, 96}, 2, false, false, "Egg1", "", "", "", "Enemy", "BtlHG"},
		{"Basil", 8393276, 34656287, 4, 0, 21, 51, 71, 2, 5, 100, {8, 8, 2, 8}, {16, 16, 8, 8}, {256, 64, 288, 80}, 2, false, false, "Egg1", "", "", "", "Enemy", "Basil"},
		{"Beetle (Homing)", 33554431, 104854558, 163881, 4, 21, 53, 71, 2, 2, 2, {6, 5, 6, 6}, {8, 8, 8, 8}, {80, 80, 96, 96}, 2, false, false, "Egg1", "", "", "", "Enemy", "BtlF1"},
		{"Balrog (Drop-in)", 33554431, 104854558, 41, 96, 22, 52, 72, 3, 18, 0, {14, 14, 11, 12}, {20, 20, 12, 12}, {0, 0, 40, 24}, 9, false, false, "Balrog", "", "", "", "Blrg", "drop"},
		{"Balrog (shooting)", 33554431, 104854558, 163873, 96, 22, 52, 72, 3, 18, 0, {14, 14, 11, 12}, {20, 20, 12, 12}, {0, 0, 40, 24}, 9, false, false, "Balrog", "", "", "", "Boss", "Blrg0"},
		{"Balrog Projectile", 33554431, 104854558, 32, 1, 20, 0, 70, 1, 0, 2, {4, 4, 4, 4}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "Balrog", "", "", "", "Proj", "Blrg1"},
		{"Balrog (cutscene)", 33554431, 104854558, 131072, 1, 22, 52, 72, 3, 18, 0, {14, 14, 11, 12}, {20, 20, 12, 12}, {0, 0, 40, 24}, 9, false, false, "Balrog", "", "", "", "Blrg", ""},
		{"Forcefield", 33492991, 104854558, 68, 0, 20, 0, 0, 0, 0, 1, {8, 8, 8, 8}, {8, 8, 8, 8}, {128, 0, 144, 16}, 34, false, false, "Misc", "", "", "", "Force", "field"},
		{"Santa's Key", 33492991, 104854558, 262144, 0, 20, 0, 0, 0, 0, 0, {5, 5, 6, 6}, {8, 8, 8, 8}, {192, 0, 208, 16}, 34, false, false, "Misc", "", "", "", "Santa", "Key"},
		{"Chest (Closed)", 66620, 3158110, 0, 0, 20, 0, 0, 0, 0, 0, {5, 5, 6, 8}, {8, 8, 8, 8}, {240, 0, 256, 16}, 34, false, false, "General", "", "", "", "Chest", "closd"},
		{"Save Point", 66108, 61470, 0, 0, 20, 0, 0, 0, 0, 0, {5, 5, 6, 8}, {8, 8, 8, 8}, {96, 16, 112, 32}, 34, false, false, "General", "", "", "", "Save", "point"},
		{"Health Refill", 66108, 61470, 0, 0, 20, 0, 0, 0, 0, 0, {5, 5, 6, 8}, {8, 8, 8, 8}, {288, 0, 304, 16}, 34, false, false, "General", "", "", "", "Refil", ""},
		{"Door", 524, 1061894, 8, 0, 20, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 16, 8}, {224, 16, 240, 40}, 34, false, false, "General", "", "", "", "Door", ""},
		{"Balrog (burst)", 33554431, 104854558, 33, 96, 22, 52, 72, 3, 18, 0, {14, 14, 11, 12}, {20, 20, 12, 12}, {0, 0, 40, 24}, 9, false, false, "Balrog", "", "", "", "Blrg", "bust"},
		{"Computer", 33492991, 104854558, 8, 0, 20, 0, 0, 0, 0, 0, {4, 4, 8, 8}, {16, 16, 16, 8}, {288, 16, 320, 40}, 34, false, false, "Misc", "", "", "", "Compu", ""},
		{"Chest (open)", 33554431, 104854558, 262144, 0, 20, 0, 0, 0, 0, 0, {5, 5, 0, 8}, {8, 8, 0, 8}, {224, 40, 240, 48}, 34, false, false, "General", "", "One tile down", "", "Chest", "open"},
		{"Teleporter", 33554431, 104854558, 0, 0, 20, 0, 0, 0, 0, 0, {10, 10, 1, 8}, {12, 12, 24, 8}, {240, 16, 264, 48}, 34, false, false, "Misc", "", "", "", "Telep", ""},
		{"Teleporter lights", 12, 3194886, 262144, 0, 20, 0, 0, 0, 0, 0, {12, 12, 4, 0}, {12, 12, 4, 0}, {264, 16, 288, 20}, 34, false, false, "Misc", "", "", "", "Telep", "light"},
		{"Power Critter", 8450685, 45151262, 163873, 16, 21, 52, 72, 3, 7, 2, {9, 9, 6, 7}, {12, 12, 12, 12}, {0, 0, 24, 24}, 4, false, false, "Grasstown", "", "", "", "Enemy", "PCrit"},
		{"Egg Corridor Lift", 572, 104854558, 13, 0, 20, 0, 0, 0, 0, 0, {14, 14, 9, 8}, {16, 16, 8, 8}, {256, 64, 288, 80}, 34, false, false, "Misc", "", "", "", "Lift", "platf"},
		{"Bat (Grasstown)", 8467068, 36762654, 163880, 4, 21, 50, 70, 1, 3, 2, {4, 4, 7, 4}, {8, 8, 8, 8}, {32, 80, 48, 96}, 4, false, false, "Grasstown", "", "", "", "Enemy", "BatBC"},
		{"Death Trap", 12, 6, 8, 0, 20, 0, 0, 0, 0, 127, {8, 24, 8, 8}, {8, 24, 16, 8}, {96, 64, 128, 88}, 34, false, false, "Misc", "", "", "", "Death", "trap"},
		{"Flying Critter (Weed)", 8515709, 45151262, 163873, 5, 21, 51, 71, 2, 3, 2, {6, 6, 4, 5}, {8, 8, 8, 8}, {0, 48, 16, 64}, 4, false, false, "Grasstown", "", "", "", "Enemy", "CrtFy"},
		{"Cthulu", 33554431, 104854558, 131072, 0, 23, 0, 0, 0, 0, 0, {4, 4, 4, 4}, {8, 8, 16, 8}, {0, 192, 16, 216}, 35, false, false, "Characters", "", "", "", "Cthu", ""},
		{"Gunsmith", 33554431, 104854558, 0, 0, 21, 0, 0, 2, 0, 0, {6, 6, 6, 6}, {8, 8, 8, 8}, {48, 0, 64, 16}, 0, false, false, "Characters", "", "", "", "Hermt", "Gunsm"},
		{"Bat (Hanging Weed)", 10564221, 36762654, 163872, 4, 21, 50, 70, 1, 3, 2, {4, 4, 7, 4}, {8, 8, 8, 8}, {0, 80, 16, 96}, 4, false, false, "Grasstown", "", "", "", "Enemy", "BatBH"},
		{"Life Capsule", 33554431, 104854558, 262144, 0, 20, 0, 0, 2, 0, 0, {6, 6, 8, 6}, {8, 8, 8, 8}, {32, 96, 48, 112}, 34, false, false, "General", "", "", "", "Capsl", "life"},
		{"Balrog Bounce Proj", 33554431, 104854558, 32, 1, 22, 0, 70, 1, 0, 2, {4, 4, 4, 4}, {8, 8, 8, 8}, {240, 64, 256, 80}, 9, false, false, "Balrog", "", "", "", "Proj", "Blrg2"},
		{"Bed", 33554431, 104854558, 262152, 0, 20, 0, 0, 0, 0, 0, {8, 8, 8, 8}, {16, 16, 8, 8}, {192, 48, 224, 64}, 34, false, false, "General", "", "", "", "Bed", ""},
		{"Mannan", 33554431, 104854558, 32808, 100, 21, 54, 0, 1, 2, 3, {4, 4, 4, 20}, {12, 12, 8, 24}, {96, 64, 120, 96}, 4, false, false, "Grasstown", "", "", "", "Enemy", "Manan"},
		{"Balrog (Hover)", 33554431, 104854558, 163873, 240, 22, 52, 72, 3, 18, 0, {14, 14, 11, 12}, {20, 20, 12, 12}, {0, 0, 40, 24}, 9, false, false, "Balrog", "", "", "", "Boss", "Blrg2"},
		{"Signpost", 16843900, 3207198, 262144, 0, 20, 0, 0, 0, 0, 0, {5, 5, 0, 8}, {8, 8, 8, 8}, {192, 64, 208, 80}, 34, false, false, "General", "", "", "", "Sign", "post"},
		{"Fireplace", 33554431, 104854558, 262144, 0, 20, 0, 0, 0, 0, 0, {5, 5, 0, 8}, {8, 8, 8, 8}, {128, 64, 144, 80}, 34, false, false, "Misc", "", "", "", "Fire", "place"},
		{"Save Sign", 33554431, 104854558, 262144, 0, 20, 0, 0, 0, 0, 0, {5, 5, 0, 8}, {8, 8, 8, 8}, {224, 64, 240, 80}, 34, false, false, "Misc", "", "Cocktail sign", "", "Save", "sign"},
		{"Santa", 33554431, 104854558, 131080, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {0, 32, 16, 48}, 35, false, false, "Characters", "", "", "", "Santa", ""},
		{"Busted Door", 33554431, 104854558, 262144, 0, 2, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {24, 24, 24, 8}, {0, 80, 48, 128}, 10, false, false, "Misc", "", "", "", "Door", "bustd"},
		{"Sue", 14679679, 54522910, 131072, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 35, false, false, "Characters", "", "", "", "Sue", ""},
		{"Chalkboard", 33554431, 104854558, 262152, 0, 20, 0, 0, 0, 0, 0, {12, 12, 8, 8}, {20, 20, 24, 8}, {128, 80, 168, 112}, 34, false, false, "Misc", "", "", "", "Black", "board"},
		{"Polish", 33554431, 104854558, 32800, 120, 21, 52, 72, 3, 12, 10, {12, 12, 12, 12}, {16, 16, 16, 16}, {0, 0, 32, 32}, 6, false, false, "Sand Zone", "", "", "", "Enemy", "Polsh"},
		{"Baby", 2101260, 407897094, 32800, 2, 21, 51, 71, 2, 2, 3, {5, 5, 5, 5}, {8, 8, 8, 8}, {0, 32, 16, 48}, 6, false, false, "Sand Zone", "", "", "", "Enemy", "Baby"},
		{"H/V Trigger", 33554431, 104854558, 393216, 0, 20, 0, 0, 2, 0, 0, {6, 6, 6, 6}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "General", "Follow player", "Vertical", "Don't ignore dead players", "H/V", "trigr"},
		{"Sandcroc", 33554431, 104854558, 163872, 30, 21, 52, 72, 2, 9, 0, {7, 7, 24, 8}, {24, 24, 24, 8}, {192, 48, 240, 80}, 6, false, false, "Sand Zone", "", "", "", "Enemy", "ScrcG"},
		{"Omega Projectiles", 33554431, 104854558, 32, 1, 22, 0, 70, 1, 1, 4, {4, 4, 4, 4}, {8, 8, 8, 8}, {288, 88, 304, 104}, 7, false, false, "Sand Zone", "", "", "", "Proj", "Omega"},
		{"Skullhead", 14941823, 57799710, 163872, 20, 21, 52, 71, 3, 10, 8, {14, 14, 8, 4}, {16, 16, 16, 8}, {0, 80, 32, 104}, 6, false, false, "Sand Zone", "", "", "", "Enemy", "SkulH"},
		{"Skeleton Projectile", 33554431, 104854558, 32, 1, 21, 0, 70, 1, 0, 4, {4, 4, 4, 4}, {8, 8, 8, 8}, {48, 32, 64, 48}, 6, false, false, "Sand Zone", "", "", "", "Proj", "SkulH"},
		{"Crow & Skullhead", 33554431, 104854558, 163872, 20, 21, 51, 71, 2, 9, 4, {8, 8, 8, 8}, {16, 16, 16, 16}, {96, 80, 128, 112}, 6, false, false, "Sand Zone", "", "", "", "Enemy", "CrowS"},
		{"Blue Robot (sit)", 33554431, 104854558, 8, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {240, 96, 256, 112}, 35, false, false, "Misc", "", "", "", "BRobt", "sit"},
		{"Skullstep Leg", 4454013, 20050974, 32, 10, 21, 52, 70, 2, 2, 8, {10, 8, 4, 4}, {12, 12, 8, 8}, {0, 128, 24, 144}, 6, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Skullstep", 33554431, 104854558, 32800, 20, 21, 52, 71, 3, 10, 8, {14, 14, 8, 4}, {16, 16, 16, 8}, {0, 80, 32, 104}, 6, false, false, "Sand Zone", "", "", "", "Enemy", "SkulS"},
		{"Kazuma", 33554431, 104854558, 0, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 16, 8}, {192, 192, 208, 216}, 35, false, false, "Characters", "", "", "", "Kazum", ""},
		{"Beetle (Sandzone)", 33554431, 104854558, 163873, 4, 21, 53, 71, 2, 2, 2, {6, 5, 6, 6}, {8, 8, 8, 8}, {0, 144, 16, 160}, 6, false, false, "Sand Zone", "", "", "", "Enemy", "BtlHB"},
		{"Crow", 211892860, 51580564510, 163872, 20, 21, 51, 71, 2, 9, 4, {8, 8, 8, 8}, {16, 16, 16, 16}, {96, 80, 128, 112}, 6, false, false, "Sand Zone", "", "", "", "Enemy", "Crow"},
		{"Basu (Egg)", 33554431, 104854558, 163872, 15, 21, 52, 71, 3, 9, 0, {6, 6, 6, 6}, {12, 12, 12, 12}, {192, 0, 216, 24}, 2, false, false, "Egg1", "", "", "", "Enemy", "Basu1"},
		{"Eye Door", 33554431, 104854558, 163872, 6, 20, 52, 72, 2, 2, 4, {5, 5, 8, 8}, {8, 8, 16, 8}, {224, 16, 240, 40}, 34, false, false, "Misc", "", "", "", "Enemy", "Door"},
		{"Toroko", 8467070, 104854558, 655360, 0, 23, 0, 0, 0, 0, 0, {4, 4, 8, 8}, {8, 8, 8, 8}, {0, 64, 16, 80}, 35, false, false, "Characters", "", "", "", "Torok", ""},
		{"King", 33554431, 104854558, 524288, 0, 23, 0, 0, 0, 0, 0, {6, 5, 8, 8}, {8, 8, 8, 8}, {224, 32, 240, 48}, 35, false, false, "Characters", "", "", "", "King", ""},
		{"Kazuma (Computer)", 33554431, 104854558, 8, 0, 23, 0, 0, 0, 0, 0, {6, 6, 8, 8}, {8, 8, 16, 8}, {272, 192, 288, 216}, 35, false, false, "Characters", "", "", "", "Kazum", "compu"},
		{"Toroko (stick)", 33554431, 104854558, 8, 100, 23, 50, 70, 2, 0, 1, {6, 6, 8, 8}, {8, 8, 8, 8}, {64, 64, 80, 80}, 35, false, false, "Characters", "", "", "", "Torok", "attck"},
		{"First Cave Critter", 33554431, 104854558, 163873, 2, 21, 51, 71, 2, 1, 1, {6, 6, 4, 5}, {8, 8, 8, 8}, {0, 0, 16, 16}, 8, false, false, "First Cave", "", "", "", "Enemy", "CrtHB"},
		{"First Cave Bat", 33554431, 104854558, 163872, 1, 21, 50, 70, 1, 1, 1, {4, 4, 7, 4}, {8, 8, 8, 8}, {32, 32, 48, 48}, 8, false, false, "First Cave", "", "", "", "Enemy", "BatBu"},
		{"Misery Bubble", 33554431, 104854558, 8, 0, 23, 50, 70, 2, 0, 1, {5, 5, 8, 8}, {12, 12, 12, 12}, {32, 192, 56, 216}, 35, false, false, "Unsure", "", "", "", "Bubbl", ""},
		{"Misery (flioat)", 33554431, 104854558, 0, 0, 23, 50, 70, 2, 0, 1, {5, 5, 8, 8}, {8, 8, 8, 8}, {80, 0, 96, 16}, 35, false, false, "Misery", "", "", "", "Msry", "float"},
		{"Balrog (running)", 33554431, 104854558, 163872, 60, 22, 52, 72, 3, 18, 0, {14, 14, 11, 12}, {20, 20, 12, 12}, {0, 0, 40, 24}, 9, false, false, "Balrog", "", "", "", "Boss", "Blrg1"},
		{"Pignon", 33554431, 104854558, 32801, 2, 21, 50, 71, 2, 0, 1, {6, 6, 5, 5}, {8, 8, 8, 8}, {48, 0, 64, 16}, 8, false, false, "Graveyard", "", "", "", "Enemy", "Pign"},
		{"Sparkle", 33554431, 104854558, 262144, 0, 20, 0, 0, 0, 0, 0, {5, 5, 0, 8}, {8, 8, 8, 8}, {96, 48, 112, 64}, 34, false, false, "General", "", "", "", "Spark", "item"},
		{"Chinfish", 2097216, 104854558, 32801, 2, 21, 50, 70, 1, 0, 1, {6, 6, 4, 4}, {8, 8, 8, 8}, {64, 32, 80, 48}, 0, false, false, "Misc", "", "", "", "Enemy", "Chinf"},
		{"Sprinkler", 33554431, 104854558, 262144, 0, 20, 0, 0, 2, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {224, 48, 240, 64}, 34, false, false, "Misc", "", "", "", "Sprnk", ""},
		{"Water droplet", 33554431, 104854558, 262144, 0, 19, 0, 0, 2, 0, 0, {1, 1, 1, 1}, {1, 1, 1, 1}, {72, 16, 74, 18}, 0, false, false, "Misc", "", "", "", "Water", "drop"},
		{"Jack", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {64, 0, 80, 16}, 0, false, false, "Characters", "", "", "", "Jack", ""},
		{"Kanpachi (fishing)", 33554431, 104854558, 131072, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {16, 8, 8, 16}, {272, 32, 296, 56}, 0, false, false, "Characters", "", "", "", "Kanpa", "fishn"},
		{"Flowers", 33554431, 104854558, 262144, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 10, false, false, "Yamashita Farm", "", "", "", "Flowr", ""},
		{"Yamashita", 33554431, 104854558, 262144, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {20, 28, 24, 8}, {0, 16, 48, 48}, 10, false, false, "Yamashita Farm", "", "", "", "Sanda", "pavil"},
		{"Pot", 33554431, 104854558, 262144, 0, 20, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {160, 48, 176, 64}, 34, false, false, "Misc", "", "", "", "Pot", ""},
		{"Mahin", 33554431, 104854558, 131072, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 0, false, false, "Characters", "", "", "", "Mahin", ""},
		{"Gravekeeper", 33554431, 104854558, 163844, 20, 21, 51, 71, 2, 4, 2, {4, 4, 12, 8}, {16, 8, 16, 8}, {0, 64, 24, 88}, 8, false, false, "Graveyard", "", "", "", "Enemy", "GravK"},
		{"Giant pignon", 33554431, 104854558, 163873, 12, 21, 52, 72, 3, 0, 1, {8, 8, 8, 12}, {12, 12, 12, 12}, {144, 64, 168, 88}, 8, false, false, "Graveyard", "", "", "", "Enemy", "GPign"},
		{"Misery (standing)", 33554431, 104854558, 0, 0, 23, 50, 70, 2, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {112, 0, 128, 16}, 35, false, false, "Misery", "", "", "", "Msry", "stand"},
		{"Igor (cutscene)", 33554431, 104854558, 0, 10, 22, 0, 0, 3, 0, 2, {8, 8, 1, 16}, {20, 20, 20, 20}, {0, 0, 40, 40}, 3, false, false, "Mimiga", "", "", "", "Igor", ""},
		{"Basu Projectile", 33554431, 104854558, 0, 1, 21, 50, 70, 1, 0, 1, {2, 2, 2, 2}, {8, 8, 8, 8}, {48, 48, 64, 64}, 2, false, false, "Egg1", "", "", "", "Proj", "Basu1"},
		{"Terminal", 33497087, 104854558, 131072, 0, 20, 0, 0, 2, 0, 0, {5, 5, 8, 8}, {8, 8, 16, 8}, {256, 96, 272, 120}, 34, false, false, "Misc", "", "", "", "Termn", ""},
		{"Missile", 2032639, 3275806, 0, 1, 20, 0, 0, 2, 1, 0, {6, 6, 6, 6}, {8, 8, 8, 8}, {0, 80, 16, 96}, 34, false, false, "General", "", "", "", "Missl", ""},
		{"Heart", 79932, 3274783, 0, 2, 20, 0, 0, 2, 2, 0, {6, 6, 6, 6}, {8, 8, 8, 8}, {32, 80, 48, 96}, 34, false, false, "General", "", "No flash", "", "Heart", ""},
		{"Igor (boss)", 33554431, 104854558, 163872, 300, 22, 52, 72, 3, 1, 0, {8, 8, 16, 16}, {20, 20, 20, 20}, {0, 0, 40, 40}, 3, false, false, "Mimiga", "", "", "", "Boss", "Igor"},
		{"Igor (defeated)", 33554431, 104854558, 131072, 300, 22, 52, 72, 3, 0, 0, {8, 8, 16, 16}, {20, 20, 20, 20}, {80, 80, 120, 120}, 3, false, false, "Mimiga", "", "", "", "Dead", "Igor"},
		{"Background", 33554431, 104854558, 262144, 0, 22, 0, 0, 0, 0, 0, {4, 4, 8, 8}, {8, 8, 16, 8}, {280, 80, 296, 104}, 18, false, false, "Unsure", "", "", "", "Backg", ""},
		{"Cage", 33554431, 104854558, 262144, 0, 20, 0, 0, 0, 0, 0, {4, 4, 8, 8}, {16, 16, 16, 8}, {96, 88, 128, 112}, 34, false, false, "Misc", "", "", "", "Cage", ""},
		{"Sue at PC", 33554431, 104854558, 8, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 16, 8}, {272, 216, 288, 240}, 35, false, false, "Characters", "", "", "", "Sue", "compu"},
		{"Chaco", 33554431, 104854558, 131072, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {128, 0, 144, 16}, 0, false, false, "Characters", "", "", "", "Chaco", ""},
		{"Kulala", 33554431, 104854558, 163872, 50, 21, 52, 72, 3, 15, 5, {16, 16, 7, 7}, {24, 24, 12, 12}, {272, 0, 320, 24}, 4, false, false, "Grasstown", "", "", "", "Enemy", "Kulal"},
		{"Jelly", 33554431, 104854558, 32800, 4, 21, 53, 21, 3, 2, 5, {6, 6, 6, 6}, {8, 8, 8, 8}, {208, 64, 224, 80}, 4, false, false, "Grasstown", "", "", "", "Enemy", "Jelly"},
		{"Fan (left)", 33554431, 104854558, 262208, 0, 20, 0, 0, 0, 0, 0, {8, 8, 9, 8}, {8, 8, 8, 8}, {272, 120, 288, 136}, 34, false, false, "Misc", "", "", "", "Fan", "left"},
		{"Fan (up)", 33554431, 104854558, 262208, 0, 20, 0, 0, 0, 0, 0, {8, 8, 9, 8}, {8, 8, 8, 8}, {272, 136, 288, 152}, 34, false, false, "Misc", "", "", "", "Fan", "up"},
		{"Fan (right)", 33554431, 104854558, 262208, 0, 20, 0, 0, 0, 0, 0, {8, 8, 9, 8}, {8, 8, 8, 8}, {272, 152, 288, 168}, 34, false, false, "Misc", "", "", "", "Fan", "right"},
		{"Fan (down)", 33554431, 104854558, 262208, 0, 20, 0, 0, 0, 0, 0, {8, 8, 9, 8}, {8, 8, 8, 8}, {272, 168, 288, 184}, 34, false, false, "Misc", "", "", "", "Fan", "down"},
		{"Grate", 33554431, 104854558, 262152, 0, 20, 0, 0, 0, 0, 0, {8, 8, 8, 8}, {8, 8, 8, 8}, {112, 64, 128, 80}, 0, false, false, "Misc", "", "", "", "Grate", ""},
		{"Malco comp screen", 33554431, 104854558, 8, 0, 20, 0, 0, 0, 0, 0, {8, 8, 8, 8}, {8, 8, 8, 8}, {240, 136, 256, 152}, 34, false, false, "Misc", "", "", "", "PowCn", "scrn"},
		{"Malco comp wave", 33554431, 104854558, 8, 0, 20, 0, 0, 0, 0, 0, {8, 8, 8, 8}, {8, 8, 8, 8}, {208, 120, 224, 136}, 34, false, false, "Misc", "", "", "", "PowCn", "pflow"},
		{"Mannan projectile", 33554431, 104854558, 8, 0, 21, 0, 0, 2, 0, 3, {3, 3, 8, 8}, {8, 8, 12, 12}, {192, 96, 208, 120}, 4, false, false, "Grasstown", "", "", "", "Proj", "Manan"},
		{"Frog", 8409724, 36762654, 163880, 10, 21, 52, 72, 3, 9, 3, {8, 8, 9, 10}, {16, 16, 16, 16}, {0, 112, 32, 144}, 4, false, false, "Grasstown", "", "", "", "Enemy", "Frog"},
		{"HEY! (low)", 33554431, 104854558, 8, 0, 20, 0, 0, 0, 0, 0, {8, 8, 8, 8}, {8, 8, 8, 8}, {128, 32, 144, 48}, 34, false, false, "Misc", "", "", "", "Bloon", "HeyL"},
		{"HEY! (high)", 33554431, 104854558, 8, 0, 20, 0, 0, 0, 0, 0, {8, 8, 8, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 34, false, false, "Misc", "", "", "", "Bloon", "HeyH"},
		{"Malco", 33554431, 104854558, 0, 20, 21, 0, 0, 2, 0, 0, {5, 5, 5, 8}, {8, 8, 16, 8}, {144, 0, 160, 24}, 4, false, false, "Grasstown", "", "", "", "Malco", ""},
		{"Balfrog Projectile", 33554431, 104854558, 32, 1, 21, 21, 21, 1, 0, 4, {4, 4, 4, 4}, {8, 8, 8, 8}, {96, 48, 112, 64}, 4, false, false, "Grasstown", "", "", "", "Proj", "Bfrog"},
		{"Malco (broken)", 33554431, 104854558, 131072, 20, 21, 0, 0, 2, 0, 0, {5, 5, 5, 8}, {8, 8, 16, 8}, {240, 0, 256, 24}, 4, false, false, "Grasstown", "", "", "", "Malco", "damgd"},
		{"Puchi", 33554431, 104854558, 163880, 2, 21, 50, 70, 2, 2, 1, {4, 4, 4, 5}, {8, 8, 8, 8}, {96, 128, 112, 144}, 4, false, false, "Grasstown", "", "", "", "Enemy", "Puchi"},
		{"Quote (teleport out)", 33554431, 104854558, 0, 1, 16, 0, 0, 3, 0, 0, {6, 5, 6, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 35, false, false, "Quote", "", "", "", "Quote", "t-out"},
		{"Quote (teleport in)", 33554431, 104854558, 0, 1, 16, 0, 0, 3, 0, 0, {6, 5, 6, 16}, {8, 8, 8, 8}, {0, 0, 16, 16}, 35, false, false, "Quote", "", "", "", "Quote", "t-in"},
		{"Professor Booster", 33554431, 104854558, 0, 1, 23, 0, 0, 3, 0, 0, {6, 5, 6, 8}, {8, 8, 8, 8}, {224, 0, 240, 16}, 35, false, false, "Characters", "", "", "", "Boost", ""},
		{"Press", 33554431, 104854558, 163936, 32, 20, 52, 72, 3, 5, 0, {8, 8, 13, 12}, {8, 8, 12, 12}, {144, 112, 160, 136}, 34, false, false, "Misc", "", "", "", "Enemy", "Press"},
		{"Ravil", 33554431, 104854558, 163873, 40, 21, 52, 72, 2, 10, 0, {8, 8, 8, 8}, {12, 12, 16, 8}, {0, 120, 24, 144}, 3, false, false, "Mimiga", "", "", "", "Enemy", "Ravil"},
		{"Red petals", 33554431, 104854558, 262144, 0, 20, 0, 0, 0, 0, 0, {20, 20, 8, 8}, {24, 24, 8, 8}, {272, 184, 320, 200}, 34, false, false, "Misc", "", "", "", "RFlow", "petal"},
		{"Curly", 33554431, 104854558, 655360, 0, 23, 0, 0, 2, 0, 0, {6, 6, 6, 8}, {8, 8, 8, 8}, {0, 96, 16, 112}, 35, false, false, "Curly", "", "", "", "Curly", ""},
		{"Curly (boss)", 33554431, 104854558, 688160, 320, 22, 57, 0, 2, 0, 0, {6, 6, 6, 8}, {16, 16, 16, 16}, {0, 32, 32, 56}, 12, false, false, "Curly", "", "", "", "Boss", "Curly"},
		{"Table & chair", 33554431, 104854558, 262144, 0, 20, 0, 0, 2, 0, 0, {6, 6, 6, 8}, {12, 12, 8, 8}, {248, 184, 272, 200}, 34, false, false, "Misc", "", "", "", "Table", "chair"},
		{"Colon (1)", 33554431, 104854558, 0, 0, 22, 0, 0, 2, 0, 0, {6, 6, 6, 8}, {8, 8, 8, 8}, {64, 0, 80, 16}, 12, false, false, "Unsure", "", "", "", "Colon", "1"},
		{"Colon (2)", 33554431, 104854558, 0, 0, 22, 0, 0, 2, 0, 0, {6, 6, 6, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 12, false, false, "Unsure", "", "", "", "Colon", "2"},
		{"Colon (attacking)", 33554431, 104854558, 655360, 0, 22, 50, 0, 2, 0, 0, {6, 6, 6, 8}, {8, 8, 8, 8}, {80, 0, 96, 16}, 12, false, false, "Unsure", "", "", "", "Enemy", "Colon"},
		{"Curly Boss Projectile", 33554431, 104854558, 0, 0, 22, 0, 0, 2, 0, 6, {4, 4, 4, 4}, {8, 8, 8, 8}, {192, 0, 208, 16}, 12, false, false, "Curly", "", "", "", "Proj", "Curly"},
		{"Sunstone", 33554431, 104854558, 68, 0, 21, 0, 0, 3, 0, 0, {16, 16, 17, 16}, {16, 16, 16, 16}, {160, 0, 192, 32}, 6, false, false, "Sand Zone", "", "", "", "Sun", "stone"},
		{"Hidden item", 33554431, 104854558, 32, 1000, 20, 104, 0, 2, 0, 0, {6, 6, 6, 6}, {8, 8, 8, 8}, {0, 96, 16, 112}, 34, false, false, "General", "", "", "", "Hiddn", "H/M"},
		{"Puppy (running)", 12595772, 53489694, 131072, 0, 21, 0, 0, 0, 0, 0, {4, 4, 8, 8}, {8, 8, 8, 8}, {48, 144, 64, 160}, 6, false, false, "Sand Zone", "", "", "", "Puppy", "run"},
		{"M.Gun Trail Lvl2", 33554431, 104854558, 262152, 1, 19, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {8, 8, 8, 8}, {112, 48, 128, 64}, 0, false, false, "Unsure", "", "", "", "Glows", "Momnt"},
		{"M.Gun Trail Lvl3", 33554431, 104854558, 262152, 1, 19, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {8, 8, 8, 8}, {176, 16, 184, 32}, 0, false, false, "Unsure", "", "", "", "Glows", "Momnt"},
		{"F.Ball Trail Lvl2&3", 33554431, 104854558, 262152, 1, 19, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {8, 8, 8, 8}, {128, 48, 144, 64}, 0, false, false, "Unsure", "", "", "", "Glows", "Momnt"},
		{"Puppy (sit,  wag tail)", 33554431, 14757395255636523000, 131072, 0, 21, 0, 0, 0, 0, 0, {4, 4, 8, 8}, {8, 8, 8, 8}, {48, 144, 64, 160}, 6, false, false, "Sand Zone", "", "", "", "Puppy", "wag"},
		{"Puppy (sleeping)", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {4, 4, 8, 8}, {8, 8, 8, 8}, {144, 144, 160, 160}, 6, false, false, "Sand Zone", "", "", "", "Puppy", "sleep"},
		{"Puppy (barking)", 33554431, 104854558, 131072, 0, 21, 0, 0, 0, 0, 0, {4, 4, 8, 8}, {8, 8, 8, 8}, {128, 144, 144, 160}, 6, false, false, "Sand Zone", "", "", "", "Puppy", "bark"},
		{"Jenka", 33554431, 104854558, 0, 0, 23, 0, 0, 0, 0, 0, {6, 6, 8, 8}, {8, 8, 8, 8}, {176, 32, 192, 48}, 35, false, false, "Characters", "", "", "", "Jenka", ""},
		{"Armadillo", 33554431, 104854558, 163872, 20, 21, 52, 72, 2, 5, 6, {8, 8, 4, 8}, {16, 16, 8, 8}, {224, 0, 256, 16}, 6, false, false, "Sand Zone", "", "", "", "Enemy", "Armdl"},
		{"Skeleton", 33554431, 104854558, 163872, 40, 21, 54, 72, 3, 20, 6, {8, 8, 13, 8}, {16, 16, 24, 8}, {288, 32, 320, 64}, 6, false, false, "Sand Zone", "", "", "", "Enemy", "Skelt"},
		{"Puppy (carried)", 33554431, 104854558, 131080, 0, 21, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {8, 8, 8, 8}, {48, 144, 64, 160}, 6, false, false, "Sand Zone", "", "", "", "Carry", "Puppy"},
		{"Large door (frame)", 33554431, 104854558, 262144, 0, 20, 0, 0, 2, 0, 0, {6, 6, 6, 6}, {16, 16, 24, 8}, {96, 136, 128, 188}, 34, false, false, "Misc", "", "", "", "LDoor", "frame"},
		{"Large door (door)", 33554431, 104854558, 262144, 0, 20, 0, 0, 2, 0, 0, {6, 6, 6, 6}, {8, 8, 16, 8}, {96, 112, 112, 136}, 34, false, false, "Misc", "", "", "", "LDoor", "doors"},
		{"Doctor", 33554431, 104854558, 0, 0, 23, 0, 0, 3, 0, 0, {6, 6, 6, 16}, {16, 8, 16, 16}, {0, 128, 24, 160}, 35, false, false, "Doctor", "", "", "", "Doctr", ""},
		{"Toroko (frenzied)", 33554431, 104854558, 163840, 500, 21, 52, 72, 3, 1, 5, {8, 8, 8, 13}, {16, 16, 16, 16}, {32, 0, 64, 32}, 15, false, false, "Toroko (boss)", "", "", "", "Boss", "Torok"},
		{"Toroko Block Projectile", 12654205, 53539870, 655364, 1, 21, 50, 70, 1, 6, 10, {8, 8, 8, 8}, {8, 8, 8, 8}, {288, 32, 304, 48}, 15, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Flower Cub", 33554431, 104854558, 163872, 1, 21, 50, 70, 1, 6, 2, {6, 6, 6, 8}, {8, 8, 8, 8}, {0, 128, 16, 144}, 15, false, false, "Toroko (boss)", "", "", "", "Enemy", "Flwcb"},
		{"Jenka (collapsed)", 33554431, 104854558, 0, 0, 23, 0, 0, 0, 0, 0, {6, 6, 8, 8}, {8, 8, 8, 8}, {208, 32, 224, 48}, 35, false, false, "Characters", "", "", "", "Jenka", "uncon"},
		{"Toroko (teleport in)", 33554431, 104854558, 524288, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {0, 64, 16, 80}, 35, false, false, "Characters", "", "", "", "Torok", "telep"},
		{"King's Sword", 6419068, 19984414, 0, 0, 20, 0, 0, 2, 0, 0, {6, 6, 6, 6}, {8, 8, 8, 8}, {96, 32, 112, 48}, 34, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Lightning", 33554431, 104854558, 8, 0, 19, 0, 0, 2, 0, 0, {6, 6, 230, 6}, {8, 8, 232, 8}, {256, 0, 272, 240}, 0, false, false, "Misc", "", "", "", "Ligtn", ""},
		{"Critter (purple)", 33554431, 104854558, 163872, 10, 21, 51, 71, 2, 4, 8, {4, 4, 4, 5}, {8, 8, 8, 8}, {0, 96, 16, 112}, 5, false, false, "Labyrinth", "", "", "", "Enemy", "CrtHv"},
		{"Purple Critter Proj.", 33554431, 104854558, 32, 5, 21, 21, 70, 1, 0, 5, {4, 4, 4, 4}, {4, 4, 4, 4}, {96, 96, 104, 104}, 5, false, false, "Labyrinth", "", "", "", "Proj", "CrtHv"},
		{"Moving Block", 33554431, 104854558, 131140, 1, 2, 0, 0, 3, 0, 0, {16, 16, 17, 14}, {16, 16, 16, 16}, {16, 0, 48, 32}, 8, false, false, "Labyrinth", "", "", "", "Block", "MoveH"},
		{"Quote", 33554431, 104854558, 0, 1, 16, 0, 0, 3, 0, 0, {6, 5, 6, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 35, false, false, "Quote", "", "", "", "Quote", ""},
		{"Blue robot (standing)", 33554431, 104854558, 0, 1, 23, 0, 0, 2, 0, 0, {6, 5, 6, 8}, {8, 8, 8, 8}, {192, 0, 208, 16}, 35, false, false, "Misc", "", "", "", "BRobt", "stand"},
		{"Shutter stuck", 33554431, 104854558, 32, 40, 20, 54, 72, 1, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 34, false, false, "Core", "", "", "", "Shutr", "stuck"},
		{"Gaudi", 33554431, 104854558, 163872, 1000, 21, 54, 71, 3, 8, 5, {8, 8, 11, 5}, {12, 12, 16, 8}, {0, 0, 24, 24}, 5, false, false, "Labyrinth", "", "", "", "Enemy", "Gaudi"},
		{"Gaudi (dead)", 33554431, 104854558, 0, 1000, 21, 51, 71, 3, 8, 5, {8, 8, 11, 5}, {12, 12, 16, 8}, {192, 0, 216, 24}, 5, false, false, "Labyrinth", "", "", "", "Dead", "Gaudi"},
		{"Gaudi (flying)", 33554431, 104854558, 163880, 1000, 21, 54, 71, 3, 8, 5, {8, 8, 11, 5}, {12, 12, 16, 8}, {0, 48, 24, 72}, 5, false, false, "Labyrinth", "", "", "", "Enemy", "GaudF"},
		{"Gaudi Projectile", 33554431, 104854558, 32, 1, 21, 21, 21, 1, 1, 3, {4, 4, 4, 4}, {8, 8, 8, 8}, {96, 112, 112, 128}, 5, false, false, "Labyrinth", "", "", "", "Proj", "GaudF"},
		{"Moving block (vertical)", 8454716, 36745246, 131140, 1, 2, 0, 0, 3, 0, 0, {16, 16, 17, 14}, {16, 16, 16, 16}, {16, 0, 48, 32}, 8, false, false, "Labyrinth", "", "", "", "Block", "MoveV"},
		{"Fish Missile", 33554431, 104854558, 131112, 1, 22, 51, 72, 1, 5, 5, {4, 4, 4, 4}, {8, 8, 8, 8}, {0, 224, 16, 240}, 16, false, false, "Labyrinth", "", "", "", "Proj", "MonX"},
		{"Moster X (defeated)", 33554431, 104854558, 8, 1, 22, 51, 72, 1, 0, 0, {4, 4, 4, 4}, {24, 24, 36, 36}, {144, 128, 192, 200}, 16, false, false, "Labyrinth", "", "", "", "Dead", "MonX"},
		{"Puu Black", 33554431, 104854558, 163880, 300, 22, 54, 72, 3, 18, 0, {14, 14, 11, 12}, {20, 20, 12, 12}, {0, 0, 40, 24}, 17, false, false, "Pooh Black", "", "", "", "Boss", "PoohB"},
		{"Puu Black Projectile", 33554431, 104854558, 40, 100, 22, 21, 0, 1, 0, 5, {6, 6, 6, 6}, {8, 8, 8, 8}, {0, 48, 16, 64}, 17, false, false, "Pooh Black", "", "", "", "Proj", "PoohB"},
		{"Puu Black (dead)", 33554431, 104854558, 131080, 500, 22, 52, 72, 3, 18, 0, {14, 14, 11, 12}, {20, 20, 12, 12}, {40, 0, 80, 24}, 17, false, false, "Pooh Black", "", "", "", "Dead", "PoohB"},
		{"Dr Gero", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {6, 6, 8, 8}, {8, 8, 8, 8}, {192, 0, 208, 16}, 0, false, false, "Characters", "", "", "", "Dr", "Gero"},
		{"Nurse Hasumi", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {6, 6, 8, 8}, {8, 8, 8, 8}, {224, 0, 240, 16}, 0, false, false, "Characters", "", "", "", "Nurse", "Hasum"},
		{"Curly (collapsed)", 33554431, 104854558, 131072, 0, 23, 0, 0, 0, 0, 0, {6, 6, 8, 8}, {8, 8, 8, 8}, {192, 96, 208, 112}, 35, false, false, "Curly", "", "", "", "Curly", "uncon"},
		{"Chaba", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {8, 8, 8, 8}, {20, 20, 16, 8}, {144, 104, 184, 128}, 5, false, false, "Labyrinth", "", "", "", "Chaba", ""},
		{"Prof. Booster (fall)", 33554431, 14757395255636523000, 0, 0, 23, 0, 0, 0, 0, 0, {6, 6, 8, 8}, {8, 8, 8, 8}, {304, 0, 320, 16}, 35, false, false, "Characters", "", "", "", "Boost", "fall"},
		{"Boulder", 33554431, 104854558, 0, 0, 22, 0, 0, 0, 0, 0, {8, 6, 8, 8}, {28, 28, 32, 8}, {264, 56, 320, 96}, 9, false, false, "Labyrinth", "", "", "", "Bould", ""},
		{"Balrog (missile)", 33554431, 104854558, 163872, 500, 22, 52, 72, 3, 18, 0, {14, 14, 11, 12}, {20, 20, 12, 12}, {0, 0, 40, 24}, 9, false, false, "Balrog", "", "", "", "Boss", "Blrg3"},
		{"Balrog missile", 33554431, 104854558, 131104, 10, 21, 51, 71, 3, 3, 5, {6, 6, 4, 4}, {8, 8, 4, 4}, {112, 96, 128, 104}, 5, false, false, "Labyrinth", "", "", "", "Proj", "Blrg3"},
		{"Fire Whirrr", 33554431, 104854558, 163873, 50, 21, 52, 72, 3, 20, 10, {4, 4, 12, 12}, {16, 16, 16, 16}, {120, 48, 152, 80}, 5, false, false, "Labyrinth", "", "", "", "Enemy", "FWhir"},
		{"Fire Whirrr Projectile", 33554431, 104854558, 0, 50, 21, 34, 0, 3, 10, 5, {4, 4, 12, 12}, {8, 8, 16, 16}, {248, 48, 264, 80}, 5, false, false, "Labyrinth", "", "", "", "Proj", "FWhir"},
		{"Gaudi (armoured)", 33554431, 104854558, 163872, 1000, 21, 54, 71, 3, 15, 5, {8, 8, 11, 5}, {12, 12, 16, 8}, {0, 128, 24, 152}, 5, false, false, "Labyrinth", "", "", "", "Enemy", "GaudA"},
		{"Armoured-Gaudi Proj", 33554431, 104854558, 4, 1, 21, 21, 21, 1, 0, 3, {4, 4, 4, 4}, {8, 8, 8, 8}, {120, 80, 136, 96}, 5, false, false, "Labyrinth", "", "", "", "Proj", "GaudA"},
		{"Gaudi egg", 33554431, 104854558, 32800, 100, 21, 51, 71, 3, 6, 0, {8, 8, 10, 10}, {12, 12, 12, 12}, {168, 80, 192, 104}, 5, false, false, "Labyrinth", "", "", "", "Enemy", "GaudE"},
		{"BuyoBuyo Base", 33554431, 104854558, 163880, 1000, 21, 54, 72, 3, 9, 1, {4, 4, 12, 12}, {16, 16, 8, 8}, {96, 128, 128, 144}, 5, false, false, "Labyrinth", "", "", "", "Enemy", "BuyoB"},
		{"BuyoBuyo", 33554431, 104854558, 163872, 3, 21, 53, 70, 1, 2, 3, {6, 6, 6, 6}, {8, 8, 8, 8}, {192, 128, 208, 144}, 5, false, false, "Labyrinth", "", "", "", "Enemy", "Buyo"},
		{"Core Blade Projectile", 61564, 104854558, 40, 6, 22, 21, 70, 1, 1, 3, {6, 6, 6, 6}, {8, 8, 8, 8}, {0, 224, 16, 240}, 18, false, false, "Core", "", "", "", "Proj", "Core1"},
		{"Core wisp projectile", 33554431, 104854558, 40, 2, 22, 50, 70, 1, 5, 3, {6, 6, 4, 4}, {8, 8, 8, 8}, {48, 224, 72, 240}, 18, false, false, "Core", "", "", "", "Proj", "Core2"},
		{"Curly AI", 33554431, 104854558, 0, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {0, 96, 16, 112}, 35, false, false, "Curly", "", "", "", "Curly", "A.I."},
		{"Curly AI Machine Gun", 33554431, 104854558, 8, 0, 20, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {216, 152, 232, 168}, 34, false, false, "Unsure", "", "", "", "?", ""},
		{"Curly AI Polar Star", 33554431, 104854558, 8, 0, 20, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {184, 152, 200, 168}, 34, false, false, "Unsure", "", "", "", "?", ""},
		{"Curly Air Tank Bubble", 33554431, 104854558, 8, 0, 19, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {12, 12, 12, 12}, {56, 96, 80, 120}, 0, false, false, "Unsure", "", "", "", "?", ""},
		{"Shutter big", 60, 104854558, 64, 0, 2, 0, 0, 3, 0, 0, {16, 16, 17, 16}, {16, 16, 16, 16}, {0, 64, 32, 96}, 14, false, false, "Core", "", "", "", "Shutr", "large"},
		{"Shutter small", 33554431, 104854558, 68, 0, 2, 0, 0, 3, 0, 0, {8, 8, 17, 16}, {8, 8, 16, 16}, {96, 64, 112, 96}, 14, false, false, "Core", "", "", "", "Shutr", "small"},
		{"Lift block", 60, 104854558, 68, 0, 2, 0, 0, 2, 0, 0, {8, 8, 9, 8}, {8, 8, 8, 8}, {48, 48, 64, 64}, 14, false, false, "Core", "", "", "", "Lift", "block"},
		{"Fuzz Core", 33554431, 104854558, 163872, 10, 21, 53, 71, 1, 7, 1, {10, 10, 10, 10}, {16, 16, 16, 16}, {224, 104, 256, 136}, 5, false, false, "Labyrinth", "", "", "", "Enemy", "FuzzC"},
		{"Fuzz", 33554431, 121631774, 688168, 8, 21, 21, 70, 1, 1, 1, {4, 4, 4, 4}, {8, 8, 8, 8}, {288, 104, 304, 120}, 5, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Homing flame unused", 33554431, 104854558, 131080, 0, 20, 0, 0, 0, 1, 1, {4, 4, 4, 4}, {4, 4, 12, 4}, {224, 184, 232, 200}, 34, false, false, "Unsure", "", "", "", "Proj", "HomFl"},
		{"Broken Robot", 33554431, 104854558, 8, 0, 20, 0, 0, 0, 1, 0, {4, 4, 4, 4}, {8, 8, 8, 8}, {192, 32, 208, 48}, 34, false, false, "Misc", "", "", "", "Surfc", "robot"},
		{"Water Level", 33554431, 104854558, 8, 0, 20, 0, 0, 0, 1, 0, {4, 4, 4, 4}, {8, 8, 8, 8}, {0, 0, 0, 0}, 34, false, false, "Core", "", "", "", "Water", "level"},
		{"Scooter", 33554431, 104854558, 8, 0, 23, 0, 0, 0, 1, 0, {8, 8, 4, 4}, {16, 16, 8, 8}, {224, 64, 256, 80}, 35, false, false, "Misc", "", "", "", "Scoot", ""},
		{"Scooter (broken)", 33554431, 104854558, 8, 0, 23, 0, 0, 0, 1, 0, {20, 20, 4, 4}, {32, 32, 8, 8}, {256, 96, 320, 112}, 35, false, false, "Misc", "", "", "", "Scoot", "dead"},
		{"Blue robot (broken)", 33554431, 104854558, 8, 0, 23, 0, 0, 0, 1, 0, {8, 8, 4, 4}, {16, 16, 4, 4}, {192, 120, 224, 128}, 35, false, false, "Misc", "", "", "", "BRobt", "dead"},
		{"Grate 2", 33554431, 104854558, 8, 0, 2, 0, 0, 0, 1, 0, {4, 4, 4, 4}, {8, 8, 8, 8}, {112, 64, 128, 80}, 11, false, false, "Core", "", "", "", "Grate", "mouth"},
		{"Ironhead motion wall", 33554431, 104854558, 8, 0, 2, 0, 0, 0, 1, 0, {4, 4, 4, 4}, {16, 16, 8, 8}, {112, 64, 144, 80}, 12, false, false, "Main Artery", "", "", "", "Mtion", "wall"},
		{"Porcupine Fish", 33554431, 104854558, 32800, 3, 21, 50, 70, 1, 5, 0, {6, 6, 6, 6}, {8, 8, 8, 8}, {0, 0, 16, 16}, 13, false, false, "Unsure", "", "", "", "Enemy", "PFish"},
		{"Ironhead projectile", 33554431, 104854558, 0, 6, 22, 0, 0, 1, 0, 3, {6, 6, 10, 10}, {8, 8, 12, 12}, {208, 48, 224, 72}, 14, false, false, "Main Artery", "", "", "", "Proj", "IronH"},
		{"Water/wind particles", 33554431, 104854558, 262152, 0, 19, 0, 0, 1, 0, 0, {1, 1, 1, 1}, {1, 1, 1, 1}, {72, 16, 74, 18}, 0, false, false, "Misc", "", "", "", "Water", "curnt"},
		{"Dragon Zombie", 33554431, 104854558, 32800, 1000, 21, 52, 72, 3, 20, 10, {12, 12, 18, 12}, {20, 20, 24, 16}, {0, 0, 40, 40}, 19, false, false, "Egg2", "", "", "", "Enemy", "DragZ"},
		{"Dragon Zombie (dead)", 33554431, 104854558, 0, 16, 21, 52, 72, 3, 0, 0, {10, 10, 16, 12}, {20, 20, 24, 16}, {200, 0, 240, 40}, 19, false, false, "Egg2", "", "", "", "Dead", "DragZ"},
		{"Dragon Zombie Proj.", 33554431, 104854558, 32, 1, 20, 0, 70, 1, 0, 10, {6, 6, 4, 4}, {8, 8, 16, 8}, {184, 216, 200, 240}, 34, false, false, "Egg2", "", "", "", "Proj", "DragZ"},
		{"Critter (eggs2)", 33554431, 104854558, 163872, 4, 21, 51, 71, 2, 4, 5, {6, 6, 5, 5}, {8, 8, 8, 8}, {0, 80, 16, 96}, 19, false, false, "Egg2", "", "", "", "Enemy", "CrtHA"},
		{"Falling spike (small)", 33554431, 14757395255636523000, 131076, 4, 2, 0, 0, 0, 0, 5, {6, 6, 5, 2}, {8, 8, 8, 8}, {240, 80, 256, 96}, 2, false, false, "Egg2", "", "", "", "Spike", "FallS"},
		{"Falling spike (large)", 33554431, 104854558, 131140, 80, 2, 28, 12, 3, 0, 0, {8, 8, 17, 8}, {8, 8, 16, 8}, {112, 80, 128, 112}, 2, false, false, "Egg2", "", "", "", "Spike", "FallL"},
		{"Counter Bomb", 33554431, 104854558, 163880, 127, 21, 54, 72, 1, 7, 1, {10, 10, 10, 10}, {20, 20, 20, 20}, {80, 80, 120, 120}, 19, false, false, "Egg2", "", "", "", "Enemy", "CBomb"},
		{"C.Bomb's Countdown", 33554431, 104854558, 8, 0, 20, 0, 0, 1, 0, 0, {6, 6, 6, 6}, {8, 8, 8, 8}, {0, 144, 16, 160}, 34, false, false, "Misc", "", "", "", "Bloon", "CDown"},
		{"Basu (Eggs2)", 33554431, 104854558, 163872, 15, 21, 52, 71, 3, 9, 10, {6, 6, 6, 6}, {12, 12, 12, 12}, {248, 80, 272, 104}, 19, false, false, "Egg2", "", "", "", "Enemy", "Basu2"},
		{"Basu Proj. (Eggs2)", 33554431, 104854558, 0, 1, 21, 50, 70, 1, 0, 5, {2, 2, 2, 2}, {8, 8, 8, 8}, {232, 96, 248, 112}, 19, false, false, "Egg2", "", "", "", "Proj", "Basu2"},
		{"Beetle (Eggs2)", 33554431, 104854558, 163880, 4, 21, 53, 71, 2, 2, 5, {6, 5, 6, 6}, {8, 8, 8, 8}, {0, 112, 16, 128}, 19, false, false, "Egg2", "", "", "", "Enemy", "BtlF2"},
		{"Spikes (small)", 33554431, 104854558, 8, 0, 20, 0, 0, 0, 0, 5, {6, 6, 6, 6}, {8, 8, 8, 8}, {256, 200, 272, 216}, 34, false, false, "Misc", "", "", "", "Spike", ""},
		{"Sky Dragon", 33554431, 104854558, 131080, 0, 23, 0, 0, 3, 0, 0, {10, 10, 8, 12}, {20, 20, 24, 16}, {160, 152, 200, 192}, 35, false, false, "Misc", "", "", "", "Sky", "Dragn"},
		{"Night Spirit", 33554431, 104854558, 163840, 64, 21, 54, 72, 3, 22, 0, {8, 8, 16, 16}, {24, 24, 24, 24}, {0, 0, 48, 48}, 21, false, false, "Outer Wall", "", "", "", "Enemy", "NiteS"},
		{"Night Spirit projectile", 33554431, 104854558, 0, 0, 21, 54, 72, 2, 0, 5, {6, 6, 3, 3}, {8, 24, 8, 8}, {144, 48, 176, 64}, 21, false, false, "Outer Wall", "", "", "", "Proj", "NiteS"},
		{"Sandcroc (oside)", 33554431, 104854558, 163872, 30, 21, 52, 72, 2, 9, 0, {7, 7, 24, 8}, {24, 24, 24, 8}, {144, 96, 192, 128}, 21, false, false, "Outer Wall", "", "", "", "Enemy", "ScrcW"},
		{"Debug Cat", 33554431, 104854558, 0, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 16, 8}, {256, 192, 272, 216}, 35, false, false, "Characters", "", "", "", "Debug", "cat"},
		{"Itoh", 33554431, 104854558, 0, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {144, 64, 160, 80}, 35, false, false, "Characters", "", "", "", "Itoh", ""},
		{"Core Energy Ball", 33554431, 104854558, 8, 0, 22, 0, 0, 0, 0, 20, {12, 12, 12, 12}, {16, 16, 16, 16}, {256, 120, 288, 152}, 18, false, false, "Unsure", "", "", "", "Proj?", ""},
		{"Smoke Generator", 33554431, 104854558, 8, 0, 21, 0, 0, 0, 0, 0, {1, 1, 1, 1}, {1, 1, 1, 1}, {0, 0, 0, 0}, 0, false, false, "Misc", "", "", "", "Genrt", "Smoke"},
		{"Shovel Brigade", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {0, 64, 16, 80}, 0, false, false, "Characters", "", "", "", "SBrig", "stand"},
		{"Shov. Brigade (walk)", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {32, 64, 48, 80}, 0, false, false, "Characters", "", "", "", "SBrig", "walk"},
		{"Prison bars", 33554431, 104854558, 262212, 0, 20, 0, 0, 0, 0, 0, {5, 5, 16, 16}, {8, 8, 16, 16}, {96, 168, 112, 200}, 34, false, false, "Misc", "", "", "", "Prisn", "bars"},
		{"Momorin", 33554431, 104854558, 131072, 0, 23, 0, 0, 0, 0, 0, {5, 5, 16, 8}, {8, 8, 16, 8}, {80, 192, 96, 216}, 35, false, false, "Characters", "", "", "", "Momo", ""},
		{"Chie", 33554431, 104854558, 131072, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {112, 32, 128, 48}, 0, false, false, "Characters", "", "", "", "Chie", ""},
		{"Megane", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {64, 64, 80, 80}, 0, false, false, "Characters", "", "", "", "Megan", ""},
		{"Kanpachi", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 16, 8}, {256, 56, 272, 80}, 0, false, false, "Characters", "", "", "", "Kanpa", "stand"},
		{"Bucket", 33554431, 104854558, 262144, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {208, 32, 224, 48}, 0, false, false, "Misc", "", "", "", "Buckt", ""},
		{"Droll (guard)", 33554431, 104854558, 163840, 0, 22, 0, 0, 3, 0, 0, {8, 8, 18, 12}, {16, 16, 24, 16}, {0, 0, 32, 40}, 22, false, false, "Plantation", "", "", "", "Droll", "guard"},
		{"Red Flowers (sprouts)", 33554431, 104854558, 0, 0, 21, 0, 0, 2, 0, 0, {8, 8, 8, 8}, {24, 24, 8, 8}, {0, 96, 48, 112}, 0, false, false, "Misc", "", "", "", "RFlow", "sprts"},
		{"Red Flower (blooming)", 33554431, 104854558, 0, 0, 21, 0, 0, 2, 0, 0, {8, 8, 8, 8}, {24, 24, 24, 8}, {48, 96, 96, 128}, 0, false, false, "Misc", "", "", "", "RFlow", "bloom"},
		{"Rocket", 33554431, 104854558, 131140, 0, 21, 0, 0, 2, 0, 0, {12, 12, 6, 8}, {16, 16, 8, 8}, {176, 32, 208, 48}, 0, false, false, "Misc", "", "", "", "Rockt", ""},
		{"Orangeball", 33554431, 104854558, 32800, 64, 22, 52, 72, 3, 9, 2, {10, 10, 10, 10}, {16, 16, 16, 16}, {128, 0, 160, 32}, 22, false, false, "Plantation", "", "", "", "Enemy", "Orang"},
		{"Orangeball bat", 12841596, 53728286, 688168, 4, 22, 50, 70, 1, 2, 2, {4, 4, 7, 4}, {8, 8, 8, 8}, {256, 0, 272, 16}, 22, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Red Flowers (picked)", 33554431, 104854558, 262144, 0, 21, 0, 0, 2, 0, 0, {8, 8, 8, 8}, {24, 24, 8, 8}, {144, 96, 192, 112}, 0, false, false, "Misc", "", "", "", "RFlow", "pickd"},
		{"Midorin", 33554431, 104854558, 32929, 16, 21, 51, 71, 2, 2, 3, {5, 5, 4, 8}, {8, 8, 8, 8}, {192, 96, 208, 112}, 0, false, false, "Characters", "", "", "", "Midor", ""},
		{"Gunfish", 33554431, 104854558, 32801, 32, 22, 51, 71, 3, 20, 0, {11, 11, 8, 8}, {12, 12, 12, 12}, {128, 64, 152, 88}, 22, false, false, "Plantation", "", "", "", "Enemy", "Gunfs"},
		{"Gunfish projectile", 33554431, 104854558, 0, 0, 22, 51, 71, 3, 0, 3, {2, 2, 2, 2}, {4, 4, 4, 4}, {312, 32, 320, 40}, 22, false, false, "Plantation", "", "", "", "Proj", "Gunfs"},
		{"Press (sideways)", 33554431, 104854558, 131148, 32, 20, 52, 72, 2, 5, 0, {8, 8, 8, 8}, {8, 8, 8, 8}, {184, 200, 208, 216}, 34, false, false, "Misc", "", "", "", "Enemy", "PresK"},
		{"Cage bars", 33554431, 104854558, 8, 0, 2, 0, 0, 1, 0, 0, {2, 2, 2, 2}, {32, 32, 24, 8}, {192, 48, 256, 80}, 5, false, false, "Misc", "", "", "", "Cage", "bars"},
		{"Mimiga (jailed)", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {7, 7, 8, 8}, {8, 8, 8, 8}, {160, 64, 176, 80}, 0, false, false, "Characters", "", "", "", "Mimig", "jail"},
		{"Critter (Last cave)", 33554431, 104854558, 163873, 6, 21, 51, 71, 2, 1, 6, {6, 6, 4, 5}, {8, 8, 8, 8}, {0, 0, 16, 16}, 24, false, false, "Last Cave", "", "", "", "Enemy", "CritHR"},
		{"Bat (Last cave)", 33554431, 14757395255636523000, 32808, 1, 21, 50, 70, 1, 1, 2, {4, 4, 7, 4}, {8, 8, 8, 8}, {32, 32, 48, 48}, 24, false, false, "Last Cave", "", "", "", "Enemy", "BatRd"},
		{"Bat generator", 33554431, 104854558, 0, 0, 21, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "Last Cave", "", "", "", "Genrt", "BatRd"},
		{"Lava drop", 33554431, 104854558, 131072, 1, 21, 0, 0, 1, 0, 4, {2, 2, 7, 4}, {4, 4, 8, 8}, {96, 0, 104, 16}, 24, false, false, "Last Cave", "", "", "", "Acid", "drop"},
		{"Lava drop generator", 33554431, 104854558, 0, 0, 21, 0, 0, 1, 0, 0, {2, 2, 7, 4}, {4, 4, 8, 8}, {120, 0, 128, 16}, 24, false, false, "Last Cave", "", "", "", "Genrt", "Acid"},
		{"Press (proximity)", 14809212, 36758558, 163936, 32, 20, 52, 72, 3, 5, 0, {8, 8, 13, 12}, {8, 8, 12, 12}, {144, 112, 160, 136}, 34, false, false, "Misc", "", "", "", "Enemy", "PresP"},
		{"Misery (boss)", 33554431, 104854558, 163840, 500, 22, 57, 0, 2, 20, 1, {5, 5, 8, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 25, false, false, "Misery", "", "", "", "Boss", "Msry"},
		{"Boss Misery (vanish)", 33554431, 104854558, 0, 1, 22, 0, 70, 1, 0, 6, {4, 4, 4, 4}, {8, 8, 8, 8}, {32, 48, 48, 64}, 25, false, false, "Misery", "", "", "", "Boss", "MsryV"},
		{"Misery Energy Shot", 33554431, 104854558, 0, 0, 22, 0, 0, 2, 0, 1, {5, 5, 8, 8}, {8, 8, 8, 8}, {48, 48, 64, 64}, 25, false, false, "Misery", "", "", "", "Proj", "Msry1"},
		{"Misery lightning ball", 33554431, 104854558, 131072, 1, 22, 0, 70, 1, 0, 6, {4, 4, 4, 4}, {8, 8, 8, 8}, {0, 32, 16, 48}, 25, false, false, "Misery", "", "", "", "Proj", "Msry2"},
		{"Miserty lightning", 33554431, 104854558, 0, 1, 22, 0, 70, 1, 0, 10, {7, 7, 7, 7}, {8, 8, 16, 16}, {80, 32, 96, 64}, 25, false, false, "Misery", "", "", "", "Proj", "Msry3"},
		{"Misery bats", 33554431, 54522910, 688140, 2, 22, 50, 70, 1, 6, 2, {4, 4, 4, 4}, {8, 8, 8, 8}, {48, 32, 64, 48}, 25, false, false, "Crash", "", "", "", "CRASH", ""},
		{"EXP capsule", 33554431, 104854558, 32800, 102, 20, 39, 25, 2, 0, 0, {6, 6, 4, 4}, {8, 8, 8, 8}, {0, 64, 16, 80}, 34, false, false, "General", "", "", "", "Capsl", "enrgy"},
		{"Helicopter", 33554431, 104854558, 8, 0, 22, 0, 0, 2, 0, 0, {32, 32, 4, 4}, {64, 64, 56, 8}, {0, 0, 128, 64}, 23, false, false, "Sky", "", "", "", "Heli", "copter"},
		{"Helicopter blades", 33554431, 54522910, 524296, 0, 22, 0, 0, 2, 0, 0, {6, 6, 4, 4}, {56, 56, 8, 8}, {128, 0, 240, 16}, 23, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Doctor (facing away)", 33554431, 104854558, 0, 0, 23, 0, 0, 3, 0, 0, {6, 6, 6, 16}, {16, 8, 16, 16}, {48, 160, 72, 192}, 35, false, false, "Doctor", "", "", "", "Doctr", "back"},
		{"Red crystal", 33554431, 104854558, 12, 0, 20, 0, 0, 2, 0, 1, {4, 4, 8, 8}, {2, 2, 2, 2}, {176, 32, 184, 48}, 34, false, false, "Doctor", "", "", "", "Red", "Cryst"},
		{"Mimiga (sleeping)", 33554431, 104854558, 0, 0, 21, 0, 0, 2, 0, 0, {6, 6, 4, 4}, {8, 8, 8, 8}, {48, 32, 64, 48}, 0, false, false, "Characters", "", "", "", "Mimig", "sleep"},
		{"Curly (carried dead)", 33554431, 104854558, 8, 0, 23, 0, 0, 0, 0, 0, {4, 4, 8, 8}, {8, 8, 8, 8}, {224, 96, 240, 112}, 35, false, false, "Curly", "", "", "", "Carry", "Curl1"},
		{"ShovelBrigade (caged)", 33554431, 104854558, 131072, 0, 21, 0, 0, 0, 0, 0, {6, 6, 8, 8}, {8, 8, 8, 8}, {128, 64, 144, 80}, 0, false, false, "Characters", "", "", "", "SBrig", "cage"},
		{"Chie (caged)", 33554431, 104854558, 131072, 0, 21, 0, 0, 0, 0, 0, {6, 6, 8, 8}, {8, 8, 8, 8}, {112, 32, 128, 48}, 0, false, false, "Characters", "", "", "", "Chie", "cage"},
		{"Chaco (caged)", 33554431, 104854558, 131072, 0, 21, 0, 0, 0, 0, 0, {6, 6, 8, 8}, {8, 8, 8, 8}, {128, 0, 144, 16}, 0, false, false, "Characters", "", "", "", "Chaco", "cage"},
		{"Doctor (boss)", 33554431, 104854558, 32800, 300, 22, 52, 0, 3, 1, 3, {6, 6, 12, 16}, {16, 8, 16, 16}, {0, 0, 24, 32}, 26, false, false, "Doctor", "", "", "", "Boss", "Doct1"},
		{"Dr Red Wave", 33554431, 104854558, 12, 2, 22, 50, 70, 1, 0, 5, {4, 4, 4, 4}, {8, 8, 8, 8}, {288, 0, 304, 16}, 26, false, false, "Doctor", "", "", "", "Proj", "Doct1"},
		{"Dr Red Ball", 33554431, 104854558, 8, 0, 22, 50, 70, 1, 6, 0, {4, 4, 4, 4}, {8, 8, 8, 8}, {288, 16, 304, 32}, 26, false, false, "Unsure", "", "", "", "Proj", "RFBal"},
		{"Dr Red Ball (bouncing)", 33554431, 104854558, 4, 2, 22, 50, 70, 1, 0, 5, {4, 4, 4, 4}, {8, 8, 8, 8}, {304, 16, 320, 32}, 26, false, false, "Unsure", "", "", "", "Proj", "RSBal"},
		{"Muscle Doctor", 33554431, 104854558, 32800, 300, 22, 54, 0, 3, 1, 5, {10, 10, 8, 16}, {20, 20, 28, 20}, {0, 64, 40, 112}, 26, false, false, "Doctor", "", "", "", "Boss", "Doct2"},
		{"Igor (enemy)", 33554431, 104854558, 163873, 300, 21, 52, 72, 3, 19, 5, {8, 8, 8, 16}, {20, 20, 20, 20}, {40, 0, 80, 40}, 3, false, false, "Mimiga", "", "", "", "Enemy", "Igor"},
		{"Red Bat (bouncing)", 33554431, 104854558, 32, 2, 22, 0, 34, 1, 0, 2, {4, 4, 4, 4}, {8, 8, 8, 8}, {232, 0, 248, 16}, 26, false, false, "Doctor", "", "", "", "Enemy", "BatRE"},
		{"Doctor's blood", 4194304, 16839710, 0, 2, 20, 0, 34, 1, 0, 0, {4, 4, 4, 0}, {2, 2, 2, 2}, {170, 34, 174, 38}, 26, false, false, "Doctor", "", "", "", "Red", "enrgy"},
		{"Ironhead block", 33554431, 104854558, 12, 2, 2, 0, 34, 1, 0, 3, {6, 6, 6, 6}, {8, 8, 8, 8}, {112, 0, 128, 16}, 12, false, false, "Main Artery", "", "", "", "UBlok", ""},
		{"Ironhead block genr", 33554431, 104854558, 262144, 2, 2, 0, 34, 1, 0, 0, {8, 8, 8, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "Main Artery", "", "", "", "Genrt", "UBlok"},
		{"Droll projectile", 33554431, 104854558, 0, 0, 22, 0, 0, 3, 0, 6, {4, 4, 4, 4}, {12, 12, 12, 12}, {248, 40, 272, 64}, 22, false, false, "Plantation", "", "", "", "Proj", "Droll"},
		{"Droll", 33554431, 104854558, 163872, 64, 22, 54, 72, 3, 9, 3, {8, 8, 18, 12}, {16, 16, 24, 16}, {0, 0, 32, 40}, 22, false, false, "Plantation", "", "", "", "Enemy", "Droll"},
		{"Puppy (plantation)", 33554431, 104854558, 131072, 0, 21, 0, 0, 0, 0, 0, {4, 4, 8, 8}, {8, 8, 8, 8}, {272, 80, 288, 96}, 0, false, false, "Plantation", "", "", "", "Puppy", "items"},
		{"Red Demon", 33554431, 104854558, 163840, 300, 21, 54, 72, 3, 100, 10, {8, 8, 18, 12}, {16, 16, 24, 16}, {0, 64, 32, 104}, 24, false, false, "Last Cave", "", "", "", "Boss", "RDemn"},
		{"Red Demon Proj", 33554431, 104854558, 0, 0, 21, 0, 0, 3, 0, 6, {4, 4, 4, 4}, {12, 12, 12, 12}, {128, 0, 152, 24}, 24, false, false, "Last Cave", "", "", "", "Proj", "RDemn"},
		{"Little family", 33554431, 104854558, 0, 0, 8, 0, 0, 0, 0, 0, {4, 4, 4, 4}, {4, 4, 4, 4}, {0, 120, 8, 128}, 0, false, false, "Little", "", "", "", "Littl", "famly"},
		{"Falling block (large)", 33554431, 104854558, 8, 0, 2, 0, 0, 3, 0, 0, {16, 16, 17, 16}, {16, 16, 16, 16}, {0, 16, 32, 48}, 18, false, false, "Misc", "", "", "", "FBlck", "large"},
		{"Sue (teleported)", 33554431, 104854558, 524288, 0, 23, 50, 70, 2, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {112, 32, 128, 48}, 35, false, false, "Characters", "", "", "", "Sue", "telep"},
		{"Doctor (ghost)", 33554431, 104854558, 0, 0, 23, 50, 70, 2, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "Doctor", "", "", "", "Doctr", "enrgy"},
		{"UC_MiniActive", 33554431, 104854558, 131144, 0, 22, 50, 70, 2, 0, 0, {19, 19, 15, 15}, {32, 32, 20, 20}, {256, 120, 320, 160}, 27, false, false, "Unsure", "", "", "", "Mini", "UCore"},
		{"Misery (beast)", 33554431, 104854558, 32800, 1000, 21, 54, 72, 3, 1, 0, {8, 8, 12, 16}, {16, 16, 16, 16}, {0, 64, 32, 96}, 25, false, false, "Undead Core", "", "", "", "Enemy", "Msry"},
		{"Sue (beast)", 33554431, 104854558, 163872, 1000, 21, 57, 72, 3, 1, 0, {8, 8, 8, 13}, {16, 16, 16, 16}, {0, 128, 32, 160}, 25, false, false, "Undead Core", "", "", "", "Enemy", "Sue"},
		{"UC_P_Spiral", 33554431, 104854558, 12, 2, 22, 50, 70, 1, 0, 4, {4, 4, 4, 4}, {8, 8, 8, 8}, {232, 104, 248, 120}, 27, false, false, "Unsure", "", "", "", "Proj", "OSpir"},
		{"UC_P_Spiral_Trail", 33554431, 104854558, 8, 0, 22, 50, 70, 1, 6, 0, {4, 4, 4, 4}, {8, 8, 8, 8}, {232, 120, 248, 136}, 27, false, false, "Unsure", "", "", "", "Orang", "Dot"},
		{"Orange Smoke", 33554431, 104854558, 0, 1, 22, 0, 0, 2, 0, 0, {4, 4, 4, 4}, {8, 8, 8, 8}, {64, 224, 80, 240}, 27, false, false, "Unsure", "", "", "", "Orang", "Smoke"},
		{"UC_ExplodeRock", 33554431, 104854558, 131072, 1, 22, 0, 0, 2, 0, 4, {4, 4, 4, 4}, {8, 8, 8, 8}, {232, 72, 248, 88}, 27, false, false, "Unsure", "", "", "", "Proj", "GRock"},
		{"Critter (misery)", 33554431, 104854558, 131072, 2, 21, 51, 71, 1, 1, 0, {6, 6, 4, 5}, {8, 8, 8, 8}, {160, 32, 176, 48}, 25, false, false, "Undead Core", "", "", "", "Enemy", "CrtHO"},
		{"Bat (misery)", 33554431, 104854558, 131080, 2, 21, 51, 71, 1, 1, 0, {6, 6, 4, 5}, {8, 8, 8, 8}, {112, 32, 128, 48}, 25, false, false, "Undead Core", "", "", "", "Enemy", "BatOr"},
		{"UC_MiniInactive", 33554431, 104854558, 72, 0, 22, 50, 70, 2, 0, 0, {19, 19, 15, 15}, {32, 32, 20, 20}, {256, 0, 320, 40}, 27, false, false, "Undead Core", "", "", "", "UCore", "Mini"},
		{"Quake", 33554431, 104854558, 0, 0, 20, 50, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "General", "", "", "", "Quake", ""},
		{"UC_EnergyShot", 33554431, 104854558, 8, 0, 22, 0, 0, 0, 0, 10, {15, 15, 15, 15}, {20, 20, 20, 20}, {240, 200, 280, 240}, 27, false, false, "Unsure", "", "", "", "Proj", "HEner"},
		{"Quake + FallBlockGen", 33554431, 104854558, 0, 0, 20, 50, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "Misc", "", "", "", "Quake", "FBlck"},
		{"Cloud", 33554431, 104854558, 262152, 0, 2, 50, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 208, 64}, 19, false, false, "Sky", "", "", "", "Cloud", ""},
		{"Cloud Generator", 33554431, 104854558, 262144, 0, 20, 50, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "Sky", "", "", "", "Genrt", "Cloud"},
		{"Sue (dragon's mouth)", 33554431, 54522910, 262144, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {112, 48, 128, 64}, 0, false, false, "Crash", "", "", "", "CRASH", "SOME"},
		{"Doctor (intro)", 33554431, 104854558, 0, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 16, 16}, {72, 128, 88, 160}, 0, false, false, "Intro", "", "", "", "Intro", "Doctr"},
		{"Balrog/Misery (intro)", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {24, 24, 24, 24}, {0, 0, 48, 48}, 28, false, false, "Intro", "", "", "", "Intro", "Bubbl"},
		{"Demon crown (intro)", 33554431, 104854558, 262144, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {192, 80, 208, 96}, 0, false, false, "Intro", "", "", "", "Intro", "Crown"},
		{"Fish missle (misery)", 33554431, 104854558, 131112, 1, 21, 51, 72, 1, 1, 3, {4, 4, 4, 4}, {8, 8, 8, 8}, {144, 0, 160, 16}, 25, false, false, "Undead Core", "", "", "", "Enemy", "FishO"},
		{"Camera focus marker", 33554431, 53538822, 131080, 0, 20, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "Unsure", "", "", "", "End", "Scene"},
		{"Curly's Machine Gun", 33554431, 54522910, 8, 0, 20, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {216, 152, 232, 168}, 0, false, false, "Unsure", "", "", "", "?", ""},
		{"Gaudi (hospital)", 33554431, 104854558, 0, 0, 22, 51, 71, 3, 8, 5, {8, 8, 8, 8}, {12, 12, 8, 8}, {0, 176, 24, 192}, 5, false, false, "Labyrinth", "", "", "", "Gaudi", "sit"},
		{"Small puppy", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {4, 4, 8, 8}, {8, 8, 8, 8}, {160, 144, 176, 160}, 6, false, false, "Sand Zone", "", "", "", "Puppy", "small"},
		{"Balrog (nurse)", 33554431, 104854558, 33, 96, 21, 52, 72, 3, 18, 0, {14, 14, 11, 12}, {20, 20, 12, 12}, {240, 96, 280, 128}, 0, false, false, "Characters", "", "", "", "Blrg", "nurse"},
		{"Caged Santa", 33554431, 104854558, 131072, 0, 23, 0, 0, 0, 0, 0, {6, 6, 8, 8}, {8, 8, 8, 8}, {0, 32, 16, 48}, 0, false, false, "Characters", "", "", "", "Santa", "cage"},
		{"Stumpy", 33554431, 104854558, 163872, 4, 22, 53, 71, 2, 2, 2, {6, 5, 6, 6}, {8, 8, 8, 8}, {128, 112, 144, 128}, 22, false, false, "Plantation", "", "", "", "Enemy", "Stump"},
		{"Bute", 33554431, 104854558, 163840, 1000, 21, 55, 70, 1, 3, 0, {6, 6, 7, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 29, false, false, "Hell", "", "", "", "Enemy", "Bute"},
		{"Bute (sword)", 33554431, 104854558, 32768, 1000, 21, 55, 70, 1, 3, 3, {8, 8, 7, 8}, {12, 12, 8, 8}, {32, 0, 56, 16}, 29, false, false, "Hell", "", "", "", "Enemy", "ButeS"},
		{"Bute archer", 33554431, 104854558, 163872, 1000, 21, 55, 70, 1, 3, 3, {8, 8, 7, 8}, {12, 12, 12, 12}, {0, 32, 24, 56}, 29, false, false, "Hell", "", "", "", "Enemy", "ButeA"},
		{"Bute arrow projectile", 33554431, 104854558, 8, 4, 20, 51, 70, 1, 0, 8, {4, 4, 2, 2}, {8, 8, 8, 8}, {0, 160, 16, 176}, 0, false, false, "Hell", "", "", "", "Proj", "ButeA"},
		{"Ma Pignon", 33554431, 104854558, 163840, 300, 21, 51, 71, 2, 1, 0, {6, 6, 5, 5}, {8, 8, 8, 8}, {128, 0, 144, 16}, 8, false, false, "Graveyard", "", "", "", "Boss", "MPign"},
		{"Ma Pignon rock", 33554431, 104854558, 8, 0, 2, 0, 0, 2, 0, 0, {4, 4, 4, 4}, {8, 8, 8, 8}, {64, 64, 80, 80}, 11, false, false, "Unsure", "", "", "", "???", ""},
		{"Ma Pignon clone", 33554431, 104854558, 32776, 5, 21, 51, 71, 2, 1, 3, {6, 6, 5, 5}, {8, 8, 8, 8}, {128, 0, 144, 16}, 8, false, false, "Unsure", "", "", "", "Enemy", "???"},
		{"Bute (dead)", 33554431, 104854558, 0, 1000, 21, 51, 70, 1, 3, 0, {8, 8, 7, 8}, {12, 12, 12, 12}, {248, 32, 272, 56}, 29, false, false, "Hell", "", "", "", "Dead", "Bute"},
		{"Mesa", 33554431, 104854558, 163873, 1000, 21, 52, 72, 3, 9, 6, {8, 8, 18, 12}, {16, 16, 24, 16}, {0, 80, 32, 120}, 29, false, false, "Hell", "", "", "", "Enemy", "Mesa"},
		{"Mesa (dead)", 33554431, 104854558, 0, 1000, 21, 52, 72, 3, 3, 0, {8, 8, 18, 12}, {16, 16, 24, 16}, {224, 80, 256, 120}, 29, false, false, "Hell", "", "", "", "Dead", "Mesa"},
		{"Mesa block", 33554431, 19984414, 524300, 1, 2, 50, 70, 1, 6, 10, {8, 8, 8, 8}, {8, 8, 8, 8}, {16, 0, 32, 16}, 20, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Curly (carried,  shoot)", 33554431, 104854558, 8, 0, 23, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {16, 96, 32, 112}, 0, false, false, "Curly", "", "", "", "Carry", "Curl2"},
		{"Curly's Nemesis", 33554431, 54522910, 8, 0, 20, 0, 0, 0, 0, 0, {5, 5, 8, 8}, {8, 8, 8, 8}, {136, 152, 152, 168}, 0, false, false, "Unsure", "", "", "", "?", ""},
		{"Deleet", 33554431, 104854558, 32, 1000, 20, 54, 72, 3, 1, 0, {12, 12, 12, 12}, {12, 12, 12, 12}, {272, 216, 296, 240}, 0, false, false, "Hell", "", "", "", "Enemy", "Delet"},
		{"Bute (spinning)", 33554431, 104854558, 163848, 1000, 21, 55, 70, 1, 3, 0, {6, 6, 7, 8}, {8, 8, 12, 8}, {216, 32, 232, 56}, 29, false, false, "Hell", "", "", "", "Enemy", "ButeG"},
		{"Bute Generator", 33554431, 104854558, 8, 1000, 21, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "Hell", "", "", "", "Genrt", "Bute"},
		{"HeavyPressLightning", 33554431, 104854558, 8, 1000, 22, 0, 0, 1, 0, 0, {10, 10, 8, 88}, {16, 16, 16, 16}, {240, 96, 272, 128}, 30, false, false, "Hell", "", "", "", "Proj", "HeavP"},
		{"Sue/Itoh humans", 33554431, 104854558, 0, 1000, 21, 0, 0, 2, 0, 0, {8, 8, 8, 8}, {8, 8, 16, 8}, {64, 128, 80, 152}, 0, false, false, "Characters", "", "", "", "turn", "human"},
		{"Sneeze", 33554431, 54522910, 524288, 1000, 21, 0, 0, 2, 0, 0, {8, 8, 8, 8}, {8, 8, 8, 8}, {240, 80, 256, 96}, 0, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Human contraption", 33554431, 104854558, 0, 1000, 2, 0, 0, 1, 0, 0, {8, 8, 8, 8}, {16, 16, 40, 8}, {96, 0, 128, 48}, 22, false, false, "Laboratory", "", "", "", "human", "machn"},
		{"Laboratory fan", 33554431, 104854558, 0, 1000, 2, 0, 0, 1, 0, 0, {8, 8, 8, 8}, {8, 8, 8, 8}, {48, 0, 64, 16}, 22, false, false, "Laboratory", "", "", "", "Bldg", "fan"},
		{"Rolling", 33554431, 104854558, 4, 1000, 20, 0, 0, 2, 0, 6, {7, 7, 7, 7}, {8, 8, 8, 8}, {144, 136, 160, 152}, 0, false, false, "Hell", "", "", "", "Enemy", "Rolln"},
		{"Ballos bone projectile", 33554431, 104854558, 32, 1, 21, 0, 0, 2, 0, 4, {4, 4, 4, 4}, {8, 8, 8, 8}, {288, 80, 304, 96}, 31, false, false, "Ballos", "", "", "", "Proj", "Blls1"},
		{"Ballos shockwave", 33554431, 104854558, 0, 1, 19, 0, 0, 2, 0, 8, {4, 4, 4, 4}, {12, 12, 12, 12}, {144, 96, 168, 120}, 0, false, false, "Ballos", "", "", "", "Proj", "Blls2"},
		{"Ballos lightning", 33554431, 104854558, 8, 1, 19, 0, 0, 2, 0, 0, {4, 4, 4, 4}, {12, 12, 12, 12}, {80, 120, 104, 144}, 0, false, false, "Ballos", "", "", "", "Proj", "Blls3"},
		{"Sweat", 33554431, 104854558, 8, 0, 20, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {4, 4, 8, 8}, {160, 184, 168, 200}, 0, false, false, "Misc", "", "", "", "Sweat", ""},
		{"Ikachan", 33554431, 104854558, 8, 0, 21, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {8, 8, 8, 8}, {0, 16, 16, 32}, 13, false, false, "Main Artery", "", "", "", "Ika-", "chan"},
		{"Ikachan generator", 33554431, 104854558, 8, 0, 20, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "Unsure", "", "", "", "Genrt", "Ika"},
		{"Numhachi", 33554431, 104854558, 0, 0, 22, 0, 0, 3, 0, 0, {8, 8, 18, 12}, {16, 16, 24, 16}, {288, 112, 320, 152}, 22, false, false, "Plantation", "", "", "", "Numa", "hachi"},
		{"Green Devil", 33554431, 104854558, 32776, 2, 21, 53, 71, 1, 5, 0, {6, 6, 4, 5}, {8, 8, 8, 8}, {288, 0, 304, 16}, 31, false, false, "Ballos", "", "", "", "Enemy", "GDevl"},
		{"Green Devil generator", 33554431, 104854558, 8, 0, 20, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "Ballos", "", "", "", "Genrt", "GDevl"},
		{"Ballos", 33554431, 104854558, 164356, 1000, 21, 54, 0, 3, 1, 0, {12, 12, 18, 14}, {24, 24, 24, 16}, {0, 0, 48, 40}, 31, false, false, "Ballos", "", "", "", "Boss", "Blls1"},
		{"Ballos 1 head", 14762108, 16835614, 0, 0, 21, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {16, 16, 8, 8}, {288, 32, 320, 48}, 31, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Ballos 3 eye", 33554431, 54522910, 688140, 0, 22, 52, 71, 2, 1, 0, {12, 12, 12, 12}, {20, 20, 20, 20}, {240, 48, 280, 88}, 32, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Ballos 2 cutscene", 33554431, 54522910, 524296, 0, 22, 0, 0, 3, 0, 0, {0, 0, 0, 0}, {60, 60, 60, 60}, {0, 0, 120, 120}, 32, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Ballos 2 eyes", 33554431, 54522910, 524296, 0, 22, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {12, 12, 0, 0}, {272, 0, 296, 16}, 32, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Ballos skull projectile", 33554431, 104854558, 524292, 0, 2, 0, 0, 1, 0, 4, {6, 6, 6, 6}, {8, 8, 8, 8}, {128, 176, 144, 192}, 20, false, false, "Ballos", "", "", "", "Proj", "Blls4"},
		{"Ballos 4 orbit platform", 13103678, 53735454, 524360, 0, 22, 0, 0, 2, 1, 0, {16, 16, 9, 2}, {16, 16, 8, 8}, {240, 0, 272, 16}, 32, false, false, "Crash", "", "", "", "CRASH", ""},
		{"Hoppy", 33554431, 104854558, 163872, 10, 21, 50, 70, 2, 1, 4, {8, 8, 6, 6}, {8, 8, 8, 8}, {256, 48, 272, 64}, 21, false, false, "Outer Wall", "", "", "", "Enemy", "Hoppy"},
		{"Ballos 4 spikes", 33554431, 104854558, 8, 0, 2, 0, 0, 1, 0, 0, {8, 24, 8, 8}, {8, 24, 16, 8}, {128, 152, 160, 176}, 20, false, false, "Ballos", "", "", "", "Spike", "Blls"},
		{"Statue", 33554431, 104854558, 8, 1000, 20, 0, 0, 2, 0, 0, {4, 4, 3, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 0, false, false, "Statue", "", "", "", "Statu", ""},
		{"Flying Bute archer", 33554431, 104854558, 131112, 4, 21, 55, 70, 1, 6, 3, {8, 8, 7, 8}, {12, 12, 12, 12}, {24, 160, 48, 184}, 31, false, false, "Ballos", "", "", "", "Enemy", "ButRA"},
		{"Statue (shootable)", 33554431, 104854558, 0, 1000, 2, 12, 44, 3, 20, 0, {8, 8, 12, 12}, {16, 16, 20, 20}, {0, 96, 32, 136}, 20, false, false, "Statue", "", "", "", "Statu", "shoot"},
		{"Ending characters", 33554431, 104854558, 0, 100, 23, 0, 0, 2, 0, 0, {8, 8, 8, 8}, {8, 8, 8, 8}, {304, 48, 320, 64}, 0, false, false, "Characters", "", "", "", "King", "sword"},
		{"Bute with sword (fly)", 33554431, 104854558, 32776, 4, 21, 55, 70, 1, 3, 0, {6, 6, 7, 8}, {8, 8, 12, 8}, {168, 160, 184, 184}, 31, false, false, "Ballos", "", "", "", "Enemy", "ButRS"},
		{"Invis. deathtrap wall", 33554431, 104854558, 8, 0, 20, 0, 0, 1, 0, 127, {6, 6, 8, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "Unsure", "", "", "", "Invis", "Trap"},
		{"Quote&Curly on Balrg", 33554431, 54522910, 524296, 0, 0, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "Crash", "", "", "", "CRASH", "SOME"},
		{"Balrog rescue", 33554431, 104854558, 8, 0, 21, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {20, 20, 12, 12}, {240, 128, 280, 152}, 0, false, false, "Sky", "", "", "", "Blrg", "rescu"},
		{"Puppy ghost", 33554431, 104854558, 8, 0, 20, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {8, 8, 8, 8}, {224, 136, 240, 152}, 0, false, false, "Ballos", "", "", "", "Puppy", "ghost"},
		{"Misery (hair blowing)", 33554431, 104854558, 0, 0, 21, 0, 0, 1, 0, 0, {0, 0, 0, 0}, {8, 8, 16, 8}, {272, 8, 288, 32}, 25, false, false, "Misery", "", "", "", "Msry", "wind"},
		{"Water droplet gen.", 33554431, 104854558, 0, 0, 20, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "Misc", "", "", "", "Genrt", "WDrop"},
		{"Thank you", 33554431, 104854558, 8, 0, 21, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {24, 24, 4, 4}, {0, 176, 48, 184}, 0, false, false, "Misc", "", "", "", "Thank", "you!"},
		{"Screen blocker", 33554431, 104854558, 32800, 1000, 21, 51, 0, 2, 3, 5, {6, 6, 6, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "Misc", "", "", "", "Scrn", "Blckr"},
		{"nothing", 33554431, 104854558, 0, 1, 21, 0, 0, 2, 0, 0, {6, 6, 6, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "", ""},
		{"Auto-Run Script", 33554431, 104854558, 0, 5, 21, 21, 70, 1, 0, 5, {4, 4, 4, 4}, {4, 4, 4, 4}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", ""},
		{"Map Tile Manipulator", 33554431, 104854558, 32801, 16, 21, 52, 72, 3, 7, 2, {9, 9, 6, 7}, {12, 12, 12, 12}, {0, 0, 0, 0}, 0, false, false, "Misc", "", "", "", "Tile", "Chngr"},
		{"", 33554431, 104854558, 32801, 5, 21, 51, 71, 2, 3, 2, {6, 6, 4, 5}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "", ""},
		{"", 33554431, 104854558, 32800, 4, 21, 50, 70, 1, 3, 2, {4, 4, 7, 4}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "", ""},
		{"", 33554431, 104854558, 0, 0, 0, 0, 0, 0, 0, 0, {1, 1, 1, 1}, {1, 1, 1, 1}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 32800, 1000, 21, 54, 71, 3, 2, 4, {8, 8, 11, 5}, {12, 12, 16, 8}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 32800, 1000, 21, 51, 0, 2, 12, 5, {6, 6, 6, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 32800, 1000, 2, 71, 44, 3, 0, 5, {12, 12, 12, 12}, {12, 12, 12, 12}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 0, 20, 0, 0, 2, 0, 0, {6, 6, 6, 6}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "Misc", "Is Right / Is Bottom", "Is Horizontal", "", "Screen", "ScrnBl"},
		{"", 33554431, 104854558, 0, 1, 16, 0, 0, 3, 0, 0, {6, 5, 6, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "Networking", "", "", "", "Net", "NetPlr"},
		{"", 33554431, 37745694, 229408, 1000, 22, 52, 44, 3, 30, 0, {32, 32, 48, 64}, {64, 64, 64, 64}, {0, 0, 128, 128}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 9000, 22, 0, 0, 0, 0, 0, {48, 48, 48, 64}, {64, 64, 64, 64}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 1, 16, 0, 0, 3, 0, 0, {6, 5, 6, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 1, 0, 0, 0, 0, 0, 0, {6, 6, 6, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "Misc", "", "", "", "AutoS", "AutoSc"},
		{"", 33554431, 104854558, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "Misc", "", "", "", "MapTM", "MapMan"},
		{"", 33554431, 104854558, 0, 0, 21, 0, 0, 0, 0, 0, {30, 36, 3, 24}, {36, 36, 72, 24}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 32768, 1000, 22, 52, 44, 3, 30, 0, {5, 5, 48, 64}, {64, 64, 64, 64}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 1, 16, 0, 0, 3, 0, 0, {6, 5, 6, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 1, 16, 0, 0, 3, 0, 0, {6, 5, 6, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"Player (agility)", 10482238, 37744670, 589824, 1, 16, 0, 0, 3, 0, 0, {6, 5, 6, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 0, false, false, "CaveNet", "", "", "", "Net", "Agility"},
		{"", 33554431, 104854558, 0, 0, 8, 0, 0, 0, 0, 0, {4, 4, 2, 2}, {4, 4, 2, 2}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 9, 96, 22, 52, 72, 3, 18, 0, {14, 14, 11, 12}, {20, 20, 12, 12}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 1, 16, 0, 0, 3, 0, 0, {6, 5, 6, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "Cutscene", "", "", "", "Curly", "Curly"},
		{"", 33554431, 104854558, 0, 0, 20, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "Misc", "", "", "", "Wether", "Wether"},
		{"", 33554431, 104854558, 0, 0, 19, 0, 0, 2, 0, 0, {1, 1, 1, 1}, {1, 1, 1, 1}, {0, 0, 0, 0}, 0, false, false, "Weather", "", "", "", "WRain", "WRain"},
		{"Player (carried)", 67108863, 109047839, 589832, 1, 16, 0, 0, 0, 0, 0, {5, 5, 5, 5}, {8, 8, 8, 8}, {0, 0, 16, 16}, 0, false, false, "CaveNet", "", "", "", "Net", "Carry"},
		{"", 33554431, 104854558, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 0, 70, 0, 0, 20, 0, 0, 2, 0, 0, {6, 6, 6, 6}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "Networking", "", "", "", "Net", "TSpwn"},
		{"", 33554431, 104854558, 0, 1, 22, 0, 0, 0, 0, 0, {8, 8, 8, 8}, {8, 32, 8, 77}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 1, 22, 0, 0, 0, 0, 0, {8, 8, 8, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"Grate (tutorial)", 0, 0, 262152, 0, 2, 0, 0, 0, 0, 0, {8, 8, 8, 8}, {8, 8, 8, 8}, {0, 0, 0, 0}, 0, false, false, "Tutorial", "", "", "", "Net", "NTut"},
		{"Player (dancing)", 33554431, 104854558, 65537, 0, 21, 0, 0, 0, 0, 0, {4, 4, 3, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 35, false, false, "CaveNet", "", "", "", "Net", "Dance"},
		{"Player (downed)", 33554431, 104854558, 589824, 1, 16, 0, 0, 3, 0, 0, {6, 5, 6, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 0, false, false, "CaveNet", "", "", "Net", "Downd" },
		{"Player (teleport)", 33554431, 104854558, 589824, 1, 16, 0, 0, 3, 0, 0, {6, 5, 6, 8}, {8, 8, 8, 8}, {0, 0, 16, 16}, 0, false, false, "CaveNet", "", "", "Net", "Telprt" },
		{"Player (fishing)", 5198460, 53998622, 589824, 1, 16, 0, 0, 3, 0, 0, {6, 5, 7, 7}, {12, 11, 7, 7}, {0, 0, 16, 16}, 0, false, false, "CaveNet", "", "", "", "Net", "Fish"},
		{"Detail NPC (netlobby)", 12, 16106192902, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "NetLobby", "", "Face Right", "", "Net", "Detail"},
		{"", 33554431, 104854558, 131072, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" },
		{"", 33554431, 104854558, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, false, false, "", "", "", "", "" }
	};

	return (iIndex < 0 || iIndex >= (sizeof(gpzDefaultNPCs) / sizeof(PXMOD_NPC_STRUCT)) ? NULL : (const PXMOD_NPC_STRUCT*)&gpzDefaultNPCs[iIndex]);
}

const PXMOD_WEAPON_STRUCT* ModConfigResource::GetDefaultWeapon(int iIndex)
{
	static PXMOD_WEAPON_STRUCT gpzDefaultWeapons[] =
	{
		{ { { 0, 0, { 0, 0, 0, 0 } }, { 1, 0, { 0, 0, 0, 0 } }, { 2, 100, { 0, 0, 0, 0 } } }, { 0, 0, 0, 0, 0, 0 }, "Nothing" },
		{ { { 3, 30, { 8, 8, 8, 8 } }, { 4, 40, { 8, 8, 8, 8 } }, { 5, 16, { 8, 8, 8, 8 } } }, { 24, 0, 48, 16, 8, 0 }, "Snake" },
		{ { { 6, 10, { 8, 8, 8, 8 } }, { 7, 20, { 8, 8, 8, 8 } }, { 8, 10, { 8, 8, 8, 8 } } }, { 48, 0, 72, 16, 8, 0 }, "Polar Star" },
		{ { { 9, 10, { 8, 8, 8, 8 } }, { 10, 20, { 8, 8, 8, 8 } }, { 11, 20, { 8, 8, 8, 8 } } }, { 72, 0, 96, 16, 8, 0 }, "Fireball" },
		{ { { 12, 30, { 8, 8, 8, 8 } }, { 13, 40, { 8, 8, 8, 8 } }, { 14, 10, { 8, 8, 8, 8 } } }, { 96, 0, 120, 16, 8, 0 }, "Machine Gun" },
		{ { { 15, 10, { 8, 8, 8, 8 } }, { 16, 20, { 8, 8, 8, 8 } }, { 17, 10, { 0, 0, 0, 0 } } }, { 120, 0, 144, 16, 8, 0 }, "Missle Launcher" },
		{ { { 18, 10, { 0, 0, 0, 0 } }, { 19, 20, { 0, 0, 0, 0 } }, { 20, 30, { 0, 0, 0, 0 } } }, { 0, 0, 0, 0, 0, 0 }, "" },
		{ { { 21, 10, { 4, 4, 4, 4 } }, { 22, 20, { 4, 4, 4, 4 } }, { 23, 5, { 4, 4, 4, 4 } } }, { 168, 0, 192, 16, 8, 0 }, "Bubbler" },
		{ { { 24, 10, { 4, 4, 4, 4 } }, { 25, 20, { 12, 12, 12, 12 } }, { 26, 100, { 0, 0, 0, 0 } } }, { 0, 0, 0, 0, 8, 8 }, "" },
		{ { { 27, 30, { 8, 8, 8, 8 } }, { 28, 60, { 12, 12, 12, 12 } }, { 29, 0, { 12, 12, 12, 12 } } }, { 0, 0, 24, 16, 0, 0 }, "Blade" },
		{ { { 30, 30, { 8, 8, 8, 8 } }, { 31, 60, { 8, 8, 8, 8 } }, { 32, 10, { 8, 8, 8, 8 } } }, { 240, 0, 264, 16, 8, 0 }, "Super Missle Launcher" },
		{ { { 33, 10, { 0, 0, 0, 0 } }, { 34, 20, { 0, 0, 0, 0 } }, { 35, 100, { 0, 0, 0, 0 } } }, { 0, 0, 0, 0, 0, 0 }, "" },
		{ { { 36, 1, { 8, 8, 24, 8 } }, { 37, 1, { 8, 8, 24, 8 } }, { 38, 1, { 8, 8, 24, 8 } } }, { 288, 0, 312, 16, 8, 0 }, "Nemesis" },
		{ { { 39, 40, { 8, 8, 8, 8 } }, { 40, 60, { 8, 8, 8, 8 } }, { 41, 200, { 8, 8, 8, 8 } } }, { 0, 96, 24, 112, 8, 0 }, "Spur" },
		{ { { 42, 10, { 4, 4, 4, 4 } }, { 43, 20, { 4, 4, 4, 4 } }, { 44, 30, { 4, 4, 4, 4 } } }, { 0, 0, 24, 16, 0, 0 }, "Agility" },
		{ { { 45, 2, { 8, 8, 24, 8 } }, { 46, 2, { 0, 0, 0, 0 } }, { 47, 1, { 4, 4, 4, 4 } } }, { 0, 0, 0, 0, 0, 0 }, "Whimsical Star" },
		{ { { 48, 128, { 8, 8, 24, 8 } }, { 49, 12, { 0, 0, 0, 0 } }, { 50, 8, { 1, 1, 1, 1 } } }, { 0, 0, 0, 0, 0, 0 }, "" }
	};

	return (iIndex < 0 || iIndex >= (sizeof(gpzDefaultWeapons) / sizeof(PXMOD_WEAPON_STRUCT)) ? NULL : (const PXMOD_WEAPON_STRUCT*)&gpzDefaultWeapons[iIndex]);
}

const PXMOD_BULLET_STRUCT* ModConfigResource::GetDefaultBullet(int iIndex)
{
	static PXMOD_BULLET_STRUCT gpzDefaultBullets[] =
	{
		{ 0, 0, 0, 0, { 0, 0 }, { 0, 0 }, { 0, 0, 0, 0 }, "Nothing_L1" },
		{ 0, 0, 0, 0, { 0, 0 }, { 0, 0 }, { 0, 0, 0, 0 }, "Nothing_L2" },
		{ 0, 0, 0, 0, { 0, 0 }, { 0, 0 }, { 0, 0, 0, 0 }, "Nothing_L3" },
		{ 36, 4, 1, 20, { 4, 4 }, { 2, 2 }, { 136, 152, 80, 96 }, "Snake_L1" },
		{ 36, 6, 1, 23, { 4, 4 }, { 2, 2 }, { 120, 136, 80, 96 }, "Snake_L2" },
		{ 36, 8, 1, 30, { 4, 4 }, { 2, 2 }, { 136, 152, 64, 80 }, "Snake_L3" },
		{ 32, 1, 1, 8, { 6, 6 }, { 2, 2 }, { 128, 144, 32, 48 }, "Polar Star_L1" },
		{ 32, 2, 1, 12, { 6, 6 }, { 2, 2 }, { 160, 176, 32, 48 }, "Polar Star_L2" },
		{ 32, 4, 1, 16, { 6, 6 }, { 2, 2 }, { 128, 144, 48, 64 }, "Polar Star_L3" },
		{ 8, 2, 2, 100, { 8, 16 }, { 4, 2 }, { 128, 144, 0, 16 }, "Fireball_L1" },
		{ 8, 3, 2, 100, { 4, 4 }, { 4, 2 }, { 128, 144, 16, 32 }, "Fireball_L2" },
		{ 8, 3, 2, 100, { 4, 4 }, { 4, 2 }, { 192, 208, 16, 32 }, "Fireball_L3" },
		{ 32, 2, 1, 20, { 2, 2 }, { 2, 2 }, { 64, 80, 0, 16 }, "Machine Gun_L1" },
		{ 32, 4, 1, 20, { 2, 2 }, { 2, 2 }, { 64, 80, 16, 32 }, "Machine Gun_L2" },
		{ 32, 6, 1, 20, { 2, 2 }, { 2, 2 }, { 64, 80, 32, 48 }, "Machine Gun_L3" },
		{ 40, 0, 10, 50, { 2, 2 }, { 2, 2 }, { 0, 16, 0, 16 }, "Missle Launcher_L1" },
		{ 40, 0, 10, 70, { 4, 4 }, { 4, 4 }, { 0, 16, 16, 32 }, "Missle Launcher_L2" },
		{ 40, 0, 10, 90, { 4, 4 }, { 0, 0 }, { 0, 16, 32, 48 }, "Missle Launcher_L3" },
		{ 20, 1, 100, 100, { 16, 16 }, { 0, 0 }, { 0, 16, 0, 16 }, "_L1" },
		{ 20, 1, 100, 100, { 16, 16 }, { 0, 0 }, { 0, 16, 0, 16 }, "_L2" },
		{ 20, 1, 100, 100, { 16, 16 }, { 0, 0 }, { 0, 16, 0, 16 }, "_L3" },
		{ 8, 1, 1, 20, { 2, 2 }, { 2, 2 }, { 216, 224, 0, 8 }, "Bubbler_L1" },
		{ 8, 2, 1, 20, { 2, 2 }, { 2, 2 }, { 216, 224, 8, 16 }, "Bubbler_L2" },
		{ 8, 2, 1, 20, { 4, 4 }, { 4, 4 }, { 248, 256, 24, 32 }, "Bubbler_L3" },
		{ 32, 3, 1, 32, { 2, 2 }, { 2, 2 }, { 224, 232, 0, 8 }, "BubblerThorn" },
		{ 36, 0, 100, 0, { 8, 8 }, { 8, 8 }, { 0, 24, 64, 88 }, "BladeEdge" },
		{ 4, 127, 1, 2, { 8, 4 }, { 8, 4 }, { 0, 0, 0, 0 }, "BladeDrop" },
		{ 36, 15, 1, 30, { 8, 8 }, { 4, 2 }, { 0, 16, 48, 64 }, "Blade_L1" },
		{ 36, 6, 3, 18, { 10, 10 }, { 4, 2 }, { 160, 184, 48, 72 }, "Blade_L2" },
		{ 36, 1, 100, 30, { 6, 6 }, { 4, 4 }, { 272, 296, 0, 24 }, "Blade_L3" },
		{ 40, 0, 10, 30, { 2, 2 }, { 2, 2 }, { 120, 136, 96, 112 }, "Super Missle Launcher_L1" },
		{ 40, 0, 10, 40, { 4, 4 }, { 4, 4 }, { 184, 200, 96, 112 }, "Super Missle Launcher_L2" },
		{ 40, 0, 10, 40, { 4, 4 }, { 0, 0 }, { 184, 200, 96, 112 }, "Super Missle Launcher_L3" },
		{ 20, 2, 100, 100, { 12, 12 }, { 0, 0 }, { 0, 16, 0, 16 }, "_L1" },
		{ 20, 2, 100, 100, { 12, 12 }, { 0, 0 }, { 0, 16, 0, 16 }, "_L2" },
		{ 36, 2, 100, 100, { 12, 12 }, { 0, 0 }, { 0, 16, 0, 16 }, "_L3" },
		{ 32, 4, 4, 20, { 4, 4 }, { 3, 3 }, { 0, 32, 128, 144 }, "Nemesis_L1" },
		{ 32, 4, 2, 20, { 2, 2 }, { 2, 2 }, { 0, 32, 128, 144 }, "Nemesis_L2" },
		{ 32, 1, 1, 20, { 2, 2 }, { 2, 2 }, { 64, 96, 112, 128 }, "Nemesis_L3" },
		{ 64, 4, 4, 30, { 6, 6 }, { 3, 3 }, { 128, 144, 32, 48 }, "Spur_L1" },
		{ 64, 8, 8, 30, { 6, 6 }, { 3, 3 }, { 160, 176, 32, 48 }, "Spur_L2" },
		{ 64, 12, 12, 30, { 6, 6 }, { 3, 3 }, { 128, 144, 48, 64 }, "Spur_L3" },
		{ 32, 3, 100, 30, { 6, 6 }, { 3, 3 }, { 192, 200, 32, 40 }, "SpurTail_L1" },
		{ 32, 6, 100, 30, { 6, 6 }, { 3, 3 }, { 216, 224, 32, 40 }, "SpurTail_L2" },
		{ 0, 11, 100, 30, { 6, 6 }, { 3, 3 }, { 240, 248, 32, 40 }, "SpurTail_L3" },
		{ 32, 4, 4, 20, { 4, 4 }, { 3, 3 }, { 0, 8, 0, 8 }, "" },
		{ 4, 0, 4, 4, { 0, 0 }, { 0, 0 }, { 0, 0, 0, 0 }, "" },
		{ 36, 1, 1, 1, { 1, 1 }, { 1, 1 }, { 0, 2, 0, 2 }, "WHIMSTAR" },
		{ 4, 4, 4, 20, { 4, 4 }, { 3, 3 }, { 0, 0, 0, 0 }, "" },
		{ 4, 0, 4, 4, { 0, 0 }, { 0, 0 }, { 0, 0, 0, 0 }, "" },
		{ 4, 1, 1, 1, { 1, 1 }, { 1, 1 }, { 0, 0, 0, 0 }, "" }
	};

	return (iIndex < 0 || iIndex >= (sizeof(gpzDefaultBullets) / sizeof(PXMOD_BULLET_STRUCT)) ? NULL : (const PXMOD_BULLET_STRUCT*)&gpzDefaultBullets[iIndex]);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ModConfigResource::DefaultWeapons()
{
	// Remove all weapons
	while (mWeapons.count) RemoveWeapon(mWeapons.list[0].name);
	while (mBullets.count) RemoveBullet(mBullets.list[0].name);
	while (mNpcs.count)	   RemoveNPC(mNpcs.list[0].name);

	// Copy default NPCs
	for (int i = 0; ; i++)
	{
		const PXMOD_NPC_STRUCT* pNpc = GetDefaultNpc(i);

		// Stop looping once we reach out of bounds
		if (!pNpc)
			break;

		int iIndex = AddNPC(pNpc->name);
		memcpy(&mNpcs.list[iIndex], pNpc, sizeof(PXMOD_NPC_STRUCT));
		mNpcs.list[iIndex].acts.count          = 0;
		mNpcs.list[iIndex].acts.highest_act_no = 0;
		mNpcs.list[iIndex].acts.list           = NULL;
	}

	// Copy default bullets
	for (int i = 0; ; i++)
	{
		const PXMOD_BULLET_STRUCT* pBullet = GetDefaultBullet(i);

		// Stop looping once we reach out of bounds
		if (!pBullet)
			break;

		int iIndex = AddBullet(pBullet->name);
		memcpy(&mBullets.list[i], pBullet, sizeof(PXMOD_BULLET_STRUCT));
	}
	
	// Copy default weapons
	for (int i = 0; ; i++)
	{
		const PXMOD_WEAPON_STRUCT* pWeapon = GetDefaultWeapon(i);

		// Stop looping once we reach out of bounds
		if (!pWeapon)
			break;

		int iIndex = AddWeapon(pWeapon->name);
		memcpy(&mWeapons.list[i], pWeapon, sizeof(PXMOD_WEAPON_STRUCT));
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _CAVESTORY_MULTIPLAYER
#include <CSMAPI_endcode.h>
#endif