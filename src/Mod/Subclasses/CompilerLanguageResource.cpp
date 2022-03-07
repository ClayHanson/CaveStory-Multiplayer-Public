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

#include <CavestoryModAPI.h>
#include "LanguageResource.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H
#include FT_BITMAP_H

#include "Draw.h"
#include "../../BitStream.h"
#include "CRC32.h"
#include "List.h"
#include <stdarg.h>

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
	 FILE FORMAT FOR .PXCLANG:
	 * +-----+--------+--------------------------------------------------------------------------------------------------------------------------- -  -   -     -
	 * | VER |  TYPE  | DESCRIPTION
	 * +-----+--------+--------------------------------------------------------------------------------------------------------------------------- -  -   -     -
	 * |  1  | STRING | File Header ("PXLANG")
	 * |  1  | UCHAR  | File Version
	 * |  1  | BOOL   | Localization Hidden
	 * |  1  | UCHAR  | Localization Encoding Type
	 * |  1  | UCHAR  | Localization Charset Type
	 * |  1  | USHORT | Localization Font Width
	 * |  1  | USHORT | Localization Font Height
	 * |  1  | UINT   | Language Name Length
	 * |  1  | STRING | Language Name
	 * |  1  | UINT   | Language Font Length
	 * |  1  | STRING | Language Font
	 * |  1  | UINT   | Uncompiled File CRC
	 * |  1  | USHORT | Uncompiled File Name Length
	 * |  1  | STRING | Uncompiled File Name
	 * |  1  | UINT   | String Table Size
	 * |  1  | UINT   | Int Table Size
	 * |  1  | UINT   | Minimum integer stored in the int table
	 * |  1  | UINT   | Maximum integer stored in the int table
	 * |     |        | >>==================================== CHUNK: Int_ Table ====================================<<
	 * |  1  | RANGED | Integer
	 * |     |        | >>=================================== CHUNK: String Table ===================================<<
	 * |  1  | WCHAR  | Null-Terminated Wide String
	 * |     |        | >>================================== CHUNK: Instruction Set ==================================<<
	 * |  1  | RANGED | Instruction opcode
	 * |  1  | ...    | Opcode arguments (if necessary)
	 * +-----+--------+--------------------------------------------------------------------------------------------------------------------------- -  -   -     -
*/
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Structs

struct PXLANG_COMPILER_STRING
{
	int group_idx;
	int name_idx;
	int string_idx;
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Enumeration

enum PxLangOpCode
{
	// No arguments
	PXL_OP_NUL = 0,

	// 01 (Name CRC - Index # to int table) (Group Byte Size)
	PXL_OP_GRP = 1,

	// 02 (Name CRC - Index # to int table) (String Content - Index # to string table)
	PXL_OP_STR = 2,

	// Maximum number of opcodes
	PXL_MAX_OP
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Internal functions

#define CHARSET_CHECK(CSSTR)	if (!strcmpi(pCharSet, #CSSTR)) { return CSSTR##_CHARSET; }
#define ENCODING_CHECK(CSSTR)	if (!strcmpi(pEncoding, #CSSTR)) { return FT_Encoding::FT_ENCODING_##CSSTR; }

int GetGameFontCharset(const char* pCharSet)
{
	CHARSET_CHECK(DEFAULT);
	CHARSET_CHECK(ANSI);
	CHARSET_CHECK(SYMBOL);
	CHARSET_CHECK(SHIFTJIS);
	CHARSET_CHECK(HANGEUL);
	CHARSET_CHECK(HANGUL);
	CHARSET_CHECK(GB2312);
	CHARSET_CHECK(CHINESEBIG5);
	CHARSET_CHECK(OEM);
	CHARSET_CHECK(JOHAB);
	CHARSET_CHECK(HEBREW);
	CHARSET_CHECK(ARABIC)
	CHARSET_CHECK(GREEK);
	CHARSET_CHECK(TURKISH);
	CHARSET_CHECK(VIETNAMESE);
	CHARSET_CHECK(THAI);
	CHARSET_CHECK(EASTEUROPE);
	CHARSET_CHECK(RUSSIAN);
	CHARSET_CHECK(MAC);
	CHARSET_CHECK(BALTIC);

	return -1;
}

int GetGameFontEncoding(const char* pEncoding)
{
	ENCODING_CHECK(NONE);
	ENCODING_CHECK(MS_SYMBOL);
	ENCODING_CHECK(UNICODE);
	ENCODING_CHECK(SJIS);
	ENCODING_CHECK(PRC);
	ENCODING_CHECK(BIG5);
	ENCODING_CHECK(WANSUNG);
	ENCODING_CHECK(JOHAB);

	return -1;
}

// Get the arguments from a string like '\c{CMD:ARG1:ARG2:ARG3}'.
// Returns the number of arguments gotten. Also returns '-1' if a syntax error occurred.
// This should start at the command's start (e.g. '\' in '\c{CMD:ARG1:ARG2:ARG3}').
//
// _ArgTypeList is just a string of argument types defined by characters. Do NOT use this as you would sscanf() -- It should ONLY have valid argument types that you can find in the list below! NO WHITESPACE.
//
// Possible _ArgTypeList types:
//  'd' - int*
//  's' - char*    (Must also include the size of the buffer in the next variadic argument)
//  'S' - wchar_t* (Must also include the size of the buffer in the next variadic argument)
static int CSM_PxLang_INTERNAL_GetArgument(int line_no, wchar_t*& ptr, int cmd_name_len, const char* _ArgTypeList, ...)
{
	// It MUST start with '\'
	wchar_t* pArgStart = NULL;
	wchar_t* pArgEnd   = NULL;
	wchar_t* pArgPtr   = NULL;
	int iArgCount      = 0;
	char pBuffer[1024];

	// Validate it.
	if (ptr[0] != L'\\' || *((pArgStart = &ptr[1 + cmd_name_len + 1]) - 1) != L'{' || (pArgEnd = wcschr(pArgStart, L'}')) == NULL)
		return 0;

	// Set the pointer
	pArgPtr = pArgStart;

	// Begin the argument list
	va_list vPtr;
	va_start(vPtr, _ArgTypeList);

	// Parse the format
	for (const char* fptr = _ArgTypeList; *fptr != 0; fptr++)
	{
		if (pArgPtr >= pArgEnd)
			break;

		if (pArgPtr != pArgStart)
		{
			if (*pArgPtr != ':')
			{
				printf("ERROR: CSM_PxLang_Compile() - Unexpected character '%.1S'; Expected argument delimiter ':' (Line # %d)!\r\n", pArgPtr, line_no);
				iArgCount = -1;
				goto END_FOR_LOOP;
			}
			else
				++pArgPtr;
		}

		void* pOutPointer = va_arg(vPtr, void*);
		char* pBufferPtr  = pBuffer;

		switch (*fptr)
		{
			case 'd':
			{
				for (; *pArgPtr != L'\0' && *pArgPtr != L':' && *pArgPtr != L'}'; pArgPtr++)
				{
					// Catch invalid characters
					if (*pArgPtr < L'0' || *pArgPtr > L'9')
					{
						printf("ERROR: CSM_PxLang_Compile() - Unexpected character '%.1S'; Expected number (Line # %d)!\r\n", pArgPtr, line_no);
						iArgCount = -1;
						goto END_FOR_LOOP;
					}

					*pBufferPtr++ = (char)*pArgPtr;
				}

				*pBufferPtr++ = 0;

				// Set the value.
				*((int*)pOutPointer) = atoi(pBuffer);
				++iArgCount;
				break;
			}
			case 's':
			{
				int iOutPointerSize = va_arg(vPtr, int);

				for (; *pArgPtr != L'\0' && *pArgPtr != L':' && *pArgPtr != L'}'; pArgPtr++)
				{
					if (pBufferPtr - pBuffer >= iOutPointerSize - 1)
						break;

					*pBufferPtr++ = (char)*pArgPtr;
				}

				*pBufferPtr++ = 0;

				// Set the value.
				strcpy((char*)pOutPointer, pBuffer);
				++iArgCount;
				break;
			}
			case 'S':
			{
				int iOutPointerSize = va_arg(vPtr, int);

				wchar_t* pOutWideChar = (wchar_t*)pOutPointer;
				for (; *pArgPtr != L'\0' && *pArgPtr != L':' && *pArgPtr != L'}'; pArgPtr++)
				{
					if ((wchar_t*)pOutPointer - pOutWideChar >= iOutPointerSize - 1)
						break;

					*pOutWideChar++ = (char)*pArgPtr;
				}

				*pOutWideChar++ = 0;
				++iArgCount;
				break;
			}
			default:
			{
				printf("ERROR: " __FUNCTION__ "() - Unknown format type '%.1s'.\r\n", fptr);
				break;
			}
		}
	}

	if (*pArgPtr != L'}')
	{
		printf("ERROR: CSM_PxLang_Compile() - Unexpected character '%.1S'; Expected end of arguments list (Line # %d)!\r\n", pArgPtr, line_no);
		iArgCount = -1;
		goto END_FOR_LOOP;
	}

END_FOR_LOOP:
	ptr = pArgEnd + 1;
	va_end(vPtr);

	return iArgCount;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_begincode.h>

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Exported functions

#define IS_COMMAND(CMDNAME) wcsstr(line, L#CMDNAME) == line

int CSM_PxLang_Compile(const char* pFileName)
{
	char pCompiledFile[260];
	const char* pptr;

	// Validate the filename
	if ((pptr = strrchr(pFileName, '.')) != NULL && strcmpi(pptr, ".pxlang"))
		return 0;
	
	strcpy(pCompiledFile, pFileName);
	pCompiledFile[pptr - pFileName] = 0;
	strcat(pCompiledFile, ".pxclang");

	FILE* fp;
	if ((fp = fopen(pFileName, "r")) == NULL)
	{
		printf("ERROR: CSM_PxLang_Compile() - Failed to open \"%s\"!\r\n", pFileName);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	unsigned long long int iFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* pEntireFile = (char*)malloc(sizeof(char) * iFileSize);
	fread(pEntireFile, iFileSize, 1, fp);
	fseek(fp, 0, SEEK_SET);
	unsigned int iUncompiledFileCrc = StringCRC32(pEntireFile, iFileSize);
	free(pEntireFile);

	// Begin the variables
	List<PXLANG_COMPILER_STRING*> pLocalizationStringTable;
	List<long long int> pIntTable;
	List<wchar_t*> pStringTable;
	List<unsigned int> pGroupTable;
	SharedBitStream pStream;
	char pLangName[256];
	char pLangFont[256];
	*pLangFont = NULL;

	// Get the default language name
	if (1)
	{
		const char* pStart1 = strrchr(pFileName, '/');
		const char* pStart2 = strrchr(pFileName, '\\');
		const char* pStart  = (!pStart1 ? pStart2 : pStart1);

		if (pStart)
			++pStart;
		else
			pStart = pFileName;

		char* pLangNamePtr = pLangName;

		// Loop through the string
		for (pptr = pStart; *pptr != 0; pptr++)
			if ((*pptr >= 'A' && *pptr <= 'Z') || (*pptr >= 'a' && *pptr <= 'z') || (*pptr >= '0' && *pptr <= '9') || *pptr != '_')
				*pLangNamePtr++ = *pptr;

		*pLangNamePtr++ = 0;
	}

	wchar_t line[4096];
	wchar_t* ptr;
	int iLineNo       = 0;
	int iReturnValue  = 0;
	int iCurrentGroup = 0;
	int iNameIndex    = 0;
	int iContentIndex = 0;
	bool bLangHidden  = false;
	char pTempBuffer[1024];
	int iEncodingType = FT_Encoding::FT_ENCODING_NONE;
	int iCharsetType  = DEFAULT_CHARSET;
	int iFontWidth    = -1;
	int iFontHeight   = -1;

	pIntTable.Insert(StringCRC32(L"GLOBAL", sizeof(wchar_t) * wcslen(L"GLOBAL")));
	pGroupTable.Insert(0);

	// Get all integers & strings + syntax check + get special command values
	while (fgetws(line, sizeof(line), fp))
	{
		++iLineNo;

		// Remove comment if it exists
		if ((ptr = wcsstr(line, L"//")) != NULL)
			*ptr = 0;

		// Trim left
		while (line[0] == L' ' || line[0] == L'\t')
			memmove(line, &line[1], sizeof(line) - sizeof(wchar_t));
		
		// Trim right
		ptr = &line[wcslen(line) - 1];
		while (*ptr == L' ' || *ptr == L'\t' || *ptr == L'\n' || *ptr == L'\r')
		{
			if (ptr <= line)
			{
				*ptr = 0;
				break;
			}

			*ptr = 0;
			--ptr;
		}

		// Skip blank lines
		if (!line[0])
			continue;

		// Find the second half of this line
		wchar_t* pLineValue = line;
		bool bGotWhitespace = 0;
		for (; *pLineValue != L'\0'; pLineValue++)
		{
			if (bGotWhitespace && *pLineValue != L' ' && *pLineValue != L'\t')
				break;
			else if (*pLineValue == L' ' || *pLineValue == L'\t')
				bGotWhitespace = true;
		}

		if (!pLineValue || *pLineValue == L'\0')
		{
			printf("ERROR: CSM_PxLang_Compile() - Found an incomplete line (Line # %d)!\r\n", iLineNo);
			iReturnValue = -2;
			goto FREE_EVERYTHING;
		}

		// Determine what to do with this line.
		if (line[0] == L'#')
		{
			// Localization string; Read the string name.
			int iLength = 0;
			for (ptr = line + 1; *ptr != L'\0'; ptr++)
			{
				if (*ptr == L'\t' || *ptr == L' ')
					break;
				else if (!(*ptr >= L'A' && *ptr <= L'Z') && !(*ptr >= L'a' && *ptr <= L'z') && !(*ptr >= L'0' && *ptr <= L'9') && *ptr != L'_')
				{
					printf("ERROR: CSM_PxLang_Compile() - Found an incomplete line (Line # %d)!\r\n", iLineNo);
					iReturnValue = -2;
					goto FREE_EVERYTHING;
				}

				++iLength;
			}

			if (!iLength)
			{
				printf("ERROR: CSM_PxLang_Compile() - Expected a localization string name (Line # %d)!\r\n", iLineNo);
				iReturnValue = -7;
				goto FREE_EVERYTHING;
			}

			// Get the crc
			long long int iNameCRC = (long long int)StringCRC32(&line[1], sizeof(wchar_t) * iLength);

			// Add to the integer table
			if ((iNameIndex = pIntTable.FindIndex(iNameCRC)) == -1)
				iNameIndex = pIntTable.Insert(iNameCRC);

			wchar_t pLineValueComplete[2048];
			wchar_t* pLineValueCompletePtr = pLineValueComplete;

			// Parse the string.
			for (ptr = pLineValue; *ptr != L'\0'; ptr++)
			{
				if (*ptr == L'\\')
				{
					if (ptr[1] == L'n')
					{
						// New line
						*pLineValueCompletePtr++ = '\n';
						ptr++;
					}
					else if (ptr[1] == L'i')
					{
						// Blank character
						ptr++;
					}
					else if (ptr[1] == L'k')
					{
						// NOD
						*pLineValueCompletePtr = 0;
						wcsncat(pLineValueCompletePtr, L"<NOD", 4);
						pLineValueCompletePtr += 4;
						ptr++;
					}
					else if (ptr[1] == L'w')
					{
						// WAI
						wchar_t pWaitMS[5];
						int iWaitMS   = 0;
						int iArgCount = CSM_PxLang_INTERNAL_GetArgument(iLineNo, ptr, 1, "d", &iWaitMS);

						if (iArgCount == -1)
						{
							iReturnValue = -13;
							goto FREE_EVERYTHING;
						}
						else if (iArgCount == 0)
						{
							printf("ERROR: CSM_PxLang_Compile() - Escaped command 'w' requires an argument list (Line # %d)!\r\n", iLineNo);
							iReturnValue = -14;
							goto FREE_EVERYTHING;
						}

						wsprintfW(pWaitMS, L"%04d", iWaitMS);

						*pLineValueCompletePtr = 0;
						wcscat(pLineValueCompletePtr, L"<WAI");
						wcscat(pLineValueCompletePtr, pWaitMS);
						pLineValueCompletePtr += 8;
						--ptr;
					}
					else if (ptr[1] == L'c')
					{
						// TSC Command
						wchar_t pCmdName[4];
						wchar_t pArg1[5];
						wchar_t pArg2[5];
						wchar_t pArg3[5];
						wchar_t pArg4[5];
						int iArg1 = 0;
						int iArg2 = 0;
						int iArg3 = 0;
						int iArg4 = 0;
						int iArgCount = CSM_PxLang_INTERNAL_GetArgument(iLineNo, ptr, 1, "Sdddd", pCmdName, sizeof(pCmdName), &iArg1, &iArg2, &iArg3, &iArg4);

						if (iArgCount == -1)
						{
							iReturnValue = -13;
							goto FREE_EVERYTHING;
						}
						else if (iArgCount == 0)
						{
							printf("ERROR: CSM_PxLang_Compile() - Escaped command 'c' requires an argument list (Line # %d)!\r\n", iLineNo);
							iReturnValue = -14;
							goto FREE_EVERYTHING;
						}

						// Build the integer strings
						wsprintfW(pArg1, L"%04d", CLAMP(iArg1, 0, 9999));
						wsprintfW(pArg2, L"%04d", CLAMP(iArg2, 0, 9999));
						wsprintfW(pArg3, L"%04d", CLAMP(iArg3, 0, 9999));
						wsprintfW(pArg4, L"%04d", CLAMP(iArg4, 0, 9999));

						*pLineValueCompletePtr = 0;
						wcscat(pLineValueCompletePtr, L"<");
						wcscat(pLineValueCompletePtr, pCmdName);

						if (iArgCount >= 2)
							wcscat(pLineValueCompletePtr, pArg1);

						if (iArgCount >= 3)
						{
							wcscat(pLineValueCompletePtr, L":");
							wcscat(pLineValueCompletePtr, pArg2);
						}

						if (iArgCount >= 4)
						{
							wcscat(pLineValueCompletePtr, L":");
							wcscat(pLineValueCompletePtr, pArg3);
						}

						if (iArgCount >= 5)
						{
							wcscat(pLineValueCompletePtr, L":");
							wcscat(pLineValueCompletePtr, pArg4);
						}

						pLineValueCompletePtr += 4 + ((iArgCount - 1) * 4) + MAX(0, (iArgCount - 2));
						--ptr;
					}
					else if (ptr[1] == L'a')
					{
						// String argument
						int iArgIndex = 0;
						int iArgCount = CSM_PxLang_INTERNAL_GetArgument(iLineNo, ptr, 1, "d", &iArgIndex);

						if (iArgCount == -1)
						{
							iReturnValue = -13;
							goto FREE_EVERYTHING;
						}
						else if (iArgCount == 0)
						{
							printf("ERROR: CSM_PxLang_Compile() - Escaped command 'a' requires an argument list (Line # %d)!\r\n", iLineNo);
							iReturnValue = -14;
							goto FREE_EVERYTHING;
						}

						*pLineValueCompletePtr++ = '\1';
						*pLineValueCompletePtr++ = CLAMP(iArgIndex + 1, 1, 65535);

						// String arguments
						--ptr;
					}
					else if (ptr[1] == L'u')
					{
						// Arbitrary unicode value
						int iUniValue = 0;
						int iArgCount = CSM_PxLang_INTERNAL_GetArgument(iLineNo, ptr, 1, "d", &iUniValue);

						if (iArgCount == -1)
						{
							iReturnValue = -13;
							goto FREE_EVERYTHING;
						}
						else if (iArgCount == 0)
						{
							printf("ERROR: CSM_PxLang_Compile() - Escaped command 'u' requires an argument list (Line # %d)!\r\n", iLineNo);
							iReturnValue = -14;
							goto FREE_EVERYTHING;
						}

						// Add it
						*pLineValueCompletePtr++ = (wchar_t)iUniValue;
						--ptr;
					}

					continue;
				}

				*pLineValueCompletePtr++ = *ptr;
			}

			*pLineValueCompletePtr++ = 0;

			// Add the string to the table
			iContentIndex = -1;
			for (int i = 0; i < pStringTable.Size(); i++)
			{
				wchar_t* pString = pStringTable[i];

				if (wcscmp(pString, pLineValueComplete))
					continue;

				iContentIndex = i;
				break;
			}

			if (iContentIndex == -1)
				iContentIndex = pStringTable.Insert(wcsdup(pLineValueComplete));

			PXLANG_COMPILER_STRING* pNewString = (PXLANG_COMPILER_STRING*)malloc(sizeof(PXLANG_COMPILER_STRING));

			pNewString->group_idx  = iCurrentGroup;
			pNewString->name_idx   = iNameIndex;
			pNewString->string_idx = iContentIndex;

			pLocalizationStringTable.Insert(pNewString);
		}
		else if (IS_COMMAND(LANGUAGE_NAME))
		{
			// Set language name
			char* pNameOut = pLangName;
			for (ptr = pLineValue; *ptr != L'\0'; ptr++)
				*pNameOut++ = (char)*ptr;

			*pNameOut++ = 0;
		}
		else if (IS_COMMAND(LOCALIZATION_AUTHOR))
		{
			// Set localization author
		}
		else if (IS_COMMAND(LOCALIZATION_DATE))
		{
			// Set localization date
		}
		else if (IS_COMMAND(LOCALIZATION_VERSION))
		{
			// Set localization version
		}
		else if (IS_COMMAND(LOCALIZATION_FONT_SIZE))
		{
			// Set the font size (default is (CAVESTORY_TEXT_SIZE / 2)x(CAVESTORY_TEXT_SIZE))
			wcstombs(pTempBuffer, pLineValue, sizeof(pTempBuffer));
			sscanf(pTempBuffer, "%d %d", &iFontWidth, &iFontHeight);
		}
		else if (IS_COMMAND(LOCALIZATION_FONT))
		{
			// Set localization font
			wcstombs(pLangFont, pLineValue, wcslen(pLineValue) + 1);
		}
		else if (IS_COMMAND(LOCALIZATION_ENCODING))
		{
			// Set encoding
			wcstombs(pTempBuffer, pLineValue, sizeof(pTempBuffer));

			if ((iEncodingType = GetGameFontEncoding(pTempBuffer)) == -1)
			{
				printf("ERROR: CSM_PxLang_Compile() - Invalid encoding type \"%S\" (Line # %d)!\r\n", pLineValue, iLineNo);
				iReturnValue = -12;
				goto FREE_EVERYTHING;
			}
		}
		else if (IS_COMMAND(LOCALIZATION_CHARSET))
		{
			// Set charset
			wcstombs(pTempBuffer, pLineValue, sizeof(pTempBuffer));

			if ((iCharsetType = GetGameFontCharset(pTempBuffer)) == -1)
			{
				printf("ERROR: CSM_PxLang_Compile() - Invalid charset type \"%S\" (Line # %d)!\r\n", pLineValue, iLineNo);
				iReturnValue = -11;
				goto FREE_EVERYTHING;
			}
		}
		else if (IS_COMMAND(LANGUAGE_HIDDEN))
		{
			// Set hidden
			bLangHidden = true;
		}
		else if (IS_COMMAND(GROUP_BEGIN))
		{
			// Begin group
			if (!wcscmp(pLineValue, L"GLOBAL"))
			{
				printf("ERROR: CSM_PxLang_Compile() - Group name \"GLOBAL\" is reserved (Line # %d)!\r\n", iLineNo);
				iReturnValue = -4;
				goto FREE_EVERYTHING;
			}

			if (*pLineValue == L'\0')
			{
				printf("ERROR: CSM_PxLang_Compile() - Expected a group name (Line # %d)!\r\n", iLineNo);
				iReturnValue = -5;
				goto FREE_EVERYTHING;
			}
			
			if (iCurrentGroup != 0)
			{
				printf("ERROR: CSM_PxLang_Compile() - Groups cannot be nested within eachother (Line # %d)!\r\n", iLineNo);
				iReturnValue = -6;
				goto FREE_EVERYTHING;
			}

			unsigned int iGroupCrc = StringCRC32(pLineValue, sizeof(wchar_t) * wcslen(pLineValue));

			if ((iNameIndex = pIntTable.FindIndex(iGroupCrc)) == -1)
				iNameIndex = pIntTable.Insert(iGroupCrc);

			if ((iCurrentGroup = pGroupTable.FindIndex(iNameIndex)) == -1)
				iCurrentGroup = pGroupTable.Insert(iNameIndex);
		}
		else if (IS_COMMAND(GROUP_END))
		{
			// End group
			if (iCurrentGroup == 0)
			{
				printf("ERROR: CSM_PxLang_Compile() - We're not in a group, so we cannot close it (Line # %d)!\r\n", iLineNo);
				iReturnValue = -8;
				goto FREE_EVERYTHING;
			}

			iCurrentGroup = 0;
		}
		else
		{
			int iLength = 0;
			for (ptr = line; *ptr != L'\0'; ptr++)
			{
				if (*ptr == L'\t' || *ptr == L' ')
					break;

				++iLength;
			}

			char pFormat[128];
			sprintf(pFormat, "ERROR: CSM_PxLang_Compile() - Unknown command \"%%.%dS\" (Line # %d)!\r\n", iLength, iLineNo);
			printf(pFormat, line);

			iReturnValue = -3;
			goto FREE_EVERYTHING;
		}
	}

	if (iCurrentGroup != 0)
	{
		printf("ERROR: CSM_PxLang_Compile() - Expected 'GROUP_END' but got 'EOF' instead (Line # %d)!\r\n", iLineNo);
		iReturnValue = -9;
		goto FREE_EVERYTHING;
	}

	// Go back to the beginning
	fseek(fp, 0, SEEK_SET);

	// Write the compiled file
	pStream.Write(PXLANG_MAGIC, (sizeof(PXLANG_MAGIC) - 1) << 3);
	pStream.WriteInt(PXLANG_VERSION, sizeof(unsigned char) << 3);
	pStream.WriteFlag(bLangHidden);
	pStream.WriteInt(iEncodingType, sizeof(char) << 3);
	pStream.WriteInt(iCharsetType, sizeof(char) << 3);
	pStream.WriteInt(iFontWidth, sizeof(short) << 3);
	pStream.WriteInt(iFontHeight, sizeof(short) << 3);
	pStream.WriteString(pLangName);
	pStream.WriteString(pLangFont);
	pStream.WriteInt(iUncompiledFileCrc, sizeof(unsigned int) << 3);
	pStream.WriteString(pFileName, 260);
	pStream.WriteInt(pStringTable.Size(), sizeof(unsigned int) << 3);
	pStream.WriteInt(pIntTable.Size(), sizeof(unsigned int) << 3);

	// Write the int table
	for (List<long long int>::iterator it = pIntTable.begin(); it != pIntTable.end(); it++)
	{
		long long int pIntObj = *it;

		pStream.WriteLLInt(pIntObj, sizeof(unsigned long long int) << 3);
	}

	// Write the string table
	for (List<wchar_t*>::iterator it = pStringTable.begin(); it != pStringTable.end(); it++)
	{
		wchar_t* pStringObj = *it;

		pStream.WriteString(pStringObj, 512);
	}

	// Now write the instruction set
	for (int i = 0; i < pGroupTable.Size(); i++)
	{
		unsigned int iGroupNumIndex = pGroupTable[i];
		int iLocalizedStringCount   = 0;

		for (List<PXLANG_COMPILER_STRING*>::iterator cit = pLocalizationStringTable.begin(); cit != pLocalizationStringTable.end(); cit++)
		{
			PXLANG_COMPILER_STRING* pStrObj = *cit;

			// Skip mismatches
			if (pStrObj->group_idx == i)
				++iLocalizedStringCount;
		}

		if (!iLocalizedStringCount)
			continue;

		pStream.WriteRangedInt(PxLangOpCode::PXL_OP_GRP, 0, PxLangOpCode::PXL_MAX_OP);
		pStream.WriteRangedInt(iGroupNumIndex, 0, pIntTable.Size());
		pStream.WriteInt(iLocalizedStringCount, sizeof(unsigned short) << 3);
		
		// Write all localized strings that belong to this group
		for (List<PXLANG_COMPILER_STRING*>::iterator cit = pLocalizationStringTable.begin(); cit != pLocalizationStringTable.end(); cit++)
		{
			PXLANG_COMPILER_STRING* pStrObj = *cit;
		
			// Skip mismatches
			if (pStrObj->group_idx != i)
				continue;

			pStream.WriteRangedInt(PxLangOpCode::PXL_OP_STR, 0, PxLangOpCode::PXL_MAX_OP);
			pStream.WriteRangedInt(pStrObj->name_idx, 0, pIntTable.Size());
			pStream.WriteRangedInt(pStrObj->string_idx, 0, pStringTable.Size());
		}
	}

	if (!pStream.SaveToFile(pCompiledFile, false))
	{
		printf("ERROR: CSM_PxLang_Compile() - Failed to save compiled result to \"%s\"!\r\n", pCompiledFile);
		iReturnValue = -10;
	}

FREE_EVERYTHING:
	for (List<PXLANG_COMPILER_STRING*>::iterator it = pLocalizationStringTable.begin(); it != pLocalizationStringTable.end(); it++)
	{
		PXLANG_COMPILER_STRING* pStrObj = *it;
		free(pStrObj);
	}

	for (List<wchar_t*>::iterator it = pStringTable.begin(); it != pStringTable.end(); it++)
	{
		wchar_t* pString = *it;
		free(pString);
	}

	pStringTable.Free();
	pIntTable.Free();

	// Done
	fclose(fp);

	return iReturnValue;
}

int CSM_PxLang_Load(PXLANG_RESOURCE_STRUCT* pLangObj, const char* pFileName, const char* pGroupName, bool bRecompile)
{
	if (!pLangObj)
		return 1;

	SharedBitStream pStream;

	if (pLangObj->valid_crc == PXLANG_VALID_CRC)
		CSM_PxLang_Free(pLangObj);

	// Initialize the language object
	memset(pLangObj, 0, sizeof(PXLANG_RESOURCE_STRUCT));
	pLangObj->valid_crc  = PXLANG_VALID_CRC;
	pLangObj->path_name  = (char*)malloc(260);
	*pLangObj->path_name = 0;

	GetFullPathNameA(pFileName, 260, pLangObj->path_name, NULL);

	if (!*pLangObj->path_name)
		strcpy(pLangObj->path_name, pFileName);

	if (strrchr(pLangObj->path_name, '/'))
		*strrchr(pLangObj->path_name, '/') = 0;
	if (strrchr(pLangObj->path_name, '\\'))
		*strrchr(pLangObj->path_name, '\\') = 0;

	// If a group name is provided, then set it.
	if (pGroupName)
	{
		wchar_t* pGroupChar = new wchar_t[strlen(pGroupName) + 1];
		wchar_t* pptr = pGroupChar;

		for (const char* ptr = pGroupName; *ptr != 0; ptr++)
			*pptr++ = *ptr;

		*pptr++ = 0;
		pLangObj->selected_group_crc = StringCRC32(pGroupChar, sizeof(wchar_t) * wcslen(pGroupChar));
		delete[] pGroupChar;
	}

	const char* pptr;
	char pCompiledFile[260];

	// Validate the filename
	if ((pptr = strrchr(pFileName, '.')) != NULL && !strcmpi(pptr, ".pxlang"))
	{
		// Not a compiled file; Check to see if we need to compile it!!
		strcpy(pCompiledFile, pFileName);
		pCompiledFile[pptr - pFileName] = 0;
		strcat(pCompiledFile, ".pxclang");

		// If we can't open the compiled file, then we'll have to compile it ourselves.
		// If we CAN'T compile it, then error.
#ifdef ALWAYS_RECOMPILE_PXLANG
		if (CSM_PxLang_Compile(pFileName) || !pStream.LoadFromFile(pCompiledFile, false))
#else
		if (!pStream.LoadFromFile(pCompiledFile, false) && (CSM_PxLang_Compile(pFileName) || !pStream.LoadFromFile(pCompiledFile, false)))
#endif
		{
			printf("ERROR: CSM_PxLang_Load() - Failed to load \"%s\" (Compile failure / File error).\r\n", pFileName);
			return -1;
		}
	}
	else
	{
		strcpy(pCompiledFile, pFileName);

		if (!pStream.LoadFromFile(pCompiledFile, false))
		{
			printf("ERROR: CSM_PxLang_Load() - Failed to load \"%s\" (Compile failure / File error).\r\n", pFileName);
			return -1;
		}
	}

	// Check the magic
	if (!pStream.mBuffer || memcmp(pStream.mBuffer, PXLANG_MAGIC, sizeof(PXLANG_MAGIC) - 1))
	{
		printf("ERROR: CSM_PxLang_Load() - Failed to load \"%s\" (Invalid magic)\r\n", pFileName);
		return -2;
	}

	unsigned char iVersion = pStream.mBuffer[sizeof(PXLANG_MAGIC) - 1];

	if (iVersion != PXLANG_VERSION)
	{
		if (bRecompile)
		{
			CSM_PxLang_Compile(pFileName);
			return CSM_PxLang_Load(pLangObj, pCompiledFile, pGroupName, false);
		}

		printf("ERROR: CSM_PxLang_Load() - Failed to load \"%s\" (Version mismatch)\r\n", pFileName);
		return -3;
	}

	// Start reading
	pStream.SetCurPos(((sizeof(PXLANG_MAGIC) - 1) + 1) << 3);
	pLangObj->hidden        = pStream.ReadFlag();
	pLangObj->encoding_type = pStream.ReadInt(sizeof(char) << 3);
	pLangObj->charset_type  = pStream.ReadInt(sizeof(char) << 3);
	pLangObj->font_width    = pStream.ReadInt(sizeof(short) << 3);
	pLangObj->font_height   = pStream.ReadInt(sizeof(short) << 3);
	pStream.ReadAllocateString(pLangObj->display_name);
	pStream.ReadAllocateString(pLangObj->font_name);
	unsigned int iOriginalCRC = pStream.ReadInt(sizeof(unsigned int) << 3);
	pStream.ReadAllocateString(pLangObj->file_name);

	if (pLangObj->file_name && bRecompile)
	{
		FILE* fp;
		if ((fp = fopen(pFileName, "r")) != NULL)
		{
			fseek(fp, 0, SEEK_END);
			unsigned long long int iFileSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			char* pEntireFile = (char*)malloc(sizeof(char) * iFileSize);
			fread(pEntireFile, iFileSize, 1, fp);
			fseek(fp, 0, SEEK_SET);
			unsigned int iUncompiledFileCrc = StringCRC32(pEntireFile, iFileSize);
			free(pEntireFile);
			fclose(fp);

			if (iUncompiledFileCrc != iOriginalCRC)
			{
				CSM_PxLang_Compile(pLangObj->file_name);
				return CSM_PxLang_Load(pLangObj, pCompiledFile, pGroupName, false);
			}
		}
	}

	pLangObj->string_table_size = pStream.ReadInt(sizeof(unsigned int) << 3);
	unsigned int iIntTableSize  = pStream.ReadInt(sizeof(unsigned int) << 3);

	// Allocate tables
	pLangObj->string_table   = (wchar_t**)malloc(sizeof(wchar_t*) * pLangObj->string_table_size);
	long long int* pIntTable = (long long int*)malloc(sizeof(long long int) * iIntTableSize);

	// Read the int table
	for (int i = 0; i < iIntTableSize; i++)
		pIntTable[i] = pStream.ReadIntLL(sizeof(unsigned long long int) << 3);

	// Read the string table
	for (int i = 0; i < pLangObj->string_table_size; i++)
		pStream.ReadAllocateString(pLangObj->string_table[i], 512);

	PXLANG_GROUP_STRUCT* pGroup = NULL;
	unsigned int iGlobalCRC     = StringCRC32(L"GLOBAL", sizeof(wchar_t) * wcslen(L"GLOBAL"));

	// Loop through the file until we're done
	while (pStream.GetCurPos() < pStream.mBufferLen << 3)
	{
		PxLangOpCode OP_CODE = (PxLangOpCode)pStream.ReadRangedInt(0, PxLangOpCode::PXL_MAX_OP);

		// Determine the instruction
		switch (OP_CODE)
		{
			case PxLangOpCode::PXL_OP_NUL:
			{
				break;
			}
			case PxLangOpCode::PXL_OP_GRP:
			{
				int iGroupIdx           = pStream.ReadRangedInt(0, iIntTableSize);
				unsigned int iGroupCRC  = pIntTable[iGroupIdx];
				unsigned int iGroupSize = pStream.ReadInt(sizeof(unsigned short) << 3);
				unsigned int iBitSize   = iGroupSize * (SharedBitStream::GetRangedIntBits(0, PxLangOpCode::PXL_MAX_OP) + SharedBitStream::GetRangedIntBits(0, iIntTableSize) + SharedBitStream::GetRangedIntBits(0, pLangObj->string_table_size));

				// Allocate a new slot for the group
				if (!pLangObj->group_list)
				{
					pLangObj->group_count = 1;
					pLangObj->group_list  = (PXLANG_GROUP_STRUCT*)malloc(sizeof(PXLANG_GROUP_STRUCT) * pLangObj->group_count);
				}
				else
				{
					pLangObj->group_count += 1;
					pLangObj->group_list   = (PXLANG_GROUP_STRUCT*)realloc((void*)pLangObj->group_list, sizeof(PXLANG_GROUP_STRUCT) * pLangObj->group_count);
				}

				pGroup               = &pLangObj->group_list[pLangObj->group_count - 1];
				pGroup->crc          = iGroupCRC;
				pGroup->loaded       = (pLangObj->selected_group_crc == iGroupCRC) || (iGroupCRC == iGlobalCRC);
				pGroup->string_size  = iGroupSize;
				pGroup->string_count = 0;
				pGroup->string_list  = (!pGroup->loaded ? NULL : (PXLANG_STRING_STRUCT*)malloc(sizeof(PXLANG_STRING_STRUCT) * pGroup->string_size));

				if (pGroup->string_list)
					memset(pGroup->string_list, 0, sizeof(PXLANG_STRING_STRUCT) * pGroup->string_size);

				// Skip the group if we're not selecting this one
				if (!pGroup->loaded)
				{
					pStream.SetCurPos(pStream.GetCurPos() + iBitSize);
					break;
				}

				break;
			}
			case PxLangOpCode::PXL_OP_STR:
			{
				if (!pGroup)
					continue;

				PXLANG_STRING_STRUCT* pString = &pGroup->string_list[pGroup->string_count++];

				unsigned int iNameIndex   = pStream.ReadRangedInt(0, iIntTableSize);
				unsigned int iStringIndex = pStream.ReadRangedInt(0, pLangObj->string_table_size);

				pString->crc     = pIntTable[iNameIndex];
				pString->content = pLangObj->string_table[iStringIndex];

				break;
			}
		}
	}

	if (PXLANG_RESOURCE_STRUCT::first)
		PXLANG_RESOURCE_STRUCT::first->prev = pLangObj;

	pLangObj->prev                = NULL;
	pLangObj->next                = PXLANG_RESOURCE_STRUCT::first;
	PXLANG_RESOURCE_STRUCT::first = pLangObj;
	free(pIntTable);

	return 0;
}

BOOL CSM_PxLang_Free(PXLANG_RESOURCE_STRUCT* pLangObj)
{
	if (!pLangObj)
		return FALSE;

	if (pLangObj->valid_crc != PXLANG_VALID_CRC)
		return TRUE;

	if (pLangObj->display_name)
		free(pLangObj->display_name);

	if (pLangObj->file_name)
		free(pLangObj->file_name);
	
	if (pLangObj->font_name)
		free(pLangObj->font_name);
	
	if (pLangObj->path_name)
		free(pLangObj->path_name);

	for (int i = 0; i < pLangObj->group_count; i++)
	{
		PXLANG_GROUP_STRUCT* pGroup = &pLangObj->group_list[i];

		if (pGroup->string_list)
		{
			// Free ascii strings.
			for (int j = 0; j < pGroup->string_count; j++)
				if (pGroup->string_list[j].content_ascii)
					free(pGroup->string_list[j].content_ascii);

			free(pGroup->string_list);
		}
	}

	for (int i = 0; i < pLangObj->string_table_size; i++)
		free(pLangObj->string_table[i]);

	if (pLangObj->string_table)
		free(pLangObj->string_table);

	if (PXLANG_RESOURCE_STRUCT::first == pLangObj)
		PXLANG_RESOURCE_STRUCT::first = pLangObj->next;

	if (pLangObj->next)
		pLangObj->next->prev = pLangObj->prev;
	
	if (pLangObj->prev)
		pLangObj->prev->next = pLangObj->next;

	pLangObj->valid_crc = 0;

	return FALSE;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CSM_PxLang_IsLoaded(PXLANG_RESOURCE_STRUCT* pLangObj)
{
	return ((pLangObj && pLangObj->valid_crc == PXLANG_VALID_CRC) ? TRUE : FALSE);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_endcode.h>