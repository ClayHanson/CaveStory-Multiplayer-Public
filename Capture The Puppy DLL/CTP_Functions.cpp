#include "Weapons/ModInfo.h"

#define SubpixelToPixel(A) ((A) / 0x200)

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CTP_PickupWeapon(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc)
{
	// Ensure this is actually valid to be calling in the current context
	if ((CaveNet_Client_IsConnected() && !CaveNet_Server_IsHosting()) || !pMC->life)
		return;

	// Stop here if we have no valid client object.
	if (CaveNet_Server_IsHosting() && !pClient)
		return;

	// Stop if this team cannot pickup weapons.
	if (pClient && pClient->GetTeam() && (CTP_GetTeamInfo(pClient->GetTeam()->mTeamType)->flags & CTP_TeamFlags::CTP_FLAG_CANNOT_PICKUP_WEAPONS))
		return;

	// Pick a weapon at random
	const PWP_WEAPON* pInfo = &pWeaponCodeTable[pNpc->code_flag + 1];

	// Check to see if the player even has it.
	if (!CaveNet_Client_IsConnected())
	{
		SafeClientInterface pSCI;
		pSCI.Set(NULL);

		if (!HasArmsData(pInfo->weapon_id))
			AddArmsData2(pInfo->weapon_id, pInfo->max_ammo, pInfo->max_ammo, Random(pInfo->min_rand_level, pInfo->max_rand_level));
		else if (!pSCI.HasArmsEnergy(pInfo->weapon_id, pInfo->max_ammo))
			AddArmsData2(pInfo->weapon_id, pInfo->add_ammo, pInfo->max_ammo, 0);
		else
			return;
	}
	else
	{
		if (!pClient->HasArmsData(pInfo->weapon_id))
			pClient->AddArmsData2(pInfo->weapon_id, pInfo->max_ammo, pInfo->max_ammo, Random(pInfo->min_rand_level, pInfo->max_rand_level));
		else if (!pClient->GetInterface()->HasArmsEnergy(pInfo->weapon_id, pInfo->max_ammo))
			pClient->AddArmsData2(pInfo->weapon_id, pInfo->add_ammo, pInfo->max_ammo, 0);
		else
			return;
	}

	// Despawn the NPC
	pNpc->bits    &= ~NPC_flags::npc_option3;
	pNpc->ani_no   = 0;
	pNpc->act_no   = 1;
	pNpc->act_wait = 0;
}

void CTP_PickupPuppy(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc)
{
	CaveNet::DataStructures::NetTeam* pTeam = (pClient ? pClient->GetTeam() : NULL);

	int* pCollectableCount   = (CaveNet_Client_IsConnected() && pClient ? &pClient->mCache.collectable_count : &gCollectableCount);
	bool* pCollectablesShown = (CaveNet_Client_IsConnected() && pClient ? &pClient->mCache.collectables_shown : &gCollectablesShown);

	// Ignoring this client
	if (!pMC->life || pNpc->client2 && (!CaveNet_Client_IsConnected() || pNpc->client2->GetGhostId() == pClient->GetGhostId()))
		return;

	// Stop if this team cannot pickup flags.
	if (pClient && pClient->GetTeam() && (CTP_GetTeamInfo(pClient->GetTeam()->mTeamType)->flags & CTP_TeamFlags::CTP_FLAG_CANNOT_PICKUP_FLAG))
		return;

	if ((!pTeam && (*pCollectableCount) > 0) || (pTeam && pTeam->GetNpcSpawnIndex() == pNpc->code_flag))
	{
		// We're on the same team as the dog!
		if (*pCollectableCount)
		{
			// We're delivering a dog!
			NPCHAR* pCarryDog = &gNPC[*pCollectableCount];
			pCarryDog->act_no = 5;
			pCarryDog->x      = pCarryDog->pNpc->x;
			pCarryDog->y      = pCarryDog->pNpc->y - 0x2400;
			pCarryDog->focus  = NULL;
			pCarryDog->client = NULL;
			pCarryDog->bits  |= NPC_flags::npc_ignoreSolid | NPC_flags::npc_option3;
			pCarryDog->ani_no = 0;
			pCarryDog->count2 = 0;
			pCarryDog->direct = (pCarryDog->bits & NPC_flags::npc_altDir ? Directions::DIRECT_RIGHT : Directions::DIRECT_LEFT);

			// Set the physics of this player
			CTP_RevokeCarryPuppyPhysics(pClient, pMC);

			// Set collectable count
			(*pCollectableCount)  = 0;
			(*pCollectablesShown) = true;

			// Transmit collectables
			if (pClient && !pClient->mCache.is_ghost)
				pClient->TransmitInventory(CaveNet::UpdateInventoryFlags::UPDATE_INV_COLLECTABLES);

			if (pTeam)
			{
				++pTeam->mConfig.score;
				pTeam->UpdateTeam();
			}

			if (pClient && !pClient->mCache.is_ghost)
				CaveNet_Server_ClientGroup_TransmitSystemMessage("<color:00FF00>* <spush><color:%s>%s<spop> captured <spush><color:%s>%s<spop> team's puppy!", pClient->GetTeam()->GetHexColor(), pClient->mCache.net_name, CaveNet::DataStructures::NetTeam::FindTeamByType((CaveNet::DataStructures::NetTeam::TeamType)pCarryDog->code_flag, false)->GetHexColor(), CaveNet::DataStructures::NetTeam::FindTeamByType((CaveNet::DataStructures::NetTeam::TeamType)pCarryDog->code_flag, false)->GetName());
		}
	}
	else if (!(*pCollectableCount))
	{
		// We're stealing the dog!
		if (pClient && !pClient->mCache.is_ghost)
			CaveNet_Server_ClientGroup_TransmitSystemMessage("<color:00FF00>* <spush><color:%s>%s<spop> picked up the <spush><color:%s>%s<spop> puppy!", pClient->GetTeam()->GetHexColor(), pClient->mCache.net_name, CaveNet::DataStructures::NetTeam::FindTeamByType((CaveNet::DataStructures::NetTeam::TeamType)pNpc->code_flag, false)->GetHexColor(), CaveNet::DataStructures::NetTeam::FindTeamByType((CaveNet::DataStructures::NetTeam::TeamType)pNpc->code_flag, false)->GetName());

		pNpc->act_no = 4;
		pNpc->client = pClient;
		pNpc->focus  = pMC;
		pNpc->bits  &= ~NPC_flags::npc_option3;
		pNpc->bits  |= NPC_flags::npc_ignoreSolid;
		pNpc->ani_no = 0;
		pNpc->count2 = 0;

		// Set the physics of this player
		CTP_ApplyCarryPuppyPhysics(pClient, pMC);

		// Set collectable count
		(*pCollectableCount)  = pNpc - gNPC;
		(*pCollectablesShown) = true;

		// Transmit collectables
		if (pClient && !pClient->mCache.is_ghost)
			pClient->TransmitInventory(CaveNet::UpdateInventoryFlags::UPDATE_INV_COLLECTABLES);
	}
}

void CTP_HandleNoPuppyZone(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc)
{
	if ((!pClient && !gCollectableCount) || (pClient && !pClient->mCache.collectable_count))
		return;

	NPCHAR* pDog = &gNPC[(!pClient ? gCollectableCount : pClient->mCache.collectable_count)];

	// Drop the dog
	CTP_DropPuppy(pDog);

	// Set position
	if (pNpc->tgt_x)
		pDog->x = pNpc->tgt_x;
	if (pNpc->tgt_y)
		pDog->y = pNpc->tgt_y;
}

void CTP_HandleKillZone(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc)
{
	// Do not kill players who are already dead.
	if (!pMC->life)
		return;

	// This doubles as a no-puppy-zone. Drop the puppy.
	if ((!pClient && gCollectableCount) || (pClient && pClient->mCache.collectable_count))
	{
		NPCHAR* pDog = &gNPC[(!pClient ? gCollectableCount : pClient->mCache.collectable_count)];

		// Drop the dog
		CTP_DropPuppy(pDog);

		// Set position
		pDog->x  = pMC->x;
		pDog->y  = pMC->y;
		pDog->xm = pMC->xm;
		pDog->ym = pMC->ym;
	}

	if (pPlayer)
		pPlayer->Kill(false, PlayerDeathType::DEATHTYPE_OUT_OF_BOUNDS, NULL, NULL);
	else
	{
		SafeClientInterface pSCI;
		pSCI.Set(NULL);
		pSCI.DamagePlayer(9999);
	}
}

void CTP_HandleResupplyCabinet(CaveNet::DataStructures::NetClient* pClient, CaveNet::DataStructures::NetPlayer* pPlayer, MYCHAR* pMC, NPCHAR* pNpc)
{
	if (!pMC->life || (pClient && (!pClient->GetTeam() || pClient->GetTeam()->mTeamType != pNpc->code_flag)))
		return;

	if (pClient)
	{
		// If we're on cooldown, then stop here.
		if (CaveNet_Server_GetGlobalTimerOffset() - pClient->mCache.last_supply_time < 3000)
			return;

		pClient->mCache.last_supply_time = CaveNet_Server_GetGlobalTimerOffset();
	}

	bool bUpdatedLife = false;
	bool bUpdatedGun  = false;

	// Set life to maximum
	if (pMC->life != pMC->max_life)
	{
		bUpdatedLife = true;
		pMC->life    = pMC->max_life;
	}

	SafeClientInterface pSCI;
	if (pSCI.Set(pClient))
	{
		for (int i = 0; i < ARMS_MAX; i++)
		{
			// Stop at the first invalid weapon
			if (!pSCI.mArms[i].code)
				break;

			// Don't do anything if the gun already has maximum 
			if (pSCI.mArms[i].num == pSCI.mArms[i].max_num)
				continue;

			// Refill ammo
			pSCI.mArms[i].num = pSCI.mArms[i].max_num;
			bUpdatedGun       = true;
		}
	}

	// Set the NPC to open
	if (!pNpc->count3)
		pNpc->count3 = 1;

	// Send the update
	if (pClient && !pClient->mCache.is_ghost)
	{
		if (bUpdatedLife)
			pClient->TransmitPlayerUpdate();

		if (bUpdatedGun)
			pClient->TransmitInventory(CaveNet::UpdateInventoryFlags::UPDATE_INV_WEAPONS);
	}
}

void CTP_DropPuppy(NPCHAR* pNpc)
{
	if (CaveNet_Server_IsHosting())
	{
		if (pNpc->client)
			CaveNet_Server_ClientGroup_TransmitSystemMessage("<color:AAAA00>* <spush><color:%s>%s<spop> dropped <spush><color:%s>%s<spop> team's puppy!", pNpc->client->GetTeam()->GetHexColor(), pNpc->client->mCache.net_name, CaveNet::DataStructures::NetTeam::FindTeamByType((CaveNet::DataStructures::NetTeam::TeamType)pNpc->code_flag, false)->GetHexColor(), CaveNet::DataStructures::NetTeam::FindTeamByType((CaveNet::DataStructures::NetTeam::TeamType)pNpc->code_flag, false)->GetName());
		else
			CaveNet_Server_ClientGroup_TransmitSystemMessage("<color:AAAA00>* <spush><color:%s>%s<spop> team's puppy was dropped!", CaveNet::DataStructures::NetTeam::FindTeamByType((CaveNet::DataStructures::NetTeam::TeamType)pNpc->code_flag, false)->GetHexColor(), CaveNet::DataStructures::NetTeam::FindTeamByType((CaveNet::DataStructures::NetTeam::TeamType)pNpc->code_flag, false)->GetName());
	}

	if (pNpc->focus)
		CTP_RevokeCarryPuppyPhysics(pNpc->client, pNpc->focus);

	if (pNpc->client)
	{
		pNpc->client->mCache.collectable_count  = 0;
		pNpc->client->mCache.collectables_shown = false;

		if (pNpc->client && !pNpc->client->mCache.is_ghost)
			pNpc->client->TransmitInventory(CaveNet::UpdateInventoryFlags::UPDATE_INV_COLLECTABLES);
	}
	else if (!CaveNet_Client_IsConnected())
	{
		gCollectableCount  = 0;
		gCollectablesShown = false;
	}

	pNpc->client2 = (!CaveNet_Client_IsConnected() ? (CaveNet::DataStructures::NetClient*)1 : pNpc->client);
	pNpc->focus2  = NULL;
	pNpc->focus   = NULL;
	pNpc->client  = NULL;
	pNpc->act_no  = 3;
	pNpc->ani_no  = 4;
	pNpc->bits   |= NPC_flags::npc_option3;
	pNpc->bits   &= ~NPC_flags::npc_ignoreSolid;
}

void CTP_ApplyCarryPuppyPhysics(CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC)
{
	// Apply physics
	pMC->physics_normal.max_dash     = 0x2E2;
	pMC->physics_underwater.max_dash = 0x171;

	// Transmit the physics
	if (pClient && !pClient->mCache.is_ghost)
		pClient->TransmitPlayerPhysics();
}

void CTP_RevokeCarryPuppyPhysics(CaveNet::DataStructures::NetClient* pClient, MYCHAR* pMC)
{
	// Apply physics
	pMC->physics_normal.max_dash     = 0x32C;
	pMC->physics_underwater.max_dash = 0x196;

	// Transmit the physics
	if (pClient && !pClient->mCache.is_ghost)
		pClient->TransmitPlayerPhysics();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define SHIMMER_MAX_DISTANCE 0x2000

void CTP_DrawZoneShimmer()
{
	MYCHAR* pCloseList[128];
	unsigned int iCloseCount = 0;
	CaveNet::DataStructures::NetClientIterator it;
	MYCHAR* pMC = NULL;

	unsigned int ticks = (GetTicks() % 1000);
	short cola         = (ticks < 500 ? (255 - (50 * ticks / 500)) : (205 + (50 * (ticks - 500) / 500)));

	// We're drawing to the screen
	CacheSurface::SetSurfaceID(SURFACE_ID_RENDERER);

	// Loop through all tiles
	for (MAP_ANIMATED_TILE* pTile = gMap.anim_draw; pTile; pTile = pTile->draw_next)
	{
		if (pTile->surf_id != SURFACE_ID_TEAM_SPAWN_DOORS)
			continue;

		int tile_x         = (pTile->tile_offset % gMap.width);
		int tile_y         = (pTile->tile_offset / gMap.width);
		int sub_x          = tile_x * 0x2000;
		int sub_y          = tile_y * 0x2000;
		unsigned int attrb = GetTileFlagsXY(tile_x, tile_y);
		int sub_l          = (GetTileFlagsXY(tile_x - 1, tile_y) == attrb ? 0x3000 : 0x1000);
		int sub_t          = (GetTileFlagsXY(tile_x, tile_y - 1) == attrb ? 0x3000 : 0x1000);
		int sub_r          = (GetTileFlagsXY(tile_x + 1, tile_y) == attrb ? 0x3000 : 0x1000);
		int sub_b          = (GetTileFlagsXY(tile_x, tile_y + 1) == attrb ? 0x3000 : 0x1000);
		CaveNet::DataStructures::NetTeam::TeamType iTeamType = CaveNet::DataStructures::NetTeam::TeamType::TEAM_YELLOW;
		GUI_COLOR color(255, 255, 0, 255);

		if (attrb & TileType::TT_GREEN_TEAM)		{ color = GUI_COLOR(0, 255, 0, 255); iTeamType = CaveNet::DataStructures::NetTeam::TeamType::TEAM_GREEN; }
		else if (attrb & TileType::TT_RED_TEAM)		{ color = GUI_COLOR(255, 0, 0, 255); iTeamType = CaveNet::DataStructures::NetTeam::TeamType::TEAM_RED; }
		else if (attrb & TileType::TT_BLUE_TEAM)	{ color = GUI_COLOR(0, 0, 255, 255); iTeamType = CaveNet::DataStructures::NetTeam::TeamType::TEAM_BLUE; }

		bool bIsOpen       = false;
		float fClosestDist = -1.f;

		// Loop through all clients to find a candidate
		for (CaveNet::DataStructures::NetClient* pClient = it.first(CaveNet_Server_IsHosting()); pClient; pClient = it.next())
		{
			// Skip clients that have no player
			if ((pMC = pClient->GetMyChar()) == NULL || !pMC->life)
				continue;

			int iTileSubX = CLAMP(pMC->x, sub_x - sub_l, sub_x + sub_r);
			int iTileSubY = CLAMP(pMC->y, sub_y - sub_t, sub_y + sub_b);
			int iPlayerX  = CLAMP(iTileSubX, pMC->x - pMC->view.left, pMC->x + pMC->view.right);
			int iPlayerY  = CLAMP(iTileSubY, pMC->y - pMC->view.top, pMC->y + pMC->view.bottom);
			iTileSubX     = CLAMP(iPlayerX, sub_x - sub_l, sub_x + sub_r);
			iTileSubY     = CLAMP(iPlayerY, sub_y - sub_t, sub_y + sub_b);

			if (
				iPlayerY > sub_y - sub_t && iPlayerY < sub_y + sub_b &&
				iPlayerX > sub_x - sub_l && iPlayerX < sub_x + sub_r
				)
			{
				if (iPlayerX < sub_x)
					iPlayerX = sub_x - sub_l - 0x200;
				else if (iPlayerX > sub_x)
					iPlayerX = sub_x + sub_r + 0x200;
				else if (iPlayerY < sub_y)
					iPlayerY = sub_y - sub_t - 0x200;
				else
					iPlayerY = sub_y + sub_b + 0x200;
			}

			float fOurDist = dist(iPlayerX, iPlayerY, iTileSubX, iTileSubY);

			// If we are apart of the team that owns this tile, then "open up" if we can
			if (!bIsOpen && pClient->GetTeam() && pClient->GetTeam()->mTeamType == iTeamType && fOurDist < 0x4000)
				bIsOpen = true;

			// We are not close enough to it, do not do anything
			if (fOurDist >= SHIMMER_MAX_DISTANCE)
				continue;

			pCloseList[iCloseCount++] = pMC;

			// Record the distance if it's a record
			if (fClosestDist == -1.f || fOurDist < fClosestDist)
				fClosestDist = fOurDist;
		}

		if (bIsOpen && pTile->frame != 3)
		{
			if (++pTile->timer >= 8)
			{
				pTile->timer = 0;
				++pTile->frame;
			}
		}
		else if (!bIsOpen && pTile->frame != 0)
		{
			if (++pTile->timer >= 8)
			{
				pTile->timer = 0;
				--pTile->frame;
			}
		}

		if (fClosestDist != -1.f)
		{
			GUI_RECT pShimmerRc = GUI_RECT::FromRect(
				pTile->rect_list[pTile->frame].left % 64,
				112 + pTile->rect_list[pTile->frame].top,
				pTile->rect_list[pTile->frame].right % 64,
				112 + pTile->rect_list[pTile->frame].top + 16
			);

			// Calculate the position to draw this thing at.
			GUI_POINT pDrawPoint(SubpixelToPixel(sub_x - 0x1000) - SubpixelToPixel(gFrame.x), SubpixelToPixel(sub_y - 0x1000) - SubpixelToPixel(gFrame.y));

			// Draw the bitmap
			for (int i = 0; i < 256; i++) // Draw 16x16 tile
			{
				unsigned int pixel_x    = (i % 16);
				unsigned int pixel_y    = (i / 16);
				unsigned int subpixel_x = (sub_x - 0x1000) + (pixel_x * 0x200);
				unsigned int subpixel_y = (sub_y - 0x1000) + (pixel_y * 0x200);

				// Get distance
				float fClosestDistance = -1.f;

				// Calculate the sum of all of this stuff
				for (int j = 0; j < iCloseCount; j++)
				{
					float fOurScore = dist(
						subpixel_x,
						subpixel_y,
						CLAMP(subpixel_x, pCloseList[j]->x - pCloseList[j]->view.left, pCloseList[j]->x + pCloseList[j]->view.right),
						CLAMP(subpixel_y, pCloseList[j]->y - pCloseList[j]->view.top, pCloseList[j]->y + pCloseList[j]->view.bottom)
					);

					if (fClosestDistance == -1.f || fOurScore < fClosestDistance)
						fClosestDistance = fOurScore;
				}

				int color_alpha = (fClosestDistance <= 0x200 ? cola : (-cola * fClosestDistance / SHIMMER_MAX_DISTANCE));
				if (color_alpha < -255)
					continue;

				ModBitmap(SURFACE_ID_TEAM_SPAWN_DOORS, CSM_RGBA(255, 255, 255, (unsigned char)color_alpha));
				PutBitmap10(PixelToScreenCoord(pDrawPoint.x + pixel_x), PixelToScreenCoord(pDrawPoint.y + pixel_y), &GUI_RECT(pShimmerRc.x + pixel_x, pShimmerRc.y + pixel_y, 1, 1).to_rect(), SURFACE_ID_TEAM_SPAWN_DOORS);
			}

			ClearBitmapMod(SURFACE_ID_TEAM_SPAWN_DOORS);

			//unsigned int iAlpha = CLAMP((int)255 - int(255.f * float(fClosestDist - 0x1000) / float(SHIMMER_MAX_DISTANCE - 0x1000)), 0, 255);
			//ModBitmap(SURFACE_ID_TEAM_SPAWN_DOORS, CSM_RGBA(255, 255, 255, (unsigned char)iAlpha));
			//PutBitmap10(SubpixelToScreenCoord(sub_x - 0x1000) - SubpixelToScreenCoord(gFrame.x), SubpixelToScreenCoord(sub_y - 0x1000) - SubpixelToScreenCoord(gFrame.y), &pShimmerRc.to_rect(), SURFACE_ID_TEAM_SPAWN_DOORS);
			//ClearBitmapMod(SURFACE_ID_TEAM_SPAWN_DOORS);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------