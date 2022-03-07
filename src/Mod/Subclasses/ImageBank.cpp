#include <CavestoryModAPI.h>
#include <Subclasses/ImageBank.h>

#include "../../CustomPlayer.h"
#include "Main.h"
#include "CRC32.h"
#include "Generic.h"
#include "VideoTexture.h"

#include <CSMAPI_begincode.h>

CAVESTORY_MOD_API CSM_BANK_IMAGE* sz_pFirstImageBank = NULL;
unsigned long long int sz_iImageBankDebugCurrFrame   = 0;
unsigned long long int sz_iImageBankDebugFreeFrame   = 0;
VideoTexture* sz_pImageBankVideoTexture              = NULL;
GUI_POINT* sz_ImageBank_Debug_pValidPixelList        = NULL;
GUI_POINT* sz_ImageBank_Debug_pInvalidPixelList      = NULL;
unsigned int sz_ImageBank_Debug_iValidPixelCount     = 0;
unsigned int sz_ImageBank_Debug_iInvalidPixelCount   = 0;
unsigned int sz_ImageBank_Debug_iPixelListCapacity   = 0;
extern unsigned long long int gGameFrameCount;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define SET_BIT(ARRAY, IDX)					ARRAY[(IDX) >> 3] |= (1 << ((IDX) & 0x7));
#define CLEAR_BIT(ARRAY, IDX)				ARRAY[(IDX) >> 3] &= ~(1 << ((IDX) & 0x7));
#define TEST_BIT(ARRAY, IDX)				((ARRAY[(IDX) >> 3] & (1 << ((IDX) & 0x7))) != 0)
#define BANK_TILE_XY(BANK, X, Y)			((Y * BANK->tile_size.x) + X)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

inline int SubpixelToPixel(int iPixel)
{
	return iPixel / 0x200;
}

inline int FastMin(int a, int b, int c, int d)
{
	a = (a < b ? a : b);
	c = (c < d ? c : d);

	return (a < c ? a : c);
}

inline int FastMax(int a, int b, int c, int d)
{
	a = (a > b ? a : b);
	c = (c > d ? c : d);

	return (a > c ? a : c);
}

BOOL CSM_INTERNAL_ImageBank_DefaultCompareFunc(Surface_Ids surf_id, int x, int y)
{
	return CacheSurface::GetPixelColor(GUI_POINT(x, y), surf_id).a >= 127;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSM_INTERNAL_ImageBank_CheckDebugSurface(CSM_BANK_IMAGE* pBank, char iDebugLevel)
{
	if (!sz_pImageBankVideoTexture && !iDebugLevel)
		return;

	if (sz_iImageBankDebugFreeFrame != 0 && sz_iImageBankDebugFreeFrame == gGameFrameCount)
	{
		// Free it
		if (sz_pImageBankVideoTexture)
			VideoTexture::FreeTexture(sz_pImageBankVideoTexture);

		if (sz_ImageBank_Debug_pInvalidPixelList)
		{
			free(sz_ImageBank_Debug_pInvalidPixelList);
			sz_ImageBank_Debug_pInvalidPixelList = NULL;
		}
		
		if (sz_ImageBank_Debug_pValidPixelList)
		{
			free(sz_ImageBank_Debug_pValidPixelList);
			sz_ImageBank_Debug_pValidPixelList = NULL;
		}

		sz_ImageBank_Debug_iPixelListCapacity = 0;
		sz_pImageBankVideoTexture             = NULL;
		sz_iImageBankDebugFreeFrame           = 0;

		return;
	}

	if (sz_pImageBankVideoTexture && !iDebugLevel)
	{
		// Free this NEXT frame, so that if any other calls to this function happens in the current frame w/ a debug level of non-0, we don't have to reallocate it again
		sz_iImageBankDebugFreeFrame = gGameFrameCount + 4;

		return;
	}
	
	// Allocate it
	if (!sz_pImageBankVideoTexture)
		sz_pImageBankVideoTexture = VideoTexture::CreateTexture(WINDOW_WIDTH, WINDOW_HEIGHT);

	if ((pBank->tile_size.x * pBank->tile_size.y) * (sz_pImageBankVideoTexture->m_iMagnification * sz_pImageBankVideoTexture->m_iMagnification) > sz_ImageBank_Debug_iPixelListCapacity)
	{
		sz_ImageBank_Debug_iPixelListCapacity = (pBank->tile_size.x * pBank->tile_size.y) * (sz_pImageBankVideoTexture->m_iMagnification * sz_pImageBankVideoTexture->m_iMagnification);

		if (sz_ImageBank_Debug_pInvalidPixelList)
			sz_ImageBank_Debug_pInvalidPixelList = (GUI_POINT*)realloc((void*)sz_ImageBank_Debug_pInvalidPixelList, sizeof(GUI_POINT) * sz_ImageBank_Debug_iPixelListCapacity);
		else
			sz_ImageBank_Debug_pInvalidPixelList = (GUI_POINT*)malloc(sizeof(GUI_POINT) * sz_ImageBank_Debug_iPixelListCapacity);
		
		if (sz_ImageBank_Debug_pValidPixelList)
			sz_ImageBank_Debug_pValidPixelList = (GUI_POINT*)realloc((void*)sz_ImageBank_Debug_pValidPixelList, sizeof(GUI_POINT) * sz_ImageBank_Debug_iPixelListCapacity);
		else
			sz_ImageBank_Debug_pValidPixelList = (GUI_POINT*)malloc(sizeof(GUI_POINT) * sz_ImageBank_Debug_iPixelListCapacity);
	}

	// Reset this variable so that we're NOT freeing next frame.
	sz_iImageBankDebugFreeFrame = 0;

	if (sz_iImageBankDebugCurrFrame != gGameFrameCount)
	{
		sz_iImageBankDebugCurrFrame = gGameFrameCount;

		// Clear the surface
		sz_pImageBankVideoTexture->Lock();
		sz_pImageBankVideoTexture->ClearTexture(NULL);
		sz_pImageBankVideoTexture->Unlock();
	}

	sz_ImageBank_Debug_iInvalidPixelCount = 0;
	sz_ImageBank_Debug_iValidPixelCount   = 0;
}

void CSM_INTERNAL_ImageBank_RenderDebugSurface()
{
	if (!sz_pImageBankVideoTexture)
		return;

	sz_pImageBankVideoTexture->Put(0, 0);
}

inline CSM_BANK_RECT CSM_INTERNAL_ImageBank_CreateRect(int left, int top, int right, int bottom)
{
	CSM_BANK_RECT rc = { left, top, right, bottom, right - left, bottom - top, left + ((right - left) / 2), top + ((bottom - top) / 2) };

	return rc;
}

inline void CSM_INTERNAL_ImageBank_GetEntryName(int iBankNo, int iEntryNo, char* pOut, int iOutSize)
{
	sprintf_s(pOut, iOutSize, "%s/Image/%04d/%04d.png", gCaveMod.mModPath, iBankNo, iEntryNo);
}

unsigned int CSM_INTERNAL_ImageBank_GetEntryNameCRC(int iBankNo, int iEntryNo)
{
	// Get the name of the bank
	char pBuffer[260];
	CSM_INTERNAL_ImageBank_GetEntryName(iBankNo, iEntryNo, pBuffer, sizeof(pBuffer));

	// Get the CRC32 checksum
	return StringCRC32(pBuffer, strlen(pBuffer));
}

CSM_BANK_IMAGE* CSM_INTERNAL_ImageBank_GetEntryByCRC(int iFileCRC, char iOffset)
{
	for (CSM_BANK_IMAGE* walk = sz_pFirstImageBank; walk; walk = walk->next)
	{
		if (iOffset < 0)
		{
			// Invalid; Couldn't offset by that many entries behind our target entry!
			if (walk->file_name_crc == iFileCRC)
				return NULL;

			int iIndex = 0;
			for (CSM_BANK_IMAGE* walk2 = walk; walk2; walk2++)
			{
				// Increment the index
				iIndex++;

				// If it's more than the offset we want, then stop here; This entry isn't the one we want
				if (iIndex > -iOffset)
					break;
				else if (iIndex == -iOffset && walk2->next && walk2->next->file_name_crc == iFileCRC)
					return walk;
			}
		}
		else if (iOffset > 0)
		{
			// We want an entry in front of the desired entry
			if (walk->file_name_crc != iFileCRC)
				continue;

			for (int j = 0; j < iOffset; j++)
			{
				walk = walk->next;

				if (!walk)
					return NULL;
			}

			return walk;
		}
		else if (walk->file_name_crc == iFileCRC)
			return walk;
	}

	return NULL;
}

#if 1
void CSM_INTERNAL_BankImage_GetDebugName(CSM_BANK_IMAGE* pBank, char* pOut)
{
		sprintf(pOut, "%04X", pBank->file_name_crc);
}

void CSM_INTERNAL_BankImage_ShowDebugPos(CSM_BANK_IMAGE* pBank, const char* pLabel)
{
	char pName[512];

	if (!pBank)
	{
		if (pLabel)
			printf("[%s] <NULL>\r\n", pLabel);
		else
			printf("<NULL>\r\n");

		return;
	}

	if (pLabel)
		printf("[%s] Image Bank @ 0x%08X: [", pLabel, (unsigned int)pBank);
	else
		printf("Image Bank @ 0x%08X: [", (unsigned int)pBank);

	for (CSM_BANK_IMAGE* walk = sz_pFirstImageBank; walk; walk = walk->next)
	{
		CSM_INTERNAL_BankImage_GetDebugName(walk, pName);

		if (walk == pBank)
			printf(">%s<%s", pName, (walk->next ? "|" : "]"));
		else
			printf(" %s %s", pName, (walk->next ? "|" : "]"));
	}

	if (pLabel)
		sprintf(pName, "[%s] Image Bank @ 0x%08X: ", pLabel, (unsigned int)pBank);
	else
		sprintf(pName, "Image Bank @ 0x%08X: ", (unsigned int)pBank);

	printf("\r\n");

	int iLen = strlen(pName);
	for (int i = 0; i < iLen; i++)
		printf(" ");

	printf("[");

	int iIndex = 0;
	for (CSM_BANK_IMAGE* walk = sz_pFirstImageBank; walk; walk = walk->next)
		printf("   %04d   %s", iIndex++, (walk->next ? "|" : ""));

	printf("]\r\n");
}
#endif

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int CSM_ImageBank_Load(int iBankNo, int iEntryNo, CSM_BANK_IMAGE** pOut, const char* pInternalName, int iTileSizeW, int iTileSizeH, BOOL(*pSolidFunc)(Surface_Ids surf_id, int x, int y), bool* bAlreadyExists)
{
	iBankNo  = CLAMP(iBankNo, 0, 9999);
	iEntryNo = CLAMP(iEntryNo, 0, 9999);

	char pFileName[260];
	unsigned int iCrc     = CSM_INTERNAL_ImageBank_GetEntryNameCRC(iBankNo, iEntryNo);
	CSM_BANK_IMAGE* pBank = NULL;
	Surface_Ids iSurfId   = SURFACE_ID_MAX;

	// Attempt to find a match first
	for (CSM_BANK_IMAGE* walk = sz_pFirstImageBank; walk; walk = walk->next)
	{
		if (walk->file_name_crc != iCrc)
			continue;

		if (pOut)
			*pOut = walk;

		if (bAlreadyExists)
			*bAlreadyExists = true;

		// Already exists!
		return 0;
	}

	// Build the filename
	CSM_INTERNAL_ImageBank_GetEntryName(iBankNo, iEntryNo, pFileName, sizeof(pFileName));

	// Check if it exists
	if (!CheckFileExists2(pFileName))
		return 2; //< File does not exist!

	// Allocate a new surface
	if ((iSurfId = CacheSurface::GetNextFreeSurfaceID()) == SURFACE_ID_MAX)
		return 3; //< Ran out of allocatable surface slots!
				  
	// Load the file
	if (!CacheSurface::LoadFileToSurface(pFileName, iSurfId))
		return 4; //< Failed to load the image!

	// Allocate & configure the new bank object
	CSM_BANK_IMAGE* pNewBank   = new CSM_BANK_IMAGE();
	pNewBank->file_name_crc    = iCrc;
	pNewBank->bank_no          = iBankNo;
	pNewBank->entry_no         = iEntryNo;
	pNewBank->name             = NULL;
	pNewBank->img_size         = CacheSurface::GetSurfaceIDSize(iSurfId);
	pNewBank->tile_size        = GUI_POINT(-1, -1);
	pNewBank->tile_total_count = 0;
	pNewBank->tile_count       = GUI_POINT(0, 0);
	pNewBank->tile_list        = NULL;
	pNewBank->surf_id          = iSurfId;
	pNewBank->solid_func       = (!pSolidFunc ? CSM_INTERNAL_ImageBank_DefaultCompareFunc : pSolidFunc);

	CacheSurface::SetSurfaceID(SURFACE_ID_TITLE);

	if (iTileSizeW == -1)
		iTileSizeW = CacheSurface::GetSurfaceIDSize(iSurfId).x;
	
	if (iTileSizeH == -1)
		iTileSizeH = CacheSurface::GetSurfaceIDSize(iSurfId).y;

	// Create the name
	if (pInternalName)
		pNewBank->name = strdup(pInternalName);
	else
		pNewBank->name = NULL;

	// Link this bank
	pNewBank->next     = sz_pFirstImageBank;
	sz_pFirstImageBank = pNewBank;

	// Setup the tile size
	CSM_ImageBank_SetTileSize(pNewBank, iTileSizeW, iTileSizeH);

	// Set the out variable
	if (pOut)
		*pOut = pNewBank;

	if (bAlreadyExists)
		*bAlreadyExists = false;

	// Done
	return 0;
}

Surface_Ids CSM_ImageBank_LoadAndGetSurfaceId(int iBankNo, int iEntryNo, CSM_BANK_IMAGE** pOut, const char* pInternalName, int iTileSizeW, int iTileSizeH, BOOL(*pSolidFunc)(Surface_Ids surf_id, int x, int y))
{
	CSM_BANK_IMAGE* pBank = NULL;

	if (CSM_ImageBank_Load(iBankNo, iEntryNo, &pBank, pInternalName, iTileSizeW, iTileSizeH, pSolidFunc))
		return SURFACE_ID_MAX;
	
	if (pOut)
		*pOut = pBank;

	return pBank->surf_id;
}

void CSM_ImageBank_Free(CSM_BANK_IMAGE* pBank)
{
	if (!pBank)
		return;

	int iStringCrc = CSM_INTERNAL_ImageBank_GetEntryNameCRC(pBank->bank_no, pBank->entry_no);

	// Find the entry that was behind this one
	CSM_BANK_IMAGE* pPrevBank = CSM_INTERNAL_ImageBank_GetEntryByCRC(pBank->file_name_crc, -1);

	if (!pPrevBank && sz_pFirstImageBank == pBank)
		sz_pFirstImageBank = pBank->next;
	else if (pPrevBank)
		pPrevBank->next = pBank->next;

	// Free this bank's memory
	if (pBank->tile_list)
	{
		for (int i = 0; i < pBank->tile_total_count; i++)
		{
			if (pBank->tile_list[i].solid_pixel_list)
				free(pBank->tile_list[i].solid_pixel_list);
			if (pBank->tile_list[i].solid_pixel_map)
				free(pBank->tile_list[i].solid_pixel_map);
		}

		free(pBank->tile_list);
	}

	if (pBank->name)
		free(pBank->name);

	CacheSurface::FreeSurfaceID(pBank->surf_id);
	delete pBank;
}

void CSM_ImageBank_FreeAll()
{
	while (sz_pFirstImageBank)
	{
		for (CSM_BANK_IMAGE* walk = sz_pFirstImageBank; walk; walk = walk->next)
		{
			if (walk->next)
				continue;

			CSM_ImageBank_Free(walk);
			break;
		}
	}
}

BOOL CSM_ImageBank_FindBySurfaceId(Surface_Ids iSurfId, CSM_BANK_IMAGE** pOut)
{
	for (CSM_BANK_IMAGE* walk = sz_pFirstImageBank; walk; walk = walk->next)
	{
		if (walk->surf_id != iSurfId)
			continue;

		if (pOut)
			*pOut = walk;

		return TRUE;
	}

	return FALSE;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSM_ImageBank_SetTileSize(CSM_BANK_IMAGE* pBank, int iTileSizeX, int iTileSizeY)
{
	// Free the old memory
	if (pBank->tile_list)
	{
		for (int i = 0; i < pBank->tile_total_count; i++)
		{
			if (pBank->tile_list[i].solid_pixel_list)
				free(pBank->tile_list[i].solid_pixel_list);
			if (pBank->tile_list[i].solid_pixel_map)
				free(pBank->tile_list[i].solid_pixel_map);
		}

		free(pBank->tile_list);
		pBank->tile_list        = NULL;
		pBank->tile_total_count = 0;
	}

	// Calculate the new metadata
	pBank->tile_size = GUI_POINT(iTileSizeX, iTileSizeY);

	// Do not cause a division by zero error
	if (iTileSizeX <= 0 || iTileSizeY <= 0)
		return;

	pBank->tile_count       = GUI_POINT(pBank->img_size.x / pBank->tile_size.x, pBank->img_size.y / pBank->tile_size.y);
	pBank->tile_total_count = pBank->tile_count.x * pBank->tile_count.y;

	// Do not cause an allocation error
	if (pBank->tile_total_count <= 0)
	{
		pBank->tile_total_count = 0;
		return;
	}

	// Allocate the new list
	pBank->tile_list = (CSM_BANK_TILE*)malloc(sizeof(CSM_BANK_TILE) * pBank->tile_total_count);

	// Reset the memory for every tile
	memset(pBank->tile_list, 0, sizeof(CSM_BANK_TILE) * pBank->tile_total_count);
}

int CSM_ImageBank_GetTileIndex(CSM_BANK_IMAGE* pBank, int iTileX, int iTileY)
{
	if (!pBank || !pBank->tile_list || iTileX < 0 || iTileX >= pBank->tile_count.x || iTileY < 0 || iTileY >= pBank->tile_count.y)
		return -1;

	return (iTileY * pBank->tile_count.x) + iTileX;
}

int CSM_ImageBank_GetTileIndexByPixel(CSM_BANK_IMAGE* pBank, int iPixelX, int iPixelY)
{
	if (!pBank || !pBank->tile_list || iPixelX < 0 || iPixelX >= pBank->img_size.x || iPixelY < 0 || iPixelY >= pBank->img_size.y)
		return -1;

	return ((iPixelY / pBank->tile_size.y) * pBank->tile_count.x) + (iPixelX / pBank->tile_size.x);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static GUI_POINT rotate_point(float cx, float cy, float angle, GUI_POINT p)
{
	angle   = (angle * (M_PI_F / 180.f));
	float s = sin(angle);
	float c = cos(angle);

	// translate point back to origin:
	p.x -= int(cx);
	p.y -= int(cy);

	// rotate point
	float xnew = (float)p.x * c - (float)p.y * s;
	float ynew = (float)p.x * s + (float)p.y * c;

	// translate point back:
	p.x = int(xnew + cx);
	p.y = int(ynew + cy);

	return p;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSM_ImageBank_ScanTile(CSM_BANK_IMAGE* pBank, int iTileIndex, bool bForce)
{
	if (!pBank || !pBank->tile_list || iTileIndex < 0 || iTileIndex >= pBank->tile_total_count)
		return FALSE;

	// Stop if we're not being forced to rescan & the pixelmap already exists
	if (pBank->tile_list[iTileIndex].done && !bForce)
		return TRUE;

	// Free the old memory
	if (pBank->tile_list[iTileIndex].solid_pixel_list)
	{
		free(pBank->tile_list[iTileIndex].solid_pixel_list);
		pBank->tile_list[iTileIndex].solid_pixel_list = NULL;
	}

	// Get the tile we want
	CSM_BANK_TILE* pTile = &pBank->tile_list[iTileIndex];
	int x                = pBank->tile_size.x * (iTileIndex % pBank->tile_count.x);
	int y                = pBank->tile_size.y * (iTileIndex / pBank->tile_count.x);

	// Set this to true regardless of what happens now
	pTile->done = true;

	// Allocate the pixel map first
	pTile->solid_pixel_map = (unsigned char*)malloc(sizeof(unsigned char) * (((pBank->tile_size.x * pBank->tile_size.y) + 7) >> 3));

	// Reset the newly allocated memory
	memset(pTile->solid_pixel_map, 0, sizeof(unsigned char) * (((pBank->tile_size.x * pBank->tile_size.y) + 7) >> 3));

	// Initiate counting procedure
	int iSolidCount = 0;

	// Count the number of solid pixels first
	for (int i = 0; i < pBank->tile_size.x * pBank->tile_size.y; i++)
	{
		if (pBank->solid_func(pBank->surf_id, x + (i % pBank->tile_size.x), y + (i / pBank->tile_size.x)))
		{
			// Increment the number of solids
			++iSolidCount;

			// And set it in the pixelmap
			SET_BIT(pTile->solid_pixel_map, i);
		}
	}
	
	// Allocate the memory
	pTile->solid_pixel_list = (unsigned int*)malloc(sizeof(unsigned int) * iSolidCount);
	pTile->solid_count      = 0;

	// Reset the newly allocated memory
	memset(pTile->solid_pixel_list, 0, sizeof(unsigned int) * iSolidCount);
	
	// Scan the tile
	for (int i = 0; i < pBank->tile_size.x * pBank->tile_size.y; i++)
	{
		// Skip non-solid pixels
		if (!pBank->solid_func(pBank->surf_id, x + (i % pBank->tile_size.x), y + (i / pBank->tile_size.x)))
			continue;

		// Add this pixel index to the list
		pTile->solid_pixel_list[pTile->solid_count++] = i;

		// Make sure we don't go out of bounds.
		if (pTile->solid_count >= iSolidCount)
			break;
	}

	return TRUE;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSM_ImageBank_IsPixelSolid(CSM_BANK_IMAGE* pBank, int iTileIndex, int iX, int iY, float fAngle, int iCenterX, int iCenterY)
{
	if (!CSM_ImageBank_ScanTile(pBank, iTileIndex))
		return FALSE;

	// Rotate the points if necessary
	if (fAngle)
	{
		GUI_POINT pnt = rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(iX * 0x200, iY * 0x200));
		iX            = pnt.x / 0x200;
		iY            = pnt.y / 0x200;
	}

	// Validate that we're still in-bounds
	if (iX < 0 || iX >= pBank->tile_size.x || iY < 0 || iY >= pBank->tile_size.y)
		return FALSE;

	return TEST_BIT(pBank->tile_list[CLAMP(iTileIndex, 0, pBank->tile_total_count - 1)].solid_pixel_map, BANK_TILE_XY(pBank, iX, iY));
}

int CSM_ImageBank_GetFloorPosition(CSM_BANK_IMAGE* pBank, int iTileIndex, int iX, int iY, float fAngle, int iCenterX, int iCenterY, GUI_POINT debug_bank_sub_pos)
{
	if (!CSM_ImageBank_ScanTile(pBank, iTileIndex) || iX < 0 || iX >= pBank->tile_size.x || iY < 0 || iY >= pBank->tile_size.y)
		return FALSE;

	GUI_POINT pnt;

	// Check debug surface
	CSM_INTERNAL_ImageBank_CheckDebugSurface(pBank, 2);

	if (fAngle)
	{
		pnt = rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(iX * 0x200, iY * 0x200)) / 0x200;
		iX  = pnt.x;
		iY  = pnt.y;
	}

	sz_pImageBankVideoTexture->Lock();

	for (int y = iY; y < pBank->tile_size.y; y++)
	{
		sz_pImageBankVideoTexture->DrawPixel(GUI_POINT((SubpixelToScreenCoord(debug_bank_sub_pos.x) + PixelToScreenCoord(iX)) - SubpixelToScreenCoord(gFrame.x), (SubpixelToScreenCoord(debug_bank_sub_pos.y) + PixelToScreenCoord(y)) - SubpixelToScreenCoord(gFrame.y)).from_screenspace(), RGBA(255, 0, 0, 255));

		// Rotate the points if necessary
		if (fAngle)
			pnt = rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(iX * 0x200, y * 0x200)) / 0x200;
		else
			pnt = GUI_POINT(iX, y);

		sz_pImageBankVideoTexture->DrawPixel(GUI_POINT((SubpixelToScreenCoord(debug_bank_sub_pos.x) + PixelToScreenCoord(pnt.x)) - SubpixelToScreenCoord(gFrame.x), (SubpixelToScreenCoord(debug_bank_sub_pos.y) + PixelToScreenCoord(pnt.y)) - SubpixelToScreenCoord(gFrame.y)).from_screenspace(), RGBA(0, 255, 0, 255));

		// Validate that we're still in-bounds
		if (pnt.x < 0 || pnt.x >= pBank->tile_size.x || pnt.y < 0 || pnt.y >= pBank->tile_size.y)
			continue;

		// If we've found it, then return it.
		if (TEST_BIT(pBank->tile_list[CLAMP(iTileIndex, 0, pBank->tile_total_count - 1)].solid_pixel_map, BANK_TILE_XY(pBank, pnt.x, pnt.y)))
		{
			sz_pImageBankVideoTexture->Unlock();
			return pnt.y;
		}
	}

	sz_pImageBankVideoTexture->Unlock();

	return -1;
}

int CSM_ImageBank_TestPixelCollision_MYCHAR(CSM_BANK_IMAGE* pBank, MYCHAR* pMC, int iTileIndex, GUI_POINT pBankPos, int(*pCollideFunc)(const CSM_BANK_COLLISION_INFO* pInfo), float fAngle, int iCenterX, int iCenterY, char iDebugLevel)
{
	if (!CSM_ImageBank_ScanTile(pBank, iTileIndex))
		return 0;

	// Check debug surface
	CSM_INTERNAL_ImageBank_CheckDebugSurface(pBank, iDebugLevel);

	// Get the collision information array ready.
	CSM_BANK_COLLISION_INFO hit_info;

	// Get the tile
	CSM_BANK_TILE* pTile = &pBank->tile_list[iTileIndex];

	// Stop if there are no solid pixels in this tile
	if (!pTile->solid_count)
		return 0;

	// Init a bunch of variables
	GUI_POINT rotated;
	int iCollideFlags = 0;
	int iTestL        = pMC->x - (0x100 * RECT_WIDTH(pMC->rect));
	int iTestT        = pMC->y - (0x100 * RECT_HEIGHT(pMC->rect));
	int iTestR        = pMC->x + (0x100 * RECT_WIDTH(pMC->rect));
	int iTestB        = pMC->y + (0x100 * RECT_HEIGHT(pMC->rect));
	int iBankX        = pBankPos.x - (0x100 * pBank->tile_size.x);
	int iBankY        = pBankPos.y - (0x100 * pBank->tile_size.y);
	int iBankR        = pBankPos.x + (0x100 * pBank->tile_size.x);
	int iBankB        = pBankPos.y + (0x100 * pBank->tile_size.y);
	int iSolidX;
	int iSolidY;
	int iOffsetX;
	int iOffsetY;
	int iTestOffsetX;
	int iTestOffsetY;

	if (iDebugLevel)
		CacheSurface::SetCurrentTexture(sz_pImageBankVideoTexture);

	if (iDebugLevel >= 2)
	{
		// Draw bank rect
		GUI_POINT FRAM = GUI_POINT(gFrame.x * magnification / 0x200, gFrame.y * magnification / 0x200);
		GUI_POINT rcTL = GUI_POINT(rotate_point(pBankPos.x, pBankPos.y, fAngle, GUI_POINT(iBankX, iBankY)).to_screenspace() / 0x200) - FRAM;
		GUI_POINT rcTR = GUI_POINT(rotate_point(pBankPos.x, pBankPos.y, fAngle, GUI_POINT(iBankX + (0x200 * pBank->tile_size.x), iBankY)).to_screenspace() / 0x200) - FRAM;
		GUI_POINT rcBL = GUI_POINT(rotate_point(pBankPos.x, pBankPos.y, fAngle, GUI_POINT(iBankX, iBankY + (0x200 * pBank->tile_size.y))).to_screenspace() / 0x200) - FRAM;
		GUI_POINT rcBR = GUI_POINT(rotate_point(pBankPos.x, pBankPos.y, fAngle, GUI_POINT(iBankX + (0x200 * pBank->tile_size.x), iBankY + (0x200 * pBank->tile_size.y))).to_screenspace() / 0x200) - FRAM;

		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcTL, rcTR, RGBA(255, 0, 0, 255));
		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcTL, rcBL, RGBA(255, 0, 0, 255));
		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcTR, rcBR, RGBA(255, 0, 0, 255));
		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcBL, rcBR, RGBA(255, 0, 0, 255));

		rcTL = GUI_POINT(iTestL * magnification / 0x200, iTestT * magnification / 0x200) - FRAM;
		rcTR = GUI_POINT(iTestR * magnification / 0x200, iTestT * magnification / 0x200) - FRAM;
		rcBL = GUI_POINT(iTestL * magnification / 0x200, iTestB * magnification / 0x200) - FRAM;
		rcBR = GUI_POINT(iTestR * magnification / 0x200, iTestB * magnification / 0x200) - FRAM;

		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcTL, rcTR, RGBA(0, 255, 0, 255));
		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcTL, rcBL, RGBA(0, 255, 0, 255));
		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcTR, rcBR, RGBA(0, 255, 0, 255));
		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcBL, rcBR, RGBA(0, 255, 0, 255));
	}

	if (!fAngle)
	{
		if (
				!(
					(
						(iTestL >= iBankX && iTestL <= iBankR) ||
						(iTestR >= iBankX && iTestR <= iBankR) ||
						(iTestL <= iBankX && iTestR >= iBankR)
					)
					&&
					(
						(iTestT >= iBankY && iTestT <= iBankB) ||
						(iTestB >= iBankY && iTestB <= iBankB) ||
						(iTestT <= iBankY && iTestB >= iBankB)
					)
				 )
		   )
		{
			if (iDebugLevel)
				sz_pImageBankVideoTexture->Unlock();

			return FALSE;
		}
	}
	else
	{
		GUI_POINT rcTL = rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(0, 0));
		GUI_POINT rcTR = rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(0x200 * pBank->tile_size.x, 0));
		GUI_POINT rcBL = rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(0, 0x200 * pBank->tile_size.y));
		GUI_POINT rcBR = rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(0x200 * pBank->tile_size.x, 0x200 * pBank->tile_size.y));
		int L          = iBankX + FastMin(rcTL.x, rcTR.x, rcBL.x, rcBR.x);
		int T          = iBankY + FastMin(rcTL.y, rcTR.y, rcBL.y, rcBR.y);
		int R          = iBankX + FastMax(rcTL.x, rcTR.x, rcBL.x, rcBR.x);
		int B          = iBankY + FastMax(rcTL.y, rcTR.y, rcBL.y, rcBR.y);

		if (
				!(
					(
						(iTestL >= L && iTestL <= R) ||
						(iTestR >= L && iTestR <= R) ||
						(iTestL <= L && iTestR >= R)
					)
					&&
					(
						(iTestT >= T && iTestT <= B) ||
						(iTestB >= T && iTestB <= B) ||
						(iTestT <= T && iTestB >= B)
					)
				 )
		   )
		{
			if (iDebugLevel)
				sz_pImageBankVideoTexture->Unlock();

			return FALSE;
		}
	}

	// Configure the hit info struct beforehand
	hit_info.object_subrect = CSM_INTERNAL_ImageBank_CreateRect(iTestL, iTestT, iTestR, iTestB);
	hit_info.bank_subrect   = CSM_INTERNAL_ImageBank_CreateRect(iBankX, iBankY, iBankR, iBankB);
	hit_info.bank           = pBank;
	hit_info.object         = (void*)pMC;
	hit_info.object_old_xm  = pMC->xm;
	hit_info.object_old_ym  = pMC->ym;
	hit_info.tile_index_0   = iTileIndex;
	hit_info.tile_index_1   = -1;
	hit_info.pass_count     = -1;

	// Calculate the offsets
	for (int i = 0; i < pTile->solid_count; i++)
	{
		iSolidX = (pTile->solid_pixel_list[pTile->solid_count - (i + 1)] % pBank->tile_size.x);
		iSolidY = (pTile->solid_pixel_list[pTile->solid_count - (i + 1)] / pBank->tile_size.x);

		if (fAngle)
		{
			rotated = rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(iSolidX * 0x200, iSolidY * 0x200));
			iSolidX = rotated.x / 0x200;
			iSolidY = rotated.y / 0x200;
		}

		iOffsetX = iBankX + (iSolidX * 0x200);
		iOffsetY = iBankY + (iSolidY * 0x200);

		// Skip if we're not even in the player's collision box
		if (
			(iOffsetX < iTestL || iOffsetX > iTestR) &&
			(iOffsetY < iTestT || iOffsetY > iTestB)
			)
		{
			if (iDebugLevel >= 3)
			{
				sz_ImageBank_Debug_pInvalidPixelList[sz_ImageBank_Debug_iInvalidPixelCount++] = GUI_POINT(SubpixelToPixel(iOffsetX) - SubpixelToPixel(gFrame.x), SubpixelToPixel(iOffsetY) - SubpixelToPixel(gFrame.y));
			}

			continue;
		}

		// Get the offsets
		iTestOffsetX = (pMC->direct == Directions::DIRECT_RIGHT ? (iTestR - iTestL) - (iTestR - iOffsetX) : iOffsetX - iTestL);
		iTestOffsetY = iOffsetY - iTestT;

		// Test the pixel on the player's frame
		if (!pMC->character->IsPixelSolid(PlayerAnimFrame::FRAME_IDLE, iTestOffsetX / 0x200, iTestOffsetY / 0x200))
		{
			if (iDebugLevel >= 3)
			{
				sz_ImageBank_Debug_pInvalidPixelList[sz_ImageBank_Debug_iInvalidPixelCount++] = GUI_POINT(SubpixelToPixel(iOffsetX) - SubpixelToPixel(gFrame.x), SubpixelToPixel(iOffsetY) - SubpixelToPixel(gFrame.y));
			}

			continue;
		}

		if (iDebugLevel >= 3)
		{
			sz_ImageBank_Debug_pValidPixelList[sz_ImageBank_Debug_iValidPixelCount++] = GUI_POINT(SubpixelToPixel(iOffsetX) - SubpixelToPixel(gFrame.x), SubpixelToPixel(iOffsetY) - SubpixelToPixel(gFrame.y));
		}

		if (!pCollideFunc)
		{
			iCollideFlags = 0x10000;
			break;
		}

		// Configure the hit info struct
		hit_info.map_tile_pos            = GUI_POINT(iOffsetX / 0x200 / 0x10, iOffsetY / 0x200 / 0x10);
		hit_info.bank_hit_pixel_subpos   = GUI_POINT(iOffsetX, iOffsetY);
		hit_info.object_hit_pixel_subpos = GUI_POINT(iTestL + iTestOffsetX, iTestT + iTestOffsetY);
		hit_info.pass_count++;

		// Call the collision function
		iCollideFlags |= pCollideFunc((const CSM_BANK_COLLISION_INFO*)&hit_info);

		if (iCollideFlags & GMC_flag::gmc_flag_collide_left)
		{
			for (int j = i; j >= 0; j--)
			{
				if ((pTile->solid_pixel_list[pTile->solid_count - (j + 1)] / pBank->tile_size.x) < (pTile->solid_pixel_list[pTile->solid_count - (i + 1)] / pBank->tile_size.x))
				{
					i = j;
					break;
				}
			}
		}

		// Recalculate all positions
		iTestL = pMC->x - (0x100 * RECT_WIDTH(pMC->rect));
		iTestT = pMC->y - (0x100 * RECT_HEIGHT(pMC->rect));
		iTestR = pMC->x + (0x100 * RECT_WIDTH(pMC->rect));
		iTestB = pMC->y + (0x100 * RECT_HEIGHT(pMC->rect));
		iBankX = pBankPos.x - (0x100 * pBank->tile_size.x);
		iBankY = pBankPos.y - (0x100 * pBank->tile_size.y);
		iBankR = pBankPos.x + (0x100 * pBank->tile_size.x);
		iBankB = pBankPos.y + (0x100 * pBank->tile_size.y);

		// Update hit info
		hit_info.object_subrect = CSM_INTERNAL_ImageBank_CreateRect(iTestL, iTestT, iTestR, iTestB);
		hit_info.bank_subrect   = CSM_INTERNAL_ImageBank_CreateRect(iBankX, iBankY, iBankR, iBankB);
	}

	if (iDebugLevel)
	{
		if (iDebugLevel >= 3)
		{
			if (sz_ImageBank_Debug_iInvalidPixelCount)
				sz_pImageBankVideoTexture->DrawPixels(sz_ImageBank_Debug_pInvalidPixelList, sz_ImageBank_Debug_iInvalidPixelCount, RGBA(255, 0, 0, 255));

			if (sz_ImageBank_Debug_iValidPixelCount)
				sz_pImageBankVideoTexture->DrawPixels(sz_ImageBank_Debug_pValidPixelList, sz_ImageBank_Debug_iValidPixelCount, RGBA(0, 255, 0, 255));
		}

		CacheSurface::SetCurrentTexture(NULL);
	}
	
	return iCollideFlags;
}

BOOL CSM_ImageBank_TestPixelCollision_NPCHAR(CSM_BANK_IMAGE* pBank, NPCHAR* pNpc, int iTileIndex, GUI_POINT pBankPos, int(*pCollideFunc)(const CSM_BANK_COLLISION_INFO* pInfo), float fAngle, int iCenterX, int iCenterY, char iDebugLevel)
{
	if (!CSM_ImageBank_ScanTile(pBank, iTileIndex))
		return FALSE;

	// Check debug surface
	CSM_INTERNAL_ImageBank_CheckDebugSurface(pBank, iDebugLevel);

	// Get the collision information array ready.
	CSM_BANK_COLLISION_INFO hit_info;

	// Get the tile
	CSM_BANK_TILE* pTile = &pBank->tile_list[iTileIndex];

	// Stop if there are no solid pixels in this tile
	if (!pTile->solid_count)
		return FALSE;

	// Init a bunch of variables
	GUI_POINT rotated;
	BOOL bCollide = FALSE;
	bool bSeekBck = true;
	int iTestL    = pNpc->x - (0x100 * RECT_WIDTH(pNpc->rect));
	int iTestT    = pNpc->y - (0x100 * RECT_HEIGHT(pNpc->rect));
	int iTestR    = pNpc->x + (0x100 * RECT_WIDTH(pNpc->rect));
	int iTestB    = pNpc->y + (0x100 * RECT_HEIGHT(pNpc->rect));
	int iBankX    = pBankPos.x - (0x100 * pBank->tile_size.x);
	int iBankY    = pBankPos.y - (0x100 * pBank->tile_size.y);
	int iBankR    = pBankPos.x + (0x100 * pBank->tile_size.x);
	int iBankB    = pBankPos.y + (0x100 * pBank->tile_size.y);
	int iSolidX;
	int iSolidY;
	int iOffsetX;
	int iOffsetY;
	int iTestOffsetX;
	int iTestOffsetY;

	if (iDebugLevel)
		sz_pImageBankVideoTexture->Lock();

	if (iDebugLevel >= 2)
	{
		// Draw bank rect
		GUI_POINT FRAM = GUI_POINT(gFrame.x * magnification / 0x200, gFrame.y * magnification / 0x200);
		GUI_POINT rcTL = GUI_POINT(rotate_point(pBankPos.x, pBankPos.y, fAngle, GUI_POINT(iBankX, iBankY)).to_screenspace() / 0x200) - FRAM;
		GUI_POINT rcTR = GUI_POINT(rotate_point(pBankPos.x, pBankPos.y, fAngle, GUI_POINT(iBankX + (0x200 * pBank->tile_size.x), iBankY)).to_screenspace() / 0x200) - FRAM;
		GUI_POINT rcBL = GUI_POINT(rotate_point(pBankPos.x, pBankPos.y, fAngle, GUI_POINT(iBankX, iBankY + (0x200 * pBank->tile_size.y))).to_screenspace() / 0x200) - FRAM;
		GUI_POINT rcBR = GUI_POINT(rotate_point(pBankPos.x, pBankPos.y, fAngle, GUI_POINT(iBankX + (0x200 * pBank->tile_size.x), iBankY + (0x200 * pBank->tile_size.y))).to_screenspace() / 0x200) - FRAM;

		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcTL, rcTR, RGBA(255, 0, 0, 255));
		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcTL, rcBL, RGBA(255, 0, 0, 255));
		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcTR, rcBR, RGBA(255, 0, 0, 255));
		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcBL, rcBR, RGBA(255, 0, 0, 255));

		rcTL = GUI_POINT(iTestL * magnification / 0x200, iTestT * magnification / 0x200) - FRAM;
		rcTR = GUI_POINT(iTestR * magnification / 0x200, iTestT * magnification / 0x200) - FRAM;
		rcBL = GUI_POINT(iTestL * magnification / 0x200, iTestB * magnification / 0x200) - FRAM;
		rcBR = GUI_POINT(iTestR * magnification / 0x200, iTestB * magnification / 0x200) - FRAM;

		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcTL, rcTR, RGBA(0, 255, 0, 255));
		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcTL, rcBL, RGBA(0, 255, 0, 255));
		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcTR, rcBR, RGBA(0, 255, 0, 255));
		sz_pImageBankVideoTexture->DrawMagnifiedLine(rcBL, rcBR, RGBA(0, 255, 0, 255));
	}

	if (!fAngle)
	{
		if (
			!(
			(
				(iTestL >= iBankX && iTestL <= iBankR) ||
				(iTestR >= iBankX && iTestR <= iBankR) ||
				(iTestL <= iBankX && iTestR >= iBankR)
				)
				&&
				(
				(iTestT >= iBankY && iTestT <= iBankB) ||
					(iTestB >= iBankY && iTestB <= iBankB) ||
					(iTestT <= iBankY && iTestB >= iBankB)
					)
				)
			)
		{
			if (iDebugLevel)
				sz_pImageBankVideoTexture->Unlock();

			return FALSE;
		}
	}
	else
	{
		GUI_POINT rcTL = rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(0, 0));
		GUI_POINT rcTR = rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(0x200 * pBank->tile_size.x, 0));
		GUI_POINT rcBL = rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(0, 0x200 * pBank->tile_size.y));
		GUI_POINT rcBR = rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(0x200 * pBank->tile_size.x, 0x200 * pBank->tile_size.y));
		int L          = iBankX + FastMin(rcTL.x, rcTR.x, rcBL.x, rcBR.x);
		int T          = iBankY + FastMin(rcTL.y, rcTR.y, rcBL.y, rcBR.y);
		int R          = iBankX + FastMax(rcTL.x, rcTR.x, rcBL.x, rcBR.x);
		int B          = iBankY + FastMax(rcTL.y, rcTR.y, rcBL.y, rcBR.y);

		if (
				!(
					(
						(iTestL >= L && iTestL <= R) ||
						(iTestR >= L && iTestR <= R) ||
						(iTestL <= L && iTestR >= R)
					)
					&&
					(
						(iTestT >= T && iTestT <= B) ||
						(iTestB >= T && iTestB <= B) ||
						(iTestT <= T && iTestB >= B)
					)
				 )
		   )
		{
			if (iDebugLevel)
				sz_pImageBankVideoTexture->Unlock();

			return FALSE;
		}
	}

	// Configure the hit info struct beforehand
	hit_info.object_subrect = CSM_INTERNAL_ImageBank_CreateRect(iTestL, iTestT, iTestR, iTestB);
	hit_info.bank_subrect   = CSM_INTERNAL_ImageBank_CreateRect(iBankX, iBankY, iBankR, iBankB);
	hit_info.bank           = pBank;
	hit_info.object         = (void*)pNpc;
	hit_info.object_old_xm  = pNpc->xm;
	hit_info.object_old_ym  = pNpc->ym;
	hit_info.tile_index_0   = iTileIndex;
	hit_info.tile_index_1   = -1;
	hit_info.pass_count     = -1;

	// Calculate the offsets
	for (int i = 0; i < pTile->solid_count; i++)
	{
		iSolidX = (pTile->solid_pixel_list[pTile->solid_count - (i + 1)] % pBank->tile_size.x);
		iSolidY = (pTile->solid_pixel_list[pTile->solid_count - (i + 1)] / pBank->tile_size.x);

		if (fAngle)
		{
			rotated = GUI_POINT(rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(iSolidX * 0x200, iSolidY * 0x200)));
			iSolidX = rotated.x / 0x200;
			iSolidY = rotated.y / 0x200;
		}

		iOffsetX = iBankX + (iSolidX * 0x200);
		iOffsetY = iBankY + (iSolidY * 0x200);

		// Skip if we're not even in the player's collision box
		if (
			(iOffsetX < iTestL || iOffsetX > iTestR) &&
			(iOffsetY < iTestT || iOffsetY > iTestB)
			)
			continue;

		// Get the offsets
		iTestOffsetX = (pNpc->direct == Directions::DIRECT_RIGHT ? iTestR - iOffsetX : iOffsetX - iTestL);
		iTestOffsetY = iOffsetY - iTestT;

		// Test the pixel on the player's frame
		if (!pBank->solid_func(pNpc->surf, iTestOffsetX / 0x200, iTestOffsetY / 0x200))
			continue;

		if (!pCollideFunc)
			return TRUE;

		// Set the fact that we collided with something
		bCollide = TRUE;

		// Configure the hit info struct
		hit_info.map_tile_pos            = GUI_POINT(iOffsetX / 0x200 / 0x10, iOffsetY / 0x200 / 0x10);
		hit_info.bank_hit_pixel_subpos   = GUI_POINT(iOffsetX, iOffsetY);
		hit_info.object_hit_pixel_subpos = GUI_POINT(iTestL + iTestOffsetX, iTestT + iTestOffsetY);
		hit_info.pass_count++;

		// Call the collision function
		pCollideFunc((const CSM_BANK_COLLISION_INFO*)&hit_info);

		// Recalculate all positions
		iTestL = pNpc->x - (0x100 * RECT_WIDTH(pNpc->rect));
		iTestT = pNpc->y - (0x100 * RECT_HEIGHT(pNpc->rect));
		iTestR = pNpc->x + (0x100 * RECT_WIDTH(pNpc->rect));
		iTestB = pNpc->y + (0x100 * RECT_HEIGHT(pNpc->rect));
		iBankX = pBankPos.x - (0x100 * pBank->tile_size.x);
		iBankY = pBankPos.y - (0x100 * pBank->tile_size.y);
		iBankR = pBankPos.x + (0x100 * pBank->tile_size.x);
		iBankB = pBankPos.y + (0x100 * pBank->tile_size.y);

		// Update hit info
		hit_info.object_subrect = CSM_INTERNAL_ImageBank_CreateRect(iTestL, iTestT, iTestR, iTestB);
		hit_info.bank_subrect   = CSM_INTERNAL_ImageBank_CreateRect(iBankX, iBankY, iBankR, iBankB);
	}

	if (iDebugLevel)
		sz_pImageBankVideoTexture->Unlock();
	
	return bCollide;
}

BOOL CSM_ImageBank_TestPixelCollision_BANK(CSM_BANK_IMAGE* pBank, CSM_BANK_IMAGE* pOther, int iTileIndex1, int iTileIndex2, GUI_POINT pBankPos1, GUI_POINT pBankPos2, void(*pCollideFunc)(const CSM_BANK_COLLISION_INFO* pInfo), float fAngle, int iCenterX, int iCenterY, char iDebugLevel)
{
	if (!CSM_ImageBank_ScanTile(pBank, iTileIndex1) || !CSM_ImageBank_ScanTile(pOther, iTileIndex2))
		return FALSE;

	// Check debug surface
	CSM_INTERNAL_ImageBank_CheckDebugSurface(pBank, iDebugLevel);

	// Get the collision information array ready.
	CSM_BANK_COLLISION_INFO hit_info;

	// Get the tile
	CSM_BANK_TILE* pTile = &pBank->tile_list[iTileIndex1];

	// Stop if there are no solid pixels in this tile
	if (!pTile->solid_count)
		return FALSE;

	// Init a bunch of variables
	GUI_POINT rotated;
	BOOL bCollide = FALSE;
	bool bSeekBck = (pBankPos2.y < pBankPos1.y);
	int iTestL    = pBankPos2.x - (0x100 * pOther->tile_size.x);
	int iTestT    = pBankPos2.y - (0x100 * pOther->tile_size.y);
	int iTestR    = pBankPos2.x + (0x100 * pOther->tile_size.x);
	int iTestB    = pBankPos2.y + (0x100 * pOther->tile_size.y);
	int iBankX    = pBankPos1.x - (0x100 * pBank->tile_size.x);
	int iBankY    = pBankPos1.y - (0x100 * pBank->tile_size.y);
	int iBankR    = pBankPos1.x + (0x100 * pBank->tile_size.x);
	int iBankB    = pBankPos1.y + (0x100 * pBank->tile_size.y);
	int iSolidX;
	int iSolidY;
	int iOffsetX;
	int iOffsetY;
	int iTestOffsetX;
	int iTestOffsetY;

	if (!fAngle)
	{
		if (
			!(
			(
				(iTestL >= iBankX && iTestL <= iBankR) ||
				(iTestR >= iBankX && iTestR <= iBankR) ||
				(iTestL <= iBankX && iTestR >= iBankR)
				)
				&&
				(
				(iTestT >= iBankY && iTestT <= iBankB) ||
					(iTestB >= iBankY && iTestB <= iBankB) ||
					(iTestT <= iBankY && iTestB >= iBankB)
					)
				)
			)
			return FALSE;
	}

	// Configure the hit info struct beforehand
	hit_info.object_subrect = CSM_INTERNAL_ImageBank_CreateRect(iTestL, iTestT, iTestR, iTestB);
	hit_info.bank_subrect   = CSM_INTERNAL_ImageBank_CreateRect(iBankX, iBankY, iBankR, iBankB);
	hit_info.bank           = pBank;
	hit_info.object         = (void*)pOther;
	hit_info.tile_index_0   = iTileIndex1;
	hit_info.tile_index_1   = iTileIndex2;
	hit_info.pass_count     = -1;

	// Calculate the offsets
	for (int i = 0; i < pTile->solid_count; i++)
	{
		iSolidX = (pTile->solid_pixel_list[bSeekBck ? pTile->solid_count - (i + 1) : i] % pBank->tile_size.x);
		iSolidY = (pTile->solid_pixel_list[bSeekBck ? pTile->solid_count - (i + 1) : i] / pBank->tile_size.x);

		if (fAngle)
		{
			rotated = GUI_POINT(rotate_point(iCenterX, iCenterY, fAngle, GUI_POINT(iSolidX * 0x200, iSolidY * 0x200)));
			iSolidX = rotated.x / 0x200;
			iSolidY = rotated.y / 0x200;
		}

		iOffsetX = iBankX + (iSolidX * 0x200);
		iOffsetY = iBankY + (iSolidY * 0x200);

		// Skip if we're not even in the player's collision box
		if (
			(iOffsetX < iTestL || iOffsetX > iTestR) &&
			(iOffsetY < iTestT || iOffsetY > iTestB)
			)
			continue;

		// Get the offsets
		iTestOffsetX = iOffsetX - iTestL;
		iTestOffsetY = iOffsetY - iTestT;

		// Test the pixel on the player's frame
		if (!pBank->solid_func(pOther->surf_id, iTestOffsetX / 0x200, iTestOffsetY / 0x200))
			continue;

		if (!pCollideFunc)
			return TRUE;

		// Set the fact that we collided with something
		bCollide = TRUE;

		// Configure the hit info struct
		hit_info.map_tile_pos            = GUI_POINT(iOffsetX / 0x200 / 0x10, iOffsetY / 0x200 / 0x10);
		hit_info.bank_hit_pixel_subpos   = GUI_POINT(iOffsetX, iOffsetY);
		hit_info.object_hit_pixel_subpos = GUI_POINT(iTestL + iTestOffsetX, iTestT + iTestOffsetY);
		hit_info.pass_count++;

		// Call the collision function
		pCollideFunc((const CSM_BANK_COLLISION_INFO*)&hit_info);
		
		// Recalculate all positions
		iTestL = pBankPos2.x - (0x100 * pOther->tile_size.x);
		iTestT = pBankPos2.y - (0x100 * pOther->tile_size.y);
		iTestR = pBankPos2.x + (0x100 * pOther->tile_size.x);
		iTestB = pBankPos2.y + (0x100 * pOther->tile_size.y);
		iBankX = pBankPos1.x - (0x100 * pBank->tile_size.x);
		iBankY = pBankPos1.y - (0x100 * pBank->tile_size.y);
		iBankR = pBankPos1.x + (0x100 * pBank->tile_size.x);
		iBankB = pBankPos1.y + (0x100 * pBank->tile_size.y);

		// Update hit info
		hit_info.object_subrect = CSM_INTERNAL_ImageBank_CreateRect(iTestL, iTestT, iTestR, iTestB);
		hit_info.bank_subrect   = CSM_INTERNAL_ImageBank_CreateRect(iBankX, iBankY, iBankR, iBankB);
	}
	
	return bCollide;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_endcode.h>
