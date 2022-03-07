#include "ModInfo.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Weapon shooting
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ShootBullet_Sword(ShootInfo* sData, int level)
{
	if (CountArmsBullet(sData->arms_code, sData->ghostId) > 0)
		return;

	// Wait until shoot
	if (!(sData->keyTrg & gKeyShot))
		return;

	if (sData->ourC->up)
	{
		if (sData->ourC->direct == Directions::DIRECT_LEFT)
			SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x200, sData->ourC->y + 0x800, Directions::DIRECT_UP, sData->arms_code, sData->arms_level, sData->ghostId);
		else
			SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x200, sData->ourC->y + 0x800, Directions::DIRECT_UP, sData->arms_code, sData->arms_level, sData->ghostId);
	}
	else if (sData->ourC->down)
	{
		if (sData->ourC->direct == Directions::DIRECT_LEFT)
			SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0x200, sData->ourC->y - 0xC00, Directions::DIRECT_DOWN, sData->arms_code, sData->arms_level, sData->ghostId);
		else
			SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0x200, sData->ourC->y - 0xC00, Directions::DIRECT_DOWN, sData->arms_code, sData->arms_level, sData->ghostId);
	}
	else
	{
		if (sData->ourC->direct == Directions::DIRECT_LEFT)
			SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x + 0xC00, sData->ourC->y - 0x600, Directions::DIRECT_LEFT, sData->arms_code, sData->arms_level, sData->ghostId);
		else
			SetBullet(sData->bullet_ids[sData->arms_level], sData->ourC->x - 0xC00, sData->ourC->y - 0x600, Directions::DIRECT_RIGHT, sData->arms_code, sData->arms_level, sData->ghostId);
	}

	PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 34, SOUND_MODE_STOP_THEN_PLAY);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Bullet Handling
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ActBullet_Sword1(BULLET* bul, int level)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->count1 == 3)
			bul->bbits &= ~4;

		if (bul->count1 % 5 == 1)
			PlaySoundObject2D(bul->x, bul->y, 0x6000, true, 34, SOUND_MODE_STOP_THEN_PLAY);

		if (bul->act_no == 0)
		{
			bul->act_no = 1;

			switch (bul->direct)
			{
			case 0:
				bul->xm = -0x800;
				break;
			case 1:
				bul->ym = -0x800;
				break;
			case 2:
				bul->xm = 0x800;
				break;
			case 3:
				bul->ym = 0x800;
				break;
			}
		}
		else
		{
			bul->x += bul->xm;
			bul->y += bul->ym;
		}

		RECT rcLeft[4] =
		{
			{0, 48, 16, 64},
			{16, 48, 32, 64},
			{32, 48, 48, 64},
			{48, 48, 64, 64},
		};

		RECT rcRight[4] =
		{
			{64, 48, 80, 64},
			{80, 48, 96, 64},
			{96, 48, 112, 64},
			{112, 48, 128, 64},
		};

		if (++bul->ani_wait > 1)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 3)
			bul->ani_no = 0;

		if (bul->direct == 0)
			bul->rect = rcLeft[bul->ani_no];
		else
			bul->rect = rcRight[bul->ani_no];
	}
}

void ActBullet_Sword2(BULLET* bul, int level)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->count1 == 3)
			bul->bbits &= ~4;

		if (bul->count1 % 7 == 1)
			PlaySoundObject2D(bul->x, bul->y, 0x6000, true, 106, SOUND_MODE_STOP_THEN_PLAY);

		if (bul->act_no == 0)
		{
			bul->act_no = 1;

			switch (bul->direct)
			{
			case 0:
				bul->xm = -0x800;
				break;
			case 1:
				bul->ym = -0x800;
				break;
			case 2:
				bul->xm = 0x800;
				break;
			case 3:
				bul->ym = 0x800;
				break;
			}
		}
		else
		{
			bul->x += bul->xm;
			bul->y += bul->ym;
		}

		RECT rcLeft[4] =
		{
			{160, 48, 184, 72},
			{184, 48, 208, 72},
			{208, 48, 232, 72},
			{232, 48, 256, 72},
		};

		RECT rcRight[4] =
		{
			{160, 72, 184, 96},
			{184, 72, 208, 96},
			{208, 72, 232, 96},
			{232, 72, 256, 96},
		};

		if (++bul->ani_wait > 1)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->ani_no > 3)
			bul->ani_no = 0;

		if (bul->direct == 0)
			bul->rect = rcLeft[bul->ani_no];
		else
			bul->rect = rcRight[bul->ani_no];
	}
}

void ActBullet_Sword3(BULLET* bul, int level)
{
	RECT rcLeft[2] =
	{
		{272, 0, 296, 24},
		{296, 0, 320, 24},
	};

	RECT rcUp[2] =
	{
		{272, 48, 296, 72},
		{296, 0, 320, 24},
	};

	RECT rcRight[2] =
	{
		{272, 24, 296, 48},
		{296, 24, 320, 48},
	};

	RECT rcDown[2] =
	{
		{296, 48, 320, 72},
		{296, 24, 320, 48},
	};

	switch (bul->act_no)
	{
	case 0:
		bul->act_no = 1;
		bul->xm     = 0;
		bul->ym     = 0;
		// Fallthrough

	case 1:
		switch (bul->direct)
		{
		case 0:
			bul->xm = -0x800;
			break;
		case 1:
			bul->ym = -0x800;
			break;
		case 2:
			bul->xm = 0x800;
			break;
		case 3:
			bul->ym = 0x800;
			break;
		}

		if (bul->life != 100)
		{
			bul->act_no = 2;
			bul->ani_no = 1;
			bul->damage = -1;
			bul->act_wait = 0;
		}

		if (++bul->act_wait % 4 == 1)
		{
			PlaySoundObject2D(bul->x, bul->y, 0x6000, true, 106, SOUND_MODE_STOP_THEN_PLAY);

			if (++bul->count1 % 2)
				SetBullet(bul->code_bullet - 4, bul->x, bul->y, 0, bul->code_arms, bul->level, bul->ghostId);
			else
				SetBullet(bul->code_bullet - 4, bul->x, bul->y, 2, bul->code_arms, bul->level, bul->ghostId);
		}

		if (++bul->count1 == 5)
			bul->bbits &= ~4;

		if (bul->count1 > bul->life_count)
		{
			bul->cond = 0;
			SetCaret(bul->x, bul->y, 3, 0);
			return;
		}

		break;

	case 2:
		bul->xm = 0;
		bul->ym = 0;
		++bul->act_wait;

		if (Random(-1, 1) == 0)
		{
			PlaySoundObject2D(bul->x, bul->y, 0x6000, true, 106, SOUND_MODE_STOP_THEN_PLAY);

			if (Random(0, 1) % 2)
				SetBullet(bul->code_bullet - 4, bul->x + (Random(-0x40, 0x40) * 0x200), bul->y + (Random(-0x40, 0x40) * 0x200), 0, bul->code_arms, bul->level, bul->ghostId);
			else
				SetBullet(bul->code_bullet - 4, bul->x + (Random(-0x40, 0x40) * 0x200), bul->y + (Random(-0x40, 0x40) * 0x200), 2, bul->code_arms, bul->level, bul->ghostId);
		}

		if (bul->act_wait > 50)
			bul->cond = 0;
	}

	bul->x += bul->xm;
	bul->y += bul->ym;

	switch (bul->direct)
	{
	case 0:
		bul->rect = rcLeft[bul->ani_no];
		break;
	case 1:
		bul->rect = rcUp[bul->ani_no];
		break;
	case 2:
		bul->rect = rcRight[bul->ani_no];
		break;
	case 3:
		bul->rect = rcDown[bul->ani_no];
		break;
	}

	if (bul->act_wait % 2)
		bul->rect.right = 0;
}

void ActBullet_Edge(BULLET* bul, int level)
{
	switch (bul->act_no)
	{
	case 0:
		bul->act_no = 1;
		bul->y -= 0x1800;

		if (bul->direct == 0)
			bul->x += 0x2000;
		else
			bul->x -= 0x2000;
		// Fallthrough
	case 1:
		if (++bul->ani_wait > 2)
		{
			bul->ani_wait = 0;
			++bul->ani_no;
		}

		if (bul->direct == 0)
			bul->x -= 0x400;
		else
			bul->x += 0x400;

		bul->y += 0x400;

		if (bul->ani_no == 1)
			bul->damage = 2;
		else
			bul->damage = 1;

		if (bul->ani_no > 4)
			bul->cond = 0;

		break;
	}

	RECT rcLeft[5] = {
		{0, 64, 24, 88},
		{24, 64, 48, 88},
		{48, 64, 72, 88},
		{72, 64, 96, 88},
		{96, 64, 120, 88},
	};

	RECT rcRight[5] = {
		{0, 88, 24, 112},
		{24, 88, 48, 112},
		{48, 88, 72, 112},
		{72, 88, 96, 112},
		{96, 88, 120, 112},
	};

	if (bul->direct == 0)
		bul->rect = rcLeft[bul->ani_no];
	else
		bul->rect = rcRight[bul->ani_no];
}

void ActBullet_Drop(BULLET* bul, int level)
{
	RECT rc = { 0, 0, 0, 0 };

	if (++bul->act_wait > 2)
		bul->cond = 0;

	bul->rect = rc;
}