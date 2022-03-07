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
  @file ExternalWindow.h
*/

#ifndef CAVESTORY_MOD_API_H_
#error "Please include CavestoryModAPI.h before including sub-classes."
#endif

#include <CSMAPI_begincode.h>

#pragma once

/// @ingroup ExternalWindow
/// @{
///

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class VideoTexture;
class ExternalWindow;
struct SDL_Texture;
struct SDL_Window;
struct SDL_Renderer;
union SDL_Event;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Hittest result.
///
enum ExternalWindow_HitResult
{
	/// No special properties
	EXTERNAL_WINDOW_HITTEST_NORMAL = 0,//SDL_HITTEST_NORMAL,

	/// Region can drag entire window.
	EXTERNAL_WINDOW_HITTEST_DRAGGABLE = 1,//SDL_HITTEST_DRAGGABLE,

	/// Region is the topleft resizer
	EXTERNAL_WINDOW_HITTEST_RESIZE_TOPLEFT = 2,//SDL_HITTEST_RESIZE_TOPLEFT,

	/// Region is the top resizer
	EXTERNAL_WINDOW_HITTEST_RESIZE_TOP = 3,//SDL_HITTEST_RESIZE_TOP,

	/// Region is the topright resizer
	EXTERNAL_WINDOW_HITTEST_RESIZE_TOPRIGHT = 4,//SDL_HITTEST_RESIZE_TOPRIGHT,

	/// Region is the right resizer
	EXTERNAL_WINDOW_HITTEST_RESIZE_RIGHT = 5,//SDL_HITTEST_RESIZE_RIGHT,

	/// Region is the bottomright resizer
	EXTERNAL_WINDOW_HITTEST_RESIZE_BOTTOMRIGHT = 6,//SDL_HITTEST_RESIZE_BOTTOMRIGHT,

	/// Region is the bottom resizer
	EXTERNAL_WINDOW_HITTEST_RESIZE_BOTTOM = 7,//SDL_HITTEST_RESIZE_BOTTOM,

	/// Region is the bottomleft resizer
	EXTERNAL_WINDOW_HITTEST_RESIZE_BOTTOMLEFT = 8,//SDL_HITTEST_RESIZE_BOTTOMLEFT,

	/// Region is the left resizer
	EXTERNAL_WINDOW_HITTEST_RESIZE_LEFT = 9,//SDL_HITTEST_RESIZE_LEFT
};

/// @brief Callback for hittesting.
///
/// @param pWindow The window object.
/// @param pPoint The mouse point.
/// @param pUserData The user data.
typedef ExternalWindow_HitResult (*ExternalWindow_HitTestFunc)(ExternalWindow* pWindow, GUI_POINT pPoint, void* pUserData);

/// @brief Callback for resizing the window.
///
/// @param pWindow The window object.
/// @param pOldSize The old size of the window.
/// @param pNewSize The new size of the window.
typedef void (*ExternalWindow_OnResizeFunc)(ExternalWindow* pWindow, GUI_POINT pOldSize, GUI_POINT pNewSize, void* pUserData);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Flags for ExternalWindow.
///
enum ExternalWindowFlags
{
	/// Window is minimized
	EXTERNAL_WINDOW_MINIMIZED = BIT(0),

	/// Window is maximized
	EXTERNAL_WINDOW_MAXIMIZED = BIT(1),

	/// Window is hidden
	EXTERNAL_WINDOW_HIDDEN = BIT(2),

	/// Window has no border
	EXTERNAL_WINDOW_BORDERLESS = BIT(3),

	/// Window can be resized
	EXTERNAL_WINDOW_RESIZABLE = BIT(4),

	/// Window has input focus
	///
	/// @note This does nothing if set initially!
	EXTERNAL_WINDOW_HAS_INPUT_FOCUS = BIT(5),

	/// Window has mouse focus
	///
	/// @note This does nothing if set initially!
	EXTERNAL_WINDOW_HAS_MOUSE_FOCUS = BIT(6),

	/// Window is a tooltip window.
	EXTERNAL_WINDOW_TOOLWINDOW = BIT(7),

	/// Window is always topmost window
	EXTERNAL_WINDOW_TOPMOST = BIT(8),
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Holds information for window-specific information pertaining to surf[].
struct EXTERNAL_WINDOW_TEXTURE
{
	/// The texture object for this texture.
	///
	SDL_Texture* texture;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief An external window to render stuff on.
///
class CAVESTORY_MOD_API ExternalWindow
{
public: // Linkage

	/// First entry in list
	///
	static SDL_Window* mouse_focused;
	
	/// First entry in list
	///
	static ExternalWindow* first;

	/// Previous entry in list
	///
	ExternalWindow* prev;

	/// Next entry in list
	///
	ExternalWindow* next;

protected: // Protected variables

	/// Whether this window is initialized already or not.
	///
	bool m_bInitialized;

	/// Texture allocated to this window.
	///
	VideoTexture** m_pVideoTextureList;

	/// The number of textures allocated to this window.
	///
	int m_iVideoTextureCount;
	
	/// The number of texture slots allocated.
	///
	int m_iVideoTextureSize;

public: // Callbacks

	/// User data for callback.
	///
	void* CallbackUserData;

	/// Called every time the window's size is changed
	///
	ExternalWindow_OnResizeFunc OnResize;

public: // Important variables

	/// This window's parent window. If set to 'NULL', then it will be the child of the main window.
	///
	ExternalWindow* m_pParent;

	/// A pointer to the hit test function.
	///
	ExternalWindow_HitTestFunc m_pHitTestFunc;

	/// The user data for the hitfunc.
	///
	void* m_pHitTestUserData;

	/// The SDL renderer pointer for this window
	///
	SDL_Renderer* m_pRenderer;

	/// The SDL window pointer for this window
	///
	SDL_Window* m_pWindow;

	/// The flags for this window. Should be updated dynamically.
	///
	unsigned int m_iFlags;

	/// Texture array for this window.
	///
	EXTERNAL_WINDOW_TEXTURE m_pTextureList[SURFACE_ID_MAX];
	
	/// The renderer texture for this external window. Allows one to draw directly to this window without using a middle-man texture.
	///
	VideoTexture* m_pRendererTexture;

public: // Cache variables

	/// Cache variables stored for this window
	///
	struct
	{
		/// The window's rect
		///
		GUI_RECT WindowRect;

		/// The window's ID
		///
		int WindowID;
	} m_pCache;

public: // C++ Stuff

	/// @brief Constructor
	///
	ExternalWindow();

	/// @brief Deconstructor
	///
	~ExternalWindow();

public: // Protected methods

	/// @brief Initialize this window.
	///
	/// @param pTitle The title of the window.
	/// @param pPoint The initial position of the window. Pass 'NULL' here to center the window.
	/// @param pSize The size of the window.
	/// @param iFlags The initial flags for this window.
	/// @param iMagnification The renderer magnification for this window. Setting this to '-1' will use the current magnification level.
	///
	/// @return Returns true on success, otherwise returns false.
	bool Init(const char* pTitle, GUI_POINT* pPoint, GUI_POINT pSize, unsigned int iFlags = 0, int iMagnification = -1);

	/// @brief Reset this window and its memory.
	///
	void Free();

	/// @brief Check to see if this window has been initialized.
	///
	/// @return Returns the result.
	inline bool IsInitialized() { return m_bInitialized; }

public: // Update methods

	/// @brief Update a window's flags
	///
	void UpdateFlags();

public: // Texture management

	/// @brief Register a texture to this window.
	///
	/// @param pTexture The texture object.
	void RegisterTexture(VideoTexture* pTexture);

	/// @brief Unregister a texture from this window.
	///
	/// @param pTexture The texture object.
	void UnregisterTexture(VideoTexture* pTexture);

public: // Window management methods

	/// @brief Get the size of this window.
	///
	/// @return Returns the size of the window.
	GUI_POINT GetWindowSize();

	/// @brief Resize this window.
	/// 
	/// @param pNewSize The new size of the window.
	void SetWindowSize(GUI_POINT pNewSize);

	/// @brief Maximize this window.
	///
	void Maximize();
	
	/// @brief Minimize this window.
	///
	void Minimize();

public: // Input methods

	/// @brief Handle an input.
	///
	/// @param pEvent The event to parse.
	void OnInputEvent(SDL_Event* pEvent);

	/// @brief Set the hittest function.
	///
	/// @param pFunc The function.
	void SetHitTestFunc(ExternalWindow_HitTestFunc pFunc, void* pUserData);

	/// @brief Get the mouse position relative to this window.
	///
	/// @return Returns the mouse position.
	GUI_POINT GetMousePosition();

	/// @brief Test a window's input focus.
	///
	/// @param pWindow The window to test. Set as 'NULL' test gWindow.
	///
	/// @return Returns true if the specified window has input focus.
	static bool TestWindowInputFocus(ExternalWindow* pWindow);
	
	/// @brief Test a window's mouse focus.
	///
	/// @param pWindow The window to test. Set as 'NULL' test gWindow.
	///
	/// @return Returns true if the specified window has mouse focus.
	static bool TestWindowMouseFocus(ExternalWindow* pWindow);

public: // Render methods

	/// @brief Get the texture specific to this window's renderer for a surface.
	///
	/// @param iSurfaceId The surface's ID.
	///
	/// @return Returns the texture 
	SDL_Texture* GetSurfaceTexture(Surface_Ids iSurfaceId);

	/// @brief Flip this window.
	///
	void FlipWindow();
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_endcode.h>

/// @}
///