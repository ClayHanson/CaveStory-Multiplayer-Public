#pragma once

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "CsmpCheckBox.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CMessageBoxPreview : public CDialog
{
	DECLARE_DYNAMIC(CMessageBoxPreview)

public:
	enum { IDD = TSCDLG_MESSAGE_BOX };

protected:
	DECLARE_MESSAGE_MAP()

public: // Enumerations
	enum KeyPress
	{
		KP_OK     = 0b0000001,
		KP_CANCEL = 0b0000010,
		KP_LEFT   = 0b0000100,
		KP_RIGHT  = 0b0001000
	};

public: // Variables
	CImage m_TextBoxImage;
	CImage m_SpritesheetImage;
	CImage* m_SymbolImage;
	CFont m_Font_Text;

public: // Variables pertaining to displaying the text accurately
	CsmpCheckBox m_CheckBox_AutoFast;
	const char* m_ScriptPtr;
	CRichEditCtrl* m_ScriptPtrGui;
	UINT_PTR m_ProcTimer;
	CString m_ScriptString;
	bool m_ScriptUpdated;
	bool m_Init;

	int m_Key;
	int m_KeyTrg;
	int m_KeyOld;
	int m_DesiredOffset;

	struct
	{
		bool show_blink;
		int mode;
		char gTSText[0x100];
		int ypos_line[4];
		int offsetY;
		CRect rcText;
		int face;
		int face_x;
		CImage* LINE_BMP_1;
		CImage* LINE_BMP_2;
		CImage* LINE_BMP_3;
		CImage* LINE_BMP_4;
		signed char flags;
		unsigned char wait_beam;
		unsigned int p_read;
		int p_tur_x;
		int p_write;
		int line;
		int wait;
		int wait_next;
		int next_event;
		int select;
		int current_event;
	} m_TextInfo;

public:
	CMessageBoxPreview(CWnd* pParent = nullptr);
	virtual ~CMessageBoxPreview();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

public:
	inline unsigned char GetScriptPtr(int iOffset);
	void ExecuteEvent(int iEventNo);
	int GetTextScriptNo(int a);
	void ClearTextLine();
	void CheckNewLine();
	void ProcessTextBox();
	void PutLineText(int x, int y, const char* text, unsigned long color, int iLineIndex, bool center = false);
	int GetTextWidth(const char* pString, int iLength);
	void SizeContents();

public:
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

public: // Input
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();

public:
	void RedrawTextBox(CDC* cdc, CRect* rect);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
