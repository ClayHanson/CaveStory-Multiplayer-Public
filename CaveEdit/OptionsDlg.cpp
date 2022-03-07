// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "OptionsDlg.h"
#include ".\optionsdlg.h"

#include "General.h"

// OptionsDlg dialog

IMPLEMENT_DYNAMIC(OptionsDlg, CDialog)
OptionsDlg::OptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(OptionsDlg::IDD, pParent)
{
}

OptionsDlg::~OptionsDlg()
{
}

void OptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(OptionsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, SLIDER_ALPHA, OnNMCustomdrawAlpha)
	ON_BN_CLICKED(CHECK_COMPILE_ON_SAVE, &OptionsDlg::OnBnClickedCompileOnSave)
	ON_BN_CLICKED(CHECK_ALT_RECT_EDIT_MODE, &OptionsDlg::OnBnClickedAltRectEditMode)
	ON_BN_CLICKED(CHECK_PLAY_SOUND, &OptionsDlg::OnBnClickedPlaySound)
	ON_BN_CLICKED(BUTTON_CHANGE_DEFAULT_MOD, &OptionsDlg::OnBnClickedChangeDefaultMod)
	ON_EN_CHANGE(EDIT_COLORDEPTH, &OptionsDlg::OnEnChangeColordepth)
END_MESSAGE_MAP()


// OptionsDlg message handlers

void OptionsDlg::OnBnClickedOk()
{
	CS_DEFAULT_SCALE                = GetDlgItemInt(EDIT_SCALE);
	CS_DEFAULT_WHEEL_ZOOM           = IsDlgButtonChecked(RADIO_WHEEL_ZOOM);
	CS_DEFAULT_TILESET_WINDOW       = IsDlgButtonChecked(CHECK_TILESET_WINDOW);
	CS_DEFAULT_TILESET_SCALE        = GetDlgItemInt(EDIT_TILESET_SCALE);
	CS_DEFAULT_TILETYPE_ALPHA       = ((CSliderCtrl*)GetDlgItem(SLIDER_ALPHA))->GetPos();
	CS_DEFUALT_READ_ONLY            = IsDlgButtonChecked(CHECK_READ_ONLY);
	CS_DEFAULT_COMPILE_ON_SAVE      = IsDlgButtonChecked(CHECK_COMPILE_ON_SAVE);
	CS_GRAPHIC_COLOR_DEPTH          = GetDlgItemInt(EDIT_COLORDEPTH);
	CS_DEFAULT_USE_RECT_XYWH        = IsDlgButtonChecked(CHECK_ALT_RECT_EDIT_MODE);
	CS_DEFAULT_PLAY_SOUND           = IsDlgButtonChecked(CHECK_PLAY_SOUND);
	CS_DEFAULT_ME_FOCUS_ON_MAP_LOAD = IsDlgButtonChecked(CHECK_FOCUS_WINDOW_ON_MAPLOAD);
	CS_DEFAULT_ME_MAXIMIZE_ON_INIT  = IsDlgButtonChecked(CHECK_MAXIMIZE_WINDOW_ON_INITIAL_LOAD);
	CS_DEFAULT_ME_SYNC_ACROSS_TABS  = IsDlgButtonChecked(CHECK_SYNC_ACROSS_TABS);
	char buffer[1024];
	::GetCurrentDirectory(1024, buffer);
	::SetCurrentDirectory(progPath);

	SaveConfig_CaveEditor();

	::SetCurrentDirectory(buffer);

	OnOK();
}

BOOL OptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemInt(EDIT_SCALE, CS_DEFAULT_SCALE);

	if (CS_DEFAULT_WHEEL_ZOOM)
		CheckRadioButton(RADIO_WHEEL_ZOOM, RADIO_WHEEL_SCROLL, RADIO_WHEEL_ZOOM);
	else
		CheckRadioButton(RADIO_WHEEL_ZOOM, RADIO_WHEEL_SCROLL, RADIO_WHEEL_SCROLL);

	CheckDlgButton(CHECK_TILESET_WINDOW, CS_DEFAULT_TILESET_WINDOW);

	SetDlgItemInt(EDIT_TILESET_SCALE, CS_DEFAULT_TILESET_SCALE);

	((CSliderCtrl*)GetDlgItem(SLIDER_ALPHA))->SetRange(0, 255);
	((CSliderCtrl*)GetDlgItem(SLIDER_ALPHA))->SetPos(CS_DEFAULT_TILETYPE_ALPHA);

	CheckDlgButton(CHECK_READ_ONLY, CS_DEFUALT_READ_ONLY);
	CheckDlgButton(CHECK_COMPILE_ON_SAVE, CS_DEFAULT_COMPILE_ON_SAVE);
	CheckDlgButton(CHECK_ALT_RECT_EDIT_MODE, CS_DEFAULT_USE_RECT_XYWH);
	CheckDlgButton(CHECK_PLAY_SOUND, CS_DEFAULT_PLAY_SOUND);
	CheckDlgButton(CHECK_FOCUS_WINDOW_ON_MAPLOAD, CS_DEFAULT_ME_FOCUS_ON_MAP_LOAD);
	CheckDlgButton(CHECK_MAXIMIZE_WINDOW_ON_INITIAL_LOAD, CS_DEFAULT_ME_MAXIMIZE_ON_INIT);
	CheckDlgButton(CHECK_SYNC_ACROSS_TABS, CS_DEFAULT_ME_SYNC_ACROSS_TABS);
	SetDlgItemText(TEXT_MOD_NAME, (*CS_DEFAULT_MOD_PATH == 0 ? "N/A" : CS_DEFAULT_MOD_PATH));

	if (*CS_DEFAULT_MOD_PATH != 0)
	{
		SetDlgItemText(BUTTON_CHANGE_DEFAULT_MOD, "X");
	}
	else
	{
		SetDlgItemText(BUTTON_CHANGE_DEFAULT_MOD, "...");
	}

	SetDlgItemInt(EDIT_COLORDEPTH, CS_GRAPHIC_COLOR_DEPTH);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void OptionsDlg::OnNMCustomdrawAlpha(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	SetDlgItemInt(TEXT_ALPHA, ((CSliderCtrl*)GetDlgItem(SLIDER_ALPHA))->GetPos());

	RECT rect;
	GetDlgItem(TEXT_ALPHA_TEST_AREA)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	rect.left++;
	rect.right--;
	rect.top++;
	rect.bottom--;

	CDC* cdc = GetDC();

	GetDlgItem(TEXT_ALPHA_TEST_AREA)->RedrawWindow();
	cdc->TextOut(rect.left, rect.top, "Testing Transparency");

	CDC cdc2;
	CBitmap bmpFinal;

	cdc2.CreateCompatibleDC(cdc);
	bmpFinal.CreateBitmap(rect.right-rect.left, rect.bottom-rect.top, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	CBitmap* pOldBmp = (CBitmap *)(cdc2.SelectObject(&bmpFinal));

	cdc2.FillSolidRect(&rect, 0);

	BLENDFUNCTION blend;
	blend.AlphaFormat = 0;
	blend.BlendFlags = 0;
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = GetDlgItemInt(TEXT_ALPHA);
	cdc->AlphaBlend(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, &cdc2,
		0, 0, rect.right-rect.left, rect.bottom-rect.top, blend);

	ReleaseDC(cdc);
	bmpFinal.DeleteObject();
	cdc2.DeleteDC();

	*pResult = 0;
}


void OptionsDlg::OnBnClickedCompileOnSave()
{
}

void OptionsDlg::OnBnClickedAltRectEditMode()
{
}

void OptionsDlg::OnBnClickedPlaySound()
{
	// TODO: Add your control notification handler code here
}

void OptionsDlg::OnBnClickedChangeDefaultMod()
{
	if (*CS_DEFAULT_MOD_PATH != 0)
	{
		*CS_DEFAULT_MOD_PATH = 0;
		SetDlgItemText(TEXT_MOD_NAME, "N/A");
		SetDlgItemText(BUTTON_CHANGE_DEFAULT_MOD, "...");

		return;
	}

	int i;
	char szFilters[] = "Pixel Mod File (*.pxmod)|*.pxmod||";
	char buffer[1024];
	char temp[64];

	// Create an Open dialog
	CFileDialog loadFileDialog(TRUE, "*.pxmod", "mod.pxmod", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	if (loadFileDialog.DoModal() != IDOK)
		return;

	CString pStr = loadFileDialog.GetPathName();
	pStr.Replace('\\', '/');

	// We only want the directory.
	if (pStr.ReverseFind('/') != -1)
		pStr.Delete(pStr.ReverseFind('/'), pStr.GetLength() - pStr.ReverseFind('/'));

	// Set it!
	strcpy(CS_DEFAULT_MOD_PATH, pStr.GetBuffer());
	SetDlgItemText(TEXT_MOD_NAME, CS_DEFAULT_MOD_PATH);
	SetDlgItemText(BUTTON_CHANGE_DEFAULT_MOD, "X");
}


void OptionsDlg::OnEnChangeColordepth()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
