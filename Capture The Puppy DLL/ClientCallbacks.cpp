#include "Weapons/ModInfo.h"
#include "CavestoryModAPI.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static GUI_COLOR pTeamColors[] = { GUI_COLOR(255, 255, 0, 255), GUI_COLOR(0, 255, 0, 255), GUI_COLOR(255, 56, 56, 255), GUI_COLOR(37, 177, 249, 255) };

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MAP_TEAM_INFO* gTeamInfoList             = NULL;
unsigned int gTeamInfoCount              = 0;
MAP_COVER_IMAGE_CACHE* gCacheSurfaceList = NULL;
unsigned int gCacheSurfaceCount          = 0;
MAP_COVER_IMAGE* gPublicCoverImageList   = NULL;
unsigned int gPublicCoverImageCount      = 0;
MAP_AUDIO_FILE* gTeamAudioList           = NULL;
unsigned int gTeamAudioCount             = 0;
CaveNet::DataStructures::NetTeam::TeamType gOurCurrentTeamType = (CaveNet::DataStructures::NetTeam::TeamType) - 1;
char g_CurrentStage_Name[260] = { 0 };

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CheckFileExists(const char* name)
{
	FILE* file = fopen(name, "rb");
	if (file)
	{
		fclose(file);
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//#define IGNORE_NODE(Index)		CSM_Log("[AI] Ignoring Node # %d...\r\n", Index); g_AI_NodeInfo.node_ignore_list[Index] = true;
//#define UNIGNORE_NODE(Index)	CSM_Log("[AI] No longer ignoring Node # %d...\r\n", Index); g_AI_NodeInfo.node_ignore_list[Index] = false;
#define IGNORE_NODE(Index)		g_AI_NodeInfo.node_ignore_list[Index] = true;
#define UNIGNORE_NODE(Index)	g_AI_NodeInfo.node_ignore_list[Index] = false;
#define IS_IGNORING_NODE(Index)	g_AI_NodeInfo.node_ignore_list[Index]

bool Puppy_POP_RECURSIVE(List<MAP_PUPPY_NODE_MOVE_INFO>* pOutList, MAP_PUPPY_NODE* pStart, int iDestX, int iDestY, bool bAddToPot = true)
{
	bool bGoToDest = false;

	int iValidList[32];
	int iValidCount = 0;

	// Check to see if we can see the point
	if (!CSM_Raycasting_CastRay_Quick(pStart->x * 0x2000, pStart->y * 0x2000, iDestX, iDestY, TileType::TT_NO_PLAYER, NULL, true, true))
	{
		// Instantly return TRUE if we can reach it
		if (dist(pStart->x * 0x2000, pStart->y * 0x2000, iDestX, iDestY) <= 0x2000)
			goto ADD_DEST_MOVE_INSTRUCTION;

		// Otherwise, check all connections to see if we can get closer to it before we just go to it blindly.
		bGoToDest = true;
	}

	// Look through all nodes
	for (int i = 0; i < pStart->conn_count; i++)
	{
		MAP_PUPPY_NODE_CONNECTION* pConn = &g_AI_NodeInfo.node_connection_list[pStart->conn_list[i]];
		int iNextNodeIdx                 = (&g_AI_NodeInfo.node_list[pConn->one] == pStart ? pConn->two : pConn->one);

		// Skip ignored nodes
		if (IS_IGNORING_NODE(iNextNodeIdx))
			continue;

		// Insert into the ignore list
		IGNORE_NODE(iNextNodeIdx);

		// Get the result
		bool bResult = Puppy_POP_RECURSIVE(pOutList, &g_AI_NodeInfo.node_list[iNextNodeIdx], iDestX, iDestY, false);

		// Add to valid path if it was valid
		if (bResult)
		{
			if (!bAddToPot)
			{
				UNIGNORE_NODE(iNextNodeIdx);
			}

			if (iValidCount + 1 >= 32)
				CSM_Log("Puppy_POP_RECURSIVE() - Unable to add more than 32 connections to the random list!\r\n");
			else
				iValidList[iValidCount++] = i;
		}
	}

	if (iValidCount)
	{
		// Pick a random path
		if (bAddToPot)
		{
			MAP_PUPPY_NODE_CONNECTION* pConn   = &g_AI_NodeInfo.node_connection_list[pStart->conn_list[iValidList[Random(0, iValidCount - 1)]]];
			MAP_PUPPY_NODE_MOVE_INFO* pNewMove = pOutList->Increment();
			bool bMovingTo                     = (&g_AI_NodeInfo.node_list[pConn->one] == pStart);
			pNewMove->tgt_x                    = g_AI_NodeInfo.node_list[bMovingTo ? pConn->two : pConn->one].x * 0x2000;
			pNewMove->tgt_y                    = g_AI_NodeInfo.node_list[bMovingTo ? pConn->two : pConn->one].y * 0x2000;
			pNewMove->move_type                = (bMovingTo ? pConn->move_type_to : pConn->move_type_from);

			Puppy_POP_RECURSIVE(pOutList, &g_AI_NodeInfo.node_list[pConn->two], iDestX, iDestY, true);
		}

		return true;
	}

	if (bGoToDest)
	{
	ADD_DEST_MOVE_INSTRUCTION:
		if (bAddToPot)
		{
			MAP_PUPPY_NODE_MOVE_INFO* pNewMove = pOutList->Increment();
			pNewMove->tgt_x                    = iDestX;
			pNewMove->tgt_y                    = iDestY;
			pNewMove->move_type                = PuppyMoveType::PUPPY_MOVE_JUMP;
		}

		return true;
	}

	return false;
}

// iStartX = subpixel X
// iStartY = subpixel Y
// iDestX  = subpixel X
// iDestY  = subpixel Y
bool Puppy_TST_RECURSIVE(MAP_PUPPY_NODE* pStart, int iDestX, int iDestY)
{
	bool bOurResult = false;

	// Check to see if we can see the point
	if (!CSM_Raycasting_CastRay_Quick(pStart->x * 0x2000, pStart->y * 0x2000, iDestX, iDestY, TileType::TT_NO_PLAYER, NULL, true, true))
	{
		// Instantly return TRUE if we can reach it
		if (dist(pStart->x * 0x2000, pStart->y * 0x2000, iDestX, iDestY) <= 0x2000)
			return true;

		// Otherwise, check all connections to see if we can get closer to it before we just go to it blindly.
		bOurResult = true;
	}

	// Look through all nodes
	for (int i = 0; i < pStart->conn_count; i++)
	{
		MAP_PUPPY_NODE_CONNECTION* pConn = &g_AI_NodeInfo.node_connection_list[pStart->conn_list[i]];
		int iNextNodeIdx                 = (&g_AI_NodeInfo.node_list[pConn->one] == pStart ? pConn->two : pConn->one);

		// Skip ignored nodes
		if (IS_IGNORING_NODE(iNextNodeIdx))
			continue;

		// Insert into the ignore list
		IGNORE_NODE(iNextNodeIdx);

		// Get the result
		bool bResult = Puppy_TST_RECURSIVE(&g_AI_NodeInfo.node_list[iNextNodeIdx], iDestX, iDestY);

		// Add to valid path if it was valid
		if (bResult)
		{
			UNIGNORE_NODE(iNextNodeIdx);
			bOurResult = true;
		}
	}

	return bOurResult;
}

bool Puppy_PopulateMoveList(List<MAP_PUPPY_NODE_MOVE_INFO>* pOutList, int iStartX, int iStartY, int iDestX, int iDestY)
{
	bool bValidAnyways    = false;
	MAP_PUPPY_NODE* pNode = NULL;

	// Find the closest node to start from
	float fScoreDist = -1.f;
	int iBestNode    = -1;
	bool bFoundPath  = false;

	if (!CSM_Raycasting_CastRay_Quick(iStartX, iStartY, iDestX, iDestY, TileType::TT_NO_PLAYER, NULL, true, true))
	{
		if (dist(iStartX, iStartY, iDestX, iDestY) <= 0x4000)
			goto ADD_DEST_MOVEMENT_INSTRUCTION;

		bValidAnyways = true;
	}

	// Loop through the nodes for this info object
	for (int j = 0; j < g_AI_NodeInfo.node_count; j++)
	{
		pNode           = &g_AI_NodeInfo.node_list[j];
		float fOurScore = dist(iStartX, iStartY, pNode->x * 0x2000, pNode->y * 0x2000);

		// Skip nodes we cannot see
		//if ((pNode->x != npc->x / 0x2000) && (pNode->y != npc->y / 0x2000) && CSM_Raycasting_CastRay_Quick(npc->x, npc->y, pNode->x * 0x2000, pNode->y * 0x2000, TileType::TT_NO_PLAYER))
		if (CSM_Raycasting_CastRay_Quick(iStartX, iStartY, pNode->x * 0x2000, pNode->y * 0x2000, TileType::TT_NO_PLAYER))
			continue;

		// Continue if this doesn't beat the current record
		if (iBestNode == -1 || fOurScore < fScoreDist)
		{
			iBestNode  = j;
			fScoreDist = fOurScore;
		}
	}

	// Stop here, because we've failed.
	if (iBestNode == -1)
		return false;

	pNode = &g_AI_NodeInfo.node_list[iBestNode];

	if (!g_AI_NodeInfo.node_ignore_list || g_AI_NodeInfo.node_ignore_size != g_AI_NodeInfo.node_count)
	{
		// Set the cache variable
		g_AI_NodeInfo.node_ignore_size = g_AI_NodeInfo.node_count;

		// Free the old list
		if (g_AI_NodeInfo.node_ignore_list)
			free(g_AI_NodeInfo.node_ignore_list);

		// Allocate the newly sized list
		g_AI_NodeInfo.node_ignore_list = (bool*)malloc(sizeof(bool) * g_AI_NodeInfo.node_count);

		CSM_Log("[AI] Allocated an ignore list of %d bytes...\r\n", sizeof(bool) * g_AI_NodeInfo.node_count);
	}

	// Zero-mem it
	memset(g_AI_NodeInfo.node_ignore_list, 0, sizeof(bool) * g_AI_NodeInfo.node_count);

	// Set the best node's ignore value to true
	IGNORE_NODE(iBestNode);

	// Look through all nodes
	for (int i = 0; i < pNode->conn_count; i++)
	{
		MAP_PUPPY_NODE_CONNECTION* pConn = &g_AI_NodeInfo.node_connection_list[pNode->conn_list[i]];
		int iNextNodeIdx                 = (pConn->one == iBestNode ? pConn->two : pConn->one);

		// Insert into the ignore list
		IGNORE_NODE(iNextNodeIdx);

		// Get the result
		bool bResult = Puppy_TST_RECURSIVE(&g_AI_NodeInfo.node_list[iNextNodeIdx], iDestX, iDestY);

		// Add to valid path if it was valid
		if (bResult)
		{
			UNIGNORE_NODE(iNextNodeIdx);
			bFoundPath = true;
		}
	}

	// Clear the list
	pOutList->Clear();

	// Stop if we got no valid path
	if (!bFoundPath)
	{
		if (bValidAnyways)
		{
ADD_DEST_MOVEMENT_INSTRUCTION:
			MAP_PUPPY_NODE_MOVE_INFO* pNewMove = pOutList->Increment();
			pNewMove->move_type                = PuppyMoveType::PUPPY_MOVE_JUMP;
			pNewMove->tgt_x                    = iDestX;
			pNewMove->tgt_y                    = iDestY;

			return true;
		}

		return false;
	}

	MAP_PUPPY_NODE_MOVE_INFO* pNewMove = pOutList->Increment();
	pNewMove->move_type                = PuppyMoveType::PUPPY_MOVE_RUN;
	pNewMove->tgt_x                    = pNode->x * 0x2000;
	pNewMove->tgt_y                    = pNode->y * 0x2000;

	Puppy_POP_RECURSIVE(pOutList, pNode, iDestX, iDestY, true);

	return true;
}

// iDestX = subpixel x
// iDestY = subpixel y
bool Puppy_GetNextNodePlayer(List<MAP_PUPPY_NODE_MOVE_INFO>* pOutList, int iTeamType, int iDestX, int iDestY, MYCHAR* npc, PuppyMoveType* pMoveType, int* tgt_x, int* tgt_y)
{
	for (int i = 0; i < gTeamInfoCount; i++)
	{
		MAP_TEAM_INFO* pInfo = &gTeamInfoList[i];

		// Skip team mismatches
		if (pInfo->team_type != iTeamType)
			continue;

		float fScoreDist = -1.f;
		int iBestNode    = -1;

		// Loop through the nodes for this info object
		for (int j = 0; j < pInfo->node_count; j++)
		{
			MAP_PUPPY_NODE* pNode = &pInfo->node_list[j];
			float fOurScore       = dist(npc->x, npc->y, pNode->x * 0x2000, pNode->y * 0x2000);

			// Skip nodes we cannot see
			//if ((pNode->x != npc->x / 0x2000) && (pNode->y != npc->y / 0x2000) && CSM_Raycasting_CastRay_Quick(npc->x, npc->y, pNode->x * 0x2000, pNode->y * 0x2000, TileType::TT_NO_PLAYER))
			if (CSM_Raycasting_CastRay_Quick(npc->x, npc->y, pNode->x * 0x2000, pNode->y * 0x2000, TileType::TT_NO_PLAYER))
				continue;

			// Continue if this doesn't beat the current record
			if (iBestNode == -1 || fOurScore < fScoreDist)
			{
				iBestNode  = j;
				fScoreDist = fOurScore;
			}
		}

		// Stop here, because we've failed.
		if (iBestNode == -1)
			break;

		int iConList[128];
		int iConCount = 0;
		
		// Loop through and find the connection.
		for (int j = 0; j < pInfo->node_connection_count; j++)
		{
			MAP_PUPPY_NODE_CONNECTION* pConn = &pInfo->node_connection_list[j];

			// Skip mismatches
			if (pConn->one != iBestNode)
				continue;
			
			iConList[iConCount++] = j;

			// Stop if we're at the limit
			if (iConCount + 1 >= 128)
				break;
		}

		if (iConCount)
		{
			MAP_PUPPY_NODE_CONNECTION* pConn = &pInfo->node_connection_list[iConList[Random(0, iConCount - 1)]];

			*tgt_x     = pInfo->node_list[pConn->two].x * 0x2000;
			*tgt_y     = pInfo->node_list[pConn->two].y * 0x2000;
			*pMoveType = pConn->move_type_to;
			
			return true;
		}
	}

	return false;
}

bool Puppy_GetNextNode(NPCHAR* npc, PuppyMoveType* pMoveType, int* tgt_x, int* tgt_y)
{
	for (int i = 0; i < gTeamInfoCount; i++)
	{
		MAP_TEAM_INFO* pInfo = &gTeamInfoList[i];

		// Skip team mismatches
		if (pInfo->team_type != npc->code_flag)
			continue;

		float fScoreDist = -1;
		int iBestNode    = -1;

		// Loop through the nodes for this info object
		for (int j = 0; j < pInfo->node_count; j++)
		{
			MAP_PUPPY_NODE* pNode = &pInfo->node_list[j];
			float fOurScore       = dist(npc->x, npc->y, pNode->x * 0x2000, pNode->y * 0x2000);

			// Continue if this doesn't beat the current record
			if (iBestNode == -1 || fOurScore < fScoreDist)
			{
				iBestNode  = j;
				fScoreDist = fOurScore;
			}
		}

		// Stop here, because we've failed.
		if (iBestNode == -1)
			break;
		
		// Loop through and find the connection.
		int iConList[128];
		int iConCount = 0;
		
		// Loop through and find the connection.
		for (int j = 0; j < pInfo->node_connection_count; j++)
		{
			MAP_PUPPY_NODE_CONNECTION* pConn = &pInfo->node_connection_list[j];

			// Skip mismatches
			if (pConn->one != iBestNode)
				continue;
			
			iConList[iConCount++] = j;

			// Stop if we're at the limit
			if (iConCount + 1 >= 128)
				break;
		}

		if (iConCount)
		{
			MAP_PUPPY_NODE_CONNECTION* pConn = &pInfo->node_connection_list[iConList[Random(0, iConCount - 1)]];

			*tgt_x     = pInfo->node_list[pConn->two].x * 0x2000;
			*tgt_y     = pInfo->node_list[pConn->two].y * 0x2000;
			*pMoveType = pConn->move_type_to;
			
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Puppy_PutString(int x, int y, const char* str, int r, int g, int b)
{
	static RECT rcClient = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	static RECT rect[]   =
	{
		{0,0,7,7},{7,0,14,7},{14,0,21,7},{21,0,28,7},{28,0,35,7},{35,0,42,7},{42,0,49,7},{49,0,56,7},{56,0,63,7},{63,0,70,7},{70,0,77,7},{77,0,84,7},{84,0,91,7},{91,0,98,7},{98,0,105,7},{105,0,112,7},{0,7,7,14},{7,7,14,14},{14,7,21,14},{21,7,28,14},{28,7,35,14},{35,7,42,14},{42,7,49,14},{49,7,56,14},{56,7,63,14},{63,7,70,14},{70,7,77,14},{77,7,84,14},{84,7,91,14},{91,7,98,14},{98,7,105,14},{105,7,112,14},{0,14,7,21},{7,14,14,21},{14,14,21,21},{21,14,28,21},{28,14,35,21},{35,14,42,21},{42,14,49,21},{49,14,56,21},{56,14,63,21},{63,14,70,21},{70,14,77,21},{77,14,84,21},{84,14,91,21},{91,14,98,21},{98,14,105,21},{105,14,112,21},{0,21,7,28},{7,21,14,28},{14,21,21,28},{21,21,28,28},{28,21,35,28},{35,21,42,28},{42,21,49,28},{49,21,56,28},{56,21,63,28},{63,21,70,28},{70,21,77,28},{77,21,84,28},{84,21,91,28},{91,21,98,28},{98,21,105,28},{105,21,112,28},{0,28,7,35},{7,28,14,35},{14,28,21,35},{21,28,28,35},{28,28,35,35},{35,28,42,35},{42,28,49,35},{49,28,56,35},{56,28,63,35},{63,28,70,35},{70,28,77,35},{77,28,84,35},{84,28,91,35},{91,28,98,35},{98,28,105,35},{105,28,112,35},{0,35,7,42},{7,35,14,42},{14,35,21,42},{21,35,28,42},{28,35,35,42},{35,35,42,42},{42,35,49,42},{49,35,56,42},{56,35,63,42},{63,35,70,42},{70,35,77,42},{77,35,84,42},{84,35,91,42},{91,35,98,42},{98,35,105,42},{105,35,112,42},{0,42,7,49},{7,42,14,49},{14,42,21,49},{21,42,28,49},{28,42,35,49},{35,42,42,49},{42,42,49,49},{49,42,56,49},{56,42,63,49},{63,42,70,49},{70,42,77,49},{77,42,84,49},{84,42,91,49},{91,42,98,49},{98,42,105,49},{105,42,112,49},{0,49,7,56},{7,49,14,56},{14,49,21,56},{21,49,28,56},{28,49,35,56},{35,49,42,56},{42,49,49,56},{49,49,56,56},{56,49,63,56},{63,49,70,56},{70,49,77,56},{77,49,84,56},{84,49,91,56},{91,49,98,56},{98,49,105,56}
	};

	RECT pDestRect;

	pDestRect.left   = x;
	pDestRect.right  = x + 7;
	pDestRect.top    = y + 1;
	pDestRect.bottom = y + 8;

	// Draw the shadow first
	ModBitmap(SURFACE_ID_LEVEL_SPRITESET_2, CSM_RGB(153, 122, 0));
	for (const char* ptr = str; *ptr != 0; ptr++)
	{
		if (*ptr < ' ')
			continue;

		pDestRect.left  = (x + (9 * (ptr - str))) + 1;
		pDestRect.right = pDestRect.left + 7;

		PutBitmap15(&pDestRect, &rect[(*ptr) - ' '], SURFACE_ID_LEVEL_SPRITESET_2);
	}

	pDestRect.top    = y;
	pDestRect.bottom = y + 7;

	// Draw the actual text
	ModBitmap(SURFACE_ID_LEVEL_SPRITESET_2, CSM_RGB(255, 214, 48));
	for (const char* ptr = str; *ptr != 0; ptr++)
	{
		if (*ptr < ' ')
			continue;

		pDestRect.left  = x + (9 * (ptr - str));
		pDestRect.right = pDestRect.left + 7;

		PutBitmap15(&pDestRect, &rect[(*ptr) - ' '], SURFACE_ID_LEVEL_SPRITESET_2);
	}

	ClearBitmapMod(SURFACE_ID_LEVEL_SPRITESET_2);
}

unsigned int Puppy_GetStringWidth(const char* str)
{
	if (!*str)
		return 0;

	return (strlen(str) * 9) - 2;
}

void Puppy_PutNumber(int x, int y, int value, bool draw_colon)
{
	static RECT rcClient = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	static RECT rect[10] =
	{
		DRECT(208, 0, 8, 8),
		DRECT(216, 0, 8, 8),
		DRECT(224, 0, 8, 8),
		DRECT(232, 0, 8, 8),
		DRECT(240, 0, 8, 8),
		DRECT(248, 0, 8, 8),
		DRECT(256, 0, 8, 8),
		DRECT(264, 0, 8, 8),
		DRECT(272, 0, 8, 8),
		DRECT(280, 0, 8, 8),
	};

	static RECT colon = DRECT(200, 0, 8, 8);

	if (value == 0)
	{
		if (draw_colon)
		{
			PutBitmap3(&rcClient, PixelToScreenCoord(x), PixelToScreenCoord(y), &colon, SURFACE_ID_LEVEL_SPRITESET_2);
			PutBitmap3(&rcClient, PixelToScreenCoord(x + 9), PixelToScreenCoord(y), &rect[0], SURFACE_ID_LEVEL_SPRITESET_2);
			PutBitmap3(&rcClient, PixelToScreenCoord(x + 18), PixelToScreenCoord(y), &rect[0], SURFACE_ID_LEVEL_SPRITESET_2);
		}
		else
			PutBitmap3(&rcClient, PixelToScreenCoord(x), PixelToScreenCoord(y), &rect[0], SURFACE_ID_LEVEL_SPRITESET_2);

		return;
	}

	int iValue = value;
	int iMod = 1;
	int iOffset = 0;

	while (iValue / iMod > 0)
		iMod *= 10;

	iMod /= 10;

	if (draw_colon)
	{
		PutBitmap3(&rcClient, PixelToScreenCoord(x + 9 * iOffset), PixelToScreenCoord(y), &colon, SURFACE_ID_LEVEL_SPRITESET_2);
		++iOffset;

		if (value < 10)
		{
			PutBitmap3(&rcClient, PixelToScreenCoord(x + 9 * iOffset), PixelToScreenCoord(y), &rect[0], SURFACE_ID_LEVEL_SPRITESET_2);
			++iOffset;
		}
	}

	while (iMod != 0)
	{
		PutBitmap3(&rcClient, PixelToScreenCoord(x + 9 * iOffset), PixelToScreenCoord(y), &rect[((iValue / iMod) % 10)], SURFACE_ID_LEVEL_SPRITESET_2);

		++iOffset;
		iMod /= 10;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CTP_OnClientConnected(CaveNet::DataStructures::NetClient* pClient)
{
	pClient->mCache.collectables_shown = false;
	pClient->mCache.collectable_count  = 0;

	pClient->TransmitInventory(0xFFFF);
}

void CTP_OnClientDisconnected(CaveNet::DataStructures::NetClient* pClient)
{
	CSM_Log("[MOD] %s disconnected!\r\n", pClient->mCache.net_name);
}

void CTP_OnClientSpawn(CaveNet::DataStructures::NetClient* pClient)
{
	if (pClient->GetPlayer())
	{
		if (pClient->mIsBot)
			pClient->GetPlayer()->StartAI();

		pClient->GetPlayer()->ShowMyChar(true);

		if (!pClient->mCache.is_ghost)
			pClient->TransmitPlayerUpdate();
	}

	//CSM_Log("[MOD] %s spawned!\r\n", pClient->mCache.net_name);
}

void CTP_OnClientDie(CaveNet::DataStructures::NetClient* pClient, PlayerDeathType iDeathType, NPCHAR* pKillerNpc, CaveNet::DataStructures::NetClient* pKillerClient)
{
	int iDropType = Random(0, 3);

	switch (iDropType)
	{
		// Ammo
		case 0:
		{
			SetNpChar_Backwards(86, pClient->GetMyChar()->x, pClient->GetMyChar()->y, 0, 0, 0, NULL, NPC_MAX - 1, NULL);
			break;
		}
		// Health
		case 1:
		{
			SetNpChar_Backwards(87, pClient->GetMyChar()->x, pClient->GetMyChar()->y, 0, 0, 0, NULL, NPC_MAX - 1, NULL);
			break;
		}
		// Experience
		case 2:
		{
			SetNpChar_Backwards(1, pClient->GetMyChar()->x, pClient->GetMyChar()->y, 0, 0, 0, NULL, NPC_MAX - 1, NULL);
			break;
		}
	}

	if (pClient->GetTeam())
		pClient->Respawn(CTP_GetTeamInfo(pClient->GetTeam()->mTeamType)->player_respawn_time);
	else
		pClient->Respawn(1000);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CTP_OnTeamAddMember(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetClient* pServerClient, CaveNet::DataStructures::NetTeam* pTeam)
{
	if (!pClient)
		return;

	if (pClient->GetGhostId() == CaveNet_Client_GetGhostId())
		gOurCurrentTeamType = pTeam->mTeamType;
}

void CTP_OnTeamRemoveMember(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetClient* pServerClient, CaveNet::DataStructures::NetTeam* pTeam)
{
	if (!pClient)
		return;

	if (pClient->GetGhostId() == CaveNet_Client_GetGhostId())
		gOurCurrentTeamType = (CaveNet::DataStructures::NetTeam::TeamType)-1;

	if (CaveNet_Server_IsHosting())
	{
		if (pServerClient)
		{
			if (pServerClient->mCache.collectable_count)
			{
				gNPC[pServerClient->mCache.collectable_count].client = NULL;
				CTP_DropPuppy(&gNPC[pServerClient->mCache.collectable_count]);
			}

			pServerClient->mCache.collectables_shown = false;
			pServerClient->mCache.collectable_count  = 0;
		}
		else if (pClient->mCache.collectable_count)
		{
			gNPC[pClient->mCache.collectable_count].client = NULL;
			CTP_DropPuppy(&gNPC[pClient->mCache.collectable_count]);
		}
	}

	pClient->mCache.collectables_shown = false;
	pClient->mCache.collectable_count  = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CTP_OnHitMyCharNpChar(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc)
{
	// Test for validity
	if (!(pNpc->bits & NPC_flags::npc_option3))
		return;

	switch (pNpc->code_char)
	{
		// Puppy
		case 411:
		{
			CTP_PickupPuppy(pClient, pPlayer, pMC, pNpc);
			break;
		}
		// Weapon Spawn
		case 415:
		{
			CTP_PickupWeapon(pClient, pPlayer, pMC, pNpc);
			break;
		}
		// No Puppy Zone
		case 416:
		{
			CTP_HandleNoPuppyZone(pClient, pPlayer, pMC, pNpc);
			break;
		}
		// Kill Zone
		case 417:
		{
			CTP_HandleKillZone(pClient, pPlayer, pMC, pNpc);
			break;
		}
		// Resupply PC
		case 418:
		{
			CTP_HandleResupplyCabinet(pClient, pPlayer, pMC, pNpc);
			break;
		}
	}

	// test for redundency
	//CSM_Log("CTP_OnHitMyCharNpChar(0x%08X, 0x%08X, 0x%08X<%s>) - Called [%08d]!\r\n", (unsigned int)pClient, (unsigned int)pPlayer, (unsigned int)pMC, (pMC == &gMC ? "gMC" : "extern"), GetTicks());
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CTP_OnPlayerPickupExp(NPCHAR* pNpc, CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, int iAmount)
{
	if (pMC->life < 1)
		return FALSE;

	return TRUE;
}

BOOL CTP_OnPlayerPickupHeart(NPCHAR* pNpc, CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, int iAmount)
{
	if (pMC->life < 1)
		return FALSE;

	return TRUE;
}

BOOL CTP_OnPlayerPickupMissile(NPCHAR* pNpc, CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC, int iAmount)
{
	if (pMC->life < 1)
		return FALSE;

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* GetTabToken(const char* pString, char* pBuffer, int iTokenIndex, const char* pDefaultValue = "")
{
	// Init the output buffer
	int iCurrentToken = 0;
	char* pOut        = pBuffer;
	*pOut             = 0;

	for (const char* pPtr = pString; *pPtr != 0; pPtr++)
	{
		if (*pPtr == '\t' || *pPtr == ' ')
		{
		SEEK_NEXT_TOKEN:
			if (pPtr != pString)
				++iCurrentToken;

			// Get to the next token.
			for (; *pPtr != 0; pPtr++)
				if (*pPtr > ' ')
					break;

			--pPtr;

			continue;
		}

		// Skip this token if we don't want it
		if (iCurrentToken != iTokenIndex)
		{
			for (; *pPtr != 0; pPtr++)
				if (*pPtr <= ' ')
					break;

			// Safe guard
			if (*pPtr == 0)
				break;

			goto SEEK_NEXT_TOKEN;
		}

		const char* pStart = pPtr;

		// Found it!
		for (; *pPtr != 0; pPtr++)
		{
			if (*pPtr <= ' ')
				break;

			*pOut++ = *pPtr;
		}

		// Null terminate it
		*pOut++ = 0;

		// Done!
		return pStart;
	}

	strcpy(pBuffer, pDefaultValue);

	return NULL;
}

static bool GetFieldValue(const char* pString, const char* pFieldName, char* pBuffer, const char* pDefaultValue = "")
{
	char tmp_buf[128];
	int iTokenIndex = 2;

	while (GetTabToken(pString, tmp_buf, iTokenIndex) != NULL)
	{
		// Skip mismatches
		if (strcmp(tmp_buf, pFieldName))
		{
			++iTokenIndex;
			continue;
		}

		return GetTabToken(pString, pBuffer, iTokenIndex + 1, pDefaultValue) != NULL;
	}

	strcpy(pBuffer, pDefaultValue);

	return false;
}

void CTP_CleanupCoverSurfaces()
{
	if (!gTeamInfoList)
		return;

	if (gCacheSurfaceList)
	{
		for (int j = 0; j < gCacheSurfaceCount; j++)
		{
			free(gCacheSurfaceList[j].file_name);
			CacheSurface::FreeSurfaceID(gCacheSurfaceList[j].id);
		}

		free(gCacheSurfaceList);
		gCacheSurfaceList = NULL;
		gCacheSurfaceCount = 0;
	}

	if (gPublicCoverImageList)
	{
		free(gPublicCoverImageList);
		gPublicCoverImageList  = NULL;
		gPublicCoverImageCount = 0;
	}

	if (gTeamAudioList)
	{
		// Unload all sounds
		for (int i = 0; i < gTeamAudioCount; i++)
			CSM_UnloadCustomSound(gTeamAudioList[i].id);

		free(gTeamAudioList);
		gTeamAudioList  = NULL;
		gTeamAudioCount = 0;
	}
	
	if (gTeamInfoList)
	{
		for (int i = 0; i < gTeamInfoCount; i++)
		{
			MAP_TEAM_INFO* pInfo = &gTeamInfoList[i];

			for (int j = 0; j < pInfo->node_count; j++)
			{
				if (pInfo->node_list[j].conn_list)
					free(pInfo->node_list[j].conn_list);
			}

			// Now free the cover list itself
			if (pInfo->node_ignore_list)
				free(pInfo->node_ignore_list);

			if (pInfo->cover_list)
				free(pInfo->cover_list);

			if (pInfo->node_list)
				free(pInfo->node_list);
			
			if (pInfo->node_connection_list)
				free(pInfo->node_connection_list);
		}

		free(gTeamInfoList);

		gTeamInfoList  = NULL;
		gTeamInfoCount = 0;
	}

	if (g_AI_NodeInfo.node_list)
	{
		for (int i = 0; i < g_AI_NodeInfo.node_count; i++)
		{
			if (g_AI_NodeInfo.node_list[i].conn_list)
				free(g_AI_NodeInfo.node_list[i].conn_list);
		}

		free(g_AI_NodeInfo.node_list);
	}

	if (g_AI_NodeInfo.node_connection_list)
		free(g_AI_NodeInfo.node_connection_list);

	if (g_AI_NodeInfo.node_ignore_list)
		free(g_AI_NodeInfo.node_ignore_list);

	memset(&g_AI_NodeInfo, 0, sizeof(MAP_TEAM_INFO));
}

#define SET_SCRIPT_ERROR(FORMAT, ...) \
	iERROR_LINE_NO = iLineNo; \
	bERROR         = true; \
	sprintf_s(pERROR_BUFFER, sizeof(pERROR_BUFFER), FORMAT, __VA_ARGS__);

void CTP_OnStageChange(const char* pStageName, int iAreaIndex)
{
	strcpy(g_CurrentStage_Name, pStageName);

	// Cleanup old surfaces
	CTP_CleanupCoverSurfaces();

	// Cleanup old teams
	CaveNet::DataStructures::NetTeam::ClearAllTeams();

	char pBuffer[1024];
	char pFilePath[1024];
	char pMapConfigPath[1024];

	// Error stuff
	char pERROR_BUFFER[1024];
	bool bERROR        = false;
	int iERROR_LINE_NO = 0;

	// Load this stage's configuration
	sprintf(pBuffer, "ctp/Stage/Covers/%s.cfg", pStageName);
	sprintf(pFilePath, "ctp/Stage/Covers/%s.cfg", pStageName);
	sprintf(pMapConfigPath, "ctp/Stage/Covers/%s/", pStageName);

	FILE* fp;
	char line[2048]  = { 0 };
	size_t len       = 0;
	size_t read      = 0;
	int iLineNo      = -1;

	fp = fopen(pBuffer, "r");
	if (fp == NULL)
		return;

	MAP_TEAM_INFO* pCurrentInfo    = NULL;
	MAP_COVER_IMAGE* pCurrentImage = NULL;
	MAP_AUDIO_FILE* pCurrentAudio  = NULL;

	// Read map config
	while (fgets(line, 2048, fp))
	{
		++iLineNo;

		char pCommand[32];
		char pArg1[260];
		char pArg2[260];
		char pArg3[260];
		char pArg4[260];

		if (strchr(line, '\r'))
			* strchr(line, '\r') = NULL;
		if (strchr(line, '\n'))
			*strchr(line, '\n') = NULL;

		GetTabToken(line, pCommand, 0);

		// Parse the instruction
		if (pCurrentImage)
		{
			if (!strcmp(pCommand, "END_COVER"))
			{
				pCurrentImage = NULL;
			}
			else if (!strcmp(pCommand, "SET_DRAW_OP"))
			{
				// Make sure it makes sense to be using this command
				if (!pCurrentInfo)
				{
					SET_SCRIPT_ERROR("Found a '%s' command, but the cover image object we're defining isn't inside of a team object...", pCommand);
					break;
				}

				// Get arguments
				GetTabToken(line, pArg1, 1); // Draw Requisite Type

				// Process
				if (!strcmpi(pArg1, "ONLY_DRAW_FOR_TEAM_MEMBERS"))
					pCurrentImage->show_for_team_members = true;
				else if (!strcmpi(pArg1, "DO_NOT_DRAW_FOR_TEAM_MEMBERS"))
					pCurrentImage->show_for_team_members = false;
				else
				{
					SET_SCRIPT_ERROR("Unknown draw requisite type \"%s\".\n\nValid types:\n\n"
						"\"ONLY_DRAW_FOR_TEAM_MEMBERS\"   : Only draws this cover image for clients who are in this team.\n"
						"\"DO_NOT_DRAW_FOR_TEAM_MEMBERS\" : Only draws this cover image for clients who are not in this team.", pArg1
					);
					break;
				}
			}
			else if (!strcmp(pCommand, "SET_LAYER"))
			{
				// Set draw requisite

				// Get arguments
				GetTabToken(line, pArg1, 1); // Layer type

				// Process
				if (!strcmpi(pArg1, "BACKGROUND"))
					pCurrentImage->background = true;
				else if (!strcmpi(pArg1, "FOREGROUND"))
					pCurrentImage->background = false;
				else
				{
					SET_SCRIPT_ERROR("Unknown renderlayer type \"%s\"\n\n"
						"\"BACKGROUND\" : Draws the image in the background.\n"
						"\"FOREGROUND\" : Draws the image in the foreground.", pArg1);
					break;
				}
			}
			else if (!strcmp(pCommand, "SET_OFFSET"))
			{
				// Set team name

				// Get arguments
				GetTabToken(line, pArg1, 1); // Map Tile X
				GetTabToken(line, pArg2, 2); // Map Tile Y
				GetFieldValue(line, "RELATIVE_TO_NPC", pArg3, "0"); // Npc Event Number

				int iTileX = CLAMP(atoi(pArg1), 0, gMap.width) * 0x2000;
				int iTileY = CLAMP(atoi(pArg2), 0, gMap.length) * 0x2000;
				int iRelEvent = atoi(pArg3);

				if (iRelEvent)
				{
					for (int i = 0; i < NPC_MAX; i++)
					{
						if ((gNPC[i].cond & 0x80) && gNPC[i].code_event == iRelEvent)
						{
							// Found it
							iTileX += 0x2000 * unsigned int(gNPC[i].x / 0x2000);
							iTileY += 0x2000 * unsigned int(gNPC[i].y / 0x2000);

							break;
						}
					}
				}

				// Process
				pCurrentImage->off_x = iTileX;
				pCurrentImage->off_y = iTileY;
			}
			else if (!strcmp(pCommand, "LOAD_IMAGE"))
			{
				// Load the image

				// Get arguments
				strcpy(pArg1, GetTabToken(line, pArg2, 1)); // File name

				// Append it onto the local file path
				sprintf_s(pBuffer, sizeof(pBuffer), "ctp/Stage/Covers/%s", pArg1);

				// Make sure it doesn't already exist
				int j = 0;
				for (j = 0; j < gCacheSurfaceCount; j++)
				{
					if (!strcmpi(gCacheSurfaceList[j].file_name, pBuffer))
						break;
				}

				if (j != gCacheSurfaceCount)
				{
					// Already loaded!
					pCurrentImage->id = gCacheSurfaceList[j].id;

					// Setup the source
					pCurrentImage->src_rect = { 0, 0, CacheSurface::GetSurfaceIDSize(pCurrentImage->id).x, CacheSurface::GetSurfaceIDSize(pCurrentImage->id).y };

					continue;
				}

				// Obtain a free surface ID
				if ((pCurrentImage->id = CacheSurface::GetNextFreeSurfaceID()) == SURFACE_ID_MAX)
				{
					CSM_Log("[WARN] Failed to find a free surface ID!\r\n");
					continue;
				}

				// Load the image
				if (!CacheSurface::LoadFileToSurface(pBuffer, pCurrentImage->id))
				{
					CSM_Log("[WARN] Failed to load image \"%s\"!\r\n", pBuffer);
					pCurrentImage->id = SURFACE_ID_MAX;
					continue;
				}

				// Allocate a new image
				if (!gCacheSurfaceList)
				{
					gCacheSurfaceCount = 1;
					gCacheSurfaceList = (MAP_COVER_IMAGE_CACHE*)malloc(sizeof(MAP_COVER_IMAGE_CACHE));
				}
				else
				{
					gCacheSurfaceCount += 1;
					gCacheSurfaceList = (MAP_COVER_IMAGE_CACHE*)realloc((void*)gCacheSurfaceList, sizeof(MAP_COVER_IMAGE_CACHE) * gCacheSurfaceCount);
				}

				gCacheSurfaceList[gCacheSurfaceCount - 1].id = pCurrentImage->id;
				gCacheSurfaceList[gCacheSurfaceCount - 1].file_name = strdup(pBuffer);

				// Bypass a bug with GetSurfaceIDSize
				CacheSurface::SetSurfaceID(SURFACE_ID_RENDERER);

				// Setup the source
				pCurrentImage->src_rect = { 0, 0, CacheSurface::GetSurfaceIDSize(pCurrentImage->id).x, CacheSurface::GetSurfaceIDSize(pCurrentImage->id).y };
			}
			else
			{
				SET_SCRIPT_ERROR("Unexpected command \"%s\"...", pCommand);
				break;
			}
		}
		else if (pCurrentAudio)
		{
			if (!strcmp(pCommand, "END_AUDIO"))
			{
				// End audio file
				if (pCurrentAudio->id == -1)
				{
					SET_SCRIPT_ERROR("Ending audio object without loading any audio file!!");
					break;
				}

				// Done with this audio file
				pCurrentAudio = NULL;
			}
			else if (!strcmp(pCommand, "FILE"))
			{
				// Load file

				// Get arguments
				strcpy(pArg1, pMapConfigPath);
				strcat(pArg1, GetTabToken(line, pArg2, 1)); // File name

				// Process
				if (pCurrentAudio->id != -1)
					CSM_UnloadCustomSound(pCurrentAudio->id);

				// Attepmt to load it
				if ((pCurrentAudio->id = CSM_LoadCustomSound(pArg1)) == -1)
				{
					SET_SCRIPT_ERROR("Failed to find audio file \"%s\".", pArg1);
					break;
				}
			}
			else if (!strcmp(pCommand, "ALWAYS_ON_SCREEN"))
			{
				// Always on screen

				// Get arguments
				GetTabToken(line, pArg1, 1, "0"); // Boolean

				// Process
				pCurrentAudio->always_on_screen = (!strcmpi(pArg1, "true") || atoi(pArg1) == 1 ? true : false);
			}
			else if (!strcmp(pCommand, "RADIUS"))
			{
				// Set audio radius

				// Get arguments
				GetTabToken(line, pArg1, 1, "0");
				GetTabToken(line, pArg2, 2, "PIXELS");

				// Process
				if (!strcmpi(pArg2, "PIXELS") || !strcmpi(pArg2, "PIXEL"))
				{
					pCurrentAudio->radius = ((int)fabsf(atoi(pArg1))) * 0x400;
				}
				else if (!strcmpi(pArg2, "TILES") || !strcmpi(pArg2, "TILE"))
				{
					pCurrentAudio->radius = ((int)fabsf(atoi(pArg1))) * 0x4000;
				}
				else
				{
					SET_SCRIPT_ERROR("Unknown unit type \"%s\".\n\nValid types:\n\n"
							"\"PIXEL\"     : Specify absolute pixel radius.\n"
							"\"TILE\"      : Specify tile radius."
							, pArg1
						);
					break;
				}
			}
			else if (!strcmp(pCommand, "POSITION"))
			{
				// Set audio position

				// Get arguments
				GetTabToken(line, pArg1, 1, "0");
				GetTabToken(line, pArg2, 2, "0");
				GetTabToken(line, pArg3, 3, "PIXELS");

				if (*pArg1 == '(')
				{
					strcpy(pBuffer, pArg1 + 1);
					strcpy(pArg1, pBuffer);
				}

				if (pArg1[strlen(pArg1) - 1] == ',')
					pArg1[strlen(pArg1) - 1] = 0;
				
				if (pArg1[strlen(pArg2) - 1] == ')')
					pArg1[strlen(pArg2) - 1] = 0;

				// Process
				if (!strcmpi(pArg3, "PIXELS") || !strcmpi(pArg3, "PIXEL"))
				{
					pCurrentAudio->x = ((int)fabsf(atoi(pArg1))) * 0x200;
					pCurrentAudio->y = ((int)fabsf(atoi(pArg2))) * 0x200;
				}
				else if (!strcmpi(pArg3, "TILES") || !strcmpi(pArg3, "TILE"))
				{
					pCurrentAudio->x = ((int)fabsf(atoi(pArg1))) * 0x2000;
					pCurrentAudio->y = ((int)fabsf(atoi(pArg2))) * 0x2000;
				}
				else
				{
					SET_SCRIPT_ERROR("Unknown unit type \"%s\".\n\nValid types:\n\n"
						"\"PIXEL\"     : Specify absolute pixel position.\n"
						"\"TILE\"      : Specify tile position."
						, pArg1
					);
					break;
				}
			}
			else
			{
				SET_SCRIPT_ERROR("Unexpected command \"%s\"...", pCommand);
				break;
			}
		}
		else if (pCurrentInfo)
		{
			if (!strcmp(pCommand, "SET_NAME"))
			{
				// Set team name
				
				// Get arguments
				strcpy(pArg1, GetTabToken(line, pArg2, 1)); // File name

				// Process
				pCurrentInfo->team->SetName(pArg1);
			}
			else if (!strcmp(pCommand, "SET_MEMBER_LIMIT"))
			{
				// Add a team condition flag
				
				// Get arguments
				GetTabToken(line, pArg1, 1, "0"); // File name

				// Process
				pCurrentInfo->team->mConfig.max_members = atoi(pArg1);
			}
			else if (!strcmp(pCommand, "ADD_TEAM_COND"))
			{
				// Add a team condition flag
				
				// Get arguments
				GetTabToken(line, pArg1, 1, ""); // File name

				// Process
				if (!strcmpi(pArg1, "CTP_FLAG_CANNOT_PICKUP_FLAG"))				pCurrentInfo->flags |= CTP_TeamFlags::CTP_FLAG_CANNOT_PICKUP_FLAG;
				else if (!strcmpi(pArg1, "CTP_FLAG_CANNOT_PICKUP_WEAPONS"))		pCurrentInfo->flags |= CTP_TeamFlags::CTP_FLAG_CANNOT_PICKUP_WEAPONS;
				else if (!strcmpi(pArg1, "CTP_FLAG_TOUCH_FLAG_TO_RETURN"))		pCurrentInfo->flags |= CTP_TeamFlags::CTP_FLAG_TOUCH_FLAG_TO_RETURN;
				else if (!strcmpi(pArg1, "CTP_IGNORE_AUTOBALANCE"))				pCurrentInfo->team->mConfig.allow_autobalance = false;
				else
				{
					SET_SCRIPT_ERROR("Unknown team condition \"%s\".\n\nValid types:\n\n"
								"\"CTP_FLAG_CANNOT_PICKUP_FLAG\"    : Team members cannot pickup flags.\n"
								"\"CTP_FLAG_CANNOT_PICKUP_WEAPONS\" : Team members cannot pickup weapons.\n"
								"\"CTP_FLAG_TOUCH_FLAG_TO_RETURN\"  : Team members can touch their flag to instantly return it to their base.\n"
								"\"CTP_IGNORE_AUTOBALANCE\"         : This team is ignored in auto balancing."
								, pArg1
							);
					break;
				}
			}
			else if (!strcmp(pCommand, "SET_TYPE"))
			{
				// Set team type
				
				// Get arguments
				GetTabToken(line, pArg1, 1);

				// Process
				if (!strcmpi(pArg1, "TEAM_YELLOW"))
					pCurrentInfo->team_type = CaveNet::DataStructures::NetTeam::TeamType::TEAM_YELLOW;
				else if (!strcmpi(pArg1, "TEAM_GREEN"))
					pCurrentInfo->team_type = CaveNet::DataStructures::NetTeam::TeamType::TEAM_GREEN;
				else if (!strcmpi(pArg1, "TEAM_BLUE"))
					pCurrentInfo->team_type = CaveNet::DataStructures::NetTeam::TeamType::TEAM_BLUE;
				else if (!strcmpi(pArg1, "TEAM_RED"))
					pCurrentInfo->team_type = CaveNet::DataStructures::NetTeam::TeamType::TEAM_RED;
				else
				{
					SET_SCRIPT_ERROR("Unknown team type \"%s\".\n\nValid types:\n\n"
								"\"TEAM_YELLOW\" : Yellow team\n"
								"\"TEAM_GREEN\"  : Green team\n"
								"\"TEAM_BLUE\"   : Blue team\n"
								"\"TEAM_RED\"    : Red team"
								, pArg1
							);
					break;
				}

				pCurrentInfo->team->mTeamType = pCurrentInfo->team_type;
				pCurrentInfo->team->SetNpcSpawnIndex(pCurrentInfo->team_type);

				if (pCurrentInfo->team->GetColor() == GUI_COLOR(255, 255, 255))
					pCurrentInfo->team->SetColor(pTeamColors[pCurrentInfo->team_type]);
			}
			else if (!strcmp(pCommand, "SET_MAX_LIFE"))
			{
				// Set maximum life

				// Get arguments
				GetTabToken(line, pArg1, 1, "1");

				// Process
				pCurrentInfo->team->mConfig.max_life = MAX(atoi(pArg1), 1);
			}
			else if (!strcmp(pCommand, "SET_RESPAWN_TIME"))
			{
				// Set team type

				// Get arguments
				GetTabToken(line, pArg1, 1, "0"); // Time in ms

				// Process
				pCurrentInfo->player_respawn_time = atoi(pArg1);
			}
			else if (!strcmp(pCommand, "SET_FLAG_EXPIRE_TIME"))
			{
				// Set flag expiration time

				// Get arguments
				GetTabToken(line, pArg1, 1, "0"); // Time in ms

				// Process
				pCurrentInfo->puppy_respawn_time = atoi(pArg1);
			}
			else if (!strcmp(pCommand, "SET_COLOR"))
			{
				// Set team type

				// Get arguments
				GetTabToken(line, pArg1, 1, "0"); // Red
				GetTabToken(line, pArg2, 2, "0"); // Green
				GetTabToken(line, pArg3, 3, "0"); // Blue

				// Process
				pCurrentInfo->team->SetColor(GUI_COLOR(atoi(pArg1), atoi(pArg2), atoi(pArg3)));
			}
			else if (!strcmp(pCommand, "ADD_TECHNOLOGY"))
			{
				// Add equip flags

				// Get arguments
				GetTabToken(line, pArg1, 1); // Internal equip flag name

				// Process
				unsigned int iFlags = 0;

				if (!strcmpi(pArg1, "technology_air_tank"))				iFlags |= GMC_equipflags::gmc_eflag_air_tank;
				else if (!strcmpi(pArg1, "technology_arms_barrier"))	iFlags |= GMC_equipflags::gmc_eflag_arms_barrier;
				else if (!strcmpi(pArg1, "technology_booster_08"))		iFlags |= GMC_equipflags::gmc_eflag_booster_08;
				else if (!strcmpi(pArg1, "technology_booster_2"))		iFlags |= GMC_equipflags::gmc_eflag_booster_2;
				else if (!strcmpi(pArg1, "technology_map_system"))		iFlags |= GMC_equipflags::gmc_eflag_map_system;
				else if (!strcmpi(pArg1, "technology_mimiga_mask"))		iFlags |= GMC_equipflags::gmc_eflag_mimiga_mask;
				else if (!strcmpi(pArg1, "technology_timer"))			iFlags |= GMC_equipflags::gmc_eflag_nikumaru_timer;
				else if (!strcmpi(pArg1, "technology_turbo_charge"))	iFlags |= GMC_equipflags::gmc_eflag_turbocharge;
				else if (!strcmpi(pArg1, "technology_whimsical_star"))	iFlags |= GMC_equipflags::gmc_eflag_whimsical_star;
				else
				{
					SET_SCRIPT_ERROR("Unknown team technology type \"%s\".\n\nValid types:\n\n"
								"\"technology_air_tank\"       : Grants Curly's air tank.\n"
								"\"technology_arms_barrier\"   : Grants Arms Barrier.\n"
								"\"technology_booster_08\"     : Grants Booster v0.8.\n"
								"\"technology_booster_2\"      : Grants Booster v2.\n"
								"\"technology_map_system\"     : Grants Map System.\n"
								"\"technology_mimiga_mask\"    : Grants Mimiga Mask.\n"
								"\"technology_turbo_charge\"   : Grants Turbo Charge.\n"
								"\"technology_whimsical_star\" : Grants Whimsical Star."
								, pArg1
							);
					break;
				}

				// Process
				pCurrentInfo->team->mConfig.equip_flags |= iFlags;
			}
			else if (!strcmp(pCommand, "ADD_WEAPON"))
			{
				// Add equip flags

				// Get arguments
				GetTabToken(line, pArg1, 1);                 // Internal weapon name
				GetFieldValue(line, "MAX_AMMO", pArg2, "0"); // Max Ammo
				GetFieldValue(line, "LEVEL",	pArg3, "1"); // Level

				// Process
				int iWeaponId = 0;

				// Get weapon ID
				if (!strcmpi(pArg1, "weapon_snake"))			iWeaponId = 1;
				else if (!strcmpi(pArg1, "weapon_polar_star"))	iWeaponId = 2;
				else if (!strcmpi(pArg1, "weapon_fire_ball"))	iWeaponId = 3;
				else if (!strcmpi(pArg1, "weapon_machine_gun"))	iWeaponId = 4;
				else if (!strcmpi(pArg1, "weapon_rpg"))			iWeaponId = 5;
				else if (!strcmpi(pArg1, "weapon_bubbler"))		iWeaponId = 7;
				else if (!strcmpi(pArg1, "weapon_sword"))		iWeaponId = 9;
				else if (!strcmpi(pArg1, "weapon_super_rpg"))	iWeaponId = 10;
				else if (!strcmpi(pArg1, "weapon_nemesis"))		iWeaponId = 12;
				else if (!strcmpi(pArg1, "weapon_spur"))		iWeaponId = 13;
				else if (!strcmpi(pArg1, "weapon_agility"))		iWeaponId = 14;
				else
				{
					SET_SCRIPT_ERROR("Unknown weapon type \"%s\".\n\nValid types:\n\n"
								"\"weapon_snake\"       : Grants Snake.\n"
								"\"weapon_polar_star\"  : Grants Polar Star.\n"
								"\"weapon_fire_ball\"   : Grants Fireball.\n"
								"\"weapon_machine_gun\" : Grants Machine Gun.\n"
								"\"weapon_rpg\"         : Grants Rocket Launcher.\n"
								"\"weapon_bubbler\"     : Grants Bubbler.\n"
								"\"weapon_sword\"       : Grants Sword.\n"
								"\"weapon_super_rpg\"   : Grants Super Rocket Launcher.\n"
								"\"weapon_nemesis\"     : Grants Nemesis.\n"
								"\"weapon_spur\"        : Grants Spur.\n"
								"\"weapon_agility\"     : Grants Agility.\n"
								, pArg1
							);
					break;
				}

				// Parse the arguments further
				int iMaxAmmo = MAX(atoi(pArg2), 0);
				int iLevel   = CLAMP(atoi(pArg3), 1, 3);

				// Find a free slot
				int x = 0;
				for (x = 0; x < ARMS_MAX; x++)
					if (!pCurrentInfo->team->mConfig.spawn_weapons[x].weapon_id)
						break;

				// Couldn't find a free slot? Stop.
				if (x == ARMS_MAX)
				{
					CSM_Log("[WARN] Couldn't find a free weapon slot to add the new weapon to!\r\n");
					continue;
				}

				pCurrentInfo->team->mConfig.spawn_weapons[x].weapon_id = iWeaponId;
				pCurrentInfo->team->mConfig.spawn_weapons[x].ammo      = iMaxAmmo;
				pCurrentInfo->team->mConfig.spawn_weapons[x].max_ammo  = iMaxAmmo;
				pCurrentInfo->team->mConfig.spawn_weapons[x].level     = iLevel;
			}
			else if (!strcmp(pCommand, "END_TEAM"))
			{
				// Finalize the team

				pCurrentInfo->team->mSpawnPoints.Clear();
				pCurrentInfo->team->mSpawnPoints.Free();

				// Look for spawn points
				for (int i = 0; i < NPC_MAX; i++)
				{
					// Look for capture point
					if (!pCurrentInfo->team->mCaptureZoneNpc && (gNPC[i].cond & 0x80) && gNPC[i].code_char == 412 && gNPC[i].code_flag == pCurrentInfo->team->mTeamType)
					{
						// Init this capture point
						if (gNPC[i].act_no == 0)
							ActNpc412(&gNPC[i]);

						pCurrentInfo->team->mCaptureZoneNpc = &gNPC[i];
						pCurrentInfo->team->mFlagNpc        = gNPC[i].pNpc;
					}

					if ((gNPC[i].cond & 0x80) && gNPC[i].code_char == 392 && gNPC[i].code_flag == pCurrentInfo->team->mTeamType)
						pCurrentInfo->team->AddSpawnPoint(i);
				}

				// Import the nodes for this team.
				CTP_ImportNodes(gTeamInfoCount - 1);

				// Send the team to everybody
				if (CaveNet_Server_IsHosting())
					pCurrentInfo->team->UpdateTeam();

				// Balance teams
				CaveNet::DataStructures::NetTeam::BalanceTeams(false);

				// We no longer have a team
				pCurrentInfo = NULL;
			}
			else if (!strcmp(pCommand, "BEGIN_COVER"))
			{
				goto LBL_BEGIN_COVER;
			}
			else
			{
				SET_SCRIPT_ERROR("Unexpected command \"%s\"...", pCommand);
				break;
			}
		}
		else if (!strcmp(pCommand, "BEGIN_AUDIO"))
		{
			// Add audio file
			
			// Allocate a new slot
			if (!gTeamAudioList)
			{
				gTeamAudioCount = 1;
				gTeamAudioList  = (MAP_AUDIO_FILE*)malloc(sizeof(MAP_AUDIO_FILE) * gTeamAudioCount);
			}
			else
			{
				gTeamAudioCount += 1;
				gTeamAudioList   = (MAP_AUDIO_FILE*)realloc((void*)gTeamAudioList, sizeof(MAP_AUDIO_FILE) * gTeamAudioCount);
			}

			// Set it as active
			pCurrentAudio = &gTeamAudioList[gTeamAudioCount - 1];

			// Set defaults
			memset(pCurrentAudio, 0, sizeof(pCurrentAudio));
			pCurrentAudio->id               = -1;
			pCurrentAudio->always_on_screen = true;
		}
		else if (!strcmp(pCommand, "BEGIN_COVER"))
		{
		LBL_BEGIN_COVER:
			// Add cover image
			
			if (!pCurrentInfo)
			{
				// Allocate a new image
				if (!gPublicCoverImageList)
				{
					gPublicCoverImageCount = 1;
					gPublicCoverImageList  = (MAP_COVER_IMAGE*)malloc(sizeof(MAP_COVER_IMAGE));
				}
				else
				{
					gPublicCoverImageCount += 1;
					gPublicCoverImageList   = (MAP_COVER_IMAGE*)realloc((void*)gPublicCoverImageList, sizeof(MAP_COVER_IMAGE) * gPublicCoverImageCount);
				}

				// Set it as active
				pCurrentImage = &gPublicCoverImageList[gPublicCoverImageCount - 1];
			}
			else
			{
				// Allocate a new image
				if (!pCurrentInfo->cover_list)
				{
					pCurrentInfo->cover_count = 1;
					pCurrentInfo->cover_list  = (MAP_COVER_IMAGE*)malloc(sizeof(MAP_COVER_IMAGE));
				}
				else
				{
					pCurrentInfo->cover_count += 1;
					pCurrentInfo->cover_list   = (MAP_COVER_IMAGE*)realloc((void*)pCurrentInfo->cover_list, sizeof(MAP_COVER_IMAGE) * pCurrentInfo->cover_count);
				}

				// Set it as active
				pCurrentImage = &pCurrentInfo->cover_list[pCurrentInfo->cover_count - 1];
			}

			// Set defaults
			pCurrentImage->id                    = SURFACE_ID_MAX;
			pCurrentImage->off_x                 = 0;
			pCurrentImage->off_y                 = 0;
			pCurrentImage->show_for_team_members = false;
			pCurrentImage->background            = false;
			pCurrentImage->src_rect              = { 0, 0, 0, 0 };
		}
		else if (!strcmp(pCommand, "BEGIN_TEAM"))
		{
			// Allocate a new team
			
			if (!gTeamInfoList)
			{
				gTeamInfoCount = 1;
				gTeamInfoList  = (MAP_TEAM_INFO*)malloc(sizeof(MAP_TEAM_INFO));
			}
			else
			{
				gTeamInfoCount += 1;
				gTeamInfoList   = (MAP_TEAM_INFO*)realloc((void*)gTeamInfoList, sizeof(MAP_TEAM_INFO) * gTeamInfoCount);
			}

			pCurrentInfo = &gTeamInfoList[gTeamInfoCount - 1];

			// Zero mem the memory space safely.
			memset(pCurrentInfo, 0, sizeof(MAP_TEAM_INFO));

			pCurrentInfo->cover_list            = NULL;
			pCurrentInfo->cover_count           = 0;
			pCurrentInfo->node_list             = NULL;
			pCurrentInfo->node_count            = 0;
			pCurrentInfo->node_connection_list  = NULL;
			pCurrentInfo->node_connection_count = 0;
			pCurrentInfo->team_type             = CaveNet::DataStructures::NetTeam::TeamType::TEAM_YELLOW;
			pCurrentInfo->player_respawn_time   = 2000;
			pCurrentInfo->puppy_respawn_time    = 3000;

			// Create the team
			CaveNet_CreateTeam(CaveNet::DataStructures::NetTeam::GetNextFreeTeamId(!CaveNet_Server_IsHosting()), !CaveNet_Server_IsHosting(), (void*)&pCurrentInfo->team);

			// Configure it
			pCurrentInfo->team->SetName("UNKNOWN TEAM");
			pCurrentInfo->team->SetColor(GUI_COLOR(255, 255, 255));
			pCurrentInfo->team->SetNpcSpawnIndex(0);

			// Configure its NPCs
			pCurrentInfo->team->mTeamType       = CaveNet::DataStructures::NetTeam::TeamType::TEAM_YELLOW;
			pCurrentInfo->team->mCaptureZoneNpc = NULL;
			pCurrentInfo->team->mFlagNpc        = NULL;
		}
		else
		{
			SET_SCRIPT_ERROR("Unknown command \"%s\"...", pCommand);
			break;
		}
	}

	if (!bERROR)
	{
		if (pCurrentInfo)
		{
			SET_SCRIPT_ERROR("Expected 'END_TEAM'");
		}
		else if (pCurrentAudio)
		{
			SET_SCRIPT_ERROR("Expected 'END_AUDIO'");
		}
		else if (pCurrentImage)
		{
			SET_SCRIPT_ERROR("Expected 'END_COVER'");
		}
	}

	fclose(fp);

	if (bERROR)
	{
		CaveNet::DataStructures::NetTeam::ClearAllTeams();

		char pEntireError[4096];

		sprintf(pEntireError,
			"A map's configuration file had an error in it and needs to be resolved.\n\n"
			"Config File:   \"%s\"\n"
			"Line No#:      %d\n"
			"Error Message:\n%s",
			pFilePath, iERROR_LINE_NO, pERROR_BUFFER
		);

		CTP_CleanupCoverSurfaces();

		CSM_Explode(0x102, "CTP_MAP_CONFIG_ERROR", pEntireError, false);
		return;
	}

	// Load AI nodes
	CTP_ImportNodes(-1);

	// Play sounds
	for (int i = 0; i < gTeamAudioCount; i++)
	{
		// Play sound loop
		PlaySoundObject2D(gTeamAudioList[i].x, gTeamAudioList[i].y, gTeamAudioList[i].radius, gTeamAudioList[i].always_on_screen, gTeamAudioList[i].id, -1);
	}

	if (CaveNet_Server_IsHosting())
	{
		CaveNet::DataStructures::NetClientIterator it;

		for (int i = 0; i < 0; i++)
		{
			char a[32];
			sprintf(a, "Bot %d", i);
			CaveNet_Server_ClientGroup_AddBot(a, "Quote", 0, 0, 0, true);
		}

		// Make sure all clients are on a team
		for (CaveNet::DataStructures::NetClient* pClient = it.first(true); pClient; pClient = it.next())
			CaveNet::DataStructures::NetTeam::server_first->AddMember(pClient, false);

		CaveNet::DataStructures::NetTeam::BalanceTeams(false);

		for (CaveNet::DataStructures::NetClient* pClient = it.first(true); pClient; pClient = it.next())
			pClient->SpawnAuto();

		for (CaveNet::DataStructures::NetClient* pClient = it.first(true); pClient; pClient = it.next())
			if (pClient->mIsBot && pClient->GetPlayer())
				pClient->GetPlayer()->StartAI();
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CTP_OnPostDraw()
{
	if (CaveNet_Server_IsHosting())
	{
		if ((gKey & KEY_ALT_DOWN) && (gKeyTrg & KEY_F1))
		{
			// Toggle node editor
			g_NodeEditor_Enabled         = !g_NodeEditor_Enabled;
			g_NodeEditor_SelectedTeamIdx = -1;
			g_NodeEditor_Mode            = NodeEditorMode::MODE_CREATE_NODES;// NodeEditorMode::MODE_CREATE_NODES;

			gKeyTrg &= ~(KEY_LMB | KEY_RMB);
			gKey    &= ~(KEY_LMB | KEY_RMB);

			if (g_NodeEditor_Enabled)
			{
				BaseModeInstance::ShowCursor();
				SetPlayerState(CaveNet_Server_ClientGroup_GetClient(0)->GetInterface(), 11, true);
			}
			else
			{
				BaseModeInstance::HideCursor();
				SetPlayerState(CaveNet_Server_ClientGroup_GetClient(0)->GetInterface(), 0, true);
			}
		}
		else if (g_NodeEditor_Enabled)
		{
			if (gKeyTrg & KEY_ALT_DOWN)
			{
				// Alternate selected team
				++g_NodeEditor_SelectedTeamIdx;

				if (g_NodeEditor_SelectedTeamIdx >= gTeamInfoCount)
					g_NodeEditor_SelectedTeamIdx = -1;
			}
			else if (gKeyTrg & KEY_RMB)
			{
				// Alternate tool
				if (gKey & KEY_SHIFT)
				{
					g_NodeEditor_Mode = (NodeEditorMode)((unsigned int)g_NodeEditor_Mode - 1);

					if (g_NodeEditor_Mode < 0)
						g_NodeEditor_Mode = (NodeEditorMode)(NodeEditorMode::MODE_COUNT - 1);
				}
				else
				{
					g_NodeEditor_Mode = (NodeEditorMode)((unsigned int)g_NodeEditor_Mode + 1);

					if (g_NodeEditor_Mode >= NodeEditorMode::MODE_COUNT)
						g_NodeEditor_Mode = NodeEditorMode::MODE_CREATE_NODES;
				}
			}
		}
	}

	if (g_NodeEditor_Enabled)
		CTP_DrawNodeEditor();

#if 0
	CaveNet::DataStructures::NetClientIterator it;
	GUI_RECT tmp;

#define AI_PATH_LIST	*((List<MAP_PUPPY_NODE_MOVE_INFO>**)&npc->m_CustomVarList[0])
#define AI_PATH_INDEX	*((int*)&npc->m_CustomVarList[1])

	for (CaveNet::DataStructures::NetClient* pBotClient = it.first(true); pBotClient; pBotClient = it.next())
	{
		if (!pBotClient->mIsBot)
			continue;

		CaveNet::DataStructures::NetPlayer* pPlayer = pBotClient->GetPlayer();
		CaveNet::DataStructures::AI_Brain* npc      = pPlayer->m_Ai.brain;
		MYCHAR* pMyChar                             = &pPlayer->m_Player.npc;
		GUI_POINT pMCPos(SubpixelToScreenCoord(pMyChar->x) - SubpixelToScreenCoord(gFrame.x), SubpixelToScreenCoord(pMyChar->y) - SubpixelToScreenCoord(gFrame.y));

		for (int i = AI_PATH_INDEX; i < (AI_PATH_LIST)->mListCount; i++)
		{
			MAP_PUPPY_NODE_MOVE_INFO* pMove = &(AI_PATH_LIST)->mList[i];
			GUI_POINT pStart((i == AI_PATH_INDEX ? pMyChar->x : (AI_PATH_LIST)->mList[i - 1].tgt_x), (i == AI_PATH_INDEX ? pMyChar->y : (AI_PATH_LIST)->mList[i - 1].tgt_y));
			GUI_POINT pEnd(pMove->tgt_x, pMove->tgt_y);

			// Draw the line
			CacheSurface::DrawLine(
				GUI_POINT(
					(pStart.x / 0x200) - (gFrame.x / 0x200),
					(pStart.y / 0x200) - (gFrame.y / 0x200)
				).to_screenspace(),
				GUI_POINT(
					(pEnd.x / 0x200) - (gFrame.x / 0x200),
					(pEnd.y / 0x200) - (gFrame.y / 0x200)
				).to_screenspace(),
				CSM_RGB(255, 0, 0, 255),
				false
			);
		}
	}
#endif

	if (!CaveNet_Client_IsConnected() || gMC.life > 0)
		return;

	char pBuffer[128];

	int iRespawnTime = CaveNet_Client_ClientGroup_GetClientByGhostId(CaveNet_Client_GetGhostId())->mCache.respawn_time;
	int iGlobalTimer = CaveNet_Client_GetGlobalTimerOffset();
	int iSeconds = (iGlobalTimer > iRespawnTime ? 0 : (iRespawnTime - iGlobalTimer) / 1000) + 1;
	int iMinutes = iSeconds / 60;
	int iHours   = iMinutes / 60;

	if (iHours > 0)
		sprintf_s(pBuffer, sizeof(pBuffer), "YOU WILL RESPAWN IN %d:%02d:%02d", iHours, iMinutes % 60, iSeconds % 60);
	else if (iMinutes > 0)
		sprintf_s(pBuffer, sizeof(pBuffer), "YOU WILL RESPAWN IN %d:%02d", iMinutes, iSeconds % 60);
	else
		sprintf_s(pBuffer, sizeof(pBuffer), "YOU WILL RESPAWN IN %d SECOND%s", iSeconds, (iSeconds == 1 ? " " : "S"));

	int iOffsetX = (WINDOW_WIDTH / 2) - (Puppy_GetStringWidth(pBuffer) / 2);
	int iOffsetY = (WINDOW_HEIGHT / 2) - 4;

	// Draw "YOU WILL RESPAWN IN" to the screen
	Puppy_PutString((WINDOW_WIDTH / 2) - (Puppy_GetStringWidth(pBuffer) / 2), (WINDOW_HEIGHT / 2) - 4, pBuffer);
}

void CTP_OnPreDrawHUD()
{
	if (!gTeamInfoCount)
	{
		CTP_DrawZoneShimmer();
		return;
	}

	// Draw the covers
	for (int i = 0; i < gPublicCoverImageCount; i++)
		if (!gPublicCoverImageList[i].background)
			PutBitmap3(&grcGame, -SubpixelToScreenCoord(gFrame.x + 0x1000) + SubpixelToScreenCoord(gPublicCoverImageList[i].off_x), -SubpixelToScreenCoord(gFrame.y + 0x1000) + SubpixelToScreenCoord(gPublicCoverImageList[i].off_y), &gPublicCoverImageList[i].src_rect, gPublicCoverImageList[i].id);

	CTP_DrawZoneShimmer();

	// Draw the foreground covers
	if (!g_NodeEditor_Enabled)
	{
		for (int i = 0; i < gTeamInfoCount; i++)
		{
			MAP_TEAM_INFO* pInfo = &gTeamInfoList[i];

			for (int j = 0; j < pInfo->cover_count; j++)
			{
				MAP_COVER_IMAGE* pImage = &pInfo->cover_list[j];

				if (
					pImage->background ||
					(pImage->show_for_team_members && pInfo->team->mTeamType != gOurCurrentTeamType) ||
					(!pImage->show_for_team_members && pInfo->team->mTeamType == gOurCurrentTeamType)
					)
					continue;

				// We've met a draw requisite
				PutBitmap3(&grcGame, -SubpixelToScreenCoord(gFrame.x + 0x1000) + SubpixelToScreenCoord(pImage->off_x), -SubpixelToScreenCoord(gFrame.y + 0x1000) + SubpixelToScreenCoord(pImage->off_y), &pImage->src_rect, pImage->id);
			}
		}
	}
}

void CTP_OnDrawMapBack()
{
	// Draw the covers
	for (int i = 0; i < gPublicCoverImageCount; i++)
		if (gPublicCoverImageList[i].background)
			PutBitmap3(&grcGame, -SubpixelToScreenCoord(gFrame.x + 0x1000) + SubpixelToScreenCoord(gPublicCoverImageList[i].off_x), -SubpixelToScreenCoord(gFrame.y + 0x1000) + SubpixelToScreenCoord(gPublicCoverImageList[i].off_y), &gPublicCoverImageList[i].src_rect, gPublicCoverImageList[i].id);

	if (!gTeamInfoCount)
		return;
	
	// Draw the covers
	for (int i = 0; i < gTeamInfoCount; i++)
	{
		MAP_TEAM_INFO* pInfo = &gTeamInfoList[i];

		for (int j = 0; j < pInfo->cover_count; j++)
		{
			MAP_COVER_IMAGE* pImage = &pInfo->cover_list[j];

			if (
				!pImage->background ||
				(pImage->show_for_team_members && pInfo->team->mTeamType != gOurCurrentTeamType) ||
				(!pImage->show_for_team_members && pInfo->team->mTeamType == gOurCurrentTeamType)
				)
				continue;

			// We've met a draw requisite
			PutBitmap3(&grcGame, -SubpixelToScreenCoord(gFrame.x + 0x1000) + SubpixelToScreenCoord(pImage->off_x), -SubpixelToScreenCoord(gFrame.y + 0x1000) + SubpixelToScreenCoord(pImage->off_y), &pImage->src_rect, pImage->id);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MAP_TEAM_INFO* CTP_GetTeamInfo(int iTeamType)
{
	for (int i = 0; i < gTeamInfoCount; i++)
		if (gTeamInfoList[i].team_type == (CaveNet::DataStructures::NetTeam::TeamType)iTeamType)
			return &gTeamInfoList[i];

	return NULL;
}

MAP_TEAM_INFO* CTP_GetTeamInfoByTeam(CaveNet::DataStructures::NetTeam* pTeam)
{
	for (int i = 0; i < gTeamInfoCount; i++)
		if (gTeamInfoList[i].team && gTeamInfoList[i].team->GetTeamId() == pTeam->GetTeamId())
			return &gTeamInfoList[i];

	return NULL;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------