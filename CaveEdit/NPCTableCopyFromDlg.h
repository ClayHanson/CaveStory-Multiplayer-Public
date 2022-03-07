#pragma once


// NPCTableCopyFromDlg dialog

class NPCTableCopyFromDlg : public CDialog
{
	DECLARE_DYNAMIC(NPCTableCopyFromDlg)

public:
	NPCTableCopyFromDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~NPCTableCopyFromDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = DIALOG_NTE_COPYFROM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
