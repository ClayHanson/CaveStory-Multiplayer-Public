#pragma once

#include "General.h"


// MapTransferDialog dialog

class MapTransferDialog : public CDialog
{
	DECLARE_DYNAMIC(MapTransferDialog)

public:
	CaveMap* mMap;
	CaveMap mSelectedMap;

	NPCtable npcTable;
	CComboBox mMapList;
	CStatic mViewFrame;//this is the map view frame
	CStatic view;//this is a child of the map view frame and is the actual image
	CBitmap bmpMem[5];//used for flicker free display (sort of)
	bool init;

	int offX;
	int offY;
	int scale;
	bool lmb;
	bool mwheel;
	bool mDragging;
	CPoint mDragStart;
	bool redraw;
	RECT mSelRect;

	HCURSOR mCursor_Arrow;
	HCURSOR mCursor_Move;

	struct {
		int xpos;
		int ypos;
		int eveno;
		int mapidx;
	} mOldValues;

public:
	MapTransferDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~MapTransferDialog();

// Dialog Data
	enum { IDD = DIALOG_MAP_TRANSFER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL ValidateInputs();

public:
	afx_msg void OnPaint();
	CRect GetViewPort();
	void DrawSelection();
	void SetSelectedPos(int x, int y);
	bool SetSelectedMap(int i);
	void SetOffset(int x, int y);
	void RedrawMap(CDC& dc, bool regen = 0);
	void DisplayEntity(CDC& cdc2, CBrush& brush, int index);

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedMsOk();
	afx_msg void OnBnClickedMsCancel();
	afx_msg void OnCbnSelchangeMsMap();
	afx_msg void OnBnClickedMsEventview();
	afx_msg void OnEnChangeMsXpos();
	afx_msg void OnEnChangeMsYpos();
};
