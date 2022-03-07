#pragma once


class CaveMapEditDlg;//forward declaration for circular referencing

// TileSelectDlg dialog

class TileSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(TileSelectDlg)

public:
	TileSelectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~TileSelectDlg();

	int scale;
	int x;
	int y;
	int x2;
	int y2;
	int selected;
	unsigned int selection_number;
	bool showTileTypes;
	CaveMapEditDlg* mapEditDialog;
// Dialog Data
	enum { IDD = DIALOG_TILE_SELECT };

	CaveTileSet* tileSet;

	void loadTiles(CaveTileSet* tileSet);
	void Redraw();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
