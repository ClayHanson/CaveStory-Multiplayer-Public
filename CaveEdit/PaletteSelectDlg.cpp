// PaletteSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "PaletteSelectDlg.h"
#include ".\paletteselectdlg.h"

#include "General.h"

//used to convert RGBQUAD to COLORREF
#define RGBQ(x) ((COLORREF)(((BYTE)(x.rgbRed)|((WORD)((BYTE)(x.rgbGreen))<<8))|(((DWORD)(BYTE)(x.rgbBlue))<<16)))

#define COLOR_SIZE 12

// PaletteSelectDlg dialog

IMPLEMENT_DYNAMIC(PaletteSelectDlg, CDialog)
PaletteSelectDlg::PaletteSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PaletteSelectDlg::IDD, pParent)
{
	selected = 0;
	palSize = 0;
}

PaletteSelectDlg::~PaletteSelectDlg()
{
}

void PaletteSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PaletteSelectDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(NM_CUSTOMDRAW, SLIDER_RED, OnNMCustomdrawRed)
	ON_NOTIFY(NM_CUSTOMDRAW, SLIDER_GREEN, OnNMCustomdrawGreen)
	ON_NOTIFY(NM_CUSTOMDRAW, SLIDER_BLUE, OnNMCustomdrawBlue)
END_MESSAGE_MAP()


// PaletteSelectDlg message handlers
BOOL PaletteSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	RECT r1, r2;
	GetWindowRect(&r1);//get window coords
	GetClientRect(&r2);//get clent area
	((CSliderCtrl*)GetDlgItem(SLIDER_RED))->SetRange(0, 255);
	((CSliderCtrl*)GetDlgItem(SLIDER_GREEN))->SetRange(0, 255);
	((CSliderCtrl*)GetDlgItem(SLIDER_BLUE))->SetRange(0, 255);
	selected = 0;

	//now we set client area to 64 x 64
//	r1.right += 64 - r2.right;
//	r1.bottom += 64 - r2.bottom;
//	MoveWindow(&r1);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void PaletteSelectDlg::loadPal(RGBQUAD* pal, int count)
{
	RECT r1, r2;

	palSize = count;
	GetWindowRect(&r1);
	GetClientRect(&r2);
	selected = 0;

	if (palSize == -1)
	{
		GetDlgItem(TEXT_INDEX)->ShowWindow(SW_HIDE);
		GetDlgItem(TEXT_RED)->ShowWindow(SW_HIDE);
		GetDlgItem(TEXT_GREEN)->ShowWindow(SW_HIDE);
		GetDlgItem(TEXT_BLUE)->ShowWindow(SW_HIDE);

		GetDlgItem(TEXT_RED2)->ShowWindow(SW_SHOW);
		GetDlgItem(TEXT_GREEN2)->ShowWindow(SW_SHOW);
		GetDlgItem(TEXT_BLUE2)->ShowWindow(SW_SHOW);
		GetDlgItem(SLIDER_RED)->ShowWindow(SW_SHOW);
		GetDlgItem(SLIDER_GREEN)->ShowWindow(SW_SHOW);
		GetDlgItem(SLIDER_BLUE)->ShowWindow(SW_SHOW);
		GetDlgItem(FRAME_COLOR)->ShowWindow(SW_SHOW);

		selected = RGB(((CSliderCtrl*)GetDlgItem(SLIDER_RED))->GetPos(), ((CSliderCtrl*)GetDlgItem(SLIDER_GREEN))->GetPos(), ((CSliderCtrl*)GetDlgItem(SLIDER_BLUE))->GetPos());

		r1.right -= r2.right;
		r1.bottom -= r2.bottom;
		GetDlgItem(FRAME_24BIT)->GetClientRect(&r2);
		r1.right += r2.right;
		r1.bottom +=r2.bottom;
		MoveWindow(&r1, FALSE);
		Invalidate();
		return;
	}

	GetDlgItem(TEXT_INDEX)->ShowWindow(SW_SHOW);
	GetDlgItem(TEXT_RED)->ShowWindow(SW_SHOW);
	GetDlgItem(TEXT_GREEN)->ShowWindow(SW_SHOW);
	GetDlgItem(TEXT_BLUE)->ShowWindow(SW_SHOW);

	GetDlgItem(TEXT_RED2)->ShowWindow(SW_HIDE);
	GetDlgItem(TEXT_GREEN2)->ShowWindow(SW_HIDE);
	GetDlgItem(TEXT_BLUE2)->ShowWindow(SW_HIDE);
	GetDlgItem(SLIDER_RED)->ShowWindow(SW_HIDE);
	GetDlgItem(SLIDER_GREEN)->ShowWindow(SW_HIDE);
	GetDlgItem(SLIDER_BLUE)->ShowWindow(SW_HIDE);
	GetDlgItem(FRAME_COLOR)->ShowWindow(SW_HIDE);

	for (int i = 0; i < count; i++)
		palette[i] = pal[i];

	switch (palSize)
	{
	case 2:
		height = 1;
		break;
	case 4:
		height = 1;
		break;
	case 8:
		height = 2;
		break;
	case 16:
		height = 4;
		break;
	case 256:
		height = 16;
		break;
	default:
		height = 8;
		break;
	};
	r1.right += (palSize/height)*COLOR_SIZE - r2.right+64;
	r1.bottom += max(height*COLOR_SIZE+1, 64) - r2.bottom;
	MoveWindow(&r1);

	GetClientRect(&r1);
	GetDlgItem(TEXT_INDEX)->GetWindowRect(&r2);
	ScreenToClient(&r2);
	r2.left = r1.right - 60;
	r2.right = r1.right;
	GetDlgItem(TEXT_INDEX)->MoveWindow(&r2);
	GetDlgItem(TEXT_RED)->GetWindowRect(&r2);
	ScreenToClient(&r2);
	r2.left = r1.right - 60;
	r2.right = r1.right;
	GetDlgItem(TEXT_RED)->MoveWindow(&r2);
	GetDlgItem(TEXT_GREEN)->GetWindowRect(&r2);
	ScreenToClient(&r2);
	r2.left = r1.right - 60;
	r2.right = r1.right;
	GetDlgItem(TEXT_GREEN)->MoveWindow(&r2);
	GetDlgItem(TEXT_BLUE)->GetWindowRect(&r2);
	ScreenToClient(&r2);
	r2.left = r1.right - 60;
	r2.right = r1.right;
	GetDlgItem(TEXT_BLUE)->MoveWindow(&r2);

	Redraw();
}

void PaletteSelectDlg::OnOK()
{
}


void PaletteSelectDlg::OnCancel()
{
}

void PaletteSelectDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (palSize != -1)
		Redraw();
	else
	{
		RECT rect;
		GetDlgItem(FRAME_COLOR)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		dc.FillSolidRect(&rect, selected);
	}
}

void PaletteSelectDlg::Redraw()
{
	char buffer[16];
	int i, j;
	CDC* cdc = GetDC();

	CDC cdc2;
	CBitmap bmpFinal;

	cdc2.CreateCompatibleDC(cdc);
	bmpFinal.CreateBitmap((palSize/height)*COLOR_SIZE+1, height*COLOR_SIZE+1, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	CBitmap* pOldBmp = (CBitmap *)(cdc2.SelectObject(&bmpFinal));

	i = selected % (palSize/height);
	j = selected / (palSize/height);
	cdc2.FillSolidRect(i*COLOR_SIZE, j*COLOR_SIZE, COLOR_SIZE+1, COLOR_SIZE+1, 0xffffff);

	for (i = 0; i < palSize/height; i++)
		for (j = 0; j < height; j++)
			cdc2.FillSolidRect(i*COLOR_SIZE+1, j*COLOR_SIZE+1, COLOR_SIZE-1, COLOR_SIZE-1, RGBQ(palette[i+(palSize/height)*j]));

	cdc->BitBlt(0, 0, (palSize/height)*COLOR_SIZE+1, height*COLOR_SIZE+1, &cdc2, 0, 0, SRCCOPY);
	bmpFinal.DeleteObject();
	ReleaseDC(cdc);

	sprintf(buffer, "Index: %i", selected);
	SetDlgItemText(TEXT_INDEX, buffer);
	sprintf(buffer, "Red: %i", palette[selected].rgbRed);
	SetDlgItemText(TEXT_RED, buffer);
	sprintf(buffer, "Green: %i", palette[selected].rgbGreen);
	SetDlgItemText(TEXT_GREEN, buffer);
	sprintf(buffer, "Blue: %i", palette[selected].rgbBlue);
	SetDlgItemText(TEXT_BLUE, buffer);
	cdc2.DeleteDC();
}

void PaletteSelectDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	RECT rect;
	if (palSize != -1)
	{
		GetClientRect(&rect);
		if (point.x <= rect.right-64 && point.y < height*COLOR_SIZE)
		{
			selected = (point.x/COLOR_SIZE)+((palSize/height))*(point.y/COLOR_SIZE);

			Redraw();
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void PaletteSelectDlg::OnNMCustomdrawRed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	selected = (selected&0xFFFF00)+((CSliderCtrl*)GetDlgItem(SLIDER_RED))->GetPos();
	CDC* cdc = GetDC();
	RECT rect;
	GetDlgItem(FRAME_COLOR)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	cdc->FillSolidRect(&rect, selected);
	ReleaseDC(cdc);
	*pResult = 0;
}

void PaletteSelectDlg::OnNMCustomdrawGreen(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	selected = (selected&0xFF00FF)+(((CSliderCtrl*)GetDlgItem(SLIDER_GREEN))->GetPos()<<8);
	CDC* cdc = GetDC();
	RECT rect;
	GetDlgItem(FRAME_COLOR)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	cdc->FillSolidRect(&rect, selected);
	ReleaseDC(cdc);
	*pResult = 0;
}

void PaletteSelectDlg::OnNMCustomdrawBlue(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	selected = (selected&0x00FFFF)+(((CSliderCtrl*)GetDlgItem(SLIDER_BLUE))->GetPos()<<16);
	CDC* cdc = GetDC();
	RECT rect;
	GetDlgItem(FRAME_COLOR)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	cdc->FillSolidRect(&rect, selected);
	ReleaseDC(cdc);
	*pResult = 0;
}
