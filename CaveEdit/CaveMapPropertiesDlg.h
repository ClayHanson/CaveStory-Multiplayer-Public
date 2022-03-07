#pragma once
#include "afxwin.h"


// CaveMapPropertiesDlg dialog

class CaveMapPropertiesDlg : public CDialog
{
	DECLARE_DYNAMIC(CaveMapPropertiesDlg)

public:
	CString oldFileName;
	CaveMapPropertiesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CaveMapPropertiesDlg();
	CaveMapINFO* caveInfo;//pointer to Cave Map Editor Dialog CaveMap object

// Dialog Data
	enum { IDD = DIALOG_MAP_PROPERITES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	int width;
	int height;
	bool changes;
	bool redraw;
	CComboBox m_backScrollType;
	CComboBox m_backImage;
	CComboBox m_mapTileset;
	CComboBox m_npcTileset1;
	CComboBox m_npcTileset2;
	CComboBox m_bossID;
	CComboBox m_area;
	CButton m_FocusCenterX;
	CButton m_FocusCenterY;
protected:
	virtual void OnOK();
public:
	afx_msg void OnPaint();
	afx_msg void OnCbnSelchangeMapTileset();
	afx_msg void OnCbnSelchangeMapNpcTileset1();
	afx_msg void OnCbnSelchangeMapNpcTileset2();
	afx_msg void OnCbnSelchangeMapBackground();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeArea();
	afx_msg void OnBnClickedFocusX();
	afx_msg void OnBnClickedFocusY();
};
