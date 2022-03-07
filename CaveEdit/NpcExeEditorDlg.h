#pragma once


// NpcExeEditorDlg dialog

class NpcExeEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(NpcExeEditorDlg)

public:
	NpcExeEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~NpcExeEditorDlg();

// Dialog Data
	enum { IDD = DIALOG_NPC_EXE_EDITOR };

	char codeBuf[16000];
	int index;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	RECT findNpcRect(char* buffer);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedPrev();
	afx_msg void OnBnClickedNext();
	afx_msg void OnEnChangeNpcCode();
	afx_msg void OnEnVscrollNpcCode();
};
