#include "stdafx.h"

#define NPCHAR_EXTENDED_INFO
#include "../src/NpChar_Shared.h"
#include "Tab_NpcInfo.h"
#include "afxdialogex.h"
#include "CaveEditor.h"
#include "General.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Tab_NpcInfo::Tab_NpcInfo()
{
}

Tab_NpcInfo::~Tab_NpcInfo()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Tab_NpcInfo::PopulateTabPage()
{

}

void Tab_NpcInfo::SaveTabPage(PXMOD_NPC_STRUCT* ent, int act_no)
{
	GetDlgItem(EDIT_CATEGORY)->GetWindowTextA(ent->category, sizeof(ent->category));
	GetDlgItem(EDIT_OPTION_1)->GetWindowTextA(ent->option1, sizeof(ent->option1));
	GetDlgItem(EDIT_OPTION_2)->GetWindowTextA(ent->option2, sizeof(ent->option2));
	GetDlgItem(EDIT_OPTION_3)->GetWindowTextA(ent->option3, sizeof(ent->option3));
	GetDlgItem(EDIT_SHORTNAME_1)->GetWindowTextA(ent->shortname1, sizeof(ent->shortname1));
	GetDlgItem(EDIT_SHORTNAME_2)->GetWindowTextA(ent->shortname2, sizeof(ent->shortname2));
}

void Tab_NpcInfo::DisplayRecord(PXMOD_NPC_STRUCT* ent, int act_no)
{
	GetDlgItem(EDIT_CATEGORY)->SetWindowTextA(ent->category);
	GetDlgItem(EDIT_OPTION_1)->SetWindowTextA(ent->option1);
	GetDlgItem(EDIT_OPTION_2)->SetWindowTextA(ent->option2);
	GetDlgItem(EDIT_OPTION_3)->SetWindowTextA(ent->option3);
	GetDlgItem(EDIT_SHORTNAME_1)->SetWindowTextA(ent->shortname1);
	GetDlgItem(EDIT_SHORTNAME_2)->SetWindowTextA(ent->shortname2);
}

BEGIN_MESSAGE_MAP(Tab_NpcInfo, NpcEditorTabBase)
END_MESSAGE_MAP()
