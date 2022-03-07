#include "Weapons/ModInfo.h"
#include "CavestoryModAPI.h"
#include <stdarg.h>

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* gMoveTypeNames[] =
{
	"MOVE_RUN",
	"MOVE_JUMP"
};

static const char* gpNodeEditorModeNames[] =
{
	"CREATE NODE",
	"DELETE NODE",
	"LINK NODE (RUN)",
	"LINK NODE (JUMP)",
	"UNLINK NODES",
	"CHANGE TEAM COND",
	"SUMMON PUPPY",
	"SUMMON BOT",
	"EXPORT NODES"
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static char _avar_buffer[2048];
const char* avar(const char* _Format, ...)
{
	va_list ptr;
	va_start(ptr, _Format);
	vsprintf(_avar_buffer, _Format, ptr);
	va_end(ptr);
	return _avar_buffer;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool g_NodeEditor_Enabled        = false;
int g_NodeEditor_SelectedTeamIdx = 0;
int g_NodeEditor_SelectedNode    = -1;
NodeEditorMode g_NodeEditor_Mode = NodeEditorMode::MODE_CREATE_NODES;
MAP_TEAM_INFO g_AI_NodeInfo;

MAP_TEAM_INFO* CTP_NodeEditor_GetCurrentTeamInfo()
{
	if (g_NodeEditor_SelectedTeamIdx == -1)
		return &g_AI_NodeInfo;

	return &gTeamInfoList[g_NodeEditor_SelectedTeamIdx];
}

GUI_POINT CTP_GetMapTileUnderMouse()
{
	GUI_POINT pMouseTile = GUI_POINT(gFrame.x + 0x1000, gFrame.y + 0x1000) / 0x200;

	pMouseTile.x += Mouse::GetMousePosition().from_screenspace().x;
	pMouseTile.y += Mouse::GetMousePosition().from_screenspace().y;
	pMouseTile.x /= 16;
	pMouseTile.y /= 16;

	return pMouseTile;
}

int CTP_GetNodeUnderMouse()
{
	MAP_TEAM_INFO* pInfo = CTP_NodeEditor_GetCurrentTeamInfo();
	GUI_POINT pMouseTile = CTP_GetMapTileUnderMouse();

	for (int j = 0; j < pInfo->node_count; j++)
	{
		MAP_PUPPY_NODE* pNode = &pInfo->node_list[j];
		GUI_RECT pNodeRect((-((gFrame.x + 0x1000) / 0x200) + (pNode->x * 16)), (-((gFrame.y + 0x1000) / 0x200) + (pNode->y * 16)), 16, 16);

		if (!pNodeRect.pointInRect(Mouse::GetMousePosition().from_screenspace()))
			continue;

		return j;
	}

	return -1;
}

void CTP_DeleteNode(int iNodeIdx)
{
	MAP_TEAM_INFO* pInfo = CTP_NodeEditor_GetCurrentTeamInfo();

	if (!pInfo->node_count || iNodeIdx < 0 || iNodeIdx >= pInfo->node_count)
	{
		CSM_Log("Failed to delete node # %d\r\n", iNodeIdx);
		return;
	}

	// Remove all connections
	if (pInfo->node_connection_list)
	{
		for (int i = 0; i < pInfo->node_connection_count; i++)
		{
			MAP_PUPPY_NODE_CONNECTION* pConn = &pInfo->node_connection_list[i];

			// Skip connections that don't have us in them
			if (pConn->one != iNodeIdx && pConn->two != iNodeIdx)
			{
				if (pConn->one > iNodeIdx)
					--pConn->one;
				if (pConn->two > iNodeIdx)
					--pConn->two;

				continue;
			}

			LIST_ERASE(pInfo->node_connection_list, pInfo->node_connection_count, MAP_PUPPY_NODE_CONNECTION, i);
			--pInfo->node_connection_count;
			--i;
		}

		// Re-allocate the list
		if (!pInfo->node_connection_count)
		{
			// Free it entirely
			free(pInfo->node_connection_list);
			pInfo->node_connection_list = NULL;
		}
		else
		{
			// Resize it
			pInfo->node_connection_list = (MAP_PUPPY_NODE_CONNECTION*)realloc((void*)pInfo->node_connection_list, sizeof(MAP_PUPPY_NODE_CONNECTION) * pInfo->node_connection_count);
		}
	}

	// Erase it from the
	LIST_ERASE(pInfo->node_list, pInfo->node_count, MAP_PUPPY_NODE, iNodeIdx);
	--pInfo->node_count;

	// Re-allocate the list
	if (!pInfo->node_count)
	{
		// Free it entirely
		free(pInfo->node_list);
		pInfo->node_list = NULL;
	}
	else
	{
		// Resize it
		pInfo->node_list = (MAP_PUPPY_NODE*)realloc((void*)pInfo->node_list, sizeof(MAP_PUPPY_NODE) * pInfo->node_count);
	}
}

void CTP_CreateNode(int x, int y, short iTeamCond = -1)
{
	MAP_TEAM_INFO* pInfo = CTP_NodeEditor_GetCurrentTeamInfo();

	// Loop through all nodes and check to see if X and Y are already taken
	for (int i = 0; i < pInfo->node_count; i++)
	{
		MAP_PUPPY_NODE* pNode = &pInfo->node_list[i];

		// Stop here
		if (pNode->x == x && pNode->y == y)
		{
			//CSM_Log("Node already exists at (%d, %d)!!\r\n", x, y);
			if (iTeamCond != -1)
				pNode->team_cond = iTeamCond;
			return;
		}
	}

	if (!pInfo->node_list)
	{
		pInfo->node_count = 1;
		pInfo->node_list  = (MAP_PUPPY_NODE*)malloc(sizeof(MAP_PUPPY_NODE) * pInfo->node_count);
	}
	else
	{
		pInfo->node_count += 1;
		pInfo->node_list   = (MAP_PUPPY_NODE*)realloc((void*)pInfo->node_list, sizeof(MAP_PUPPY_NODE) * pInfo->node_count);
	}

	pInfo->node_list[pInfo->node_count - 1].x          = x;
	pInfo->node_list[pInfo->node_count - 1].y          = y;
	pInfo->node_list[pInfo->node_count - 1].team_cond  = iTeamCond == -1 ? 0 : iTeamCond;
	pInfo->node_list[pInfo->node_count - 1].conn_list  = NULL;
	pInfo->node_list[pInfo->node_count - 1].conn_count = 0;
}

void CTP_AlternateTeamCond(int A)
{
	MAP_TEAM_INFO* pInfo = CTP_NodeEditor_GetCurrentTeamInfo();

	if (++pInfo->node_list[A].team_cond >= gTeamInfoCount + 1)
		pInfo->node_list[A].team_cond = 0;
}

void CTP_AddConnectionToNode(MAP_PUPPY_NODE* pNode, int iConnIndex)
{
	for (int i = 0; i < pNode->conn_count; i++)
	{
		if (pNode->conn_list[i] == iConnIndex)
			return;
	}

	if (!pNode->conn_list)
	{
		pNode->conn_count = 1;
		pNode->conn_list  = (int*)malloc(sizeof(int) * pNode->conn_count);
	}
	else
	{
		pNode->conn_count += 1;
		pNode->conn_list   = (int*)realloc((void*)pNode->conn_list, sizeof(int) * pNode->conn_count);
	}

	pNode->conn_list[pNode->conn_count - 1] = iConnIndex;
}

void CTP_RemoveConnectionFromNode(MAP_PUPPY_NODE* pNode, int iConnIndex)
{
	for (int i = 0; i < pNode->conn_count; i++)
	{
		if (pNode->conn_list[i] != iConnIndex)
			continue;

		// Erase it from the list
		LIST_ERASE(pNode->conn_list, pNode->conn_count, int, i);
		--pNode->conn_count;

		// Shrink the list
		if (!pNode->conn_count)
		{
			free(pNode->conn_list);
			pNode->conn_list = NULL;
		}
		else
		{
			pNode->conn_list = (int*)realloc((void*)pNode->conn_list, sizeof(int) * pNode->conn_count);
		}

		return;
	}
}

void CTP_LinkNodes(int A, int B, PuppyMoveType iMoveType)
{
	MAP_TEAM_INFO* pInfo = CTP_NodeEditor_GetCurrentTeamInfo();

	// Loop through all nodes and check to see if X and Y are already taken
	for (int i = 0; i < pInfo->node_connection_count; i++)
	{
		MAP_PUPPY_NODE_CONNECTION* pConn = &pInfo->node_connection_list[i];

		// Stop here
		if ((pConn->one == A && pConn->two == B) || (pConn->one == B && pConn->two == A))
		{
			if (pConn->one == B && pConn->two == A)
				pConn->move_type_from = iMoveType;
			else
				pConn->move_type_to = iMoveType;

			if (!pConn->mutual && (pConn->one == B && pConn->two == A))
			{
				pConn->mutual = true;
				CTP_AddConnectionToNode(&pInfo->node_list[A], i);
				CTP_AddConnectionToNode(&pInfo->node_list[B], i);
			}

			return;
		}
	}

	if (!pInfo->node_connection_list)
	{
		pInfo->node_connection_count = 1;
		pInfo->node_connection_list  = (MAP_PUPPY_NODE_CONNECTION*)malloc(sizeof(MAP_PUPPY_NODE_CONNECTION) * pInfo->node_connection_count);
	}
	else
	{
		pInfo->node_connection_count += 1;
		pInfo->node_connection_list   = (MAP_PUPPY_NODE_CONNECTION*)realloc((void*)pInfo->node_connection_list, sizeof(MAP_PUPPY_NODE_CONNECTION) * pInfo->node_connection_count);
	}

	pInfo->node_connection_list[pInfo->node_connection_count - 1].one            = A;
	pInfo->node_connection_list[pInfo->node_connection_count - 1].two            = B;
	pInfo->node_connection_list[pInfo->node_connection_count - 1].move_type_to   = iMoveType;
	pInfo->node_connection_list[pInfo->node_connection_count - 1].move_type_from = iMoveType;
	pInfo->node_connection_list[pInfo->node_connection_count - 1].mutual         = false;

	CTP_AddConnectionToNode(&pInfo->node_list[A], pInfo->node_connection_count - 1);

	return;
}

void CTP_UnlinkNodes(int A, int B)
{
	MAP_TEAM_INFO* pInfo = CTP_NodeEditor_GetCurrentTeamInfo();

	if (!pInfo->node_count || A < 0 || A >= pInfo->node_count || B < 0 || B >= pInfo->node_count)
	{
		CSM_Log("Failed to unlink node # %d from node # %d\r\n", A, B);
		return;
	}

	for (int i = 0; i < pInfo->node_connection_count; i++)
	{
		MAP_PUPPY_NODE_CONNECTION* pConn = &pInfo->node_connection_list[i];

		// Skip connections that don't have us in them
		if ((pConn->one == A && pConn->two == B) || (pConn->one == B && pConn->two == A))
		{
			if (pConn->one == A && pConn->two == B && pConn->mutual)
			{
				pConn->one            = B;
				pConn->two            = A;
				pConn->move_type_to   = pConn->move_type_from;
				pConn->move_type_from = PuppyMoveType::PUPPY_MOVE_RUN;
				pConn->mutual         = false;

				CTP_RemoveConnectionFromNode(&pInfo->node_list[A], i);

				return;
			}
			else if (pConn->one == B && pConn->two == A && pConn->mutual)
			{
				// Remove mutual connection
				pConn->mutual = false;
				CTP_RemoveConnectionFromNode(&pInfo->node_list[B], i);
				return;
			}

			CTP_RemoveConnectionFromNode(&pInfo->node_list[A], i);

			// Fix indices for other nodes
			for (int j = 0; j < pInfo->node_count; j++)
				for (int k = 0; k < pInfo->node_list[j].conn_count; k++)
					if (pInfo->node_list[j].conn_list[k] >= i)
						--pInfo->node_list[j].conn_list[k];

			LIST_ERASE(pInfo->node_connection_list, pInfo->node_connection_count, MAP_PUPPY_NODE_CONNECTION, i);
			--pInfo->node_connection_count;
			--i;

			break;
		}
	}

	// Re-allocate the list
	if (!pInfo->node_connection_count)
	{
		// Free it entirely
		free(pInfo->node_connection_list);
		pInfo->node_connection_list = NULL;
	}
	else
	{
		// Resize it
		pInfo->node_connection_list = (MAP_PUPPY_NODE_CONNECTION*)realloc((void*)pInfo->node_connection_list, sizeof(MAP_PUPPY_NODE_CONNECTION) * pInfo->node_connection_count);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const unsigned short g_NodeEditor_VERSION = 4;

void CTP_ImportNodes(int iTeamIdx)
{
	MAP_TEAM_INFO* pInfo = (iTeamIdx == -1 ? &g_AI_NodeInfo : &gTeamInfoList[iTeamIdx]);
	FILE* fp             = fopen((iTeamIdx == -1 ? avar("ctp/Stage/Covers/%s/NodeData-AI.bin", g_CurrentStage_Name) : avar("ctp/Stage/Covers/%s/NodeData-T%d.bin", g_CurrentStage_Name, iTeamIdx)), "rb");

	g_NodeEditor_SelectedTeamIdx = iTeamIdx;

	// No node data!!
	if (!fp)
		return;

	unsigned short iVersion = 0;
	unsigned int iNodeCount = 0;
	unsigned int iLinkCount = 0;

	// Read the version
	fread(&iVersion, sizeof(unsigned short), 1, fp);

	// Compare version to current
	if (iVersion > g_NodeEditor_VERSION)
	{
		CSM_Log("[ERROR] Outdated nodegraph \"%d\" (expected %d)...\r\n", iVersion, g_NodeEditor_VERSION);
		fclose(fp);
		return;
	}

	// Read counts
	fread(&iNodeCount, sizeof(unsigned int), 1, fp);
	fread(&iLinkCount, sizeof(unsigned int), 1, fp);

	// Read nodes
	for (int i = 0; i < iNodeCount; i++)
	{
		unsigned short iNodeX = 0;
		unsigned short iNodeY = 0;
		unsigned short iTCond = 0;

		// Read node information
		fread(&iNodeX, sizeof(unsigned short), 1, fp);
		fread(&iNodeY, sizeof(unsigned short), 1, fp);

		if (iVersion >= 4)
			fread(&iTCond, sizeof(unsigned short), 1, fp);

		// Create the node
		CTP_CreateNode(iNodeX, iNodeY, iTCond);
	}
	
	// Read links
	for (int i = 0; i < iLinkCount; i++)
	{
		unsigned char iMoveTypeTo    = 0;
		unsigned char iMoveTypeFrom  = 0;
		unsigned int iNodeOne        = 0;
		unsigned int iNodeTwo        = 0;
		bool bMutual                 = false;

		// Read link information
		if (iVersion >= 3)
		{
			fread(&iMoveTypeTo, sizeof(unsigned char), 1, fp);
			fread(&iMoveTypeFrom, sizeof(unsigned char), 1, fp);
			fread(&iNodeOne, sizeof(unsigned int), 1, fp);
			fread(&iNodeTwo, sizeof(unsigned int), 1, fp);
			fread(&bMutual, sizeof(bool), 1, fp);
		}
		else
		{
			fread(&iMoveTypeTo, sizeof(unsigned char), 1, fp);
			fread(&iNodeOne, sizeof(unsigned int), 1, fp);
			fread(&iNodeTwo, sizeof(unsigned int), 1, fp);

			if (iVersion >= 2)
				fread(&bMutual, sizeof(bool), 1, fp);
		}

		// Create link
		CTP_LinkNodes(CLAMP(iNodeOne, 0, iNodeCount - 1), CLAMP(iNodeTwo, 0, iNodeCount - 1), (PuppyMoveType)iMoveTypeTo);

		if (bMutual)
			CTP_LinkNodes(CLAMP(iNodeTwo, 0, iNodeCount - 1), CLAMP(iNodeOne, 0, iNodeCount - 1), (PuppyMoveType)iMoveTypeFrom);
	}

	// Done!
	fclose(fp);
}

void CTP_ExportNodes()
{
	MAP_TEAM_INFO* pInfo = CTP_NodeEditor_GetCurrentTeamInfo();
	FILE* fp             = fopen((g_NodeEditor_SelectedTeamIdx == -1 ? avar("ctp/Stage/Covers/%s/NodeData-AI.bin", g_CurrentStage_Name) : avar("ctp/Stage/Covers/%s/NodeData-T%d.bin", g_CurrentStage_Name, g_NodeEditor_SelectedTeamIdx)), "wb");

	if (!fp)
	{
		CSM_Log("Failed to open the output file.\r\n");
		return;
	}

	// Write file version
	fwrite(&g_NodeEditor_VERSION, sizeof(unsigned short), 1, fp);

	// Write node count & link count
	fwrite(&pInfo->node_count,				sizeof(unsigned int), 1, fp);
	fwrite(&pInfo->node_connection_count,	sizeof(unsigned int), 1, fp);

	// Write node information
	for (int i = 0; i < pInfo->node_count; i++)
	{
		MAP_PUPPY_NODE* pNode = &pInfo->node_list[i];

		fwrite(&pNode->x, sizeof(unsigned short), 1, fp);
		fwrite(&pNode->y, sizeof(unsigned short), 1, fp);
		fwrite(&pNode->team_cond, sizeof(unsigned short), 1, fp);
	}

	// Write link information
	for (int i = 0; i < pInfo->node_connection_count; i++)
	{
		MAP_PUPPY_NODE_CONNECTION* pConn = &pInfo->node_connection_list[i];

		fwrite(&pConn->move_type_to,	sizeof(unsigned char),	1, fp);
		fwrite(&pConn->move_type_from,	sizeof(unsigned char),	1, fp);
		fwrite(&pConn->one,				sizeof(unsigned int),	1, fp);
		fwrite(&pConn->two,				sizeof(unsigned int),	1, fp);
		fwrite(&pConn->mutual,			sizeof(bool),			1, fp);
	}

	fclose(fp);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define PULSE_TIME		300
#define	PULSE_COLOR		CSM_RGB(255, 0, 0)

void CTP_DrawLine(GUI_POINT start, GUI_POINT end, unsigned int iColor, bool bPulse = false)
{
	// Draw the shadow
	CacheSurface::DrawLine(
		GUI_POINT(start.x, start.y + 1).to_screenspace(),
		GUI_POINT(end.x, end.y + 1).to_screenspace(),
		CSM_RGB(70, 70, 70),
		false
	);

	if (bPulse)
	{
		unsigned long colors[8] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };

		colors[(GetTicks() % (PULSE_TIME * 8)) / PULSE_TIME] = PULSE_COLOR;

		CacheSurface::DrawGradientLine(
			start.to_screenspace(),
			end.to_screenspace(),
			colors,
			8
		);

		return;
	}
	
	CacheSurface::DrawLine(
		start.to_screenspace(),
		end.to_screenspace(),
		iColor,
		false
	);
}

void CTP_DrawConnection(MAP_TEAM_INFO* pInfo, MAP_PUPPY_NODE_CONNECTION* pConn)
{
	GUI_POINT pNode1Pos((-((gFrame.x + 0x1000) / 0x200) + (pInfo->node_list[pConn->one].x * 16)), (-((gFrame.y + 0x1000) / 0x200) + (pInfo->node_list[pConn->one].y * 16)));
	GUI_POINT pNode2Pos((-((gFrame.x + 0x1000) / 0x200) + (pInfo->node_list[pConn->two].x * 16)), (-((gFrame.y + 0x1000) / 0x200) + (pInfo->node_list[pConn->two].y * 16)));
	
	GUI_POINT pNode3Pos((-((gFrame.x + 0x1000) / 0x200) + (pInfo->node_list[pConn->two].x * 16)), (-((gFrame.y + 0x1000) / 0x200) + (pInfo->node_list[pConn->two].y * 16)));
	GUI_POINT pNode4Pos((-((gFrame.x + 0x1000) / 0x200) + (pInfo->node_list[pConn->one].x * 16)), (-((gFrame.y + 0x1000) / 0x200) + (pInfo->node_list[pConn->one].y * 16)));

	if (
		(pNode1Pos.x < 0 && pNode2Pos.x < 0) ||
		(pNode1Pos.y < 0 && pNode2Pos.y < 0) ||
		(pNode1Pos.x >= WINDOW_WIDTH && pNode2Pos.x >= WINDOW_WIDTH) ||
		(pNode1Pos.y >= WINDOW_HEIGHT && pNode2Pos.y >= WINDOW_HEIGHT)
		)
		return;

	unsigned int iPulseColor  = PULSE_COLOR;
	unsigned int iNormalColor = (pConn->mutual ? CSM_RGB(27, 221, 254) : CSM_RGB(255, 255, 255));
	PuppyMoveType iMoveType   = pConn->move_type_to;
	bool bDrawingMutual       = false;

REDO_DRAW:
	switch (iMoveType)
	{
		case PuppyMoveType::PUPPY_MOVE_JUMP:
		{
			GUI_POINT start(pNode1Pos.x + 8, pNode1Pos.y + 8);
			GUI_POINT end(pNode2Pos.x + 8, pNode2Pos.y + 8);
			GUI_POINT midpoint((start.x + (end.x - start.x) / 2), (start.y + (end.y - start.y) / 2) - 32);
			GUI_POINT last(pNode1Pos.x + 8, pNode1Pos.y + 8);

			for (float k = 0.f; k < 1; k += 0.1f)
			{
				GUI_POINT m1(
					(int)LerpInt((float)start.x, (float)midpoint.x, k),
					(int)LerpInt((float)start.y, (float)midpoint.y, k)
				);
				GUI_POINT m2(
					(int)LerpInt((float)midpoint.x, (float)end.x, k),
					(int)LerpInt((float)midpoint.y, (float)end.y, k)
				);
				GUI_POINT curr(
					LerpInt((float)m1.x, (float)m2.x, k),
					LerpInt((float)m1.y, (float)m2.y, k)
				);

				/*
				npc->x = (unsigned int)LerpInt((float)m1_x, (float)m2_x, ((float)npc->act_wait) / JUMP_SPEED_F);
				npc->y = (unsigned int)LerpInt((float)m1_y, (float)m2_y, ((float)npc->act_wait) / JUMP_SPEED_F);
				*/

				CTP_DrawLine(
					last,
					curr,
					(unsigned int((GetTicks() % (11 * PULSE_TIME)) / PULSE_TIME) == unsigned int(10.f * k) ? iPulseColor : iNormalColor)
				);

				last = curr;
			}

			CTP_DrawLine(
				last,
				end,
				(unsigned int((GetTicks() % (11 * PULSE_TIME)) / PULSE_TIME) == 10 ? iPulseColor : iNormalColor)
			);

			break;
		}
		case PuppyMoveType::PUPPY_MOVE_RUN:
		{
			CTP_DrawLine(
				GUI_POINT(pNode1Pos.x + 8, pNode1Pos.y + 8),
				GUI_POINT(pNode2Pos.x + 8, pNode2Pos.y + 8),
				iNormalColor,
				!pConn->mutual
			);

			break;
		}
	}

	if (pConn->mutual && !bDrawingMutual && pConn->move_type_from != pConn->move_type_to)
	{
		bDrawingMutual = true;
		iMoveType      = pConn->move_type_from;
		pNode1Pos      = pNode3Pos;
		pNode2Pos      = pNode4Pos;

		if (
			(pNode1Pos.x < 0 && pNode2Pos.x < 0) ||
			(pNode1Pos.y < 0 && pNode2Pos.y < 0) ||
			(pNode1Pos.x >= WINDOW_WIDTH && pNode2Pos.x >= WINDOW_WIDTH) ||
			(pNode1Pos.y >= WINDOW_HEIGHT && pNode2Pos.y >= WINDOW_HEIGHT)
			)
			return;

		goto REDO_DRAW;
	}
}

void CTP_DrawNodeEditor()
{
	CacheSurface::SetSurfaceID(SURFACE_ID_RENDERER);

	unsigned int iColor = 0;

	for (int i = 0; i < gTeamAudioCount; i++)
	{
		MAP_AUDIO_FILE* pSound = &gTeamAudioList[i];

		if (
				(
					(pSound->x - (pSound->radius / 2) < gFrame.x) ||
					(pSound->x + (pSound->radius / 2) > gFrame.x + (WINDOW_WIDTH * 0x200))
				)
				&&
				(
					(pSound->y - (pSound->radius / 2) < gFrame.y) ||
					(pSound->y + (pSound->radius / 2) > gFrame.y + (WINDOW_HEIGHT * 0x200))
				)
			)
			continue;

		CacheSurface::DrawCircle(
			GUI_POINT
			(
				((pSound->x / 0x200) - (gFrame.x / 0x200)),
				((pSound->y / 0x200) - (gFrame.y / 0x200))
			),
			pSound->radius / 0x200,
			CSM_RGBA(255, 0, 0, 40)
		);
		CacheSurface::DrawCircle(
			GUI_POINT
			(
			((pSound->x / 0x200) - (gFrame.x / 0x200)),
				((pSound->y / 0x200) - (gFrame.y / 0x200))
			),
			pSound->radius / 0xC00,
			CSM_RGBA(255, 255, 0, 40)
		);
		CacheSurface::DrawCircle(
			GUI_POINT
			(
				((pSound->x / 0x200) - (gFrame.x / 0x200)),
				((pSound->y / 0x200) - (gFrame.y / 0x200))
			),
			8,
			CSM_RGBA(0, 255, 0, 40)
		);
	}

	// Draw all nodes
	if (g_NodeEditor_SelectedNode != -1)
	{
		MAP_TEAM_INFO* pInfo  = CTP_NodeEditor_GetCurrentTeamInfo();
		MAP_PUPPY_NODE* pNode = &pInfo->node_list[g_NodeEditor_SelectedNode];

		for (int i = 0; i < pNode->conn_count; i++)
		{
			CTP_DrawConnection(pInfo, &pInfo->node_connection_list[pNode->conn_list[i]]);
		}
	}

	if (g_NodeEditor_SelectedTeamIdx >= -1 && g_NodeEditor_SelectedTeamIdx < (int)gTeamInfoCount)
	{
		MAP_TEAM_INFO* pInfo = CTP_NodeEditor_GetCurrentTeamInfo();
		iColor               = CSM_RGB(255, 255, 255);

		// Draw nodes
		for (int j = 0; j < pInfo->node_count; j++)
		{
			MAP_PUPPY_NODE* pNode = &pInfo->node_list[j];
			GUI_POINT pNodePos((-((gFrame.x + 0x1000) / 0x200) + (pNode->x * 16)), (-((gFrame.y + 0x1000) / 0x200) + (pNode->y * 16)));

			// Don't render nodes that are off-screen
			if (pNodePos.x < 0 || pNodePos.y < 0 || pNodePos.x >= WINDOW_WIDTH || pNodePos.y >= WINDOW_HEIGHT)
				continue;

			// Draw the actual rect
			CacheSurface::DrawRect(&GUI_RECT(pNodePos, 16, 16), (g_NodeEditor_SelectedNode == j ? CSM_RGB(127, 0, 255) : (pNode->team_cond == 0 ? iColor : gTeamInfoList[pNode->team_cond - 1].team->GetColor().to_rgb())));

			// Draw the node number
			Puppy_PutNumber(pNodePos.x, pNodePos.y, j, false);
		}

		if (g_NodeEditor_SelectedNode == -1)
		{
			for (int j = 0; j < pInfo->node_connection_count; j++)
			{
				MAP_PUPPY_NODE_CONNECTION* pConn = &pInfo->node_connection_list[j];
				CTP_DrawConnection(pInfo, pConn);
			}
		}
	}

	// Get the mouse point on the map
	GUI_POINT pMouseTile = CTP_GetMapTileUnderMouse();

	// Put debug information
	Puppy_PutString(0, 0, avar("%s - (%d, %d)", gpNodeEditorModeNames[g_NodeEditor_Mode], pMouseTile.x, pMouseTile.y), 255, 255, 255);

	const char* pText = NULL;
	
	if (g_NodeEditor_SelectedTeamIdx == -1)
		pText = avar("AI Nodes [%d/%d]", gTeamInfoCount + 1, gTeamInfoCount + 1);
	else
		pText = avar("%s [%d/%d]", gTeamInfoList[g_NodeEditor_SelectedTeamIdx].team->GetName(), g_NodeEditor_SelectedTeamIdx + 1, gTeamInfoCount + 1);

	Puppy_PutString(WINDOW_WIDTH - Puppy_GetStringWidth(pText), 0, pText, 255, 255, 255);

	// Draw the mouse tile
	if (
		g_NodeEditor_Mode == NodeEditorMode::MODE_CREATE_NODES ||
		g_NodeEditor_Mode == NodeEditorMode::MODE_DELETE_NODES ||
		g_NodeEditor_Mode == NodeEditorMode::MODE_LINK_NODE_RUN ||
		g_NodeEditor_Mode == NodeEditorMode::MODE_LINK_NODE_JUMP ||
		g_NodeEditor_Mode == NodeEditorMode::MODE_SUMMON_PUPPY ||
		g_NodeEditor_Mode == NodeEditorMode::MODE_CHANGE_TEAM_COND ||
		g_NodeEditor_Mode == NodeEditorMode::MODE_UNLINK_NODES
		)
	{
		CacheSurface::DrawRect(&
			GUI_RECT(
				(-((gFrame.x + 0x1000) / 0x200) + (pMouseTile.x * 16)),
				(-((gFrame.y + 0x1000) / 0x200) + (pMouseTile.y * 16)),
				16,
				16
			), CSM_RGB(255, 120, 100));
	}

	// Performing clicks
	if (gKeyTrg & KEY_LMB)
	{
		int iNodeIdx = CTP_GetNodeUnderMouse();

		// Determine how to process this click
		switch (g_NodeEditor_Mode)
		{
			case NodeEditorMode::MODE_CREATE_NODES:
			{
				if (iNodeIdx != -1)
				{
					CSM_Log("Cannot create a node ontop of another node!!\r\n");
					return;
				}

				CTP_CreateNode(pMouseTile.x, pMouseTile.y);

				break;
			}
			case NodeEditorMode::MODE_DELETE_NODES:
			{
				if (iNodeIdx == -1)
					return;

				CTP_DeleteNode(iNodeIdx);
				break;
			}
			case NodeEditorMode::MODE_LINK_NODE_RUN:
			{
				if (iNodeIdx == -1 || g_NodeEditor_SelectedNode == iNodeIdx)
				{
					if (g_NodeEditor_SelectedNode != -1)
						g_NodeEditor_SelectedNode = -1;

					return;
				}

				if (g_NodeEditor_SelectedNode == -1)
				{
					// Set selected node
					g_NodeEditor_SelectedNode = iNodeIdx;
					return;
				}

				CTP_LinkNodes(g_NodeEditor_SelectedNode, iNodeIdx, PuppyMoveType::PUPPY_MOVE_RUN);
				g_NodeEditor_SelectedNode = -1;

				break;
			}
			case NodeEditorMode::MODE_LINK_NODE_JUMP:
			{
				if (iNodeIdx == -1 || g_NodeEditor_SelectedNode == iNodeIdx)
				{
					if (g_NodeEditor_SelectedNode != -1)
						g_NodeEditor_SelectedNode = -1;

					return;
				}

				if (g_NodeEditor_SelectedNode == -1)
				{
					// Set selected node
					g_NodeEditor_SelectedNode = iNodeIdx;
					return;
				}

				CTP_LinkNodes(g_NodeEditor_SelectedNode, iNodeIdx, PuppyMoveType::PUPPY_MOVE_JUMP);
				g_NodeEditor_SelectedNode = -1;
				break;
			}
			case NodeEditorMode::MODE_CHANGE_TEAM_COND:
			{
				if (iNodeIdx == -1)
				{
					CSM_Log("Please click on a node that exists.\r\n");
					return;
				}

				CTP_AlternateTeamCond(iNodeIdx);
				break;
			}
			case NodeEditorMode::MODE_UNLINK_NODES:
			{
				if (iNodeIdx == -1 || g_NodeEditor_SelectedNode == iNodeIdx)
				{
					if (g_NodeEditor_SelectedNode != -1)
						g_NodeEditor_SelectedNode = -1;

					return;
				}

				if (g_NodeEditor_SelectedNode == -1)
				{
					// Set selected node
					g_NodeEditor_SelectedNode = iNodeIdx;
					return;
				}

				CTP_UnlinkNodes(g_NodeEditor_SelectedNode, iNodeIdx);
				g_NodeEditor_SelectedNode = -1;
				break;
			}
			case NodeEditorMode::MODE_SUMMON_PUPPY:
			{
				if (CTP_NodeEditor_GetCurrentTeamInfo()->team && CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc)
				{
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->x        = pMouseTile.x * 0x2000;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->y        = pMouseTile.y * 0x2000;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->xm       = 0;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->ym       = 0;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->tgt_x    = 0;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->tgt_y    = 0;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->count1   = 0;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->count2   = 0;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->count3   = 0;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->count4   = 0;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->act_wait = 0;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->act_no   = 10;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->ani_no   = 3;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->ani_wait = 0;
					CTP_NodeEditor_GetCurrentTeamInfo()->team->mFlagNpc->bits    &= ~(NPC_flags::npc_option3 | NPC_flags::npc_ignoreSolid);
				}

				break;
			}
			case NodeEditorMode::MODE_SUMMON_BOT:
			{
				CaveNet::DataStructures::NetClientIterator it;

				// Look for a bot
				for (CaveNet::DataStructures::NetClient* pBotClient = it.first(true); pBotClient; pBotClient = it.next())
				{
					if (!pBotClient->mIsBot || !pBotClient->GetPlayer() || !pBotClient->GetPlayer()->m_Ai.enabled)
						continue;

					CaveNet::DataStructures::AI_Brain* pBrain = pBotClient->GetPlayer()->m_Ai.brain;
					MYCHAR* pMC                               = &pBotClient->GetPlayer()->m_Player.npc;

					// Found one!
					*((unsigned int*)&pBrain->ptr_1) = CTP_NodeEditor_GetCurrentTeamInfo()->team_type;
					pBrain->act_no                   = 2;
					pMC->x                           = pMouseTile.x * 0x2000;
					pMC->y                           = pMouseTile.y * 0x2000;
					pMC->xm                          = 0;
					pMC->ym                          = 0;

					(*((List<MAP_PUPPY_NODE_MOVE_INFO>**)& pBrain->m_CustomVarList[0]))->Free();
					*((int*)& pBrain->m_CustomVarList[1]) = 0;
					*((int*)& pBrain->m_CustomVarList[2]) = -1;
					*((int*)& pBrain->m_CustomVarList[3]) = -1;

					break;
				}

				break;
			}
			case NodeEditorMode::MODE_EXPORT_NODES:
			{
				CTP_ExportNodes();
				CSM_Log("[MOD] Exported nodes.\r\n");
				break;
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------