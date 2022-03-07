// CaveEditorDlg.cpp : implementation file
//


#include "stdafx.h"
#include "General.h"
#include "CaveEditor.h"
#include "CaveMapInfoDlg.h"
#include "CaveMapEditDlg.h"
#include "ScriptEditorDlg.h"
#include "PaletteSelectDlg.h"
#include "PBMEditorDlg.h"
#include "TilesetEditorDlg.h"
#include "WeaponEditorDlg.h"
#include "NpcExeEditorDlg.h"
#include "ReorderMapsDlg.h"

#include "NPCTableEditorDlg.h"

#include "CaveEditorDlg.h"
#include ".\caveeditordlg.h"
#include "CaveMapPropertiesDlg.h"
#include "SoundLoad.h"
#include "EventSelectorDialog.h"
#include "CCreateModDlg.h"
#include "CEditAreaDlg.h"
#include "DlgWeaponEditor.h"
#include "DlgBulletEditor.h"
#include "ErrorWindow.h"
#include "ModPackerDlg.h"

#include "Organya.h"

extern CCaveEditorDlg* mainWindow;
extern ScriptEditorDlg scriptEditorDlg;
//#include "SharedWindows.h"

#include "TextEntryDlg.h"
#include "OptionsDlg.h"
#include "GameSettingsDlg.h"

#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "Organya.h"



// CAboutDlg dialog used for App About


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
public:
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnStnClickedIdentifierText();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//	ON_WM_KEYDOWN()
ON_STN_CLICKED(VERSION_IDENTIFIER_TEXT, &CAboutDlg::OnStnClickedIdentifierText)
END_MESSAGE_MAP()


// CCaveEditorDlg dialog

CCaveEditorDlg::CCaveEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCaveEditorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	close = false;
	changes = false;
	AfxInitRichEdit2();
	mainWindow = this;

	CDC cdc;
	cdc.CreateCompatibleDC(NULL);

	LoadConfig_CaveEditor();

	CS_GRAPHIC_COLOR_DEPTH = cdc.GetDeviceCaps(BITSPIXEL);

}

void CCaveEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_MAPS, m_ListMaps);
	DDX_Control(pDX, LIST_TILESETS, m_ListTilesets);
	DDX_Control(pDX, LIST_SCRIPTS, m_ListScripts);
	DDX_Control(pDX, LIST_SPRITES, m_ListSpritesets);
	DDX_Control(pDX, LIST_BACKGROUNDS, m_ListBackgrounds);
	DDX_Control(pDX, LIST_AREAS, m_ListAreas);
	DDX_Control(pDX, LIST_MUSIC, m_ListMusic);
}

BEGIN_MESSAGE_MAP(CCaveEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_COMPILERELEASEMOD, OnFileCompileRelease)
	ON_COMMAND(ID_FILE_PACKMOD, OnFilePackMod)
	ON_COMMAND(ID_FILE_TEST, OnFileTest)
	ON_COMMAND(ID_FILE_SAVEMAPLIST, OnFileSaveMapList)
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_COMMAND(ID_DATA_COMPILE_MAPS, OnDataCompileMaps)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_BN_CLICKED(BUTTON_MAP_DELETE, OnBnClickedMapDelete)
	ON_BN_CLICKED(BUTTON_MAP_ADD, OnBnClickedMapAdd)
	//ON_BN_CLICKED(BUTTON_MAP_COPY, OnBnClickedMapCopy)
	ON_BN_CLICKED(BUTTON_MAP_EDIT, OnBnClickedMapEdit)
	ON_BN_CLICKED(BUTTON_TILESET_EDIT, OnBnClickedTilesetEdit)
	ON_BN_CLICKED(BUTTON_TILESET_COPY, OnBnClickedTilesetCopy)
	ON_BN_CLICKED(BUTTON_TILESET_DELETE, OnBnClickedTilesetDelete)
	ON_BN_CLICKED(BUTTON_SCRIPT_EDIT, OnBnClickedScriptEdit)
	ON_BN_CLICKED(BUTTON_NPCTABLE_EDIT, OnBnClickedNpctableEdit)
	ON_BN_CLICKED(BUTTON_GAME_SETTINGS, OnBnClickedGameSettings)
	ON_BN_CLICKED(BUTTON_SPRITES_ADD, OnBnClickedSpritesAdd)
	ON_BN_CLICKED(BUTTON_SPRITES_COPY, OnBnClickedSpritesCopy)
	ON_BN_CLICKED(BUTTON_SPRITES_EDIT, OnBnClickedSpritesEdit)
	ON_BN_CLICKED(BUTTON_SPRITES_DELETE, OnBnClickedSpritesDelete)
	ON_BN_CLICKED(BUTTON_BACKGROUND_ADD, OnBnClickedBackgroundAdd)
	ON_BN_CLICKED(BUTTON_BACKGROUND_COPY, OnBnClickedBackgroundCopy)
	ON_BN_CLICKED(BUTTON_BACKGROUND_EDIT, OnBnClickedBackgroundEdit)
	ON_BN_CLICKED(BUTTON_BACKGROUND_DELETE, OnBnClickedBackgroundDelete)
	ON_LBN_DBLCLK(LIST_MAPS, OnLbnDblclkMaps)
	ON_LBN_DBLCLK(LIST_TILESETS, OnLbnDblclkTilesets)
	ON_LBN_DBLCLK(LIST_SPRITES, OnLbnDblclkSprites)
	ON_LBN_DBLCLK(LIST_BACKGROUNDS, OnLbnDblclkBackgrounds)
	ON_LBN_DBLCLK(LIST_SCRIPTS, OnLbnDblclkScripts)
	ON_COMMAND(ID_MAPLIST_EDITMAP, OnMaplistEditmap)
	ON_COMMAND(ID_MAPLIST_EDITMAPPROPERTIES, OnMaplistEditmapproperties)
	ON_COMMAND(ID_MAPLIST_EDITMAPSCRIPT, OnMaplistEditmapscript)
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_TILESETLIST_EDITTILESET, OnTilesetlistEdittileset)
	ON_COMMAND(ID_SPRITESETLIST_EDITSPRITESET, OnSpritesetlistEditspriteset)
	ON_COMMAND(ID_BACKGROUNDLIST_EDITBACKGROUND, OnBackgroundlistEditbackground)
	ON_COMMAND(ID_SCRIPTLIST_EDITSCRIPT, OnScriptlistEditscript)
	ON_COMMAND(ID_AREALIST_EDITAREA, OnAreaListEdit)
	ON_COMMAND(ID_AREALIST_COPYAREA, OnAreaListCopy)
	ON_COMMAND(ID_AREALIST_DELETEAREA, OnAreaListDelete)
	ON_BN_CLICKED(ID_EDITGAMEPLAY_EDITBULLETS, OnEditBullets)
	ON_BN_CLICKED(ID_EDITGAMEPLAY_EDITWEAPONS, OnEditWeapons)
	ON_BN_CLICKED(ID_EDITGAMEPLAY_EDITHUD, OnEditHUD)
	ON_WM_DESTROY()
	ON_COMMAND(ID_MAPLIST_COPYMAP, OnMaplistCopymap)
	ON_COMMAND(ID_MAPLIST_ADDMAP, OnMaplistAddmap)
	ON_COMMAND(ID_MAPLIST_DELETEMAP, OnMaplistDeletemap)
	ON_COMMAND(ID_DATA_IMPORTMAPS, OnImportMaps)
	ON_COMMAND(ID_DATA_BULLETDATA, OnDataBulletdata)
	ON_COMMAND(ID_DATA_NPCEXEDATA, OnDataNpcexedata)
	ON_COMMAND(ID_DATA_REORDERMAPLIST, OnDataReordermaplist)
	ON_BN_CLICKED(BUTTON_TILESET_ADD, OnBnClickedTilesetAdd)
	ON_BN_CLICKED(BUTTON_AREA_ADD, OnBnClickedAreaAdd)
	ON_BN_CLICKED(BUTTON_AREA_COPY, OnBnClickedAreaCopy)
	ON_BN_CLICKED(BUTTON_AREA_EDIT, OnBnClickedAreaEdit)
	ON_BN_CLICKED(BUTTON_AREA_DELETE, OnBnClickedAreaDelete)
	ON_LBN_DBLCLK(LIST_AREAS, OnLbnDblclkAreas)
	ON_BN_CLICKED(BUTTON_MUSIC_ADD, OnBnClickedMusicAdd)
	ON_BN_CLICKED(BUTTON_MUSIC_DELETE, OnBnClickedMusicDelete)
	ON_BN_CLICKED(ID_MUSICLIST_DELETEMUSIC, OnBnClickedMusicDelete)
	ON_BN_CLICKED(BUTTON_MUSIC_PLAY, OnBnClickedMusicPlay)
	ON_BN_CLICKED(BUTTON_GAMEPLAY_EDIT, OnBnClickedGameplayEdit)
	ON_BN_CLICKED(BUTTON_MAP_IMPORT, OnBnClickedMapImport)
	ON_BN_CLICKED(BUTTON_MUSIC_UP, OnBnClickedMusicUp)
	ON_BN_CLICKED(BUTTON_MUSIC_DOWN, OnBnClickedMusicDown)
	ON_LBN_SELCHANGE(LIST_MUSIC, OnLbnSelchangeMusic)
END_MESSAGE_MAP()


void CCaveEditorDlg::OnOK()
{
}

void CCaveEditorDlg::OnCancel()
{
	if (close)
	{
		if (changes)
		{
			switch(AfxMessageBox("Do you want to save before you quit?", MB_YESNOCANCEL))
			{
			case IDYES:
				OnFileSave();
				break;
			case IDNO:
				break;
			case IDCANCEL:
				close = false;
				return;
				break;
			}
			ExitSoundSubsystem();
			CDialog::OnCancel();
			return;
		}
		else {
			ExitSoundSubsystem();
			CDialog::OnCancel();// don't allow [Escape]
		}
		close = false;
	}
}

void CCaveEditorDlg::OnClose()
{
	close = true;

	CDialog::OnClose();
}


//This handles Context menus available through right clicking
void CCaveEditorDlg::OnContextMenu(CWnd* cwnd, CPoint point)
{
	int i = -1;
	BOOL outside;
	RECT rect;

	//select context specific menu
	m_ListMaps.GetClientRect(&rect);
	m_ListMaps.ClientToScreen(&rect);
	if (PtInRect(&rect, point))
		i = 0;

	m_ListTilesets.GetClientRect(&rect);
	m_ListTilesets.ClientToScreen(&rect);
	if (PtInRect(&rect, point))
		i = 1;

	m_ListSpritesets.GetClientRect(&rect);
	m_ListSpritesets.ClientToScreen(&rect);
	if (PtInRect(&rect, point))
		i = 2;

	m_ListBackgrounds.GetClientRect(&rect);
	m_ListBackgrounds.ClientToScreen(&rect);
	if (PtInRect(&rect, point))
		i = 3;
	
	m_ListScripts.GetClientRect(&rect);
	m_ListScripts.ClientToScreen(&rect);
	if (PtInRect(&rect, point))
		i = 4;

	m_ListAreas.GetClientRect(&rect);
	m_ListAreas.ClientToScreen(&rect);
	if (PtInRect(&rect, point))
		i = 5;
	
	m_ListMusic.GetClientRect(&rect);
	m_ListMusic.ClientToScreen(&rect);
	if (PtInRect(&rect, point))
		i = 6;

	if (i == -1)//no context menu to display here
		return;

	switch (i)
	{
		case 0:
			m_ListMaps.ScreenToClient(&point);
			m_ListMaps.SetCurSel(m_ListMaps.ItemFromPoint(point, outside));
			m_ListMaps.ClientToScreen(&point);
			break;
		case 1:
			m_ListTilesets.ScreenToClient(&point);
			m_ListTilesets.SetCurSel(m_ListTilesets.ItemFromPoint(point, outside));
			m_ListTilesets.ClientToScreen(&point);
			break;
		case 2:
			m_ListSpritesets.ScreenToClient(&point);
			m_ListSpritesets.SetCurSel(m_ListSpritesets.ItemFromPoint(point, outside));
			m_ListSpritesets.ClientToScreen(&point);
			break;
		case 3:
			m_ListBackgrounds.ScreenToClient(&point);
			m_ListBackgrounds.SetCurSel(m_ListBackgrounds.ItemFromPoint(point, outside));
			m_ListBackgrounds.ClientToScreen(&point);
			break;
		case 4:
			m_ListScripts.ScreenToClient(&point);
			m_ListScripts.SetCurSel(m_ListScripts.ItemFromPoint(point, outside));
			m_ListScripts.ClientToScreen(&point);
			break;
		case 5:
			m_ListAreas.ScreenToClient(&point);
			m_ListAreas.SetCurSel(m_ListAreas.ItemFromPoint(point, outside));
			m_ListAreas.ClientToScreen(&point);
			break;
		case 6:
			m_ListMusic.ScreenToClient(&point);
			m_ListMusic.SetCurSel(m_ListMusic.ItemFromPoint(point, outside));
			m_ListMusic.ClientToScreen(&point);
			break;

	};

   CMenu* pPopup = conMenu.GetSubMenu(i);
   ASSERT(pPopup != NULL);
   pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x+1, point.y, this);
}

//#include "CMessageBoxPreview.h"

// CCaveEditorDlg message handlers
BOOL CCaveEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	mapTabsDialog.Create(DIALOG_MAP_TAB_MANAGER, NULL);
	mapEditDialog.Create(DIALOG_MAP_EDIT, NULL);
	scriptEditorDlg.Create(DIALOG_SCRIPT_EDITOR, NULL);
	
//	pbmEditorDlg.Create(DIALOG_PBM_EDITOR, this);

	VERIFY(conMenu.LoadMenu(MENU_LIST));//load context menu now

	//completments of http://www.codeguru.com/Cpp/W-D/dislog/tutorials/article.php/c4965
	//keyboard shortcuts!
	m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), m_lpszTemplateName);
	if (!m_hAccel)
		MessageBox("The hotkey table was not loaded");


	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, TRUE);		  // Set small icon

	// TODO: Add extra initialization here
	int i;
	char buffer[1024];
	char temp[64];

	GetModuleFileName(NULL, buffer, MAX_PATH);

	while (strchr(buffer, '\\'))
		*strchr(buffer, '\\') = '/';

	if (strrchr(buffer, '/'))
		*strrchr(buffer, '/') = 0;
	if (strrchr(buffer, '/'))
		*strrchr(buffer, '/') = 0;

	if (*CS_DEFAULT_MOD_PATH != 0)
		LoadMod(CS_DEFAULT_MOD_PATH);

	//static const char* SCRIPT = "<MSGLol!! This is a test.<NOD\nAnd this is, too.<NOD\nYESSS PLEASE\nYEEEES!!!!<NOD<FAC0032<CLRHi. It is I.<NOD\nIdiot.<NOD<END";
	//CMessageBoxPreview test;
	//test.m_ScriptPtr = SCRIPT;
	//test.DoModal();

	return TRUE;
}
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCaveEditorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		return;
	}

	CDialog::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCaveEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CCaveEditorDlg::LoadMod(const char* pModPath)
{
	ExitSoundSubsystem();
	StopOrganyaMusic();
	EndOrganya();

	// Calculate the path to the wave table
	if (!exe.Open(pModPath))
	{
		filePath = "";
		exe.Clear();
		return false;
	}

	if (*CS_DEFAULT_MOD_PATH == 0)
	{
		char pBuffer[1024];

		sprintf(pBuffer, "Do you want to set \"%s\" as the startup mod?", exe.mod.mModPath);
		int iResult = MessageBox(pBuffer, "Notice!", MB_YESNO);

		if (iResult == IDYES)
		{
			strcpy(CS_DEFAULT_MOD_PATH, exe.mod.mModPath);
			SaveConfig_CaveEditor();
		}
	}

	// Activate all working controls
	GetDlgItem(STATIC_MAPS)->EnableWindow(true);
	GetDlgItem(STATIC_TILESETS)->EnableWindow(true);
	GetDlgItem(STATIC_SPRITES)->EnableWindow(true);
	GetDlgItem(STATIC_BACKGROUNDS)->EnableWindow(true);
	GetDlgItem(STATIC_SCRIPTS)->EnableWindow(true);
	GetDlgItem(STATIC_AREAS)->EnableWindow(true);
	GetDlgItem(STATIC_MUSIC)->EnableWindow(true);

	GetDlgItem(BUTTON_SCRIPT_EDIT)->EnableWindow(true);
	GetDlgItem(BUTTON_NPCTABLE_EDIT)->EnableWindow(true);
	GetDlgItem(BUTTON_GAME_SETTINGS)->EnableWindow(true);

	GetDlgItem(LIST_MAPS)->EnableWindow(true);
	GetDlgItem(LIST_TILESETS)->EnableWindow(true);
	GetDlgItem(LIST_SCRIPTS)->EnableWindow(true);
	GetDlgItem(LIST_SPRITES)->EnableWindow(true);
	GetDlgItem(LIST_BACKGROUNDS)->EnableWindow(true);
	GetDlgItem(LIST_AREAS)->EnableWindow(true);
	GetDlgItem(LIST_MUSIC)->EnableWindow(true);

	GetDlgItem(BUTTON_MAP_ADD)->EnableWindow(true);
	GetDlgItem(BUTTON_MAP_EDIT)->EnableWindow(true);
	GetDlgItem(BUTTON_MAP_COPY)->EnableWindow(true);
	GetDlgItem(BUTTON_MAP_DELETE)->EnableWindow(true);
	
	GetDlgItem(BUTTON_AREA_ADD)->EnableWindow(true);
	GetDlgItem(BUTTON_AREA_EDIT)->EnableWindow(true);
	GetDlgItem(BUTTON_AREA_COPY)->EnableWindow(true);
	GetDlgItem(BUTTON_AREA_DELETE)->EnableWindow(true);

	GetDlgItem(BUTTON_TILESET_ADD)->EnableWindow(true);
	GetDlgItem(BUTTON_TILESET_EDIT)->EnableWindow(true);
	//	GetDlgItem(BUTTON_TILESET_COPY)->EnableWindow(true);
	GetDlgItem(BUTTON_TILESET_DELETE)->EnableWindow(true);

	GetDlgItem(BUTTON_SPRITES_ADD)->EnableWindow(true);
	GetDlgItem(BUTTON_SPRITES_EDIT)->EnableWindow(true);
	//	GetDlgItem(BUTTON_SPRITES_COPY)->EnableWindow(true);
	GetDlgItem(BUTTON_SPRITES_DELETE)->EnableWindow(true);

	GetDlgItem(BUTTON_BACKGROUND_ADD)->EnableWindow(true);
	GetDlgItem(BUTTON_BACKGROUND_EDIT)->EnableWindow(true);
	//	GetDlgItem(BUTTON_BACKGROUND_COPY)->EnableWindow(true);
	GetDlgItem(BUTTON_BACKGROUND_DELETE)->EnableWindow(true);

	GetDlgItem(BUTTON_MUSIC_ADD)->EnableWindow(true);
	GetDlgItem(BUTTON_MUSIC_PLAY)->EnableWindow(true);
	GetDlgItem(BUTTON_MUSIC_EDIT)->EnableWindow(true);
	GetDlgItem(BUTTON_MUSIC_DELETE)->EnableWindow(true);

	GetDlgItem(BUTTON_GAMEPLAY_EDIT)->EnableWindow(true);

	//adjust menu items availability (allow save, disable load)
	GetMenu()->EnableMenuItem(ID_FILE_SAVE, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_FILE_TEST, MF_BYCOMMAND | MF_GRAYED);
	//GetMenu()->EnableMenuItem(ID_FILE_LOAD, MF_BYCOMMAND | MF_GRAYED);

	GetMenu()->EnableMenuItem(ID_DATA_BULLETDATA, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_DATA_NPCEXEDATA, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_DATA_REORDERMAPLIST, MF_BYCOMMAND | MF_ENABLED);

	//adjust context menu item availability
	conMenu.EnableMenuItem(ID_MAPLIST_EDITMAP, MF_BYCOMMAND | MF_ENABLED);
	conMenu.EnableMenuItem(ID_MAPLIST_EDITMAPPROPERTIES, MF_BYCOMMAND | MF_ENABLED);
	conMenu.EnableMenuItem(ID_MAPLIST_EDITMAPSCRIPT, MF_BYCOMMAND | MF_ENABLED);
	conMenu.EnableMenuItem(ID_MAPLIST_COPYMAP, MF_BYCOMMAND | MF_ENABLED);
	conMenu.EnableMenuItem(ID_MAPLIST_ADDMAP, MF_BYCOMMAND | MF_ENABLED);
	conMenu.EnableMenuItem(ID_MAPLIST_DELETEMAP, MF_BYCOMMAND | MF_ENABLED);

	conMenu.EnableMenuItem(ID_TILESETLIST_EDITTILESET, MF_BYCOMMAND | MF_ENABLED);
	//	conMenu.EnableMenuItem(ID_TILESETLIST_COPYTILESET, MF_BYCOMMAND | MF_ENABLED);
	//	conMenu.EnableMenuItem(ID_TILESETLIST_DELETETILESET, MF_BYCOMMAND | MF_ENABLED);

	conMenu.EnableMenuItem(ID_SPRITESETLIST_EDITSPRITESET, MF_BYCOMMAND | MF_ENABLED);
	//	conMenu.EnableMenuItem(ID_SPRITESETLIST_COPYSPRITESET, MF_BYCOMMAND | MF_ENABLED);
	//	conMenu.EnableMenuItem(ID_SPRITESETLIST_DELETESPRITESET, MF_BYCOMMAND | MF_ENABLED);

	conMenu.EnableMenuItem(ID_BACKGROUNDLIST_EDITBACKGROUND, MF_BYCOMMAND | MF_ENABLED);
	//	conMenu.EnableMenuItem(ID_BACKGROUNDLIST_COPYBACKGROUND, MF_BYCOMMAND | MF_ENABLED);
	//	conMenu.EnableMenuItem(ID_BACKGROUNDLIST_DELETEBACKGROUND, MF_BYCOMMAND | MF_ENABLED);

	conMenu.EnableMenuItem(ID_SCRIPTLIST_EDITSCRIPT, MF_BYCOMMAND | MF_ENABLED);

	char buffer[1024];
	char temp[1024];
	int i;

	//*
		//Load list of non map related scripts
	sprintf(buffer, "%s/", exe.mod.mModPath);//which path to .\data\ 
	::SetCurrentDirectory(buffer);
	while (m_ListScripts.GetCount()) m_ListScripts.DeleteString(0);
	m_ListScripts.Dir(0x0010, "*.tsc");//get list of tsc script files
	i = m_ListScripts.GetCount();
	for (; i > 0; i--)
	{
		m_ListScripts.GetText(0, temp);
		temp[strlen(temp) - 4] = '\0';
		strcpy(buffer, &(temp[0]));
		m_ListScripts.DeleteString(0);
		m_ListScripts.AddString(buffer);
	};
	
	::SetCurrentDirectory(filePath);

	reloadMapList();
	if (NPCtable::us)
		NPCtable::us->reload_current_mod_npc_table();

	return true;
}

void CCaveEditorDlg::OnFileNew()
{
	int i;
	char szFilters[] = "Cave Story Multiplayer Executable (*.exe)|*.exe||";
	char buffer[1024];
	char temp[64];

	// Create an Open dialog
	CFileDialog loadFileDialog(TRUE, "*.exe", "Doukutsu.exe", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	MessageBox("Please locate and select the Cavestory Multiplayer executable (Doukutsu.exe).", "Help - CaveEditor 2.0", MB_OK);

	if (loadFileDialog.DoModal() != IDOK)
		return;

	filePath = loadFileDialog.GetPathName();
	EXEname  = loadFileDialog.GetFileName();
	i        = filePath.ReverseFind('\\');
	filePath.Delete(i + 1, 256);

	// Ask what name the mod should have
	CCreateModDlg cdlg;

	if (cdlg.DoModal() != IDOK)
		return;

	if (filePath[filePath.GetLength() - 1] == '/' || filePath[filePath.GetLength() - 1] == '\\')
		sprintf(buffer, "%s%s", filePath.GetBuffer(), cdlg.ModDataPath);
	else
		sprintf(buffer, "%s/%s", filePath.GetBuffer(), cdlg.ModDataPath);

	if (exe.mod.CreateMod(buffer, false, cdlg.ModTitle, cdlg.ModAuthor, PXMOD_VERSION_STRUCT(1, 0, 0, 0)) && exe.mod.Save())
		LoadMod(buffer);

	if (NPCtable::us)
		NPCtable::us->reload_current_mod_npc_table();
}

void CCaveEditorDlg::OnFileSave()
{
	char pMessage[256];

	// Save everything else
	if (exe.Save(false))
		strcpy(pMessage, "Successfully wrote to 'dbg-mod.pxmod'!");
	else
		strcpy(pMessage, "Could not write to 'dbg-mod.pxmod'.");

	if (CS_DEFAULT_COMPILE_ON_SAVE)
	{
		if (exe.Save(true))
			strcat(pMessage, "\nSuccessfully wrote to 'mod.pxmod'!");
		else
			strcat(pMessage, "\nCould not write to 'mod.pxmod'.");
	}

	MessageBox(pMessage, "Save - CaveEditor 2.0", MB_OK);

	changes = false;
}

void CCaveEditorDlg::OnFileCompileRelease()
{
	if (exe.Save(true))
		MessageBox("Compiled successfully!", "Success - CaveEditor 2.0", MB_OK);
}

void CCaveEditorDlg::OnFilePackMod()
{
	ModPackerDlg MPD;
	MPD.DoModal();
}

void CCaveEditorDlg::OnFileTest()
{
	OnFileSave();
	::SetCurrentDirectory(filePath);
	::spawnl(_P_NOWAIT, EXEname, EXEname);
//	::_execl(buffer, buffer);
}

void CCaveEditorDlg::OnFileSaveMapList()
{
	exe.Save();

	if (CS_DEFAULT_COMPILE_ON_SAVE)
		exe.Save(true);
}

void CCaveEditorDlg::OnFileLoad()
{
	int i;
	char szFilters[] = "Pixel Mod File (*.pxmod)|*.pxmod||";
	char buffer[1024];
	char temp[64];

	// Create an Open dialog
	CFileDialog loadFileDialog(TRUE, "*.pxmod", "mod.pxmod", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	if (loadFileDialog.DoModal() != IDOK)
		return;

	filePath = loadFileDialog.GetPathName();
	EXEname = loadFileDialog.GetFileName();
	i = filePath.ReverseFind('\\');
	filePath.Delete(i+1, 256);

	LoadMod(filePath.GetBuffer());
}


void CCaveEditorDlg::reloadMapList()
{
	char buffer[256];
	int i;
	int sel;
	int top;

	int sel_tileset = m_ListTilesets.GetCurSel();
	int sel_backgro = m_ListBackgrounds.GetCurSel();
	int sel_sprites = m_ListSpritesets.GetCurSel();
	int sel_areas = m_ListAreas.GetCurSel();
	int sel_music = m_ListMusic.GetCurSel();

	sel = m_ListMaps.GetCurSel();
	top = m_ListMaps.GetTopIndex();
	i = m_ListMaps.GetCount();
	for (; i > 0; i--)//clear list
		m_ListMaps.DeleteString(0);

	while (m_ListTilesets.GetCount()) m_ListTilesets.DeleteString(0);
	while (m_ListSpritesets.GetCount()) m_ListSpritesets.DeleteString(0);
	while (m_ListBackgrounds.GetCount()) m_ListBackgrounds.DeleteString(0);
	while (m_ListAreas.GetCount()) m_ListAreas.DeleteString(0);
	while (m_ListMusic.GetCount()) m_ListMusic.DeleteString(0);

	for (i = 0; i < exe.mod.mStages.count; i++)//load map list
	{
		if (strcmp(exe.mod.mStages.list[i].file, "\\empty") == 0)
			sprintf(buffer, "[%.3i] ···No Map···", i);
		else
			sprintf(buffer, "[%.3i] %s (%s)", i, exe.mod.mStages.list[i].name, exe.mod.mStages.list[i].file);

		m_ListMaps.AddString(buffer);
	}

	for (i = 0; i < exe.mod.mTilesets.count; i++)
		m_ListTilesets.AddString(exe.mod.mTilesets.list[i].name);

	for (i = 0; i < exe.mod.mSpritesheets.count; i++)
		m_ListSpritesets.AddString(exe.mod.mSpritesheets.list[i].name);

	for (i = 0; i < exe.mod.mBackgrounds.count; i++)
		m_ListBackgrounds.AddString(exe.mod.mBackgrounds.list[i].name);

	for (i = 0; i < exe.mod.mAreas.count; i++)
	{
		// Build the string
		sprintf(buffer, "[%.3i] %s", i, exe.mod.mAreas.list[i].name);

		// Add it to the list
		m_ListAreas.AddString(buffer);
	}

	for (i = 0; i < exe.mod.mMusic.count; i++)
	{
		sprintf(buffer, "[%.3i] %s", i, exe.mod.mMusic.list[i].name);
		m_ListMusic.AddString(buffer);
	}

	if (sel < exe.mod.mStages.count)
		m_ListMaps.SetCurSel(sel);

	if (sel_tileset < m_ListTilesets.GetCount())
		m_ListTilesets.SetCurSel(sel_tileset);
	
	if (sel_backgro < m_ListBackgrounds.GetCount())
		m_ListBackgrounds.SetCurSel(sel_backgro);
	
	if (sel_sprites < m_ListSpritesets.GetCount())
		m_ListSpritesets.SetCurSel(sel_sprites);
	
	if (sel_areas < m_ListAreas.GetCount())
		m_ListAreas.SetCurSel(sel_areas);
	
	if (sel_music < m_ListMusic.GetCount())
		m_ListMusic.SetCurSel(sel_music);

	m_ListMaps.SetTopIndex(top);
}

void CCaveEditorDlg::OnOptions()
{
	OptionsDlg o;
	o.DoModal();
}

void CCaveEditorDlg::OnDataCompileMaps()
{
	exe.CompileMaps();
}

void CCaveEditorDlg::OnHelpContents()
{
	// TODO: Add your command handler code here
}

void CCaveEditorDlg::OnHelpAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CCaveEditorDlg::OnBnClickedMapDelete()
{
	if (m_ListMaps.GetCurSel() == LB_ERR)
		return;


	if (exe.DeleteMap(m_ListMaps.GetCurSel()))
	{
		changes = true;
		reloadMapList();
		m_ListMaps.GetFocus();
	}
}

void CCaveEditorDlg::OnBnClickedMapAdd()
{
	CaveMapINFO info;

	info.fileName[0] = '\0';
	info.caption[0] = '\0';
	info.bossNum = 0;
	info.backType = 0;
	info.area = 0;
	info.focus_center_x = false;
	info.focus_center_y = false;
	strcpy(info.NPCset1, "0");
	strcpy(info.NPCset2, "0");
	strcpy(info.tileset, "0");
	strcpy(info.background, "bk0");

	CaveMapPropertiesDlg mapProp;
	mapProp.width = 21;
	mapProp.height = 16;
	mapProp.caveInfo = &info;
	if (mapProp.DoModal() != IDOK)
		return;

	if (exe.AddMap(&info, mapProp.width, mapProp.height))
	{
		exe.mod.mStages.list[exe.mod.mStages.count - 1].area_index     = info.area;
		exe.mod.mStages.list[exe.mod.mStages.count - 1].focus_center_x = info.focus_center_x;
		exe.mod.mStages.list[exe.mod.mStages.count - 1].focus_center_y = info.focus_center_y;
		changes = true;
		reloadMapList();
	}
}

void CCaveEditorDlg::OnBnClickedMapCopy()
{
	if (m_ListMaps.GetCurSel() == LB_ERR)
		return;

	TextEntryDlg txt;
	txt.text = exe.mod.mStages.list[m_ListMaps.GetCurSel()].file;
	txt.text += "_";

	if (txt.DoModal() != IDOK)
		return;
	if (txt.text.IsEmpty())
		return;

	if (exe.CopyMap(m_ListMaps.GetCurSel(), txt.text))
	{
		changes = true;
		reloadMapList();
	}
}

void CCaveEditorDlg::OnBnClickedMapEdit()
{
	CaveMapINFO info;

	int i = m_ListMaps.GetCurSel();
	if (m_ListMaps.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a map first.", "Error");
		return;
	}
	if (strcmp(exe.mod.mStages.list[m_ListMaps.GetCurSel()].file, "\\empty") == 0)
	{
		if (AfxMessageBox("Map previously deleted.  Create a new map?", MB_YESNO) == IDNO)
			return;

		//User wanted to create a map
		info.fileName[0] = '\0';
		info.caption[0] = '\0';
		info.bossNum = 0;
		info.backType = 0;
		strcpy(info.NPCset1, "0");
		strcpy(info.NPCset2, "0");
		strcpy(info.tileset, "0");
		strcpy(info.background, "bk0");

		CaveMapPropertiesDlg mapProp;
		mapProp.width = 2;
		mapProp.height = 2;
		mapProp.caveInfo = &info;
		if (mapProp.DoModal() != IDOK)//Get map properties for new map
			return;

		//Adding map
		if (exe.AddMap(&info, mapProp.width, mapProp. height,m_ListMaps.GetCurSel()))
		{
			changes = true;
			reloadMapList();
		}
		else
			return;//don't try to edit the map on failure

	}

	::SetCurrentDirectory(filePath);
	mapTabsDialog.LoadMap(m_ListMaps.GetCurSel());
	if (!mapTabsDialog.IsWindowVisible())
		mapTabsDialog.ShowWindow(!CS_DEFAULT_ME_MAXIMIZE_ON_INIT ? SW_SHOW : SW_SHOWMAXIMIZED);
	if (!CS_DEFAULT_ME_FOCUS_ON_MAP_LOAD)
		BringWindowToTop();

	return;
	if (mapEditDialog.loadMap(&CaveMapINFO(exe.mod.mStages.list[m_ListMaps.GetCurSel()], &exe.mod), m_ListMaps.GetCurSel()))
	{
		bool bWasVisible = mapEditDialog.IsWindowVisible() == TRUE;

		mapEditDialog.ShowWindow(SW_SHOW);
		mapEditDialog.BringWindowToTop();

		if (!bWasVisible)
			mapEditDialog.ShowWindow(SW_MAXIMIZE);
	}
}

void CCaveEditorDlg::OnBnClickedTilesetEdit()
{
	if (m_ListTilesets.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a tileset first.", "Error");
		return;
	}
	TilesetEditorDlg tilesetEditorDlg;

	char buffer[256];

	m_ListTilesets.GetText(m_ListTilesets.GetCurSel(), buffer);
	tilesetEditorDlg.tileset.load(buffer);

	if (tilesetEditorDlg.tileset.tileName[0] == '\0')
		return;//return if no file selected

	tilesetEditorDlg.DoModal();

	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedTilesetCopy()
{
	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedScriptEdit()
{
	if (m_ListScripts.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a script first.", "Error");
		return;
	}
	char temp[256];
	temp[0] = '\0';
	char buffer[1024];
	m_ListScripts.GetText(m_ListScripts.GetCurSel(), temp);
	if (temp[0] == '\0')//no script selected
		return;

	sprintf(buffer, "%s/%s.tsc", exe.mod.mModPath, temp);
	scriptEditorDlg.ShowWindow(SW_SHOW);
	scriptEditorDlg.Open(buffer, m_ListScripts.GetCurSel());
}

void CCaveEditorDlg::OnBnClickedNpctableEdit()
{
	NPCTableEditorDlg npc;
	npc.DoModal();
}

void CCaveEditorDlg::OnBnClickedGameSettings()
{
	GameSettingsDlg settings;

	settings.collectables.enabled            = exe.mod.mHeader.collectables.enabled;
	settings.collectables.sprite_rect.left   = exe.mod.mHeader.collectables.sprite_rect.left;
	settings.collectables.sprite_rect.top    = exe.mod.mHeader.collectables.sprite_rect.top;
	settings.collectables.sprite_rect.right  = exe.mod.mHeader.collectables.sprite_rect.right;
	settings.collectables.sprite_rect.bottom = exe.mod.mHeader.collectables.sprite_rect.bottom;
	settings.collectables.surface_id         = exe.mod.mHeader.collectables.surface_id;
	settings.hud.draw_hp_bar                 = exe.mod.mHeader.hud.draw_hp_bar;
	settings.hud.draw_hp                     = exe.mod.mHeader.hud.draw_hp;
	settings.hud.draw_weapons                = exe.mod.mHeader.hud.draw_weapons;
	settings.hud.draw_ammo                   = exe.mod.mHeader.hud.draw_ammo;
	settings.hud.draw_exp_bar                = exe.mod.mHeader.hud.draw_exp_bar;
	settings.hud.draw_level                  = exe.mod.mHeader.hud.draw_level;
	settings.gameplay.allow_carrying_players = exe.mod.mHeader.gameplay.allow_carrying_players;
	settings.gameplay.allow_agility          = exe.mod.mHeader.gameplay.allow_agility;
	settings.gameplay.allow_fishing          = exe.mod.mHeader.gameplay.allow_fishing;

	settings.title  = exe.mod.mHeader.title;
	settings.author = exe.mod.mHeader.author;
	settings.version_0 = exe.mod.GetVersion().data[0];
	settings.version_1 = exe.mod.GetVersion().data[1];
	settings.version_2 = exe.mod.GetVersion().data[2];
	settings.version_3 = exe.mod.GetVersion().data[3];

	settings.titleX = exe.mod.mTitle.x;
	settings.titleY = exe.mod.mTitle.y;
	settings.titleMapEvent = exe.mod.mTitle.event;
	settings.titleMap = exe.mod.mTitle.map;

	settings.startX = exe.mod.mStart.x;
	settings.startY = exe.mod.mStart.y;
	settings.startMapEvent = exe.mod.mStart.event;
	settings.startMap = exe.mod.mStart.map;

	settings.startCurHP = exe.mod.mStart.hp;
	settings.startMaxHP = exe.mod.mStart.max_hp;

	settings.startFlags = exe.mod.mStart.flags;
	settings.startFacing = exe.mod.mStart.direct;

	if (settings.DoModal() == IDOK)
	{
		exe.mod.SetTitle(settings.title);
		exe.mod.SetAuthor(settings.author);
		exe.mod.SetVersion(PXMOD_VERSION_STRUCT(settings.version_0, settings.version_1, settings.version_2, settings.version_3));

		exe.mod.mHeader.collectables.enabled            = settings.collectables.enabled;
		exe.mod.mHeader.collectables.sprite_rect.left   = settings.collectables.sprite_rect.left;
		exe.mod.mHeader.collectables.sprite_rect.top    = settings.collectables.sprite_rect.top;
		exe.mod.mHeader.collectables.sprite_rect.right  = settings.collectables.sprite_rect.right;
		exe.mod.mHeader.collectables.sprite_rect.bottom = settings.collectables.sprite_rect.bottom;
		exe.mod.mHeader.collectables.surface_id         = settings.collectables.surface_id;
		exe.mod.mHeader.hud.draw_hp_bar                 = settings.hud.draw_hp_bar;
		exe.mod.mHeader.hud.draw_hp                     = settings.hud.draw_hp;
		exe.mod.mHeader.hud.draw_weapons                = settings.hud.draw_weapons;
		exe.mod.mHeader.hud.draw_ammo                   = settings.hud.draw_ammo;
		exe.mod.mHeader.hud.draw_exp_bar                = settings.hud.draw_exp_bar;
		exe.mod.mHeader.hud.draw_level                  = settings.hud.draw_level;
		exe.mod.mHeader.gameplay.allow_carrying_players = settings.gameplay.allow_carrying_players;
		exe.mod.mHeader.gameplay.allow_agility          = settings.gameplay.allow_agility;
		exe.mod.mHeader.gameplay.allow_fishing          = settings.gameplay.allow_fishing;

		exe.mod.mTitle.x     = settings.titleX;
		exe.mod.mTitle.y     = settings.titleY;
		exe.mod.mTitle.event = settings.titleMapEvent;
		exe.mod.mTitle.map   = settings.titleMap;

		exe.mod.mStart.x     = settings.startX;
		exe.mod.mStart.y     = settings.startY;
		exe.mod.mStart.event = settings.startMapEvent;
		exe.mod.mStart.map   = settings.startMap;

		exe.mod.mStart.hp     = settings.startCurHP;
		exe.mod.mStart.max_hp = settings.startMaxHP;
		
		exe.mod.mStart.flags  = settings.startFlags;
		exe.mod.mStart.direct = (PXMOD_DIRECTIONS)settings.startFacing;

		changes = true;
	}
}

void CCaveEditorDlg::OnBnClickedSpritesCopy()
{
	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedSpritesEdit()
{
	if (m_ListSpritesets.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a sprite set first.", "Error");
		return;
	}

	PBMEditorDlg pbmEditorDlg(DIALOG_TILESET_EDITOR);
	char buffer[256];
	char temp[256];

	m_ListSpritesets.GetText(m_ListSpritesets.GetCurSel(), buffer);

	sprintf(temp, "%s/Npc/Npc%s.png", exe.mod.mModPath, buffer);
	if (!pbmEditorDlg.pbm.Load(temp))
	{
		MessageBox(temp, "Error loading file");
		return;//return with useless error message if error loading pbm
	};
	pbmEditorDlg.title = "Sprite Editor";
	pbmEditorDlg.DoModal();
}

void CCaveEditorDlg::OnBnClickedBackgroundCopy()
{
	// TODO: Add your control notification handler code here
}

void CCaveEditorDlg::OnBnClickedBackgroundEdit()
{
	if (m_ListBackgrounds.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a background first.", "Error");
		return;
	}
	PBMEditorDlg pbmEditorDlg(DIALOG_TILESET_EDITOR);
	char buffer[256];
	char temp[256];

	m_ListBackgrounds.GetText(m_ListBackgrounds.GetCurSel(), buffer);

	sprintf(temp, "%s/bk%s.png", exe.mod.mModPath, buffer);
	if (!pbmEditorDlg.pbm.Load(temp))
	{
		MessageBox(temp, "Error loading file");
		return;//return with useless error message if error loading pbm
	};
	pbmEditorDlg.title = "Background Editor";
	pbmEditorDlg.DoModal();
}

void CCaveEditorDlg::OnLbnDblclkMaps()
{
	OnBnClickedMapEdit();
}

void CCaveEditorDlg::OnLbnDblclkTilesets()
{
	OnBnClickedTilesetEdit();
}

void CCaveEditorDlg::OnLbnDblclkSprites()
{
	OnBnClickedSpritesEdit();
}

void CCaveEditorDlg::OnLbnDblclkBackgrounds()
{
	OnBnClickedBackgroundEdit();
}

void CCaveEditorDlg::OnLbnDblclkScripts()
{
	OnBnClickedScriptEdit();
}

void CCaveEditorDlg::OnMaplistEditmap()
{
	OnBnClickedMapEdit();
}

void CCaveEditorDlg::OnTilesetlistEdittileset()
{
	OnBnClickedTilesetEdit();
}

void CCaveEditorDlg::OnSpritesetlistEditspriteset()
{
	OnBnClickedSpritesEdit();
}

void CCaveEditorDlg::OnBackgroundlistEditbackground()
{
	OnBnClickedBackgroundEdit();
}

void CCaveEditorDlg::OnScriptlistEditscript()
{
	OnBnClickedScriptEdit();
}

void CCaveEditorDlg::OnAreaListEdit()
{
	OnBnClickedAreaEdit();
}

void CCaveEditorDlg::OnImportMaps()
{
	OnBnClickedMapImport();
}

void CCaveEditorDlg::OnAreaListCopy()
{
	OnBnClickedAreaCopy();
}

void CCaveEditorDlg::OnAreaListDelete()
{
	OnBnClickedAreaDelete();
}

void CCaveEditorDlg::OnMaplistEditmapproperties()
{
	CaveMapPropertiesDlg temp;

	int Sel = m_ListMaps.GetCurSel();

	CaveMapINFO mapinfo(exe.mod.mStages.list[Sel], &exe.mod);

	temp.width       = -1;
	temp.height      = -1;
	temp.caveInfo    = &mapinfo;
	temp.oldFileName = exe.mod.mStages.list[Sel].file;
	
	if (temp.DoModal() == IDOK)
	{
		int iCache;

		mapinfo.SetModStage(&exe.mod.mStages.list[Sel], &exe.mod);

		reloadMapList();
		changes = true;
	}

}

void CCaveEditorDlg::OnMaplistEditmapscript()
{
	char temp[_MAX_PATH];
	sprintf(temp, "%s/Stage/%s.tsc", exe.mod.mModPath, exe.mod.mStages.list[m_ListMaps.GetCurSel()].file);

	scriptEditorDlg.Open(temp, m_ListMaps.GetCurSel());
	scriptEditorDlg.ShowWindow(SW_SHOW);
}

void CCaveEditorDlg::OnFileExit()
{
	OnClose();//just like how pressing the [x] works
	OnCancel();
}

void CCaveEditorDlg::OnDestroy()
{
	scriptEditorDlg.DestroyWindow();

	CDialog::OnDestroy();
}

void CCaveEditorDlg::OnEditBullets()
{
	DlgBulletEditor DBE;

	int result = DBE.DoModal();
	if (result == IDOK)
		changes = true;
}

void CCaveEditorDlg::OnEditWeapons()
{
	DlgWeaponEditor DWE;

	int result = DWE.DoModal();
	if (result == IDOK)
		changes = true;
}

void CCaveEditorDlg::OnEditHUD()
{

}

void CCaveEditorDlg::OnMaplistCopymap()
{
	OnBnClickedMapCopy();
}

void CCaveEditorDlg::OnMaplistAddmap()
{
	OnBnClickedMapAdd();
}

void CCaveEditorDlg::OnMaplistDeletemap()
{
	OnBnClickedMapDelete();
}

void CCaveEditorDlg::OnDataBulletdata()
{
	WeaponEditorDlg dlg;
	dlg.DoModal();
}

void CCaveEditorDlg::OnDataNpcexedata()
{
	NpcExeEditorDlg dlg;
	dlg.DoModal();
}

void CCaveEditorDlg::OnDataReordermaplist()
{
	ReorderMapsDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		changes = true;
		reloadMapList();
	}

}

BOOL CCaveEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST && m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CCaveEditorDlg::OnBnClickedSpritesAdd()
{
	int i;
	char szFilters[] = "Spritesheet File (Npc*.png)|Npc*.png||";
	char buffer[1024];
	char temp[64];

	// Create an Open dialog
	CFileDialog loadFileDialog(TRUE, "Npc*.png", NULL, OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	if (loadFileDialog.DoModal() != IDOK)
		return;

	char RequiredFilePath[1024];

	sprintf_s(RequiredFilePath, sizeof(RequiredFilePath), "%s/Npc", exe.mod.mModPath);

	POSITION pos(loadFileDialog.GetStartPosition());
	int iModPathLen = strlen(RequiredFilePath);
	while (pos)
	{
		CString filename = loadFileDialog.GetNextPathName(pos);

		while (filename.Find('\\', 0) != -1)
			filename.SetAt(filename.Find('\\', 0), '/');

		if (strstr(filename.GetBuffer(), RequiredFilePath) != filename.GetBuffer())
			continue;

		const char* pFileName = filename.GetBuffer() + iModPathLen + 1;
		if (
			strlen(pFileName) >= 8 &&
			pFileName[0] == 'N' &&
			pFileName[1] == 'p' &&
			pFileName[2] == 'c' &&
			pFileName[strlen(pFileName) - 4] == '.' &&
			pFileName[strlen(pFileName) - 3] == 'p' &&
			pFileName[strlen(pFileName) - 2] == 'n' &&
			pFileName[strlen(pFileName) - 1] == 'g'
			)
		{
			filename.SetAt(filename.ReverseFind('.'), 0);
			exe.mod.AddSpritesheet(pFileName + 3);
		}
	}

	reloadMapList();
}

void CCaveEditorDlg::OnBnClickedTilesetAdd()
{
	int i;
	char szFilters[] = "Tileset File (Prt*.png)|Prt*.png||";
	char buffer[1024];
	char temp[64];

	// Create an Open dialog
	CFileDialog loadFileDialog(TRUE, "Prt*.png", NULL, OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	if (loadFileDialog.DoModal() != IDOK)
		return;

	char RequiredFilePath[1024];

	sprintf_s(RequiredFilePath, sizeof(RequiredFilePath), "%s/Stage", exe.mod.mModPath);

	POSITION pos(loadFileDialog.GetStartPosition());
	int iModPathLen = strlen(RequiredFilePath);
	while (pos)
	{
		CString filename = loadFileDialog.GetNextPathName(pos);

		while (filename.Find('\\', 0) != -1)
			filename.SetAt(filename.Find('\\', 0), '/');

		if (strstr(filename.GetBuffer(), RequiredFilePath) != filename.GetBuffer())
			continue;

		const char* pFileName = filename.GetBuffer() + iModPathLen + 1;
		if (
			strlen(pFileName) >= 8 &&
			pFileName[0] == 'P' &&
			pFileName[1] == 'r' &&
			pFileName[2] == 't' &&
			pFileName[strlen(pFileName) - 4] == '.' &&
			pFileName[strlen(pFileName) - 3] == 'p' &&
			pFileName[strlen(pFileName) - 2] == 'n' &&
			pFileName[strlen(pFileName) - 1] == 'g'
			)
		{
			filename.SetAt(filename.ReverseFind('.'), 0);
			exe.mod.AddTileset(pFileName + 3);
		}
	}

	reloadMapList();
}

void CCaveEditorDlg::OnBnClickedBackgroundAdd()
{
	int i;
	char szFilters[] = "Background File (bk*.png)|bk*.png||";
	char buffer[1024];
	char temp[64];

	// Create an Open dialog
	CFileDialog loadFileDialog(TRUE, "bk*.png", NULL, OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	if (loadFileDialog.DoModal() != IDOK)
		return;

	POSITION pos(loadFileDialog.GetStartPosition());
	int iModPathLen = strlen(exe.mod.mModPath);
	while (pos)
	{
		CString filename = loadFileDialog.GetNextPathName(pos);

		while (filename.Find('\\', 0) != -1)
			filename.SetAt(filename.Find('\\', 0), '/');

		if (strstr(filename.GetBuffer(), exe.mod.mModPath) != filename.GetBuffer())
			continue;

		const char* pFileName = filename.GetBuffer() + iModPathLen + 1;
		if (
			strlen(pFileName) >= 7 &&
			pFileName[0] == 'b' &&
			pFileName[1] == 'k' &&
			pFileName[strlen(pFileName) - 4] == '.' &&
			pFileName[strlen(pFileName) - 3] == 'p' &&
			pFileName[strlen(pFileName) - 2] == 'n' &&
			pFileName[strlen(pFileName) - 1] == 'g'
			)
		{
			filename.SetAt(filename.ReverseFind('.'), 0);
			exe.mod.AddBackground(pFileName + 2);
		}
	}

	reloadMapList();
}

void CCaveEditorDlg::OnBnClickedSpritesDelete()
{
	if (m_ListSpritesets.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a spritesheet first.", "Error");
		return;
	}

	int Sel = m_ListSpritesets.GetCurSel();

	if (MessageBox("Are you sure you want to remove this spritesheet?", "CaveStory Multiplayer Mod Editor", MB_YESNO) != IDYES)
		return;

	if (exe.mod.RemoveSpritesheet(exe.mod.mSpritesheets.list[m_ListSpritesets.GetCurSel()].name))
	{
		reloadMapList();

		if (Sel >= m_ListSpritesets.GetCount())
			Sel = m_ListSpritesets.GetCount() - 1;

		m_ListSpritesets.SetCurSel(Sel);
	}
}

void CCaveEditorDlg::OnBnClickedTilesetDelete()
{
	if (m_ListTilesets.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a tileset first.", "Error");
		return;
	}

	int Sel = m_ListTilesets.GetCurSel();

	if (MessageBox("Are you sure you want to remove this tileset?", "CaveStory Multiplayer Mod Editor", MB_YESNO) != IDYES)
		return;

	if (exe.mod.RemoveTileset(exe.mod.mTilesets.list[m_ListTilesets.GetCurSel()].name))
	{
		reloadMapList();

		if (Sel >= m_ListTilesets.GetCount())
			Sel = m_ListTilesets.GetCount() - 1;

		m_ListTilesets.SetCurSel(Sel);
	}
}

void CCaveEditorDlg::OnBnClickedBackgroundDelete()
{
	if (m_ListBackgrounds.GetCurSel() == LB_ERR)
	{
		MessageBox("Select a background first.", "Error");
		return;
	}

	int Sel = m_ListBackgrounds.GetCurSel();

	if (MessageBox("Are you sure you want to remove this background?", "CaveStory Multiplayer Mod Editor", MB_YESNO) != IDYES)
		return;

	if (exe.mod.RemoveBackground(exe.mod.mBackgrounds.list[m_ListBackgrounds.GetCurSel()].name))
	{
		reloadMapList();

		if (Sel >= m_ListBackgrounds.GetCount())
			Sel = m_ListBackgrounds.GetCount() - 1;

		m_ListBackgrounds.SetCurSel(Sel);
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void CCaveEditorDlg::OnBnClickedAreaAdd()
{
	CEditAreaDlg dlg;

	strcpy_s(dlg.m_AreaInfo.name, sizeof(dlg.m_AreaInfo.name), "Default Area");
	dlg.m_AreaInfo.stage_idx         = 0;
	dlg.m_AreaInfo.keep_on_release   = true;
	dlg.m_AreaInfo.singleplayer_only = false;

	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	int iIndex = exe.mod.AddArea(dlg.m_AreaInfo.name);

	if (iIndex == -1)
		return;

	exe.mod.mAreas.list[iIndex].keep_on_release   = dlg.m_AreaInfo.keep_on_release;
	exe.mod.mAreas.list[iIndex].singleplayer_only = dlg.m_AreaInfo.singleplayer_only;
	exe.mod.mAreas.list[iIndex].stage_idx         = dlg.m_AreaInfo.stage_idx;

	reloadMapList();
}

void CCaveEditorDlg::OnBnClickedAreaCopy()
{
	int iIndex = m_ListAreas.GetCurSel();
	if (iIndex == LB_ERR)
	{
		MessageBox("Select an area first.", "Error");
		return;
	}

	CEditAreaDlg dlg;

	strcpy_s(dlg.m_AreaInfo.name, sizeof(dlg.m_AreaInfo.name), exe.mod.mAreas.list[iIndex].name);
	dlg.m_AreaInfo.stage_idx         = exe.mod.mAreas.list[iIndex].stage_idx;
	dlg.m_AreaInfo.keep_on_release   = exe.mod.mAreas.list[iIndex].keep_on_release;
	dlg.m_AreaInfo.singleplayer_only = exe.mod.mAreas.list[iIndex].singleplayer_only;

	if (dlg.DoModal() != IDOK)
		return;

	iIndex = exe.mod.AddArea(dlg.m_AreaInfo.name);

	if (iIndex == -1)
		return;

	exe.mod.mAreas.list[iIndex].keep_on_release = dlg.m_AreaInfo.keep_on_release;
	exe.mod.mAreas.list[iIndex].singleplayer_only = dlg.m_AreaInfo.singleplayer_only;
	exe.mod.mAreas.list[iIndex].stage_idx = dlg.m_AreaInfo.stage_idx;

	reloadMapList();
}

void CCaveEditorDlg::OnBnClickedAreaEdit()
{
	int iIndex = m_ListAreas.GetCurSel();
	if (iIndex == LB_ERR)
	{
		MessageBox("Select an area first.", "Error");
		return;
	}

	CEditAreaDlg dlg;

	strcpy_s(dlg.m_AreaInfo.name, sizeof(dlg.m_AreaInfo.name), exe.mod.mAreas.list[iIndex].name);
	dlg.m_AreaInfo.stage_idx         = exe.mod.mAreas.list[iIndex].stage_idx;
	dlg.m_AreaInfo.keep_on_release   = exe.mod.mAreas.list[iIndex].keep_on_release;
	dlg.m_AreaInfo.singleplayer_only = exe.mod.mAreas.list[iIndex].singleplayer_only;

	if (dlg.DoModal() != IDOK)
		return;

	strcpy(exe.mod.mAreas.list[iIndex].name, dlg.m_AreaInfo.name);
	exe.mod.mAreas.list[iIndex].keep_on_release   = dlg.m_AreaInfo.keep_on_release;
	exe.mod.mAreas.list[iIndex].singleplayer_only = dlg.m_AreaInfo.singleplayer_only;
	exe.mod.mAreas.list[iIndex].stage_idx         = dlg.m_AreaInfo.stage_idx;

	reloadMapList();
}

void CCaveEditorDlg::OnBnClickedAreaDelete()
{
	int iIndex = m_ListAreas.GetCurSel();
	if (iIndex == LB_ERR)
	{
		MessageBox("Select an area first.", "Error");
		return;
	}

	if (exe.mod.mAreas.count == 1)
	{
		MessageBox("Mod must have at least one area!", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	// Remove the area
	exe.mod.RemoveArea(exe.mod.mAreas.list[iIndex].name);

	// Reload the list
	reloadMapList();
}


void CCaveEditorDlg::OnLbnDblclkAreas()
{
	OnBnClickedAreaEdit();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void CCaveEditorDlg::OnBnClickedMusicAdd()
{
	/*
		PXMOD_MUSIC_TYPE_INVALID = 0,
	PXMOD_MUSIC_TYPE_ORGANYA = 1,
	PXMOD_MUSIC_TYPE_PTCOP   = 2,
	PXMOD_MUSIC_TYPE_WAV     = 3,
	PXMOD_MUSIC_TYPE_MP3     = 4,
	PXMOD_MUSIC_TYPE_OGG     = 5,
	*/
	int i;
	char szFilters[] = "All Valid Files|*.org;*.ptcop;*.wav;*.ogg;*.mp3;music.tbl|Organya Music File (*.org)|*.org|PxTone Music File (*.ptcop)|*.ptcop|Waveform Audio File (*.wav)|*.wav|Ogg Vorbis Audio File (*.ogg)|*.ogg|MP3 Audio File (*.mp3)|*.mp3|Music.tbl (music.tbl)|music.tbl||";
	char buffer[1024];
	char temp[64];

	// Create an Open dialog
	CFileDialog loadFileDialog(TRUE, "*.org", NULL, OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	if (loadFileDialog.DoModal() != IDOK)
		return;

	char RequiredFilePath[1024];

	sprintf_s(RequiredFilePath, sizeof(RequiredFilePath), "%s/Music", exe.mod.mModPath);

	POSITION pos(loadFileDialog.GetStartPosition());
	int iModPathLen = strlen(RequiredFilePath);
	while (pos)
	{
		CString filename = loadFileDialog.GetNextPathName(pos);

		while (filename.Find('\\', 0) != -1)
			filename.SetAt(filename.Find('\\', 0), '/');

		if (strrchr(filename.GetBuffer(), '/') != NULL && !strcmpi(strrchr(filename.GetBuffer(), '/') + 1, "music.tbl"))
		{
			ImportMusicTbl(filename.GetBuffer());
			continue;
		}

		if (strstr(filename.GetBuffer(), exe.mod.mModPath) != filename.GetBuffer())
			continue;
		
		const char* pFileName = filename.GetBuffer() + iModPathLen + 1;
		if (
			(strlen(pFileName) >= 5 && !strcmpi((pFileName + (strlen(pFileName) - 4)), ".org")) ||
			(strlen(pFileName) >= 7 && !strcmpi((pFileName + (strlen(pFileName) - 6)), ".ptcop")) ||
			(strlen(pFileName) >= 5 && !strcmpi((pFileName + (strlen(pFileName) - 4)), ".wav")) ||
			(strlen(pFileName) >= 5 && !strcmpi((pFileName + (strlen(pFileName) - 4)), ".mp3")) ||
			(strlen(pFileName) >= 5 && !strcmpi((pFileName + (strlen(pFileName) - 4)), ".ogg"))
			)
		{
			filename.SetAt(filename.ReverseFind('.'), 0);
			exe.mod.AddMusic(pFileName);
		}
	}

	reloadMapList();
}

void CCaveEditorDlg::OnBnClickedMusicDelete()
{
	int iIndex = m_ListMusic.GetCurSel();
	if (iIndex == LB_ERR)
	{
		MessageBox("Select a piece of music first.", "Error");
		return;
	}

	// Remove the area
	exe.mod.RemoveMusic(exe.mod.mMusic.list[iIndex].name);

	// Remove from the list
	m_ListMusic.DeleteString(iIndex);

	// Set the new selection
	m_ListMusic.SetCurSel(min(iIndex, exe.mod.mMusic.count - 1));
}

void CCaveEditorDlg::OnBnClickedMusicPlay()
{
	// Stop playing if we're done
	if (IsOrganyaPlaying())
	{
		StopOrganyaMusic();

		// Set text appropriately
		GetDlgItem(BUTTON_MUSIC_PLAY)->SetWindowTextA("Play Music");

		return;
	}

	int iIndex = m_ListMusic.GetCurSel();
	if (iIndex == LB_ERR)
	{
		MessageBox("Select a piece of music first.", "Error");
		return;
	}

	// Remove the area
	PlayOrganyaTrack(iIndex);

	// Set text appropriately
	if (IsOrganyaPlaying())
		GetDlgItem(BUTTON_MUSIC_PLAY)->SetWindowTextA("Stop Music");
}

void CCaveEditorDlg::OnBnClickedMusicEdit()
{
	MessageBox("Not Implemented!", "ERROR", 0);
}

void CCaveEditorDlg::OnBnClickedGameplayEdit()
{
	RECT rect;

	//select context specific menu
	GetDlgItem(BUTTON_GAMEPLAY_EDIT)->GetClientRect(&rect);
	GetDlgItem(BUTTON_GAMEPLAY_EDIT)->ClientToScreen(&rect);

	CMenu* pPopup = conMenu.GetSubMenu(7);
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rect.left, rect.bottom, this);
}

void CCaveEditorDlg::OnBnClickedMapImport()
{
	int i;
	char szFilters[] = "All Files (*.pxm, stage.tbl, mrmap.bin)|*.pxm;stage.tbl;mrmap.bin|Pixel Map File|*.pxm|Stage Table|stage.tbl|Master Record Map Table|mrmap.bin||";
	char buffer[1024];
	char temp[64];

	// Create an Open dialog
	CFileDialog loadFileDialog(TRUE, "*.pxm;stage.tbl;mrmap.bin", NULL, OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	TCHAR pFileBuffer[MAX_PATH * 256];
	*pFileBuffer = 0;
	loadFileDialog.GetOFN().lpstrFile = pFileBuffer;
	loadFileDialog.GetOFN().nMaxFile  = sizeof(pFileBuffer);

	if (loadFileDialog.DoModal() != IDOK)
		return;

	// Build the required path
	char RequiredFilePath[1024];
	sprintf_s(RequiredFilePath, sizeof(RequiredFilePath), "%s/Stage", exe.mod.mModPath);

	POSITION pos = loadFileDialog.GetStartPosition();
	int iModPathLen = strlen(RequiredFilePath);

	while (pos)
	{
		CString filename = loadFileDialog.GetNextPathName(pos);

		if (strstr(filename.GetBuffer(), "stage.tbl"))
		{
			ImportStageTbl(filename.GetBuffer());
			continue;
		}
		else if (strstr(filename.GetBuffer(), "mrmap.bin"))
		{
			ImportMrMap(filename.GetBuffer());
			continue;
		}

		while (filename.Find('\\', 0) != -1)
			filename.SetAt(filename.Find('\\', 0), '/');

		if (strstr(filename.GetBuffer(), RequiredFilePath) != filename.GetBuffer())
			continue;

		const char* pFileName = filename.GetBuffer() + iModPathLen + 1;
		if (
			strlen(pFileName) >= 4 &&
			pFileName[strlen(pFileName) - 4] == '.' &&
			pFileName[strlen(pFileName) - 3] == 'p' &&
			pFileName[strlen(pFileName) - 2] == 'x' &&
			pFileName[strlen(pFileName) - 1] == 'm'
			)
		{
			filename.SetAt(filename.ReverseFind('.'), 0);

			exe.mod.AddStage(pFileName, pFileName, "0", "0", "0", "0", PXMOD_BK_TYPE::PXMOD_BK_FIXED, PXMOD_BOSS_TYPE::PXMOD_BOSS_NONE);
		}
	}

	reloadMapList();
}

unsigned char* LoadFileToMemory(const char* file_path, size_t* file_size)
{
	unsigned char* buffer = NULL;

	FILE* file = fopen(file_path, "rb");

	if (file != NULL)
	{
		if (!fseek(file, 0, SEEK_END))
		{
			const long _file_size = ftell(file);

			if (_file_size >= 0)
			{
				rewind(file);
				buffer = (unsigned char*)malloc(_file_size);

				if (buffer != NULL)
				{
					if (fread(buffer, _file_size, 1, file) == 1)
					{
						fclose(file);
						*file_size = (size_t)_file_size;
						return buffer;
					}

					free(buffer);
				}
			}
		}

		fclose(file);
	}

	return NULL;
}

struct STAGE_TABLE
{
	char parts[0x20];
	char map[0x20];
	int bkType;
	char back[0x20];
	char npc[0x20];
	char boss[0x20];
	signed char boss_no;
	char name[0x22];
};

void CCaveEditorDlg::ImportMusicTbl(const char* pFileName)
{
	unsigned char* file_buffer;
	size_t file_size;

	file_buffer = LoadFileToMemory(pFileName, &file_size);

	if (!file_buffer)
		return;

	const unsigned long entry_count = file_size / 0x10;
	ErrorWindow ProgWindow;

	char pBuffer[512];
	sprintf(pBuffer, "Importing \"%s\"...\n\n(%d / %d)", pFileName, 1, entry_count);

	ProgWindow.SetProgressBarShown(true);
	ProgWindow.SetProgressMinMax(0, entry_count);
	ProgWindow.SetProgress(0);
	ProgWindow.SetTitle("Import MUSIC.TBL");
	ProgWindow.SetDescription(pBuffer);
	ProgWindow.SetButtonEnabled_Cancel(true);
	ProgWindow.SetButtonEnabled_OK(false);
	ProgWindow.SetShown(true);
	ProgWindow.Process();

	for (unsigned long i = 0; i < entry_count; ++i)
	{
		const char* pMusicFile = (const char*)(file_buffer + i * 0x10);

		ProgWindow.AddContent("Importing music @ 0x%08X\r\n", (i * 0x10));

		// Update the error window's content
		ProgWindow.SetProgress(1 + i);
		sprintf(pBuffer, "Importing \"%s\"...\n\n(%d / %d)", pFileName, i + 1, entry_count);
		ProgWindow.SetDescription(pBuffer);

		// Make sure the resources are in the mod
		if (exe.mod.FindMusic(pMusicFile))
		{
			ProgWindow.AddContent("  > Registering music file \"%s\"...\r\n", pMusicFile);

			if (!exe.mod.AddMusic(pMusicFile))
				ProgWindow.AddContent("    > Failed!\r\n");
		}
		else
			ProgWindow.AddContent("  > Skipping music file \"%s\" (already registered)\r\n", pMusicFile);

		ProgWindow.AddContent("\r\n");

		ProgWindow.Process();
	}

	free(file_buffer);

	// Set ending state
	sprintf(pBuffer, "Imported \"%s\"!\n\n(DONE)", pFileName);
	ProgWindow.SetProgressBarShown(false);
	ProgWindow.SetDescription(pBuffer);
	ProgWindow.SetButtonEnabled_OK(true);

	// Wait forever until they press OK.
	while (!ProgWindow.WasOkPressed() && !ProgWindow.WasCancelPressed())
		ProgWindow.Process();

	ProgWindow.SetShown(false);
	reloadMapList();
}

void CCaveEditorDlg::ImportMrMap(const char* pFileName)
{
	unsigned char* file_buffer;
	size_t file_size;

	file_buffer = LoadFileToMemory(pFileName, &file_size);

	if (file_buffer == NULL)
		return;

	const unsigned long entry_count = file_buffer[0] | (file_buffer[1] << 8) | (file_buffer[2] << 16) | (file_buffer[3] << 24);
	ErrorWindow ProgWindow;

	char pBuffer[512];
	sprintf(pBuffer, "Importing \"%s\"...\n\n(%d / %d)", pFileName, 1, entry_count);

	ProgWindow.SetProgressBarShown(true);
	ProgWindow.SetProgressMinMax(0, entry_count);
	ProgWindow.SetProgress(0);
	ProgWindow.SetTitle("Import MRMAP.BIN");
	ProgWindow.SetDescription(pBuffer);
	ProgWindow.SetButtonEnabled_Cancel(true);
	ProgWindow.SetButtonEnabled_OK(false);
	ProgWindow.SetShown(true);
	ProgWindow.Process();

	LONG lIdle = 0;

	for (unsigned long i = 0; i < entry_count; ++i)
	{
		STAGE_TABLE pTMT;

		unsigned char* entry = file_buffer + 4 + i * 0x74;

		memcpy(pTMT.parts,			entry, 0x10);
		memcpy(pTMT.map,			entry + 0x10, 0x10);
		pTMT.bkType =				entry[0x20];
		memcpy(pTMT.back,			entry + 0x21, 0x10);
		memcpy(pTMT.npc,			entry + 0x31, 0x10);
		memcpy(pTMT.boss,			entry + 0x41, 0x10);
		pTMT.boss_no =				entry[0x51];
		memcpy(pTMT.name,			entry + 0x52, 0x22);

		ProgWindow.AddContent("Importing stage @ 0x%08X\r\n", (4 + i * 0x74));

		// Update the error window's content
		ProgWindow.SetProgress(1 + i);
		sprintf(pBuffer, "Importing \"%s\"...\n\n(%d / %d)", pFileName, i + 1, entry_count);
		ProgWindow.SetDescription(pBuffer);

		// Make sure the resources are in the mod
		if (*pTMT.back && strlen(pTMT.back) >= 3)
		{
			if (exe.mod.FindBackground(pTMT.back + 2) == -1)
			{
				ProgWindow.AddContent("  > Registering background \"%s\"...\r\n", pTMT.back + 2);

				if (!exe.mod.AddBackground(pTMT.back + 2))
					ProgWindow.AddContent("    > Failed!\r\n");
			}
			else
				ProgWindow.AddContent("  > Skipping background \"%s\" (already registered)\r\n", pTMT.back + 2);
		}
		
		// Add the tileset
		if (exe.mod.FindTileset(pTMT.parts) == -1)
		{
			ProgWindow.AddContent("  > Registering tileset \"%s\"...\r\n", pTMT.parts);
			if (!exe.mod.AddTileset(pTMT.parts))
				ProgWindow.AddContent("    > Failed!\r\n");
		}
		else
			ProgWindow.AddContent("  > Skipping tileset \"%s\" (already registered)\r\n", pTMT.parts);

		// Add the NPC spritesheet 1
		if (exe.mod.FindSpritesheet(pTMT.npc) == -1)
		{
			ProgWindow.AddContent("  > Registering spritesheet \"%s\"...\r\n", pTMT.npc);
			if (!exe.mod.AddSpritesheet(pTMT.npc))
				ProgWindow.AddContent("    > Failed!\r\n");
		}
		else
			ProgWindow.AddContent("  > Skipping spritesheet \"%s\" (already registered)\r\n", pTMT.npc);
		
		// Add the NPC spritesheet 2
		if (exe.mod.FindSpritesheet(pTMT.boss) == -1)
		{
			ProgWindow.AddContent("  > Registering spritesheet \"%s\"...\r\n", pTMT.boss);
			if (!exe.mod.AddSpritesheet(pTMT.boss))
				ProgWindow.AddContent("    > Failed!\r\n");
		}
		else
			ProgWindow.AddContent("  > Skipping spritesheet \"%s\" (already registered)\r\n", pTMT.boss);
		
		// Now add the stage
		ProgWindow.AddContent("  > Registering stage \"%s\" (%s)\r\n", pTMT.name, pTMT.map);

		if (!exe.mod.AddStage(pTMT.map, pTMT.name, pTMT.parts, pTMT.npc, pTMT.boss, (*pTMT.back && strlen(pTMT.back) >= 3 ? pTMT.back + 2 : pTMT.back), (PXMOD_BK_TYPE)pTMT.bkType, (PXMOD_BOSS_TYPE)pTMT.boss_no))
		{
			ProgWindow.AddContent("    > Failed to register this stage.\r\n");
			ProgWindow.AddContent("      This could be caused by missing assets.\r\n\r\n");
		}

		ProgWindow.AddContent("\r\n");

		ProgWindow.Process();
	}

	free(file_buffer);

	// Set ending state
	sprintf(pBuffer, "Imported \"%s\"!\n\n(DONE)", pFileName);
	ProgWindow.SetProgressBarShown(false);
	ProgWindow.SetDescription(pBuffer);
	ProgWindow.SetButtonEnabled_OK(true);

	// Wait forever until they press OK.
	while (!ProgWindow.WasOkPressed() && !ProgWindow.WasCancelPressed())
		ProgWindow.Process();

	ProgWindow.SetShown(false);
}

void CCaveEditorDlg::ImportStageTbl(const char* pFileName)
{
	unsigned char* file_buffer;
	size_t file_size;

	file_buffer = LoadFileToMemory(pFileName, &file_size);

	if (!file_buffer)
		return;

	const unsigned long entry_count = file_size / 0xE5;
	ErrorWindow ProgWindow;

	char pBuffer[512];
	sprintf(pBuffer, "Importing \"%s\"...\n\n(%d / %d)", pFileName, 1, entry_count);

	ProgWindow.SetProgressBarShown(true);
	ProgWindow.SetProgressMinMax(0, entry_count);
	ProgWindow.SetProgress(0);
	ProgWindow.SetTitle("Import STAGE.TBL");
	ProgWindow.SetDescription(pBuffer);
	ProgWindow.SetButtonEnabled_Cancel(true);
	ProgWindow.SetButtonEnabled_OK(false);
	ProgWindow.SetShown(true);
	ProgWindow.Process();

	for (unsigned long i = 0; i < entry_count; ++i)
	{
		STAGE_TABLE pTMT;

		unsigned char* entry = file_buffer + i * 0xE5;

		memcpy(pTMT.parts,			entry, 0x20);
		memcpy(pTMT.map,			entry + 0x20, 0x20);
		pTMT.bkType =				(entry[0x40 + 3] << 24) | (entry[0x40 + 2] << 16) | (entry[0x40 + 1] << 8) | entry[0x40];
		memcpy(pTMT.back,			entry + 0x44, 0x20);
		memcpy(pTMT.npc,			entry + 0x64, 0x20);
		memcpy(pTMT.boss,			entry + 0x84, 0x20);
		pTMT.boss_no =				entry[0xA4];
		memcpy(pTMT.name,			entry + 0xC5, 0x20);

		ProgWindow.AddContent("Importing stage @ 0x%08X\r\n", (i * 0xE5));

		// Update the error window's content
		ProgWindow.SetProgress(1 + i);
		sprintf(pBuffer, "Importing \"%s\"...\n\n(%d / %d)", pFileName, i + 1, entry_count);
		ProgWindow.SetDescription(pBuffer);

		// Make sure the resources are in the mod
		if (*pTMT.back && strlen(pTMT.back) >= 3)
		{
			if (exe.mod.FindBackground(pTMT.back + 2) == -1)
			{
				ProgWindow.AddContent("  > Registering background \"%s\"...\r\n", pTMT.back + 2);

				if (!exe.mod.AddBackground(pTMT.back + 2))
					ProgWindow.AddContent("    > Failed!\r\n");
			}
			else
				ProgWindow.AddContent("  > Skipping background \"%s\" (already registered)\r\n", pTMT.back + 2);
		}

		// Add the tileset
		if (exe.mod.FindTileset(pTMT.parts) == -1)
		{
			ProgWindow.AddContent("  > Registering tileset \"%s\"...\r\n", pTMT.parts);
			if (!exe.mod.AddTileset(pTMT.parts))
				ProgWindow.AddContent("    > Failed!\r\n");
		}
		else
			ProgWindow.AddContent("  > Skipping tileset \"%s\" (already registered)\r\n", pTMT.parts);

		// Add the NPC spritesheet 1
		if (exe.mod.FindSpritesheet(pTMT.npc) == -1)
		{
			ProgWindow.AddContent("  > Registering spritesheet \"%s\"...\r\n", pTMT.npc);
			if (!exe.mod.AddSpritesheet(pTMT.npc))
				ProgWindow.AddContent("    > Failed!\r\n");
		}
		else
			ProgWindow.AddContent("  > Skipping spritesheet \"%s\" (already registered)\r\n", pTMT.npc);

		// Add the NPC spritesheet 2
		if (exe.mod.FindSpritesheet(pTMT.boss) == -1)
		{
			ProgWindow.AddContent("  > Registering spritesheet \"%s\"...\r\n", pTMT.boss);
			if (!exe.mod.AddSpritesheet(pTMT.boss))
				ProgWindow.AddContent("    > Failed!\r\n");
		}
		else
			ProgWindow.AddContent("  > Skipping spritesheet \"%s\" (already registered)\r\n", pTMT.boss);

		// Now add the stage
		ProgWindow.AddContent("  > Registering stage \"%s\" (%s)\r\n", pTMT.name, pTMT.map);

		if (!exe.mod.AddStage(pTMT.map, pTMT.name, pTMT.parts, pTMT.npc, pTMT.boss, (*pTMT.back && strlen(pTMT.back) >= 3 ? pTMT.back + 2 : pTMT.back), (PXMOD_BK_TYPE)pTMT.bkType, (PXMOD_BOSS_TYPE)pTMT.boss_no))
		{
			ProgWindow.AddContent("    > Failed to register this stage.\r\n");
			ProgWindow.AddContent("      This could be caused by missing assets.\r\n\r\n");
		}

		ProgWindow.AddContent("\r\n");

		ProgWindow.Process();
	}

	free(file_buffer);

	// Set ending state
	sprintf(pBuffer, "Imported \"%s\"!\n\n(DONE)", pFileName);
	ProgWindow.SetProgressBarShown(false);
	ProgWindow.SetDescription(pBuffer);
	ProgWindow.SetButtonEnabled_OK(true);

	// Wait forever until they press OK.
	while (!ProgWindow.WasOkPressed() && !ProgWindow.WasCancelPressed())
		ProgWindow.Process();

	ProgWindow.SetShown(false);
}

void CCaveEditorDlg::OnBnClickedMusicUp()
{
	int iIndex = m_ListMusic.GetCurSel();
	if (iIndex == LB_ERR)
	{
		MessageBox("Select a piece of music first.", "Error");
		return;
	}

	if (iIndex == 0)
		return;

	// Move the memory up
	PXMOD_MUSIC_STRUCT temp;
	memcpy(&temp, &exe.mod.mMusic.list[iIndex], sizeof(PXMOD_MUSIC_STRUCT));
	memcpy(&exe.mod.mMusic.list[iIndex], &exe.mod.mMusic.list[iIndex - 1], sizeof(PXMOD_MUSIC_STRUCT));
	memcpy(&exe.mod.mMusic.list[iIndex - 1], &temp, sizeof(PXMOD_MUSIC_STRUCT));

	// Refresh the music list
	char buffer[1024];
	m_ListMusic.ResetContent();
	for (int i = 0; i < exe.mod.mMusic.count; i++)
	{
		sprintf(buffer, "[%.3i] %s", i, exe.mod.mMusic.list[i].name);
		m_ListMusic.AddString(buffer);
	}

	m_ListMusic.SetCurSel(iIndex - 1);
	OnLbnSelchangeMusic();
}

void CCaveEditorDlg::OnBnClickedMusicDown()
{
	int iIndex = m_ListMusic.GetCurSel();
	if (iIndex == LB_ERR)
	{
		MessageBox("Select a piece of music first.", "Error");
		return;
	}

	if (iIndex == exe.mod.mMusic.count - 1)
		return;

	// Move the memory up
	PXMOD_MUSIC_STRUCT temp;
	memcpy(&temp, &exe.mod.mMusic.list[iIndex], sizeof(PXMOD_MUSIC_STRUCT));
	memcpy(&exe.mod.mMusic.list[iIndex], &exe.mod.mMusic.list[iIndex + 1], sizeof(PXMOD_MUSIC_STRUCT));
	memcpy(&exe.mod.mMusic.list[iIndex + 1], &temp, sizeof(PXMOD_MUSIC_STRUCT));

	// Refresh the music list
	char buffer[1024];
	m_ListMusic.ResetContent();
	for (int i = 0; i < exe.mod.mMusic.count; i++)
	{
		sprintf(buffer, "[%.3i] %s", i, exe.mod.mMusic.list[i].name);
		m_ListMusic.AddString(buffer);
	}

	m_ListMusic.SetCurSel(iIndex + 1);
	OnLbnSelchangeMusic();
}

void CCaveEditorDlg::OnLbnSelchangeMusic()
{
	int iIndex = m_ListMusic.GetCurSel();

	GetDlgItem(BUTTON_MUSIC_UP)->EnableWindow(iIndex == 0 ? FALSE : TRUE);
	GetDlgItem(BUTTON_MUSIC_DOWN)->EnableWindow(iIndex == exe.mod.mMusic.count - 1 ? FALSE : TRUE);
}


void CAboutDlg::OnStnClickedIdentifierText()
{
	// TODO: Add your control notification handler code here
}

bool GetProductAndVersion(CStringA & strProductVersion)
{
    // get the filename of the executable containing the version resource
    TCHAR szFilename[MAX_PATH + 1] = {0};
    if (GetModuleFileName(NULL, szFilename, MAX_PATH) == 0)
    {
        TRACE("GetModuleFileName failed with error %d\n", GetLastError());
        return false;
    }

    // allocate a block of memory for the version info
    DWORD dummy;
    DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
    if (dwSize == 0)
    {
        TRACE("GetFileVersionInfoSize failed with error %d\n", GetLastError());
        return false;
    }
    std::vector<BYTE> data(dwSize);

    // load the version info
    if (!GetFileVersionInfo(szFilename, NULL, dwSize, &data[0]))
    {
        TRACE("GetFileVersionInfo failed with error %d\n", GetLastError());
        return false;
    }

    // get the name and version strings
    LPVOID pvProductVersion = NULL;
    unsigned int iProductVersionLen = 0;

    // replace "040904e4" with the language ID of your resources
    if (!VerQueryValue(&data[0], _T("\\StringFileInfo\\040904e4\\ProductVersion"), &pvProductVersion, &iProductVersionLen))
    {
        TRACE("Can't obtain ProductVersion from resources\n");
        return false;
    }

    strProductVersion.SetString((LPCSTR)pvProductVersion, iProductVersionLen);

    return true;
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString editor_ver;
	GetProductAndVersion(editor_ver);

	char pBuffer[1024];

	sprintf(pBuffer, "CaveEditor Version %s", editor_ver.GetString());
	SetDlgItemText(VERSION_IDENTIFIER_TEXT, pBuffer);

	return TRUE;
}