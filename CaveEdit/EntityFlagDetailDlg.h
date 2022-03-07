#pragma once

#include "General.h"


// EntityFlagDetailDlg dialog

class EntityFlagDetailDlg : public CDialog
{
	DECLARE_DYNAMIC(EntityFlagDetailDlg)

public:
	unsigned int flags;
	EntityFlagDetailDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~EntityFlagDetailDlg();

// Dialog Data
	enum { IDD = DIALOG_FLAG_DETAIL };

public:
	Entity* mEntity;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void CheckBoxes();
	void CheckEdit();
	afx_msg void OnBnClickedFlag1();
	afx_msg void OnBnClickedFlag2();
	afx_msg void OnBnClickedFlag3();
	afx_msg void OnBnClickedFlag4();
	afx_msg void OnBnClickedFlag5();
	afx_msg void OnBnClickedFlag6();
	afx_msg void OnBnClickedFlag7();
	afx_msg void OnBnClickedFlag8();
	afx_msg void OnBnClickedFlag9();
	afx_msg void OnBnClickedFlag10();
	afx_msg void OnBnClickedFlag11();
	afx_msg void OnBnClickedFlag12();
	afx_msg void OnBnClickedFlag13();
	afx_msg void OnBnClickedFlag14();
	afx_msg void OnBnClickedFlag15();
	afx_msg void OnBnClickedFlag16();
	afx_msg void OnBnClickedFlag17();
	afx_msg void OnEnUpdateFlag();
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
