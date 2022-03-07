#pragma once


// ResizeDlg dialog

class ResizeDlg : public CDialog
{
	DECLARE_DYNAMIC(ResizeDlg)

public:

	CString name;//window title

	CString wName;//width edit box label
	CString hName;//height edit box label
	int width;//initial width
	int height;//initial height

	CString c1Name;//label of check box 1
	CString c2Name;//label of check box 2
	bool check1;//initial check box 1 state
	bool check2;//initial check box 2 state

	ResizeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ResizeDlg();

// Dialog Data
	enum { IDD = DIALOG_RESIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
