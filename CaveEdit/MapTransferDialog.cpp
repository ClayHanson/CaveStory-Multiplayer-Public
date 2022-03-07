#include "stdafx.h"
#include "CaveEditor.h"
#include "CaveMapEditDlg.h"
#include "MapTransferDialog.h"
#include "afxdialogex.h"
#include "EventSelectorDialog.h"

IMPLEMENT_DYNAMIC(MapTransferDialog, CDialog)

MapTransferDialog::MapTransferDialog(CWnd* pParent) : CDialog(MapTransferDialog::IDD, pParent) {
	// Init the old values
	mOldValues.xpos   = 0;
	mOldValues.ypos   = 0;
	mOldValues.eveno  = 90;
	mOldValues.mapidx = 0;
	redraw            = true;
	mDragging         = false;
	offX              = 0;
	offY              = 0;
	scale             = 1;
	mwheel            = false;
	lmb               = false;
}

MapTransferDialog::~MapTransferDialog() {
	if (bmpMem[0].m_hObject != NULL)
		bmpMem[0].DeleteObject();
	if (bmpMem[1].m_hObject != NULL)
		bmpMem[1].DeleteObject();
	if (bmpMem[2].m_hObject != NULL)
		bmpMem[2].DeleteObject();
	if (bmpMem[3].m_hObject != NULL)
		bmpMem[3].DeleteObject();
	if (bmpMem[4].m_hObject != NULL)
		bmpMem[4].DeleteObject();
}

BEGIN_MESSAGE_MAP(MapTransferDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(BUTTON_MS_OK, &MapTransferDialog::OnBnClickedMsOk)
	ON_BN_CLICKED(BUTTON_MS_CANCEL, &MapTransferDialog::OnBnClickedMsCancel)
	ON_CBN_SELCHANGE(DROPLIST_MS_MAP, &MapTransferDialog::OnCbnSelchangeMsMap)
	ON_BN_CLICKED(BUTTON_MS_EVENTVIEW, &MapTransferDialog::OnBnClickedMsEventview)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_EN_CHANGE(INPUT_MS_XPOS, &MapTransferDialog::OnEnChangeMsXpos)
	ON_EN_CHANGE(INPUT_MS_YPOS, &MapTransferDialog::OnEnChangeMsYpos)
END_MESSAGE_MAP()

void MapTransferDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, BITMAP_MS_MAP_PREVIEW, mViewFrame);
	DDX_Control(pDX, DROPLIST_MS_MAP, mMapList);
}

BOOL MapTransferDialog::OnInitDialog() {
	if (!CDialog::OnInitDialog())
		return FALSE;

	mCursor_Arrow = ::LoadCursor(NULL, IDC_ARROW);
	mCursor_Move  = ::LoadCursor(NULL, IDC_SIZEALL);

	npcTable.reload_current_mod_npc_table();

	char buffer[1024];
	RECT rect;

	if (mOldValues.mapidx >= exe.mod.mStages.count)
		mOldValues.mapidx = exe.mod.mStages.count - 1;

	// Populate the map list
	mMapList.Clear();
	for (int i = 0; i < exe.mod.mStages.count; i++)
	{
		// Format it correctly
		if (strcmp(exe.mod.mStages.list[i].file, "\\empty") == 0)
			sprintf(buffer, "[%.3i] ···No Map···", i);
		else
			sprintf(buffer, "[%.3i] %s (%s)", i, exe.mod.mStages.list[i].name, exe.mod.mStages.list[i].file);

		// Add it
		mMapList.AddString(buffer);
	}

	// Create the view
	mViewFrame.GetClientRect(&rect);

	view.Create("", WS_CHILD | WS_VISIBLE | SS_BITMAP, rect, &mViewFrame);
	view.ShowWindow(SW_SHOW);

	// Set the current map
	int x      = mOldValues.xpos;
	int y      = mOldValues.ypos;
	int mapidx = mOldValues.mapidx;

	mOldValues.xpos   = 0;
	mOldValues.ypos   = 0;
	mOldValues.mapidx = 0;
	
	SetSelectedMap(mapidx);
	SetSelectedPos(x, y);

	mMapList.SetCurSel(mapidx);

	sprintf(buffer, "%d", mOldValues.eveno);
	SetDlgItemText(INPUT_MS_EVENT, buffer);
	return TRUE;
}

void MapTransferDialog::OnPaint() {
	CPaintDC dc(this); // device context for painting
	RedrawMap(dc);
	CDialog::OnPaint();
}

CRect MapTransferDialog::GetViewPort() {
	CRect rect;
	mViewFrame.GetWindowRect(&rect);
	ScreenToClient(&rect);

	CRect ret = { offX, offY, offX + (rect.right - rect.left), offY + (rect.bottom - rect.top) };
	return ret;
}

void MapTransferDialog::DrawSelection() {
	// Clear the old cursor
	CRgn rgn, rgn2, rgn3;
	CDC* cdc = view.GetDC();

	// Draw the cursor selection
	if (!redraw) {
		RECT rect = mSelRect;
		rect.left   -= offX;
		rect.right  -= offX;
		rect.top    -= offY;
		rect.bottom -= offY;

		rgn.CreateRectRgnIndirect(&rect);
		rect.bottom--;
		rect.top++;
		rect.left++;
		rect.right--;
		rgn2.CreateRectRgnIndirect(&rect);
		rgn.CombineRgn(&rgn, &rgn2, RGN_XOR);

		cdc->InvertRgn(&rgn);
		rgn.DeleteObject();
		rgn2.DeleteObject();
	}

	// Set the selection rect
	mSelRect.left   = mOldValues.xpos * 16;
	mSelRect.top    = mOldValues.ypos * 16;
	mSelRect.right  = mSelRect.left + 16;
	mSelRect.bottom = mSelRect.top + 16;

	// Draw the new cursor
	RECT rect = mSelRect;
	rect.left   -= offX;
	rect.right  -= offX;
	rect.top    -= offY;
	rect.bottom -= offY;

	rgn.CreateRectRgnIndirect(&rect);

	rect.bottom--;
	rect.top++;
	rect.left++;
	rect.right--;
	rgn2.CreateRectRgnIndirect(&rect);

	//this makes a 1 pixel thick rectangle
	rgn.CombineRgn(&rgn, &rgn2, RGN_XOR);

	cdc->InvertRgn(&rgn);
	redraw = false;

	view.ReleaseDC(cdc);
}

void MapTransferDialog::SetSelectedPos(int x, int y) {
	char buffer[256];

	// Clamp values
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= mSelectedMap.width) x = mSelectedMap.width - 1;
	if (y >= mSelectedMap.height) y = mSelectedMap.height - 1;

	// Do not accept invalid input
	if (x == mOldValues.xpos && y == mOldValues.ypos)
		return;

	// Set old values for position
	mOldValues.xpos = x;
	mOldValues.ypos = y;

	// Set values
	sprintf(buffer, "%d", mOldValues.xpos);
	SetDlgItemText(INPUT_MS_XPOS, buffer);
	sprintf(buffer, "%d", mOldValues.ypos);
	SetDlgItemText(INPUT_MS_YPOS, buffer);

	// Check if the scroll needs to be set
	CRect viewrect(GetViewPort());
	if ((x * 16) + 16 < viewrect.left || (y * 16) + 16 < viewrect.top || (x * 16) >= viewrect.right || (y * 16) >= viewrect.bottom)
		SetOffset(((x * 16) - 8) - (viewrect.Width() / 2), ((y * 16) - 8) - (viewrect.Height() / 2));

	DrawSelection();
}

bool MapTransferDialog::SetSelectedMap(int i)
{
	// Don't have to do anything; this is already selected
	if (i == mOldValues.mapidx)
		return true;

	// If we cannot load the map at this index, then load the old one.
	if (mSelectedMap.load(&CaveMapINFO(exe.mod.mStages.list[i % exe.mod.mStages.count], &exe.mod)))
	{
		mMapList.SetCurSel(mOldValues.mapidx);
		return false;
	}

	// Set the current map
	mOldValues.mapidx = i;

	// Init our bitmap memory
	CDC* cdc;
	cdc = GetDC();
	if (bmpMem[0].m_hObject != NULL)
		bmpMem[0].DeleteObject();
	if (bmpMem[1].m_hObject != NULL)
		bmpMem[1].DeleteObject();
	if (bmpMem[2].m_hObject != NULL)
		bmpMem[2].DeleteObject();
	if (bmpMem[3].m_hObject != NULL)
		bmpMem[3].DeleteObject();
	if (bmpMem[4].m_hObject != NULL)
		bmpMem[4].DeleteObject();

	bmpMem[0].CreateBitmap(mSelectedMap.width * 16, mSelectedMap.height * 16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	bmpMem[1].CreateBitmap(mSelectedMap.width * 16, mSelectedMap.height * 16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	bmpMem[2].CreateBitmap(mSelectedMap.width * 16, mSelectedMap.height * 16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	bmpMem[3].CreateBitmap(mSelectedMap.width * 16, mSelectedMap.height * 16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);
	bmpMem[4].CreateBitmap(mSelectedMap.width * 16, mSelectedMap.height * 16, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);

	// Invalidate ourselves so we can redraw the map
	RedrawMap(*cdc, true);
	ReleaseDC(cdc);

	// Set selected pos
	SetSelectedPos(mOldValues.xpos, mOldValues.ypos);

	// Set offset
	SetOffset(offX, offY);

	// Done!
	return true;
}

void MapTransferDialog::SetOffset(int x, int y) {
	CRect rect;

	// Get the map dimensions
	int mW = mSelectedMap.width * 16;
	int mH = mSelectedMap.height * 16;

	// Get the viewport dimensions
	mViewFrame.GetWindowRect(&rect);
	ScreenToClient(&rect);//gets map display rectangle relative to main window area

	// Limit the offsets
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x > mW - (rect.right - rect.left)) x = mW - (rect.right - rect.left);
	if (y > mH - (rect.bottom - rect.top)) y = mH - (rect.bottom - rect.top);

	// Compare & ignore if they're the same
	if (offX == x && offY == y)
		return;

	// Set the new offsets
	offX = x;
	offY = y;

	// Redraw the map
	CDC* cdc = GetDC();
	RedrawMap(*cdc);
	ReleaseDC(cdc);
}

void MapTransferDialog::RedrawMap(CDC& dc, bool regen) {
	int i, j, w, h;
	RECT rect;

	CDC cdc;
	cdc.CreateCompatibleDC(&dc);
	CDC cdc2;
	cdc2.CreateCompatibleDC(&dc);

	CBrush brush;
	CBitmap* pOldBmp;
	CBitmap* pOldBmp2;
	CBitmap bmpFinal;

	mViewFrame.GetClientRect(&rect);
	mViewFrame.ClientToScreen(&rect);
	ScreenToClient(&rect);

	//used for triple buffering.  I think that's what this is...
	bmpFinal.CreateBitmap(rect.right - rect.left, rect.bottom - rect.top, 1, CS_GRAPHIC_COLOR_DEPTH, NULL);

	pOldBmp2 = (CBitmap*)(cdc2.SelectObject(&bmpFinal));

	w = min(int((rect.right - rect.left) / scale), mSelectedMap.width * 16);
	h = min(int((rect.bottom - rect.top) / scale), mSelectedMap.height * 16);
	if (w + offX > mSelectedMap.width * 16)
		w = mSelectedMap.width * 16 - offX;
	if (h + offY > mSelectedMap.height * 16)
		h = mSelectedMap.height * 16 - offY;

	cdc2.FillSolidRect(0, 0, rect.right - rect.left, rect.bottom - rect.top, 0x000000);
	cdc2.FillSolidRect(0, 0, int(w * scale + 1), int(h * scale + 1), 0xFFFF00FF);
	cdc2.FillSolidRect(0, 0, int(w * scale), int(h * scale), 0x000000);

	BLENDFUNCTION blend;

	if (CS_GRAPHIC_COLOR_DEPTH == 32)
		blend.AlphaFormat = AC_SRC_ALPHA;
	else
		blend.AlphaFormat = 0;

	blend.BlendFlags = 0;
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = CS_DEFAULT_TILETYPE_ALPHA;

	for (i = 0; i < MAP_DISPLAY_LAST - 1; i++) {
		// select the bitmap to memory dc
		pOldBmp = (CBitmap*)(cdc.SelectObject(&bmpMem[i]));

		// ***Start Drawing***
		if (regen) {
			switch (i)
			{
			case MAP_BACKGROUND:
				cdc.FillSolidRect(0, 0, mSelectedMap.width * 16, mSelectedMap.height * 16, 0x000000);
				mSelectedMap.displayBackground(&cdc);
				break;
			case MAP_BACK_TILES:
				cdc.FillSolidRect(0, 0, mSelectedMap.width * 16, mSelectedMap.height * 16, 0x000000);
				mSelectedMap.displayBackMap(&cdc);
				break;
			case MAP_FRONT_TILES:
				cdc.FillSolidRect(0, 0, mSelectedMap.width * 16, mSelectedMap.height * 16, 0x000000);
				mSelectedMap.displayFrontMap(&cdc);
				break;
			case MAP_TILE_TYPES:
				cdc.FillSolidRect(0, 0, mSelectedMap.width * 16, mSelectedMap.height * 16, 0x000000);
				mSelectedMap.displayTileTypes(&cdc);
				break;
			}
		}

		cdc2.TransparentBlt(0, 0, int(w * scale), int(h * scale), &cdc, offX, offY, w, h, 0);

		//remove created bitmap and release memory
		cdc.SelectObject(pOldBmp);

	}

	// *** Draw Entities *** 
	{
		CFont font;

		font.CreatePointFont(scale * 40, "Arial Narrow", &cdc2);
		CFont* oldFont = cdc2.SelectObject(&font);
		npcTable;
		//then show entities
		cdc2.SetBkMode(TRANSPARENT);//this way there's no box around the text
		cdc2.SetTextColor(0xcccccc);
		brush.CreateSolidBrush(0x00ff00);

		for (j = 0; j < mSelectedMap.entityNum; j++)
			DisplayEntity(cdc2, brush, j);

		oldFont = cdc2.SelectObject(oldFont);
		font.DeleteObject();
		brush.DeleteObject();
	}

	//finally draw everything to the screen
	blend.AlphaFormat = 0;
	blend.BlendFlags = 0;
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;
	dc.AlphaBlend(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, &cdc2, 0, 0, rect.right - rect.left, rect.bottom - rect.top, blend);

	redraw = true;

	//final cleanup
	(cdc2.SelectObject(&pOldBmp2));
	bmpFinal.DeleteObject();
	cdc.DeleteDC();
	cdc2.DeleteDC();

	DrawSelection();
}

void MapTransferDialog::DisplayEntity(CDC& cdc2, CBrush& brush, int index)
{
	RECT entRect;

	npcTable.entity[index];

	int x = int((mSelectedMap.entity[index].x * 16 + 8 - offX) * scale);
	int y = int((mSelectedMap.entity[index].y * 16 + 8 - offY) * scale);

	entRect.left = x - 8 * scale;
	entRect.top = y - 8 * scale;
	entRect.right = x + 8 * scale;
	entRect.bottom = y + 8 * scale;;
	cdc2.FrameRect(&entRect, &brush);
	entRect.right--;
	entRect.bottom--;

	if (mSelectedMap.entity[index].npc >= 0 && npcTable.extra[mSelectedMap.entity[index].npc].loaded)
	{
		//add a slight shadow effect to added readability
		cdc2.SetTextColor(0x000000);
		cdc2.ExtTextOut(entRect.left + 0.5 * scale, entRect.top + 0.5 * scale, ETO_CLIPPED, &entRect, npcTable.extra[mSelectedMap.entity[index].npc].shortDesc1, strlen(npcTable.extra[mSelectedMap.entity[index].npc].shortDesc1), NULL);
		entRect.top += scale * 8;//start halfway down entity box
		cdc2.ExtTextOut(entRect.left + 0.5 * scale, entRect.top + 0.5 * scale, ETO_CLIPPED, &entRect, npcTable.extra[mSelectedMap.entity[index].npc].shortDesc2, strlen(npcTable.extra[mSelectedMap.entity[index].npc].shortDesc2), NULL);

		//write text
		entRect.top -= scale * 8;//back to top of entity box
		cdc2.SetTextColor(0xcccccc);
		cdc2.ExtTextOut(entRect.left, entRect.top, ETO_CLIPPED, &entRect, npcTable.extra[mSelectedMap.entity[index].npc].shortDesc1, strlen(npcTable.extra[mSelectedMap.entity[index].npc].shortDesc1), NULL);
		entRect.top += scale * 8;//start halfway down entity box
		cdc2.ExtTextOut(entRect.left, entRect.top, ETO_CLIPPED, &entRect, npcTable.extra[mSelectedMap.entity[index].npc].shortDesc2, strlen(npcTable.extra[mSelectedMap.entity[index].npc].shortDesc2), NULL);
	}
}

#define VALIDATE_NUMBER(varName, resourceName, cmpOp, errstr, ...) \
	if (!(*error) && (cmpOp)) {\
		sprintf(error, errstr, __VA_ARGS__);\
		sprintf(buffer, "%d", mOldValues.##varName);\
		SetDlgItemText(resourceName, buffer);\
	} else if(!(cmpOp)) { \
		GetDlgItemText(resourceName, buffer, 256);\
		mOldValues.##varName = atoi(buffer);\
	}

BOOL MapTransferDialog::ValidateInputs() {
	char buffer[256];
	int xpos  = GetDlgItemInt(INPUT_MS_XPOS);
	int ypos  = GetDlgItemInt(INPUT_MS_YPOS);
	int eveno = GetDlgItemInt(INPUT_MS_EVENT);

	mOldValues.mapidx = mMapList.GetCurSel();

	// Init the error buffer
	char error[1024];
	*error = 0;

	int err = 0;
	if ((err = mSelectedMap.load(&CaveMapINFO(exe.mod.mStages.list[mOldValues.mapidx], &exe.mod))) != 0) sprintf(error, "Failed to open map file \"%s\".\nError Code # %04d", exe.mod.mStages.list[mOldValues.mapidx].file, err);

	// Try to get an error
	VALIDATE_NUMBER(xpos, INPUT_MS_XPOS, (xpos < 0 || xpos >= mSelectedMap.width), "The X position should be inbetween 0 and %d.", mSelectedMap.width);
	VALIDATE_NUMBER(ypos, INPUT_MS_YPOS, (ypos < 0 || ypos >= mSelectedMap.height), "The Y position should be inbetween 0 and %d.", mSelectedMap.height);
	VALIDATE_NUMBER(eveno, INPUT_MS_EVENT, (eveno < 0 || eveno >= 9999), "The event ID should be inbetween 0 and 9999.");

	// Print the error if there is one
	if (*error) {
		char complete_error_msg[2048];
		sprintf(complete_error_msg, "There was an error while modifying this command:\n\n%s", error);
		MessageBox(complete_error_msg, "Error - The Cave Story Editor", MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

void MapTransferDialog::OnBnClickedMsOk() {
	if (!ValidateInputs())
		return;

	this->EndDialog(1);
}

void MapTransferDialog::OnBnClickedMsCancel() {
	this->EndDialog(0);
}

void MapTransferDialog::OnCbnSelchangeMsMap() {
	SetSelectedMap(mMapList.GetCurSel());
}

void MapTransferDialog::OnBnClickedMsEventview() {
	EventSelectorDialog dlg;

	// Load the path
	char temp[1024];

	// Load events
	sprintf(temp, "%s/Head.tsc", exe.mod.mModPath);
	dlg.LoadEvents(temp);
	sprintf(temp, "%s/Stage/%s.tsc", exe.mod.mModPath, mSelectedMap.mapInfo.fileName);
	dlg.LoadEvents(temp);

	GetDlgItemText(INPUT_MS_EVENT, temp, 1024);
	dlg.mInitEvent = atoi(temp);

	// Show the dialog
	int result = dlg.DoModal();
	if (result == -1)
		return;

	// Done
	char buff[256];
	sprintf(buff, "%d", result);
	SetDlgItemText(INPUT_MS_EVENT, buff);
}

void MapTransferDialog::OnRButtonDown(UINT nFlags, CPoint point) {
	if (mwheel)
		return;

	CRect rect;
	mViewFrame.GetWindowRect(&rect);
	ScreenToClient(&rect);//gets map display rectangle relative to main window area

	if (!rect.PtInRect(point))
		return;

	mDragging  = true;
	mDragStart = { point.x - rect.left, point.y - rect.top };
	mwheel     = true;

	SetCursor(mCursor_Move);
}

void MapTransferDialog::OnRButtonUp(UINT nFlags, CPoint point) {
	mwheel    = false;
	mDragging = false;

	SetCursor(mCursor_Arrow);
}

void MapTransferDialog::OnLButtonDown(UINT nFlags, CPoint point) {
	if (lmb)
		return;

	CRect rect;
	mViewFrame.GetWindowRect(&rect);
	ScreenToClient(&rect);//gets map display rectangle relative to main window area

	if (!rect.PtInRect(point))
		return;

	lmb = true;

	// User clicked
	int xC = int((point.x - rect.left) / 16.0 / scale + offX / 16.0);
	int yC = int((point.y - rect.top) / 16.0 / scale + offY / 16.0);

	if (xC >= mSelectedMap.width || yC >= mSelectedMap.height)
		return;

	SetSelectedPos(xC, yC);
}

void MapTransferDialog::OnLButtonUp(UINT nFlags, CPoint point) {
	lmb = false;
}

void MapTransferDialog::OnMouseMove(UINT nFlags, CPoint point) {
	if (!mDragging) {
		if (lmb) {
			lmb = false;
			OnLButtonDown(nFlags, point);
			lmb = true;
		}

		return;
	}

	CRect rect;
	mViewFrame.GetWindowRect(&rect);
	ScreenToClient(&rect);//gets map display rectangle relative to main window area

	int rX = (point.x - (rect.left + mDragStart.x)) / 2;
	int rY = (point.y - (rect.top + mDragStart.y)) / 2;

	SetOffset(offX - rX, offY - rY);
	ClientToScreen(&rect);

	// Lock the cursor
	SetCursorPos(rect.left + mDragStart.x, rect.top + mDragStart.y);
	SetCursor(mCursor_Move);
}

static bool ignore = false;
void MapTransferDialog::OnEnChangeMsXpos()
{
	if (ignore)
		return;

	char buffer[256];
	bool setval = false;

	// Get the value
	GetDlgItemText(INPUT_MS_XPOS, buffer, 256);

	// Get the old cursor position
	int old_sel = ((CEdit*)GetDlgItem(INPUT_MS_XPOS))->GetSel();

	if (mOldValues.xpos == 0) {
		if (buffer[0] == '0') {
			buffer[0] = buffer[1];
			buffer[1] = 0;
			old_sel   = 1;
			setval    = true;
		} else if (buffer[1] == '0') {
			buffer[1] = 0;
			setval    = true;
		}

		if (buffer[0] == 0) {
			buffer[0] = '0';
			buffer[1] = 0;
			old_sel   = 1;
		}
	}

	// Set it
	SetSelectedPos(atoi(buffer), mOldValues.ypos);

	// Set the cursor position
	((CEdit*)GetDlgItem(INPUT_MS_XPOS))->SetSel(old_sel);

	if (setval) {
		ignore = true;
		SetDlgItemText(INPUT_MS_XPOS, buffer);
		ignore = false;
	}
}


void MapTransferDialog::OnEnChangeMsYpos() {
	if (ignore)
		return;

	char buffer[256];
	bool setval = false;

	// Get the value
	GetDlgItemText(INPUT_MS_YPOS, buffer, 256);

	// Get the old cursor position
	int old_sel = ((CEdit*)GetDlgItem(INPUT_MS_YPOS))->GetSel();

	if (mOldValues.ypos == 0) {
		if (buffer[0] == '0') {
			buffer[0] = buffer[1];
			buffer[1] = 0;
			setval    = true;
			old_sel   = 1;
		} else if (buffer[1] == '0') {
			buffer[1] = 0;
			setval    = true;
		}

		if (buffer[0] == 0) {
			buffer[0] = '0';
			buffer[1] = 0;
			old_sel   = 1;
		}
	}

	// Set it
	SetSelectedPos(mOldValues.xpos, atoi(buffer));

	// Set the cursor position
	((CEdit*)GetDlgItem(INPUT_MS_YPOS))->SetSel(old_sel);

	if (setval) {
		ignore = true;
		SetDlgItemText(INPUT_MS_YPOS, buffer);
		ignore = false;
	}
}
