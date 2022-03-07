#include "stdafx.h"
#include "resource.h"
#include "CsmpCheckBox.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

extern IStream* CreateStreamOnResource(LPCSTR lpName, LPCSTR lpType);

IMPLEMENT_DYNAMIC(CsmpCheckBox, CDialog)

BEGIN_MESSAGE_MAP(CsmpCheckBox, CDialog)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CsmpCheckBox::CsmpCheckBox(CWnd* pParent) : CDialog(CSMPGUI_CHECKBOX, pParent)
{
	m_bCheckBoxValue = false;
	m_bHover         = false;
}

CsmpCheckBox::~CsmpCheckBox()
{
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CsmpCheckBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define CSM_RGBA(r,g,b,a)								(COLORREF)((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))
BOOL CsmpCheckBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	IStream* pStream = CreateStreamOnResource(MAKEINTRESOURCE(BITMAP_GAME_SHEET), "PNG");
	if (pStream != nullptr)
	{
		m_SpritesheetImage.Load(pStream);
		m_SpritesheetImage.SetHasAlphaChannel(true);
		pStream->Release();
	}

	SetWindowPos(NULL, 0, 0, 12, 12, SWP_NOZORDER | SWP_NOREPOSITION);

	return TRUE;
}

void CsmpCheckBox::OnDestroy()
{
	TRACKMOUSEEVENT tme;
	tme.cbSize      = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags     = TME_LEAVE | TME_CANCEL;
	tme.hwndTrack   = this->GetSafeHwnd();
	tme.dwHoverTime = HOVER_DEFAULT;
	TrackMouseEvent(&tme);

	m_SpritesheetImage.Destroy();
	CDialog::OnDestroy();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CsmpCheckBox::OnOK()
{

}

void CsmpCheckBox::OnCancel()
{

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CsmpCheckBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);
}

void CsmpCheckBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonUp(nFlags, point);

	m_bCheckBoxValue = !m_bCheckBoxValue;
	Invalidate(TRUE);
}

void CsmpCheckBox::OnMouseMove(UINT nFlags, CPoint point)
{
	CDialog::OnMouseMove(nFlags, point);

	if (!m_bHover)
	{
		m_bHover = true;
		TRACKMOUSEEVENT tme;
		tme.cbSize      = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags     = TME_LEAVE;
		tme.hwndTrack   = this->GetSafeHwnd();
		tme.dwHoverTime = HOVER_DEFAULT;
		TrackMouseEvent(&tme);

		Invalidate(TRUE);
	}
}

void CsmpCheckBox::OnMouseHover(UINT nFlags, CPoint point)
{
	CDialog::OnMouseHover(nFlags, point);
}

void CsmpCheckBox::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnMouseLeave();

	m_bHover = false;
	Invalidate(TRUE);
}


UINT CsmpCheckBox::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CsmpCheckBox::Redraw(CDC* cdc, CRect* rect)
{
	// Draw the background color
	cdc->FillSolidRect(0, 0, rect->Width(), rect->Height(), RGB(240, 240, 240));

	// Determine the client rect
	CRect rcCheckBox(64, 0, 76, 12);

	if (m_bHover)				rcCheckBox.MoveToX(rcCheckBox.left + 12);
	if (!IsWindowEnabled())		rcCheckBox.MoveToX(rcCheckBox.left + 24);
	if (m_bCheckBoxValue)		rcCheckBox.MoveToX(rcCheckBox.left + 48);

	// Draw the background image
	m_SpritesheetImage.Draw(*cdc, *rect, rcCheckBox);
}

void CsmpCheckBox::OnPaint()
{
	CRect rect;

	GetWindowRect(&rect);
	ScreenToClient(&rect);

	CPaintDC dc(this);

	CDC cdc;
	cdc.CreateCompatibleDC(&dc);

	CBitmap bmpFinal;
	bmpFinal.CreateBitmap(rect.Width(), rect.Height(), 1, 32, NULL);

	CBitmap* pOldBmp = (CBitmap*)(cdc.SelectObject(&bmpFinal));

	Redraw(&cdc, &rect);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &cdc, 0, 0, SRCCOPY);

	(cdc.SelectObject(&pOldBmp));
	bmpFinal.DeleteObject();
	cdc.DeleteDC();
}

BOOL CsmpCheckBox::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------