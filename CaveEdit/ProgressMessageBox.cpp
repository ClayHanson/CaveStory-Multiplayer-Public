#include "stdafx.h"
#include "CaveEditor.h"
#include "ProgressMessageBox.h"

ProgressMessageBox::ProgressMessageBox(CWnd* pParent) : CDialog(DIALOG_PROGRESS_BOX, pParent)
{
	mFunc     = NULL;
	mUserArgs = NULL;
	mRunning  = false;
	mTimerPtr = NULL;
}

ProgressMessageBox::~ProgressMessageBox() {
	if (mUserArgs)
		delete[] mUserArgs;

	if (mTimerPtr)
		this->KillTimer(mTimerPtr);
}

void ProgressMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL ProgressMessageBox::OnInitDialog()
{
	((CProgressCtrl*)GetDlgItem(IDC_PROGRESS_CURRENT))->SetRange(0, 100);
	ModifyStyle(WS_SYSMENU, 0);
	return TRUE;
}


BEGIN_MESSAGE_MAP(ProgressMessageBox, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ProgressMessageBox::SetTaskName(const char* text) {
	SetDlgItemText(TEXT_PROGRESS_TASK, text);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ProgressMessageBox::SetCurrentProgress(int percent, const char* text) {
	if (text)
		SetDlgItemText(TEXT_PROGRESS_CURRENT, text);

	((CProgressCtrl*)GetDlgItem(IDC_PROGRESS_CURRENT))->SetPos(percent);
}

void ProgressMessageBox::SetOverallProgress(int percent, const char* text) {
	if (text)
		SetDlgItemText(TEXT_PROGRESS_OVERALL, text);

	((CProgressCtrl*)GetDlgItem(IDC_PROGRESS_OVERALL))->SetPos(percent);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ProgressMessageBox::SetMaxCurrentProgress(int val) {
	((CProgressCtrl*)GetDlgItem(IDC_PROGRESS_CURRENT))->SetRange(0, val);
}

void ProgressMessageBox::SetMaxOverallProgress(int val) {
	((CProgressCtrl*)GetDlgItem(IDC_PROGRESS_OVERALL))->SetRange(0, val);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ProgressMessageBox::StartMainTimer() {
	// Start the timer
	mTimerPtr = this->SetTimer(1, 1, NULL);
}

void ProgressMessageBox::OnTimer(UINT_PTR timer_ptr) {
	if (!mRunning) {
		delete this;
		return;
	}

	mFunc(mUserArgs);
}