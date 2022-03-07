// CaveMapInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "General.h"
#include "CaveEditor.h"
#include "CaveMapInfoDlg.h"
#include "EntityFlagDetailDlg.h"
#include "CaveMapEditDlg.h"
#include ".\cavemapinfodlg.h"
#include "DlgEntityEditor.h"

//#include "SharedWindows.h"

// CaveMapInfoDlg dialog

IMPLEMENT_DYNAMIC(CaveMapInfoDlg, CDialog)
CaveMapInfoDlg::CaveMapInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CaveMapInfoDlg::IDD, pParent), npcTable()
{
	init = false;
	index = -1;
	mapEditDialog = (CaveMapEditDlg*)pParent;
}

CaveMapInfoDlg::~CaveMapInfoDlg()
{
}

void CaveMapInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CaveMapInfoDlg, CDialog)
	ON_BN_CLICKED(BUTTON_FLAG_DETAILS, OnBnClickedFlagDetails)
	ON_BN_CLICKED(BUTTON_SHOW_TILESET, OnBnClickedShowTileset)
	ON_EN_CHANGE(EDIT_FLAG_ID, OnEnChangeFlagId)
	ON_EN_CHANGE(EDIT_EVENT_ID, OnEnChangeEventId)
	ON_EN_CHANGE(EDIT_FLAGS, OnEnChangeFlags)
	ON_CBN_SELCHANGE(COMBO_ENTITY_TYPE, OnCbnSelchangeEntityType)
	ON_CBN_KILLFOCUS(COMBO_ENTITY_TYPE, OnCbnKillfocusEntityType)
	ON_EN_KILLFOCUS(EDIT_FLAG_ID, OnEnKillfocusFlagId)
	ON_EN_KILLFOCUS(EDIT_EVENT_ID, OnEnKillfocusEventId)
	ON_BN_CLICKED(BUTTON_PROPERTY_DETAILS, OnBnClickedPropertyDetails)
END_MESSAGE_MAP()


// CaveMapInfoDlg message handlers

void CaveMapInfoDlg::OnBnClickedFlagDetails()
{
	char buffer[10];
	EntityFlagDetailDlg temp;
	int flags;

	GetDlgItemText(EDIT_FLAGS, buffer, 9);
	sscanf(buffer, "%x", &flags);
	temp.flags    = flags;
	temp.mEntity  = mEntity;

	if (temp.DoModal() == IDOK)
	{
		flags = temp.flags;
		sprintf(buffer, "%.4X", flags);
		SetDlgItemText(EDIT_FLAGS, buffer);

		((CaveMapEditDlg*)GetParent())->UpdateEntity();
	}
}


void CaveMapInfoDlg::clear()
{
	//*/
	index = -1;
	GetDlgItem(COMBO_ENTITY_TYPE)->EnableWindow(false);
	GetDlgItem(EDIT_FLAG_ID)->EnableWindow(false);
	SetDlgItemText(EDIT_FLAG_ID, "");
	GetDlgItem(EDIT_FLAGS)->EnableWindow(false);
	SetDlgItemText(EDIT_FLAGS, "");
	GetDlgItem(EDIT_EVENT_ID)->EnableWindow(false);
	SetDlgItemText(EDIT_EVENT_ID, "");
	GetDlgItem(BUTTON_FLAG_DETAILS)->EnableWindow(false);
	GetDlgItem(BUTTON_PROPERTY_DETAILS)->EnableWindow(false);

	//*/
	GetParent()->SetDlgItemText(TEXT_INFO, "Entity - ");
}

void CaveMapInfoDlg::load(Entity* entity, int id)
{
	if (id == -1)
	{
		clear();
		return;
	}

	mEntity = entity;
	index = -1;
	char buffer[32];
	GetDlgItem(COMBO_ENTITY_TYPE)->EnableWindow(true);
	GetDlgItem(EDIT_FLAG_ID)->EnableWindow(true);
	GetDlgItem(EDIT_FLAGS)->EnableWindow(true);
	GetDlgItem(EDIT_EVENT_ID)->EnableWindow(true);
	GetDlgItem(BUTTON_FLAG_DETAILS)->EnableWindow(true);
	GetDlgItem(BUTTON_PROPERTY_DETAILS)->EnableWindow(true);

	type = entity->npc;
	sprintf(buffer, "%i - ", type);
	((CComboBox*)GetDlgItem(COMBO_ENTITY_TYPE))->SetCurSel(
		((CComboBox*)GetDlgItem(COMBO_ENTITY_TYPE))->FindString(-1, buffer));

	SetDlgItemInt(EDIT_EVENT_ID, entity->event);
	SetDlgItemInt(EDIT_FLAG_ID, entity->flagID);
	sprintf(buffer, "%.4X", entity->flags);
	SetDlgItemText(EDIT_FLAGS, buffer);

	sprintf(buffer, "Entity - %i", id);
	mapEditDialog->SetDlgItemText(TEXT_INFO, buffer);

	index = id;
}

void CaveMapInfoDlg::OnBnClickedShowTileset()
{
	mapEditDialog->tileWindow.ShowWindow(SW_SHOW);
	mapEditDialog->SetFocus();
}

void CaveMapInfoDlg::OnCbnSelchangeEntityType()
{
	char number[256];
	if (index != -1)
	{
		((CComboBox*)GetDlgItem(COMBO_ENTITY_TYPE))->GetLBText(((CComboBox*)GetDlgItem(COMBO_ENTITY_TYPE))->GetCurSel(), number);
		if (number[0] != '*')//this is a header and not an actual selectable type
		{
			number[3] = '\0';
			type = atoi(number);
			((CaveMapEditDlg*)GetParent())->UpdateEntity();
		}
	}
	if (((CaveMapEditDlg*)GetParent())->IsDlgButtonChecked(CHECK_ENTITY_SPRITES) || ((CaveMapEditDlg*)GetParent())->IsDlgButtonChecked(CHECK_ENTITY_TEXT))
		((CaveMapEditDlg*)GetParent())->Refresh();
}

void CaveMapInfoDlg::OnEnChangeFlagId()
{
//	if (index != -1)
//		((CaveMapEditDlg*)GetParent())->UpdateEntity();
}

void CaveMapInfoDlg::OnEnChangeEventId()
{
//	if (index != -1)
//		((CaveMapEditDlg*)GetParent())->UpdateEntity();
}

void CaveMapInfoDlg::OnEnChangeFlags()
{
//	if (index != -1)
//		((CaveMapEditDlg*)GetParent())->UpdateEntity();
}

BOOL CaveMapInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CaveMapInfoDlg::OnCbnKillfocusEntityType()
{
	if (index != -1)
		((CaveMapEditDlg*)GetParent())->UpdateEntity();

	char buffer[256];
	sprintf(buffer, "%i - ", type);
	((CComboBox*)GetDlgItem(COMBO_ENTITY_TYPE))->SetCurSel(
		((CComboBox*)GetDlgItem(COMBO_ENTITY_TYPE))->FindString(-1, buffer));
}

void CaveMapInfoDlg::OnEnKillfocusFlagId()
{
	if (index != -1)
		((CaveMapEditDlg*)GetParent())->UpdateEntity();
}

void CaveMapInfoDlg::OnEnKillfocusEventId()
{
	if (index != -1)
		((CaveMapEditDlg*)GetParent())->UpdateEntity();
}

void CaveMapInfoDlg::OnBnClickedPropertyDetails()
{
	DlgEntPropertyEditor prop_edit_test;

	for (int i = 0; i < mapEditDialog->caveMap.properties.size(); i++)
	{
		if (mapEditDialog->caveMap.properties[i].ent_idx != index)
			continue;

		prop_edit_test.EntProp_Set(mapEditDialog->caveMap.properties[i].name, mapEditDialog->caveMap.properties[i].value);
	}

	INT_PTR nResponse = prop_edit_test.DoModal();
	if (nResponse != IDOK)
		return;

	// remove the existants
	for (int i = 0; i < mapEditDialog->caveMap.properties.size(); i++)
	{
		if (mapEditDialog->caveMap.properties[i].ent_idx != index)
			continue;

		mapEditDialog->caveMap.properties.erase(mapEditDialog->caveMap.properties.begin() + i);
		--i;
	}

	CaveMapEntProperty prop;
	prop.ent_idx = index;

	// re-add them
	for (int i = 0; i < prop_edit_test.m_PropCount; i++)
	{
		strcpy(prop.name, prop_edit_test.m_PropList[i].prop->name);
		strcpy(prop.value, prop_edit_test.m_PropList[i].prop->value);

		mapEditDialog->caveMap.properties.push_back(prop);
	}

	if (!mapEditDialog->changes)
		mapEditDialog->changes = prop_edit_test.m_Changes;
}
