#pragma once


// TextEntryDlg dialog

class TextEntryDlg : public CDialog
{
	DECLARE_DYNAMIC(TextEntryDlg)

public:
	TextEntryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~TextEntryDlg();

	CString text;
// Dialog Data
	enum { IDD = DIALOG_TEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
