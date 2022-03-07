/*
  Cavestory Multiplayer API
  Copyright (C) 2021 Johnny Ledger

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
	 claim that you wrote the original software. If you use this software
	 in a product, an acknowledgment in the product documentation would be
	 appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
	 misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*!
  @file CSMAPI_BaseModInterface.h
*/

#pragma once

#include <CSMAPI_begincode.h>

/// @brief The base mod interface.
/// @ingroup GUI
///
/// This class MUST be defined in a child class for every mod.
/// It provides useful callbacks for mods.
class CAVESTORY_MOD_API BaseModInterface
{
public: // Private stuff!

	/// The current mod interface
	///
	static BaseModInterface* gsModInterface;

public: // C++ garbage

	/// Default constructor
	///
	/// @note This does NOT call OnInit().
	BaseModInterface();

	/// Default deconstructor
	///
	/// @note This does NOT call OnShutdown().
	~BaseModInterface();

public: // Mod functions

	/// @brief Called when the mod is initialized.
	///
	/// @return Should return 0 on success, non-zero on error.
	virtual int OnInit() { return 0; }
	
	/// @brief Called when the mod is exiting.
	///
	virtual void OnShutdown() {}

	/// @brief Called when the titlescreen is initialized.
	///
	/// @param bImmediateReturn Set this to true to have the function who called this immediately return with this function's return value. Useful for custom titlescreens.
	///
	/// @return Should return the next mode.
	virtual int OnEnterTitleScreen(bool& bImmediateReturn, unsigned char& pAction) { return 0; }

	/// @brief Defines whether this mod supports multiplayer or not.
	///
	/// @return Returns true if this mod supports multiplayer, false if not.
	virtual bool IsMultiplayer() { return false; }
	
	/// @brief Defines whether this mod supports singleplayer or not.
	///
	/// @return Returns true if this mod supports singleplayer, false if not.
	virtual bool IsSingleplayer() { return true; }
	
	/// @brief Define whether this mod has an intro sequence or not.
	///
	/// @return Returns true if this mod has an intro sequence, false if not.
	virtual bool HasIntroSequence() { return true; }
	
	/// @brief Define whether this mod allows reviving players.
	///
	/// @return Returns true if this mod allows reviving players.
	virtual bool IsRevivingPlayersAllowed() { return true; }
	
	/// @brief Defines whether this mod should force the 'TestEveryoneDied()' to always return 'false'.
	///
	/// @return Should return 'true' if 'TestEveryoneDied()' should be forced to always return 'false'.
	virtual bool DisableEveryoneDiedEvent() { return false; }
};

#include <CSMAPI_endcode.h>