// CaveMapZoneInfo.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "CaveMapZoneInfo.h"
#include "afxdialogex.h"
#include "General.h"
#include "InputBoxDlg.h"
#include "CaveMapEditDlg.h"


// CaveMapZoneInfo dialog

IMPLEMENT_DYNAMIC(CaveMapZoneInfo, CDialog)

CaveMapZoneInfo::CaveMapZoneInfo(CWnd* pParent /*=nullptr*/)
	: CDialog(DIALOG_NETGROUP_INFO, pParent)
{
	mMap         = NULL;
	mFillEnabled = false;
}

CaveMapZoneInfo::~CaveMapZoneInfo()
{
}

void CaveMapZoneInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CaveMapZoneInfo, CDialog)
	ON_CBN_SELCHANGE(CBOX_GROUP_NUM, OnCbnSelchangeGroupNum)
	ON_BN_CLICKED(RADIO_BRUSH_NETGROUP, OnBnClickedBrushNetgroup)
	ON_BN_CLICKED(RADIO_FILL_NETGROUP, OnBnClickedFillNetgroup)
	ON_LBN_SELCHANGE(LIST_ZONE_AREA, OnLbnSelchangeZoneArea)
	ON_BN_CLICKED(BUTTON_ADD_ZONE, OnBnClickedAddZone)
	ON_BN_CLICKED(BUTTON_REMOVE_ZONE, OnBnClickedRemoveZone)
	ON_BN_CLICKED(BUTTON_NAME_ZONE, OnBnClickedNameZone)
END_MESSAGE_MAP()


// CaveMapZoneInfo message handlers

int CaveMapZoneInfo::GetSelectedType()
{
	return m_ListBox->GetCurSel();
}

void CaveMapZoneInfo::SetSelectedType(int iZoneId)
{
	if (iZoneId < 0)
		iZoneId = 0;

	if (iZoneId >= m_ListBox->GetCount())
		iZoneId = m_ListBox->GetCount() - 1;

	m_ListBox->SetCurSel(iZoneId);
}

void CaveMapZoneInfo::Refresh()
{
	int iOldSel = m_ListBox->GetCurSel();

	m_ListBox->ResetContent();
	if (mMap)
	{
		char str[1024];
		for (std::vector<CaveMapZoneDev>::iterator it = mMap->zones.begin(); it != mMap->zones.end(); it++)
		{
			CaveMapZoneDev* pZone = &(*it);

			sprintf(str, "%04d: %s", (pZone->id) + 1, pZone->name);
			m_ListBox->AddString(str);
		}

		// Enable buttons
		m_AddButton->EnableWindow(TRUE);
		m_DelButton->EnableWindow(FALSE);
		m_NameButton->EnableWindow(FALSE);
	}
	else
	{
		((CListBox*)GetDlgItem(LIST_ZONE_AREA))->AddString("-- NO MAP --");

		// Disable all buttons
		m_AddButton->EnableWindow(FALSE);
		m_DelButton->EnableWindow(FALSE);
		m_NameButton->EnableWindow(FALSE);
	}

	if (iOldSel >= m_ListBox->GetCount())
		iOldSel = m_ListBox->GetCount() - 1;

	m_ListBox->SetCurSel(iOldSel);
	OnLbnSelchangeZoneArea();
}

void CaveMapZoneInfo::OnCbnSelchangeGroupNum()
{
	// Do nothing
}

bool CaveMapZoneInfo::IsFillEnabled()
{
	return mFillEnabled;
}

BOOL CaveMapZoneInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ListBox    = ((CListBox*)GetDlgItem(LIST_ZONE_AREA));
	m_AddButton  = ((CButton*)GetDlgItem(BUTTON_ADD_ZONE));
	m_DelButton  = ((CButton*)GetDlgItem(BUTTON_REMOVE_ZONE));
	m_NameButton = ((CButton*)GetDlgItem(BUTTON_NAME_ZONE));
	
	//((CComboBox*)GetDlgItem(CBOX_GROUP_NUM))->Clear();
	//((CComboBox*)GetDlgItem(CBOX_GROUP_NUM))->InsertString(0, "Zone # 1");
	//((CComboBox*)GetDlgItem(CBOX_GROUP_NUM))->InsertString(1, "Zone # 2");
	//((CComboBox*)GetDlgItem(CBOX_GROUP_NUM))->InsertString(2, "Zone # 3");
	//((CComboBox*)GetDlgItem(CBOX_GROUP_NUM))->InsertString(3, "Zone # 4");
	//((CComboBox*)GetDlgItem(CBOX_GROUP_NUM))->InsertString(4, "Zone # 5");
	//((CComboBox*)GetDlgItem(CBOX_GROUP_NUM))->InsertString(5, "Zone # 6");
	//((CComboBox*)GetDlgItem(CBOX_GROUP_NUM))->InsertString(6, "Zone # 7");
	//((CComboBox*)GetDlgItem(CBOX_GROUP_NUM))->InsertString(7, "Zone # 8");
	//((CComboBox*)GetDlgItem(CBOX_GROUP_NUM))->SetCurSel(0);
	Refresh();

	CheckRadioButton(RADIO_BRUSH_NETGROUP, RADIO_FILL_NETGROUP, RADIO_BRUSH_NETGROUP);

	return TRUE;
}

void CaveMapZoneInfo::OnBnClickedBrushNetgroup()
{
	// TODO: Add your control notification handler code here
	mFillEnabled = false;

	CheckRadioButton(RADIO_BRUSH_NETGROUP, RADIO_FILL_NETGROUP, RADIO_BRUSH_NETGROUP);
}


void CaveMapZoneInfo::OnBnClickedFillNetgroup()
{
	// TODO: Add your control notification handler code here
	mFillEnabled = true;

	CheckRadioButton(RADIO_BRUSH_NETGROUP, RADIO_FILL_NETGROUP, RADIO_FILL_NETGROUP);
}


void CaveMapZoneInfo::OnLbnSelchangeZoneArea()
{
	// TODO: Add your control notification handler code here
	m_AddButton->EnableWindow(TRUE);
	m_NameButton->EnableWindow(TRUE);
	if (m_ListBox->GetCurSel() == 0)
		m_DelButton->EnableWindow(FALSE);
	else
		m_DelButton->EnableWindow(TRUE);
}


void CaveMapZoneInfo::OnBnClickedAddZone()
{
	mMap->addMapZone("");
	Refresh();
	m_ListBox->SetCurSel(m_ListBox->GetCurSel() - 1);
	OnLbnSelchangeZoneArea();
}


void CaveMapZoneInfo::OnBnClickedRemoveZone()
{
	int iCount = -1;
	for (std::vector<CaveMapZoneDev>::iterator it = mMap->zones.begin(); it != mMap->zones.end(); it++)
	{
		CaveMapZoneDev* pZone = &(*it);

		if (++iCount == m_ListBox->GetCurSel())
		{
			mMap->removeMapZone(pZone->id);

			if (m_parent_dlg)
				m_parent_dlg->Refresh(true);

			break;
		}
	}

	Refresh();
}

void CaveMapZoneInfo::OnBnClickedNameZone()
{
	CaveMapZoneDev* pZone = &mMap->zones[GetSelectedType()];

	InputBoxDlg::InputBoxResult Result;
	InputBoxDlg::InputBoxConfig Config;
	Config.info_text   = "Please insert a new name for this zone.";
	Config.error_value = pZone->name;
	Config.min_length  = 1;
	Config.max_length  = sizeof(pZone->name);

	if (InputBoxDlg::OpenInputBox(&Result, &Config, pZone->name))
	{
		strcpy(pZone->name, Result.value);
		Result.Reset();
		Refresh();
	}
}
