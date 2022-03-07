// WeaponEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"

#include "General.h"

#include "WeaponEditorDlg.h"

#include ".\weaponeditordlg.h"

// WeaponEditorDlg dialog

IMPLEMENT_DYNAMIC(WeaponEditorDlg, CDialog)
WeaponEditorDlg::WeaponEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(WeaponEditorDlg::IDD, pParent)
{
	char buffer[256];
	sprintf(buffer, "%s/ArmsItem.png", exe.mod.mModPath);
	wepImage.Load(buffer);
}

WeaponEditorDlg::~WeaponEditorDlg()
{
}

void WeaponEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(WeaponEditorDlg, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(BUTTON_PREV, OnBnClickedPrev)
	ON_BN_CLICKED(BUTTON_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(BUTTON_OK, OnBnClickedOk)
	ON_EN_CHANGE(EDIT_WEP_NAME, OnEnChangeWepName)
END_MESSAGE_MAP()


// WeaponEditorDlg message handlers
BOOL WeaponEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	RECT rect;

	GetDlgItem(FRAME_WEP_IMAGE)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top + 48;
	rect.right = rect.left + 48;
	GetDlgItem(FRAME_WEP_IMAGE)->MoveWindow(&rect);

	index = 0;

	updateView();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void WeaponEditorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if ((index/3)*16+32 > wepImage.GetWidth())
		return;

	RECT rect;

	GetDlgItem(FRAME_WEP_IMAGE)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	wepImage.StretchBlt(dc.m_hDC, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, (index/3)*16+16, 0, 16, 16);
}

void WeaponEditorDlg::saveData()
{
	/*
	exe.shotInfo[index].damage = GetDlgItemInt(EDIT_WEP_DAMAGE);
	exe.shotInfo[index].penPower = GetDlgItemInt(EDIT_WEP_HITS);
	exe.shotInfo[index].range = GetDlgItemInt(EDIT_WEP_RANGE);
	exe.shotInfo[index].width = GetDlgItemInt(EDIT_WEP_SHOT_WIDTH);
	exe.shotInfo[index].height = GetDlgItemInt(EDIT_WEP_SHOT_HEIGHT);
	exe.shotInfo[index].wallWidth = GetDlgItemInt(EDIT_WEP_WALL_WIDTH);
	exe.shotInfo[index].wallHeight = GetDlgItemInt(EDIT_WEP_WALL_HEIGHT);
	exe.shotInfo[index].leftGraphicOffset = GetDlgItemInt(EDIT_WEP_LEFT_GRPHIC_OFFSET);
	exe.shotInfo[index].rightGraphicOffset = GetDlgItemInt(EDIT_WEP_RIGHT_GRPHIC_OFFSET);
	exe.shotInfo[index].vertGraphicOffset = GetDlgItemInt(EDIT_WEP_VERT_GRPHIC_OFFSET);

	exe.wepEnergy[index] = GetDlgItemInt(EDIT_WEP_EXP);

	exe.shotInfo[index].flag01 = IsDlgButtonChecked(CHECK_FLAG01); 
	exe.shotInfo[index].flag02 = IsDlgButtonChecked(CHECK_FLAG02); 
	exe.shotInfo[index].flag04 = IsDlgButtonChecked(CHECK_FLAG04); 
	exe.shotInfo[index].flag08 = IsDlgButtonChecked(CHECK_FLAG08); 
	exe.shotInfo[index].flag10 = IsDlgButtonChecked(CHECK_FLAG10); 
	exe.shotInfo[index].flag20 = IsDlgButtonChecked(CHECK_FLAG20); 
	exe.shotInfo[index].flag40 = IsDlgButtonChecked(CHECK_FLAG40); 
	exe.shotInfo[index].flag80 = IsDlgButtonChecked(CHECK_FLAG80); 
	*/
}

void WeaponEditorDlg::updateView()
{
	char buffer[100];
	if (index <= 38)
		sprintf(buffer, "Weapon Editor - Weapon ID %i (%i)", index/3+1, index+3);
	else
		sprintf(buffer, "Weapon Editor - Weapon ID %i (%i) *OOB*", index/3+1, index+3);
	SetWindowText(buffer);

	SetDlgItemInt(TEXT_WEP_LEVEL, index%3+1);
	/*
	SetDlgItemInt(EDIT_WEP_DAMAGE, exe.shotInfo[index].damage);
	SetDlgItemInt(EDIT_WEP_HITS, exe.shotInfo[index].penPower);
	SetDlgItemInt(EDIT_WEP_RANGE, exe.shotInfo[index].range);
	SetDlgItemInt(EDIT_WEP_SHOT_WIDTH, exe.shotInfo[index].width);
	SetDlgItemInt(EDIT_WEP_SHOT_HEIGHT, exe.shotInfo[index].height);
	SetDlgItemInt(EDIT_WEP_WALL_WIDTH, exe.shotInfo[index].wallWidth);
	SetDlgItemInt(EDIT_WEP_WALL_HEIGHT, exe.shotInfo[index].wallHeight);
	SetDlgItemInt(EDIT_WEP_LEFT_GRPHIC_OFFSET, exe.shotInfo[index].leftGraphicOffset);
	SetDlgItemInt(EDIT_WEP_RIGHT_GRPHIC_OFFSET, exe.shotInfo[index].rightGraphicOffset);
	SetDlgItemInt(EDIT_WEP_VERT_GRPHIC_OFFSET, exe.shotInfo[index].vertGraphicOffset);

	SetDlgItemInt(EDIT_WEP_EXP, exe.wepEnergy[index]);

	CheckDlgButton(CHECK_FLAG01, exe.shotInfo[index].flag01); 
	CheckDlgButton(CHECK_FLAG02, exe.shotInfo[index].flag02); 
	CheckDlgButton(CHECK_FLAG04, exe.shotInfo[index].flag04); 
	CheckDlgButton(CHECK_FLAG08, exe.shotInfo[index].flag08); 
	CheckDlgButton(CHECK_FLAG10, exe.shotInfo[index].flag10); 
	CheckDlgButton(CHECK_FLAG20, exe.shotInfo[index].flag20); 
	CheckDlgButton(CHECK_FLAG40, exe.shotInfo[index].flag40); 
	CheckDlgButton(CHECK_FLAG80, exe.shotInfo[index].flag80); 
	*/


	RECT rect;

	GetDlgItem(FRAME_WEP_IMAGE)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	if ((index/3)*16+32 > wepImage.GetWidth())
		InvalidateRect(&rect);
	else
	{
		CDC* cdc = GetDC();
		wepImage.StretchBlt(cdc->m_hDC, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, (index/3)*16+16, 0, 16, 16);
		ReleaseDC(cdc);
	}
}

void WeaponEditorDlg::OnBnClickedPrev()
{
	if (index == 0)
		return;

	saveData();
	index--;
	updateView();
}

void WeaponEditorDlg::OnBnClickedNext()
{
	if (index > 63)
		return;

	saveData();
	index++;
	updateView();
}

void WeaponEditorDlg::OnBnClickedOk()
{
	saveData();
	CDialog::OnOK();
}


void WeaponEditorDlg::OnEnChangeWepName()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
