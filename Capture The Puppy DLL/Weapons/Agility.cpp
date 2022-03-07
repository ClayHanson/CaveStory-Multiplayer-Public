#include "ModInfo.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Weapon shooting
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ShootBullet_Agility(ShootInfo* sData, int level)
{
	// Wait until we shoot
	if (!(sData->keyTrg & gKeyShot) || (GetPlayerStateFlags(sData->ourC) & PLAYER_STATE_FLAGS::PSF_PUT_BUBBLE) || !IsAgilityAllowed())
		return;

	// If we aren't connected, then just start tackling immediately
	if (!CaveNet_Client_IsConnected())
	{
		NPCHAR* out = NULL;

		// Start tackling
		SafeClientInterface pSCI;
		pSCI.Set(NULL);
		SetPlayerState(&pSCI, FindPlayerStateByName("PLAYER_STATE_TACKLE_CHARGE"));

		/*
		if (SetAnimNpChar(383, sData->ourC->xm, sData->ourC->ym, sData->ourC->direct, NULL, ANIM_NPC_START_OFFSET, sData->ourC, sData->ourC->character, &out))
		{
			out->client = sData->client;
			out->focus  = sData->ourC;
			out->count3 = level;
		}
		*/
	}
	else if (sData->client && !sData->client->mCache.is_ghost)
	{
		// Otherwise, if this is a server-side client, have them tackle
		sData->client->GetPlayer()->Tackle(level);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Bullet Handling
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
