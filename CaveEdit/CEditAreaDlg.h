#pragma once

#include "ModConfigResource.h"

// CEditAreaDlg dialog

class CEditAreaDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditAreaDlg)

public:
	CEditAreaDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CEditAreaDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = DIALOG_AREA_EDIT };
#endif

protected:
	CWnd m_AreaName;
	CComboBox m_StageList;
	CButton m_SinglePlayer;
	CButton m_KeepInRelease;

public:
	PXMOD_AREA_STRUCT m_AreaInfo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
