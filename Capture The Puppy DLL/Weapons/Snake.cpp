#include "ModInfo.h"
#include <stdio.h>

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Weapon shooting
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ShootBullet_Snake(ShootInfo* sData, int level)
{
	if (CountArmsBullet(sData->arms_code, sData->ghostId) > 3)
		return;

	if (sData->keyTrg & GetKeybind(CSM_KEYBIND_DESC_SHOOT))
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
				// Shoot up
				if (sData->ourC->direct == Directions::DIRECT_LEFT)
				{
					// Shoot left
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x600, sData->ourC->y - 0x1400, Directions::DIRECT_UP, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x600, sData->ourC->y - 0x1400, 3, 0);
				}
				else
				{
					// Shoot right
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x600, sData->ourC->y - 0x1400, Directions::DIRECT_UP, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x600, sData->ourC->y - 0x1400, 3, 0);
				}
			}
			else if (sData->ourC->down)
			{
				// Shoot down
				if (sData->ourC->direct == Directions::DIRECT_LEFT)
				{
					// Shoot left
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x600, sData->ourC->y + 0x1400, Directions::DIRECT_DOWN, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x600, sData->ourC->y + 0x1400, 3, 0);
				}
				else
				{
					// Shoot right
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x600, sData->ourC->y + 0x1400, Directions::DIRECT_DOWN, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x600, sData->ourC->y + 0x1400, 3, 0);
				}
			}
			else
			{
				// Shoot straight
				if (sData->ourC->direct == Directions::DIRECT_LEFT)
				{
					// Shoot left
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0xC00, sData->ourC->y + 0x400, Directions::DIRECT_LEFT, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x1800, sData->ourC->y + 0x400, 3, 0);
				}
				else
				{
					// Shoot right
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0xC00, sData->ourC->y + 0x400, Directions::DIRECT_RIGHT, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x1800, sData->ourC->y + 0x400, 3, 0);
				}
			}

			PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 33, SOUND_MODE_STOP_THEN_PLAY);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Bullets
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ActBullet_Snake(BULLET* bul, int level)
{
	if (level == 0)
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
				bul->ani_no = Random(0, 2);
				bul->act_no = 1;

				switch (bul->direct)
				{
				case 0:
					bul->xm = -0x600;
					break;
				case 1:
					bul->ym = -0x600;
					break;
				case 2:
					bul->xm = 0x600;
					break;
				case 3:
					bul->ym = 0x600;
					break;
				}
			}
			else
			{
				bul->x += bul->xm;
				bul->y += bul->ym;
			}

			if (++bul->ani_wait > 0)
			{
				bul->ani_wait = 0;
				++bul->ani_no;
			}

			if (bul->ani_no > 3)
				bul->ani_no = 0;

			RECT rcLeft[4] =
			{
				{136, 80, 152, 80},
				{120, 80, 136, 96},
				{136, 64, 152, 80},
				{120, 64, 136, 80},
			};

			RECT rcRight[4] =
			{
				{120, 64, 136, 80},
				{136, 64, 152, 80},
				{120, 80, 136, 96},
				{136, 80, 152, 80},
			};

			if (bul->direct == 0)
				bul->rect = rcLeft[bul->ani_no];
			else
				bul->rect = rcRight[bul->ani_no];
		}

		return;
	}

	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->act_no == 0)
		{
			bul->ani_no = Random(0, 2);
			bul->act_no = 1;

			switch (bul->direct)
			{
			case 0:
				bul->xm = -0x200;
				break;
			case 1:
				bul->ym = -0x200;
				break;
			case 2:
				bul->xm = 0x200;
				break;
			case 3:
				bul->ym = 0x200;
				break;
			}

			static unsigned int inc;
			++inc;

			switch (bul->direct)
			{
			case 0:
			case 2:
				if (inc % 2)
					bul->ym = 0x400;
				else
					bul->ym = -0x400;

				break;

			case 1:
			case 3:
				if (inc % 2)
					bul->xm = 0x400;
				else
					bul->xm = -0x400;

				break;
			}
		}
		else
		{
			switch (bul->direct)
			{
			case 0:
				bul->xm -= 0x80;
				break;
			case 1:
				bul->ym -= 0x80;
				break;
			case 2:
				bul->xm += 0x80;
				break;
			case 3:
				bul->ym += 0x80;
				break;
			}

			switch (bul->direct)
			{
			case 0:
			case 2:
				if (bul->count1 % 5 == 2)
				{
					if (bul->ym < 0)
						bul->ym = 0x400;
					else
						bul->ym = -0x400;
				}

				break;

			case 1u:
			case 3u:
				if (bul->count1 % 5 == 2)
				{
					if (bul->xm < 0)
						bul->xm = 0x400;
					else
						bul->xm = -0x400;
				}

				break;
			}

			bul->x += bul->xm;
			bul->y += bul->ym;
		}

		if (++bul->ani_wait > 0)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 2)
			bul->ani_no = 0;

		RECT rect[3] =
		{
			{192, 16, 208, 32},
			{208, 16, 224, 32},
			{224, 16, 240, 32},
		};

		bul->rect = rect[bul->ani_no];

		if (level == 2)
			SetNpChar(129, bul->x, bul->y, 0, -0x200, bul->ani_no, 0, 0x100);
		else
			SetNpChar(129, bul->x, bul->y, 0, -0x200, bul->ani_no + 3, 0, 0x100);
	}
}