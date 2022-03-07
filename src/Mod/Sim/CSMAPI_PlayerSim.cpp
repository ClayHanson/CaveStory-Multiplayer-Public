#include "CavestoryModAPI.h"
#include "CSMAPI_PlayerSim.h"
#include "../../MC_Actions.h"
#include "CaveNet/CaveNet.h"
#include "Hooks.h"
#include "Main.h"
#include "Subclasses/GameDemo.h"

#include <CSMAPI_begincode.h>

using namespace CaveNet;
using namespace CaveNet::DataStructures;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

BOOL CSM_Player_Kill(SafeClientInterface* pSCI, bool bResetVelocity, PlayerDeathType iDeathType, NPCHAR* pKillerNpc, CaveNet::DataStructures::NetClient* pKillerClient, BOOL bSimulating)
{
	NetPlayer* pPlayer = (pSCI->mClient ? pSCI->mClient->GetPlayer() : NULL);

	// Clear the movement history. We don't care about what they are seeing.
	if (pPlayer)
		pPlayer->mMoveHistoryCount = 0;

	if (pSCI->mClient)
	{
		if (pSCI->mClient->IsCarrying())
			pSCI->mClient->GetCarryStackUp()->Dismount();
		if (pSCI->mClient->IsBeingCarried())
			pSCI->mClient->Dismount(false);

		if (!pSCI->mClient->mCache.is_ghost && !pPlayer->m_Player.dead)
		{
			char pClientName[512]  = { 0 };
			char pKillerName[512]  = { 'n', 'u', 'l', 'l', 0 };
			char pFlavorText[2048] = { 0 };

			// Populate client name
			if (!pSCI->mClient->GetTeam())
				strcpy(pClientName, pSCI->mClient->mCache.net_name);
			else
			{
				strcpy(pClientName, "<spush><color:");
				strcat(pClientName, pSCI->mClient->GetTeam()->GetHexColor());
				strcat(pClientName, ">");
				strcat(pClientName, pSCI->mClient->mCache.net_name);
				strcat(pClientName, "<spop>");
			}

			// Populate the killer name
			if (pKillerClient)
			{
				if (!pKillerClient->GetTeam())
					strcpy(pKillerName, pKillerClient->mCache.net_name);
				else
				{
					strcpy(pKillerName, "<spush><color:");
					strcat(pKillerName, pKillerClient->GetTeam()->GetHexColor());
					strcat(pKillerName, ">");
					strcat(pKillerName, pKillerClient->mCache.net_name);
					strcat(pKillerName, "<spop>");
				}
			}
			else if (pKillerNpc)
			{
				strcpy(pKillerName, gCaveMod.mNpcs.list[CLAMP(pKillerNpc->code_char, 0, gCaveMod.mNpcs.count - 1)].name);
			}

			// Determine the flavor text
			switch (iDeathType)
			{
				case PlayerDeathType::DEATHTYPE_UNKNOWN:
				{
					strcpy(pFlavorText, " died!");

					break;
				}
				case PlayerDeathType::DEATHTYPE_KILLED_BY_PLAYER:
				case PlayerDeathType::DEATHTYPE_KILLED_BY_NPC:
				{
					strcpy(pFlavorText, " was killed by ");
					strcat(pFlavorText, pKillerName);
					strcat(pFlavorText, "!");

					break;
				}
				case PlayerDeathType::DEATHTYPE_SUICIDE:
				{
					strcpy(pFlavorText, " bit farewell, cruel world!");
					break;
				}
				case PlayerDeathType::DEATHTYPE_OUT_OF_BOUNDS:
				{
					strcpy(pFlavorText, " fell to a clumsy death.");
					break;
				}
				case PlayerDeathType::DEATHTYPE_DROWNED:
				{
					strcpy(pFlavorText, " drowned!");
					break;
				}
			}

			CaveNet::Server::ServerClientGroup::TransmitSystemMessage("<color:FF3333>* %s%s", pClientName, pFlavorText);
		}
	}

	// Play death sound effect
	if (!bSimulating)
		PlaySoundObject2D(pSCI->mMC->x, pSCI->mMC->y, 0x2000, true, 17, 1);

	if (pPlayer)
		pPlayer->m_Player.dead = true;

	pSCI->mMC->life = 0;

	if (pSCI->mClient)
	{
		pSCI->mClient->mCache.key        = 0;
		pSCI->mClient->mCache.keyTrg     = 0;
		pSCI->mClient->mCache.key_old    = 0;
		pSCI->mClient->mCache.gameKey    = 0;
		pSCI->mClient->mCache.gameKeyTrg = 0;
		pSCI->mClient->mCache.gameKeyOld = 0;
		pSCI->mClient->mCache.gameFlags &= ~2;
		pSCI->mClient->mCache.gameFlags |= 1;
		pSCI->mClient->TransmitUpdate();

		if (!pSCI->mClient->mCache.is_ghost)
		{
			if (pPlayer)
				pPlayer->BecomeDead();

			pSCI->mClient->SpectatePlayer(pSCI->mClient->GetGhostId());

			if (pPlayer && pPlayer->m_Player.npc.xm == 0 && pPlayer->m_Player.npc.ym == 0)
			{
				pPlayer->m_Player.npc.xm = Random(-4, 4) * 0x100;
				pPlayer->m_Player.npc.ym = -0x200;
			}
		}
	}
	else if (pPlayer)
	{
		pPlayer->BecomeDead();

		if (pPlayer->m_Player.npc.xm == 0 && pPlayer->m_Player.npc.ym == 0)
		{
			pPlayer->m_Player.npc.xm = Random(-4, 4) * 0x100;
			pPlayer->m_Player.npc.ym = -0x200;
		}
	}

	if (bResetVelocity)
	{
		pSCI->mMC->xm = 0;
		pSCI->mMC->ym = 0;
	}

	// Check if all players are dead
	if (pSCI->mClient && Server::IsHosting() && !pSCI->mClient->mCache.is_ghost)
	{
		Server::TestEveryoneDied(pSCI->mClient);

		if (Hooks::gOnClientDie)
			Hooks::gOnClientDie(pSCI->mClient, iDeathType, pKillerNpc, pKillerClient);
	}

	return TRUE;
}

BOOL CSM_Player_Animate(BOOL bKey, SafeClientInterface* pSCI)
{
	if ((pSCI->mMC->cond & 2) || pSCI->mMC->netanim == PlayerStates::PLAYER_STATE_INVALID || pSCI->mMC->netanim >= PlayerStates::PLAYER_STATE_COUNT)
		return FALSE;

	// Get the output
	int iAnimateResult = gPlayerStateList[pSCI->mMC->netanim].AnimFunc(bKey, pSCI->mMC, pSCI);

	// Determine what to do with it
	if (GetPlayerStateAnimatorType(pSCI->mMC) == PlayerStateAnimator_OutputType::PSAOT_PLAYER_ANIMATION)
	{
		iAnimateResult = CLAMP(iAnimateResult, 0, PlayerAnimation::MAX_PLAYER_ANIMATIONS - 1);

		CustomPlayerAnimation* pCurrentAnimation = (pSCI->mMC->anim_info.current_anim_type < 0 || pSCI->mMC->anim_info.current_anim_type >= PlayerAnimation::MAX_PLAYER_ANIMATIONS ? NULL : &pSCI->mMC->character->mAnimList[pSCI->mMC->anim_info.current_anim_type]);
		CustomPlayerAnimation* pDesiredAnimation = pSCI->mMC->character->GetAnimation((PlayerAnimation)iAnimateResult);

		if (
			// If there is no animation playing right now, then switch
			!pCurrentAnimation ||

			// If the current animation isn't set to wait until finish, then switch
			!(pCurrentAnimation->mFlags & PlayerCustomAnimFlags::PCAF_FINISH) ||

			// If the current animation has played at least once, then switch
			pCurrentAnimation->HasPlayed(&pSCI->mMC->anim_info, 1) ||

			// If the group IDs between the two animations differ, then switch
			gPlayerAnimationInfo[pCurrentAnimation->mType].group_id != gPlayerAnimationInfo[pDesiredAnimation->mType].group_id ||

			// If the priority of the desired animation is higher than the current one, then switch
			gPlayerAnimationInfo[pCurrentAnimation->mType].priority < gPlayerAnimationInfo[pDesiredAnimation->mType].priority
			)
		{
			// Animate ourselves
			pDesiredAnimation->Tick(&pSCI->mMC->anim_info);
		}
		else
		{
			// Animate ourselves
			pCurrentAnimation->Tick(&pSCI->mMC->anim_info);
		}
	}
	else if (GetPlayerStateAnimatorType(pSCI->mMC) == PlayerStateAnimator_OutputType::PSAOT_PLAYER_FRAME)
	{
		pSCI->mMC->anim_info.curr_anim = iAnimateResult;
	}
	else
	{
		pSCI->mMC->anim_info.curr_anim = 0;
		return TRUE;
	}

	// Get the draw offset for this frame
	if (pSCI->mMC->anim_info.current_anim_type >= 0 && pSCI->mMC->anim_info.current_anim_type < PlayerAnimation::MAX_PLAYER_ANIMATIONS)
		pSCI->mMC->character->mAnimList[pSCI->mMC->anim_info.current_anim_type].GetFrameSpriteOffset(&pSCI->mMC->anim_info, &pSCI->mMC->draw_offset_x, &pSCI->mMC->draw_offset_y);

	// Fetch the rect associated with this animation
	pSCI->mMC->character->GetAnimationRect((PlayerAnimFrame)pSCI->mMC->anim_info.curr_anim, &pSCI->mMC->rect);

	// Flip it if we're facing the other way
	if (pSCI->mMC->direct != 0 && !(pSCI->mMC->character->GetFrameFlags((PlayerAnimFrame)pSCI->mMC->anim_info.curr_anim) & PlayerAnimFlags::PAF_SPECIFIC_DIRECTION))
	{
		int iHeight             = (pSCI->mMC->rect.bottom - pSCI->mMC->rect.top);
		pSCI->mMC->rect.top    += iHeight;
		pSCI->mMC->rect.bottom += iHeight;
	}

	return TRUE;
}

BOOL CSM_Player_ActCamera(BOOL bKey, SafeClientInterface* pSCI)
{
	// Camera
	bool bFocusOnCenter = (GetPlayerStateFlags(pSCI->mMC) & PLAYER_STATE_FLAGS::PSF_CENTER_CAMERA);

	if (bFocusOnCenter)
	{
		pSCI->mMC->index_x = 0;
		pSCI->mMC->index_y = 0;
	}
	else
	{
		if (pSCI->mMC->direct == 0)
		{
			pSCI->mMC->index_x -= 0x200;
			if (pSCI->mMC->index_x < -0x8000)
				pSCI->mMC->index_x = -0x8000;
		}
		else
		{
			pSCI->mMC->index_x += 0x200;
			if (pSCI->mMC->index_x > 0x8000)
				pSCI->mMC->index_x = 0x8000;
		}
		if ((*pSCI->mKey) & CaveNet::NetInputFlag::NET_INPUT_FLAG_UP && bKey)
		{
			pSCI->mMC->index_y -= 0x200;
			if (pSCI->mMC->index_y < -0x8000)
				pSCI->mMC->index_y = -0x8000;
		}
		else if ((*pSCI->mKey) & CaveNet::NetInputFlag::NET_INPUT_FLAG_DOWN && bKey)
		{
			pSCI->mMC->index_y += 0x200;
			if (pSCI->mMC->index_y > 0x8000)
				pSCI->mMC->index_y = 0x8000;
		}
		else
		{
			if (pSCI->mMC->index_y > 0x200)
				pSCI->mMC->index_y -= 0x200;
			if (pSCI->mMC->index_y < -0x200)
				pSCI->mMC->index_y += 0x200;
		}
	}

	pSCI->mMC->tgt_x = pSCI->mMC->x + pSCI->mMC->index_x;
	pSCI->mMC->tgt_y = pSCI->mMC->y + pSCI->mMC->index_y;

	return TRUE;
}

BOOL CSM_Player_AirProcess(SafeClientInterface* pSCI, BOOL bSimulating)
{
	if (pSCI->mMC->equip & 0x10)
	{
		pSCI->mMC->air     = 1000;
		pSCI->mMC->air_get = 0;
	}
	else
	{
		if (!(pSCI->mMC->flag & 0x100) || pSCI->mMC->ignore_water)
		{
			pSCI->mMC->air = 1000;
		}
		else
		{
			if (--pSCI->mMC->air <= 0)
			{
				if (GetNPCFlag(4000))
				{
					// Core cutscene
					if (bSimulating == FALSE)
					{
						TEXT_SCRIPT_PTR_DATA context;
						context.Char      = pSCI->mMC;
						context.GameFlags = pSCI->mGameFlags;
						context.Client    = pSCI->mClient;
						context.Key       = pSCI->mKey;
						context.KeyTrg    = pSCI->mKeyTrg;
						context.Player    = (pSCI->mClient ? pSCI->mClient->GetPlayer() : NULL);

						StartTextScript(1100, true, &context);
					}
				}
				else
				{
					// Drown
					CSM_Player_Kill(pSCI, true, PlayerDeathType::DEATHTYPE_DROWNED);
				}
			}
		}

		if (pSCI->mMC->flag & 0x100)
		{
			pSCI->mMC->air_get = 60;
		}
		else if (pSCI->mMC->air_get)
		{
			--pSCI->mMC->air_get;
		}
	}

	return TRUE;
}

BOOL CSM_Player_ResetFlag(SafeClientInterface* pSCI)
{
	pSCI->mMC->flag = 0;

	return TRUE;
}

BOOL CSM_Player_GetTrg(SafeClientInterface* pSCI)
{
	*pSCI->mKeyTrg = (*pSCI->mKey) & ((*pSCI->mKey) ^ (*pSCI->mKeyOld));
	*pSCI->mKeyOld = *pSCI->mKey;

	return TRUE;
}

BOOL CSM_Player_Act(BOOL bKey, SafeClientInterface* pSCI, BOOL bRecord, BOOL bSimulating)
{
	pSCI->mMC->built_xm = 0;
	pSCI->mMC->built_ym = 0;

	static int ZONE = 0;

	CSM_Player_GetTrg(pSCI);

	// Record the player's inputs ONLY if we're hosting & this is a server-sided client.
	if (bRecord)
	{
		if (pSCI->mClient && CaveNet::Server::IsHosting() && !pSCI->mClient->mCache.is_ghost && pSCI->mClient->mCache.recorder_player && (pSCI->mClient->mCache.recorder_player->state.input_flags != (*pSCI->mKey) || pSCI->mClient->mCache.recorder_player->state.input_flags_trg != (*pSCI->mKeyTrg)))
			CSM_GameDemo_Record_PlayerState(gRecordingDemo, pSCI->mClient->mCache.net_name, pSCI);
		else if (((CaveNet::Server::IsHosting() && !pSCI->mClient) || !CaveNet::Client::IsConnected()) && gRecordingDemo && (gRecordingDemo->mPlayers.list[0].state.input_flags != (*pSCI->mKey) || gRecordingDemo->mPlayers.list[0].state.input_flags_trg != (*pSCI->mKeyTrg)))
			CSM_GameDemo_Record_PlayerState(gRecordingDemo, "", pSCI);
	}

	if ((pSCI->mMC->cond & 0x80) == 0)
		return TRUE;

#ifdef USE_NET_PLAYER_ZONE_FUNCTIONALITY
	if (!mClient->mCache.is_ghost && (ZONE = gMap.ngdata[((pSCI->mMC->x / 0x2000) + (pSCI->mMC->y / 0x2000) * gMap.width) % (gMap.width * gMap.length)]) != pSCI->mMC->zone)
	{
		// Remove old zone
		if (pSCI->mMC->zone >= 0)
			gMap.zones[pSCI->mMC->zone].used--;

		// Set new zone
		pSCI->mMC->zone = ZONE;

		// Add to used count
		gMap.zones[ZONE].used++;
	}
#endif

	if (pSCI->mMC->exp_wait)
		--pSCI->mMC->exp_wait;

	if (pSCI->mMC->shock)
	{
		--pSCI->mMC->shock;
	}
	else if (pSCI->mMC->exp_count)
	{
		if (pSCI->mIsOurUser)
			SetValueView(&pSCI->mMC->x, &pSCI->mMC->y, pSCI->mMC->exp_count);

		pSCI->mMC->exp_count = 0;
	}

	if (pSCI->mMC->netanim != PlayerStates::PLAYER_STATE_INVALID && pSCI->mMC->netanim < PlayerStates::PLAYER_STATE_COUNT)
	{
		// Process air
		if (!(gPlayerStateList[pSCI->mMC->netanim].Flags & PLAYER_STATE_FLAGS::PSF_NO_AIR) && (!((*pSCI->mGameFlags) & 4) && bKey) || pSCI->mMC->npc_control)
			CSM_Player_AirProcess(pSCI, bSimulating);

		gPlayerStateList[pSCI->mMC->netanim].ActFunc(bKey, pSCI, pSCI->mMC, &((pSCI->mMC->flag & 0x100) ? pSCI->mMC->physics_underwater : pSCI->mMC->physics_normal));

		// Process the camera
		if (!(gPlayerStateList[pSCI->mMC->netanim].Flags & PLAYER_STATE_FLAGS::PSF_NO_CAMERA))
			CSM_Player_ActCamera(bKey, pSCI);
	}

	pSCI->mMC->cond &= ~0x20;

	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

#include <CSMAPI_endcode.h>