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