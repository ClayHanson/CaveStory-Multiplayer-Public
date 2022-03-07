// NpcExeEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "NpcExeEditorDlg.h"
#include ".\npcexeeditordlg.h"

#include "General.h"


// NpcExeEditorDlg dialog

IMPLEMENT_DYNAMIC(NpcExeEditorDlg, CDialog)
NpcExeEditorDlg::NpcExeEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(NpcExeEditorDlg::IDD, pParent)
{
}

NpcExeEditorDlg::~NpcExeEditorDlg()
{
}

void NpcExeEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(NpcExeEditorDlg, CDialog)
	ON_BN_CLICKED(BUTTON_PREV, OnBnClickedPrev)
	ON_BN_CLICKED(BUTTON_NEXT, OnBnClickedNext)
	ON_EN_CHANGE(EDIT_NPC_CODE, OnEnChangeNpcCode)
	ON_EN_VSCROLL(EDIT_NPC_CODE, OnEnVscrollNpcCode)
END_MESSAGE_MAP()


// NpcExeEditorDlg message handlers

BOOL NpcExeEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	index = 0;

	return FALSE;
}

RECT NpcExeEditorDlg::findNpcRect(char* buffer)
{
	RECT nothing = { 0, 0, 0, 0 };
	return nothing;
}

void NpcExeEditorDlg::OnBnClickedPrev()
{
}

void NpcExeEditorDlg::OnBnClickedNext()
{
}

void NpcExeEditorDlg::OnEnChangeNpcCode()
{
}


void NpcExeEditorDlg::OnEnVscrollNpcCode()
{
}
