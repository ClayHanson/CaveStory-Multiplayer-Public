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
  @file CSMAPI_drawing.h
*/

#pragma once

class VideoTexture;
struct NPCHAR;
#include <List.h>
#include <CSMAPI_begincode.h>

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @addtogroup Mouse_Input
/// @{
///

struct Mouse
{
	static CAVESTORY_MOD_API void SetMousePosition(GUI_POINT new_position);
	static CAVESTORY_MOD_API GUI_POINT GetMousePosition();

	static CAVESTORY_MOD_API void SetVisible(bool val);
	static CAVESTORY_MOD_API bool IsVisible();
};

/// @}
///

/// @addtogroup Fonts
/// @{
///

typedef struct FontObject FontObject;
class CAVESTORY_MOD_API FontManager;

#define FONT_OBJECT_SIZE 1040

#define TEXT_SIZE(font_object, string, ptr_x, ptr_y) GetTextSize(font_object, string, strlen(string), &ptr_x, &ptr_y); ptr_x /= magnification; ptr_y /= magnification;
#define TEXT_SIZE_2(font_object, string, string_length, ptr_x, ptr_y) GetTextSize(font_object, string, string_length, &ptr_x, &ptr_y); ptr_x /= magnification; ptr_y /= magnification;
#define TEXT_WIDTH(font_object, string) (GetTextWidth(font_object, string) / magnification)
#define TEXT_HEIGHT(font_object, string) (GetTextHeight(font_object, string) / magnification)
#define TEXT_WIDTH_2(font_object, string, string_length) (GetTextWidth(font_object, string, string_length) / magnification)
#define TEXT_HEIGHT_2(font_object, string, string_length) (GetTextHeight(font_object, string, string_length) / magnification)
#define TEXT_WIDTH_2S(font_object, string) (GetTextWidth(font_object, string, strlen(string)) / magnification)
#define TEXT_HEIGHT_2S(font_object, string) (GetTextHeight(font_object, string, strlen(string)) / magnification)
#define FONT_ASCII_HEIGHT(font_object) (GetTextHeight(font_object, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+-=[]\\{}|:\";',./<>?~`") / magnification)

extern CAVESTORY_MOD_API void SetFontCharacterWhitelist(const unsigned char* pWhitelist);
extern CAVESTORY_MOD_API void SetFontWhitelistEnabled(bool bEnabled);
extern CAVESTORY_MOD_API const unsigned char* GetFontCharacterWhitelist();
extern CAVESTORY_MOD_API FontObject* LoadFontFromFile(const char* pFileName, unsigned int cell_width, unsigned int cell_height, VideoTexture* pTexture = NULL, FontManager* pManager = NULL);
extern CAVESTORY_MOD_API FontObject* LoadFontFromData(const unsigned char* data, size_t data_size, unsigned int cell_width, unsigned int cell_height, VideoTexture* pTexture = NULL, FontManager* pManager = NULL);
extern CAVESTORY_MOD_API FontObject* LoadFont(const char* font_filename, unsigned int cell_width, unsigned int cell_height, VideoTexture* pTexture = NULL, FontManager* pManager = NULL);
extern CAVESTORY_MOD_API void GetTextSize(FontObject* font_object, const char* string, size_t string_length, int* size_x, int* size_y);
extern CAVESTORY_MOD_API unsigned int GetTextWidth(FontObject* font_object, const char* string, size_t string_length = size_t(-1));
extern CAVESTORY_MOD_API unsigned int GetTextHeight(FontObject* font_object, const char* string, size_t string_length = size_t(-1));
extern CAVESTORY_MOD_API BOOL IsFontLinkedToTexture(FontObject* font_object, VideoTexture* pTexture);
extern CAVESTORY_MOD_API int GetFontMagnification(FontObject* font_object);
extern CAVESTORY_MOD_API int GetFontHeight(FontObject* font_object);
extern CAVESTORY_MOD_API void DrawNormalText(FontObject* font_object, unsigned char* bitmap_buffer, size_t bitmap_pitch, int bitmap_width, int bitmap_height, int x, int y, unsigned long colour, const char* string, size_t string_length);
extern CAVESTORY_MOD_API void DrawClippedText(FontObject* font_object, unsigned char* bitmap_buffer, size_t bitmap_pitch, int bitmap_width, int bitmap_height, int x, int y, unsigned long colour, const char* string, size_t string_length, int clipLEFT, int clipTOP, int clipRIGHT, int clipBOTTOM, float iOutAlpha = 1.f);
extern CAVESTORY_MOD_API void UnloadFont(FontObject* font_object);
extern CAVESTORY_MOD_API unsigned long UTF8ToUnicode(const unsigned char* string, unsigned int* bytes_read);

extern CAVESTORY_MOD_API void InitFont();
extern CAVESTORY_MOD_API void DeInitFont();

/// @}
///

/// @addtogroup GUI
/// @{
///

/// @brief A node for SpecialTextCache.
///
struct CAVESTORY_MOD_API SpecialTextNode
{
	/// The drawing offset for this node
	///
	GUI_POINT draw_offset;
	
	/// The font object to use
	///
	FontObject* FO;

	/// The text color
	///
	unsigned long color;

	/// A pointer to the start of the string
	///
	char* start;

	/// The length of this node's text
	///
	int length;

	/// The drawing offset for this node's bitmap
	///
	GUI_POINT bmp_offset;

	/// Bitmap surface ID
	///
	Surface_Ids surf_id;

	/// Bitmap rect
	///
	GUI_RECT src_rect;

	/// Caret position
	///
	int caret_pos;

	/// Line position
	///
	int line_no;
};

/// @brief A cache object to store parsed special text.
///
struct CAVESTORY_MOD_API SpecialTextCache
{
	/// The draw rect. This doesn't affect anything at the moment.
	///
	GUI_RECT rect;

	/// The total size of the text, including font changes, new lines, etc.
	/// 
	GUI_POINT mTotalSize;

	/// Node list for this cache
	///
	List<SpecialTextNode*> mTextList;

	/// The full text for this cache
	///
	char* mFullText;
	int mFullTextLen;

	/// Constructor
	///
	SpecialTextCache();

	/// Deconstructor
	///
	~SpecialTextCache();

	/// @brief Parse a string.
	///
	/// @param pFont The font to draw the string with.
	/// @param pString The string to parse.
	/// @param iLength The length of the string. If -1, then it will be determined using 'strlen()'.
	/// @param iColor The starting color of the text.
	/// @param iDestWidth The width of the rect where the text will be drawn. If you want to enable text wrapping, then set this to anything but 0.
	///
	/// @return Returns true on success.
	bool Parse(FontObject* pFont, const char* pString, int iLength = -1, unsigned long iColor = 0xFFFFFFFF, int iDestWidth = 0);

	/// @brief Free this object's cache.
	///
	void Free();
};

/// @brief Rendering pipeline for Cavestory Multiplayer.
///
struct CAVESTORY_MOD_API CacheSurface
{
	/// Bitmap box type for #CacheSurface::GetBitmapBoxSize and #CacheSurface::DrawBitmapBox.
	///
	enum BitmapBoxType
	{
		/// Dynamic width (requires 3 'bmp_rects' elements) -- Left, mid-loop, right
		BMPBOX_TYPE_WIDTH = 0,

		/// Dynamic height (requires 3 'bmp_rects' elements) -- Top, mid-loop, bottom
		BMPBOX_TYPE_HEIGHT,

		/// Dynamic width & height (requires 9 'bmp_rects' elements) -- Top-left, top-loop, top-right, mid-left, mid-loop, mid-right, btm-left, btm-loop, btm-right
		BMPBOX_TYPE_BOTH
	};
	
	/// @brief Get the size of a string of text w/ a font.
	///
	/// @param pFont The font to use.
	/// @param pText The text to get the size of.
	/// @param iTextLen The text length.
	///
	/// @return Returns the text size.
	static GUI_POINT GetTextSize(FontObject* pFont, const char* pText, int iTextLen = -1);

	/// @brief Set the texture that should be used when rendering stuff.
	///
	/// @param pTexture The texture to set.
	///
	/// @see Surface_Ids
	/// @return Returns true on success, false otherwise.
	static BOOL SetCurrentTexture(VideoTexture* pTexture);
	
	/// @brief Set the surface ID that we're editing.
	///
	/// @param sid The surface ID to edit.
	///
	/// @see Surface_Ids
	/// @return Returns true on success, false otherwise.
	static BOOL SetSurfaceID(Surface_Ids sid);

	/// @brief Get the surface ID that we're currently editing.
	///
	/// @return Returns the selected surface ID.
	static Surface_Ids GetSurfaceID();
	
	/// @brief Get the currently selected video texture.
	///
	/// @return Returns the selected video texture object.
	static VideoTexture* GetCurrentVideoTexture();

	/// @brief Self-explanatory.
	///
	/// @return Returns how many dynamic surfaces are still allocatable.
	static int GetFreeSurfaceCount();

	/// @brief Try to contain a point inside of the clip rect.
	///
	/// @param p The point to test.
	/// 
	/// @return Returns true if the point was successfully truncated to the cliprect, false if it could not be.
	static bool TryClipPoint(GUI_POINT& p);
	
	/// @brief Check to see if a point is inside the current clip rect.
	///
	/// @param p The point to test.
	/// 
	/// @return Returns true if the given point is inside the current clip rect.
	static bool IsInClipRect(GUI_POINT p);

	/// @brief Check to see if a rect is inside the current clip rect.
	///
	/// @param r The rect to test.
	/// 
	/// @return Returns true if the given rect is inside the current clip rect.
	static bool IsInClipRect(GUI_RECT r);

	/// @brief Add a new clip rect.
	///
	/// @param r The rect to set.
	///
	/// @note When a clip rect is set, it is added to a list of Clip Rects, and can be removed from said list by passing 'NULL' to 'r'.
	/// @note Additionally, if there is already a clip rect in the list, it will contain the new clip rect to be completely inside of the old one.
	static void SetClipRect(GUI_RECT* r);
	
	/// @brief Get the current clip rect.
	///
	/// @param r A pointer to a RECT that will hold the clip rect.
	///
	/// @return Returns the current clip rect.
	static void GetClipRect(GUI_RECT* r);

	/// @brief Clear the clip rect.
	///
	/// @see SetClipRect
	static void ClearClipRectList();

	/// @brief Get the surface size of a surface.
	///
	/// @param sid The surface ID to get the size of.
	///
	/// @return Returns the size of the specified surface id if it's allocated; returns GUI_POINT(0, 0) if the given surface ID is not allocated.
	static GUI_POINT GetSurfaceIDSize(Surface_Ids sid);
	
	/// @brief Get the surface magnification level of a surface.
	///
	/// @param sid The surface ID to get the magnifier of.
	///
	/// @return Returns the magnification level of the specified surface id if it's allocated; returns the global magnification level if the given surface ID is not allocated.
	static int GetSurfaceIDMagnification(Surface_Ids sid);

	/// @brief Get the next free Dynamic Surface ID.
	///
	/// @param iSkipCount Skip this many free slots before returning one.
	/// @param bAsyncUse If set to true, then this function will act as if this surface is in use, and will not return it next time.
	/// @param bLockMutex If set to true, then the mutex will be locked.
	///
	/// @return Returns the next allocatable surface ID if available; Otherwise, returns Surface_Ids::SURFACE_ID_MAX if no surfaces are available.
	///
	/// @see CacheSurface::FreeSurfaceID
	static Surface_Ids GetNextFreeSurfaceID(int iSkipCount = 0, bool bAsyncUse = false, bool bLockMutex = true);

	/// @brief Checks whether the specified surface is allocated or not.
	///
	/// @param sid The surface ID to check.
	///
	/// @return Returns true if the surface is in use.
	static bool IsSurfaceAllocated(Surface_Ids sid);
	
	/// @brief Free a surface ID.
	///
	/// @param sid The surface ID to free.
	static void FreeSurfaceID(Surface_Ids sid);
	
	/// @brief Free a surface ID and set it as non-async.
	///
	/// @param sid The surface ID to free.
	static void UnlockSurfaceID(Surface_Ids sid);

	/// @brief Allocate a surface ID.
	///
	/// @param w The width of the new surface.
	/// @param h The height of the new surface.
	/// @param iSurfId The surface to use. Leave as 'SURFACE_ID_MAX' to use the currently selected surface.
	///
	/// @return Returns true on success.
	///
	/// @see CacheSurface::SetSurfaceID
	/// @see CacheSurface::GetNextFreeSurfaceID
	/// @see CacheSurface::FreeSurfaceID
	static bool AllocateSurface(int w, int h, Surface_Ids iSurfId = SURFACE_ID_MAX);

	/// @brief Load an image from a file into the selected surface ID.
	///
	/// @param pFileName The file name of the image.
	/// @param iSurfId The surface to use. Leave as 'SURFACE_ID_MAX' to use the currently selected surface.
	/// @param bAsync Whether this is an asynchronous operation or not.
	///
	/// @return Returns true on success.
	///
	/// @see CacheSurface::SetSurfaceID
	/// @see CacheSurface::GetNextFreeSurfaceID
	/// @see CacheSurface::FreeSurfaceID
	static bool LoadFileToSurface(const char* pFileName, Surface_Ids iSurfId = SURFACE_ID_MAX, bool bAsync = false);

	/// @brief Draw a pixel.
	///
	/// @param pPoint The point to draw a pixel at.
	/// @param color The color of the pixel.
	/// @param bAbsolutePos If this is true, then pPoint will not be magnified.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawPixel(GUI_POINT pPoint, unsigned long color, bool bAbsolutePos = false);
	
	/// @brief Draw pixels.
	///
	/// @param pPointList The point to draw a pixel at.
	/// @param iPointCount The number of points in pPointList.
	/// @param color The color of the pixel.
	/// @param bAbsolutePos If this is true, then points in the pPointList will not be magnified.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawPixels(GUI_POINT* pPointList, unsigned int iPointCount, unsigned long color, bool bAbsolutePos = false);

	/// @brief Draw a line.
	///
	/// @param start Line start point.
	/// @param end Line end point.
	/// @param color The color of the line.
	/// @param bUseMagnification Draw a line as big as the current magnification amount.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawLine(GUI_POINT start, GUI_POINT end, unsigned long color, bool bUseMagnification = true);
	
	/// @brief Draw a striped line.
	///
	/// @param start Line start point.
	/// @param end Line end point.
	/// @param color The color of the line.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawStripedLine(GUI_POINT start, GUI_POINT end, unsigned long color);
	
	/// @brief Draw a striped rect.
	///
	/// @param pRect The rect to draw.
	/// @param iColor The color of the line.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawStripedRect(GUI_RECT* pRect, unsigned long iColor);
	
	/// @brief Draw a line with color gradient.
	///
	/// @param start Line start point.
	/// @param end Line end point.
	/// @param colors A pointer to an array of colors.
	/// @param colorcount How many colors are in the 'colors' array. This is how many times the line will be segmented to display each color.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawGradientLine(GUI_POINT start, GUI_POINT end, unsigned long* colors, int colorcount);

	/// @brief Clear a given rect.
	///
	/// @param r The rect to clear. Can be NULL to clear the entire surface.
	static void DrawClearRect(GUI_RECT* r);

	/// @brief Fill a given rect.
	///
	/// @param r The rect to fill. Can be NULL to fill the entire surface.
	/// @param color The color to fill with.
	/// @param bAbsolutePos If this is true, then pPoint will not be magnified.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawFilledRect(GUI_RECT* r, unsigned long color, bool bAbsolutePos = false);

	/// @brief Outline a given rect.
	///
	/// @param r The rect to outline. Can be NULL to outline the entire surface.
	/// @param color The color to fill with.
	/// @param bAbsolutePos If this is true, then pPoint will not be magnified.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawRect(GUI_RECT* r, unsigned long color, bool bAbsolutePos = false);

	/// @brief Fill a given rect.
	///
	/// @param r The rect to fill. Can be NULL to fill the entire surface.
	/// @param color The color to fill with.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawFilledRectTransparent(GUI_RECT* r, unsigned long color);

	/// @brief Outline a given rect.
	///
	/// @param r The rect to outline. Can be NULL to outline the entire surface.
	/// @param color The color to fill with.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawRectTransparent(GUI_RECT* r, unsigned long color);

	/// @brief Draw a filled circle.
	/// 
	/// @param p The center of the circle.
	/// @param size The size of the circle.
	/// @param color The color of the circle.
	/// @param invert If set to true, then fill everything EXCEPT the inside of the circle.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawFilledCircle(GUI_POINT p, int size, unsigned long color, bool invert = false);

	/// @brief Draw a filled triangle.
	/// 
	/// @param points A pointer to a GUI_POINT array w/ minimum size 3.
	/// @param color The color of the triangle.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawFilledTriangle(GUI_POINT* points, unsigned long color);

	/// @brief Draw a circle.
	/// 
	/// @param p The center of the circle.
	/// @param size The size of the circle.
	/// @param color The color of the circle.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawCircle(GUI_POINT p, int size, unsigned long color);

	/// @brief Draw a triangle.
	/// 
	/// @param points A pointer to a GUI_POINT array w/ minimum size 3.
	/// @param color The color of the triangle.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawTriangle(GUI_POINT* points, unsigned long color);

	/// @brief Draw a filled bitmap rect.
	///
	/// @param drawRect The rect to fill with the source image.
	/// @param srcRect The rect to copy from. Can be NULL to copy the entire source image.
	/// @param sid The source image's surface ID.
	/// @param drawPadding Margin for drawRect.
	///
	/// @return Returns the size of the validated rect.
	static GUI_POINT DrawFilledBitmapRect(GUI_RECT* drawRect, GUI_RECT* srcRect, Surface_Ids sid, GUI_POINT drawPadding = GUI_POINT(0, 0));

	/// @brief Draw an image.
	///
	/// @param p The position to draw the image at.
	/// @param srcRect The rect to copy from. Can be NULL to copy the entire source image.
	/// @param sid The source image's surface ID.
	///
	/// @return Returns the size of the validated rect.
	static GUI_POINT DrawBitmap(GUI_POINT p, GUI_RECT* srcRect, Surface_Ids sid);
	
	/// @brief Draw an image.
	///
	/// @param r The rect to draw the bitmap in.
	/// @param srcRect The rect to copy from. Can be NULL to copy the entire source image.
	/// @param sid The source image's surface ID.
	/// @param color The color to modulate the source image by.
	///
	/// @return Returns the size of the validated rect.
	static GUI_POINT DrawStretchedBitmap(GUI_RECT* r, GUI_RECT* srcRect, Surface_Ids sid, GUI_COLOR color);

	/// @brief Draw a colored image.
	///
	/// @param p The position to draw the image at.
	/// @param srcRect The rect to copy from. Can be NULL to copy the entire source image.
	/// @param sid The source image's surface ID.
	/// @param color The color to modulate the source image by.
	///
	/// @return Returns the size of the validated rect.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static GUI_POINT DrawColoredBitmap(GUI_POINT p, GUI_RECT* srcRect, Surface_Ids sid, GUI_COLOR color);

	/// @brief Draw text.
	///
	/// @param p The position to draw the text at.
	/// @param font The font to use.
	/// @param text The text to draw.
	/// @param color The color to draw the text with.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawText(GUI_POINT p, FontObject* font, const char* text, unsigned long color);

	/// @brief Draw clipped text.
	///
	/// @param p The position to draw the text at.
	/// @param font The font to use.
	/// @param text The text to draw.
	/// @param color The color to draw the text with.
	/// @param iStringLength The length of the string to draw. Can be -1 to automatically detect the length of the string.
	/// @param bWrap Wrap the string if it reaches the edge of the clip rect.
	/// @param pDrawSize A pointer to a GUI_POINT that will hold the size of the output text.
	/// @param iShadowSize The size of the text's shadow. If set to 0, no shadow will be drawn.
	/// @param iShadowColor The color of the shadow.
	///
	/// @note Unlike CacheSurface::DrawText, this function enforces the clip rect.
	///
	/// @see CSM_RGB
	/// @see CSM_RGBA
	static void DrawClippedText(GUI_POINT p, FontObject* font, const char* text, unsigned long color, int iStringLength = -1, bool bWrap = false, GUI_POINT* pDrawSize = NULL, int iShadowSize = 0, unsigned long iShadowColor = 0);
	
	/// @brief Draw clipped special text.
	///
	/// @param p The position to draw the text at.
	/// @param pCache The special text cache that holds the drawing instructions.
	static void DrawSpecialText(GUI_POINT p, SpecialTextCache* pCache);

	/// @brief Draw a bitmap box.
	///
	/// @param out_rect The rect to draw it at. Can be NULL to use the entire destination surface.
	/// @param bmp_rects A list of GUI_RECT. Must conform to the size required by parameter 'type'.
	/// @param type The type of bitmap box to draw.
	/// @param bmp_id The source image's surface ID.
	static void DrawBitmapBox(GUI_RECT* out_rect, GUI_RECT* bmp_rects, BitmapBoxType type, Surface_Ids bmp_id);
	
	/// @brief Draw a transparent bitmap box.
	///
	/// @param out_rect The rect to draw it at. Can be NULL to use the entire destination surface.
	/// @param bmp_rects A list of GUI_RECT. Must conform to the size required by parameter 'type'.
	/// @param type The type of bitmap box to draw.
	/// @param bmp_id The source image's surface ID.
	static void DrawTransparentBitmapBox(GUI_RECT* out_rect, GUI_RECT* bmp_rects, BitmapBoxType type, Surface_Ids bmp_id, int iAlpha);

	/// @brief Force an update for this surface internally.
	///
	static void SetNeedsUpdating();

	/// @brief Get the size of a bitmap box.
	///
	/// @param desired_extent The desired extent of the output.
	/// @param bmp_rects A list of GUI_RECT. Must conform to the size required by parameter 'type'.
	/// @param type The type of bitmap box to draw.
	///
	/// @return Returns the best-fitting extent.
	static GUI_POINT GetBitmapBoxSize(GUI_POINT* desired_extent, GUI_RECT* bmp_rects, BitmapBoxType type);

	/// @brief Draw a surface to the screen.
	///
	/// @param out_pos Where to draw the surface at.
	/// @param sid The surface ID to draw. Use CacheSurface::SURFACE_ID_MAX to draw the selected Surface ID.
	static void Put(GUI_POINT out_pos, Surface_Ids sid = SURFACE_ID_MAX);

	/// @brief Draw a transparent surface to the screen.
	///
	/// @param out_pos Where to draw the surface at.
	/// @param sid The surface ID to draw. Use CacheSurface::SURFACE_ID_MAX to draw the selected Surface ID.
	/// @param Alpha The amount the image should blend into the background.
	static void PutAlpha(GUI_POINT out_pos, Surface_Ids sid = SURFACE_ID_MAX, unsigned char Alpha = 255);

#ifdef _DEBUG
	static void ShowSurfaceId(Surface_Ids sid, GUI_RECT* r, bool waitForInput = false);
	static void DebugShow();
#endif

	static GUI_COLOR GetPixelColor(GUI_POINT p, Surface_Ids sid);
	static GUI_COLOR GetEntityPixel(NPCHAR* npc, GUI_POINT p);
};

/// Placeholder until we restore the WinAPI code
///
extern CAVESTORY_MOD_API int ghWnd;

/// @brief Draw FPS in the top-right corner.
///
extern CAVESTORY_MOD_API void PutFramePerSecound();

/// @brief Flip the renderer.
///
/// @param hWnd ghWnd
///
/// @return Returns true if the application can continue.
extern CAVESTORY_MOD_API BOOL Flip_SystemTask(
	int hWnd
);

/// @brief Draw bitmap to the screen with transparency.
///
/// @param rcView The clip rect to use.
/// @param x The X position.
/// @param y The Y position.
/// @param rect The source rect to draw.
/// @param surf_no Where to get the image to draw from.
extern CAVESTORY_MOD_API void PutBitmap3(
	RECT* rcView,
	int x,
	int y,
	RECT* rect, 
	Surface_Ids surf_no
);

/// @brief Draw bitmap to the screen.
///
/// @param rcView The clip rect to use.
/// @param x The X position.
/// @param y The Y position.
/// @param rect The source rect to draw.
/// @param surf_no Where to get the image to draw from.
extern CAVESTORY_MOD_API void PutBitmap4(
	RECT *rcView,
	int x,
	int y,
	RECT *rect,
	Surface_Ids surf_no
);

/// @brief Draw bitmap to the screen.
///
/// @param rcView The clip rect to use.
/// @param x The X position.
/// @param y The Y position.
/// @param rect The source rect to draw.
/// @param surf_no Where to get the image to draw from.
/// @param scalar How much to scale the image.
extern CAVESTORY_MOD_API void PutBitmap5(
	RECT *rcView,
	int x,
	int y,
	RECT *rect,
	Surface_Ids surf_no,
	int scalar
);

/// @brief Draw bitmap to the screen.
///
/// @param rcView The clip rect to use.
/// @param x The X position.
/// @param y The Y position.
/// @param rect The source rect to draw.
/// @param surf_no Where to get the image to draw from.
/// @param scalar How much to scale the image.
extern CAVESTORY_MOD_API void PutBitmap6(
	RECT *rcView, 
	int x, 
	int y,
	RECT *rect,
	Surface_Ids surf_no,
	int scalar
);

/// @brief Draw bitmap to the screen.
///
/// @param rcView The clip rect to use.
/// @param x The X position.
/// @param y The Y position.
/// @param rect The source rect to draw.
/// @param surf_no Where to get the image to draw from.
extern CAVESTORY_MOD_API void PutBitmap7(
	RECT* rcView,
	int x,
	int y,
	RECT* rect,
	Surface_Ids surf_no
);

/// @brief Draw bitmap to the screen.
///
/// @param rcView The clip rect to use.
/// @param x The X position.
/// @param y The Y position.
/// @param rect The source rect to draw.
/// @param surf_no Where to get the image to draw from.
/// @param r The red modulation color
/// @param g The green modulation color
/// @param b The blue modulation color
extern CAVESTORY_MOD_API void PutBitmap8(RECT* rcView, int x, int y, RECT* rect, Surface_Ids surf_no, int r, int g, int b);

/// @brief Draw bitmap to the screen.
///
/// @param rcView The clip rect to use.
/// @param x The X position.
/// @param y The Y position.
/// @param rect The source rect to draw.
/// @param surf_no Where to get the image to draw from.
/// @param alpha The transparency amount.
extern CAVESTORY_MOD_API void PutBitmap9(RECT* rcView, int x, int y, RECT* rect, Surface_Ids surf_no, int alpha);

/// @brief Draw bitmap to the screen.
///
/// @param x The X position.
/// @param y The Y position.
/// @param rect The source rect to draw.
/// @param surf_no Where to get the image to draw from.
extern CAVESTORY_MOD_API void PutBitmap10(int x, int y, RECT* rect, Surface_Ids surf_no);

/// @brief Draw bitmap to the screen.
///
/// @param x The X position.
/// @param y The Y position.
/// @param rect The source rect to draw.
/// @param surf_no Where to get the image to draw from.
/// @param r The red modulation color
/// @param g The green modulation color
/// @param b The blue modulation color
extern CAVESTORY_MOD_API void PutBitmap11(int x, int y, RECT* rect, Surface_Ids surf_no, int r, int g, int b);

/// @brief Draw bitmap to the screen with transparency.
///
/// @param dst_surf_no The surface number of the destination.
/// @param x The X position.
/// @param y The Y position.
/// @param src_rect The source rect for the image. Can be NULL.
/// @param src_surf_no The source surface number to copy from.
/// @param async Whether this is an asynchronous call or not.
extern CAVESTORY_MOD_API void PutBitmap12(RECT* rcView, Surface_Ids dst_surf_no, int x, int y, RECT* src_rect, Surface_Ids src_surf_no, bool async = false);

/// @brief Draw bitmap to the screen with transparency & color modulation.
///
/// @param dst_surf_no The surface number of the destination.
/// @param x The X position.
/// @param y The Y position.
/// @param src_rect The source rect for the image. Can be NULL.
/// @param src_surf_no The source surface number to copy from.
/// @param r The red value.
/// @param g The green value.
/// @param b The blue value.
/// @param async Whether this is an asynchronous call or not.
extern CAVESTORY_MOD_API void PutBitmap13(RECT* rcView, Surface_Ids dst_surf_no, int x, int y, RECT* src_rect, Surface_Ids src_surf_no, int r, int g, int b, bool async = false);

/// @brief Draw a stretched colored bitmap to the screen.
///
/// @param x The X position.
/// @param y The Y position.
/// @param w The width.
/// @param h The height.
/// @param rect The source rect to draw.
/// @param surf_no Where to get the image to draw from.
/// @param r The red modulation color
/// @param g The green modulation color
/// @param b The blue modulation color
extern CAVESTORY_MOD_API void PutBitmap14(int x, int y, int w, int h, RECT* rect, Surface_Ids surf_no, int r, int g, int b);

/// @brief Draw a surface directly to the screen.
///
/// @param pDstRect The destination rect to draw the bitmap at on the screen.
/// @param pSrcRect The source rect to draw.
/// @param iSurfaceNo Where to get the image to draw from.
extern CAVESTORY_MOD_API void PutBitmap15(
	RECT* pDstRect,
	RECT* pSrcRect,
	Surface_Ids iSurfaceNo
);

/// @brief Draw a bitmap that's rotated around a center point.
///
/// @param rcView The clip rect to use.
/// @param x The X position.
/// @param y The Y position.
/// @param rect The source rect to draw.
/// @param surf_no Where to get the image to draw from.
/// @param angle The angle at which to draw the bitmap.
/// @param center_x The rotation center of the sprite.
/// @param center_y The rotation center of the sprite.
extern CAVESTORY_MOD_API void PutBitmap16(
	RECT* rcView,
	int x,
	int y,
	RECT* rect,
	Surface_Ids surf_no,
	float angle,
	int center_x,
	int center_y
);

/// @brief Draw a surface directly to the screen, with magnified support.
///
/// @param pDstRect The destination rect to draw the bitmap at on the screen.
/// @param pSrcRect The source rect to draw.
/// @param iSurfaceNo Where to get the image to draw from.
extern CAVESTORY_MOD_API void PutBitmap17(
	RECT* pDstRect,
	RECT* pSrcRect,
	Surface_Ids iSurfaceNo
);

/// @brief Draw a surface directly to another surface, with magnified support.
///
/// @param pDstRect The destination rect to draw the bitmap at on the screen.
/// @param pSrcRect The source rect to draw.
/// @param iSurfaceNo Where to get the image to draw from.
/// @param iDestSurfaceNo The surface we're going to draw on.
extern CAVESTORY_MOD_API void PutBitmap18(
	RECT* pDstRect,
	RECT* pSrcRect,
	Surface_Ids iSurfaceNo,
	Surface_Ids iDestSurfaceNo
);

/// @brief Set the color modulation of a surface.
///
/// @param iSurfNo The surface number.
/// @param iColor The desired of the surface. Use CSM_RGB or CSM_RGBA.
extern CAVESTORY_MOD_API void ModBitmap(
	Surface_Ids iSurfNo,
	unsigned long iColor
);

/// @brief Clear the color modulation of a surface.
///
/// @param iSurfNo The surface whose color modulation should be reset.
extern CAVESTORY_MOD_API void ClearBitmapMod(
	Surface_Ids iSurfNo
);

/// @brief Draw a number to the screen.
///
/// @param x The X position.
/// @param y The Y position.
/// @param value The number to draw.
extern CAVESTORY_MOD_API void PutNumber3(int x, int y, int value);

/// @brief Draw a number to the screen.
///
/// @param x The X position.
/// @param y The Y position.
/// @param value The number to draw.
/// @param bZero If this should draw a zero.
extern CAVESTORY_MOD_API void PutNumber4(int x, int y, int value, BOOL bZero);

/// @brief Draw an NPC to the screen.
///
/// @param npc The NPC to draw.
/// @param fx The camera X position.
/// @param fy The camera Y position.
/// @param bIgnorePutFunc Whether to ignore the put function this NPC may or may not have.
/// @param iDrawX A pointer to an integer to hold the position that the entity was drawn at (top left corner).
/// @param iDrawY A pointer to an integer to hold the position that the entity was drawn at (top left corner).
extern CAVESTORY_MOD_API void PutSpecificNpChar(NPCHAR* npc, int fx, int fy, bool bIgnorePutFunc = false, int* iDrawX = NULL, int* iDrawY = NULL);

/// @}
///

#include <CSMAPI_endcode.h>

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------