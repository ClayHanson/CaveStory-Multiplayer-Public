#include "stdafx.h"

#define NPCHAR_EXTENDED_INFO
#include "../src/NpChar_Shared.h"
#include "Tab_TransmitNodes.h"
#include "afxdialogex.h"
#include "CaveEditor.h"
#include "General.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Tab_TransmitNodes::Tab_TransmitNodes()
{
}

Tab_TransmitNodes::~Tab_TransmitNodes()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void Tab_TransmitNodes::PopulateTabPage()
{
	GUI_RECT ourSize(this->GetRect());

	int Height = 45;

	for (int i = 0; i < NPC_TRANSMIT_NODES_COUNT; i++)
	{
		NPCHAR_NODE_INFO* pNode = &gNpCharTransmitNodeInfo[i];
		CreateCheckbox(1400 + i, GUI_RECT(2, 45 + (16 * i), ourSize.w, 16), pNode->node_name);
		Height = (45 + (16 * i) + 16);
	}

	SetWindowPos(NULL, 0, 0, GetRect().w, Height, SWP_NOREPOSITION | SWP_NOZORDER);
}

void Tab_TransmitNodes::SaveTabPage(PXMOD_NPC_STRUCT* ent, int act_no)
{
	unsigned long long int nodes;
	nodes = 0;

	for (int i = 0; i < NPC_TRANSMIT_NODES_COUNT; i++)
	{
		NPCHAR_NODE_INFO* pNode = &gNpCharTransmitNodeInfo[i];
		if (!(((CButton*)GetDlgItem(1400 + i))->GetState() & 0x00001))
			continue;

		nodes += pNode->node;
	}

	if (act_no == -1)
		ent->transmit_nodes = nodes;
	else
		ent->acts.list[act_no].transmit_nodes = nodes;
}

void Tab_TransmitNodes::DisplayRecord(PXMOD_NPC_STRUCT* ent, int act_no)
{
	for (int i = 0; i < NPC_TRANSMIT_NODES_COUNT; i++)
	{
		NPCHAR_NODE_INFO* pNode = &gNpCharTransmitNodeInfo[i];

		if (act_no == -1)
			CheckDlgButton(1400 + i, ((ent->transmit_nodes & pNode->node) ? true : false));
		else
			CheckDlgButton(1400 + i, ((ent->acts.list[act_no].transmit_nodes &pNode->node) ? true : false));
	}
}