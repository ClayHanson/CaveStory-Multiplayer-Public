#pragma once

// WeaponEditorDlg dialog

class WeaponEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(WeaponEditorDlg)

public:
	enum { IDD = DIALOG_WEAPON_EDITOR };

protected:
	DECLARE_MESSAGE_MAP()
	CaveBitMap wepImage;

public: // Public config
	int index;

public:
	WeaponEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~WeaponEditorDlg();

protected:
	void updateView();
	void saveData();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedPrev();
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeWepName();


};
