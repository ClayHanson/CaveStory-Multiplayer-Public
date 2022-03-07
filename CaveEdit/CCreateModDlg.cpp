// CCreateModDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "CCreateModDlg.h"
#include "afxdialogex.h"


// CCreateModDlg dialog

IMPLEMENT_DYNAMIC(CCreateModDlg, CDialog)

CCreateModDlg::CCreateModDlg()
	: CDialog(DIALOG_CREATE_MOD)
{
	strcpy(ModTitle, "My Mod");
	strcpy(ModAuthor, "Me");
	strcpy(ModDataPath, "mymod");
}

CCreateModDlg::~CCreateModDlg()
{
}

void CCreateModDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CCreateModDlg::OnInitDialog()
{
	GetDlgItem(INPUT_MTITLE)->SetWindowTextA(ModTitle);
	GetDlgItem(INPUT_MAUTHOR)->SetWindowTextA(ModAuthor);
	GetDlgItem(INPUT_MDATAPATH)->SetWindowTextA(ModDataPath);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CCreateModDlg, CDialog)
	ON_EN_CHANGE(INPUT_MDATAPATH, &CCreateModDlg::OnEnChangeMdatapath)
	ON_EN_CHANGE(INPUT_MAUTHOR, &CCreateModDlg::OnEnChangeMauthor)
	ON_EN_CHANGE(INPUT_MTITLE, &CCreateModDlg::OnEnChangeMtitle)
	ON_BN_CLICKED(IDOK, &CCreateModDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CCreateModDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

void CCreateModDlg::OnEnChangeMdatapath()
{
}

void CCreateModDlg::OnEnChangeMauthor()
{
}

void CCreateModDlg::OnEnChangeMtitle()
{
}

void CCreateModDlg::OnBnClickedOk()
{
	char Buffer[1024];
	char Buffer2[1024];
	int Valid = 0;


	// Validate title
	{
		GetDlgItem(INPUT_MTITLE)->GetWindowTextA(Buffer, 255);

		char* Ptr2 = Buffer2;

		// Get the valid string
		for (char* ptr = Buffer; *ptr != 0; ptr++)
		{
			if (*ptr < ' ' || *ptr >= 127)
				continue;

			*Ptr2++ = *ptr;
		}

		// Null terminate it
		*Ptr2++ = 0;

		if (*Buffer2 == 0)
		{
			MessageBox("Please input something for the mod title!", "ERROR", 0);
			return;
		}
		else
			strcpy(ModTitle, Buffer2);
	}



	// Validate title
	{
		GetDlgItem(INPUT_MAUTHOR)->GetWindowTextA(Buffer, 255);

		char* Ptr2 = Buffer2;

		// Get the valid string
		for (char* ptr = Buffer; *ptr != 0; ptr++)
		{
			if (*ptr < ' ' || *ptr >= 127)
				continue;

			*Ptr2++ = *ptr;
		}

		// Null terminate it
		*Ptr2++ = 0;

		if (*Buffer2 == 0)
		{
			MessageBox("Please input something for the mod author!", "ERROR", 0);
			return;
		}
		else
			strcpy(ModAuthor, Buffer2);
	}

	{
		GetDlgItem(INPUT_MDATAPATH)->GetWindowTextA(Buffer, 32);

		char* Ptr2 = Buffer2;

		// Get the valid string
		for (char* ptr = Buffer; *ptr != 0; ptr++)
		{
			if ((*ptr < '0' || *ptr > '9') && (*ptr < 'a' || *ptr > 'z') && *ptr != '_')
				continue;

			*Ptr2++ = *ptr;
		}

		// Null terminate it
		*Ptr2++ = 0;

		if (*Buffer2 == 0)
		{
			MessageBox("Mod data path is invalid. It can only include a-z, 0-9 and '_'.", "ERROR", 0);
			return;
		}
		else
			strcpy(ModDataPath, Buffer2);
	}
	
	CDialog::OnOK();
}

void CCreateModDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}
