// NpcEditorTabBase.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "NpcEditorTabBase.h"
#include "afxdialogex.h"


// NpcEditorTabBase dialog

IMPLEMENT_DYNAMIC(NpcEditorTabBase, CDialogEx)

NpcEditorTabBase::NpcEditorTabBase(CWnd* pParent /*=nullptr*/)
	: CDialogEx(TAB_NPCEDIT_PAGE1, pParent)
{
	m_bDragging = FALSE;
}

NpcEditorTabBase::~NpcEditorTabBase()
{
}

void NpcEditorTabBase::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL NpcEditorTabBase::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Populate first
	PopulateTabPage();

	// save the original size
	GetWindowRect(m_rcOriginalRect);

	// initial scroll position
	m_nScrollPos = 0;
	RecalculateScroll();

	return TRUE;
}

CWnd* NpcEditorTabBase::GetChild(int idx)
{
	CWnd* pChild = this->GetWindow(GW_CHILD);
	int Index = 0;

	while (pChild)
	{
		if (Index == idx)
			return pChild;

		++Index;
		pChild = pChild->GetNextWindow();
	}

	return NULL;
}

int NpcEditorTabBase::GetChildCount()
{
	CWnd* pChild = this->GetWindow(GW_CHILD);
	int Index    = 0;

	while (pChild)
	{
		Index++;
		pChild = pChild->GetNextWindow();
	}

	return Index;
}

void NpcEditorTabBase::RecalculateScroll()
{
}

BEGIN_MESSAGE_MAP(NpcEditorTabBase, CDialogEx)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()


// NpcEditorTabBase message handlers

void NpcEditorTabBase::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nDelta;
	int nMaxPos = m_rcOriginalRect.Height() - m_nCurHeight;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;

		nDelta = min(max(nMaxPos / 20, 5), nMaxPos - m_nScrollPos);
		break;

	case SB_LINEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(max(nMaxPos / 20, 5), m_nScrollPos);
		break;
	case SB_PAGEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(max(nMaxPos / 10, 5), nMaxPos - m_nScrollPos);
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPos;
		break;

	case SB_PAGEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(max(nMaxPos / 10, 5), m_nScrollPos);
		break;

	default:
		return;
	}
	m_nScrollPos += nDelta;
	SetScrollPos(SB_VERT, m_nScrollPos, TRUE);
	ScrollWindow(0, -nDelta);
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void NpcEditorTabBase::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_nCurHeight = cy;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = m_rcOriginalRect.Height();
	si.nPage = cy;
	si.nPos = 0;
	SetScrollInfo(SB_VERT, &si, TRUE);
}


BOOL NpcEditorTabBase::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int nMaxPos = m_rcOriginalRect.Height() - m_nCurHeight;

	if (zDelta < 0)
	{
		if (m_nScrollPos < nMaxPos)
		{
			zDelta = min(max(nMaxPos / 20, 5), nMaxPos - m_nScrollPos);

			m_nScrollPos += zDelta;
			SetScrollPos(SB_VERT, m_nScrollPos, TRUE);
			ScrollWindow(0, -zDelta);
		}
	}
	else
	{
		if (m_nScrollPos > 0)
		{
			zDelta = -min(max(nMaxPos / 20, 5), m_nScrollPos);

			m_nScrollPos += zDelta;
			SetScrollPos(SB_VERT, m_nScrollPos, TRUE);
			ScrollWindow(0, -zDelta);
		}
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void NpcEditorTabBase::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bDragging = TRUE;
	SetCapture();

	m_ptDragPoint = point;

	CDialog::OnLButtonDown(nFlags, point);
}


void NpcEditorTabBase::OnLButtonUp(UINT nFlags, CPoint point)
{
	EndDrag();

	CDialog::OnLButtonUp(nFlags, point);
}


void NpcEditorTabBase::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		int nDelta = m_ptDragPoint.y - point.y;
		m_ptDragPoint = point;

		int nNewPos = m_nScrollPos + nDelta;

		if (nNewPos < 0)
			nNewPos = 0;
		else if (nNewPos > m_rcOriginalRect.Height() - m_nCurHeight)
			nNewPos = m_rcOriginalRect.Height() - m_nCurHeight;

		nDelta = nNewPos - m_nScrollPos;
		m_nScrollPos = nNewPos;

		SetScrollPos(SB_VERT, m_nScrollPos, TRUE);
		ScrollWindow(0, -nDelta);
	}

	CDialog::OnMouseMove(nFlags, point);
}


void NpcEditorTabBase::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);

	EndDrag();
}

void NpcEditorTabBase::EndDrag()
{
	m_bDragging = FALSE;
	ReleaseCapture();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// nID = The ID you want it to have, so you can call GetDlgItem(nId) later.
CButton* NpcEditorTabBase::CreateCheckbox(int nID, GUI_RECT rect, const char* text)
{
	CButton* pCheckBox = new CButton;

	pCheckBox->Create(_T((char*)text), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect(rect.x, rect.y, rect.x + rect.w, rect.y + rect.h), this, nID);

	return pCheckBox;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GUI_RECT NpcEditorTabBase::GetRect()
{
	RECT rct;
	this->GetWindowRect(&rct);

	GUI_RECT ret(rct.left, rct.top, rct.right - rct.left, rct.bottom - rct.top);
	return ret;
}