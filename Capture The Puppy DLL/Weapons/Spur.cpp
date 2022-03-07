#include "ModInfo.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Weapon shooting
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ShootBullet_Spur(ShootInfo* sData, int level)
{
	BOOL bShot = FALSE;

	// Charge a shot while the button is held down
	if (sData->key & gKeyShot)
	{
		if (sData->sClient.HasArmsEnergy(sData->arms_code, 1))
		{
			if ((*sData->sClient.mSpurCharge) < 10 || sData->sClient.IsMaxExp() || sData->sClient.UseArmsEnergy(1))
			{
				sData->sClient.AddExp((sData->ourC->equip & 8 ? 3 : 2), true);

				// Increment the spur charge
				(*sData->sClient.mSpurCharge)++;

				// Play an appropriate charging sound
				if (*sData->sClient.mSpurCharge / 2 % 2)
				{
					switch (level)
					{
					case 1:
						PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 59, SOUND_MODE_STOP_THEN_PLAY);
						break;

					case 2:
						PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 60, SOUND_MODE_STOP_THEN_PLAY);
						break;

					case 3:
						if (!sData->sClient.IsMaxExp())
						{
							PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 61, SOUND_MODE_STOP_THEN_PLAY);
						}

						break;
					}
				}
			}
		}
	}
	else
	{
		// We let go of the shoot button!

		// If there is any charge, then shoot it.
		if (*sData->sClient.mSpurCharge)
			bShot = TRUE;

		// And then reset it back to zero.
		*sData->sClient.mSpurCharge = 0;
	}

	// If we're at maximum level & EXP
	if (((*sData->sClient.mSpurCharge) != 0 && !sData->sClient.HasArmsEnergy(sData->arms_code, 1)) || sData->sClient.IsMaxExp())
	{
		// If we don't think we're at maximum charge, then set it.
		if (!(*sData->sClient.mSpurMax))
		{
			*sData->sClient.mSpurMax = true;

			// Play 'fully charged' sound
			if (!sData->sClient.IsMaxExp())
				PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 37, SOUND_MODE_STOP_THEN_PLAY);
			else
				PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 65, SOUND_MODE_STOP_THEN_PLAY);
		}
	}
	else if (*sData->sClient.mSpurMax)
	{
		// We're not at maximum charge anymore!
		*sData->sClient.mSpurMax = false;
	}

	// Zero this weapon's EXP if we don't have gKeyShot held down.
	if (!(sData->key & gKeyShot))
		sData->sClient.ZeroExp();

	// Determine which bullet should be fired
	int bul_no  = 0;
	int bul_lvl = sData->arms_level;
	switch (level)
	{
	case 1:
		bul_no  = 8; // Polar Star Level 3 Bullet
		bul_lvl = 2;
		bShot   = FALSE;
		break;
	case 2:
		bul_no  = sData->bullet_ids[0];
		bul_lvl = 0;
		break;
	case 3:
		if (*sData->sClient.mSpurMax)
		{
			bul_no  = sData->bullet_ids[2];
			bul_lvl = 2;
		}
		else
		{
			bul_no  = sData->bullet_ids[1];
			bul_lvl = 1;
		}

		break;
	}

	// Make sure there aren't too many bullets on screen.
	if (
		sData->sClient.CountBulletNum(sData->bullet_ids[0]) > 0 ||
		sData->sClient.CountBulletNum(sData->bullet_ids[1]) > 0 ||
		sData->sClient.CountBulletNum(sData->bullet_ids[2]) > 0 ||
		sData->sClient.CountBulletNum(sData->bullet_ids[2] + 1) > 0 ||
		sData->sClient.CountBulletNum(sData->bullet_ids[2] + 2) > 0 ||
		sData->sClient.CountBulletNum(sData->bullet_ids[2] + 3) > 0
		)
		return;

	// Shoot the weapon if we are so inclined to!
	if (sData->keyTrg & gKeyShot || bShot)
	{
		if (!bShot && !sData->sClient.UseArmsEnergy(1))
		{
			// Play 'empty' sound
			PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 37, SOUND_MODE_STOP_THEN_PLAY);

			// Show 'EMPTY'
			if (empty == 0 && sData->sClient.mIsOurUser)
			{
				SetCaret(sData->ourC->x, sData->ourC->y, 16, 0);
				empty = 50;
			}
		}
		else
		{
			// Spawn the bullet depending on which direction we're facing.
			if (sData->ourC->up)
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(bul_no, sData->ourC->x - 0x200, sData->ourC->y - 0x1000, 1, sData->arms_code, bul_lvl, sData->ghostId);
					SetCaret(sData->ourC->x - 0x200, sData->ourC->y - 0x1000, 3, 0);
				}
				else
				{
					SetBullet(bul_no, sData->ourC->x + 0x200, sData->ourC->y - 0x1000, 1, sData->arms_code, bul_lvl, sData->ghostId);
					SetCaret(sData->ourC->x + 0x200, sData->ourC->y - 0x1000, 3, 0);
				}
			}
			else if (sData->ourC->down)
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(bul_no, sData->ourC->x - 0x200, sData->ourC->y + 0x1000, 3, sData->arms_code, bul_lvl, sData->ghostId);
					SetCaret(sData->ourC->x - 0x200, sData->ourC->y + 0x1000, 3, 0);
				}
				else
				{
					SetBullet(bul_no, sData->ourC->x + 0x200, sData->ourC->y + 0x1000, 3, sData->arms_code, bul_lvl, sData->ghostId);
					SetCaret(sData->ourC->x + 0x200, sData->ourC->y + 0x1000, 3, 0);
				}
			}
			else
			{
				if (sData->ourC->direct == 0)
				{
					SetBullet(bul_no, sData->ourC->x - 0xC00, sData->ourC->y + 0x600, 0, sData->arms_code, bul_lvl, sData->ghostId);
					SetCaret(sData->ourC->x - 0x1800, sData->ourC->y + 0x600, 3, 0);
				}
				else
				{
					SetBullet(bul_no, sData->ourC->x + 0xC00, sData->ourC->y + 0x600, 2, sData->arms_code, bul_lvl, sData->ghostId);
					SetCaret(sData->ourC->x + 0x1800, sData->ourC->y + 0x600, 3, 0);
				}
			}

			// Play the 'shoot' sound effect
			if (bul_no == 8)
				PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 49, SOUND_MODE_STOP_THEN_PLAY);
			else if (bul_no == sData->bullet_ids[0])
				PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 62, SOUND_MODE_STOP_THEN_PLAY);
			else if (bul_no == sData->bullet_ids[1])
				PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 63, SOUND_MODE_STOP_THEN_PLAY);
			else if (bul_no == sData->bullet_ids[2])
				PlaySoundObject2D(sData->sClient.mMC->x, sData->sClient.mMC->y, 0x6000, true, 64, SOUND_MODE_STOP_THEN_PLAY);
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Bullet Handling
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void ActBullet_Spur(BULLET* bul, int level)
{
	if (++bul->count1 > bul->life_count)
	{
		bul->cond = 0;
		SetCaret(bul->x, bul->y, 3, 0);
	}
	else
	{
		if (bul->damage && bul->life != 100)
			bul->damage = 0;

		if (bul->act_no == 0)
		{
			bul->act_no = 1;

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
			bul->x += bul->xm;
			bul->y += bul->ym;
		}

		RECT rect1[2] =
		{
			{128, 32, 144, 48},
			{144, 32, 160, 48},
		};

		RECT rect2[2] =
		{
			{160, 32, 176, 48},
			{176, 32, 192, 48},
		};

		RECT rect3[2] =
		{
			{128, 48, 144, 64},
			{144, 48, 160, 64},
		};

		bul->damage = bul->life;

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

		SetBullet(bul->code_bullet + (4 - level), bul->x, bul->y, bul->direct, -1, bul->level, bul->ghostId);
	}
}

void ActBullet_SpurTail(BULLET* bul, int level)
{
	if (++bul->count1 > 20)
		bul->ani_no = bul->count1 - 20;
	if (bul->ani_no > 2)
		bul->cond = 0;
	if (bul->damage && bul->life != 100)
		bul->damage = 0;

	RECT rc_h_lv1[3] =
	{
		{192, 32, 200, 40},
		{200, 32, 208, 40},
		{208, 32, 216, 40},
	};

	RECT rc_v_lv1[3] =
	{
		{192, 40, 200, 48},
		{200, 40, 208, 48},
		{208, 40, 216, 48},
	};

	RECT rc_h_lv2[3] =
	{
		{216, 32, 224, 40},
		{224, 32, 232, 40},
		{232, 32, 240, 40},
	};

	RECT rc_v_lv2[3] =
	{
		{216, 40, 224, 48},
		{224, 40, 232, 48},
		{232, 40, 240, 48},
	};

	RECT rc_h_lv3[3] =
	{
		{240, 32, 248, 40},
		{248, 32, 256, 40},
		{256, 32, 264, 40},
	};

	RECT rc_v_lv3[3] =
	{
		{240, 32, 248, 40},
		{248, 32, 256, 40},
		{256, 32, 264, 40},
	};

	switch (level)
	{
	case 1:
		if (bul->direct == 0 || bul->direct == 2)
			bul->rect = rc_h_lv1[bul->ani_no];
		else
			bul->rect = rc_v_lv1[bul->ani_no];

		break;

	case 2:
		if (bul->direct == 0 || bul->direct == 2)
			bul->rect = rc_h_lv2[bul->ani_no];
		else
			bul->rect = rc_v_lv2[bul->ani_no];

		break;

	case 3:
		if (bul->direct == 0 || bul->direct == 2)
			bul->rect = rc_h_lv3[bul->ani_no];
		else
			bul->rect = rc_v_lv3[bul->ani_no];

		break;
	}
}