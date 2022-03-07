#include "stdafx.h"

#define NPCHAR_EXTENDED_INFO
#include "../src/NpChar_Shared.h"
#include "Tab_NpcFlags.h"
#include "afxdialogex.h"
#include "CaveEditor.h"
#include "General.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Tab_NpcFlags::Tab_NpcFlags()
{
}

Tab_NpcFlags::~Tab_NpcFlags()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Tab_NpcFlags::PopulateTabPage()
{

}

void Tab_NpcFlags::SaveTabPage(PXMOD_NPC_STRUCT* ent, int act_no)
{
	unsigned int bits;
	bits = 0;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG1))->GetState() & 0x00001) << 0;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG2))->GetState() & 0x00001) << 1;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG3))->GetState() & 0x00001) << 2;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG4))->GetState() & 0x00001) << 3;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG5))->GetState() & 0x00001) << 4;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG6))->GetState() & 0x00001) << 5;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG7))->GetState() & 0x00001) << 6;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG8))->GetState() & 0x00001) << 7;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG9))->GetState() & 0x00001) << 8;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG10))->GetState() & 0x00001) << 9;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG11))->GetState() & 0x00001) << 10;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG12))->GetState() & 0x00001) << 11;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG13))->GetState() & 0x00001) << 12;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG14))->GetState() & 0x00001) << 13;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG15))->GetState() & 0x00001) << 14;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG16))->GetState() & 0x00001) << 15;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG17))->GetState() & 0x00001) << 16;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG18))->GetState() & 0x00001) << 17;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG19))->GetState() & 0x00001) << 18;
	bits += (((CButton*)GetDlgItem(CHECK_FLAG20))->GetState() & 0x00001) << 19;

	ent->bits = bits;// = GetDlgItemInt(EDIT_, NULL, FALSE);
}

void Tab_NpcFlags::DisplayRecord(PXMOD_NPC_STRUCT* ent, int act_no)
{
	CheckDlgButton(CHECK_FLAG1, 0x00001 & ent->bits);
	CheckDlgButton(CHECK_FLAG2, 0x00002 & ent->bits);
	CheckDlgButton(CHECK_FLAG3, 0x00004 & ent->bits);
	CheckDlgButton(CHECK_FLAG4, 0x00008 & ent->bits);
	CheckDlgButton(CHECK_FLAG5, 0x00010 & ent->bits);
	CheckDlgButton(CHECK_FLAG6, 0x00020 & ent->bits);
	CheckDlgButton(CHECK_FLAG7, 0x00040 & ent->bits);
	CheckDlgButton(CHECK_FLAG8, 0x00080 & ent->bits);
	CheckDlgButton(CHECK_FLAG9, 0x00100 & ent->bits);
	CheckDlgButton(CHECK_FLAG10, 0x00200 & ent->bits);
	CheckDlgButton(CHECK_FLAG11, 0x00400 & ent->bits);
	CheckDlgButton(CHECK_FLAG12, 0x00800 & ent->bits);
	CheckDlgButton(CHECK_FLAG13, 0x01000 & ent->bits);
	CheckDlgButton(CHECK_FLAG14, 0x02000 & ent->bits);
	CheckDlgButton(CHECK_FLAG15, ent->useNewColMethod);
	CheckDlgButton(CHECK_FLAG16, 0x08000 & ent->bits);
	CheckDlgButton(CHECK_FLAG17, 0x10000 & ent->bits);
	CheckDlgButton(CHECK_FLAG18, 0x20000 & ent->bits);
	CheckDlgButton(CHECK_FLAG19, 0x40000 & ent->bits);
	CheckDlgButton(CHECK_FLAG20, 0x80000 & ent->bits);
}