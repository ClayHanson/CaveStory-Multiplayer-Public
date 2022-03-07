#include "../Weapons/ModInfo.h"
#include <Subclasses/GameDemo.h>

#define TEAM(TYPE) CaveNet::DataStructures::NetTeam::TeamType::TEAM_##TYPE
static RECT Dll_grcGame                                        = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
static const char* pTeamNames[]                                = { "Yellow", "Green", "Red", "Blue" };
static GUI_COLOR pTeamColors[]                                 = { GUI_COLOR(255, 255, 0, 255), GUI_COLOR(0, 255, 0, 255), GUI_COLOR(255, 0, 0, 255), GUI_COLOR(0, 0, 255, 255) };
static CaveNet::DataStructures::NetTeam::TeamType gTeamTypes[] = { TEAM(YELLOW), TEAM(GREEN), TEAM(RED), TEAM(BLUE) };
#undef TEAM

#define SET_FLAG(x, i) ((x)[(i) / 8] |= 1 << (i) % 8)
#define UNSET_FLAG(x, i) ((x)[(i) / 8] &= ~(1 << (i) % 8))
#define GET_FLAG(x, i) ((x)[(i) / 8] & (1 << (i) % 8))

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Pathfinding AI Node
void ActNpc414(NPCHAR* npc)
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define PUPPY_FLAG_TIMEOUT 3000
#else
#define PUPPY_FLAG_TIMEOUT 20000
#endif

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void PutNpc411(NPCHAR* npc, int fx, int fy)
{
	int iDrawX = 0;
	int iDrawY = 0;

	// Draw the puppy
	PutSpecificNpChar(npc, fx, fy, true, &iDrawX, &iDrawY);

	// Now draw the puppy's collar
	PutBitmap8(&Dll_grcGame, iDrawX, iDrawY, &GUI_RECT(npc->rect.left, npc->rect.top + ((npc->rect.bottom - npc->rect.top) * 2), npc->rect.right - npc->rect.left, npc->rect.bottom - npc->rect.top).to_rect(), (Surface_Ids)npc->surf, pTeamColors[npc->code_flag].r, pTeamColors[npc->code_flag].g, pTeamColors[npc->code_flag].b);

	if (npc->count2 && CaveNet_Client_GetGlobalTimerOffset() >= npc->count2)
	{
		npc->count2 = 0;
	}
	else if (npc->act_no < 10 && npc->count2)
	{
		int iTimeLeft = ((npc->count2 - CaveNet_Client_GetGlobalTimerOffset()) / 1000) + 1;

		if (iTimeLeft >= 60)
		{
			int iMinute  = iTimeLeft / 60;
			int iSecond  = iTimeLeft % 60;
			int iOffsetX = ((iDrawX / magnification) + (npc->view.left / 0x200));
			iOffsetX    -= 17;

			Puppy_PutNumber(iOffsetX, (iDrawY / magnification) - 10, iMinute, false);
			Puppy_PutNumber(iOffsetX + 9, (iDrawY / magnification) - 10, iSecond, true);
		}
		else
		{
			Puppy_PutNumber(((iDrawX / magnification) + (npc->view.left / 0x200)) - (iTimeLeft > 9 ? 8 : 4), (iDrawY / magnification) - 10, iTimeLeft, false);
		}
	}
}

// Puppy (CTP)
void ActNpc411(NPCHAR* npc)
{
	RECT rcLeft[] =
	{
		// 00 : Sit
		DRECT(0, 0, 16, 16),
		// 01 : Sit (Blink)
		DRECT(16, 0, 16, 16),
		// 02 : Sit (Tail Wag)
		DRECT(32, 0, 16, 16),
		// 03 : Stand
		DRECT(48, 0, 16, 16),
		// 04 : Stand (Run)
		DRECT(64, 0, 16, 16),
		// 05 : Stand (Barking)
		DRECT(80, 0, 16, 16),
		// 06 : Dragon Dog 1
		DRECT(96, 0, 16, 17),
		// 07 : Dragon Dog 2
		DRECT(112, 0, 16, 17),
		// 08 : Bubble Small
		DRECT(128, 0, 16, 16),
		// 09 : Bubble Big
		DRECT(144, 0, 16, 16)
	};

	RECT rcRight[] =
	{
		// 00 : Sit
		DRECT(0, 16, 16, 16),
		// 01 : Sit (Blink)
		DRECT(16, 16, 16, 16),
		// 02 : Sit (Tail Wag)
		DRECT(32, 16, 16, 16),
		// 03 : Stand
		DRECT(48, 16, 16, 16),
		// 04 : Stand (Run)
		DRECT(64, 16, 16, 16),
		// 05 : Stand (Barking)
		DRECT(80, 16, 16, 16),
		// 06 : Dragon Dog 1
		DRECT(96, 17, 16, 17),
		// 07 : Dragon Dog 2
		DRECT(112, 17, 16, 17),
		// 08 : Bubble Small
		DRECT(128, 16, 16, 16),
		// 09 : Bubble Big
		DRECT(144, 16, 16, 16)
	};

	/* npc->count2 = Countdown timer */
	/* npc->count3 = Last time */

	if (!npc->putFunc)
		npc->putFunc = PutNpc411;

	if (npc->client2 && ++npc->count1 >= 48)
	{
		// Clear ignore variable
		npc->count1  = 0;
		npc->client2 = NULL;
	}

	if (npc->act_no <= 3 || npc->act_no == 7)
	{
		MAP_TEAM_INFO* pInfo = CTP_GetTeamInfo(npc->code_flag);

		if (pInfo)
		{
			if (!npc->count2) // Start the timer
				npc->count2 = CaveNet_Client_GetGlobalTimerOffset() + pInfo->puppy_respawn_time;
			else if (CaveNet_Client_GetGlobalTimerOffset() >= npc->count2)
			{
				// Timer ended!
				if (CaveNet_Server_IsHosting())
				{
					CaveNet::DataStructures::NetTeam* pTeam = CaveNet::DataStructures::NetTeam::FindTeamByType((CaveNet::DataStructures::NetTeam::TeamType)npc->code_flag, false);

					if (pTeam)
						CaveNet_Server_ClientGroup_TransmitSystemMessage("<color:FF5555>* <spush><color:%s>%s<spop> team's puppy is running back to its base!\r\n", pTeam->GetHexColor(), pTeam->GetName());
				}

				// Begin running back to our base
				if (npc->act_no == 7)
				{
					// We're flying above a pit, so we cannot run home. Just fly home instead
					goto START_FLYING_TO_BASE;
				}

				npc->count2   = 0;
				npc->act_no   = 10;
				npc->ani_no   = Random(3, 4);
				npc->ani_wait = 0;
				npc->bits    &= ~NPC_flags::npc_option3;
			}
			else
				npc->count3 = CaveNet_Client_GetGlobalTimerOffset();
		}
	}

	switch (npc->act_no)
	{
		// Init
		case 0:
		{
			// Find our team
			npc->act_no   = 1;
			npc->ani_no   = 0;
			npc->ani_wait = 0;

			// Fallthrough
		}
		// Sit & wait for someone to pick us up
		case 1:
		{
			if (Random(0, 120) == 10)
			{
				npc->act_no   = 2;
				npc->act_wait = 0;
				npc->ani_no   = 1;
			}

			break;
		}
		// Blinking
		case 2:
		{
			if (++npc->act_wait > 8)
			{
				npc->act_no = 1;
				npc->ani_no = 0;
			}

			break;
		}
		// Fall from being carried
		case 3:
		{
			if (npc->flag & NPC_collision::npc_collide_down)
			{
				if (npc->ani_no != 3)
				{
					// Stand on the ground
					npc->ani_no   = 3;
					npc->act_wait = 0;
				}
				else if (++npc->act_wait >= 24)
				{
					// Sit down
					npc->ani_no   = 0;
					npc->act_no   = 1;
					npc->act_wait = 0;
				}
			}
			else if (npc->ani_no != 4)
				npc->ani_no = 4; // Play running animation anytime we're not on the ground.
			else if (npc->y >= gMap.length * 0x2000)
			{
				// Fell off map!!
				npc->ym       = 0;
				npc->act_no   = 7;
				npc->ani_no   = 8;
				npc->act_wait = 0;
				npc->tgt_x    = npc->x;
				npc->tgt_y    = gMap.length * 0x2000;
				npc->bits    |= NPC_flags::npc_ignoreSolid;

				for (int i = 0; i < 4; i++)
				{
					int iNewY = gMap.length - i;

					if (!CompareTileTypeXY(npc->tgt_x / 0x2000, iNewY, TT_SOLID | TT_DAMAGE | TT_DESTROYABLE))
						npc->tgt_y = iNewY * 0x2000;
				}
			}

			break;
		}
		// Being carried
		case 4:
		{
			if (!npc->focus || npc->focus->ques || npc->y - npc->view.top >= (gMap.length * 0x2000) || !npc->focus->life)
			{
				CTP_DropPuppy(npc);
				break;
			}
			
			if (npc->act_wait == 0 && Random(0, 120) == 10)
			{
				npc->act_wait = 1;
				npc->ani_no   = 1;
			}
			else if (npc->act_wait != 0 && ++npc->act_wait >= 9)
			{
				npc->ani_no   = 0;
				npc->act_wait = 0;
			}

			if (npc->focus->direct == 0)
				npc->direct = 0;
			else
				npc->direct = 2;

			GUI_POINT pHeldOffset(0x800, 0x1400);

			if (npc->focus && npc->focus->character)
				CSM_Puppy_GetDrawOffset(npc->focus, &pHeldOffset);

			pHeldOffset.x = (pHeldOffset.x / magnification) * 0x200;
			pHeldOffset.y = (pHeldOffset.y / magnification) * 0x200;
			npc->x        = npc->focus->x - pHeldOffset.x;
			npc->y        = npc->focus->y + pHeldOffset.y;

			break;
		}
		// In bubble
		case 5:
		{
			npc->act_no   = 6;
			npc->ani_no   = 8;
			npc->act_wait = 0;

			break;
		}
		// In bubble, floating above capture zone
		case 6:
		{
			if (++npc->act_wait / 50 % 2)
				npc->y += 0x40;
			else
				npc->y -= 0x40;

			if (++npc->ani_wait > 8)
			{
				npc->ani_wait = 0;
				npc->ani_no   = (npc->ani_no == 8 ? 9 : 8);
			}

			break;
		}
		// Dragon dog, floating above pit
		case 7:
		{
			int TgtX = npc->tgt_x - (cosf(M_PI_F * (CaveNet_Client_GetGlobalTimerOffset() % 2000) / 1000) * 0x1000);
			int TgtY = npc->tgt_y - (sinf(M_PI_F * (CaveNet_Client_GetGlobalTimerOffset() % 2000) / 1000) * 0x1000);

			npc->x += CLAMP((TgtX - npc->x) / 12, -0x200, 0x200);
			npc->y += CLAMP((TgtY - npc->y) / 12, -0x200, 0x200);

			if (++npc->ani_wait > 4)
			{
				npc->ani_wait = 0;
				npc->ani_no   = (npc->ani_no == 7 ? 6 : 7);
			}

			break;
		}
		// Dragon dog, going to own capture point
		case 8:
		{
			if (dist(npc->x, npc->y, npc->tgt_x, npc->tgt_y) < 0x1600)
			{
				// Close enough.
				npc->act_no   = 5;
				npc->ani_no   = 8;
				npc->ani_wait = 0;
				npc->x        = npc->pNpc->x;
				npc->y        = npc->pNpc->y - 0x2400;
				npc->xm       = 0;
				npc->ym       = 0;
				npc->direct   = (npc->bits & NPC_flags::npc_altDir ? Directions::DIRECT_RIGHT : Directions::DIRECT_LEFT);
				npc->bits    |= NPC_flags::npc_option3;
				npc->bits    &= ~NPC_flags::npc_ignoreSolid;

				break;
			}

			if (npc->x < npc->tgt_x)
				npc->direct = Directions::DIRECT_RIGHT;
			else
				npc->direct = Directions::DIRECT_LEFT;

			npc->xm -= CLAMP((npc->x + npc->xm) - npc->tgt_x, -0x20, 0x20);
			npc->ym -= CLAMP((npc->y + npc->ym) - npc->tgt_y, -0x20, 0x20);

			if (npc->xm >  0x300)
				npc->xm =  0x300;
			if (npc->xm < -0x300)
				npc->xm = -0x300;

			if (npc->ym >  0x300)
				npc->ym =  0x300;
			if (npc->ym < -0x300)
				npc->ym = -0x300;

			npc->x += npc->xm;
			npc->y += npc->ym;

			if (++npc->ani_wait > 2)
			{
				npc->ani_wait = 0;
				npc->ani_no   = (npc->ani_no == 7 ? 6 : 7);
			}

			break;
		}
		// Begin flying back to base
		case 9:
		{
			goto START_FLYING_TO_BASE;
			break;
		}
		// Decide our next move
		case 10:
		{
			PuppyMoveType iMoveType;

			if (!Puppy_GetNextNode(npc, &iMoveType, &npc->tgt_x, &npc->tgt_y))
			{
			START_FLYING_TO_BASE:
				npc->act_no   = 8;
				npc->ani_no   = 8;
				npc->ani_wait = 0;
				npc->bits    &= ~NPC_flags::npc_option3;
				npc->bits    |= NPC_flags::npc_ignoreSolid;
				npc->tgt_x    = npc->pNpc->x;
				npc->tgt_y    = npc->pNpc->y - 0x2400;

				break;
			}

			npc->xm       = (iMoveType == PuppyMoveType::PUPPY_MOVE_JUMP ? 0 : ((iMoveType == PuppyMoveType::PUPPY_MOVE_RUN && npc->act_no == 12) ? (npc->tgt_x < npc->x ? -0x40 : 0x40) : npc->xm));
			npc->act_no   = (iMoveType == PuppyMoveType::PUPPY_MOVE_JUMP ? 12 : 11);
			npc->act_wait = 0;
			npc->ani_no   = 3;
			npc->ani_wait = 0;
			npc->bits    &= ~NPC_flags::npc_ignoreSolid;
			npc->count1   = npc->x > npc->tgt_x; // If count1 == 1, then our target is to the left of us.

			break;
		}
		// Run to our base
		case 11:
		{
			if ((npc->x > npc->tgt_x && npc->xm > 0) || (npc->x < npc->tgt_x && npc->xm < 0))
			{
				//npc->xm = 0;
				//npc->act_no = 13;
				//break;
			}

			if ((npc->flag & NPC_collision::npc_collide_down) && (dist(npc->x, npc->y, npc->tgt_x, npc->tgt_y) <= 0x1000 || (npc->count1 && npc->x <= npc->tgt_x) || (!npc->count1 && npc->x >= npc->tgt_x)))
			{
				// Continue movement
				npc->act_no = 10;

				break;
			}

			if ((npc->flag & NPC_collision::npc_collide_left) || (npc->flag & NPC_collision::npc_collide_right))
				npc->xm = 0;

			if (npc->flag & 8 || npc->flag & 0xf0000)
			{
				if (
					(npc->direct == Directions::DIRECT_LEFT && npc->xm < 0) ||
					(npc->direct == Directions::DIRECT_RIGHT && npc->xm > 0)
					)
				{
					// Run animation
					if (++npc->ani_wait > 2)
					{
						npc->ani_wait = 0;
						npc->ani_no   = (npc->ani_no == 3 ? 4 : 3);
					}
				}
				else
					// Sliding animation
					npc->ani_no = 3;
			}
			else
			{
				npc->ani_no   = 4;
				npc->ani_wait = 0;
			}

			if (npc->flag & NPC_collision::npc_collide_down || npc->flag & 0xf0000)
			{
				if (npc->x < npc->tgt_x)
					npc->direct = Directions::DIRECT_RIGHT;
				if (npc->x > npc->tgt_x)
					npc->direct = Directions::DIRECT_LEFT;

				// Go forward
				if (npc->direct == 0)
					npc->xm -= 0x40;
				else
					npc->xm += 0x40;
			}

			if (npc->xm > 0x5FF)
				npc->xm = 0x400;

			if (npc->xm < -0x5FF)
				npc->xm = -0x400;

			npc->x += npc->xm;

			break;
		}
		// Jump back to the next node
		case 12:
		{
			npc->count3 = (npc->count1 + (npc->tgt_x - npc->count1) / 2);
			npc->count4 = (npc->count2 + (npc->tgt_y - npc->count2) / 2) - 0x4000;

			switch (npc->xm)
			{
				case 0:
				{
					// Assuming point[0] and point[2] are your starting point and destination
					// and all points are Vector3
					// point[1] = start + (end - start) / 2 + Vector3.up * 5.0f; // Play with 5.0 to change the curve

					if (++npc->act_wait > 0)
					{
						npc->ani_no   = 4;
						npc->count1   = npc->x;
						npc->count2   = npc->y;
						npc->count3   = (npc->x + (npc->tgt_x - npc->x) / 2);
						npc->count4   = (npc->y + (npc->tgt_y - npc->y) / 2) - 0x4000;
						npc->act_wait = 0;
						npc->xm       = 1;
						npc->bits    |= NPC_flags::npc_ignoreSolid;

						if (npc->x < npc->tgt_x)
							npc->direct = Directions::DIRECT_RIGHT;
						if (npc->x > npc->tgt_x)
							npc->direct = Directions::DIRECT_LEFT;
					}
					else
					{
						npc->ani_no = 3;

						if (npc->x < npc->tgt_x)
							npc->direct = Directions::DIRECT_RIGHT;
						if (npc->x > npc->tgt_x)
							npc->direct = Directions::DIRECT_LEFT;

						break;
					}
					// Fallthrough
				}
				case 1:
				{
					#define JUMP_SPEED		20
					#define JUMP_SPEED_F	20.f

					// Follow the path
					if (npc->act_wait < JUMP_SPEED)
					{
						int m1_x = LerpInt((float)npc->count1, (float)npc->count3, ((float)npc->act_wait) / JUMP_SPEED_F);
						int m1_y = LerpInt((float)npc->count2, (float)npc->count4, ((float)npc->act_wait) / JUMP_SPEED_F);
						int m2_x = LerpInt((float)npc->count3, (float)npc->tgt_x, ((float)npc->act_wait) / JUMP_SPEED_F);
						int m2_y = LerpInt((float)npc->count4, (float)npc->tgt_y, ((float)npc->act_wait) / JUMP_SPEED_F);

						npc->x = (unsigned int)LerpInt((float)m1_x, (float)m2_x, ((float)npc->act_wait) / JUMP_SPEED_F);
						npc->y = (unsigned int)LerpInt((float)m1_y, (float)m2_y, ((float)npc->act_wait) / JUMP_SPEED_F);

						++npc->act_wait;

						break;
					}
					else
					{
						npc->xm     = 2;
						npc->ani_no = 3;
					}
				}
				case 2:
				{
					// Continue movement
					npc->count1 = 0;
					npc->count2 = 0;
					npc->count3 = 0;
					npc->count4 = 0;
					npc->act_no = 10;
					npc->xm     = (npc->direct == Directions::DIRECT_LEFT ? -0x5FF : 0x5FF);
					npc->bits  &= ~NPC_flags::npc_ignoreSolid;

					break;
				}
			}

			break;
		}
		// Slide towards objective
		case 13:
		{
			if (npc->x < npc->tgt_x)
				npc->direct = Directions::DIRECT_RIGHT;
			if (npc->x > npc->tgt_x)
				npc->direct = Directions::DIRECT_LEFT;

			if ((npc->direct == Directions::DIRECT_LEFT && npc->xm > 0) || (npc->direct == Directions::DIRECT_RIGHT && npc->xm < 0))
			{
				// Slow down considerably
				npc->ani_no = 3;
				npc->act_no = 13;
				npc->xm    /= 2;
			}
			else
			{
				npc->act_no = 10;
			}

			if (npc->xm > 0x5FF)
				npc->xm = 0x400;

			if (npc->xm < -0x5FF)
				npc->xm = -0x400;

			npc->x += npc->xm;

			break;
		}
		// Jump back to the next node
		case 14:
		{
			if (npc->flag & 8)
			{
				npc->ani_no = 3;
				npc->xm     = 0;

				if (dist(npc->x, npc->y, npc->tgt_x, npc->tgt_y) <= 0x1600 || (npc->count1 && npc->x <= npc->tgt_x) || (!npc->count1 && npc->x >= npc->tgt_x))
				{
					// Continue movement
					npc->act_no = 10;
					npc->xm     = 0;
					npc->ym     = 0;

					break;
				}
				else if (++npc->act_wait >= 16)
				{
					// Jump towards our target point
					unsigned char deg = 0;

					unsigned int tgt_x = npc->tgt_x - ((npc->x - npc->tgt_x) / 2);
					unsigned int tgt_y = npc->tgt_y - 0x4000;

					// Decide the direction of which we'll jump in
					if (npc->direct == 0)
						deg = GetArktan(npc->x - 0x2400 - tgt_x, npc->y - tgt_y);
					else
						deg = GetArktan(npc->x + 0x2400 - tgt_x, npc->y - tgt_y);

					deg         = GetArktan(npc->x - tgt_x, npc->y - tgt_y);
					float fDist = dist(npc->x, npc->y, tgt_x, tgt_y) / 0x2000;

					npc->xm       = GetCos(deg) * (fDist / 2.f);
					npc->ym       = GetSin(deg) * fDist;
					npc->ani_no   = 4;
				}

				// Face towards the target
				if (npc->x < npc->tgt_x)
					npc->direct = Directions::DIRECT_RIGHT;
				if (npc->x > npc->tgt_x)
					npc->direct = Directions::DIRECT_LEFT;
			}
			
			// If we hit our head / a wall, then stop the respective motion
			if ((npc->flag & NPC_collision::npc_collide_left) || (npc->flag & NPC_collision::npc_collide_right))
				npc->xm = 0;
			if (npc->flag & NPC_collision::npc_collide_up)
				npc->ym = 0;

			if (npc->xm > 0x5FF)
				npc->xm = 0x400;

			if (npc->xm < -0x5FF)
				npc->xm = -0x400;

			npc->x += npc->xm;
			
			break;
		}
	}

	if (npc->act_no < 4 || npc->act_no == 11 || npc->act_no == 13)
	{
		npc->ym += 0x40;
		if (npc->ym > 0x5FF)
			npc->ym = 0x5FF;

		npc->y += npc->ym;
	}

	if (npc->direct == 0)
		npc->rect = rcLeft[npc->ani_no];
	else
		npc->rect = rcRight[npc->ani_no];
}

// Capture Point
void ActNpc412(NPCHAR* npc)
{
	RECT rcRects[] =
	{
		// 00 : Yellow
		DRECT(288, 16, 32, 12),
		// 01 : Green
		DRECT(288, 28, 32, 12),
		// 02 : Red
		DRECT(288, 40, 32, 12),
		// 03 : Blue
		DRECT(288, 52, 32, 12)
	};

	if (npc->ym2 == 0)
	{
		NPCHAR* pFlag                              = NULL;
		CaveNet::DataStructures::NetTeam* pOurTeam = NULL;

		if (!CaveNet_Client_IsConnected() || CaveNet_Server_IsHosting())
		{
			// Offset ourselves
			npc->x += 0x1000;
			npc->y += 0x400;

			// Create the flag NPC
			SetNpChar(411, npc->x, npc->y - 0x2400, 0, 0, npc->bits & NPC_flags::npc_altDir ? Directions::DIRECT_RIGHT : Directions::DIRECT_LEFT, npc, 1, &pFlag);

			// Pass on the alternate direction flag
			if (npc->bits & NPC_flags::npc_altDir)
				pFlag->bits |= NPC_flags::npc_altDir;

			// Configure it
			pFlag->bits     |= NPC_flags::npc_option3;
			pFlag->code_flag = npc->code_flag;
			pFlag->act_no    = 5;
			npc->pNpc        = pFlag;

			/*
			if (npc->code_flag == CaveNet::DataStructures::NetTeam::TeamType::TEAM_BLUE)
			{
				pFlag->x      = 84 * 0x2000;
				pFlag->y      = 27 * 0x2000;
				pFlag->act_no = 10;
			}
			*/
		}

		if (CaveNet_Server_IsHosting())
		{
			pOurTeam   = CaveNet::DataStructures::NetTeam::FindTeamByType(gTeamTypes[npc->code_flag], false);
			npc->xm2   = (unsigned int)pOurTeam;
			pFlag->xm2 = npc->xm2;

			if (((CaveNet::DataStructures::NetTeam*)npc->xm2) == NULL)
				CSM_Log("[ERROR] Failed to find team %s!\r\n", pTeamNames[npc->code_flag]);
		}
		else if (CaveNet_Client_IsConnected())
		{
			pOurTeam   = CaveNet::DataStructures::NetTeam::FindTeamByType(gTeamTypes[npc->code_flag], true);
			npc->xm2   = (unsigned int)pOurTeam;
			//pFlag->xm2 = npc->xm2;

			if (((CaveNet::DataStructures::NetTeam*)npc->xm2) == NULL)
				CSM_Log("[ERROR] Failed to find team %s!\r\n", pTeamNames[npc->code_flag]);
		}

		npc->ym2 = 1;
	}

	npc->rect = rcRects[npc->code_flag];
}

// Arrow
void ActNpc413(NPCHAR* npc)
{
	/* npc->direct == 0 ::: UP-LEFT */
	/* npc->direct == 1 ::: UP */
	/* npc->direct == 2 ::: UP-RIGHT */
	/* npc->direct == 3 ::: LEFT */
	/* npc->direct == 4 ::: INVALID */
	/* npc->direct == 5 ::: RIGHT */
	/* npc->direct == 6 ::: DOWN-LEFT */
	/* npc->direct == 7 ::: DOWN */
	/* npc->direct == 8 ::: DOWN-RIGHT */

	RECT pSpriteList[] =
	{
		// up left
		DRECT(224, 24, 16, 16),
		// up
		DRECT(224, 8, 16, 16),
		// up right
		DRECT(240, 24, 16, 16),
		// left
		DRECT(272, 8, 16, 16),
		// invalid
		DRECT(0, 0, 16, 16),
		// right
		DRECT(256, 8, 16, 16),
		// down-left
		DRECT(256, 24, 16, 16),
		// down
		DRECT(240, 8, 16, 16),
		// down-right
		DRECT(272, 24, 16, 16)
	};

	if (!npc->pNpc)
	{
		npc->x = gMC.x;
		npc->y = gMC.y - gMC.view.top - gMC.view.bottom;
	}
	else
	{
		npc->x = npc->pNpc->x;
		npc->y = npc->pNpc->y - npc->pNpc->view.top - npc->pNpc->view.bottom;

		if (!IsNpcOnScreen(npc->pNpc))
		{
			npc->x = CLAMP(npc->x, gFrame.x + npc->view.left, gFrame.x + (WINDOW_WIDTH * 0x200) - npc->view.right);
			npc->y = CLAMP(npc->y, gFrame.y + npc->view.top, gFrame.y + (WINDOW_HEIGHT * 0x200) - npc->view.bottom);

			if (npc->pNpc->x < gFrame.x && npc->pNpc->y < gFrame.y)															npc->direct = 0;
			else if (npc->pNpc->x < gFrame.x && npc->pNpc->y > gFrame.y + (WINDOW_HEIGHT * 0x200))							npc->direct = 6;
			else if (npc->pNpc->x > gFrame.x + (WINDOW_WIDTH * 0x200) && npc->pNpc->y < gFrame.y)							npc->direct = 2;
			else if (npc->pNpc->x > gFrame.x + (WINDOW_WIDTH * 0x200) && npc->pNpc->y > gFrame.y + (WINDOW_HEIGHT * 0x200)) npc->direct = 8;
			else if (npc->pNpc->x < gFrame.x)																				npc->direct = 3;
			else if (npc->pNpc->x > gFrame.x + (WINDOW_WIDTH * 0x200))														npc->direct = 5;
			else if (npc->pNpc->y < gFrame.y)																				npc->direct = 1;
			else if (npc->pNpc->y > gFrame.y + (WINDOW_HEIGHT * 0x200))														npc->direct = 7;
		}
		else
			npc->direct = 7;
	}

	if (--npc->ani_wait <= 0)
	{
		npc->ani_wait = 8;
		npc->ani_no   = !npc->ani_no;

		if (!npc->ani_no)
			npc->rect = pSpriteList[npc->direct];
		else
			npc->rect = { 0, 0, 0, 0 };
	}
}