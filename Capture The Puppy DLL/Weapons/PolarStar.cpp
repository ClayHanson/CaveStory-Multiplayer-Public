#include "ModInfo.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Weapon shooting
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ShootBullet_PoleStar(ShootInfo* sData, int level)
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
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x200, sData->ourC->y - 0x1000, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x200, sData->ourC->y - 0x1000, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x200, sData->ourC->y - 0x1000, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x200, sData->ourC->y - 0x1000, 3, 0);
				}
			}
			else if (sData->ourC->down)
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x200, sData->ourC->y + 0x1000, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x200, sData->ourC->y + 0x1000, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x200, sData->ourC->y + 0x1000, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x200, sData->ourC->y + 0x1000, 3, 0);
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

			if (level == 3)
				PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 49, SOUND_MODE_STOP_THEN_PLAY);
			else
				PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 32, SOUND_MODE_STOP_THEN_PLAY);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Bullet Handling
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ActBullet_PoleStar(BULLET* bul, int level)
{
	RECT rect1[2];
	RECT rect2[2];
	RECT rect3[2];

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

			// Set speed
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

			// Set hitbox
			switch (level)
			{
				case 1:
					switch (bul->direct)
					{
						case 0:
							bul->enemyYL = 0x400;
							break;
						case 1:
							bul->enemyXL = 0x400;
							break;
						case 2:
							bul->enemyYL = 0x400;
							break;
						case 3:
							bul->enemyXL = 0x400;
							break;
					}
					break;
				case 2:
					switch (bul->direct)
					{
						case 0:
							bul->enemyYL = 0x800;
							break;
						case 1:
							bul->enemyXL = 0x800;
							break;
						case 2:
							bul->enemyYL = 0x800;
							break;
						case 3:
							bul->enemyXL = 0x800;
							break;
					}
					break;
			}
		}
		else
		{
			// Move
			bul->x += bul->xm;
			bul->y += bul->ym;
		}

		SET_RECT(rect1[0], 128, 32, 144, 48)
		SET_RECT(rect1[1], 144, 32, 160, 48)

		SET_RECT(rect2[0], 160, 32, 176, 48)
		SET_RECT(rect2[1], 176, 32, 192, 48)

		SET_RECT(rect3[0], 128, 48, 144, 64)
		SET_RECT(rect3[1], 144, 48, 160, 64)

		//Set framerect
		switch (level)
		{
			case 1:
				if (bul->direct == 1 || bul->direct == 3)
					bul->rect = rect1[1];
				else
					bul->rect = rect1[0];

				break;
			case 2:
				if (bul->direct == 1 || bul->direct == 3)
					bul->rect = rect2[1];
				else
					bul->rect = rect2[0];

				break;
			case 3:
				if (bul->direct == 1 || bul->direct == 3)
					bul->rect = rect3[1];
				else
					bul->rect = rect3[0];

				break;
		}
	}
}
