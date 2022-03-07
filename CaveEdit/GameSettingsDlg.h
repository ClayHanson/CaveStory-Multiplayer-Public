#pragma once


// GameSettingsDlg dialog

class GameSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(GameSettingsDlg)

public:
	GameSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~GameSettingsDlg();

	CFont m_CourierFont;
	CString title;
	CString author;
	int version_0;
	int version_1;
	int version_2;
	int version_3;

	int titleX;
	int titleY;
	int titleMap;
	int titleMapEvent;

	int startX;
	int startY;
	int startMapEvent;
	int startMap;

	struct
	{
		bool enabled;
		unsigned short surface_id;
		CRect sprite_rect;
	} collectables;

	struct
	{
		bool allow_carrying_players;
		bool allow_agility;
		bool allow_fishing;
	} gameplay;

	struct
	{
		bool draw_hp_bar;
		bool draw_hp;
		bool draw_weapons;
		bool draw_ammo;
		bool draw_exp_bar;
		bool draw_level;
	} hud;

	struct
	{
		int title;
		int netmenu;
	} music;

	int startCurHP;
	int startMaxHP;
	union
	{
		unsigned char startFlags;
		struct
		{
			unsigned char startFlag01:1;
			unsigned char startFlag02:1;
			unsigned char startFlag04:1;
			unsigned char startFlag08:1;
			unsigned char startFlag10:1;
			unsigned char startFlag20:1;
			unsigned char startFlag40:1;
			unsigned char startFlag80:1;
		};
	};
	int startFacing;

// Dialog Data
	enum { IDD = DIALOG_GAME_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnUpdatePbmExt();
	afx_msg void OnEnUpdateSaveFileName();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnUpdateCPixel();
	afx_msg void OnBnClickedNoCPixel();
	afx_msg void OnEnChangeVersion1();
	afx_msg void OnEnChangeVersion2();
	afx_msg void OnEnChangeVersion3();
	afx_msg void OnEnChangeVersion4();

public: // Collectables
	afx_msg void OnBnClickedAllowCollectables();
	afx_msg void OnCbnSelchangeCollectableSurf();
	afx_msg void OnEnChangeCollectableX();
	afx_msg void OnEnChangeCollectableY();
	afx_msg void OnEnChangeCollectableW();
	afx_msg void OnEnChangeCollectableH();
	void RenderCollectable();
	afx_msg void OnBnClickedDrawHpbar();
	afx_msg void OnBnClickedDrawExpBar();

	afx_msg void KillFocus_CoreDrown();
	afx_msg void GainFocus_CoreDrown();

	afx_msg void KillFocus_EveryoneDied();
	afx_msg void GainFocus_EveryoneDied();

	afx_msg void KillFocus_Drown();
	afx_msg void GainFocus_Drown();
	
	afx_msg void KillFocus_Death();
	afx_msg void GainFocus_Death();
	
	afx_msg void KillFocus_FlagCoreDrown();
	afx_msg void GainFocus_FlagCoreDrown();
	afx_msg void OnCbnSelchangeMusicTitle();
	afx_msg void OnCbnSelchangeMusicNetmenu();
};
