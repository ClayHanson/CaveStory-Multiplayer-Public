// GameSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "GameSettingsDlg.h"
#include ".\gamesettingsdlg.h"

#include "General.h"

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define CLAMP(X, AMIN, AMAX) ((X) > (AMAX) ? (AMAX) : ((X) < (AMIN) ? (AMIN) : (X)))
#define GET_CENTER_OFFSET(w1, w2) ((MAX((w1), (w2)) / 2) - (MIN((w1), (w2)) / 2))
#define RECT_WIDTH(ARECT) (ARECT.right - ARECT.left)
#define RECT_HEIGHT(ARECT) (ARECT.bottom - ARECT.top)

// GameSettingsDlg dialog

IMPLEMENT_DYNAMIC(GameSettingsDlg, CDialog)
GameSettingsDlg::GameSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(GameSettingsDlg::IDD, pParent)
{
}

GameSettingsDlg::~GameSettingsDlg()
{
	m_CourierFont.DeleteObject();
}

void GameSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(GameSettingsDlg, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_EN_UPDATE(EDIT_PBM_EXT, OnEnUpdatePbmExt)
	ON_EN_UPDATE(EDIT_SAVE_FILE_NAME, OnEnUpdateSaveFileName)
	ON_EN_UPDATE(EDIT_C_PIXEL, OnEnUpdateCPixel)
	ON_BN_CLICKED(CHECK_NO_C_PIXEL, OnBnClickedNoCPixel)
	ON_EN_CHANGE(EDIT_VERSION_1, OnEnChangeVersion1)
	ON_EN_CHANGE(EDIT_VERSION_2, OnEnChangeVersion2)
	ON_EN_CHANGE(EDIT_VERSION_3, OnEnChangeVersion3)
	ON_EN_CHANGE(EDIT_VERSION_4, OnEnChangeVersion4)
	ON_BN_CLICKED(CHECK_ALLOW_COLLECTABLES, OnBnClickedAllowCollectables)
	ON_CBN_SELCHANGE(LIST_COLLECTABLE_SURF, OnCbnSelchangeCollectableSurf)
	ON_EN_CHANGE(EDIT_COLLECTABLE_X, OnEnChangeCollectableX)
	ON_EN_CHANGE(EDIT_COLLECTABLE_Y, OnEnChangeCollectableY)
	ON_EN_CHANGE(EDIT_COLLECTABLE_W, OnEnChangeCollectableW)
	ON_EN_CHANGE(EDIT_COLLECTABLE_H, OnEnChangeCollectableH)
	ON_BN_CLICKED(CHECK_DRAW_HPBAR, OnBnClickedDrawHpbar)
	ON_BN_CLICKED(CHECK_DRAW_EXP_BAR, OnBnClickedDrawExpBar)

	ON_EN_KILLFOCUS(EDIT_EVENT_COREDROWN, KillFocus_CoreDrown)
	ON_EN_SETFOCUS(EDIT_EVENT_COREDROWN, GainFocus_CoreDrown)

	ON_EN_KILLFOCUS(EDIT_EVENT_EVERYONE_DIED, KillFocus_EveryoneDied)
	ON_EN_SETFOCUS(EDIT_EVENT_EVERYONE_DIED, GainFocus_EveryoneDied)

	ON_EN_KILLFOCUS(EDIT_EVENT_DROWN, KillFocus_Drown)
	ON_EN_SETFOCUS(EDIT_EVENT_DROWN, GainFocus_Drown)

	ON_EN_KILLFOCUS(EDIT_EVENT_DEATH, KillFocus_Death)
	ON_EN_SETFOCUS(EDIT_EVENT_DEATH, GainFocus_Death)

	ON_EN_KILLFOCUS(EDIT_FLAG_CORE_DROWN, KillFocus_FlagCoreDrown)
	ON_EN_SETFOCUS(EDIT_FLAG_CORE_DROWN, GainFocus_FlagCoreDrown)
	ON_CBN_SELCHANGE(COMBO_MUSIC_TITLE, OnCbnSelchangeMusicTitle)
	ON_CBN_SELCHANGE(COMBO_MUSIC_NETMENU, OnCbnSelchangeMusicNetmenu)
END_MESSAGE_MAP()


// GameSettingsDlg message handlers

void GameSettingsDlg::OnPaint()
{
	CDialog::OnPaint();
	RenderCollectable();
}

void GameSettingsDlg::OnBnClickedOk()
{
	bool changes = false;
	CString temp;

	GetDlgItemText(EDIT_TITLE, temp);
	if (temp != title) changes = true;
	GetDlgItemText(EDIT_AUTHOR, temp);
	if (temp != author) changes = true;
	GetDlgItemText(EDIT_VERSION_1, temp);
	if (atoi(temp.GetBuffer()) != version_0) changes = true;
	GetDlgItemText(EDIT_VERSION_2, temp);
	if (atoi(temp.GetBuffer()) != version_1) changes = true;
	GetDlgItemText(EDIT_VERSION_3, temp);
	if (atoi(temp.GetBuffer()) != version_2) changes = true;
	GetDlgItemText(EDIT_VERSION_4, temp);
	if (atoi(temp.GetBuffer()) != version_3) changes = true;

	GetDlgItemText(EDIT_TITLE, title);
	GetDlgItemText(EDIT_AUTHOR, author);
	GetDlgItemText(EDIT_VERSION_1, temp); version_0 = atoi(temp.GetBuffer());
	GetDlgItemText(EDIT_VERSION_2, temp); version_1 = atoi(temp.GetBuffer());
	GetDlgItemText(EDIT_VERSION_3, temp); version_2 = atoi(temp.GetBuffer());
	GetDlgItemText(EDIT_VERSION_4, temp); version_3 = atoi(temp.GetBuffer());

	exe.mod.mHeader.events.drown_core_flag = GENERAL_EventTextEdit_GetValue(GetDlgItem(EDIT_FLAG_CORE_DROWN));
	exe.mod.mHeader.events.drown_core      = GENERAL_EventTextEdit_GetValue(GetDlgItem(EDIT_EVENT_COREDROWN));
	exe.mod.mHeader.events.everyone_died   = GENERAL_EventTextEdit_GetValue(GetDlgItem(EDIT_EVENT_EVERYONE_DIED));
	exe.mod.mHeader.events.death           = GENERAL_EventTextEdit_GetValue(GetDlgItem(EDIT_EVENT_DEATH));
	exe.mod.mHeader.events.drown           = GENERAL_EventTextEdit_GetValue(GetDlgItem(EDIT_EVENT_DROWN));
	exe.mod.mHeader.music.netmenu          = ((CComboBox*)GetDlgItem(COMBO_MUSIC_NETMENU))->GetCurSel();
	exe.mod.mHeader.music.title            = ((CComboBox*)GetDlgItem(COMBO_MUSIC_TITLE))->GetCurSel();

	if (collectables.enabled != (bool)IsDlgButtonChecked(CHECK_ALLOW_COLLECTABLES))
	{
		changes = true;
		collectables.enabled = (bool)IsDlgButtonChecked(CHECK_ALLOW_COLLECTABLES);
	}
	
	if (collectables.surface_id != ((CComboBox*)GetDlgItem(LIST_COLLECTABLE_SURF))->GetCurSel())
	{
		changes = true;
		collectables.surface_id = ((CComboBox*)GetDlgItem(LIST_COLLECTABLE_SURF))->GetCurSel();
	}

	int iSrcL = GetDlgItemInt(EDIT_COLLECTABLE_X);
	int iSrcT = GetDlgItemInt(EDIT_COLLECTABLE_Y);
	int iSrcR = (CS_DEFAULT_USE_RECT_XYWH ? iSrcL : 0) + GetDlgItemInt(EDIT_COLLECTABLE_W);
	int iSrcB = (CS_DEFAULT_USE_RECT_XYWH ? iSrcT : 0) + GetDlgItemInt(EDIT_COLLECTABLE_H);
	
	if (collectables.sprite_rect.left   != iSrcL) { changes = true; collectables.sprite_rect.left   = iSrcL; }
	if (collectables.sprite_rect.top    != iSrcT) { changes = true; collectables.sprite_rect.top    = iSrcT; }
	if (collectables.sprite_rect.right  != iSrcR) { changes = true; collectables.sprite_rect.right  = iSrcR; }
	if (collectables.sprite_rect.bottom != iSrcB) { changes = true; collectables.sprite_rect.bottom = iSrcB; }

	if (hud.draw_hp_bar  != (bool)IsDlgButtonChecked(CHECK_DRAW_HPBAR))   { changes = true; hud.draw_hp_bar  = (bool)IsDlgButtonChecked(CHECK_DRAW_HPBAR);   }
	if (hud.draw_hp      != (bool)IsDlgButtonChecked(CHECK_DRAW_HP))      { changes = true; hud.draw_hp      = (bool)IsDlgButtonChecked(CHECK_DRAW_HP);      }
	if (hud.draw_weapons != (bool)IsDlgButtonChecked(CHECK_DRAW_WEAPONS)) { changes = true; hud.draw_weapons = (bool)IsDlgButtonChecked(CHECK_DRAW_WEAPONS); }
	if (hud.draw_ammo    != (bool)IsDlgButtonChecked(CHECK_DRAW_AMMO))    { changes = true; hud.draw_ammo    = (bool)IsDlgButtonChecked(CHECK_DRAW_AMMO);    }
	if (hud.draw_exp_bar != (bool)IsDlgButtonChecked(CHECK_DRAW_EXP_BAR)) { changes = true; hud.draw_exp_bar = (bool)IsDlgButtonChecked(CHECK_DRAW_EXP_BAR); }
	if (hud.draw_level   != (bool)IsDlgButtonChecked(CHECK_DRAW_LEVEL))   { changes = true; hud.draw_level   = (bool)IsDlgButtonChecked(CHECK_DRAW_LEVEL);   }

	if (gameplay.allow_carrying_players != (bool)IsDlgButtonChecked(CHECK_ALLOW_CARRYING)) { changes = true; gameplay.allow_carrying_players = (bool)IsDlgButtonChecked(CHECK_ALLOW_CARRYING); }
	if (gameplay.allow_agility          != (bool)IsDlgButtonChecked(CHECK_ALLOW_AGILITY))  { changes = true; gameplay.allow_agility          = (bool)IsDlgButtonChecked(CHECK_ALLOW_AGILITY);  }
	if (gameplay.allow_fishing          != (bool)IsDlgButtonChecked(CHECK_ALLOW_FISHING))  { changes = true; gameplay.allow_fishing          = (bool)IsDlgButtonChecked(CHECK_ALLOW_FISHING);  }

	CComboBox* pCombo = (CComboBox*)GetDlgItem(LIST_COLLECTABLE_SURF);
	CWnd* pPosX = GetDlgItem(EDIT_COLLECTABLE_X);
	CWnd* pPosY = GetDlgItem(EDIT_COLLECTABLE_Y);
	CWnd* pPosW = GetDlgItem(EDIT_COLLECTABLE_W);
	CWnd* pPosH = GetDlgItem(EDIT_COLLECTABLE_H);

	if (music.title != ((CComboBox*)GetDlgItem(COMBO_MUSIC_TITLE))->GetCurSel())
	{
		changes     = true;
		music.title = ((CComboBox*)GetDlgItem(COMBO_MUSIC_TITLE))->GetCurSel();
	}
	if (music.netmenu != ((CComboBox*)GetDlgItem(COMBO_MUSIC_NETMENU))->GetCurSel())
	{
		changes       = true;
		music.netmenu = ((CComboBox*)GetDlgItem(COMBO_MUSIC_NETMENU))->GetCurSel();
	}
	if (titleX != GetDlgItemInt(EDIT_TITLE_X))
	{
		changes = true;
		titleX = GetDlgItemInt(EDIT_TITLE_X);
	}
	if (titleY != GetDlgItemInt(EDIT_TITLE_Y))
	{
		changes = true;
		titleY = GetDlgItemInt(EDIT_TITLE_Y);
	}
	if (titleMapEvent != GetDlgItemInt(EDIT_TITLE_EVENT))
	{
		changes = true;
		titleMapEvent = GetDlgItemInt(EDIT_TITLE_EVENT);
	}
	if (titleMap != ((CComboBox*)GetDlgItem(COMBO_TITLE_MAP))->GetCurSel())
	{
		changes = true;
		titleMap = ((CComboBox*)GetDlgItem(COMBO_TITLE_MAP))->GetCurSel();
	}

	if (startX != GetDlgItemInt(EDIT_START_X))
	{
		changes = true;
		startX = GetDlgItemInt(EDIT_START_X);
	}
	if (startY != GetDlgItemInt(EDIT_START_Y))
	{
		changes = true;
		startY = GetDlgItemInt(EDIT_START_Y);
	}
	if (startMapEvent != GetDlgItemInt(EDIT_START_EVENT))
	{
		changes = true;
		startMapEvent = GetDlgItemInt(EDIT_START_EVENT);
	}
	if (startMap != ((CComboBox*)GetDlgItem(COMBO_START_MAP))->GetCurSel())
	{
		changes = true;
		startMap = ((CComboBox*)GetDlgItem(COMBO_START_MAP))->GetCurSel();
	}
	if (startCurHP != GetDlgItemInt(EDIT_START_HP))
	{
		changes = true;
		startCurHP = GetDlgItemInt(EDIT_START_HP);
	}
	if (startMaxHP != GetDlgItemInt(EDIT_START_MAX_HP))
	{
		changes = true;
		startMaxHP = GetDlgItemInt(EDIT_START_MAX_HP);
	}

	if (startFacing != GetDlgItemInt(EDIT_START_FACING))
	{
		changes = true;
		startFacing = GetDlgItemInt(EDIT_START_FACING);
	}

	if (startFlag01 != IsDlgButtonChecked(CHECK_FLAG1))
	{
		changes = true;
		startFlag01 = IsDlgButtonChecked(CHECK_FLAG1);
	}
	if (startFlag02 != IsDlgButtonChecked(CHECK_FLAG2))
	{
		changes = true;
		startFlag02 = IsDlgButtonChecked(CHECK_FLAG2);
	}
	if (startFlag04 != IsDlgButtonChecked(CHECK_FLAG3))
	{
		changes = true;
		startFlag04 = IsDlgButtonChecked(CHECK_FLAG3);
	}
	if (startFlag08 != IsDlgButtonChecked(CHECK_FLAG4))
	{
		changes = true;
		startFlag08 = IsDlgButtonChecked(CHECK_FLAG4);
	}
	if (startFlag10 != IsDlgButtonChecked(CHECK_FLAG5))
	{
		changes = true;
		startFlag10 = IsDlgButtonChecked(CHECK_FLAG5);
	}
	if (startFlag20 != IsDlgButtonChecked(CHECK_FLAG6))
	{
		changes = true;
		startFlag20 = IsDlgButtonChecked(CHECK_FLAG6);
	}
	if (startFlag40 != IsDlgButtonChecked(CHECK_FLAG7))
	{
		changes = true;
		startFlag40 = IsDlgButtonChecked(CHECK_FLAG7);
	}
	if (startFlag80 != IsDlgButtonChecked(CHECK_FLAG8))
	{
		changes = true;
		startFlag80 = IsDlgButtonChecked(CHECK_FLAG8);
	}


	if (changes)
		OnOK();
	else
		OnCancel();
}

void GameSettingsDlg::OnBnClickedCancel()
{
	OnCancel();
}

void GameSettingsDlg::OnEnUpdatePbmExt()
{
	char test[5];
	int sel;
	GetDlgItemText(EDIT_PBM_EXT, test, 5);
	if (strlen(test) > 3)
	{
		test[3] = '\0';
		sel = ((CEdit*)GetDlgItem(EDIT_PBM_EXT))->GetSel();
		if (sel > 3)
			sel = 3;
		SetDlgItemText(EDIT_PBM_EXT, test);
		((CEdit*)GetDlgItem(EDIT_PBM_EXT))->SetSel(sel, sel);
	}
}

void GameSettingsDlg::OnEnUpdateSaveFileName()
{
	char test[15];
	int sel;
	GetDlgItemText(EDIT_SAVE_FILE_NAME, test, 15);
	if (strlen(test) > 12)
	{
		test[12] = '\0';
		sel = ((CEdit*)GetDlgItem(EDIT_SAVE_FILE_NAME))->GetSel();
		if (sel > 12)
			sel = 12;
		SetDlgItemText(EDIT_SAVE_FILE_NAME, test);
		((CEdit*)GetDlgItem(EDIT_SAVE_FILE_NAME))->SetSel(sel, sel);
	}
}

void GameSettingsDlg::OnEnUpdateCPixel()
{
	char test[10];
	int sel;
	GetDlgItemText(EDIT_C_PIXEL, test, 10);
	if (strlen(test) > 8)
	{
		test[8] = '\0';
		sel = ((CEdit*)GetDlgItem(EDIT_C_PIXEL))->GetSel();
		if (sel > 8)
			sel = 8;
		SetDlgItemText(EDIT_C_PIXEL, test);
		((CEdit*)GetDlgItem(EDIT_C_PIXEL))->SetSel(sel, sel);
	}
}

void GameSettingsDlg::OnBnClickedNoCPixel()
{
	GetDlgItem(EDIT_C_PIXEL)->EnableWindow(!IsDlgButtonChecked(CHECK_NO_C_PIXEL));
}

BOOL GameSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_CourierFont.CreatePointFont(120, _T("Courier New"));

	if (GetDlgItem(EDIT_EVENT_COREDROWN))
		GetDlgItem(EDIT_EVENT_COREDROWN)->SetFont(&m_CourierFont);
	GetDlgItem(EDIT_EVENT_DEATH)->SetFont(&m_CourierFont);
	GetDlgItem(EDIT_EVENT_DROWN)->SetFont(&m_CourierFont);
	GetDlgItem(EDIT_EVENT_EVERYONE_DIED)->SetFont(&m_CourierFont);
	GetDlgItem(EDIT_FLAG_CORE_DROWN)->SetFont(&m_CourierFont);

	SetDlgItemText(EDIT_FLAG_CORE_DROWN,		avar("%d", exe.mod.mHeader.events.drown_core_flag));
	SetDlgItemText(EDIT_EVENT_COREDROWN,		avar("%d", exe.mod.mHeader.events.drown_core));
	SetDlgItemText(EDIT_EVENT_EVERYONE_DIED,	avar("%d", exe.mod.mHeader.events.everyone_died));
	SetDlgItemText(EDIT_EVENT_DEATH,			avar("%d", exe.mod.mHeader.events.death));
	SetDlgItemText(EDIT_EVENT_DROWN,			avar("%d", exe.mod.mHeader.events.drown));

	GENERAL_EventTextEdit_LoseFocus(GetDlgItem(EDIT_FLAG_CORE_DROWN));
	GENERAL_EventTextEdit_LoseFocus(GetDlgItem(EDIT_EVENT_COREDROWN));
	GENERAL_EventTextEdit_LoseFocus(GetDlgItem(EDIT_EVENT_EVERYONE_DIED));
	GENERAL_EventTextEdit_LoseFocus(GetDlgItem(EDIT_EVENT_DEATH));
	GENERAL_EventTextEdit_LoseFocus(GetDlgItem(EDIT_EVENT_DROWN));

	int i;
	char buffer[256];
	//create maps lists
	for (i = 0; i < exe.mod.mStages.count; i++)//load map list
	{
		if (strcmp(exe.mod.mStages.list[i].file, "\\empty") == 0)
			sprintf(buffer, "[%.3i] ***No Map***", i);
		else
			sprintf(buffer, "[%.3i] %s (%s)", i, exe.mod.mStages.list[i].name, exe.mod.mStages.list[i].file);
		((CComboBox*)GetDlgItem(COMBO_START_MAP))->AddString(buffer);
		((CComboBox*)GetDlgItem(COMBO_TITLE_MAP))->AddString(buffer);
	};

	CComboBox* pMusicCombo1 = (CComboBox*)GetDlgItem(COMBO_MUSIC_TITLE);
	CComboBox* pMusicCombo2 = (CComboBox*)GetDlgItem(COMBO_MUSIC_NETMENU);

	for (int i = 0; i < exe.mod.mMusic.count; i++)
	{
		pMusicCombo1->AddString(exe.mod.mMusic.list[i].name);
		pMusicCombo2->AddString(exe.mod.mMusic.list[i].name);
	}

	pMusicCombo1->SetCurSel(exe.mod.mHeader.music.title);
	pMusicCombo2->SetCurSel(exe.mod.mHeader.music.netmenu);

	CheckDlgButton(CHECK_ALLOW_COLLECTABLES, collectables.enabled);
	CComboBox* pCombo = (CComboBox*)GetDlgItem(LIST_COLLECTABLE_SURF);
	CWnd* pPosX       = GetDlgItem(EDIT_COLLECTABLE_X);
	CWnd* pPosY       = GetDlgItem(EDIT_COLLECTABLE_Y);
	CWnd* pPosW       = GetDlgItem(EDIT_COLLECTABLE_W);
	CWnd* pPosH       = GetDlgItem(EDIT_COLLECTABLE_H);

	while (pCombo->GetCount()) pCombo->DeleteString(0);

	pCombo->AddString("00 - Title");
	pCombo->AddString("01 - Pixel");
	pCombo->AddString("02 - Map Tileset");
	pCombo->AddString("03 - <unused>");
	pCombo->AddString("04 - <unused>");
	pCombo->AddString("05 - <unused>");
	pCombo->AddString("06 - Fade");
	pCombo->AddString("07 - <unused>");
	pCombo->AddString("08 - ItemImage");
	pCombo->AddString("09 - Map");
	pCombo->AddString("10 - ScreenGrab");
	pCombo->AddString("11 - Arms");
	pCombo->AddString("12 - ArmsImage");
	pCombo->AddString("13 - RoomName");
	pCombo->AddString("14 - StageItem");
	pCombo->AddString("15 - Loading");
	pCombo->AddString("16 - MyChar");
	pCombo->AddString("17 - Bullet");
	pCombo->AddString("18 - <unused>");
	pCombo->AddString("19 - Caret");
	pCombo->AddString("20 - NpcSym");
	pCombo->AddString("21 - NPC Tileset 1");
	pCombo->AddString("22 - NPC Tileset 2");
	pCombo->AddString("23 - NpcRegu");
	pCombo->AddString("24 - <unused>");
	pCombo->AddString("25 - <unused>");
	pCombo->AddString("26 - TextBox");

	pCombo->EnableWindow(collectables.enabled ? TRUE : FALSE);
	pPosX->EnableWindow(collectables.enabled ? TRUE : FALSE);
	pPosY->EnableWindow(collectables.enabled ? TRUE : FALSE);
	pPosW->EnableWindow(collectables.enabled ? TRUE : FALSE);
	pPosH->EnableWindow(collectables.enabled ? TRUE : FALSE);

	pCombo->SetCurSel(collectables.surface_id);
	SetDlgItemInt(EDIT_COLLECTABLE_X, collectables.sprite_rect.left);
	SetDlgItemInt(EDIT_COLLECTABLE_Y, collectables.sprite_rect.top);
	SetDlgItemInt(EDIT_COLLECTABLE_W, (CS_DEFAULT_USE_RECT_XYWH ? collectables.sprite_rect.right - collectables.sprite_rect.left : collectables.sprite_rect.right));
	SetDlgItemInt(EDIT_COLLECTABLE_H, (CS_DEFAULT_USE_RECT_XYWH ? collectables.sprite_rect.bottom - collectables.sprite_rect.top : collectables.sprite_rect.bottom));

	CheckDlgButton(CHECK_ALLOW_CARRYING, gameplay.allow_carrying_players);
	CheckDlgButton(CHECK_ALLOW_AGILITY, gameplay.allow_agility);
	CheckDlgButton(CHECK_ALLOW_FISHING, gameplay.allow_fishing);
	
	CheckDlgButton(CHECK_DRAW_HPBAR, hud.draw_hp_bar);
	CheckDlgButton(CHECK_DRAW_HP, (!hud.draw_hp_bar ? false : hud.draw_hp));
	CheckDlgButton(CHECK_DRAW_WEAPONS, hud.draw_weapons);
	CheckDlgButton(CHECK_DRAW_AMMO, hud.draw_ammo);
	CheckDlgButton(CHECK_DRAW_EXP_BAR, hud.draw_exp_bar);
	CheckDlgButton(CHECK_DRAW_LEVEL, (!hud.draw_exp_bar ? false : hud.draw_level));
	GetDlgItem(CHECK_DRAW_HP)->EnableWindow(hud.draw_hp_bar);
	GetDlgItem(CHECK_DRAW_LEVEL)->EnableWindow(hud.draw_exp_bar);

	SetDlgItemText(EDIT_TITLE, title);
	SetDlgItemText(EDIT_AUTHOR, author);

	sprintf(buffer, "%d", version_0); SetDlgItemText(EDIT_VERSION_1, buffer);
	sprintf(buffer, "%d", version_1); SetDlgItemText(EDIT_VERSION_2, buffer);
	sprintf(buffer, "%d", version_2); SetDlgItemText(EDIT_VERSION_3, buffer);
	sprintf(buffer, "%d", version_3); SetDlgItemText(EDIT_VERSION_4, buffer);

	SetDlgItemInt(EDIT_TITLE_X, titleX);
	SetDlgItemInt(EDIT_TITLE_Y, titleY);
	SetDlgItemInt(EDIT_TITLE_EVENT, titleMapEvent);
	((CComboBox*)GetDlgItem(COMBO_TITLE_MAP))->SetCurSel(titleMap);

	SetDlgItemInt(EDIT_START_X, startX);
	SetDlgItemInt(EDIT_START_Y, startY);
	SetDlgItemInt(EDIT_START_EVENT, startMapEvent);
	((CComboBox*)GetDlgItem(COMBO_START_MAP))->SetCurSel(startMap);

	SetDlgItemInt(EDIT_START_HP, startCurHP);
	SetDlgItemInt(EDIT_START_MAX_HP, startMaxHP);

	CheckDlgButton(CHECK_FLAG1, startFlag01);
	CheckDlgButton(CHECK_FLAG2, startFlag02);
	CheckDlgButton(CHECK_FLAG3, startFlag04);
	CheckDlgButton(CHECK_FLAG4, startFlag08);
	CheckDlgButton(CHECK_FLAG5, startFlag10);
	CheckDlgButton(CHECK_FLAG6, startFlag20);
	CheckDlgButton(CHECK_FLAG7, startFlag40);
	CheckDlgButton(CHECK_FLAG8, startFlag80);

	SetDlgItemInt(EDIT_START_FACING, startFacing);

	RenderCollectable();

	return TRUE;  // return TRUE unless you set the focus to a control
}

static bool gbChangeTick = false;

void GameSettingsDlg::OnEnChangeVersion1()
{
	if (gbChangeTick)
	{
		gbChangeTick = false;
		return;
	}

	gbChangeTick = true;
	CWnd* pInput = GetDlgItem(EDIT_VERSION_1);

	char Buffer[1024];

	pInput->GetWindowTextA(Buffer, sizeof(Buffer));
	unsigned int CaretPos = ((CEdit*)pInput)->GetCaretPos().x;
	int BufLen = pInput->GetWindowTextLengthA();

	if (atoi(Buffer) < 0)
		sprintf_s(Buffer, sizeof(Buffer), "%d", -atoi(Buffer));

	if (atoi(Buffer) >= 10)
		strcpy(Buffer, "9");

	pInput->SetWindowTextA(Buffer);

	((CEdit*)pInput)->SetSel(0, -1);
	((CEdit*)pInput)->SetSel(-1);
}

void GameSettingsDlg::OnEnChangeVersion2()
{
	if (gbChangeTick)
	{
		gbChangeTick = false;
		return;
	}

	gbChangeTick = true;
	CWnd* pInput = GetDlgItem(EDIT_VERSION_2);

	char Buffer[1024];

	pInput->GetWindowTextA(Buffer, sizeof(Buffer));
	unsigned int CaretPos = ((CEdit*)pInput)->GetCaretPos().x;
	int BufLen = pInput->GetWindowTextLengthA();

	if (atoi(Buffer) < 0)
		sprintf_s(Buffer, sizeof(Buffer), "%d", -atoi(Buffer));

	if (atoi(Buffer) >= 10)
		strcpy(Buffer, "9");

	pInput->SetWindowTextA(Buffer);

	((CEdit*)pInput)->SetSel(0, -1);
	((CEdit*)pInput)->SetSel(-1);
}

void GameSettingsDlg::OnEnChangeVersion3()
{
	if (gbChangeTick)
	{
		gbChangeTick = false;
		return;
	}

	gbChangeTick = true;
	CWnd* pInput = GetDlgItem(EDIT_VERSION_3);

	char Buffer[1024];

	pInput->GetWindowTextA(Buffer, sizeof(Buffer));
	unsigned int CaretPos = ((CEdit*)pInput)->GetCaretPos().x;
	int BufLen = pInput->GetWindowTextLengthA();

	if (atoi(Buffer) < 0)
		sprintf_s(Buffer, sizeof(Buffer), "%d", -atoi(Buffer));

	if (atoi(Buffer) >= 10)
		strcpy(Buffer, "9");

	pInput->SetWindowTextA(Buffer);

	((CEdit*)pInput)->SetSel(0, -1);
	((CEdit*)pInput)->SetSel(-1);
}

void GameSettingsDlg::OnEnChangeVersion4()
{
	if (gbChangeTick)
	{
		gbChangeTick = false;
		return;
	}

	gbChangeTick = true;
	CWnd* pInput = GetDlgItem(EDIT_VERSION_4);

	char Buffer[1024];

	pInput->GetWindowTextA(Buffer, sizeof(Buffer));
	unsigned int CaretPos = ((CEdit*)pInput)->GetCaretPos().x;
	int BufLen = pInput->GetWindowTextLengthA();

	if (atoi(Buffer) < 0)
		sprintf_s(Buffer, sizeof(Buffer), "%d", -atoi(Buffer));

	if (atoi(Buffer) >= 10)
		strcpy(Buffer, "9");

	pInput->SetWindowTextA(Buffer);

	((CEdit*)pInput)->SetSel(0, -1);
	((CEdit*)pInput)->SetSel(-1);
}

void GameSettingsDlg::OnBnClickedAllowCollectables()
{
	bool bAllow       = IsDlgButtonChecked(CHECK_ALLOW_COLLECTABLES);
	CComboBox* pCombo = (CComboBox*)GetDlgItem(LIST_COLLECTABLE_SURF);
	CWnd* pPosX       = GetDlgItem(EDIT_COLLECTABLE_X);
	CWnd* pPosY       = GetDlgItem(EDIT_COLLECTABLE_Y);
	CWnd* pPosW       = GetDlgItem(EDIT_COLLECTABLE_W);
	CWnd* pPosH       = GetDlgItem(EDIT_COLLECTABLE_H);

	// Setup states
	if (bAllow)
	{
		pCombo->EnableWindow(TRUE);
		pPosX->EnableWindow(TRUE);
		pPosY->EnableWindow(TRUE);
		pPosW->EnableWindow(TRUE);
		pPosH->EnableWindow(TRUE);
	}
	else
	{
		pCombo->EnableWindow(FALSE);
		pPosX->EnableWindow(FALSE);
		pPosY->EnableWindow(FALSE);
		pPosW->EnableWindow(FALSE);
		pPosH->EnableWindow(FALSE);
	}

	RenderCollectable();
}

void GameSettingsDlg::OnCbnSelchangeCollectableSurf()
{
	RenderCollectable();
}

void GameSettingsDlg::OnEnChangeCollectableX()
{
	RenderCollectable();
}

void GameSettingsDlg::OnEnChangeCollectableY()
{
	RenderCollectable();
}

void GameSettingsDlg::OnEnChangeCollectableW()
{
	RenderCollectable();
}

void GameSettingsDlg::OnEnChangeCollectableH()
{
	RenderCollectable();
}

void GameSettingsDlg::RenderCollectable()
{
	int iSurfaceId      = GetDlgItemInt(LIST_COLLECTABLE_SURF);
	CWnd* pPictureFrame = GetDlgItem(PICTURE_COLLECTABLE);
	CDC* pCDC           = pPictureFrame->GetDC();

	// Determine the source rect
	int iSrcL = GetDlgItemInt(EDIT_COLLECTABLE_X);
	int iSrcT = GetDlgItemInt(EDIT_COLLECTABLE_Y);
	int iSrcR = (CS_DEFAULT_USE_RECT_XYWH ? iSrcL : 0) + GetDlgItemInt(EDIT_COLLECTABLE_W);
	int iSrcB = (CS_DEFAULT_USE_RECT_XYWH ? iSrcT : 0) + GetDlgItemInt(EDIT_COLLECTABLE_H);

	RECT rcSourceImage = { iSrcL, iSrcT, iSrcR, iSrcB };

	// Get the rect of the control
	CRect rc;
	pPictureFrame->GetWindowRect(rc);

	// Get the rect of the char draw box
	RECT DstRect    = { 2, 2, rc.Width() - 4, rc.Height() - 4 };
	RECT EntireRect = { 0, 0, rc.Width(), rc.Height() };

	// Draw the stretch sprite
		// Make a reasonable rect
	RECT TotalSrcRect = { 0, 0, RECT_WIDTH(rcSourceImage), RECT_HEIGHT(rcSourceImage) };

	{
		int max_dim = (RECT_WIDTH(TotalSrcRect) >= RECT_HEIGHT(TotalSrcRect)) ? RECT_WIDTH(TotalSrcRect) : RECT_HEIGHT(TotalSrcRect);
		float scale = ((float)RECT_WIDTH(DstRect)) / float(max_dim);

		struct
		{
			int x;
			int y;
			int w;
			int h;
		} NewRect;

		if (RECT_WIDTH(TotalSrcRect) >= RECT_HEIGHT(TotalSrcRect))
		{
			NewRect.w = RECT_WIDTH(DstRect);
			NewRect.x = 0;
			NewRect.h = RECT_HEIGHT(TotalSrcRect) * scale;
			NewRect.y = (RECT_WIDTH(DstRect) - NewRect.h) / 2;
		}
		else
		{
			NewRect.y = 0;
			NewRect.h = RECT_WIDTH(DstRect);
			NewRect.w = RECT_WIDTH(TotalSrcRect) * scale;
			NewRect.x = (RECT_WIDTH(DstRect) - NewRect.w) / 2;
		}

		DstRect.left = GET_CENTER_OFFSET(RECT_WIDTH(EntireRect), NewRect.w);
		DstRect.top = GET_CENTER_OFFSET(RECT_HEIGHT(EntireRect), NewRect.h);
		DstRect.right = DstRect.left + NewRect.w;
		DstRect.bottom = DstRect.top + NewRect.h;
	}

	// Fill it first
	HBRUSH pFillBrush = CreateSolidBrush(RGB(0, 0, 32));
	FillRect(pCDC->GetSafeHdc(), &EntireRect, pFillBrush);

	if (!exe.spriteSheet[iSurfaceId].IsNull())
	{
		// Draw the bitmap
		exe.spriteSheet[iSurfaceId].AlphaBlend(pCDC->GetSafeHdc(), DstRect.left, DstRect.top, DstRect.right - DstRect.left, DstRect.bottom - DstRect.top, rcSourceImage.left, rcSourceImage.top, rcSourceImage.right - rcSourceImage.left, rcSourceImage.bottom - rcSourceImage.top);
	}

	pPictureFrame->ReleaseDC(pCDC);
}

void GameSettingsDlg::OnBnClickedDrawHpbar()
{
	if (!IsDlgButtonChecked(CHECK_DRAW_HPBAR))
		CheckDlgButton(CHECK_DRAW_HP, FALSE);

	GetDlgItem(CHECK_DRAW_HP)->EnableWindow(IsDlgButtonChecked(CHECK_DRAW_HPBAR));
}

void GameSettingsDlg::OnBnClickedDrawExpBar()
{
	if (!IsDlgButtonChecked(CHECK_DRAW_EXP_BAR))
		CheckDlgButton(CHECK_DRAW_LEVEL, FALSE);

	GetDlgItem(CHECK_DRAW_LEVEL)->EnableWindow(IsDlgButtonChecked(CHECK_DRAW_EXP_BAR));
}

//------------

void GameSettingsDlg::KillFocus_CoreDrown()
{
	GENERAL_EventTextEdit_LoseFocus(GetDlgItem(EDIT_EVENT_COREDROWN));
}

void GameSettingsDlg::GainFocus_CoreDrown()
{
	GENERAL_EventTextEdit_GainFocus(GetDlgItem(EDIT_EVENT_COREDROWN));
}

//------------

void GameSettingsDlg::KillFocus_EveryoneDied()
{
	GENERAL_EventTextEdit_LoseFocus(GetDlgItem(EDIT_EVENT_EVERYONE_DIED));
}

void GameSettingsDlg::GainFocus_EveryoneDied()
{
	GENERAL_EventTextEdit_GainFocus(GetDlgItem(EDIT_EVENT_EVERYONE_DIED));
}

//------------

void GameSettingsDlg::KillFocus_Drown()
{
	GENERAL_EventTextEdit_LoseFocus(GetDlgItem(EDIT_EVENT_DROWN));
}

void GameSettingsDlg::GainFocus_Drown()
{
	GENERAL_EventTextEdit_GainFocus(GetDlgItem(EDIT_EVENT_DROWN));
}

//------------

void GameSettingsDlg::KillFocus_Death()
{
	GENERAL_EventTextEdit_LoseFocus(GetDlgItem(EDIT_EVENT_DEATH));
}

void GameSettingsDlg::GainFocus_Death()
{
	GENERAL_EventTextEdit_GainFocus(GetDlgItem(EDIT_EVENT_DEATH));
}

//------------

void GameSettingsDlg::KillFocus_FlagCoreDrown()
{
	GENERAL_EventTextEdit_LoseFocus(GetDlgItem(EDIT_FLAG_CORE_DROWN));
}

void GameSettingsDlg::GainFocus_FlagCoreDrown()
{
	GENERAL_EventTextEdit_GainFocus(GetDlgItem(EDIT_FLAG_CORE_DROWN));
}

//------------

void GameSettingsDlg::OnCbnSelchangeMusicTitle()
{
}

void GameSettingsDlg::OnCbnSelchangeMusicNetmenu()
{
}

//------------
