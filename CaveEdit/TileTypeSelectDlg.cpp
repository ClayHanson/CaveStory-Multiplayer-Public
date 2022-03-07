// TileTypeSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "TileTypeSelectDlg.h"
#include ".\tiletypeselectdlg.h"

#include "General.h"

// TileTypeSelectDlg dialog

IMPLEMENT_DYNAMIC(TileTypeSelectDlg, CDialog)
TileTypeSelectDlg::TileTypeSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TileTypeSelectDlg::IDD, pParent)
{
	tiles_per_row = 0;
	tile = -1;
}

TileTypeSelectDlg::~TileTypeSelectDlg()
{
}

void TileTypeSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TileTypeSelectDlg, CDialog)
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()


// TileTypeSelectDlg message handlers

void TileTypeSelectDlg::OnCancel()
{
	CDialog::OnCancel();//allow [Escape]
}

void TileTypeSelectDlg::OnOK()
{
//	CDialog::OnOK();//don't allow [Enter]
}

void TileTypeSelectDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	tile = point.x/tiles_per_row +(point.y/ tiles_per_row)*16;

	CDialog::OnLButtonUp(nFlags, point);
	CDialog::OnOK();
}

void TileTypeSelectDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CDC cdc;
	cdc.CreateCompatibleDC(&dc);

	BLENDFUNCTION blend;
	if (CS_GRAPHIC_COLOR_DEPTH == 32)
		blend.AlphaFormat = AC_SRC_ALPHA;
	else
		blend.AlphaFormat = 0;
	blend.BlendFlags = 0;
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;

	CBitmap bmp;
	bmp.CreateBitmap(256, 256, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	CBitmap* pOldBmp;
	pOldBmp = (CBitmap *)(cdc.SelectObject(&bmp));

	cdc.FillSolidRect(0, 0, 256, 256, 0x00);
	CaveTileSet::typeMap.DrawPart(0, 0, 256, 256, &cdc, 0, 0);

	dc.FillSolidRect(0, 0, 256, 256, 0x00);
	dc.AlphaBlend(0, 0, 256, 256, &cdc, 0, 0, 256, 256, blend);

	cdc.SelectObject(pOldBmp);

	bmp.DeleteObject();
	cdc.DeleteDC();

}

BOOL TileTypeSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	MoveWindow(pt.x, pt.y, 256, 256);//fixed size

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void TileTypeSelectDlg::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);
	if (tile == -1)
		CDialog::OnCancel();//just like a context menu! sort of
}

void TileTypeSelectDlg::OnCaptureChanged(CWnd *pWnd)
{
	CDialog::OnCancel();//just like a context menu! sort of

	CDialog::OnCaptureChanged(pWnd);
}

