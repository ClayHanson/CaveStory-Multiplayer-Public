#include "ModInfo.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Weapon shooting
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ShootBullet_MachineGun(ShootInfo* sData, int level)
{
	if (CountArmsBullet(sData->arms_code, sData->ghostId) > 4)
		return;

	if (!(sData->key & GetKeybind(CSM_KEYBIND_DESC::CSM_KEYBIND_DESC_SHOOT)))
		sData->ourC->rensha = 6;

	if (sData->key & GetKeybind(CSM_KEYBIND_DESC::CSM_KEYBIND_DESC_SHOOT))
	{
		if (++sData->ourC->rensha < 6)
			return;

		sData->ourC->rensha = 0;

		if (!UseArmsEnergy(1, sData->arms, sData->selectedArm))
		{
			PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 37, SOUND_MODE_STOP_THEN_PLAY);

			if (empty == 0 && sData->sClient.mIsOurUser)
			{
				SetCaret(sData->ourC->x, sData->ourC->y, 16, 0);
				empty = 50;
			}

			return;
		}
		else
		{
			if (sData->ourC->up)
			{
				if (level == 3)
					sData->ourC->ym += 0x100;

				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x600, sData->ourC->y - 0x1000, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x600, sData->ourC->y - 0x1000, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x600, sData->ourC->y - 0x1000, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x600, sData->ourC->y - 0x1000, 3, 0);
				}
			}
			else if (sData->ourC->down)
			{
				if (level == 3)
				{
					if (sData->client && sData->client->mCache.carried_by_ghost_id != -1)
					{
						//if (CaveNet::Server::Config::AllowMachineGunFly.ToBool())
						if (Config_GetBool("AllowMachineGunFly"))
						{
							// Get the most down in the chain
							CaveNet::DataStructures::NetClient* bottom_client;
							bottom_client = sData->client;

							// Find the bottom of the link
							while (bottom_client && bottom_client->mCache.carried_by_ghost_id != -1)
								bottom_client = bottom_client->GetCarryStackDown();

							CaveNet::DataStructures::NetPlayer* pPlayer = (bottom_client ? bottom_client->GetPlayer() : NULL);

							if (pPlayer)
							{
								if (pPlayer->m_Player.npc.ym > 0)
									pPlayer->m_Player.npc.ym /= 2;

								if (pPlayer->m_Player.npc.ym > -0x400)
								{
									pPlayer->m_Player.npc.ym -= 0x200;
									if (pPlayer->m_Player.npc.ym < -0x400)
										pPlayer->m_Player.npc.ym = -0x400;
								}
							}
						}
					}
					else
					{
						if (sData->ourC->ym > 0)
							sData->ourC->ym /= 2;

						if (sData->ourC->ym > -0x400)
						{
							sData->ourC->ym -= 0x200;
							if (sData->ourC->ym < -0x400)
								sData->ourC->ym = -0x400;
						}
					}
				}

				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x600, sData->ourC->y + 0x1000, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x600, sData->ourC->y + 0x1000, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x600, sData->ourC->y + 0x1000, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x600, sData->ourC->y + 0x1000, 3, 0);
				}
			}
			else
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x1800, sData->ourC->y + 0x600, 0, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x1800, sData->ourC->y + 0x600, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x1800, sData->ourC->y + 0x600, 2, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x1800, sData->ourC->y + 0x600, 3, 0);
				}
			}

			if (level == 3)
				PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 49, SOUND_MODE_STOP_THEN_PLAY);
			else
				PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 32, SOUND_MODE_STOP_THEN_PLAY);
		}
	}
	/*
	else
	{
		static int wait = 0;

		++wait;
		if (sData->ourC->equip & 8)
		{
			if (wait > 1)
			{
				wait = 0;
				ChargeArmsEnergy(1, sData->arms, sData->selectedArm);
			}
		}
		else
		{
			if (wait > 4)
			{
				wait = 0;
				ChargeArmsEnergy(1, sData->arms, sData->selectedArm);
			}
		}
	}
	*/
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Bullet Handling
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ActBullet_MachineGun(BULLET* bul, int level)
{
	int move = 0;

	RECT rect1[4] =
	{
		{64, 0, 80, 16},
		{80, 0, 96, 16},
		{96, 0, 112, 16},
		{112, 0, 128, 16},
	};

	RECT rect2[4] =
	{
		{64, 16, 80, 32},
		{80, 16, 96, 32},
		{96, 16, 112, 32},
		{112, 16, 128, 32},
	};

	RECT rect3[4] =
	{
		{64, 32, 80, 48},
		{80, 32, 96, 48},
		{96, 32, 112, 48},
		{112, 32, 128, 48},
	};

	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->act_no == 0)
		{
			switch (level)
			{
			case 1:
				move = 0x1000;
				break;
			case 2:
				move = 0x1000;
				break;
			case 3:
				move = 0x1000;
				break;
			}

			bul->act_no = 1;

			switch (bul->direct)
			{
			case 0:
				bul->xm = -move;
				bul->ym = Random(-0xAA, 0xAA);
				break;
			case 1:
				bul->ym = -move;
				bul->xm = Random(-0xAA, 0xAA);
				break;
			case 2:
				bul->xm = move;
				bul->ym = Random(-0xAA, 0xAA);
				break;
			case 3:
				bul->ym = move;
				bul->xm = Random(-0xAA, 0xAA);
				break;
			}
		}
		else
		{
			bul->x += bul->xm;
			bul->y += bul->ym;

			switch (level)
			{
			case 1:
				bul->rect = rect1[bul->direct];
				break;
			case 2:
				bul->rect = rect2[bul->direct];

				if (bul->direct == 1 || bul->direct == 3)
					SetNpChar(127, bul->x, bul->y, 0, 0, 1, 0, 256);
				else
					SetNpChar(127, bul->x, bul->y, 0, 0, 0, 0, 256);

				break;
			case 3:
				bul->rect = rect3[bul->direct];
				SetNpChar(128, bul->x, bul->y, 0, 0, bul->direct, 0, 256);
				break;
			}
		}
	}
}