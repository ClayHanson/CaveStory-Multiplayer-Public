// CaveMapPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "General.h"
#include "CaveEditor.h"
#include "CaveMapInfoDlg.h"
#include "CaveMapEditDlg.h"
#include "CaveEditorDlg.h"
#include "CaveMapPropertiesDlg.h"
#include ".\cavemappropertiesdlg.h"

extern CCaveEditorDlg* mainWindow;
//#include "SharedWindows.h"

// CaveMapPropertiesDlg dialog

IMPLEMENT_DYNAMIC(CaveMapPropertiesDlg, CDialog)
CaveMapPropertiesDlg::CaveMapPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CaveMapPropertiesDlg::IDD, pParent)
{
}

CaveMapPropertiesDlg::~CaveMapPropertiesDlg()
{
}

BOOL CaveMapPropertiesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString item;
	int i, j;
	changes = false;
	redraw = false;

	SetDlgItemText(EDIT_MAP_NAME, caveInfo->fileName);
	SetDlgItemText(EDIT_MAP_CAPTION, caveInfo->caption);

	if (width > 0)//this is to account for opening the map properties with a map open
	{
		SetDlgItemInt(EDIT_MAP_SIZEX, width);
		SetDlgItemInt(EDIT_MAP_SIZEY, height);
	}
	else
	{
		GetDlgItem(EDIT_MAP_SIZEX)->EnableWindow(false);
		GetDlgItem(EDIT_MAP_SIZEY)->EnableWindow(false);
	}

	SetDlgItemText(EDIT_MAP_NAME, caveInfo->fileName);

	j = mainWindow->m_ListTilesets.GetCount();
	for (i = 0; i < j; i++)//transfer names from main list to combo box
	{
		mainWindow->m_ListTilesets.GetText(i, item);
		m_mapTileset.AddString(item);
	};
	m_mapTileset.SetCurSel(m_mapTileset.FindStringExact(0, caveInfo->tileset));

	//these are all hard coded boss IDs may add ability to place any number here but why?
	m_backScrollType.AddString("0 - fixed");
	m_backScrollType.AddString("1 - move slow");
	m_backScrollType.AddString("2 - follow forground");
	m_backScrollType.AddString("3 - hide");
	m_backScrollType.AddString("4 - hide");
	m_backScrollType.AddString("5 - scroll fast left");
	m_backScrollType.AddString("6 - scroll left (layered), items falling left");
	m_backScrollType.AddString("7 - scroll left (layered)");
	m_backScrollType.AddString("8 - hide");
	m_backScrollType.SetCurSel(caveInfo->backType);

	j = mainWindow->m_ListBackgrounds.GetCount();
	for (i = 0; i < j; i++)//transfer names from main list to combo box
	{
		mainWindow->m_ListBackgrounds.GetText(i, item);
		m_backImage.AddString(item);
	};
	m_backImage.SetCurSel(m_backImage.FindStringExact(0, caveInfo->background));

	j = mainWindow->m_ListSpritesets.GetCount();
	for (i = 0; i < j; i++)//transfer names from main list to combo box
	{
		mainWindow->m_ListSpritesets.GetText(i, item);
		m_npcTileset1.AddString(item);
	};
	m_npcTileset1.SetCurSel(m_npcTileset1.FindStringExact(0, caveInfo->NPCset1));

	j = mainWindow->m_ListSpritesets.GetCount();
	for (i = 0; i < j; i++)//transfer names from main list to combo box
	{
		mainWindow->m_ListSpritesets.GetText(i, item);
		m_npcTileset2.AddString(item);
	};
	m_npcTileset2.SetCurSel(m_npcTileset2.FindStringExact(0, caveInfo->NPCset2));

	//these are all hard coded boss IDs may add ability to place any number here but why?
	m_bossID.AddString("0 - No special boss");
	m_bossID.AddString("1 - Sand Zone Boss");
	m_bossID.AddString("2 - Balrog Frog");
	m_bossID.AddString("3 - Weapon X");
	m_bossID.AddString("4 - Core");
	m_bossID.AddString("5 - Fish Boss (Ironhead)");
	m_bossID.AddString("6 - Dragon Sisters");
	m_bossID.AddString("7 - Undead Core");
	m_bossID.AddString("8 - Heavy Press");
	m_bossID.AddString("9 - Ballos");
	m_bossID.SetCurSel(caveInfo->bossNum);

	m_FocusCenterX.SetCheck(caveInfo->focus_center_x);
	m_FocusCenterY.SetCheck(caveInfo->focus_center_y);

	while (m_area.GetCount()) m_area.DeleteString(0);

	char buffer[1024];
	for (i = 0; i < exe.mod.mAreas.count; i++)
	{
		PXMOD_AREA_STRUCT* pArea = &exe.mod.mAreas.list[i];

		sprintf(buffer, "[%.3i] %s", i, pArea->name);

		m_area.AddString(buffer);
	}
	m_area.SetCurSel(caveInfo->area);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CaveMapPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, COMBO_MAP_BACKGROUND_SCROLL, m_backScrollType);
	DDX_Control(pDX, COMBO_MAP_BACKGROUND, m_backImage);
	DDX_Control(pDX, COMBO_MAP_TILESET, m_mapTileset);
	DDX_Control(pDX, COMBO_MAP_NPC_TILESET1, m_npcTileset1);
	DDX_Control(pDX, COMBO_MAP_NPC_TILESET2, m_npcTileset2);
	DDX_Control(pDX, COMBO_BOSS_ID, m_bossID);
	DDX_Control(pDX, COMBO_AREA, m_area);
	DDX_Control(pDX, CHECK_FOCUS_X, m_FocusCenterX);
	DDX_Control(pDX, CHECK_FOCUS_Y, m_FocusCenterY);
}


BEGIN_MESSAGE_MAP(CaveMapPropertiesDlg, CDialog)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(COMBO_MAP_TILESET, OnCbnSelchangeMapTileset)
	ON_CBN_SELCHANGE(COMBO_MAP_NPC_TILESET1, OnCbnSelchangeMapNpcTileset1)
	ON_CBN_SELCHANGE(COMBO_MAP_NPC_TILESET2, OnCbnSelchangeMapNpcTileset2)
	ON_CBN_SELCHANGE(COMBO_MAP_BACKGROUND, OnCbnSelchangeMapBackground)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_SELCHANGE(COMBO_AREA, &CaveMapPropertiesDlg::OnCbnSelchangeArea)
	ON_BN_CLICKED(CHECK_FOCUS_X, &CaveMapPropertiesDlg::OnBnClickedFocusX)
	ON_BN_CLICKED(CHECK_FOCUS_Y, &CaveMapPropertiesDlg::OnBnClickedFocusY)
END_MESSAGE_MAP()


// CaveMapPropertiesDlg message handlers

void CaveMapPropertiesDlg::OnOK()
{
	CFileFind file;
	char buffer[1024];
	char buffer2[1024];
	//store all new info

	CString temp;
	int i;

	GetDlgItemText(EDIT_MAP_NAME, temp);
	if (temp != oldFileName)//see if map with this name already exists
	{
		for (i = 0; i < exe.mod.mStages.count; i++)
		{
			if (strcmp(exe.mod.mStages.list[i].file, temp) == 0)
			{
				MessageBox("Map with this name already exists!", "Error!");
				return;
			}
		}
		sprintf(buffer, "%s/Stage/%s.tsc", exe.mod.mModPath, (const char*)temp);
		sprintf(buffer2, "%s/Stage/%s.tsc", exe.mod.mModPath, (const char*)oldFileName);
		if (file.FindFile(buffer2))
		{
			::rename(buffer2, buffer);
		}
		sprintf(buffer, "%s/Stage/%s.pxm", exe.mod.mModPath, (const char*)temp);
		sprintf(buffer2, "%s/Stage/%s.pxm", exe.mod.mModPath, (const char*)oldFileName);
		if (file.FindFile(buffer2))
		{
			::rename(buffer2, buffer);
		}

		sprintf(buffer, "%s/Stage/%s.pxe", exe.mod.mModPath, (const char*)temp);
		sprintf(buffer2, "%s/Stage/%s.pxe", exe.mod.mModPath, (const char*)oldFileName);
		if (file.FindFile(buffer2))
		{
			::rename(buffer2, buffer);
		}
	}

	if (temp != caveInfo->fileName)
	{
		changes = true;
		strcpy(caveInfo->fileName, temp);
	}

	//background and map tileset
	m_backImage.GetLBText(m_backImage.GetCurSel(), temp);
	if (strcmp(caveInfo->background, temp))
	{
		changes = true;
		redraw = true;
		strcpy(caveInfo->background, temp);
	}

	if (caveInfo->backType != m_backScrollType.GetCurSel())
	{
		changes = true;
		caveInfo->backType = m_backScrollType.GetCurSel();
	}

	m_mapTileset.GetLBText(m_mapTileset.GetCurSel(), temp);
	if (strcmp(caveInfo->tileset, temp))
	{
		changes = true;
		redraw = true;
		strcpy(caveInfo->tileset, temp);
	}

	//npc data
	m_npcTileset1.GetLBText(m_npcTileset1.GetCurSel(), temp);
	if (strcmp(caveInfo->NPCset1, temp))
	{
		changes = true;
		strcpy(caveInfo->NPCset1, temp);
	}
	m_npcTileset2.GetLBText(m_npcTileset2.GetCurSel(), temp);
	if (strcmp(caveInfo->NPCset2, temp))
	{
		changes = true;
		strcpy(caveInfo->NPCset2, temp);
	}
	if (caveInfo->bossNum != m_bossID.GetCurSel())
	{
		changes = true;
		caveInfo->bossNum = m_bossID.GetCurSel();
	}
	if (caveInfo->area != m_area.GetCurSel())
	{
		changes = true;
		caveInfo->area = m_area.GetCurSel();
	}

	if (caveInfo->focus_center_x != (m_FocusCenterX.GetState() & BST_CHECKED))
	{
		caveInfo->focus_center_x = (m_FocusCenterX.GetState() & BST_CHECKED);
		changes                  = true;
	}

	if (caveInfo->focus_center_y != (m_FocusCenterY.GetState() & BST_CHECKED))
	{
		caveInfo->focus_center_y = (m_FocusCenterY.GetState()& BST_CHECKED);
		changes                  = true;
	}

	GetDlgItemText(EDIT_MAP_NAME, temp);
	if (temp != caveInfo->fileName)
	{
		changes = true;
		strcpy(caveInfo->fileName, temp);
	}

	GetDlgItemText(EDIT_MAP_CAPTION, temp);
	if (temp != caveInfo->caption)
	{
		changes = true;
		strcpy(caveInfo->caption, temp);
	}

	if (width != GetDlgItemInt(EDIT_MAP_SIZEX) || height != GetDlgItemInt(EDIT_MAP_SIZEY))
	{
		changes = true;
		width = GetDlgItemInt(EDIT_MAP_SIZEX);
		height = GetDlgItemInt(EDIT_MAP_SIZEY);
	}

	if (changes)
	{
		CDialog::OnOK();
		exe.Save();
	}
	else
		CDialog::OnCancel();
}

void CaveMapPropertiesDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void CaveMapPropertiesDlg::OnCbnSelchangeMapTileset()
{
	CString temp;
	char buffer[1024];
	CaveBitMap bmp1;
	m_mapTileset.GetLBText(m_mapTileset.GetCurSel(), temp);
	sprintf(buffer, "%s/Stage/Prt%s.png", exe.mod.mModPath, (const char*)temp);
	bmp1.Load(buffer);

	RECT rect;
	CDC* cdc = GetDlgItem(FRAME_TILESET)->GetDC();
	GetDlgItem(FRAME_TILESET)->GetClientRect(&rect);
	cdc->FillSolidRect(&rect, 0);
	bmp1.DrawPart(0, 0, rect.right, rect.bottom, cdc, 0, 0, false);
	GetDlgItem(FRAME_TILESET)->ReleaseDC(cdc);
}

void CaveMapPropertiesDlg::OnCbnSelchangeMapNpcTileset1()
{
	CString temp;
	char buffer[1024];
	CaveBitMap bmp1;
	m_npcTileset1.GetLBText(m_npcTileset1.GetCurSel(), temp);
	sprintf(buffer, "%s/Npc/Npc%s.png", exe.mod.mModPath, (const char*)temp);
	bmp1.Load(buffer);

	RECT rect;
	CDC* cdc = GetDlgItem(FRAME_TILESET)->GetDC();
	GetDlgItem(FRAME_TILESET)->GetClientRect(&rect);
	cdc->FillSolidRect(&rect, 0);
	bmp1.DrawPart(0, 0, rect.right, rect.bottom, cdc, 0, 0, false);
	GetDlgItem(FRAME_TILESET)->ReleaseDC(cdc);
}

void CaveMapPropertiesDlg::OnCbnSelchangeMapNpcTileset2()
{
	CString temp;
	char buffer[1024];
	CaveBitMap bmp1;
	m_npcTileset2.GetLBText(m_npcTileset2.GetCurSel(), temp);
	sprintf(buffer, "%s/Npc/Npc%s.png", exe.mod.mModPath, (const char*)temp);
	bmp1.Load(buffer);

	RECT rect;
	CDC* cdc = GetDlgItem(FRAME_TILESET)->GetDC();
	GetDlgItem(FRAME_TILESET)->GetClientRect(&rect);
	cdc->FillSolidRect(&rect, 0);
	bmp1.DrawPart(0, 0, rect.right, rect.bottom, cdc, 0, 0, false);
	GetDlgItem(FRAME_TILESET)->ReleaseDC(cdc);
}


void CaveMapPropertiesDlg::OnCbnSelchangeMapBackground()
{
	CString temp;
	char buffer[1024];
	CaveBitMap bmp1;
	m_backImage.GetLBText(m_backImage.GetCurSel(), temp);
	sprintf(buffer, "%s/bk%s.png", exe.mod.mModPath, (const char*)temp);
	bmp1.Load(buffer);

	RECT rect;
	CDC* cdc = GetDlgItem(FRAME_BACKGROUND)->GetDC();
	GetDlgItem(FRAME_BACKGROUND)->GetClientRect(&rect);
	cdc->FillSolidRect(&rect, 0);
	bmp1.DrawPart(0, 0, rect.right, rect.bottom, cdc, 0, 0, false);
	GetDlgItem(FRAME_BACKGROUND)->ReleaseDC(cdc);
}

void CaveMapPropertiesDlg::OnCbnSelchangeArea()
{
	
}

void CaveMapPropertiesDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CaveMapPropertiesDlg::OnBnClickedFocusX()
{
	changes = true;
}

void CaveMapPropertiesDlg::OnBnClickedFocusY()
{
	changes = true;
}