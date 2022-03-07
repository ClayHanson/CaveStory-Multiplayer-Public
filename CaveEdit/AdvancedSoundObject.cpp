#include "stdafx.h"
#include "SoundHelper.h"
#include <mciapi.h>
#include <mmsystem.h>
#include <Vfw.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Msvfw32.lib")

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

using namespace SoundHelper;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static BOOL CALLBACK ASO_EnumWindows(HWND hWnd, LPARAM lparam)
{
	HWND* pParentHwnd = (HWND*)lparam;

	// If not null, then we'll just skip everything
	if (pParentHwnd != NULL)
		return TRUE;

	// Check process ID
	if (GetWindowThreadProcessId(hWnd, NULL) == GetCurrentProcessId())
		*pParentHwnd = hWnd;

	return TRUE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AdvancedSoundObject* AdvancedSoundObject::first = NULL;

AdvancedSoundObject::AdvancedSoundObject()
{
	memset(&State, 0, sizeof(State));
	memset(&Handles, 0, sizeof(Handles));

	HWND pParentHwnd = NULL;

	// Find the parent window
	EnumWindows(ASO_EnumWindows, (LPARAM)&pParentHwnd);

	// Create the handle
	Handles.hMCI = MCIWndCreate(pParentHwnd, GetModuleHandle(NULL), MCIWNDF_NOPLAYBAR | MCIWNDF_NOOPEN | MCIWNDF_NOMENU | MCIWNDF_NOERRORDLG, NULL);
	
	// Link us up
	if (first)
		first->prev = this;

	prev  = NULL;
	next  = first;
	first = this;
}

AdvancedSoundObject::~AdvancedSoundObject()
{
	// Close ourselves
	Close();

	if (State.pFileName)
	{
		free(State.pFileName);
		State.pFileName = NULL;
	}

	MCIWndDestroy(Handles.hMCI);

	// Unlink ourselves
	if (first == this)
		first = next;

	if (next)
		next->prev = prev;
	
	if (prev)
		prev->next = next;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL AdvancedSoundObject::SetFile(const char* pFile)
{
	// Close it first
	if (State.pFileName)
	{
		Close();
		free(State.pFileName);
		State.pFileName = NULL;
	}

	// Setup memory
	memset(&State, 0, sizeof(State));
	State.pFileName = strdup(pFile);

	int iError = 0;
	// Open the media file
	if ((iError = MCIWndOpen(Handles.hMCI, pFile, 0)) != 0)
	{
		printf("SetFile() - Failed to set file to \"%s\" (Error # %d).\r\n", pFile, iError);
		free(State.pFileName);
		State.pFileName = NULL;
		return FALSE;
	}

	return TRUE;
}

void AdvancedSoundObject::Close()
{
	MCIWndClose(Handles.hMCI);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void AdvancedSoundObject::SetVolume(unsigned char iVolume)
{
	MCIWndSetVolume(Handles.hMCI, 1000 * (int)iVolume / 100);
}

void AdvancedSoundObject::Play(BOOL bLoop)
{
	if (!MCIWndCanPlay(Handles.hMCI))
		return;

	// Update the state
	State.bPlaying = true;
	State.bPaused  = false;

	// Execute the command
	MCIWndPlayFrom(Handles.hMCI, 0);
}

void AdvancedSoundObject::Stop()
{
	if (!MCIWndCanPlay(Handles.hMCI))
		return;

	// Update the state
	State.bPlaying = false;
	State.bPaused  = false;

	// Execute the command
	MCIWndStop(Handles.hMCI);
}

void AdvancedSoundObject::Resume()
{
	if (!MCIWndCanPlay(Handles.hMCI))
		return;

	// Update the state
	State.bPlaying = true;
	State.bPaused  = false;

	// Execute the command
	MCIWndResume(Handles.hMCI);
}

void AdvancedSoundObject::Pause()
{
	if (!MCIWndCanPlay(Handles.hMCI))
		return;

	// Update the state
	State.bPlaying = false;
	State.bPaused  = true;

	// Execute the command
	MCIWndPause(Handles.hMCI);
}

void AdvancedSoundObject::Rewind()
{
	if (!MCIWndCanPlay(Handles.hMCI))
		return;

	MCIWndHome(Handles.hMCI);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL AdvancedSoundObject::PlayFile(const char* pFile, BOOL bLoop)
{
	for (AdvancedSoundObject* walk = AdvancedSoundObject::first; walk; walk = walk->next)
	{
		// Skip mismatches
		if (!walk->State.pFileName || strcmpi(walk->State.pFileName, pFile))
			continue;

		walk->Play(bLoop);
		return TRUE;
	}

	AdvancedSoundObject* pNewSound = new AdvancedSoundObject();

	// Attempt to load the file
	if (!pNewSound->SetFile(pFile))
	{
		printf("PlayFile() - Failed to open \"%s\".\r\n", pFile);
		delete pNewSound;
		return FALSE;
	}

	pNewSound->Play(bLoop);

	return FALSE;
}

BOOL AdvancedSoundObject::StopFile(const char* pFile)
{
	for (AdvancedSoundObject* walk = AdvancedSoundObject::first; walk; walk = walk->next)
	{
		// Skip mismatches
		if (!walk->State.pFileName || strcmpi(walk->State.pFileName, pFile))
			continue;

		walk->Stop();
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------