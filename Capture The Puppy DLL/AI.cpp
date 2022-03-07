#include "Weapons/ModInfo.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define TEST_TEAM(COLOR) \
			if (pTeam->mTeamType != CaveNet::DataStructures::NetTeam::TeamType::TEAM_##COLOR && CaveNet::DataStructures::NetTeam::FindTeamByType(CaveNet::DataStructures::NetTeam::TeamType::TEAM_##COLOR, false)) \
				pValidTeamTable[iValidTeamCount++] = CaveNet::DataStructures::NetTeam::TeamType::TEAM_##COLOR;

#define AI_PATH_LIST	*((List<MAP_PUPPY_NODE_MOVE_INFO>**)&npc->m_CustomVarList[0])
#define AI_PATH_INDEX	*((int*)&npc->m_CustomVarList[1])
#define CURRENT_DEST_X	*((int*)&npc->m_CustomVarList[2])
#define CURRENT_DEST_Y	*((int*)&npc->m_CustomVarList[3])
#define FOCUSED_TEAM	*((int*)&npc->m_CustomVarList[4])

#define GKEY		*(iKey)
#define GKEYTRG		*(iKeyTrg)

static GUI_POINT Math_Normalize(GUI_POINT vect)
{
	float fX      = (float)vect.x / 512.f;
	float fY      = (float)vect.y / 512.f;
	float fLength = sqrtf((fX * fX) + (fY * fY));

	// Normalize product!
	return GUI_POINT(
		(int)(512.f * fX / fLength),
		(int)(512.f * fY / fLength)
	);
}

static float Math_Dot(GUI_POINT vect_A, GUI_POINT vect_B)
{
	float fX_A     = (float)vect_A.x / 512.f;
	float fY_A     = (float)vect_A.y / 512.f;
	float fX_B     = (float)vect_B.x / 512.f;
	float fY_B     = (float)vect_B.y / 512.f;
	float fProduct = (fX_A * fX_B) + (fY_A * fY_B);

	return fProduct;
}

void CTP_FreeAllBots()
{
	if (!CaveNet_Server_IsHosting())
		return;

	CaveNet::DataStructures::NetClientIterator it;

	// Loop through all clients & find bots
	for (CaveNet::DataStructures::NetClient* walk = it.first(true); walk; walk = it.next())
	{
		// Skip invalid clients
		if (!walk->mIsBot || !walk->GetPlayer() || !walk->GetPlayer()->m_Ai.brain)
			continue;

		CaveNet::DataStructures::AI_Brain* npc = walk->GetPlayer()->m_Ai.brain;

		delete AI_PATH_LIST;
		AI_PATH_INDEX    = 0;
		npc->m_OnCleanup = NULL;
	}
}

void CTP_OnProcessAI(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, CaveNet::DataStructures::NetTeam* pTeam)
{
	if (!pTeam)
		return;

	CaveNet::DataStructures::AI_Brain* npc       = pPlayer->m_Ai.brain;
	MYCHAR* chr                                  = &pPlayer->m_Player.npc;
	int iTeamType                                = pTeam->mTeamType;
	int* iKey                                    = &pClient->mCache.gameKey;
	int* iKeyTrg                                 = &pClient->mCache.gameKeyTrg;
	CaveNet::DataStructures::NetTeam* pFocusTeam = CaveNet::DataStructures::NetTeam::FindTeamByType((CaveNet::DataStructures::NetTeam::TeamType)FOCUSED_TEAM, false);

	if (!AI_PATH_LIST)
	{
		// INIT!!
		AI_PATH_LIST  = new List<MAP_PUPPY_NODE_MOVE_INFO>();
		AI_PATH_INDEX = 0;

		npc->m_OnCleanup = [](CaveNet::DataStructures::AI_Brain* npc)
		{
			delete AI_PATH_LIST;
			AI_PATH_INDEX = 0;
		};
	}
		
	Directions tgt_direct   = (npc->tgt_x < chr->x ? Directions::DIRECT_LEFT : Directions::DIRECT_RIGHT);
	bool moving_towards_tgt = (tgt_direct == Directions::DIRECT_LEFT ? (chr->xm < 0) : (chr->xm > 0));

	bool bJump  = false;
	bool bLeft  = false;
	bool bRight = false;
	bool bUp    = false;
	bool bDown  = false;
	bool bShot  = false;

	if (npc->act_no == 3 || npc->act_no == 4 || npc->act_no == 5 || npc->act_no == 6)
	{
		if (chr->x != npc->timer_1 || chr->y != npc->timer_2)
		{
			npc->timer_1 = chr->x;
			npc->timer_2 = chr->y;
			npc->count2 = 0;
		}

		if (++npc->count2 >= 64)
		{
			npc->count2 = 0;
			npc->act_no = 2;
		}
	}

	if (pPlayer->m_Player.dead && npc->act_no != 10)
		npc->act_no = 10;

	// If we're not carrying a puppy, then select a team type that is different to ours.
	if (!pClient->mCache.collectable_count)
		iTeamType = FOCUSED_TEAM;

	int iDestX = 0;
	int iDestY = 0;

	if (npc->act_no >= 2)
	{
		if (!pClient->mCache.collectable_count && pFocusTeam)
		{
			iDestX = pFocusTeam->mFlagNpc->x;
			iDestY = pFocusTeam->mFlagNpc->y;
		}
		else if (pClient->mCache.collectable_count && !(pTeam->mFlagNpc->bits & NPC_flags::npc_option3))
		{
			// Someone has our flag, so we cannot capture the flag WE have! Kill. Kill. Kill. Kill.
			if (pTeam->mFlagNpc->focus)
			{
				iDestX = pTeam->mFlagNpc->focus->x;
				iDestY = pTeam->mFlagNpc->focus->y;
			}
			else
			{
				iDestX = pTeam->mFlagNpc->x;
				iDestY = pTeam->mFlagNpc->y;
			}
		}
		else if (pClient->mCache.collectable_count && (pTeam->mFlagNpc->bits & NPC_flags::npc_option3))
		{
			iDestX = pTeam->mFlagNpc->x;
			iDestY = pTeam->mFlagNpc->y;
		}

		unsigned int iDist = dist(CURRENT_DEST_X, CURRENT_DEST_Y, iDestX, iDestY);

		if (npc->act_no != 2 && iDist >= 0x4000)
			npc->act_no = 2;
	}
	
	// Determine what to do
	switch (npc->act_no)
	{
		case 0:
		{
			if (++npc->count8 < 2)
				return;

			npc->act_no = 1;
			npc->count8 = 0;

			// Fallthrough
		}
		// Determine which team to go after
		case 1:
		{
			int pValidTeamTable[4] = { 0, 0, 0, 0 };
			int iValidTeamCount    = 0;
			npc->act_no            = 2;

			TEST_TEAM(YELLOW);
			TEST_TEAM(GREEN);
			TEST_TEAM(RED);
			TEST_TEAM(BLUE);

			FOCUSED_TEAM = pValidTeamTable[Random(0, iValidTeamCount - 1)];

			return;
		}
		// Determine the next movement we should make
		case 2:
		{
			if (AI_PATH_INDEX < (AI_PATH_LIST)->mListCount)
			{
			GOTO_NEXT_PATH_ON_LIST:
				MAP_PUPPY_NODE_MOVE_INFO* pMoveInfo = &(*(AI_PATH_LIST))[(AI_PATH_INDEX)++];

				//CSM_Log("Move # %d: Move to (%d, %d) AKA (%.2f, %.2f) by %s.\r\n", AI_PATH_INDEX - 1, pMoveInfo->tgt_x, pMoveInfo->tgt_y, (float)pMoveInfo->tgt_x / 8192.f, (float)pMoveInfo->tgt_y / 8192.f, (pMoveInfo->move_type == PuppyMoveType::PUPPY_MOVE_JUMP ? "jumping" : "running"));

				npc->tgt_x                          = pMoveInfo->tgt_x;
				npc->tgt_y                          = pMoveInfo->tgt_y;
				npc->act_no                         = (pMoveInfo->move_type == PuppyMoveType::PUPPY_MOVE_JUMP ? 4 : 3);
			}
			else if ((CURRENT_DEST_X != iDestX || CURRENT_DEST_Y != iDestY) && Puppy_PopulateMoveList(AI_PATH_LIST, chr->x, chr->y, iDestX, iDestY))
			{
				// We got a node
				AI_PATH_INDEX  = 0;
				CURRENT_DEST_X = iDestX;
				CURRENT_DEST_Y = iDestY;
				goto GOTO_NEXT_PATH_ON_LIST;
			}
			else
			{
				// We must move towards the goal
				npc->act_no = 3;
				npc->tgt_x  = pFocusTeam->mFlagNpc->x;
				npc->tgt_y  = pFocusTeam->mFlagNpc->y;
			}

			npc->count2 = 0; // Timeout
			npc->count1 = chr->x > npc->tgt_x; // If count1 == 1, then our target is to the left of us.

			break;
		}
		// Run to node
		case 3:
		{
			if ((pClient->mCache.collectable_count && !(pTeam->mFlagNpc->bits & NPC_flags::npc_option3)) || (chr->flag & NPC_collision::npc_collide_down) && (dist(chr->x, chr->y, npc->tgt_x, npc->tgt_y) <= 0x1000 || (npc->count1 && chr->x <= npc->tgt_x) || (!npc->count1 && chr->x >= npc->tgt_x)))
			{
				// Continue movement
				npc->act_no = 2;
				break;
			}

			// Look at the target
			chr->direct = (npc->tgt_x < chr->x ? Directions::DIRECT_LEFT : Directions::DIRECT_RIGHT);

			// Go forwards
			*(chr->direct == Directions::DIRECT_LEFT ? &bLeft : &bRight) = true;

			break;
		}
		// Jump to node init
		case 4:
		{
			npc->act_no   = 5;
			npc->count3   = false;															//< Have we stopped jumping
			npc->count4   = 0;																//< Index number on which arc we're on
			npc->count5   = chr->x;															//< Start X
			npc->count6   = chr->y;															//< Start Y
			npc->count7   = (npc->count1 + (npc->tgt_x - npc->count1) / 2);					//< Midpoint X
			npc->count8   = (npc->count2 + (npc->tgt_y - npc->count2) / 2) - 0x4000;		//< Midpoint Y
			npc->act_wait = 0;
			bJump         = false;

			// Fallthrough
		}
		// Jump to node
		case 5:
		{
			if ((chr->flag & NPC_collision::npc_collide_down) && (dist(chr->x, chr->y, npc->tgt_x, npc->tgt_y) <= 0x1000 || (npc->count1 && chr->x <= npc->tgt_x) || (!npc->count1 && chr->x >= npc->tgt_x)))
			{
				// Continue movement
				npc->act_no = 2;
				break;
			}

			// Look at the target
			chr->direct = tgt_direct;

			// Go forwards
			*(tgt_direct == Directions::DIRECT_LEFT ? &bLeft : &bRight) = true;

			if (
				(chr->flag & GMC_flag::gmc_flag_collide_floor) && 
				(
					(tgt_direct == Directions::DIRECT_LEFT && chr->xm <= 0) ||
					(tgt_direct == Directions::DIRECT_RIGHT && chr->xm >= 0)
				) || chr->ym < 0)
				bJump = true;

			int x  = chr->x;
			int y  = chr->y;
			int xm = chr->xm;
			int ym = chr->ym;
			for (int i = 0; i < 60; i++)
			{
				if (dist(x, y, npc->tgt_x, npc->tgt_y) <= 0x1000)
				{
					bLeft  = false;
					bRight = false;
					bJump  = false;

					break;
				}

				ym += chr->physics_normal.gravity1;

				if (xm < -chr->physics_normal.max_move)
					xm = -chr->physics_normal.max_move;
				if (xm > chr->physics_normal.max_move)
					xm = chr->physics_normal.max_move;
				if (ym < -chr->physics_normal.max_move)
					ym = -chr->physics_normal.max_move;
				if (ym > chr->physics_normal.max_move)
					ym = chr->physics_normal.max_move;

				x += xm;
				y += ym;
			}

			break;
		}
		// Kill the idiot who has our flag
		case 6:
		{
			MYCHAR* pOther = pTeam->mFlagNpc->focus;

			if (!pOther || !pClient->mCache.collectable_count || pTeam->mFlagNpc->bits & NPC_flags::npc_option3)
			{
				// Go back to choosing what to do
				npc->act_no = 2;
				break;
			}

			int tgt_x = pOther->x;
			int tgt_y = pOther->y;

			// Look at the target
			chr->direct = (tgt_x < chr->x ? Directions::DIRECT_LEFT : Directions::DIRECT_RIGHT);

			// Go forwards
			*(chr->direct == Directions::DIRECT_LEFT ? &bLeft : &bRight) = true;

			// We're in the air, and the target is below us
			if (tgt_y < chr->y && tgt_x > chr->x - 0x6000 && tgt_x < chr->x + 0x6000)
				bUp = true;
			else if (!(chr->flag & npc_collide_down) && tgt_y > chr->y && tgt_x > chr->x - 0x6000 && tgt_x < chr->x + 0x6000)
				bDown = true;
			
			// Determine when to shoot
			if (
				(!bUp && bDown && tgt_x >= chr->x - 0x1000 && tgt_x <= chr->x + 0x1000) ||
				(bUp && !bDown && tgt_x >= chr->x - 0x1000 && tgt_x <= chr->x + 0x1000) ||
				(!bUp && !bDown && tgt_x < chr->x && chr->direct == Directions::DIRECT_LEFT && tgt_y > chr->y - 0x1000 && tgt_y < chr->y + 0x1000) ||
				(!bUp && !bDown && tgt_x > chr->x && chr->direct == Directions::DIRECT_RIGHT && tgt_y > chr->y - 0x1000 && tgt_y < chr->y + 0x1000)
				)
				bShot = true;

			break;
		}
		// We're dead.
		case 10:
		{
			if (!pPlayer->m_Player.dead)
				npc->act_no = 1;

			break;
		}
	}

	// Switch arms if we're out of ammo
	if (pClient->mCache.armsData[pClient->mCache.currentWeapon].max_num != 0 && pClient->mCache.armsData[pClient->mCache.currentWeapon].num == 0)
		GKEYTRG |= gKeyArms;
	
	// Shoot at any idiots we come across
	CaveNet::DataStructures::NetClientIterator it;
	for (CaveNet::DataStructures::NetClient* walk = it.first(true); walk; walk = it.next())
	{
		// Do not shoot at teammates
		MYCHAR* pOther = walk->GetMyChar();

		if (!pOther || walk->GetTeam()->GetTeamId() == pTeam->GetTeamId())
			continue;

		int tgt_x = pOther->x;
		int tgt_y = pOther->y;

		if (
			pOther->life &&
			((!bUp && bDown && tgt_x >= chr->x - 0x1000 && tgt_x <= chr->x + 0x1000) ||
			(bUp && !bDown && tgt_x >= chr->x - 0x1000 && tgt_x <= chr->x + 0x1000) ||
				(!bUp && !bDown && tgt_x < chr->x && chr->direct == Directions::DIRECT_LEFT && tgt_y > chr->y - 0x1000 && tgt_y < chr->y + 0x1000) ||
				(!bUp && !bDown && tgt_x > chr->x && chr->direct == Directions::DIRECT_RIGHT && tgt_y > chr->y - 0x1000 && tgt_y < chr->y + 0x1000)) &&
			!CSM_Raycasting_CastRay_Quick(chr->x, chr->y, tgt_x, tgt_y, TileType::TT_NO_PLAYER, NULL, true, true)
			)
		{
			bShot = true;
			break;
		}
	}

#define KEY_PRESSER(MOVENAME) if (b##MOVENAME) { GKEYTRG |= gKey##MOVENAME; } else { GKEYTRG &= ~gKey##MOVENAME; GKEY &= ~gKey##MOVENAME; }

	KEY_PRESSER(Jump);
	KEY_PRESSER(Left);
	KEY_PRESSER(Right);
	KEY_PRESSER(Up);
	KEY_PRESSER(Down);
	KEY_PRESSER(Shot);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------