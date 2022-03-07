#include "stdafx.h"
#include "General.h"
#include "CaveEditor.h"
#include "DlgEntityEditor.h"
#include "afxdialogex.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(DlgEntPropertyEditor, CDialog)

BEGIN_MESSAGE_MAP(DlgEntPropertyEditor, CDialog)
	ON_BN_CLICKED(IDOK,								Clicked_Ok)
	ON_BN_CLICKED(IDCANCEL,							Clicked_Cancel)
	ON_BN_CLICKED(BUTTON_PROP_ADD,					Clicked_Add)
	ON_WM_VSCROLL()
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static DlgEntPropertyEditor* sz_pPropEditorInstance = NULL;

inline char* safe_strcpy(char* pBuffer, const char* pInput, int iMaxSize)
{
	if (strlen(pInput) >= iMaxSize)
	{
		*pBuffer = 0;
		return strncat(pBuffer, pInput, iMaxSize - 1);
	}

	return strcpy(pBuffer, pInput);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DlgEntPropertyEditor::DlgEntPropertyEditor(CWnd* pParent /*=nullptr*/)
	: CDialog(DIALOG_ENTITY_PROPERTIES, pParent)
{
	m_Initialized = false;
	m_Changes     = false;
	m_PropList    = NULL;
	m_PropCount   = 0;
}

DlgEntPropertyEditor::~DlgEntPropertyEditor()
{
	while (m_PropList)
		EntProp_Delete(m_PropList[0].prop->name);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgEntPropertyEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, FRAME_PROP_EDITOR_LIST, m_ListBox);
	DDX_Control(pDX, SCROLL_PROP_EDITOR, m_ScrollBar);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL DlgEntPropertyEditor::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_Initialized = true;

	// Get the minimum size
	RECT rc;
	GetWindowRect(&rc);
	m_iMinSizeX = rc.right - rc.left;
	m_iMinSizeY = rc.bottom - rc.top;

	// Create all property items' GUI entries
	for (int i = 0; i < m_PropCount; i++)
	{
		DLG_ENT_PROPERTY_EDITOR_ITEM* pInfo = &m_PropList[i];

		// Create the dialog
		pInfo->gui_item                 = new DlgItemProperty();
		pInfo->gui_item->m_pEditorOwner = this;
		pInfo->gui_item->m_pProperty    = pInfo->prop;
		pInfo->gui_item->Create(DLGITEM_PROPERTY_ENTRY, &m_ListBox);
		pInfo->gui_item->ShowWindow(SW_SHOW);
		pInfo->gui_item->PopulatePropInfo();
	}

	// Re-calculate the positions of every item
	EntProp_Calculate();

	// Setup this up for the timer.
	sz_pPropEditorInstance = this;

	return TRUE;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DLG_ENT_PROPERTY_EDITOR_ITEM* DlgEntPropertyEditor::EntProp_Get(const char* pName, int* iIndex)
{
	for (int i = 0; i < m_PropCount; i++)
	{
		// Skip mismatches
		if (strcmpi(m_PropList[i].prop->name, pName))
			continue;

		if (iIndex)
			*iIndex = i;

		return &m_PropList[i];
	}

	if (iIndex)
		*iIndex = -1;

	return NULL;
}

void DlgEntPropertyEditor::EntProp_Set(const char* pName, const char* pValue)
{
	DLG_ENT_PROPERTY_EDITOR_ITEM* pInfo = NULL;

	// Check to see if it already exists.
	if ((pInfo = EntProp_Get(pName)) != NULL)
	{
		// Update the value if it already exists
		safe_strcpy(pInfo->prop->value, pValue, MAX_ENTITY_PROPERTY_STRING_LENGTH);
		m_Changes = true;
		return;
	}

	// Allocate the new slot
	if (!m_PropList)
	{
		m_PropCount = 1;
		m_PropList  = (DLG_ENT_PROPERTY_EDITOR_ITEM*)malloc(sizeof(DLG_ENT_PROPERTY_EDITOR_ITEM) * m_PropCount);
	}
	else
	{
		m_PropCount += 1;
		m_PropList   = (DLG_ENT_PROPERTY_EDITOR_ITEM*)realloc((void*)m_PropList, sizeof(DLG_ENT_PROPERTY_EDITOR_ITEM) * m_PropCount);
	}

	// Get the newly allocated item
	pInfo = &m_PropList[m_PropCount - 1];

	// Setup the new information
	pInfo->prop           = new PXCM_ENTITY_CUSTOM_PROPERTY();
	pInfo->prop->name_crc = 0;
	pInfo->gui_item      = (m_Initialized ? new DlgItemProperty() : NULL);

	// Set them
	safe_strcpy(pInfo->prop->name, pName, MAX_ENTITY_PROPERTY_STRING_LENGTH);
	safe_strcpy(pInfo->prop->value, pValue, MAX_ENTITY_PROPERTY_STRING_LENGTH);

	if (!m_Initialized)
		return;

	// Create the dialog
	pInfo->gui_item->m_pEditorOwner = this;
	pInfo->gui_item->m_pProperty    = pInfo->prop;
	pInfo->gui_item->Create(DLGITEM_PROPERTY_ENTRY, &m_ListBox);
	pInfo->gui_item->ShowWindow(SW_SHOW);
	pInfo->gui_item->PopulatePropInfo();

	// There are changes!
	m_Changes = true;

	// Re-calculate the positions of every item
	EntProp_Calculate();
}

void DlgEntPropertyEditor::EntProp_Delete(const char* pName)
{
	DLG_ENT_PROPERTY_EDITOR_ITEM* pInfo = NULL;
	int iIndex                          = 0;

	// Stop if it doesn't exist.
	if ((pInfo = EntProp_Get(pName, &iIndex)) == NULL)
		return;

	// Free the memory that this is taking up.
	if (pInfo->gui_item)
	{
		pInfo->gui_item->DestroyWindow();
		delete pInfo->gui_item;
	}

	delete pInfo->prop;

	// There are changes!
	m_Changes = true;

	// Then, determine what to do with the property list after this.
	if (m_PropCount == 1)
	{
		// Delete the entire propery list
		free(m_PropList);

		// Clear it out
		m_PropList  = NULL;
		m_PropCount = 0;
	}
	else
	{
		// Remove it from the list
		LIST_ERASE(m_PropList, m_PropCount, DLG_ENT_PROPERTY_EDITOR_ITEM, iIndex);

		// Now resize the list to be shorter
		m_PropCount -= 1;
		m_PropList   = (DLG_ENT_PROPERTY_EDITOR_ITEM*)realloc((void*)m_PropList, sizeof(DLG_ENT_PROPERTY_EDITOR_ITEM) * m_PropCount);
	}

	if (m_Initialized)
		EntProp_Calculate();
}

void DlgEntPropertyEditor::EntProp_MoveUp(const char* pName)
{
	DLG_ENT_PROPERTY_EDITOR_ITEM* pInfo = NULL;
	int iIndex                          = 0;

	// Stop if it doesn't exist.
	if ((pInfo = EntProp_Get(pName, &iIndex)) == NULL)
		return;

	// Do not move it up if it is already at the top of the list.
	if (!iIndex)
		return;

	// There are changes!
	m_Changes = true;

	DLG_ENT_PROPERTY_EDITOR_ITEM pThisEntry;
	DLG_ENT_PROPERTY_EDITOR_ITEM pEntry_Up;

	// Gather the memory
	memcpy(&pThisEntry, pInfo,					 sizeof(DLG_ENT_PROPERTY_EDITOR_ITEM));
	memcpy(&pEntry_Up,  &m_PropList[iIndex - 1], sizeof(DLG_ENT_PROPERTY_EDITOR_ITEM));

	// Swap them around
	memcpy(&m_PropList[iIndex],		&pEntry_Up,		sizeof(DLG_ENT_PROPERTY_EDITOR_ITEM));
	memcpy(&m_PropList[iIndex - 1],	&pThisEntry,	sizeof(DLG_ENT_PROPERTY_EDITOR_ITEM));

	EntProp_Calculate();
}

void DlgEntPropertyEditor::EntProp_MoveDown(const char* pName)
{
	DLG_ENT_PROPERTY_EDITOR_ITEM* pInfo = NULL;
	int iIndex                          = 0;

	// Stop if it doesn't exist.
	if ((pInfo = EntProp_Get(pName, &iIndex)) == NULL)
		return;

	// Do not move it up if it is already at the top of the list.
	if (iIndex >= m_PropCount - 1)
		return;

	// There are changes!
	m_Changes = true;

	DLG_ENT_PROPERTY_EDITOR_ITEM pThisEntry;
	DLG_ENT_PROPERTY_EDITOR_ITEM pEntry_Down;

	// Gather the memory
	memcpy(&pThisEntry,		pInfo,					 sizeof(DLG_ENT_PROPERTY_EDITOR_ITEM));
	memcpy(&pEntry_Down,	&m_PropList[iIndex + 1], sizeof(DLG_ENT_PROPERTY_EDITOR_ITEM));

	// Swap them around
	memcpy(&m_PropList[iIndex],		&pEntry_Down,	sizeof(DLG_ENT_PROPERTY_EDITOR_ITEM));
	memcpy(&m_PropList[iIndex + 1],	&pThisEntry,	sizeof(DLG_ENT_PROPERTY_EDITOR_ITEM));

	EntProp_Calculate();
}

void DlgEntPropertyEditor::EntProp_Calculate()
{
	if (!m_PropCount)
		return;

	RECT ListRect;
	RECT ItemRect;

	// Get the desired rect
	m_ListBox.GetWindowRect(&ListRect);
	ScreenToClient(&ListRect);

	int iEntryWidth = RECT_WIDTH(ListRect) - 4;

	// Calculate the first item
	m_PropList[0].gui_item->ResizeElements(iEntryWidth);
	m_PropList[0].gui_item->GetWindowRect(&ItemRect);
	m_ListBox.ScreenToClient(&ItemRect);

	int iEntryHeight  = RECT_HEIGHT(ItemRect);
	int iEntryYOffset = -m_ScrollBar.GetScrollPos();
	
	for (int i = 0; i < m_PropCount; i++)
	{
		DLG_ENT_PROPERTY_EDITOR_ITEM* pInfo = &m_PropList[i];

		pInfo->gui_item->m_MoveUp.EnableWindow(i == 0 ? FALSE : TRUE);
		pInfo->gui_item->m_MoveDown.EnableWindow(i == m_PropCount - 1 ? FALSE : TRUE);

		// Set the new rect
		pInfo->gui_item->ResizeElements(iEntryWidth);
		pInfo->gui_item->SetWindowPos((i == 0 ? NULL : m_PropList[i - 1].gui_item), 0, iEntryYOffset, 0, 0, SWP_NOREPOSITION | SWP_NOSIZE);

		// Add pos for the next item
		iEntryYOffset += iEntryHeight;
	}

	m_ScrollBar.SetScrollRange(0, max(((iEntryYOffset + m_ScrollBar.GetScrollPos()) + iEntryHeight) - RECT_HEIGHT(ListRect), 0));
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgEntPropertyEditor::Clicked_Ok()
{
	CDialog::OnOK();
}

void DlgEntPropertyEditor::Clicked_Cancel()
{
	CDialog::OnCancel();
}

void DlgEntPropertyEditor::Clicked_Add()
{
	char pNewBuffer[256];
	int iTry = 0;

	while (true)
	{
		sprintf(pNewBuffer, "NEW_PROPERTY_%d", iTry++);

		if (EntProp_Get(pNewBuffer) == NULL)
			break;
	}

	EntProp_Set(pNewBuffer, "");
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgEntPropertyEditor::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int CurPos = pScrollBar->GetScrollPos();

	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		CurPos = 0;
		break;

	case SB_RIGHT:      // Scroll to far right.
		CurPos = 100;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if (CurPos > 0)
			CurPos--;
		break;

	case SB_LINERIGHT:   // Scroll right.
		if (CurPos < 100)
			CurPos++;
		break;

	case SB_PAGELEFT:    // Scroll one page left.
	{
		// Get the page size. 
		SCROLLINFO   info;
		m_ScrollBar.GetScrollInfo(&info, SIF_ALL);

		if (CurPos > 0)
			CurPos = max(0, CurPos - (int)info.nPage);
	}
	break;

	case SB_PAGERIGHT:      // Scroll one page right
	{
		// Get the page size. 
		SCROLLINFO   info;
		m_ScrollBar.GetScrollInfo(&info, SIF_ALL);

		if (CurPos < 100)
			CurPos = min(100, CurPos + (int)info.nPage);
	}
	break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		CurPos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		CurPos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	m_ScrollBar.SetScrollPos(CurPos);  //orignally it was CurPos

	EntProp_Calculate();

	CDialog::OnVScroll(nSBCode, CurPos, pScrollBar);
}


void DlgEntPropertyEditor::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CDialog::OnGetMinMaxInfo(lpMMI);

	lpMMI->ptMinTrackSize.x = m_iMinSizeX;
	lpMMI->ptMinTrackSize.y = m_iMinSizeY;
}

void DlgEntPropertyEditor::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	for (int i = 0; i < m_PropCount; i++)
	{
		m_PropList[i].gui_item->DestroyWindow();
		delete m_PropList[i].gui_item;
		m_PropList[i].gui_item = NULL;
	}

	m_Initialized = false;
}
