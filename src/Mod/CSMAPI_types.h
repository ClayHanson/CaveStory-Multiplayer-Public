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
  @file CSMAPI_types.h
*/

#pragma once

namespace CaveNet
{
	namespace DataStructures
	{
		class NetPacket;
	}
}

class VideoTexture;

#include "CSMAPI_begincode.h"

#ifdef CAVESTORY_MOD_EXPORTS
#include <SDL.h>
#else
typedef struct SDL_Rect
{
	int x, y;
	int w, h;
} SDL_Rect;
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

typedef int BOOL;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

// wip
namespace CaveNet
{
	namespace DataStructures
	{
		class NetClient;
		class NetPlayer;
		class NetTeam;
	}
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

extern CAVESTORY_MOD_API int magnification;
extern CAVESTORY_MOD_API bool fullscreen;
extern CAVESTORY_MOD_API const char** MainArgv;
extern CAVESTORY_MOD_API int MainArgc;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

typedef void* NetClientPointer;
typedef int NetClientGhostId;

class CustomPlayer;
struct NPCHAR;
struct MYCHAR;
extern CAVESTORY_MOD_API int empty;

/// @brief A custom draw function for #NPCHAR.
///
/// @param npc A pointer to the NPC.
/// @param fx The X offset of the camera.
/// @param fy The Y offset of the camera.
///
typedef void(*PutNPCFunc)(NPCHAR* npc, int fx, int fy);

/// @brief A custom free function for #NPCHAR. Called just before an NPC is changed.
///
/// @param npc A pointer to the NPC.
typedef void(*FreeNPCFunc)(NPCHAR* npc);

/// @brief Custom collision detection function.
///
/// @param npc A pointer to the NPC.
///
/// @return Should return TRUE if the normal collision detection routine should be run.
typedef BOOL(*CollideNPCFunc)(NPCHAR* npc);

#if !defined(_CAVEEDITOR) && (!defined(CAVESTORY_MOD_EXPORTS) || !defined(DONT_DEFINE_RECT))
/// Rect structure
///
struct RECT
{
	/// Left position (X)
	///
	union
	{
		int left;
		int front;
	};

	/// Top position (Y)
	///
	int top;

	/// Right position (X + W)
	///
	union
	{
		int right;
		int back;
	};

	/// Bottom position (Y + H)
	///
	int bottom;
};
#endif

/// Point structure
///
struct CSM_POINT
{
	/// X Position
	///
	int x;

	/// Y Position
	///
	int y;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// Camera struct.
///
struct FRAME
{
	/// X offset
	///
	int x;

	/// Y offset
	///
	int y;

	/// Target X offset
	///
	int* tgt_x;

	/// Target Y offset
	///
	int* tgt_y;

	/// How fast the camera should be
	///
	int wait;

	/// Quake counter
	///
	int quake;

	/// Quake 2 counter
	///
	int quake2;

	/// Force the camera to focus on the horizontal center of the map
	///
	bool focus_center_x;
	
	/// Force the camera to focus on the vertical center of the map
	///
	bool focus_center_y;

	/// The last tile X position
	///
	int last_tile_x;

	/// The last tile Y position
	///
	int last_tile_y;
};

/// Custom frame limit object
///
struct CUSTOM_FRAME_LIMITS
{
	/// Whether to use or not.
	///
	bool use;

	/// X limit
	///
	int x;

	/// Y limit
	///
	int y;

	/// W limit
	///
	int w;

	/// H limit
	///
	int h;
};

extern CAVESTORY_MOD_API CUSTOM_FRAME_LIMITS gFrameLimits;
extern CAVESTORY_MOD_API FRAME gFrame;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

struct CAVESTORY_MOD_API GUI_RECT;
struct CAVESTORY_MOD_API GUI_POINT;

typedef struct FontObject FontObject;

/// Manages fonts.
///
class CAVESTORY_MOD_API FontManager
{
public:
	/// First font
	/// @private
	static FontManager* lFirst;

	/// Next font in linkage
	/// @private
	FontManager* lNext;

	/// Previous font in linkage
	/// @private
	FontManager* lPrev;

public:
	/// The name of this font
	///
	char mFontName[127];

	/// The font object tied to this manager
	///
	FontObject* mFont;

	/// How many users this font has
	/// If 0, it will be freed
	///
	int mUserCount;

	/// Font width
	///
	int mWidth;

	/// Font height
	///
	int mHeight;

public:

	FontManager(const char* font_name, FontObject* fo, int width, int height);
	~FontManager();

public:
	/// @brief Load a font by name & size.
	///
	/// @param font_name The name of the font.
	/// @param font_width The width of the font.
	/// @param font_height The height of the font.
	/// @param bDoNotuse Don't increment this font's ref value (unless it wasn't previously cached). DO NOT USE THIS UNLESS YOU KNOW WHAT YOU'RE DOING.
	/// @param pTexture The texture this font belongs to.
	///
	/// @return Returns a FontObject* on success, NULL on failure.
	static FontObject* LoadFont(const char* font_name, int font_width, int font_height, bool bDoNotUse = false, VideoTexture* pTexture = NULL);

	/// @brief Unload a font object.
	///
	/// @param fo The font object to unload.
	static void UnloadFont(FontObject* fo);

	/// @brief Increment the use count for a font.
	///
	/// @param pFont The font object.
	static void UseFont(FontObject* pFont);

	/// @brief Ready all loaded font objects for a window resizing.
	///
	static void ReadyAllFontsForWindowResize();
	
	/// @brief Resize all fonts following a magnification change.
	///
	static void ResizeAllFonts();

	/// @brief Destroy all textures related to the given renderer.
	///
	/// @param pTarget The target renderer.
	static void DestroyRendererTextures(void* pTarget);
};

/// Manages colors.
///
struct CAVESTORY_MOD_API GUI_COLOR
{
	/// Red value.
	///
	unsigned char r;

	/// Green value.
	///
	unsigned char g;

	/// Blue value.
	///
	unsigned char b;

	/// Alpha value.
	///
	unsigned char a;

	/// @brief Convert this color to RGBA.
	///
	/// @return Returns the color.
	inline unsigned long to_rgb()
	{
		return ((r) | ((g) << 8) | ((b) << 16) | ((a) << 24));
	}

	/// @brief Convert from RGBA to fill r, g, b and a.
	///
	/// @param color The color to use.
	inline void from_rgb(unsigned long color)
	{
		r = (unsigned char)((color) & 0xFF);
		g = (unsigned char)(((color) >> 8) & 0xFF);
		b = (unsigned char)(((color) >> 16) & 0xFF);
		a = (unsigned char)(((color) >> 24) & 0xFF);
	}

	bool operator==(GUI_COLOR other) { return (other.r == r && other.g == g && other.b == b && other.a == a); }
	bool operator!=(GUI_COLOR other) { return (other.r != r || other.g != g || other.b != b || other.a != a); }

	GUI_COLOR()                                     { r = 0; g = 0; b = 0; a = 0; }
	GUI_COLOR(int _r, int _g, int _b, int _a = 255) : r(_r), g(_g), b(_b), a(_a) {}
	GUI_COLOR(unsigned long color)                  { from_rgb(color); }
};

struct CAVESTORY_MOD_API GUI_POINT_F;

/// Manages points.
///
struct CAVESTORY_MOD_API GUI_POINT
{
	/// The X position.
	///
	int x;

	/// The Y position.
	///
	int y;

	GUI_POINT() : x(0), y(0)
	{
	}

	GUI_POINT(int _x, int _y) : x(_x), y(_y)
	{
	}

	GUI_POINT(RECT r) : x(r.left), y(r.top)
	{
	}

	GUI_POINT(GUI_RECT r);

	// Multiplies the points by the current screen magnification
	inline GUI_POINT to_screenspace()
	{
		GUI_POINT ret;
		ret.x = x * magnification;
		ret.y = y * magnification;
		return ret;
	}

	// Divides the points by the current screen magnification
	inline GUI_POINT from_screenspace()
	{
		GUI_POINT ret;
		ret.x = x / magnification;
		ret.y = y / magnification;
		return ret;
	}

	// Divides the points by the current screen magnification
	inline GUI_POINT make_frame_relative()
	{
		GUI_POINT ret;
		ret.x = x + gFrame.x;
		ret.y = y + gFrame.y;
		return ret;
	}

	// Divides the points by the current screen magnification
	inline GUI_POINT reset_frame_relativity()
	{
		GUI_POINT ret;
		ret.x = x - gFrame.x;
		ret.y = y - gFrame.y;
		return ret;
	}

	inline GUI_POINT normalize()
	{
		float mag = sqrtf((x * x) + (y * y));
		GUI_POINT ret(x / mag, y / mag);

		return ret;
	}

	inline float angle_to(GUI_POINT other)
	{
		float dot = (float)x * (float)other.x + (float)y * (float)other.y;
		float det = (float)x * (float)other.y - (float)y * (float)other.x;

		return atan2f(det, dot);
	}

	inline float distance(GUI_POINT other)
	{
		return sqrtf(powf(other.x - x, 2) + powf(other.y - y, 2));
	}

	inline float distance_x(GUI_POINT other)
	{
		return fabsf(other.x - x);
	}

	inline float distance_y(GUI_POINT other)
	{
		return fabsf(other.y - y);
	}

	inline int dot(GUI_POINT other)
	{
		return (this->x * other.x) + (this->y * other.y);
	}
	
	inline int cross(GUI_POINT other)
	{
		return (this->x * other.y) - (this->y * other.x);
	}

	// line1 = a to b, line2 = c to d
	static inline bool intersect(GUI_POINT a, GUI_POINT b, GUI_POINT c, GUI_POINT d, GUI_POINT* hit)
	{
		double detL1 = ((double)a.x * (double)b.y) - ((double)a.y * (double)b.x);
		double detL2 = ((double)c.x * (double)d.y) - ((double)c.y * (double)d.x);
		double x1mx2 = a.x - b.x;
		double x3mx4 = c.x - d.x;
		double y1my2 = a.y - b.y;
		double y3my4 = c.y - d.y;

		double xnom  = detL1 * x3mx4 - x1mx2 * detL2;
		double ynom  = detL1 * y3my4 - y1my2 * detL2;
		double denom = x1mx2 * y3my4 - y1my2 * x3mx4;

		if (denom == 0.0)
			return false;

		hit->x = xnom / denom;
		hit->y = ynom / denom;

		return true;
	}

	inline void operator+=(const GUI_POINT& n)     { x += n.x; y += n.y; }
	inline void operator*=(const int n)            { x *= n; y *= n; }
	inline bool operator==(const GUI_POINT& o)     { return (o.x == x && o.y == y); }
	inline bool operator!=(const GUI_POINT& o)     { return (o.x != x || o.y != y); }
	inline GUI_POINT operator+(const GUI_POINT& n) { GUI_POINT out; out.x = x + n.x; out.y = y + n.y; return out; }
	inline GUI_POINT operator-(const GUI_POINT& n) { GUI_POINT out; out.x = x - n.x; out.y = y - n.y; return out; }
	inline GUI_POINT operator*(const int n)        { return GUI_POINT(x * n, y * n); }
	inline GUI_POINT operator/(const int n)        { return GUI_POINT(x / n, y / n); }
	inline GUI_POINT operator*(const float n)      { return GUI_POINT(int((float)x * n), int((float)y * n)); }
	inline GUI_POINT operator/(const float n)      { return GUI_POINT(int((float)x / n), int((float)y / n)); }
	bool operator==(const GUI_POINT_F& o);
	bool operator!=(const GUI_POINT_F& o);
	GUI_POINT_F operator+(const GUI_POINT_F& n);
	GUI_POINT_F operator-(const GUI_POINT_F& n);
	GUI_POINT_F operator*(const GUI_POINT_F& n);
	GUI_POINT_F operator/(const GUI_POINT_F& n);

	inline int HashIndex()                              { return ((y & 0xFFFF) << 16) | ((x & 0xFFFF)); }
	inline int HashIndex() const                        { return ((y & 0xFFFF) << 16) | ((x & 0xFFFF)); }
	inline bool operator<(GUI_POINT other)              { return HashIndex() <  other.HashIndex(); }
	inline bool operator<=(GUI_POINT other)             { return HashIndex() <= other.HashIndex(); }
	inline bool operator>(GUI_POINT other)              { return HashIndex() >  other.HashIndex(); }
	inline bool operator>=(GUI_POINT other)             { return HashIndex() >= other.HashIndex(); }
	inline bool operator<(const GUI_POINT other)  const { return HashIndex() <  other.HashIndex(); }
	inline bool operator<=(const GUI_POINT other) const { return HashIndex() <= other.HashIndex(); }
	inline bool operator>(const GUI_POINT other)  const { return HashIndex() >  other.HashIndex(); }
	inline bool operator>=(const GUI_POINT other) const { return HashIndex() >= other.HashIndex(); }
};

/// Manages points (float).
///
struct CAVESTORY_MOD_API GUI_POINT_F
{
	float x;
	float y;

	GUI_POINT_F() : x(0), y(0) {}
	GUI_POINT_F(float _x, float _y) : x(_x), y(_y) {}
	GUI_POINT_F(RECT r) : x(r.left), y(r.top) {}
	GUI_POINT_F(GUI_RECT r);

	/// Multiplies the points by the current screen magnification
	///
	inline GUI_POINT_F to_screenspace()
	{
		GUI_POINT_F ret;
		ret.x = x * (float)magnification;
		ret.y = y * (float)magnification;
		return ret;
	}

	/// Divides the points by the current screen magnification
	///
	inline GUI_POINT_F from_screenspace()
	{
		GUI_POINT_F ret;
		ret.x = x / (float)magnification;
		ret.y = y / (float)magnification;
		return ret;
	}

	/// Divides the points by the current screen magnification
	///
	inline GUI_POINT_F make_frame_relative()
	{
		GUI_POINT_F ret;
		ret.x = x + ((float)gFrame.x / 512.f);
		ret.y = y + ((float)gFrame.y / 512.f);
		return ret;
	}

	/// Divides the points by the current screen magnification
	///
	inline GUI_POINT_F reset_frame_relativity()
	{
		GUI_POINT_F ret;
		ret.x = x - ((float)gFrame.x / 512.f);
		ret.y = y - ((float)gFrame.y / 512.f);
		return ret;
	}

	inline GUI_POINT_F normalize()
	{
		float mag = sqrtf((x * x) + (y * y));
		GUI_POINT_F ret(x / mag, y / mag);
		return ret;
	}

	inline float angle_to(GUI_POINT_F other)
	{
		float dot = x * other.x + y * other.y;
		float det = x * other.y - y * other.x;
		return atan2f(det, dot);
	}

	inline float angle_to(GUI_POINT other)
	{
		float dot = x * (float)other.x + y * (float)other.y;
		float det = x * (float)other.y - y * (float)other.x;
		return atan2f(det, dot);
	}

	inline float distance(GUI_POINT_F other)
	{
		return sqrtf(powf(other.x - x, 2) + powf(other.y - y, 2));
	}

	inline GUI_POINT_F max_normal()
	{
		GUI_POINT_F ret(x < 0 ? -1 : (x == 0 ? 0 : 1), y < 0 ? -1 : (y == 0 ? 0 : 1));
		return ret;
	}

	static inline bool intersect(GUI_POINT_F a, GUI_POINT_F b, GUI_POINT_F c, GUI_POINT_F d, GUI_POINT_F* hit)
	{
		// Line AB represented as a1x + b1y = c1 
		float a1 = b.y - a.y;
		float b1 = a.x - b.x;
		float c1 = a1 * (a.x) + b1 * (a.y);

		// Line CD represented as a2x + b2y = c2 
		float a2 = d.y - c.y;
		float b2 = c.x - d.x;
		float c2 = a2 * (c.x) + b2 * (c.y);

		float determinant = a1 * b2 - a2 * b1;

		// Check to see if the determinant is valid
		if (determinant != 0.f)
		{
			hit->x = (b2 * c1 - b1 * c2) / determinant;
			hit->y = (a1 * c2 - a2 * c1) / determinant;

			return true;
		}
		
		return false;
	}

	inline bool operator==(const GUI_POINT_F& o)       { return (o.x == x && o.y == y); }
	inline bool operator!=(const GUI_POINT_F& o)       { return (o.x != x || o.y != y); }
	inline bool operator==(const GUI_POINT& o)         { return ((float)o.x == x && (float)o.y == y); }
	inline bool operator!=(const GUI_POINT& o)         { return ((float)o.x != x || (float)o.y != y); }
	inline void operator+=(const GUI_POINT_F& n)       { x += n.x; y += n.y; }
	inline void operator*=(const float n)              { x *= n; y *= n; }
	inline GUI_POINT_F operator+(const GUI_POINT_F& n) { GUI_POINT_F out; out.x = x + n.x; out.y = y + n.y; return out; }
	inline GUI_POINT_F operator-(const GUI_POINT_F& n) { GUI_POINT_F out; out.x = x - n.x; out.y = y - n.y; return out; }
	inline GUI_POINT_F operator*(const float n)        { return GUI_POINT_F(x * n, y * n); }
	inline GUI_POINT_F operator/(const float n)        { return GUI_POINT_F(x / n, y / n); }
	inline GUI_POINT_F operator+(const GUI_POINT& n)   { GUI_POINT_F out; out.x = x + (float)n.x; out.y = y + (float)n.y; return out; }
	inline GUI_POINT_F operator-(const GUI_POINT& n)   { GUI_POINT_F out; out.x = x - (float)n.x; out.y = y - (float)n.y; return out; }
};

inline bool GUI_POINT::operator==(const GUI_POINT_F& o) { return ((float)x == o.x && (float)y == o.y); }
inline bool GUI_POINT::operator!=(const GUI_POINT_F& o) { return ((float)x != o.x || (float)y != o.y); }
inline GUI_POINT_F GUI_POINT::operator+(const GUI_POINT_F& n) { GUI_POINT_F out; out.x = (float)x + n.x; out.y = (float)y + n.y; return out; }
inline GUI_POINT_F GUI_POINT::operator-(const GUI_POINT_F& n) { GUI_POINT_F out; out.x = (float)x - n.x; out.y = (float)y - n.y; return out; }
inline GUI_POINT_F GUI_POINT::operator*(const GUI_POINT_F& n) { GUI_POINT_F out; out.x = (float)x * n.x; out.y = (float)y * n.y; return out; }
inline GUI_POINT_F GUI_POINT::operator/(const GUI_POINT_F& n) { GUI_POINT_F out; out.x = (float)x / n.x; out.y = (float)y / n.y; return out; }

struct CAVESTORY_MOD_API GUI_RECT
{
	int x;
	int y;
	int w;
	int h;

	GUI_RECT()                               : x(0), y(0), w(0), h(0) {}
	GUI_RECT(NPCHAR* npc);
	GUI_RECT(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
	GUI_RECT(GUI_POINT one, int _w, int _h)  : x(one.x), y(one.y), w(_w), h(_h) {}
	GUI_RECT(int _x, int _y, GUI_POINT one)  : x(_x), y(_y), w(one.x), h(one.y) {}
	GUI_RECT(GUI_POINT one, GUI_POINT two)   : x(one.x), y(one.y), w(two.x), h(two.y) {}
	GUI_RECT(SDL_Rect r)                     : x(r.x), y(r.y), w(r.w), h(r.h) {}
	GUI_RECT(RECT r)                         : x(r.left), y(r.top), w(r.right - r.left), h(r.bottom - r.top) {}
	GUI_RECT(const GUI_RECT& r)              : x(r.x), y(r.y), w(r.w), h(r.h) {}
	GUI_RECT(GUI_RECT* r)                    : x(r->x), y(r->y), w(r->w), h(r->h) {}

	void from_npc(NPCHAR* npc);

	inline GUI_POINT get_center()
	{
		return GUI_POINT(
			x + (w / 2),
			y + (h / 2)
		);
	}

	inline bool operator==(GUI_RECT other)
	{
		return (
			x == other.x &&
			y == other.y &&
			w == other.w &&
			h == other.h
			);
	}
	
	inline bool operator==(const GUI_RECT other) const
	{
		return (
			x == other.x &&
			y == other.y &&
			w == other.w &&
			h == other.h
			);
	}
	
	inline bool operator!=(GUI_RECT other)
	{
		return (
			x != other.x ||
			y != other.y ||
			w != other.w ||
			h != other.h
			);
	}

	GUI_POINT   uncontain_point(GUI_POINT pnt);
	GUI_POINT_F uncontain_point(GUI_POINT_F pnt);
	GUI_POINT   contain_point(GUI_POINT pnt);
	GUI_POINT_F contain_point(GUI_POINT_F pnt);

	inline bool is_point_bordering(GUI_POINT p)
	{
		if (contain_point(p) != p)
			return false;

		return (p.x == x || p.y == y || p.x == x + w || p.y == y + h);
	}

	// Get the closest bording point
	inline GUI_POINT border(GUI_POINT rel)
	{
		GUI_POINT p(get_center());

		p.x = rel.x < p.x ? x : x + w;
		p.y = rel.y < p.y ? y : y + h;

		return p;
	}

	inline GUI_RECT to_screenspace()
	{
		GUI_RECT ret;
		ret.x = x * magnification;
		ret.y = y * magnification;
		ret.w = w * magnification;
		ret.h = h * magnification;
		return ret;
	}
	
	inline GUI_RECT to_custom(int mag)
	{
		GUI_RECT ret;
		ret.x = x * mag;
		ret.y = y * mag;
		ret.w = w * mag;
		ret.h = h * mag;
		return ret;
	}
	
	inline GUI_RECT from_custom(int mag)
	{
		GUI_RECT ret;
		ret.x = x / mag;
		ret.y = y / mag;
		ret.w = w / mag;
		ret.h = h / mag;
		return ret;
	}
	
	inline GUI_RECT from_screenspace()
	{
		GUI_RECT ret;
		ret.x = x / magnification;
		ret.y = y / magnification;
		ret.w = w / magnification;
		ret.h = h / magnification;
		return ret;
	}

	inline GUI_RECT to_game_rect()
	{
		GUI_RECT ret;
		ret.x = x;
		ret.y = y;
		ret.w = ret.x + w;
		ret.h = ret.y + h;
		return ret;
	}

	inline RECT to_rect()
	{
		RECT ret;
		ret.left   = x;
		ret.top    = y;
		ret.right  = x + w;
		ret.bottom = y + h;
		return ret;
	}

	inline SDL_Rect to_sdl_rect()
	{
		SDL_Rect ret = { x, y, w, h };
		return ret;
	}

	inline GUI_POINT position() { return GUI_POINT(x, y); }
	inline GUI_POINT extent()   { return GUI_POINT(w, h); }
	inline int left()           { return x; }
	inline int right()          { return x + w; }
	inline int top()            { return y; }
	inline int bottom()         { return y + h; }
	inline int width()          { return w; }
	inline int height()         { return h; }

	inline bool pointInRect(const GUI_POINT& pt) const
	{
		return (pt.x >= x && pt.x < x + w && pt.y >= y && pt.y < y + h);
	}

	inline bool contains(const GUI_RECT& R) const
	{
		if (x <= R.x && y <= R.y)
			if (R.x + R.w <= x + w)
				if (R.y + R.h <= y + h)
					return true;
		return false;
	}

	inline void contain(GUI_RECT& other)
	{
		int LEFT   = other.left();
		int RIGHT  = other.right();
		int TOP    = other.top();
		int BOTTOM = other.bottom();

		if (LEFT < x)
			LEFT = x;
		if (TOP < y)
			TOP = y;
		if (RIGHT > right())
			RIGHT = right();
		if (BOTTOM > bottom())
			BOTTOM = bottom();
		
		other.x = LEFT;
		other.y = TOP;
		other.w = (RIGHT - LEFT);
		other.h = (BOTTOM - TOP);
	}

	bool isValidRect() const
	{
		return (w > 0 && h > 0);
	}

	inline bool intersect(const GUI_RECT& clipRect)
	{
		GUI_POINT bottomL;
		bottomL.x = MIN(x + w - 1, clipRect.x + clipRect.w - 1);
		bottomL.y = MIN(y + h - 1, clipRect.y + clipRect.h - 1);

		x = MAX(x, clipRect.x);
		y = MAX(y, clipRect.y);

		w = bottomL.x - x + 1;
		h = bottomL.y - y + 1;

		return isValidRect();
	}

	inline bool overlaps(const GUI_RECT& R) const
	{
		GUI_RECT test = *this;
		return test.intersect(R);
	}

	static inline GUI_RECT FromRect(int left, int top, int right, int bottom)
	{
		RECT pRect;
		pRect.left   = left;
		pRect.top    = top;
		pRect.right  = right;
		pRect.bottom = bottom;
		return GUI_RECT(pRect);
	}

	inline int HashIndex()                      { return (x + w) + ((y + h) * 0x007FFFFF); }
	inline int HashIndex() const                { return (x + w) + ((y + h) * 0x007FFFFF); }
	bool operator<(GUI_RECT other)              { return HashIndex() < other.HashIndex();  }
	bool operator<=(GUI_RECT other)             { return HashIndex() <= other.HashIndex(); }
	bool operator>(GUI_RECT other)              { return HashIndex() > other.HashIndex();  }
	bool operator>=(GUI_RECT other)             { return HashIndex() >= other.HashIndex(); }
	bool operator<(const GUI_RECT other)  const { return HashIndex() < other.HashIndex();  }
	bool operator<=(const GUI_RECT other) const { return HashIndex() <= other.HashIndex(); }
	bool operator>(const GUI_RECT other)  const { return HashIndex() > other.HashIndex();  }
	bool operator>=(const GUI_RECT other) const { return HashIndex() >= other.HashIndex(); }
	void operator*=(float iMagnification)       { x *= iMagnification; y *= iMagnification; w *= iMagnification; h *= iMagnification; }
	void operator/=(float iMagnification)       { x /= iMagnification; y /= iMagnification; w /= iMagnification; h /= iMagnification; }
	void operator*=(GUI_RECT other)             { x *= other.x; y *= other.y; w *= other.w; h *= other.h; }
	void operator/=(GUI_RECT other)             { x /= other.x; y /= other.y; w /= other.w; h /= other.h; }
	GUI_RECT operator*(float fAmount)           { GUI_RECT a; a.x = x * fAmount; a.y = y * fAmount; a.w = w * fAmount; a.h = h * fAmount; return a; }
	GUI_RECT operator/(float fAmount)           { GUI_RECT a; a.x = x / fAmount; a.y = y / fAmount; a.w = w / fAmount; a.h = h / fAmount; return a; }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// Bullet object
///
struct BULLET
{
	/// Bullet flags
	///
	int flag;

	/// The ID for this bullet
	///
	int code_bullet;

	/// The ID of the weapon this bullet belongs to. Will be counted towards that weapon's number of bullets on-screen.
	///
	int code_arms;

	/// Bullet bit-flags
	///
	int bbits;
	
	/// This bullet's condition
	///
	int cond;

	/// X Position
	///
	int x;

	/// Y Position
	///
	int y;

	/// X Motion (x velocity)
	///
	int xm;

	/// Y Motion (y velocity)
	///
	int ym;

	/// Target X
	///
	int tgt_x;

	/// Target Y
	///
	int tgt_y;

	/// Act number
	///
	int act_no;

	/// Act wait counter
	///
	int act_wait;

	/// Animation wait counter
	///
	int ani_wait;

	/// Animation number
	///
	int ani_no;

	/// Direction
	///
	unsigned char direct;

	/// Sprite rect
	///
	RECT rect;

	/// Custom counter 1
	///
	int count1;

	/// Custom counter 2
	///
	int count2;

	/// Bullet range
	///
	int life_count;

	/// Bullet damage
	///
	int damage;

	/// Hits left before the bullet is deleted
	///
	int life;
	
	/// Enemy collision width
	///
	int enemyXL;

	/// Enemy collision height
	///
	int enemyYL;

	/// Wall collision width
	///
	int blockXL;

	/// Wall collision height
	///
	int blockYL;

	/// View rect
	///
	RECT view;

	/// The client's Ghost ID who this belongs to
	///
	int ghostId;

	/// Level of the bullet
	///
	int level;

	/// This bullet's Team ID.
	///
	int team_id;
};

/// Permit stage object
///
struct PERMIT_STAGE
{
	/// Index in gPermitStage
	int index;

	/// The event # to call
	int event;
};

/// Weapon object
///
struct ARMS
{
	/// ID of this weapon
	///
	int code;

	/// Level of the weapon
	///
	int level;

	/// Current experience points gathered
	///
	int exp;

	/// Maximum ammo
	///
	int max_num;

	/// Current ammo
	///
	int num;
};

/// Item object
///
struct ITEM
{
	/// Item ID
	///
	int code;
};

/// Stores particle information.
///
struct CARET
{
	/// The condition of the caret.
	///
	int cond;

	/// The caret's ID.
	///
	int code;

	/// The direction of the caret.
	///
	int direct;

	/// Subpixel X position
	///
	int x;

	/// Subpixel Y position
	///
	int y;

	/// Subpixel X velocity
	///
	int xm;

	/// Subpixel Y velocity
	///
	int ym;

	/// The act number
	///
	int act_no;

	/// The act timer
	///
	int act_wait;

	/// The current animation number
	///
	int ani_no;

	/// The current animation timer
	///
	int ani_wait;

	/// Subpixel width of the caret (half-size)
	///
	int view_left;

	/// Subpixel height of the caret (half-size)
	///
	int view_top;

	/// The rect to be drawn.
	///
	RECT rect;
};

/// Physics specifier for MYCHAR.
///
struct MYCHAR_PHYSICS
{
	/// Maximum dash amount
	///
	int max_dash;

	/// Maximum move speed
	///
	int max_move;

	/// Normal gravity applied
	///
	int gravity1;

	/// Gravity applied when holding the jumping key
	///
	int gravity2;

	/// Acceleration (1)
	///
	int dash1;

	/// Acceleration (2)
	///
	int dash2;

	/// Air resistance
	///
	int resist;

	/// Jump force
	///
	int jump;
};

/// Custom object structure.
///
struct CUSTOM_OBJECT
{
	/// Hitbox
	///
	RECT hit;

	/// X Position
	///
	int x;

	/// Y Position
	///
	int y;

	/// X Motion
	///
	int xm;

	/// Y Motion
	///
	int ym;

	/// Target X
	///
	int tgt_x;
	
	/// Target Y
	///
	int tgt_y;

	/// Collision flags
	///
	int flag;
};

/// Animation information for CustomPlayerAnimation.
///
struct CUSTOM_PLAYER_ANIMATION_INFO
{
	/// The current animation type.
	///
	int current_anim_type;

	/// This player's current animation frame.
	///
	int curr_anim;

	/// The expire point of the current delay that must be passed in order for frame_no to continue ticking.
	///
	unsigned int delay_expire;

	/// Current frame number.
	///
	unsigned int frame_no;

	/// Current animation frame.
	///
	void* current_frame_ptr;

	/// Whether or not this is the main character.
	///
	bool is_my_player;

	/// The number of animation cycles that have completed since this animation started playing.
	///
	int cycle_count;

	/// The last time that the player's animation changed.
	///
	unsigned int change_time;
};

/// Player character object
///
struct MYCHAR
{
	/// Custom player character
	///
	CustomPlayer* character;

	/// Condition
	///
	unsigned short cond;

	/// Flags
	///
	unsigned int flag;

	/// Next frame flags
	///
	unsigned int next_flag;

	/// Direction
	///
	/// @see #Direction
	int direct;

	/// NPC Control
	///
	int npc_control;

	/// Is facing up?
	///
	int up;

	/// Is facing down?
	///
	int down;

	/// Animation index
	///
	int netanim;

	/// Current equip flags
	///
	int equip;

	/// X Position
	///
	int x;

	/// Y Position
	///
	int y;

	/// X Target Position
	///
	int tgt_x;

	/// Y Target Position
	///
	int tgt_y;

	/// Draw offset
	///
	int draw_offset_x;
	
	/// Draw offset
	///
	int draw_offset_y;

	/// Index X
	///
	int index_x;

	/// Index Y
	///
	int index_y;

	/// X Motion
	///
	int xm;

	/// Y Motion
	///
	int ym;

	/// Old X motion (before HitMyCharMap was called)
	///
	int old_xm;

	/// Old Y motion (before HitMyCharMap was called)
	///
	int old_ym;

	/// Animation wait counter
	///
	int ani_wait;

	/// Initialize state
	///
	bool init_state;

	/// Free state
	///
	bool free_state;

	/// Animation number
	///
	int ani_no;

	/// Act number
	///
	int act_no;

	/// Act wait
	///
	int act_wait;

	/// Count1
	///
	int count1;

	/// Count2
	///
	int count2;

	/// Count3
	///
	int count3;

	/// Count4
	///
	int count4;

	/// Hit rect
	///
	RECT hit;

	/// View rect
	///
	RECT view;

	/// Sprite  rect
	///
	RECT rect;

	/// Arms sprite rect
	///
	RECT rect_arms;

	/// HUD stuff
	///
	int level;

	/// HUD stuff
	///
	int exp_wait;

	/// HUD stuff
	///
	int exp_count;

	/// Shock
	///
	/// Set when the player is damaged, decreases with every frame.
	///
	/// Used for invincible frames
	///
	unsigned char shock;

	/// Flash shock
	///
	/// If set to true, it will be set to 'false' when 'shock' equals 0.
	bool dont_flash_shock;

	/// ?
	///
	unsigned char no_life;

	/// Weapon rensha
	///
	unsigned char rensha;

	/// Bubble
	///
	unsigned char bubble;

	/// Current health
	///
	short life;

	/// How many stars we have revolving around us
	///
	/// Max is 3
	///
	short star;

	/// Max health
	///
	short max_life;

	/// a
	///
	short a;

	/// HUD stuff
	///
	int lifeBr;

	/// HUD stuff
	///
	int lifeBr_count;

	/// How much air we have. Maximum value is 1000
	///
	int air;

	/// How much air we're getting.
	///
	int air_get;

	/// HUD stuff
	///
	signed char sprash;

	/// HUD stuff
	///
	signed char ques;

	/// HUD stuff
	///
	signed char boost_sw;

	/// Booster amount left
	///
	int boost_cnt;

	/// Booster max amount
	///
	int boost_max;

	/// Red value for the color of their character
	///
	unsigned char col_r;

	/// Green value for the color of their character
	///
	unsigned char col_g;

	/// Blue value for the color of their character
	///
	unsigned char col_b;

	/// Whether to use the custom color or not
	///
	bool use_char_color;

	/// Which zone this character is in
	///
	int zone;

	/// Normal physics
	///
	MYCHAR_PHYSICS physics_normal;

	/// Underwater physics
	///
	MYCHAR_PHYSICS physics_underwater;
	
	/// If set to 'true', then this player cannot drown.
	///
	bool ignore_water;

	/// Built xm.
	///
	int built_xm;

	/// Built ym.
	///
	int built_ym;

	/// Pointer NPC.
	///
	NPCHAR* pNpc;

	/// Other player
	///
	CaveNet::DataStructures::NetClient* pOtherClient;

	/// Other player
	///
	MYCHAR* pOther;

	/// The animation info for this player.
	///
	CUSTOM_PLAYER_ANIMATION_INFO anim_info;

	/// Custom object
	///
	CUSTOM_OBJECT obj;
};

/// SafeClientInterface
///
struct CAVESTORY_MOD_API SafeClientInterface
{
	/// The client we're interfacing with. This will always be the server-sided client, UNLESS we aren't hosting the server.
	///
	CaveNet::DataStructures::NetClient* mClient;

	/// Whether this interface is client-sided or not.
	///
	bool mClientSide;

	/// The ghost ID of this user.
	///
	int mGhostId;

	/// Whether this is our user.
	///
	bool mIsOurUser;

	/// A pointer to this client's selected arms.
	///
	int* mSelectedArms;

	/// This client's arms table.
	///
	ARMS* mArms;

	/// This client's items table.
	///
	ITEM* mItems;

	/// This client's player. Can be NULL.
	///
	MYCHAR* mMC;

	/// This client's gameKey variable.
	///
	int* mKey;

	/// This client's gameKeyTrg variable.
	///
	int* mKeyTrg;
	
	/// This client's gameKeyOld variable.
	///
	int* mKeyOld;

	/// This client's spur charge.
	///
	int* mSpurCharge;

	/// Whether this client's spur charge is at maximum.
	///
	bool* mSpurMax;

	/// This client's game flags.
	///
	int* mGameFlags;

	/// Permitted stages list.
	///
	PERMIT_STAGE* mPermitStages;

	/// constructor
	///
	SafeClientInterface()
	{
		memset(this, 0, sizeof(SafeClientInterface));
	}

	/// Constructor 2
	///
	SafeClientInterface(CaveNet::DataStructures::NetClient* pClient, unsigned int iSetType = SCI_SetType::SCI_ST_AUTO)
	{
		memset(this, 0, sizeof(SafeClientInterface));
		Set(pClient, iSetType);
	}

	/// @brief Interface with a client.
	///
	/// @param pClient The client to interface with. Can be NULL to interface with the local player.
	/// @param iSetType The flags used to determine the best candidate for this interface.
	///
	/// @return Returns true on success, otherwise returns false if it couldn't find a suitable client.
	/// @see SCI_SetType
	bool Set(CaveNet::DataStructures::NetClient* pClient, unsigned int iSetType = SCI_SetType::SCI_ST_AUTO);

	/// @brief Damage this client's player.
	///
	/// @param iDamage The amount of damage to do.
	///
	/// @note This function handles death.
	void DamagePlayer(int iDamage);

	/// @brief Add experience to the currently selected weapon.
	///
	/// @param iAmount The amount of experience 
	/// @param bSpur Should be set to 'true' if a weapon similar to Spur is using this.
	void AddExp(int iAmount, bool bSpur = false);
	
	/// @brief Check to see if we're at max EXP for the held weapon.
	///
	/// @return Returns true if the client is at max EXP for the selected weapon.
	bool IsMaxExp();

	/// @brief Set the held weapon's exp to 0 and level to 1.
	///
	void ZeroExp();

	/// @brief Get the number of active bullets w/ the given Weapon ID that this client has shot.
	///
	/// @param iArmsCode The weapon ID the bullets should be tied to.
	///
	/// @return Returns the number of bullets matching the parameters.
	int CountArmsBullet(int iArmsCode);
	
	/// @brief Get the number of active bullets w/ the given Bullet ID that this client has shot.
	///
	/// @param iBulletCode The bullet ID the bullets should have.
	///
	/// @return Returns the number of bullets matching the parameters.
	int CountBulletNum(int iBulletCode);

	/// @brief Add ammo to the held weapon.
	///
	/// @param iAmmoAmt The amount of ammo to add.
	///
	/// @return Returns true on success.
	bool ChangeArmsEnergy(int iAmmoAmt);

	/// @brief Use ammo from the held weapon.
	///
	/// @param iAmount The amount of ammo to check for & decrement if available.
	///
	/// @return Returns true if there was enough ammo, false if not.
	bool UseArmsEnergy(int iAmount);

	/// @brief Test ammo amount for the given weapon.
	///
	/// @param iWeaponId The weapon ID.
	/// @param iAmount The amount to check for.
	///
	/// @return Returns true if the weapon has exactly (or more than) iAmount.
	bool HasArmsEnergy(int iWeaponId, int iAmount);

	/// @brief Get the amount of ammo a wepaon has.
	///
	/// @param iWeaponId The weapon ID.
	///
	/// @return Returns the amount of ammo the weapon has, if we have it. If we don't have the specified weapon, then this returns -1.
	int GetArmsEnergy(int iWeaponId);
	
	/// @brief Get the maximum amount of ammo a wepaon can have.
	///
	/// @param iWeaponId The weapon ID.
	///
	/// @return Returns the maximum amount of ammo the weapon can have, if we have it. If we don't have the specified weapon, then this returns -1.
	int GetArmsMaxEnergy(int iWeaponId);
	
	/// @brief Get the amount of EXP a weapon has.
	///
	/// @param iWeaponId The weapon ID.
	/// @param iMaxExp A pointer to an int, to hold the amount of EXP needed to get to the next level. This can be NULL.
	///
	/// @return Returns the amount of EXP the specified weapon has, unless we don't have it, in which this returns -1.
	int GetArmsExp(int iWeaponId, int* iMaxExp = NULL);

	/// @brief Change to this client's first weapon.
	///
	void ChangeToFirstArms();

	/// @brief Rotate arms to next weapon.
	///
	void NextWeapon();

	/// @brief Rotate arms to previous weapon.
	///
	void PreviousWeapon();

	/// @brief Test input flags against this client's Key
	///
	/// @param iFlags The flags to test.
	/// @param iInputType The input type to test.
	///
	/// @return Returns the result of testing the given flags against the input.
	///
	/// @see KEYBIND
	/// @see KEYBIND2
	/// @see KEYBIND_JOY
	unsigned long long int TestInput(unsigned long long int iFlags, CSM_KEY_DESC iInputType = CSM_KEY_DESC::CSM_KEY_DESC_KEY);
};

/// ShootInfo structure
///
struct ShootInfo
{
	/// The client tied to this shoot event. Can be NULL!
	///
	CaveNet::DataStructures::NetClient* client;

	/// The character tied to this shoot event
	///
	MYCHAR* ourC;

	/// The key input state
	///
	int key;

	/// The key-Trg input state
	///
	int keyTrg;

	/// The shooter's current weapon list
	///
	ARMS* arms;

	/// The shooter's selected weapon index
	///
	int selectedArm;

	/// The shooter's Ghost ID
	///
	int ghostId;

	/// Whether this client is our user
	///
	bool IsOurClient;

	/// Which weapon ID was shot
	///
	int arms_code;

	/// The level at which the weapon was shot at
	///
	int arms_level;

	/// Ease-of-use: List of bullet IDs defined in CaveEditor for each weapon level
	///
	int bullet_ids[3];

	/// A safe interface for the client who shot the gun.
	///
	SafeClientInterface sClient;
};

/// NPC object
///
struct NPCHAR
{
	/// A timestamp of when this NPC was last updated
	///
	unsigned int last_update_time;

	/// Node flags
	///
	unsigned long long int nodes;

	/// Transmit node flags
	///
	unsigned long long int transmit_nodes;

	/// NPC Condition flags
	///
	unsigned char cond;

	/// Internal flags
	///
	int flag;

	/// X Position
	///
	int x;

	/// Y Position
	///
	int y;

	/// X Motion
	///
	int xm;

	/// Y Motion
	///
	int ym;

	/// X Motion 2
	///
	int xm2;

	/// Y Motion 2
	///
	int ym2;

	/// Target X
	///
	int tgt_x;

	/// Target Y
	///
	int tgt_y;

	/// ID of this NPC
	///
	int code_char;

	/// NPC Flag ID
	/// Details which NPC flag to test when first spawning this NPC
	int code_flag;

	/// The event this NPC should call
	///
	int code_event;

	/// Surface ID -- Reserved for CaveEditor -- Do not change unless you know what you're doing
	///
	Surface_Ids surf;

	/// Hit voice -- Reserved for CaveEditor -- Do not change unless you know what you're doing
	///
	int hit_voice;

	/// Destroy voice -- Reserved for CaveEditor -- Do not change unless you know what you're doing
	///
	int destroy_voice;

	/// NPC Health
	///
	int life;

	/// How much experience points this NPC drops on death
	///
	int exp;

	/// NPC Scale
	///
	int size;

	/// NPC Direction
	///
	int direct;

	/// Map flags for this NPC
	///
	unsigned int bits;

	/// Sprite rect
	///
	RECT rect;

	/// Animation wait counter
	///
	int ani_wait;

	/// Animation number
	///
	int ani_no;

	/// Custom counter 1
	///
	int count1;

	/// Custom counter 2
	///
	int count2;

	/// Custom counter 3
	///
	int count3;

	/// Custom counter 4
	///
	int count4;

	/// Act number
	///
	int act_no;

	/// Act wait counter
	///
	int act_wait;

	/// Hit rect -- Reserved for CaveEditor -- Do not change unless you know what you're doing
	///
	RECT hit;

	/// View rect -- Reserved for CaveEditor -- Do not change unless you know what you're doing
	///
	RECT view;

	/// Shock
	/// Set when this NPC gets hurt, and decrements to 0 every frame
	unsigned char shock;

	/// Damage view
	///
	int damage_view;

	/// How much damage this NPC deals to the player
	///
	int damage;

	/// NPC Pointer
	/// If you use this, you should specify "pNpc" in both Node flags & Transmit node flags in CaveEditor
	///
	NPCHAR* pNpc;

	/// Custom draw function. If NULL, then it will draw the NPC normally.
	///
	PutNPCFunc putFunc;

	/// Custom free function. If NULL, then this will not be called.
	///
	FreeNPCFunc freeFunc;

	/// Custom collide function. If NULL, then the normal collision detection will ALWAYS run.
	///
	CollideNPCFunc collideFunc;

	/// Should call free function. Do not mess with this variable unless you know what you're doing.
	///
	bool should_call_free;

	/// cache -- do not use
	///
	int old_x;

	/// cache -- do not use
	///
	int old_y;

	/// cache -- do not use
	///
	int old_ani_no;

	/// Which zone this NPC is currently in -- Do not change this variable
	///
	int zone;

	/// Client pointer -- Do not change this variable
	///
	CaveNet::DataStructures::NetClient* client;

	/// Client pointer -- Do not change this variable
	///
	CaveNet::DataStructures::NetClient* client2;

	/// Display focus -- Do not change this variable
	///
	MYCHAR* dfocus;

	/// A pointer to the character that this NPC is focusing.
	///
	/// @note If you are using this variable, please run down this checklist to ensure validity in multiplayer:
	///
	///         1. In CaveEditor's NPC Editor, select the NPC that you want to focus somebody
	///         2. In the 'Flags' tab, enable the 'Focus Closest Players' checkmark
	///         3. In the 'Nodes' tab, enable the 'focus' checkmark
	///         4. In the 'Transmit Nodes' tab, enable the 'focus' checkmark
	///
	MYCHAR* focus;

	/// Focus 2 -- Do not change this variable
	///
	MYCHAR* focus2;

	/// Counter for updating focus -- Do not change this variable
	///
	int updateTicks;

	/// Whether this NPC is custom sized.
	///
	bool sized;

	/// Scalar for this NPC.
	///
	int scalar;

	/// Custom user data
	///
	void* data;

	/// The initial index of this NPC. This controls entity properties; don't mess with this.
	///
	int entity_init_index;

	/// The client ID of the client we're dependent on.
	///
	CaveNet::DataStructures::NetClient* dependent;
	
	/// Will only damage this specific client if not set to NULL.
	///
	CaveNet::DataStructures::NetClient* damage_target;

	/// @brief Get this NPC's hash.
	///
	/// @param bForce Force a hash, even if this NPC is not considered as 'active' (cond & 0x80).
	///
	/// @return The hash for this NPC.
	CAVESTORY_MOD_API unsigned int GetHash(bool bForce = false);

	/// @brief Get this NPC's draw position.
	///
	/// @return Returns the top left corner of the NPC in screen coordinates.
	CAVESTORY_MOD_API GUI_POINT GetDrawPosition(int fx = -1, int fy = -1);
};

/// Whimsical star object
///
struct WHIMSICAL_STAR
{
	int cond;
	int x;
	int y;
	int xm;
	int ym;

	/// The animation information for this whimsical star.
	///
	CUSTOM_PLAYER_ANIMATION_INFO anim_info;
};

#define _NPCHAR_CACHE_SET(varName) this->varName = npc->varName;
#define _NPCHAR_CACHE_TRANSMIT(varName) npc->varName = this->varName;
#define _NPCHAR_CACHE_EQUALS(varName) npc->varName == this->varName

struct _NPCHAR_CACHE
{
	unsigned char cond;
	int x;
	int y;
	int xm;
	int ym;
	unsigned short code_char;
	unsigned short act_no;
	unsigned short act_wait;
	unsigned short ani_no;
	unsigned short ani_wait;
	unsigned short life;
	unsigned char shock;
	char direct;
	int count1;
	int count2;
	int count3;
	int count4;
	NPCHAR* pNpc;
	MYCHAR* focus;

	inline void SetNpc(NPCHAR* npc)
	{
		_NPCHAR_CACHE_TRANSMIT(cond);
		_NPCHAR_CACHE_TRANSMIT(x);
		_NPCHAR_CACHE_TRANSMIT(y);
		_NPCHAR_CACHE_TRANSMIT(xm);
		_NPCHAR_CACHE_TRANSMIT(ym);
		_NPCHAR_CACHE_TRANSMIT(code_char);
		_NPCHAR_CACHE_TRANSMIT(act_no);
		_NPCHAR_CACHE_TRANSMIT(act_wait);
		_NPCHAR_CACHE_TRANSMIT(ani_no);
		_NPCHAR_CACHE_TRANSMIT(ani_wait);
		_NPCHAR_CACHE_TRANSMIT(pNpc);
		_NPCHAR_CACHE_TRANSMIT(focus);
		_NPCHAR_CACHE_TRANSMIT(shock);
		_NPCHAR_CACHE_TRANSMIT(life);
		_NPCHAR_CACHE_TRANSMIT(direct);
		_NPCHAR_CACHE_TRANSMIT(count1);
		_NPCHAR_CACHE_TRANSMIT(count2);
		_NPCHAR_CACHE_TRANSMIT(count3);
		_NPCHAR_CACHE_TRANSMIT(count4);
	}

	inline _NPCHAR_CACHE* operator=(NPCHAR* npc)
	{
		_NPCHAR_CACHE_SET(cond);
		_NPCHAR_CACHE_SET(x);
		_NPCHAR_CACHE_SET(y);
		_NPCHAR_CACHE_SET(xm);
		_NPCHAR_CACHE_SET(ym);
		_NPCHAR_CACHE_SET(code_char);
		_NPCHAR_CACHE_SET(act_no);
		_NPCHAR_CACHE_SET(act_wait);
		_NPCHAR_CACHE_SET(ani_no);
		_NPCHAR_CACHE_SET(ani_wait);
		_NPCHAR_CACHE_SET(pNpc);
		_NPCHAR_CACHE_SET(focus);
		_NPCHAR_CACHE_SET(shock);
		_NPCHAR_CACHE_SET(life);
		_NPCHAR_CACHE_SET(direct);
		_NPCHAR_CACHE_SET(count1);
		_NPCHAR_CACHE_SET(count2);
		_NPCHAR_CACHE_SET(count3);
		_NPCHAR_CACHE_SET(count4);

		return this;
	}

	inline bool operator==(NPCHAR* npc)
	{
		return (
			_NPCHAR_CACHE_EQUALS(cond) &&
			_NPCHAR_CACHE_EQUALS(x) &&
			_NPCHAR_CACHE_EQUALS(y) &&
			_NPCHAR_CACHE_EQUALS(xm) &&
			_NPCHAR_CACHE_EQUALS(ym) &&
			_NPCHAR_CACHE_EQUALS(code_char) &&
			_NPCHAR_CACHE_EQUALS(act_no) &&
			_NPCHAR_CACHE_EQUALS(act_wait) &&
			_NPCHAR_CACHE_EQUALS(ani_no) &&
			_NPCHAR_CACHE_EQUALS(ani_wait) &&
			_NPCHAR_CACHE_EQUALS(pNpc) &&
			_NPCHAR_CACHE_EQUALS(focus) &&
			_NPCHAR_CACHE_EQUALS(shock) &&
			_NPCHAR_CACHE_EQUALS(life) &&
			_NPCHAR_CACHE_EQUALS(direct) &&
			_NPCHAR_CACHE_EQUALS(count1) &&
			_NPCHAR_CACHE_EQUALS(count2) &&
			_NPCHAR_CACHE_EQUALS(count3) &&
			_NPCHAR_CACHE_EQUALS(count4)
			);
	}

	inline bool operator!=(NPCHAR* npc) { return !this->operator==(npc); }
};
#undef _NPCHAR_CACHE_SET
#undef _NPCHAR_CACHE_EQUALS
#undef _NPCHAR_CACHE_TRANSMIT

//------------------------------------------------------------------------------------------------------------

/// Stores information for save files
///
struct PROFILE
{
	/// Magic code
	///
	char code[8];

	/// Stage index
	///
	int stage;

	/// Music index
	///
	int music;

	/// Player X position
	///
	int x;

	/// Player Y position
	///
	int y;

	/// Player direction
	///
	int direct;

	/// Player max life
	///
	short max_life;

	/// Player stars
	///
	short star;

	/// Player life
	///
	short life;

	/// Player A
	///
	short a;

	/// Select arms
	///
	int select_arms;

	/// Selected item
	///
	int select_item;

	/// Equip flags
	///
	int equip;

	/// Universe number
	///
	int unit;

	/// Nikamaru Counter
	///
	int counter;

	/// Arms table
	///
	ARMS arms[8];

	/// Item table
	///
	ITEM items[32];

	/// Permit stage table
	///
	PERMIT_STAGE permitstage[8];

	/// Permit mapping
	///
	signed char permit_mapping[0x80];

	/// FLAG
	///
	char FLAG[4];

	/// NPC & skip flags
	///
	unsigned char flags[1000];

	/// Unused
	///
	int unused1;

	/// Unused
	///
	int unused2;

	/// Preferred character name
	///
	char CharName[256];

	/// How many frames have been processed and rendered since the start of this playthrough
	///
	long long int frame_count;

	/// Collectables
	///
	int collectables;

	/// Maximum booster fuel
	///
	int booster_max;

	/// Collectables shown
	///
	bool collectables_shown;

	/// Extra permit stages
	///
	PERMIT_STAGE permitstage2[16];
};

//------------------------------------------------------------------------------------------------------------

/// Text script pointer data
///
/// This is a 'context' structure, pointing to various details about the user who is currently executing a text script event.
struct TEXT_SCRIPT_PTR_DATA
{
	/// Their character
	///
	MYCHAR* Char;

	/// Their game flags
	///
	int* GameFlags;

	/// Their input flags
	///
	int* Key;

	/// Their trg input flags
	///
	int* KeyTrg;

	/// Their client
	///
	CaveNet::DataStructures::NetClient* Client;

	/// Their player
	///
	CaveNet::DataStructures::NetPlayer* Player;
};

/// Text script state object
///
struct TEXT_SCRIPT
{
	/// Path (reload when exit teleporter menu/inventory)
	///
	char path[PATH_LENGTH];

	/// Script buffer size
	///
	int size;

	/// Script buffer size
	///
	char* data;

	/// Mode (ex. NOD, WAI)
	///
	signed char mode;

	/// 
	TextScriptTarget target;

	/// Flags
	///
	signed char flags;

	/// Current positions (read position in buffer, x position in line)
	///
	unsigned int p_read;

	/// Current write position in text
	///
	int p_write;

	/// When in instant text display mode, this stores how far along the pen's X offset is.
	///
	int p_tur_x;

	/// Current line to write to
	///
	int line;

	/// Line y positions
	///
	int ypos_line[4];

	/// Event wait
	///
	int wait;

	/// Wait Next
	///
	int wait_next;

	/// Next event
	///
	int next_event;

	/// Wait NPC
	///
	int wait_npc;

	/// Wait NPC act
	///
	int wait_npc_act;

	/// Yes/no selected
	///
	signed char select;

	/// Current face
	///
	int face;

	/// Face X offset
	///
	int face_x;

	/// Current item
	///
	int item;

	/// Item Y offset
	///
	int item_y;

	/// Text rect
	///
	RECT rcText;

	/// ..?
	///
	int offsetY;

	/// Whether to focus on the player who has context control
	///
	bool focus_on_plr;

	/// Lock mode
	///
	/// 0 = None\n1 = <KEY\n2 = <PRI
	char lock_mode;

	/// The event that is currently being ran
	///
	int current_event;

	/// Whether to transmit this to everyone on the server.
	///
	bool transmit;

	/// Focusing: gNPC Index
	///
	int focus_npc;

	/// Focusing: Camera speed
	///
	int focus_wait;

	/// Focusing: Old focus X ptr
	///
	int* old_focus_x;

	/// Focusing: Old focus Y ptr
	///
	int* old_focus_y;

	/// NOD cursor blink
	///
	unsigned char wait_beam;

	/// The NPC that was used to start the event.
	///
	NPCHAR* source_npc;
};

//------------------------------------------------------------------------------------------------------------

/// @addtogroup GUI
/// @{
///

/// @brief The base class for GUI elements.
///
class CAVESTORY_MOD_API GuiBase
{
public: // Enum / data
	/// Return flags for #GuiBase::process and #GuiBase::processKeys
	///
	enum ProcessReturnType
	{
		/// This GUI element is dirty and should be updated
		PRT_DIRTY                 = 0b000001,
		/// Stop processing mouse input for this frame
		PRT_IGNORE_MOUSE          = 0b000010,
		/// Stop processing key input for this frame
		PRT_IGNORE_KEYS           = 0b000100,
		/// This GUI element was selected
		PRT_SELECTED              = 0b001000,
		/// This is telling the mode to set m_ModeConfig.ProcessMouseOnMove to 'false' so it can process the next frame.
		PRT_DISABLE_MOUSE_ON_MOVE = 0b010000,
		/// This GUI element was changed
		PRT_CHANGED               = 0b100000
	};

	/// Hover callback
	///
	typedef void(*OnHoverChangeCallbackFunc)(GuiBase*, int value);

	/// Value change callback
	///
	typedef void(*OnValueChangeCallbackFunc)(GuiBase*, const char*& newValue);

public: // Cache

	/// Called in specific GuiBase children classes to indicate that a value has been changed.
	///
	OnValueChangeCallbackFunc OnValueChange;

	/// The display rect this GUI element will use.
	///
	GUI_RECT rect;

	/// Custom user data
	///
	void* userData;

public: // Configuration

	/// Called in specific GuiBase children classes to indicate that the GUI element is being hovered over.
	///
	OnHoverChangeCallbackFunc OnHoverChange;

	/// Has special mouse processing
	///
	bool has_special_mouse_processing;

	/// The special hotspot rect.
	///
	GUI_RECT hotspot_rect;

	/// Has special hotspot rect; If true, 'process' will be called everytime the mouse is inside of 'hotspot_rect' instead of 'rect'.
	///
	bool has_special_hotspot_rect;

public: // Basic constructor

	/// Default constructor
	///
	GuiBase()
	{
		rect          = GUI_RECT(0, 0, 0, 0);
		hotspot_rect  = GUI_RECT(0, 0, 0, 0);
		OnHoverChange = nullptr;
		OnValueChange = nullptr;

		// Ready the config
		has_special_mouse_processing = false;
		has_special_hotspot_rect     = false;
	}

public: // Virtual sets & gets

	/// @brief Set this GUI element as selected or not.
	/// @pure
	///
	/// @param value The selection value.
	virtual void setSelected(int value) = 0;

	/// @brief Set this GUI element as hovered over or not.
	/// @pure
	///
	/// @param value The hover value.
	virtual void setHovered(int value)  = 0;

	/// @brief Check selection status.
	/// @pure
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected()			= 0;

	/// @brief Check hover status.
	/// @pure
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered()			= 0;

	/// @brief Check hoverability status.
	/// @pure
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable()          = 0;

	/// @brief Get the selectable number of elements in this element.
	/// @pure
	///
	/// @return Returns the amount of selectable elements in this element.
	virtual int getSelectableCount()    { return 0; }

	/// @brief Get the number of elements that can be hovered over in this element.
	///
	/// @return Returns the amount of elements that can be hovered over in this element.
	virtual int getHoverableCount()     { return 0; }

	/// @brief Get a hoverable sub-element.
	///
	/// @param index The sub-element's index.
	///
	/// @return Returns a sub-element, NULL if it couldn't be resolved.
	virtual GuiBase* getHoverable(int index)  { return (!index ? this : NULL); }

	/// @brief Get a selectable sub-element.
	///
	/// @param index The sub-element's index.
	///
	/// @return Returns a sub-element, NULL if it couldn't be resolved.
	virtual GuiBase* getSelectable(int index) { return (!index ? this : NULL); }

	/// @brief Select on mouse up
	///
	/// @return Return false if you want this element to be selected on mousedown instead of mouseup
	virtual bool SelectOnMouseUp() { return true; }

public: // Virtual methods

	/// @brief Set the font for this GUI element.
	///
	/// @param pFont The font object to use.
	virtual void setFont(FontObject* pFont) {}

	/// @brief Get the font for this GUI element.
	///
	/// @return Returns the font object for this element.
	virtual FontObject* getFont() { return NULL; }

	/// @brief Process mouse input for this element
	/// @pure
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse) = 0;

	/// @brief Re-calculate this element's data
	/// @pure
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate()             = 0;

	/// @brief Render this element.
	/// @pure
	///
	virtual void render()                = 0;

	/// @brief Process key input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int processKeys() { return 0; }

	/// @brief Render the important bits of this element.
	///
	/// @note This function is used to draw things that should be at the front of the screen, so make sure to call it AFTER rendering everything else.
	virtual void importantRender() {}
};

//------------------------------------------------------------------------------------------------------------

/// Draw centered text
///
class CAVESTORY_MOD_API CenteredTextInfo : public GuiBase
{
protected: // Cache variables

	/// Text color RGB
	/// @protected
	///
	unsigned int mTextColor_RGB;

	/// Font for this element
	/// @protected
	///
	FontObject* mFont;

	/// Text point for this element
	/// @protected
	///
	GUI_POINT mTextPoint;

	/// Minimum size for this point
	/// @protected
	///
	GUI_POINT mMinimumSize;

	/// The text that this element should display
	/// @protected
	///
	char mText[256];

public: // Configuration

	/// The text color
	///
	GUI_COLOR mTextColor;

	/// A list of pointers to the new lines in mText.
	///
	/// @warning Do not directly modify this variable.
	char* mNewLines[8];
	
	/// The list size of mNewLines.
	///
	/// @warning Do not directly modify this variable.
	int mNewLineCount;

public:

	/// @brief Default constructor
	///
	CenteredTextInfo();

public:

	/// @brief Set this GUI element as selected or not.
	///
	/// @param value The selection value.
	virtual void setSelected(int value);

	/// @brief Set this GUI element as hovered over or not.
	///
	/// @param value The hover value.
	virtual void setHovered(int value);

	/// @brief Check selection status.
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected();

	/// @brief Check hover status.
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered();

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

public:
	/// @brief Set this element's font
	///
	/// @param pFont The new font.
	virtual void setFont(FontObject* pFont) { mFont = pFont; }

	/// @brief Get this element's font.
	///
	/// @return Returns the font.
	inline FontObject* getFont() { return mFont; }

public:
	/// @brief Get new line count
	///
	/// @return Returns new line count
	int getLineCount();

	/// @brief Check line length.
	///
	/// @param index The index of the line of text
	///
	/// @return Returns the length of a line of text
	int getLineLength(int index);

	/// @brief Fetch line text.
	///
	/// @param index The index of the line of text
	/// @param buffer The buffer that will hold the line.
	/// @param buffer_len Size of buffer
	void getLineText(int index, char* buffer, int buffer_len);

	/// @brief Fetch line graphical size
	///
	/// @param index The index of the line of text
	///
	/// @return Returns calculated size of the text using this element's font.
	GUI_POINT getLineSize(int index);

	/// @brief Set the text of this element.
	///
	/// @param text The text to set.
	void setText(const char* text);

	/// @brief Get this object's text.
	///
	/// @return Returns this element's text.
	inline const char* getText() { return mText; }

public:
	/// @brief Process mouse input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse);

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();
};

//------------------------------------------------------------------------------------------------------------

/// Text
///
class CAVESTORY_MOD_API LabelInfo : public GuiBase
{
protected: // Cache variables

	/// Text color RGB
	/// @protected
	///
	unsigned int mTextColor_RGB;

	/// Font for this element
	/// @protected
	///
	FontObject* mFont;

	/// The text that this element should display
	/// @protected
	///
	char mText[256];

public: // Configuration

	/// The text color
	///
	GUI_COLOR mTextColor;

public:

	/// @brief Default constructor
	///
	LabelInfo();

public:

	/// @brief Set this GUI element as selected or not.
	///
	/// @param value The selection value.
	virtual void setSelected(int value);

	/// @brief Set this GUI element as hovered over or not.
	///
	/// @param value The hover value.
	virtual void setHovered(int value);

	/// @brief Check selection status.
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected();

	/// @brief Check hover status.
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered();

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

public:
	/// @brief Set this element's font
	///
	/// @param pFont The new font.
	virtual void setFont(FontObject* pFont) { mFont = pFont; }

public:
	/// @brief Set the text of this element.
	///
	/// @param text The text to set.
	void setText(const char* text);

	/// @brief Get the text of this element.
	///
	/// @return Returns this element's text.
	inline const char* getText() { return mText;}

public:
	/// @brief Process mouse input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse);

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();
};

//------------------------------------------------------------------------------------------------------------

/// A scroll area
///
class CAVESTORY_MOD_API ScrollAreaInfo : public GuiBase
{
private:
	/// @private
	///
	static GUI_RECT srcScrollBarX_BG[3];

	/// @private
	///
	static GUI_RECT srcScrollBarX_LeftBtn[2];

	/// @private
	///
	static GUI_RECT srcScrollBarX_RightBtn[2];

	/// @private
	///
	static GUI_RECT srcScrollBarX_Handle[3];

	/// @private
	///
	static GUI_RECT srcScrollBarX_HandleSel[3];

	/// @private
	///
	static GUI_RECT srcScrollBarY_BG[3];

	/// @private
	///
	static GUI_RECT srcScrollBarY_UpBtn[2];

	/// @private
	///
	static GUI_RECT srcScrollBarY_DownBtn[2];

	/// @private
	///
	static GUI_RECT srcScrollBarY_Handle[3];

	/// @private
	///
	static GUI_RECT srcScrollBarY_HandleSel[3];

	/// @private
	///
	static unsigned int srcBackgroundColor;

	/// @private
	///
	static unsigned int srcOutlineColor;

public:

	/// Scroll hover type
	///
	enum HoverType : unsigned char
	{
		/// Not hovering
		HOVER_NONE = 0,

		/// Left / Up arrow button
		HOVER_BUTTON_1,

		/// Scroll handle
		HOVER_HANDLE,

		/// Right / Down arrow button
		HOVER_BUTTON_2
	};

	/// State of scrolling
	///
	enum ScrollState
	{
		/// Not scrolling
		SCROLL_NONE = 0,

		/// User is scrolling the bottom (horizontal) scrollbar
		SCROLL_X,

		/// User is scrolling the right (vertical) scrollbar
		SCROLL_Y
	};
	
	/// Func callback for when a scroll element scrolls.
	///
	/// @param pGui The GUI element being scrolled.
	/// @param scrollAmt Normalized scroll amount.
	typedef void(*OnScrollCallbackFunc)(ScrollAreaInfo* pGui, GUI_POINT_F scrollAmt);

protected: // Cache variables

	/// @protected
	///
	ScrollState scrollState;

	/// @protected
	///
	GUI_RECT scrollBarXBtn_Left;

	/// @protected
	///
	GUI_RECT scrollBarXBtn_Right;

	/// @protected
	///
	GUI_RECT scrollBarYBtn_Up;

	/// @protected
	///
	GUI_RECT scrollBarYBtn_Down;

	/// @protected
	///
	GUI_RECT scrollBarHandleY;

	/// @protected
	///
	GUI_RECT scrollBarY;

	/// @protected
	///
	GUI_RECT scrollBarHandleX;

	/// @protected
	///
	GUI_RECT scrollBarX;

	/// @protected
	///
	GUI_POINT offsetPoint;

	/// @protected
	///
	bool showingHandleX;

	/// @protected
	///
	bool showingHandleY;

public: // Public configuration

	/// Called when this element's scroll amount changes.
	OnScrollCallbackFunc OnScroll;

	/// Normalized scroll amounts (for both x and y)
	///
	GUI_POINT_F scroll;

	/// Draw scroll background
	///
	bool drawBackground;
	
	/// Draw scroll border
	///
	bool drawBorder;

	/// Show the horizontal scrollbar
	///
	bool showScrollX;

	/// Show the vertical scrollbar
	///
	bool showScrollY;

	/// The size of the scrollbar (height for the horizontal scrollbar, width for the vertical scrollbar)
	///
	int scrollBarSize;

	/// Scroll bar handle margin
	///
	int scrollBarHandleMargin;

	/// Scroll bar handle minimum size (width for horizontal scrollbar, width for vertical scrollbar)
	///
	int scrollBarHandleMinSize;

	/// The current hover state for the horizontal bar
	///
	HoverType hoverScrollX;

	/// The current hover state for the vertical bar
	///
	HoverType hoverScrollY;

	/// Size of the content area
	///
	GUI_POINT contentSize;

	/// Amount of scroll on mousewheel movement for both X and Y
	///
	GUI_POINT mouseWheelScroll;

public:

	/// @brief Default constructor
	///
	ScrollAreaInfo();

public:

	/// @brief Check if this element is being scrolled.
	///
	/// @return Returns true if the element is being scrolled.
	inline bool isScrolling()
	{
		return (scrollState != ScrollState::SCROLL_NONE);
	}

public: // Configuration

	/// @brief Set this GUI element as selected or not.
	///
	/// @param value The selection value.
	virtual void setSelected(int value);

	/// @brief Set this GUI element as hovered over or not.
	///
	/// @param value The hover value.
	virtual void setHovered(int value);

	/// @brief Check selection status.
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected();

	/// @brief Check hover status.
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered();

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

public:
	/// @brief Fit the point into the content
	///
	/// @param p The point to show
	///
	/// Resizes the contentSize variable to accomodate showing p
	void fitIntoContent(GUI_POINT p);

	/// @brief Fit the rect into the content
	///
	/// @param r The rect to show
	///
	/// Resizes the contentSize variable to accomodate showing the entirety of r
	void fitIntoContent(GUI_RECT r);

	/// @brief Scroll to a rect
	///
	/// @param r The rect to scroll to
	///
	/// Automatically scroll to show the entirety of rect r
	void scrollTo(GUI_RECT r);

	/// @brief Scroll by pixel amount
	///
	/// @param p The pixel amounts to scroll by
	void scrollByPixelAmount(GUI_POINT p);

public:
	/// @brief Get the scroll offsets.
	///
	/// @return Returns the scroll offset in pixels.
	///
	/// You should use this offset things that should be inside this scroll.
	GUI_POINT getOffset();

	/// @brief Get the scroll offsets.
	///
	/// @return Returns the scroll offset in pixels.
	GUI_POINT getRelativeOffset();

	/// @brief Calculate the content area for this scroll.
	///
	/// @return Returns the currently visible content area rect.
	GUI_RECT getContentArea();

	/// @brief Process mouse input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse);

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();

	/// @brief Enforce a clip rect that accomodates the content area.
	///
	/// @note <i><b>ALWAYS</b></i> use this <i>BEFORE</i> rendering things that should be inside the scroll!
	void begin();

	/// @brief Clear the clip rect enforced by begin().
	///
	/// @note <i><b>ALWAYS</b></i> use this <i>AFTER</i> rendering things that should be inside the scroll!
	/// @note ...Unless you didn't use begin() for whatever reason.
	void end();
};

//------------------------------------------------------------------------------------------------------------

/// Show a text input box
///
class CAVESTORY_MOD_API TextInputInfo : public GuiBase
{
public:

	/// Cancel key types
	///
	enum CANCEL_KEY_TYPE
	{
		/// Enter button
		/// todo: Make pressing enter call a callback
		ENTER,

		/// Escape button
		ESCAPE,

		/// Mouse button (clicked outside of the input box)
		MOUSE_BUTTON
	};

private:

	/// @private
	///
	static GUI_RECT srcInputBox[9];

	/// @private
	///
	static GUI_RECT srcInputBoxSel[9];
	
	/// @private
	///
	static GUI_RECT srcInputBox_Disabled[9];

	/// @private
	///
	static GUI_RECT srcInputBoxSel_Disabled[9];

	/// @private
	///
	static unsigned int srcSelBgColor;

	/// @private
	///
	static unsigned int srcCursorColor;

	/// @private
	///
	static unsigned int srcTextColor;
	
	/// @private
	///
	static unsigned int srcTextColorDisabled;

	/// @private
	///
	static unsigned int srcTextColorCensored;

public: // Data

	/// @brief Func callback for validating input
	///
	/// @param pTextInput The GUI element.
	/// @param pValue The value the user entered.
	/// @param pValidateOutput A buffer that this function should populate with the validated output.
	///
	/// @return Returns true if pValidateOutput shouldn't be used, false if it should be used.
	typedef bool(*ValidateInputCallbackFunc)(TextInputInfo* pTextInput, const char* pValue, char pValidateOutput[1024]);

private: // Cache variables

	/// @private
	///
	FontObject* font;

	/// @private
	///
	char value[1024];

	/// @private
	///
	char* censoredvalue;

	/// @private
	///
	bool isHovering;

	/// @private
	///
	bool isTyping;

	/// @private
	///
	GUI_POINT textSize;

	/// @private
	///
	GUI_RECT textRect;

	/// @private
	///
	GUI_RECT selectionRect;

	/// @private
	///
	GUI_RECT cursorRect;

	/// @private
	///
	int fontHeight;

	/// @private
	///
	int textXOffset;

	/// @private
	///
	int cursorBlink;

public: // Public configuration

	/// Called when text is finalized (by pressing enter, cancel or clicking outside of the input box). Should validate the text. Can be NULL.
	///
	ValidateInputCallbackFunc ValidateInput;

	/// Which cancel key was pressed to lose focus
	///
	CANCEL_KEY_TYPE mCancelKey;

	/// Maximum input length
	///
	int max_length;

	/// Selection start
	///
	int sel_start;

	/// Selection end
	///
	int sel_end;

	/// Is selecting
	///
	bool is_selecting;

	/// Is password box
	///
	bool is_password;

	/// Is disabled
	///
	bool is_disabled;

	/// Call the OnValueChange callback on every keystroke.
	///
	bool callback_on_change;

	/// Text margin
	///
	int margin;

public: // Methods

	/// Default constructor
	///
	TextInputInfo();

	/// Default deconstructor
	///
	~TextInputInfo();

protected:

	/// @brief Resize the text size.
	///
	void resizeTextSize();

public:

	/// @brief Set this GUI element as selected or not.
	///
	/// @param value The selection value.
	virtual void setSelected(int value);

	/// @brief Set this GUI element as hovered over or not.
	///
	/// @param value The hover value.
	virtual void setHovered(int value);

	/// @brief Check selection status.
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected();

	/// @brief Check hover status.
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered();

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

	/// @brief Get the selectable number of elements in this element.
	///
	/// @return Returns the amount of selectable elements in this element.
	virtual int getSelectableCount() { return 1; }

	/// @brief Get the number of elements that can be hovered over in this element.
	///
	/// @return Returns the amount of elements that can be hovered over in this element.
	virtual int getHoverableCount() { return 1; }

	/// @brief Select on mouse up
	///
	/// @return Return false if you want this element to be selected on mousedown instead of mouseup
	virtual bool SelectOnMouseUp() { return false; }

public:
	/// @brief Set the value of the inputbox.
	///
	/// @param bUseCallback Use the OnValueChange callback.
	///
	/// @param value The new value.
	void setValue(const char* value, bool bUseCallback = true);

	/// @brief Get the value of the inputbox.
	///
	/// @return Returns the current inputted value.
	const char* getValue();

	/// @brief Set the font for this element.
	/// 
	/// @param pFont The font to use.
	virtual void setFont(FontObject* pFont);

	/// @brief Set this inputbox as 'focused', and start inputting text.
	///
	/// @param mouse The mouse's position. Used for selecting an initial place in the textbox to start inputting text at
	void startInput(GUI_POINT mouse = GUI_POINT(0, 0));

	/// @brief Stop inputting text.
	///
	void stopInput();

	/// @brief Get input offset by mouse position
	///
	/// @param mouse The mouse position.
	///
	/// @return Returns the index in this input box's value where the mouse position is at
	int getStringOffsetByMouse(GUI_POINT mouse);

	/// @brief Process key input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int processKeys();

	/// @brief Process mouse input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse);

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();
};

//------------------------------------------------------------------------------------------------------------

/// Show a text input box
///
class CAVESTORY_MOD_API KeyInputInfo : public GuiBase
{
public:

	/// Cancel key types
	///
	enum CANCEL_KEY_TYPE
	{
		/// Enter button
		/// todo: Make pressing enter call a callback
		ENTER,

		/// Escape button
		ESCAPE,

		/// Mouse button (clicked outside of the input box)
		MOUSE_BUTTON
	};

private:

	/// @private
	///
	static GUI_RECT srcInputBox[9];

	/// @private
	///
	static GUI_RECT srcInputBoxSel[9];
	
	/// @private
	///
	static GUI_RECT srcInputBox_Disabled[9];

	/// @private
	///
	static GUI_RECT srcInputBoxSel_Disabled[9];

	/// @private
	///
	static unsigned int srcSelBgColor;

	/// @private
	///
	static unsigned int srcCursorColor;

	/// @private
	///
	static unsigned int srcTextColor;
	
	/// @private
	///
	static unsigned int srcTextColorDisabled;

	/// @private
	///
	static unsigned int srcTextColorCensored;

public: // Data

	/// @brief Func callback for validating input
	///
	/// @param pKeyInput The GUI element.
	/// @param iKeyValue The key the user entered. This is an SDL_Scancode value.
	///
	/// @return Returns true if the value should be used, false otherwise.
	typedef bool(*ValidateKeyInputCallbackFunc)(KeyInputInfo* pKeyInput, int iKeyValue);

private: // Cache variables

	/// @private
	///
	FontObject* font;

	/// @private
	///
	int m_KeyValue;

	/// @private
	///
	bool isHovering;

	/// @private
	///
	bool isTyping;

	/// @private
	///
	GUI_RECT textRect;

	/// @private
	///
	int fontHeight;

	/// The display text.
	///
	char disp_text[256];

public: // Public configuration

	/// Pointer key value
	///
	int* m_PtrKeyValue;

	/// Called when text is finalized (by pressing enter, cancel or clicking outside of the input box). Should validate the text. Can be NULL.
	///
	ValidateKeyInputCallbackFunc ValidateInput;

	/// Which cancel key was pressed to lose focus
	///
	CANCEL_KEY_TYPE mCancelKey;

	/// Is disabled
	///
	bool is_disabled;

	/// Text margin
	///
	int margin;

public: // Methods

	/// Default constructor
	///
	KeyInputInfo();

	/// Default deconstructor
	///
	~KeyInputInfo();

public:

	/// @brief Set this GUI element as selected or not.
	///
	/// @param value The selection value.
	virtual void setSelected(int value);

	/// @brief Set this GUI element as hovered over or not.
	///
	/// @param value The hover value.
	virtual void setHovered(int value);

	/// @brief Check selection status.
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected();

	/// @brief Check hover status.
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered();

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

	/// @brief Get the selectable number of elements in this element.
	///
	/// @return Returns the amount of selectable elements in this element.
	virtual int getSelectableCount() { return 1; }

	/// @brief Get the number of elements that can be hovered over in this element.
	///
	/// @return Returns the amount of elements that can be hovered over in this element.
	virtual int getHoverableCount() { return 1; }

	/// @brief Select on mouse up
	///
	/// @return Return false if you want this element to be selected on mousedown instead of mouseup
	virtual bool SelectOnMouseUp() { return false; }

public:
	/// @brief Set the value of the keyinput.
	///
	/// @param iScancode The SDL_Scancode to use.
	/// @param bUseCallback Use the OnValueChange callback.
	///
	/// @param value The new value.
	void setValue(int iScancode, bool bUseCallback = true);

	/// @brief Get the value of the keyinputinfo.
	///
	/// @return Returns the current key.
	int getValue();

	/// @brief Set the font for this element.
	/// 
	/// @param pFont The font to use.
	virtual void setFont(FontObject* pFont);

	/// @brief Update the display text for this control.
	///
	void updateDisplayText();

	/// @brief Set this inputbox as 'focused', and start inputting text.
	///
	void startInput();

	/// @brief Stop inputting text.
	///
	/// @param bSaveValue Save the value.
	void stopInput(bool bSaveValue = true);

	/// @brief Process key input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int processKeys();

	/// @brief Process mouse input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse);

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();
};

//------------------------------------------------------------------------------------------------------------

/// Show a toggleable checkbox
///
class CAVESTORY_MOD_API CheckBoxInfo : public GuiBase
{

private: // Cache variables

	/// @private
	///
	bool mIsHovering;

public: // Public configuration

	/// Whether this checkbox is enabled or not
	///
	bool mValue;

	/// Radio group ID
	///
	int mRadioGroup;

	/// Use small sprite instead of the big one
	///
	bool mSmall;

	/// Disable.
	///
	bool mIsDisabled;

public:

	/// Default constructor
	///
	CheckBoxInfo();

public:

	/// @brief Set this GUI element as selected or not.
	///
	/// @param value The selection value.
	virtual void setSelected(int value);

	/// @brief Set this GUI element as hovered over or not.
	///
	/// @param value The hover value.
	virtual void setHovered(int value);

	/// @brief Check selection status.
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected();

	/// @brief Check hover status.
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered();

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

	/// @brief Get the selectable number of elements in this element.
	///
	/// @return Returns the amount of selectable elements in this element.
	virtual int getSelectableCount() { return 1; }

	/// @brief Get the number of elements that can be hovered over in this element.
	///
	/// @return Returns the amount of elements that can be hovered over in this element.
	virtual int getHoverableCount() { return 1; }

public:
	/// @brief Set the value of this checkbox
	///
	/// @param val The value of the checkbox.
	void setValue(bool val);

	/// @brief Get the value of this checkbox
	///
	/// @return Returns true if the checkbox is ticked, false if not.
	bool getValue();

public:

	/// @brief Process key input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int processKeys();

	/// @brief Process mouse input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse);

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();
};

//------------------------------------------------------------------------------------------------------------

/// Show a number slider
///
class CAVESTORY_MOD_API SliderIntInfo : public GuiBase
{
private:

	/// @private
	///
	static GUI_RECT srcHeadButton[2];

	/// @private
	///
	static GUI_RECT srcTimeline[3];
	
	/// @private
	///
	static unsigned int srcTooltipTextColor;

private:

	/// @private
	///
	char mText[32];

	/// @private
	///
	FontObject* font;

	/// @private
	///
	bool mIsHovering;

	/// @private
	///
	bool mIsDragging;

	/// @private
	///
	char mKeyWait;

	/// @private
	///
	int mKeyHeld;

	/// @private
	///
	int mKeyHeldOld;

	/// @private
	///
	GUI_POINT mButton;

	/// @private
	///
	GUI_RECT mTooltip;

	/// @private
	///
	GUI_POINT mTextPoint;

	/// @private
	///
	GUI_POINT mTooltipArrow;

public: // Public configuration

	/// The selected number
	///
	float mValue;
	
	/// The maximum value
	///
	float mMax;

	/// The minimum value
	///
	float mMin;

	/// Round mValue to nearest value on value change
	///
	bool mIntOnly;

	/// Update the value when being dragged
	///
	bool mLiveUpdate;

public:

	/// Default constructor
	///
	SliderIntInfo();

public:

	/// @brief Set this GUI element as selected or not.
	///
	/// @param value The selection value.
	virtual void setSelected(int value);

	/// @brief Set this GUI element as hovered over or not.
	///
	/// @param value The hover value.
	virtual void setHovered(int value);

	/// @brief Check selection status.
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected();

	/// @brief Check hover status.
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered();

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

	/// @brief Get the selectable number of elements in this element.
	///
	/// @return Returns the amount of selectable elements in this element.
	virtual int getSelectableCount() { return 1; }

	/// @brief Get the number of elements that can be hovered over in this element.
	///
	/// @return Returns the amount of elements that can be hovered over in this element.
	virtual int getHoverableCount() { return 1; }

	/// @brief Select on mouse up
	///
	/// @return Return false if you want this element to be selected on mousedown instead of mouseup
	virtual bool SelectOnMouseUp() { return false; }

public:

	/// @brief Set the value
	///
	/// @param val The new value
	void setValue(int val);

	/// @brief Get the value
	///
	/// @return Returns the integer value for this element
	int getValue();

public:
	/// @brief Set this element's font
	///
	/// @param pFont The new font.
	virtual void setFont(FontObject* pFont) { font = pFont; }

	/// @brief Process key input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int processKeys();

	/// @brief Process mouse input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse);

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();

	/// @brief Render the important bits of this element.
	///
	/// @note This function is used to draw things that should be at the front of the screen, so make sure to call it AFTER rendering everything else.
	virtual void importantRender();
};

//------------------------------------------------------------------------------------------------------------

/// Show a clickable button
///
class CAVESTORY_MOD_API ButtonInfo : public GuiBase
{
public:
	
	/// OnPressed Callback
	///
	typedef void(*OnButtonPressedFunc)(ButtonInfo*, void*);

private:

	/// @private
	///
	static GUI_RECT srcButton[9];

	/// @private
	///
	static GUI_RECT srcButtonSel[9];

	/// Text color
	/// @private
	static unsigned int srcTextColor;

private: // Cache variables

	/// @private
	///
	FontObject* font;

	/// @private
	///
	GUI_POINT textPoint;

	/// @private
	///
	bool isHovering;

public: // Public configuration

	/// The text to display on the button
	///
	char text[256];

	/// Vertical text margin
	///
	int vmargin;

	/// Text margin
	///
	int margin;

	/// On press callback
	///
	OnButtonPressedFunc OnClicked;

public: // Methods

	/// Default constructor
	///
	ButtonInfo();

public:

	/// @brief Set this GUI element as selected or not.
	///
	/// @param value The selection value.
	virtual void setSelected(int value);

	/// @brief Set this GUI element as hovered over or not.
	///
	/// @param value The hover value.
	virtual void setHovered(int value);

	/// @brief Check selection status.
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected();

	/// @brief Check hover status.
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered();

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

	/// @brief Get the selectable number of elements in this element.
	///
	/// @return Returns the amount of selectable elements in this element.
	virtual int getSelectableCount() { return 1; }

	/// @brief Get the number of elements that can be hovered over in this element.
	///
	/// @return Returns the amount of elements that can be hovered over in this element.
	virtual int getHoverableCount() { return 1; }

public:

	/// @brief Set this element's font
	///
	/// @param pFont The new font.
	virtual void setFont(FontObject* pFont) { font = pFont; }
	
	/// @brief Get this element's font
	///
	/// @param pFont The new font.
	virtual FontObject* getFont() { return font; }

	/// @brief Process key input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int processKeys();

	/// @brief Process mouse input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse);

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();
};

//------------------------------------------------------------------------------------------------------------

/// Show a dropdown box
///
class CAVESTORY_MOD_API DropdownBoxInfo : public GuiBase
{
private:

	/// @private
	///
	static GUI_RECT srcDropdownBoxBG_Sel[3];

	/// @private
	///
	static GUI_RECT srcDropdownBoxBG_Off[3];

	/// @private
	///
	static GUI_RECT srcDropdownBoxBG_OffSel[3];

	/// @private
	///
	static GUI_RECT srcDropdownBoxBG[3];

	/// @private
	///
	static GUI_RECT srcContentBG[9];

	/// @private
	///
	static GUI_RECT srcPointer;

public:

	/// Dropdown box item struct
	///
	struct DBI_Item
	{
		/// Text for this item
		///
		char text[256];

		/// This item's rect
		///
		GUI_RECT item_rect;

		/// The position for this item's text
		///
		GUI_POINT text_point;

		/// The size of this item's text
		///
		GUI_POINT text_size;
	};

public: // Public data

	/// @private
	///
	DBI_Item* mItems;

	/// @private
	///
	int mItemCount;

	/// @private
	///
	int mItemsSize;

private: // Cache variables

	/// @private
	///
	GUI_POINT mTextPoint;

	/// @private
	///
	GUI_POINT mTextSize;

	/// @private
	///
	GUI_POINT mPointerPos;

	/// @private
	///
	GUI_RECT mTextRect;

	/// @private
	///
	GUI_RECT mContentBG;

	/// @private
	///
	FontObject* mFont;

	/// @private
	///
	bool mIsOpen;

	/// @private
	///
	bool mHovered;

	/// @private
	///
	int mHoverId;

	/// @private
	///
	ScrollAreaInfo mScroll;


public: // Public configuration

	/// Containing rect.
	///
	GUI_RECT containing_rect;

	/// The color of the text when the element is disabled
	///
	GUI_COLOR mText_Disabled;

	/// The color of the text when the element is normal
	///
	GUI_COLOR mText_Normal;
	
	/// The color of the text when the element is selected in the list
	///
	GUI_COLOR mText_ListSelected;

	/// The color of the text when the element is not selected
	///
	GUI_COLOR mText_NotSelected;

	/// Whether this element is disabled or not
	///
	bool mDisabled;

	/// Text margin
	///
	int mTextMargin;

	/// Item margin
	///
	int mItemMargin;

	/// Whether the dropbox is expanded or not
	///
	int mSelected;

public: // Methods

	/// Default constructor
	///
	DropdownBoxInfo();

	/// Default deconstructor
	///
	~DropdownBoxInfo();

public:
	/// @brief Get item text by index
	///
	/// @param index The index of the item
	///
	/// @return Returns the item text for the item at the index. Returns NULL if not valid.
	const char* getItemText(int index);

	/// @brief Set this element's font
	///
	/// @param pFont The new font.
	virtual void setFont(FontObject* pFont);

	/// @brief Add an item to the list.
	///
	/// @param text The text of the item.
	void addItem(const char* text);

	/// @brief Clear all items
	///
	void clearItems();

	/// @brief Get number of selectable options.
	///
	/// @return Returns number of selectable options.
	int getCount();

	/// @brief Set the selected item.
	///
	void setSelectedItem(int iItemId);

public:
	
	/// @brief Set this GUI element as selected or not.
	///
	/// @param value The selection value.
	virtual void setSelected(int value);

	/// @brief Set this GUI element as hovered over or not.
	///
	/// @param value The hover value.
	virtual void setHovered(int value);

	/// @brief Check selection status.
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected();

	/// @brief Check hover status.
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered();

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

	/// @brief Get the selectable number of elements in this element.
	///
	/// @return Returns the amount of selectable elements in this element.
	virtual int getSelectableCount() { return 1; }

	/// @brief Get the number of elements that can be hovered over in this element.
	///
	/// @return Returns the amount of elements that can be hovered over in this element.
	virtual int getHoverableCount() { return 1; }

public:
	/// @brief Process key input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int processKeys();

	/// @brief Process mouse input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse);

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();

	/// @brief Render the important bits of this element.
	///
	/// @note This function is used to draw things that should be at the front of the screen, so make sure to call it AFTER rendering everything else.
	virtual void importantRender();
};

//------------------------------------------------------------------------------------------------------------

/// Several combined GUI elements to form a color picker
///
class CAVESTORY_MOD_API ColorPickerInfo : public GuiBase
{
private:
	/// @private
	///
	static GUI_RECT srcPanelLeft[9];

	/// @private
	///
	static GUI_RECT srcPanelRight[9];

private: // Cache variables

	/// @private
	///
	FontObject* mFont;

	/// @private
	///
	GUI_RECT mSpriteRect;

	/// @private
	///
	GUI_RECT mLeftPanel;

	/// @private
	///
	GUI_RECT mRightPanel;

	/// @private
	///
	bool isHovering;

	/// @private
	///
	GuiBase* mGuiBaseList[6];

public:
	
	/// Red slider
	///
	SliderIntInfo mSlider_Red;

	/// Green slider
	///
	SliderIntInfo mSlider_Green;

	/// Blue slider
	///
	SliderIntInfo mSlider_Blue;

	/// Red value textbox
	///
	TextInputInfo mEdit_Red;

	/// Green value textbox
	///
	TextInputInfo mEdit_Green;

	/// Blue value textbox
	///
	TextInputInfo mEdit_Blue;

public: // Public configuration

	/// Called when render() is called. Renders an icon on the left of the element
	///
	/// @param pGui The element object.
	/// @param pRect The rect of the icon.
	/// @param pColor The current color value.
	void(*OnRenderSprite)(ColorPickerInfo* pGui, GUI_RECT pRect, GUI_COLOR pColor);

	/// The sprite size to accomadate for when calling OnRenderSprite()
	///
	GUI_POINT spriteSize;

	/// The current color value
	///
	GUI_COLOR color;

	/// Item margin
	///
	int margin;

public: // Methods

	/// Default constructor
	///
	ColorPickerInfo();

public:
	/// @brief Set this GUI element as selected or not.
	///
	/// @param value The selection value.
	virtual void setSelected(int value);

	/// @brief Set this GUI element as hovered over or not.
	///
	/// @param value The hover value.
	virtual void setHovered(int value);

	/// @brief Check selection status.
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected();

	/// @brief Check hover status.
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered();

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

	/// @brief Get the selectable number of elements in this element.
	///
	/// @return Returns the amount of selectable elements in this element.
	virtual int getSelectableCount();

	/// @brief Get the number of elements that can be hovered over in this element.
	///
	/// @return Returns the amount of elements that can be hovered over in this element.
	virtual int getHoverableCount();

	/// @brief Get a hoverable sub-element.
	///
	/// @param index The sub-element's index.
	///
	/// @return Returns a sub-element, NULL if it couldn't be resolved.
	virtual GuiBase* getHoverable(int index);

	/// @brief Get a selectable sub-element.
	///
	/// @param index The sub-element's index.
	///
	/// @return Returns a sub-element, NULL if it couldn't be resolved.
	virtual GuiBase* getSelectable(int index);

public:

	/// @brief Update current color
	///
	/// @param callback Use the 'OnValueChange' callback
	void updateColor(bool callback = true);

public:

	/// @brief Set this element's font
	///
	/// @param pFont The new font.
	virtual void setFont(FontObject* pFont);

	/// @brief Process mouse input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse);

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();

	/// @brief Render the important bits of this element.
	///
	/// @note This function is used to draw things that should be at the front of the screen, so make sure to call it AFTER rendering everything else.
	virtual void importantRender();
};

//------------------------------------------------------------------------------------------------------------

/// Show a clickable button
///
class CAVESTORY_MOD_API DraggableWindowInfo : public GuiBase
{
public:
	
	/// OnPressed Callback
	///
	typedef void(*OnWindowDraggedFunc)(DraggableWindowInfo*);
	
	/// OnCollapse Callback
	///
	typedef void(*OnWindowCollapsedFunc)(DraggableWindowInfo*);

	/// OnExpand Callback
	///
	typedef void(*OnWindowExpandedFunc)(DraggableWindowInfo*);
	
	/// OnClose Callback
	///
	typedef void(*OnWindowClosedFunc)(DraggableWindowInfo*);

public:

	enum ActionType
	{
		ACTION_NONE,
		ACTION_HOVER_CLOSE,
		ACTION_HOVER_MINMAX,
		ACTION_CLICK_CLOSE,
		ACTION_CLICK_MINMAX,
		ACTION_DRAG,
		ACTION_RESIZE_LEFT,
		ACTION_RESIZE_TOP,
		ACTION_RESIZE_RIGHT,
		ACTION_RESIZE_BOTTOM,
		ACTION_RESIZE_BOTTOMLEFT,
		ACTION_RESIZE_BOTTOMRIGHT,
		ACTION_RESIZE_TOPLEFT
	};

private:

	/// Text color
	/// @private
	static unsigned int srcTextColor;

private: // Cache variables

	/// @private
	///
	GUI_POINT last_mouse_pos;

	/// @private
	///
	GUI_RECT last_rect;

	/// @private
	///
	bool using_lmb;

public: // Draggable stuff

	/// @private
	///
	CenteredTextInfo text;
	
	/// @private
	///
	GUI_RECT rect_resize_width[2];

	/// @private
	///
	GUI_RECT rect_resize_height[2];

public: //

	/// @private
	///
	GUI_RECT topRect;
	
	/// @private
	///
	GUI_RECT bottomRect;
	
	/// @private
	///
	GUI_RECT Button_Close;

	/// @private
	///
	GUI_RECT Button_MinMax;

	/// @private
	///
	bool is_collapsed;

	/// @private
	///
	ActionType current_action;

public: // Public configuration

	/// The text margin for this object.
	///
	int margin;

	/// On drag callback
	///
	OnWindowDraggedFunc OnDragged;
	
	/// On drag start callback
	///
	OnWindowDraggedFunc OnDragStart;
	
	/// On drag end callback
	///
	OnWindowDraggedFunc OnDragEnd;
	
	/// On resize start callback
	///
	OnWindowDraggedFunc OnResizeStart;
	
	/// On resizing callback
	///
	OnWindowDraggedFunc OnResizing;
	
	/// On resize end callback
	///
	OnWindowDraggedFunc OnResizeEnd;
	
	/// On collapse callback
	///
	OnWindowCollapsedFunc OnCollapse;
	
	/// On expand callback
	///
	OnWindowExpandedFunc OnExpand;

	/// On close callback
	///
	OnWindowClosedFunc OnClose;

	/// Show the close button
	///
	bool show_button_close;
	
	/// Show the minmax button
	///
	bool show_button_minmax;

	/// Whether this window is draggable or not
	///
	bool is_draggable;

	/// Whether the width of this window can be resized.
	///
	bool can_resize_width;
	
	/// Whether the height of this window can be resized.
	///
	bool can_resize_height;

	/// The resize hotspot size
	///
	int resize_hotspot_size;

	/// The rect at which the window's contents should be at
	///
	GUI_RECT content_rect;

	/// The point at which the window is collapsed at.
	///
	GUI_POINT collapse_position;
	
	/// The normal coordinates at which the element will move when positioning relative to collapse_position.
	///
	GUI_POINT collapse_normal;

	/// The minimum content size this window can have.
	///
	GUI_POINT min_content_size;

	/// The valid rect that this window can be dragged to.
	///
	GUI_RECT valid_drag_rect;

public: // Methods

	/// Default constructor
	///
	DraggableWindowInfo();
	
	/// Default deconstructor
	///
	~DraggableWindowInfo();

public:

	/// @brief Check to see if this window is collapsed.
	///
	/// @return
	inline bool isCollapsed() { return is_collapsed; }

	/// @brief Set whether this window is collapsed.
	///
	/// @param bValue The new value.
	inline void setCollapsed(bool bValue) { is_collapsed = bValue; }

	/// @brief Set this element's text.
	///
	/// @param pValue The text to set.
	void setText(const char* pValue);

	/// @brief Set this GUI element as selected or not.
	///
	/// @param value The selection value.
	virtual void setSelected(int value);

	/// @brief Set this GUI element as hovered over or not.
	///
	/// @param value The hover value.
	virtual void setHovered(int value);

	/// @brief Check selection status.
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected();

	/// @brief Check hover status.
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered();

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

	/// @brief Get the selectable number of elements in this element.
	///
	/// @return Returns the amount of selectable elements in this element.
	virtual int getSelectableCount() { return 1; }

	/// @brief Get the number of elements that can be hovered over in this element.
	///
	/// @return Returns the amount of elements that can be hovered over in this element.
	virtual int getHoverableCount() { return 1; }

public:

	/// @brief Get the area that content goes in.
	/// 
	GUI_RECT getContentArea();
	
	/// @brief Set the content size of this window.
	/// 
	void setContentSize(GUI_POINT pSize);

	/// @brief Set this element's font
	///
	/// @param pFont The new font.
	virtual void setFont(FontObject* pFont);

	/// @brief Process key input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int processKeys();

	/// @brief Process mouse input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse);

	/// @brief Calculate the top.
	///
	void calculateTop();

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();
};

//------------------------------------------------------------------------------------------------------------

/// Show a windows-style group box
///
class CAVESTORY_MOD_API GroupBoxInfo : public GuiBase
{
private:

	/// @private
	///
	FontObject* mFont;

public:

	/// The text to display with this group.
	///
	char text[128];

	GUI_POINT mTextPoint;
	GUI_POINT mTextSize;
	GUI_RECT mContentRect;
	GUI_POINT mLine_Top_1[2];
	GUI_POINT mLine_Top_2[2];
	GUI_POINT mLine_Left[2];
	GUI_POINT mLine_Right[2];
	GUI_POINT mLine_Bottom[2];

public: // Methods

	/// Default constructor
	///
	GroupBoxInfo();

public:

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

	/// @brief Get the selectable number of elements in this element.
	///
	/// @return Returns the amount of selectable elements in this element.
	virtual int getSelectableCount() { return 0; }

	/// @brief Get the number of elements that can be hovered over in this element.
	///
	/// @return Returns the amount of elements that can be hovered over in this element.
	virtual int getHoverableCount() { return 0; }

public:

	virtual void setSelected(int value) {}
	virtual void setHovered(int value) { }
	virtual bool isSelected() { return false; }
	virtual bool isHovered() { return false; }
	virtual int process(GUI_POINT) { return 0; }

public:

	/// @brief Resize this element to fit the given content rect.
	///
	/// @param content_rect The content that should fit in this group.
	void fitRect(GUI_RECT content_rect, int margin = 0);

	/// @brief Set this element's font
	///
	/// @param pFont The new font.
	virtual void setFont(FontObject* pFont) { mFont = pFont; }

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();
};

//------------------------------------------------------------------------------------------------------------

/// Show a tab selector
///
class CAVESTORY_MOD_API TabBookInfo : public GuiBase
{
public:

	/// OnPressed Callback
	///
	typedef void(*OnTabSelectedFunc)(TabBookInfo*, int);
	
	/// OnTabClose Callback
	///
	typedef void(*OnTabCloseFunc)(TabBookInfo*, int);

private:

	/// @private
	///
	static GUI_RECT srcSelectedTabButton_Normal[9];

	/// @private
	///
	static GUI_RECT srcSelectedTabButton_Pressed[9];

	/// @private
	///
	static GUI_RECT srcTabButton_Normal[9];

	/// @private
	///
	static GUI_RECT srcTabButton_Pressed[9];
	
	/// @private
	///
	static GUI_RECT srcTabButton_CloseButton[3];

public:

	/// Dropdown box item struct
	///
	struct TAB
	{
		/// Text for this item
		///
		char text[64];
		
		/// This item's rect
		///
		GUI_RECT item_rect;
		
		/// This item's draw rect
		///
		GUI_RECT draw_rect;

		/// The position for this item's text
		///
		GUI_POINT text_point;

		/// The size of this item's text
		///
		GUI_POINT text_size;

		/// This item's close button rect
		///
		GUI_RECT close_rect;

		/// Whether this tab has a close button or not.
		///
		bool is_closable;
	};

private: // Cache variables

	/// @private
	///
	TAB* mItems;

	/// @private
	///
	int mItemCount;

	/// @private
	///
	FontObject* mFont;

	/// @private
	///
	int mHoveredTab;
	
	/// @private
	///
	bool mHoveredTab_Close;


	/// @private
	///
	int mLargestTabHeight;

public: // Public configuration

	/// Content Rect
	///
	GUI_RECT mTabContentRect;

	/// The 'on tab selection' callback
	///
	OnTabSelectedFunc OnTabSelectCallback;

	/// The 'on tab closed' callback
	///
	OnTabCloseFunc OnTabCloseCallback;

	/// The selected tab
	///
	int mSelectedTab;

	/// Text margin
	///
	int mMargin;

	/// Draw the border color.
	///
	bool mDrawBorder;

	/// Draw the fill color.
	///
	bool mDrawFill;

public: // Methods

	/// Default constructor
	///
	TabBookInfo();

	/// Default deconstructor
	///
	~TabBookInfo();

public:

	/// @brief Set this element's font
	///
	/// @param pFont The new font.
	virtual void setFont(FontObject* pFont) { mFont = pFont; }

	/// @brief Add an item to the list.
	///
	/// @param text The text of the item.
	/// @param bCanBeClosed Whether the new tab can be closed.
	void addTab(const char* pTabText, bool bCanBeClosed = false);

	/// @brief Clear all items
	///
	void clearTabs();

	/// @brief Get number of selectable options.
	///
	/// @return Returns number of selectable options.
	int getTabCount();

	/// @brief Get a tab by index.
	///
	/// @param tab The tab's index.
	///
	/// @return Returns the tab from the given index if it exists, otherwise returns NULL.
	TAB* getTab(int tab);

public:

	/// @brief Set whether we've hovered over the close button.
	///
	/// @param bValue The new value.
	inline void setHoveredOverCloseButton(bool bValue) { mHoveredTab_Close = bValue; }
	
	/// @brief Get whether we've hovered over the close button.
	///
	/// @return Returns the value.
	inline bool getHoveredOverCloseButton() { return mHoveredTab_Close; }

	/// @brief Set this GUI element as selected or not.
	///
	/// @param value The selection value.
	virtual void setSelected(int value);

	/// @brief Set this GUI element as hovered over or not.
	///
	/// @param value The hover value.
	virtual void setHovered(int value);

	/// @brief Check selection status.
	///
	/// @return Returns true if this element is selected.
	virtual bool isSelected();

	/// @brief Check hover status.
	///
	/// @return Returns true if this element is being hovered over.
	virtual bool isHovered();

	/// @brief Check hoverability status.
	///
	/// @return Returns true if this element can be hovered over.
	virtual bool isHoverable();

	/// @brief Get the selectable number of elements in this element.
	///
	/// @return Returns the amount of selectable elements in this element.
	virtual int getSelectableCount();

	/// @brief Get the number of elements that can be hovered over in this element.
	///
	/// @return Returns the amount of elements that can be hovered over in this element.
	virtual int getHoverableCount();

	/// @brief Get the draw offset for this control.
	///
	/// @return Returns this GUI element's rect + the draw offset for tab content.
	inline GUI_RECT getDrawRect() { return mTabContentRect; }

public:

	/// @brief Process key input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int processKeys();

	/// @brief Process mouse input for this element
	///
	/// @return Returns #ProcessReturnType flags.
	/// @see ProcessReturnType
	virtual int process(GUI_POINT mouse);

	/// @brief Re-calculate this element's data
	///
	/// @warning You should ALWAYS call this at least once <i>BEFORE</i> you call #render()!
	virtual void calculate();

	/// @brief Render this element.
	///
	virtual void render();
};

/// @}
///

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @brief Stores information from CSM_CastRay().
///
struct RAYCAST_QUERY
{
	/// The hit position (subpixel coordinates).
	///
	GUI_POINT m_HitPos;

	/// The hit map tile's map coordinates
	///
	GUI_POINT tileHitPos;

	/// The normal of the face that we hit.
	///
	GUI_POINT_F mNormal;

	/// Whether the ray went out of bounds or not.
	///
	bool mOutOfBounds;

	/// The hit NPC.
	///
	NPCHAR* mHitNPC;

	/// Whether the ray hit the start block
	///
	bool mHitStartBlock;

	/// Whether the ray hit the end block
	///
	bool mHitEndBlock;

	/// This stores which way we pushed the point out of the hit face. So, if it's DIRECT_LEFT, then we pushed the hit-point to the left of the collision, to get it out of the collision area.
	///
	unsigned char mDirect;

	/// Options
	///
	struct
	{
		/// Bits that the entities we hit MUST have to be a valid hit target.
		///
		unsigned int ent_bits_mask;

		/// Whether to allow hitting entities or not.
		///
		bool hit_entities;

		/// Whether to allow hitting slopes or not.
		///
		bool hit_slopes;
	} mConfig;

	/// Constructor!
	///
	RAYCAST_QUERY()
	{
		mConfig.ent_bits_mask = 0;
		mConfig.hit_slopes    = true;
		mConfig.hit_entities  = false;
	}
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

struct MAP_CHANGED_TILE
{
	int tile_index;
	short new_tile_id;
	short old_tile_id;

	inline bool operator==(const MAP_CHANGED_TILE& other) const { return tile_index == other.tile_index; }
	inline bool operator!=(const MAP_CHANGED_TILE& other) const { return tile_index != other.tile_index; }
	inline bool operator< (const MAP_CHANGED_TILE& other) const { return tile_index < other.tile_index; }
	inline bool operator<=(const MAP_CHANGED_TILE& other) const { return tile_index <= other.tile_index; }
	inline bool operator> (const MAP_CHANGED_TILE& other) const { return tile_index > other.tile_index; }
	inline bool operator>=(const MAP_CHANGED_TILE& other) const { return tile_index >= other.tile_index; }

	MAP_CHANGED_TILE()
	{
		tile_index = 0;
		new_tile_id = 0;
		old_tile_id = 0;
	}

	MAP_CHANGED_TILE(int index)
	{
		MAP_CHANGED_TILE();

		tile_index = index;
	}
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

#define MAP_MAX_ZONES 0x7FFFFFFF

/// @brief A map zone to be used for network optimization.
///
/// Optimizes multiplayer games by only transmitting the NPCs within zones to the players who are also within the same zone.
class CAVESTORY_MOD_API MAP_ZONE
{
public:
	/// The ID for this zone
	///
	int id;

	/// The rect for this zone.
	///
	struct MAP_ZONE_RECT_STRUCT
	{
		/// X
		///
		int x;

		/// Y
		///
		int y;

		/// Width
		///
		int w;

		/// Height
		///
		int h;
	} rect;

	/// 'Use' count for this zone
	///
	int used;

	/// deprectated
	///
	int soft_npc_count;

private: // Data

	/// NPC table struct
	///
	void* tables_struct;

public:

	/// Constructor
	///
	MAP_ZONE();

	/// Deconstructor
	///
	~MAP_ZONE();

public: // General
	/// @brief Initialize this object.
	///
	void Init();

	/// @brief Free all memory associated with this object.
	///
	void Free();

public: // NPC Management
	/// @brief Add an NPCHAR to the list.
	///
	/// @param npc The NPC to add.
	void AddNpChar(NPCHAR* npc);

	/// @brief Remove an NPCHAR from the list.
	///
	/// @param npc The NPC to add.
	void RemoveNpChar(NPCHAR* npc);

	/// @brief Delete all NPCs that are inside of this zone.
	///
	void DeleteAllNpChar();

	/// @brief Think tick all npcs
	///
	void ActNpChar();

	/// @brief Draw npcs
	///
	void PutNpChar(int frame_x, int frame_y);

	/// @brief Check if this zone has an npc in it.
	///
	/// @param npc The NPCHAR to find.
	///
	/// @returns bool Returns true if the npc was found, false if otherwise.
	bool HasNpChar(NPCHAR* npc);

public: // Boss Management
	/// @brief Add a boss to the list.
	///
	/// @param npc The boss npc to add.
	void AddBoss(NPCHAR* npc);

	/// @brief Remove a boss from the list.
	///
	/// @param npc The boss npc to add.
	void RemoveBoss(NPCHAR* npc);

	/// @brief Delete all bosses that are inside of this zone.
	///
	void DeleteAllBoss();

	/// @brief Think tick all npcs
	///
	void ActBoss();

	/// @brief Draw npcs
	///
	void PutBoss(int frame_x, int frame_y);

	/// @brief Check if this zone has a boss npc in it.
	///
	/// @param npc The NPCHAR to find.
	///
	/// @returns bool Returns true if the boss was found, false if otherwise.
	bool HasBoss(NPCHAR* npc);
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// Contains an animated map tile's information.
///
struct MAP_ANIMATED_TILE
{
	/// Whether this should be drawn in the foreground(TRUE) or background(FALSE).
	///
	bool foreground;

	/// The offset of the map tile we're replacing. Should equal (x + y * gMap.width)
	///
	unsigned int tile_offset;

	/// The surface IDs for this tile.
	///
	Surface_Ids surf_id;

	/// The rect list for this tile.
	///
	RECT* rect_list;
	
	/// The number of rects in this tile.
	///
	unsigned char rect_count;
	
	/// The amount of frames that need to pass before the framecounter for this tile is incremented.
	///
	unsigned short wait;

	// **--** RUNTIME CACHE VARIABLES **--**

	/// The current frame for this tile.
	///
	unsigned char frame;

	/// The timer for this tile.
	///
	unsigned char timer;

	/// The next linked 
	///
	MAP_ANIMATED_TILE* next;
	
	/// The next linked to draw
	///
	MAP_ANIMATED_TILE* draw_next;
	
	/// The next linked to process
	///
	MAP_ANIMATED_TILE* proc_next;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// Contains the currently loaded stage's map data.
///
struct MAP_DATA
{
	/// Tile indices
	///
	unsigned char* data;

	/// Zone tile data
	///
	unsigned char* ngdata;

	/// Map tileset attribute table
	///
	unsigned char atrb[0x10001]; //Why is this 257 bytes?

	/// Width of map
	///
	short width;

	/// Height of map
	///
	short length;

	/// Map zones
	///
	MAP_ZONE* zones;

	/// Number of map zones
	///
	int zone_count;

	/// Drawn animated tiles
	///
	MAP_ANIMATED_TILE* anim_draw;

	/// Process list
	///
	MAP_ANIMATED_TILE* anim_proc_list;
	
	/// Animated map tiles. This should ALWAYS be sorted by tile offset.
	///
	MAP_ANIMATED_TILE* anim_list;

	/// Number of animated foreground map tiles
	///
	int anim_count;

	/// The CRC of this map
	///
	unsigned int crc;

	/// internal use; do not use / set
	///
	void* compiled_map_resource;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @brief The act function for this player state.
///
/// @param bKey Whether key inputs should be allowed for this tick or not.
/// @param pInterface The safe interface for this player.
/// @param pMC The player character we should be manipulating.
/// @param pPhysics The physics we should be using for this tick. If the player is in water, this will be pMC->physics_underwater. If not, then it will be pMC->physics_normal.
typedef void(*PlayerState_ActFuncType)(BOOL bKey, SafeClientInterface* pInterface, MYCHAR* pMC, const MYCHAR_PHYSICS* pPhysics);

/// @brief The animation function for this player state.
///
/// @param bKey Whether key inputs should be allowed for this tick or not.
/// @param pMC The player character we should be manipulating.
/// @param pInterface The safe interface for this player.
///
/// @return Should return a PlayerAnimFrame. This will be set as the current player animation.
typedef unsigned int(*PlayerState_AnimFuncType)(BOOL bKey, MYCHAR* pMC, SafeClientInterface* pInterface);

/// @brief The draw function for this player state.
///
/// @param iFrameX The camera X offset.
/// @param iFrameY The camera Y offset.
/// @param pMC The player character we should be manipulating.
/// @param pInterface The interface for this player.
typedef void(*PlayerState_DrawFuncType)(int iFrameX, int iFrameY, MYCHAR* pMC, SafeClientInterface* pInterface);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @brief Player state argument info
///
struct MYCHAR_STATE_ARG_INFO
{
	/// Whether this is the end of the argument list or not.
	///
	bool Used;

	/// The field this argument info object represents.
	///
	PLAYER_STATE_TRANSMIT_FLAGS Field;

	/// Use the default size for this field.
	///
	bool UseDefaultSize;

	/// Traditional size.
	///
	unsigned char NetSize;

	/// Whether this argument info uses ranges OR just uses traditional size.
	///
	bool Ranged;

	/// The minimum range for this argument.
	///
	int MinValue;
	
	/// The maximum range for this argument.
	///
	int MaxValue;
};

/// @brief Player state information.
///
struct MYCHAR_STATE_INFO
{
	/// The name of this player state.
	///
	char* StateName;

	/// @brief The act function for this player state.
	///
	/// @param bKey Whether key inputs should be allowed for this tick or not.
	/// @param pInterface The safe interface for this player.
	/// @param pMC The player character we should be manipulating.
	/// @param pPhysics The physics we should be using for this tick. If the player is in water, this will be pMC->physics_underwater. If not, then it will be pMC->physics_normal.
	PlayerState_ActFuncType ActFunc;

	/// @brief The animation function for this player state.
	///
	/// @param bKey Whether key inputs should be allowed for this tick or not.
	/// @param pMC The player character we should be manipulating.
	/// @param pInterface The safe interface for this player.
	///
	/// @return Should return a value corresponding to the set AnimateRetType value. This will be set as the current player animation.
	PlayerState_AnimFuncType AnimFunc;

	/// @brief The draw function for this player state.
	///
	/// @param iFrameX The camera X offset.
	/// @param iFrameY The camera Y offset.
	/// @param pMC The player character we should be manipulating.
	/// @param pInterface The interface for this player.
	PlayerState_DrawFuncType DrawFunc;

	/// The return type of the animate function.
	///
	PlayerStateAnimator_OutputType AnimateRetType;

	/// The flags for this player state.
	///
	/// @see PLAYER_STATE_FLAGS
	unsigned long long int Flags;

	/// The transmit flags for this player state.
	///
	/// @see PLAYER_STATE_TRANSMIT_FLAGS
	unsigned int TransmitFlags;

	/// The custom argument list for this player state.
	///
	MYCHAR_STATE_ARG_INFO* ArgList;

	/// Whether this was loaded / overloaded by the currently loaded mod or not.
	///
	bool LoadedByMod;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @brief Helper function for creating rects
///
/// @param x X Position
/// @param y Y Position
/// @param w Width
/// @param h Height
///
/// @return Returns a RECT defined as { x, y, x + w, y + h }
static RECT RECT_XYWH(int x, int y, int w, int h)
{
	RECT ret;
	ret.left   = x;
	ret.top    = y;
	ret.right  = x + w;
	ret.bottom = y + h;
	return ret;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

/// @brief Called every frame a weapon is equipped, for every client.
/// 
/// @param sData The shoot information.
/// @param iLevel The level of the weapon, starting from 1.
typedef void(*CSM_Weapon_ShootFunc)
(
	ShootInfo* sData,
	int iLevel
);


/// @brief Called every frame a bullet is active.
/// 
/// @param pBul A pointer to the bullet.
/// @param iLevel The level of the bullet.
typedef void(*CSM_Bullet_ActFunc)
(
	BULLET* pBul,
	int iLevel
);

/// @brief Called every frame an NPC is active.
/// 
/// @param pNpc A pointer to the NPC.
typedef void(*CSM_Npc_ActFunc)
(
	NPCHAR* pNpc
);

/// @brief Called every frame a caret is active.
/// 
/// @param caret A pointer to the caret.
typedef void(*CSM_Caret_ActFunc)
(
	CARET* caret
);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

extern CAVESTORY_MOD_API MYCHAR gMC;
extern CAVESTORY_MOD_API MYCHAR gNOT_USED;
extern CAVESTORY_MOD_API NPCHAR gNPC[NPC_MAX];
extern CAVESTORY_MOD_API CARET gCrt[CARET_MAX];
extern CAVESTORY_MOD_API int gKey;
extern CAVESTORY_MOD_API int gKeyTrg;
extern CAVESTORY_MOD_API int gKeyOld;

extern CAVESTORY_MOD_API int gKeyJump;
extern CAVESTORY_MOD_API int gKeyShot;
extern CAVESTORY_MOD_API int gKeyArms;
extern CAVESTORY_MOD_API int gKeyArmsRev;
extern CAVESTORY_MOD_API int gKeyItem;
extern CAVESTORY_MOD_API int gKeyMap;
extern CAVESTORY_MOD_API int gKeyOk;
extern CAVESTORY_MOD_API int gKeyCancel;
extern CAVESTORY_MOD_API int gKeyLeft;
extern CAVESTORY_MOD_API int gKeyUp;
extern CAVESTORY_MOD_API int gKeyRight;
extern CAVESTORY_MOD_API int gKeyDown;
extern CAVESTORY_MOD_API int gKeyMaxVal;

extern CAVESTORY_MOD_API int gKey2;
extern CAVESTORY_MOD_API int gKeyTrg2;
extern CAVESTORY_MOD_API int gKeyOld2;

extern CAVESTORY_MOD_API int gKey_JOY;
extern CAVESTORY_MOD_API int gKeyTrg_JOY;
extern CAVESTORY_MOD_API int gKeyOld_JOY;

extern CAVESTORY_MOD_API void GetTrg();

extern CAVESTORY_MOD_API int gStageNo;
extern CAVESTORY_MOD_API int gMusicNo;
extern CAVESTORY_MOD_API int g_GameFlags;

extern CAVESTORY_MOD_API TEXT_SCRIPT_PTR_DATA default_TextScriptPtrData;
extern CAVESTORY_MOD_API TEXT_SCRIPT_PTR_DATA gCurrentPtrData;
extern CAVESTORY_MOD_API TEXT_SCRIPT gTS;

extern CAVESTORY_MOD_API int gCollectableCount;
extern CAVESTORY_MOD_API bool gCollectablesShown;

extern CAVESTORY_MOD_API MAP_DATA gMap;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

#include "CSMAPI_endcode.h"