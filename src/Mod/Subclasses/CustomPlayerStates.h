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

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Macros
#define NULL_DRAW_FUNC(NAME)													
#define USE_DRAW_FUNC(NAME)														void __DRAW__##NAME(int, int, MYCHAR*, SafeClientInterface*);
#define DECLARE_PLAYER_STATE(NAME, CUSTOM_DRAW)									void __ACT__##NAME(BOOL, SafeClientInterface*, MYCHAR*, const MYCHAR_PHYSICS*);\
																				unsigned int __ANIMATE__##NAME(BOOL, MYCHAR*, SafeClientInterface*);\
																				static int PLAYER_STATE_##NAME;\
																				CUSTOM_DRAW(NAME)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define NULL_DRAW_FUNC_IMPLEMENT(NAME)											PlayerStates::__ACT__##NAME,\
																				PlayerStates::__ANIMATE__##NAME,\
																				NULL
#define USE_DRAW_FUNC_IMPLEMENT(NAME)											PlayerStates::__ACT__##NAME,\
																				PlayerStates::__ANIMATE__##NAME,\
																				PlayerStates::__DRAW__##NAME
#define IMPLEMENT_PLAYER_STATE(NAME, FLAGS, TRANSMIT_FLAGS, CUSTOM_DRAW, OVERRIDE, ANIMATE_TYPE)\
																				CSM_RegisterPlayerState(\
																					"PLAYER_STATE_" #NAME,\
																					FLAGS,\
																					TRANSMIT_FLAGS,\
																					CUSTOM_DRAW##_IMPLEMENT(NAME),\
																					&PlayerStates::PLAYER_STATE_##NAME,\
																					OVERRIDE,\
																					PlayerStateAnimator_OutputType::PSAOT_##ANIMATE_TYPE\
																				);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief When the player hits water, create splash particles.
///
/// Only use this in implemented playerstate ACT functions.
#define PROCESS_WATER_SPLASHING()\
	if (!pMC->ignore_water && !pMC->sprash && (pMC->flag & GMC_flag::gmc_flag_on_water_tile)){\
		int dir = ((pMC->flag & 0x800) ? 2 : 0);\
		if ((pMC->flag & 8) == 0 && pMC->ym > 0x200){for (int a = 0; a < 8; a++){int x = pMC->x + (Random(-8, 8) * 0x200);SetNpChar(73, x, pMC->y, pMC->xm + Random(-0x200, 0x200), Random(-0x200, 0x80) - pMC->ym / 2, dir, 0, 0);}PlaySoundObject(56, 1);}\
		else{if (pMC->xm > 0x200 || pMC->xm < -0x200){for (int a = 0; a < 8; a++){int x = pMC->x + (Random(-8, 8) * 0x200);SetNpChar(73, x, pMC->y, pMC->xm + Random(-0x200, 0x200), Random(-0x200, 0x80), dir, 0, 0);}PlaySoundObject(56, 1);}}\
		pMC->sprash = 1;}\
	if (!(pMC->flag & 0x100))pMC->sprash = 0;

/// @brief If the player is inside of any wind / water current tiles, move them accordingly.
///
/// Only use this in implemented playerstate ACT functions.
#define PROCESS_MOVEMENT_TILES()\
	if (pMC->flag & GMC_flag::gmc_flag_in_current_left)\
		pMC->xm -= 0x88;\
	if (pMC->flag & GMC_flag::gmc_flag_in_current_up)\
		pMC->ym -= 0x80;\
	if (pMC->flag & GMC_flag::gmc_flag_in_current_right)\
		pMC->xm += 0x88;\
	if (pMC->flag & GMC_flag::gmc_flag_in_current_down)\
		pMC->ym += 0x55;

/// @brief If the player is touching damaging map tiles, damage them.
///
/// Only use this in implemented playerstate ACT functions.
#define PROCESS_DAMAGE_TILES()\
	if (pMC->flag & GMC_flag::gmc_flag_on_damage_tile)\
	{\
		if (!CaveNet::Client::IsConnected() || CaveNet::Server::IsHosting())\
			pInterface->DamagePlayer(10);\
	}

/// @brief Limit the player's velocity. Use a multiplier, too.
///
/// Only use this in implemented playerstate ACT functions.
#define LIMIT_VELOCITY(MULT)\
	if (pMC->ym > (physics->max_move * MULT))\
		pMC->ym = (physics->max_move * MULT);\
	if (pMC->ym < -(physics->max_move * MULT))\
		pMC->ym = -(physics->max_move * MULT);\
	if (pMC->xm > (physics->max_move * MULT))\
		pMC->xm = (physics->max_move * MULT);\
	if (pMC->xm < -(physics->max_move * MULT))\
		pMC->xm = -(physics->max_move * MULT);

/// @brief Limit the player's velocity.
///
/// Only use this in implemented playerstate ACT functions.
#define LIMIT_VELOCITY_NORMAL()\
	if (pMC->ym > physics->max_move)\
		pMC->ym = physics->max_move;\
	if (pMC->ym < -physics->max_move)\
		pMC->ym = -physics->max_move;\
	if (pMC->xm > physics->max_move)\
		pMC->xm = physics->max_move;\
	if (pMC->xm < -physics->max_move)\
		pMC->xm = -physics->max_move;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define IMPLEMENT_PLAYER_STATE_ANIM(NAME)		unsigned int PlayerStates::__ANIMATE__##NAME(BOOL bKey, MYCHAR* pMC, SafeClientInterface* pInterface)
#define IMPLEMENT_PLAYER_STATE_ACT(NAME)		void PlayerStates::__ACT__##NAME(BOOL bKey, SafeClientInterface* pInterface, MYCHAR* pMC, const MYCHAR_PHYSICS* physics)
#define IMPLEMENT_PLAYER_STATE_DRAW(NAME)		void PlayerStates::__DRAW__##NAME(int iFrameX, int iFrameY, MYCHAR* pMC, SafeClientInterface* pInterface)
#define GAMEFLAGS								(*pInterface->mGameFlags)
#define KEY										(*pInterface->mKey)
#define KEYTRG									(*pInterface->mKeyTrg)
#define CLIENT									pInterface->mClient
#define ARMS_TABLE								pInterface->mArms
#define SELECTED_WEAPON							(*pInterface->mSelectedArms)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_endcode.h>

/// @}
///