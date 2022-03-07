#pragma once


// OptionsDlg dialog

class OptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(OptionsDlg)

public:
	int zoom;
	bool wheelZoom;
	bool tilesetWindow;

	OptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~OptionsDlg();

// Dialog Data
	enum { IDD = DIALOG_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnNMCustomdrawAlpha(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCompileOnSave();
	afx_msg void OnBnClickedAltRectEditMode();
	afx_msg void OnBnClickedPlaySound();
	afx_msg void OnBnClickedChangeDefaultMod();
	afx_msg void OnEnChangeColordepth();
};
