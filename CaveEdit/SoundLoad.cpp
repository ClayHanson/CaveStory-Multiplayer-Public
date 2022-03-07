#include "stdafx.h"

#include <SDL.h>
#include <stdio.h>

#include "SoundLoad.h"
#include "PixTone.h"
#include "Sound.h"
#include "Organya.h"
#include "General.h"
#include "SoundHelper.h"

enum
{
	SOUND_TYPE_PIXTONE,
	SOUND_TYPE_OTHER
};

static const struct
{
	int slot;
	const char* path;
	int type;
} gPtpTable[] = {
	// TODO - Name the files
	{1, "PixTone\\001.pxt", SOUND_TYPE_PIXTONE},
	{2, "PixTone\\002.pxt", SOUND_TYPE_PIXTONE},
	{3, "PixTone\\003.pxt", SOUND_TYPE_PIXTONE},
	{4, "PixTone\\004.pxt", SOUND_TYPE_PIXTONE},
	{5, "PixTone\\005.pxt", SOUND_TYPE_PIXTONE},
	{6, "PixTone\\006.pxt", SOUND_TYPE_PIXTONE},
	{7, "PixTone\\007.pxt", SOUND_TYPE_PIXTONE},
	{11, "PixTone\\011.pxt", SOUND_TYPE_PIXTONE},
	{12, "PixTone\\012.pxt", SOUND_TYPE_PIXTONE},
	{14, "PixTone\\014.pxt", SOUND_TYPE_PIXTONE},
	{15, "PixTone\\015.pxt", SOUND_TYPE_PIXTONE},
	{16, "PixTone\\016.pxt", SOUND_TYPE_PIXTONE},
	{17, "PixTone\\017.pxt", SOUND_TYPE_PIXTONE},
	{18, "PixTone\\018.pxt", SOUND_TYPE_PIXTONE},
	{20, "PixTone\\020.pxt", SOUND_TYPE_PIXTONE},
	{21, "PixTone\\021.pxt", SOUND_TYPE_PIXTONE},
	{22, "PixTone\\022.pxt", SOUND_TYPE_PIXTONE},
	{23, "PixTone\\023.pxt", SOUND_TYPE_PIXTONE},
	{24, "PixTone\\024.pxt", SOUND_TYPE_PIXTONE},
	{25, "PixTone\\025.pxt", SOUND_TYPE_PIXTONE},
	{26, "PixTone\\026.pxt", SOUND_TYPE_PIXTONE},
	{27, "PixTone\\027.pxt", SOUND_TYPE_PIXTONE},
	{28, "PixTone\\028.pxt", SOUND_TYPE_PIXTONE},
	{29, "PixTone\\029.pxt", SOUND_TYPE_PIXTONE},
	{30, "PixTone\\030.pxt", SOUND_TYPE_PIXTONE},
	{31, "PixTone\\031.pxt", SOUND_TYPE_PIXTONE},
	{32, "PixTone\\032.pxt", SOUND_TYPE_PIXTONE},
	{33, "PixTone\\033.pxt", SOUND_TYPE_PIXTONE},
	{34, "PixTone\\034.pxt", SOUND_TYPE_PIXTONE},
	{35, "PixTone\\035.pxt", SOUND_TYPE_PIXTONE},
	{37, "PixTone\\037.pxt", SOUND_TYPE_PIXTONE},
	{38, "PixTone\\038.pxt", SOUND_TYPE_PIXTONE},
	{39, "PixTone\\039.pxt", SOUND_TYPE_PIXTONE},
	{40, "PixTone\\040.pxt", SOUND_TYPE_PIXTONE},
	{41, "PixTone\\041.pxt", SOUND_TYPE_PIXTONE},
	{42, "PixTone\\042.pxt", SOUND_TYPE_PIXTONE},
	{43, "PixTone\\043.pxt", SOUND_TYPE_PIXTONE},
	{44, "PixTone\\044.pxt", SOUND_TYPE_PIXTONE},
	{45, "PixTone\\045.pxt", SOUND_TYPE_PIXTONE},
	{46, "PixTone\\046.pxt", SOUND_TYPE_PIXTONE},
	{47, "PixTone\\047.pxt", SOUND_TYPE_PIXTONE},
	{48, "PixTone\\048.pxt", SOUND_TYPE_PIXTONE},
	{49, "PixTone\\049.pxt", SOUND_TYPE_PIXTONE},
	{50, "PixTone\\050.pxt", SOUND_TYPE_PIXTONE},
	{51, "PixTone\\051.pxt", SOUND_TYPE_PIXTONE},
	{52, "PixTone\\052.pxt", SOUND_TYPE_PIXTONE},
	{53, "PixTone\\053.pxt", SOUND_TYPE_PIXTONE},
	{54, "PixTone\\054.pxt", SOUND_TYPE_PIXTONE},
	{55, "PixTone\\055.pxt", SOUND_TYPE_PIXTONE},
	{56, "PixTone\\056.pxt", SOUND_TYPE_PIXTONE},
	{57, "PixTone\\057.pxt", SOUND_TYPE_PIXTONE},
	{58, "PixTone\\058.pxt", SOUND_TYPE_PIXTONE},
	{59, "PixTone\\059.pxt", SOUND_TYPE_PIXTONE},
	{60, "PixTone\\060.pxt", SOUND_TYPE_PIXTONE},
	{61, "PixTone\\061.pxt", SOUND_TYPE_PIXTONE},
	{62, "PixTone\\062.pxt", SOUND_TYPE_PIXTONE},
	{63, "PixTone\\063.pxt", SOUND_TYPE_PIXTONE},
	{64, "PixTone\\064.pxt", SOUND_TYPE_PIXTONE},
	{65, "PixTone\\065.pxt", SOUND_TYPE_PIXTONE},
	{70, "PixTone\\070.pxt", SOUND_TYPE_PIXTONE},
	{71, "PixTone\\071.pxt", SOUND_TYPE_PIXTONE},
	{72, "PixTone\\072.pxt", SOUND_TYPE_PIXTONE},
	{100, "PixTone\\100.pxt", SOUND_TYPE_PIXTONE},
	{101, "PixTone\\101.pxt", SOUND_TYPE_PIXTONE},
	{102, "PixTone\\102.pxt", SOUND_TYPE_PIXTONE},
	{103, "PixTone\\103.pxt", SOUND_TYPE_PIXTONE},
	{104, "PixTone\\104.pxt", SOUND_TYPE_PIXTONE},
	{105, "PixTone\\105.pxt", SOUND_TYPE_PIXTONE},
	{106, "PixTone\\106.pxt", SOUND_TYPE_PIXTONE},
	{107, "PixTone\\107.pxt", SOUND_TYPE_PIXTONE},
	{108, "PixTone\\108.pxt", SOUND_TYPE_PIXTONE},
	{109, "PixTone\\109.pxt", SOUND_TYPE_PIXTONE},
	{110, "PixTone\\110.pxt", SOUND_TYPE_PIXTONE},
	{111, "PixTone\\111.pxt", SOUND_TYPE_PIXTONE},
	{112, "PixTone\\112.pxt", SOUND_TYPE_PIXTONE},
	{113, "PixTone\\113.pxt", SOUND_TYPE_PIXTONE},
	{114, "PixTone\\114.pxt", SOUND_TYPE_PIXTONE},
	{115, "PixTone\\115.pxt", SOUND_TYPE_PIXTONE},
	{116, "PixTone\\116.pxt", SOUND_TYPE_PIXTONE},
	{117, "PixTone\\117.pxt", SOUND_TYPE_PIXTONE},
	{150, "PixTone\\150.pxt", SOUND_TYPE_PIXTONE},
	{151, "PixTone\\151.pxt", SOUND_TYPE_PIXTONE},
	{152, "PixTone\\152.pxt", SOUND_TYPE_PIXTONE},
	{153, "PixTone\\153.pxt", SOUND_TYPE_PIXTONE},
	{154, "PixTone\\154.pxt", SOUND_TYPE_PIXTONE},
	{155, "PixTone\\155.pxt", SOUND_TYPE_PIXTONE}
};

int gPtpTableCount = 0;

static BOOL CheckFileExists(const char* pFile)
{
	FILE* fp;
	if ((fp = fopen(pFile, "rb")) == NULL)
		return FALSE;

	fclose(fp);
	return TRUE;
}

void PlayOrganyaTrack(int index)
{
	if (index < 0 || index >= exe.mod.mMusic.count)
		return;

	const char* pFile;
	if (
		CheckFileExists(pFile = avar("%s/Music/%s.wav", exe.mod.mModPath, exe.mod.mMusic.list[index].name)) ||
		CheckFileExists(pFile = avar("%s/Music/%s.mp3", exe.mod.mModPath, exe.mod.mMusic.list[index].name)) ||
		CheckFileExists(pFile = avar("%s/Music/%s.ogg", exe.mod.mModPath, exe.mod.mMusic.list[index].name))
		)
	{
		StopOrganyaMusic();
		SoundHelper::AdvancedSoundObject::PlayFile(pFile, TRUE);
		return;
	}

	char buffer[1024];
	sprintf(buffer, "%s/Music/%s.org", exe.mod.mModPath, exe.mod.mMusic.list[index].name);

	LoadOrganya(buffer);
	PlayOrganyaMusic();
}

static bool LoadPixToneFile(const char* filename, PIXTONEPARAMETER* pixtone_parameters)
{
	bool success = false;

	FILE* fp = fopen(filename, "r");

	if (fp)
	{
		for (unsigned int i = 0; i < 4; ++i)
		{
			float freq;
			fscanf(fp, "use  :%d\n", &pixtone_parameters[i].use);
			fscanf(fp, "size :%d\n", &pixtone_parameters[i].size);
			fscanf(fp, "main_model   :%d\n", &pixtone_parameters[i].oMain.model);
			fscanf(fp, "main_freq    :%f\n", &freq);
			pixtone_parameters[i].oMain.num = freq;
			fscanf(fp, "main_top     :%d\n", &pixtone_parameters[i].oMain.top);
			fscanf(fp, "main_offset  :%d\n", &pixtone_parameters[i].oMain.offset);
			fscanf(fp, "pitch_model  :%d\n", &pixtone_parameters[i].oPitch.model);
			fscanf(fp, "pitch_freq   :%f\n", &freq);
			pixtone_parameters[i].oPitch.num = freq;
			fscanf(fp, "pitch_top    :%d\n", &pixtone_parameters[i].oPitch.top);
			fscanf(fp, "pitch_offset :%d\n", &pixtone_parameters[i].oPitch.offset);
			fscanf(fp, "volume_model :%d\n", &pixtone_parameters[i].oVolume.model);
			fscanf(fp, "volume_freq  :%f\n", &freq);
			pixtone_parameters[i].oVolume.num = freq;
			fscanf(fp, "volume_top   :%d\n", &pixtone_parameters[i].oVolume.top);
			fscanf(fp, "volume_offset:%d\n", &pixtone_parameters[i].oVolume.offset);
			fscanf(fp, "initialY:%d\n", &pixtone_parameters[i].initial);
			fscanf(fp, "ax      :%d\n", &pixtone_parameters[i].pointAx);
			fscanf(fp, "ay      :%d\n", &pixtone_parameters[i].pointAy);
			fscanf(fp, "bx      :%d\n", &pixtone_parameters[i].pointBx);
			fscanf(fp, "by      :%d\n", &pixtone_parameters[i].pointBy);
			fscanf(fp, "cx      :%d\n", &pixtone_parameters[i].pointCx);
			fscanf(fp, "cy      :%d\n\n", &pixtone_parameters[i].pointCy);
		}

		fclose(fp);

		success = true;
	}

	return success;
}

bool isSoundSystemEnabled = false;
bool InitSoundSubsystem(const char* filePath)
{
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER) > 0)
	{
		printf("SDL INIT ERROR: %s\r\n", SDL_GetError());
		return false;
	}

	if (!InitDirectSound(filePath))
	{
		printf("DIRECT SOUND ERROR: %s\r\n", SDL_GetError());
		SDL_Quit();
		return false;
	}

	if (isSoundSystemEnabled)
		return true;

	gPtpTableCount       = sizeof(gPtpTable) / sizeof(gPtpTable[0]);
	isSoundSystemEnabled = true;
	int pt_size          = 0;
	for (unsigned int i = 0; i < sizeof(gPtpTable) / sizeof(gPtpTable[0]); ++i)
	{
		char path[160];
		sprintf(path, "%s/%s", filePath, gPtpTable[i].path);

		switch (gPtpTable[i].type)
		{
			case SOUND_TYPE_PIXTONE:
			{
				PIXTONEPARAMETER pixtone_parameters[4];

				if (LoadPixToneFile(path, pixtone_parameters))
				{
					int ptp_num = 0;
					while (pixtone_parameters[ptp_num].use && ptp_num < 4)
						++ptp_num;

					pt_size += MakePixToneObject(pixtone_parameters, ptp_num, gPtpTable[i].slot);
				}

				break;
			}
		}
	}

	printf("PixTone size: %04d\r\n", pt_size);

	return true;
}

void ExitSoundSubsystem()
{
	if (!isSoundSystemEnabled)
		return;

	isSoundSystemEnabled = false;
	EndDirectSound();
	SDL_Quit();
}