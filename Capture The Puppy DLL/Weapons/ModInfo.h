#include "../framework.h"

#ifndef DRECT
#define DRECT(X, Y, W, H) { X, Y, X + W, Y + H }
#endif

static inline float LerpInt(float a, float b, float f)
{
	return a + f * (b - a);
}

static inline float dist(int x1, int y1, int x2, int y2)
{
	// Calculating distance 
	return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2) * 1.f);
}

#define INVALID_WEAPON_RECT { 302, 64,  320, 70  }

static RECT grcGame = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

static const struct PWP_WEAPON
{
	int weapon_id;			///< The actual weapon ID.
	int min_rand_level;		///< The minimum level this weapon can randomly have when picking it up.
	int max_rand_level;		///< The maximum level this weapon can randomly have when picking it up.
	int max_ammo;			///< The maximum amount of ammo this weapon can hold.
	int add_ammo;			///< The amount of ammo to add if the player already has this weapon. If they DON'T have this weapon, then they get a full clip.
	RECT rect;				///< The source rect for this weapon.
} pWeaponCodeTable[] =
{
	/* [0000] Nothing                */ { 0,  1, 1, 0,   0,   INVALID_WEAPON_RECT    },
	/* [0001] Snake                  */ { 1,  1, 3, 10,  5,   { 302, 76,  320, 82  } },
	/* [0002] Polar Star             */ { 2,  3, 3, 0,   0,   { 302, 100, 320, 106 } },
	/* [0003] Fire Ball              */ { 3,  1, 3, 12,  4,   { 302, 94,  320, 100 } },
	/* [0004] Machine Gun            */ { 4,  1, 2, 100, 45,  { 302, 88,  320, 94  } },
	/* [0005] Missile Launcher       */ { 5,  1, 2, 4,   2,   { 302, 70,  320, 76  } },
	/* [0006] ...                    */ { 6,  1, 1, 0,   0,   INVALID_WEAPON_RECT    },
	/* [0007] Bubblin                */ { 7,  2, 3, 50,  45,  { 302, 82,  320, 88  } },
	/* [0008] ...                    */ { 8,  1, 1, 0,   0,   INVALID_WEAPON_RECT    },
	/* [0009] Blade                  */ { 9,  1, 1, 0,   0,   { 272, 0,   288, 16  } },
	/* [0010] Super Missile Launcher */ { 10, 1, 2, 4,   2,   { 302, 106, 320, 112 } },
	/* [0011] ...                    */ { 11, 1, 1, 0,   0,   INVALID_WEAPON_RECT    },
	/* [0012] Nemesis                */ { 12, 1, 2, 8,   2,   { 302, 112, 320, 118 } },
	/* [0013] Spur                   */ { 13, 1, 1, 141, 100, { 302, 118, 320, 124 } },
	/* [0014] Agility                */ { 14, 1, 3, 0,   0,   { 302, 124, 320, 130 } },
};

enum CTP_TeamFlags
{
	CTP_FLAG_CANNOT_PICKUP_FLAG    = 0x0001,	//< Team members cannot pick up the flag.
	CTP_FLAG_CANNOT_PICKUP_WEAPONS = 0x0002,	//< Team members cannot pick up weapons.
	CTP_FLAG_TOUCH_FLAG_TO_RETURN  = 0x0004		//< Team members can touch their own flag to instantly return it if it's not already at their base.
};

enum PuppyMoveType
{
	PUPPY_MOVE_RUN = 0,
	PUPPY_MOVE_JUMP
};

enum NodeEditorMode
{
	MODE_CREATE_NODES = 0,
	MODE_DELETE_NODES,
	MODE_LINK_NODE_RUN,
	MODE_LINK_NODE_JUMP,
	MODE_UNLINK_NODES,
	MODE_CHANGE_TEAM_COND,
	MODE_SUMMON_PUPPY,
	MODE_SUMMON_BOT,
	MODE_EXPORT_NODES,

	MODE_COUNT
};

enum MapAudioFileShowType
{
	MAFST_ALL           = 0,
	MAFST_SPECIFIC_TEAM = 1
};

struct MAP_PUPPY_NODE_CONNECTION;

struct MAP_PUPPY_NODE
{
	int x;
	int y;

	unsigned short team_cond;

	int* conn_list;
	unsigned int conn_count;
};

struct MAP_PUPPY_NODE_MOVE_INFO
{
	PuppyMoveType move_type;
	int tgt_x;
	int tgt_y;
};

struct MAP_PUPPY_NODE_CONNECTION
{
	// How the puppy should get from ONE to TWO.
	PuppyMoveType move_type_to;

	// How the puppy should get from TWO to ONE.
	PuppyMoveType move_type_from;

	// First node index
	unsigned int one;

	// Second node index
	unsigned int two;

	// Whether this is mutual or not
	bool mutual;
};

struct MAP_COVER_IMAGE
{
	Surface_Ids id;
	unsigned int off_x;
	unsigned int off_y;
	RECT src_rect;
	bool show_for_team_members;
	bool background;
};

struct MAP_AUDIO_FILE
{
	CSM_CustomSoundId id;
	unsigned int x;
	unsigned int y;
	int radius;
	bool always_on_screen;
	MapAudioFileShowType show_type;
	int team_type;
};

struct MAP_TEAM_INFO
{
	CaveNet::DataStructures::NetTeam::TeamType team_type;
	CaveNet::DataStructures::NetTeam* team;

	int puppy_respawn_time;
	int player_respawn_time;

	MAP_COVER_IMAGE* cover_list;
	int cover_count;

	MAP_PUPPY_NODE* node_list;
	unsigned int node_count;

	MAP_PUPPY_NODE_CONNECTION* node_connection_list;
	unsigned int node_connection_count;

	bool* node_ignore_list;
	int node_ignore_size;

	/// CTP_TeamFlags
	unsigned int flags;
	int anim_timer;
};

struct MAP_COVER_IMAGE_CACHE
{
	char* file_name;
	Surface_Ids id;
};

extern bool g_NodeEditor_Enabled;
extern int g_NodeEditor_SelectedTeamIdx;
extern NodeEditorMode g_NodeEditor_Mode;
extern MAP_TEAM_INFO g_AI_NodeInfo;

extern MAP_TEAM_INFO* gTeamInfoList;
extern unsigned int gTeamInfoCount;
extern MAP_COVER_IMAGE_CACHE* gCacheSurfaceList;
extern unsigned int gCacheSurfaceCount;
extern MAP_COVER_IMAGE* gPublicCoverImageList;
extern unsigned int gPublicCoverImageCount;
extern MAP_AUDIO_FILE* gTeamAudioList;
extern unsigned int gTeamAudioCount;
extern CaveNet::DataStructures::NetTeam::TeamType gOurCurrentTeamType;
extern char g_CurrentStage_Name[260];

void CTP_DrawNodeEditor();
void ShootBullet_Snake(ShootInfo* sData, int level);
void ShootBullet_PoleStar(ShootInfo* sData, int level);
void ShootBullet_FireBall(ShootInfo* sData, int level);
void ShootBullet_MachineGun(ShootInfo* sData, int level);
void ShootBullet_NormalMissile(ShootInfo* sData, int level);
void ShootBullet_SuperMissile(ShootInfo* sData, int level);
void ShootBullet_Bubblin1(ShootInfo* sData, int level);
void ShootBullet_Sword(ShootInfo* sData, int level);
void ShootBullet_Nemesis(ShootInfo* sData, int level);
void ShootBullet_Spur(ShootInfo* sData, int level);
void ShootBullet_Agility(ShootInfo* sData, int level);


void ActBullet_Snake(BULLET* bul, int level);
void ActBullet_PoleStar(BULLET* bul, int level);
void ActBullet_FireBall(BULLET* bul, int level);
void ActBullet_MachineGun(BULLET* bul, int level);
void ActBullet_Missile(BULLET* bul, int level);
void ActBullet_Bom(BULLET* bul, int level);
void ActBullet_Bubblin1(BULLET* bul, int level);
void ActBullet_Bubblin2(BULLET* bul, int level);
void ActBullet_Bubblin3(BULLET* bul, int level);
void ActBullet_Spine(BULLET* bul, int level);
void ActBullet_Edge(BULLET* bul, int level);
void ActBullet_Drop(BULLET* bul, int level);
void ActBullet_Sword1(BULLET* bul, int level);
void ActBullet_Sword2(BULLET* bul, int level);
void ActBullet_Sword3(BULLET* bul, int level);
void ActBullet_SuperMissile(BULLET* bul, int level);
void ActBullet_SuperBom(BULLET* bul, int level);
void ActBullet_Nemesis(BULLET* bul, int level);
void ActBullet_Spur(BULLET* bul, int level);
void ActBullet_SpurTail(BULLET* bul, int level);
void ActBullet_EnemyClear(BULLET* bul, int level);
void ActBullet_Star(BULLET* bul, int level);

void ActNpc411(NPCHAR* npc);
void ActNpc412(NPCHAR* npc);
void ActNpc413(NPCHAR* npc);
void ActNpc414(NPCHAR* npc);
void ActNpc415(NPCHAR* npc);
void ActNpc416(NPCHAR* npc);
void ActNpc417(NPCHAR* npc);
void ActNpc418(NPCHAR* npc);

void CTP_OnDrawGameHud(BOOL bFlash);

void CTP_OnClientConnected(CaveNet::DataStructures::NetClient* pClient);
void CTP_OnClientDisconnected(CaveNet::DataStructures::NetClient* pClient);
void CTP_OnClientSpawn(CaveNet::DataStructures::NetClient* pClient);
void CTP_OnClientDie(CaveNet::DataStructures::NetClient* pClient, PlayerDeathType iDeathType, NPCHAR* pKillerNpc, CaveNet::DataStructures::NetClient* pKillerClient);

void CTP_OnTeamAddMember(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetClient* pServerClient, CaveNet::DataStructures::NetTeam* pTeam);
void CTP_OnTeamRemoveMember(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetClient* pServerClient, CaveNet::DataStructures::NetTeam* pTeam);

void CTP_OnHitMyCharNpChar(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc);
BOOL CTP_OnPlayerPickupExp(NPCHAR* pNpc, CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, int iAmount);
BOOL CTP_OnPlayerPickupHeart(NPCHAR* pNpc, CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, int iAmount);
BOOL CTP_OnPlayerPickupMissile(NPCHAR* pNpc, CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, int iAmount);

void CTP_FreeAllBots();
void CTP_CleanupCoverSurfaces();
void CTP_OnStageChange(const char* pStageName, int iAreaIndex);

void CTP_OnPostDraw();
void CTP_OnPreDrawHUD();
void CTP_OnDrawMapBack();

MAP_TEAM_INFO* CTP_GetTeamInfo(int iTeamType);
MAP_TEAM_INFO* CTP_GetTeamInfoByTeam(CaveNet::DataStructures::NetTeam* pTeam);
void CTP_ImportNodes(int iTeamIdx);

bool Puppy_PopulateMoveList(List<MAP_PUPPY_NODE_MOVE_INFO>* pOutList, int iStartX, int iStartY, int iDestX, int iDestY);
bool Puppy_GetNextNode(NPCHAR* npc, PuppyMoveType* pMoveType, int* tgt_x, int* tgt_y);
void Puppy_PutString(int x, int y, const char* str, int r = 255, int g = 255, int b = 255);
unsigned int Puppy_GetStringWidth(const char* str);
void Puppy_PutNumber(int x, int y, int value, bool draw_colon);

void CTP_PickupWeapon(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc);
void CTP_PickupPuppy(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc);
void CTP_HandleNoPuppyZone(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc);
void CTP_HandleKillZone(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc);
void CTP_HandleResupplyCabinet(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc);
void CTP_DropPuppy(NPCHAR* pNpc);

void CTP_ApplyCarryPuppyPhysics(CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC);
void CTP_RevokeCarryPuppyPhysics(CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC);

void CTP_OnProcessAI(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, CaveNet::DataStructures::NetTeam* pTeam);

void CTP_DrawZoneShimmer();

#define CTP__PLAYER_PHYSICS__NORMAL__NOT_CARRYING_PUPPY			0x5FF
#define CTP__PLAYER_PHYSICS__UNDERWATER__NOT_CARRYING_PUPPY		0x2FF
#define CTP__PLAYER_PHYSICS__NORMAL__CARRYING_PUPPY				0x3FF
#define CTP__PLAYER_PHYSICS__UNDERWATER__CARRYING_PUPPY			0x1FF
