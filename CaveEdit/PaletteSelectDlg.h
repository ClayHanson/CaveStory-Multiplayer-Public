#pragma once


// PaletteSelectDlg dialog

class PaletteSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(PaletteSelectDlg)

public:
	PaletteSelectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~PaletteSelectDlg();

// Dialog Data
	enum { IDD = DIALOG_PALETTE_SELECT };

	int height;
	int palSize;
	RGBQUAD palette[256];//color palette
	int selected; //selected palette index

	void loadPal(RGBQUAD* pal, int count);

	void Redraw();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNMCustomdrawRed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawGreen(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawBlue(NMHDR *pNMHDR, LRESULT *pResult);
};
