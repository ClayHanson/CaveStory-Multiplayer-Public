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
  @file GameDemo.h
*/

#ifndef CAVESTORY_MOD_API_H_
#error "Please include CavestoryModAPI.h before including sub-classes."
#endif

#include <CSMAPI_begincode.h>
#pragma once

#include <Mod/SortedBinList.h>

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief The variable type for a GuiStyleVariable.
///
enum GuiStyleVarType : unsigned char
{
	/// Undefined.
	GSVT_UNDEFINED = 0,

	/// Color variable.
	GSVT_COLOR     = 1,

	/// String variable.
	GSVT_STRING    = 2,

	/// Integer variable.
	GSVT_INTEGER   = 3,
	
	/// Rect variable.
	GSVT_RECT      = 4,

	/// Point variable.
	GSVT_POINT     = 5
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief GuiStyles' color struct
///
struct GuiStyleColor
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

/// @brief GuiStyles' rect struct
///
struct GuiStyleRect
{
	int x;
	int y;
	int w;
	int h;
};

/// @brief GuiStyles' point struct
///
struct GuiStylePoint
{
	int x;
	int y;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief A sortable client class, to be used with SortedBinList.
///
class GuiStyleVariable
{
public: // Variables

	/// The key.
	///
	int m_NameHash;

	/// The type of variable this is.
	///
	GuiStyleVarType m_Type;

	/// The value for this variable.
	///
	union
	{
		GuiStyleColor color;
		GuiStylePoint point;
		GuiStyleRect rect;
		char* string;
		int integer;
	} m_Values;

public: // Constructor

	/// Constructor
	///
	GuiStyleVariable(int key);

	/// Main constructor
	///
	GuiStyleVariable();

	/// Deconstructor
	///
	~GuiStyleVariable();

public: // Operators

	/// @protected
	///
	inline bool operator< (const GuiStyleVariable& other) const { return m_NameHash < other.m_NameHash; }

	/// @protected
	///
	inline bool operator<=(const GuiStyleVariable& other) const { return m_NameHash <= other.m_NameHash; }

	/// @protected
	///
	inline bool operator> (const GuiStyleVariable& other) const { return m_NameHash > other.m_NameHash; }

	/// @protected
	///
	inline bool operator>=(const GuiStyleVariable& other) const { return m_NameHash >= other.m_NameHash; }

	/// @protected
	///
	inline bool operator!=(const GuiStyleVariable& other) const { return m_NameHash != other.m_NameHash; }

	/// @protected
	///
	inline bool operator==(const GuiStyleVariable& other) const { return m_NameHash == other.m_NameHash; }
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief 
class GuiStyleManager
{
private: // Variables

	/// A list of variables.
	///
	DynamicSortedBinList<GuiStyleVariable*, 1, true> m_VarList;

public:

	/// Default constructor
	///
	GuiStyleManager();

	/// Deconstructor
	///
	~GuiStyleManager();

public: // Variable methods

	/// @brief Get the string value of a variable.
	/// 
	/// @param pValueName The name of the variable.
	/// @param pDefaultValue The default value if the given value is not defined.
	///
	/// @return Returns the value.
	const char* GetStringVar(const char* pValueName, const char* pDefaultValue = "");

	/// @brief Get the integer value of a variable.
	/// 
	/// @param pValueName The name of the variable.
	/// @param pDefaultValue The default value if the given value is not defined.
	///
	/// @return Returns the value.
	int GetIntVar(const char* pValueName, int iDefaultValue = 0);

	/// @brief Get the integer value of a variable.
	/// 
	/// @param pValueName The name of the variable.
	/// @param pDefaultValue The default value if the given value is not defined.
	/// @param bForceOpqaue Force 255 alpha.
	///
	/// @return Returns the value.
	GUI_COLOR GetColorVar(const char* pValueName, GUI_COLOR pDefaultValue = GUI_COLOR(0, 0, 0, 0), bool bForceOpqaue = false);
	
	/// @brief Get the rect value of a variable.
	/// 
	/// @param pValueName The name of the variable.
	/// @param pDefaultValue The default value if the given value is not defined.
	///
	/// @return Returns the value.
	GUI_RECT GetRectVar(const char* pValueName, GUI_RECT pDefaultValue = GUI_RECT(0, 0, 0, 0));
	
	/// @brief Get the point value of a variable.
	/// 
	/// @param pValueName The name of the variable.
	/// @param pDefaultValue The default value if the given value is not defined.
	///
	/// @return Returns the value.
	GUI_POINT GetPointVar(const char* pValueName, GUI_POINT pDefaultValue = GUI_POINT(0, 0));

public: // Manipulation

	/// @brief Reset the manager.
	///
	void Reset();

	/// @brief Load the currently loaded mod's GuiStyle.cfg file.
	///
	/// @return Returns the error code.
	int LoadStyles(const char* pFileName);
};

extern GuiStyleManager gGuiStyleMgr;
#include <CSMAPI_endcode.h>