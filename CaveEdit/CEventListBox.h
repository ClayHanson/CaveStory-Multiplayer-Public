#pragma once


// CEventListBox

class CEventListBox : public CListBox
{
	DECLARE_DYNAMIC(CEventListBox)

public:
	CFont mFont;
	CFont mBoldFont;

public:
	CEventListBox();
	virtual ~CEventListBox();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void AppendString(LPCSTR lpszText, bool bold, COLORREF fg);
	void SetEntryHidden(int idx, bool val);
	bool IsEntryHidden(int idx);
	void SetEntryText(int idx, const char* text);
	virtual void OnDestroy();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
};


