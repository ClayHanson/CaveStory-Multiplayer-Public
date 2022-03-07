#include "stdafx.h"
#include "CaveEditor.h"
#include "MapTabManager.h"
#include "afxdialogex.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(MapTabManager, CDialog)

BEGIN_MESSAGE_MAP(MapTabManager, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, TAB_MAPS, OnSelectMapTab)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_GETMINMAXINFO()
	ON_COMMAND_RANGE(ID_MAP_SHIFTMAP, ID_FILE_PACKMOD, OnRangeCmds)
	ON_COMMAND(ID_MAPTAB_CLOSETAB, OnCloseTab)
	ON_COMMAND(ID_MAP_CLOSETAB, OnCloseTab)
	ON_WM_SIZE()
	ON_WM_MOVE()
END_MESSAGE_MAP()

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static inline void GetCaveMapEditTabName(CaveMapEditDlg* pDlg, char* pOut)
{
	sprintf(pOut, "%s%s [%s]", pDlg->caveMap.mapInfo.caption, pDlg->changes ? "*" : "", pDlg->caveMap.mapInfo.fileName);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MapTabManager::MapTabManager(CWnd* pParent) : CDialog(DIALOG_MAP_TAB_MANAGER, pParent)
{
	m_LastSelectedMap = -1;
	m_pMapList  = NULL;
	m_iMapCount = 0;
	m_bIgnore   = false;
}

MapTabManager::~MapTabManager()
{
	CloseAllMaps();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void MapTabManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, TAB_MAPS, m_TabList);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL MapTabManager::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Load icons
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, TRUE);

	// Load the context menu
	VERIFY(m_pConMenu.LoadMenu(MENU_MAP_TAB));

	return TRUE;
}

void MapTabManager::RefreshTabList()
{
	if (m_LastSelectedMap >= m_iMapCount)
		m_LastSelectedMap = m_iMapCount - 1;

	m_TabList.DeleteAllItems();

	// Add all tabs
	char pBuffer[1024];
	for (int i = 0; i < m_iMapCount; i++)
	{
		CaveMapEditDlg* pDlg = m_pMapList[i];

		GetCaveMapEditTabName(pDlg, pBuffer);

		// Add it
		m_TabList.InsertItem(i, pBuffer);
	}

	if (m_LastSelectedMap != -1)
		SetMapTab(m_LastSelectedMap);
}

#define CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))

void MapTabManager::SetMapTab(int iIndex)
{
	m_TabList.SetCurSel(iIndex);
	RECT rcTileWindow;
	int iOldTileX = 0;
	int iOldTileY = 0;

	if (m_LastSelectedMap != -1)
	{
		m_pMapList[m_LastSelectedMap]->tileWindow.GetWindowRect(&rcTileWindow);
		iOldTileX = rcTileWindow.left;
		iOldTileY = rcTileWindow.top;
		m_pMapList[m_LastSelectedMap]->ShowWindow(SW_HIDE);

		if (CS_DEFAULT_ME_SYNC_ACROSS_TABS)
		{
			// Synchronize editor tools across tabs!!
			m_pMapList[iIndex]->CheckRadioButton(RADIO_ENTITY, RADIO_NETGROUP, m_pMapList[m_LastSelectedMap]->GetCheckedRadioButton(RADIO_ENTITY, RADIO_NETGROUP));
			m_pMapList[iIndex]->CheckDlgButton(CHECK_FRONT_TILES, m_pMapList[m_LastSelectedMap]->IsDlgButtonChecked(CHECK_FRONT_TILES));
			m_pMapList[iIndex]->CheckDlgButton(CHECK_BACK_TILES, m_pMapList[m_LastSelectedMap]->IsDlgButtonChecked(CHECK_BACK_TILES));
			m_pMapList[iIndex]->CheckDlgButton(CHECK_BACKGROUND, m_pMapList[m_LastSelectedMap]->IsDlgButtonChecked(CHECK_BACKGROUND));
			m_pMapList[iIndex]->CheckDlgButton(CHECK_TILE_TYPES, m_pMapList[m_LastSelectedMap]->IsDlgButtonChecked(CHECK_TILE_TYPES));
			m_pMapList[iIndex]->CheckDlgButton(CHECK_ENTITIES, m_pMapList[m_LastSelectedMap]->IsDlgButtonChecked(CHECK_ENTITIES));
			m_pMapList[iIndex]->CheckDlgButton(CHECK_ENTITY_TEXT, m_pMapList[m_LastSelectedMap]->IsDlgButtonChecked(CHECK_ENTITY_TEXT));
			m_pMapList[iIndex]->CheckDlgButton(CHECK_ENTITY_SPRITES, m_pMapList[m_LastSelectedMap]->IsDlgButtonChecked(CHECK_ENTITY_SPRITES));
			m_pMapList[iIndex]->CheckDlgButton(CHECK_TILE_GRID, m_pMapList[m_LastSelectedMap]->IsDlgButtonChecked(CHECK_TILE_GRID));
			m_pMapList[iIndex]->SetMapZoom(m_pMapList[m_LastSelectedMap]->GetMapZoom());

			if (m_pMapList[iIndex]->entitySort != m_pMapList[m_LastSelectedMap]->entitySort)
			{
				m_pMapList[m_LastSelectedMap]->entitySort = m_pMapList[iIndex]->entitySort;

				switch (m_pMapList[iIndex]->entitySort)
				{
					case ENTITY_SORT_CATEGORY:
					{
						m_pMapList[iIndex]->OnSortentitiiesbyCategory();
						break;
					}
					case ENTITY_SORT_ID:
					{
						m_pMapList[iIndex]->OnSortentitiiesbyIdnumber();
						break;
					}
					case ENTITY_SORT_ALPHA:
					{
						m_pMapList[iIndex]->OnSortentitiiesbyName();
						break;
					}
				}
			}
		}

		// Sync copy entity
		memcpy(&m_pMapList[iIndex]->copyEntity, &m_pMapList[m_LastSelectedMap]->copyEntity, sizeof(Entity));
	}

	// Get the new menu
	m_LastSelectedMap = iIndex;

	SetMenu(&m_pMapList[iIndex]->m_OurMenu);

	// Get the desired rect
	CRect rcClient, rcWindow;

	m_pMapList[iIndex]->tileWindow.GetWindowRect(&rcTileWindow);
	m_pMapList[iIndex]->ShowWindow(SW_SHOWMAXIMIZED);

	m_TabList.GetClientRect(&rcClient);
	m_TabList.AdjustRect(FALSE, &rcClient);

	m_TabList.GetWindowRect(&rcWindow);
	ScreenToClient(rcWindow);

	rcClient.OffsetRect(rcWindow.left, rcWindow.top);

	CRect rcDesktop;
	GetDesktopWindow()->GetWindowRect(&rcDesktop);

	char buffer[1024];
	sprintf(buffer, "Cavestory Multiplayer Map Editor - %s (%s)", m_pMapList[iIndex]->caveMap.mapInfo.caption, m_pMapList[iIndex]->caveMap.mapInfo.fileName);
	SetWindowText(buffer);

	m_pMapList[iIndex]->SetWindowPos(0, rcClient.left - 5, rcClient.top, rcClient.Width() + 5, rcClient.Height(), SWP_NOZORDER | SWP_NOREPOSITION | SWP_NOREDRAW);
	m_pMapList[iIndex]->tileWindow.SetWindowPos(
		0,
		CLAMP(iOldTileX, 0, rcDesktop.Width() - RECT_WIDTH(rcTileWindow)),
		CLAMP(iOldTileY, 0, rcDesktop.Height() - RECT_HEIGHT(rcTileWindow)),
		0,
		0,
		SWP_NOZORDER | SWP_NOSIZE
	);

	if (m_pMapList[iIndex]->GetCheckedRadioButton(RADIO_ENTITY, RADIO_NETGROUP) == RADIO_MAP)
		m_pMapList[iIndex]->tileWindow.ShowWindow(SW_SHOW);
}

void MapTabManager::LoadMap(int iStageIndex)
{
	for (int i = 0; i < m_iMapCount; i++)
	{
		if (m_pMapList[i]->mapIndex != iStageIndex)
			continue;

		// Set it as the selected tab.
		SetMapTab(i);

		// Already loaded.
		return;
	}

	// Allocate a new slot
	if (!m_pMapList)
	{
		m_iMapCount = 1;
		m_pMapList  = (CaveMapEditDlg**)malloc(sizeof(CaveMapEditDlg*) * m_iMapCount);
	}
	else
	{
		m_iMapCount += 1;
		m_pMapList   = (CaveMapEditDlg**)realloc((void*)m_pMapList, sizeof(CaveMapEditDlg*) * m_iMapCount);
	}

	// Get the newly allocated slot
	CaveMapEditDlg* pNewDlg = new CaveMapEditDlg(&m_TabList);

	// Setup everything
	pNewDlg->m_MapTabMgr = this;
	pNewDlg->Create(DIALOG_MAP_EDIT, &m_TabList);
	//STYLE DS_SETFONT | DS_NOIDLEMSG | DS_FIXEDSYS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME

	// Set the slot
	m_pMapList[m_iMapCount - 1] = pNewDlg;

	if (!pNewDlg->loadMap(&CaveMapINFO(exe.mod.mStages.list[iStageIndex], &exe.mod), iStageIndex))
	{
		CloseMap(iStageIndex);
		return;
	}

	pNewDlg->SetParent(&m_TabList);
	pNewDlg->ModifyStyle(WS_BORDER | WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_THICKFRAME | WS_OVERLAPPEDWINDOW, WS_CHILD, SWP_DRAWFRAME);

	// Load the dialog
	RefreshTabList();

	SetMapTab(m_iMapCount - 1);
}

void MapTabManager::CloseMap(int iStageIndex)
{
	for (int i = 0; i < m_iMapCount; i++)
	{
		if (m_pMapList[i]->mapIndex != iStageIndex)
			continue;

		m_pMapList[i]->DestroyWindow();
		delete m_pMapList[i];

		LIST_ERASE(m_pMapList, m_iMapCount, CaveMapEditDlg*, i);
		--m_iMapCount;

		if (!m_iMapCount)
		{
			free(m_pMapList);
			m_pMapList  = NULL;
			m_iMapCount = 0;
		}
		else
		{
			// resize to fit
			m_pMapList = (CaveMapEditDlg**)realloc((void*)m_pMapList, sizeof(CaveMapEditDlg*) * m_iMapCount);
		}

		if (m_LastSelectedMap == i)
		{
			if (--m_LastSelectedMap < 0)
				m_LastSelectedMap = 0;

			if (m_LastSelectedMap >= m_iMapCount)
				m_LastSelectedMap = m_iMapCount - 1;

			char buffer[1024];
			sprintf(buffer, "Cavestory Multiplayer Map Editor");
			SetWindowText(buffer);

			SetMenu(NULL);
		}

		break;
	}

	RefreshTabList();
}

void MapTabManager::CloseAllMaps()
{
	if (!m_pMapList)
		return;

	for (int i = 0; i < m_iMapCount; i++)
	{
		m_pMapList[i]->DestroyWindow();
		delete m_pMapList[i];
	}

	free(m_pMapList);
	m_pMapList  = NULL;
	m_iMapCount = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void MapTabManager::OnMapChangeUpdate()
{
	if (m_pMapList && m_LastSelectedMap != -1)
	{
		char pBuffer[1024];
		GetCaveMapEditTabName(m_pMapList[m_LastSelectedMap], pBuffer);
		m_bIgnore = true;
		m_TabList.DeleteItem(m_LastSelectedMap);
		m_TabList.InsertItem(m_LastSelectedMap, pBuffer);
		m_pMapList[m_LastSelectedMap]->Invalidate(TRUE);
		m_bIgnore = false;
	}
}

void MapTabManager::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	//689 539
	lpMMI->ptMinTrackSize.x = 705;
	lpMMI->ptMinTrackSize.y = 578;
}

void MapTabManager::OnRangeCmds(UINT nID)
{
	if (m_pMapList && m_LastSelectedMap != -1)
		m_pMapList[m_LastSelectedMap]->SendMessage(WM_COMMAND, nID);
}

BOOL MapTabManager::PreTranslateMessage(MSG* pMsg)
{
	if (m_pMapList && m_LastSelectedMap != -1)
		return m_pMapList[m_LastSelectedMap]->PreTranslateMessage(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

void MapTabManager::OnCancel()
{
	char pBuffer[1024];

	for (int i = 0; i < m_iMapCount; i++)
	{
		CaveMapEditDlg* pDlg = m_pMapList[i];

		if (!pDlg->changes)
			continue;

		// Show 'save changes' box
		sprintf(pBuffer, "Save changes to the map \"%s\" before closing?", pDlg->caveMap.mapInfo.fileName);

		int iResult = MessageBox(pBuffer, "Warning", MB_YESNOCANCEL | MB_ICONEXCLAMATION);

		switch (iResult)
		{
			case IDYES:
			{
				pDlg->OnMapSave();
				break;
			}
			case IDNO:
			{
				break;
			}
			case IDCANCEL:
			{
				return;
			}
		}
	}

	CloseAllMaps();
	CDialog::OnCancel();
}

void MapTabManager::OnClose()
{
	CDialog::OnClose();
}

void MapTabManager::OnDestroy()
{
	CDialog::OnDestroy();
	CloseAllMaps();
}

void MapTabManager::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (pWnd != &m_TabList)
		return;

	m_ContextMenuTabIndex = m_LastSelectedMap;
	CRect itemRect;

	// Get the selected one
	for (int i = 0; i < m_iMapCount; i++)
	{
		m_TabList.GetItemRect(i, &itemRect);
		ClientToScreen(&itemRect);

		if (!PtInRect(&itemRect, point))
			continue;

		m_ContextMenuTabIndex = i;
		break;
	}

	CMenu* pPopup = m_pConMenu.GetSubMenu(0);

	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x + 1, point.y, this);
}

void MapTabManager::OnSelectMapTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	if (m_bIgnore)
		return;

	SetMapTab(m_TabList.GetCurSel());
}

void MapTabManager::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_LastSelectedMap == -1 || !m_pMapList)
		return;

	// Get the desired rect
	CRect rcClient, rcWindow;

	m_pMapList[m_LastSelectedMap]->ShowWindow(SW_SHOWMAXIMIZED);

	m_TabList.GetClientRect(&rcClient);
	m_TabList.AdjustRect(FALSE, &rcClient);

	m_TabList.GetWindowRect(&rcWindow);
	ScreenToClient(rcWindow);

	rcClient.OffsetRect(rcWindow.left, rcWindow.top);

	m_pMapList[m_LastSelectedMap]->SetWindowPos(0, rcClient.left - 5, rcClient.top, rcClient.Width() + 5, rcClient.Height(), SWP_NOZORDER | SWP_NOREPOSITION);
}

void MapTabManager::OnCloseTab()
{
	if (!m_pMapList || m_ContextMenuTabIndex == -1)
		return;

	CloseMap(m_pMapList[m_ContextMenuTabIndex]->mapIndex);
}

void MapTabManager::OnMove(int x, int y)
{
	for (int i = 0; i < m_iMapCount; i++)
		m_pMapList[m_LastSelectedMap]->OnMove(x, y);

	CDialog::OnMove(x, y);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
