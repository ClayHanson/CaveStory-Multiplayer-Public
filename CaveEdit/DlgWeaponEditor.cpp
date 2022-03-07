// DlgWeaponEditor.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "DlgWeaponEditor.h"
#include "afxdialogex.h"
#include "General.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(DlgWeaponEditor, CDialog)

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define CLAMP(X, AMIN, AMAX) ((X) > (AMAX) ? (AMAX) : ((X) < (AMIN) ? (AMIN) : (X)))
#define GET_CENTER_OFFSET(w1, w2) ((MAX((w1), (w2)) / 2) - (MIN((w1), (w2)) / 2))
#define RECT_WIDTH(ARECT) (ARECT.right - ARECT.left)
#define RECT_HEIGHT(ARECT) (ARECT.bottom - ARECT.top)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(DlgWeaponEditor, CDialog)
	ON_WM_PAINT()
	ON_EN_CHANGE(EDIT_WEAPON_NAME, OnEnChangeWeaponName)
	ON_BN_CLICKED(BUTTON_WEAPON_LEVEL_1, OnBnClickedWeaponLevel1)
	ON_BN_CLICKED(BUTTON_WEAPON_LEVEL_2, OnBnClickedWeaponLevel2)
	ON_BN_CLICKED(BUTTON_WEAPON_LEVEL_3, OnBnClickedWeaponLevel3)
	ON_EN_CHANGE(EDIT_WEAPON_EXP, OnEnChangeWeaponExp)
	ON_EN_CHANGE(EDIT_WEAPON_DISP_X, OnEnChangeWeaponDispX)
	ON_EN_CHANGE(EDIT_WEAPON_DISP_Y, OnEnChangeWeaponDispY)
	ON_EN_CHANGE(EDIT_WEAPON_DISP_W, OnEnChangeWeaponDispW)
	ON_EN_CHANGE(EDIT_WEAPON_DISP_H, OnEnChangeWeaponDispH)
	ON_LBN_SELCHANGE(LIST_WEAPONS, OnLbnSelchangeWeapons)
	ON_BN_CLICKED(BUTTON_WEAPON_ADD, OnBnClickedWeaponAdd)
	ON_BN_CLICKED(BUTTON_WEAPON_DUPLICATE, OnBnClickedWeaponDuplicate)
	ON_BN_CLICKED(BUTTON_WEAPON_DELETE, OnBnClickedWeaponDelete)
	ON_BN_CLICKED(BUTTON_WEAPON_SAVE, OnBnClickedWeaponSave)
	ON_BN_CLICKED(BUTTON_WEAPON_LOAD_PLAYER, OnBnClickedWeaponLoadPlayer)
	ON_BN_CLICKED(BUTTON_WEAPON_UNLOAD_PLAYER, OnBnClickedWeaponUnloadPlayer)
	ON_CBN_SELCHANGE(LIST_WEAPON_TEST_ANIMATIONS, OnCbnSelchangeWeaponTestAnimations)
	ON_EN_CHANGE(EDIT_WEAPON_OFFSET_X, OnEnChangeWeaponOffsetX)
	ON_EN_CHANGE(EDIT_WEAPON_OFFSET_Y, OnEnChangeWeaponOffsetY)
	ON_EN_CHANGE(EDIT_WEAPON_VIEW_LEFT, OnEnChangeWeaponViewLeft)
	ON_EN_CHANGE(EDIT_WEAPON_VIEW_TOP, OnEnChangeWeaponViewTop)
	ON_EN_CHANGE(EDIT_WEAPON_VIEW_RIGHT, OnEnChangeWeaponViewRight)
	ON_EN_CHANGE(EDIT_WEAPON_VIEW_BOTTOM, OnEnChangeWeaponViewBottom)
	ON_BN_CLICKED(CHECK_WEAPON_TEST_ALT_DIR, OnBnClickedWeaponTestAltDir)
	ON_CBN_SELCHANGE(COMBO_BULLET_LIST, OnCbnSelchangeBulletList)
	ON_CBN_SELCHANGE(COMBO_WEAPON_TEMPLATE, OnCbnSelchangeWeaponTemplate)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DlgWeaponEditor::DlgWeaponEditor(CWnd* pParent) : CDialog(DIALOG_GAMEEDIT_WEAPONS, pParent)
{
	m_TestPlayer        = NULL;
	m_SelectedLevel     = 0;
	m_IgnoreInput       = false;
	m_TestPlayer_AltDir = false;
}

DlgWeaponEditor::~DlgWeaponEditor()
{
	for (int i = 0; i < m_WeaponData.GetCount(); i++)
		delete m_WeaponData[i];

	m_ArmsImage.Destroy();
	m_Arms.Destroy();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgWeaponEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_WEAPONS, m_WeaponList);
	DDX_Control(pDX, LIST_WEAPON_TEST_ANIMATIONS, m_TestPlayerAnimList);
}

BOOL DlgWeaponEditor::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Initialize the weapon data list
	for (int i = 0; i < exe.mod.mWeapons.count; i++)
	{
		PXMOD_WEAPON_STRUCT* pNewWpn = new PXMOD_WEAPON_STRUCT();
		memcpy(pNewWpn, &exe.mod.mWeapons.list[i], sizeof(PXMOD_WEAPON_STRUCT));
		m_WeaponData.Add(pNewWpn);
	}

	// Load the weapon list
	Refresh();

	m_WeaponList.SetCurSel(0);
	LoadWeapon();

	// Populate the bullet list
	CComboBox* pBulList = (CComboBox*)GetDlgItem(COMBO_BULLET_LIST);
	while (pBulList->GetCount()) pBulList->DeleteString(0);
	for (int i = 0; i < exe.mod.mBullets.count; i++)
		pBulList->AddString(exe.mod.mBullets.list[i].name);

	char buffer[1024];
	sprintf(buffer, "%s/ArmsImage.png", exe.mod.mModPath);

	if (!m_ArmsImage.Load(buffer))
	{
		sprintf(buffer, "%s/../data/ArmsImage.png", exe.mod.mModPath);
		if (!m_ArmsImage.Load(buffer))
		{
			printf("failed to load the arms image!!!\r\n");
		}
	}
	
	sprintf(buffer, "%s/Arms.png", exe.mod.mModPath);

	if (!m_Arms.Load(buffer))
	{
		sprintf(buffer, "%s/../data/Arms.png", exe.mod.mModPath);
		if (!m_Arms.Load(buffer))
		{
			printf("failed to load arms!!!\r\n");
		}
	}

	SetIcon(m_hIcon, TRUE);		 // Set big icon
	SetIcon(m_hIcon, TRUE);	   // Set small icon

	Invalidate(TRUE);

	// Done!
	return TRUE;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgWeaponEditor::OnLbnSelchangeWeapons()
{
	LoadWeapon();
}

void DlgWeaponEditor::OnBnClickedWeaponAdd()
{
	PXMOD_WEAPON_STRUCT* pNewWpn = new PXMOD_WEAPON_STRUCT;

	// Configure the new weapon
	memset(pNewWpn, 0, sizeof(PXMOD_WEAPON_STRUCT));
	strcpy(pNewWpn->name, "Unnamed Weapon");

	// Add it to the list
	m_WeaponData.Add(pNewWpn);
	Refresh();
	m_WeaponList.SetCurSel(m_WeaponList.GetCount() - 1);
	LoadWeapon();

	CDC* cdc;
	cdc = GetDC();
	RenderArmsImage(*cdc);
	ReleaseDC(cdc);
}

void DlgWeaponEditor::OnBnClickedWeaponDuplicate()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR)
	{
		MessageBox("Select a weapon first.", "Error");
		return;
	}

	PXMOD_WEAPON_STRUCT* pNewWpn = new PXMOD_WEAPON_STRUCT;

	// Configure the new weapon
	memcpy(pNewWpn, m_WeaponData[iCurWpn], sizeof(PXMOD_WEAPON_STRUCT));
	strcpy(pNewWpn->name, m_WeaponData[iCurWpn]->name);
	strcat(pNewWpn->name, " (Duplicated)");

	// Add it to the list
	m_WeaponData.Add(pNewWpn);

	Refresh();
	m_WeaponList.SetCurSel(m_WeaponData.GetCount() - 1);
	LoadWeapon();
}

void DlgWeaponEditor::OnBnClickedWeaponDelete()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR)
	{
		MessageBox("Select a weapon first.", "Error");
		return;
	}

	delete m_WeaponData[iCurWpn];
	m_WeaponData.RemoveAt(iCurWpn, 1);
	Refresh();
	m_WeaponList.SetCurSel(m_WeaponData.GetCount() - 1);
	LoadWeapon();
}

void DlgWeaponEditor::OnBnClickedWeaponSave()
{
	SaveWeapons();
	OnOK();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgWeaponEditor::OnEnChangeWeaponName()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	GetDlgItem(EDIT_WEAPON_NAME)->GetWindowTextA(m_WeaponData[iCurWpn]->name, sizeof(m_WeaponData[iCurWpn]->name));
	
	char buffer[256];
	sprintf_s(buffer, sizeof(buffer), "[%.3i] %s", iCurWpn, m_WeaponData[iCurWpn]->name);
	m_WeaponList.DeleteString((UINT)iCurWpn);

	if (!m_WeaponList.GetCount())
		m_WeaponList.AddString(buffer);
	else
		m_WeaponList.InsertString((UINT)iCurWpn, buffer);

	m_WeaponList.SetCurSel(iCurWpn);
}

void DlgWeaponEditor::OnCbnSelchangeWeaponTemplate()
{
	
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgWeaponEditor::OnBnClickedWeaponLevel1()
{
	if (m_IgnoreInput)
		return;

	GetDlgItem(BUTTON_WEAPON_LEVEL_1)->EnableWindow(FALSE);
	GetDlgItem(BUTTON_WEAPON_LEVEL_2)->EnableWindow(TRUE);
	GetDlgItem(BUTTON_WEAPON_LEVEL_3)->EnableWindow(TRUE);

	m_SelectedLevel = 0;
	DisplayLevelInfo();
}

void DlgWeaponEditor::OnBnClickedWeaponLevel2()
{
	if (m_IgnoreInput)
		return;

	GetDlgItem(BUTTON_WEAPON_LEVEL_1)->EnableWindow(TRUE);
	GetDlgItem(BUTTON_WEAPON_LEVEL_2)->EnableWindow(FALSE);
	GetDlgItem(BUTTON_WEAPON_LEVEL_3)->EnableWindow(TRUE);

	m_SelectedLevel = 1;
	DisplayLevelInfo();
}

void DlgWeaponEditor::OnBnClickedWeaponLevel3()
{
	if (m_IgnoreInput)
		return;

	GetDlgItem(BUTTON_WEAPON_LEVEL_1)->EnableWindow(TRUE);
	GetDlgItem(BUTTON_WEAPON_LEVEL_2)->EnableWindow(TRUE);
	GetDlgItem(BUTTON_WEAPON_LEVEL_3)->EnableWindow(FALSE);

	m_SelectedLevel = 2;
	DisplayLevelInfo();
}

void DlgWeaponEditor::OnEnChangeWeaponExp()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_WEAPON_EXP)->GetWindowTextA(buffer, sizeof(buffer));
	m_WeaponData[iCurWpn]->level_info[m_SelectedLevel].exp = atoi(buffer);
}

void DlgWeaponEditor::OnCbnSelchangeBulletList()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	m_WeaponData[iCurWpn]->level_info[m_SelectedLevel].bullet_index = ((CComboBox*)GetDlgItem(COMBO_BULLET_LIST))->GetCurSel();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgWeaponEditor::OnEnChangeWeaponDispX()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_WEAPON_DISP_X)->GetWindowTextA(buffer, sizeof(buffer));
	m_WeaponData[iCurWpn]->display_info.right = atoi(buffer) + (m_WeaponData[iCurWpn]->display_info.right - m_WeaponData[iCurWpn]->display_info.left);
	m_WeaponData[iCurWpn]->display_info.left  = atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderWeapon(*cdc);
	ReleaseDC(cdc);
}

void DlgWeaponEditor::OnEnChangeWeaponDispY()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_WEAPON_DISP_Y)->GetWindowTextA(buffer, sizeof(buffer));
	m_WeaponData[iCurWpn]->display_info.bottom = atoi(buffer) + (m_WeaponData[iCurWpn]->display_info.bottom - m_WeaponData[iCurWpn]->display_info.top);
	m_WeaponData[iCurWpn]->display_info.top    = atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderWeapon(*cdc);
	ReleaseDC(cdc);
}

void DlgWeaponEditor::OnEnChangeWeaponDispW()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_WEAPON_DISP_W)->GetWindowTextA(buffer, sizeof(buffer));
	m_WeaponData[iCurWpn]->display_info.right = m_WeaponData[iCurWpn]->display_info.left + atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderWeapon(*cdc);
	ReleaseDC(cdc);
}

void DlgWeaponEditor::OnEnChangeWeaponDispH()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_WEAPON_DISP_H)->GetWindowTextA(buffer, sizeof(buffer));
	m_WeaponData[iCurWpn]->display_info.bottom = m_WeaponData[iCurWpn]->display_info.top + atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderWeapon(*cdc);
	ReleaseDC(cdc);
}

void DlgWeaponEditor::OnEnChangeWeaponOffsetX()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_WEAPON_OFFSET_X)->GetWindowTextA(buffer, sizeof(buffer));
	m_WeaponData[iCurWpn]->display_info.hold_offset_x = atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderWeapon(*cdc);
	ReleaseDC(cdc);
}

void DlgWeaponEditor::OnEnChangeWeaponOffsetY()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_WEAPON_OFFSET_Y)->GetWindowTextA(buffer, sizeof(buffer));
	m_WeaponData[iCurWpn]->display_info.hold_offset_y = atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderWeapon(*cdc);
	ReleaseDC(cdc);
}

void DlgWeaponEditor::OnEnChangeWeaponViewLeft()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_WEAPON_VIEW_LEFT)->GetWindowTextA(buffer, sizeof(buffer));
	m_WeaponData[iCurWpn]->level_info[m_SelectedLevel].view.left = atoi(buffer);
}

void DlgWeaponEditor::OnEnChangeWeaponViewTop()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_WEAPON_VIEW_TOP)->GetWindowTextA(buffer, sizeof(buffer));
	m_WeaponData[iCurWpn]->level_info[m_SelectedLevel].view.top = atoi(buffer);
}

void DlgWeaponEditor::OnEnChangeWeaponViewRight()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_WEAPON_VIEW_RIGHT)->GetWindowTextA(buffer, sizeof(buffer));
	m_WeaponData[iCurWpn]->level_info[m_SelectedLevel].view.right = atoi(buffer);
}

void DlgWeaponEditor::OnEnChangeWeaponViewBottom()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_WEAPON_VIEW_BOTTOM)->GetWindowTextA(buffer, sizeof(buffer));
	m_WeaponData[iCurWpn]->level_info[m_SelectedLevel].view.bottom = atoi(buffer);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgWeaponEditor::OnBnClickedWeaponLoadPlayer()
{
	int i;
	char szFilters[] = "Pixel Character File (*.pxchar)|*.pxchar||";
	char buffer[1024];
	char temp[64];

	// Create an Open dialog
	CFileDialog loadFileDialog(TRUE, "*.pxchar", "Player.pxchar", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	// If we didn't successfully do this, then stop here.
	if (loadFileDialog.DoModal() != IDOK)
		return;

	// Allocate a custom player if we need to
	if (!m_TestPlayer)
		m_TestPlayer = new CustomPlayer();

	// Attempt to load it
	if (!m_TestPlayer->LoadPlayer(loadFileDialog.GetPathName().GetBuffer()))
	{
		sprintf_s(buffer, sizeof(buffer), "Failed to load player \"%s\"!", loadFileDialog.GetFileName().GetBuffer());
		MessageBox(buffer, "ERROR", MB_ICONERROR | MB_OK);
		delete m_TestPlayer;
		m_TestPlayer = NULL;
		return;
	}

	if (!m_TestPlayer_NormalImage.IsNull()) m_TestPlayer_NormalImage.Destroy();
	if (!m_TestPlayer_MaskImage.IsNull()) m_TestPlayer_MaskImage.Destroy();

	sprintf_s(buffer, sizeof(buffer), "%s/Normal.png", m_TestPlayer->mPath); m_TestPlayer_NormalImage.Load(buffer);
	sprintf_s(buffer, sizeof(buffer), "%s/Mask.png", m_TestPlayer->mPath); m_TestPlayer_MaskImage.Load(buffer);

	while (m_TestPlayerAnimList.GetCount()) m_TestPlayerAnimList.DeleteString(0);

	// Populate it
	m_TestPlayerAnimList_Lookup.RemoveAll();
	for (int i = 0; i < PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES; i++)
	{
		if (!(gAnimFrameInfo[i].flags & PAF_SHOW_WEAPON))
			continue;

		m_TestPlayerAnimList.AddString(gAnimFrames[i].Name);
		m_TestPlayerAnimList_Lookup.Add(i);
	}

	GetDlgItem(BUTTON_WEAPON_LOAD_PLAYER)->EnableWindow(FALSE);
	GetDlgItem(CHECK_WEAPON_TEST_ALT_DIR)->EnableWindow(TRUE);
	GetDlgItem(BUTTON_WEAPON_UNLOAD_PLAYER)->EnableWindow(TRUE);
	m_TestPlayerAnimList.EnableWindow(TRUE);
	GetDlgItem(LABEL_PLAYER_PATH)->SetWindowTextA(m_TestPlayer->mPath);

	m_TestPlayerAnimList.SetCurSel(1);
	OnCbnSelchangeWeaponTestAnimations();
}

void DlgWeaponEditor::OnBnClickedWeaponUnloadPlayer()
{
	if (!m_TestPlayer)
		return;

	if (!m_TestPlayer_NormalImage.IsNull()) m_TestPlayer_NormalImage.Destroy();
	if (!m_TestPlayer_MaskImage.IsNull()) m_TestPlayer_MaskImage.Destroy();

	delete m_TestPlayer;
	m_TestPlayer = NULL;

	CComboBox* pList = (CComboBox*)GetDlgItem(LIST_WEAPON_TEST_ANIMATIONS);
	while (pList->GetCount()) pList->DeleteString(0);
	m_TestPlayerAnimList_Lookup.RemoveAll();

	GetDlgItem(BUTTON_WEAPON_LOAD_PLAYER)->EnableWindow(TRUE);
	GetDlgItem(CHECK_WEAPON_TEST_ALT_DIR)->EnableWindow(FALSE);
	GetDlgItem(BUTTON_WEAPON_UNLOAD_PLAYER)->EnableWindow(FALSE);
	pList->EnableWindow(FALSE);
	GetDlgItem(LABEL_PLAYER_PATH)->SetWindowTextA("LOAD A PLAYER -->");
	OnCbnSelchangeWeaponTestAnimations();
	((CButton*)GetDlgItem(CHECK_WEAPON_TEST_ALT_DIR))->SetCheck(FALSE);
}

void DlgWeaponEditor::OnCbnSelchangeWeaponTestAnimations()
{
	CComboBox* pList = (CComboBox*)GetDlgItem(LIST_WEAPON_TEST_ANIMATIONS);

	CDC* cdc;
	cdc = GetDC();
	RenderWeapon(*cdc);
	ReleaseDC(cdc);
}

void DlgWeaponEditor::OnBnClickedWeaponTestAltDir()
{
	m_TestPlayer_AltDir = (((CButton*)GetDlgItem(CHECK_WEAPON_TEST_ALT_DIR))->GetState() & BST_CHECKED);

	CDC* cdc;
	cdc = GetDC();
	RenderWeapon(*cdc);
	ReleaseDC(cdc);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgWeaponEditor::Refresh()
{
	int iOldSel = m_WeaponList.GetCurSel();

	while (m_WeaponList.GetCount()) m_WeaponList.DeleteString(0);

	char Buffer[256];
	for (int i = 0; i < m_WeaponData.GetCount(); i++)
	{
		sprintf_s(Buffer, sizeof(Buffer), "[%.3i] %s", i, m_WeaponData[i]->name);
		m_WeaponList.AddString(Buffer);
	}
}

void DlgWeaponEditor::LoadWeapon()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR)
		return;

	PXMOD_WEAPON_STRUCT* pWeapon = m_WeaponData[iCurWpn];
	char buffer[128];

	// Set level
	m_IgnoreInput = true;
	GetDlgItem(BUTTON_WEAPON_LEVEL_1)->EnableWindow(FALSE);
	GetDlgItem(BUTTON_WEAPON_LEVEL_2)->EnableWindow(TRUE);
	GetDlgItem(BUTTON_WEAPON_LEVEL_3)->EnableWindow(TRUE);
	m_SelectedLevel = 0;

	DisplayLevelInfo();

	m_IgnoreInput = true;

	// Set the display rect
	GetDlgItem(EDIT_WEAPON_NAME)->SetWindowTextA(pWeapon->name);
	sprintf_s(buffer, sizeof(buffer), "%d", pWeapon->display_info.left); GetDlgItem(EDIT_WEAPON_DISP_X)->SetWindowTextA(buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", pWeapon->display_info.top); GetDlgItem(EDIT_WEAPON_DISP_Y)->SetWindowTextA(buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", pWeapon->display_info.right - pWeapon->display_info.left); GetDlgItem(EDIT_WEAPON_DISP_W)->SetWindowTextA(buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", pWeapon->display_info.bottom - pWeapon->display_info.top); GetDlgItem(EDIT_WEAPON_DISP_H)->SetWindowTextA(buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", pWeapon->display_info.hold_offset_x); GetDlgItem(EDIT_WEAPON_OFFSET_X)->SetWindowTextA(buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", pWeapon->display_info.hold_offset_y); GetDlgItem(EDIT_WEAPON_OFFSET_Y)->SetWindowTextA(buffer);

	// Redraw some stuff
	CDC* cdc;
	cdc = GetDC();
	RenderArmsImage(*cdc);
	RenderWeapon(*cdc);
	ReleaseDC(cdc);

	m_IgnoreInput = false;
}

void DlgWeaponEditor::SaveWeapons()
{
	while (exe.mod.mWeapons.count) exe.mod.RemoveWeapon(exe.mod.mWeapons.list[0].name);

	for (int i = 0; i < m_WeaponData.GetCount(); i++)
	{
		int iInsertIndex = exe.mod.AddWeapon(m_WeaponData[i]->name);
		memcpy(&exe.mod.mWeapons.list[iInsertIndex], m_WeaponData[i], sizeof(PXMOD_WEAPON_STRUCT));
	}
}

void DlgWeaponEditor::DisplayLevelInfo()
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR)
		return;

	PXMOD_WEAPON_STRUCT* pWeapon = m_WeaponData[iCurWpn];
	m_IgnoreInput                = true;

	char buffer[128];

	sprintf_s(buffer, sizeof(buffer), "%d", pWeapon->level_info[m_SelectedLevel].exp); GetDlgItem(EDIT_WEAPON_EXP)->SetWindowTextA(buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", pWeapon->level_info[m_SelectedLevel].view.left); GetDlgItem(EDIT_WEAPON_VIEW_LEFT)->SetWindowTextA(buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", pWeapon->level_info[m_SelectedLevel].view.top); GetDlgItem(EDIT_WEAPON_VIEW_TOP)->SetWindowTextA(buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", pWeapon->level_info[m_SelectedLevel].view.right); GetDlgItem(EDIT_WEAPON_VIEW_RIGHT)->SetWindowTextA(buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", pWeapon->level_info[m_SelectedLevel].view.bottom); GetDlgItem(EDIT_WEAPON_VIEW_BOTTOM)->SetWindowTextA(buffer);
	((CComboBox*)GetDlgItem(COMBO_BULLET_LIST))->SetCurSel(pWeapon->level_info[m_SelectedLevel].bullet_index);

	m_IgnoreInput = false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgWeaponEditor::OnPaint()
{
	int x, y;
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		x = (rect.Width() - cxIcon + 1) / 2;
		y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}

	CPaintDC dc(this); // device context for painting
	CDialog::OnPaint();

	// Render everything else
	RenderWeapon(dc);
	RenderArmsImage(dc);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgWeaponEditor::RenderArmsImage(CDC& dc)
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_ArmsImage.IsNull())
		return;

	RECT dstRect;

	GetDlgItem(PICTURE_ARMS)->GetClientRect(&dstRect);
	GetDlgItem(PICTURE_ARMS)->ClientToScreen(&dstRect);
	ScreenToClient(&dstRect);

	RECT srcRect   = { 16 * iCurWpn, 0, (16 * iCurWpn) + 16, 16 };
	dstRect.right  = dstRect.left + 32;
	dstRect.bottom = dstRect.top  + 32;

	m_ArmsImage.StretchBlt(dc.GetSafeHdc(), dstRect, srcRect, 13369376UL);
}

void DlgWeaponEditor::RenderWeapon(CDC& dc)
{
	int iCurWpn = m_WeaponList.GetCurSel();
	if (iCurWpn == LB_ERR || m_Arms.IsNull())
		return;

	PXMOD_WEAPON_STRUCT* pWeapon = m_WeaponData[iCurWpn];
	PlayerAnimFrame iPlrFrame    = (m_TestPlayer ? (PlayerAnimFrame)m_TestPlayerAnimList_Lookup[m_TestPlayerAnimList.GetCurSel()] : (PlayerAnimFrame)0);

	// Get the player's animation frame rect
	RECT rcPlayerAnim = { 0, 0, 0, 0 };
	if (m_TestPlayer)
		m_TestPlayer->GetAnimationRect(iPlrFrame, &rcPlayerAnim);

	CRect EntireRect;
	CRect DstRect;
	RECT rect;

	int max_dim = 0;
	float scale = 0.f;

	// Get the size of the picture box
	GetDlgItem(PICTURE_WEAPON_PREVIEW)->GetWindowRect(&EntireRect);
	GetDlgItem(PICTURE_WEAPON_PREVIEW)->GetWindowRect(&DstRect);

	// Get the rect of the picture box
	GetDlgItem(PICTURE_WEAPON_PREVIEW)->GetClientRect(&rect);
	GetDlgItem(PICTURE_WEAPON_PREVIEW)->ClientToScreen(&rect);
	ScreenToClient(&rect);

	// Draw the background
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(dc.GetSafeHdc(), &rect, brush);

	// Size (player + weapon bounding box)
	CPoint TotalSize(
		MAX(RECT_WIDTH(rcPlayerAnim), RECT_WIDTH(pWeapon->display_info) + pWeapon->display_info.hold_offset_x),
		MAX(RECT_HEIGHT(rcPlayerAnim), RECT_HEIGHT(pWeapon->display_info))
	);

	CPoint CenterPoint(
		rect.left + GET_CENTER_OFFSET(RECT_WIDTH(rect), TotalSize.x),
		rect.top + GET_CENTER_OFFSET(RECT_HEIGHT(rect), TotalSize.y)
	);

	DstRect.left   = CenterPoint.x - (TotalSize.x / 2);
	DstRect.top    = CenterPoint.y - (TotalSize.y / 2);
	DstRect.right  = CenterPoint.x + (TotalSize.x / 2);
	DstRect.bottom = CenterPoint.y + (TotalSize.y / 2);

	CRect SrcRect             = { pWeapon->display_info.left, pWeapon->display_info.top, pWeapon->display_info.right, pWeapon->display_info.bottom };
	bool bFaceRight           = m_TestPlayer && ((((CButton*)GetDlgItem(CHECK_WEAPON_TEST_ALT_DIR))->GetState() & BST_CHECKED) || strstr(gAnimFrames[iPlrFrame].Name, "RIGHT"));
	bool bWalkBob             = m_TestPlayer && (
		!strcmp(gAnimFrames[iPlrFrame].Name, "FRAME_WALK_1") ||
		!strcmp(gAnimFrames[iPlrFrame].Name, "FRAME_WALK_2") ||
		!strcmp(gAnimFrames[iPlrFrame].Name, "FRAME_LOOK_UP_WALK_1") ||
		!strcmp(gAnimFrames[iPlrFrame].Name, "FRAME_LOOK_UP_WALK_2")
		);

	if (bFaceRight)
	{
		int iHeight     = SrcRect.bottom - SrcRect.top;
		SrcRect.top    += iHeight;
		SrcRect.bottom += iHeight;
	}

	{
		max_dim = (RECT_WIDTH(SrcRect) >= RECT_HEIGHT(SrcRect)) ? RECT_WIDTH(SrcRect) : RECT_HEIGHT(SrcRect);
		scale   = ((float)RECT_WIDTH(EntireRect)) / float(max_dim);

		struct
		{
			int x;
			int y;
			int w;
			int h;
		} NewRect;

		if (RECT_WIDTH(SrcRect) >= RECT_HEIGHT(SrcRect))
		{
			NewRect.x = 0;
			NewRect.w = RECT_WIDTH(EntireRect);
			NewRect.h = RECT_HEIGHT(SrcRect) * scale;
			NewRect.y = (RECT_WIDTH(EntireRect) - NewRect.h) / 2;
		}
		else
		{
			NewRect.y = 0;
			NewRect.h = RECT_WIDTH(EntireRect);
			NewRect.w = RECT_WIDTH(SrcRect) * scale;
			NewRect.x = (RECT_WIDTH(EntireRect) - NewRect.w) / 2;
		}

		DstRect.left   = rect.left + GET_CENTER_OFFSET(RECT_WIDTH(EntireRect), NewRect.w);
		DstRect.top    = rect.top + GET_CENTER_OFFSET(RECT_HEIGHT(EntireRect), NewRect.h);
		DstRect.right  = DstRect.left + NewRect.w;
		DstRect.bottom = DstRect.top + NewRect.h;
	}

	if (bWalkBob)
	{
		DstRect.top    -= 2;
		DstRect.bottom -= 2;
	}

	// Draw the weapon
	if (m_TestPlayer_AltDir)
	{
		DstRect.left   += pWeapon->display_info.hold_offset_x;
		DstRect.right  += pWeapon->display_info.hold_offset_x;
		DstRect.top    -= pWeapon->display_info.hold_offset_y;
		DstRect.bottom -= pWeapon->display_info.hold_offset_y;
		m_Arms.AlphaBlend(dc.GetSafeHdc(), DstRect, SrcRect);
		DstRect.left   -= pWeapon->display_info.hold_offset_x;
		DstRect.right  -= pWeapon->display_info.hold_offset_x;
		DstRect.top    += pWeapon->display_info.hold_offset_y;
		DstRect.bottom += pWeapon->display_info.hold_offset_y;
	}
	else
	{
		DstRect.left   -= pWeapon->display_info.hold_offset_x;
		DstRect.right  -= pWeapon->display_info.hold_offset_x;
		DstRect.top    -= pWeapon->display_info.hold_offset_y;
		DstRect.bottom -= pWeapon->display_info.hold_offset_y;
		m_Arms.AlphaBlend(dc.GetSafeHdc(), DstRect, SrcRect);
		DstRect.left   += pWeapon->display_info.hold_offset_x;
		DstRect.right  += pWeapon->display_info.hold_offset_x;
		DstRect.top    += pWeapon->display_info.hold_offset_y;
		DstRect.bottom += pWeapon->display_info.hold_offset_y;
	}

	if (bWalkBob)
	{
		DstRect.top    += 2;
		DstRect.bottom += 2;
	}

	if (!m_TestPlayer || m_TestPlayer_NormalImage.IsNull() || m_TestPlayer_MaskImage.IsNull())
		return;

	// Calculate another rect
	SrcRect = rcPlayerAnim;

	if (bFaceRight)
	{
		int iHeight     = SrcRect.bottom - SrcRect.top;
		SrcRect.top    += iHeight;
		SrcRect.bottom += iHeight;
	}

	EntireRect = DstRect;
	{
		int DN  = (RECT_WIDTH(DstRect) < RECT_HEIGHT(DstRect)) ? RECT_WIDTH(DstRect) : RECT_HEIGHT(DstRect);
		max_dim = (RECT_WIDTH(SrcRect) >= RECT_HEIGHT(SrcRect)) ? RECT_WIDTH(SrcRect) : RECT_HEIGHT(SrcRect);
		scale   = ((float)DN) / float(max_dim);

		struct
		{
			int x;
			int y;
			int w;
			int h;
		} NewRect;

		if (RECT_WIDTH(SrcRect) >= RECT_HEIGHT(SrcRect))
		{
			NewRect.w = DN;
			NewRect.x = 0;
			NewRect.h = RECT_HEIGHT(SrcRect) * scale;
			NewRect.y = (DN - NewRect.h) / 2;
		}
		else
		{
			NewRect.y = 0;
			NewRect.h = DN;
			NewRect.w = RECT_WIDTH(SrcRect) * scale;
			NewRect.x = (DN - NewRect.w) / 2;
		}

		DstRect.left   = DstRect.left + GET_CENTER_OFFSET(RECT_WIDTH(EntireRect), NewRect.w);
		DstRect.top    = DstRect.top + GET_CENTER_OFFSET(RECT_HEIGHT(EntireRect), NewRect.h);
		DstRect.right  = DstRect.left + NewRect.w;
		DstRect.bottom = DstRect.top + NewRect.h;
	}

	// Draw the normal image
	m_TestPlayer_NormalImage.AlphaBlend(dc.GetSafeHdc(), DstRect, SrcRect);

	{
		int SrcRectW = CLAMP(RECT_WIDTH(SrcRect), 0, m_TestPlayer_MaskImage.GetWidth() - SrcRect.left);
		int SrcRectH = CLAMP(RECT_HEIGHT(SrcRect), 0, m_TestPlayer_MaskImage.GetHeight() - SrcRect.top);

		unsigned char iRed   = 127;
		unsigned char iGreen = 0;
		unsigned char iBlue  = 255;

		unsigned char* pOldTable = new unsigned char[(SrcRectW * SrcRectH) * 3];
		unsigned char* pOTblPtr  = pOldTable;

		// Restore color
		for (int i = 0; i < SrcRectW * SrcRectH; i++)
		{
			unsigned char* ColorAddr = (unsigned char*)m_TestPlayer_MaskImage.GetPixelAddress(SrcRect.left + (i % SrcRectW), SrcRect.top + (i / SrcRectW));
			if (ColorAddr[3] != 255)
				continue;

			*pOTblPtr++ = ColorAddr[2];
			*pOTblPtr++ = ColorAddr[1];
			*pOTblPtr++ = ColorAddr[0];

			ColorAddr[0] = unsigned char(CLAMP(float(ColorAddr[0]) * float(float(iBlue) / 255.f), 0.f, 255.f));
			ColorAddr[1] = unsigned char(CLAMP(float(ColorAddr[1]) * float(float(iGreen) / 255.f), 0.f, 255.f));
			ColorAddr[2] = unsigned char(CLAMP(float(ColorAddr[2]) * float(float(iRed) / 255.f), 0.f, 255.f));
		}

		// Put the mask down
		m_TestPlayer_MaskImage.AlphaBlend(dc.GetSafeHdc(), DstRect, SrcRect);

		// Restore color
		pOTblPtr = pOldTable;
		for (int i = 0; i < SrcRectW * SrcRectH; i++)
		{
			unsigned char* ColorAddr = (unsigned char*)m_TestPlayer_MaskImage.GetPixelAddress(SrcRect.left + (i % SrcRectW), SrcRect.top + (i / SrcRectW));
			if (ColorAddr[3] != 255)
				continue;

			ColorAddr[0] = pOTblPtr[0];
			ColorAddr[1] = pOTblPtr[1];
			ColorAddr[2] = pOTblPtr[2];
			pOTblPtr += 3;
		}

		delete[] pOldTable;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------