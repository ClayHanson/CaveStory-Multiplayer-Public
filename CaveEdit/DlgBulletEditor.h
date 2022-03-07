#pragma once

#include "ModConfigResource.h"
#include "General.h"

class DlgBulletEditor : public CDialog
{
	DECLARE_DYNAMIC(DlgBulletEditor)

public:
	enum { IDD = DIALOG_GAMEEDIT_BULLET };

protected:
	DECLARE_MESSAGE_MAP()

protected: // Additional resources
	CaveBitMap m_BulletImage;

protected: // Copying stuff

	struct
	{
		bool behavior;
		bool flags;
		bool bounds;
		bool collision;
		bool sprite;
	} m_Copy;

protected: // Variables
	CArray<PXMOD_BULLET_STRUCT*> m_BulletData;
	CListBox m_BulletList;
	bool m_IgnoreInput;
	HICON m_hIcon;

public:
	DlgBulletEditor(CWnd* pParent = nullptr);
	virtual ~DlgBulletEditor();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

public: // List manipulation
	afx_msg void OnLbnSelchangeBullets();
	afx_msg void OnBnClickedBulletAdd();
	afx_msg void OnBnClickedBulletDuplicate();
	afx_msg void OnBnClickedBulletDelete();
	afx_msg void OnBnClickedBulletSave();

public: // Basic configuration
	afx_msg void OnEnChangeBulletName();
	afx_msg void OnEnChangeBulletBoundsW();
	afx_msg void OnEnChangeBulletBoundsH();
	afx_msg void OnEnChangeBulletColW();
	afx_msg void OnEnChangeBulletColH();

public: // Level management
	afx_msg void OnEnChangeBulletDamage();
	afx_msg void OnEnChangeBulletLife();
	afx_msg void OnEnChangeBulletLifeCount();

public: // Flag management
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
	afx_msg void OnBnClickedCheck5();

public: // Copying
	afx_msg void OnBnClickedBulletCopyBehavior();
	afx_msg void OnBnClickedBulletCopyBounds();
	afx_msg void OnBnClickedBulletCopyCollision();
	afx_msg void OnBnClickedBulletCopyFlags();
	afx_msg void OnBnClickedBulletCopySprite();

public: // Helpers
	void Refresh();
	void LoadBullet();
	void SaveBullets();

public: // bob ross
	afx_msg void OnPaint();

public: // Rendering
	void RenderBullet(CDC& dc);
	afx_msg void OnEnChangeBulletDispX();
	afx_msg void OnEnChangeBulletDispY();
	afx_msg void OnEnChangeBulletDispW();
	afx_msg void OnEnChangeBulletDispH();
};
