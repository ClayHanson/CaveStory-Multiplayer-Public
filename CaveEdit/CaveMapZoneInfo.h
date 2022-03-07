#pragma once

// CaveMapNetGroupInfo dialog
class CaveMapEditDlg;
class CaveMap;

class CaveMapZoneInfo : public CDialog
{
	DECLARE_DYNAMIC(CaveMapZoneInfo)

public:
	CaveMapEditDlg* m_parent_dlg;

	CaveMapZoneInfo(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CaveMapZoneInfo();

// Dialog Data
	enum { IDD = DIALOG_NETGROUP_INFO };
	int mFillEnabled;
	CaveMap* mMap;

private:
	CListBox* m_ListBox;
	CButton* m_AddButton;
	CButton* m_DelButton;
	CButton* m_NameButton;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int GetSelectedType();
	void SetSelectedType(int iZoneId);
	bool IsFillEnabled();

public:
	void Refresh();
	afx_msg void OnCbnSelchangeGroupNum();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBrushNetgroup();
	afx_msg void OnBnClickedFillNetgroup();
	afx_msg void OnLbnSelchangeZoneArea();
	afx_msg void OnBnClickedAddZone();
	afx_msg void OnBnClickedRemoveZone();
	afx_msg void OnBnClickedNameZone();
};
