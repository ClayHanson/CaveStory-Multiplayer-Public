#include "stdafx.h"
#include "SoundHelper.h"
#include "Sound.h"
#include "Organya.h"
#include "General.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

#ifndef CLAMP
#define CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
#endif

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const unsigned int g_SZ_Res_StopThenPlay = SND_APPLICATION | SND_ASYNC | SND_RESOURCE | SND_NOSTOP;
static const unsigned int g_SZ_Res_Play         = SND_APPLICATION | SND_ASYNC | SND_RESOURCE;

void SoundHelper::PlaySoundObject(SoundType iType, PlayMode iMode, const char* pSoundFileName, int iSoundId)
{
	switch (iType)
	{
		case SoundHelper::ST_WAVE_RESOURCE:
		{
			if (iMode == PlayMode::PM_STOP)
				PlaySoundA(NULL, NULL, 0);
			else
				PlaySoundA(pSoundFileName, GetModuleHandle(NULL), SND_APPLICATION | SND_ASYNC | SND_RESOURCE | (iMode == PlayMode::PM_LOOP ? SND_LOOP : 0) | (iMode == PlayMode::PM_STOP_THEN_PLAY ? 0 : SND_NOSTOP));

			break;
		}
		case SoundHelper::ST_WAVE_FILE:
		{
			if (iMode == PlayMode::PM_STOP)
				PlaySoundA(NULL, NULL, 0);
			else
				PlaySoundA(pSoundFileName, NULL, SND_NODEFAULT | SND_FILENAME | SND_ASYNC | (iMode == PlayMode::PM_LOOP ? SND_LOOP : 0) | (iMode == PlayMode::PM_STOP_THEN_PLAY ? 0 : SND_NOSTOP));

			break;
		}
		case SoundHelper::ST_ORGANYA:
		{
			if (iMode == PlayMode::PM_STOP)
				StopOrganyaMusic();
			else
			{
				char buffer[1024];
				sprintf(buffer, "%s/Music/%s.org", exe.mod.mModPath, exe.mod.mMusic.list[CLAMP(iSoundId, 0, exe.mod.mMusic.count - 1)].name);

				LoadOrganya(buffer);
				PlayOrganyaMusic();
			}
			
			break;
		}
		case SoundHelper::ST_PXTONE:
		{
			::PlaySoundObject(iSoundId, (iMode == PlayMode::PM_STOP ? 0 : (iMode == PlayMode::PM_STOP_THEN_PLAY ? 1 : (iMode == PlayMode::PM_LOOP ? -1 : 2))));

			break;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------