// PBMEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "PaletteSelectDlg.h"
#include "ResizeDlg.h"
#include "PaletteEditorDlg.h"

#include "PBMEditorDlg.h"
#include ".\pbmeditordlg.h"


// PBMEditorDlg dialog

IMPLEMENT_DYNAMIC(PBMEditorDlg, CDialog)
PBMEditorDlg::PBMEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PBMEditorDlg::IDD, pParent), palette(this)
{
	title = "PBM Editor";
	close = false;
	lclick = false;
	init = false;
	grid = 0;
	offX = 0;
	offY = 0;
}

PBMEditorDlg::PBMEditorDlg(UINT nTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nTemplate, pParent)
{
	title = "PBM Editor";
	change = false;
	close = false;
	lclick = false;
	init = false;
	offX = 0;
	offY = 0;
}

PBMEditorDlg::~PBMEditorDlg()
{
}

void PBMEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PBMEditorDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_VIEW_16X16GRID, OnView16x16grid)
	ON_COMMAND(ID_VIEW_32X32GRID, OnView32x32grid)
	ON_COMMAND(ID_VIEW_64X64GRID, OnView64x64grid)
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_VIEW_GRIDOFF, OnViewGridoff)
	ON_COMMAND(ID_TOOLS_RESIZE, OnToolsResize)
	ON_COMMAND(ID_TOOLS_EDITPALETTE, OnToolsEditpalette)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// PBMEditorDlg message handlers

void PBMEditorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	displayMap(dc);

}

//displays bitmap
void PBMEditorDlg::displayMap(CDC &dc)
{
	int i, j;
	RECT rect;

	GetClientRect(&rect);

	CDC cdc, cdc2;
	cdc.CreateCompatibleDC(&dc);
	cdc2.CreateCompatibleDC(&dc);

	CBitmap bmp;
	bmp.CreateBitmap(max(pbm.GetWidth(), rect.right), max(pbm.GetHeight(), rect.bottom), 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	CBitmap* pOldBmp;
	pOldBmp = (CBitmap *)(cdc.SelectObject(&bmp));

	CBitmap bmp2;
	bmp2.CreateBitmap(rect.right, rect.bottom, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	CBitmap* pOldBmp2;
	pOldBmp2 = (CBitmap *)(cdc2.SelectObject(&bmp2));

	cdc2.FillSolidRect(&rect, 0);

	rect.right = pbm.GetWidth()*scale + 1;
	rect.bottom = pbm.GetHeight()*scale + 1;
	cdc2.FillSolidRect(&rect, 0xff00ff);

	pbm.DrawPart(0, 0, pbm.GetWidth(), pbm.GetHeight(), &cdc, 0, 0, false);
	cdc2.StretchBlt(-offX*scale, -offY*scale, pbm.GetWidth()*scale, pbm.GetHeight()*scale, &cdc, 0,0, pbm.GetWidth(), pbm.GetHeight(), SRCCOPY);

	CBrush brush;
	if (grid > 0)
	{
		brush.CreateSolidBrush(0xff00ff);
		for (i = 0; i < pbm.GetWidth(); i += grid)
			for (j = 0; j < pbm.GetHeight(); j += grid)
			{
				rect.left = i*scale;
				rect.top = j*scale;
				rect.right = min((i+grid)*scale+1, pbm.GetWidth()*scale+1);
				rect.bottom = min((j+grid)*scale+1, pbm.GetHeight()*scale+1);
				cdc2.FrameRect(&rect, &brush);
			}
		brush.DeleteObject();
	}

	GetClientRect(&rect);

	dc.BitBlt(0, 0, rect.right, rect.bottom, &cdc2, 0,0, SRCCOPY);
	cdc.SelectObject(&pOldBmp);
	cdc2.SelectObject(&pOldBmp2);

	cdc2.DeleteDC();
	cdc.DeleteDC();
}


void PBMEditorDlg::UpdateScrollBars()
{
	if (scrollbars)
		return;

	if (scale == 0)
		scale = 1;

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

	SetScrollRange(SB_HORZ, 0, pbm.GetWidth(), false);
	SetScrollRange(SB_VERT, 0, pbm.GetHeight(), false);

	//image is within window size
	if (pbm.GetWidth()*scale <= rect2.right && pbm.GetHeight()*scale <= rect2.bottom)
	{
		ShowScrollBar(SB_VERT, FALSE);
		ShowScrollBar(SB_HORZ, FALSE);
	}
	else
	//image is taller than window and scrollbar fits on side
	if (pbm.GetWidth()*scale <= rect.right)
	{
		ShowScrollBar(SB_VERT, TRUE);
		ShowScrollBar(SB_HORZ, FALSE);
	}
	else
	//image is wider than window and scrollbar fits on bottom
	if (pbm.GetHeight()*scale <= rect.bottom)
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

	sInfo.nPage = rect.right/scale;
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
	CDC* cdc = GetDC();
	displayMap(*cdc);
	ReleaseDC(cdc);

}

void PBMEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UpdateScrollBars();
}

BOOL PBMEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, TRUE);		  // Set small icon

	SetWindowText(title.GetString());

	init = true;
	scrollbars = false;
	scale = 2;
	GetMenu()->CheckMenuItem(ID_VIEW_GRIDOFF, MF_CHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_16X16GRID, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_32X32GRID, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(ID_VIEW_64X64GRID, MF_UNCHECKED);

	palette.Create(DIALOG_PALETTE_SELECT, this);

	loadPal();

	palette.ShowWindow(SW_SHOW);
	UpdateScrollBars();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void PBMEditorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void PBMEditorDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void PBMEditorDlg::OnFileSave()
{
	if (!pbm.Save())
	{
		MessageBox("Error saving file!");
		return;
	}
	change = false;
}

void PBMEditorDlg::OnFileLoad()
{
	char szFilters[]= "PBM Files|*.pbm|BMP Files|*.bmp||";

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog loadFileDialog(TRUE, "*.pbm", 0, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

	if (loadFileDialog.DoModal() != IDOK)
		return;//return if no file selected

	if (!pbm.Load(loadFileDialog.GetPathName().GetBuffer()))
	{
		MessageBox(loadFileDialog.GetPathName(), "Error loading file");
		return;//return with useless error message if error loading pbm
	};

	scale = 2;
	offX = 0;
	offY = 0;

	if (init)
	{
		loadPal();
		CDC* cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
	}
	change = false;
}


void PBMEditorDlg::OnViewGridoff()
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

void PBMEditorDlg::OnView16x16grid()
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

void PBMEditorDlg::OnView32x32grid()
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

void PBMEditorDlg::OnView64x64grid()
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

BOOL PBMEditorDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest != HTCLIENT)
		return CDialog::OnSetCursor(pWnd, nHitTest, message);

	HCURSOR hCursor = AfxGetApp()->LoadCursor(CURSOR_PIXEL);

	ASSERT(hCursor);
    ::SetCursor(hCursor);
    return TRUE;
}

void PBMEditorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (palette.IsChild(GetFocus()))//I'm not sure why this works instead of what's used in CaveMapEditor
		SetFocus();
	if ((MK_LBUTTON & nFlags) == MK_LBUTTON && lclick)
		OnLButtonDown(nFlags, point);
	CRect rect;

	CDialog::OnMouseMove(nFlags, point);
}

void PBMEditorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect;
	GetClientRect(&rect);

	lclick = true;
	if (rect.PtInRect(point))
	{
		p.x = point.x/scale + offX;
		p.y = point.y/scale + offY;
		if (p.x < pbm.GetWidth() && p.y < pbm.GetHeight())
		{
			if (!pbm.IsIndexed())
			{
				change = true;
				pbm.SetPixel(p.x, p.y, palette.selected);
 				CDC* cdc = GetDC();
				cdc->FillSolidRect((p.x-offX)*scale, (p.y-offY)*scale, scale, scale, palette.selected);
				ReleaseDC(cdc);
			}
			else
			if (pbm.GetPixel(p.x, p.y) != RGBQ(palette.palette[palette.selected]))
			{
				change = true;
				pbm.SetPixel(p.x, p.y, RGBQ(palette.palette[palette.selected]));
 				CDC* cdc = GetDC();
				cdc->FillSolidRect((p.x-offX)*scale, (p.y-offY)*scale, scale, scale, RGBQ(palette.palette[palette.selected]));
				ReleaseDC(cdc);
			}
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void PBMEditorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonUp(nFlags, point);
}

BOOL PBMEditorDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

	ScreenToClient(&pt);

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

void PBMEditorDlg::loadPal()
{
	if (!pbm.IsIndexed())
	{
		GetMenu()->EnableMenuItem(ID_TOOLS_EDITPALETTE, MF_BYCOMMAND | MF_GRAYED);
		palette.loadPal(NULL, -1);
		return;
	}

	GetMenu()->EnableMenuItem(ID_TOOLS_EDITPALETTE, MF_BYCOMMAND | MF_ENABLED);
	RGBQUAD colors[256];
	pbm.GetColorTable(0, min(pbm.GetMaxColorTableEntries(), 256), colors);
	palette.loadPal(colors, min(pbm.GetMaxColorTableEntries(), 256));
}

void PBMEditorDlg::OnToolsResize()
{
	CDC* cdc;
	ResizeDlg resize;
	resize.width = pbm.GetWidth();
	resize.height = pbm.GetHeight();

	if (resize.DoModal() == IDOK)
	{
		pbm.Resize(resize.width, resize.height);
		UpdateScrollBars();
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
	}
}

void PBMEditorDlg::OnToolsEditpalette()
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
		pbm.SetColorTable(0, pe.palSize, pe.palette);
		cdc = GetDC();
		displayMap(*cdc);
		ReleaseDC(cdc);
	}
}

void PBMEditorDlg::OnFileExit()
{
	OnClose();//just like how pressing the [x] works
	OnCancel();
}

void PBMEditorDlg::OnClose()
{
	close = true;

	CDialog::OnClose();
}

void PBMEditorDlg::OnCancel()
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

void PBMEditorDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}
