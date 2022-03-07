#pragma once

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CsmpCheckBox : public CDialog
{
	DECLARE_DYNAMIC(CsmpCheckBox)

protected:
	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = CSMPGUI_CHECKBOX };

private: // Variables
	CImage m_SpritesheetImage;
	bool m_bHover;

public: // Variables pertaining to displaying the text accurately
	bool m_bCheckBoxValue;

public:
	CsmpCheckBox(CWnd* pParent = nullptr);
	virtual ~CsmpCheckBox();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

public:
	bool GetValue() { return m_bCheckBoxValue; }
	void SetValue(bool bValue) { m_bCheckBoxValue = bValue; Invalidate(TRUE); }

public:
	virtual void OnOK();
	virtual void OnCancel();

public: // Input
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();

public:
	void Redraw(CDC* cdc, CRect* rect);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
