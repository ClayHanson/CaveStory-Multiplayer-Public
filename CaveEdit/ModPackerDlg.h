#pragma once


// ModPackerDlg dialog

class ModPackerDlg : public CDialog
{
	DECLARE_DYNAMIC(ModPackerDlg)

	CFont m_HelpFont;
public:
	ModPackerDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ModPackerDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = DIALOG_MOD_PACKER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSelectOutDir();
	afx_msg void OnPackMod();
	afx_msg void ImportPackerOptions();
	afx_msg void ExportPackerOptions();
};
