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

#pragma once

/// @ingroup Drawing
/// @{
///

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_begincode.h>

class ExternalWindow;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define INVALID_TEXTURE_COLOR 0x10203040

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CAVESTORY_MOD_API VideoTexture
{
public: // Linkage

	static VideoTexture* first;
	VideoTexture* prev;
	VideoTexture* next;

public: // Video specific stuff

	/// The window object this texture belongs to.
	///
	ExternalWindow* m_pWindow;

	/// The current draw color.
	///
	GUI_COLOR m_pDrawColor;

	/// The old lock texture to restore after unlocking this texture.
	///
	SDL_Texture* m_pOldLockTexture;
	
	/// The old clip rect to restore after unlocking this texture.
	///
	GUI_RECT m_pOldClipRect;

	/// Whether the renderer can use the old clip rect.
	///
	bool m_bHasOldClip;

	/// This texture's ACTUAL texture.
	///
	SDL_Texture* m_pTexture;

	/// This texture's renderer.
	///
	SDL_Renderer* m_pRenderer;

	/// The width of this texture.
	///
	int m_iWidth;

	/// The height of this texture.
	///
	int m_iHeight;
	
	/// Number of bits per pixel.
	///
	int m_iBitsPerPixel;

	/// A list of clip rects.
	///
	GUI_RECT* m_pClipRectList;

	/// The number of elements in the clip rect list.
	///
	int m_iClipRectCount;

	/// The size of the clip rect size.
	///
	int m_iClipRectSize;

	/// The local magnification for this texture.
	///
	int m_iMagnification;

	/// Whether this texture is currently locked or not.
	///
	bool m_bLocked;

	/// A temporary array containing SDL rects. Used for converting GUI_RECTs in functions like DrawRects.
	///
	SDL_Rect* m_pTempArray;

	/// The number of rects in m_pTempArray.
	///
	int m_iTempCount;

	/// The size of m_pTempArray.
	///
	int m_iTempSize;

public: // C++ Stuff
	
	/// @brief Default constructor.
	///
	VideoTexture();
	
	/// @brief Default deconstructor.
	///
	~VideoTexture();

public: // Init / free methods

	/// @brief Create a texture.
	///
	/// @param iWidth The width of the new texture.
	/// @param iHeight The height of the new texture.
	/// @param iBitsPerPixel Bits per pixel.
	/// @param pWindow The external window this texture should belong to.
	/// @param pRenderer The renderer that should create this texture. Leave 'NULL' to use the default renderer.
	/// @param iMagnification The magnification level for this texture. Leave at '-1' to use the current screen magnification level.
	/// @param bNoTexture If set to 'true', then no texture will be allocated.
	///
	/// @return Returns a newly allocated VideoTexture object on success, otherwise returns NULL.
	static VideoTexture* CreateTexture(int iWidth, int iHeight, int iBitsPerPixel = 32, ExternalWindow* pWindow = NULL, int iMagnification = -1, bool bNoTexture = false);

	/// @brief Free a texture.
	///
	/// @param pTexture The texture to free.
	///
	/// @return Returns true on success, false on failure.
	static bool FreeTexture(VideoTexture* pTexture);

	/// @brief Reload the main renderer's texture.
	///
	/// @param iMagnification The magnification level of the new renderer texture.
	static void ReloadRenderer(int iMagnification);

protected: // Protected methods

	/// @brief Check to see if a rect is inside of the clip rect.
	/// 
	/// @param pRect The rect to check.
	///
	/// @return Returns true if the rect is visible, false otherwise.
	inline bool InClipRect(GUI_RECT pRect);

public: // Manipulation methods

	/// @brief Resize this video texture.
	///
	/// @param iWidth The new width.
	/// @param iHeight The new height.
	void SetTextureSize(int iWidth, int iHeight);
	
	/// @brief Recreate this texture.
	///
	/// @warn This does not destroy the old texture beforehand.
	void RecreateTexture();

	/// @brief Set the clip rect for this texture.
	///
	/// @param pRect The rect to use. Pass 'NULL' to clear the last cliprect.
	void SetClipRect(GUI_RECT* pRect);
	
	/// @brief Clear the clip rect list.
	///
	void ClearClipRects();

	/// @brief Get the current clip rect.
	///
	/// @return Returns the current clip rect.
	GUI_RECT GetClipRect();
	
	/// @brief Get the current clip rect (not scaled to local magnification level).
	///
	/// @return Returns the current clip rect.
	GUI_RECT GetNonScaledClipRect();

	/// @brief Set the drawing color.
	///
	/// @param iRed The red value.
	/// @param iGreen The green value.
	/// @param iBlue The blue value.
	/// @param iAlpha The alpha value.
	void SetDrawColor(unsigned char iRed, unsigned char iGreen, unsigned char iBlue, unsigned char iAlpha);
	
	/// @brief Set the drawing color.
	///
	/// @param iColor The color.
	void SetDrawColor(unsigned long iColor);

	/// @brief Lock this texture for editing.
	///
	void Lock();

	/// @brief Unlock this texture.
	///
	void Unlock();

public: // Drawing methods

	/// @brief Clear this texture.
	///
	/// @param pRect The rect to clear. Pass 'NULL' to clear the entire texture.
	void ClearTexture(GUI_RECT* pRect = NULL);

	/// @brief Clear multiple pixels.
	///
	/// @param pPointList A list containing all points that should be drawn.
	/// @param iPointCount The number of points stored in pPointList.
	/// @param iColor The color to modulate the texture by.
	void ClearPixels(GUI_POINT* pPointList, unsigned int iPointCount);

	/// @brief Draw a pixel.
	///
	/// @param pPoint The point to draw the pixel at.
	/// @param iColor The color to modulate the texture by.
	void DrawPixel(GUI_POINT pPoint, unsigned long iColor = INVALID_TEXTURE_COLOR);

	/// @brief Draw multiple pixels.
	///
	/// @param pPointList A list containing all points that should be drawn.
	/// @param iPointCount The number of points stored in pPointList.
	/// @param iColor The color to modulate the texture by.
	void DrawPixels(GUI_POINT* pPointList, unsigned int iPointCount, unsigned long iColor = INVALID_TEXTURE_COLOR);
	
	/// @brief Draw multiple pre-magnified pixels.
	///
	/// @param pPointList A list containing all points that should be drawn.
	/// @param iPointCount The number of points stored in pPointList.
	/// @param iColor The color to modulate the texture by.
	void DrawMagnifiedPixels(GUI_POINT* pPointList, unsigned int iPointCount, unsigned long iColor = INVALID_TEXTURE_COLOR);
	
	/// @brief Draw a stretched bitmap.
	///
	/// @param pRect The point to draw the bitmap inside of. Pass 'NULL' to use the entire destination texture's size.
	/// @param pSourceRect The source rect the surface should use. Pass 'NULL' to draw the entire surface.
	/// @param iSurfaceId The source surface id.
	/// @param iColor The color to modulate the texture by.
	///
	/// @param pRect The rect to draw the rect at. Pass 'NULL' to fill the entire texture.
	void DrawStretchedBitmap(GUI_RECT* pRect, GUI_RECT* pSourceRect, Surface_Ids iSurfaceId, unsigned long iColor = INVALID_TEXTURE_COLOR);

	/// @brief Draw a colored bitmap.
	///
	/// @param pPoint The point to draw the bitmap at.
	/// @param pSourceRect The source rect the surface should use. Pass 'NULL' to draw the entire surface.
	/// @param iSurfaceId The source surface id.
	/// @param iColor The color to modulate the texture by.
	///
	/// @param pRect The rect to draw the rect at. Pass 'NULL' to fill the entire texture.
	void DrawColoredBitmap(GUI_POINT pPoint, GUI_RECT* pSourceRect, Surface_Ids iSurfaceId, unsigned long iColor);
	
	/// @brief Draw a bitmap.
	///
	/// @param pPoint The point to draw the bitmap at.
	/// @param pSourceRect The source rect the surface should use. Pass 'NULL' to draw the entire surface.
	/// @param iSurfaceId The source surface id.
	/// @param bAbsolutePos If true, then the rect is not magnified.
	///
	/// @param pRect The rect to draw the rect at. Pass 'NULL' to fill the entire texture.
	void DrawBitmap(GUI_POINT pPoint, GUI_RECT* pSourceRect, Surface_Ids iSurfaceId, bool bAbsolutePos = false);

	/// @brief Draw a bitmap box.
	///
	/// @param out_rect The rect in which the box will be drawn. Pass 'NULL' to fill the entire texture.
	/// @param bmp_rects
	/// @param type The source surface id.
	/// @param bmp_id The surface to pull the source textures from.
	/// @param iAlpha The alpha transparency value.
	///
	void DrawBitmapBox(GUI_RECT* out_rect, GUI_RECT* bmp_rects, CacheSurface::BitmapBoxType type, Surface_Ids bmp_id, int iAlpha = 255);
	
	/// @brief Draw a striped rectangle to this texture.
	///
	/// @param pRect The rect to draw the rect at. Pass 'NULL' to fill the entire texture.
	/// @param iColor The color of the rect. Keep as 'INVALID_TEXTURE_COLOR' to use the currently defined draw color.
	void DrawStripedRect(GUI_RECT* pRect, unsigned long iColor = INVALID_TEXTURE_COLOR);

	/// @brief Draw a rectangle to this texture.
	///
	/// @param pRect The rect to draw the rect at. Pass 'NULL' to fill the entire texture.
	/// @param iColor The color of the rect. Keep as 'INVALID_TEXTURE_COLOR' to use the currently defined draw color.
	/// @param bAbsolutePos If true, then the rect is not magnified.
	void DrawRect(GUI_RECT* pRect, unsigned long iColor = INVALID_TEXTURE_COLOR, bool bAbsolutePos = false);
	
	/// @brief Draw a filled rectangle to this texture.
	///
	/// @param pRect The rect to draw the rect at. Pass 'NULL' to fill the entire texture.
	/// @param iColor The color to draw.
	/// @param bAbsolutePos If true, then the rect is not magnified.
	void DrawFilledRect(GUI_RECT* pRect, unsigned long iColor = INVALID_TEXTURE_COLOR, bool bAbsolutePos = false);

	/// @brief Draw a striped line.
	///
	/// @param pStart The starting point.
	/// @param pEnd The ending point.
	/// @param iColor The color of the rect. Keep as 'INVALID_TEXTURE_COLOR' to use the currently defined draw color.
	void DrawStripedLine(GUI_POINT pStart, GUI_POINT pEnd, unsigned long iColor = INVALID_TEXTURE_COLOR);

	/// @brief Draw a line.
	///
	/// @param pStart The starting point.
	/// @param pEnd The ending point.
	void DrawLine(GUI_POINT pStart, GUI_POINT pEnd, unsigned long iColor = INVALID_TEXTURE_COLOR);
	
	/// @brief Draw a pre-magnified line.
	///
	/// @param pStart The starting point.
	/// @param pEnd The ending point.
	void DrawMagnifiedLine(GUI_POINT pStart, GUI_POINT pEnd, unsigned long iColor = INVALID_TEXTURE_COLOR);
	
	/// @brief Draw text on this texture.
	///
	/// @param pPoint The point at which to draw the text.
	/// @param pFont The font object to use.
	/// @param pText The text to draw.
	/// @param iTextLen The length of the text.
	/// @param iColor The color of the text. Keep as 'INVALID_TEXTURE_COLOR' to use the currently defined draw color.
	/// @param bWrap Wrap the text inside of the clip rect.
	/// @param iShadowSize The size of the text shadow.
	/// @param iShadowColor The color of the text shadow.
	void DrawText(GUI_POINT pPoint, FontObject* pFont, const char* pText, int iTextLen = -1, unsigned long iColor = INVALID_TEXTURE_COLOR, bool bWrap = false, int iShadowSize = 0, unsigned long iShadowColor = 0);

public: // Methods that require temporary rect list

	/// @brief Draw multiple rectangles to this texture.
	///
	/// @param iColor The color of the rect. Keep as 'INVALID_TEXTURE_COLOR' to use the currently defined draw color.
	void DrawRects(unsigned long iColor = INVALID_TEXTURE_COLOR);

public: // Ease-of-access methods

	/// @brief Reset the temporary rectangle list.
	///
	void ClearTempRectList();

	/// @brief Add a temporary rect to the rect list.
	///
	/// @param pRect The rect to add.
	///
	/// @note This function is used for DrawRects functionality.
	void AddTempRect(GUI_RECT pRect);

public: // Self-drawing methods

	/// @brief Draw this texture onto the screen with an alpha value.
	///
	/// @param iX The X position to draw the texture at.
	/// @param iY The Y position to draw the texture at.
	/// @param iAlpha The alpha value.
	void PutAlpha(int iX, int iY, int iAlpha);
	
	/// @brief Draw this texture onto the screen at a subpixel coordinate.
	///
	/// @param iX The X position to draw the texture at.
	/// @param iY The Y position to draw the texture at.
	/// @param iAlpha The alpha value.
	void PutSubPixel(int iX, int iY, int iAlpha);
	
	/// @brief Draw this texture onto the screen.
	///
	/// @param iX The X position to draw the texture at.
	/// @param iY The Y position to draw the texture at.
	void Put(int iX, int iY);
};

extern CAVESTORY_MOD_API VideoTexture* gRendererTexture;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <CSMAPI_endcode.h>

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


/// @}
///