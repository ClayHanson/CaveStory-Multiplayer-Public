#pragma once

class DlgErrorWindow : public CDialog
{
	DECLARE_DYNAMIC(DlgErrorWindow)

protected: // Message map

	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = DIALOG_ERROR_WINDOW };

public:
	CRichEditCtrl mErrorContent;
	CProgressCtrl mProgressBar;
	CButton mOk;
	CButton mCancel;
	CWnd mMainText;

public:
	bool mOkPressed;
	bool mCancelPressed;

public: // Constructor / deconstructor
	DlgErrorWindow(CWnd* pParent = nullptr);
	virtual ~DlgErrorWindow();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL ContinueModal();

public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};

//--------------------------------------------------------------------------------------------------//

// Interface for DlgErrorWindow
class ErrorWindow
{
protected: // Variables

	DlgErrorWindow* mDialog;

public: // Constructor / deconstructor

	ErrorWindow(CWnd* pParent = nullptr);
	~ErrorWindow();

public: // Window methods

	void SetShown(bool bShown);
	void SetTitle(const char* pTitle);
	void SetDescription(const char* pDesc);

public: // Progress bar methods

	void SetProgressBarShown(bool bShown);
	void SetProgressMinMax(int iMin, int iMax);
	void SetProgress(int iAmt);

public: // Error list management

	void AddContent(const char* pFormat, ...);
	void ClearContent();

public: // Button methods

	void SetButtonEnabled_OK(bool bEnabled);
	void SetButtonEnabled_Cancel(bool bEnabled);
	void SetButtonText_OK(const char* pText);
	void SetButtonText_Cancel(const char* pText);

	void Process();

public: // Misc methods

	inline bool WasOkPressed() { bool ret = mDialog->mOkPressed; mDialog->mOkPressed = false; return ret; }
	inline bool WasCancelPressed() { bool ret = mDialog->mCancelPressed; mDialog->mCancelPressed = false; return ret; }
};