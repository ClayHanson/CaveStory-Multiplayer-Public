#pragma once

extern int gPtpTableCount;

bool IsOrganyaPlaying();
void PlayOrganyaTrack(int index);
bool InitSoundSubsystem(const char* filePath);
void ExitSoundSubsystem();