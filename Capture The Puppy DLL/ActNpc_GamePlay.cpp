#include "Weapons/ModInfo.h"

// Weapon Spawn
void ActNpc415(NPCHAR* npc)
{
	/* npc->count1  =  Cache'd ani_no */
	/* npc->count2  =  Respawn time */
	switch (npc->act_no)
	{
		// Init
		case 0:
		{
			if (!npc->count2)
			{
				npc->count2     = (npc->code_event - 1) * 60;
				npc->code_event = 44;
			}

			if (npc->bits & NPC_flags::npc_eventTouch)
				npc->x += 0x1000;

			npc->bits    &= ~(NPC_flags::npc_eventTouch | NPC_flags::npc_option3);
			npc->act_no   = 2;
			npc->ani_no   = 0;
			npc->act_wait = 0;
			npc->count1   = 222;

			break;
		}
		// Wait to spawn
		case 1:
		{
			if (++npc->act_wait < npc->count2)
			{
				CSM_Npc_SkipUpdate(npc);
				break;
			}

			npc->act_wait = 0;
			npc->act_no   = 2;

			// Fallthrough
		}
		// Spawn
		case 2:
		{
			npc->act_no = 3;
			npc->ani_no = 1 + npc->code_flag;
			npc->bits  |= NPC_flags::npc_option3;

			// Fallthrough
		}
		// Wait for a taker
		case 3:
		{
			break;
		}
		// Despawn
		case 4:
		{
			npc->bits    &= ~NPC_flags::npc_option3;
			npc->ani_no   = 0;
			npc->act_no   = 1;
			npc->act_wait = 0;

			break;
		}
	}

	if (npc->count1 != npc->ani_no)
	{
		npc->count1 = npc->ani_no;
		npc->rect   = pWeaponCodeTable[npc->ani_no].rect;
	}
}

// No Puppy Zone (CTP) / Kill Zone (CTP)
void ActNpc416(NPCHAR* npc)
{
	if (npc->act_no == 0)
	{
		// Initialization act
		npc->bits  |= NPC_flags::npc_option3;
		npc->act_no = 1;
		npc->count1 = 0;

		if (CompareTileTypeNPC(npc, TileType::TT_MOVE_LEFT))
		{
			// | <- [US]
			npc->x     += 0x2000;
			npc->tgt_x  = npc->x - 0x2000;
			npc->tgt_y  = 0;
			npc->direct = 2;
		}
		else if (CompareTileTypeNPC(npc, TileType::TT_MOVE_RIGHT))
		{
			// [US] -> |
			npc->x     -= 0x2000;
			npc->tgt_x  = npc->x + 0x2000;
			npc->tgt_y  = 0;
			npc->direct = 2;
		}
		else if (CompareTileTypeNPC(npc, TileType::TT_MOVE_DOWN))
		{
			// [US]
			//   |
			//   V
			// ----
			npc->y     -= 0x2000;
			npc->tgt_x  = 0;
			npc->tgt_y  = npc->y + 0x2000;
			npc->direct = 0;
		}
		else if (CompareTileTypeNPC(npc, TileType::TT_MOVE_UP))
		{
			// ----
			//   ^
			//   |
			// [US]
			npc->y     += 0x2000;
			npc->direct = 0;

			float fLastScore = -1.f;

			// We have to find a suitable place to put the puppy
			for (int x = (npc->x / 0x2000) - 4; x < (npc->x / 0x2000) + 4; x++)
			{
				for (int y = (npc->y / 0x2000) - 5; y < (npc->y / 0x2000) - 1; y++)
				{
					// Skip unsuitable tiles
					if (CompareTileTypeXY(x, y, TileType::TT_ANY_TEAM) || CompareTileTypeXY(x, y, TileType::TT_NO_NPC) || !CompareTileTypeXY(x, y + 1, TileType::TT_NO_NPC))
						continue;

					// Calculate a score based on distance
					float fOurScore = dist(npc->x, npc->y - 0x2000, x * 0x2000, y * 0x2000);

					// And compare it to the current best score
					if (fLastScore != -1.f && fOurScore >= fLastScore)
						continue;

					fLastScore = fOurScore;
					npc->tgt_x = x * 0x2000;
					npc->tgt_y = y * 0x2000;
				}
			}
		}
	}

	if ((int)g_NodeEditor_Enabled != npc->count1)
	{
		npc->count1 = (int)g_NodeEditor_Enabled;

		// Set rect accordingly
		if (g_NodeEditor_Enabled)
			npc->rect = { 0, 160, 16, 176 };
		else
			npc->rect = { 0, 0, 0, 0 };
	}

	if (CaveNet_Client_IsConnected())
	{
		CaveNet::DataStructures::NetClientIterator it;

		float fScore = -1.f;

		// Find the closest player
		for (CaveNet::DataStructures::NetClient* pClient = it.first(CaveNet_Server_IsHosting()); pClient; pClient = it.next())
		{
			MYCHAR* pMC = pClient->GetMyChar();

			// We're looking for players to get close to.
			if (!pMC)
				continue;

			// Calculate our score
			float fOurScore = dist(pMC->x, pMC->y, npc->x, npc->y);

			if (npc->direct == 0 && pMC->y > (npc->y - npc->hit.top) && pMC->y < (npc->y + npc->hit.bottom))
			{
				// Horizontal
				if (fScore == -1.f || fOurScore < fScore)
				{
					fScore      = fOurScore;
					npc->focus  = pMC;
					npc->client = pClient;
				}
			}
			else if (npc->direct == 2 && pMC->x > (npc->x - npc->hit.left) && pMC->x < (npc->x + npc->hit.right))
			{
				// Vertical
				if (fScore == -1.f || fOurScore < fScore)
				{
					fScore      = fOurScore;
					npc->focus  = pMC;
					npc->client = pClient;
				}
			}
		}
	}

	if (npc->direct == 0)
	{
		// Horizontal; Move along X axis
		int iTargetX = npc->focus->x / 0x2000;
		int iOurX    = npc->x / 0x2000;
		int iOurY    = npc->y / 0x2000;
		int iMoveAmt = CLAMP(npc->focus->x - npc->x, -1, 1);

		if (iMoveAmt == 0)
			return;

		while (iOurX != iTargetX)
		{
			if (CompareTileTypeXY(iOurX + iMoveAmt, iOurY, TileType::TT_NO_NPC))
				break;

			iOurX += iMoveAmt;
		}

		// Set our position
		if (iOurX == iTargetX)
		{
			// -1 = Move Left
			//  1 = Move Right
			if (!CompareTileTypeXY(iOurX + iMoveAmt, iOurY, TileType::TT_NO_NPC) || (iMoveAmt == 1 && npc->focus->x + npc->hit.right < (iOurX * 0x2000)) || (iMoveAmt == -1 && npc->focus->x - npc->hit.left > (iOurX * 0x2000)))
				npc->x = npc->focus->x;
			else
				npc->x = iOurX * 0x2000;
		}
		else
			npc->x = iOurX * 0x2000;
	}
	else
	{
		// Vertical; Move along Y axis
		int iTargetY = npc->focus->y / 0x2000;
		int iOurX    = npc->x / 0x2000;
		int iOurY    = npc->y / 0x2000;
		int iMoveAmt = CLAMP(npc->focus->y - npc->y, -1, 1);

		if (iMoveAmt == 0)
			return;

		while (iOurY != iTargetY)
		{
			if (CompareTileTypeXY(iOurX, iOurY + iMoveAmt, TileType::TT_NO_NPC))
				break;

			iOurY += iMoveAmt;
		}

		// Set our position
		if (iOurY == iTargetY)
		{
			// -1 = Move Up
			// 1  = Move Down
			if (!CompareTileTypeXY(iOurX, iOurY + iMoveAmt, TileType::TT_NO_NPC) || (iMoveAmt == 1 && npc->focus->y + npc->hit.bottom < (iOurY * 0x2000)) || (iMoveAmt == -1 && npc->focus->y - npc->hit.top > (iOurY * 0x2000)))
				npc->y = npc->focus->y;
			else
				npc->y = iOurY * 0x2000;
		}
		else
			npc->y = iOurY * 0x2000;
	}
}

void PutNpc417(NPCHAR* npc, int fx, int fy)
{
	PutSpecificNpChar(
		npc,
		(npc->x < 0 ? fx - 0x3000 : (npc->x >= (gMap.width * 0x2000) ? fx + 0x3000 : fx)),
		(npc->y < 0 ? fy - 0x3000 : (npc->y >= (gMap.length *0x2000) ? fy + 0x3000 : fy)),
		true
	);
}

// Kill Zone (CTP)
void ActNpc417(NPCHAR* npc)
{
	if (npc->act_no == 0)
	{
		// Initialization act
		npc->act_no = 1;
		npc->count1 = 0;
		npc->direct = ((npc->bits & NPC_flags::npc_eventTouch) ? Directions::DIRECT_UP : Directions::DIRECT_LEFT);

		if (npc->direct == 0)
		{
			// Horizontal movement; Get min & max X values
			npc->tgt_x = 0;						//< Min
			npc->tgt_y = (gMap.width * 0x2000);	//< Max

			// Min first
			for (int i = (npc->x / 0x2000); i >= 0; i--)
			{
				// Stop at a solid block
				if (CompareTileTypeXY(i, (npc->y / 0x2000), TileType::TT_NO_NPC))
				{
					npc->tgt_x = (i * 0x2000) + 0x2000;
					break;
				}
			}

			// Max next
			for (int i = (npc->x / 0x2000); i < gMap.width; i++)
			{
				// Stop at a solid block
				if (CompareTileTypeXY(i, (npc->y / 0x2000), TileType::TT_NO_NPC))
				{
					npc->tgt_y = (i * 0x2000) - 0x2000;
					break;
				}
			}
		}
		else
		{
			// Vertical movement; Get min & max Y values
			npc->tgt_x = 0x1000;					//< Min
			npc->tgt_y = (gMap.length * 0x2000);	//< Max

			// Min first
			for (int i = (npc->y / 0x2000); i >= 0; i--)
			{
				// Stop at a solid block
				if (CompareTileTypeXY((npc->x / 0x2000), i, TileType::TT_NO_NPC))
				{
					npc->tgt_x = (i * 0x2000) + 0x2000;
					break;
				}
			}

			// Max next
			for (int i = (npc->y / 0x2000); i < gMap.length; i++)
			{
				// Stop at a solid block
				if (CompareTileTypeXY((npc->x / 0x2000), i, TileType::TT_NO_NPC))
				{
					npc->tgt_y = (i * 0x2000) - 0x2000;
					break;
				}
			}
		}

		// Now determine if they want to go OOB
		if (npc->bits & NPC_flags::npc_altDir)
		{
			// Option2 -> Go OOB one tile
			if (npc->direct == 0)
			{
				// Horizontal movement; Only go up or down one tile

				// Add one tile position
				if ((npc->y / 0x2000) == 0)						npc->y = -0x2000;
				else if ((npc->y / 0x2000) == gMap.length - 1)	npc->y = (gMap.length * 0x2000);
			}
			else
			{
				// Vertical movement; Only go left or right one tile
				if ((npc->x / 0x2000) == 0)						npc->x = -0x2000;
				else if ((npc->x / 0x2000) == gMap.width - 1)	npc->x = (gMap.width * 0x2000);
			}
			
		}

		// Setup bits
		npc->bits &= ~NPC_flags::npc_eventTouch;
		npc->bits |= NPC_flags::npc_option3;
	}

	if ((int)g_NodeEditor_Enabled != (int)npc->count1)
	{
		npc->count1 = g_NodeEditor_Enabled;

		// Set rect accordingly
		if (g_NodeEditor_Enabled)
		{
			npc->rect    = { 16, 160, 32, 176 };
			npc->putFunc = PutNpc417;
		}
		else
		{
			npc->rect    = { 0, 0, 0, 0 };
			npc->putFunc = NULL;
		}
	}

	if (CaveNet_Client_IsConnected())
	{
		CaveNet::DataStructures::NetClientIterator it;

		float fScore = -1.f;

		// Find the closest player
		for (CaveNet::DataStructures::NetClient* pClient = it.first(CaveNet_Server_IsHosting()); pClient; pClient = it.next())
		{
			MYCHAR* pMC = pClient->GetMyChar();

			// We're looking for players to get close to.
			if (!pMC)
				continue;

			// Calculate our score
			float fOurScore = dist(pMC->x, pMC->y, npc->x, npc->y);

			if (npc->direct == 0 && pMC->y > (npc->y - npc->hit.top) && pMC->y < (npc->y + npc->hit.bottom))
			{
				// Horizontal
				if (fScore == -1.f || fOurScore < fScore)
				{
					fScore      = fOurScore;
					npc->focus  = pMC;
					npc->client = pClient;
				}
			}
			else if (npc->direct == 2 && pMC->x > (npc->x - npc->hit.left) && pMC->x < (npc->x + npc->hit.right))
			{
				// Vertical
				if (fScore == -1.f || fOurScore < fScore)
				{
					fScore      = fOurScore;
					npc->focus  = pMC;
					npc->client = pClient;
				}
			}
		}
	}

	if (npc->direct == 0)
	{
		// Horizontal; Move along X axis
		npc->x = CLAMP(npc->focus->x, npc->tgt_x, npc->tgt_y);
	}
	else
	{
		// Vertical; Move along Y axis
		npc->y = CLAMP(npc->focus->y, npc->tgt_x, npc->tgt_y);
	}
}

void PutNpc418(NPCHAR* npc, int fx, int fy)
{
	PutSpecificNpChar(npc, fx, fy, true);

	// Calculate open amount
	if (npc->count3)
	{
		// Make ourselves dirty
		if (!npc->count4)
			npc->count4 = 1;

		if (npc->count3 < 13)
		{
			// Increment it
			++npc->count3;

			// Opening the shutter
			npc->count1 = (4 * (npc->count3 - 1) / 12);
		}
		else if (npc->count3 <= 20)
		{
			// Increment it
			++npc->count3;

			// Shutter is open.
			npc->count1 = 4;
		}
		else if (npc->count3 <= 121)
		{
			// Increment it
			++npc->count3;

			// Shutter is open.
			npc->count1 = 5;
		}
		else if (npc->count3 < 133)
		{
			// Increment it
			++npc->count3;

			// Close the shutter.
			npc->count1 = 5 + ((4 * (npc->count3 - 121) / 12) == 4 ? -5 : (4 * (npc->count3 - 121) / 12));
		}
		else
		{
			// Reset
			npc->count1 = 0;
			npc->count3 = 0;
			npc->count4 = 0; //< Make ourselves dirty again
		}
	}

	// Draw the screen first
	RECT rcScreen = { 268, 140 + (npc->ani_no * 9), 284, 149 + (npc->ani_no * 9) };

	PutBitmap3(
		&grcGame,
		(SubpixelToScreenCoord(npc->x - npc->view.left) - SubpixelToScreenCoord(gFrame.x)) + PixelToScreenCoord(5),
		(SubpixelToScreenCoord(npc->y - npc->view.top) - SubpixelToScreenCoord(gFrame.y)) + PixelToScreenCoord(12),
		&rcScreen,
		npc->surf
	);

	// Draw the shutter next
	RECT rcShutter = { 284, 140 + (npc->count1 * 4), 292, 144 + (npc->count1 * 4) };

	PutBitmap3(
		&grcGame,
		(SubpixelToScreenCoord(npc->x - npc->view.left) - SubpixelToScreenCoord(gFrame.x)) + PixelToScreenCoord(9),
		(SubpixelToScreenCoord(npc->y - npc->view.top) - SubpixelToScreenCoord(gFrame.y)) + PixelToScreenCoord(27),
		&rcShutter,
		npc->surf
	);
}

// Resupply Cabinet (CTP)
void ActNpc418(NPCHAR* npc)
{
	// Process act
	switch (npc->act_no)
	{
		// Init
		case 0:
		{
			// Set it to its rect
			npc->rect    = { 292, 140, 320, 176 };
			npc->putFunc = PutNpc418;

			if (!CaveNet_Client_IsConnected() || CaveNet_Server_IsHosting())
			{
				if (npc->bits & NPC_flags::npc_eventTouch)
					npc->x += 0x1000;

				npc->y -= 0x1400;

				npc->bits  &= ~NPC_flags::npc_eventTouch;
				npc->bits  |= NPC_flags::npc_option3;
			}

			npc->act_no = 1;
			npc->count1 = 0;

			// Fallthrough
		}
		// Decide how to flicker
		case 1:
		{
			unsigned char a = Random(0, 30);

			if (a < 10)
			{
				// Turn screen off for a period of time
				npc->act_no = 2;
				npc->ani_no = 0;
			}
			else if (a < 25)
				npc->act_no = 3;
			else
			{
				// Turn screen on for a period of time
				npc->act_no = 2;
				npc->ani_no = ((Random(0, 1000) < 30) ? 2 : 1);
			}

			npc->act_wait = Random(0x10, 0x40);
			npc->ani_wait = 0;
			break;
		}
		// Have screen be off for a short amount of time
		case 2:
		{
			if (--npc->act_wait == 0)
				npc->act_no = 1;

			break;
		}
		// Flicker randomly
		case 3:
		{
			if (++npc->ani_wait % 2)
				npc->ani_no = 0;
			else
				npc->ani_no = 1;

			if (--npc->act_wait == 0)
				npc->act_no = 1;

			break;
		}
	}
}