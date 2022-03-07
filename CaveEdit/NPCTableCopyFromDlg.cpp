// NPCTableCopyFromDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "General.h"

#include "NPCTableCopyFromDlg.h"
#include "NPCTableEditorDlg.h"

// NPCTableCopyFromDlg dialog

IMPLEMENT_DYNAMIC(NPCTableCopyFromDlg, CDialog)

NPCTableCopyFromDlg::NPCTableCopyFromDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(DIALOG_NTE_COPYFROM, pParent)
{

}

NPCTableCopyFromDlg::~NPCTableCopyFromDlg()
{
}

void NPCTableCopyFromDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(NPCTableCopyFromDlg, CDialog)
	ON_BN_CLICKED(BUTTON_CANCEL, OnCancel)
	ON_BN_CLICKED(BUTTON_OK, OnOK)
END_MESSAGE_MAP()


// NPCTableCopyFromDlg message handlers
void NPCTableCopyFromDlg::OnOK()
{
	UINT record = GetDlgItemInt(IDC_NPCTOCOPYFROM);

	if (record >= NPC_TYPE_COUNT)
	{
		MessageBox("ERROR", "Invalid NPC ID.");
		return;
	}

	ShowWindow(SW_HIDE);
}

void NPCTableCopyFromDlg::OnCancel() {
	ShowWindow(SW_HIDE);
}