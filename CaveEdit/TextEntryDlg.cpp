// TextEntryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "TextEntryDlg.h"
#include ".\textentrydlg.h"


// TextEntryDlg dialog

IMPLEMENT_DYNAMIC(TextEntryDlg, CDialog)
TextEntryDlg::TextEntryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TextEntryDlg::IDD, pParent)
{
}

TextEntryDlg::~TextEntryDlg()
{
}

void TextEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TextEntryDlg, CDialog)
END_MESSAGE_MAP()


// TextEntryDlg message handlers

BOOL TextEntryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText(EDIT_TEXT, text);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void TextEntryDlg::OnOK()
{
	GetDlgItemText(EDIT_TEXT, text);

	CDialog::OnOK();
}
