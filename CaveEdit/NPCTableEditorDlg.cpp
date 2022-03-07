// NPCTableEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "General.h"
#include "../src/NpChar_Shared.h"

#include "NPCTableCopyFromDlg.h"
#include "NPCTableEditorDlg.h"
#include ".\npctableeditordlg.h"
#include "ModConfigResource.h"
#include <CaveNet_Config.h>
#include "../src/Mod/CSMAPI_enums.h"

//#include "SharedWindows.h"

// NPCTableEditorDlg dialo

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define LIST_ERASE(LISTV, LISTC, LISTTYPE, INDEX)		memmove(&LISTV[INDEX], &LISTV[INDEX + 1], (LISTC - INDEX - 1) * sizeof(LISTTYPE))

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define CLAMP(X, AMIN, AMAX) ((X) > (AMAX) ? (AMAX) : ((X) < (AMIN) ? (AMIN) : (X)))
#define GET_CENTER_OFFSET(w1, w2) ((MAX((w1), (w2)) / 2) - (MIN((w1), (w2)) / 2))
#define RECT_WIDTH(ARECT) (ARECT.right - ARECT.left)
#define RECT_HEIGHT(ARECT) (ARECT.bottom - ARECT.top)

#define TAB_OFFSET(tabName) (int)offsetof(class NPCTableEditorDlg, tabName)
static struct
{
	UINT mTemplateId;
	unsigned int pTabOffset;
	const char* pTabName;
} gNpcEditorTabs[] =
{
	// TABS! TABS! TABS! TABS! TABS! TABS! TABS! TABS! TABS! TABS! TABS! TABS! TABS! TABS! TABS! TABS! TABS! TABS! TABS! 
	{ TAB_NPCEDIT_PAGE5, TAB_OFFSET(m_Tab4), "Info" },
	{ TAB_NPCEDIT_PAGE1, TAB_OFFSET(m_Tab1), "Flags" },
	{ TAB_NPCEDIT_PAGE2, TAB_OFFSET(m_Tab2), "Nodes" },
	{ TAB_NPCEDIT_PAGE4, TAB_OFFSET(m_Tab3), "Transmit Nodes" }
};
#undef TAB_OFFSET

#define GET_NPCEDIT_TAB(tabIndex) ((NpcEditorTabBase*)((intptr_t)this + gNpcEditorTabs[tabIndex].pTabOffset))

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define NPC_EDITOR_TAB_COUNT (sizeof(gNpcEditorTabs) / sizeof(gNpcEditorTabs[0]))


IMPLEMENT_DYNAMIC(NPCTableEditorDlg, CDialog)
NPCTableEditorDlg::NPCTableEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(NPCTableEditorDlg::IDD, pParent)
{
	old_record = -1;
	change = false;
	char temp[100];
	::GetCurrentDirectory(100, temp);
}

NPCTableEditorDlg::~NPCTableEditorDlg()
{
	for (int i = 0; i < m_NpcData.GetCount(); i++)
	{
		if (m_NpcData[i]->acts.list)
			free(m_NpcData[i]->acts.list);

		delete m_NpcData[i];
	}

	if (!m_pNpcImage.IsNull())
		m_pNpcImage.Destroy();
}

void NPCTableEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_NPCS, m_NpcList);
	DDX_Control(pDX, NPC_EDITOR_ACT_LIST, m_ActList);
}

BEGIN_MESSAGE_MAP(NPCTableEditorDlg, CDialog)
	ON_BN_CLICKED(BUTTON_SAVE, OnBnClickedSave)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(COMBO_TILESET, OnCbnSelchangeTileset)
	ON_EN_CHANGE(EDIT_HIT_TOP, OnEnChangeHitTop)
	ON_EN_CHANGE(EDIT_HIT_RIGHT, OnEnChangeHitRight)
	ON_EN_CHANGE(EDIT_HIT_LEFT, OnEnChangeHitLeft)
	ON_EN_CHANGE(EDIT_HIT_BOTTOM, OnEnChangeHitBottom)
	ON_EN_CHANGE(EDIT_DISPLAY_TOP, OnEnChangeDisplayTop)
	ON_EN_CHANGE(EDIT_DISPLAY_LEFT, OnEnChangeDisplayLeft)
	ON_EN_CHANGE(EDIT_DISPLAY_BOTTOM, OnEnChangeDisplayBottom)
	ON_EN_CHANGE(EDIT_DISPLAY_RIGHT, OnEnChangeDisplayRight)
	ON_BN_CLICKED(BUTTON_CANCEL, OnBnClickedCancel)
	ON_WM_VSCROLL()
	ON_CBN_SELCHANGE(COMBO_HURT_SOUND, OnCbnSelchangeHurtSound)
	ON_CBN_SELCHANGE(COMBO_DEATH_SOUND, OnCbnSelchangeDeathSound)
	ON_CBN_SELCHANGE(COMBO_DEATH_GRAPHIC, OnCbnSelchangeDeathGraphic)
	ON_BN_CLICKED(CHECK_FLAG1, OnBnClickedFlag1)
	ON_BN_CLICKED(CHECK_FLAG2, OnBnClickedFlag2)
	ON_BN_CLICKED(CHECK_FLAG3, OnBnClickedFlag3)
	ON_BN_CLICKED(CHECK_FLAG4, OnBnClickedFlag4)
	ON_BN_CLICKED(CHECK_FLAG5, OnBnClickedFlag5)
	ON_BN_CLICKED(CHECK_FLAG6, OnBnClickedFlag6)
	ON_BN_CLICKED(CHECK_FLAG7, OnBnClickedFlag7)
	ON_BN_CLICKED(CHECK_FLAG8, OnBnClickedFlag8)
	ON_BN_CLICKED(CHECK_FLAG9, OnBnClickedFlag9)
	ON_BN_CLICKED(CHECK_FLAG10, OnBnClickedFlag10)
	ON_BN_CLICKED(CHECK_FLAG11, OnBnClickedFlag11)
	ON_BN_CLICKED(CHECK_FLAG12, OnBnClickedFlag12)
	ON_BN_CLICKED(CHECK_FLAG13, OnBnClickedFlag13)
	ON_BN_CLICKED(CHECK_FLAG14, OnBnClickedFlag14)
	ON_BN_CLICKED(CHECK_FLAG15, OnBnClickedFlag15)
	ON_BN_CLICKED(CHECK_FLAG16, OnBnClickedFlag16)
	ON_BN_CLICKED(CHECK_FLAG17, OnBnClickedFlag17)
	ON_BN_CLICKED(CHECK_FLAG18, OnBnClickedFlag18)
	ON_BN_CLICKED(CHECK_FLAG19, OnBnClickedFlag19)
	ON_BN_CLICKED(CHECK_FLAG20, OnBnClickedFlag20)
	ON_BN_CLICKED(BUTTON_COPYFROM, OnCopyClicked)
	ON_NOTIFY(TCN_SELCHANGE, NPCEDITOR_TAB, OnTcnSelchangeTab)
	ON_EN_CHANGE(EDIT_NPC_NAME, OnEnChangeNpcName)
	ON_BN_CLICKED(BUTTON_NPC_ADD, OnBnClickedNpcAdd)
	ON_BN_CLICKED(BUTTON_NPC_DELETE, OnBnClickedNpcDelete)
	ON_EN_CHANGE(EDIT_NPC_SPRITE_X, OnEnChangeNpcSpriteX)
	ON_EN_CHANGE(EDIT_NPC_SPRITE_Y, OnEnChangeNpcSpriteY)
	ON_EN_CHANGE(EDIT_NPC_SPRITE_W, OnEnChangeNpcSpriteW)
	ON_EN_CHANGE(EDIT_NPC_SPRITE_H, OnEnChangeNpcSpriteH)
	ON_LBN_SELCHANGE(LIST_NPCS, OnLbnSelchangeNpcs)
	ON_BN_CLICKED(BUTTON_IMPORT_NPC_TBL, OnBnClickedImportNpcTbl)
	ON_CBN_SELCHANGE(COMBO_TEST_SPRITESHEET_LIST, OnCbnSelchangeTestSpritesheetList)
	ON_EN_CHANGE(EDIT_HP, OnEnChangeHp)
	ON_EN_CHANGE(EDIT_ATTACK, OnEnChangeAttack)
	ON_EN_CHANGE(EDIT_XP, OnEnChangeXp)
	ON_LBN_SELCHANGE(NPC_EDITOR_ACT_LIST, OnLbnSelchangeEditorActList)
	ON_BN_CLICKED(BUTTON_ACT_ADD, OnBnClickedActAdd)
	ON_BN_CLICKED(BUTTON_ACT_REMOVE, OnBnClickedActRemove)
	ON_EN_CHANGE(EDIT_NPC_ACT_NAME, OnEnChangeNpcActName)
	ON_EN_CHANGE(EDIT_NPC_ACT_NO, OnEnChangeNpcActNo)
END_MESSAGE_MAP()

BOOL NPCTableEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Initialize the weapon data list
	for (int i = 0; i < exe.mod.mNpcs.count; i++)
	{
		PXMOD_NPC_STRUCT* pNewNpc = new PXMOD_NPC_STRUCT;
		memcpy(pNewNpc, &exe.mod.mNpcs.list[i], sizeof(PXMOD_NPC_STRUCT));

		if (exe.mod.mNpcs.list[i].acts.list)
		{
			pNewNpc->acts.list = (PXMOD_NPC_ACT_STRUCT*)malloc(sizeof(PXMOD_NPC_ACT_STRUCT) * exe.mod.mNpcs.list[i].acts.count);
			memcpy(pNewNpc->acts.list, exe.mod.mNpcs.list[i].acts.list, sizeof(PXMOD_NPC_ACT_STRUCT) * exe.mod.mNpcs.list[i].acts.count);
		}

		m_NpcData.Add(pNewNpc);
	}

	char buffer[1024];

	int i;
	CComboBox* ptr;
	char temp[32];

	copyFromDlg.Create(DIALOG_NTE_COPYFROM, this);

	old_act_no = 0;
	ptr = (CComboBox*)GetDlgItem(COMBO_TILESET);
	ptr->AddString("00 - Title");
	ptr->AddString("01 - Pixel");
	ptr->AddString("02 - Map Tileset");
	ptr->AddString("03 - <unused>");
	ptr->AddString("04 - <unused>");
	ptr->AddString("05 - <unused>");
	ptr->AddString("06 - Fade");
	ptr->AddString("07 - <unused>");
	ptr->AddString("08 - ItemImage");
	ptr->AddString("09 - Map");
	ptr->AddString("10 - ScreenGrab");
	ptr->AddString("11 - Arms");
	ptr->AddString("12 - ArmsImage");
	ptr->AddString("13 - RoomName");
	ptr->AddString("14 - StageItem");
	ptr->AddString("15 - Loading");
	ptr->AddString("16 - MyChar");
	ptr->AddString("17 - Bullet");
	ptr->AddString("18 - <unused>");
	ptr->AddString("19 - Caret");
	ptr->AddString("20 - NpcSym");
	ptr->AddString("21 - NPC Tileset 1");
	ptr->AddString("22 - NPC Tileset 2");
	ptr->AddString("23 - NpcRegu");
	ptr->AddString("24 - <unused>");
	ptr->AddString("25 - <unused>");
	ptr->AddString("26 - TextBox");

	//this will load both hurt and death sound effects lists
	for (i = 0; i < 2; i++)
	{
		ptr = (CComboBox*)GetDlgItem(COMBO_HURT_SOUND+i);
		ptr->AddString("00 - <nothing>");
		ptr->AddString("01 - [blip]");
		ptr->AddString("02 - Message typing");
		ptr->AddString("03 - Bonk");
		ptr->AddString("04 - Weapon switch");
		ptr->AddString("05 - ???");
		ptr->AddString("06 - Critter hop");
		ptr->AddString("07 - ???");
		ptr->AddString("08 - ???");
		ptr->AddString("09 - ???");
		ptr->AddString("10 - ???");
		ptr->AddString("11 - Door");
		ptr->AddString("12 - [*plsh*]");
		ptr->AddString("13 - ???");
		ptr->AddString("14 - Get weapon energy");
		ptr->AddString("15 - [click]");
		ptr->AddString("16 - Take damage");
		ptr->AddString("17 - Die");
		ptr->AddString("18 - [Menu?]");
		ptr->AddString("19 - ???");
		ptr->AddString("20 - Health/ammo refill");
		ptr->AddString("21 - [bubble]");
		ptr->AddString("22 - [Click]");
		ptr->AddString("23 - [Thud]");
		ptr->AddString("24 - [Tap] walking?");
		ptr->AddString("25 - Enemy killed?");
		ptr->AddString("26 - [Loud thud]");
		ptr->AddString("27 - Level up!");
		ptr->AddString("28 - [Thump]");
		ptr->AddString("29 - Teleport");
		ptr->AddString("30 - Jump");
		ptr->AddString("31 - [Ting!]");
		ptr->AddString("32 - Polar Star lvl");
		ptr->AddString("33 - Fireball");
		ptr->AddString("34 - Fireball bounce");
		ptr->AddString("35 - Explosion");
		ptr->AddString("36 - ???");
		ptr->AddString("37 - [click]");
		ptr->AddString("38 - Get item?");
		ptr->AddString("39 - [*bvng*]");
		ptr->AddString("40 - Water");
		ptr->AddString("41 - Water");
		ptr->AddString("42 - [Beep]");
		ptr->AddString("43 - [BEEP] computer");
		ptr->AddString("44 - [*KAPOW!*]");
		ptr->AddString("45 - [Ping] Weapon energy bounce");
		ptr->AddString("46 - [*ftt*] Out of ammo");
		ptr->AddString("47 - ???");
		ptr->AddString("48 - Bubble pop");
		ptr->AddString("49 - Spur level 1");
		ptr->AddString("50 - [Squeek!]");
		ptr->AddString("51 - [Squeal!]");
		ptr->AddString("52 - [ROAR!]");
		ptr->AddString("53 - [*bblblbl*]");
		ptr->AddString("54 - [Thud]");
		ptr->AddString("55 - [Squeek]");
		ptr->AddString("56 - [Splash]");
		ptr->AddString("57 - Little damage sound");
		ptr->AddString("58 - [*chlk*]");
		ptr->AddString("59 - ???");
		ptr->AddString("60 - Spur charge (lower)");
		ptr->AddString("61 - Spur charge (higher)");
		ptr->AddString("62 - Spur level 2");
		ptr->AddString("63 - Spur level 3");
		ptr->AddString("64 - Spur MAX");
		ptr->AddString("65 - Spur full?");
		ptr->AddString("66 - ???");
		ptr->AddString("67 - ???");
		ptr->AddString("68 - ???");
		ptr->AddString("69 - ???");
		ptr->AddString("70 - [Whud]");
		ptr->AddString("71 - Tiny explosion");
		ptr->AddString("72 - Small explosion");
		ptr->AddString("73 - ???");
		ptr->AddString("74 - ???");
		ptr->AddString("75 - ???");
		ptr->AddString("76 - ???");
		ptr->AddString("77 - ???");
		ptr->AddString("78 - ???");
		ptr->AddString("78 - ???");
		ptr->AddString("79 - ???");
		ptr->AddString("80 - ???");
		ptr->AddString("81 - ???");
		ptr->AddString("82 - ???");
		ptr->AddString("83 - ???");
		ptr->AddString("84 - ???");
		ptr->AddString("85 - ???");
		ptr->AddString("86 - ???");
		ptr->AddString("87 - ???");
		ptr->AddString("87 - ???");
		ptr->AddString("88 - ???");
		ptr->AddString("89 - ???");
		ptr->AddString("90 - ???");
		ptr->AddString("91 - ???");
		ptr->AddString("92 - ???"); 
		ptr->AddString("93 - ???");
		ptr->AddString("94 - ???");
		ptr->AddString("95 - ???");
		ptr->AddString("96 - ???");
		ptr->AddString("98 - ???");
		ptr->AddString("99 - ???");
		ptr->AddString("100 - ???");
		ptr->AddString("101 - ???");
		ptr->AddString("102 - ???");
		ptr->AddString("103 - ???");
		ptr->AddString("104 - ???");
		ptr->AddString("105 - ???");
		ptr->AddString("106 - ???");
		ptr->AddString("107 - ???");
		ptr->AddString("108 - ???");
		ptr->AddString("109 - ???");
		ptr->AddString("110 - ???");
		ptr->AddString("111 - ???");
		ptr->AddString("112 - ???");
		ptr->AddString("113 - ???");
		ptr->AddString("114 - ???");
		ptr->AddString("115 - ???");
		ptr->AddString("116 - ???");
		ptr->AddString("117 - ???");
		ptr->AddString("118 - ???");
		ptr->AddString("119 - ???");
		ptr->AddString("120 - ???");
		ptr->AddString("121 - ???");
		ptr->AddString("122 - ???");
		ptr->AddString("123 - ???");
		ptr->AddString("124 - ???");
		ptr->AddString("125 - ???");
		ptr->AddString("126 - ???");
		ptr->AddString("127 - ???");
		ptr->AddString("128 - ???");
		ptr->AddString("129 - ???");
		ptr->AddString("130 - ???");
		ptr->AddString("131 - ???");
		ptr->AddString("132 - ???");
		ptr->AddString("133 - ???");
		ptr->AddString("134 - ???");
		ptr->AddString("135 - ???");
		ptr->AddString("136 - ???");
		ptr->AddString("137 - ???");
		ptr->AddString("138 - ???");
		ptr->AddString("139 - ???");
		ptr->AddString("140 - ???");
		ptr->AddString("141 - ???");
		ptr->AddString("142 - ???");
		ptr->AddString("143 - ???");
		ptr->AddString("144 - ???");
		ptr->AddString("145 - ???");
		ptr->AddString("146 - ???");
		ptr->AddString("147 - ???");
		ptr->AddString("148 - ???");
		ptr->AddString("149 - ???");
		ptr->AddString("150 - ???");
		ptr->AddString("151 - ???");
		ptr->AddString("152 - ???");
		ptr->AddString("153 - ???");
		ptr->AddString("154 - ???");
		ptr->AddString("155 - ???");
		ptr->AddString("156 - ???");
		ptr->AddString("157 - ???");
		ptr->AddString("158 - ???");
		ptr->AddString("159 - ???");
		ptr->AddString("150 - ???");
		ptr->AddString("161 - ???");
		ptr->AddString("162 - ???");
		ptr->AddString("163 - ???");
		ptr->AddString("164 - ???");
		ptr->AddString("165 - ???");
		ptr->AddString("166 - ???");
		ptr->AddString("167 - ???");
		ptr->AddString("168 - ???");
		ptr->AddString("169 - ???");
		ptr->AddString("170 - ???");
		ptr->AddString("171 - ???");
		ptr->AddString("172 - ???");
		ptr->AddString("173 - ???");
		ptr->AddString("174 - ???");
		ptr->AddString("175 - ???");
		ptr->AddString("176 - ???");
		ptr->AddString("177 - ???");
		ptr->AddString("178 - ???");
		ptr->AddString("179 - ???");
		ptr->AddString("180 - ???");
		ptr->AddString("181 - ???");
		ptr->AddString("182 - ???");
		ptr->AddString("183 - ???");
		ptr->AddString("184 - ???");
		ptr->AddString("185 - ???");
		ptr->AddString("186 - ???");
		ptr->AddString("187 - ???");
		ptr->AddString("188 - ???");
		ptr->AddString("189 - ???");
		ptr->AddString("190 - ???");
		ptr->AddString("191 - ???");
		ptr->AddString("192 - ???");
		ptr->AddString("193 - ???");
		ptr->AddString("194 - ???");
		ptr->AddString("195 - ???");
		ptr->AddString("196 - ???");
		ptr->AddString("197 - ???");
		ptr->AddString("198 - ???");
		ptr->AddString("199 - ???");
		ptr->AddString("200 - ???");
		ptr->AddString("201 - ???");
		ptr->AddString("202 - ???");
		ptr->AddString("203 - ???");
		ptr->AddString("204 - ???");
		ptr->AddString("205 - ???");
		ptr->AddString("206 - ???");
		ptr->AddString("207 - ???");
		ptr->AddString("208 - ???");
		ptr->AddString("209 - ???");
		ptr->AddString("210 - ???");
		ptr->AddString("211 - ???");
		ptr->AddString("212 - ???");
		ptr->AddString("213 - ???");
		ptr->AddString("214 - ???");
		ptr->AddString("215 - ???");
		ptr->AddString("216 - ???");
		ptr->AddString("217 - ???");
		ptr->AddString("218 - ???");
		ptr->AddString("219 - ???");
		ptr->AddString("220 - ???");
		ptr->AddString("221 - ???");
		ptr->AddString("222 - ???");
		ptr->AddString("223 - ???");
		ptr->AddString("224 - ???");
		ptr->AddString("225 - ???");
		ptr->AddString("226 - ???");
		ptr->AddString("227 - ???");
		ptr->AddString("228 - ???");
		ptr->AddString("229 - ???");
		ptr->AddString("230 - ???");
		ptr->AddString("231 - ???");
		ptr->AddString("232 - ???");
		ptr->AddString("233 - ???");
		ptr->AddString("234 - ???");
		ptr->AddString("235 - ???");
		ptr->AddString("236 - ???");
		ptr->AddString("237 - ???");
		ptr->AddString("238 - ???");
		ptr->AddString("239 - ???");
		ptr->AddString("240 - ???");
		ptr->AddString("241 - ???");
		ptr->AddString("242 - ???");
		ptr->AddString("243 - ???");
		ptr->AddString("244 - ???");
		ptr->AddString("245 - ???");
		ptr->AddString("246 - ???");
		ptr->AddString("247 - ???");
		ptr->AddString("248 - ???");
		ptr->AddString("249 - ???");
		ptr->AddString("250 - ???");
		ptr->AddString("251 - ???");
		ptr->AddString("252 - ???");
		ptr->AddString("253 - ???");
		ptr->AddString("254 - ???");
		ptr->AddString("255 - ???");
	}

	ptr = (CComboBox*)GetDlgItem(COMBO_DEATH_GRAPHIC);
	ptr->AddString("00 - Nothing");
	ptr->AddString("01 - Small Cloud");
	ptr->AddString("02 - Medium Cloud");
	ptr->AddString("03 - Large Cloud");

	::SetCurrentDirectory(filePath);

	// Create tabs
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(NPCEDITOR_TAB);
	CRect pTabRect;
	TCITEM ItemTabText;

	((CEdit*)GetDlgItem(EDIT_NPC_NAME))->SetLimitText(sizeof(PXMOD_NPC_STRUCT::name) - 1);
	printf("Should be able to display %d character(s)\r\n", ((CEdit*)GetDlgItem(EDIT_NPC_NAME))->GetLimitText());

	RECT tmp;
	pTabCtrl->GetWindowRect(&tmp);
	int TabCtrlW = tmp.right - tmp.left;
	int TabCtrlH = tmp.bottom - tmp.top;

	for (int i = 0; i < NPC_EDITOR_TAB_COUNT; i++)
	{
		NpcEditorTabBase* pTab = (NpcEditorTabBase*)GET_NPCEDIT_TAB(i);

		pTab->Create(gNpcEditorTabs[i].mTemplateId, pTabCtrl);
	
		ItemTabText.mask    = TCIF_TEXT | TCIF_PARAM;
		ItemTabText.lParam  = (LPARAM)pTab;
		ItemTabText.pszText = (LPSTR)_T(gNpcEditorTabs[i].pTabName);
		pTabCtrl->InsertItem(i, &ItemTabText);

		pTabCtrl->GetItemRect(0, &pTabRect);
		pTab->SetWindowPos(NULL, pTabRect.left, pTabRect.bottom + 1, TabCtrlW - 4, (TabCtrlH - (pTabRect.bottom - pTabRect.top)) - 5, SWP_NOZORDER);
		pTab->ShowWindow(SW_HIDE);
		pTab->RecalculateScroll();
	}

	m_IgnoreTabChange = false;
	m_OldTab          = 0;
	PickTab(0);

	Refresh();
	m_NpcList.SetCurSel(1);
	LoadNPC();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void NPCTableEditorDlg::PickTab(int index)
{
	if (index < 0 || index >= NPC_EDITOR_TAB_COUNT)
		return;

	// Ignore changes; This is to stop from automatically calling this function
	m_IgnoreTabChange = true;

	// Save the changes on the last one
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc != LB_ERR)
	{
		PXMOD_NPC_STRUCT* pSelNPC = m_NpcData[iCurNpc];
		GET_NPCEDIT_TAB(m_OldTab)->SaveTabPage(m_NpcData[iCurNpc], m_ActList.GetCurSel() - 1);
	}

	// Get the tab control
	CDialogEx* pTab = GET_NPCEDIT_TAB(index);

	// Hide the old tab
	GET_NPCEDIT_TAB(m_OldTab)->ShowWindow(SW_HIDE);

	// Show the new tab
	pTab->ShowWindow(SW_SHOW);

	// Set the new old tab
	m_OldTab = index;

	// Reset change
	m_IgnoreTabChange = false;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void NPCTableEditorDlg::OnBnClickedSave()
{
	SaveNPCs();
	CDialog::OnOK();
}

void NPCTableEditorDlg::OnOK()
{
	// Don't go on enter. This is a very dumb feature of MFC.
}

void NPCTableEditorDlg::OnCancel()
{
	if (change)
	{
		switch (MessageBox("Do you want to save your changes?", 0, MB_YESNOCANCEL))
		{
		case IDYES:
			OnBnClickedSave();
		case IDNO:
			break;
		case IDCANCEL:
			return;
			break;
		}
	}

	CDialog::OnCancel();//don't allow [Escape]
}

void NPCTableEditorDlg::OnClose()
{
	CDialog::OnClose();
}

void NPCTableEditorDlg::OnCopyClicked()
{
}

void NPCTableEditorDlg::OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	if (m_IgnoreTabChange)
		return;

	PickTab(((CTabCtrl*)GetDlgItem(NPCEDITOR_TAB))->GetCurSel());
}

void NPCTableEditorDlg::OnBnClickedNpcAdd()
{
	PXMOD_NPC_STRUCT* pNewNpc = new PXMOD_NPC_STRUCT;

	// Configure the new bullet
	memset(pNewNpc, 0, sizeof(PXMOD_NPC_STRUCT));
	strcpy(pNewNpc->name, "New NPC");

	// Add it to the list
	m_NpcData.Add(pNewNpc);
	Refresh();
	m_NpcList.SetCurSel(m_NpcList.GetCount() - 1);
	LoadNPC();

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnBnClickedNpcDelete()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR)
	{
		MessageBox("Select an NPC first.", "Error");
		return;
	}

	delete m_NpcData[iCurNpc];
	m_NpcData.RemoveAt(iCurNpc, 1);
	Refresh();
	m_NpcList.SetCurSel(m_NpcList.GetCount() - 1);
	LoadNPC();
}

void NPCTableEditorDlg::OnBnClickedCancel()
{
	CDialog::OnClose();
	CDialog::OnCancel();
}

void NPCTableEditorDlg::OnLbnSelchangeNpcs()
{
	LoadNPC();
}

void NPCTableEditorDlg::OnBnClickedImportNpcTbl()
{
	int i;
	char szFilters[] = "Pixel NPC tbl (npc.tbl)|npc.tbl||";
	char buffer[1024];
	char temp[260];

	// Create an Open dialog
	CFileDialog loadFileDialog(TRUE, "npc.tbl", "npc.tbl", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);

	// If we didn't successfully do this, then stop here.
	if (loadFileDialog.DoModal() != IDOK)
		return;
	
	NPCtable tbl;

	sprintf(temp, "\x01%s", loadFileDialog.GetPathName().GetBuffer());
	if (!tbl.load(temp))
	{
		sprintf(buffer, "Failed to load NPC table '%s'!", temp + 1);
		MessageBox(buffer, "FAIL", 0);
		return;
	}

	// Copy the NPCs into the new format
	for (int i = 0; i < tbl.NPC_COUNT; i++)
	{
		const PXMOD_NPC_STRUCT* pDefNPC = ModConfigResource::GetDefaultNpc(i);
		PXMOD_NPC_STRUCT* pNewNPC       = (i >= m_NpcData.GetCount() ? new PXMOD_NPC_STRUCT : m_NpcData[i]);
		EntityINFO* pOldNPC             = &tbl.entity[i];
		bool bNewEntry                  = (i >= m_NpcData.GetCount());

		// Setup details
		pNewNPC->nodes             = (pDefNPC ? pDefNPC->nodes : pOldNPC->nodes);
		pNewNPC->transmit_nodes    = (pDefNPC ? pDefNPC->transmit_nodes : pOldNPC->transmit_nodes);
		pNewNPC->bits              = pOldNPC->flags;
		pNewNPC->life              = pOldNPC->hp;
		pNewNPC->surf              = pOldNPC->tileSet;
		pNewNPC->hit_voice         = pOldNPC->hurtSound;
		pNewNPC->destroy_voice     = pOldNPC->deathSound;
		pNewNPC->size              = pOldNPC->deathPBM;
		pNewNPC->exp               = pOldNPC->weaponDrop;
		pNewNPC->damage            = pOldNPC->attack;
		pNewNPC->hit.front         = pOldNPC->hitLeft;
		pNewNPC->hit.top           = pOldNPC->hitTop;
		pNewNPC->hit.back          = pOldNPC->hitRight;
		pNewNPC->hit.bottom        = pOldNPC->hitBottom;
		pNewNPC->view.front        = pOldNPC->hitX;
		pNewNPC->view.top          = pOldNPC->hitY;
		pNewNPC->view.back         = pOldNPC->cenX;
		pNewNPC->view.bottom       = pOldNPC->cenY;

		if (bNewEntry)
		{
			sprintf_s(pNewNPC->name, sizeof(pNewNPC->name), "Unnamed %d", i);
			pNewNPC->sprite.left       = 0;
			pNewNPC->sprite.top        = 0;
			pNewNPC->sprite.right      = 0;
			pNewNPC->sprite.bottom     = 0;
			pNewNPC->spritesheet_index = 0;

			m_NpcData.Add(pNewNPC);
		}
	}

	sprintf(buffer, "Successfully imported %d NPC%s!", tbl.NPC_COUNT, (tbl.NPC_COUNT == 1 ? "" : "s"));
	MessageBox(buffer, "Success!", 0);
	Refresh();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void NPCTableEditorDlg::OnCbnSelchangeTileset()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->surf = ((CComboBox*)GetDlgItem(COMBO_TILESET))->GetCurSel();

	LoadNPC();
}

void NPCTableEditorDlg::OnEnChangeHitTop()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->hit.top = GetDlgItemInt(EDIT_HIT_TOP);

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnEnChangeHitRight()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->hit.back = GetDlgItemInt(EDIT_HIT_RIGHT);

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnEnChangeHitLeft()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->hit.front = GetDlgItemInt(EDIT_HIT_LEFT);

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnEnChangeHitBottom()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->hit.bottom = GetDlgItemInt(EDIT_HIT_BOTTOM);

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnEnChangeDisplayTop()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->view.top = GetDlgItemInt(EDIT_DISPLAY_TOP);

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnEnChangeDisplayLeft()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->view.front = GetDlgItemInt(EDIT_DISPLAY_LEFT);

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnEnChangeDisplayBottom()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->view.bottom = GetDlgItemInt(EDIT_DISPLAY_BOTTOM);

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnEnChangeDisplayRight()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->view.back = GetDlgItemInt(EDIT_DISPLAY_RIGHT);

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnEnChangeNpcName()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	GetDlgItem(EDIT_NPC_NAME)->GetWindowTextA(m_NpcData[iCurNpc]->name, sizeof(m_NpcData[iCurNpc]->name));

	char buffer[256];
	sprintf_s(buffer, sizeof(buffer), "[%.3i] %s", iCurNpc, m_NpcData[iCurNpc]->name);
	m_NpcList.DeleteString((UINT)iCurNpc);

	if (!m_NpcList.GetCount())
		m_NpcList.AddString(buffer);
	else
		m_NpcList.InsertString((UINT)iCurNpc, buffer);

	m_IgnoreInput = true;
	m_NpcList.SetCurSel(iCurNpc);
	m_IgnoreInput = false;

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);

	((CEdit*)GetDlgItem(EDIT_NPC_NAME))->SetLimitText(sizeof(PXMOD_NPC_STRUCT::name) - 1);
}

void NPCTableEditorDlg::OnEnChangeNpcSpriteX()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	if (CS_DEFAULT_USE_RECT_XYWH)
	{
		m_NpcData[iCurNpc]->sprite.right = GetDlgItemInt(EDIT_NPC_SPRITE_X) + (m_NpcData[iCurNpc]->sprite.right - m_NpcData[iCurNpc]->sprite.left);
		m_NpcData[iCurNpc]->sprite.left  = GetDlgItemInt(EDIT_NPC_SPRITE_X);
	}
	else
	{
		m_NpcData[iCurNpc]->sprite.left  = GetDlgItemInt(EDIT_NPC_SPRITE_X);
	}

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnEnChangeNpcSpriteY()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	if (CS_DEFAULT_USE_RECT_XYWH)
	{
		m_NpcData[iCurNpc]->sprite.bottom = GetDlgItemInt(EDIT_NPC_SPRITE_Y) + (m_NpcData[iCurNpc]->sprite.bottom - m_NpcData[iCurNpc]->sprite.top);
		m_NpcData[iCurNpc]->sprite.top    = GetDlgItemInt(EDIT_NPC_SPRITE_Y);
	}
	else
	{
		m_NpcData[iCurNpc]->sprite.top    = GetDlgItemInt(EDIT_NPC_SPRITE_Y);
	}

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnEnChangeNpcSpriteW()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	if (CS_DEFAULT_USE_RECT_XYWH)
	{
		m_NpcData[iCurNpc]->sprite.right = m_NpcData[iCurNpc]->sprite.left + GetDlgItemInt(EDIT_NPC_SPRITE_W);
	}
	else
	{
		m_NpcData[iCurNpc]->sprite.right = GetDlgItemInt(EDIT_NPC_SPRITE_W);
	}

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnEnChangeNpcSpriteH()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;

	if (CS_DEFAULT_USE_RECT_XYWH)
	{
		m_NpcData[iCurNpc]->sprite.bottom = m_NpcData[iCurNpc]->sprite.top + GetDlgItemInt(EDIT_NPC_SPRITE_H);
	}
	else
	{
		m_NpcData[iCurNpc]->sprite.bottom = GetDlgItemInt(EDIT_NPC_SPRITE_H);
	}

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnEnChangeHp()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->life = GetDlgItemInt(EDIT_HP);
}

void NPCTableEditorDlg::OnEnChangeAttack()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->damage = GetDlgItemInt(EDIT_ATTACK);
}

void NPCTableEditorDlg::OnEnChangeXp()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->exp = GetDlgItemInt(EDIT_XP);
}

void NPCTableEditorDlg::OnCbnSelchangeHurtSound()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->hit_voice = ((CComboBox*)GetDlgItem(COMBO_HURT_SOUND))->GetCurSel();
}

void NPCTableEditorDlg::OnCbnSelchangeDeathSound()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->destroy_voice = ((CComboBox*)GetDlgItem(COMBO_DEATH_SOUND))->GetCurSel();
}

void NPCTableEditorDlg::OnCbnSelchangeDeathGraphic()
{
	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR || m_IgnoreInput)
		return;

	change = true;
	m_NpcData[iCurNpc]->size = ((CComboBox*)GetDlgItem(COMBO_DEATH_GRAPHIC))->GetCurSel();
}

void NPCTableEditorDlg::OnBnClickedFlag1()
{
}

void NPCTableEditorDlg::OnBnClickedFlag2()
{
}

void NPCTableEditorDlg::OnBnClickedFlag3()
{
}

void NPCTableEditorDlg::OnBnClickedFlag4()
{
}

void NPCTableEditorDlg::OnBnClickedFlag5()
{
}

void NPCTableEditorDlg::OnBnClickedFlag6()
{
}

void NPCTableEditorDlg::OnBnClickedFlag7()
{
}

void NPCTableEditorDlg::OnBnClickedFlag8()
{
}

void NPCTableEditorDlg::OnBnClickedFlag9()
{
}

void NPCTableEditorDlg::OnBnClickedFlag10()
{
}

void NPCTableEditorDlg::OnBnClickedFlag11()
{
}

void NPCTableEditorDlg::OnBnClickedFlag12()
{
}

void NPCTableEditorDlg::OnBnClickedFlag13()
{
}

void NPCTableEditorDlg::OnBnClickedFlag14()
{
}

void NPCTableEditorDlg::OnBnClickedFlag15()
{
}

void NPCTableEditorDlg::OnBnClickedFlag16()
{
}

void NPCTableEditorDlg::OnBnClickedFlag17()
{
}

void NPCTableEditorDlg::OnBnClickedFlag18()
{
}

void NPCTableEditorDlg::OnBnClickedFlag19()
{
}

void NPCTableEditorDlg::OnBnClickedFlag20()
{
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void NPCTableEditorDlg::PopulateSurfaceList(std::vector<char*>* pList, int iSurfType)
{
	char buffer[512];

	if (iSurfType == 2)
	{
		// Map Tilesets
		for (int i = 0; i < exe.mod.mTilesets.count; i++)
		{
			sprintf_s(buffer, sizeof(buffer), "%s/Stage/Prt%s.png", exe.mod.mModPath, exe.mod.mTilesets.list[i].name);
			pList->push_back(strdup(buffer));
		}
	}
	else if (iSurfType == 8)
	{
		// ItemImage
		sprintf_s(buffer, sizeof(buffer), "%s/ItemImage.png", exe.mod.mModPath);

		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(buffer) && GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			strcpy(buffer, exe.mod.mModPath);
			*strrchr(buffer, '/') = 0;
			strcat(buffer, "/data/ItemImage.png");
		}

		pList->push_back(strdup(buffer));
	}
	else if (iSurfType == 11)
	{
		// Arms
		sprintf_s(buffer, sizeof(buffer), "%s/Arms.png", exe.mod.mModPath);

		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(buffer) && GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			strcpy(buffer, exe.mod.mModPath);
			*strrchr(buffer, '/') = 0;
			strcat(buffer, "/data/Arms.png");
		}

		pList->push_back(strdup(buffer));
	}
	else if (iSurfType == 12)
	{
		// ArmsImage
		sprintf_s(buffer, sizeof(buffer), "%s/ArmsImage.png", exe.mod.mModPath);

		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(buffer) && GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			strcpy(buffer, exe.mod.mModPath);
			*strrchr(buffer, '/') = 0;
			strcat(buffer, "/data/ArmsImage.png");
		}

		pList->push_back(strdup(buffer));
	}
	else if (iSurfType == 16)
	{
		// MyChar
		sprintf_s(buffer, sizeof(buffer), "%s/MyChar.png", exe.mod.mModPath);

		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(buffer) && GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			strcpy(buffer, exe.mod.mModPath);
			*strrchr(buffer, '/') = 0;
			strcat(buffer, "/data/MyChar.png");
		}

		pList->push_back(strdup(buffer));
	}
	else if (iSurfType == 17)
	{
		// Bullet
		sprintf_s(buffer, sizeof(buffer), "%s/Bullet.png", exe.mod.mModPath);

		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(buffer) && GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			strcpy(buffer, exe.mod.mModPath);
			*strrchr(buffer, '/') = 0;
			strcat(buffer, "/data/Bullet.png");
		}

		pList->push_back(strdup(buffer));
	}
	else if (iSurfType == 19)
	{
		// Caret
		sprintf_s(buffer, sizeof(buffer), "%s/Caret.png", exe.mod.mModPath);

		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(buffer) && GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			strcpy(buffer, exe.mod.mModPath);
			*strrchr(buffer, '/') = 0;
			strcat(buffer, "/data/Caret.png");
		}

		pList->push_back(strdup(buffer));
	}
	else if (iSurfType == 20)
	{
		// NpcSym
		sprintf_s(buffer, sizeof(buffer), "%s/Npc/NpcSym.png", exe.mod.mModPath);

		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(buffer) && GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			strcpy(buffer, exe.mod.mModPath);
			*strrchr(buffer, '/') = 0;
			strcat(buffer, "/data/NpcSym.png");
		}

		pList->push_back(strdup(buffer));
	}
	else if (iSurfType == 23)
	{
		// NpcRegu
		sprintf_s(buffer, sizeof(buffer), "%s/Npc/NpcRegu.png", exe.mod.mModPath);

		if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(buffer) && GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			strcpy(buffer, exe.mod.mModPath);
			*strrchr(buffer, '/') = 0;
			strcat(buffer, "/data/NpcRegu.png");
		}

		pList->push_back(strdup(buffer));
	}
	else
	{
		// NPC Tilesets (show all spritesheets)
		for (int i = 0; i < exe.mod.mSpritesheets.count; i++)
		{
			sprintf_s(buffer, sizeof(buffer), "%s/Npc/Npc%s.png", exe.mod.mModPath, exe.mod.mSpritesheets.list[i].name);
			pList->push_back(strdup(buffer));
		}
	}
}

void NPCTableEditorDlg::FreeSurfaceList(std::vector<char*>* pList)
{
	for (std::vector<char*>::iterator it = pList->begin(); it != pList->end(); it++)
		delete[](*it);

	pList->clear();
}

bool NPCTableEditorDlg::LoadSurfaceBitmap(CaveBitMap* pCBM, int iSurfType, int iIndex)
{
	std::vector<char*> pList;

	// Destroy the image if it's already loaded in
	if (!pCBM->IsNull())
		pCBM->Destroy();

	// Populate the surface list
	PopulateSurfaceList(&pList, iSurfType);

	// Clamp the index
	if (iIndex < 0 || iIndex >= pList.size())
		iIndex = pList.size() - 1;

	// Load the imae
	if (pList.size()) printf("Load \"%s\"\r\n", pList[iIndex]);
	bool bResult = (pList.size() ? pCBM->Load(pList[iIndex]) : false);

	// Free the list
	FreeSurfaceList(&pList);

	// Done!
	return bResult;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void NPCTableEditorDlg::Refresh()
{
	while (m_NpcList.GetCount())
		m_NpcList.DeleteString(0);

	char Buffer[256];
	for (int i = 0; i < m_NpcData.GetCount(); i++)
	{
		sprintf_s(Buffer, sizeof(Buffer), "[%.3i] %s", i, m_NpcData[i]->name);

		m_NpcList.AddString(Buffer);
	}
}

void NPCTableEditorDlg::LoadNPC()
{
	if (old_record >= 0 && old_record < m_NpcData.GetCount())
	{
		m_Tab1.SaveTabPage(m_NpcData[old_record], old_act_no - 1);
		m_Tab2.SaveTabPage(m_NpcData[old_record], old_act_no - 1);
		m_Tab3.SaveTabPage(m_NpcData[old_record], old_act_no - 1);
		m_Tab4.SaveTabPage(m_NpcData[old_record], old_act_no - 1);
	}

	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc == LB_ERR)
		return;

	PXMOD_NPC_STRUCT* pSelNPC = m_NpcData[iCurNpc];

	old_record    = iCurNpc;
	m_IgnoreInput = true;

	CComboBox* ptr;
	ptr = (CComboBox*)GetDlgItem(COMBO_TEST_SPRITESHEET_LIST);

	// Delete all existing items in the list
	while (ptr->GetCount()) ptr->DeleteString(0);

	// Get the surface type
	std::vector<char*> test_list;
	PopulateSurfaceList(&test_list, pSelNPC->surf);

	// Populate the combo box
	char buffer[1024];
	char temp[1024];
	for (int i = 0; i < test_list.size(); i++)
	{
		strcpy(temp, test_list[i]);

		if (strrchr(temp, '/'))
			strcpy(buffer, strrchr(temp, '/') + 1);
		else
			strcpy(buffer, temp);

		if (buffer[0] == 'N' && buffer[1] == 'p' && buffer[2] == 'c')
		{
			strcpy(temp, buffer + 3);
			strcpy(buffer, temp);
		}
		else if (buffer[0] == 'P' && buffer[1] == 'r' && buffer[2] == 't')
		{
			strcpy(temp, buffer + 3);
			strcpy(buffer, temp);
		}

		if (strrchr(buffer, '.'))
			*strrchr(buffer, '.') = 0;

		ptr->AddString(buffer);
	}

	FreeSurfaceList(&test_list);
	ptr->SetCurSel(CLAMP(pSelNPC->spritesheet_index, 0, ptr->GetCount() - 1));

	sprintf(buffer, "NPC Table Editor - Entity %i", iCurNpc);
	SetWindowText(buffer);

	SetDlgItemText(EDIT_NPC_NAME, pSelNPC->name);
	SetDlgItemInt(EDIT_HP, pSelNPC->life, FALSE);
	SetDlgItemInt(EDIT_ATTACK, pSelNPC->damage, FALSE);
	SetDlgItemInt(EDIT_XP, pSelNPC->exp, FALSE);

	SetDlgItemInt(EDIT_HIT_TOP, pSelNPC->hit.top, FALSE);
	SetDlgItemInt(EDIT_HIT_BOTTOM, pSelNPC->hit.bottom, FALSE);
	SetDlgItemInt(EDIT_HIT_LEFT, pSelNPC->hit.front, FALSE);
	SetDlgItemInt(EDIT_HIT_RIGHT, pSelNPC->hit.back, FALSE);

	SetDlgItemInt(EDIT_DISPLAY_TOP, pSelNPC->view.top, FALSE);
	SetDlgItemInt(EDIT_DISPLAY_BOTTOM, pSelNPC->view.bottom, FALSE);
	SetDlgItemInt(EDIT_DISPLAY_LEFT, pSelNPC->view.front, FALSE);
	SetDlgItemInt(EDIT_DISPLAY_RIGHT, pSelNPC->view.back, FALSE);

	SetDlgItemInt(EDIT_NPC_SPRITE_X, pSelNPC->sprite.left, FALSE);
	SetDlgItemInt(EDIT_NPC_SPRITE_Y, pSelNPC->sprite.top, FALSE);
	if (CS_DEFAULT_USE_RECT_XYWH)
	{
		SetDlgItemInt(EDIT_NPC_SPRITE_W, pSelNPC->sprite.right - pSelNPC->sprite.left, FALSE);
		SetDlgItemInt(EDIT_NPC_SPRITE_H, pSelNPC->sprite.bottom - pSelNPC->sprite.top, FALSE);
	}
	else
	{
		SetDlgItemInt(EDIT_NPC_SPRITE_W, pSelNPC->sprite.right, FALSE);
		SetDlgItemInt(EDIT_NPC_SPRITE_H, pSelNPC->sprite.bottom, FALSE);
	}

	for (int i = 0; i < NPC_EDITOR_TAB_COUNT; i++)
		GET_NPCEDIT_TAB(i)->DisplayRecord(pSelNPC, -1);

	((CComboBox*)GetDlgItem(COMBO_TILESET))->SetCurSel(pSelNPC->surf);
	((CComboBox*)GetDlgItem(COMBO_HURT_SOUND))->SetCurSel(pSelNPC->hit_voice);
	((CComboBox*)GetDlgItem(COMBO_DEATH_SOUND))->SetCurSel(pSelNPC->destroy_voice);
	((CComboBox*)GetDlgItem(COMBO_DEATH_GRAPHIC))->SetCurSel(pSelNPC->size);

	while (m_ActList.GetCount())
		m_ActList.DeleteString(0);

	// Populate the act list
	m_ActList.InsertString(0, "Default");

	for (int i = 0; i < pSelNPC->acts.count; i++)
	{
		PXMOD_NPC_ACT_STRUCT* pAct = &pSelNPC->acts.list[i];

		// Construct string
		sprintf(buffer, "[%04d] %s", pAct->act_no, pAct->name);

		// Insert it
		m_ActList.InsertString(i + 1, buffer);
	}

	m_ActList.SetCurSel(0);
	old_act_no = 0;

	LoadSurfaceBitmap(&m_pNpcImage, pSelNPC->surf, pSelNPC->spritesheet_index);

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
	m_IgnoreInput = false;

	OnLbnSelchangeEditorActList();
}

void NPCTableEditorDlg::SaveNPCs()
{
	if (old_record >= 0 && old_record < m_NpcData.GetCount())
	{
		m_Tab1.SaveTabPage(m_NpcData[old_record], old_act_no - 1);
		m_Tab2.SaveTabPage(m_NpcData[old_record], old_act_no - 1);
		m_Tab3.SaveTabPage(m_NpcData[old_record], old_act_no - 1);
		m_Tab4.SaveTabPage(m_NpcData[old_record], old_act_no - 1);
	}

	while (exe.mod.mNpcs.count) exe.mod.RemoveNPC(exe.mod.mNpcs.list[0].name);

	for (int i = 0; i < m_NpcData.GetCount(); i++)
	{
		int iInsertIndex = exe.mod.AddNPC(m_NpcData[i]->name);
		if (iInsertIndex == -1)
			continue;

		printf("NPC # %04d: (\"%s\" === %s)\r\n", i, m_NpcData[i]->name, m_NpcData[i]->category);

		memcpy(&exe.mod.mNpcs.list[iInsertIndex], m_NpcData[i], sizeof(PXMOD_NPC_STRUCT));

		// Insert acts
		exe.mod.mNpcs.list[iInsertIndex].acts.list  = NULL;
		exe.mod.mNpcs.list[iInsertIndex].acts.count = 0;
		for (int j = 0; j < m_NpcData[i]->acts.count; j++)
			exe.mod.AddNpcAct(iInsertIndex, m_NpcData[i]->acts.list[j].act_no, m_NpcData[i]->acts.list[j].name, m_NpcData[i]->acts.list[j].nodes, m_NpcData[i]->acts.list[j].transmit_nodes);
	}

	// reload npc table
	if (NPCtable::us)
		NPCtable::us->reload_current_mod_npc_table();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

void NPCTableEditorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	RenderNPC(dc);
}

void NPCTableEditorDlg::RenderNPC(CDC& dc)
{
	int iCurBul     = m_NpcList.GetCurSel();
	CaveBitMap* bmp = &m_pNpcImage;
	
	switch (m_NpcData[iCurBul]->surf)
	{
		case SURFACE_ID_LEVEL_TILESET:
		case SURFACE_ID_LEVEL_SPRITESET_1:
		case SURFACE_ID_LEVEL_SPRITESET_2:
		{
			bmp = &m_pNpcImage;
			break;
		}
		default:
		{
			if (m_NpcData[iCurBul]->surf >= sizeof(exe.spriteSheet) / sizeof(exe.spriteSheet[0]))
			{
				bmp = &m_pNpcImage;
			}
			else
			{
				if (exe.spriteSheet[m_NpcData[iCurBul]->surf].IsNull())
					bmp = &m_pNpcImage;
				else
					bmp = &exe.spriteSheet[m_NpcData[iCurBul]->surf];
			}

			break;
		}
	}

	if (iCurBul == LB_ERR || bmp->IsNull())
		return;

	PXMOD_NPC_STRUCT* pNpc = m_NpcData[iCurBul];
	CRect EntireRect;
	CRect DstRect;
	CRect SrcRect = { pNpc->sprite.left, pNpc->sprite.top, pNpc->sprite.right, pNpc->sprite.bottom };
	RECT rect;

	int max_dim = 0;
	float scale = 0.f;

	// Get the size of the picture box
	GetDlgItem(FRAME_NPC)->GetWindowRect(&EntireRect);
	GetDlgItem(FRAME_NPC)->GetWindowRect(&DstRect);

	// Get the rect of the picture box
	GetDlgItem(FRAME_NPC)->GetClientRect(&rect);
	GetDlgItem(FRAME_NPC)->ClientToScreen(&rect);
	ScreenToClient(&rect);

	// Draw the background
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(dc.GetSafeHdc(), &rect, brush);

	{
		max_dim = (RECT_WIDTH(SrcRect) >= RECT_HEIGHT(SrcRect)) ? RECT_WIDTH(SrcRect) : RECT_HEIGHT(SrcRect);
		scale = ((float)RECT_WIDTH(EntireRect)) / float(max_dim);

		struct
		{
			int x;
			int y;
			int w;
			int h;
		} NewRect;

		if (RECT_WIDTH(SrcRect) >= RECT_HEIGHT(SrcRect))
		{
			NewRect.x = 0;
			NewRect.w = RECT_WIDTH(EntireRect);
			NewRect.h = RECT_HEIGHT(SrcRect) * scale;
			NewRect.y = (RECT_WIDTH(EntireRect) - NewRect.h) / 2;
		}
		else
		{
			NewRect.y = 0;
			NewRect.h = RECT_WIDTH(EntireRect);
			NewRect.w = RECT_WIDTH(SrcRect) * scale;
			NewRect.x = (RECT_WIDTH(EntireRect) - NewRect.w) / 2;
		}

		DstRect.left   = rect.left + GET_CENTER_OFFSET(RECT_WIDTH(EntireRect), NewRect.w);
		DstRect.top    = rect.top + GET_CENTER_OFFSET(RECT_HEIGHT(EntireRect), NewRect.h);
		DstRect.right  = DstRect.left + NewRect.w;
		DstRect.bottom = DstRect.top + NewRect.h;
	}

	// Draw the bullet
	bmp->AlphaBlend(dc.GetSafeHdc(), DstRect, SrcRect);

	// Draw the hitboxes
	float PixelW = float(RECT_WIDTH(DstRect)) / float(RECT_WIDTH(SrcRect));
	float PixelH = float(RECT_HEIGHT(DstRect)) / float(RECT_HEIGHT(SrcRect));
	int CenterX  = (DstRect.left + (RECT_WIDTH(DstRect) / 2));
	int CenterY  = (DstRect.top + (RECT_HEIGHT(DstRect) / 2));

	HBRUSH red   = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH green = CreateSolidBrush(RGB(0, 255, 0));

	// View box
	{
		RECT ViewBoxRc =
		{
			CenterX - (LONG)roundf(float(pNpc->view.front) * PixelW),
			CenterY - (LONG)roundf(float(pNpc->view.top) * PixelH),
			CenterX + (LONG)roundf(float(pNpc->view.back) * PixelW),
			CenterY + (LONG)roundf(float(pNpc->view.bottom) * PixelH)
		};

		if (ViewBoxRc.left < rect.left)     ViewBoxRc.left = rect.left;
		if (ViewBoxRc.top < rect.top)       ViewBoxRc.top = rect.top;
		if (ViewBoxRc.right > rect.right)   ViewBoxRc.right = rect.right;
		if (ViewBoxRc.bottom > rect.bottom) ViewBoxRc.bottom = rect.bottom;

		FrameRect(dc, &ViewBoxRc, green);
	}

	// Hit box
	{
		RECT HitBoxRc =
		{
			CenterX - (LONG)roundf(float(pNpc->hit.front) * PixelW),
			CenterY - (LONG)roundf(float(pNpc->hit.top) * PixelH),
			CenterX + (LONG)roundf(float(pNpc->hit.back) * PixelW),
			CenterY + (LONG)roundf(float(pNpc->hit.bottom) * PixelH)
		};

		if (HitBoxRc.left < rect.left)     HitBoxRc.left = rect.left;
		if (HitBoxRc.top < rect.top)       HitBoxRc.top = rect.top;
		if (HitBoxRc.right > rect.right)   HitBoxRc.right = rect.right;
		if (HitBoxRc.bottom > rect.bottom) HitBoxRc.bottom = rect.bottom;

		FrameRect(dc, &HitBoxRc, red);
	}
}

void NPCTableEditorDlg::OnCbnSelchangeTestSpritesheetList()
{
	CComboBox* ptr = (CComboBox*)GetDlgItem(COMBO_TEST_SPRITESHEET_LIST);

	int iCurNpc = m_NpcList.GetCurSel();
	if (iCurNpc != LB_ERR && !m_IgnoreInput)
	{
		change = true;
		m_NpcData[iCurNpc]->spritesheet_index = ptr->GetCurSel();

		LoadSurfaceBitmap(&m_pNpcImage, m_NpcData[iCurNpc]->surf, m_NpcData[iCurNpc]->spritesheet_index);
	}

	CDC* cdc = GetDC();
	RenderNPC(*cdc);
	ReleaseDC(cdc);
}

void NPCTableEditorDlg::OnLbnSelchangeEditorActList()
{
	int iCurBul = m_NpcList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	PXMOD_NPC_STRUCT* pNpc = m_NpcData[iCurBul];

	for (int i = 0; i < NPC_EDITOR_TAB_COUNT; i++)
	{
		GET_NPCEDIT_TAB(i)->SaveTabPage(pNpc, old_act_no - 1);
		GET_NPCEDIT_TAB(i)->DisplayRecord(pNpc, m_ActList.GetCurSel() - 1);
	}

	old_act_no = m_ActList.GetCurSel();

	m_IgnoreInput = true;
	if (old_act_no == 0)
	{
		GetDlgItem(EDIT_NPC_ACT_NAME)->EnableWindow(FALSE);
		GetDlgItem(EDIT_NPC_ACT_NO)->EnableWindow(FALSE);
		SetDlgItemText(EDIT_NPC_ACT_NAME, "Default Act");
		SetDlgItemText(EDIT_NPC_ACT_NO, "N/A");
	}
	else
	{
		GetDlgItem(EDIT_NPC_ACT_NAME)->EnableWindow(TRUE);
		GetDlgItem(EDIT_NPC_ACT_NO)->EnableWindow(TRUE);
		SetDlgItemText(EDIT_NPC_ACT_NAME, pNpc->acts.list[old_act_no - 1].name);
		SetDlgItemInt(EDIT_NPC_ACT_NO, pNpc->acts.list[old_act_no - 1].act_no);
	}
	m_IgnoreInput = false;
}

void NPCTableEditorDlg::OnBnClickedActAdd()
{
	int iCurBul = m_NpcList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	PXMOD_NPC_STRUCT* pNpc = m_NpcData[iCurBul];

	if (!pNpc->acts.list)
	{
		pNpc->acts.count = 1;
		pNpc->acts.list  = (PXMOD_NPC_ACT_STRUCT*)malloc(sizeof(PXMOD_NPC_ACT_STRUCT));
	}
	else
	{
		pNpc->acts.count += 1;
		pNpc->acts.list   = (PXMOD_NPC_ACT_STRUCT*)realloc((void*)pNpc->acts.list, sizeof(PXMOD_NPC_ACT_STRUCT) * pNpc->acts.count);
	}

	PXMOD_NPC_ACT_STRUCT* pAct = &pNpc->acts.list[pNpc->acts.count - 1];
	strcpy(pAct->name, "New Act");
	pAct->act_no         = 0;
	pAct->nodes          = pNpc->nodes;
	pAct->transmit_nodes = pNpc->transmit_nodes;

	m_ActList.InsertString(pNpc->acts.count, "[0000] New Act");
	m_ActList.SetCurSel(pNpc->acts.count);
	OnLbnSelchangeEditorActList();
}

void NPCTableEditorDlg::OnBnClickedActRemove()
{
	int iCurBul = m_NpcList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput || old_act_no == 0)
		return;

	PXMOD_NPC_STRUCT* pNpc = m_NpcData[iCurBul];

	// Erase from our list
	LIST_ERASE(pNpc->acts.list, pNpc->acts.count, PXMOD_NPC_ACT_STRUCT, (old_act_no - 1));

	// Shrink the list
	if (!--pNpc->acts.count)
	{
		free(pNpc->acts.list);
		pNpc->acts.list = NULL;
	}
	else
	{
		pNpc->acts.list = (PXMOD_NPC_ACT_STRUCT*)realloc((void*)pNpc->acts.list, sizeof(PXMOD_NPC_ACT_STRUCT) * pNpc->acts.count);
	}

	m_ActList.DeleteString(old_act_no);
	m_ActList.SetCurSel(old_act_no - 1);
}

void NPCTableEditorDlg::OnEnChangeNpcActName()
{
	int iCurBul = m_NpcList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput || old_act_no == 0)
		return;

	PXMOD_NPC_STRUCT* pNpc = m_NpcData[iCurBul];

	change = true;
	GetDlgItemText(EDIT_NPC_ACT_NAME, pNpc->acts.list[old_act_no - 1].name, sizeof(pNpc->acts.list[old_act_no - 1].name));

	// Re-add to list
	m_ActList.DeleteString(old_act_no);
	char buffer[1024];
	sprintf(buffer, "[%04d] %s", pNpc->acts.list[old_act_no - 1].act_no, pNpc->acts.list[old_act_no - 1].name);
	m_ActList.InsertString(old_act_no, buffer);
	m_ActList.SetCurSel(old_act_no);
}

void NPCTableEditorDlg::OnEnChangeNpcActNo()
{
	int iCurBul = m_NpcList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput || old_act_no == 0)
		return;

	change        = true;
	m_IgnoreInput = true;

	PXMOD_NPC_STRUCT* pNpc = m_NpcData[iCurBul];
	pNpc->acts.list[old_act_no - 1].act_no = GetDlgItemInt(EDIT_NPC_ACT_NO);

	// Re-add to list
	m_ActList.DeleteString(old_act_no);
	char buffer[1024];
	sprintf(buffer, "[%04d] %s", pNpc->acts.list[old_act_no - 1].act_no, pNpc->acts.list[old_act_no - 1].name);
	m_ActList.InsertString(old_act_no, buffer);
	m_ActList.SetCurSel(old_act_no);

	m_IgnoreInput = false;
}
