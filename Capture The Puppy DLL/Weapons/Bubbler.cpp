#include "ModInfo.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Weapon shooting
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ShootBullet_Bubblin2(ShootInfo* sData, int level)
{
	static int wait;

	if (sData->sClient.CountArmsBullet(sData->arms_code) > 15)
		return;

	// Reset rensha if not shooting
	if (!(sData->key & gKeyShot))
		sData->ourC->rensha = 6;

	// Wait until Shoot
	if (sData->key & gKeyShot)
	{
		if (++sData->ourC->rensha < 7)
			return;

		sData->ourC->rensha = 0;

		if (!sData->sClient.UseArmsEnergy(1))
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
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x600, sData->ourC->y - 0x1000, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x600, sData->ourC->y - 0x2000, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x600, sData->ourC->y - 0x1000, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x600, sData->ourC->y - 0x2000, 3, 0);
				}
			}
			else if (sData->ourC->down)
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x600, sData->ourC->y + 0x1000, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x600, sData->ourC->y + 0x2000, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x600, sData->ourC->y + 0x1000, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x600, sData->ourC->y + 0x2000, 3, 0);
				}
			}
			else
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0xC00, sData->ourC->y + 0x600, 0, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x1800, sData->ourC->y + 0x600, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0xC00, sData->ourC->y + 0x600, 2, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x1800, sData->ourC->y + 0x600, 3, 0);
				}
			}

			PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 48, SOUND_MODE_STOP_THEN_PLAY);
		}
	}
	//else if (++wait > 1)
	//{
		//wait = 0;
//		sData->sClient.ChangeArmsEnergy(1);
	//}
}

void ShootBullet_Bubblin1(ShootInfo* sData, int level)
{
	if (level > 1)
	{
		ShootBullet_Bubblin2(sData, level);
		return;
	}

	static int wait;

	if (CountArmsBullet(sData->arms_code, sData->ghostId) > 3)
		return;

	if (sData->keyTrg & GetKeybind(CSM_KEYBIND_DESC::CSM_KEYBIND_DESC_SHOOT))
	{
		if (!UseArmsEnergy(1, sData->arms, sData->selectedArm))
		{
			PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 37, SOUND_MODE_STOP_THEN_PLAY);

			if (empty == 0)
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
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x200, sData->ourC->y - 0x400, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x200, sData->ourC->y - 0x400, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x200, sData->ourC->y - 0x400, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x200, sData->ourC->y - 0x400, 3, 0);
				}
			}
			else if (sData->ourC->down)
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x200, sData->ourC->y + 0x400, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x200, sData->ourC->y + 0x400, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x200, sData->ourC->y + 0x400, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x200, sData->ourC->y + 0x400, 3, 0);
				}
			}
			else
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0xC00, sData->ourC->y + 0x600, 0, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x1800, sData->ourC->y + 0x600, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0xC00, sData->ourC->y + 0x600, 2, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x1800, sData->ourC->y + 0x600, 3, 0);
				}
			}

			PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 48, SOUND_MODE_STOP_THEN_PLAY);
		}
	}
	/*
	else if (++wait > 20)
	{
		wait = 0;
		ChargeArmsEnergy(1, sData->arms, sData->selectedArm);
	}
	*/
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Bullet Handling
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ActBullet_Bubblin1(BULLET* bul, int level)
{
	RECT rect[4];

	if (bul->flag & 0x2FF)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 2, 0);
	}
	else
	{
		switch (bul->act_no)
		{
		case 0:
			bul->act_no = 1;

			switch (bul->direct)
			{
			case 0:
				bul->xm = -0x600;
				break;
			case 2:
				bul->xm = 0x600;
				break;
			case 1:
				bul->ym = -0x600;
				break;
			case 3:
				bul->ym = 0x600;
				break;
			}

			break;
		}

		switch (bul->direct)
		{
		case 0:
			bul->xm += 42;
			break;
		case 2:
			bul->xm -= 42;
			break;
		case 1:
			bul->ym += 42;
			break;
		case 3:
			bul->ym -= 42;
			break;
		}

		bul->x += bul->xm;
		bul->y += bul->ym;

		if (++bul->act_wait > 40)
		{
			bul->cond = 0;
			SetCaret(bul->x, bul->y, 15, 0);
		}

		SET_RECT(rect[0], 192, 0, 200, 8)
		SET_RECT(rect[1], 200, 0, 208, 8)
		SET_RECT(rect[2], 208, 0, 216, 8)
		SET_RECT(rect[3], 216, 0, 224, 8)

		if (++bul->ani_wait > 3)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 3)
			bul->ani_no = 3;

		bul->rect = rect[bul->ani_no];
	}
}

void ActBullet_Bubblin2(BULLET* bul, int level)
{
	RECT rect[4];
	BOOL bDelete;

	bDelete = FALSE;

	if (bul->direct == 0 && bul->flag & 1)
		bDelete = TRUE;
	if (bul->direct == 2 && bul->flag & 4)
		bDelete = TRUE;
	if (bul->direct == 1 && bul->flag & 2)
		bDelete = TRUE;
	if (bul->direct == 3 && bul->flag & 8)
		bDelete = TRUE;

	if (bDelete)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 2, 0);
	}
	else
	{
		switch (bul->act_no)
		{
			case 0:
				bul->act_no = 1;

				switch (bul->direct)
				{
					case 0:
						bul->xm = -0x600;
						bul->ym = Random(-0x100, 0x100);
						break;
					case 2:
						bul->xm = 0x600;
						bul->ym = Random(-0x100, 0x100);
						break;
					case 1:
						bul->ym = -0x600;
						bul->xm = Random(-0x100, 0x100);
						break;
					case 3:
						bul->ym = 0x600;
						bul->xm = Random(-0x100, 0x100);
						break;
				}

				break;
		}

		switch (bul->direct)
		{
			case 0:
				bul->xm += 0x10;
				break;
			case 2:
				bul->xm -= 0x10;
				break;
			case 1:
				bul->ym += 0x10;
				break;
			case 3:
				bul->ym -= 0x10;
				break;
		}

		bul->x += bul->xm;
		bul->y += bul->ym;

		if (++bul->act_wait > 60)
		{
			bul->cond = 0;
			SetCaret(bul->x, bul->y, 15, 0);
		}

		SET_RECT(rect[0], 192, 8, 200, 16)
		SET_RECT(rect[1], 200, 8, 208, 16)
		SET_RECT(rect[2], 208, 8, 216, 16)
		SET_RECT(rect[3], 216, 8, 224, 16)

		if (++bul->ani_wait > 3)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 3)
			bul->ani_no = 3;

		bul->rect = rect[bul->ani_no];
	}
}

void ActBullet_Bubblin3(BULLET* bul, int level)
{
	CaveNet::DataStructures::NetClient* pClient = (!CaveNet_Client_IsConnected() && !CaveNet_Server_IsHosting() ? NULL : (CaveNet_Server_IsHosting() ? CaveNet_Server_ClientGroup_GetClientByGhostId : CaveNet_Client_ClientGroup_GetClientByGhostId)(bul->ghostId));
	MYCHAR* pMC                                 = (!pClient ? &gMC : pClient->GetMyChar(true));

	if (++bul->act_wait > 100 || !((pClient ? pClient->mCache.gameKey : gKey) & GetKeybind(CSM_KEYBIND_DESC::CSM_KEYBIND_DESC_SHOOT)))
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 2, 0);
		PlaySoundObject2D(bul->x, bul->y, 0x6000, true, 100, SOUND_MODE_STOP_THEN_PLAY);

		if (pMC->up)
			SetBullet(bul->code_bullet + 1, bul->x, bul->y, 1, -1, bul->level, bul->ghostId);
		else if (pMC->down)
			SetBullet(bul->code_bullet + 1, bul->x, bul->y, 3, -1, bul->level, bul->ghostId);
		else
			SetBullet(bul->code_bullet + 1, bul->x, bul->y, pMC->direct, -1, bul->level, bul->ghostId);
	}
	else
	{
		switch (bul->act_no)
		{
			case 0:
				bul->act_no = 1;

				switch (bul->direct)
				{
					case 0:
						bul->xm = Random(-0x400, -0x200);
						bul->ym = (Random(-4, 4) * 0x200) / 2;
						break;
					case 2:
						bul->xm = Random(0x200, 0x400);
						bul->ym = (Random(-4, 4) * 0x200) / 2;
						break;
					case 1:
						bul->ym = Random(-0x400, -0x200);
						bul->xm = (Random(-4, 4) * 0x200) / 2;
						break;
					case 3:
						bul->ym = Random(0x80, 0x100);
						bul->xm = (Random(-4, 4) * 0x200) / 2;
						break;
				}

				break;
		}

		if (bul->x < pMC->x)
			bul->xm += 0x20;
		if (bul->x > pMC->x)
			bul->xm -= 0x20;

		if (bul->y < pMC->y)
			bul->ym += 0x20;
		if (bul->y > pMC->y)
			bul->ym -= 0x20;

		if (bul->xm < 0 && bul->flag & 1)
			bul->xm = 0x400;
		if (bul->xm > 0 && bul->flag & 4)
			bul->xm = -0x400;

		if (bul->ym < 0 && bul->flag & 2)
			bul->ym = 0x400;
		if (bul->ym > 0 && bul->flag & 8)
			bul->ym = -0x400;

		bul->x += bul->xm;
		bul->y += bul->ym;

		RECT rect[4] =
		{
			{240, 16, 248, 24},
			{248, 16, 256, 24},
			{240, 24, 248, 32},
			{248, 24, 256, 32},
		};

		if (++bul->ani_wait > 3)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 3)
			bul->ani_no = 3;

		bul->rect = rect[bul->ani_no];
	}
}

void ActBullet_Spine(BULLET* bul, int level)
{
	if (++bul->count1 > bul->life_count || bul->flag & 8)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->act_no == 0)
		{
			bul->act_no = 1;

			switch (bul->direct)
			{
			case 0:
				bul->xm = (-Random(10, 16) * 0x200) / 2;
				break;
			case 1:
				bul->ym = (-Random(10, 16) * 0x200) / 2;
				break;
			case 2:
				bul->xm = (Random(10, 16) * 0x200) / 2;
				break;
			case 3:
				bul->ym = (Random(10, 16) * 0x200) / 2;
				break;
			}
		}
		else
		{
			bul->x += bul->xm;
			bul->y += bul->ym;
		}

		if (++bul->ani_wait > 1)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 1)
			bul->ani_no = 0;

		RECT rcLeft[2] =
		{
			{224, 0, 232, 8},
			{232, 0, 240, 8},
		};

		RECT rcRight[2] =
		{
			{224, 0, 232, 8},
			{232, 0, 240, 8},
		};

		RECT rcDown[2] =
		{
			{224, 8, 232, 16},
			{232, 8, 240, 16},
		};

		switch (bul->direct)
		{
		case 0:
			bul->rect = rcLeft[bul->ani_no];
			break;
		case 1:
			bul->rect = rcDown[bul->ani_no];
			break;
		case 2:
			bul->rect = rcRight[bul->ani_no];
			break;
		case 3:
			bul->rect = rcDown[bul->ani_no];
			break;
		}
	}
}