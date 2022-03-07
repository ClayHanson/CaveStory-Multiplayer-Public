#include <CavestoryModAPI.h>

#include "../../BitStream.h"
#include "GameDemo.h"
#include "GameDemoEvents.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define SIZE_BOOL														1
#define SIZE_UCHAR														(sizeof(unsigned char) << 3)
#define SIZE_USHORT														(sizeof(unsigned short) << 3)
#define SIZE_UINT														(sizeof(unsigned int) << 3)
#define SIZE_ULONGLONG													(sizeof(unsigned long long) << 3)
#ifdef _DEBUG
#define PLAYERSTATE_FIELD(FILESIZE, VARNAME)							{ #VARNAME, FILESIZE, sizeof(PXDEMO_PLAYERSTATE_STRUCT::##VARNAME), PXDEMO_PLAYERSTATE_CHANGE::PXDPSC_##VARNAME, offsetof(struct PXDEMO_PLAYERSTATE_STRUCT, VARNAME), [](PXDEMO_PLAYERSTATE_STRUCT* pData, void* pOut)->void* { memcpy(pOut, &pData->##VARNAME, sizeof(pData->##VARNAME)); return pOut; }, [](PXDEMO_PLAYERSTATE_STRUCT* pData, void* pIn)->void* { memcpy(&pData->##VARNAME, pIn, sizeof(pData->##VARNAME)); return pIn; } },
#define PLAYERSTATE_MASKF(FILESIZE, MASK, VARNAME)						{ #VARNAME, FILESIZE, sizeof(PXDEMO_PLAYERSTATE_STRUCT::##VARNAME), PXDEMO_PLAYERSTATE_CHANGE::PXDPSC_##MASK, offsetof(struct PXDEMO_PLAYERSTATE_STRUCT, VARNAME), [](PXDEMO_PLAYERSTATE_STRUCT* pData, void* pOut)->void* { memcpy(pOut, &pData->##VARNAME, sizeof(pData->##VARNAME)); return pOut; }, [](PXDEMO_PLAYERSTATE_STRUCT* pData, void* pIn)->void* { memcpy(&pData->##VARNAME, pIn, sizeof(pData->##VARNAME)); return pIn; } },
#else
#define PLAYERSTATE_FIELD(FILESIZE, VARNAME)							{ FILESIZE, sizeof(PXDEMO_PLAYERSTATE_STRUCT::##VARNAME), PXDEMO_PLAYERSTATE_CHANGE::PXDPSC_##VARNAME, offsetof(struct PXDEMO_PLAYERSTATE_STRUCT, VARNAME), [](PXDEMO_PLAYERSTATE_STRUCT* pData, void* pOut)->void* { memcpy(pOut, &pData->##VARNAME, sizeof(pData->##VARNAME)); return pOut; }, [](PXDEMO_PLAYERSTATE_STRUCT* pData, void* pIn)->void* { memcpy(&pData->##VARNAME, pIn, sizeof(pData->##VARNAME)); return pIn; } },
#define PLAYERSTATE_MASKF(FILESIZE, MASK, VARNAME)						{ FILESIZE, sizeof(PXDEMO_PLAYERSTATE_STRUCT::##VARNAME), PXDEMO_PLAYERSTATE_CHANGE::PXDPSC_##MASK, offsetof(struct PXDEMO_PLAYERSTATE_STRUCT, VARNAME), [](PXDEMO_PLAYERSTATE_STRUCT* pData, void* pOut)->void* { memcpy(pOut, &pData->##VARNAME, sizeof(pData->##VARNAME)); return pOut; }, [](PXDEMO_PLAYERSTATE_STRUCT* pData, void* pIn)->void* { memcpy(&pData->##VARNAME, pIn, sizeof(pData->##VARNAME)); return pIn; } },
#endif

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DEMO_PLAYERSTATE_FIELD gDemoPlayerstateFieldList[PXDPSC_COUNT] =
{
	PLAYERSTATE_FIELD( SIZE_UCHAR,					cond)
	PLAYERSTATE_FIELD( SIZE_UINT,					x)
	PLAYERSTATE_FIELD( SIZE_UINT,					y)
	PLAYERSTATE_FIELD( SIZE_UINT,					xm)
	PLAYERSTATE_FIELD( SIZE_UINT,					ym)
	PLAYERSTATE_FIELD( SIZE_USHORT,					max_life)
	PLAYERSTATE_FIELD( SIZE_USHORT,					life)
	PLAYERSTATE_FIELD( SIZE_UINT,					input_flags)
	PLAYERSTATE_FIELD( SIZE_UINT,					input_flags_trg)
	PLAYERSTATE_FIELD( SIZE_USHORT,					weapon_code)
	PLAYERSTATE_FIELD( SIZE_USHORT,					weapon_ammo)
	PLAYERSTATE_FIELD( SIZE_USHORT,					weapon_max_ammo)
	PLAYERSTATE_FIELD( SIZE_UCHAR,					weapon_level)
	PLAYERSTATE_FIELD( SIZE_USHORT,					weapon_exp)
	PLAYERSTATE_FIELD( SIZE_USHORT,					selectedArms)
	PLAYERSTATE_FIELD( SIZE_UCHAR,					direct)
	PLAYERSTATE_FIELD( SIZE_USHORT,					ani_no)
	PLAYERSTATE_FIELD( SIZE_USHORT,					ani_wait)
	PLAYERSTATE_FIELD( SIZE_UINT,					equip)
	PLAYERSTATE_FIELD( SIZE_USHORT,					star)
	PLAYERSTATE_FIELD( SIZE_UCHAR,					shock)
	PLAYERSTATE_FIELD( SIZE_UINT,					netanim)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_normal, physics_normal.max_dash)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_normal, physics_normal.max_move)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_normal, physics_normal.gravity1)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_normal, physics_normal.gravity2)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_normal, physics_normal.dash1)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_normal, physics_normal.dash2)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_normal, physics_normal.resist)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_normal, physics_normal.jump)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_underwater, physics_underwater.max_dash)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_underwater, physics_underwater.max_move)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_underwater, physics_underwater.gravity1)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_underwater, physics_underwater.gravity2)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_underwater, physics_underwater.dash1)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_underwater, physics_underwater.dash2)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_underwater, physics_underwater.resist)
	PLAYERSTATE_MASKF( SIZE_UINT,					physics_underwater, physics_underwater.jump)
	PLAYERSTATE_FIELD( SIZE_BOOL,					ignore_water)
	PLAYERSTATE_FIELD( SIZE_UCHAR,					boost_sw)
	PLAYERSTATE_FIELD( SIZE_UINT,					boost_cnt)
	PLAYERSTATE_FIELD( SIZE_UINT,					flag)
	PLAYERSTATE_MASKF( SIZE_BOOL,					updown, up)
	PLAYERSTATE_MASKF( SIZE_BOOL,					updown, down)
	PLAYERSTATE_MASKF( SIZE_UINT,					counts, count1)
	PLAYERSTATE_MASKF( SIZE_UINT,					counts, count2)
	PLAYERSTATE_MASKF( SIZE_UINT,					counts, count3)
	PLAYERSTATE_MASKF( SIZE_UINT,					counts, count4)
	PLAYERSTATE_FIELD( SIZE_UINT,					act_no)
	PLAYERSTATE_FIELD( SIZE_UINT,					act_wait)
};

int GetDemoPlayerstateFieldCount()
{
	return (sizeof(gDemoPlayerstateFieldList) / sizeof(DEMO_PLAYERSTATE_FIELD));
}