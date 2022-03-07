// EntityFlagDetailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "EntityFlagDetailDlg.h"
#include ".\entityflagdetaildlg.h"


// EntityFlagDetailDlg dialog

IMPLEMENT_DYNAMIC(EntityFlagDetailDlg, CDialog)
EntityFlagDetailDlg::EntityFlagDetailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(EntityFlagDetailDlg::IDD, pParent)
{
	flags = 0;
}

EntityFlagDetailDlg::~EntityFlagDetailDlg()
{
}

void EntityFlagDetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(EntityFlagDetailDlg, CDialog)
	ON_BN_CLICKED(CHECK_FLAG1, OnBnClickedFlag1)
	ON_BN_CLICKED(CHECK_FLAG2, OnBnClickedFlag2)
	ON_BN_CLICKED(CHECK_FLAG3, OnBnClickedFlag3)
	ON_BN_CLICKED(CHECK_FLAG4, OnBnClickedFlag4)
	ON_BN_CLICKED(CHECK_FLAG5, OnBnClickedFlag5)
	ON_BN_CLICKED(CHECK_FLAG6, OnBnClickedFlag6)
	ON_BN_CLICKED(CHECK_FLAG7, OnBnClickedFlag7)
	ON_BN_CLICKED(CHECK_FLAG8, OnBnClickedFlag8)
	ON_BN_CLICKED(CHECK_FLAG9, OnBnClickedFlag9)
	ON_BN_CLICKED(CHECK_FLAG10, OnBnClickedFlag10)
	ON_BN_CLICKED(CHECK_FLAG11, OnBnClickedFlag11)
	ON_BN_CLICKED(CHECK_FLAG12, OnBnClickedFlag12)
	ON_BN_CLICKED(CHECK_FLAG13, OnBnClickedFlag13)
	ON_BN_CLICKED(CHECK_FLAG14, OnBnClickedFlag14)
	ON_BN_CLICKED(CHECK_FLAG15, OnBnClickedFlag15)
	ON_BN_CLICKED(CHECK_FLAG16, OnBnClickedFlag16)
	ON_BN_CLICKED(CHECK_FLAG17, OnBnClickedFlag17)
	ON_EN_UPDATE(EDIT_FLAGS, OnEnUpdateFlag)
END_MESSAGE_MAP()

void EntityFlagDetailDlg::CheckEdit()
{
	char buffer[10];
	GetDlgItemText(EDIT_FLAGS, buffer, 9);
	sscanf(buffer, "%x", (unsigned int*)&flags);

	CheckDlgButton(CHECK_FLAG1, flags & 0x00001);
	CheckDlgButton(CHECK_FLAG2, flags & 0x00002);
	CheckDlgButton(CHECK_FLAG3, flags & 0x00004);
	CheckDlgButton(CHECK_FLAG4, flags & 0x00008);
	CheckDlgButton(CHECK_FLAG5, flags & 0x00010);
	CheckDlgButton(CHECK_FLAG6, flags & 0x00020);
	CheckDlgButton(CHECK_FLAG7, flags & 0x00040);
	CheckDlgButton(CHECK_FLAG8, flags & 0x00080);
	CheckDlgButton(CHECK_FLAG9, flags & 0x00100);
	CheckDlgButton(CHECK_FLAG10, flags & 0x00200);
	CheckDlgButton(CHECK_FLAG11, flags & 0x00400);
	CheckDlgButton(CHECK_FLAG12, flags & 0x00800);
	CheckDlgButton(CHECK_FLAG13, flags & 0x01000);
	CheckDlgButton(CHECK_FLAG14, flags & 0x02000);
	CheckDlgButton(CHECK_FLAG15, flags & 0x04000);
	CheckDlgButton(CHECK_FLAG16, flags & 0x08000);
	CheckDlgButton(CHECK_FLAG17, flags & 0x200000);

}

void EntityFlagDetailDlg::CheckBoxes()
{
	flags = 0;
	if (IsDlgButtonChecked(CHECK_FLAG1))
		flags += 0x00001;
	if (IsDlgButtonChecked(CHECK_FLAG2))
		flags += 0x00002;
	if (IsDlgButtonChecked(CHECK_FLAG3))
		flags += 0x00004;
	if (IsDlgButtonChecked(CHECK_FLAG4))
		flags += 0x00008;
	if (IsDlgButtonChecked(CHECK_FLAG5))
		flags += 0x00010;
	if (IsDlgButtonChecked(CHECK_FLAG6))
		flags += 0x00020;
	if (IsDlgButtonChecked(CHECK_FLAG7))
		flags += 0x00040;
	if (IsDlgButtonChecked(CHECK_FLAG8))
		flags += 0x00080;
	if (IsDlgButtonChecked(CHECK_FLAG9))
		flags += 0x00100;
	if (IsDlgButtonChecked(CHECK_FLAG10))
		flags += 0x00200;
	if (IsDlgButtonChecked(CHECK_FLAG11))
		flags += 0x00400;
	if (IsDlgButtonChecked(CHECK_FLAG12))
		flags += 0x00800;
	if (IsDlgButtonChecked(CHECK_FLAG13))
		flags += 0x01000;
	if (IsDlgButtonChecked(CHECK_FLAG14))
		flags += 0x02000;
	if (IsDlgButtonChecked(CHECK_FLAG15))
		flags += 0x04000;
	if (IsDlgButtonChecked(CHECK_FLAG16))
		flags += 0x08000;
	if (IsDlgButtonChecked(CHECK_FLAG17))
		flags += 0x200000;

	char buffer[10];
	sprintf(buffer, "%.5X", flags);
	SetDlgItemText(EDIT_FLAGS, buffer);
}

// EntityFlagDetailDlg message handlers
void EntityFlagDetailDlg::OnBnClickedFlag1()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag2()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag3()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag4()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag5()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag6()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag7()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag8()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag9()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag10()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag11()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag12()
{
	if (IsDlgButtonChecked(CHECK_FLAG12))
		CheckDlgButton(CHECK_FLAG15, false);
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag13()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag14()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag15()
{
	if (IsDlgButtonChecked(CHECK_FLAG15))
		CheckDlgButton(CHECK_FLAG12, false);
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag16()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnBnClickedFlag17()
{
	CheckBoxes();
}

void EntityFlagDetailDlg::OnEnUpdateFlag()
{
	CheckEdit();
}

BOOL EntityFlagDetailDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	NPCtable nTbl;
	
	char buffer[1024];
	sprintf(buffer, "%.5X", flags);
	SetDlgItemText(EDIT_FLAGS, buffer);
	CheckEdit();

	if (nTbl.extra[mEntity->npc].option1.GetLength() != 0)
	{
		sprintf(buffer, "0x00100 - %s", nTbl.extra[mEntity->npc].option1.GetString());
		SetDlgItemText(CHECK_FLAG9, buffer);
	}

	if (nTbl.extra[mEntity->npc].option2.GetLength() != 0)
	{
		sprintf(buffer, "0x01000 - %s", nTbl.extra[mEntity->npc].option2.GetString());
		SetDlgItemText(CHECK_FLAG13, buffer);
	}
	
	if (nTbl.extra[mEntity->npc].option3.GetLength() != 0)
	{
		sprintf(buffer, "0x20000 - %s", nTbl.extra[mEntity->npc].option3.GetString());
		SetDlgItemText(CHECK_FLAG17, buffer);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void EntityFlagDetailDlg::OnOK()
{
//	flags = GetDlgItemInt(EDIT_FLAGS);
	char buffer[10];
	GetDlgItemText(EDIT_FLAGS, buffer, 9);
	sscanf(buffer, "%x", (unsigned int*)&flags);

	CDialog::OnOK();
}
