#pragma once
#include "afxcmn.h"


// ReorderMapsDlg dialog

class ReorderMapsDlg : public CDialog
{
	DECLARE_DYNAMIC(ReorderMapsDlg)

public:
	ReorderMapsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ReorderMapsDlg();

// Dialog Data
	enum { IDD = DIALOG_MAP_REORDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CDragListBox m_mapList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
