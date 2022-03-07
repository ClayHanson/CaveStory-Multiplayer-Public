#pragma once
#include "afxwin.h"
#include "NPCTableCopyFromDlg.h"
#include "NpcEditorTabBase.h"

#include "Tab_TransmitNodes.h"
#include "Tab_DirtyFlags.h"
#include "Tab_NpcFlags.h"
#include "Tab_NpcInfo.h"

#include <list>

// NPCTableEditorDlg dialog

class NPCTableEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(NPCTableEditorDlg)

protected:
	NPCTableCopyFromDlg copyFromDlg;

public:
	Tab_NpcFlags m_Tab1;
	Tab_DirtyFlags m_Tab2;
	Tab_TransmitNodes m_Tab3;
	Tab_NpcInfo m_Tab4;

public:
	CaveBitMap	m_pNpcImage;
	int old_record;
	int old_act_no;
	bool change;
	NPCTableEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~NPCTableEditorDlg();

// Dialog Data
	enum { IDD = DIALOG_NPC_TABLE_EDITOR };

public:
	CArray<PXMOD_NPC_STRUCT*> m_NpcData;
	CListBox m_NpcList;
	CListBox m_ActList;
	bool m_IgnoreInput;
	bool m_IgnoreTabChange;
	int m_OldTab;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
	void PickTab(int idx);

protected:
	virtual void OnOK();
	virtual void OnCancel();

public:
	afx_msg void OnBnClickedSave();
	afx_msg void OnClose();
	afx_msg void OnPaint();

public: // Graphics and Effects
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
	afx_msg void OnBnClickedFlag18();
	afx_msg void OnBnClickedFlag19();
	afx_msg void OnBnClickedFlag20();
	afx_msg void OnEnChangeDisplayTop();
	afx_msg void OnEnChangeDisplayLeft();
	afx_msg void OnEnChangeDisplayBottom();
	afx_msg void OnEnChangeDisplayRight();
	afx_msg void OnEnChangeHitTop();
	afx_msg void OnEnChangeHitRight();
	afx_msg void OnEnChangeHitLeft();
	afx_msg void OnEnChangeHitBottom();
	afx_msg void OnCbnSelchangeTileset();
	afx_msg void OnCbnSelchangeHurtSound();
	afx_msg void OnCbnSelchangeDeathSound();
	afx_msg void OnCbnSelchangeDeathGraphic();

public: // List management
	afx_msg void OnBnClickedNpcAdd();
	afx_msg void OnBnClickedNpcDelete();
	afx_msg void OnBnClickedCancel();

public: // Helpers
	static void PopulateSurfaceList(std::vector<char*>* pList, int iSurfType);
	static void FreeSurfaceList(std::vector<char*>* pList);
	static bool LoadSurfaceBitmap(CaveBitMap* pCBM, int iSurfType, int iIndex);
	void Refresh();
	void LoadNPC();
	void SaveNPCs();

public: // Rendering
	void RenderNPC(CDC& dc);
	void SetupNodeFlags();

	afx_msg void OnCopyClicked();
	afx_msg void OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeNpcName();
	afx_msg void OnEnChangeNpcSpriteX();
	afx_msg void OnEnChangeNpcSpriteY();
	afx_msg void OnEnChangeNpcSpriteW();
	afx_msg void OnEnChangeNpcSpriteH();
	afx_msg void OnLbnSelchangeNpcs();
	afx_msg void OnBnClickedImportNpcTbl();
	afx_msg void OnCbnSelchangeTestSpritesheetList();
	afx_msg void OnEnChangeHp();
	afx_msg void OnEnChangeAttack();
	afx_msg void OnEnChangeXp();
	afx_msg void OnLbnSelchangeEditorActList();
	afx_msg void OnBnClickedActAdd();
	afx_msg void OnBnClickedActRemove();
	afx_msg void OnEnChangeNpcActName();
	afx_msg void OnEnChangeNpcActNo();
};
