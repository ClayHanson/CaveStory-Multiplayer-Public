#pragma once

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_BaseModInterface.h>

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class DataModInterface : public BaseModInterface
{
public: // Mod stuff

	virtual int OnInit();
	virtual void OnShutdown();

public: // Config

	virtual bool IsMultiplayer() { return true; }
	virtual bool DisableEveryoneDiedEvent() { return true; }
	virtual bool IsSingleplayer() { return false; }
	virtual bool HasIntroSequence() { return false; }
	virtual bool IsRevivingPlayersAllowed() { return false; }
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------