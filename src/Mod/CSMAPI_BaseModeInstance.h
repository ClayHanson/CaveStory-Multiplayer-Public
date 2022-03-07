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
  @file CSMAPI_BaseModeInstance.h
*/

#pragma once

class VideoTexture;

#include <CSMAPI_begincode.h>
class CAVESTORY_MOD_API BaseModeInstance;
class CAVESTORY_MOD_API ModeInputVector;
#include <CSMAPI_endcode.h>

//------------------------------------------------------------------------------------------------------------------------------------

namespace Platform
{
	void OpenBugReporter();
}

namespace ChatEmotes
{
	extern int CheckOpenList(BaseModeInstance*);
}

#include <CSMAPI_begincode.h>
extern void CAVESTORY_MOD_API MessageBox_DrawMode(void*);

//------------------------------------------------------------------------------------------------------------------------------------

/// Maximum allocatable textures per mode instance
#define MODE_INSTANCE_MAX_TEXTURES 64

/// Maximum allocatable surfaces per mode instance
#define MODE_INSTANCE_MAX_SURFACES 64

/// Maximum allocatable threads per mode instance
#define MODE_INSTANCE_MAX_THREADS 8

/// Enable forced debug mode
#define MODE_INSTANCE_DEBUG

/// Enable safety guards
#define MODE_INSTANCE_GUARDS

//------------------------------------------------------------------------------------------------------------------------------------
// Configuration

/// The amount of time (in milliseconds) added to a tooltip's lifetime so that it can play the fadein/fadeout animation
#define MODE_TOOLTIP_ANIMATION_TIME 1000

/// The center flag for DrawSurface(). Do not change this.
#define MODE_DRAW_CENTERED_SURFACE -0x7FFFFFF

/// The "i'm still running!" return code for DoModeTick
#define MODE_IM_STILL_RUNNING 0x7FFFFFFF

/// The control keys surface margin
#define CONTROL_MARGIN 4

/// Invalid ID for ModeThreadInstance.
#define MODE_INVALID_THREAD_ID 0

//------------------------------------------------------------------------------------------------------------------------------------

/// How to draw the menu controls
///
enum MENU_CONTROLS_DRAW_POSITION
{
	/// Draw them in the top left
	CDP_TOP_LEFT  = 0,

	/// Draw them in top right
	CDP_TOP_RIGHT = 1,

	/// Draw them in bottom left
	CDP_BTM_LEFT  = 2,

	/// Draw them in bottom right
	CDP_BTM_RIGHT = 3,

	/// Draw them in the middle of the screen
	CDP_CENTER    = 4
};

/// States of the menu control element
///
enum MENU_CONTROLS_DRAW_FRAME
{
	/// Being animated (opening, closing)
	ANIMATED  = 0,

	/// Opened
	NORMAL    = 1,

	/// Closed
	DEPRESSED = 2
};

/// Act descriptors for menu controls
///
enum MENU_CONTROLS_ACTS
{
	/// First open
	MCA_FIRST_OPEN = 0,

	/// Opened
	MCA_OPENED     = 1,

	/// Closing
	MCA_CLOSING    = 2,

	/// Closed
	MCA_CLOSED     = 3,

	/// Opening
	MCA_OPENING    = 4
};

//------------------------------------------------------------------------------------------------------------------------------------

/// For MenuControls
///
struct KeyControlStruct
{
	/// Whether this control is a mouse control or not
	///
	bool Mouse;

	/// What button to display (gKeyShot, gKeyCancel, etc.) Only takes one trigger flag, so don't include more than one.
	///
	int Key;

	/// The description of what happens when you press this button.
	///
	char Desc[128];
};

//------------------------------------------------------------------------------------------------------------------------------------

/// ToolTip Info
///
struct ToolTipStruct
{
	/// Whether or not this tooltip is currently being shown.
	///
	bool shown;

	/// The showing position for this tooltip
	///
	GUI_RECT rect;

	/// The surface index that is allocated for this tooltip
	///
	int surface_idx;
	
	/// The texture index that is allocated for this tooltip
	///
	int texture_idx;

	/// The time that this tooltip started being displayed
	///
	int start_time;

	/// The life time for this tooltip in milliseconds. Can be -1 to never close.
	///
	int life_time;
};

//------------------------------------------------------------------------------------------------------------------------------------

/// The identifier for a ModeThreadInstance's ID.
typedef unsigned int ModeThreadId;

//------------------------------------------------------------------------------------------------------------------------------------

/// @brief Easy multi-threading for modes.
///
class CAVESTORY_MOD_API ModeThreadInstance
{
	friend class BaseModeInstance;
	friend int ModeThreadInstance_AsyncFunc(void* ptr);

private: // Static variables

	/// The main thread mutex for all base mode instances.
	/// @private
	static void* sMainModeMutex;

	/// The next thread ID.
	/// @private
	static ModeThreadId sNextThreadId;

public: // Enumerations

	/// Thread states.
	///
	enum ThreadState
	{
		/// Thread has not started running yet.
		THREAD_NOT_RUNNING = 0,

		/// Thread is starting.
		THREAD_STARTING    = 1,

		/// Thread is running.
		THREAD_RUNNING     = 2,

		/// Thread is being forced to stop.
		THREAD_STOPPING    = 3,

		/// Thread is ready to be deleted.
		THREAD_DELETABLE   = 4,
	};

public: // Typedefs

	/// Callback function for when a thread completes.
	///
	typedef void(*ModeCallback_OnThreadComplete)(BaseModeInstance* pMode, ModeThreadInstance* pThread, void* pUserData);
	
	/// Callback function for when a thread completes.
	///
	typedef void(*ModeCallback_OnThreadLoop)(BaseModeInstance* pMode, ModeThreadInstance* pThread, void* pUserData);

protected: // Variables

	/// The thread name.
	/// @protected
	char m_ThreadName[128];

	/// This thread's local ID.
	/// @protected
	ModeThreadId m_ThreadId;

	/// Called when the asynchronous operation ends, by any means.
	/// @protected
	ModeCallback_OnThreadComplete m_OnComplete;

	/// Called every frame.
	/// @protected
	ModeCallback_OnThreadComplete m_OnLoop;

	/// The owner of this thread.
	/// @protected
	BaseModeInstance* m_Owner;

	/// The thread for this mode.
	/// @protected
	void* m_Thread;
	
	/// This thread's unique mutex.
	/// @protected
	void* m_Mutex;

	/// The state of this thread.
	///
	ThreadState m_State;

	/// This is controlled by the async function.
	///
	bool m_Running;
	
	/// The result from the async operation.
	///
	int m_Result;

	/// Function callback userdata
	///
	void* m_UserData;

public: // Constructor & deconstructor
	
	/// Main constructor
	///
	ModeThreadInstance();

	/// Deconstructor
	///
	~ModeThreadInstance();

protected: // Internal methods

	/// @brief Initialize this thread instance.
	///
	/// @param pOwner The owner of this thread.
	/// @param pOnLoopFunc The 'OnLoop' callback function.
	/// @param pOnCompleteFunc The 'OnComplete' callback function.
	/// @param pUserData The user data for the callback functions.
	///
	/// @return Returns true on success, false otherwise.
	bool InitThread(BaseModeInstance* pOwner, ModeCallback_OnThreadLoop pOnLoopFunc, ModeCallback_OnThreadComplete pOnCompleteFunc, void* pUserData = NULL);

	/// @brief Complete this mode. Do not call this.
	///
	void __Complete_And_Send_Result__();

public: // Locking & unlocking

	/// @brief Try to lock a critical part of code.
	///
	/// @return Returns true on success, false if it's already locked.
	static bool TryLock();
	
	/// @brief Lock a critical part of code.
	///
	static void Lock();

	/// @brief Unlock a critical part of code.
	///
	static void Unlock();
	
public: // Unique mutex locking & unlocking

	/// @brief Try to lock a critical part of code.
	///
	/// @return Returns true on success, false if it's already locked.
	bool TryLockUnique();
	
	/// @brief Lock a critical part of code.
	///
	void LockUnique();

	/// @brief Unlock a critical part of code.
	///
	void UnlockUnique();

public: // Methods

	/// @brief Start the thread.
	///
	bool Start();

	/// @brief End the thread.
	///
	void End(int iResult = 0);

	/// @brief Wait for the thread to finish.
	///
	void WaitToFinish();

	/// @brief Detach this thread.
	///
	void Detach();

	/// @brief Get the return result.
	///
	int GetResult();
};

//------------------------------------------------------------------------------------------------------------------------------------

/// @brief Easy UI management.
///
/// This class controls most of the new UI in the game, and is packed with features to make developers' lives easy.
///
/// FreeRun instances are mode instances that run concurrently with other modes.
///
/// Instead of using #gKey, #gKeyTrg, etc. you should use the built-in m_ModeInput struct.
class CAVESTORY_MOD_API BaseModeInstance
{
	friend class ModeInputVector;
	friend class ModeThreadInstance;
	friend ModeThreadInstance::ModeCallback_OnThreadLoop;
	friend ModeThreadInstance::ModeCallback_OnThreadComplete;
	friend extern void SetGameResolution(int iNewWidth, int iNewHeight, int iNewMagnification);
	friend extern void CAVESTORY_MOD_API MessageBox_DrawMode(void*);
	friend extern int ChatEmotes::CheckOpenList(BaseModeInstance* pMode);
	friend extern void Platform::OpenBugReporter();

private: // Screen grab stuff

	/// Struct for caching eligible screen grab surfaces.
	///
	struct BASEMODE_SCREEN_GRAB_CACHE
	{
		/// The surface this screengrab uses
		/// @private
		Surface_Ids surf;

		/// Whether this screengrab is being used
		///
		bool used;
	};

	/// Cache for storing the screengrabs
	/// @private
	static BASEMODE_SCREEN_GRAB_CACHE m_szScreenGrabData[3];

public: // Typedefs

	/// Callback function for when a mode ends.
	///
	typedef void(*ModeCallback_OnEnd)(BaseModeInstance* pMode, const int& iResult, bool& bDelete);

public: // Linkage

	/// Linkage: Currently focused instance
	/// @private
	static BaseModeInstance* m_FocusedInstance;

	/// Linkage: First instance in link
	/// @private
	static BaseModeInstance* m_FirstInstance;

	/// Linkage: First freerun instance in link
	/// @private
	static BaseModeInstance* m_FreeRun_Start;

	/// Linkage: Next freerun instance in link
	/// @private
	BaseModeInstance* m_FreeRun_Next;

	/// Linkage: Next instance in link
	/// @private
	BaseModeInstance* m_NextInstance;

private: // Variables for free-running mode

	/// Old mouse point
	/// @private
	GUI_POINT m_OldMousePoint;

	/// Current mouse point
	/// @private
	GUI_POINT m_MousePoint;

private: // Private data

	/// How many modes have cursor enabled
	/// @private
	static int m_ModeCursorCount;

	/// The default font
	/// @private
	static FontObject* m_DefaultFont;

	/// An array of surface IDs
	/// @private
	Surface_Ids m_SurfaceIDList[MODE_INSTANCE_MAX_SURFACES];

	/// All video textures for this mode.
	/// @private
	VideoTexture* m_TextureList[MODE_INSTANCE_MAX_TEXTURES];

	/// An array of threads.
	/// @private
	ModeThreadInstance* m_ThreadList[MODE_INSTANCE_MAX_THREADS];

	/// Amount of allocated threads.
	/// @private
	int m_AllocThreadCount;

protected: // Tooltip data

	/// The tooltip that is fading out.
	/// @protected
	ToolTipStruct m_OldTooltip;

	/// The tooltip that is currently being shown.
	/// @protected
	ToolTipStruct m_CurTooltip;

private: // Control table surfaces

	struct
	{
		/// Stores the local surface indexes for the allocated Menu Control Table surfaces.
		/// @private
		int Indices[2];

		/// Stores the actual menu control size
		/// @private
		GUI_POINT MenuSize;

		/// Animation timer for menu controls
		/// @private
		int AnimTimer;

		/// Frame timer for menu controls
		/// @private
		int FrameTimer;

		/// The act this menu is in
		/// @private
		MENU_CONTROLS_ACTS ActNo;

		/// Has joypad?
		/// @private
		bool Joypad;
	} m_MenuControls;

public: // Callbacks

	struct
	{
		/// This is called when the mode is ending, just before 'Free()'.
		///
		/// @note This is only called in FreeCall mode
		ModeCallback_OnEnd OnEnd;
	} m_Callbacks;

private: // 'Mouse Rect Check' (MRC)

	/// The current invalid mouse rect. This will be set to (0, 0, 0, 0) when GetRectHoverTime() is called with a rect that matches this rect, AND if the mouse position provided is not inside of the rect.
	/// @private
	GUI_RECT m_MRC_Last;

	/// The last valid mouse rect.
	/// @private
	GUI_RECT m_MRC_Curr;

	/// Timestamp at which the mouse entered the last rect.
	/// @private
	unsigned int m_MRC_Time;

private: // Key input private variables

	/// Controls the old key presses. Children classes shouldn't be messing with this anyways.
	/// @private
	unsigned int m_KeyOld;

	/// Controls the old key2 presses. Children classes shouldn't be messing with this anyways.
	/// @private
	unsigned int m_KeyOld2;

	/// Controls the old key_joy presses. Children classes shouldn't be messing with this anyways.
	/// @private
	unsigned int m_KeyOld_JOY;

protected: // Shared data

	/// The configuration for this mode
	///
	struct ModeConfiguration
	{
		/// Allow keyboard control -- If false, then BaseModeInstance::ProcessKeys is not called.
		///
		bool AllowKeyControl;

		/// Allow mouse control -- If false, then BaseModeInstance::ProcessMouse is not called.
		///
		bool AllowMouseControl;

		/// Only process mouse on movement -- If true, then BaseModeInstance::ProcessMouse will only be called when the mouse is moved, rather than every frame.
		///
		bool ProcessMouseOnMove;

		/// The return code that pressing Alt+F4 should provide.
		///
		int AltF4RetValue;

		/// Menu controls configuration
		///
		struct
		{
			/// If menu controls are actually enabled
			///
			bool Enabled;

			/// The draw position for menu controls
			///
			MENU_CONTROLS_DRAW_POSITION DrawPosition;

			/// Automatically hide the menu controls after a while
			///
			bool AllowAutoHide;

			/// Open when 'TAB' is pressed
			///
			bool OpenOnTab;

			/// Open when hovered over
			///
			bool OpenOnHover;

			/// The alpha value when the menu controls are closed (0 - 255)
			///
			int ClosedAlpha;

			/// The alpha value when the menu controls are opened (0 - 255)
			///
			int OpenedAlpha;

			/// The amount of time it takes to open the menu controls.
			///
			int FrameTime;

			/// The amount of time it takes to close the menu controls.
			///
			int HideTime;
		} MenuControls;
	} m_ModeConfig;

	/// Used for handling input.
	/// @protected
	///
	struct ModeInput
	{
		/// Same as gKey
		/// @protected
		unsigned int Key;

		/// Same as gKeyTrg
		/// @protected
		unsigned int KeyTrg;

		/// Same as gKey2
		/// @protected
		unsigned int Key2;

		/// Same as gKeyTrg2
		/// @protected
		unsigned int KeyTrg2;

		/// Same as gKey_JOY
		/// @protected
		unsigned int Key_JOY;

		/// Same as gKeyTrg_JOY
		/// @protected
		unsigned int KeyTrg_JOY;
	} m_ModeInput;

	/// The screen grab surface ID
	///
	Surface_Ids m_ScreenGrabSurf;

	/// Controls whether this mode should re-render
	///
	bool m_Dirty;

	/// Controls whether this mode should end.
	///
	bool m_End;

	/// Controls whether this mode is in free-call mode. Do NOT change this!
	///
	bool m_FreeCall;

	/// Controls the return value of DoMode() internally.
	///
	/// @see GameProcessMode
	int m_RetValue;

public: // Data

	/// Pointer to private data structure, defined by each mode
	///
	void* m_PrivData;

public: // Constructor

	/// Initialize certain values.
	BaseModeInstance();
	~BaseModeInstance();

protected: // Main protected methods

	/// @brief Initialize this mode.
	///
	/// @return Returns a non-zero value if an error occured. On this occasion, DoMode() will not continue execution.
	virtual int Init() = 0;

	/// @brief De-initialize this mode.
	///
	/// @return Returns a non-zero value if an error occured. An exception will be thrown if a non-zero value is encountered.
	virtual int Free() = 0;

	/// @brief Ask the mode to populate pControlBuffer with its control scheme. If pControlCount is set to 0, then the control menu will not be displayed.
	///
	/// @param pControlBuffer Populate this buffer with KeyControlStruct. Max 32.
	/// @param pControlCount Set this to how many keys were populated in pControlBuffer.
	/// @param bStackControls Whether to display the controls from top to bottom, or left to right.
	virtual void QueryMenuControls(KeyControlStruct pControlBuffer[32], int& pControlCount, bool& bStackControls);

	/// @brief Process the logic of this menu.
	///
	virtual void ProcessLogic() = 0;

	/// @brief Process this menu's mouse.
	///
	/// @param mouse_point The mouse point.
	virtual void ProcessMouse(GUI_POINT mouse_point) = 0;

	/// @brief Process key input.
	///
	virtual void ProcessKeys() = 0;

	/// @brief Render everything to surfaces.
	///
	virtual void PreRender() = 0;

	/// @brief Draw rendered surfaces.
	///
	virtual void Draw() = 0;

	/// @brief Draw rendered surfaces over top of everything else.
	///
	virtual void PostDraw() {}

public: // HELPER: Static helper methods

	/// @brief Show the cursor.
	///
	/// @note Acts as a ref counter. If you call ShowCursor(), then you should call this when you're finished, or the mouse will never be hidden.
	static void ShowCursor();

	/// @brief Hide the cursor.
	///
	/// @note Acts as a ref counter. If you call ShowCursor(), then you should call this when you're finished, or the mouse will never be hidden.
	static void HideCursor();

	/// @brief Set the current cursor.
	///
	/// @param iType The cursor type to set.
	/// @param iEventId An attached event ID, so GUI elements can identify if it was themselves that set a cursor.
	/// @param pRect The rect that the mouse should stay within to keep this cursor type. If the mouse exits this rect, then it is set back to NORMAL. This can be 'NULL' to ignore this functionality.
	static void SetCursor(CSMP_CURSOR_TYPE iType, int iEventId = -1, GUI_RECT* pRect = NULL);

	/// @brief Get the current cursor type.
	///
	/// @param pValidRect A pointer to a GUI_RECT object that will hold the valid cursortype area. Can be 'NULL'.
	/// @param pEventId A pointer to an int object that will hold the event ID. Can be 'NULL'.
	///
	/// @return Returns the cursor type.
	static CSMP_CURSOR_TYPE GetCursor(GUI_RECT** pRect = NULL, int* pEventId = NULL);

	/// @brief Reload all currently opened modes.
	///
	static void ReloadOpenModes();

	/// @brief Get the default font that all GUIs should use.
	///
	/// @return Returns the font that should be used.
	static FontObject* GetDefaultFont();
	
	/// @brief Reload the default font.
	///
	/// @param pFontName The name of the font.
	/// @param pFontPath The path of the font to try in. Do not lead with a trailing '/' or '\'.
	///
	/// @return Returns the font that should be used.
	static void ReloadFont(const char* pFontName, const char* pFontPath, int iFontW = -1, int iFontH = -1);

	/// @brief Show a messagebox that allows the user to scroll through a list of tutorial pages.
	///
	/// @param pCaller The mode that should be drawn underneath the tutorial. Can be NULL.
	/// @param iPageCount The number of pages.
	/// @param ... Page text for each page. Only accepts char*.
	///
	/// @return Returns a GameProcessMode value.
	static GameProcessMode ShowTutorialPages(BaseModeInstance* pCaller, int iPageCount, ...);

protected: // HELPER: Tooltip methods

	/// @brief Check to see if a tooltip is open. Only checks m_CurTooltip.
	///
	/// @return Returns true if a tooltip is currently open.
	bool IsTooltipOpen();

	/// @brief Create a tooltip.
	///
	/// @param point The point at which the tooltip should be shown at.
	/// @param font The font that should be used to create this tooltip object.
	/// @param text The text that should be shown on this tooltip.
	/// @param life_time The life time of the tooltip, in milliseconds.
	/// 
	/// @return Returns true if the tooltip was created.
	///
	/// @note This function allocates a new 
	bool CreateTooltip(GUI_POINT point, FontObject* font, const char* text, int life_time = 5000);

	/// @brief Create a texture tooltip.
	///
	/// @param point The point at which the tooltip should be shown at.
	/// @param font The font that should be used to create this tooltip object.
	/// @param text The text that should be shown on this tooltip.
	/// @param life_time The life time of the tooltip, in milliseconds.
	/// @param Magnification The magnification level of the texture.
	/// 
	/// @return Returns true if the tooltip was created.
	///
	/// @note This function allocates a new 
	bool CreateTextureTooltip(GUI_POINT point, FontObject* font, const char* text, int life_time = 5000, int Magnification = -1);

	/// @brief Close the currently opened tooltip.
	///
	/// @param fade_out If true, the tooltip will not fade out, and will instead just instantly dissapear.
	///
	/// @return Returns true if the tooltip was closed.
	bool CloseTooltip(bool fade_out = true);

	/// @brief Get a pointer to the tooltip's rect.
	///
	/// @return Returns a pointer to the rect of the tooltip. Returns NULL if the tooltip is not currently open.
	GUI_RECT* GetTooltipRect();

	/// @brief Set the tooltip's position
	///
	/// @param new_position The new position to set the tooltip to.
	void SetTooltipPosition(GUI_POINT new_position);

	/// @brief Process the current & old tooltips
	///
	void ProcessTooltips();

	/// @brief Draw the current & old tooltips
	///
	void DrawTooltips();

protected: // HELPER: Rect timers

	/// @brief A helper method. It can be used to count how long the mouse pointer has stayed inside of a rect.
	///
	/// @param mouse_point The position of the mouse.
	/// @param rect The rect in which the mouse should be in.
	///
	/// @return Returns the amount of time the mouse has been inside of the rect for. Returns -1 if it isn't in the given rect, and also returns -2 if the mouse WAS in the given rect, but isn't anymore.
	///
	/// @note This only works for one rect. When the mouse is detected as 'inside a rect', then it will overwrite the last timer.
	uint32_t GetRectHoverTime(GUI_POINT mouse_point, GUI_RECT rect);

	/// @brief Reset rect hover system.
	///
	void ResetRectHover();

public: // HELPER: Static uniform drawing methods

	/// @brief Draw game elements
	///
	/// @param AllowACT Allow game input.
	/// @param OffsetX X offset for the camera.
	/// @param OffsetY Y offset for the camera.
	/// @param DoPutFade Whether to draw the fade.
	static void DrawGame(bool AllowACT = true, int OffsetX = 0, int OffsetY = 0, bool DoPutFade = true);

	/// @brief Draw HUD
	///
	static void DrawHUD();

	/// @brief Get the size of what DrawControls() would output to the screen.
	///
	/// @param Buttons A list of keys to use.
	/// @param ButtonCount Number of keys included in Buttons.
	/// @param StackControls Whether to display the controls from top to bottom, or left to right.
	///
	/// @return Returns the calculated size of the menu controls UI.
	static GUI_POINT GetControlKeySize(const KeyControlStruct* Buttons, int ButtonCount, bool StackControls = false);

	/// @brief Draw standard controls
	///
	/// @param DrawPosition The position to draw the controls at.
	/// @param destSID The destination surface ID.
	/// @param Buttons A list of keys to use.
	/// @param ButtonCount Number of keys included in Buttons.
	/// @param Flash Whether the keys should be pressed or not.
	/// @param StackControls Whether to display the controls from top to bottom, or left to right.
	/// @param Alpha The alpha of the menu.
	static void DrawControls(MENU_CONTROLS_DRAW_POSITION DrawPosition, Surface_Ids destSID, const KeyControlStruct* Buttons, int ButtonCount, bool Flash = false, bool StackControls = false, int Alpha = 255);

private: // HELPER: Asynchronous methods (private)

	/// @brief Complete an asynchronous operation.
	///
	/// @param pCompleteThread The thread that was completed.
	void CompleteAsyncOperation(ModeThreadInstance* pCompleteThread);

public: // HELPER: Asynchronous methods

	/// @brief Begin a thread-safe asynchronous operation.
	///
	/// @param pOnLoopFunc The onloop callback function.
	/// @param pOnLoopFunc The oncomplete callback function. Can be NULL.
	/// @param pUserData The userdata to use for the callback functions. Can be NULL.
	///
	/// @return Returns the ID of the created thread on success, otherwise returns MODE_INVALID_THREAD_ID on error.
	ModeThreadId StartAsyncOperation(ModeThreadInstance::ModeCallback_OnThreadLoop pOnLoopFunc, ModeThreadInstance::ModeCallback_OnThreadComplete pOnCompleteFunc = NULL, void* pUserData = NULL);

	/// @brief Stop a thread.
	///
	/// @param iThreadId The thread's ID.
	/// @param bWaitForFinish Wait for the thread to finish.
	///
	/// @return Returns true if the thread was stopped successfully, false otherwise.
	bool StopThread(ModeThreadId iThreadId, bool bWaitForFinish = false);

	/// @brief Start all new threads.
	///
	void StartAllNewThreads();

	/// @brief Stop & wait for all threads to finish.
	///
	void FreeAllThreads();

public: // HELPER: Uniform drawing methods

	/// @brief Ensure a rect conforms to the size of a basic window. Helps to deal with broken tiles in the background of the window.
	///
	/// @param pRect The rect to enforce the sizing rule on.
	void EnforceBasicWindowSize(GUI_RECT* pRect);

	/// @brief Draw a basic window.
	/// 
	/// @param pRect The rect to draw the window with. Can be NULL.
	///
	/// @return Returns the margin of the window.
	int DrawBasicWindow(GUI_RECT* pRect = NULL);

protected: // Menu Control Table

	/// @brief Create control surfaces.
	///
	/// @param Buttons A list of keys to use.
	/// @param ButtonCount Number of keys included in Buttons.
	/// @param StackControls Whether to display the controls from top to bottom, or left to right.
	///
	/// @return Returns true if the menu controls were configured successfully.
	bool CreateMenuControls(const KeyControlStruct* Buttons, int ButtonCount, bool StackControls = false);

	/// @brief Draw the menu control surfaces generated by CreateMenuControls.
	///
	/// @param DrawPosition The position to draw the menu controls at.
	/// @param DrawFrame The frame of the menu controls UI.
	void DrawMenuControls(MENU_CONTROLS_DRAW_POSITION DrawPosition, MENU_CONTROLS_DRAW_FRAME DrawFrame);

	/// @brief Get the act number of the menu controls menu
	///
	/// @return Returns the current act descriptor.
	MENU_CONTROLS_ACTS GetMenuControlsAct();

	/// @brief Set the act number of the menu controls menu
	///
	/// Param NewAct The new act descriptor.
	void SetMenuControlsAct(MENU_CONTROLS_ACTS NewAct);

protected: // Non-overridable protected methods

	/// @brief Backup the screen to a surface.
	///
	bool GrabScreen();

	/// @brief Draw the backed up screen.
	///
	void DrawScreenGrab();

	/// @brief Release this mode's screengrab.
	///
	void ReleaseScreenGrab();

	/// @brief Open the escape menu.
	///
	/// @note This function automatically sets m_RetValue and m_End if necessary.
	void DoEscapeMenu();

	/// @brief Allocate private resources. Said resources are privated simply because they should not be tampered with.
	///
	/// @param InsertIndex The index in the local surface table to insert the new surface at. Pass '-1' to start from 0.
	/// @param Width The width of the surface.
	/// @param Height The height of the surface.
	///
	/// @return Returns the index (relative to m_SurfaceIDList) where the newly allocated surface resides. Returns -1 on error.
	int AllocSurface(int InsertIndex = -1, int Width = WINDOW_WIDTH, int Height = WINDOW_HEIGHT);

	/// @brief Allocate private resources. Said resources are privated simply because they should not be tampered with.
	///
	/// @param FileName The filename to load.
	/// @param InsertIndex The index in the local surface table to insert the new surface at. Pass '-1' to start from 0.
	///
	/// @return Returns the index (relative to m_SurfaceIDList) where the newly loaded image's surface resides. Returns -1 on error.
	int LoadImageToSurface(const char* FileName, int InsertIndex = -1);

	/// @brief Free a previously allocated surface.
	///
	/// @param Index The index of the surface.
	void FreeSurface(int Index);

	/// @brief Free all allocated surfaces.
	///
	void FreeAllSurfaces();

	/// @brief Check to see if a surface at a given local index is allocated.
	///
	/// @param Index The index in the local surface table to check.
	///
	/// @return Returns true if the surface at the given index is allocated, false otherwise.
	bool IsLocalSurfaceAllocated(int Index);

	/// @brief Select a previously allocated surface.
	///
	/// @param Index The index of the surface.
	void SelectSurface(int Index);

	/// @brief Get the currently selected surface.
	///
	/// @return Returns the index (relative to m_SurfaceIDList) where the allocated surface is.
	int GetSelectedSurface();

	/// @brief Convert the local index of a surface we own to a global index.
	///
	/// @param Index The index of the surface.
	///
	/// @return Returns the global index of the provided surface.
	Surface_Ids LocalSurfaceIndexToGlobal(int Index);

	/// @brief Convert the global index of a surface to a local index.
	///
	/// @param Index The index of the surface.
	///
	/// @return Returns the local index of the provided surface.
	int GlobalSurfaceIndexToLocal(Surface_Ids sId);

	/// @brief Draw an allocated surface on the screen.
	///
	/// @param Index The index of the surface.
	/// @param X The X position to draw the surface at. If MODE_DRAW_CENTERED_SURFACE, it will center it.
	/// @param Y The Y position to draw the surface at. If MODE_DRAW_CENTERED_SURFACE, it will center it.
	/// @param Alpha The alpha of the surface.
	void DrawSurface(int Index, int X = 0, int Y = 0, unsigned char Alpha = 255);

	/// @brief Get the width of an allocated surface.
	///
	/// @param Index The index of the surface.
	///
	/// @return The width of the given surface.
	int GetSurfaceWidth(int Index);

	/// @brief Get the height of an allocated surface.
	///
	/// @param Index The index of the surface.
	///
	/// @return The height of the given surface.
	int GetSurfaceHeight(int Index);

	/// @brief Get the screen-centered X position of a surface.
	///
	/// @param Index The index of the surface.
	///
	/// @return The X position.
	int GetCenteredX(int Index);

	/// @brief Get the screen-centered Y position of a surface.
	///
	/// @param Index The index of the surface.
	///
	/// @return The Y position.
	int GetCenteredY(int Index);

	/// @brief Check to see if this mode is the currently focused mode.
	///
	/// @return Returns true if this mode is the currently focused mode.
	bool IsFocusedMode();

public: // Texture methods

	/// @brief Get a locally allocated video texture object by its index.
	///
	/// @param iLocalIndex The local index of the texture.
	///
	/// @return Returns the video texture object if it exists, otherwise returns 'NULL' if it doesn't.
	VideoTexture* GetLocalVideoTexture(int LocalIndex);

	/// @brief Allocate private resources. Said resources are privated simply because they should not be tampered with.
	///
	/// @param InsertIndex The index in the local texture table to insert the new surface at. Pass '-1' to start from 0.
	/// @param Width The width of the texture.
	/// @param Height The height of the texture.
	/// @param BitsPerPixel The bits per pixel.
	/// @param Magnification Magnification level. Leave as '-1' to use the current magnification.
	/// @param WindowIndex The index of the window.
	///
	/// @return Returns the index (relative to m_TextureList) where the newly allocated surface resides. Returns -1 on error.
	int AllocTexture(int InsertIndex = -1, int Width = WINDOW_WIDTH, int Height = WINDOW_HEIGHT, int BitsPerPixel = 32, int Magnification = -1, int WindowIndex = 0);

	/// @brief Free a previously allocated texture.
	///
	/// @param Index The index of the texture.
	void FreeTexture(int Index);

	/// @brief Free all allocated textures.
	///
	void FreeAllTextures();

	/// @brief Check to see if a texture at a given local index is allocated.
	///
	/// @param Index The index in the local surface table to check.
	///
	/// @return Returns true if the surface at the given index is allocated, false otherwise.
	bool IsLocalTextureAllocated(int Index);

	/// @brief Select a previously allocated surface.
	///
	/// @param Index The index of the surface.
	void SelectTexture(int Index);

	/// @brief Get the currently selected texture.
	///
	/// @return Returns the index (relative to m_TextureList) where the allocated texture is.
	int GetSelectedTexture();

	/// @brief Finish drawing a texture.
	///
	/// @param The index of the texture.
	void FinishTexture(int Index);

	/// @brief Draw an allocated texture onto the screen.
	///
	/// @param Index The index of the texture.
	/// @param X The X position to draw the texture at. If MODE_DRAW_CENTERED_SURFACE, it will center it.
	/// @param Y The Y position to draw the texture at. If MODE_DRAW_CENTERED_SURFACE, it will center it.
	/// @param Alpha The alpha of the texture.
	/// @param bSubPixels Whether the specified X and Y values are subpixel coordinates or not.
	void DrawTexture(int Index, int X = 0, int Y = 0, unsigned char Alpha = 255, bool bSubPixels = false);

	/// @brief Get the screen-centered X position of a texture.
	///
	/// @param Index The index of the surface.
	///
	/// @return The X position.
	int GetTextureCenteredX(int Index);

	/// @brief Get the screen-centered Y position of a texture.
	///
	/// @param Index The index of the surface.
	///
	/// @return The Y position.
	int GetTextureCenteredY(int Index);

	/// @brief Get the width of an allocated texture.
	///
	/// @param Index The index of the texture.
	///
	/// @return The width of the given texture.
	int GetTextureWidth(int Index);

	/// @brief Get the height of an allocated texture.
	///
	/// @param Index The index of the texture.
	///
	/// @return The height of the given texture.
	int GetTextureHeight(int Index);

	/// @brief Get the current mouse position relative to a texture.
	///
	/// @param Index The index of the texture.
	///
	/// @return Returns the adjusted mouse position.
	GUI_POINT GetTextureRelativeMousePoint(int Index);

private: // Control menu methods

	/// @brief Initialize the control menu.
	///
	void InitControlMenu();

private: // Input methods

	/// @brief Set the input stuff for this mode
	///
	void ProcKey();

protected: // Protected input methods

	/// @brief Remove key inputs after processing them.
	///
	/// Remove a keypress. This should always be used after checking input.
	///
	/// @param iInputFlags The flags to remove.
	/// @param iType The type of input to remove the key from.
	void RemoveKey(int iInputFlags, KeyInputType iType = KeyInputType::KIT_Key);

	/// @brief Get the mouse position relative to a surface.
	///
	/// @param iIndex The local surface index to make the mouse position relative to.
	///
	/// @return Returns the mouse position, relative to the given surface.
	GUI_POINT GetRelativeMousePosition(int iIndex);

public: // Free-call methods

	/// @brief Initialize the mode in free-call mode. This allows you to do /other/ stuff and not be limited to the code specified in this mode.
	///
	/// @return Should return true if started successfully, false if not.
	virtual bool StartMode();

	/// @brief End this mode.
	///
	/// @param value The return value the mode should return.
	virtual void EndMode(int value = 0x6FFFFFFF);

	/// @brief Run this mode for one frame.
	///
	/// @return True if the mode is still running; False if otherwise.
	virtual bool ModeTick();

	/// @brief Get the return value for this mode.
	///
	/// @return This mode's return value.
	int GetRetValue();

public: // General methods

	/// @brief Set this mode as dirty.
	///
	inline void SetDirty() { m_Dirty = true; }

	/// @brief Reload this mode.
	///
	void ReloadMode();

	/// @brief Reset the mode.
	///
	void ResetMode();

	/// @brief Set the return value.
	///
	/// @param value The value it should return.
	void SetRetValue(int value);

	/// @brief Run this mode until it stops.
	///
	/// This can be overwritten with a custom mode loop function. By default, this function runs in this order:
	/// 1. Call ResetConfiguration().
	/// 2. Call Init(). After this, start the main mode loop:
	///   2a. Call ProcessMouse() if m_ModeConfig.AllowMouseControl is set to true.
	///   2b. Call ProcessKeys() if m_ModeConfig.AllowKeyControl is set to true.
	///   2c. Call ProcessLogic()
	///   2d. If m_End is true, then it breaks the loop here.
	///   2e. Call Render() if m_Dirty is set to true. Sets m_Dirty to false afterwards.
	///   2f. Call Draw().
	///   2g. Flips the window's surface
	/// 3. Call Free().
	///
	/// @return The current mode.
	virtual int DoMode();
};

//------------------------------------------------------------------------------------------------------------------------------------

inline bool BaseModeInstance::IsTooltipOpen()
{
	return m_CurTooltip.shown;
}

#include <CSMAPI_endcode.h>