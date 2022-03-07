#pragma once


// InputBoxDlg dialog

class InputBoxDlg : public CDialog
{
	DECLARE_DYNAMIC(InputBoxDlg)

public:
	class InputBoxResult
	{
	public:
		enum IBR_StatusCode
		{
			IBR_STATUS_UNINITIALIZED = 0,
			IBR_STATUS_NOT_COMPLETE,
			IBR_STATUS_CANCEL,
			IBR_STATUS_OK
		};

		/// Self-explanatory.
		IBR_StatusCode status_code;

		/// The value. This must be free'd afterwards.
		char* value;

	public:
		InputBoxResult()
		{
			value = NULL;
			Reset();
		}

		~InputBoxResult()
		{
			if (value)
				delete[] value;
		}

		void Reset()
		{
			if (value)
				delete[] value;

			status_code = IBR_STATUS_UNINITIALIZED;
			value       = NULL;
		}
	};

	struct InputBoxConfig
	{
		/// The text to display to the user.
		const char* info_text;

		/// If the user does not press OK, then this will be returned.
		char* error_value;

		/// Disable the cancel button.
		bool disable_cancel_button;

		/// Only allow numbers.
		bool only_numeric;

		/// Max input length.
		int max_length;

		/// Minimum input length.
		int min_length;

		InputBoxConfig()
		{
			Reset();
		}

		void Reset()
		{
			// Default config
			info_text             = "Please insert the desired value.";
			error_value           = NULL;
			disable_cancel_button = false;
			only_numeric          = false;
			max_length            = 256;
			min_length            = 0;
		}
	};

private:
	CEdit m_InputBox;
	CButton m_CancelButton;
	CButton m_OKButton;

	// Configuration
	InputBoxResult* m_Result;
	InputBoxConfig m_Config;

public:
	InputBoxDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~InputBoxDlg();

// Dialog Data
	enum { IDD = DIALOG_INPUT_BOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	/// @brief Opens an input box to accept user input.
	///
	/// @param result_ptr A pointer to the result struct.
	/// @param config The configuration for this input box. Can be nullptr to use default configuration.
	/// @param default_value The default value in the input box.
	///
	/// @return Returns true if OK was pressed, false if otherwise.
	static bool OpenInputBox(InputBoxResult* result_ptr, InputBoxConfig* config = nullptr, const char* default_value = nullptr);

	/// Update input box.
	void UpdateInputBox();

public:
	afx_msg void OnBnClickedInputboxOk();
	afx_msg void OnBnClickedInputboxCancel();
	afx_msg void OnInputChanged();

	afx_msg void OnClose();
};
