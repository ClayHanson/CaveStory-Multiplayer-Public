// CEventListBox.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "CEventListBox.h"

typedef struct EVENTLIST_INFO
{
	CString strText; // text content, default value is _T("")
	COLORREF fgColor;
	char flags;
	EVENTLIST_INFO() // constructor
	{
		strText.Empty();
		flags = 0;
		fgColor = RGB(0, 0, 0);
	}
};

// CEventListBox

IMPLEMENT_DYNAMIC(CEventListBox, CListBox)

CEventListBox::CEventListBox() {
	LOGFONT logFont;

	CFont tmpFont;
	tmpFont.CreatePointFont(96, "Lucida Console");
	tmpFont.GetLogFont(&logFont);
	tmpFont.DeleteObject();

	mFont.CreateFontIndirect(&logFont);
	logFont.lfWeight = FW_BOLD;
	mBoldFont.CreateFontIndirect(&logFont);
}

CEventListBox::~CEventListBox() {
	mFont.DeleteObject();
	mBoldFont.DeleteObject();
}

enum InfoFlags {
	INFO_FLAG_BOLD   = 0b01,
	INFO_FLAG_HIDDEN = 0b10
};

BEGIN_MESSAGE_MAP(CEventListBox, CListBox)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CEventListBox::AppendString(LPCSTR lpszText, bool bold, COLORREF fg) {
	EVENTLIST_INFO* pInfo = new EVENTLIST_INFO;
	pInfo->strText.Format(_T("%s"), lpszText);
	pInfo->flags = 0;
	if (bold)
		pInfo->flags |= INFO_FLAG_BOLD;
	pInfo->fgColor = fg;
	SetItemDataPtr(AddString(pInfo->strText), pInfo);
}

void CEventListBox::SetEntryHidden(int idx, bool val) {
	EVENTLIST_INFO* pListBox = (EVENTLIST_INFO*)GetItemDataPtr(idx);
	if (!pListBox)
		return;

	pListBox->flags = (!val ? pListBox->flags & ~INFO_FLAG_HIDDEN : pListBox->flags | INFO_FLAG_HIDDEN);
}

bool CEventListBox::IsEntryHidden(int idx) {
	EVENTLIST_INFO* pListBox = (EVENTLIST_INFO*)GetItemDataPtr(idx);
	if (!pListBox)
		return false;

	return pListBox->flags & INFO_FLAG_HIDDEN;
}

void CEventListBox::SetEntryText(int idx, const char* text) {
	EVENTLIST_INFO* pListBox = (EVENTLIST_INFO*)GetItemDataPtr(idx);
	if (!pListBox)
		return;

	pListBox->strText = _T(text);
}


// DrawItem virtual function, draw text and color
void CEventListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	// TODO: Add your code to draw the specified item
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);
	EVENTLIST_INFO* pListBox = (EVENTLIST_INFO*)GetItemDataPtr(lpDrawItemStruct->itemID);
	ASSERT(NULL != pListBox);

	if (pListBox->flags & INFO_FLAG_HIDDEN)
		return;

	CDC dc;

	dc.Attach(lpDrawItemStruct->hDC);
	dc.SelectObject(pListBox->flags & INFO_FLAG_BOLD ? &mBoldFont : &mFont);

	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	dc.SetTextColor(pListBox->fgColor);

	// If this item is selected, set the background color 
	// and the text color to appropriate values. Also, erase
	// rect by filling it with the background color.
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
		(lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, RGB(0, 120, 215));

		int r = 255 - GetRValue(pListBox->fgColor);
		int g = 255 - GetGValue(pListBox->fgColor);
		int b = 255 - GetBValue(pListBox->fgColor);

		dc.SetTextColor(RGB(r, g, b));
	}
	else
	{
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, RGB(255, 255, 255));
	}

	lpDrawItemStruct->rcItem.left += 5;
	// Draw the text.

	dc.DrawText(pListBox->strText, pListBox->strText.GetLength(), &lpDrawItemStruct->rcItem,
		DT_WORDBREAK);

	// Reset the background color and the text color back to their
	// original values.
	dc.SetTextColor(crOldTextColor);

	dc.Detach();
}

void CEventListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) {
	// TODO: Add your code to determine the size of specified item
	ASSERT(lpMeasureItemStruct->CtlType == ODT_LISTBOX);
	EVENTLIST_INFO* pListBox = (EVENTLIST_INFO*)GetItemDataPtr(lpMeasureItemStruct->itemID);
	ASSERT(NULL != pListBox);

	CString strText(_T("&#00;"));
	GetText(lpMeasureItemStruct->itemID, strText);
	ASSERT(TRUE != strText.IsEmpty());
	CRect rect;
	GetItemRect(lpMeasureItemStruct->itemID, &rect);

	CDC* pDC = GetDC();
	pDC->SelectObject(pListBox->flags & INFO_FLAG_BOLD ? &mBoldFont : &mFont);
	lpMeasureItemStruct->itemHeight = pDC->DrawText(strText, -1, rect,
		DT_WORDBREAK | DT_CALCRECT);
	ReleaseDC(pDC);
}

void CEventListBox::OnDestroy()
{
	CListBox::OnDestroy();

	// TODO: Add your message handler code here 
	int nCount = GetCount();
	for (int i = 0; i < nCount; i++)
	{
		EVENTLIST_INFO* pList = (EVENTLIST_INFO*)GetItemDataPtr(i);
		delete pList;
		pList = NULL;
	}
}