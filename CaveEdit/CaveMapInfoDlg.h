#pragma once

#include "General.h"
#include "DlgEntityEditor.h"

class CaveMapEditDlg;//forward declaration for circular referencing

// CaveMapInfoDlg dialog

class CaveMapInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CaveMapInfoDlg)

public:
	CaveMapInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CaveMapInfoDlg();

// Dialog Data
	enum { IDD = DIALOG_ENTITY_INFO };

public:
	Entity* mEntity;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	NPCtable npcTable;
	CaveMapEditDlg* mapEditDialog;
	bool init;
	int index;
	int type;
	void clear();
	void load(Entity* entity, int id);
	afx_msg void OnBnClickedFlagDetails();
	afx_msg void OnBnClickedShowTileset();
	afx_msg void OnEnChangeFlagId();
	afx_msg void OnEnChangeEventId();
	afx_msg void OnEnChangeFlags();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeEntityType();
	afx_msg void OnCbnKillfocusEntityType();
	afx_msg void OnEnKillfocusFlagId();
	afx_msg void OnEnKillfocusEventId();
	afx_msg void OnBnClickedPropertyDetails();
};
