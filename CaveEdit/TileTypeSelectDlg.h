#pragma once


// TileTypeSelectDlg dialog

class TileTypeSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(TileTypeSelectDlg)

public:
	CPoint pt;
	int tile;
	int tiles_per_row;
	TileTypeSelectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~TileTypeSelectDlg();

// Dialog Data
	enum { IDD = DIALOG_DISPLAY_VIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
};
