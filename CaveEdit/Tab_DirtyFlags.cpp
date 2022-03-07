#include "stdafx.h"

#define NPCHAR_EXTENDED_INFO
#include "../src/NpChar_Shared.h"
#include "Tab_DirtyFlags.h"
#include "afxdialogex.h"
#include "CaveEditor.h"
#include "General.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Tab_DirtyFlags::Tab_DirtyFlags()
{
}

Tab_DirtyFlags::~Tab_DirtyFlags()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Tab_DirtyFlags::PopulateTabPage()
{
	GUI_RECT ourSize(this->GetRect());

	int Height = 45;

	for (int i = 0; i < NPC_NODES_COUNT; i++)
	{
		NPCHAR_NODE_INFO* pNode = &gNpCharNodeInfo[i];

		CreateCheckbox(1248 + i, GUI_RECT(2, 45 + (16 * i), ourSize.w, 16), pNode->node_name);

		Height = (45 + (16 * i) + 16);
	}

	SetWindowPos(NULL, 0, 0, GetRect().w, Height, SWP_NOREPOSITION | SWP_NOZORDER);
}

void Tab_DirtyFlags::SaveTabPage(PXMOD_NPC_STRUCT* ent, int act_no)
{
	unsigned long long int nodes;
	nodes = 0;

	for (int i = 0; i < NPC_NODES_COUNT; i++)
	{
		NPCHAR_NODE_INFO* pNode = &gNpCharNodeInfo[i];
		if (!(((CButton*)GetDlgItem(1248 + i))->GetState() & 0x00001))
			continue;

		nodes += pNode->node;
	}

	if (act_no == -1)
		ent->nodes = nodes;
	else
		ent->acts.list[act_no].nodes = nodes;
}

void Tab_DirtyFlags::DisplayRecord(PXMOD_NPC_STRUCT* ent, int act_no)
{
	for (int i = 0; i < NPC_NODES_COUNT; i++)
	{
		NPCHAR_NODE_INFO* pNode = &gNpCharNodeInfo[i];

		if (act_no == -1)
			CheckDlgButton(1248 + i, ((ent->nodes & pNode->node) ? true : false));
		else
			CheckDlgButton(1248 + i, ((ent->acts.list[act_no].nodes & pNode->node) ? true : false));
	}
}