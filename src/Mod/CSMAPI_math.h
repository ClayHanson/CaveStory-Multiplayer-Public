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
  @file CSMAPI_math.h
*/

#pragma once

#include <math.h>

// Smooth moving

#define GET_INT_NORMAL(val, max)			(float(val) / float(max))
#define GET_CENTER_OFFSET(w1, w2)			((MAX((w1), (w2)) / 2) - (MIN((w1), (w2)) / 2))
#define GET_CENTER_OFFSET_FIXED(w1, w2)		(((w1) / 2) - ((w2) / 2))
#define degToRad(angleInDegrees)			((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians)			((angleInRadians) * 180.0 / M_PI)
#define M_PI_F								3.141592653589793f
#define SCROLL_BTN_AMOUNT					0.1f

static inline int __ease_linear(float t, float b, float c, float d)
{
	return int(c * t / d + b);
}

static inline int __ease_quad(float t, float b, float c, float d)
{
	t /= d / 2.f;
	if (t < 1.0) return int(c / 2.f * t * t + b);
	t--;
	return int(-c / 2.f * (t * (t - 2.f) - 1.f) + b);
}

static inline int __ease_sine(float t, float b, float c, float d)
{
	return int(-c / 2.f * (cosf(M_PI_F * t / d) - 1.f) + b);
}

static inline int __ease_expo(float t, float b, float c, float d)
{
	t /= d / 2.f;
	if (t < 1.0) return int(c / 2.f * powf(2.f, 10.f * (t - 1.f)) + b);
	t--;
	return int(c / 2.f * (-powf(2.f, -10.f * t) + 2.f) + b);
}

static inline int __ease_circ(float t, float b, float c, float d)
{
	t /= d / 2.f;
	if (t < 1.0) return int(-c / 2.f * (sqrtf(1.f - t * t) - 1.f) + b);
	t -= 2.f;
	return int(c / 2.f * (sqrtf(1.f - t * t) + 1.f) + b);
}

static inline int __ease_cube(float t, float b, float c, float d)
{
	t /= d / 2.f;
	if (t < 1.0) return int(c / 2.f * t * t * t + b);
	t -= 2.f;
	return int(c / 2.f * (t * t * t + 2.f) + b);
}

static inline int __ease_quart(float t, float b, float c, float d)
{
	t /= d / 2.f;
	if (t < 1.0) return int(c / 2.f * t * t * t * t + b);
	t -= 2.f;
	return int(-c / 2.f * (t * t * t * t - 2.f) + b);
}

#define __MOVE_FUNC(func_name, time_start, start, end, delay) (start + __ease_##func_name(float(CLAMP((GetTicks() - time_start), 0, delay)), 0.f, float(-(start - end)), float(delay)))
#define __MOVE_FUNC2(func_name, time_start, start, end, delay) (start + __ease_##func_name(float((GetTicks() - time_start) % delay), 0.f, float(-(start - end)), float(delay)))
#define MOVE_LINEAR(time_start, start, end, delay)				__MOVE_FUNC(linear, time_start, start, end, delay)
#define MOVE_QUAD(time_start, start, end, delay)				__MOVE_FUNC(quad, time_start, start, end, delay)
#define MOVE_SINE(time_start, start, end, delay)				__MOVE_FUNC(sine, time_start, start, end, delay)
#define MOVE_EXPO(time_start, start, end, delay)				__MOVE_FUNC(expo, time_start, start, end, delay)
#define MOVE_CIRC(time_start, start, end, delay)				__MOVE_FUNC(circ, time_start, start, end, delay)
#define MOVE_CUBE(time_start, start, end, delay)				__MOVE_FUNC(cube, time_start, start, end, delay)
#define MOVE_CUBE2(time_start, start, end, delay)				__MOVE_FUNC2(cube, time_start, start, end, delay)
#define MOVE_QUART(time_start, start, end, delay)				__MOVE_FUNC(quart, time_start, start, end, delay)