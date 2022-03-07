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
  @file ModeInputVector.h
*/

#ifndef CAVESTORY_MOD_API_H_
#error "Please include CavestoryModAPI.h before including sub-classes."
#endif

#include <CSMAPI_begincode.h>
class CAVESTORY_MOD_API BaseModeInstance;

#pragma once

/// @addtogroup GUI
/// @{
///

/// Used for ModeInputOption
///
typedef unsigned int ModeInputSelectionId;

/// For ModeInputVector's indexes
///
typedef unsigned int ModeInputIndex;

/// Coordinate type for ModeInputOption
typedef int ModeInputOptionCoord;

/// @brief Controls the button states for non-GUI driven ModeInputOption s.
///
enum ModeInputButtonState
{
	/// Normal draw type
	MIBS_NORMAL  = 0,

	/// Button is being hovered over
	MIBS_HOVERED = 1,

	/// Button is being pressed
	MIBS_PRESSED = 2
};


/// @brief Advanced configuration for a ModeInputOption.
struct ModeInputOptionConfig
{
	/// Whether this option is clickable or not.
	/// 
	/// Useful for options that represent a tooltip area.
	bool clickable;

	/// Increment options
	///
	struct
	{
		/// How much pressing 'left' while this option is selected will increment the selection position by.
		///
		int left;

		/// How much pressing 'up' while this option is selected will increment the selection position by.
		///
		int up;

		/// How much pressing 'right' while this option is selected will increment the selection position by.
		///
		int right;

		/// How much pressing 'down' while this option is selected will increment the selection position by.
		///
		int down;
	} increment;

	/// Tooltip options
	///
	struct
	{
		/// Whether or not to open the tooltip when the user selects it with the keyboard / controller.
		///
		bool open_for_keyboard;

		/// Whether or not to duplicate the string (true), or to just store a pointer to a buffer (false).
		///
		bool allocate_text;

		/// The screen offset of the surface that this option belongs to.
		///
		GUI_POINT surface_offset;

		/// The amount of magnification for the tooltip. If this equals '-1', then the current global magnification level is used.
		///
		int magnification_lvl;

		/// The font to be used for this tooltip.
		///
		FontObject* font;
	} tooltip;

	/// Default constructor
	///
	ModeInputOptionConfig()
	{
		clickable                 = true;
		tooltip.open_for_keyboard = true;
		tooltip.allocate_text     = true;
		tooltip.surface_offset.x  = 0;
		tooltip.surface_offset.y  = 0;
		tooltip.magnification_lvl = -1;
		tooltip.font              = NULL;
		increment.left            = 1;
		increment.up              = 1;
		increment.right           = 1;
		increment.down            = 1;
	}
};

/// @brief ModeInputVector's input object
///
struct ModeInputOption
{
	/// @brief Flags for ModeInputVector options.
	enum OptionFlags
	{
		/// Auto expand the option's array width to the fullest possible extent.
		AUTO_EXPAND_WIDTH  = BIT(0),

		/// Auto expand the option's array height to the fullest possible extent.
		AUTO_EXPAND_HEIGHT = BIT(1),

		/// Subtract this option's input_rect.x value from the array width to get the option's true X position.
		START_FROM_RIGHT   = BIT(2),

		/// Subtract this option's input_rect.y value from the array height to get the option's true Y position.
		START_FROM_BOTTOM  = BIT(3),

		/// Start this option on a new line.
		START_ON_NEW_LINE  = BIT(4),

		/// Automation flags.
		AUTOMATION_FLAGS   = AUTO_EXPAND_WIDTH | AUTO_EXPAND_HEIGHT
	};

	/// The local index of this option.
	///
	ModeInputIndex index;

	/// Flags for this option.
	///
	unsigned int flags;

	/// The ID that will be returned when this option is selected / clicked on.
	///
	ModeInputSelectionId selection_id;

	/// The input vector position of this option
	///
	struct
	{
		/// Whether this option can be built into the array or not.
		///
		/// If set to false, then this option cannot be reached using a controller / the keyboard.
		bool use;

		/// X Position
		///
		ModeInputOptionCoord x;

		/// Y Position
		///
		ModeInputOptionCoord y;
		
		/// Width
		///
		ModeInputOptionCoord w;
		
		/// Height
		///
		ModeInputOptionCoord h;
	} input_rect;

	/// The calculated input vector rect for this option
	///
	struct
	{
		/// X Position
		///
		ModeInputOptionCoord x;

		/// Y Position
		///
		ModeInputOptionCoord y;
		
		/// Width
		///
		ModeInputOptionCoord w;
		
		/// Height
		///
		ModeInputOptionCoord h;
	} calculated_input_rect;

	/// The clip rect that should be accessed before this option can be processed.
	///
	struct
	{
		/// Whether to use this clip rect or not.
		///
		bool use;

		/// The rect to use.
		///
		GUI_RECT rect;
	} clip;

	/// The tooltip for this option.
	///
	struct
	{
		/// Whether this tooltip is enabled or not.
		///
		bool enabled;

		/// The tooltip text to use.
		///
		char* text;
	} tooltip;

	/// The configuration for this option.
	///
	ModeInputOptionConfig config;

	/// A pointer to the scroll this option is inside of.
	///
	ScrollAreaInfo* scroll_ptr;

	/// A pointer to this input option's on-screen rect
	///
	GUI_RECT* rect_ptr;

	/// This option's GUI element. Can be NULL.
	///
	GuiBase* gui_ptr;

	/// This option's rect. Only use this if both rect_ptr and gui_ptr are NULL.
	///
	GUI_RECT rect_plain;
};

/// @brief A helpful 2D vector class to manage mode input.
///
/// A 2D vector of ModeInputOption objects that allow mode instances to
/// manage their key & mouse input in a neat & uniform fashion.
///
class CAVESTORY_MOD_API ModeInputVector
{
private: // Mechanical variables

	/// The minimum size for this vector.
	/// @private
	///
	GUI_POINT m_MinSize;

	/// An array containing the input options
	/// @private
	///
	ModeInputOption** m_OptionList;

	/// Size of m_OptionList
	/// @private
	///
	int m_OptionCount;

	/// An array containing all unique scroll area elements.
	/// @private
	///
	ScrollAreaInfo** m_ScrollList;

	/// Size of m_ScrollList
	/// @private
	///
	int m_ScrollCount;

	/// An array containing indicies that tell which ModeInputOption should be at the given X, Y coordinates. Size is (m_ArrayWidth * m_ArrayHeight)
	/// @private
	///
	ModeInputIndex* m_Array;

	/// The width of m_Array
	/// @private
	///
	int m_ArrayWidth;

	/// The height of m_Array
	/// @private
	///
	int m_ArrayHeight;

private: // Selected variables

	/// The mode instance that owns this mode input vector.
	/// @private
	///
	BaseModeInstance* m_Owner;

	/// The selected option's index in m_OptionList.
	/// @private
	///
	int m_TooltipOption;

	/// The selected option's index in m_OptionList.
	/// @private
	///
	int m_SelectedOption;

	/// The old selected option's index in m_OptionList.
	/// @private
	///
	int m_OldSelectedOption;

	/// Whether the keyboard was used to get to m_SelectedOption.
	/// @private
	///
	bool m_UsedKeyboard;

	/// Selected position relative to m_Array
	/// @private
	///
	GUI_POINT m_SelectedPos;

	/// Whether the selected option is being processed or not.
	/// @private
	///
	bool m_IsSelectionProcessing;

private: // Lock variables

	/// Whether this vector needs to rebuild its array or not.
	/// @private
	///
	bool m_Dirty;

	/// Whether this vector can rebuild its array or not.
	///
	bool m_Locked;

public: // State variables

	/// Are we clicking on the selected option whilst hovering over it?
	/// Don't change this.
	///
	bool m_Clicking;

	/// Are we hovering over the selected option?
	/// Don't change this.
	///
	bool m_Hovering;

public: // C++ stuff

	/// @brief Constructor
	///
	ModeInputVector();

	/// @brief Deconstructor
	///
	~ModeInputVector();

protected: // Internal methods

	/// @brief Rebuild the m_Array array.
	/// @protected
	///
	/// @return Returns true on success.
	bool RebuildArray();

	/// @brief Index all options.
	/// @protected
	///
	/// @return Returns true on success.
	bool IndexOptions();

	/// @brief Allocate an option.
	/// @protected
	///
	/// @return Returns a newly allocated ModeInputOption .
	ModeInputOption* AllocateOption();

	/// @brief Find an option by ID.
	///
	/// @param iSelectionId The ID of the option.
	/// @param pIndexPtr A pointer to an int that will store the index that the option was found at. Can be NULL.
	/// 
	/// @return Returns a pointer to the option, otherwise returns NULL if it couldn't be found.
	ModeInputOption* FindOptionById(ModeInputSelectionId iSelectionId, int* pIndexPtr = NULL);

	/// @brief Get the screen rect for an option.
	///
	/// @param pOption The option to get the rect of.
	///
	/// @return Returns the rect for the option.
	inline GUI_RECT Internal_GetOptionRect(ModeInputOption* pOption);

	/// @brief The general internal method to add an option. All AddOption methods call this method.
	///
	/// @param iSelectionId The ID that should be returned by ProcessMouse or ProcessKeys if this option is selected.
	/// @param iX The X position of the input option in the vector.
	/// @param iY The Y position of the input option in the vector.
	/// @param iW The width of the input option in the vector.
	/// @param iH The height of the input option in the vector.
	/// @param pRect A pointer to the on-screen rect.
	/// @param pScroll The scroll object that this rect is controlled by. Can be NULL.
	/// @param pClipRect The clip rect for this option. Can be NULL.
	/// @param pTooltipText The tooltip text for this option. Can be NULL to disable tooltips for this option.
	/// @param iFlags The flags for this option.
	/// @param pConfig The advanced configuration to use for this option. Can be NULL to use default configuration settings.
	///
	/// @return Returns the newly allocated option.
	/// @see ModeInputOption::OptionFlags
	ModeInputOption* Internal_AddOption(ModeInputSelectionId iSelectionId, ModeInputOptionCoord iX, ModeInputOptionCoord iY, ModeInputOptionCoord iW, ModeInputOptionCoord iH, ScrollAreaInfo* pScroll, GUI_RECT* pClipRect, char* pTooltipText, ModeInputOptionConfig* pConfig, unsigned int iFlags);

public: // Public memory management

	/// @brief Set the minimum size for this vector.
	///
	/// @param iWidth The minimum width.
	/// @param iHeight The minimum height.
	void SetMinArraySize(ModeInputOptionCoord iWidth, ModeInputOptionCoord iHeight);

	/// @brief Add an option to the vector.
	/// 
	/// @param iSelectionId The ID that should be returned by ProcessMouse or ProcessKeys if this option is selected.
	/// @param iX The X position of the input option in the vector.
	/// @param iY The Y position of the input option in the vector.
	/// @param iW The width of the input option in the vector.
	/// @param iH The height of the input option in the vector.
	/// @param pRect A pointer to the on-screen rect.
	/// @param pScroll The scroll object that this rect is controlled by. Can be NULL.
	/// @param pClipRect The clip rect for this option. Can be NULL.
	/// @param pTooltipText The tooltip text for this option. Can be NULL to disable tooltips for this option.
	/// @param pConfig The advanced configuration to use for this option. Can be NULL to use default configuration settings.
	///
	/// @return Returns true on success.
	/// @warning pRect should NOT be NULL and it should be a valid pointer until this ModeInputVector is reset / deleted.
	/// @warning iX, iY, iW and iH should not be the on-screen position & extent of the input. It's the local position & extent in the input vector.
	/// @see ModeInputOption::OptionFlags
	bool AddOption(ModeInputSelectionId iSelectionId, ModeInputOptionCoord iX, ModeInputOptionCoord iY, ModeInputOptionCoord iW, ModeInputOptionCoord iH, GUI_RECT* pRect, ScrollAreaInfo* pScroll = NULL, GUI_RECT* pClipRect = NULL, const char* pTooltipText = NULL, ModeInputOptionConfig* pConfig = NULL, unsigned int iFlags = 0);

	/// @brief Add an option to the vector.
	/// 
	/// @param iSelectionId The ID that should be returned by ProcessMouse or ProcessKeys if this option is selected.
	/// @param iX The X position of the input option in the vector.
	/// @param iY The Y position of the input option in the vector.
	/// @param iW The width of the input option in the vector.
	/// @param iH The height of the input option in the vector.
	/// @param pGui A pointer to the guibase that controls this option.
	/// @param pScroll The scroll object that this rect is controlled by. Can be NULL.
	/// @param pClipRect The clip rect for this option. Can be NULL.
	/// @param pTooltipText The tooltip text for this option. Can be NULL to disable tooltips for this option.
	/// @param pConfig The advanced configuration to use for this option. Can be NULL to use default configuration settings.
	///
	/// @return Returns true on success.
	/// @warning pGui should NOT be NULL and it should be a valid pointer until this ModeInputVector is reset / deleted.
	/// @see ModeInputOption::OptionFlags
	bool AddOption(ModeInputSelectionId iSelectionId, ModeInputOptionCoord iX, ModeInputOptionCoord iY, ModeInputOptionCoord iW, ModeInputOptionCoord iH, GuiBase* pGui, ScrollAreaInfo* pScroll = NULL, GUI_RECT* pClipRect = NULL, const char* pTooltipText = NULL, ModeInputOptionConfig* pConfig = NULL, unsigned int iFlags = 0);
	
	/// @brief Add an option to the vector.
	/// 
	/// @param iSelectionId The ID that should be returned by ProcessMouse or ProcessKeys if this option is selected.
	/// @param iX The X position of the input option in the vector.
	/// @param iY The Y position of the input option in the vector.
	/// @param iW The width of the input option in the vector.
	/// @param iH The height of the input option in the vector.
	/// @param pRect The plain on-screen rect for this option.
	/// @param pScroll The scroll object that this rect is controlled by. Can be NULL.
	/// @param pClipRect The clip rect for this option. Can be NULL.
	/// @param pTooltipText The tooltip text for this option. Can be NULL to disable tooltips for this option.
	/// @param pConfig The advanced configuration to use for this option. Can be NULL to use default configuration settings.
	///
	/// @return Returns true on success.
	/// @warning pGui should NOT be NULL and it should be a valid pointer until this ModeInputVector is reset / deleted.
	/// @see ModeInputOption::OptionFlags
	bool AddOption(ModeInputSelectionId iSelectionId, ModeInputOptionCoord iX, ModeInputOptionCoord iY, ModeInputOptionCoord iW, ModeInputOptionCoord iH, GUI_RECT pRect, ScrollAreaInfo* pScroll = NULL, GUI_RECT* pClipRect = NULL, const char* pTooltipText = NULL, ModeInputOptionConfig* pConfig = NULL, unsigned int iFlags = 0);
	
	/// @brief Add an unselectable option to the vector.
	/// 
	/// @param iSelectionId The ID that should be returned by ProcessMouse or ProcessKeys if this option is selected.
	/// @param pRect A pointer to the on-screen rect.
	/// @param pScroll The scroll object that this rect is controlled by. Can be NULL.
	/// @param pClipRect The clip rect for this option. Can be NULL.
	/// @param pTooltipText The tooltip text for this option. Can be NULL to disable tooltips for this option.
	/// @param pConfig The advanced configuration to use for this option. Can be NULL to use default configuration settings.
	///
	/// @return Returns true on success.
	/// @warning pRect should NOT be NULL and it should be a valid pointer until this ModeInputVector is reset / deleted.
	/// @warning iX, iY, iW and iH should not be the on-screen position & extent of the input. It's the local position & extent in the input vector.
	/// @see ModeInputOption::OptionFlags
	bool AddOption(ModeInputSelectionId iSelectionId, GUI_RECT* pRect, ScrollAreaInfo* pScroll = NULL, GUI_RECT* pClipRect = NULL, const char* pTooltipText = NULL, ModeInputOptionConfig* pConfig = NULL, unsigned int iFlags = 0);

	/// @brief Add an unselectable option to the vector.
	/// 
	/// @param iSelectionId The ID that should be returned by ProcessMouse or ProcessKeys if this option is selected.
	/// @param iX The X position of the input option in the vector.
	/// @param iY The Y position of the input option in the vector.
	/// @param iW The width of the input option in the vector.
	/// @param iH The height of the input option in the vector.
	/// @param pGui A pointer to the guibase that controls this option.
	/// @param pScroll The scroll object that this rect is controlled by. Can be NULL.
	/// @param pClipRect The clip rect for this option. Can be NULL.
	/// @param pTooltipText The tooltip text for this option. Can be NULL to disable tooltips for this option.
	/// @param pConfig The advanced configuration to use for this option. Can be NULL to use default configuration settings.
	///
	/// @return Returns true on success.
	/// @warning pGui should NOT be NULL and it should be a valid pointer until this ModeInputVector is reset / deleted.
	/// @see ModeInputOption::OptionFlags
	bool AddOption(ModeInputSelectionId iSelectionId, GuiBase* pGui, ScrollAreaInfo* pScroll = NULL, GUI_RECT* pClipRect = NULL, const char* pTooltipText = NULL, ModeInputOptionConfig* pConfig = NULL, unsigned int iFlags = 0);
	
	/// @brief Add an unselectable option to the vector.
	/// 
	/// @param iSelectionId The ID that should be returned by ProcessMouse or ProcessKeys if this option is selected.
	/// @param iX The X position of the input option in the vector.
	/// @param iY The Y position of the input option in the vector.
	/// @param iW The width of the input option in the vector.
	/// @param iH The height of the input option in the vector.
	/// @param pRect The plain on-screen rect for this option.
	/// @param pScroll The scroll object that this rect is controlled by. Can be NULL.
	/// @param pClipRect The clip rect for this option. Can be NULL.
	/// @param pTooltipText The tooltip text for this option. Can be NULL to disable tooltips for this option.
	/// @param pConfig The advanced configuration to use for this option. Can be NULL to use default configuration settings.
	///
	/// @return Returns true on success.
	/// @warning pGui should NOT be NULL and it should be a valid pointer until this ModeInputVector is reset / deleted.
	/// @see ModeInputOption::OptionFlags
	bool AddOption(ModeInputSelectionId iSelectionId, GUI_RECT pRect, ScrollAreaInfo* pScroll = NULL, GUI_RECT* pClipRect = NULL, const char* pTooltipText = NULL, ModeInputOptionConfig* pConfig = NULL, unsigned int iFlags = 0);

	/// @brief Remove an option by ID.
	///
	/// @param iSelectionId The ID that should be removed.
	///
	/// @return Returns true on success.
	bool RemoveOption(ModeInputSelectionId iSelectionId);

	/// @brief Add a controllable scroll area.
	///
	/// @param pScroll A pointer to the ScrollAreaInfo element.
	///
	/// @return Returns true on success.
	bool AddScroll(ScrollAreaInfo* pScroll);

	/// @brief Reset everything -- Unallocates all buffers and resets all variables.
	///
	void Reset();

public: // More option methods

	/// @brief Get the selected button's determined state.
	///
	/// @param iSelectionId The ID of the option to get the state of.
	///
	/// @return Returns the option's state.
	ModeInputButtonState GetOptionState(ModeInputSelectionId iSelectionId);

	/// @brief Get the rect for an option.
	///
	/// @param iSelectionId The ID of the option.
	///
	/// @return Returns the rect for the option.
	GUI_RECT GetOptionRect(ModeInputSelectionId iSelectionId);

public: // Selection manipulation methods

	/// @brief Set the selected option.
	///
	/// @param iX The desired X position.
	/// @param iY The desired Y position.
	/// @param bUsedKeyboard Whether this should be considered as "We used the keyboard to get to this position" or "we used the mouse to get to this position".
	/// @param bOpenTooltip Whether to open the tooltip for this option as well.
	void SetSelectionByPos(int iX, int iY, bool bUsedKeyboard, bool bOpenTooltip = true);

	/// @brief Set the selected selection ID.
	///
	/// @param iSelectionId The ID that should be selected.
	/// @param bUsedKeyboard Whether this should be considered as "We used the keyboard to get to this position" or "we used the mouse to get to this position".
	///
	/// @note This sets the selection position to the top-left of the option's input rect.
	void SetSelectionById(ModeInputSelectionId iSelectionId, bool bUsedKeyboard);

	/// @brief Get the selected position.
	///
	/// @return Returns the current selected coordinates.
	GUI_POINT GetSelectedPos();

	/// @brief Get the selected ID.
	///
	/// @return Returns the current selected option ID. Returns -1 if no option is selected.
	int GetSelectedId();
	
	/// @brief Get the selected index.
	///
	/// @return Returns the current selected option index. Returns -1 if no option is selected.
	int GetSelectedIndex();

	/// @brief Self explanatory.
	///
	/// @return Returns true if the keyboard was used to get to the current selection, false otherwise.
	inline bool WasKeyboardUsed() { return m_UsedKeyboard; }

protected: // Protected tooltip methods

	/// @brief Open an option's tooltip.
	///
	/// @param pOption The option to use.
	void SetTooltip(ModeInputOption* pOption);

	/// @brief Test to see if the current tooltip is set to an option.
	///
	/// @param pOption The option to compare against.
	///
	/// @return Returns true if the option's tooltip is open, otherwise returns false.
	bool IsTooltipSetTo(ModeInputOption* pOption);

public: // Tooltip processing

	/// @brief Set the mode to use for processing.
	///
	/// @param pMode The mode to use.
	inline void SetModeOwner(BaseModeInstance* pMode) { m_Owner = pMode; }

public: // Processing methods

	/// @brief Get the array's dimensions.
	///
	/// @return Returns the array's dimensions.
	GUI_POINT GetArraySize();

	/// @brief Move the current selection.
	///
	/// @param iXM The amount to move X by.
	/// @param iYM The amount to move Y by.
	///
	/// @return Returns GuiBase::ProcessReturnType flags.
	int MoveSelection(int iXM, int iYM);
	
	/// @brief Process key input for the vector.
	///
	/// @param iSelectedOutTrg A pointer to an int that will store the selected option's ID for one frame if it is selected.
	///
	/// @return Returns GuiBase::ProcessReturnType flags.
	int ProcessKeys(int* iSelectedOutTrg);

	/// @brief Process mouse input.
	///
	/// @param iSelectedOutTrg A pointer to an int that will store the selected option's ID for one frame if it is selected.
	/// @param pMousePoint The mouse position.
	///
	/// @return Returns GuiBase::ProcessReturnType flags.
	int ProcessMouse(int* iSelectedOutTrg, GUI_POINT pMousePoint);

	/// @brief Lock the input vector from rebuilding its array
	///
	void Lock();
	
	/// @brief Allow the vector to rebuild its array if necessary
	///
	void Unlock();

public: // Debugging methods

	/// @brief Dump the input map.
	///
	void DumpMap();
};

/// @}
///

#include <CSMAPI_endcode.h>