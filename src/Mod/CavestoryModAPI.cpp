/**
 * @file CavestoryModAPI.cpp
 *
 * Implementing Cavestory Mod API.
 *
 */

extern int spur_charge;
extern bool spur_max;

#include <CavestoryModAPI.h>
#include "../Game/System/Platform/Platform.h"
#include "../CaveNet/CaveNet.h"
#include "../Sound.h"
#include "../Stage.h"
#include "../Bullet.h"
#include "../Caret.h"
#include "../NpChar.h"
#include "../ArmsItem.h"
#include "../Game.h"
#include "../KeyControl.h"
#include "../MyChar.h"
#include "../MycParam.h"
#include "../Config.h"
#include "../ConfigVar.h"
#include "../Triangle.h"
#include "../Main.h"
#include "../CrashMode.h"

bool bModCrashed = false;

#include <stdarg.h>
#include <CSMAPI_begincode.h>

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void CSM_Log(const char* pFormat, ...)
{
	va_list aptr;
	va_start(aptr, pFormat);
	vprintf(pFormat, aptr);
	va_end(aptr);
}

void CSM_Explode(int iErrorCode, const char* pErrorName, const char* pTitle, const char* pMessage, bool bGenerateCrashLog)
{
	if (bGenerateCrashLog)
	{
		bModCrashed = true;
		Platform::TryAndExcept([]()->void
		{
			throw;
		});
		bModCrashed = false;
	}

	CrashMode CM;

	CM.SetRetValue(2);
	CM.SetCrashInfo(iErrorCode, pErrorName, pTitle, pMessage, bGenerateCrashLog);

	int iResult = CM.DoMode();

	// If it results in anything other than 2, we need to exit
	if (iResult != 2 && BaseModeInstance::m_FocusedInstance)
		BaseModeInstance::m_FocusedInstance->EndMode(iResult);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

bool CSM_CaveNet_ConnectedAsClient()
{
	return CaveNet::Client::IsConnected();
}

bool CSM_CaveNet_IsHosting()
{
	return CaveNet::Server::IsHosting();
}

int CSM_CaveNet_GetClientCount()
{
	return CaveNet::Server::ServerClientGroup::GetClientCount();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

bool Config_GetBool(const char* pCVarName)
{
	ConfigVar* pCfg = ConfigVar::GetVariable(pCVarName);
	return (pCfg ? pCfg->ToBool() : false);
}

int Config_GetInt(const char* pCVarName)
{
	ConfigVar* pCfg = ConfigVar::GetVariable(pCVarName);
	return (pCfg ? pCfg->ToInt() : -1);
}

const char* Config_GetString(const char* pCVarName)
{
	ConfigVar* pCfg = ConfigVar::GetVariable(pCVarName);
	return (pCfg ? pCfg->ToString() : NULL);
}

void Config_GetColor(const char* pCVarName, GUI_COLOR& pColor)
{
	ConfigVar* pCfg = ConfigVar::GetVariable(pCVarName);

	// Couldn't find the variable
	if (!pCfg)
		return;

	// Set the color
	pColor = pCfg->ToColor();
}

bool Config_SetValue(const char* pCVarName, const char* pValue, bool bUserInput)
{
	ConfigVar* pCfg = ConfigVar::GetVariable(pCVarName);

	// Couldn't find it!
	if (!pCfg)
		return false;

	// Set its value..
	pCfg->SetValue(pValue, bUserInput, true);

	// Done!
	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

MYCHAR* GetLocalPlayerCharacter()
{
	return &gMC;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

int GetKeybind(CSM_KEYBIND_DESC iType)
{
	int* keys[] = { &gKeyJump, &gKeyShot, &gKeyArms, &gKeyArmsRev, &gKeyItem, &gKeyMap, &gKeyOk, &gKeyCancel, &gKeyLeft, &gKeyUp, &gKeyRight, &gKeyDown };

	return *keys[(int)iType % (sizeof(keys) / sizeof(keys[0]))];
}

int GetInput(CSM_KEY_DESC iType)
{
	int* keys[] = { &gKey, &gKeyTrg, &gKey2, &gKeyTrg2, &gKey_JOY, &gKeyTrg_JOY };

	return *keys[(int)iType % (sizeof(keys) / sizeof(keys[0]))];
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

bool SafeClientInterface::Set(CaveNet::DataStructures::NetClient* pClient, unsigned int iSetType)
{
	CaveNet::DataStructures::NetClient* pGotClient = NULL;
	mIsOurUser                                     = false;

	if ((iSetType & SCI_SetType::SCI_ST_EXACT) == SCI_SetType::SCI_ST_EXACT)
	{
		// Just use the client parameter.
		pGotClient = pClient;
	}
	else
	{
		// Validate set flags have at least ONE specifier in them
		if (!(iSetType & SCI_SetType::SCI_ST_CLIENT) && !(iSetType & SCI_SetType::SCI_ST_SERVER))
			return false;

		// Handle automatic detection
		if ((iSetType & SCI_SetType::SCI_ST_AUTO) == SCI_SetType::SCI_ST_AUTO)
		{
			// Determine the best set type
			if (pClient == NULL)
				iSetType &= ~(CaveNet::Server::IsHosting() ? SCI_SetType::SCI_ST_CLIENT : SCI_SetType::SCI_ST_SERVER);
			else
				iSetType &= ~(pClient->mCache.is_ghost ? SCI_SetType::SCI_ST_SERVER : SCI_SetType::SCI_ST_CLIENT);
		}

		// Get the best-scoring client
		if (iSetType & SCI_SetType::SCI_ST_CLIENT)
		{
			// Client-sided
			if (pClient == NULL)
			{
				// Local player
				pGotClient = NULL;

				// Do something different if the set flags specify only clients can be used
				if ((iSetType & SCI_SetType::SCI_ST_ONLY_CLIENTS))
				{
					// Get the local player's client-sided client
					pGotClient = CaveNet::Client::GetOurClient();

					// Validate the client
					if (!pGotClient || !pGotClient->mCache.is_ghost)
						return false;
				}
			}
			else if (pClient->mCache.is_ghost)
			{
				// This is already valid
				pGotClient = pClient;
			}
			else if (!CaveNet::Client::IsConnected())
			{
				// We're not connected to a server! Stop here.
				return false;
			}
			else
			{
				// Attempt to find this client in the client-side clientgroup
				pGotClient = CaveNet::Client::ClientClientGroup::GetClientByGhostID(pClient->GetGhostId());

				// Validate the client
				if (!pGotClient || !pGotClient->mCache.is_ghost)
					return false;
			}
		}
		else if (iSetType & SCI_SetType::SCI_ST_SERVER)
		{
			// Server-sided
			if (pClient == NULL)
			{
				// Local player
				pGotClient = NULL;

				// Do something different if the set flags specify only clients can be used
				if ((iSetType & SCI_SetType::SCI_ST_ONLY_CLIENTS))
				{
					// Get the local player's client-sided client
					pGotClient = CaveNet::Server::GetLocalClient();

					// Validate the client
					if (!pGotClient || pGotClient->mCache.is_ghost)
						return false;
				}
			}
			else if (!pClient->mCache.is_ghost)
			{
				// This is already valid
				pGotClient = pClient;
			}
			else if (!CaveNet::Server::IsHosting())
			{
				// We're not hosting a server! Stop here.
				return false;
			}
			else
			{
				// Attempt to find this client in the client-side clientgroup
				pGotClient = CaveNet::Server::ServerClientGroup::GetClientByGhostID(pClient->GetGhostId());

				// Validate the client
				if (!pGotClient || pGotClient->mCache.is_ghost)
					return false;
			}
		}
	}

	// Handle getting the local player
	if (pGotClient == NULL)
	{
		// Always local player
		mClient       = CaveNet::Client::GetOurClient();
		mGameFlags    = &g_GameFlags;
		mSelectedArms = &gSelectedArms;
		mArms         = gArmsData;
		mItems        = gItemData;
		mMC           = &gMC;
		mKey          = &gKey;
		mKeyTrg       = &gKeyTrg;
		mKeyOld       = &gKeyOld;
		mGhostId      = CSM_OUR_GHOST_ID;
		mSpurCharge   = &spur_charge;
		mSpurMax      = &spur_max;
		mPermitStages = gPermitStage;
		mIsOurUser    = true;
		mClientSide   = true;

		return true;
	}

	// Otherwise, set it based on pGotClient.
	mClient       = pGotClient;
	mGameFlags    = &mClient->mCache.gameFlags;
	mSelectedArms = &mClient->mCache.currentWeapon;
	mArms         = mClient->mCache.armsData;
	mItems        = mClient->mCache.itemData;
	mMC           = mClient->GetMyChar((iSetType & SCI_ST_ENSURE_PLAYER) ? true : false);
	mKey          = &mClient->mCache.gameKey;
	mKeyTrg       = &mClient->mCache.gameKeyTrg;
	mKeyOld       = &mClient->mCache.gameKeyOld;
	mGhostId      = mClient->GetGhostId();
	mSpurCharge   = &mClient->mCache.spur_charge;
	mSpurMax      = &mClient->mCache.spur_max;
	mPermitStages = mClient->mCache.permitStage;
	mIsOurUser    = (mClient->mCache.is_ghost ? (mClient == CaveNet::Client::GetOurClient()) : (mClient == CaveNet::Server::GetLocalClient()));
	mClientSide   = mClient->mCache.is_ghost;

	// Done
	return true;
}

void SafeClientInterface::DamagePlayer(int iDamage)
{
	if (mIsOurUser)
		DamageMyChar(iDamage);

	if (mClient && mClient->GetPlayer())
		mClient->GetPlayer()->Damage(iDamage);
}

void SafeClientInterface::AddExp(int iAmount, bool bSpur)
{
	if (mIsOurUser)
		AddExpMyChar(iAmount, bSpur);

	if (mClient)
		mClient->AddExp(iAmount, bSpur);
}

bool SafeClientInterface::IsMaxExp()
{
	return (mClient ? mClient->IsMaxExp() : (mIsOurUser ? IsMaxExpMyChar() : false));
}

void SafeClientInterface::ZeroExp()
{
	if (mIsOurUser)
		::ZeroExpMyChar();

	if (mClient)
		mClient->ZeroExpMyChar();
}

int SafeClientInterface::CountArmsBullet(int iArmsCode)
{
	int iCount = ::CountArmsBullet(iArmsCode, mGhostId);

	if (mIsOurUser)
		return ::CountArmsBullet(iArmsCode, CSM_OUR_GHOST_ID);

	return iCount;
}

int SafeClientInterface::CountBulletNum(int iBulletCode)
{
	int iCount = ::CountBulletNum(iBulletCode, mGhostId);

	if (mIsOurUser)
		return ::CountBulletNum(iBulletCode, CSM_OUR_GHOST_ID);

	return iCount;
}

bool SafeClientInterface::ChangeArmsEnergy(int iAmmoAmt)
{
	if (*mSelectedArms < 0 || *mSelectedArms >= ARMS_MAX)
		return false;

	mArms[*mSelectedArms].num += iAmmoAmt;

	if (mArms[*mSelectedArms].num > mArms[*mSelectedArms].max_num)
		mArms[*mSelectedArms].num = mArms[*mSelectedArms].max_num;

	return true;
}

bool SafeClientInterface::UseArmsEnergy(int iAmount)
{
	return ::UseArmsEnergy(iAmount, mArms, *mSelectedArms);
}

bool SafeClientInterface::HasArmsEnergy(int iWeaponId, int iAmount)
{
	return ::CheckArmsEnergy(iWeaponId, iAmount, mArms);
}

int SafeClientInterface::GetArmsEnergy(int iWeaponId)
{
	for (int i = 0; i < ARMS_MAX; i++)
	{
		if (mArms[i].code != iWeaponId)
			continue;

		return mArms[i].num;
	}

	return -1;
}

int SafeClientInterface::GetArmsMaxEnergy(int iWeaponId)
{
	for (int i = 0; i < ARMS_MAX; i++)
	{
		if (mArms[i].code != iWeaponId)
			continue;

		return mArms[i].max_num;
	}

	return -1;
}

int SafeClientInterface::GetArmsExp(int iWeaponId, int* iMaxExp)
{
	for (int i = 0; i < ARMS_MAX; i++)
	{
		if (mArms[i].code != iWeaponId)
			continue;

		// Found the weapon!
		if (iMaxExp)
			*iMaxExp = gCaveMod.mWeapons.list[iWeaponId].level_info[mArms[i].level - 1].exp;

		return mArms[i].exp;
	}

	if (iMaxExp)
		*iMaxExp = -1;

	return -1;
}

void SafeClientInterface::ChangeToFirstArms()
{
	if (mIsOurUser)
		::ChangeToFirstArms();

	if (mClient)
	{
		mClient->mCache.currentWeapon = 0;
		PlaySoundObject(4, 1);

		mClient->TransmitPlayerUpdate();
	}
}

void SafeClientInterface::NextWeapon()
{
	if (mIsOurUser)
		::RotationArms();

	if (mClient)
	{
		mClient->ArmsNext();
		mClient->TransmitPlayerUpdate();
	}
}

void SafeClientInterface::PreviousWeapon()
{
	if (mIsOurUser)
		::RotationArmsRev();

	if (mClient)
	{
		mClient->ArmsPrev();
		mClient->TransmitPlayerUpdate();
	}
}

unsigned long long int SafeClientInterface::TestInput(unsigned long long int iFlags, CSM_KEY_DESC iInputType)
{
	switch (iInputType)
	{
		case CSM_KEY_DESC_KEY:
		{
			return (mIsOurUser ? gKey : (mClient ? mClient->mCache.gameKey : 0)) & iFlags;
		}
		case CSM_KEY_DESC_KEYTRG:
		{
			return (mIsOurUser ? gKey : (mClient ? mClient->mCache.gameKey : 0)) & iFlags;
		}
		case CSM_KEY_DESC_KEY2:
		{
			return (mIsOurUser ? gKey2 : 0) & iFlags;
		}
		case CSM_KEY_DESC_KEYTRG2:
		{
			return (mIsOurUser ? gKeyTrg2 : 0) & iFlags;
		}
		case CSM_KEY_DESC_KEY_JOY:
		{
			return (mIsOurUser ? gKey_JOY : 0) & iFlags;
		}
		case CSM_KEY_DESC_KEYTRG_JOY:
		{
			return (mIsOurUser ? gKeyTrg_JOY : 0) & iFlags;
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void CaveNet_CreateTeam(int iTeamId, bool bClientSide, void* pOut)
{
	*((CaveNet::DataStructures::NetTeam**)pOut) = new CaveNet::DataStructures::NetTeam(iTeamId, bClientSide);
}

unsigned int CaveNet_Client_GetGlobalTimerOffset()
{
	return (CaveNet::Server::IsHosting() ? CaveNet::Server::GetGlobalTimerOffset() : CaveNet::Client::GetGlobalTimerOffset());
}

bool CaveNet_Client_IsInGui()
{
	return CaveNet::Client::gCurrentGUI != CaveNet::DataStructures::GUI_Type::GUI_NONE;
}

bool CaveNet_Client_IsConnected()
{
	return CaveNet::Client::IsConnected();
}

int CaveNet_Client_GetGhostId()
{
	return CaveNet::Client::GetConfig()->ghost_id;
}

CaveNet::DataStructures::NetClient* CaveNet_Client_ClientGroup_GetClient(int index)
{
	return CaveNet::Client::ClientClientGroup::GetClient(index);
}

CaveNet::DataStructures::NetClient* CaveNet_Client_ClientGroup_GetClientByGhostId(int ghost_id)
{
	return CaveNet::Client::ClientClientGroup::GetClientByGhostID(ghost_id);
}

int CaveNet_Client_ClientGroup_GetClientCount()
{
	return CaveNet::Client::ClientClientGroup::GetClientCount();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

unsigned int CaveNet_Server_GetGlobalTimerOffset()
{
	return CaveNet::Server::GetGlobalTimerOffset();
}

bool CaveNet_Server_IsHosting()
{
	return CaveNet::Server::IsHosting();
}

bool CaveNet_Server_InLobby()
{
	return CaveNet::Server::InLobby();
}

CaveNet::DataStructures::NetClient* CaveNet_Server_ClientGroup_AddBot(const char* pBotName, const char* pCharName, unsigned char iColorR, unsigned char iColorG, unsigned char iColorB, bool bRandomColor)
{
	if (!CaveNet_Server_IsHosting())
		return NULL;

	return CaveNet::Server::ServerClientGroup::AddBot(pBotName, pCharName, iColorR, iColorG, iColorB, bRandomColor);
}

CaveNet::DataStructures::NetClient* CaveNet_Server_ClientGroup_GetClient(int index)
{
	return CaveNet::Server::ServerClientGroup::GetClient(index);
}

CaveNet::DataStructures::NetClient* CaveNet_Server_ClientGroup_GetClientByGhostId(int ghost_id)
{
	return CaveNet::Server::ServerClientGroup::GetClientByGhostID(ghost_id);
}

int CaveNet_Server_ClientGroup_GetClientCount()
{
	return CaveNet::Server::ServerClientGroup::GetClientCount();
}

void CaveNet_Server_ClientGroup_TransmitSystemMessage(const char* pFormat, ...)
{
	// Format it
	char ChatMsg[2048];
	va_list vARGs;
	va_start(vARGs, pFormat);
	vsprintf_s(ChatMsg, sizeof(ChatMsg), pFormat, vARGs);
	va_end(vARGs);

	// Send it to everyone
	CaveNet::Server::ServerClientGroup::TransmitSystemMessage("%s", ChatMsg);
}

void CaveNet_Server_ClientGroup_ReviveAllDeadPlayers()
{
	CaveNet::Server::ServerClientGroup::ReviveAllDeadPlayers();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

#include <CSMAPI_endcode.h>

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Main page
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @mainpage Cavestory Multiplayer Modding
///
/// @section intro_sec Introduction
///
/// Welcome to the documentation for Cavestory Multiplayer modding tools!
///
/// @section modhelp_sec Modding Help
///
/// @ref getstarted \n
/// @ref ccaveedit \n
/// @ref ccharedit 
///
/// @section license_sec License
///
///  Cavestory Multiplayer API\n
///  Copyright (C) 2021 Johnny Ledger
///
///  This software is provided 'as-is', without any express or implied
///  warranty. In no event will the authors be held liable for any damages
///  arising from the use of this software.
///
///  Permission is granted to anyone to use this software for any purpose,
///  including commercial applications, and to alter it and redistribute it
///  freely, subject to the following restrictions:
///
///  1. The origin of this software must not be misrepresented; you must not
///	 claim that you wrote the original software. If you use this software
///	 in a product, an acknowledgment in the product documentation would be
///	 appreciated but is not required.
///  2. Altered source versions must be plainly marked as such, and must not be
///	 misrepresented as being the original software.
///  3. This notice may not be removed or altered from any source distribution.

//------

/// @page getstarted Getting Started
///
/// WIP

//------

/// @page ccharedit Cavestory Character Editor
///
/// WIP

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Groups
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @defgroup Raycasting
/// @brief Casting game rays.
///
/// Game raycasting functions.

//------

/// @defgroup GUI
/// @brief Graphical user interface functions
///
/// GUI drawing & planning.

//------

/// @defgroup Mouse_Input
///
/// Mouse control

//------

/// @defgroup Fonts
/// @brief Stencil 72!!!
///
/// Font manipulation.

//------

/// @defgroup Logging
/// @brief Printing to the console.
///
/// Functions to print to the console with.

//------

/// @defgroup Function_Hooking
/// @brief Function hooking.
///
/// Hooking functions to various game aspects.

//------

/// @defgroup Configuration
/// @brief Config_SetValue("SecretSue", "1");
///
/// Functions to manipulate game configuration variables.

//------

/// @defgroup Game
///
/// A list of game manipulation functions.

//------

/// @defgroup Input
/// @brief Input management
///
/// Functions for input management.

//------

/// @defgroup Time
///
/// Functions for telling time.

//------

/// @defgroup Math
/// @brief In-house math functions.
///
/// Several helpful in-house math functions.

//------

/// @defgroup Sound
/// @brief In-house math functions.
///
/// Several helpful in-house math functions.

//------

/// @defgroup Hooks
/// @brief Function Hooking
///
/// Helps modders execute their code in-line with the game's normal executing code.

//------

/// @defgroup CaveNet
/// @brief Cavestory Networking
///
/// Cavestory Multiplayer Backend

//------

/// @defgroup Demos
/// @brief Create & manipulate game demos
///
/// These functions assist with creating & manipulating demos.
/// An example of loading a GameDemo:
/// @code{.cpp}
/// GAME_DEMO_STATE pDemo;
///
/// CSM_GameDemo_Init(&pDemo);
///
/// if (CSM_GameDemo_Load(&pDemo, "demos/MyDemo.pxdemo"))
/// {
///		// Do stuff here!
/// }
///
/// CSM_GameDemo_Free(&pDemo);
/// @endcode

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Examples
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @example WeaponShoot.cpp
/// @brief An example of #CSM_Weapon_ShootFunc.
/// 
/// @param sData The shoot information.
/// @param iLevel The level of the weapon, starting from 1.

//------

/// @example BulletAct.cpp
/// @brief An example of #CSM_Bullet_ActFunc.
/// 
/// @param pBul A pointer to the bullet.
/// @param iLevel The level of the bullet, starting from 1.

//------

/// @example WelcomeMode.cpp
/// @brief An example of a GUI mode.

//------

/// @example WelcomeMode.h
/// @brief An example of a GUI mode.

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Defines
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @def CAVESTORY_MOD_API
/// @brief Exports / imports Cavestory Mod API functions & classes.
/// @ingroup Game
/// 

//------

/// @def CSM_GETRED
/// @brief Get the red value from an unsigned int color.
/// @ingroup Math
///
/// @param x The number.
/// 
/// @return Returns an 'unsigned char' of the red value.
/// @see CSM_RGBA
/// @see CSM_RGB

//------

/// @def CSM_GETGREEN
/// @brief Get the green value from an unsigned int color.
/// @ingroup Math
///
/// @param x The number.
/// 
/// @return Returns an 'unsigned char' of the green value.
/// @see CSM_RGBA
/// @see CSM_RGB

//------

/// @def CSM_GETBLUE
/// @brief Get the blue value from an unsigned int color.
/// @ingroup Math
///
/// @param x The number.
/// 
/// @return Returns an 'unsigned char' of the blue value.
/// @see CSM_RGBA
/// @see CSM_RGB

//------

/// @def CSM_GETALPHA
/// @brief Get the alpha value from an unsigned int color.
/// @ingroup Math
///
/// @param x The number.
/// 
/// @return Returns an 'unsigned char' of the alpha value.
/// @see CSM_RGBA
/// @see CSM_RGB

//------

/// @def CSM_RGBA
/// @brief Combine R, G, B, A into one number.
/// @ingroup Math
///
/// @param r The red value.
/// @param g The green value.
/// @param b The blue alpha.
/// @param a The alpha alpha.
/// 
/// @return Returns an 'unsigned int' of 'r', 'g', 'b' and 'a' being combined.

//------

/// @def CSM_RGB
/// @brief Combine R, G, B into one number.
/// @ingroup Math
///
/// @param r The red value.
/// @param g The green value.
/// @param b The blue alpha.
/// 
/// @return Returns an 'unsigned int' of 'r', 'g' and 'b' being combined.

//------

/// @def SET_RECT
/// @brief Set a #RECT object's values.
/// @ingroup Math
///
/// @param rect The rect to set.
/// @param l Left (X) value.
/// @param t Top (Y) value.
/// @param r Right (X + W) value.
/// @param b Bottom (Y + H) value.

//------

/// @def MAX
/// @brief Get the maximum value between two values.
/// @ingroup Math
///
/// @param A The first value.
/// @param B The second value.
///
/// @return Returns 'A' if 'A' is greater than 'B', otherwise returns 'B'.

//------

/// @def MIN
/// @brief Get the minimum value between two values.
/// @ingroup Math
///
/// @param A The first value.
/// @param B The second value.
///
/// @return Returns 'A' if 'A' is less than 'B', otherwise returns 'B'.

//------

/// @def CLAMP
/// @brief Clamp a value.
/// @ingroup Math
///
/// @param x The value to clamp.
/// @param min The minimum allowed value.
/// @param max The maximum allowed value.
///
/// @return Returns the 'x' value clamped between 'min' and 'max'.

//------

/// @def NPC_MAX
/// @ingroup Game
///
/// How many npcs can be initialized at one time.

//------

/// @def BOSS_MAX
/// @ingroup Game
///
/// How many bosses can be initialized at one time.

//------

/// @def ITEM_MAX
/// @ingroup Game
///
/// How many items can be initialized at one time for each client.

//------

/// @def ARMS_MAX
/// @ingroup Game
///
/// How many weapons can be initialized at one time for each client.

//------

/// @def BULLET_MAX
/// @ingroup Game
///
/// How many bullets can be initialized at one time.

//------

/// @def CAVENET_VERSION
/// @ingroup CaveNet
///
/// The current version of CaveNet

//------

/// @def SOUND_MODE_STOP
/// @brief Stop sound.
/// @see PlaySoundObject
/// @ingroup Sound
///
/// Tells PlaySoundObject to stop the sound.

//------

/// @def SOUND_MODE_STOP_THEN_PLAY
/// @brief Stop and then play a sound.
/// @see PlaySoundObject
/// @ingroup Sound
///
/// Tells PlaySoundObject to stop the sound, and then plays it afterwards.

//------

/// @def SOUND_MODE_PLAY
/// @brief Play a sound.
/// @see PlaySoundObject
/// @ingroup Sound
/// 
/// Tells PlaySoundObject to immediately play a sound.

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//