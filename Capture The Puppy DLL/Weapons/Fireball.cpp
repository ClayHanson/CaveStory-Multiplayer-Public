#include "ModInfo.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Weapon shooting
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ShootBullet_FireBall(ShootInfo* sData, int level)
{
	int bul_no;

	if (CountArmsBullet(sData->arms_code, sData->ghostId) > level)
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
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x800, sData->ourC->y - 0x1000, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x800, sData->ourC->y - 0x1000, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x800, sData->ourC->y - 0x1000, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x800, sData->ourC->y - 0x1000, 3, 0);
				}
			}
			else if (sData->ourC->down)
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x800, sData->ourC->y + 0x1000, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x800, sData->ourC->y + 0x1000, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x800, sData->ourC->y + 0x1000, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x800, sData->ourC->y + 0x1000, 3, 0);
				}
			}
			else
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0xC00, sData->ourC->y + 0x400, 0, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x1800, sData->ourC->y + 0x400, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0xC00, sData->ourC->y + 0x400, 2, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x1800, sData->ourC->y + 0x400, 3, 0);
				}
			}

			PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 34, SOUND_MODE_STOP_THEN_PLAY);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Bullet Handling
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ActBullet_FireBall(BULLET* bul, int level)
{
	bool bBreak;
	RECT rect_left1[4];
	RECT rect_right1[4];
	RECT rect_left2[3];
	RECT rect_right2[3];

	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		bBreak = FALSE;
		if (bul->flag & 2 && bul->flag & 8)
			bBreak = TRUE;
		if (bul->flag & 1 && bul->flag & 4)
			bBreak = TRUE;

		if (!bul->direct && bul->flag & 1)
			bul->direct = 2;
		if (bul->direct == 2 && bul->flag & 4)
			bul->direct = 0;

		if (bBreak)
		{
			bul->cond = 0;
			SetCaret(bul->x, bul->y, 2, 0);
			PlaySoundObject2D(bul->x, bul->y, 0x6000, true, 28, SOUND_MODE_STOP_THEN_PLAY);
		}
		else
		{
			if (bul->act_no == 0)
			{
				bul->act_no = 1;

				switch (bul->direct)
				{
					case 0:
						bul->xm = -0x400;
						break;
					case 1:
						bul->xm = GetLocalPlayerCharacter()->xm;

						if (GetLocalPlayerCharacter()->xm < 0)
							bul->direct = 0;
						else
							bul->direct = 2;

						if (GetLocalPlayerCharacter()->direct == 0)
							bul->xm -= 0x80;
						else
							bul->xm += 0x80;

						bul->ym = -0x5FF;
						break;
					case 2:
						bul->xm = 0x400;
						break;
					case 3:
						bul->xm = GetLocalPlayerCharacter()->xm;

						if (GetLocalPlayerCharacter()->xm < 0)
							bul->direct = 0;
						else
							bul->direct = 2;

						bul->ym = 0x5FF;

						break;
				}
			}
			else
			{
				if (bul->flag & 8)
					bul->ym = -0x400;
				else if (bul->flag & 1)
					bul->xm = 0x400;
				else if (bul->flag & 4)
					bul->xm = -0x400;

				bul->ym += 85;
				if (bul->ym > 0x3FF)
					bul->ym = 0x3FF;

				bul->x += bul->xm;
				bul->y += bul->ym;

				if (bul->flag & 0xD)
					PlaySoundObject2D(bul->x, bul->y, 0x6000, true, 34, SOUND_MODE_STOP_THEN_PLAY);
			}

			SET_RECT(rect_left1[0], 128, 0, 144, 16)
			SET_RECT(rect_left1[1], 144, 0, 160, 16)
			SET_RECT(rect_left1[2], 160, 0, 176, 16)
			SET_RECT(rect_left1[3], 176, 0, 192, 16)

			SET_RECT(rect_right1[0], 128, 16, 144, 32)
			SET_RECT(rect_right1[1], 144, 16, 160, 32)
			SET_RECT(rect_right1[2], 160, 16, 176, 32)
			SET_RECT(rect_right1[3], 176, 16, 192, 32)

			SET_RECT(rect_left2[0], 192, 16, 208, 32)
			SET_RECT(rect_left2[1], 208, 16, 224, 32)
			SET_RECT(rect_left2[2], 224, 16, 240, 32)

			SET_RECT(rect_right2[0], 224, 16, 240, 32)
			SET_RECT(rect_right2[1], 208, 16, 224, 32)
			SET_RECT(rect_right2[2], 192, 16, 208, 32)

			bul->ani_no++;

			if (level == 1)
			{
				if (bul->ani_no > 3)
					bul->ani_no = 0;

				if (bul->direct == 0)
					bul->rect = rect_left1[bul->ani_no];
				else
					bul->rect = rect_right1[bul->ani_no];
			}
			else
			{
				if (bul->ani_no > 2)
					bul->ani_no = 0;

				if (bul->direct == 0)
					bul->rect = rect_left2[bul->ani_no];
				else
					bul->rect = rect_right2[bul->ani_no];

				if (level == 2)
					SetNpChar(129, bul->x, bul->y, 0, -0x200, bul->ani_no, 0, 0x100);
				else
					SetNpChar(129, bul->x, bul->y, 0, -0x200, bul->ani_no + 3, 0, 0x100);
			}
		}
	}
}