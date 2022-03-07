#include "stdafx.h"
#include "CaveEditor.h"
#include "ModPackerDlg.h"
#include "afxdialogex.h"
#include "General.h"
#include "direct.h"
#include "ErrorWindow.h"

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define PACKER_OPTIONS_VERSION 1

IMPLEMENT_DYNAMIC(ModPackerDlg, CDialog)

BEGIN_MESSAGE_MAP(ModPackerDlg, CDialog)
	ON_BN_CLICKED(BUTTON_SELECT_OUT_DIR, OnSelectOutDir)
	ON_BN_CLICKED(BUTTON_PACK_MOD, OnPackMod)
END_MESSAGE_MAP()

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool DoesPathExist(const char* dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ModPackerDlg::ModPackerDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(DIALOG_MOD_PACKER, pParent)
{

}

ModPackerDlg::~ModPackerDlg()
{
	m_HelpFont.DeleteObject();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ModPackerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL ModPackerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CWnd* pWnd;
	CFont* pFont;
	LOGFONT lf;
	
	// Set help text as italic
	pWnd  = GetDlgItem(TEXT_HELP_0);
	pFont = pWnd->GetFont();
	pFont->GetLogFont(&lf);

	// Configure the log font
	lf.lfItalic = TRUE;

	m_HelpFont.CreateFontIndirectA(&lf);

	// Set italics for all help text
	pWnd = GetDlgItem(TEXT_HELP_0);
	pWnd->SetFont(&m_HelpFont);
	pWnd = GetDlgItem(TEXT_HELP_1);
	pWnd->SetFont(&m_HelpFont);

	ImportPackerOptions();

	return TRUE;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ModPackerDlg::OnSelectOutDir()
{
	BROWSEINFO bi     = { 0 };
	bi.lpszTitle      = _T("Select Output Path");
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl != 0)
	{
		// get the name of the folder
		TCHAR path[MAX_PATH];
		SHGetPathFromIDList(pidl, path);

		SetDlgItemText(EDIT_OUT_PATH, path);

		// free memory used
		IMalloc* imalloc = 0;
		if (SUCCEEDED(SHGetMalloc(&imalloc)))
		{
			imalloc->Free(pidl);
			imalloc->Release();
		}
	}
}

#define LOG_MSG(FMT, ...)		ProgWindow.AddContent("            " FMT "\r\n", __VA_ARGS__);
#define LOG_ERROR(FMT, ...)		ProgWindow.AddContent("[  ERROR  ] " FMT "\r\n", __VA_ARGS__);
#define LOG_WARN(FMT, ...)		ProgWindow.AddContent("[ WARNING ] " FMT "\r\n", __VA_ARGS__);
#define INCREMENT_PROGRESS()	ProgWindow.SetProgress(++iProgress);

#define COPY_MOD_FILE(MODFILEFMT, ...) \
	{\
		sprintf(pFrom, "%s/" MODFILEFMT, exe.mod.mModPath, __VA_ARGS__); \
		sprintf(pTo, "%s/" MODFILEFMT, pOutDir, __VA_ARGS__);\
		LOG_MSG("Copy \"" MODFILEFMT "\"", __VA_ARGS__);\
		if (!CopyFileA(pFrom, pTo, FALSE))\
		{\
			LOG_ERROR("Failed to copy \"" MODFILEFMT "\"!", __VA_ARGS__);\
			goto DONE_WITH_EXPORT;\
		}\
	}
#define COPY_MOD_FILE_OPTIONAL(MODFILEFMT, ...) \
	sprintf(pFrom, "%s/" MODFILEFMT, exe.mod.mModPath, __VA_ARGS__); \
	if (PathFileExists(pFrom))\
	{\
		sprintf(pTo, "%s/" MODFILEFMT, pOutDir, __VA_ARGS__);\
		LOG_MSG("Copy \"" MODFILEFMT "\"", __VA_ARGS__);\
		if (!CopyFileA(pFrom, pTo, FALSE))\
		{\
			LOG_WARN("Failed to copy \"" MODFILEFMT "\"!", __VA_ARGS__);\
		}\
	}

#define CREATE_DIRECTORY(DIRNAME) \
	strcpy(pBuffer, pOutDir); strcat(pBuffer, "/" DIRNAME); _mkdir(pBuffer);

#define CREATE_DIRECTORY_IF_EXISTS(DIRNAME)		\
	strcpy(pBuffer, exe.mod.mModPath);			\
	strcat(pBuffer, "/" DIRNAME);				\
	if (DoesPathExist(pBuffer))					\
	{											\
		strcpy(pBuffer, pOutDir);				\
		strcat(pBuffer, "/" DIRNAME);			\
		_mkdir(pBuffer);						\
	}

#define DOES_MOD_FILE_EXIST(FORMAT, ...)			(sprintf(pBuffer, "%s/" FORMAT, exe.mod.mModPath, __VA_ARGS__) >= 0 && PathFileExists(pBuffer) == TRUE)

void ModPackerDlg::OnPackMod()
{
	// Export options first of all
	ExportPackerOptions();

	// Get options
	ModConfigResource MCR;
	char pFrom[MAX_PATH];
	char pTo[MAX_PATH];
	char pBuffer[1024];
	char pBuffer2[1024];
	char pOutDir[MAX_PATH];
	bool bIncludeDbgMod         = (bool)IsDlgButtonChecked(CHECK_INCLUDE_DEBUG_MOD);
	bool bIncludeUncompiledMaps = (bool)IsDlgButtonChecked(CHECK_INCLUDE_UNCOMPILED_MAPS);
	GetDlgItemText(EDIT_OUT_PATH, pOutDir, sizeof(pOutDir));

	// Replace \ with /
	char* pBadChr = pOutDir;
	while ((pBadChr = strchr(pBadChr, '\\')) != NULL)
		*pBadChr = '/';

	// Make sure it doesn't end with a slash
	int iStrLen = strlen(pOutDir);
	if (pOutDir[iStrLen - 1] == '/')
		pOutDir[iStrLen - 1] = 0;

	ErrorWindow ProgWindow;

	// 0 : Compile release mod
	// 1 : Compile maps & copy map files
	// 2 : Copy tileset PNGs & PXAs
	// 3 : Copy backgrounds
	// 4 : Copy spritesheets
	// 5 : Copy head scripts
	// 6 : Copy music

	ProgWindow.SetProgressBarShown(true);
	ProgWindow.SetProgressMinMax(0, exe.mod.mTilesets.count + exe.mod.mStages.count + exe.mod.mSpritesheets.count + exe.mod.mBackgrounds.count + exe.mod.mMusic.count);
	ProgWindow.SetProgress(0);
	ProgWindow.SetTitle("Export Mod");
	ProgWindow.SetDescription("Exporting mod...");
	ProgWindow.SetButtonEnabled_Cancel(true);
	ProgWindow.SetButtonEnabled_OK(false);
	ProgWindow.SetShown(true);
	ProgWindow.Process();

	// Check that the directory exists
	if (!DoesPathExist(pOutDir))
	{
		LOG_WARN("Path \"%s\" does not exist", pOutDir);
		int iResult = MessageBox("The output directory does not exist!\n\nDo you want to create it?", "CaveEditor 2.0", MB_YESNO);
		if (iResult != IDYES)
		{
			LOG_ERROR("Packer operation canceled by user", pOutDir);
			goto DONE_WITH_EXPORT;
		}

		// Find the first slash
		char* pStart = strchr(pOutDir, '/');

		// Skip volume label
		if (pStart && pStart != pOutDir && *(pStart - 1) == ':')
			++pStart;

		// Create the subpaths
		while ((pStart = strchr(pStart, '/')) != NULL)
		{
			*pStart = 0;

			if (!DoesPathExist(pOutDir) && !CreateDirectoryA(pOutDir, NULL))
			{
				LOG_ERROR("Failed creating path \"%s\"", pOutDir);
				goto DONE_WITH_EXPORT;
			}

			*pStart = '/';
			++pStart;
		}

		// Create the overall directory
		if (!CreateDirectoryA(pOutDir, NULL))
		{
			LOG_ERROR("Failed creating path \"%s\"", pOutDir);
			goto DONE_WITH_EXPORT;
		}
	}

	// Create paths
	CREATE_DIRECTORY("Npc");
	CREATE_DIRECTORY("Stage");
	CREATE_DIRECTORY("Music");
	CREATE_DIRECTORY("Saves");
	CREATE_DIRECTORY("Demos");

	// Create custom image paths
	if (1)
	{
		CREATE_DIRECTORY_IF_EXISTS("Image");

		int iPathNo = -1;
		while (true)
		{
			++iPathNo;

			// Build the path string
			sprintf(pBuffer, "%s/Image/%04d", exe.mod.mModPath, iPathNo);

			printf("look for path %s\r\n", pBuffer);

			// If it doesn't exist, then stop here.
			if (!DoesPathExist(pBuffer))
				break;

			sprintf(pBuffer, "%s/Image/%04d", pOutDir, iPathNo);
			_mkdir(pBuffer);

			// Copy all image files over
			int iFileNo = -1;
			while (true)
			{
				++iFileNo;

				// Build the file path string
				sprintf(pBuffer, "%s/Image/%04d/%04d.png", exe.mod.mModPath, iPathNo, iFileNo);
				
				// If it doesn't exist, then stop here.
				if (!PathFileExists(pBuffer))
					break;

				// Copy the file!
				COPY_MOD_FILE("Image/%04d/%04d.png", iPathNo, iFileNo);
			}
		}
	}

	// Copy sounds
	if (1)
	{
		CREATE_DIRECTORY_IF_EXISTS("Sound");

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;

		sprintf(pBuffer2, "%s/Sound/*", exe.mod.mModPath);

		hFind = FindFirstFile(pBuffer2, &FindFileData);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			// Find every file
			while (FindNextFile(hFind, &FindFileData) != 0)
			{
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					continue;

				const char* pFileExt = strrchr(FindFileData.cFileName, '.');

				// Skip files w/ no file extension
				if (!pFileExt)
					continue;

				if (
					!stricmp(pFileExt, ".wav") ||
					!stricmp(pFileExt, ".mp3") ||
					!stricmp(pFileExt, ".ogg") ||
					!stricmp(pFileExt, ".ptcop") ||
					!stricmp(pFileExt, ".ptnoise") ||
					!stricmp(pFileExt, ".pxt") ||
					!stricmp(pFileExt, ".wav")
				   )
				{
					COPY_MOD_FILE("Sound/%s", FindFileData.cFileName);
				}
				else
				{
					LOG_WARN("File \"%s\" has unknown file extension '%s'", FindFileData.cFileName, (strrchr(FindFileData.cFileName, '.') == NULL ? "<null>" : strrchr(FindFileData.cFileName, '.')));
				}
			}

			// Make sure we close this
			FindClose(hFind);
		}
	}

	int iProgress = 0;

	// Save the mod as resource
	if (!exe.mod.Save(true))
	{
		LOG_ERROR("Failed to compile mod for release!");
		goto DONE_WITH_EXPORT;
	}

	// Force load the release mod
	if (!MCR.Load(exe.mod.mModPath, false, true, false))
	{
		LOG_ERROR("Failed to load the release version!");
		goto DONE_WITH_EXPORT;
	}

	// Copy debug mod
	if (bIncludeDbgMod)
	{
		COPY_MOD_FILE("dbg-mod.pxmod");
	}

	// Copy mod
	COPY_MOD_FILE("mod.pxmod");
	
	// Copy stage resources
	for (int i = 0; i < MCR.mStages.count; i++)
	{
		PXMOD_STAGE_STRUCT* pStage = &MCR.mStages.list[i];

		// Log this
		LOG_MSG("Compiling \"%s\"...", pStage->name);

		// Compile maps
		if (!exe.CompileMap(i, false))
		{
			LOG_ERROR("Failed compiling map \"%s\"!", pStage->name);
			INCREMENT_PROGRESS();
			ProgWindow.Process();
			continue;
		}

		// Copy map files
		if (bIncludeUncompiledMaps)
		{
			COPY_MOD_FILE("Stage/%s.pxm", pStage->file);
			COPY_MOD_FILE("Stage/%s.pxe", pStage->file);
			COPY_MOD_FILE("Stage/%s.pxn", pStage->file);
			COPY_MOD_FILE("Stage/%s.tsc", pStage->file);
			COPY_MOD_FILE_OPTIONAL("Stage/%s.pxpr", pStage->file);
		}

		// Copy the compiled map
		COPY_MOD_FILE("Stage/%s.pxcm", pStage->file);
		INCREMENT_PROGRESS();
		ProgWindow.Process();
	}

	// Copy backgrounds
	for (int i = 0; i < MCR.mBackgrounds.count; i++)
	{
		PXMOD_BACKGROUND_STRUCT* pBack = &MCR.mBackgrounds.list[i];
		COPY_MOD_FILE("bk%s.png", pBack->name);
		INCREMENT_PROGRESS();
		ProgWindow.Process();
	}

	// Copy spritesheets
	for (int i = 0; i < MCR.mSpritesheets.count; i++)
	{
		PXMOD_SPRITESHEET_STRUCT* pSheet = &MCR.mSpritesheets.list[i];
		COPY_MOD_FILE("Npc/Npc%s.png", pSheet->name);
		INCREMENT_PROGRESS();
		ProgWindow.Process();
	}
	
	// Copy tilesets
	for (int i = 0; i < MCR.mTilesets.count; i++)
	{
		PXMOD_TILESET_STRUCT* pTileset = &MCR.mTilesets.list[i];
		COPY_MOD_FILE("Stage/Prt%s.png", pTileset->name);
		COPY_MOD_FILE("Stage/%s.pxa", pTileset->name);
		INCREMENT_PROGRESS();
		ProgWindow.Process();
	}
	
	// Copy music
	for (int i = 0; i < MCR.mMusic.count; i++)
	{
		PXMOD_MUSIC_STRUCT* pMusic = &MCR.mMusic.list[i];

		if (DOES_MOD_FILE_EXIST("Music/%s.wav", pMusic->name))
		{
			COPY_MOD_FILE("Music/%s.wav", pMusic->name);
		}
		else if (DOES_MOD_FILE_EXIST("Music/%s.ogg", pMusic->name))
		{
			COPY_MOD_FILE("Music/%s.ogg", pMusic->name);
		}
		else if (DOES_MOD_FILE_EXIST("Music/%s.mp3", pMusic->name))
		{
			COPY_MOD_FILE("Music/%s.mp3", pMusic->name);
		}
		else if (DOES_MOD_FILE_EXIST("Music/%s.ptcop", pMusic->name))
		{
			COPY_MOD_FILE("Music/%s.ptcop", pMusic->name);
		}
		else if (DOES_MOD_FILE_EXIST("Music/%s.org", pMusic->name))
		{
			COPY_MOD_FILE("Music/%s.org", pMusic->name);
		}

		INCREMENT_PROGRESS();
		ProgWindow.Process();
	}

	// Copy common images
	COPY_MOD_FILE_OPTIONAL("Npc/NpcSym.png");
	COPY_MOD_FILE_OPTIONAL("Arms.png");
	COPY_MOD_FILE_OPTIONAL("ArmsImage.png");
	COPY_MOD_FILE_OPTIONAL("ArmsItem.tsc");
	COPY_MOD_FILE_OPTIONAL("Bullet.png");
	COPY_MOD_FILE_OPTIONAL("Caret.png");
	COPY_MOD_FILE_OPTIONAL("casts.png");
	COPY_MOD_FILE_OPTIONAL("Credit.tsc");
	COPY_MOD_FILE_OPTIONAL("Face.png");
	COPY_MOD_FILE_OPTIONAL("Fade.png");
	COPY_MOD_FILE_OPTIONAL("Head.tsc");
	COPY_MOD_FILE_OPTIONAL("ItemImage.png");
	COPY_MOD_FILE_OPTIONAL("Loading.png");
	COPY_MOD_FILE_OPTIONAL("MyChar.png");
	COPY_MOD_FILE_OPTIONAL("StageImage.png");
	COPY_MOD_FILE_OPTIONAL("StageSelect.tsc");
	COPY_MOD_FILE_OPTIONAL("TextBox.png");
	COPY_MOD_FILE_OPTIONAL("Title.png");
	COPY_MOD_FILE_OPTIONAL("%s.dll", exe.mod.GetInternalName());

DONE_WITH_EXPORT:
	MCR.Reset();
	ProgWindow.SetProgressBarShown(false);
	ProgWindow.SetDescription("Finished packing your mod!");
	ProgWindow.SetButtonEnabled_OK(true);

	// Wait forever until they press OK.
	while (!ProgWindow.WasOkPressed() && !ProgWindow.WasCancelPressed())
		ProgWindow.Process();

	ProgWindow.SetShown(false);

	CDialog::OnOK();
}

#undef COPY_MOD_FILE
#undef COPY_MOD_FILE_OPTIONAL
#undef LOG_WARN
#undef LOG_ERROR
#undef LOG_MSG

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ModPackerDlg::ImportPackerOptions()
{
	char pOutDir[260];
	short iStrLen               = 0;
	int iVersion                = 0;
	bool bIncludeDbgMod         = false;
	bool bIncludeUncompiledMaps = false;
	CFile ConfigFile;

	sprintf(pOutDir, "%s/OUT/", exe.mod.mModPath);

	if (ConfigFile.Open("ModPacker.cfg", CFile::OpenFlags::modeRead))
	{
		ConfigFile.Read(&iVersion, sizeof(iVersion));
		ConfigFile.Read(&iStrLen, sizeof(iStrLen));
		ConfigFile.Read(pOutDir, iStrLen);
		pOutDir[iStrLen] = 0;
		ConfigFile.Read(&bIncludeDbgMod, 1);
		ConfigFile.Read(&bIncludeUncompiledMaps, 1);
		ConfigFile.Close();
	}

	CheckDlgButton(CHECK_INCLUDE_DEBUG_MOD, (UINT)bIncludeDbgMod);
	CheckDlgButton(CHECK_INCLUDE_UNCOMPILED_MAPS, (UINT)bIncludeUncompiledMaps);
	SetDlgItemText(EDIT_OUT_PATH, pOutDir);

	if (exe.mod.mHeader.build_type == PXMOD_BUILD_TYPE::PXMOD_BUILD_RELEASE)
	{
		GetDlgItem(CHECK_INCLUDE_DEBUG_MOD)->EnableWindow(FALSE);
		CheckDlgButton(CHECK_INCLUDE_DEBUG_MOD, FALSE);
	}
}

void ModPackerDlg::ExportPackerOptions()
{
	char pOutDir[260];
	short iStrLen               = 0;
	int iVersion                = PACKER_OPTIONS_VERSION;
	bool bIncludeDbgMod         = (bool)IsDlgButtonChecked(CHECK_INCLUDE_DEBUG_MOD);
	bool bIncludeUncompiledMaps = (bool)IsDlgButtonChecked(CHECK_INCLUDE_UNCOMPILED_MAPS);
	CFile ConfigFile;

	// Set everything
	GetDlgItemText(EDIT_OUT_PATH, pOutDir, sizeof(pOutDir));
	iStrLen = strlen(pOutDir);
	
	if (!ConfigFile.Open("ModPacker.cfg", CFile::OpenFlags::modeCreate | CFile::OpenFlags::modeWrite))
		return;

	ConfigFile.Write(&iVersion, sizeof(iVersion));
	ConfigFile.Write(&iStrLen, sizeof(iStrLen));
	ConfigFile.Write(pOutDir, iStrLen);
	ConfigFile.Write(&bIncludeDbgMod, 1);
	ConfigFile.Write(&bIncludeUncompiledMaps, 1);
	ConfigFile.Close();
}