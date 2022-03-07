#include "Weapons/ModInfo.h"
#include "CavestoryModAPI.h"

//--------------------------------------------

static NPCHAR pGuideArrow;
static bool bGuideArrowInit   = false;
static bool bGuideArrowNpcSet = false;

static void InitGuideArrow()
{
	NPCHAR* pRealNpc;
	SetNpChar(413, 0, 0, 0, 0, 0, NULL, 100, &pRealNpc);

	if (pRealNpc)
	{
		bGuideArrowInit = true;
		memcpy(&pGuideArrow, pRealNpc, sizeof(NPCHAR));
		pRealNpc->cond  = 0;
	}
}

//--------------------------------------------

#define RECT_WIDTH(RC) ((RC).right - (RC).left)
#define RECT_HEIGHT(RC) ((RC).bottom - (RC).top)
#define RECT_WIDTH2(RC) (((RC).right - (RC).left) / 2)
#define RECT_HEIGHT2(RC) (((RC).bottom - (RC).top) / 2)

static RECT RC_TEAM_FLAG_CIRCLE              = { 0, 176, 23, 199 };
static RECT RC_TEAM_FLAG_CIRCLE_MASK         = { 0, 199, 20, 219 };
static RECT RC_TEAM_FLAG_CIRCLE_ARROW        = { 26, 194, 64, 232 };
static RECT RC_TEAM_FLAG_CIRCLE_ARROW_SHADOW = { 67, 194, 105, 232 };
static RECT RC_TEAM_FLAG_CIRCLE_ARROW_SMALL  = { 42, 194, 49, 203 };

static RECT RC_INDICATORS[] =
{
	/* 00: BG                       */ { 32, 176, 48, 192 },
	/* 01: EXCLAMATION MARK         */ { 48, 176, 64, 192 },
	/* 02: PUPPY                    */ { 64, 176, 80, 192 },
	/* 03: PUPPY COLOR MASK         */ { 80, 176, 96, 192 },
	/* 02: PUPPY (RIGHT)            */ { 96, 176, 112, 192 },
	/* 03: PUPPY COLOR MASK (RIGHT) */ { 112, 176, 128, 192 }
};

static RECT RC_NUMBERS[] =
{
	/* 0 */ { 0, 234, 9, 243 },
	/* 1 */ { 9, 234, 15, 243 },
	/* 2 */ { 15, 234, 24, 243 },
	/* 3 */ { 24, 234, 33, 243 },
	/* 4 */ { 33, 234, 42, 243 },
	/* 5 */ { 42, 234, 51, 243 },
	/* 6 */ { 51, 234, 60, 243 },
	/* 7 */ { 60, 234, 69, 243 },
	/* 8 */ { 69, 234, 78, 243 },
	/* 9 */ { 78, 234, 87, 243 }
};

#if 0
static RECT RC_NUMBERS[] =
{
	/* 0 */ { 0, 56, 8, 64 },
	/* 1 */ { 10, 56, 15, 64 },
	/* 2 */ { 16, 56, 24, 64 },
	/* 3 */ { 24, 56, 32, 64 },
	/* 4 */ { 32, 56, 40, 64 },
	/* 5 */ { 40, 56, 48, 64 },
	/* 6 */ { 48, 56, 56, 64 },
	/* 7 */ { 56, 56, 64, 64 },
	/* 8 */ { 64, 56, 72, 64 },
	/* 9 */ { 72, 56, 80, 64 }
};
#endif

//--------------------------------------------

int CTP_GetTeamScoreWidth(int iScore)
{
	if (iScore == 0)
		return RECT_WIDTH(RC_NUMBERS[0]);

	int iValue  = iScore;
	int iWidth  = 0;
	int iMod    = 1;
	int iOffset = 0;

	while (iValue / iMod > 0)
		iMod *= 10;

	iMod /= 10;

	while (iMod != 0)
	{
		iWidth += RECT_WIDTH(RC_NUMBERS[((iValue / iMod) % 10)]) + 1;

		++iOffset;
		iMod /= 10;
	}

	iWidth -= 1;

	return iWidth;
}

void CTP_PutTeamScore(int iScore, int iX, int iY, bool bCenterScore)
{
	if (bCenterScore)
		iX -= CTP_GetTeamScoreWidth(iScore) / 2;
	
	if (iScore == 0)
	{
		PutBitmap3(&grcGame, PixelToScreenCoord(iX), PixelToScreenCoord(iY), &RC_NUMBERS[0], SURFACE_ID_TEXT_BOX);
		return;
	}

	int iValue  = iScore;
	int iMod    = 1;
	int iOffset = 0;

	while (iValue / iMod > 0)
		iMod *= 10;

	iMod /= 10;

	while (iMod != 0)
	{
		PutBitmap3(&grcGame, PixelToScreenCoord(iX), PixelToScreenCoord(iY), &RC_NUMBERS[((iValue / iMod) % 10)], SURFACE_ID_TEXT_BOX);
		iX += RECT_WIDTH(RC_NUMBERS[(iValue / iMod) % 10]) + 1;

		++iOffset;
		iMod /= 10;
	}
}

void CTP_DrawTeamIndicator(CaveNet::DataStructures::NetTeam* pTeam, int iX, int iY, int iScoreX, int iScoreY)
{
	if (!pTeam || !pTeam->mFlagNpc)
		return;

	// Draw the background
	ModBitmap(SURFACE_ID_TEXT_BOX, CSM_RGB(unsigned char(pTeam->GetColor().r * 0.5f), unsigned char(pTeam->GetColor().g * 0.5f), unsigned char(pTeam->GetColor().b * 0.5f)));
	PutBitmap3(&grcGame, PixelToScreenCoord(iX), PixelToScreenCoord(iY), &RC_INDICATORS[0], SURFACE_ID_TEXT_BOX);

	// Draw team score
	ModBitmap(SURFACE_ID_TEXT_BOX, CSM_RGB(unsigned char(pTeam->GetColor().r), unsigned char(pTeam->GetColor().g), unsigned char(pTeam->GetColor().b)));
	CTP_PutTeamScore(pTeam->mConfig.score, iScoreX, iScoreY, true);
	ClearBitmapMod(SURFACE_ID_TEXT_BOX);

	int iCenterX = iX + (RECT_WIDTH(RC_INDICATORS[0]) / 2);
	int iCenterY = iY + (RECT_HEIGHT(RC_INDICATORS[0]) / 2);

	// Draw the icon on the indicator
	switch (pTeam->mFlagNpc->act_no)
	{
		case 5:
		case 6:
		{
			// Idle, waiting at its base to be picked up

		DRAW_PUPPY:
			// Draw the puppy
			PutBitmap3(&grcGame, PixelToScreenCoord(iX), PixelToScreenCoord(iY), &RC_INDICATORS[2], SURFACE_ID_TEXT_BOX);

			// Draw color mask
			PutBitmap8(&grcGame, PixelToScreenCoord(iX), PixelToScreenCoord(iY), &RC_INDICATORS[3], SURFACE_ID_TEXT_BOX, pTeam->GetColor().r, pTeam->GetColor().g, pTeam->GetColor().b);

			break;
		}
		case 0:
		case 1:
		case 2:
		case 3:
		case 7:
		{
			// Waiting to be picked up / expire
			int iTimeLeft = ((pTeam->mFlagNpc->count2 - CaveNet_Client_GetGlobalTimerOffset()) / 1000) + 1;

			if (iTimeLeft > 99)
				iTimeLeft = 99;
			
			ModBitmap(SURFACE_ID_TEXT_BOX, CSM_RGB(255, 0, 0));
			CTP_PutTeamScore(iTimeLeft, iCenterX, iCenterY - 4, true);
			ClearBitmapMod(SURFACE_ID_TEXT_BOX);

			break;
		}
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		{
			// Going to back to own base
			if ((CaveNet_Client_GetGlobalTimerOffset() % 1000) >= 500)
				goto DRAW_PUPPY;

			break;
		}
		case 4:
		{
			// Being carried!
			if ((CaveNet_Client_GetGlobalTimerOffset() % 1000) >= 500)
				PutBitmap3(&grcGame, PixelToScreenCoord(iX), PixelToScreenCoord(iY), &RC_INDICATORS[1], SURFACE_ID_TEXT_BOX);

			break;
		}
	}
}

void CTP_DrawGameScoreboard()
{
	// Do not execute if there are no teams
	if (!CaveNet::DataStructures::NetTeam::client_first)
		return;

	int iTotalWidth = 0;
	int iIndex      = -1;
	for (CaveNet::DataStructures::NetTeam* walk = CaveNet::DataStructures::NetTeam::client_first; walk; walk = walk->next)
	{
		// Skip teams that have no flag
		if (!walk->mFlagNpc)
			continue;

		// Increment the index
		++iIndex;

		// And add the width
		iTotalWidth += RECT_HEIGHT(RC_TEAM_FLAG_CIRCLE_ARROW) + (iIndex == 0 ? 0 : 2);
	}

	// Calculate the draw positions
	int iDrawX = (WINDOW_WIDTH / 2) - (iTotalWidth / 2);
	int iDrawY = WINDOW_HEIGHT - 8 - 2 - RECT_HEIGHT(RC_TEAM_FLAG_CIRCLE_ARROW);

	// And then loop through the teams
	for (CaveNet::DataStructures::NetTeam* walk = CaveNet::DataStructures::NetTeam::client_first; walk; walk = walk->next)
	{
		if (!walk->mFlagNpc)
			continue;

		int iIndicatorX = iDrawX + ((RECT_WIDTH(RC_TEAM_FLAG_CIRCLE_ARROW) / 2) - (RECT_WIDTH(RC_TEAM_FLAG_CIRCLE) / 2));
		int iIndicatorY = iDrawY + ((RECT_HEIGHT(RC_TEAM_FLAG_CIRCLE_ARROW) / 2) - (RECT_HEIGHT(RC_TEAM_FLAG_CIRCLE) / 2));

		// Get the flag's draw position
		int iSubDrawX = gFrame.x + ((iIndicatorX + (RECT_WIDTH(RC_TEAM_FLAG_CIRCLE_ARROW) / 2)) * 0x200);
		int iSubDrawY = gFrame.y + ((iIndicatorY + (RECT_HEIGHT(RC_TEAM_FLAG_CIRCLE_ARROW) / 2)) * 0x200);

		float fAngle = (!walk->mFlagNpc ? 0.f : atan2f(
							(SubpixelToScreenCoord(walk->mFlagNpc->x + 0x1000) - SubpixelToScreenCoord(gFrame.x)) - (SubpixelToScreenCoord(iSubDrawX) - SubpixelToScreenCoord(gFrame.x)),
							(SubpixelToScreenCoord(iSubDrawY) - SubpixelToScreenCoord(gFrame.y)) - (SubpixelToScreenCoord(walk->mFlagNpc->y + 0x1000) - SubpixelToScreenCoord(gFrame.y))
						) * (180.0 / 3.141592653589793238463));

		// Draw the arrow shadow
		if (walk->mFlagNpc && ((walk->mFlagNpc->bits & NPC_flags::npc_option3) || walk->mFlagNpc->act_no == 4))
		{
			PutBitmap16(
				&grcGame,
				PixelToScreenCoord(iDrawX + 1),
				PixelToScreenCoord(iDrawY + 1),
				&RC_TEAM_FLAG_CIRCLE_ARROW_SHADOW,
				SURFACE_ID_TEXT_BOX,
				fAngle,
				PixelToScreenCoord(RECT_WIDTH(RC_TEAM_FLAG_CIRCLE_ARROW) / 2),
				PixelToScreenCoord(RECT_HEIGHT(RC_TEAM_FLAG_CIRCLE_ARROW) / 2)
			);
		}

		// Draw the indicator
		PutBitmap3(&grcGame, PixelToScreenCoord(iIndicatorX), PixelToScreenCoord(iIndicatorY), &RC_TEAM_FLAG_CIRCLE, SURFACE_ID_TEXT_BOX);

		// Draw the fill color
		PutBitmap8(&grcGame, PixelToScreenCoord(iIndicatorX + 1), PixelToScreenCoord(iIndicatorY + 1), &RC_TEAM_FLAG_CIRCLE_MASK, SURFACE_ID_TEXT_BOX, walk->GetColor().r, walk->GetColor().g, walk->GetColor().b);

		// Draw the background color
		PutBitmap8(&grcGame, PixelToScreenCoord(iIndicatorX + 3), PixelToScreenCoord(iIndicatorY + 3), &RC_INDICATORS[0], SURFACE_ID_TEXT_BOX, walk->GetColor().r * 0.5f, walk->GetColor().g * 0.5f, walk->GetColor().b * 0.5f);

		// Draw the actual arrow
		if (walk->mFlagNpc && ((walk->mFlagNpc->bits & NPC_flags::npc_option3) || walk->mFlagNpc->act_no == 4))
		{
			PutBitmap16(
				&grcGame,
				PixelToScreenCoord(iDrawX),
				PixelToScreenCoord(iDrawY),
				&RC_TEAM_FLAG_CIRCLE_ARROW,
				SURFACE_ID_TEXT_BOX,
				fAngle,
				PixelToScreenCoord(RECT_WIDTH(RC_TEAM_FLAG_CIRCLE_ARROW) / 2),
				PixelToScreenCoord(RECT_HEIGHT(RC_TEAM_FLAG_CIRCLE_ARROW) / 2)
			);
		}

		// Draw this team
		CTP_DrawTeamIndicator(walk, iIndicatorX + 3, iIndicatorY + 3, iDrawX + RECT_WIDTH2(RC_TEAM_FLAG_CIRCLE_ARROW), iDrawY + RECT_HEIGHT(RC_TEAM_FLAG_CIRCLE_ARROW) - 2);

		iDrawX += RECT_HEIGHT(RC_TEAM_FLAG_CIRCLE_ARROW) + 2;
	}
}

//--------------------------------------------

static RECT gBoosterBarRc      = { 112, 111, 152, 117 };
static int gBoosterAmount      = 0;
static int gBoosterAmountStart = 0;
static int gBoosterLastAmount  = -1;
static int gBoosterTimer       = 0;

void CTP_OnDrawGameHud(BOOL bFlash)
{
	if (!CaveNet_Client_IsConnected())
		return;

	CTP_DrawGameScoreboard();
	
	// Draw the booster bar
	if ((gMC.equip & GMC_equipflags::gmc_eflag_booster_08) || (gMC.equip & GMC_equipflags::gmc_eflag_booster_2))
	{
		RECT gBoosterFullRc = { 113, 117, 150, 120 };

		// Set amount
		int iAmount = (gBoosterFullRc.right - gBoosterFullRc.left) * gMC.boost_cnt / gMC.boost_max;
		if (iAmount > gBoosterAmount)
		{
			if (gBoosterLastAmount != iAmount)
			{
				//set a new timer
				if (!gBoosterTimer)
					gBoosterTimer = GetTicks();

				gBoosterLastAmount  = iAmount;
				gBoosterAmountStart = gBoosterAmount;
			}

			if (GetTicks() - gBoosterTimer >= 100)
			{
				// Timer is over
				gBoosterTimer      = 0;
				gBoosterLastAmount = -1;
				gBoosterAmount     = iAmount;
			}
			else
			{
				// Timer is running
				gBoosterAmount = MOVE_LINEAR(gBoosterTimer, gBoosterAmountStart, iAmount, 100);
			}
		}
		else
		{
			gBoosterAmount     = iAmount;
			gBoosterTimer      = 0;
			gBoosterLastAmount = -1;
			gBoosterAmount     = iAmount;
		}

		gBoosterFullRc.right = gBoosterFullRc.left + gBoosterAmount;

		// Put bar background
		PutBitmap3(&grcGame, PixelToScreenCoord(381), PixelToScreenCoord(230), &gBoosterBarRc, SURFACE_ID_TEXT_BOX);

		// Put the bar itself
		PutBitmap3(&grcGame, PixelToScreenCoord(382), PixelToScreenCoord(231), &gBoosterFullRc, SURFACE_ID_TEXT_BOX);
	}

	// Initialize the guide arrow
	if (!bGuideArrowInit)
		InitGuideArrow();
	
	// Point towards objective
	if (gCollectableCount)
	{
		if (!bGuideArrowNpcSet)
		{
			CaveNet::DataStructures::NetClient* pOurClient = (CaveNet_Server_IsHosting() ? CaveNet_Server_ClientGroup_GetClientByGhostId(CaveNet_Client_GetGhostId()) : CaveNet_Client_ClientGroup_GetClientByGhostId(CaveNet_Client_GetGhostId()));
			CaveNet::DataStructures::NetTeam* pOurTeam     = (pOurClient ? pOurClient->GetTeam() : NULL);
			bGuideArrowNpcSet                              = true;
			pGuideArrow.pNpc                               = (!pOurTeam ? &gNPC[0] : pOurTeam->mFlagNpc);
		}

		// Draw the arrow that shows you where to go
		ActNpc413(&pGuideArrow);

		// Draw it
		PutSpecificNpChar(&pGuideArrow, gFrame.x, gFrame.y, false);
	}
	else
		bGuideArrowNpcSet = false;
}

//--------------------------------------------