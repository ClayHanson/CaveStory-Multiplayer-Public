#pragma once

#include "DlgScrollable.h"
#include "ModConfigResource.h"

// NpcEditorTab_Page1 dialog
class EntityINFO;


struct GUI_POINT
{
	int x;
	int y;

	GUI_POINT() { x = 0; y = 0; }
	GUI_POINT(int x, int y) : x(x), y(y) { }
};

struct GUI_RECT
{
	int x;
	int y;
	int w;
	int h;

	GUI_RECT() { x = 0; y = 0; w = 0; h = 0; }
	GUI_RECT(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) { }
	GUI_RECT(GUI_POINT pos, int w, int h) : x(pos.x), y(pos.y), w(w), h(h) { }
	GUI_RECT(int x, int y, GUI_POINT ext) : x(x), y(y), w(ext.x), h(ext.y) { }
	GUI_RECT(GUI_POINT pos, GUI_POINT ext) : x(pos.x), y(pos.y), w(ext.x), h(ext.y) { }
};

class NpcEditorTabBase : public CDialogEx
{
	DECLARE_DYNAMIC(NpcEditorTabBase)

public:
	NpcEditorTabBase(CWnd* pParent = nullptr);   // standard constructor
	virtual ~NpcEditorTabBase();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_NPCEDIT_BASE };
#endif

protected:
	// dialog size as you see in the resource view (original size)
	CRect	m_rcOriginalRect;

	// dragging
	BOOL	m_bDragging;
	CPoint	m_ptDragPoint;

	// actual scroll position
	int		m_nScrollPos;

	// actual dialog height
	int		m_nCurHeight;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

public:
	CWnd* GetChild(int idx);
	int GetChildCount();

public:
	virtual void PopulateTabPage() {}
	virtual void SaveTabPage(PXMOD_NPC_STRUCT* ent, int act_no) {}
	virtual void DisplayRecord(PXMOD_NPC_STRUCT* ent, int act_no) {}

public:
	void RecalculateScroll();
	void EndDrag();

protected: // GUI Creation stuff
	CButton* CreateCheckbox(int nID, GUI_RECT rect, const char* text);

public: // Helper functions
	GUI_RECT GetRect();

public:

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};
