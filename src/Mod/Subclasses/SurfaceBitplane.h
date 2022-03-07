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


/// @ingroup Collision
/// @{

/// The flags responsible for identifying what information a bitplane object holds about a surface.
///
enum SurfaceBitplaneFlags
{
	/// This bitplane contains a calculated bounding box.
	SURF_BITPLANE_HAS_BOUNDS   = 0x0001,

	/// This bitplane contains a calculated collision bitplane.
	SURF_BITPLANE_HAS_BITPLANE = 0x0002,

	/// This bitplane contains everything listed above.
	SURF_BITPLANE_HAS_ALL      = 0xFFFF
};

/// @brief A calculated collision bitplane for a surface.
///
struct SURFACE_BITPLANE
{
	/// The surface ID that this bitplane uses.
	///
	Surface_Ids SurfId;

	/// The source rect that was used during the calculation of this bitplane.
	///
	GUI_RECT SourceRect;

	/// The flags for this bitplane.
	///
	/// @see SurfaceBitplaneFlags
	unsigned int Flags;

	/// The calculated bounding box for this rect.
	///
	GUI_RECT Bounds;

	/// The calculated bitplane for this rect.
	///
	/// @note The width and height of this array are stored in 'SourceRect'.
	/// @note Bits that equal '1' in this plane mean the pixel is solid. '0' means that the pixel has no collision.
	char* Plane;
};

/// @brief Initialize a bitplane object.
///
/// @param pPlane The plane to initialize.
extern CAVESTORY_MOD_API void CSM_SurfaceBitplane_Init(
	SURFACE_BITPLANE* pPlane
);

/// @brief Free an allocated bitplane.
///
/// @param pPlane The plane to free from memory.
extern CAVESTORY_MOD_API void CSM_SurfaceBitplane_Free(
	SURFACE_BITPLANE* pPlane
);

/// @brief Create a surface bitplane.
///
/// @param iSurfId The surface ID to scan.
/// @param pRect The rect to scan. Can be NULL to scan the entire surface.
/// @param pPlane The bitplane object to be populated with information.
/// @param iFlags The flags to use when creating the bitplane.
/// @param iAlphaThreshold The alpha value that pixel colors have to be equal to or less than to be considered a '0' in the bitplane.
///
/// @return Returns TRUE on success, FALSE on failure.
/// @warn Must initialize the bitplane before using it here with CSM_SurfaceBitplane_Init()!
extern CAVESTORY_MOD_API BOOL CSM_SurfaceBitplane_Scan(
	Surface_Ids iSurfId,
	GUI_RECT* pRect,
	SURFACE_BITPLANE* pPlane,
	unsigned int iFlags = SurfaceBitplaneFlags::SURF_BITPLANE_HAS_ALL,
	unsigned char iAlphaThreshold = 10
);

/// @brief Check to see if a pixel is collidable.
///
/// @param pPlane The plane to use.
/// @param iX The pixel's X position.
/// @param iY The pixel's Y position.
///
/// @return Returns TRUE if the pixel is collidable, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_SurfaceBitplane_IsPixelSolid(
	SURFACE_BITPLANE* pPlane,
	int iX,
	int iY
);

/// @}
///

#include <CSMAPI_endcode.h>