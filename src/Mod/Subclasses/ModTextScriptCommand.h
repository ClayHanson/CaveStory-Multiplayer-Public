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
  @file ModTextScriptCommand.h
*/

#include <CavestoryModAPI.h>

#pragma once

/// @addtogroup TextScript
/// @{
///

/// @brief Convert three characters to a unique identifier for textscript commands.
///
/// @param A The first character.
/// @param B The second character.
/// @param C The third character.
#define TSC_NAME_TO_HEX(A,B,C) (int(A) | (int(B) << 8) | (int(C) << 16))

/// Automatically sets up every textscript command in the mod after calling ModTextScriptCommand::Init().
class ModTextScriptCommand
{
public: // Typedefs

	/// @brief Run a TextScriptCommand object's code.
	///
	/// @param x X value.
	/// @param y Y value.
	/// @param z Z value.
	/// @param w W value.
	/// @param iError The error code. Don't mess with it if there is no error.
	typedef void(*TSCRunFunc)(int& x, int& y, int& z, int& w, bool& bExit, int& iError);

public: // Linking

	/// The first command in the linkage.
	/// @private
	static ModTextScriptCommand* first;

	/// The next command in the linkage.
	/// @private
	ModTextScriptCommand* next;

protected: // Variables

	/// Execute function
	///
	TSCRunFunc mExecute;

public: // Human information

	/// This command's identifier
	///
	char mCommand[4];

	/// This command's shorthand descriptor.
	///
	char* mShortDesc;

	/// This command's descriptor.
	///
	char* mDesc;

	/// This command's arguments.
	///
	char* mArguments;

public: // Constructors

	/// @brief Blank constructor, for sorted bin list.
	///
	ModTextScriptCommand() {}

	/// @brief Constructor.
	///
	ModTextScriptCommand(TSCRunFunc pCommand, char pFirst, char pSecond, char pThird, const char* pShortDesc, const char* pDesc, const char* pArgs);

	/// @brief Deconstructor.
	///
	~ModTextScriptCommand();

public: // Public, static methods

	/// @brief Initialize the text script commands.
	///
	static void Init();
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

inline ModTextScriptCommand::ModTextScriptCommand(TSCRunFunc pCommand, char pFirst, char pSecond, char pThird, const char* pShortDesc, const char* pDesc, const char* pArgs)
{
	mCommand[0] = pFirst;
	mCommand[1] = pSecond;
	mCommand[2] = pThird;
	mCommand[3] = 0;
	mShortDesc  = strdup(pShortDesc);
	mDesc       = strdup(pDesc);
	mArguments  = strdup(pArgs);
	mExecute    = pCommand;

	// Link us up
	next  = first;
	first = this;
}

inline ModTextScriptCommand::~ModTextScriptCommand()
{
	if (mShortDesc)
		free(mShortDesc);
	
	if (mDesc)
		free(mDesc);
	
	if (mArguments)
		free(mArguments);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

inline void ModTextScriptCommand::Init()
{
	// Populate the lists
	for (ModTextScriptCommand* walk = ModTextScriptCommand::first; walk; walk = walk->next)
	{
		if (CSM_RegisterTextScriptCommand(walk->mCommand, walk->mShortDesc, walk->mDesc, walk->mArguments, walk->mExecute) == FALSE)
		{
			CSM_Log("ERROR: Failed to register TextScript command <");
			CSM_Log("%.3s!\r\n", walk->mCommand);
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _TEXT_SCRIPT_FILE_
/// @brief Define a textscript command
///
/// @param NAMEA The first character in the command's name.
/// @param NAMEB The second character in the command's name.
/// @param NAMEC The third character in the command's name.
/// @param SHORTHAND_DESC The short hand description.
/// @param DESC The complete description.
/// @param ARGS The argument specifications for this command.
#define DEFINE_TEXTSCRIPT_COMMAND(NAMEA, NAMEB, NAMEC, SHORTHAND_DESC, DESC, ARGS) \
	extern void __declspec(dllexport) __##NAMEA##NAMEB##NAMEC##__RUNFUNC__(int& x, int& y, int& z, int& w, bool& bExit, int& iError);\
	static ModTextScriptCommand __CMD_##NAMEA##NAMEB##NAMEC##(&__##NAMEA##NAMEB##NAMEC##__RUNFUNC__, #@NAMEA, #@NAMEB, #@NAMEC, SHORTHAND_DESC, DESC, ARGS);\
	void __##NAMEA##NAMEB##NAMEC##__RUNFUNC__(int& x, int& y, int& z, int& w, bool& bExit, int& iError)

#define TGetMyChar				(*gCurrentPtrData.Char)
#define TGetGameFlags			(*gCurrentPtrData.GameFlags)
#define TGetPlayer				gCurrentPtrData.Player
#define TGetClient				gCurrentPtrData.Client
#define TGetKey					(*gCurrentPtrData.Key)
#define TGetKeyTrg				(*gCurrentPtrData.KeyTrg)
#endif

/// @}
///