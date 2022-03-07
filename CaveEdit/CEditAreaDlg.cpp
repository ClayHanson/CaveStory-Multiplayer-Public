// CEditAreaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "CEditAreaDlg.h"
#include "afxdialogex.h"
#include "General.h"


// CEditAreaDlg dialog

IMPLEMENT_DYNAMIC(CEditAreaDlg, CDialog)

CEditAreaDlg::CEditAreaDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(DIALOG_AREA_EDIT, pParent)
{
	memset(&m_AreaInfo, 0, sizeof(m_AreaInfo));
}

CEditAreaDlg::~CEditAreaDlg()
{
}

void CEditAreaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, EDIT_AREA_NAME, m_AreaName);
	DDX_Control(pDX, COMBO_LOBBY_STAGE, m_StageList);
	DDX_Control(pDX, CHECK_SINGLE_PLAYER, m_SinglePlayer);
	DDX_Control(pDX, CHECK_KEEP_IN_RELEASE, m_KeepInRelease);
}

BEGIN_MESSAGE_MAP(CEditAreaDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CEditAreaDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CEditAreaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Clear the list
	while (m_StageList.GetCount()) m_StageList.DeleteString(0);

	// Insert maps
	char buffer[1024];
	int i;

	for (i = 0; i < exe.mod.mStages.count; i++)//load map list
	{
		if (strcmp(exe.mod.mStages.list[i].file, "\\empty") == 0 || *exe.mod.mStages.list[i].file == 0)
			sprintf(buffer, "[%.3i] ···No Map···", i);
		else
			sprintf(buffer, "[%.3i] %s (%s)", i, exe.mod.mStages.list[i].name, exe.mod.mStages.list[i].file);

		m_StageList.AddString(buffer);
	}

	// Set information
	m_AreaName.SetWindowTextA(m_AreaInfo.name);
	m_StageList.SetCurSel(m_AreaInfo.stage_idx);
	m_SinglePlayer.SetCheck(m_AreaInfo.singleplayer_only ? BST_CHECKED : BST_UNCHECKED);
	m_KeepInRelease.SetCheck(m_AreaInfo.keep_on_release ? BST_CHECKED : BST_UNCHECKED);

	return TRUE;
}

void CEditAreaDlg::OnBnClickedOk()
{
	// Populate the area info
	m_AreaName.GetWindowTextA(m_AreaInfo.name, sizeof(m_AreaInfo.name));
	m_AreaInfo.keep_on_release   = (m_KeepInRelease.GetCheck() == BST_CHECKED);
	m_AreaInfo.singleplayer_only = (m_SinglePlayer.GetCheck() == BST_CHECKED);
	m_AreaInfo.stage_idx         = m_StageList.GetCurSel();
	
	CDialog::OnOK();
}
