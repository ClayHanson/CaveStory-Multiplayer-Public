#include "ModInfo.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Weapon shooting
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
void ShootBullet_Missile(ShootInfo* sData, int level, BOOL bSuper)
{
	if (bSuper)
	{
		switch (level)
		{
		case 1:
			if (CountArmsBullet(sData->arms_code, sData->ghostId) > 0)
				return;

			break;

		case 2:
			if (CountArmsBullet(sData->arms_code, sData->ghostId) > 1)
				return;

			break;

		case 3:
			if (CountArmsBullet(sData->arms_code, sData->ghostId) > 3)
				return;

			break;
		}
	}
	else
	{
		switch (level)
		{
		case 1:
			if (CountArmsBullet(sData->arms_code, sData->ghostId) > 0)
				return;

			break;

		case 2:
			if (CountArmsBullet(sData->arms_code, sData->ghostId) > 1)
				return;

			break;

		case 3:
			if (CountArmsBullet(sData->arms_code, sData->ghostId) > 3)
				return;

			break;
		}
	}

	if (sData->keyTrg & GetKeybind(CSM_KEYBIND_DESC::CSM_KEYBIND_DESC_SHOOT))
	{
		if (level < 3)
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
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0xC00, sData->ourC->y, 0, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x1800, sData->ourC->y, 3, 0);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0xC00, sData->ourC->y, 2, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x1800, sData->ourC->y, 3, 0);
				}
			}
		}
		else
		{
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

			if (sData->ourC->up)
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x200, sData->ourC->y - 0x1000, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x200, sData->ourC->y - 0x1000, 3, 0);
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x600, sData->ourC->y, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x600, sData->ourC->y, 1, sData->arms_code, sData->arms_level, sData->ghostId);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x200, sData->ourC->y - 0x1000, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x200, sData->ourC->y - 0x1000, 3, 0);
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x600, sData->ourC->y, 1, sData->arms_code, sData->arms_level, sData->ghostId);
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x600, sData->ourC->y, 1, sData->arms_code, sData->arms_level, sData->ghostId);
				}
			}
			else if (sData->ourC->down)
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x200, sData->ourC->y + 0x1000, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x200, sData->ourC->y + 0x1000, 3, 0);
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x600, sData->ourC->y, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x600, sData->ourC->y, 3, sData->arms_code, sData->arms_level, sData->ghostId);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x200, sData->ourC->y + 0x1000, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x200, sData->ourC->y + 0x1000, 3, 0);
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x600, sData->ourC->y, 3, sData->arms_code, sData->arms_level, sData->ghostId);
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x600, sData->ourC->y, 3, sData->arms_code, sData->arms_level, sData->ghostId);
				}
			}
			else
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0xC00, sData->ourC->y + 0x200, 0, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x - 0x1800, sData->ourC->y + 0x200, 3, 0);
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x, sData->ourC->y - 0x1000, 0, sData->arms_code, sData->arms_level, sData->ghostId);
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x800, sData->ourC->y - 0x200, 0, sData->arms_code, sData->arms_level, sData->ghostId);
				}
				else
				{
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0xC00, sData->ourC->y + 0x200, 2, sData->arms_code, sData->arms_level, sData->ghostId);
					SetCaret(sData->ourC->x + 0x1800, sData->ourC->y + 0x200, 3, 0);
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x, sData->ourC->y - 0x1000, 2, sData->arms_code, sData->arms_level, sData->ghostId);
					SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x800, sData->ourC->y - 0x200, 2, sData->arms_code, sData->arms_level, sData->ghostId);
				}
			}
		}

		PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 32, SOUND_MODE_STOP_THEN_PLAY);
	}
}

void ShootBullet_SuperMissile(ShootInfo* sData, int level)
{
	ShootBullet_Missile(sData, level, TRUE);
}

void ShootBullet_NormalMissile(ShootInfo* sData, int level)
{
	ShootBullet_Missile(sData, level, FALSE);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Bullet Handling
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ActBullet_Missile(BULLET* bul, int level)
{
	BOOL bHit;

	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
		return;
	}

	bHit = FALSE;

	if (bul->life != 10)
		bHit = TRUE;
	if (bul->direct == 0 && bul->flag & 1)
		bHit = TRUE;
	if (bul->direct == 2 && bul->flag & 4)
		bHit = TRUE;
	if (bul->direct == 1 && bul->flag & 2)
		bHit = TRUE;
	if (bul->direct == 3 && bul->flag & 8)
		bHit = TRUE;
	if (bul->direct == 0 && bul->flag & 0x80)
		bHit = TRUE;
	if (bul->direct == 0 && bul->flag & 0x20)
		bHit = TRUE;
	if (bul->direct == 2 && bul->flag & 0x40)
		bHit = TRUE;
	if (bul->direct == 2 && bul->flag & 0x10)
		bHit = TRUE;

	if (bHit)
	{
		SetBullet(bul->code_bullet + (4 - level), bul->x, bul->y, 0, bul->code_arms, bul->level, bul->ghostId);
		bul->cond = 0;
	}

	switch (bul->act_no)
	{
	case 0:
		bul->act_no = 1;

		switch (bul->direct)
		{
		case 0:
		case 2:
			bul->tgt_y = bul->y;
			break;
		case 1:
		case 3:
			bul->tgt_x = bul->x;
			break;
		}

		if (level == 3)
		{
			switch (bul->direct)
			{
			case 0:
			case 2:
				if (bul->y > GetLocalPlayerCharacter()->y)
					bul->ym = 0x100;
				else
					bul->ym = -0x100;

				bul->xm = Random(-0x200, 0x200);
				break;

			case 1:
			case 3:
				if (bul->x > GetLocalPlayerCharacter()->x)
					bul->xm = 0x100;
				else
					bul->xm = -0x100;

				bul->ym = Random(-0x200, 0x200);
				break;
			}

			static unsigned int inc;

			switch (++inc % 3)
			{
			case 0:
				bul->ani_no = 0x80;
				break;
			case 1:
				bul->ani_no = 0x40;
				break;
			case 2:
				bul->ani_no = 0x33;
				break;
			}
		}
		else
		{
			bul->ani_no = 0x80;
		}
		// Fallthrough
	case 1:
		switch (bul->direct)
		{
		case 0:
			bul->xm += -bul->ani_no;
			break;
		case 1:
			bul->ym += -bul->ani_no;
			break;
		case 2:
			bul->xm += bul->ani_no;
			break;
		case 3:
			bul->ym += bul->ani_no;
			break;
		}

		if (level == 3)
		{
			switch (bul->direct)
			{
			case 0:
			case 2:
				if (bul->y < bul->tgt_y)
					bul->ym += 0x20;
				else
					bul->ym -= 0x20;

				break;

			case 1:
			case 3:
				if (bul->x < bul->tgt_x)
					bul->xm += 0x20;
				else
					bul->xm -= 0x20;
				break;
			}
		}

		if (bul->xm < -0xA00)
			bul->xm = -0xA00;
		if (bul->xm > 0xA00)
			bul->xm = 0xA00;

		if (bul->ym < -0xA00)
			bul->ym = -0xA00;
		if (bul->ym > 0xA00)
			bul->ym = 0xA00;

		bul->x += bul->xm;
		bul->y += bul->ym;

		break;
	}

	if (++bul->count2 > 2)
	{
		bul->count2 = 0;

		switch (bul->direct)
		{
		case 0:
			SetCaret(bul->x + 0x1000, bul->y, 7, 2);
			break;
		case 1:
			SetCaret(bul->x, bul->y + 0x1000, 7, 3);
			break;
		case 2:
			SetCaret(bul->x - 0x1000, bul->y, 7, 0);
			break;
		case 3:
			SetCaret(bul->x, bul->y - 0x1000, 7, 1);
			break;
		}
	}

	RECT rect1[4] = {
		{0, 0, 16, 16},
		{16, 0, 32, 16},
		{32, 0, 48, 16},
		{48, 0, 64, 16},
	};

	RECT rect2[4] = {
		{0, 16, 16, 32},
		{16, 16, 32, 32},
		{32, 16, 48, 32},
		{48, 16, 64, 32},
	};

	RECT rect3[4] = {
		{0, 32, 16, 48},
		{16, 32, 32, 48},
		{32, 32, 48, 48},
		{48, 32, 64, 48},
	};

	switch (level)
	{
	case 1:
		bul->rect = rect1[bul->direct];
		break;
	case 2:
		bul->rect = rect2[bul->direct];
		break;
	case 3:
		bul->rect = rect3[bul->direct];
		break;
	}
}

void ActBullet_Bom(BULLET* bul, int level)
{
	switch (bul->act_no)
	{
	case 0:
		bul->act_no = 1;

		switch (level)
		{
		case 1:
			bul->act_wait = 10;
			break;
		case 2:
			bul->act_wait = 15;
			break;
		case 3:
			bul->act_wait = 5;
			break;
		}

		PlaySoundObject2D(bul->x, bul->y, 0x6000, true, 44, SOUND_MODE_STOP_THEN_PLAY);
		// Fallthrough
	case 1:
		switch (level)
		{
		case 1:
			if (bul->act_wait % 3 == 0)
				SetDestroyNpCharUp(bul->x + (Random(-16, 16) << 9), bul->y + (Random(-16, 16) << 9), bul->enemyXL, 2);
			break;

		case 2:
			if (bul->act_wait % 3 == 0)
				SetDestroyNpCharUp(bul->x + (Random(-32, 32) << 9), bul->y + (Random(-32, 32) << 9), bul->enemyXL, 2);
			break;

		case 3:
			if (bul->act_wait % 3 == 0)
				SetDestroyNpCharUp(bul->x + (Random(-40, 40) << 9), bul->y + (Random(-40, 40) << 9), bul->enemyXL, 2);
			break;
		}

		if (--bul->act_wait < 0)
			bul->cond = 0;
		break;
	}
}