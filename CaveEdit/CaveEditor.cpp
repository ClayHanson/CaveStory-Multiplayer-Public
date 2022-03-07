// CaveEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "General.h"
#include "CaveEditor.h"
#include "CaveMapEditDlg.h"
#include "CaveEditorDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCaveEditorApp

BEGIN_MESSAGE_MAP(CCaveEditorApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCaveEditorApp construction

CCaveEditorApp::CCaveEditorApp()
{
}

CCaveEditorApp::~CCaveEditorApp()
{
}

// The one and only CCaveEditorApp object

CCaveEditorApp theApp;


// CCaveEditorApp initialization

BOOL CCaveEditorApp::InitInstance()
{
	CWinApp::InitInstance();

	AfxEnableControlContainer();
	AfxInitRichEdit2();

	char buffer[1024];
	::GetCurrentDirectory(1024, buffer);//is way we can get back to the program path regardless of what else we've done
	progPath = buffer;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CCaveEditorDlg dlg;
//	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
