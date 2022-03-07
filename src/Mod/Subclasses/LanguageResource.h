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
  @file LanguageResource.h
*/

#ifndef CAVESTORY_MOD_API_H_
#error "Please include CavestoryModAPI.h before including sub-classes."
#endif

#include <CSMAPI_begincode.h>

#pragma once

/// @ingroup LanguageResources
/// @{
///

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define PXLANG_MAGIC		"PXLANG"
#define PXLANG_VERSION		1
#define PXLANG_VALID_CRC	0x1F5F1F44

// Enable to always recompile PXLANG. Useful when working on the compiler.
//#define ALWAYS_RECOMPILE_PXLANG

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define __VA_NARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...)													N
#define __EXPAND(x)																											x
#define __VA_NARGS(...)																										__EXPAND(__VA_NARGS_IMPL(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1))

// Localization Macros
#define USE_LOCALE_NAME_AS_DEFAULT																							NULL
#define CopyLocaleStringWithArgumentsA(BUFFER, LOCALE_NAME, ARG_FORMAT, DEFAULT_STRING, ...)								CSM_PxLang_GetString(LOCALE_NAME, BUFFER, DEFAULT_STRING, NULL, ARG_FORMAT, __VA_NARGS(__VA_ARGS__), ## __VA_ARGS__)
#define GetLocaleStringWithArgumentsA(LOCALE_NAME, ARG_FORMAT, DEFAULT_STRING, ...)											CSM_PxLang_GetString(LOCALE_NAME, NULL, DEFAULT_STRING, NULL, ARG_FORMAT, __VA_NARGS(__VA_ARGS__), ## __VA_ARGS__)
#define CopyLocaleStringA(BUFFER, LOCALE_NAME, DEFAULT_STRING)																CSM_PxLang_GetString(LOCALE_NAME, BUFFER, DEFAULT_STRING, NULL, NULL, 0)
#define GetLocaleStringA(LOCALE_NAME, DEFAULT_STRING)																		CSM_PxLang_GetString(LOCALE_NAME, NULL, DEFAULT_STRING, NULL, NULL, 0)
#define BuildLocaleString(STRING)																							CSM_PxLang_BuildFormat(STRING)
#define LoadNonLocaleFont(FONTNAME, FONTW, FONTH)																			(CSM_PxLang_IsLoaded(&gCurrentLocalization) && strcmpi(gCurrentLocalization.font_name, "Courier New") ? FontManager::LoadFont(NULL, (gCurrentLocalization.font_width / 1.1), (gCurrentLocalization.font_height / 1.1)) : FontManager::LoadFont(FONTNAME, FONTW, FONTH))
#define LoadNonLocaleFontWithSubstitue(FONTNAME, FONTNAME2, FONTW, FONTH)													(CSM_PxLang_IsLoaded(&gCurrentLocalization) && strcmpi(gCurrentLocalization.font_name, "Courier New") ? FontManager::LoadFont(FONTNAME2, (gCurrentLocalization.font_width / 1.1), (gCurrentLocalization.font_height / 1.1)) : FontManager::LoadFont(FONTNAME, FONTW, FONTH))

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// Argument types for compiled pxlang files
///
enum PXLANG_ArgType
{
	PXLANG_TYPE_STRING = 0,
	PXLANG_TYPE_INT    = 1
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Holds information about a specific localization string.
///
struct PXLANG_STRING_STRUCT
{
	/// The CRC checksum of the name for this localization string.
	///
	unsigned int crc;

	/// The content of this string.
	///
	wchar_t* content;

	/// The ASCII content of this string. Only gets allocated when this string is fetched from CSM_PxLang_GetString().
	char* content_ascii;
};

/// @brief Holds information about a localization group.
///
struct PXLANG_GROUP_STRUCT
{
	/// The CRC checksum of this group's name.
	///
	unsigned int crc;
	
	/// The amount of localized strings in this group.
	///
	unsigned int string_size;

	/// The amount of loaded strings in this group.
	///
	unsigned int string_count;

	/// The string list.
	///
	PXLANG_STRING_STRUCT* string_list;

	/// Whether this group's strings are loaded or not.
	///
	bool loaded;
};

/// @brief Holds information for a compiled pixel language file (.pxclang).
///
struct PXLANG_RESOURCE_STRUCT
{
	// Internal ------------------------------------------------

	/// The validity CRC for this object. Do not edit this.
	/// @private
	///
	unsigned int valid_crc;

	// Metadata ------------------------------------------------

	/// The path name of this resource. Does not end with a trailing slash.
	///
	char* path_name;

	/// This localization resource's desired font width.
	///
	unsigned int font_width;
	
	/// This localization resource's desired font height.
	///
	unsigned int font_height;
	
	/// Used to compare against the uncompiled file's CRC, and if they are different, then it will be re-compiled.
	///
	unsigned int file_crc;

	/// The file name of this resource.
	///
	char* file_name;
	
	/// The font name of this resource.
	///
	char* font_name;

	// Resource configuration ----------------------------------

	/// The encoding type.
	///
	int encoding_type;
	
	/// The charset type.
	///
	int charset_type;

	/// The display name of this localization file.
	///
	char* display_name;

	/// The selected group CRC.
	///
	unsigned int selected_group_crc;
	
	/// The amount of groups in this resource.
	///
	unsigned int group_count;

	/// The group list.
	///
	PXLANG_GROUP_STRUCT* group_list;

	/// The amount of localized strings.
	///
	unsigned int string_table_size;

	/// The string list.
	///
	wchar_t** string_table;

	/// Whether this resource is hidden or not.
	///
	bool hidden;

	// Linkage -------------------------------------------------

	/// The first language resource.
	///
	static PXLANG_RESOURCE_STRUCT* first;
	
	/// The next language resource.
	///
	PXLANG_RESOURCE_STRUCT* next;
	
	/// The previous language resource.
	///
	PXLANG_RESOURCE_STRUCT* prev;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// This global variable holds the currently loaded localization file.
///
extern CAVESTORY_MOD_API PXLANG_RESOURCE_STRUCT gCurrentLocalization;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Compile a language resource file.
///
/// @param pLangObj The language resource object to load the pxlang file into.
/// @param pFileName The file path of the pxlang file.
///
/// @return Returns 0 on success, non-zero on error.
extern CAVESTORY_MOD_API int CSM_PxLang_Compile(
	const char* pFileName
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Load a language resource file.
///
/// @param pLangObj The language resource object to load the pxlang file into.
/// @param pFileName The file path of the pxlang file.
/// @param pGroupName The name of the group to be loaded.
/// @param bRecompile Recompile the file if the file's CRC mismatches
///
/// @return Returns 0 on success, non-zero on error.
/// @note If the language object is already allocated to another pxlang file, then this will free the previous contents.
/// @note If no compiled pxlang file is found that matches the given pxlang file, then one will be compiled on the fly.
extern CAVESTORY_MOD_API int CSM_PxLang_Load(
	PXLANG_RESOURCE_STRUCT* pLangObj,
	const char* pFileName,
	const char* pGroupName = NULL,
	bool bRecompile = true
);

/// @brief Free up a language resource object.
///
/// @param pLangObj The object to free the memory of.
///
/// @return Returns TRUE on success, FALSE on failure.
extern CAVESTORY_MOD_API BOOL CSM_PxLang_Free(
	PXLANG_RESOURCE_STRUCT* pLangObj
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Check to see if a language object is loaded.
///
/// @param pLangObj The localization object to check.
///
/// @return Returns TRUE if it is loaded, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_PxLang_IsLoaded(
	PXLANG_RESOURCE_STRUCT* pLangObj
);

/// @brief Check to see if a language object has a localization string.
///
/// @param pStringName The name of the localization string.
/// @param pLangObj The localization object to reference. If this is NULL, then the currently loaded global localization object will be used.
///
/// @return Returns TRUE if the string was found, FALSE otherwise.
extern CAVESTORY_MOD_API BOOL CSM_PxLang_HasString(
	const char* pStringName,
	PXLANG_RESOURCE_STRUCT* pLangObj = NULL
);

/// @brief Get a string from a language object.
///
/// @param pStringName The name of the localization string.
/// @param pBuffer A pointer to a buffer that will hold the information.
/// @param pLangObj The localization object to reference. If this is NULL, then the currently loaded global localization object will be used.
/// @param pDefaultValue The default value to set if the string is not defined.
/// @param iArgCount The amount of arguments to replace.
/// @param ... A list of 
///
/// @return Returns TRUE if the string was found, FALSE otherwise.
extern CAVESTORY_MOD_API const char* CSM_PxLang_GetString(
	const char* pStringName,
	char* pBuffer = NULL,
	const char* pDefaultValue = NULL,
	PXLANG_RESOURCE_STRUCT* pLangObj = NULL,
	const char* pArgFormat = NULL,
	int iArgCount = 0,
	...
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Set the current localization object.
///
/// @param pFileName The name of the pxlang file to load.
/// @param bUseData Whether to use 'data' as the mod directory (TRUE), or the currently loaded mod (FALSE).
///
/// @return Returns TRUE on success, FALSE on failure.
/// @note You can safely free pLangObj afterwards if it was an allocated resource struct, but do not call CSM_PxLang_Free with it.
/// @note This loads the file from '<mod folder name>/Language/<pLanguageName>/main.pxlang'.
extern CAVESTORY_MOD_API BOOL CSM_PxLang_SetCurrentLocalization(
	const char* pLanguageName,
	bool bUseData = false
);

/// @brief Set the current localization object.
///
/// @param pGroupName The group name to use.
/// @param pLangObj The localization object to set as current. If this is 'NULL', then it uses gCurrentLocalization.
extern CAVESTORY_MOD_API void CSM_PxLang_SetGroup(
	const char* pGroupName,
	PXLANG_RESOURCE_STRUCT* pLangObj = NULL
);

/// @brief Build an argumentitive string.
///
/// @param The format string. Only use '%s' for specifying arguments.
///
/// @return Returns a pointer to a static buffer with the built result.
extern CAVESTORY_MOD_API const char* CSM_PxLang_BuildFormat(
	const char* pFormat
);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_endcode.h>

/// @}
///