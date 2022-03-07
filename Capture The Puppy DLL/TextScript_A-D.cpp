#define _TEXT_SCRIPT_FILE_

#include <CavestoryModAPI.h>
#include <Subclasses/ModTextScriptCommand.h>

#include <CSMAPI_begincode.h>
#include <Subclasses\GameDemo.h>

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DEFINE_TEXTSCRIPT_COMMAND(P,U,N, "PUppy None", "Unset flag 274 if no players are carrying a puppy", "")
{
	// PUppy absence Jump -- Jump to event X if nobody on the server is carrying a puppy.
	gTS.p_read += 4;

	if (CaveNet_Server_IsHosting() || CaveNet_Client_IsConnected())
	{
		CaveNet::DataStructures::NetClientIterator it;

		// Stop if we reach a client with a puppy!
		for (CaveNet::DataStructures::NetClient* pClient = it.first(CaveNet_Server_IsHosting()); pClient; pClient = it.next())
			if (pClient->mCache.collectable_count >= 3000 && pClient->mCache.collectable_count <= 3004)
				return;
	}
	else if (gCollectableCount >= 3000 && gCollectableCount <= 3004)
		return;

	DeleteNpCharEvent(401);
	CutNPCFlag(274, true);
}

DEFINE_TEXTSCRIPT_COMMAND(P,U,P, "rescue PUPpy", "Rescue a puppy!", "")
{
	// Increment read position
	gTS.p_read += 4;

	// Ensure this is actually valid to be calling in the current context
	if (!CaveNet_Server_IsHosting() || !TGetClient || !TGetPlayer || TGetClient->mCache.collectable_count > 0)
		return;

	MYCHAR* pChar = TGetClient->GetMyChar();
	int iDogNpc = -1;

	// Get the puppy object from the one who executed this event
	for (iDogNpc = 0; iDogNpc < NPC_MAX; iDogNpc++)
	{
		// Skip invalid NPCs
		if (!(gNPC[iDogNpc].cond & 0x80) || (gNPC[iDogNpc].code_char != 126 && gNPC[iDogNpc].code_char != 130 && gNPC[iDogNpc].code_char != 131 && gNPC[iDogNpc].code_char != 132) || !TGetPlayer->JudgeHitMyCharNPC3(&gNPC[iDogNpc]))
			continue;

		// Found it!
		break;
	}

	// Couldn't find it...
	if (iDogNpc == NPC_MAX)
		return;

	const char* pDogNames[] = { "Hajime", "Mick", "Shinobu", "Kakeru", "Nene" };
	NPCHAR* pDog = &gNPC[iDogNpc];

	// Announce the player's hero status
	CaveNet_Server_ClientGroup_TransmitSystemMessage("<color:00FF00>* %s rescued %s!", TGetClient->mCache.net_name, pDogNames[pDog->code_flag - 3000]);

	// Set appropriate variables
	TGetClient->mCache.collectables_shown = true;
	TGetClient->mCache.collectable_count = pDog->code_flag;

	// Transmit collectables
	TGetClient->TransmitInventory(CaveNet::UpdateInventoryFlags::UPDATE_INV_COLLECTABLES);

	// Add the dog item
	TGetClient->AddItemData(14);

	// Remove the dog
	pDog->cond = 0;
}

DEFINE_TEXTSCRIPT_COMMAND(T,R,S, "TRansport Stay", "Change the map, but keep everyone at the same position.", "me")
{
	z = GetTextScriptNo(gTS.p_read + 4);
	w = GetTextScriptNo(gTS.p_read + 9);

	// Transmit map to everyone & set their position, as well
	if (CaveNet_Server_IsHosting() && !CaveNet_Client_IsInGui())
	{
		CaveNet::DataStructures::NetClientIterator it;

		if (Config_GetBool("MapChangeRevive"))
			CaveNet_Server_ClientGroup_ReviveAllDeadPlayers();

		// Reset zones & stop all animations
		for (CaveNet::DataStructures::NetClient* pClient = it.first(true); pClient; pClient = it.next())
		{
			CaveNet::DataStructures::NetPlayer* pPlayer = pClient->GetPlayer();
			pClient->ClearZones();

			// Handle special death option
			if (!Config_GetBool("MapChangeRevive") && pPlayer->m_Player.dead)
				pClient->CacheDeath();

			// Delete any animation we have
			if (!(GetPlayerStateFlags(&pPlayer->m_Player.npc) & PLAYER_STATE_FLAGS::PSF_UNIT))
				pClient->GetPlayer()->EndAnimation();
		}

		if (!TransferStage2(z, w))
		{
			CSM_Explode(1, "FAILED_TO_TRANSFER_STAGE", "\\a{0} crashed!", "Failed to load stage.", true);
			iError = 0;
			return;
		}

		if (gRecordingDemo)
		{
			SafeClientInterface pInterface;
			pInterface.Set(NULL);
			CSM_GameDemo_Record_PlayerState(gRecordingDemo, "", &pInterface);
		}

		for (CaveNet::DataStructures::NetClient* pClient = it.first(true); pClient; pClient = it.next())
		{
			CaveNet::DataStructures::NetPlayer* pPlayer = pClient->GetPlayer();

			if (!pPlayer)
			{
				pClient->SendMap(w);

				// Clear spectation if the client is automatically revived
				if (Config_GetBool("MapChangeRevive") && pClient->mCache.is_spectating)
					pClient->ClearSpectate();

				continue;
			}

			pClient->OnMapLoad();
			pClient->SendMap(w);

			// Clear spectation if the client is automatically revived
			if (Config_GetBool("MapChangeRevive") && pClient->mCache.is_spectating)
				pClient->ClearSpectate();

			if (gRecordingDemo)
			{
				SafeClientInterface pInterface;
				pInterface.Set(pClient);
				CSM_GameDemo_Record_PlayerState(gRecordingDemo, pClient->mCache.net_name, &pInterface);
			}
		}
	}
	else if (!CaveNet_Client_IsConnected())
	{
		if (!TransferStage2(z, w))
		{
			CSM_Explode(1, "FAILED_TO_TRANSFER_STAGE", "\\a{0} crashed!", "Failed to load stage.", true);
			iError = 0;
			return;
		}

		if (gRecordingDemo)
		{
			SafeClientInterface pInterface;
			pInterface.Set(NULL);
			CSM_GameDemo_Record_PlayerState(gRecordingDemo, "", &pInterface);
		}
	}
	else
	{
		bExit = true;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_endcode.h>