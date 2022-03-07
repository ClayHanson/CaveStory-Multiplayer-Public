#include "stdafx.h"
#include "CaveEditor.h"
#include "ErrorWindow.h"
#include "afxdialogex.h"

#include <stdarg.h>

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(DlgErrorWindow, CDialog)

BEGIN_MESSAGE_MAP(DlgErrorWindow, CDialog)
	ON_BN_CLICKED(ERROR_CANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(ERROR_OK, OnBnClickedOk)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DlgErrorWindow::DlgErrorWindow(CWnd* pParent) : CDialog(DIALOG_ERROR_WINDOW, pParent)
{
	mOkPressed     = false;
	mCancelPressed = false;
}

DlgErrorWindow::~DlgErrorWindow()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgErrorWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, EDIT_ERROR_CONTENTS, mErrorContent);
	DDX_Control(pDX, ERROR_PROGRESS, mProgressBar);
	DDX_Control(pDX, TEXT_ERROR_TEXT, mMainText);
	DDX_Control(pDX, ERROR_CANCEL, mCancel);
	DDX_Control(pDX, ERROR_OK, mOk);
}

BOOL DlgErrorWindow::ContinueModal()
{
	CDialog::ContinueModal();

	return FALSE;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ErrorWindow::ErrorWindow(CWnd* pParent)
{
	mDialog = new DlgErrorWindow();

	BOOL ret = mDialog->Create(DIALOG_ERROR_WINDOW, pParent);

	if (!ret)   //Create failed.
	{
		AfxMessageBox(_T("Error creating Dialog"));
		return;
	}

	mDialog->mErrorContent.SetWindowTextA("");
	mDialog->mErrorContent.SetSel(0, 0);
}

ErrorWindow::~ErrorWindow()
{
	mDialog->DestroyWindow();
	delete mDialog;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ErrorWindow::SetShown(bool bShown)
{
	mDialog->ShowWindow(bShown ? SW_SHOW : SW_HIDE);
}

void ErrorWindow::SetTitle(const char* pTitle)
{
	mDialog->SetWindowTextA(pTitle);
}

void ErrorWindow::SetDescription(const char* pDesc)
{
	mDialog->mMainText.SetWindowTextA(pDesc);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ErrorWindow::SetProgressBarShown(bool bShown)
{
	mDialog->mProgressBar.ShowWindow(bShown ? SW_SHOW : SW_HIDE);
}

void ErrorWindow::SetProgressMinMax(int iMin, int iMax)
{
	mDialog->mProgressBar.SetRange32(iMin, iMax);
	mDialog->mProgressBar.SetRange(iMin, iMax);
}

void ErrorWindow::SetProgress(int iAmt)
{
	mDialog->mProgressBar.OffsetPos(iAmt);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ErrorWindow::AddContent(const char* pFormat, ...)
{
	char pBuffer[2048];

	va_list ptr;
	va_start(ptr, pFormat);
	vsprintf_s(pBuffer, sizeof(pBuffer), pFormat, ptr);
	va_end(ptr);

	int iOldLength = mDialog->mErrorContent.GetWindowTextLengthA();
	mDialog->mErrorContent.SetSel(-1, -1);
	mDialog->mErrorContent.ReplaceSel(pBuffer);
	mDialog->mErrorContent.SetSel(-1, -1);
	mDialog->mErrorContent.LineScroll(1, 0);
}

void ErrorWindow::ClearContent()
{
	mDialog->mErrorContent.SetWindowTextA("");
	mDialog->mErrorContent.SetSel(0, 0);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ErrorWindow::SetButtonEnabled_OK(bool bEnabled)
{
	mDialog->mOk.EnableWindow(bEnabled ? TRUE : FALSE);
}

void ErrorWindow::SetButtonEnabled_Cancel(bool bEnabled)
{
	mDialog->mCancel.EnableWindow(bEnabled ? TRUE : FALSE);
}

void ErrorWindow::SetButtonText_OK(const char* pText)
{
	mDialog->mOk.SetWindowTextA(pText);
}

void ErrorWindow::SetButtonText_Cancel(const char* pText)
{
	mDialog->mCancel.SetWindowTextA(pText);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ErrorWindow::Process()
{
	MSG pMsg;

	while (::PeekMessage(&pMsg, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!AfxGetApp()->PumpMessage())
		{
			::PostQuitMessage(0);
			break;
		}
	}
	
	LONG iIdle = 0;
	while (AfxGetApp()->OnIdle(iIdle++));
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgErrorWindow::OnBnClickedCancel()
{
	mCancelPressed = true;
}

void DlgErrorWindow::OnBnClickedOk()
{
	mOkPressed = true;
}
