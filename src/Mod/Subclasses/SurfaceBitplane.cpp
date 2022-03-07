#include <CavestoryModAPI.h>
#include <Subclasses/SurfaceBitplane.h>
#include "Draw.h"

#include <CSMAPI_begincode.h>

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSM_SurfaceBitplane_Init(SURFACE_BITPLANE* pPlane)
{
	memset(pPlane, 0, sizeof(pPlane));

	pPlane->SurfId = SURFACE_ID_MAX;
}

void CSM_SurfaceBitplane_Free(SURFACE_BITPLANE* pPlane)
{
	if ((pPlane->Flags & SurfaceBitplaneFlags::SURF_BITPLANE_HAS_BITPLANE) && pPlane->Plane)
	{
		free(pPlane->Plane);
		pPlane->Plane = NULL;
	}
}

BOOL CSM_SurfaceBitplane_Scan(Surface_Ids iSurfId, GUI_RECT* pRect, SURFACE_BITPLANE* pPlane, unsigned int iFlags, unsigned char iAlphaThreshold)
{
	if (!IsSurfaceAllocated(iSurfId))
	{
		printf("Cannot scan unallocated surface %d!\r\n", iSurfId);
		return FALSE;
	}

	// Initialize the bitplane
	pPlane->SurfId     = iSurfId;
	pPlane->SourceRect = (pRect ? *pRect : GUI_RECT(0, 0, CacheSurface::GetSurfaceIDSize(iSurfId)));
	pPlane->Flags      = iFlags;
	pPlane->Bounds     = GUI_RECT(0, 0, 0, 0);
	pPlane->Plane      = NULL;

	// Useful variables
	int iX         = pPlane->SourceRect.x;
	int iY         = pPlane->SourceRect.y;
	int iW         = pPlane->SourceRect.w;
	int iH         = pPlane->SourceRect.h;
	int iPlaneSize = ((iW * iH) + 7) >> 3;
	int iPixelBit  = 0;
	struct
	{
		int left;
		int top;
		int right;
		int bottom;
	} pBounds = { -1, -1, -1, -1 };

	// Cache variables for defining the features of this bitplane
	bool bBitplane = (iFlags & SurfaceBitplaneFlags::SURF_BITPLANE_HAS_BITPLANE);
	bool bBounds   = (iFlags & SurfaceBitplaneFlags::SURF_BITPLANE_HAS_BOUNDS);

	// If the bitplane has no features, then stop here.
	if (!bBitplane && !bBounds)
		return TRUE;

	// Create the bitplane
	if (bBitplane)
	{
		// Allocate the plane buffer
		pPlane->Plane = (char*)malloc(iPlaneSize);

		// Reset the memory at the buffer's location
		memset(pPlane->Plane, 0, iPlaneSize);
	}

	// Scan the image
	for (int y = 0; y < pPlane->SourceRect.h; y++)
	{
		for (int x = 0; x < pPlane->SourceRect.w; x++)
		{
			// Skip pixels that do not exceed the threshold
			if (CacheSurface::GetPixelColor(GUI_POINT(iX + x, iY + y), iSurfId).a <= iAlphaThreshold)
				continue;

			if (bBitplane)
			{
				// Got a transparent pixel here! Calculate the bit's position
				iPixelBit = (y * iW) + x;

				// Set the bit
				pPlane->Plane[iPixelBit / 8] |= (1 << (iPixelBit % 8));
			}

			if (bBounds)
			{
				// Calculate the bounding box
				if (pBounds.left == -1 || x < pBounds.left)
					pBounds.left = x;
				
				if (pBounds.top == -1 || y < pBounds.top)
					pBounds.top = y;
				
				if (pBounds.right == -1 || x + 1 > pBounds.right)
					pBounds.right = x + 1;
				
				if (pBounds.bottom == -1 || y + 1 > pBounds.bottom)
					pBounds.bottom = y + 1;
			}
		}
	}

	// Final step to calculate the bounding box
	if (bBounds)
	{
		pPlane->Bounds.x = MAX(pBounds.left, 0);
		pPlane->Bounds.y = MAX(pBounds.top, 0);
		pPlane->Bounds.w = MAX(pBounds.right - pBounds.left, 0);
		pPlane->Bounds.h = MAX(pBounds.bottom - pBounds.top, 0);
	}

	return TRUE;
}

BOOL CSM_SurfaceBitplane_IsPixelSolid(SURFACE_BITPLANE* pPlane, int iX, int iY)
{
	if (!(pPlane->Flags & SurfaceBitplaneFlags::SURF_BITPLANE_HAS_BITPLANE) || !pPlane->Plane || iX < 0 || iY < 0 || iX >= pPlane->SourceRect.w || iY >= pPlane->SourceRect.h)
		return FALSE;

	int iPixelPosition = (iY * pPlane->SourceRect.w) + iX;

	return (pPlane->Plane[iPixelPosition / 8] & (1 << (iPixelPosition % 8))) ? TRUE : FALSE;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_endcode.h>