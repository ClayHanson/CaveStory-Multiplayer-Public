#include "ModInfo.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Bullet Handling
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ActBullet_SuperMissile(BULLET* bul, int level)
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
			bul->enemyXL = 0x1000;
			bul->blockXL = 0x1000;
			break;

		case 1:
		case 3:
			bul->tgt_x = bul->x;
			bul->enemyYL = 0x1000;
			bul->blockYL = 0x1000;
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
				bul->ani_no = 0x200;
				break;
			case 1:
				bul->ani_no = 0x100;
				break;
			case 2:
				bul->ani_no = 0xAA;
				break;
			}
		}
		else
		{
			bul->ani_no = 0x200;
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
					bul->ym += 0x40;
				else
					bul->ym -= 0x40;

				break;
			case 1:
			case 3:
				if (bul->x < bul->tgt_x)
					bul->xm += 0x40;
				else
					bul->xm -= 0x40;

				break;
			}
		}

		if (bul->xm < -0x1400)
			bul->xm = -0x1400;
		if (bul->xm > 0x1400)
			bul->xm = 0x1400;

		if (bul->ym < -0x1400)
			bul->ym = -0x1400;
		if (bul->ym > 0x1400)
			bul->ym = 0x1400;

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
		{120, 96, 136, 112},
		{136, 96, 152, 112},
		{152, 96, 168, 112},
		{168, 96, 184, 112},
	};

	RECT rect2[4] = {
		{184, 96, 200, 112},
		{200, 96, 216, 112},
		{216, 96, 232, 112},
		{232, 96, 248, 112},
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
		bul->rect = rect1[bul->direct];
		break;
	}
}

void ActBullet_SuperBom(BULLET* bul, int level)
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
			bul->act_wait = 14;
			break;
		case 3:
			bul->act_wait = 6;
			break;
		}

		PlaySoundObject2D(bul->x, bul->y, 0x6000, true, 44, SOUND_MODE_STOP_THEN_PLAY);
		// Fallthrough
	case 1:
		switch (level)
		{
		case 1:
			if (bul->act_wait % 3 == 0)
				SetDestroyNpCharUp(bul->x + (Random(-16, 16) * 0x200), bul->y + (Random(-16, 16) * 0x200), bul->enemyXL, 2);
			break;
		case 2:
			if (bul->act_wait % 3 == 0)
				SetDestroyNpCharUp(bul->x + (Random(-32, 32) * 0x200), bul->y + (Random(-32, 32) * 0x200), bul->enemyXL, 2);
			break;
		case 3:
			if (bul->act_wait % 3 == 0)
				SetDestroyNpCharUp(bul->x + (Random(-40, 40) * 0x200), bul->y + (Random(-40, 40) * 0x200), bul->enemyXL, 2);
			break;
		}

		if (--bul->act_wait < 0)
			bul->cond = 0;

		break;
	}
}