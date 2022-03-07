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
  @file CSMAPI_WeaponTemplates.h
*/

#pragma once
#include <Subclasses/ModConfigResource.h>

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define DEFAULT_WEAPON(NAME, LEVEL1, LEVEL2, LEVEL3, DISPINFO)					{ { LEVEL1, LEVEL2, LEVEL3 }, DISPINFO, NAME }
#define LEVEL_INFO(BULINDEX, EXP, LEFT, TOP, WIDTH, HEIGHT)						{ BULINDEX, EXP, { LEFT, TOP, LEFT + WIDTH, TOP + HEIGHT } }
#define DISPLAY_INFO(LEFT, TOP, WIDTH, HEIGHT, HOLD_OFFSET_X, HOLD_OFFSET_Y)	{ LEFT, TOP, LEFT + WIDTH, TOP + HEIGHT, HOLD_OFFSET_X, HOLD_OFFSET_Y }

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PXMOD_WEAPON_STRUCT g_DefaultWeaponList[] =
{
	// ************* Agility ************* //
	DEFAULT_WEAPON
	(
		"Agility",
		// Level 1
		LEVEL_INFO
		(
			/* Bullet Index        */ 0,
			/* Level-Up Experience */ 10,
			/* View Rect           */ 0, 0, 0, 0
		),
		// Level 2
		LEVEL_INFO
		(
			/* Bullet Index        */ 1,
			/* Level-Up Experience */ 20,
			/* View Rect           */ 0, 0, 0, 0
		),
		// Level 3
		LEVEL_INFO
		(
			/* Bullet Index        */ 2,
			/* Level-Up Experience */ 30,
			/* View Rect           */ 0, 0, 0, 0
		),
		// Basic display information
		DISPLAY_INFO
		(
			/* Weapon Rect   */ 0, 0, 0, 0,
			/* Hold Offset X */ 0,
			/* Hold Offset Y */ 0
		)
	)
};

/*
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
*/

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#undef DEFINE_DEFAULT_WEAPON
#undef DEFINE_DEFAULT_WEAPON