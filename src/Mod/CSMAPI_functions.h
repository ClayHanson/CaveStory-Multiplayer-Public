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
  @file CSMAPI_functions.h
*/

#pragma once
#include <CSMAPI_begincode.h>

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Logging
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Logging
/// @{
///

 /// @brief Log a message to the console.
 ///
 /// @param pFormat The format of the string.
 /// @param ... Extra formatting arguments.
extern CAVESTORY_MOD_API void CSM_Log(
	const char* pFormat,
	...
);

/// @brief Display a crash screen
///
/// @param iErrorCode The error code to display alongside the error name.
/// @param pErrorName The name of the error (Ex. "MY_MOD_COULDNT_FIND_NPC"; Can be whatever you want). Max limit 32 characters.
/// @param pTitle The title message of the crash window. Can be a locale string (just start it with '#'). Uses the mod name as the first argument.
/// @param pMessage The crash message to be included with everything else. New lines are supported. Can be a locale string (just start it with '#').
/// @param bGenerateCrashLog Whether or not to generate a crash log.
///
/// @note This sets your DLL to be unloaded the next main-thread game tick.
extern CAVESTORY_MOD_API void CSM_Explode(
	int iErrorCode,
	const char* pErrorName,
	const char* pTitle = "\\a{0} crashed!",
	const char* pMessage = "The mod's DLL has crashed!",
	bool bGenerateCrashLog = true
);

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Function Management
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Function_Hooking
/// @{
///

/// @brief Register a weapon by name.
///
/// @param pWeaponName The (case-insensitive) name of the weapon to map the function to.
/// @param pFunc A pointer to the shoot function.
///
/// @return Returns true on success, false otherwise.
///
/// @note The function added here will be used every frame that the game is not frozen on.
extern CAVESTORY_MOD_API bool CSM_RegisterWeaponByName
(
	const char* pWeaponName,
	CSM_Weapon_ShootFunc pFunc
);

/// @brief Register a weapon.
///
/// @param iWeaponIndex The bullet that this act function belongs to.
/// @param pFunc A pointer to the shoot function.
///
/// @return Returns true on success, false otherwise.
///
/// @note The function added here will be used every frame that the game is not frozen on.
extern CAVESTORY_MOD_API bool CSM_RegisterWeapon
(
	int iWeaponIndex,
	CSM_Weapon_ShootFunc pFunc
);

/// @brief Register a bullet.
///
/// @param iBulletIndex The bullet that this act function belongs to.
/// @param pFunc A pointer to the act function to add.
///
/// @return Returns true on success, false otherwise.
///
/// @note The function added here will be used every frame that the game is not frozen on.
extern CAVESTORY_MOD_API bool CSM_RegisterBullet
(
	int iBulletIndex,
	CSM_Bullet_ActFunc pFunc
);

/// @brief Register an NPC.
///
/// @param iNpcIndex The NPC's index
/// @param pFunc The NPC's act function.
///
/// @return Returns true on success, false otherwise.
extern CAVESTORY_MOD_API bool CSM_RegisterNpc
(
	int iNpcIndex,
	CSM_Npc_ActFunc pFunc
);

/// @brief Register a caret.
///
/// @param pActFunc The caret's act function
/// @param iWidth The width of the caret, in pixels. Not subpixels / magnified.
/// @param iHeight The height of the caret, in pixels. Not subpixels / magnified.
///
/// @return Returns the index of the registered caret on success, otherwise returns -1 on fail.
extern CAVESTORY_MOD_API int CSM_RegisterCaret
(
	CSM_Caret_ActFunc pFunc,
	int iWidth,
	int iHeight
);

/// @brief Register a player state.
///
/// @param pStateName The name of the player state.
/// @param iFlags The flags for this state.
/// @param iTransmitFlags The networking transmit flags.
/// @param pActFunc The act function for this player state.
/// @param pAnimFunc The animation function for this player state.
/// @param pDrawFunc The draw function for this player state. Can be NULL.
/// @param pPlayerStateIndex A pointer to an int that will hold the index that this player state was assigned to.
/// @param iIndexOverride Use this to override a existing player state. Pass '0' to this arg to append it as a new playerstate.
/// @param iOutputType Tells the animation system how to parse the return value from the animate function for this player state.
extern CAVESTORY_MOD_API BOOL CSM_RegisterPlayerState(
	const char* pStateName,
	unsigned long long int iFlags,
	unsigned int iTransmitFlags,
	PlayerState_ActFuncType pActFunc,
	PlayerState_AnimFuncType pAnimFunc,
	PlayerState_DrawFuncType pDrawFunc = NULL,
	int* pPlayerStateIndex = NULL,
	unsigned int iIndexOverride = 0,
	PlayerStateAnimator_OutputType iOutputType = PlayerStateAnimator_OutputType::PSAOT_PLAYER_FRAME
);

/// @brief Register a textscript command.
///
/// @param pCmdName The command's name. If this string is not three characters in size, then this function will fail.
/// @param pShortDesc The command's short-hand description. Ex: <TRA has the shorthand description "TRAnsport"
/// @param pDesc A description of what the command does.
/// @param pArgs A string containing the argument types for each argument. Should length of the string shoul correspond to the number of arguments there are.
/// @param pFuncPtr A pointer to the actual TSC function.
///
/// @return Returns true on success, false on failure.
extern CAVESTORY_MOD_API BOOL CSM_RegisterTextScriptCommand(
	const char* pCmdName,
	const char* pShortDesc,
	const char* pDesc,
	const char* pArgs,
	void(*pFuncPtr)(int&, int&, int&, int&, bool&, int&)
);

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Config Functions
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Hooks
/// @{
///

/// @brief Called after the game is finished drawing gameplay elements to the screen, but before it draws GUI elements (excluding HUD).
///
typedef void(*CSM_HOOK_OnPostDrawFunc)();

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnPostDraw(
	CSM_HOOK_OnPostDrawFunc pFuncPtr
);

//-------------

/// @brief Called after the game is finished drawing everything.
///
/// @param bFlash Whether the HUD should flash or not.
typedef void(*CSM_HOOK_OnDrawHUDFunc)(BOOL bFlash);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnDrawHUD(
	CSM_HOOK_OnDrawHUDFunc pFuncPtr
);

//-------------

/// @brief Called before the map's background tiles are rendered.
///
typedef void(*CSM_HOOK_OnDrawMapBackFunc)();

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnDrawMapBack(
	CSM_HOOK_OnDrawMapBackFunc pFuncPtr
);

//-------------

/// @brief Called before the map's foreground tiles are rendered.
///
typedef void(*CSM_HOOK_OnDrawMapFrontFunc)();

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnDrawMapFront(
	CSM_HOOK_OnDrawMapFrontFunc pFuncPtr
);

//-------------

/// @brief Called after all gameplay elements are drawn, but before the HUD is drawn.
///
typedef void(*CSM_HOOK_OnPreDrawHUDFunc)();

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnPreDrawHUD(
	CSM_HOOK_OnPreDrawHUDFunc pFuncPtr
);

//-------------

/// @brief Called when an item is drawn in the inventory menu.
///
/// @param pDrawRect The draw rect.
/// @param pMC The actual player character who owns this.
/// @param pItem The item's slot info.
/// @param iItemIndex The item's index number.
///
/// @return Should return TRUE if the item should be drawn normally, or FALSE if the item shouldn't be drawn normally. This allows this callback to draw the item in a custom way.
typedef BOOL(*CSM_HOOK_OnDrawItemFunc)(GUI_RECT pDrawRect, MYCHAR* pMC, ITEM* pItem, int iItemIndex);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnDrawItem(
	CSM_HOOK_OnDrawItemFunc pFuncPtr
);

//-------------

/// @brief Called before a client is deleted after leaving.
///
/// @param pClient The client that is disconnecting.
/// @note This only works when hosting a server.
typedef void(*CSM_HOOK_OnClientDisconnectedFunc)(CaveNet::DataStructures::NetClient* pClient);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnClientDisconnected(
	CSM_HOOK_OnClientDisconnectedFunc pFuncPtr
);

//-------------

/// @brief Called when a client has connected to the server.
///
/// @param pClient The client that has connected.
/// @note This only works when hosting a server.
typedef void(*CSM_HOOK_OnClientConnectedFunc)(CaveNet::DataStructures::NetClient* pClient);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnClientConnected(
	CSM_HOOK_OnClientConnectedFunc pFuncPtr
);

//-------------

/// @brief Called every time a client spawns.
///
/// @param pClient The client that has connected.
/// @note This only works when hosting a server.
typedef void(*CSM_HOOK_OnClientSpawnFunc)(CaveNet::DataStructures::NetClient* pClient);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnClientSpawn(
	CSM_HOOK_OnClientSpawnFunc pFuncPtr
);

//-------------

/// @brief Called every time a client dies.
///
/// @param pClient The client that has connected. This is always NULL in singleplayer.
/// @param iDeathType Specifies how the client died.
/// @param pKillerNpc If iDeathType == PlayerDeathTypes::DEATHTYPE_KILLED_BY_NPC, then this will be equal to the NPC that killed us. Otherwise will be NULL.
/// @param pKillerClient If iDeathType == PlayerDeathTypes::DEATHTYPE_KILLED_BY_PLAYER, then this will be equal to the client that killed us. Otherwise will be NULL.
///
/// @note This only works when hosting a server OR if you're in singleplayer mode.
typedef void(*CSM_HOOK_OnClientDieFunc)(CaveNet::DataStructures::NetClient* pClient, PlayerDeathType iDeathType, NPCHAR* pKillerNpc, CaveNet::DataStructures::NetClient* pKillerClient);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnClientDie(
	CSM_HOOK_OnClientDieFunc pFuncPtr
);

//-------------

/// @brief Called every time a client is revived.
///
/// @param pClient The client that was revived.
/// @param pSavior The client that saved pClient. Can be NULL for instances like reviving them through the player list.
///
/// @note This only works when hosting a server.
typedef void(*CSM_HOOK_OnClientReviveFunc)(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetClient* pSavior);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnClientRevive(
	CSM_HOOK_OnClientReviveFunc pFuncPtr
);

//-------------

/// @brief Called every time the stage changes.
///
/// @param pStageName The stage's mapname.
/// @param iAreaIndex The index of the area that the stage is assigned to.
typedef void(*CSM_HOOK_OnStageChangeFunc)(const char* pStageName, int iAreaIndex);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnStageChange(
	CSM_HOOK_OnStageChangeFunc pFuncPtr
);

//-------------

/// @brief Called every time a player picks up a heart.
///
/// @param pHeart A pointer to the heart NPC.
/// @param pClient The owner of the character. NULL signifies a singleplayer client.
/// @param pMC A pointer to the player object that should get the health.
/// @param iHealth The amount of health to give.
///
/// @return Should return TRUE if we should add the health normally.
typedef BOOL(*CSM_HOOK_OnPlayerPickupHeartFunc)(NPCHAR* pHeart, CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, int iHealth);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnPlayerPickupHeart(
	CSM_HOOK_OnPlayerPickupHeartFunc pFuncPtr
);

//-------------

/// @brief Called every time a player picks up a missile.
///
/// @param pMissile A pointer to the ammo NPC.
/// @param pClient The owner of the character. NULL signifies a singleplayer client.
/// @param pMC A pointer to the player object that should get the ammo.
/// @param iAmmo The amount of ammo to add.
///
/// @return Should return TRUE if we should add the ammo normally.
typedef BOOL(*CSM_HOOK_OnPlayerPickupMissileFunc)(NPCHAR* pMissile, CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, int iAmmo);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnPlayerPickupMissile(
	CSM_HOOK_OnPlayerPickupMissileFunc pFuncPtr
);

//-------------

/// @brief Called every time a player picks up experience.
///
/// @param pExp A pointer to the experience NPC.
/// @param pClient The owner of the character. NULL signifies a singleplayer client.
/// @param pMC A pointer to the player object that should get the ammo.
/// @param iExp The amount of ammo to add.
///
/// @return Should return TRUE if we should add the experience normally.
typedef BOOL(*CSM_HOOK_OnPlayerPickupExperienceFunc)(NPCHAR* pExp, CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, int iExp);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnPlayerPickupExperience(
	CSM_HOOK_OnPlayerPickupExperienceFunc pFuncPtr
);

//-------------

/// @brief Called every time the server ticks.
///
typedef void(*CSM_HOOK_OnServerTickFunc)();

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnServerTick(
	CSM_HOOK_OnServerTickFunc pFuncPtr
);

//-------------

/// @brief Called when a client is added to a team.
///
/// @param pClient The client-side client.
/// @param pServerClient The server-side client. This is 'NULL' if we're not the server host.
/// @param pTeam The team accepting the client.
typedef void(*CSM_HOOK_OnTeamAddMemberFunc)(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetClient* pServerClient, CaveNet::DataStructures::NetTeam* pTeam);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnTeamAddMember(
	CSM_HOOK_OnTeamAddMemberFunc pFuncPtr
);

//-------------

/// @brief Called when a client is removed from a team.
///
/// @param pClient The client-side client.
/// @param pServerClient The server-side client. This is 'NULL' if we're not the server host.
/// @param pTeam The team booting the client out.
typedef void(*CSM_HOOK_OnTeamRemoveMemberFunc)(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetClient* pServerClient, CaveNet::DataStructures::NetTeam* pTeam);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnTeamRemoveMember(
	CSM_HOOK_OnTeamRemoveMemberFunc pFuncPtr
);

//-------------

/// @brief Called when an NPCHAR with a whitelisted code_char can be picked up by the player.
///
/// @param pClient The client being tested. If this is 'NULL', then we are testing the singleplayer character.
/// @param pPlayer The player being tested. If this is 'NULL', then we are testing the singleplayer character.
/// @param pMC The MYCHAR* object being tested.
/// @param pNpc The NPCHAR* object being tested for.
typedef void(*CSM_HOOK_OnPlayerPickupNpcFunc)(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnPlayerPickupNpc(
	CSM_HOOK_OnPlayerPickupNpcFunc pFuncPtr
);

/// @brief Set whether a specific code_char is whitelisted or not.
///
/// When a code_char is whitelisted, then 'OnHitMyCharNpChar' will be called for that specific NPC.
///
/// @param iCodeChar The code character.
/// @param bWhitelisted Whether to whitelist this codechar or not.
extern CAVESTORY_MOD_API void CSM_SetNpcCodeCharWhitelisted(
	int iCodeChar,
	bool bWhitelisted
);

//-------------

/// @brief Called when a bullet is tested for collision against the map.
///
/// @param pBullet The bullet object being tested.
///
/// @return Should return FALSE if the normal collision check should be run, otherwise return FALSE to skip it.
typedef BOOL(*CSM_HOOK_OnHitBulletMapFunc)(BULLET* pBullet);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnHitBulletMap(
	CSM_HOOK_OnHitBulletMapFunc pFuncPtr
);

//-------------

/// @brief Called when a player is being checked for collision with NPCs.
///
/// @param pClient The client being tested. If this is 'NULL', then we are testing the singleplayer character.
/// @param pPlayer The player object being tested. This will never be NULL.
/// @param bNoclip Whether the player is using noclip or not.
///
/// @return Should return FALSE if the normal collision check should be run, otherwise return FALSE to skip it.
typedef BOOL(*CSM_HOOK_OnHitMyCharNpCharFunc)(CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, bool bNoclip);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnHitMyCharNpChar(
	CSM_HOOK_OnHitMyCharNpCharFunc pFuncPtr
);

//-------------

/// @brief Called when a player needs to test collision against a whitelisted NPC.
///
/// @param pClient The client being tested. If this is 'NULL', then we are testing the singleplayer character.
/// @param pPlayer The player object being tested. This will never be NULL.
/// @param pNpc The NPC being tested against. This will never be NULL.
/// @param bNoclip Whether the player is using noclip or not.
/// @param bCallAgain If this is set to TRUE in the function, then this will be called AFTER main collision is calculated. This is only possible once every frame.
///
/// @return Should return FALSE if the normal collision check should be run, otherwise return FALSE to skip it.
typedef BOOL(*CSM_HOOK_OnMyCharCollisionTest_NpChar)(CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, NPCHAR* pNpc, BOOL bNoclip, BOOL& bCallAgain);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param iNpcId The NPC ID to set this function to.
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnMyCharCollisionTest_NpChar(
	int iNpcId,
	CSM_HOOK_OnMyCharCollisionTest_NpChar pFuncPtr
);

//-------------

/// @brief Called when a player is being checked for map collision against a specific map tile.
///
/// @param pClient The client being tested. If this is 'NULL', then we are testing the singleplayer character.
/// @param pMC The player object being tested. This will never be NULL.
/// @param iTileX The tile's X position.
/// @param iTileY The tile's Y position.
/// @param iTileAttrib The attribute of the tile.
/// @param bNoclip Whether the player is using noclip or not.
///
/// @return Should return FALSE if the normal collision check should be run, otherwise return FALSE to skip it.
typedef BOOL(*CSM_HOOK_OnHitMyCharMapTileFunc)(CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, int iTileX, int iTileY, int iTileAttrib, bool bNoclip);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnHitMyCharMapTile(
	CSM_HOOK_OnHitMyCharMapTileFunc pFuncPtr
);

//-------------

/// @brief Called when a player is being checked for map collision.
///
/// @param pClient The client being tested. If this is 'NULL', then we are testing the singleplayer character.
/// @param pMC The player object being tested. This will never be NULL.
/// @param bNoclip Whether the player is using noclip or not.
///
/// @return Should return FALSE if the normal collision check should be run, otherwise return FALSE to skip it.
typedef BOOL(*CSM_HOOK_OnHitMyCharMapFunc)(CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, bool bNoclip);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnHitMyCharMap(
	CSM_HOOK_OnHitMyCharMapFunc pFuncPtr
);

//-------------

/// @brief Called when an AI-controlled player is thinking.
///
/// @param pClient The AI's client.
/// @param pPlayer The AI's player.
/// @param pTeam The AI's team.
///
/// @note This is obviously only called server-side.
typedef void(*CSM_HOOK_OnProcessAIFunc)(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, CaveNet::DataStructures::NetTeam* pTeam);

/// @brief Hook up a function to make custom code run during a game event. Check out the typedef of the function pointer for further info.
///
/// @param pFuncPtr A pointer to the function to call.
extern CAVESTORY_MOD_API void CSM_SetHook_OnProcessAI(
	CSM_HOOK_OnProcessAIFunc pFuncPtr
);

//-------------

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Config Functions
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Configuration
/// @{
///

/// @brief Get a boolean value from a configuration variable.
///
/// @param pCVarName The name of the configuration variable
///
/// @return Returns the value.
extern CAVESTORY_MOD_API bool Config_GetBool(
	const char* pCVarName
);

/// @brief Get an integer value from a configuration variable.
///
/// @param pCVarName The name of the configuration variable
///
/// @return Returns the value.
extern CAVESTORY_MOD_API int Config_GetInt(
	const char* pCVarName
);

/// @brief Get a string value from a configuration variable.
///
/// @param pCVarName The name of the configuration variable
///
/// @return Returns the value.
extern CAVESTORY_MOD_API const char* Config_GetString(
	const char* pCVarName
);

/// @brief Get a color value from a configuration variable.
///
/// @param pCVarName The name of the configuration variable
/// @param pColor The color.
///
/// @return Retursn the value.
extern CAVESTORY_MOD_API void Config_GetColor(
	const char* pCVarName,
	GUI_COLOR& pColor
);

/// @brief Set the value of a configuration variable.
///
/// @param pCVarName The name of the configuration variable
/// @param pValue The new value
/// @param bUserInput Whether this was inputted by a user
///
/// @return Returns true on success, false if otherwise.
extern CAVESTORY_MOD_API bool Config_SetValue(
	const char* pCVarName,
	const char* pValue,
	bool bUserInput = false
);

/// @}


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Server Functions
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup CaveNet
/// @{

/// @brief Check to see whether we're connected to a server or not.
///
/// @return Returns true if we're connected to a server, false otherwise.
extern CAVESTORY_MOD_API bool CSM_CaveNet_ConnectedAsClient();

/// @brief Check to see whether we're hosting a server.
///
/// @return Returns true if we're hosting a server.
extern CAVESTORY_MOD_API bool CSM_CaveNet_IsHosting();

/// @brief Get how many clients are currently connected to the server.
///
/// @return Returns how many clients are connected.
extern CAVESTORY_MOD_API int CSM_CaveNet_GetClientCount();

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Game Functions
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Bosses
/// @{
///

/// @brief Reset all bosses' health back to their initial health amounts.
///
extern CAVESTORY_MOD_API void CSM_Boss_ResetAllBossesHealth(
);

/// @brief Get a boss NPC by index.
///
/// @param iIndex The index of the boss NPC.
///
/// @return Returns a pointer to an entry in gBoss.
extern CAVESTORY_MOD_API NPCHAR* CSM_Boss_GetBossNpc(
	int iIndex
);

/// @brief Get the number of valid boss functions.
///
/// @return Returns number of valid boss functions.
extern CAVESTORY_MOD_API int CSM_Boss_GetBossCount(
);

/// @brief Get the current boss' init health.
///
/// @return Returns the current boss' init health.
/// @note Current boss is determined by gBoss[0].code_char
extern CAVESTORY_MOD_API int CSM_Boss_GetCurrentBossDefaultHealth(
);

/// @brief Set the initializer health for a boss.
///
/// @param iBossType The boss type to set the init health for.
/// @param iHealth The amount of health the boss should have on initialization.
extern CAVESTORY_MOD_API void CSM_Boss_SetInitHealth(
	CSM_BossTypes iBossType,
	int iHealth
);

/// @}
///

/// @addtogroup Raycasting
/// @{
///

/// @brief Cast a ray in the game world.
///
/// @param pPointStart The ray's start point.
/// @param pPointEnd The ray's end point.
/// @param pInfo A pointer to a RAYCAST_QUERY object that will store the results of the raycast. Can be NULL.
/// @param bHitEndBlock Whether to hit the end block.
/// @param bHitStartBlock Whether to hit the start block.
///
/// @return Returns true on hit, false otherwise.
/// @note Uses non-magnified, non-game-scaled points. AKA no '* magnification' and no '* 0x200' points are allowed.
extern CAVESTORY_MOD_API BOOL CSM_Raycasting_CastRay(
	GUI_POINT pPointStart,
	GUI_POINT pPointEnd,
	RAYCAST_QUERY* pInfo = NULL,
	bool bHitEndBlock = true,
	bool bHitStartBlock = true
);

/// @brief Cast a ray in the game world, but faster.
///
/// @param iStartX The starting subpixel X coordinate.
/// @param iStartY The starting subpixel Y coordinate.
/// @param iEndX The ending subpixel X coordinate.
/// @param iEndY The ending subpixel Y coordinate.
/// @param iHitMask The TileType mask to hit.
/// @param pInfo A pointer to a RAYCAST_QUERY object that will store the results of the raycast. Can be NULL.
/// @param bHitEndBlock Whether to hit the end block.
/// @param bHitStartBlock Whether to hit the start block.
///
/// @return Returns true on hit, false otherwise.
/// @note Does not hit entities.
/// @see CSM_Raycasting_CastRay
extern CAVESTORY_MOD_API BOOL CSM_Raycasting_CastRay_Quick(
	int iStartX,
	int iStartY,
	int iEndX,
	int iEndY,
	unsigned int iHitMask,
	RAYCAST_QUERY* pInfo = NULL,
	bool bHitEndBlock = true,
	bool bHitStartBlock = true
);

/// @brief Setup raycasting bounds.
///
/// Set the raycasting bounds. Casted rays cannot exceed these bounds.
///
/// @param pRect The bounds to use. If set to NULL, rays will not be bound.
extern CAVESTORY_MOD_API void CSM_Raycasting_SetBounds(
	GUI_RECT* pRect
);

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Game Functions
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Video
/// @{
///

/// @brief Set the game's window resolution.
///
/// @param iNewWidth The new width of the window Cannot go below 320.
/// @param iNewHeight The new height of the window. Cannot go below 240.
/// @param iNewMagnification The new magnification level. Set this to '<= 0' to automatically determine a suitable magnification level.
extern CAVESTORY_MOD_API void CSM_Video_SetResolution(
	int iNewWidth,
	int iNewHeight,
	int iNewMagnification = -1
);

/// @brief Get the game's window resolution.
///
/// @param iWidth A pointer to hold the resolution width. Can be NULL.
/// @param iHeight A pointer to hold the resolution height. Can be NULL.
extern CAVESTORY_MOD_API void CSM_Video_GetResolution(
	int* iWidth,
	int* iHeight
);

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Game Functions
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Game
/// @{
///

/// @brief Draw the weapon.
///
/// @param iFrameX Camera X offset
/// @param iFrameY Camera Y offset
/// @param pInterface The interface to use
/// @param fScale The scale of the weapon
extern CAVESTORY_MOD_API void CSM_PlayerState_DrawWeapon(
	int iFrameX,
	int iFrameY,
	SafeClientInterface* pInterface,
	float fScale = 1.f
);

/// @brief Draw the character.
///
/// @param iFrameX Camera X offset
/// @param iFrameY Camera Y offset
/// @param pInterface The interface to use
/// @param fScale The scale of the player
extern CAVESTORY_MOD_API void CSM_PlayerState_DrawCharacter(
	int iFrameX,
	int iFrameY,
	SafeClientInterface* pInterface,
	float fScale = 1.f
);

/// @brief Find a player animation frame's index by its name.
///
/// @param pName The name of the player animation (ex: "FRAME_COLLAPSED")
/// 
/// @return Returns the index of the player animation on success, otherwise returns -1 if the animation could not be found.
extern CAVESTORY_MOD_API int CSM_FindPlayerAnimFrameByName(
	const char* pName
);

/// @brief Determine if a pixel on a specific frame of animation is opaque or not.
///
/// @param pCharacter The player character to determine the solidity of.
/// @param iFrame The frame to reference.
/// @param iX The X position.
/// @param iY The Y position.
/// 
/// @return Returns TRUE if the pixel has an alpha color value of 127 or higher, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_CustomPlayer_IsPixelSolid(
	CustomPlayer* pCharacter,
	int iFrame,
	int iX,
	int iY
);

/// @brief Get the surface ID for a character's normal plane.
///
/// @param pCharacter The player character object.
/// 
/// @return Returns the surface ID on success, FALSE otherwise.
extern CAVESTORY_MOD_API Surface_Ids CSM_CustomPlayer_GetNormalSurfaceId(
	CustomPlayer* pCharacter
);

/// @brief Get the surface ID for a character's normal mask plane.
///
/// @param pCharacter The player character object.
/// 
/// @return Returns the surface ID on success, FALSE otherwise.
extern CAVESTORY_MOD_API Surface_Ids CSM_CustomPlayer_GetNormalMaskSurfaceId(
	CustomPlayer* pCharacter
);

/// @brief Get the surface ID for a character's face plane.
///
/// @param pCharacter The player character object.
/// 
/// @return Returns the surface ID on success, FALSE otherwise.
extern CAVESTORY_MOD_API Surface_Ids CSM_CustomPlayer_GetFaceSurfaceId(
	CustomPlayer* pCharacter
);

/// @brief Get the surface ID for a character's face mask plane.
///
/// @param pCharacter The player character object.
/// 
/// @return Returns the surface ID on success, FALSE otherwise.
extern CAVESTORY_MOD_API Surface_Ids CSM_CustomPlayer_GetFaceMaskSurfaceId(
	CustomPlayer* pCharacter
);

/// @brief Clear all view values.
///
extern CAVESTORY_MOD_API void ClearValueView(
);

/// @brief Create a new view value.
///
/// @param pX A pointer to the damaged entity's X variable.
/// @param pY A pointer to the damaged entity's Y variable.
/// @param iValue The value to show.
extern CAVESTORY_MOD_API void SetValueView(
	int* pX, int* pY, int iValue
);

/// @brief Process all view values.
///
extern CAVESTORY_MOD_API void ActValueView(
);

/// @brief Draw all view values.
///
/// @param iFrameX The camera's X position.
/// @param iFrameY The camera's Y position.
extern CAVESTORY_MOD_API void PutValueView(
	int iFrameX,
	int iFrameY
);

/// @brief Check to see if an NPC is clipping bounds with another NPC.
///
/// @param pNpc The NPCHAR to test for.
/// @param pOther The NPCHAR to test against.
///
/// @return Returns true if they are overlapping, false otherwise.
extern CAVESTORY_MOD_API BOOL IsNpCharInsideNpChar(
	NPCHAR* pNpc,
	NPCHAR* pOther
);

/// @brief Check to see if an NPC is clipping bounds with a player.
///
/// @param pMC The player to test for.
/// @param pNpc The NPCHAR to test against.
///
/// @return Returns true if they are overlapping, false otherwise.
extern CAVESTORY_MOD_API bool IsSpecificPlayerInsideNpChar(
	MYCHAR* pMC,
	NPCHAR* pNpc
);

/// @brief Check to see if an NPC is clipping bounds with the player.
///
/// @param pNpc The NPCHAR to test against.
///
/// @return Returns true if they are overlapping, false otherwise.
/// @note This function uses 'gMC', NOT pNpc->focus.
extern CAVESTORY_MOD_API bool IsPlayerInsideNpChar(
	NPCHAR* pNpc
);

/// @brief Check to see if an NPC is clipping bounds with the NPC's focus object.
///
/// @param pNpc The NPCHAR to test against.
///
/// @return Returns true if they are overlapping, false otherwise.
/// @note This function uses 'pNpc->focus', NOT 'gMC'.
extern CAVESTORY_MOD_API bool IsFocusInsideNpChar(
	NPCHAR* pNpc
);

/// @brief Shake the screen for iTime ticks.
///
/// @param iTime The ticks to shake the screen for.
extern CAVESTORY_MOD_API void SetQuake(
	int iTime
);

/// @brief Shake the screen for iTime ticks.
///
/// @param iTime The ticks to shake the screen for.
extern CAVESTORY_MOD_API void SetQuake2(
	int iTime
);

/// @brief Stop shaking the screen.
///
extern CAVESTORY_MOD_API void ResetQuake(
);

/// @brief Check whether the current mod allows agility.
///
/// @return Returns true if it is, false otherwise.
extern CAVESTORY_MOD_API bool IsAgilityAllowed();

/// @brief Check whether the current mod allows fishing.
///
/// @return Returns true if it is, false otherwise.
extern CAVESTORY_MOD_API bool IsFishingAllowed();

/// @brief Check whether the current mod allows carrying other players.
///
/// @return Returns true if it is, false otherwise.
extern CAVESTORY_MOD_API bool IsCarryingAllowed();

/// @brief Spawn a bullet at the coordinates.
/// 
/// @param iBulNo The ID of the bullet to spawn.
/// @param iSpawnX The X position to spawn the bullet at.
/// @param iSpawnY The Y position to spawn the bullet at.
/// @param iDirection The direction the bullet should be facing.
/// @param iCodeArms Should be set to the weapon number that spawned this bullet (-1 if no weapon spawned it)
/// @param iLevel The level of the bullet
/// @param iGhostId Should be set to the client ghost ID that spawned this bullet (-1 if no client spawned it)
extern CAVESTORY_MOD_API void SetBullet(
	int iBulNo,
	int iSpawnX,
	int iSpawnY,
	int iDirection,
	int iCodeArms,
	int iLevel,
	int iGhostId = 0
);

/// @brief Spawn a caret at the coordinates.
///
/// @param iSpawnX The subpixel X position to spawn the caret at.
/// @param iSpawnY The subpixel Y position to spawn the caret at.
/// @param iCaretId The ID of the caret to spawn.
/// @param iDirection The direction to spawn the caret at.
extern CAVESTORY_MOD_API void SetCaret(
	int iSpawnX,
	int iSpawnY,
	int iCaretId,
	int iDirection
);

/// @brief Spawn an NPC at the coordinates.
/// 
/// @param iNpcId The ID of the NPC to spawn.
/// @param iSpawnX The initial X position.
/// @param iSpawnY The initial Y position.
/// @param iVelocityX The initial X velocity.
/// @param iVelocityY The initial Y velocity.
/// @param iDirection The initial direction.
/// @param pNpc The NPC this should be pointing to.
/// @param iStartIndex Which NPC slot the spawner should start from when looking for an available slot.
/// @param iOutNPC A pointer to the spawned NPC (NULL if the NPC couldn't be spawned!)
extern CAVESTORY_MOD_API void SetNpChar(
	int iNpcId,
	int iSpawnX,
	int iSpawnY,
	int iVelocityX,
	int iVelocityY,
	int iDirection,
	NPCHAR* pNpc,
	int iStartIndex,
	NPCHAR** iOutNPC = 0
);

/// @brief Delete all NPCs with a specific event number.
///
/// @param iEventNo The event number to search for.
extern CAVESTORY_MOD_API void DeleteNpCharEvent(
	int iEventNo
);

/// @brief Delete all NPCs with a specific character code.
///
/// @param iCharCode The character code to search for.
/// @param bSmoke Whether to spawn smoke on deletion or not.
extern CAVESTORY_MOD_API void DeleteNpCharCode(
	int iCharCode,
	BOOL bSmoke
);

/// @brief Spawn an NPC at the coordinates.
/// 
/// @param iNpcId The ID of the NPC to spawn.
/// @param iSpawnX The initial X position.
/// @param iSpawnY The initial Y position.
/// @param iVelocityX The initial X velocity.
/// @param iVelocityY The initial Y velocity.
/// @param iDirection The initial direction.
/// @param pNpc The NPC this should be pointing to.
/// @param iStartIndex Which NPC slot the spawner should start from when looking for an available slot.
/// @param iOutNPC A pointer to the spawned NPC (NULL if the NPC couldn't be spawned!)
void CAVESTORY_MOD_API SetNpChar_Backwards(
	int iNpcId,
	int iSpawnX,
	int iSpawnY,
	int iVelocityX,
	int iVelocityY,
	int iDirection,
	NPCHAR* pNpc,
	int iStartIndex,
	NPCHAR** iOutNPC = 0
);

/// @brief Spawn an animation NPC at the coordinates.
/// 
/// @param iNpcId The ID of the NPC to spawn.
/// @param iVelocityX The initial X velocity.
/// @param iVelocityY The initial Y velocity.
/// @param iDirection The initial direction.
/// @param pNpc The NPC this should be pointing to.
/// @param iStartIndex Which NPC slot the spawner should start from when looking for an available slot.
/// @param pMC The player character this anim npc should be tied to.
/// @param pChar The custom player character for this player.
/// @param iOutNPC A pointer to the spawned NPC (NULL if the NPC couldn't be spawned!)
///
/// @deprecated Animation NPCs have been replaced by the PlayerState system.
extern CAVESTORY_MOD_API bool SetAnimNpChar(
	int iNpcId,
	int iVelocityX,
	int iVelocityY,
	int iDirection,
	NPCHAR* pNpc,
	int iStartIndex,
	MYCHAR* pMC,
	CustomPlayer* pCustomChar,
	NPCHAR** iOutNPC = 0
);

/// @brief Get the number of bullets matching the given description onscreen.
///
/// @param iArmsCode The arms the bullets should belong to.
/// @param iGhostId The ghost ID they should belong to. Can be -1 to count bullets regardless of ghost id.
///
/// @return Returns the number of bullets matching the description.
extern CAVESTORY_MOD_API int CountArmsBullet(
	int iArmsCode,
	int iGhostId = -1
);

/// @brief Get the number of bullets matching the given description onscreen.
///
/// @param iArmsCode The arms the bullets should belong to.
/// @param iGhostId The ghost ID they should belong to. Can be -1 to count bullets regardless of ghost id.
///
/// @return Returns the number of bullets matching the description.
extern CAVESTORY_MOD_API int CountBulletNum(
	int iBulletCode,
	int iGhostId = -1
);

/// @brief Delete all bullets matching the arms ID.
///
/// @param iArmsCode The arms code whose bullets we should be deleting.
extern CAVESTORY_MOD_API void DeleteArmBullets(
	int iArmsCode
);

/// @brief Delete all bullets.
///
extern CAVESTORY_MOD_API void ClearBullet();

/// @brief Check if some bullet is active.
///
extern CAVESTORY_MOD_API bool IsActiveSomeBullet();

/// @brief Check to see if the singleplayer character has a weapon.
///
/// @param iCode The weapon ID.
///
/// @return Returns TRUE if the weapon was found, FALSE if not.
extern CAVESTORY_MOD_API BOOL HasArmsData(
	long iCode
);

/// @brief Add arms data to the singleplayer character.
///
/// @param iCode The weapon ID.
/// @param iNum The amount of ammo.
/// @param iMaxNum Maximum amount of ammo. Pass '0' to have infinite ammo.
/// @param iLevel The level of the weapon. Only pass 1 through 3.
extern CAVESTORY_MOD_API BOOL AddArmsData2(
	long iCode,
	long iNum,
	long iMaxNum,
	long iLevel
);

/// @brief Use some arms energy from the selected arm.
///
/// @param iNum The amount of energy to use.
/// @param pArmsTable A pointer to the arms table.
/// @param iSelectedIdx The currently selected weapon index.
extern CAVESTORY_MOD_API BOOL UseArmsEnergy(
	long iNum,
	ARMS* pArmsTable,
	int iSelectedIdx
);

/// @brief Switch to this client's first weapon.
///
extern CAVESTORY_MOD_API void ChangeToFirstArms();

/// @brief Get the local player.
///
/// @return Returns gMC.
extern CAVESTORY_MOD_API MYCHAR* GetLocalPlayerCharacter();

/// @brief Charge the selected weapon in pArmsTable
///
/// @param iChargeAmt The amount to charge.
/// @param pArmsTable The arms table.
/// @param iSelectedWeapon The selected weapon
///
/// @return Returns true on success, false otherwise.
extern CAVESTORY_MOD_API BOOL ChargeArmsEnergy(
	long iChargeAmt,
	ARMS* pArmsTable,
	int iSelectedWeapon
);

/// @brief Spawns smoke at the given coordinates & flashes the screen
///
/// @param iSpawnX The smoke's X spawn coordniate
/// @param iSpawnY The smoke's Y spawn coordniate
/// @param iMaxRandomXOffset The maximum random X value offset.
/// @param iSmokeAmt How many smoke particles to spawn.
extern CAVESTORY_MOD_API void SetDestroyNpCharUp(
	int iSpawnX,
	int iSpawnY,
	int iMaxRandomXOffset,
	int iSmokeAmt
);

/// @brief Transfer over to a new map.
///
/// @param iStageNo The stage index.
/// @param iEventNo The script event to call.
/// @param iSpawnX Map tile's X position to spawn on.
/// @param iSpawnY Map tile's Y position to spawn on.
///
/// @return Returns 1 on success.
extern CAVESTORY_MOD_API BOOL TransferStage(
	int iStageNo,
	int iEventNo, 
	int iSpawnX,
	int iSpawnY
);

/// @brief Transfer over to a new map without moving characters.
///
/// @param iStageNo The stage index.
/// @param iEventNo The script event to call.
///
/// @return Returns 1 on success.
extern CAVESTORY_MOD_API BOOL TransferStage2(
	int iStageNo,
	int iEventNo
);

/// @brief Get the CRC value of a property field's name.
///
/// @param pFieldName The name of the property field.
///
/// @return Returns the hashed result.
extern CAVESTORY_MOD_API unsigned int CSM_EntProp_GetFieldNameCRC(
	const char* pFieldName
);

/// @brief Lookup the index of an NPC's property field.
///
/// @param pNpc The NPC to get the field of.
/// @param iFieldCrc The field name's CRC value.
///
/// @return Returns the index of the field on success, otherwise returns -1.
/// @see CSM_EntProp_GetFieldNameCRC
extern CAVESTORY_MOD_API int CSM_EntProp_GetFieldIndexByCrc(
	NPCHAR* pNpc,
	unsigned int iFieldCrc
);

/// @brief Lookup the index of an NPC's property field.
///
/// @param pNpc The NPC to get the field of.
/// @param pFieldName The name of the property field.
///
/// @return Returns the index of the field on success, otherwise returns -1.
extern CAVESTORY_MOD_API int CSM_EntProp_GetFieldIndex(
	NPCHAR* pNpc,
	const char* pFieldName
);

/// @brief Fetch the value of a field for a specific NPC.
///
/// @param pNpc The NPC to get the field of.
/// @param iFieldIndex The index of the field.
/// @param pFormat The expected format of the string.
/// @param ... Pointers to variables that will be filled when reading the string.
///
/// @return Returns the number of variables filled.
extern CAVESTORY_MOD_API int CSM_EntProp_GetFieldValue_ScanString(
	NPCHAR* pNpc,
	int iFieldIndex,
	const char* pFormat,
	...
);

/// @brief Fetch the value of a field for a specific NPC.
///
/// @param pNpc The NPC to get the field of.
/// @param iFieldIndex The index of the field.
/// @param pOutValue A pointer to a buffer to hold the value.
///
/// @return Returns pOutValue.
extern CAVESTORY_MOD_API char* CSM_EntProp_GetFieldValue_String(
	NPCHAR* pNpc,
	int iFieldIndex,
	char* pOutValue,
	const char* pDefaultValue = ""
);

/// @brief Fetch the value of a field for a specific NPC.
///
/// @param pNpc The NPC to get the field of.
/// @param iFieldIndex The index of the field.
/// @param iDefaultValue The default value for this property if it is not defined.
///
/// @return Returns the integer value.
extern CAVESTORY_MOD_API int CSM_EntProp_GetFieldValue_Int(
	NPCHAR* pNpc,
	int iFieldIndex,
	int iDefaultValue = 0
);

/// @brief Fetch the value of a field for a specific NPC.
///
/// @param pNpc The NPC to get the field of.
/// @param iFieldIndex The index of the field.
/// @param fDefaultValue The default value for this property if it is not defined.
///
/// @return Returns the float value.
extern CAVESTORY_MOD_API float CSM_EntProp_GetFieldValue_Float(
	NPCHAR* pNpc,
	int iFieldIndex,
	float fDefaultValue = 0.0f
);

/// @brief Fetch the value of a field for a specific NPC.
///
/// @param pNpc The NPC to get the field of.
/// @param iFieldIndex The index of the field.
/// @param bDefaultValue The default value for this property if it is not defined.
///
/// @return Returns the boolean value.
extern CAVESTORY_MOD_API BOOL CSM_EntProp_GetFieldValue_Boolean(
	NPCHAR* pNpc,
	int iFieldIndex,
	BOOL bDefaultValue = FALSE
);

/// @brief Fetch the value of a field for a specific NPC.
///
/// @param pNpc The NPC to get the field of.
/// @param iFieldIndex The index of the field.
/// @param pColorPtr A pointer to the GUI_COLOR that will hold the information.
/// @param bDefaultValue The default value for this property if it is not defined.
///
/// @return Returns pColorPtr.
extern CAVESTORY_MOD_API GUI_COLOR* CSM_EntProp_GetFieldValue_Color(
	NPCHAR* pNpc,
	int iFieldIndex,
	GUI_COLOR* pColorPtr,
	GUI_COLOR pDefaultValue = GUI_COLOR(0, 0, 0, 0)
);

/// @brief Fetch the value of a field for a specific NPC.
///
/// @param pNpc The NPC to get the field of.
/// @param iFieldIndex The index of the field.
/// @param pPointPtr A pointer to the GUI_POINT that will hold the information.
/// @param bDefaultValue The default value for this property if it is not defined.
///
/// @return Returns pPointPtr.
extern CAVESTORY_MOD_API GUI_POINT* CSM_EntProp_GetFieldValue_Point(
	NPCHAR* pNpc,
	int iFieldIndex,
	GUI_POINT* pPointPtr,
	GUI_POINT pDefaultValue = GUI_POINT(0, 0)
);

/// @brief Fetch the value of a field for a specific NPC.
///
/// @param pNpc The NPC to get the field of.
/// @param iFieldIndex The index of the field.
/// @param pPointPtr A pointer to the GUI_POINT that will hold the information.
/// @param bDefaultValue The default value for this property if it is not defined.
///
/// @return Returns pRectPtr.
extern CAVESTORY_MOD_API GUI_RECT* CSM_EntProp_GetFieldValue_Rect(
	NPCHAR* pNpc,
	int iFieldIndex,
	GUI_RECT* pRectPtr,
	GUI_RECT pDefaultValue = GUI_RECT(0, 0, 0, 0)
);

/// @brief Act animated tiles.
///
extern CAVESTORY_MOD_API void CSM_Map_ActAnimatedTiles();

/// @brief Clear a specific map tile's animation.
///
/// @param bForeground Whether the foreground tiles should be cleared(TRUE), or background tiles should be cleared(FALSE).
extern CAVESTORY_MOD_API void CSM_Map_ClearAllTileAnimations(
	bool bForeground
);

/// @brief Clear a specific map tile's animation.
///
/// @param bForeground Whether the animated tile should be found in the foreground list(TRUE) or background list(FALSE).
/// @param iX The map tile's X position.
/// @param iY The map tile's Y position.
extern CAVESTORY_MOD_API void CSM_Map_ClearTileAnimation(
	bool bForeground,
	unsigned int iX,
	unsigned int iY
);

/// @brief Set a map tile's animation.
///
/// @param bForeground Whether the animated tile should appear on the foreground(TRUE) or background(FALSE).
/// @param iX The map tile's X position.
/// @param iY The map tile's Y position.
/// @param pRectList A list of RECTs defining tile data.
/// @param iRectCount The number of frames pRectList holds.
/// @param iWait The number of frames that should pass before the frame counter increments.
/// @param iSurfaceId The surface ID that should be used to draw the tile.
extern CAVESTORY_MOD_API void CSM_Map_SetTileAnimation(
	bool bForeground,
	unsigned int iX,
	unsigned int iY,
	RECT* pRectList,
	unsigned char iRectCount,
	unsigned short iWait,
	Surface_Ids iSurfaceId = SURFACE_ID_LEVEL_TILESET
);

/// @brief Get the current text script context.
/// 
/// @return Returns the current TS context.
extern CAVESTORY_MOD_API TEXT_SCRIPT_PTR_DATA* GetCurrentTextScriptConfig();

/// @brief Get the text script path.
///
/// @param pPath A buffer to hold the text script path in.
extern CAVESTORY_MOD_API void GetTextScriptPath(
	char* pPath
);

/// @brief Set the current text script context.
///
/// @param pData A pointer to the new context.
extern CAVESTORY_MOD_API void SetTextScriptContext(
	TEXT_SCRIPT_PTR_DATA* pData = &default_TextScriptPtrData
);

/// @brief Start a text script event.
///
/// @param iEventNo The event number.
/// @param bTransmit Transmit to everyone on the server.
/// @param pData The context to start the text script event with.
/// @param bUseRestrictionLevel Use the restriction level option.
/// @param pSourceNpc The NPC that was used to start this event.
///
/// @return Returns true on success.
extern CAVESTORY_MOD_API BOOL StartTextScript(
	int iEventNo,
	bool bTransmit = true,
	TEXT_SCRIPT_PTR_DATA* pData = &default_TextScriptPtrData,
	bool bUseRestrictionLevel = false,
	TextScriptTarget iTargetType = TextScriptTarget::TS_TARGET_EVERYONE,
	NPCHAR* pSourceNpc = NULL
);

/// @brief Stop text script.
/// 
extern CAVESTORY_MOD_API void StopTextScript();

/// @brief Load the time counter's total time.
///
/// @param pFileName The file name of the record to load.
///
/// @return Returns the timer's time.
extern CAVESTORY_MOD_API int LoadTimeCounter(
	const char* pFileName = "290.rec"
);

/// @brief Check if there is a profile.
///
/// @return Returns true if there is a profile.
extern CAVESTORY_MOD_API BOOL IsProfile();

/// @brief Save the current gamestate to a profile.
///
/// @param pFileName The name of the savefile.
/// @param pMC The player object to use. Set to NULL to use gMC.
/// @param pProfileName The name of the savegame.
///
/// @return Returns true on success.
extern CAVESTORY_MOD_API BOOL SaveProfile(
	const char* pFileName,
	MYCHAR* pMC = NULL
);

/// @brief Load a profile into the current gamestate.
///
/// @param pFileName The name of the savefile.
/// @param bDoGameFuncs Whether to call essential game funcs or not.
///
/// @return Returns true on success.
extern CAVESTORY_MOD_API BOOL LoadProfile(
	const char* pFileName,
	BOOL bDoGameFuncs = TRUE
);

/// @brief Save the current gamestate to a PROFILE* struct.
///
/// @param pProfile The profile to copy the gamestate into.
///
/// @return Returns true on success.
extern CAVESTORY_MOD_API BOOL SaveProfileIntoMem(
	PROFILE* pProfile
);

/// @brief Load the gamestate from a PROFILE* struct.
///
/// @param pProfile A pointer to a PROFILE to hold the information.
///
/// @return Returns true on success.
extern CAVESTORY_MOD_API BOOL LoadProfileFromMem(
	PROFILE* pProfile
);

/// @brief Load a profile into a PROFILE* struct.
///
/// @param pName The name of the savefile.
/// @param pProfile A pointer to a PROFILE to hold the information.
/// @param pModifyTime A pointer to an INT which will hold the last modified time for this save file. Can be NULL.
/// @param pProfileName A pointer to a CHAR array which will hold the profile name for this save file. Can be NULL.
///
/// @return Returns true on success.
extern CAVESTORY_MOD_API BOOL LoadProfileIntoMem(
	const char* pName,
	PROFILE* pProfile,
	unsigned long long* pModifyTime = NULL
);

/// @brief Compare tile types for tile at X, Y
///
/// @param iX The map tile's X position that we want to test
/// @param iY The map tile's Y position that we want to test
/// @param iTileTypes The tile types we want to test
///
/// @return Returns an & of the map tile's tiletype flags & tile_types
extern CAVESTORY_MOD_API unsigned int CompareTileTypeXY(
	int iX,
	int iY,
	unsigned int iTileTypes
);

/// @brief Compare tile types for the map tile that npc is on
///
/// @param pNpc The NPC to use the position of
/// @param iTileTypes The tile types we want to test
///
/// @return Returns an & of the map tile's tiletype flags & tile_types
extern CAVESTORY_MOD_API unsigned int CompareTileTypeNPC(
	NPCHAR* pNpc,
	unsigned int iTileTypes
);

/// @brief Compare two tile types against each other
///
/// @param iTileTypes The tile types we want to test
/// @param iOtherTileTypes The tile types we want to test
///
/// @return Returns a bit-wise and of tile_type and tile_types
extern CAVESTORY_MOD_API unsigned int CompareTileType(
	unsigned int iTileTypes,
	unsigned int iOtherTileTypes
);

/// @brief Fetch the tile type flags for a map tile
///
/// @param iX The map tile's X position
/// @param iY The map tile's Y position
///
/// @return Returns the tiletype flags for the corresponding map tile
extern CAVESTORY_MOD_API unsigned int GetTileFlagsXY(
	int iX,
	int iY
);

/// @brief Fetch the tile type flags for a map tile
///
/// @param pNpc The NPC to use the position of
///
/// @return Returns the tiletype flags for the corresponding map tile
/// @see TileType
extern CAVESTORY_MOD_API unsigned int GetTileFlagsNPC(
	NPCHAR* pNpc
);

/// @brief Get the current camera position.
///
/// @param iX A pointer to an int to hold the camera's X position.
/// @param iY A pointer to an int to hold the camera's Y position.
extern CAVESTORY_MOD_API void GetFramePosition(
	int* iX,
	int* iY
);

/// @brief Set frame position.
///
/// @param iX The new X position.
/// @param iY The new Y position.
extern CAVESTORY_MOD_API void SetFramePosition(
	int iX,
	int iY
);

/// @brief Completely fade the screen out.
///
extern CAVESTORY_MOD_API void SetFadeMask();

/// @brief Clear the current screen fade.
///
extern CAVESTORY_MOD_API void ClearFade();

/// @brief Fade the screen out.
///
/// @param iDirect The direction to fade out to.
///
/// @see Directions
extern CAVESTORY_MOD_API void StartFadeOut(
	signed char iDirect
);

/// @brief Fade the screen in.
///
/// @param iDirect The direction to fade in from.
///
/// @see Directions
extern CAVESTORY_MOD_API void StartFadeIn(
	signed char iDirect
);

/// @brief Process the fade.
///
extern CAVESTORY_MOD_API void ProcFade(
);

/// @brief Draw the fade.
///
extern CAVESTORY_MOD_API void PutFade(
);

/// @brief Check to see if the screen is being faded.
///
/// @return Returns true if the screen is fading in or out.
extern CAVESTORY_MOD_API BOOL GetFadeActive(
);

/// @brief Check to see if the screen is blacked out.
///
/// @return Returns true if the screen is faded out completely.
extern CAVESTORY_MOD_API BOOL IsFadedOut(
);

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Input
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Input
/// @{
///

/// @brief Process text scripts.
///
/// @return Returns the mode.
extern CAVESTORY_MOD_API int TextScriptProc();

/// @brief Put text script on the screen
///
extern CAVESTORY_MOD_API void PutTextScript();


/// @brief Get the input flags for a keybind.
///
/// @param iType The keybind descriptor.
///
/// @return Returns the key input flags for the specified keybind.
extern CAVESTORY_MOD_API int GetKeybind(
	CSM_KEYBIND_DESC iType
);

/// @brief Get the current input flags from a store.
///
/// @param iType The descriptor for where to get the input from.
///
/// @return Returns the key input flags from the specified location.
extern CAVESTORY_MOD_API int GetInput(
	CSM_KEY_DESC iType
);

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Time
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Time
/// @{
///

/// @brief Get the current ticks since the program started.
///
/// @return Returns number of ticks since the program started.
extern CAVESTORY_MOD_API unsigned int GetTicks();

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Math
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Math
/// @{
///

#ifndef NO_WINDOWS
/// @brief Get a random value.
///
/// @param iMin The minimum value it can be.
/// @param iMax The maximum value it can be.
///
/// @return Returns a random value.
extern CAVESTORY_MOD_API int Random(
	int iMin,
	int iMax
);
#endif

/// @brief Calculate cos.
///
/// @param iDeg The degrees.
///
/// @param Returns cos value.
extern CAVESTORY_MOD_API int GetCos(
	unsigned char iDeg
);

/// @brief Calculate sin.
///
/// @param iDeg The sin.
///
/// @param Returns sin value.
extern CAVESTORY_MOD_API int GetSin(
	unsigned char iDeg
);

/// @brief Get arktan from x and y.
///
/// @param iX
/// @param iY
///
/// @return Returns result.
extern CAVESTORY_MOD_API unsigned char GetArktan(
	int iX,
	int iY
);

/// @brief Check if an NPC is on screen.
///
/// @param pNpc The NPC to check.
///
/// @return Returns true if the NPC can be seen on-screen.
extern CAVESTORY_MOD_API bool IsNpcOnScreen(
	NPCHAR* pNpc
);

/// @brief Get the draw position of an NPC.
///
/// @param pNpc The NPC to get the position of.
/// @param iType The type of position to get.
/// @param iOutX A pointer to an int that will hold the X position. Can be NULL.
/// @param iOutY A pointer to an int that will hold the Y position. Can be NULL.
extern CAVESTORY_MOD_API void CSM_Npc_GetPosition(
	NPCHAR* pNpc,
	NpcPositionType iType = NpcPositionType::NPT_DRAW_POSITION,
	int* iOutX = NULL,
	int* iOutY = NULL
);

/// @brief Skip one NPC update for a specific NPC.
///
/// Skips the next update for a specific NPC. Useful for NPCs that have information that doesn't need to be updated to all clients.
///
/// @param pNpc The NPC to skip the next update for.
extern CAVESTORY_MOD_API void CSM_Npc_SkipUpdate(
	NPCHAR* pNpc
);

/// @brief Get the transmit nodes for a specific NPC type.
///
/// @param iCodeChar The character code.
/// @param iActNo The act number the NPC is on.
///
/// @return Returns the transmit node flags.
extern CAVESTORY_MOD_API unsigned int CSM_Npc_GetTransmitNodes(
	unsigned int iCodeChar,
	int iActNo
);

/// @brief Get the update nodes for a specific NPC type.
///
/// @param iCodeChar The character code.
/// @param iActNo The act number the NPC is on.
///
/// @return Returns the update node flags.
extern CAVESTORY_MOD_API unsigned int CSM_Npc_GetUpdateNodes(
	unsigned int iCodeChar,
	int iActNo
);

/// @brief Get the draw rectangle of a MYCHAR.
///
/// @param pMC The MYCHAR to check.
/// @param pOut A pointer to a GUI_RECT to hold the draw rectangle.
///
/// @return Returns the calculated draw rectangle.
extern CAVESTORY_MOD_API void GetMyCharDrawRectangle(
	MYCHAR* pMC,
	GUI_RECT* pOut
);

/// @brief Get the draw offset of a player's weapon.
///
/// @param pMC The player.
/// @param iWeaponCode The index of the weapon.
/// @param pOut The point to hold the weapon's draw offset.
///
/// @return Returns the draw position.
/// @note This function returns magnified, non-subpixel coordinates relative to the player's topleft coordinates. All you need to do is add the player's draw position.
extern CAVESTORY_MOD_API void CSM_Weapon_GetDrawOffset(
	MYCHAR* pMC,
	int iWeaponCode,
	GUI_POINT* pOut
);

/// @brief Get the held entity offset of a player's current frame.
///
/// @param pMC The player.
/// @param pOut The point to hold the held entity's draw offset.
///
/// @return Returns the draw position.
/// @note This function returns magnified, non-subpixel coordinates relative to the player's topleft coordinates. All you need to do is add the player's draw position.
extern CAVESTORY_MOD_API void CSM_Puppy_GetDrawOffset(
	MYCHAR* pMC,
	GUI_POINT* pOut
);

/// @brief Get the carried player offset of a player's current frame.
///
/// @param pMC The player.
/// @param pOut The point to hold the carried player's draw offset.
///
/// @return Returns the draw position.
/// @note This function returns magnified, non-subpixel coordinates relative to the player's topleft coordinates. All you need to do is add the player's draw position.
extern CAVESTORY_MOD_API void CSM_Carry_GetDrawOffset(
	MYCHAR* pMC,
	GUI_POINT* pOut
);

/// @brief Convert a subpixel coordinate to screen coordinate.
///
/// @param iCoord The coordinate to convert.
///
/// @return Returns (iCoord * magnification / 0x200)
inline int SubpixelToScreenCoord(
	int iCoord
)
{
	return iCoord * magnification / 0x200;
}

/// @brief Convert a pixel to a screen cordinate.
///
/// @param iCoord
///
/// @return Returns (iCoord * magnification)
inline int PixelToScreenCoord(
	int iCoord
)
{
	return iCoord * magnification;
}

/// @brief Get the current window width.
///
/// @return Returns the window width.
extern CAVESTORY_MOD_API int CSM_GetWindowWidth();

/// @brief Get the current window height.
///
/// @return Returns the window height.
extern CAVESTORY_MOD_API int CSM_GetWindowHeight();

/// @brief Set the magnification of all loaded bitmaps beyond this point.
///
/// @param The new loading magnification.
extern CAVESTORY_MOD_API void CSM_SetBitmapLoadMagnification(
	int iMagnification
);

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Misc
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Misc
/// @{
///

/// @brief Open the escape menu.
///
/// @return Returns the status mode.
extern CAVESTORY_MOD_API int OpenEscapeMenu(
);

/// @brief Open the options menu.
///
/// @param pCategoryList A pointer to a list of category names.
/// @param iCategoryCount Number of categories.
///
/// @return Returns the status mode.
/// @note If pCategoryList is not NULL and iCategoryCount is > 0, then only options belonging to the specified categories will be shown.
extern CAVESTORY_MOD_API int OpenOptionsMenu(
	const char** pCategoryList = NULL,
	int iCategoryCount = 0
);

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// PlayerStates
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup PlayerStates
/// @{
///

/// @brief Set the state of an interface.
///
/// @param pInterface The interface to apply the state to.
/// @param iStateType The type of state to set the player to. Find this in the 'PlayerStates' namespace.
/// @param bForce Force set the state, regardless of whether the given interface is already set to the given state type.
extern CAVESTORY_MOD_API void SetPlayerState(
	SafeClientInterface* pInterface,
	int iStateType = -1,
	bool bForce = false
);

/// @brief Get the current playerstate of an interface.
///
/// @param pInterface The interface to get the state from.
///
/// @return Returns the player state.
extern CAVESTORY_MOD_API int GetPlayerState(
	SafeClientInterface* pInterface
);

/// @brief Check to see if a player is in the specified state.
///
/// @param pChar The character to check for.
/// @param iStateType The state to check.
///
/// @return Returns the player state.
inline bool IsPlayerState(MYCHAR* pChar, int iStateType) { return pChar->netanim == iStateType; }

/// @brief Get the flags for a player's state.
///
/// @param pChar The player to get the state from.
///
/// @return Returns the flags on success, otherwise returns 0 if the player is in the 'Normal' state.
extern CAVESTORY_MOD_API unsigned int GetPlayerStateFlags(
	MYCHAR* pChar
);

/// @brief Get the return type of a player's state's animate function.
///
/// @param pChar The player to get the state from.
///
/// @return Returns the return type.
extern CAVESTORY_MOD_API PlayerStateAnimator_OutputType GetPlayerStateAnimatorType(
	MYCHAR* pChar
);

/// @brief Find a player state by its name.
///
/// @param pName The name of the state.
///
/// @return Returns the index of the playerstate in question on success, otherwise returns -1.
extern CAVESTORY_MOD_API int FindPlayerStateByName(
	const char* pName
);

/// @brief Transform a unit index into a player state index.
///
/// @param iUnitIndex The unit index to transform.
///
/// @return Returns a player state index on success, -1 on failure.
extern CAVESTORY_MOD_API int GetStateByUnitIndex(
	int iUnitIndex
);

/// @brief Transform a player state index into a unit index.
///
/// @param iPlayerState The player state to transform.
///
/// @return Returns a unit index on success, -1 on failure.
extern CAVESTORY_MOD_API int GetUnitIndexByState(
	int iPlayerState
);

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Text Script
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Text Script
/// @{
///

/// @brief Check if a new messagebox line needs to be created. If so, then create it.
///
extern CAVESTORY_MOD_API void CheckNewLine(
);

/// @brief Type a specific number into the message box.
///
/// @param iIndex 
extern CAVESTORY_MOD_API void SetNumberTextScript(
	int iIndex
);

/// @brief Append the character's name into the messagebox.
///
extern CAVESTORY_MOD_API void SetCharacterNameTextScript(
);

/// @brief Clear the messagebox.
///
extern CAVESTORY_MOD_API void ClearTextLine(
);

/// @brief Focus on the player that started the event.
///
extern CAVESTORY_MOD_API void Server_FocusOnCurrentPlayer(
);

/// @brief Stop focusing on the event executor.
///
extern CAVESTORY_MOD_API void Server_ClearFocusFromPlayer(
);

/// @brief Get a number argument from the script.
///
/// @param a The textscript offset to read the number from. Usually 'gTS.p_read + [offset]'.
///
/// @return Returns the value at the given offset in numerical form.
extern CAVESTORY_MOD_API int GetTextScriptNo(
	int a
);

/// @brief Jump to another event.
///
/// @param no The event number to jump to.
///
/// @return Returns true if the event was jumped to successfully, false otherwise.
extern CAVESTORY_MOD_API bool JumpTextScript(
	int no
);

/// @brief Display a custom message in the message box.
///
/// @param pLine4 The fourth line.
/// @param pLine3 The third line.
/// @param pLine2 The second line.
/// @param pLine1 The first line.
extern CAVESTORY_MOD_API void ShowTextLines(
	const char* pLine4,
	const char* pLine3,
	const char* pLine2,
	const char* pLine1
);

/// @brief Appends text to the current messagebox text.
///
/// @param pString The text to add.
extern CAVESTORY_MOD_API void DisplayCustomTextToScript(
	const char* pString
);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Flags
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Flags
/// @{
///

/// @brief Set an NPC flag to 'true'.
///
/// @param iFlagNo The flag number.
/// @param bTransmit Whether to transmit the change to the entire server or not.
extern CAVESTORY_MOD_API void SetNPCFlag(
	long iFlagNo,
	bool bTransmit = true
);

/// @brief Set an NPC flag to 'false'.
///
/// @param iFlagNo The flag number.
/// @param bTransmit Whether to transmit the change to the entire server or not.
extern CAVESTORY_MOD_API void CutNPCFlag(
	long iFlagNo,
	bool bTransmit = true
);

/// @brief Check if an NPC flag is set.
///
/// @param iFlagNo The flag number.
///
/// @return Returns TRUE if the NPC flag is set, FALSE if it isn't.
extern CAVESTORY_MOD_API BOOL GetNPCFlag(
	long iFlagNo
);

/// @brief Set a skip flag to 'true'.
///
/// @param iFlagNo The flag number.
/// @param bTransmit Whether to transmit the change to the entire server or not.
extern CAVESTORY_MOD_API void SetSkipFlag(
	long iFlagNo,
	bool bTransmit = true
);

/// @brief Set a skip flag to 'true'.
///
/// @param iFlagNo The flag number.
/// @param bTransmit Whether to transmit the change to the entire server or not.
extern CAVESTORY_MOD_API void CutSkipFlag(
	long iFlagNo,
	bool bTransmit = true
);

/// @brief Check if a skip flag is set.
///
/// @param iFlagNo The flag number.
///
/// @return Returns TRUE if the skip flag is set, FALSE if it isn't.
extern CAVESTORY_MOD_API BOOL GetSkipFlag(
	long iFlagNo
);

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Sound
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Sound
/// @{
///

/// @brief A custom sound ID.
///
typedef int CSM_CustomPixToneId;
typedef int CSM_CustomSoundId;

/// @brief Load a custom pixtone file.
///
/// @param pPath The file path of the sound file.
///
/// @return Returns a non-zero ID of the new sound on success, -1 on failure.
extern CAVESTORY_MOD_API CSM_CustomPixToneId CSM_LoadPixTone(
	const char* pPath
);

/// @brief Load a custom sound file.
///
/// @param pPath The file path of the sound file.
///
/// @return Returns a non-zero ID of the new sound on success, -1 on failure.
extern CAVESTORY_MOD_API CSM_CustomSoundId CSM_LoadCustomSound(
	const char* pPath
);

/// @brief Set the pan amount of a custom sound.
///
/// @param iSoundId The ID of the sound to manipulate.
/// @param iPanAmount The amount of pan (0 - 100). Center = 50.
extern CAVESTORY_MOD_API void CSM_SetCustomSoundPan(
	CSM_CustomSoundId iSoundId,
	unsigned char iPanAmount = 50
);

/// @brief Set the volume of a custom sound.
///
/// @param iSoundId The ID of the sound to manipulate.
/// @param iVolume The volume (0 - 100).
extern CAVESTORY_MOD_API void CSM_SetCustomSoundVolume(
	CSM_CustomSoundId iSoundId,
	unsigned char iVolume
);

/// @brief Unload a custom sound file.
///
/// @param iSoundId The ID of the sound file to unload.
///
/// @return Returns TRUE on success, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_UnloadPixTone(
	CSM_CustomPixToneId iSoundId
);

/// @brief Unload a custom sound file.
///
/// @param iSoundId The ID of the sound file to unload.
///
/// @return Returns TRUE on success, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_UnloadCustomSound(
	CSM_CustomSoundId iSoundId
);

/// @brief Process the 2D soundscape.
///
extern CAVESTORY_MOD_API void CSM_Sound_Process2D();

/// @brief Play a sound by index at a specific place in the game.
///
/// Plays a sound at a subpixel coordinate with a given distance.
///
/// @param iSubpixelX The X subpixel coordinate to play the sound at.
/// @param iSubpixelY The Y subpixel coordinate to play the sound at.
/// @param iRadius The distance that this will be hearable at, in subpixels.
/// @param bAlwaysOnScreen If this is set to 'TRUE', then iRadius is used as the distance from the edge of the screen that the sound needs to be in order to be heard.
/// @param iSoundIndex The index of the sound.
/// @param iPlayMode How to play the sound. Default is #CSM_SOUND_MODE_STOP_THEN_PLAY.
/// @param iPlayVolume The volume of the sound, from 0 to 0x100.
/// @param iFalloffRadius The sound falloff radius. Will be clamped to iRadus.
extern CAVESTORY_MOD_API void PlaySoundObject2D
(
	int iSubpixelX,
	int iSubpixelY,
	int iRadius,
	bool bAlwaysOnScreen,
	int iSoundIndex,
	int iPlayMode = SOUND_MODE_STOP_THEN_PLAY,
	unsigned int iPlayVolume = 0x100,
	unsigned int iFalloffRadius = 0x7FFFFFFE
);

/// @brief Set the pitch of a sound.
///
/// @param iSoundIndex The index of the sound.
/// @param iPitch The new pitch of the sound. Must be between 0 and 1000, with 500 being normal pitch.
extern CAVESTORY_MOD_API void CSM_Sound_SetPitch(
	int iSoundIndex,
	unsigned short iPitch
);

/// @brief Set the panning of a sound.
///
/// @param iSoundIndex The index of the sound.
/// @param iPan The new pan of the sound. Must be between -127 and 127, with 0 being normal pan.
extern CAVESTORY_MOD_API void CSM_Sound_SetPan(
	int iSoundIndex,
	char iPan
);

/// @brief Set the volume of a sound.
///
/// @param iSoundIndex The index of the sound.
/// @param iVolume The new volume of the sound. Must be between 0 and 255, with 255 being loudest.
extern CAVESTORY_MOD_API void CSM_Sound_SetVolume(
	int iSoundIndex,
	unsigned char iVolume
);

/// @brief Play a sound by index.
///
/// @param iSoundIndex The index of the sound.
/// @param iPlayMode How to play the sound. Default is #CSM_SOUND_MODE_STOP_THEN_PLAY.
/// @param iPlayVolume The volume of the sound, from 0 to 0x100.
extern CAVESTORY_MOD_API void PlaySoundObject
(
	int iSoundIndex,
	int iPlayMode = SOUND_MODE_STOP_THEN_PLAY,
	unsigned int iPlayVolume = 0x100
);

/// @brief Check if a sound is playing.
///
/// @param iSoundId The sound to test for.
///
/// @return Returns TRUE if the sound is still playing, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL IsSoundPlaying(
	int iSoundId
);

/// @brief Set the current music track
///
/// @param iMusicIndex The index of the music to play.
extern CAVESTORY_MOD_API void ChangeMusic
(
	int iMusicIndex
);

/// @brief Play custom music.
///
/// @param pFileName The filename of the music file to play.
extern CAVESTORY_MOD_API void PlayCustomMusic
(
	const char* pFileName
);

/// @brief Set the volume of music.
///
/// @param iVolume The new volume of the music. Must be 0 - 255, with 255 being the loudest and 0 being inaudible.
extern CAVESTORY_MOD_API void CSM_Music_SetVolume
(
	unsigned char iVolume
);

/// @brief Recall the last played music
///
/// @param iMusicIndex The index of the music to play
extern CAVESTORY_MOD_API void ReCallMusic();

/// @}
///

#include <CSMAPI_endcode.h>