#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "CMessageBoxPreview.h"


struct ScriptError
{
	int offset;
	int size;
	CString error;
};

// ScriptEditorDlg dialog

class CaveMapEditDlg;

class ScriptEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(ScriptEditorDlg)

public:
	POINT m_MinWindowSize;
	ScriptEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ScriptEditorDlg();

// Dialog Data
	enum { IDD = DIALOG_SCRIPT_EDITOR };

	int scriptMapIndex;

	int currentCommandArgs[32];
	int currentCommandPos;
	int currentCommandIndex;//index in text
	int currentCommandSize;//size in text

	void Open(char* file, int mapIndex);
	void Save();

	HICON m_hIcon;

protected:
	int sound0;
	int sound1;
	int sound2;
	CImage argBoxImage0;
	CImage argBoxImage1;
	CImage argBoxImage2;

protected:
	CMessageBoxPreview mTextboxPreview;
	CaveMap mCurrentMap;
	NPCtable mNpcTable;
	TSC_Info command;//list of valid commands with syntax. lets hope there aren't any more than 256
	char fileName[256];
	int size;
	bool format;
	bool help;
	bool init;
	unsigned char* scriptText;
	int scriptTextLen;
	int width;
	int height;

	CFont m_Font;

	void displayCommand(int i, int textStart = -1);
	void FormatText(bool line = false);
	bool CheckSyntax(int i = 0);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CaveMapEditDlg* mMapEditor;

public:
	BOOL SetArgBoxImage(int index, const char* dataPath, int img_index, int w, int h);
	BOOL IsLoaded() { return scriptText != NULL; }
	int Get_TRA_MapDest_FromEventNo(int iEventNo);

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedScript0();
	afx_msg void OnBnClickedScript1();
	afx_msg void OnBnClickedScript2();
	afx_msg void PlaySound(int sound);
	afx_msg void OnBnClickedSyntax();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLbnSelchangeCommands();
	afx_msg void OnLbnDblclkCommands();
	afx_msg void OnBnClickedDetails();
	CRichEditCtrl scriptControl;
	afx_msg void OnEnChangeScript();
	afx_msg void OnEnSelchangeScript(NMHDR *pNMHDR, LRESULT *pResult);
	CListBox commandListControl;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEnUpdateScript();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedEditCommand();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedShowmsgbox();
};
