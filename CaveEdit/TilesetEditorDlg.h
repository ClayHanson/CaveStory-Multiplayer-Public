#pragma once


// TilesetEditorDlg dialog

class TilesetEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(TilesetEditorDlg)

public:
	TilesetEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~TilesetEditorDlg();

	PaletteSelectDlg palette;

	bool change;
	bool close;
	bool lclick;
	bool redraw;
	bool init;
	int grid;//used for displaying grids on screen

	int xCursor;
	int yCursor;
	CPoint p;//used to draw lines instead of dots (not implemented)
	bool scrollbars;//used to prevent multiple UpdateScrollBars() calls
	int scale;
	int offX;//offset for scrolling
	int offY;//offset for scrolling

	void displayMap(CDC &dc);//displays bitmap
	CaveTileSet tileset;
// Dialog Data
	enum { IDD = DIALOG_TILESET_EDITOR };

protected:
	HICON m_hIcon;
	void loadPal();
	void UpdateScrollBars();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnFileSave();
	afx_msg void OnView16x16grid();
	afx_msg void OnView32x32grid();
	afx_msg void OnView64x64grid();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnViewGridoff();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnToolsResize();
	afx_msg void OnToolsEditpalette();
	afx_msg void OnFileExit();
	afx_msg void OnClose();
protected:
	virtual void OnCancel();
	virtual void OnOK();
};
