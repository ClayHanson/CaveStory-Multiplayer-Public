/*
  Cavestory Multiplayer API
  Copyright (C) 2021 Johnny Ledger

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
	 claim that you wrote the original software. If you use this software
	 in a product, an acknowledgment in the product documentation would be
	 appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
	 misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*!
  @file CSMAPI_MessageBoxMode.h
*/

#pragma once

#include <CSMAPI_begincode.h>

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class MessageBoxMode;

/// Function callback to call when 
typedef void(*MessageBoxBusyFunc)(void*);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// Message box button
/// @ingroup GUI
///
class CAVESTORY_MOD_API MessageBoxButton
{
	friend class MessageBoxMode;

private: // Cache variables
	struct
	{
		GUI_RECT rc_button;
		GUI_RECT rc_text;
		GUI_POINT pnt_lookup;
	} m_Cache;

public: // Button configuration

	/// What code this button returns when pressed
	///
	unsigned int m_ReturnCode;

	/// The text shown on this button. Can be NULL.
	///
	const char* m_Text;

	/// Whether this button should be on a new line. Does nothing for the first button in a button array.
	///
	short m_NewLine;

	/// If true, the message box will init with this as the selected button.
	///
	bool m_Default;

	/// If this is 'true', then the message box mode will return this button's value if ESCAPE is pressed.
	///
	bool m_Cancel;

	/// Whether this button is clickable or not
	///
	bool m_Enabled;

public: // Constructor
	inline MessageBoxButton()
	{
		memset(&m_Cache, 0, sizeof(m_Cache));

		m_ReturnCode = 0;
		m_Text = NULL;
		m_NewLine = false;
		m_Default = false;
		m_Cancel = false;
		m_Enabled = true;
	}

	MessageBoxButton(const char* pText, int iReturnCode, bool bNewLine = false, bool bDefault = false, bool bCancel = false, bool bEnabled = true);
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// Message box mode
/// @ingroup GUI
///
class CAVESTORY_MOD_API MessageBoxMode : public BaseModeInstance
{
	typedef BaseModeInstance Parent;
	friend class MessageBoxButton;

protected: // Pre-init variables
	MessageBoxBusyFunc m_BusyFunc;
	void* m_UserData;

	MessageBoxButton* m_Buttons;
	int m_ButtonCount;

public:
	/// Configuration for MessageBoxMode
	/// @ingroup GUI
	///
	struct CAVESTORY_MOD_API MessageBoxConfigStruct
	{
		/// The title of the message box.
		///
		const char* title;

		/// The text to display.
		///
		const char* text;

		/// If set to true, then the busy function will be called BEFORE the messagebox's draw function, instead of AFTER.
		///
		bool busy_func_before_draw;

		/// Perform a BaseModeInstance::GrabScreen() on messagebox exit.
		///
		bool draw_grab_on_exit;

		/// The margin of the text.
		///
		int text_margin;

		/// The title margin.
		///
		int title_margin;

		/// The button text margin.
		///
		int btn_margin;

		/// Line padding.
		///
		int line_padding;

		/// Input box configuration
		///
		struct
		{
			/// Enable the input box
			///
			bool enabled;

			/// The value the input box has.
			///
			/// Set this before running this mode to set the default value.
			/// This will be updated after the mode completes with the inputted value.
			///
			char value[1024];

			/// The minimum width of the input box.
			///
			int min_width;
		} input_box;

		/// Default constructor
		///
		MessageBoxConfigStruct()
		{
			busy_func_before_draw = true;
			draw_grab_on_exit     = true;
			title_margin          = 1;
			text_margin           = 4;
			btn_margin            = 2;
			line_padding          = 2;
			title                 = NULL;
			text                  = NULL;
			input_box.enabled     = false;
			*input_box.value      = 0;
			input_box.min_width   = 32;
		}
	} m_Config;

public: // Constructor

	/// Default constructor
	///
	inline MessageBoxMode() : m_Config()
	{
		m_BusyFunc = NULL;
		m_UserData = NULL;
		m_Buttons = NULL;
		m_ButtonCount = 0;
	}

protected: // Memory management

	/// @protected
	///
	virtual int Init();

	/// @protected
	///
	virtual int Free();

protected: // Processing

	/// @protected
	///
	virtual void ProcessLogic();

	/// @protected
	///
	virtual void ProcessMouse(GUI_POINT mouse_point);

	/// @protected
	///
	virtual void ProcessKeys();

protected: // Button management

	/// @protected
	///
	void HandleSelection();

	/// @protected
	///
	int GetShiftedButtonIndex(GUI_POINT start, GUI_POINT shift);

protected: // Text management

	/// @protected
	///
	GUI_POINT GetSpecialTextSize();

	/// @protected
	///
	void DrawSpecialText();

protected: // Pre-rendering

	/// @protected
	///
	virtual void PreRender();

protected: // Rendering

	/// @protected
	///
	virtual void Draw();

public:

	/// Special return codes
	///
	enum MessageBoxReturnCodes
	{
		/// Quit the game
		QUIT_GAME = -1,
	};

	/// @brief Show a message box.
	/// 
	/// @param pTitle The title of the message box.
	/// @param pMessage The message to display.
	/// @param pButtons A pointer to an array holding messagebox buttons.
	/// @param iButtonCount The number of buttons in pButtons.
	/// @param pBusyFunc The function to run every frame. Can be NULL.
	/// @param pUserData The user data to be passed into pBusyFunc. Can be NULL.
	/// @param pConfig The configuration for the messagebox.
	///
	/// @return Returns the selected button index, or one of MessageBoxReturnCodes .
	static int ShowMessageBox(const char* pTitle, const char* pMessage, MessageBoxButton* pButtons, int iButtonCount, MessageBoxBusyFunc pBusyFunc = NULL, void* pUserData = NULL, MessageBoxConfigStruct* pConfig = NULL);
	
	/// @brief Display a message box for this frame only.
	/// 
	/// @param pTitle The title of the message box.
	/// @param pMessage The message to display.
	/// @param pButtons A pointer to an array holding messagebox buttons.
	/// @param iButtonCount The number of buttons in pButtons.
	/// @param pConfig The configuration for the messagebox.
	///
	/// @return Returns the selected button index, or one of MessageBoxReturnCodes .
	static bool ShowMessageBoxForOneFrame(const char* pTitle, const char* pMessage, MessageBoxButton* pButtons, int iButtonCount, MessageBoxConfigStruct* pConfig = NULL);
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_endcode.h>

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// Default layouts for MessageBoxMode::ShowMessageBox() .
/// @ingroup GUI
///
namespace Default_MessageBox_ButtonLayouts
{
	/// A key table for the various modes.
	///
	enum ReturnCodes
	{
		/// LAYOUT_OK: "Ok"
		MBR_OK = 0,
		/// LAYOUT_YES_NO: "Yes"
		MBR_YES = 0,
		/// LAYOUT_YES_NO: "No"
		MBR_NO = 1,
		/// LAYOUT_RETRY_CANCEL: "Retry"
		MBR_RETRY = 0,
		/// LAYOUT_RETRY_CANCEL: "Cancel"
		MBR_CANCEL = 1,
	};

	/// OK Button
	static MessageBoxButton LAYOUT_OK[] = { MessageBoxButton("#VARIOUS__OK", ReturnCodes::MBR_OK, false, true, true) };

	/// Yes / No Buttons
	static MessageBoxButton LAYOUT_YES_NO[] = { MessageBoxButton("#VARIOUS__YES_L", ReturnCodes::MBR_YES, false, true, false), MessageBoxButton("#VARIOUS__NO", ReturnCodes::MBR_NO, false, false, true) };

	/// Retry / Cancel Buttons
	static MessageBoxButton LAYOUT_RETRY_CANCEL[] = { MessageBoxButton("#VARIOUS__RETRY_L", ReturnCodes::MBR_RETRY, false, false, false), MessageBoxButton("#VARIOUS__CANCEL_L", ReturnCodes::MBR_CANCEL, false, true, true) };
	
	/// OK / Cancle Buttons
	static MessageBoxButton LAYOUT_OK_CANCEL[] = { MessageBoxButton("#VARIOUS__OK", ReturnCodes::MBR_OK, false, true, false), MessageBoxButton("#VARIOUS__CANCEL_L", ReturnCodes::MBR_CANCEL, false, false, true) };
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
