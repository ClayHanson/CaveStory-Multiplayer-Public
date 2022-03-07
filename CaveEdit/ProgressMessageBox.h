#pragma once
#include "afxwin.h"
#include "Resource.h"
#include "ProgressBar.h"

class ProgressMessageBox : public CDialog {
public:
	ProgressWindow::ProgressFunc mFunc;
	UINT_PTR mTimerPtr;
	void** mUserArgs;
	bool mRunning;

public:
	ProgressMessageBox(CWnd* pParent = NULL);
	virtual ~ProgressMessageBox();
	enum { IDD = DIALOG_PROGRESS_BOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

public:
	DECLARE_MESSAGE_MAP();

public:
	void SetTaskName(const char* text);

public:
	void SetCurrentProgress(int percent, const char* text = NULL);
	void SetOverallProgress(int percent, const char* text = NULL);

	void SetMaxCurrentProgress(int val);
	void SetMaxOverallProgress(int val);

public:
	void StartMainTimer();

public:
	afx_msg void OnTimer(UINT_PTR timer_ptr);
};
