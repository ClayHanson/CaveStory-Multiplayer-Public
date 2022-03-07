//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------
// All modifications by Johnny Ledger ("Clay Hanson")
//-----------------------------------------------------------------------------

#pragma once

#include <math.h>

inline float dist(int x1, int y1, int x2, int y2)
{
	// Calculating distance 
	return sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2) * 1.f);
}

inline int getClosestNumber(int num, int mult)
{
	// find the quotient 
	int q = num / mult;

	// 1st possible closest number 
	int n1 = mult * q;

	// 2nd possible closest number 
	int n2 = (num * mult) > 0 ? (mult * (q + 1)) : (mult * (q - 1));

	// if true, then n1 is the required closest number 
	if (abs(num - n1) < abs(num - n2))
		return n1;

	// else n2 is the required closest number     
	return n2;
}

inline unsigned int getNextPow2(unsigned int num)
{
	num--;
	num |= num >> 1;
	num |= num >> 2;
	num |= num >> 4;
	num |= num >> 8;
	num |= num >> 16;
	num++;

	return num;
}

inline unsigned int getPrevPow2(unsigned int num)
{
	num = num | (num >> 1);
	num = num | (num >> 2);
	num = num | (num >> 4);
	num = num | (num >> 8);
	num = num | (num >> 16);

	return num - (num >> 1);
}

inline unsigned int getBinLog2(unsigned int num, bool knownPow2 = false)
{
	static const unsigned int MultiplyDeBruijnBitPosition[32] =
	{
	   0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
	   31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};

	if (!knownPow2) {
		num |= num >> 1; // first round down to power of 2 
		num |= num >> 2;
		num |= num >> 4;
		num |= num >> 8;
		num |= num >> 16;
		num = (num >> 1) + 1;
	}

	return MultiplyDeBruijnBitPosition[(num * 0x077CB531UL) >> 27];
}

// Little & big endian

inline unsigned char endianSwap(const unsigned char in_swap)
{
	return in_swap;
}

inline signed char endianSwap(const signed char in_swap)
{
	return in_swap;
}

inline unsigned short endianSwap(const unsigned short in_swap)
{
	return unsigned short(((in_swap >> 8) & 0x00ff) |
		((in_swap << 8) & 0xff00));
}

inline signed short endianSwap(const signed short in_swap)
{
	return unsigned short(endianSwap(unsigned short(in_swap)));
}

inline unsigned int endianSwap(const unsigned int in_swap)
{
	return unsigned int(((in_swap >> 24) & 0x000000ff) |
		((in_swap >> 8) & 0x0000ff00) |
		((in_swap << 8) & 0x00ff0000) |
		((in_swap << 24) & 0xff000000));
}

inline signed int endianSwap(const signed int in_swap)
{
	return int(endianSwap(unsigned int(in_swap)));
}

inline unsigned long long int endianSwap(const unsigned long long int in_swap)
{
	unsigned long long int value = in_swap;
	value = ((value & 0x00000000FFFFFFFFull) << 32) | ((value & 0xFFFFFFFF00000000ull) >> 32);
	value = ((value & 0x0000FFFF0000FFFFull) << 16) | ((value & 0xFFFF0000FFFF0000ull) >> 16);
	value = ((value & 0x00FF00FF00FF00FFull) << 8) | ((value & 0xFF00FF00FF00FF00ull) >> 8);
	return unsigned long long int(in_swap);
}

inline signed long long int endianSwap(const signed long long int in_swap)
{
	return (long long int)(endianSwap((unsigned long long int)in_swap));
}

inline signed long endianSwap(const signed long in_swap)
{
	unsigned int* inp = (unsigned int*)& in_swap;
	unsigned long ret;
	unsigned int* outp = (unsigned int*)& ret;
	outp[0] = endianSwap(inp[1]);
	outp[1] = endianSwap(inp[0]);
	return ret;
}

inline unsigned long endianSwap(const unsigned long in_swap)
{
	return long(endianSwap(unsigned long(in_swap)));
}

inline float endianSwap(const float in_swap)
{
	unsigned int result = endianSwap(*((unsigned int*)& in_swap));
	return *((float*)& result);
}

inline double endianSwap(const double in_swap)
{
	unsigned long result = endianSwap(*((unsigned long*)& in_swap));
	return *((double*)& result);
}

#define DECLARE_TEMPLATIZED_ENDIAN_CONV(type) \
   inline type convertHostToLEndian(type i) { return i; } \
   inline type convertLEndianToHost(type i) { return i; } \
   inline type convertHostToBEndian(type i) { return endianSwap(i); } \
   inline type convertBEndianToHost(type i) { return endianSwap(i); }


DECLARE_TEMPLATIZED_ENDIAN_CONV(unsigned long long int)
DECLARE_TEMPLATIZED_ENDIAN_CONV(long long int)
DECLARE_TEMPLATIZED_ENDIAN_CONV(unsigned char)
DECLARE_TEMPLATIZED_ENDIAN_CONV(signed char)
DECLARE_TEMPLATIZED_ENDIAN_CONV(unsigned short)
DECLARE_TEMPLATIZED_ENDIAN_CONV(signed short)
DECLARE_TEMPLATIZED_ENDIAN_CONV(unsigned int)
DECLARE_TEMPLATIZED_ENDIAN_CONV(signed int)
DECLARE_TEMPLATIZED_ENDIAN_CONV(unsigned long)
DECLARE_TEMPLATIZED_ENDIAN_CONV(signed long)
DECLARE_TEMPLATIZED_ENDIAN_CONV(float)
DECLARE_TEMPLATIZED_ENDIAN_CONV(double)