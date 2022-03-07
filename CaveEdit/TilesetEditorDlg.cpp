// TilesetEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "PaletteSelectDlg.h"
#include "General.h"
#include "TileTypeSelectDlg.h"
#include "ResizeDlg.h"
#include "PaletteEditorDlg.h"

#include "TilesetEditorDlg.h"
#include ".\tileseteditordlg.h"

#include <math.h>

// TilesetEditorDlg dialog

IMPLEMENT_DYNAMIC(TilesetEditorDlg, CDialog)
TilesetEditorDlg::TilesetEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TilesetEditorDlg::IDD, pParent)
{
	close = false;
	lclick = false;//used to prevent "drawing" from double click used to open window
	init = false;
	offX = 0;
	offY = 0;
	grid = 16;
	scale = 1;//I'm getting an odd divide by zero related to this and OnSinze() in the Release version but not the Debug version...
}

TilesetEditorDlg::~TilesetEditorDlg()
{
}

void TilesetEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TilesetEditorDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_VIEW_16X16GRID, OnView16x16grid)
	ON_COMMAND(ID_VIEW_32X32GRID, OnView32x32grid)
	ON_COMMAND(ID_VIEW_64X64GRID, OnView64x64grid)
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_VIEW_GRIDOFF, OnViewGridoff)
	ON_WM_SIZING()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TOOLS_RESIZE, OnToolsResize)
	ON_COMMAND(ID_TOOLS_EDITPALETTE, OnToolsEditpalette)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// TilesetEditorDlg message handlers

void TilesetEditorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	displayMap(dc);

}

//displays bitmap
void TilesetEditorDlg::displayMap(CDC &dc)
{
	int i, j;
	RECT rect, wRect;

	GetClientRect(&wRect);
	GetClientRect(&rect);

	CDC cdc, cdc2;
	cdc.CreateCompatibleDC(&dc);
	cdc2.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateBitmap(rect.right/2, rect.bottom, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	CBitmap* pOldBmp;
	pOldBmp = (CBitmap *)(cdc.SelectObject(&bmp));

	CBitmap bmp2;
	bmp2.CreateBitmap(rect.right, rect.bottom, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	CBitmap* pOldBmp2;
	pOldBmp2 = (CBitmap *)(cdc2.SelectObject(&bmp2));

	cdc2.FillSolidRect(&rect, 0);

	rect.right = tileset.tileData.GetWidth()*scale + 1;
	rect.bottom = tileset.tileData.GetHeight()*scale + 1;
	cdc2.FillSolidRect(&rect, 0xff00ff);

	tileset.tileData.DrawPart(offX, offY, tileset.tileData.GetWidth(), tileset.tileData.GetHeight(), &cdc, 0, 0, false);
	cdc2.StretchBlt(0, 0, tileset.tileData.GetWidth()*scale, tileset.tileData.GetHeight()*scale, &cdc, 0,0, tileset.tileData.GetWidth(), tileset.tileData.GetHeight(), SRCCOPY);

	CBrush brush;
	if (grid > 0)
	{
		brush.CreateSolidBrush(0xff00ff);
		for (i = 0; i < tileset.tileData.GetWidth(); i += grid)
			for (j = 0; j < tileset.tileData.GetHeight(); j += grid)
			{
				rect.left = i*scale-offX*scale;
				rect.top = j*scale-offY*scale;
				rect.right = min((i+grid)*scale+1, tileset.tileData.GetWidth()*scale+1)-offX*scale;
				rect.bottom = min((j+grid)*scale+1, tileset.tileData.GetHeight()*scale+1)-offY*scale;
				cdc2.FrameRect(&rect, &brush);
			}
		brush.DeleteObject();
	}

	//now display tile type info

	rect.left = wRect.right/2+1;
	rect.top = 0;
	rect.right = wRect.right/2+1+tileset.tileData.GetWidth()*scale + 1;
	rect.bottom = tileset.tileData.GetHeight()*scale + 1;
	cdc2.FillSolidRect(&rect, 0xff00ff);
	rect.right--;
	rect.bottom--;
	cdc2.FillSolidRect(&rect, 0x000000);

	for (i = 0; i < tileset.tileData.GetWidth()/16; i++)
		for (j = 0; j < tileset.tileData.GetHeight()/16; j++)
			tileset.displayTileType(i+j*(tileset.tileData.GetWidth()>>4), i*16-offX, j*16-offY, &cdc, false);

	cdc2.StretchBlt(wRect.right/2+1, 0, tileset.tileData.GetWidth()*scale, tileset.tileData.GetHeight()*scale, &cdc, 0,0, tileset.tileData.GetWidth(), tileset.tileData.GetHeight(), SRCCOPY);

	cdc.FillSolidRect(0, 0, wRect.right, wRect.bottom, 0);

	if (grid > 0)
	{
		brush.CreateSolidBrush(0xff00ff);
		for (i = 0; i < tileset.tileData.GetWidth(); i += grid)
			for (j = 0; j < tileset.tileData.GetHeight(); j += grid)
			{
				rect.left = (i-offX)*scale;
				rect.top = (j-offY)*scale;
				rect.right = min((i+grid)*scale+1, tileset.tileData.GetWidth()*scale+1)-offX*scale;
				rect.bottom = min((j+grid)*scale+1, tileset.tileData.GetHeight()*scale+1)-offY*scale;
				cdc.FrameRect(&rect, &brush);
			}

		cdc2.TransparentBlt(wRect.right/2+1, 0, wRect.right/2, wRect.bottom, &cdc, 0, 0, wRect.right/2, wRect.bottom, 0x000000);
		brush.DeleteObject();
	}

//*
	//place a divider so people can see the split
	rect.right = wRect.right/2;
	rect.left = rect.right+1;
	rect.top = 0;
	rect.bottom = wRect.bottom;
	cdc2.FillSolidRect(&rect, 0x00ff00);
//*/

	GetClientRect(&rect);
	dc.BitBlt(0, 0, rect.right, rect.bottom, &cdc2, 0,0, SRCCOPY);
	cdc.SelectObject(&pOldBmp);
	cdc2.SelectObject(&pOldBmp2);

	bmp.DeleteObject();
	bmp2.DeleteObject();
	cdc2.DeleteDC();
	cdc.DeleteDC();

	redraw = true;
}


void TilesetEditorDlg::UpdateScrollBars()
{
	if (scrollbars)
		return;

	//prevent flicker, sort of
	SetRedraw(FALSE);

	scrollbars = true;
	RECT rect, rect2;

	tagSCROLLINFO sInfo;

	ShowScrollBar(SB_VERT);
	ShowScrollBar(SB_HORZ);
	GetClientRect(&rect);

	ShowScrollBar(SB_VERT, FALSE);
	ShowScrollBar(SB_HORZ, FALSE);
	GetClientRect(&rect2);

	SetScrollRange(SB_HORZ, 0, tileset.tileData.GetWidth(), false);
	SetScrollRange(SB_VERT, 0, tileset.tileData.GetHeight(), false);

	//image is within window size
	if (tileset.tileData.GetWidth()*scale <= rect2.right/2 && tileset.tileData.GetHeight()*scale <= rect2.bottom)
	{
		ShowScrollBar(SB_VERT, FALSE);
		ShowScrollBar(SB_HORZ, FALSE);
	}
	else
	//image is taller than window and scrollbar fits on side
	if (tileset.tileData.GetWidth()*scale <= rect.right/2)
	{
		ShowScrollBar(SB_VERT, TRUE);
		ShowScrollBar(SB_HORZ, FALSE);
	}
	else
	//image is wider than window and scrollbar fits on bottom
	if (tileset.tileData.GetHeight()*scale <= rect.bottom)
	{
		ShowScrollBar(SB_VERT, FALSE);
		ShowScrollBar(SB_HORZ, TRUE);
	}
	else
	//image doesn't fit
	{
		ShowScrollBar(SB_VERT, TRUE);
		ShowScrollBar(SB_HORZ, TRUE);
	}

	//make scrollbars pretty
	GetClientRect(&rect);
	sInfo.cbSize = sizeof(sInfo);
	sInfo.fMask = SIF_PAGE;

	sInfo.nPage = rect.right/scale/2;// /2 because of split screen
	SetScrollInfo(SB_HORZ, &sInfo);

	sInfo.nPage = rect.bottom/scale;
	SetScrollInfo(SB_VERT, &sInfo);

	SetScrollPos(SB_VERT, offY);//set scrollbar offsets
	SetScrollPos(SB_HORZ, offX);

	offY = GetScrollPos(SB_VERT);//make sure offsets are where the scrollbars think they are
	offX = GetScrollPos(SB_HORZ);

	SetRedraw(TRUE);
	Invalidate(FALSE);

	scrollbars = false;
/*
	CDC* cdc = GetDC();
	displayMap(*cdc);
	ReleaseDC(cdc);
//*/
}

void TilesetEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UpdateScrollBars();
}

BOOL TilesetEditorDlg::OnInitDialog()
{
	change = false;
	init = true;
	scrollbars = false;
	scale = CS_DEFAULT_SCALE;
	xCursor = -10;
	yCursor = -10;

	CDialog::OnInitDialog();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, TRUE);		  // Set small icon

	GetMenu()->CheckMenuItem(ID_VIEW_GRIDOFF, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_16X16GRID, MF_CHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_32X32GRID, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_64X64GRID, MF_UNCHECKED);

	palette.Create(DIALOG_PALETTE_SELECT, this);

	loadPal();

	palette.ShowWindow(SW_SHOW);
	UpdateScrollBars();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void TilesetEditorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	
	CRect rect;
	GetWindowRect(&rect);

	CDC* cdc;
	switch (nSBCode)
	{
	case SB_LINELEFT:
	case SB_LEFT:
		offX = max(0, offX - rect.right/10);
		SetScrollPos(SB_HORZ, offX);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_PAGELEFT:
		offX = max(0, offX - rect.right/2);
		SetScrollPos(SB_HORZ, offX);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_ENDSCROLL:
		break;
	case SB_PAGERIGHT:
		offX = min(GetScrollLimit(SB_HORZ), offX + rect.right/2);
		SetScrollPos(SB_HORZ, offX);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_LINERIGHT:
	case SB_RIGHT:
		offX = min(GetScrollLimit(SB_HORZ), offX + rect.right/10);
		SetScrollPos(SB_HORZ, offX);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_THUMBPOSITION:
		offX = nPos;
		SetScrollPos(SB_HORZ, offX);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_THUMBTRACK:
		offX = nPos;
		SetScrollPos(SB_HORZ, offX);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
		break;
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void TilesetEditorDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CRect rect;
	GetWindowRect(&rect);

	CDC* cdc;
	switch (nSBCode)
	{
/*
SB_BOTTOM
SB_ENDSCROLL
SB_TOP
*/
	case SB_LINEUP:
//	case SB_LEFT:
		offY = max(0, offY - rect.bottom/10);
		SetScrollPos(SB_VERT, offY);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_PAGEUP:
		offY = max(0, offY - rect.bottom/2);
		SetScrollPos(SB_VERT, offY);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_PAGEDOWN:
		offY = min(GetScrollLimit(SB_VERT), offY + rect.bottom/2);
		SetScrollPos(SB_VERT, offY);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_LINEDOWN:
//	case SB_RIGHT:
		offY = min(GetScrollLimit(SB_VERT), offY + rect.bottom/10);
		SetScrollPos(SB_VERT, offY);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_THUMBPOSITION:
		offY = nPos;
		SetScrollPos(SB_VERT, offY);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
		break;
	case SB_THUMBTRACK:
		offY = nPos;
		SetScrollPos(SB_VERT, offY);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
		break;
	}

	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void TilesetEditorDlg::OnFileSave()
{
	if (!tileset.save())
	{
		MessageBox("Error saving tileset data!");
		return;
	}
	change = false;
}

void TilesetEditorDlg::OnViewGridoff()
{
	grid = 0;
	GetMenu()->CheckMenuItem(ID_VIEW_GRIDOFF, MF_CHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_16X16GRID, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_32X32GRID, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_64X64GRID, MF_UNCHECKED);
	CDC* cdc = GetDC();
	displayMap(*cdc);
	ReleaseDC(cdc);
}

void TilesetEditorDlg::OnView16x16grid()
{
	grid = 16;
	GetMenu()->CheckMenuItem(ID_VIEW_GRIDOFF, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_16X16GRID, MF_CHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_32X32GRID, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_64X64GRID, MF_UNCHECKED);
	CDC* cdc = GetDC();
	displayMap(*cdc);
	ReleaseDC(cdc);
}

void TilesetEditorDlg::OnView32x32grid()
{
	grid = 32;
	GetMenu()->CheckMenuItem(ID_VIEW_GRIDOFF, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_16X16GRID, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_32X32GRID, MF_CHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_64X64GRID, MF_UNCHECKED);
	CDC* cdc = GetDC();
	displayMap(*cdc);
	ReleaseDC(cdc);
}

void TilesetEditorDlg::OnView64x64grid()
{
	grid = 64;
	GetMenu()->CheckMenuItem(ID_VIEW_GRIDOFF, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_16X16GRID, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_32X32GRID, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_64X64GRID, MF_CHECKED);
	CDC* cdc = GetDC();
	displayMap(*cdc);
	ReleaseDC(cdc);
}

BOOL TilesetEditorDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest != HTCLIENT)
		return CDialog::OnSetCursor(pWnd, nHitTest, message);
	
	HCURSOR hCursor = AfxGetApp()->LoadCursor(CURSOR_PIXEL);

	ASSERT(hCursor);
    ::SetCursor(hCursor);
    return TRUE;
}

void TilesetEditorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (palette.IsChild(GetFocus()))//I'm not sure why this works instead of what's used in CaveMapEditor
		SetFocus();

	if ((MK_LBUTTON & nFlags) == MK_LBUTTON && lclick)
		OnLButtonDown(nFlags, point);

	char buffer[32];
	RECT rect, r2;
	CRgn rgn, rgn2;
	CDC* cdc;


	GetClientRect(&rect);
	GetClientRect(&r2);
	if (point.x > rect.right/2)
	{
		point.x = point.x % (rect.right/2);//makes math easier for split screen
		if (point.x > 0)
			point.x--;//accounts for pixel wide divider
	}

	//First draw a custom cursor to show which tile the mouse is over
	cdc = GetDC();
	//first remove old cursor
	if (!redraw)
	{
		rect.left = max((xCursor*16-offX)*scale, 0);
		rect.right = min(((xCursor+1)*16-offX)*scale+1, r2.right/2);
		rect.top = (yCursor*16-offY)*scale;
		rect.bottom = ((yCursor+1)*16-offY)*scale+1;
		rgn.CreateRectRgnIndirect(&rect);
//		cdc->InvertRect(&rect);
		rect.bottom--;
		rect.top++;
		rect.left = max((xCursor*16-offX)*scale+1, 0);
		rect.right = min(((xCursor+1)*16-offX)*scale, r2.right/2);
		rgn2.CreateRectRgnIndirect(&rect);
		rgn.CombineRgn(&rgn, &rgn2, RGN_XOR);
//		cdc->InvertRect(&rect);
		cdc->InvertRgn(&rgn);
		rgn.OffsetRgn(r2.right/2+1,0);
		cdc->InvertRgn(&rgn);
		rgn.DeleteObject();
		rgn2.DeleteObject();
	}

	//then draw new one
	xCursor = (int)floor((point.x)/16.0/scale+offX/16.0);
	yCursor = (int)floor((point.y)/16.0/scale+offY/16.0);

	if (xCursor < tileset.tileData.GetWidth()/16 && yCursor < tileset.tileData.GetHeight()/16)
	{
		sprintf(buffer, "Tileset Editor - index %i", xCursor+ (tileset.tileData.GetWidth()/16)*yCursor);
		SetWindowText(buffer);
	}
	rect.left = max((xCursor*16-offX)*scale, 0);
	rect.right = min(((xCursor+1)*16-offX)*scale+1, r2.right/2);
	rect.top = (yCursor*16-offY)*scale;
	rect.bottom = ((yCursor+1)*16-offY)*scale+1;
	rgn.CreateRectRgnIndirect(&rect);
	rect.bottom--;
	rect.top++;
	rect.left = max((xCursor*16-offX)*scale+1, 0);
	rect.right = min(((xCursor+1)*16-offX)*scale, r2.right/2);
	rgn2.CreateRectRgnIndirect(&rect);
	rgn.CombineRgn(&rgn, &rgn2, RGN_XOR);
	cdc->InvertRgn(&rgn);
	rgn.OffsetRgn(r2.right/2+1,0);
	cdc->InvertRgn(&rgn);
	redraw = false;

	ReleaseDC(cdc);

	CDialog::OnMouseMove(nFlags, point);
}

void TilesetEditorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	lclick = true;
	CRect rect;
	GetClientRect(&rect);

	if (rect.PtInRect(point))
	{
		p.x = point.x/scale + offX;
		p.y = point.y/scale + offY;
		if (p.x < tileset.tileData.GetWidth() && p.y < tileset.tileData.GetHeight())
		{
			if (tileset.tileData.GetPixel(p.x, p.y) != RGBQ(palette.palette[palette.selected]))
			{
				change = true;
				tileset.tileData.SetPixel(p.x, p.y, RGBQ(palette.palette[palette.selected]));
 				CDC* cdc = GetDC();
				displayMap(*cdc);
				ReleaseDC(cdc);
			}
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void TilesetEditorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{

	CDialog::OnLButtonUp(nFlags, point);
}

BOOL TilesetEditorDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	CRect rect;
	int ds = scale;//get old scale

	if (zDelta > 0)//zoom in
	{
		if (scale > 6)//only allow so much zooming
			return CDialog::OnMouseWheel(nFlags, zDelta, pt);;
		scale++;
	}
	else//zoom out
	{
		if (scale == 1)//only allow so much zooming
			return CDialog::OnMouseWheel(nFlags, zDelta, pt);;
		scale--;
	};


	GetClientRect(&rect);

	rect.right /= 2;//split window adjustment

	ScreenToClient(&pt);
	if (pt.x > rect.right)
		pt.x -= rect.right;

	int x, y;
	//make zooming in and out center on mouse location
	x = (rect.right-rect.left)/2;
	y = (rect.bottom-rect.top)/2;

	//force mouse point inside rectangle
	pt.x = min(max(pt.x, rect.left), rect.right);
	pt.y = min(max(pt.y, rect.top), rect.bottom);

	offX += (pt.x-x)/ds;
	offY += (pt.y-y)/ds;

	offX += x/ds-x/scale;
	offY += y/ds-y/scale;


	UpdateScrollBars();

	CDC* cdc;
	cdc = GetDC();
	displayMap(*cdc);
	ReleaseDC(cdc);

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void TilesetEditorDlg::loadPal()
{
/*/
	RGBQUAD colors[256];
	tileset.tileData.GetColorTable(0, min(tileset.tileData.GetMaxColorTableEntries(), 256), colors);
	
	palette.loadPal(colors, min(tileset.tileData.GetMaxColorTableEntries(), 256));

//*/
	if (!tileset.tileData.IsIndexed())
	{
		GetMenu()->EnableMenuItem(ID_TOOLS_EDITPALETTE, MF_BYCOMMAND | MF_GRAYED);
		palette.loadPal(NULL, -1);
		return;
	}

	GetMenu()->EnableMenuItem(ID_TOOLS_EDITPALETTE, MF_BYCOMMAND | MF_ENABLED);
	RGBQUAD colors[256];
	tileset.tileData.GetColorTable(0, min(tileset.tileData.GetMaxColorTableEntries(), 256), colors);
	palette.loadPal(colors, min(tileset.tileData.GetMaxColorTableEntries(), 256));
}





void TilesetEditorDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	if ((pRect->right/2)*2 == pRect->right)
		pRect->right--;//make sure the width is always odd

	// TODO: Add your message handler code here
}

void TilesetEditorDlg::OnContextMenu(CWnd* pWnd, CPoint pt)
{
	RECT rect;
	CPoint point = pt;
	ScreenToClient(&point);//convert to client coords
	GetClientRect(&rect);

	if (point.x > rect.right/2)
	{
		point.x = point.x - (rect.right/2);//makes math easier for split screen
		if (point.x > 0)
			point.x--;//accounts for pixel wide divider
	}

	int x = (int)floor((point.x)/16.0/scale+offX/16.0);
	int y = (int)floor((point.y)/16.0/scale+offY/16.0);

	TileTypeSelectDlg temp;//we're going to use a window as the context menu
	temp.pt = pt;
	temp.tiles_per_row = 16;

	//here's the problem, I can't get the dumb window to close with a click outside the popup
	if (temp.DoModal() == IDOK)
	{
		if (tileset.typData[(y * (tileset.tileData.GetWidth() >> 4)) + x] != temp.tile)
		{
			change = true;
			tileset.typData[(y * (tileset.tileData.GetWidth() >> 4)) + x] = temp.tile;
			CDC* cdc = GetDC();
			displayMap(*cdc);
			ReleaseDC(cdc);
		}
	}
}
void TilesetEditorDlg::OnToolsResize()
{
	CDC* cdc;
	ResizeDlg resize;
	resize.width = tileset.tileNum;
	resize.height = -1;
	resize.wName = "Tile Count";

	if (resize.DoModal() == IDOK)
	{
		tileset.resize(resize.width);
		UpdateScrollBars();
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
	}
}

void TilesetEditorDlg::OnToolsEditpalette()
{
	CDC* cdc;
	PaletteEditorDlg pe;
	for (int i = 0; i < palette.palSize; i++)
	{
		pe.palette[i] = palette.palette[i];
	}
	pe.palSize = palette.palSize;
	if (pe.DoModal() == IDOK)
	{
		palette.loadPal(pe.palette, pe.palSize);
		tileset.tileData.SetColorTable(0, pe.palSize, pe.palette);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
	}
}

void TilesetEditorDlg::OnFileExit()
{
	OnClose();//just like how pressing the [x] works
	OnCancel();
}

void TilesetEditorDlg::OnClose()
{
	close = true;
	CDialog::OnClose();
}

void TilesetEditorDlg::OnCancel()
{
	if (close)// don't allow [Escape]
	{
		close = false;
		if (change)//is there are changes ask if they should be saved
		{
			switch(MessageBox("Do you want to save your changes?",0, MB_YESNOCANCEL))
			{
			case IDYES:
				OnFileSave();
			case IDNO:
				break;
			case IDCANCEL:
				return;
				break;
			}
		}
		CDialog::OnCancel();
	}
}

void TilesetEditorDlg::OnOK()
{
	//CDialog::OnOK();
}
