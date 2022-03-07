// TileSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "General.h"
#include "CaveEditor.h"

#include "CaveMapEditDlg.h"

#include "TileSelectDlg.h"
#include ".\tileselectdlg.h"

//#include "SharedWindows.h"

// TileSelectDlg dialog

IMPLEMENT_DYNAMIC(TileSelectDlg, CDialog)
TileSelectDlg::TileSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TileSelectDlg::IDD, pParent)
{
	selection_number = 0;
	selected = 0;
	tileSet = NULL;
	showTileTypes = false;
	mapEditDialog = (CaveMapEditDlg*)pParent;
}

TileSelectDlg::~TileSelectDlg()
{
}

void TileSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TileSelectDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONUP()
	ON_WM_CLOSE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOVE()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


void TileSelectDlg::loadTiles(CaveTileSet* tSet)
{
	if (CS_DEFAULT_TILESET_SCALE == 2)
		scale = 2;
	else
		scale = 1;
	selected = 0;
	x = 0;
	y = 0;
	x2 = 0;
	y2 = 0;
	tileSet = tSet;
	if (tileSet == NULL)
		return;

	Redraw();
}


void TileSelectDlg::Redraw()
{
	RECT r1, r2;
	GetWindowRect(&r1);//get window coords
	GetClientRect(&r2);//get clent area

	if (r2.bottom != tileSet->tileData.GetHeight()*scale)
	{
  		//now we set client area size of tileSet
		r1.right += tileSet->tileData.GetWidth()*scale - r2.right;
		r1.bottom += tileSet->tileData.GetHeight()*scale - r2.bottom;
		MoveWindow(&r1);
		if (scale == 2)
			return;
	}

	int i, j;
	RECT rect;
	RECT rect2;
	CBrush brush;
	CDC* cdc = GetDC();

	BLENDFUNCTION blend;
	if (tileSet->typeMap.GetBPP() == 32)//should have alpha channel
	{
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.BlendFlags = 0;
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = CS_DEFAULT_TILETYPE_ALPHA;
	}
	else
	{
		blend.AlphaFormat = 0;
		blend.BlendFlags = 0;
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = 255;
	}

	CDC cdc2, cdc3;
	CBitmap* pOldBmp, *pOldBmp2;
	CBitmap bmpFinal, bmpFinal2;

	cdc3.CreateCompatibleDC(cdc);
	bmpFinal2.CreateBitmap(tileSet->tileData.GetWidth(), tileSet->tileData.GetHeight(), 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	pOldBmp2 = (CBitmap *)(cdc3.SelectObject(&bmpFinal2));


	cdc2.CreateCompatibleDC(cdc);
	bmpFinal.CreateBitmap(tileSet->tileData.GetWidth(), tileSet->tileData.GetHeight(), 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	pOldBmp = (CBitmap *)(cdc2.SelectObject(&bmpFinal));

	tileSet->tileData.Draw(cdc3, 0,0,tileSet->tileData.GetWidth(),tileSet->tileData.GetHeight(),0,0, tileSet->tileData.GetWidth(),tileSet->tileData.GetHeight());

	if (showTileTypes)
	{
		for (i = 0; i < tileSet->tileData.GetWidth()/16; i++)
		{
			for (j = 0; j < tileSet->tileNum/(tileSet->tileData.GetWidth()/16); j++)
			{
				tileSet->displayTileType(i+j*16, i*16, j*16, &cdc2);
			}
		}
		tileSet->typeMap.GetBPP();
		if (tileSet->typeMap.GetBPP() == 32)//should have alpha channel
			cdc3.AlphaBlend(0, 0, tileSet->tileData.GetWidth(), tileSet->tileData.GetHeight(), &cdc2, 0, 0, tileSet->tileData.GetWidth(), tileSet->tileData.GetHeight(), blend);
		else
			cdc3.TransparentBlt(0, 0, tileSet->tileData.GetWidth(), tileSet->tileData.GetHeight(), &cdc2, 0, 0, tileSet->tileData.GetWidth(), tileSet->tileData.GetHeight(), 0);
	}

	cdc->StretchBlt(0, 0, tileSet->tileData.GetWidth()*scale, tileSet->tileData.GetHeight()*scale, &cdc3, 0, 0, tileSet->tileData.GetWidth(), tileSet->tileData.GetHeight(), SRCCOPY);

	bmpFinal.DeleteObject();
	bmpFinal2.DeleteObject();


	//first draw tile block
	brush.CreateSolidBrush(0x00ff00);

	rect.left = min(x, x2)*16-1;
	rect.top = min(y, y2)*16-1;
	rect.right = max(x,x2)*16+17;
	rect.bottom = max(y,y2)*16+17;

	rect.left *= scale;
	rect.top *= scale;
	rect.right *= scale;
	rect.bottom *= scale;

	cdc->FrameRect(&rect, &brush);
	brush.DeleteObject();


	cdc2.DeleteDC();
	cdc3.DeleteDC();
	ReleaseDC(cdc);

	rect.left = min(x, x2)*16;
	rect.top = min(y, y2)*16;
	rect.right = max(x,x2)*16+16;
	rect.bottom = max(y,y2)*16+16;

	//now draw selected tiles in Map info box
	cdc = mapEditDialog->m_MapInfo.GetDlgItem(FRAME_TILE_VIEW)->GetDC();	
	cdc3.CreateCompatibleDC(cdc);

	i = max(rect.right-rect.left, rect.bottom-rect.top);
	bmpFinal.CreateBitmap(i, i, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	pOldBmp = (CBitmap *)(cdc3.SelectObject(&bmpFinal));

	mapEditDialog->m_MapInfo.GetDlgItem(FRAME_TILE_VIEW)->GetClientRect(&rect2);
	//draw tiles
	cdc3.FillSolidRect(0, 0, i, i, 0xff00ff);//pink filler
	tileSet->tileData.DrawPart(rect.left,rect.top,rect.right-rect.left, rect.bottom-rect.top, &cdc3, (i/2-(rect.right-rect.left)/2)/1,(i/2-(rect.bottom-rect.top)/2)/1, false);


	//then fix in the box
	cdc->StretchBlt(0, 0, rect2.right, rect2.bottom, &cdc3, 0, 0, i, i, SRCCOPY);

	bmpFinal.DeleteObject();
	cdc3.DeleteDC();
	mapEditDialog->m_MapInfo.GetDlgItem(FRAME_TILE_VIEW)->ReleaseDC(cdc);
}


// TileSelectDlg message handlers
void TileSelectDlg::OnOK()
{
}


void TileSelectDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Redraw();
}

void TileSelectDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	int xtemp = point.x/16/scale;
	int ytemp = point.y/16/scale;

	if (xtemp+ytemp*(tileSet->tileData.GetWidth()/16) < tileSet->tileNum)
	{
		x = xtemp;
		y = ytemp;
		x2 = x;
		y2 = y;
		//selected = x+y*(tileSet->tileData.GetWidth()/16);
		selected = x+y*(tileSet->tileData.GetWidth()/16);
	}

	selection_number++;

	Redraw();
//	CDialog::OnLButtonDown(nFlags, point);
}

void TileSelectDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags == MK_LBUTTON && (point.x/16+point.y/16*(tileSet->tileData.GetWidth()/16))/scale < tileSet->tileNum)
	{
		x2 = point.x/16/scale;
		y2 = point.y/16/scale;

		selected = min(x, x2)+min(y, y2)*(tileSet->tileData.GetWidth()/16);
		Redraw();
	}

	CDialog::OnMouseMove(nFlags, point);
}

void TileSelectDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	int i;
	int j;

	i = min(x, x2);
	j = min(y, y2);

	x2 = max(x, x2);
	y2 = max(y, y2);
	x = i;
	y = j;

	mapEditDialog->Refresh();
	mapEditDialog->redraw = true;
	CDialog::OnLButtonUp(nFlags, point);
}

void TileSelectDlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);

}

void TileSelectDlg::OnCancel()
{
//	CDialog::OnCancel();
}

void TileSelectDlg::OnClose()
{

	ShowWindow(SW_HIDE);
//	CDialog::OnClose();
}

void TileSelectDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
}


BOOL TileSelectDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (CS_DEFAULT_WHEEL_ZOOM)
	{
		int oldScale = scale;
		if (zDelta > 0)
			scale = 2;
		else
			scale = 1;

		if (oldScale != scale)
			Redraw();
	}
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}
