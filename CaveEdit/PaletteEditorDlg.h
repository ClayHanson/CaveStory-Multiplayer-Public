#pragma once


// PaletteEditorDlg dialog

class PaletteEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(PaletteEditorDlg)

public:
	bool change;
	int palSize;
	RGBQUAD palette[256];//color palette

	PaletteEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~PaletteEditorDlg();

// Dialog Data
	enum { IDD = DIALOG_PALETTE_EDITOR };

protected:
	void Redraw();
	int height;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
protected:
	virtual void OnOK();
};
