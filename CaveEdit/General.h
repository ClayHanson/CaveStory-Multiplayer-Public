//File created by David Shafer (C) 2006
#ifndef CAVE_STORY_GENERAL_H
#define CAVE_STORY_GENERAL_H
//list of exe offsets

#include "Assembly.h"
#include <vector>
#include "ModConfigResource.h"

const char* avar(const char* _Format, ...);

#ifndef LIST_INSERT
#define LIST_INSERT(LISTV, LISTC, LISTTYPE, INDEX)		memmove(&LISTV[INDEX + 1], &LISTV[INDEX], (LISTC - INDEX - 1) * sizeof(LISTTYPE))
#endif
#ifndef LIST_ERASE
#define LIST_ERASE(LISTV, LISTC, LISTTYPE, INDEX)		memmove(&LISTV[INDEX], &LISTV[INDEX + 1], (LISTC - INDEX - 1) * sizeof(LISTTYPE))
#endif

//used to convert RGBQUAD to COLORREF
#define RGBQ(x)  ((COLORREF)(((BYTE)(x.rgbRed)|((WORD)((BYTE)(x.rgbGreen))<<8))|(((DWORD)(BYTE)(x.rgbBlue))<<16)))


extern CString CS_TILE_TYPE_NAME;

class CaveMapINFO;
class CaveStoryMOD;

//exe offsets
extern int CAVE_MAP_INFO;

extern CaveStoryMOD exe;

//extern CaveMapINFO mapInfo[255];

//global variables (yay!)
extern CString filePath;
extern CString progPath;
extern CString EXEname;

//semi constants 
extern int CS_DEFAULT_SCALE;
extern char* CS_DEFAULT_PBM_EXT;
extern bool CS_DEFAULT_TILESET_WINDOW;
extern int CS_DEFAULT_TILESET_SCALE;
extern bool CS_DEFAULT_WHEEL_ZOOM;
extern BYTE CS_DEFAULT_TILETYPE_ALPHA;
extern bool CS_DEFUALT_READ_ONLY;
extern bool CS_DEFAULT_COMPILE_ON_SAVE;
extern bool CS_DEFAULT_USE_RECT_XYWH;
extern bool CS_DEFAULT_PLAY_SOUND;
extern char CS_DEFAULT_MOD_PATH[260];
extern int CS_GRAPHIC_COLOR_DEPTH;
extern bool CS_DEFAULT_ME_FOCUS_ON_MAP_LOAD;
extern bool CS_DEFAULT_ME_MAXIMIZE_ON_INIT;
extern bool CS_DEFAULT_ME_SYNC_ACROSS_TABS;

void SaveConfig_CaveEditor();
void LoadConfig_CaveEditor();

//string offset info
#define CS_ARMSITEM_TSC_STRING              0
#define CS_PBM_FOLDER_STRING                1
#define CS_DOUKUTSU_TIMESTAMP_STRING        2
#define CS_CONFIG_DAT_STRING                3
#define CS_VERSION_FORMAT_STRING            4
#define CS_000_DAT_STRING                   5
#define CS_PBM_FOLDER_STRING2               6
#define CS_SURFACE_STRING                   7
#define CS_PBM_FOLDER_STRING3               8
#define CS_DEBUG_OUT_STRING                 9
#define CS_CREDIT_STRING                   10
#define CS_CREDIT_TSC_STRING               11
#define CS_CASTS_STRING                    12
#define CS_NPC_TBL_STRING                  13
#define CS_XXX_STRING                      14
#define CS_C_PIXEL_STRING                  15
#define CS_PIXEL_STRING                    16
#define CS_MYCHAR_STRING                   17
#define CS_TITLE_STRING                    18
#define CS_ARMSIMAGE_STRING                19
#define CS_ARMS_STRING                     20
#define CS_ITEMIMAGE_STRING                21
#define CS_STAGEIMAGE_STRING               22
#define CS_NPC_NPCSYM_STRING               23
#define CS_NPC_NPCREGU_STRING              24
#define CS_TEXTBOX_STRING                  25
#define CS_CARET_STRING                    26
#define CS_BULLET_STRING                   27
#define CS_FACE_STRING                     28
#define CS_FADE_STRING                     29
#define CS_CREDIT01_STRING                 30
#define CS_PIXTONE_STRING                  31
#define CS_DOUKUTSU_STRING                 32
#define CS_DATA_STRING                     33
#define CS_0_STRING                        34
#define CS_PXM_STRING                      35
#define CS_PROFILE_DAT_STRING              36
#define CS_STAGESELECT_TSC_STRING          37
#define CS_STAGE_STRING                    38
#define CS_PRT_FOLDER_STRING               39
#define CS_PXA_FOLDER_STRING               40
#define CS_PXM_FOLDER_STRING               41
#define CS_PXE_FOLDER_STRING               42
#define CS_TSC_FOLDER_STRING               43
#define CS_NPC_STRING                      44
#define CS_NPC_FOLDER_STRING               45
#define CS_NPC_FOLDER2_STRING              46
#define CS_XXXX_STRING                     47
// ORG name offsets here
#define CS_HEAD_TSC_STRING                 48
#define CS_WINDOW_STRING                   49
#define CS_LOADING_STRING                  50

#define CS_EXE_STRING_COUNT                51

//ORGs
#define CS_ORG_WANPAKU_STRING               0
#define CS_ORG_ANZEN_STRING                 1
#define CS_ORG_GAMEOVER_STRING              2
#define CS_ORG_GAVITY_STRING                3
#define CS_ORG_WEED_STRING                  4
#define CS_ORG_MDOWN2_STRING                5
#define CS_ORG_FIREEYE_STRING               6
#define CS_ORG_VIVI_STRING                  7
#define CS_ORG_MURA_STRING                  8
#define CS_ORG_FANFALE1_STRING              9
#define CS_ORG_GINSUKE_STRING              10
#define CS_ORG_CEMETERY_STRING             11
#define CS_ORG_PLANT_STRING                12
#define CS_ORG_KODOU_STRING                13
#define CS_ORG_FANFALE3_STRING             14
#define CS_ORG_FANFALE2_STRING             15
#define CS_ORG_DR_STRING                   16
#define CS_ORG_ESCAPE_STRING               17
#define CS_ORG_JENKA_STRING                18
#define CS_ORG_MAZE_STRING                 19
#define CS_ORG_ACCESS_STRING               20
#define CS_ORG_IRONH_STRING                21
#define CS_ORG_GRAND_STRING                22
#define CS_ORG_CURLY_STRING                23
#define CS_ORG_OSIDE_STRING                24
#define CS_ORG_REQUIEM_STRING              25
#define CS_ORG_WANPAK2_STRING              26
#define CS_ORG_QUIET_STRING                27
#define CS_ORG_LASTCAVE_STRING             28
#define CS_ORG_BALCONY_STRING              29
#define CS_ORG_LASTBTL_STRING              30
#define CS_ORG_LASTBT3_STRING              31
#define CS_ORG_ENDING_STRING               32
#define CS_ORG_ZONBIE_STRING               33
#define CS_ORG_BDOWN_STRING                34
#define CS_ORG_HELL_STRING                 35
#define CS_ORG_JENKA2_STRING               36
#define CS_ORG_MARINE_STRING               37
#define CS_ORG_BALLOS_STRING               38
#define CS_ORG_TOROKO_STRING               39
#define CS_ORG_WHITE_STRING                40

#define CS_EXE_ORG_STRING_COUNT            41


#define DECODE_MAX     1500

enum TSC_argType : char { tat_none, tat_arms, tat_ammo, tat_direction, tat_event, tat_face, tat_flag, tat_graphic, tat_illustration, tat_item, tat_map, tat_music, tat_npc_specific, tat_npc_type, tat_sound, tat_tile, tat_x_coord, tat_y_coord, tat_number, tat_ticks, tat_count };
static const char* TSC_argTypeIdents = "-aAdefFglimuNnstxy#.";
static const char* TSC_argTypeNames[] = { "", "Arms", "Ammo", "Direction", "Event", "Face", "Flag", "Graphic", "Illustration", "Item", "Map", "Music", "NPC", "NPC Type", "Sound", "Tile", "X Pos", "Y Pos", "Number", "Ticks" };

struct TSC_ArgType_List {
	char list[100][128];
	int count;
};

extern TSC_ArgType_List TSC_argTypeLists[];

class EntityINFO //24 bytes
{
public:
	unsigned long long int nodes;

	unsigned long long int transmit_nodes;

	unsigned int flags;  //32 bits of flags (0x00000000)

	unsigned short hp;     //npc hp (0x02D2)

	unsigned int tileSet; //tile set index (0x05A4) (Shining Phantom)

	unsigned char hurtSound; //hurt Sound (0x070D)
	unsigned char deathSound;  //death sound

	unsigned char deathPBM; //death graphic (0x09DF)

	unsigned int weaponDrop;//weapon powerup dropped on death (0x0B48)

	unsigned int attack;   //attack power (0x10EC)

	//hitbox position coords measured from center of tile (0x1690)
	char hitLeft;  //back (left)
	char hitTop;    //up
	char hitRight; //front (right)
	char hitBottom;  //down

	//(0x1C34)
	char hitX;  //hit box shift right from left edge tile (does not shift sprite)
	char hitY;  //hit box shift up from bottom of tile (does not shift sprite)
	char cenX;  //maybe horizontal center offset from left of tile?
	char cenY;  //maybe vertical center offset from bottom of tile?

};

#define NPC_TYPE_COUNT	  500 //total number of npc types in npc.tbl

//NPC table flags
#define NPC_SOLID              0x0001 //Entity is solid (ex. Rhino)
#define NPC_NO_TILE44          0x0002 //Entity ignores tile type 44
#define NPC_INVINCIBLE         0x0004 //entity is invincible (ex. press)
#define NPC_NO_SOLID           0x0008 //Entity ignores solid tiles (ex. fliers)
#define NPC_BOUNCY_TOP         0x0010 //Entity's top is bouncy (ex. Rhino)
#define NPC_SHOOTABLE          0x0020 //Entity can be shot (ex. Rhino)
#define NPC_SPECIAL_SOLID      0x0040 //???
#define NPC_ONLY_FRONT_ATTACK  0x0080 //Only entity's front hurts (ex. Rhino)
#define NPC_PLAYER_FRIENDLY    0x0100 //Entity doesn't hurt player (ex. Sue)
#define NPC_RESERVE9           0x0200 //
#define NPC_RESERVEA           0x0400 //
#define NPC_RESERVEB           0x0800 //
#define NPC_RESERVEC           0x1000 //
#define NPC_NO_SHOOT           0x2000 //Entity can't be shot (ex. Sue)
#define NPC_RESERVEE           0x4000 //
#define NPC_SHOW_DAMAGE        0x8000 //Shows damage numbers on Entity (ex. Rhino)

//data structure to mimic data layout in .exe file
class CaveMapINFO
{
public:
	char tileset[32];
	char fileName[32];//name of map
	char backType;
	char unknown[3];//probably part of the backType value
	char background[32];
	char NPCset1[32];
	char NPCset2[32];
	char bossNum;
	char caption[35];//caption displayed when entering map
	int area;
	int map_index;
	bool focus_center_x;
	bool focus_center_y;

public:
	CaveMapINFO& operator=(CaveMapINFO& info);

	CaveMapINFO() {}
	CaveMapINFO(const PXMOD_STAGE_STRUCT& stage, ModConfigResource* pRes);

	void SetModStage(PXMOD_STAGE_STRUCT* pStage, ModConfigResource* pMod);
};


//a horrible bitmap class written for Cave Story, does all the basics
class CaveBitMap: public CImage
{
public:
	CaveBitMap();
	~CaveBitMap();

	bool Load(const char* file);//loads BitMap from File
	bool Save(const char* newName);//saves BitMap to specified file
	bool Save();//saves BitMap to file it was loaded from

	void Resize(int w, int h);//resizes bitmap filling new areas with palette entry 0

	void DrawPart(int x, int y, int w, int h, CDC* cdc, int xCDC, int yCDC, bool trans = true, int color = 0x000);//draws Part of Bitmap to Device Context
	void DrawPart(RECT rectSrc, CDC* cdc, RECT rectDest, bool trans = true, int color = 0x000);//draws Part of Bitmap to Device Context

	char file[1024];//file loaded from

	static char extra8[8];//this is the extra characters that need to be added to any image file
};

//stores the data for one tileset along with some useful functions
class CaveTileSet
{
public:
	CaveTileSet();
	~CaveTileSet();

	bool load(const char* tileSetName);//loads tileset

	bool save();//saves tileset
	bool save(const char* newName);//saves tileset with new name

	void displayTile(int index, int x, int y, CDC* cdc, bool trans = true);//displays tile to specified coordinates on device context
	void displayTileType(int index, int x, int y, CDC* cdc, bool trans = true);//displays tile type based on tile.bmp

	void resize(int size);//resizes tile set to hold at least size number of tiles, return actual new size

	static CaveBitMap typeMap;//the tile type bitmap
	static CaveBitMap netGrid;//the tile type bitmap
	static int count;//number of tileSet objects

	CaveBitMap tileData; //stores the bitmap data for the tiles

	int tileNum; //number of tiles in set
	unsigned char* typData; //stores the tile type data for each tile

	char tileName[1024]; //name of tileset 

};

//data structure for holding entity data
//all items are in there correct place to load whole structure from .pxe file
class Entity
{
public:
	unsigned short x;     //x coord
//	char __0x01;	  //filler (just in case it's not part of x coord)
	unsigned short y;     //y coord
//	char __0x03;	  //filler (just in case it's not part of y coord)
	unsigned short flagID;//event ID
	unsigned short event; //event index into *.tsc file
	unsigned short npc;   //NPC type
	unsigned int flags; //various flags
};

//this holds map zone rect information
class CaveMapZone
{
public:
	int id;
	int x;
	int y;
	int w;
	int h;
};

//this holds map zone information that won't be compiled
class CaveMapZoneDev
{
public:
	char name[256];
	int id;
};

class CaveMapEntProperty
{
public:
	char name[256];
	char value[256];
	int ent_idx;
};

//this keeps all (well most) data on the tile types
class TileKey
{
public:
	TileKey();

	bool sPC[256];   //solid to player
	bool sNPC[256];  //solid to non player
	bool water[256]; //is water?
	bool front[256]; //is foreground graphic
	bool seen[256];  //graphic appears in-game
};

//holds a Cave story data set (map and entities)
class CaveMap
{
public:
	CaveMap();
	~CaveMap();

	bool reload();//reloads background and tileset (used when either one changes)
	static void getErrorString(int iError, char* pBufferOut, int iBufferLen);
	int load(CaveMapINFO* info);//loads map given path to .exe and a filled out CaveMapINFO object
	bool create(CaveMapINFO* info, int width, int height);//creates map given path to .exe and a filled out CaveMapINFO object

	bool save(bool compile_map = true);//saves map
	bool save(const char* mapName, bool compile = true);//saves map with new name

	void resize(int w, int h);//resizes the map to the new width and height, buffering with tile 0 where needed
	void shift(int dx, int dy, bool wrap = true);//shifts map by dx and dy, filling edges with tile 0 if wrap is turned off

	void displayTile(CDC* cdc, int x, int y, int tx, int ty, double scale = 1);

	void displayMap(CDC* cdc, int x = 0, int y = 0);//displays map to device context
	void displayMap(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY);

	void displayBackMap(CDC* cdc);//displays background map tiles to device context
	void displayBackMap(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY);
	void displayFrontMap(CDC* cdc);//displays foreground map tiles to device context
	void displayFrontMap(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY);

	void displayTileTypes(CDC* cdc);//displays map tiles types to device context
	void displayTileTypes(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY);

	void displayNetGroupTiles(CDC* cdc);//displays map tiles types to device context
	void displayNetGroupTiles(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY);

	void displayBackground(CDC* cdc);//displays background image for map

	int addEntity();//all addEntity functions return index of added entity or -1 on failure
	int addEntity(Entity entity);
	int addEntity(Entity entity, int index);
	int addEntity(WORD x, WORD y, WORD npcType = 0, WORD event = 0, WORD flagID = 0, WORD flags = 0);

	int copyEntity(int index);//all copyEntity functions return index of added entity or -1 on failure
	int copyEntity(int index, int x, int y);

	int deleteEntity(int index);//returns number of entities after delete

	char setTile(int x, int y, char newTile);//set tile to newTile and returns old tile value
	char getTile(int x, int y);//returns tile value

	void setNetGroupTile(int x, int y, char type);//set netgroup tile to type and returns old nettile
	char getNetGroupTile(int x, int y);//returns netgroup tile value

	// Returns the newly created zone's ID
	int addMapZone(const char* name);
	bool removeMapZone(const char* name);
	bool removeMapZone(int zone_id);
	const char* getMapZoneName(int zone_id);

	bool isSolid(int x, int y);//check if the tile at the given coordinates is solid

	bool buildNetGroups(std::vector<CaveMapZone>& out, int& highest_group_id);

	CaveBitMap background;//background image of map
	CaveBitMap NPCset1;//background image of map
	CaveBitMap NPCset2;//background image of map
	std::vector<CaveMapZoneDev> zones;
	std::vector<CaveMapEntProperty> properties;

	CString oldFileName; //used to delete old files when name is changed

	CaveMapINFO mapInfo; //general map info
	int width;//width of map (in tiles)
	int height;//height of map (in tiles)

	CaveTileSet tileSet;//tileset data
	unsigned char* tiles;//list of all tiles in map
	unsigned char* netgroup_tiles;//list of all netgroup tiles in map

	int entityNum;//number of entities
	Entity* entity;//list of all entities
};

//stealing Sue's workshop data (sorry) but modified somwhat
class NPC_extras
{
public:
	NPC_extras& operator=(NPC_extras& assign);
	NPC_extras();
	bool loaded;
	int ID;
	CString shortDesc1;
	CString shortDesc2;
	CString type;
	CString option1;
	CString option2;
	CString option3;
	CString desc;
	CString notes;
};

//this class stores and allows for easy access to NPC sprites and stats
class NPCtable
{
public:
	long NPC_COUNT;
	static NPCtable* us;

public:
	NPCtable();
	~NPCtable();

	bool reload_current_mod_npc_table();//reloads NPC table
	bool load(const char* npcName = "npc.tbl");//loads NPC table

	bool save();//saves NPC table
	bool save(const char* newName);//saves to new name

	void displaySprite(int x, int y, int index, double scale, CDC* cdc);//displays NPC sprite based on upper left corner of tile
	void displaySprite(int x, int y, int index, double scale, CDC* cdc, CaveBitMap& tileset, CaveBitMap& NPCset1, CaveBitMap& NPCset2);//displays NPC sprite based on upper left corner of tile

	CString fileName;

	EntityINFO entity[NPC_TYPE_COUNT];//list of entity data

	int extraCount;
	NPC_extras extra[NPC_TYPE_COUNT * 3];

//	void resize(int newSize);//will one day hopefully resize NPC table allowing for tons of new sprites to be added if not behaviors too

};

struct TSC_struct
{
	char command[5];//command
	int args;//number of arguments
	TSC_argType types[32];//Arg types
	char name[32];//"Real" command name
	char desc[256];//Description of command
	unsigned int inspectDlgIdx;
	unsigned int command_type;
};


class TSC_Info
{
public:
	static TSC_Info* first;
	TSC_Info* next;
	TSC_Info* prev;

public:
	TSC_Info();
	~TSC_Info();

	TSC_struct cmd[512];

	int count;
	void LoadAllCommands();
	int Find(const char* command);//returns offset of matching command
	int Find(CString command);//returns offset of matching command
};

struct ShotData //44 bytes
{
	char damage;//0x00
	unsigned char penPower;//0x01
	unsigned char byte02;//0x02
	unsigned char byte03;//0x03
	int range;//0x04 - 0x07
	union
	{
		char flag;//0x08
		struct
		{
			char flag01:1;
			char flag02:1;
			char flag04:1;
			char flag08:1;
			char flag10:1;
			char flag20:1;
			char flag40:1;
			char flag80:1;
		};
	};
	char byte09;//0x09
	char byte0A;//0x0A
	char byte0B;//0x0B
	int width;//width of bullet for entity collisions
	int height;//height of bullet for entity collisions
	int wallWidth;//width used for wall collisions
	int wallHeight;//height used for wall collisions
	int leftGraphicOffset;//graphic position from back of Player, used when shooting left/up
	int vertGraphicOffset;//graphic position from player's feet when shooting left/right
	                      //position from player's right side (+left -right) when shooting up or down
	int rightGraphicOffset;//graphic position from front of Player, used when shooting right
	int unused;//no use?
};

#define CS_WEP_FLAG_01 0x01
#define CS_WEP_FLAG_02 0x02
#define CS_WEP_FLAG_IGNORE_SOLID     0x04
#define CS_WEP_FLAG_NO_SOLID_DESTROY 0x08
#define CS_WEP_FLAG_10 0x10
#define CS_WEP_FLAG_BREAK_BLOCK     0x20
#define CS_WEP_FLAG_PIERCE_BLOCKS   0x40
#define CS_WEP_FLAG_80 0x80

//The motherload class that does EVERYTHING (not really)
class CaveStoryMOD
{
public:
	CaveStoryMOD();
	~CaveStoryMOD();

	bool Open(const char* file = "MOD.CFG");//opens a file
	void Clear();//clears file from memory (please save first)

	bool Save(bool bRelease = false);//saves file
	bool CompileMap(int index, bool update_progress = false);
	bool CompileMaps(int start = 0, int count = -1);

	bool AddMap(const char* name, int width = 2, int height = 2, int index = -1);
	bool AddMap(CaveMapINFO* info, int width, int height, int index = -1);
	bool CopyMap(int index, const char* name);
	bool DeleteMap(int index);

	ModConfigResource mod;

	//Metadata
	//CString fileName;

	//CaveMapINFO* mapInfo;
	//int mapCount;

	//ShotData* shotInfo;

	//these are the constants related to title screen and game start

	CaveBitMap spriteSheet[32];
};

BOOL CreateToolTipForControl(CWnd* pDlg, const char* pszText);
void GENERAL_EventTextEdit_GainFocus(CWnd* pControl);
void GENERAL_EventTextEdit_LoseFocus(CWnd* pControl);
short GENERAL_EventTextEdit_GetValue(CWnd* pControl);

#endif CAVE_STORY_GENERAL_H
