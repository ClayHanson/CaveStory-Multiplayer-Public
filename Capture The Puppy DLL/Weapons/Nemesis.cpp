#include "ModInfo.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Weapon shooting
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ShootBullet_Nemesis(ShootInfo* sData, int level)
{
	if (CountArmsBullet(sData->arms_code, sData->ghostId) > 1)
		return;

	if (sData->keyTrg & GetKeybind(CSM_KEYBIND_DESC::CSM_KEYBIND_DESC_SHOOT))
	{
		if (!UseArmsEnergy(1, sData->arms, sData->selectedArm))
		{
			// Out of ammo!
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
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x200, sData->ourC->y - 0x1800, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x200, sData->ourC->y - 0x1800, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x200, sData->ourC->y - 0x1800, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x200, sData->ourC->y - 0x1800, 3, 0);
				}
			}
			else if (sData->ourC->down)
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x200, sData->ourC->y + 0x1800, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x200, sData->ourC->y + 0x1800, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x200, sData->ourC->y + 0x1800, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x200, sData->ourC->y + 0x1800, 3, 0);
				}
			}
			else
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x2C00, sData->ourC->y + 0x600, 0, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x2000, sData->ourC->y + 0x600, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x2C00, sData->ourC->y + 0x600, 2, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x2000, sData->ourC->y + 0x600, 3, 0);
				}
			}

			switch (level)
			{
				case 1:
					PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 117, SOUND_MODE_STOP_THEN_PLAY);
					break;
				case 2:
					PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 49, SOUND_MODE_STOP_THEN_PLAY);
					break;
				case 3:
					PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 60, SOUND_MODE_STOP_THEN_PLAY);
					break;
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Bullet Handling
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ActBullet_Nemesis(BULLET *bul, int level)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->act_no == 0)
		{
			bul->act_no = 1;
			bul->count1 = 0;

			switch (bul->direct)
			{
				case 0:
					bul->xm = -0x1000;
					break;
				case 1:
					bul->ym = -0x1000;
					break;
				case 2:
					bul->xm = 0x1000;
					break;
				case 3:
					bul->ym = 0x1000;
					break;
			}

			switch (level)
			{
				case 3:
					bul->xm /= 3;
					bul->ym /= 3;
					break;
			}
		}
		else
		{
			if (level == 1 && bul->count1 % 4 == 1)
			{
				switch (bul->direct)
				{
					case 0:
						SetNpChar(4, bul->x, bul->y, -0x200, Random(-0x200, 0x200), 2, 0, 0x100);
						break;
					case 1:
						SetNpChar(4, bul->x, bul->y, Random(-0x200, 0x200), -0x200, 2, 0, 0x100);
						break;
					case 2:
						SetNpChar(4, bul->x, bul->y, 0x200, Random(-0x200, 0x200), 2, 0, 0x100);
						break;
					case 3:
						SetNpChar(4, bul->x, bul->y, Random(-0x200, 0x200), 0x200, 2, 0, 0x100);
						break;
				}
			}

			bul->x += bul->xm;
			bul->y += bul->ym;
		}

		if (++bul->ani_no > 1)
			bul->ani_no = 0;

		RECT rcL[2] = {
			{0, 112, 32, 128},
			{0, 128, 32, 144},
		};

		RECT rcU[2] = {
			{32, 112, 48, 144},
			{48, 112, 64, 144},
		};

		RECT rcR[2] = {
			{64, 112, 96, 128},
			{64, 128, 96, 144},
		};

		RECT rcD[2] = {
			{96, 112, 112, 144},
			{112, 112, 128, 144},
		};

		switch (bul->direct)
		{
			case 0:
				bul->rect = rcL[bul->ani_no];
				break;
			case 1:
				bul->rect = rcU[bul->ani_no];
				break;
			case 2:
				bul->rect = rcR[bul->ani_no];
				break;
			case 3:
				bul->rect = rcD[bul->ani_no];
				break;
		}

		bul->rect.top    += 32 * ((level - 1) / 2);
		bul->rect.bottom += 32 * ((level - 1) / 2);
		bul->rect.left   += (level - 1) % 2 << 7;
		bul->rect.right  += (level - 1) % 2 << 7;
	}
}