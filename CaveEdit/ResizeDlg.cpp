// ResizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "ResizeDlg.h"
#include ".\resizedlg.h"


// ResizeDlg dialog

IMPLEMENT_DYNAMIC(ResizeDlg, CDialog)
ResizeDlg::ResizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ResizeDlg::IDD, pParent)
{
	name = "Resize";
	wName = "Width";
	hName = "Height";
	width = -9999;
	height = -9999;
}

ResizeDlg::~ResizeDlg()
{
}

void ResizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ResizeDlg, CDialog)
END_MESSAGE_MAP()


// ResizeDlg message handlers

BOOL ResizeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(name);
	SetDlgItemText(TEXT_WIDTH, wName);
	SetDlgItemText(TEXT_HEIGHT, hName);
	SetDlgItemInt(EDIT_WIDTH, width);
	SetDlgItemInt(EDIT_HEIGHT, height);

	if (height == -9999)//not being used
	{
		GetDlgItem(EDIT_HEIGHT)->ShowWindow(SW_HIDE);
		GetDlgItem(TEXT_HEIGHT)->ShowWindow(SW_HIDE);
	}
	if (!c1Name.IsEmpty())
	{
		SetDlgItemText(CHECK_1, c1Name);
		CheckDlgButton(CHECK_1, check1);
		GetDlgItem(CHECK_1)->ShowWindow(SW_SHOW);
	}
	if (!c2Name.IsEmpty())
	{
		SetDlgItemText(CHECK_2, c2Name);
		CheckDlgButton(CHECK_2, check2);
		GetDlgItem(CHECK_2)->ShowWindow(SW_SHOW);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ResizeDlg::OnOK()
{
	if (width == GetDlgItemInt(EDIT_WIDTH) && height == GetDlgItemInt(EDIT_HEIGHT))
	{
		CDialog::OnCancel();//this way we don't have to check on return
		return;
	}

	width = GetDlgItemInt(EDIT_WIDTH);
	height = GetDlgItemInt(EDIT_HEIGHT);

	check1 = IsDlgButtonChecked(CHECK_1);
	check2 = IsDlgButtonChecked(CHECK_2);

	CDialog::OnOK();
}
