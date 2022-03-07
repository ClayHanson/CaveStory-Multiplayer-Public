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
  @file CSMAPI_defines.h
*/

#pragma once

// The sound modes for CSM_PlaySound
#define SOUND_MODE_STOP									0 // Stop the sound completely. Don't play it.
#define SOUND_MODE_STOP_THEN_PLAY						1 // Stop the sound, then play it from the start.
#define SOUND_MODE_PLAY									2 // Play the sound immediately; don't stop it first.

// Limits
#define CSM_MAX_BULLET_FUNCS							64
#define CSM_MAX_WEAPON_SHOOT_FUNCS						64
#define CSM_MAX_NPC_FUNCS								1024
#define CSM_MAX_CUSTOM_SOUNDS							255

// Game defines
#define CARET_MAX										0x40
#define BULLET_MAX										0x200
#define ARMS_MAX										8
#define ITEM_MAX										32
#define PERMIT_STAGE_COUNT								24
#define MAX_PERMIT_INDEX								8
#define MAX_WHIMSICAL_STARS								3
#define ANIM_NPC_MAX									128
#define NPC_MAX											(0x200 + ANIM_NPC_MAX)
#define ANIM_NPC_START_OFFSET							(NPC_MAX - ANIM_NPC_MAX)
#define BOSS_MAX										20
#define MAX_DYNAMIC_SURFACES							(SURFACE_ID_ALLOCATABLE_SURFACE_END - SURFACE_ID_ALLOCATABLE_SURFACE_START)
//#define WINDOW_WIDTH									426
//#define WINDOW_HEIGHT									240
#define WINDOW_WIDTH									WINDOW_REAL_WIDTH
#define WINDOW_HEIGHT									WINDOW_REAL_HEIGHT
#define PATH_LENGTH										260

// Substitutes
#define CSM_OUR_GHOST_ID								-2 // Use this in place of iGhostId arguments to only affect our player.

// Macro helpers
#define SCALE_GUI_POINT(POINT, CENTER, SCALAR)			(((POINT - CENTER) * SCALAR) + CENTER)
#define RECT_WIDTH(RC)									((RC).right - (RC).left)
#define RECT_HEIGHT(RC)									((RC).bottom - (RC).top)
#define CLAMP(x, min, max)								((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define MIN(A, B)										((A) < (B) ? (A) : (B))
#define MAX(A, B)										((A) > (B) ? (A) : (B))
#define ABS(x)											((x) < 0 ? -(x) : (x))

#define SET_RECT(rect, l, t, r, b) \
	rect.left = l; \
	rect.top = t; \
	rect.right = r; \
	rect.bottom = b;

#define CSM_RGB(r,g,b)									((r) | ((g) << 8) | ((b) << 16) | (0xFF << 24))
#define CSM_RGBA(r,g,b,a)								((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))
#define CSM_GETRED(x)									(unsigned char)((x) & 0xFF)
#define CSM_GETGREEN(x)									(unsigned char)(((x) >> 8) & 0xFF)
#define CSM_GETBLUE(x)									(unsigned char)(((x) >> 16) & 0xFF)
#define CSM_GETALPHA(x)									(unsigned char)(((x) >> 24) & 0xFF)

#define CSM_Error(FORMAT, ...) CSM_Log("[" __FILE__ " @ %d] ERROR: " __FUNCTION__ "() - " FORMAT, __LINE__, __VA_ARGS__);

#include <CSMAPI_begincode.h>
extern CAVESTORY_MOD_API int WINDOW_REAL_WIDTH;
extern CAVESTORY_MOD_API int WINDOW_REAL_HEIGHT;
#include <CSMAPI_endcode.h>