// ReorderMapsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "ReorderMapsDlg.h"

#include "General.h"
#include ".\reordermapsdlg.h"

// ReorderMapsDlg dialog

IMPLEMENT_DYNAMIC(ReorderMapsDlg, CDialog)
ReorderMapsDlg::ReorderMapsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ReorderMapsDlg::IDD, pParent)
{

}

ReorderMapsDlg::~ReorderMapsDlg()
{
}

void ReorderMapsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_MAPS, m_mapList);
}


BEGIN_MESSAGE_MAP(ReorderMapsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// ReorderMapsDlg message handlers

BOOL ReorderMapsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	char buffer[256];
	int i;

	for (i = 0; i < exe.mod.mStages.count; i++)//load map list
	{
		if (strcmp(exe.mod.mStages.list[i].file, "\\empty") == 0)
			sprintf(buffer, "[%.3i] ·No Map·", i);
		else
			sprintf(buffer, "[%.3i] %s (%s)", i, exe.mod.mStages.list[i].name, exe.mod.mStages.list[i].file);
		m_mapList.AddString(buffer);
	};


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void ReorderMapsDlg::OnBnClickedOk()
{
	PXMOD_STAGE_STRUCT* NewStageBuffer = new PXMOD_STAGE_STRUCT[exe.mod.mStages.count];
	char buffer[256];
	int i, j;

	for (i = 0; i < exe.mod.mStages.count; i++)
	{
		m_mapList.GetText(i, buffer);
		sscanf(buffer, "[%d]", &j);

		memcpy(&NewStageBuffer[i], &exe.mod.mStages.list[j], sizeof(PXMOD_STAGE_STRUCT));
	}

	memcpy(exe.mod.mStages.list, NewStageBuffer, sizeof(PXMOD_STAGE_STRUCT) * exe.mod.mStages.count);

	delete[] NewStageBuffer;

	OnOK();
}
