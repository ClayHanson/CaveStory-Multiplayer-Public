#ifdef WINDOWS
#define RECT WINRECT
#define FindResource WinFindResource	// All these damn name collisions...
#define DrawText WinDrawText
#define LoadFont WinLoadFont
#include <windows.h>
#undef LoadFont
#undef DrawText
#undef FindResource
#undef RECT
#undef LoadBitmap
#endif

#include "Generic.h"
#include <CavestoryModAPI.h>
#include "LanguageResource.h"
#include "GenericLoad.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H
#include FT_BITMAP_H

// hacky but whatever
#define _CAVESTORY_MULTIPLAYER
#include "ModConfigResource.h"

#include "CRC32.h"
#include "Draw.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FT_Encoding gDefaultEncoding = FT_Encoding::FT_ENCODING_NONE;
DWORD gDefaultCharset        = DEFAULT_CHARSET;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_begincode.h>

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Static stuff

PXLANG_RESOURCE_STRUCT gCurrentLocalization;
PXLANG_RESOURCE_STRUCT* PXLANG_RESOURCE_STRUCT::first = NULL;
char g_PxLang_StaticFormatBuffer[4096];
char* g_Ascii_PxLang_StaticChar                       = NULL;
int g_Ascii_PxLang_StaticChar_MaxSize                 = 0;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Internal functions

static unsigned int CSM_PxLang_INTERNAL_GetStringNameCrc(const char* pName)
{
	wchar_t pBuffer[1024];
	wchar_t* pOutPtr = pBuffer;

	for (const char* ptr = pName; *ptr != 0; ptr++)
		* pOutPtr++ = *ptr;

	*pOutPtr++ = 0;

	return StringCRC32(pBuffer, sizeof(wchar_t) * wcslen(pBuffer));
}

static void CSM_PxLang_INTERNAL_BuildLocaleString_ASCII(const wchar_t* pLocaleContent, char* pBuffer, int iArgCount, const char** pArgList, int iArgsTotalLength, bool bAllocate)
{
	if (!pLocaleContent || (!pBuffer && !bAllocate))
		return;

	if (bAllocate)
	{
		// Resize if necessary
		int pOutSize = wcslen(pLocaleContent) + 1 + iArgsTotalLength;

		// Allocate a static character if we don't have a buffer to populate
		if (!g_Ascii_PxLang_StaticChar || pOutSize >= g_Ascii_PxLang_StaticChar_MaxSize)
		{
			if (!g_Ascii_PxLang_StaticChar)
			{
				g_Ascii_PxLang_StaticChar_MaxSize = pOutSize;
				g_Ascii_PxLang_StaticChar         = (char*)malloc(sizeof(char) * g_Ascii_PxLang_StaticChar_MaxSize);
			}
			else
			{
				g_Ascii_PxLang_StaticChar_MaxSize = pOutSize;
				g_Ascii_PxLang_StaticChar         = (char*)realloc((void*)g_Ascii_PxLang_StaticChar, sizeof(char) * g_Ascii_PxLang_StaticChar_MaxSize);
			}
		}

		pBuffer = g_Ascii_PxLang_StaticChar;
	}

	if (wcschr(pLocaleContent, '\1') == NULL)
	{
		wcstombs(pBuffer, pLocaleContent, wcslen(pLocaleContent) + 1);
		return;
	}

	// Parse the content string.
	for (const wchar_t* ptr = pLocaleContent; *ptr != L'\0'; ptr++)
	{
		if (*ptr == L'\1')
		{
			++ptr;

			if ((unsigned int)((*ptr) - 1) < iArgCount)
			{
				strcpy(pBuffer, pArgList[CLAMP(((*ptr) - 1), 0, iArgCount - 1)]);
				pBuffer += strlen(pArgList[CLAMP(((*ptr) - 1), 0, iArgCount - 1)]);
			}

			continue;
		}

		pBuffer += wcstombs(pBuffer, ptr, 1);
	}

	*pBuffer = 0;
}

static void CSM_PxLang_INTERNAL_BuildLocaleString_ASCII_AsciiFmt(const char* pLocaleContent, char* pBuffer, int iArgCount, const char** pArgList, int iArgsTotalLength, bool bAllocate)
{
	if (!pLocaleContent || (!pBuffer && !bAllocate))
		return;

	if (bAllocate)
	{
		// Resize if necessary
		int pOutSize = strlen(pLocaleContent) + 1 + iArgsTotalLength;

		// Allocate a static character if we don't have a buffer to populate
		if (!g_Ascii_PxLang_StaticChar || pOutSize >= g_Ascii_PxLang_StaticChar_MaxSize)
		{
			if (!g_Ascii_PxLang_StaticChar)
			{
				g_Ascii_PxLang_StaticChar_MaxSize = pOutSize;
				g_Ascii_PxLang_StaticChar         = (char*)malloc(sizeof(char) * g_Ascii_PxLang_StaticChar_MaxSize);
			}
			else
			{
				g_Ascii_PxLang_StaticChar_MaxSize = pOutSize;
				g_Ascii_PxLang_StaticChar         = (char*)realloc((void*)g_Ascii_PxLang_StaticChar, sizeof(char) * g_Ascii_PxLang_StaticChar_MaxSize);
			}
		}

		pBuffer = g_Ascii_PxLang_StaticChar;
	}

	if (strchr(pLocaleContent, '\1') == NULL)
	{
		strcpy(pBuffer, pLocaleContent);
		return;
	}

	// Parse the content string.
	for (const char* ptr = pLocaleContent; *ptr != L'\0'; ptr++)
	{
		if (*ptr == L'\1')
		{
			++ptr;

			if ((unsigned int)((*ptr) - 1) < iArgCount)
			{
				strcpy(pBuffer, pArgList[CLAMP(((*ptr) - 1), 0, iArgCount - 1)]);
				pBuffer += strlen(pArgList[CLAMP(((*ptr) - 1), 0, iArgCount - 1)]);
			}

			continue;
		}

		*pBuffer++ = *ptr;
	}

	*pBuffer = 0;
}

void CSM_PxLang_INTERNAL_DeleteArgList(const char* pArgFormat, const char**& pArgList, int iArgCount)
{
	if (!pArgList)
		return;

	int iFormatLen = (!pArgFormat ? 0 : strlen(pArgFormat));

	// Populate the argument list
	for (int i = 0; i < iArgCount; i++)
	{
		if (i < iFormatLen)
		{
			// Only format special ones
			switch (pArgFormat[i])
			{
				case 'd':
				{
					free((char*)pArgList[i]);
					break;
				}
			}
		}
		else
			break;
	}

	free(pArgList);
	pArgList = NULL;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Exported functions

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSM_PxLang_HasString(const char* pStringName, PXLANG_RESOURCE_STRUCT* pLangObj)
{
	if ((!pLangObj || !CSM_PxLang_IsLoaded(pLangObj)) && !CSM_PxLang_IsLoaded(pLangObj = &gCurrentLocalization))
		return FALSE;

	unsigned int iDesiredCRC = CSM_PxLang_INTERNAL_GetStringNameCrc(pStringName);
	for (int i = 0; i < pLangObj->group_count; i++)
	{
		PXLANG_GROUP_STRUCT* pGroup = &pLangObj->group_list[i];

		// Skip unloaded groups
		if (!pGroup->loaded)
			continue;

		for (int j = 0; j < pLangObj->string_table_size; j++)
		{
			if (pGroup->string_list[j].crc != iDesiredCRC)
				continue;

			return TRUE;
		}
	}

	return FALSE;
}

const char* CSM_PxLang_GetString(const char* pStringName, char* pBuffer, const char* pDefaultValue, PXLANG_RESOURCE_STRUCT* pLangObj, const char* pArgFormat, int iArgCount, ...)
{
	// Allocate & populate an argument array if necessary
	const char** pArgList = NULL;
	int iArgsTotalLength  = 0;
	int iFormatLen        = (!pArgFormat ? 0 : strlen(pArgFormat));
	char pTempBuffer[256];
	if (iArgCount)
	{
		if (iFormatLen < iArgCount)
			printf("ERROR: CSM_PxLang_GetString() - Arg format string does not match the argument count.\r\n");

		// Allocate it first
		pArgList = (const char**)malloc(sizeof(const char*) * iArgCount);

		va_list vPtr;
		va_start(vPtr, iArgCount);

		// Populate the argument list
		for (int l = 0; l < iArgCount; l++)
		{
			if (l < iFormatLen)
			{
				switch (pArgFormat[l])
				{
					case 'd':
					{
						int iNum          = va_arg(vPtr, int);
						sprintf(pTempBuffer, "%d", iNum);
						pArgList[l]       = strdup(pTempBuffer);
						iArgsTotalLength += strlen(pArgList[l]);
						break;
					}
					case 's':
					{
						pArgList[l]       = va_arg(vPtr, const char*);
						iArgsTotalLength += strlen(pArgList[l]);
						break;
					}
					default:
					{
						printf("ERROR: CSM_PxLang_GetString() - Unknown arg format character '%.1s'.\r\n", pArgFormat + l);
						break;
					}
				}
			}
			else
			{
				pArgList[l]       = va_arg(vPtr, const char*);
				iArgsTotalLength += strlen(pArgList[l]);
			}
		}

		// Done with this
		va_end(vPtr);
	}

	// Stop here if the mod we specified isn't loaded.
	if ((!pLangObj || !CSM_PxLang_IsLoaded(pLangObj)) && !CSM_PxLang_IsLoaded(pLangObj = &gCurrentLocalization))
	{
		// Process the default value if we have one
		if (pDefaultValue)
			CSM_PxLang_INTERNAL_BuildLocaleString_ASCII_AsciiFmt(pDefaultValue, (pBuffer ? pBuffer : g_Ascii_PxLang_StaticChar), iArgCount, pArgList, iArgsTotalLength, !pBuffer);

		// Free the argument list
		CSM_PxLang_INTERNAL_DeleteArgList(pArgFormat, pArgList, iArgCount);

		// Return it.
		return (pBuffer ? pBuffer : g_Ascii_PxLang_StaticChar);
	}

	// Loop through all groups and attempt to find the locale string.
	unsigned int iDesiredCRC = CSM_PxLang_INTERNAL_GetStringNameCrc(pStringName);
	for (int i = 0; i < pLangObj->group_count; i++)
	{
		PXLANG_GROUP_STRUCT* pGroup = &pLangObj->group_list[i];

		// Skip unloaded groups
		if (!pGroup->loaded)
			continue;

		for (int j = 0; j < pGroup->string_count; j++)
		{
			PXLANG_STRING_STRUCT* pString = &pGroup->string_list[j];

			// Skip mismatches
			if (pString->crc != iDesiredCRC)
				continue;

			// Allocate the 'content_ascii' variable.
			if (!pString->content_ascii)
			{
				// Create the ascii string
				pString->content_ascii = (char*)malloc(wcslen(pString->content) + 1);
				char* pOut = pString->content_ascii;
				for (wchar_t* ptr = pString->content; *ptr != L'\0'; ptr++)
				{
					if (*ptr == '\1')
					{
						++ptr;
						continue;
					}

					pOut += wcstombs(pOut, ptr, 1);
				}

				*pOut++ = 0;
			}

			// Build the string if we have an argument count
			if (iArgCount)
				CSM_PxLang_INTERNAL_BuildLocaleString_ASCII(pString->content, (pBuffer ? pBuffer : g_Ascii_PxLang_StaticChar), iArgCount, pArgList, iArgsTotalLength, !pBuffer);
			else if (pBuffer)
				strcpy(pBuffer, pString->content_ascii);

			CSM_PxLang_INTERNAL_DeleteArgList(pArgFormat, pArgList, iArgCount);

			return (!pBuffer ? (iArgCount ? g_Ascii_PxLang_StaticChar : pString->content_ascii) : pBuffer);
		}
	}

	if (!pDefaultValue)
	{
		CSM_PxLang_INTERNAL_DeleteArgList(pArgFormat, pArgList, iArgCount);

		return (pBuffer ? pBuffer : pStringName);
	}

	CSM_PxLang_INTERNAL_BuildLocaleString_ASCII_AsciiFmt(pDefaultValue, (pBuffer ? pBuffer : g_Ascii_PxLang_StaticChar), iArgCount, pArgList, iArgsTotalLength, !pBuffer);
	CSM_PxLang_INTERNAL_DeleteArgList(pArgFormat, pArgList, iArgCount);

	return (pBuffer ? pBuffer : g_Ascii_PxLang_StaticChar);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define LOAD_GENERIC_SURFACE_FILE(SURF_ID, FILE_NAME)\
	sprintf(pSurfPath, "%s/Language/%s/" FILE_NAME ".png", (bUseData ? "data" : gCaveMod.mModPath), pLanguageName);\
	if (CheckFileExists2(pSurfPath))\
	{\
		if (IsSurfaceAllocated(SURF_ID))\
		{\
			ReloadBitmap_File(pSurfPath, SURF_ID);\
		}\
		else\
			MakeSurface_File(pSurfPath, SURF_ID);\
	}

BOOL CSM_PxLang_SetCurrentLocalization(const char* pLanguageName, bool bUseData)
{
	char pLangPath[260];
	char pFontPath[260];
	char pSurfPath[260];

	// Build the path string
	sprintf_s(pLangPath, "%s/Language/%s/main.pxlang", (bUseData ? "data" : gCaveMod.mModPath), pLanguageName);

	// Attempt to load it
	if (CSM_PxLang_Load(&gCurrentLocalization, pLangPath, NULL, true))
	{
		printf("ERROR: CSM_PxLang_SetCurrentLocalization() - Couldn't find localization \"%s\".\r\n", pLanguageName);
		return FALSE;
	}

	// Setup everything
	gDefaultEncoding = (FT_Encoding)gCurrentLocalization.encoding_type;
	gDefaultCharset  = gCurrentLocalization.charset_type;

	// Unload the old global font
	UnloadFont(gFont);

	// Get a bunch of variables ready for re-loading the global font
	size_t data_size;
	unsigned int fontWidth  = gCurrentLocalization.font_width * magnification;
	unsigned int fontHeight = gCurrentLocalization.font_height * magnification;

	// Build the font path
	sprintf_s(pFontPath, "%s/Language/%s/%s.ttf", (bUseData ? "data" : gCaveMod.mModPath), pLanguageName, gCurrentLocalization.font_name);

	// Attempt to load the font from file first.
	if ((gFont = LoadFontFromFile(pFontPath, fontWidth, fontHeight)) == NULL)
	{
		// Build the font path
		sprintf_s(pFontPath, "%s/Language/%s/%s.ttc", (bUseData ? "data" : gCaveMod.mModPath), pLanguageName, gCurrentLocalization.font_name);

		// Attempt to load the font from file first.
		if ((gFont = LoadFontFromFile(pFontPath, fontWidth, fontHeight)) == NULL)
		{
			unsigned char* data = GetFontFromWindows(&data_size, gCurrentLocalization.font_name, fontWidth, fontHeight);

			// Load it if we got it
			if (data)
			{
				gFont = LoadFontFromData(data, data_size, fontWidth, fontHeight);
				free(data);
			}
			else
			{
				printf("ERROR: CSM_PxLang_SetCurrentLocalization() - Failed to set font to \"%s\"\r\n", gCurrentLocalization.font_name);
			}
		}
	}

	// Build the font path
	sprintf_s(pFontPath, "%s/Language/%s", (bUseData ? "data" : gCaveMod.mModPath), pLanguageName);

	// Reload the font in basemodeinstance
	BaseModeInstance::ReloadFont(gCurrentLocalization.font_name, pFontPath, gCurrentLocalization.font_width, gCurrentLocalization.font_height);

	if (*gCaveMod.mHeader.title)
		ReloadGenericData(true);
	
	// Load the surfaces
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_PIXEL,				"pixel");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_TITLE,				"Title");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_ARMS_IMAGE,		"ArmsImage");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_ARMS,				"Arms");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_ITEM_IMAGE,		"ItemImage");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_STAGE_ITEM,		"StageImage");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_MY_CHAR,			"MyChar");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_NPC_SYM,			"NpcSym");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_NPC_REGU,			"NpcRegu");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_TEXT_BOX,			"TextBox");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_CARET,				"Caret");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_BULLET,			"Bullet");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_FACE,				"Face");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_FACE_COLOR_MASK,	"FaceMask");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_FADE,				"Fade");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_CREDITS_IMAGE,		"Credit01");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_GUI,				"GUI");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_CHAT_TOPICS,		"ChatTopics");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_CONTROLS,			"Controls");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_DEMO_EDITOR,		"DemoEditor");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_HELP_IMAGES,		"HelpImages");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_TEAM_SPAWN_DOORS,	"TeamSpawnDoors");
}

void CSM_PxLang_SetGroup(const char* pGroupName, PXLANG_RESOURCE_STRUCT* pLangObj)
{
	if (((!pLangObj || !CSM_PxLang_IsLoaded(pLangObj)) && !CSM_PxLang_IsLoaded(pLangObj = &gCurrentLocalization)) || (pLangObj && !pLangObj->file_name))
		return;

	char path[260];
	strcpy(path, pLangObj->file_name);

	CSM_PxLang_Load(pLangObj, path, pGroupName, false);
}

const char* CSM_PxLang_BuildFormat(const char* pFormat)
{
	char* out         = g_PxLang_StaticFormatBuffer;
	const char* start = pFormat;
	const char* next  = strstr(pFormat, "%s");
	int arg_count     = 0;

	// If we couldn't find it, then just return the format string.
	if (!next)
		return pFormat;

	while (next)
	{
		*out = 0;
		strncat(out, start, next - start);
		out += next - start;

		++arg_count;
		*out++ = '\1';
		*out++ = (char)arg_count;

		start = next + 2;
		next  = strstr(start, "%s");
	}
	
	// Copy the rest
	strcpy(out, start);
	out[strlen(start)] = 0;

	return g_PxLang_StaticFormatBuffer;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_endcode.h>