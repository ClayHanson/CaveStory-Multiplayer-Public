#pragma once

#include "ModConfigResource.h"
#include "../CaveStoryCharacterEditor/CustomPlayer.h"
#include "General.h"

class DlgWeaponEditor : public CDialog
{
	DECLARE_DYNAMIC(DlgWeaponEditor)

public:
	enum { IDD = DIALOG_GAMEEDIT_WEAPONS };

protected:
	DECLARE_MESSAGE_MAP()

protected: // Additional resources
	CustomPlayer* m_TestPlayer;
	CaveBitMap m_TestPlayer_NormalImage;
	CaveBitMap m_TestPlayer_MaskImage;

	CaveBitMap m_ArmsImage;
	CaveBitMap m_Arms;

protected: // Variables
	 CArray<PXMOD_WEAPON_STRUCT*> m_WeaponData;
	 CArray<int> m_TestPlayerAnimList_Lookup;
	 CListBox m_WeaponList;
	 CComboBox m_TestPlayerAnimList;
	 HICON m_hIcon;
	 bool m_IgnoreInput;
	 bool m_TestPlayer_AltDir;
	 int m_SelectedLevel;

public:
	DlgWeaponEditor(CWnd* pParent = nullptr);
	virtual ~DlgWeaponEditor();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

public: // List manipulation
	afx_msg void OnLbnSelchangeWeapons();
	afx_msg void OnBnClickedWeaponAdd();
	afx_msg void OnBnClickedWeaponDuplicate();
	afx_msg void OnBnClickedWeaponDelete();
	afx_msg void OnBnClickedWeaponSave();

public: // Basic configuration
	afx_msg void OnEnChangeWeaponName();
	afx_msg void OnCbnSelchangeWeaponTemplate();

public: // Level management
	afx_msg void OnBnClickedWeaponLevel1();
	afx_msg void OnBnClickedWeaponLevel2();
	afx_msg void OnBnClickedWeaponLevel3();
	afx_msg void OnEnChangeWeaponExp();
	afx_msg void OnCbnSelchangeBulletList();

public: // Display configuration
	afx_msg void OnEnChangeWeaponDispX();
	afx_msg void OnEnChangeWeaponDispY();
	afx_msg void OnEnChangeWeaponDispW();
	afx_msg void OnEnChangeWeaponDispH();
	afx_msg void OnEnChangeWeaponOffsetX();
	afx_msg void OnEnChangeWeaponOffsetY();
	afx_msg void OnEnChangeWeaponViewLeft();
	afx_msg void OnEnChangeWeaponViewTop();
	afx_msg void OnEnChangeWeaponViewRight();
	afx_msg void OnEnChangeWeaponViewBottom();

public: // Player testing
	afx_msg void OnBnClickedWeaponLoadPlayer();
	afx_msg void OnBnClickedWeaponUnloadPlayer();
	afx_msg void OnCbnSelchangeWeaponTestAnimations();
	afx_msg void OnBnClickedWeaponTestAltDir();

public: // Helpers
	void Refresh();
	void LoadWeapon();
	void SaveWeapons();
	void DisplayLevelInfo();
	
public: // bob ross
	afx_msg void OnPaint();

public: // Rendering
	void RenderArmsImage(CDC& dc);
	void RenderWeapon(CDC& dc);
};
