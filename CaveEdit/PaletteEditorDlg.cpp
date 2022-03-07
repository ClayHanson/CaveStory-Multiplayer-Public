// PaletteEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "PaletteEditorDlg.h"
#include ".\paletteeditordlg.h"

#include "General.h"

#define RGBQ(x) ((COLORREF)(((BYTE)(x.rgbRed)|((WORD)((BYTE)(x.rgbGreen))<<8))|(((DWORD)(BYTE)(x.rgbBlue))<<16)))
#define COLOR_SIZE 16

// PaletteEditorDlg dialog

IMPLEMENT_DYNAMIC(PaletteEditorDlg, CDialog)
PaletteEditorDlg::PaletteEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PaletteEditorDlg::IDD, pParent)
{
	palSize = 0;
	change = false;
}

PaletteEditorDlg::~PaletteEditorDlg()
{
}

void PaletteEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PaletteEditorDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// PaletteEditorDlg message handlers

void PaletteEditorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	height = palSize % 16;
	if (height == 0)
		height = palSize / 16;
	else
		height = palSize / 16 + 1;

	Redraw();
}

void PaletteEditorDlg::Redraw()
{
	int i, j;
	CDC* cdc = GetDC();

	CDC cdc2;
	CBitmap bmpFinal;

	cdc2.CreateCompatibleDC(cdc);
	bmpFinal.CreateBitmap((palSize/height)*COLOR_SIZE+1, height*COLOR_SIZE+1, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	CBitmap* pOldBmp = (CBitmap *)(cdc2.SelectObject(&bmpFinal));

	for (i = 0; i < palSize/height; i++)
		for (j = 0; j < height; j++)
			cdc2.FillSolidRect(i*COLOR_SIZE+1, j*COLOR_SIZE+1, COLOR_SIZE-1, COLOR_SIZE-1, RGBQ(palette[i+(palSize/height)*j]));


	cdc->BitBlt(0, 0, (palSize/height)*COLOR_SIZE+1, height*COLOR_SIZE+1, &cdc2, 0, 0, SRCCOPY);
	bmpFinal.DeleteObject();
	ReleaseDC(cdc);
	cdc2.DeleteDC();
}

void PaletteEditorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	COLORREF ref;
	CColorDialog* color;
	point.x = (point.x/COLOR_SIZE);
	point.y = (point.y/COLOR_SIZE);

	if (point.x + point.y*16 < palSize)
	{
		color = new CColorDialog(RGBQ(palette[point.x + point.y*16]), CC_FULLOPEN);
		if (color->DoModal() == IDOK)
		{
			ref = color->GetColor();//why isn't everything stored the same way?!
			if (ref != RGBQ(palette[point.x + point.y*16]))
				change = true;

			palette[point.x + point.y*16].rgbRed = GetRValue(ref);
			palette[point.x + point.y*16].rgbGreen = GetGValue(ref);
			palette[point.x + point.y*16].rgbBlue = GetBValue(ref);
			Redraw();
		}
		delete color;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void PaletteEditorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CBrush brush;
	CRect rect;
	CDC* cdc;
	rect.left = (point.x/COLOR_SIZE)*COLOR_SIZE;
	rect.right = rect.left + COLOR_SIZE+1;
	rect.top = (point.y/COLOR_SIZE)*COLOR_SIZE;
	rect.bottom = rect.top + COLOR_SIZE+1;

	if (rect.left + rect.top*16 < palSize*COLOR_SIZE)
	{
		Redraw();
		brush.CreateSolidBrush(0xffffff);
		cdc = GetDC();
		cdc->FrameRect(&rect, &brush);
		ReleaseDC(cdc);
		brush.DeleteObject();
	}
	CDialog::OnMouseMove(nFlags, point);
}

void PaletteEditorDlg::OnOK()
{
	if (change)
		CDialog::OnOK();
	else
		CDialog::OnCancel();
}
