#include <CavestoryModAPI.h>
#include "ModeInputVector.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define ARRAY_INVALID				m_OptionCount
#define TOOLTIP_INVALID				-1
#define MAX_VALUE_OF_TYPE(x)		(1 << (sizeof(ModeInputOptionCoord) << 3))

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ModeInputVector::ModeInputVector()
{
	m_OptionList  = NULL;
	m_Owner       = NULL;
	m_Array       = NULL;
	m_ScrollList  = NULL;
	m_Locked      = false;
	m_MinSize     = GUI_POINT(0, 0);
	
	Reset();
}

ModeInputVector::~ModeInputVector()
{
	Reset();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ModeInputVector::RebuildArray()
{
	if (m_Locked)
	{
		m_Dirty = true;
		return true;
	}

	int i, j, x, y, w, h = 0;
	bool auto_flags      = false;

	// Free the old lookup array
	if (m_Array)
	{
		free(m_Array);
		m_Array = NULL;
	}

	// Free the array variables
	m_ArrayWidth  = m_MinSize.x;
	m_ArrayHeight = m_MinSize.y;

	// We're done if we have no option list
	if (!m_OptionList)
		return true;

	// First pass: Determine the size of m_Array
	for (i = 0; i < m_OptionCount; i++)
	{
		ModeInputOption* pOption = m_OptionList[i];

		// Skip options that are not configured to be built into the array.
		if (!pOption->input_rect.use)
			continue;

		// Get this option's true array rect
		w = pOption->input_rect.w;
		h = pOption->input_rect.h;
		x = pOption->flags & ModeInputOption::OptionFlags::START_FROM_RIGHT  ? 0 : pOption->input_rect.x;
		y = pOption->flags & ModeInputOption::OptionFlags::START_FROM_BOTTOM ? 0 : pOption->input_rect.y;

		if (pOption->flags & ModeInputOption::OptionFlags::START_ON_NEW_LINE)
			y = m_ArrayHeight - (h - 1);

		// Record the biggest width
		if (!(pOption->flags & ModeInputOption::OptionFlags::AUTO_EXPAND_WIDTH) && x + w > m_ArrayWidth)
			m_ArrayWidth = x + w;
		
		// Record the biggest height
		if (!(pOption->flags & ModeInputOption::OptionFlags::AUTO_EXPAND_HEIGHT) && y + h > m_ArrayHeight)
			m_ArrayHeight = y + h;
	}

	// Allocate the array
	m_Array = (ModeInputIndex*)malloc(sizeof(ModeInputIndex) * ((m_ArrayWidth + 1) * (m_ArrayHeight + 1)));

	// Set memory
	for (i = 0; i < m_ArrayWidth * m_ArrayHeight; i++)
		m_Array[i] = ARRAY_INVALID;

	// Second pass: Position all of the buttons inside of m_Array
	for (i = 0; i < m_OptionCount; i++)
	{
		ModeInputOption* pOption = m_OptionList[i];

		// Skip options that are not configured to be built into the array.
		if (!pOption->input_rect.use)
			continue;

		if (pOption->flags & ModeInputOption::OptionFlags::AUTOMATION_FLAGS)
		{
			auto_flags = true;
			continue;
		}

		// Get this option's true array rect
		w = pOption->input_rect.w;
		h = pOption->input_rect.h;
		x = pOption->flags & ModeInputOption::OptionFlags::START_FROM_RIGHT  ? m_ArrayWidth - 1 - pOption->input_rect.x : pOption->input_rect.x;
		y = pOption->flags & ModeInputOption::OptionFlags::START_FROM_BOTTOM ? m_ArrayHeight - 1 - pOption->input_rect.y : pOption->input_rect.y;
		x = CLAMP(x, 0, m_ArrayWidth - w);
		y = CLAMP(y, 0, m_ArrayHeight - h);

		// Set this option's calculated input rect
		pOption->calculated_input_rect.x = x;
		pOption->calculated_input_rect.y = y;
		pOption->calculated_input_rect.w = w;
		pOption->calculated_input_rect.h = h;

		// Populate it
		for (j = 0; j < w * h; j++)
			m_Array[(m_ArrayWidth * (y + (j / w))) + (x + (j % w))] = (ModeInputIndex)i;
	}

	// Done if there are no auto flags to build
	if (!auto_flags)
		return true;

	// Initialize more variables
	ModeInputIndex match_value, v = 0;
	int w_leftover, h_leftover, k = 0;
	bool invalid                  = false;

	// Third pass: Process automation flags
	for (i = 0; i < m_OptionCount; i++)
	{
		ModeInputOption* pOption = m_OptionList[i];

		// Skip options that are not configured to be built into the array.
		if (!pOption->input_rect.use || !(pOption->flags & ModeInputOption::OptionFlags::AUTOMATION_FLAGS))
			continue;

		// Get this option's true array rect
		w = 1;
		h = 1;
		x = pOption->flags & ModeInputOption::OptionFlags::START_FROM_RIGHT ? m_ArrayWidth - 1 - pOption->input_rect.x : pOption->input_rect.x;
		y = pOption->flags & ModeInputOption::OptionFlags::START_FROM_BOTTOM ? m_ArrayHeight - 1 - pOption->input_rect.y : pOption->input_rect.y;
		x = CLAMP(x, 0, m_ArrayWidth - 1);
		y = CLAMP(y, 0, m_ArrayHeight - 1);

		// Get the match value
		match_value = m_Array[(m_ArrayWidth * y) + x];

		// Calculate how much space we have to calculate
		w_leftover = (pOption->flags & ModeInputOption::OptionFlags::AUTO_EXPAND_WIDTH) ? m_ArrayWidth - 1 - x : 1;
		h_leftover = (pOption->flags & ModeInputOption::OptionFlags::AUTO_EXPAND_HEIGHT) ? m_ArrayHeight - 1 - y : 1;
		w          = w_leftover;
		h          = h_leftover;

		// Set the first step to 'invalid'
		invalid = false;

		// Expand the sizes
		for (j = 0; j < w_leftover; j++)
		{
			// Check the array indicies along the Y axis
			for (k = 0; k < h_leftover; k++)
			{
				v = m_Array[(m_ArrayWidth * (y + k)) + (x + j)];

				// If a mismatch occurs, stop here
				if (v != match_value)
				{
					if (j == 0)
						invalid = true;

					if (w > j)
						w = j - 1;

					if (h > k)
						h = k - 1;

					break;
				}
			}

			// Invalid steps will be the terminator
			if (invalid)
				break;
		}

		x = CLAMP(x, 0, m_ArrayWidth - w);
		y = CLAMP(y, 0, m_ArrayHeight - h);

		// Set this option's calculated input rect
		pOption->calculated_input_rect.x = x;
		pOption->calculated_input_rect.y = y;
		pOption->calculated_input_rect.w = w;
		pOption->calculated_input_rect.h = h;

		// Populate it
		for (j = 0; j < w * h; j++)
			m_Array[(m_ArrayWidth * (y + (j / w))) + (x + (j % w))] = (ModeInputIndex)i;
	}

	// Done!
	return true;
}

bool ModeInputVector::IndexOptions()
{
	if (m_OptionList)
		return false;

	// Re-index everything
	for (int i = 0; i < m_OptionCount; i++)
		m_OptionList[i]->index = i;

	// Done!
	return true;
}

ModeInputOption* ModeInputVector::AllocateOption()
{
	bool bWasSelectionInvalid = (m_SelectedOption == ARRAY_INVALID);

	if (!m_OptionList)
	{
		// Allocate a list
		m_OptionCount = 1;
		m_OptionList  = (ModeInputOption**)malloc(sizeof(ModeInputOption*) * m_OptionCount);
	}
	else
	{
		// Resize the existing list
		m_OptionCount += 1;
		m_OptionList   = (ModeInputOption**)realloc((void*)m_OptionList, sizeof(ModeInputOption*) * m_OptionCount);
	}

	ModeInputOption* pNewOption     = new ModeInputOption();
	m_OptionList[m_OptionCount - 1] = pNewOption;

	// Configure it to defaults
	pNewOption->index          = m_OptionCount - 1;
	pNewOption->input_rect.use = true;
	pNewOption->input_rect.x   = 0;
	pNewOption->input_rect.y   = 0;
	pNewOption->input_rect.w   = 0;
	pNewOption->input_rect.h   = 0;
	pNewOption->selection_id   = 0;
	pNewOption->scroll_ptr     = NULL;
	pNewOption->gui_ptr        = NULL;
	pNewOption->rect_ptr       = NULL;

	// Configure the clip rect
	pNewOption->clip.use     = false;
	pNewOption->clip.rect.x  = 0;
	pNewOption->clip.rect.y  = 0;
	pNewOption->clip.rect.w  = 0;
	pNewOption->clip.rect.h  = 0;

	// Configure the calculated input rect
	pNewOption->calculated_input_rect.x = 0;
	pNewOption->calculated_input_rect.y = 0;
	pNewOption->calculated_input_rect.w = 0;
	pNewOption->calculated_input_rect.h = 0;
	
	// Configure the tooltip
	pNewOption->tooltip.enabled = false;
	pNewOption->tooltip.text    = NULL;

	// Set default configuration for the config
	pNewOption->config.tooltip.open_for_keyboard = true;
	pNewOption->config.tooltip.allocate_text     = true;
	pNewOption->config.clickable                 = true;
	pNewOption->config.tooltip.surface_offset.x  = 0;
	pNewOption->config.tooltip.surface_offset.y  = 0;
	pNewOption->config.increment.up              = 1;
	pNewOption->config.increment.left            = 1;
	pNewOption->config.increment.down            = 1;
	pNewOption->config.increment.right           = 1;

	// Restore invalidity
	if (bWasSelectionInvalid)
		m_SelectedOption = ARRAY_INVALID;

	// Done! Return the newly allocated object
	return pNewOption;
}

ModeInputOption* ModeInputVector::FindOptionById(ModeInputSelectionId iSelectionId, int* pIndexPtr)
{
	// Attempt to find it!
	for (int i = 0; i < m_OptionCount; i++)
	{
		ModeInputOption* pOption = m_OptionList[i];

		// Obviously skip mismatches
		if (pOption->selection_id != iSelectionId)
			continue;

		// Set the index ptr if available
		if (pIndexPtr)
			*pIndexPtr = i;

		// Found it!
		return pOption;
	}

	// Couldn't find it
	return NULL;
}

GUI_RECT ModeInputVector::Internal_GetOptionRect(ModeInputOption* pOption)
{
	return *(pOption->rect_ptr ? pOption->rect_ptr : (pOption->gui_ptr ? (pOption->gui_ptr->has_special_hotspot_rect ? &pOption->gui_ptr->hotspot_rect : &pOption->gui_ptr->rect) : &pOption->rect_plain));
}

ModeInputOption* ModeInputVector::Internal_AddOption(ModeInputSelectionId iSelectionId, ModeInputOptionCoord iX, ModeInputOptionCoord iY, ModeInputOptionCoord iW, ModeInputOptionCoord iH, ScrollAreaInfo* pScroll, GUI_RECT* pClipRect, char* pTooltipText, ModeInputOptionConfig* pConfig, unsigned int iFlags)
{
	// Allocate & configure the option
	ModeInputOption* pOption = AllocateOption();
	pOption->flags           = iFlags;
	pOption->selection_id    = iSelectionId;
	pOption->scroll_ptr      = pScroll;
	pOption->input_rect.x    = iX;
	pOption->input_rect.y    = iY;
	pOption->input_rect.w    = iW;
	pOption->input_rect.h    = iH;
	pOption->clip.use        = (pClipRect != NULL);
	pOption->clip.rect       = (pClipRect ? *pClipRect : GUI_RECT(0, 0, 0, 0));
	pOption->tooltip.enabled = (pTooltipText != NULL);

	// If configuration pointer exists, copy it into the option's config variable.
	if (pConfig)
		memcpy(&pOption->config, pConfig, sizeof(ModeInputOptionConfig));

	// DEtermine whether to allocate the text string, or to just point to it
	pOption->tooltip.text = (pTooltipText ? (pOption->config.tooltip.allocate_text ? strdup(pTooltipText) : pTooltipText) : NULL);

	return pOption;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ModeInputVector::SetMinArraySize(ModeInputOptionCoord iWidth, ModeInputOptionCoord iHeight)
{
	m_MinSize.x = iWidth;
	m_MinSize.y = iHeight;
}

bool ModeInputVector::AddOption(ModeInputSelectionId iSelectionId, ModeInputOptionCoord iX, ModeInputOptionCoord iY, ModeInputOptionCoord iW, ModeInputOptionCoord iH, GUI_RECT* pRect, ScrollAreaInfo* pScroll, GUI_RECT* pClipRect, const char* pTooltipText, ModeInputOptionConfig* pConfig, unsigned int iFlags)
{
	// Validate rect
	if (!pRect)
	{
		printf(__FUNCTION__ "() - pRect shouldn't be NULL!\r\n");
		return false;
	}

	// Clamp values to acceptable bounds
	iX = MAX(iX, 0);
	iY = MAX(iY, 0);
	iW = MAX(iW, 1);
	iH = MAX(iH, 1);

	// Allocate & configure the option
	ModeInputOption* pOption = Internal_AddOption(iSelectionId, iX, iY, iW, iH, pScroll, pClipRect, (char*)pTooltipText, pConfig, iFlags);
	pOption->rect_ptr        = pRect;

	// Rebuild the array
	RebuildArray();

	// Done!
	return true;
}

bool ModeInputVector::AddOption(ModeInputSelectionId iSelectionId, ModeInputOptionCoord iX, ModeInputOptionCoord iY, ModeInputOptionCoord iW, ModeInputOptionCoord iH, GuiBase* pGui, ScrollAreaInfo* pScroll, GUI_RECT* pClipRect, const char* pTooltipText, ModeInputOptionConfig* pConfig, unsigned int iFlags)
{
	// Validate gui
	if (!pGui)
	{
		printf(__FUNCTION__ "() - pGui shouldn't be NULL!\r\n");
		return false;
	}

	// Clamp values to acceptable bounds
	iX = MAX(iX, 0);
	iY = MAX(iY, 0);
	iW = MAX(iW, 1);
	iH = MAX(iH, 1);

	// Allocate & configure the option
	ModeInputOption* pOption = Internal_AddOption(iSelectionId, iX, iY, iW, iH, pScroll, pClipRect, (char*)pTooltipText, pConfig, iFlags);
	pOption->gui_ptr         = pGui;

	// Rebuild the array
	RebuildArray();

	// Done!
	return true;
}

bool ModeInputVector::AddOption(ModeInputSelectionId iSelectionId, ModeInputOptionCoord iX, ModeInputOptionCoord iY, ModeInputOptionCoord iW, ModeInputOptionCoord iH, GUI_RECT pRect, ScrollAreaInfo* pScroll, GUI_RECT* pClipRect, const char* pTooltipText, ModeInputOptionConfig* pConfig, unsigned int iFlags)
{
	// Clamp values to acceptable bounds
	iX = MAX(iX, 0);
	iY = MAX(iY, 0);
	iW = MAX(iW, 1);
	iH = MAX(iH, 1);

	// Allocate & configure the option
	ModeInputOption* pOption = Internal_AddOption(iSelectionId, iX, iY, iW, iH, pScroll, pClipRect, (char*)pTooltipText, pConfig, iFlags);
	pOption->rect_plain      = pRect;

	// Rebuild the array
	RebuildArray();

	// Done!
	return true;
}

bool ModeInputVector::AddOption(ModeInputSelectionId iSelectionId, GUI_RECT* pRect, ScrollAreaInfo* pScroll, GUI_RECT* pClipRect, const char* pTooltipText, ModeInputOptionConfig* pConfig, unsigned int iFlags)
{
	// Validate rect
	if (!pRect)
	{
		printf(__FUNCTION__ "() - pRect shouldn't be NULL!\r\n");
		return false;
	}

	// Allocate & configure the option
	ModeInputOption* pOption = Internal_AddOption(iSelectionId, 0, 0, 0, 0, pScroll, pClipRect, (char*)pTooltipText, pConfig, iFlags);
	pOption->input_rect.use  = false;
	pOption->rect_ptr        = pRect;

	// Done!
	return true;
}

bool ModeInputVector::AddOption(ModeInputSelectionId iSelectionId, GuiBase* pGui, ScrollAreaInfo* pScroll, GUI_RECT* pClipRect, const char* pTooltipText, ModeInputOptionConfig* pConfig, unsigned int iFlags)
{
	// Validate gui
	if (!pGui)
	{
		printf(__FUNCTION__ "() - pGui shouldn't be NULL!\r\n");
		return false;
	}

	// Allocate & configure the option
	ModeInputOption* pOption = Internal_AddOption(iSelectionId, 0, 0, 0, 0, pScroll, pClipRect, (char*)pTooltipText, pConfig, iFlags);
	pOption->input_rect.use  = false;
	pOption->gui_ptr         = pGui;

	// Done!
	return true;
}

bool ModeInputVector::AddOption(ModeInputSelectionId iSelectionId, GUI_RECT pRect, ScrollAreaInfo* pScroll, GUI_RECT* pClipRect, const char* pTooltipText, ModeInputOptionConfig* pConfig, unsigned int iFlags)
{
	// Allocate & configure the option
	ModeInputOption* pOption = Internal_AddOption(iSelectionId, 0, 0, 0, 0, pScroll, pClipRect, (char*)pTooltipText, pConfig, iFlags);
	pOption->input_rect.use  = false;
	pOption->rect_plain      = pRect;

	// Done!
	return true;
}

bool ModeInputVector::RemoveOption(ModeInputSelectionId iSelectionId)
{
	ModeInputOption* pOption = NULL;
	bool bRebuildArray       = false;
	int i                    = 0;

	// Seek out & destroy
	for (i = 0; i < m_OptionCount; i++)
	{
		pOption = m_OptionList[i];

		// If we find it, then break.
		if (pOption->selection_id == iSelectionId)
			break;
	}

	// Didn't find it.
	if (i == m_OptionCount)
		return false;

	// Free memory
	if (pOption->tooltip.enabled && pOption->tooltip.text)
	{
		if (pOption->config.tooltip.allocate_text)
			free(pOption->tooltip.text);

		pOption->tooltip.text = NULL;
	}

	// If we're not using the input rect (aka we're not being built into the array), then we shouldn't rebuild the array.
	bRebuildArray = pOption->input_rect.use;

	// Erase this entry
	LIST_ERASE(m_OptionList, m_OptionCount, ModeInputOption*, i);

	// Shrink the array
	--m_OptionCount;
	if (m_OptionCount <= 0)
	{
		// Completely free it
		free(m_OptionList);
		m_OptionList  = NULL;
		m_OptionCount = 0;
	}
	else
	{
		// Just shrink it
		m_OptionList = (ModeInputOption**)realloc((void*)m_OptionList, sizeof(ModeInputOption*) * m_OptionCount);
	}

	// Re-index options
	IndexOptions();

	// Rebuild the array
	if (bRebuildArray)
		RebuildArray();

	// Done!
	return true;
}

bool ModeInputVector::AddScroll(ScrollAreaInfo* pScroll)
{
	if (!m_ScrollList)
	{
		// Allocate a list
		m_ScrollCount = 1;
		m_ScrollList  = (ScrollAreaInfo**)malloc(sizeof(ScrollAreaInfo*) * m_ScrollCount);
	}
	else
	{
		// Resize the existing list
		m_ScrollCount += 1;
		m_ScrollList   = (ScrollAreaInfo**)realloc((void*)m_ScrollList, sizeof(ScrollAreaInfo*) * m_ScrollCount);
	}

	m_ScrollList[m_ScrollCount - 1] = pScroll;

	return true;
}

void ModeInputVector::Reset()
{
	// Free the option list
	if (m_OptionList)
	{
		for (int i = 0; i < m_OptionCount; i++)
		{
			ModeInputOption* pOption = m_OptionList[i];

			// Free the allocated tooltip text for this option
			if (pOption->tooltip.enabled && pOption->tooltip.text && pOption->config.tooltip.allocate_text)
				free(pOption->tooltip.text);

			delete pOption;
		}

		free(m_OptionList);
		m_OptionList = NULL;
	}

	if (m_ScrollList)
	{
		free(m_ScrollList);
		m_ScrollList = NULL;
	}

	// Free the lookup array
	if (m_Array)
	{
		free(m_Array);
		m_Array = NULL;
	}

	// Reset everything else
	m_IsSelectionProcessing = false;
	m_Clicking              = false;
	m_Hovering              = false;
	m_OptionCount           = 0;
	m_ScrollCount           = 0;
	m_ArrayWidth            = 0;
	m_ArrayHeight           = 0;
	m_SelectedPos           = GUI_POINT(0, 0);
	m_SelectedOption        = ARRAY_INVALID;
	m_OldSelectedOption     = ARRAY_INVALID;
	m_TooltipOption         = TOOLTIP_INVALID;
	m_UsedKeyboard          = false;
	m_Dirty                 = false;

	// Reset the tooltip
	SetTooltip(NULL);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ModeInputButtonState ModeInputVector::GetOptionState(ModeInputSelectionId iSelectionId)
{
	if (m_SelectedOption == ARRAY_INVALID || m_OptionList[m_SelectedOption]->selection_id != iSelectionId)
		return ModeInputButtonState::MIBS_NORMAL;

	return (m_Clicking && m_Hovering ? ModeInputButtonState::MIBS_PRESSED : ModeInputButtonState::MIBS_HOVERED);
}

GUI_RECT ModeInputVector::GetOptionRect(ModeInputSelectionId iSelectionId)
{
	ModeInputOption* pOption = FindOptionById(iSelectionId);

	return (!pOption ? GUI_RECT(0, 0, 0, 0) : Internal_GetOptionRect(pOption));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ModeInputVector::SetSelectionByPos(int iX, int iY, bool bUsedKeyboard, bool bOpenTooltip)
{
	if (m_Array[(iY * m_ArrayWidth) + iX] == ARRAY_INVALID)
		return;

	m_SelectedOption = m_Array[(iY * m_ArrayWidth) + iX];
	m_SelectedPos.x  = m_OptionList[m_SelectedOption]->calculated_input_rect.x;
	m_SelectedPos.y  = m_OptionList[m_SelectedOption]->calculated_input_rect.y;
	m_UsedKeyboard   = bUsedKeyboard;

	if (bOpenTooltip && m_SelectedOption != ARRAY_INVALID)
		SetTooltip(m_OptionList[m_SelectedOption]);
}

void ModeInputVector::SetSelectionById(ModeInputSelectionId iSelectionId, bool bUsedKeyboard)
{
	ModeInputOption* pOption = FindOptionById(iSelectionId, &m_SelectedOption);

	// If there is no option by that ID, then do nothing.
	if (!pOption)
		return;

	m_SelectedPos.x  = pOption->calculated_input_rect.x;
	m_SelectedPos.y  = pOption->calculated_input_rect.y;
	m_UsedKeyboard   = bUsedKeyboard;
}

GUI_POINT ModeInputVector::GetSelectedPos()
{
	return m_SelectedPos;
}

int ModeInputVector::GetSelectedId()
{
	return (m_SelectedOption >= m_OptionCount || m_SelectedOption == -1 || !m_OptionList ? -1 : m_OptionList[m_SelectedOption]->selection_id);
}

int ModeInputVector::GetSelectedIndex()
{
	return (m_SelectedPos.y * m_ArrayWidth) + m_SelectedPos.x;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ModeInputVector::SetTooltip(ModeInputOption* pOption)
{
	if (!m_Owner)
		return;

	// Clear the tooltip if pOption is NULL.
	if (!pOption)
	{
		// Reset to default
		if (m_TooltipOption != TOOLTIP_INVALID)
			m_TooltipOption = TOOLTIP_INVALID;

		// Close the tooltip
		if (m_Owner->IsTooltipOpen())
			m_Owner->CloseTooltip(false);

		return;
	}

	// Sanity check
	if (!pOption->tooltip.enabled || !pOption->tooltip.text)
		return;

	// Close the old tooltip if we do not own it
	if (m_TooltipOption != pOption->selection_id && m_Owner->IsTooltipOpen())
		m_Owner->CloseTooltip(false);

	// Set the new tooltip ID.
	m_TooltipOption = pOption->index;

	// Determine the draw position for the tooltip.
	GUI_POINT pTooltipPos = (m_UsedKeyboard ? (GUI_POINT(Internal_GetOptionRect(pOption).x * magnification, Internal_GetOptionRect(pOption).bottom() * magnification) + (pOption->config.tooltip.surface_offset * magnification)) : Mouse::GetMousePosition() + GUI_POINT(15, 21));

	// Determine whether we need to re-position the tooltip, or just create one.
	if (!m_Owner->IsTooltipOpen())
	{
		// Create the new tooltip
		if (pOption->config.tooltip.magnification_lvl != -1)
			m_Owner->CreateTextureTooltip(pTooltipPos, (pOption->config.tooltip.font ? pOption->config.tooltip.font : BaseModeInstance::GetDefaultFont()), pOption->tooltip.text, 5000, pOption->config.tooltip.magnification_lvl);
		else
			m_Owner->CreateTooltip(pTooltipPos, (pOption->config.tooltip.font ? pOption->config.tooltip.font : BaseModeInstance::GetDefaultFont()), pOption->tooltip.text);
	}
	else
	{
		// Re-position the tooltip
		m_Owner->SetTooltipPosition(pTooltipPos);
	}
}

bool ModeInputVector::IsTooltipSetTo(ModeInputOption* pOption)
{
	if (!pOption)
		return (m_TooltipOption == -1);

	return (m_TooltipOption == pOption->index);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GUI_POINT ModeInputVector::GetArraySize()
{
	if (!m_OptionList)
		return GUI_POINT(0, 0);

	// If the array is already allocated, then we can safely assume that it's also been sized properly.
	if (m_Array)
		return GUI_POINT(m_ArrayWidth, m_ArrayHeight);

	// Calculate the projected size.
	int i, j, x, y, w, h = 0;
	GUI_POINT pReturnSize;

	// Free the array variables
	pReturnSize.x = m_MinSize.x;
	pReturnSize.y = m_MinSize.y;

	// Determine the size of m_Array
	for (i = 0; i < m_OptionCount; i++)
	{
		ModeInputOption* pOption = m_OptionList[i];

		// Skip options that are not configured to be built into the array.
		if (!pOption->input_rect.use)
			continue;

		// Get this option's true array rect
		w = pOption->input_rect.w;
		h = pOption->input_rect.h;
		x = pOption->flags & ModeInputOption::OptionFlags::START_FROM_RIGHT  ? 0 : pOption->input_rect.x;
		y = pOption->flags & ModeInputOption::OptionFlags::START_FROM_BOTTOM ? 0 : pOption->input_rect.y;

		if (pOption->flags & ModeInputOption::OptionFlags::START_ON_NEW_LINE)
			y = pReturnSize.y - (h - 1);

		// Record the biggest width
		if (!(pOption->flags & ModeInputOption::OptionFlags::AUTO_EXPAND_WIDTH) && x + w > pReturnSize.x)
			pReturnSize.x = x + w;
		
		// Record the biggest height
		if (!(pOption->flags & ModeInputOption::OptionFlags::AUTO_EXPAND_HEIGHT) && y + h > pReturnSize.y)
			pReturnSize.y = y + h;
	}

	// Done!
	return pReturnSize;
}

int ModeInputVector::MoveSelection(int iXM, int iYM)
{
	if (!m_Array)
		return 0;

	int iRetFlags = 0;

	// Skip if we aren't going to move anywhere
	if (!iXM && !iYM)
		return iRetFlags;

	if (iXM > 1 || iXM < -1 || iYM > 1 || iYM < -1)
	{
		int iAddX = -(CLAMP(iXM, -1, 1));
		int iAddY = -(CLAMP(iYM, -1, 1));

		while (iXM || iYM)
		{
			iRetFlags |= MoveSelection(CLAMP(iXM, -1, 1), CLAMP(iYM, -1, 1));
			iXM       += iAddX;
			iYM       += iAddY;
		}

		return iRetFlags;
	}

	// Handle using keyboard after mouse unselects an option
	if (!m_UsedKeyboard && m_SelectedOption == ARRAY_INVALID && m_OldSelectedOption != ARRAY_INVALID)
	{
		m_SelectedOption = m_OldSelectedOption;
		m_SelectedPos.x  = m_OptionList[m_SelectedOption]->calculated_input_rect.x;
		m_SelectedPos.y  = m_OptionList[m_SelectedOption]->calculated_input_rect.y;
	}

	// Determine the move amounts
	int iTempX = m_SelectedPos.x + iXM;
	int iTempY = m_SelectedPos.y + iYM;

	// Remove from owner
	if (m_Owner)
		m_Owner->RemoveKey(gKeyLeft | gKeyUp | gKeyRight | gKeyDown);

	// Ignore key presses
	iRetFlags |= GuiBase::ProcessReturnType::PRT_IGNORE_KEYS;

	// Handle using keyboard after mouse unselects an option
	if (!m_UsedKeyboard && m_SelectedOption == ARRAY_INVALID && m_OldSelectedOption != ARRAY_INVALID)
	{
		m_SelectedOption = m_OldSelectedOption;
		m_SelectedPos.x  = m_OptionList[m_SelectedOption]->calculated_input_rect.x;
		m_SelectedPos.y  = m_OptionList[m_SelectedOption]->calculated_input_rect.y;

		// Set option selection
		m_SelectedOption = m_Array[(m_ArrayWidth * m_SelectedPos.y) + m_SelectedPos.x];
		m_UsedKeyboard   = true;

		iRetFlags |= GuiBase::ProcessReturnType::PRT_DIRTY;

		return iRetFlags;
	}

	// Clamp the values to acceptable lengths
	if (iTempX < 0)					iTempX = m_ArrayWidth - 1;
	if (iTempX >= m_ArrayWidth)		iTempX = 0;
	if (iTempY < 0)					iTempY = m_ArrayHeight - 1;
	if (iTempY >= m_ArrayHeight)	iTempY = 0;

	// If the lookup array says the X and Y positions we've landed on are invalid, then look for a non-invalid coordinate
	// Additionally, if it's the same button we were on, do the same thing
	if (m_Array[(m_ArrayWidth * iTempY) + iTempX] == ARRAY_INVALID || m_Array[(m_ArrayWidth * iTempY) + iTempX] == m_SelectedOption)
	{
		while (m_Array[(m_ArrayWidth * iTempY) + iTempX] == ARRAY_INVALID || m_Array[(m_ArrayWidth * iTempY) + iTempX] == m_SelectedOption)
		{
			// Break if we reach our previous position again
			if (iTempX == m_SelectedPos.x && iTempY == m_SelectedPos.y)
				break;

			// Move the temporary variables around
			iTempX += iXM;
			iTempY += iYM;

			// Clamp them to acceptable values
			if (iTempX < 0)					iTempX = m_ArrayWidth - 1;
			if (iTempX >= m_ArrayWidth)		iTempX = 0;
			if (iTempY < 0)					iTempY = m_ArrayHeight - 1;
			if (iTempY >= m_ArrayHeight)	iTempY = 0;
		}
	}

	// Test to see if we're dirty by comparing the new values to the old values
	if (m_SelectedPos.x != iTempX || m_SelectedPos.y != iTempY)
	{
		// We're dirty. Set new values.
		m_SelectedPos.x = iTempX;
		m_SelectedPos.y = iTempY;

		// Set option selection
		m_SelectedOption = m_Array[(m_ArrayWidth * m_SelectedPos.y) + m_SelectedPos.x];
		m_UsedKeyboard   = true;

		// Try to open the tooltip
		if (m_SelectedOption != ARRAY_INVALID)
		{
			if (m_OptionList[m_SelectedOption]->tooltip.enabled && m_OptionList[m_SelectedOption]->config.tooltip.open_for_keyboard)
				SetTooltip(m_OptionList[m_SelectedOption]);
			else
				SetTooltip(NULL);
		}

		// Set dirty flag
		iRetFlags |= GuiBase::ProcessReturnType::PRT_DIRTY | GuiBase::ProcessReturnType::PRT_CHANGED;
	}

	return iRetFlags;
}

int ModeInputVector::ProcessKeys(int* iSelectedOutTrg)
{
	ModeInputOption* pSelected = NULL;
	int iRetFlags              = 0;

	// Set it immediately
	*iSelectedOutTrg = -1;

	// Process the selected option first and foremost
	//
	// Having m_IsSelectionProcessing set to 'true' assumes that both pSelected and pSelected->gui_ptr are valid pointers
	if (m_IsSelectionProcessing)
	{
		// Get the currently selected option
		pSelected  = m_OptionList[m_SelectedOption];
		iRetFlags |= pSelected->gui_ptr->processKeys();

		if (!pSelected->gui_ptr->isSelected())
		{
			// The element is no longer selected; exit out of processing mode
			m_IsSelectionProcessing = false;
			iRetFlags              |= GuiBase::ProcessReturnType::PRT_DIRTY;
		}

		return iRetFlags;
	}
	
	// Move the cursor around
	if (gKeyTrg & (gKeyLeft | gKeyUp | gKeyRight | gKeyDown) && !m_Clicking)
	{
		ModeInputOption* pCurrentOption = (m_SelectedOption != ARRAY_INVALID && m_SelectedOption < m_OptionCount ? m_OptionList[m_SelectedOption] : NULL);
		int iMoveX                      = ((gKeyTrg & gKeyLeft) ? -(pCurrentOption == NULL ? 1 : pCurrentOption->config.increment.left) : ((gKeyTrg & gKeyRight) ? (pCurrentOption == NULL ? 1 : pCurrentOption->config.increment.right) : 0));
		int iMoveY                      = ((gKeyTrg & gKeyUp) ? -(pCurrentOption == NULL ? 1 : pCurrentOption->config.increment.up) : ((gKeyTrg & gKeyDown) ? (pCurrentOption == NULL ? 1 : pCurrentOption->config.increment.down) : 0));

		// Remove key presses
		gKey    &= ~(gKeyLeft | gKeyUp | gKeyRight | gKeyDown);
		gKeyTrg &= ~(gKeyLeft | gKeyUp | gKeyRight | gKeyDown);

		// Remove from owner
		if (m_Owner)
			m_Owner->RemoveKey(gKeyLeft | gKeyUp | gKeyRight | gKeyDown);

		// Ignore key presses
		iRetFlags |= GuiBase::ProcessReturnType::PRT_IGNORE_KEYS;
		iRetFlags |= MoveSelection(iMoveX, iMoveY);
	}

	// Handle option selection
	if (m_UsedKeyboard && !(gKey & gKeyOk) && m_Clicking)
	{
		// Remove key pressed
		gKey    &= ~gKeyOk;
		gKeyTrg &= ~gKeyOk;

		// Remove from owner
		if (m_Owner)
			m_Owner->RemoveKey(gKeyOk);

		// Ignore key presses
		iRetFlags |= GuiBase::ProcessReturnType::PRT_IGNORE_KEYS;

		// Get the selected option
		pSelected = (m_SelectedOption == ARRAY_INVALID ? NULL : m_OptionList[m_SelectedOption]);

		// Set it as selected
		if (pSelected)
		{
			// Special processing for options with GUI elements
			if (pSelected->gui_ptr)
			{
				// Close the annoying tooltip
				if (m_Owner && pSelected->tooltip.enabled && m_Owner->IsTooltipOpen())
					m_Owner->CloseTooltip(false);

				// Set it as 'selected'
				pSelected->gui_ptr->setSelected(true);

				// If it is actually selected, then set our own variables
				if (pSelected->gui_ptr->isSelected())
					m_IsSelectionProcessing = true;
			}

			// Set selected ID
			iRetFlags |= GuiBase::ProcessReturnType::PRT_DIRTY;

			// Only select this option if we can
			if (m_OptionList[m_SelectedOption]->config.clickable)
			{
				*iSelectedOutTrg = pSelected->selection_id;
				iRetFlags       |= GuiBase::ProcessReturnType::PRT_SELECTED;
			}
		}
	}

	// Set clicking
	if (m_SelectedOption != ARRAY_INVALID)
	{
		if (!(gKey & gKeyOk) && m_Clicking && m_UsedKeyboard)
		{
			m_Clicking = false;
			m_Hovering = false;
			iRetFlags |= GuiBase::ProcessReturnType::PRT_DIRTY;
		}
		else if ((gKey & gKeyOk) && !m_Clicking)
		{
			m_Clicking     = true;
			m_UsedKeyboard = true;
			m_Hovering     = true;
			iRetFlags     |= GuiBase::ProcessReturnType::PRT_DIRTY;
		}
	}

	return iRetFlags;
}

int ModeInputVector::ProcessMouse(int* iSelectedOutTrg, GUI_POINT pMousePoint)
{
	ModeInputOption* pOption = NULL;
	int iRetFlags            = 0;

	// Set it immediately
	*iSelectedOutTrg = -1;

	// Process the selected option first and foremost
	//
	// Having m_IsSelectionProcessing set to 'true' assumes that both pSelected and pSelected->gui_ptr are valid pointers
	if (m_IsSelectionProcessing)
	{
		// Get the currently selected option
		pOption    = m_OptionList[m_SelectedOption];
		iRetFlags |= pOption->gui_ptr->process(pMousePoint);

		if (m_Owner)
			m_Owner->m_ModeConfig.ProcessMouseOnMove = false;

		// Set the 'ignore process mouse' flag
		iRetFlags |= GuiBase::ProcessReturnType::PRT_DISABLE_MOUSE_ON_MOVE;

		if (!pOption->gui_ptr->isSelected())
		{
			// The element is no longer selected; exit out of processing mode
			m_IsSelectionProcessing = false;
			iRetFlags              |= GuiBase::ProcessReturnType::PRT_DIRTY;
		}

		return iRetFlags;
	}

	// Loop through scroll elements
	for (int i = 0; i < m_ScrollCount; i++)
	{
		ScrollAreaInfo* pScroll = m_ScrollList[i];
		iRetFlags              |= pScroll->process(pMousePoint);

		if (iRetFlags & GuiBase::ProcessReturnType::PRT_IGNORE_MOUSE)
			return iRetFlags;

		if (pScroll->getContentArea().pointInRect(pMousePoint))
		{
			if (m_Owner)
				m_Owner->RemoveKey(KEY_SCRL_DOWN | KEY_SCRL_UP);

			gKey    &= ~(KEY_SCRL_DOWN | KEY_SCRL_UP);
			gKeyTrg &= ~(KEY_SCRL_DOWN | KEY_SCRL_UP);
		}
	}

	// If the selection is not invalid, then do some special processing with it
	if (m_SelectedOption != ARRAY_INVALID)
	{
		// Get the selected option
		pOption = m_OptionList[m_SelectedOption];

		// Set the element as 'hovered over'
		if (pOption->gui_ptr && pOption->gui_ptr->isHoverable() && !pOption->gui_ptr->has_special_mouse_processing)
			pOption->gui_ptr->setHovered(1);
	}

	// Loop through all buttons
	for (int i = 0; i < m_OptionCount; i++)
	{
		pOption              = m_OptionList[i];
		GUI_RECT pOptionRect = Internal_GetOptionRect(pOption);

		if (pOption->scroll_ptr)
			pOptionRect = GUI_RECT(pOptionRect.position() + pOption->scroll_ptr->getRelativeOffset(), pOptionRect.extent());

		// Determine if we are actually hovering over this option
		if (!pOptionRect.pointInRect(pMousePoint) || (pOption->clip.use && !pOption->clip.rect.pointInRect(pMousePoint) && !m_UsedKeyboard))
		{
			// Close the tooltip if this option was selected AND we're not in keyboard-selection mode
			if (IsTooltipSetTo(pOption) && !m_UsedKeyboard)
				SetTooltip(NULL);

			if (pOption->gui_ptr && pOption->gui_ptr->isHoverable() && pOption->gui_ptr->isHovered())
			{
				pOption->gui_ptr->setHovered(false);
				iRetFlags |= GuiBase::ProcessReturnType::PRT_DIRTY;
			}

			if (m_SelectedOption == i && m_Hovering)
			{
				// Set not hovering
				m_Hovering = false;

				// We're dirty.
				iRetFlags |= GuiBase::ProcessReturnType::PRT_DIRTY;
			}

			if (!(gKey & KEY_LMB) && m_SelectedOption == i && !m_UsedKeyboard)
			{
				// Cache the selection
				if (pOption->calculated_input_rect.w > 0 && pOption->calculated_input_rect.h > 0)
					m_OldSelectedOption = m_SelectedOption;

				// Reset selection
				m_SelectedOption = ARRAY_INVALID;

				// We're dirty.
				iRetFlags |= GuiBase::ProcessReturnType::PRT_DIRTY;
			}

			continue;
		}

		// Don't select anything else if we're drag-clicking
		if (m_SelectedOption != i && (gKey & KEY_LMB))
			continue;

		if ((gKey & KEY_LMB) && !m_Hovering)
		{
			// Set not hovering
			m_Hovering = true;

			// We're dirty.
			iRetFlags |= GuiBase::ProcessReturnType::PRT_DIRTY;
		}

		// Set hovered state
		if (pOption->gui_ptr && pOption->gui_ptr->isHoverable() && !pOption->gui_ptr->isHovered() && !pOption->gui_ptr->has_special_mouse_processing)
		{
			pOption->gui_ptr->setHovered(true);
			iRetFlags |= GuiBase::ProcessReturnType::PRT_DIRTY;
		}

		if (m_UsedKeyboard)
			m_UsedKeyboard = false;

		if (pOption->tooltip.enabled)
			SetTooltip(pOption);

		// If we're not already selected, then set ourselves as selected.
		if (m_SelectedOption != i)
		{
			if (m_SelectedOption != ARRAY_INVALID && m_OptionList[m_SelectedOption]->gui_ptr && m_OptionList[m_SelectedOption]->gui_ptr->isHoverable() && m_OptionList[m_SelectedOption]->gui_ptr->isHovered())
				m_OptionList[m_SelectedOption]->gui_ptr->setHovered(false);

			m_SelectedOption = i;
			m_SelectedPos.x  = pOption->calculated_input_rect.x;
			m_SelectedPos.y  = pOption->calculated_input_rect.y;
			iRetFlags       |= GuiBase::ProcessReturnType::PRT_DIRTY;

			break;
		}

		// Handle selection
		if (pOption->gui_ptr && pOption->gui_ptr->has_special_mouse_processing)
		{
			int iRet   = pOption->gui_ptr->process(pMousePoint);
			iRetFlags |= iRet;

			if (iRet & GuiBase::ProcessReturnType::PRT_SELECTED)
			{
				m_IsSelectionProcessing = true;
				break;
			}
			if (iRet & GuiBase::ProcessReturnType::PRT_IGNORE_MOUSE)
				break;

			continue;
		}

		if (!m_UsedKeyboard && (!m_Clicking && pOption->gui_ptr && !pOption->gui_ptr->SelectOnMouseUp() && (gKey & KEY_LMB)) || (!(gKey & KEY_LMB) && m_Clicking))
		{
			if (!pOption->gui_ptr || pOption->gui_ptr->SelectOnMouseUp())
			{
				// Remove key press
				gKey    &= ~KEY_LMB;
				gKeyTrg &= ~KEY_LMB;
			}

			// Ignore mouse input
			iRetFlags |= GuiBase::ProcessReturnType::PRT_IGNORE_MOUSE;

			// Special processing for options with GUI elements
			if (pOption->gui_ptr)
			{
				// Close the annoying tooltip
				if (m_Owner && pOption->tooltip.enabled && m_Owner->IsTooltipOpen())
					m_Owner->CloseTooltip(false);

				if (pOption->gui_ptr->SelectOnMouseUp())
				{
					// Set it as 'selected'
					pOption->gui_ptr->setSelected(true);
				}
				else
				{
					// Simply process it.
					pOption->gui_ptr->process(pMousePoint);
				}

				// If it is actually selected, then set our own variables
				if (pOption->gui_ptr->isSelected())
					m_IsSelectionProcessing = true;
			}

			// Set selected ID
			iRetFlags |= GuiBase::ProcessReturnType::PRT_DIRTY;

			// Only select this option if we can
			if (m_OptionList[m_SelectedOption]->config.clickable)
			{
				*iSelectedOutTrg = pOption->selection_id;
				iRetFlags       |= GuiBase::ProcessReturnType::PRT_SELECTED;
			}
		}

		if ((gKey & KEY_LMB) && !m_Clicking)
		{
			m_Clicking     = true;
			m_UsedKeyboard = false;
			iRetFlags     |= GuiBase::ProcessReturnType::PRT_DIRTY;
		}

		break;
	}

	// Set clicking
	if (!(gKey & KEY_LMB) && m_Clicking && !m_UsedKeyboard)
		m_Clicking = false;

	return iRetFlags;
}

void ModeInputVector::Lock()
{
	m_Locked = true;
	m_Dirty  = false;
}

void ModeInputVector::Unlock()
{
	if (!m_Locked)
		return;

	m_Locked = false;

	if (m_Dirty)
	{
		// Rebuild the array if necessary
		m_Dirty = false;
		RebuildArray();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ModeInputVector::DumpMap()
{
	char Buffer[256];
	char Whitespace[256];
	
	int iNumWidth = 0;
	for (int i = 0; i < m_ArrayWidth * m_ArrayHeight; i++)
	{
		if (m_Array[i] >= m_OptionCount)
		{
			// Print the array invalid value into the buffer
			// so we can tell how long it is
			sprintf(Buffer, "%d", m_Array[i]);
		}
		else
		{
			// Print the array invalid value into the buffer
			// so we can tell how long it is
			sprintf(Buffer, "%d", (m_Array[i] == ARRAY_INVALID ? -1 : m_OptionList[m_Array[i]]->selection_id));
		}

		if (strlen(Buffer) > iNumWidth)
			iNumWidth = strlen(Buffer);
	}

	// Build the whitespace string
	for (int i = 0; i < iNumWidth; i++)
		Whitespace[i] = ' ';

	Whitespace[iNumWidth] = 0;

	printf("ModeInputVector<%d, %d>: %d\r\n", m_ArrayWidth, m_ArrayHeight, iNumWidth);

	// Build borders
	printf("+");
	for (int i = 0; i < m_ArrayWidth; i++)
	{
		for (int j = 0; j < iNumWidth + 2; j++)
			printf("-");

		printf("+");
	}
	printf("\r\n");

	for (int y = 0; y < m_ArrayHeight; y++)
	{
		printf("|");
		for (int x = 0; x < m_ArrayWidth; x++)
		{
			if (m_Array[(m_ArrayWidth * y) + x] >= m_OptionCount)
			{
				sprintf(Buffer, "%d", m_Array[(m_ArrayWidth * y) + x]);
				printf("{%s%s}|", Buffer, Whitespace + strlen(Buffer));
				continue;
			}

			// Print whitespace
			sprintf(Buffer, "%d", (m_Array[(m_ArrayWidth * y) + x] == ARRAY_INVALID ? -1 : m_OptionList[m_Array[(m_ArrayWidth * y) + x]]->selection_id));
			printf(" %s%s |", Buffer, Whitespace + strlen(Buffer));
		}
		printf("\r\n");
	}
	
	// Build end borders
	printf("+");
	for (int i = 0; i < m_ArrayWidth; i++)
	{
		for (int j = 0; j < iNumWidth + 2; j++)
			printf("-");
		printf("+");
	}
	printf("\r\n");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------