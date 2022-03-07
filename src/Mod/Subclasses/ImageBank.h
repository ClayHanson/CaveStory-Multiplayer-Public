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
  @file ImageBank.h
*/

#ifndef CAVESTORY_MOD_API_H_
#error "Please include CavestoryModAPI.h before including sub-classes."
#endif

#include <CSMAPI_begincode.h>

#pragma once

/// @ingroup ImageBank
/// @{
///

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// Stores information about a specific tile.
///
struct CSM_BANK_TILE
{
	/// An array of indices pointing towards specific solid pixels.
	///
	unsigned int* solid_pixel_list;

	/// A bitmap for testing the solidity of pixels.
	///
	unsigned char* solid_pixel_map;

	/// The number of solid pixels held in the solid_pixels array.
	///
	unsigned short solid_count;

	/// Done boolean
	///
	bool done;
};

/// Stores data about a banked image.
///
struct CSM_BANK_IMAGE
{
	/// Stores the file name's CRC. Do not change this.
	///
	unsigned int file_name_crc;

	/// The folder index inside of the 'Images' folder.
	///
	int bank_no;

	/// The name of the image inside of the bank folder.
	///
	int entry_no;

	/// The internal name of this image.
	///
	char* name;

	/// The image size.
	///
	GUI_POINT img_size;

	/// The size of each tile in pixels.
	///
	GUI_POINT tile_size;

	/// The number of rows and columns this bank has.
	///
	GUI_POINT tile_count;

	/// The total number of tiles in this image.
	///
	int tile_total_count;

	/// The surface ID.
	///
	Surface_Ids surf_id;

	/// The tile list.
	///
	CSM_BANK_TILE* tile_list;

	/// The bank image.
	///
	CSM_BANK_IMAGE* next;

	/// A function that will determine if a pixel is solid or not.
	///
	BOOL(*solid_func)(Surface_Ids surf_id, int x, int y);
};

/// A more useful RECT struct.
///
struct CSM_BANK_RECT
{
	/// ...
	///
	int left;

	/// ...
	///
	int top;

	/// ...
	///
	int right;

	/// ...
	///
	int bottom;

	/// ...
	///
	int width;

	/// ...
	///
	int height;

	/// ...
	///
	int center_x;

	/// ...
	///
	int center_y;
};

/// A structure to hold all collision information.
///
struct CSM_BANK_COLLISION_INFO
{
	/// The bank being tested for.
	///
	CSM_BANK_IMAGE* bank;

	/// The object being tested against.
	///
	void* object;

	/// The total number of collisions so far.
	///
	int pass_count;

	/// The tile index used for collision testing.
	///
	int tile_index_0;

	/// The tile index used for collision testing for object.
	///
	int tile_index_1;

	/// The map tile position where the collision took place.
	///
	GUI_POINT map_tile_pos;

	// METADATA //

	/// The global subpixel position of the pixel that was collided against.
	///
	GUI_POINT bank_hit_pixel_subpos;

	/// The object's subpixel rect.
	///
	CSM_BANK_RECT bank_subrect;

	/// The subpixel position of the object pixel that was collided against.
	///
	GUI_POINT object_hit_pixel_subpos;

	/// The object's subpixel rect.
	///
	CSM_BANK_RECT object_subrect;

	/// Old object xm
	///
	int object_old_xm;
	
	/// Old object ym
	///
	int object_old_ym;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Load a banked image.
///
/// The file location will be built as '<InternalModName>/<iBankNo>/<iEntryNo>.png'. For example, calling CSM_ImageBank_Load(1, 4, &pOut); will load 'mymod/0001/0004.png'.
///
/// @param pInternalName The internal name of this bank image. Can be NULL.
/// @param iBankNo The bank number.
/// @param iEntryNo The image entry number.
/// @param pOut A pointer to hold the generated bank image object.
/// @param iTileSizeW The initial tile width for this bank.
/// @param iTileSizeH The initial tile height for this bank.
/// @param pSolidFunc Pointer to a function that will determine if a pixel is solid or not. Can be NULL.
///
/// @return Returns 0 on success, non-zero on error.
/// @see CSM_ImageBank_Free
extern CAVESTORY_MOD_API int CSM_ImageBank_Load(
	int iBankNo,
	int iEntryNo,
	CSM_BANK_IMAGE** pOut = NULL,
	const char* pInternalName = NULL,
	int iTileSizeW = 16,
	int iTileSizeH = 16,
	BOOL(*pSolidFunc)(Surface_Ids surf_id, int x, int y) = NULL,
	bool* bAlreadyExists = NULL
);

/// @brief Load a banked image.
///
/// The file location will be built as '<InternalModName>/<iBankNo>/<iEntryNo>.png'. For example, calling CSM_ImageBank_Load(1, 4, &pOut); will load 'mymod/0001/0004.png'.
///
/// @param pInternalName The internal name of this bank image. Can be NULL.
/// @param iBankNo The bank number.
/// @param iEntryNo The image entry number.
/// @param pOut A pointer to hold the generated bank image object.
/// @param iTileSizeW The initial tile width for this bank.
/// @param iTileSizeH The initial tile height for this bank.
/// @param pSolidFunc Pointer to a function that will determine if a pixel is solid or not. Can be NULL.
///
/// @return Returns the surface ID on success, SURFACE_ID_MAX on fail.
/// @see CSM_ImageBank_Free
extern CAVESTORY_MOD_API Surface_Ids CSM_ImageBank_LoadAndGetSurfaceId(
	int iBankNo,
	int iEntryNo,
	CSM_BANK_IMAGE** pOut = NULL,
	const char* pInternalName = NULL,
	int iTileSizeW = 16,
	int iTileSizeH = 16,
	BOOL(*pSolidFunc)(Surface_Ids surf_id, int x, int y) = NULL
);

/// @brief Free a loaded bank image.
///
/// @param pBank The bank object.
///
/// @see CSM_ImageBank_Load
extern CAVESTORY_MOD_API void CSM_ImageBank_Free(
	CSM_BANK_IMAGE* pBank
);

/// @brief Free all bank images.
///
extern CAVESTORY_MOD_API void CSM_ImageBank_FreeAll(
);

/// @brief Get a bank by its surface ID.
///
/// @param iSurfId The surface ID to match.
/// @param pOut A pointer to hold the result. Can be NULL.
///
/// @return Returns TRUE if the bank was found, FALSE otherwise.
/// @see CSM_ImageBank_Free
extern CAVESTORY_MOD_API BOOL CSM_ImageBank_FindBySurfaceId(
	Surface_Ids iSurfId,
	CSM_BANK_IMAGE** pOut = NULL
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Check if a pixel is solid.
///
/// @param pBank The bank object.
/// @param iX The X position.
/// @param iY The Y position.
extern CAVESTORY_MOD_API void CSM_ImageBank_SetTileSize(
	CSM_BANK_IMAGE* pBank,
	int iTileW,
	int iTileH
);

/// @brief Get the tile index of a bank at the given tile coordinates.
///
/// @param pBank The bank object.
/// @param iTileX The tile's X position.
/// @param iTileY The tile's Y position.
///
/// @return Returns the tile's index.
extern CAVESTORY_MOD_API int CSM_ImageBank_GetTileIndex(
	CSM_BANK_IMAGE* pBank,
	int iTileX,
	int iTileY
);

/// @brief Get the tile index of a bank at the given pixel coordinates.
///
/// @param pBank The bank object.
/// @param iPixelX The pixel's X position.
/// @param iPixelY The pixel's Y position.
///
/// @return Returns the tile's index.
extern CAVESTORY_MOD_API int CSM_ImageBank_GetTileIndexByPixel(
	CSM_BANK_IMAGE* pBank,
	int iPixelX,
	int iPixelY
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Scan a tile for solid pixels.
///
/// @param pBank The bank object.
/// @param iTileIndex The tile index.
/// @param bForce Force this function to rescan the tile if it's already scanned.
///
/// @return Returns TRUE if the scan was successful / the tile was already scanned, otherwise returns FALSE on fail.
extern CAVESTORY_MOD_API BOOL CSM_ImageBank_ScanTile(
	CSM_BANK_IMAGE* pBank,
	int iTileIndex,
	bool bForce = false
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Check if a pixel is solid.
///
/// @param pBank The bank object.
/// @param iX The X position. This is not magnified & not sub-pixel. Must be relative to (0, 0).
/// @param iY The Y position. This is not magnified & not sub-pixel. Must be relative to (0, 0).
/// @param fAngle The angle, in degrees, to rotate this by.
/// @param iCenterX The pivot X position.
/// @param iCenterY The pivot Y position.
///
/// @return Returns TRUE if the pixel is solid, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_ImageBank_IsPixelSolid(
	CSM_BANK_IMAGE* pBank,
	int iTileIndex,
	int iX,
	int iY,
	float fAngle = 0.f,
	int iCenterX = 0,
	int iCenterY = 0
);

/// @brief Scan down until we hit a solid pixel.
///
/// @param pBank The bank object.
/// @param iX The X position. This is not magnified & not sub-pixel. Must be relative to (0, 0).
/// @param iY The Y position. This is not magnified & not sub-pixel. Must be relative to (0, 0).
/// @param fAngle The angle, in degrees, to rotate this by.
/// @param iCenterX The pivot X position.
/// @param iCenterY The pivot Y position.
///
/// @return Returns the Y position of the best floor pixel (non-magnified & non-subpixel), -1 if none could be found.
extern CAVESTORY_MOD_API int CSM_ImageBank_GetFloorPosition(
	CSM_BANK_IMAGE* pBank,
	int iTileIndex,
	int iX,
	int iY,
	float fAngle = 0.f,
	int iCenterX = 0,
	int iCenterY = 0,
	GUI_POINT debug_bank_sub_pos = GUI_POINT(0, 0)
);

/// @brief Check if a pixel is overlapping a player character.
///
/// @param pBank The bank object.
/// @param pMC The player character to test against.
/// @param iTileIndex The tile index.
/// @param pBankPos The bank's subpixel position.
/// @param pCollideFunc The function to be called when a collision is found. The return value for this callback should be the collision flags applied.
/// @param fAngle The angle, in degrees, to rotate this by.
/// @param iCenterX The pivot X position.
/// @param iCenterY The pivot Y position.
/// @param iDebugLevel The debug level.
///
/// @return Returns the collision flags that were applied during the collision check.
extern CAVESTORY_MOD_API int CSM_ImageBank_TestPixelCollision_MYCHAR(
	CSM_BANK_IMAGE* pBank,
	MYCHAR* pMC,
	int iTileIndex,
	GUI_POINT pBankPos,
	int(*pCollideFunc)(const CSM_BANK_COLLISION_INFO* pInfo),
	float fAngle = 0.f,
	int iCenterX = 0,
	int iCenterY = 0,
	char iDebugLevel = 0
);

/// @brief Check if a pixel is overlapping an NPC.
///
/// @param pBank The bank object.
/// @param pNpc The NPC object to test against.
/// @param iTileIndex The tile index.
/// @param pBankPos The bank's subpixel position.
/// @param pCollideFunc The function to be called when a collision is found.
/// @param fAngle The angle, in degrees, to rotate this by.
/// @param iCenterX The pivot X position.
/// @param iCenterY The pivot Y position.
/// @param iDebugLevel The debug level.
///
/// @return Returns TRUE if a collision was caught, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_ImageBank_TestPixelCollision_NPCHAR(
	CSM_BANK_IMAGE* pBank,
	NPCHAR* pNpc,
	int iTileIndex,
	GUI_POINT pBankPos,
	int(*pCollideFunc)(const CSM_BANK_COLLISION_INFO* pInfo),
	float fAngle = 0.f,
	int iCenterX = 0,
	int iCenterY = 0,
	char iDebugLevel = 0
);

/// @brief Check if a pixel is overlapping an NPC.
///
/// @param pBank The bank object.
/// @param pOther The bank object being test against.
/// @param iTileIndex1 The first bank's tile index.
/// @param iTileIndex2 The second bank's tile index.
/// @param pBankPos1 The first bank's subpixel position.
/// @param pBankPos2 The second bank's subpixel position.
/// @param pCollideFunc The function to be called when a collision is found.
/// @param fAngle The angle, in degrees, to rotate this by.
/// @param iCenterX The pivot X position.
/// @param iCenterY The pivot Y position.
/// @param iDebugLevel The debug level.
///
/// @return Returns TRUE if a collision was caught, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_ImageBank_TestPixelCollision_BANK(
	CSM_BANK_IMAGE* pBank,
	CSM_BANK_IMAGE* pOther,
	int iTileIndex1,
	int iTileIndex2,
	GUI_POINT pBankPos1,
	GUI_POINT pBankPos2,
	void(*pCollideFunc)(const CSM_BANK_COLLISION_INFO* pInfo),
	float fAngle = 0.f,
	int iCenterX = 0,
	int iCenterY = 0,
	char iDebugLevel = 0
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

extern CAVESTORY_MOD_API CSM_BANK_IMAGE* sz_pFirstImageBank;

#include <CSMAPI_endcode.h>

/// @}
///