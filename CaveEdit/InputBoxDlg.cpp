// InputBoxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "InputBoxDlg.h"
#include "afxdialogex.h"


// InputBoxDlg dialog

IMPLEMENT_DYNAMIC(InputBoxDlg, CDialog)

InputBoxDlg::InputBoxDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(DIALOG_INPUT_BOX, pParent)
{
	m_Result = NULL;
	m_Config.Reset();
}

InputBoxDlg::~InputBoxDlg()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void InputBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, INPUT_BOX_INPUT_BOX, m_InputBox);
	DDX_Control(pDX, BUTTON_INPUTBOX_CANCEL, m_CancelButton);
	DDX_Control(pDX, BUTTON_INPUTBOX_OK, m_OKButton);
}

BEGIN_MESSAGE_MAP(InputBoxDlg, CDialog)
	ON_BN_CLICKED(BUTTON_INPUTBOX_OK, &InputBoxDlg::OnBnClickedInputboxOk)
	ON_BN_CLICKED(BUTTON_INPUTBOX_CANCEL, &InputBoxDlg::OnBnClickedInputboxCancel)
	ON_EN_CHANGE(INPUT_BOX_INPUT_BOX, &InputBoxDlg::OnInputChanged)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool InputBoxDlg::OpenInputBox(InputBoxResult* result_ptr, InputBoxConfig* config, const char* default_value)
{
	InputBoxDlg NewDlg;

	// Create it
	NewDlg.Create(IDD, NULL);

	// Setup configuration
	if (!config)
		NewDlg.m_Config.Reset();
	else
		memcpy(&NewDlg.m_Config, config, sizeof(InputBoxConfig));

	// Sanity check configuration
	if (config->min_length >= config->max_length)
	{
		printf("ERROR: " __FUNCTION__ "() - Minimum length is greater than (or equal to) maximum length!\r\n");
		return false;
	}

	// Sanity check configuration
	if (config->max_length <= 0)
	{
		printf("ERROR: " __FUNCTION__ "() - Maximum length is less than or equal to 0!\r\n");
		return false;
	}

	// Sanity check configuration
	if (config->min_length < 0)
	{
		printf("ERROR: " __FUNCTION__ "() - Minimum length is less than 0!\r\n");
		return false;
	}

	// Reset the result
	if (result_ptr)
		result_ptr->Reset();

	// Initialize
	NewDlg.m_Result = result_ptr;
	NewDlg.m_CancelButton.EnableWindow(NewDlg.m_Config.disable_cancel_button ? FALSE : TRUE);
	if (NewDlg.m_Config.info_text)
		NewDlg.GetDlgItem(LABEL_PLEASE_ENTER)->SetWindowText(NewDlg.m_Config.info_text);

	NewDlg.m_InputBox.SetWindowText(default_value == nullptr ? "" : default_value);

	// Start out
	NewDlg.UpdateInputBox();

	// Center it
	NewDlg.CenterWindow();

	// Show the window
	NewDlg.ShowWindow(SW_SHOW);

	// Run loop
	NewDlg.RunModalLoop();

	// Done!
	return (result_ptr ? (result_ptr->status_code == InputBoxResult::IBR_StatusCode::IBR_STATUS_OK) : false);
}

void InputBoxDlg::UpdateInputBox()
{
	CString Text;

	// Get the window text
	m_InputBox.GetWindowText(Text);

	// Enforce numeric-only input
	if (m_Config.only_numeric)
	{
		CString OnlyText(Text);
		CString Numeric(Text);

		// Remove numbers from this
		for (int i = 0; i < 10; i ++)
			OnlyText.Remove('0' + i);

		// Remove OnlyText from Text
		for (int i = 0; i < OnlyText.GetLength(); i++)
			Numeric.Remove(OnlyText.GetAt(i));

		// Compare & set if different
		if (Numeric != Text)
			m_InputBox.SetWindowText(Numeric.GetString());
	}

	// Enforce minimum text limit
	m_OKButton.EnableWindow(Text.GetLength() >= m_Config.min_length);

	// Limit text
	if (Text.GetLength() >= m_Config.max_length)
	{
		// Truncate it
		Text.Truncate(m_Config.max_length);

		// Reset text
		m_InputBox.SetWindowText(Text.GetString());
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void InputBoxDlg::OnBnClickedInputboxOk()
{
	// Set status & value of result ptr
	if (m_Result)
	{
		CString Value;

		m_InputBox.GetWindowText(Value);

		m_Result->status_code = InputBoxResult::IBR_StatusCode::IBR_STATUS_OK;
		m_Result->value       = new char[Value.GetLength() + 1];

		strcpy(m_Result->value, Value.GetString());
	}

	ShowWindow(SW_HIDE);
	EndDialog(0);
}

void InputBoxDlg::OnBnClickedInputboxCancel()
{
	// Set status of result
	if (m_Result)
	{
		m_Result->status_code = InputBoxResult::IBR_StatusCode::IBR_STATUS_CANCEL;

		if (m_Config.error_value != NULL)
		{
			m_Result->value = new char[strlen(m_Config.error_value) + 1];
			strcpy(m_Result->value, m_Config.error_value);
		}
	}

	ShowWindow(SW_HIDE);
	EndDialog(0);
}

void InputBoxDlg::OnInputChanged()
{
	UpdateInputBox();
}

void InputBoxDlg::OnClose()
{
	OnBnClickedInputboxCancel();
	CDialog::OnClose();
}
