#pragma once

#include "CaveMapEditDlg.h"
#include "General.h"

class MapTabManager : public CDialog
{
	DECLARE_DYNAMIC(MapTabManager)

public:
	enum { IDD = DIALOG_MAP_TAB_MANAGER };

public: // Variables
	CaveMapEditDlg** m_pMapList;
	unsigned int m_iMapCount;
	CTabCtrl m_TabList;
	HICON m_hIcon;
	CMenu m_pConMenu;
	int m_LastSelectedMap;
	int m_ContextMenuTabIndex;
	bool m_bIgnore;

public:
	MapTabManager(CWnd* pParent = nullptr);   // standard constructor
	virtual ~MapTabManager();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public: // Methods
	virtual BOOL OnInitDialog();
	void RefreshTabList();
	void SetMapTab(int iIndex);
	void LoadMap(int iStageIndex);
	void CloseMap(int iStageIndex);
	void CloseAllMaps();

public: // Callbacks
	void OnMapChangeUpdate();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnRangeCmds(UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelectMapTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCloseTab();
	afx_msg void OnMove(int x, int y);
};
