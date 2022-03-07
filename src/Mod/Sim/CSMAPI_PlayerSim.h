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
  @file CSMAPI_PlayerSim.h
*/

#ifndef CAVESTORY_MOD_API_H_
#error "Please include CavestoryModAPI.h before including simulation files."
#endif

#pragma once
#include <CSMAPI_begincode.h>

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Player Simulation
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @brief Run standard code to kill a player.
///
/// @param pSCI The interface to use.
/// @param bResetVelocity Reset the player's velocity.
/// @param iDeathType The type of death.
/// @param pKillerNpc The NPC that killed this player (if applicable).
/// @param pKillerClient The client that killed this player (if applicable).
/// @param bSimulating Whether this is a simulation frame or not.
///
/// @return Returns TRUE on success, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_Player_Kill(
	SafeClientInterface* pSCI,
	bool bResetVelocity                               = true,
	PlayerDeathType iDeathType                        = PlayerDeathType::DEATHTYPE_UNKNOWN,
	NPCHAR* pKillerNpc                                = NULL,
	CaveNet::DataStructures::NetClient* pKillerClient = NULL,
	BOOL bSimulating                                  = FALSE
);

/// @brief Run standard code to animate a player.
///
/// @param bKey Allow key input.
/// @param pSCI The interface to use.
///
/// @return Returns TRUE on success, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_Player_Animate(
	BOOL bKey,
	SafeClientInterface* pSCI
);

/// @brief Run standard code to process a player's camera.
///
/// @param bKey Allow key input.
/// @param pSCI The interface to use.
///
/// @return Returns TRUE on success, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_Player_ActCamera(
	BOOL bKey,
	SafeClientInterface* pSCI
);

/// @brief Run standard code to process amount of air for players.
///
/// @param pSCI The interface to use.
/// @param bSimulating If this is true, then the player cannot run the drown event.
///
/// @return Returns TRUE on success, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_Player_AirProcess(
	SafeClientInterface* pSCI,
	BOOL bSimulating = FALSE
);

/// @brief Reset player collision flags.
///
/// @param pSCI The interface to use
///
/// @return Returns TRUE on success, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_Player_ResetFlag(
	SafeClientInterface* pSCI
);

/// @brief Get KeyTrg from a player.
///
/// @param pSCI The interface to use
///
/// @return Returns TRUE on success, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_Player_GetTrg(
	SafeClientInterface* pSCI
);

/// @brief Run standard code to make a player act.
///
/// @param bKey Allow key input.
/// @param pSCI The interface to use.
/// @param bRecord Whether or not to record to the active demo.
/// @param bSimulating Whether this is a simulation frame or not.
///
/// @return Returns TRUE on success, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_Player_Act(
	BOOL bKey,
	SafeClientInterface* pSCI,
	BOOL bRecord = TRUE,
	BOOL bSimulating = FALSE
);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

#include <CSMAPI_endcode.h>