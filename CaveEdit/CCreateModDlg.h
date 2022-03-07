#pragma once


// CCreateModDlg dialog

class CCreateModDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateModDlg)

public:
	char ModTitle[256];
	char ModAuthor[256];
	char ModDataPath[32];

public:
	CCreateModDlg();   // standard constructor
	virtual ~CCreateModDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = DIALOG_CREATE_MOD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeMdatapath();
	afx_msg void OnEnChangeMauthor();
	afx_msg void OnEnChangeMtitle();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
