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

#include "SharedBitStream.h"
#include "ExtraMath.h"
#include <memory>

#ifdef _CHAR_EDITOR
#include "../CaveStoryCharacterEditor/framework.h"
#include "afxdialogex.h"
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef CLAMP
#define CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructors & deconstructors
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SharedBitStream::SharedBitStream()
{
	mBufferLen = 0;
	mMaxBitNum = 0;
	mBuffer    = NULL;
	mBitNum    = 0;
	mSize      = 0;
}

SharedBitStream::~SharedBitStream()
{
	if (mBuffer)
		free(mBuffer);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool SharedBitStream::ValidateAddition(unsigned long long int byteCount)
{
	if (!byteCount)
		byteCount = 1;

	if (!mBuffer)
	{
		// Allocate a new packet
		Resize(byteCount);

		return mBuffer != NULL;
	}
	else if (mBufferLen + byteCount >= mSize - 1)
	{
		// Resize current packet
		Resize(mBufferLen + byteCount);

		// Return if it worked
		return (mBufferLen + byteCount < mSize);
	}

	return true;
}

void SharedBitStream::Resize(unsigned long long int newSize)
{
	newSize += 1;

	// If we're already this size, then just stop.
	if (newSize == mSize)
		return;

	int iStart = mSize;

	// Do stuff with the old packet if it exists
	if (mBuffer)
	{
		// Resize the old buffer
		mSize   = newSize;
		mBuffer = (unsigned char*)realloc((void*)mBuffer, mSize);
	}
	else
	{
		// Create a new buffer
		mSize      = newSize;
		mBuffer    = (unsigned char*)malloc(mSize);
		mBufferLen = 0;
	}

	if (mSize > iStart)
		memset(mBuffer + iStart, 0, mSize - iStart);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int SharedBitStream::GetRangedIntBits(int min, int max)
{
	return getBinLog2(getNextPow2(max - min + 1));
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SharedBitStream::Write(const void* bitPtr, unsigned long long int bitCount)
{
	//if (!ValidateAddition(((bitCount + mBitNum - 1) >> 3) - (mBitNum >> 3)))
	if (!ValidateAddition(((bitCount + mBitNum - 1ull) >> 3ull) - (mBitNum >> 3ull)))
		return;

	if (mBitNum != mMaxBitNum)
	{
		// Starting ptr
		const unsigned char* ptr = (unsigned char*)bitPtr;

		// Write the bits
		for (unsigned long long int srcBitNum = 0; srcBitNum < bitCount; srcBitNum++)
		{
			if ((*(ptr + (srcBitNum >> 3ull)) & (1 << (srcBitNum & 0x7ull))) != 0)
				*(mBuffer + (mBitNum >> 3ull)) |= (1 << (mBitNum & 0x7ull));
			else
				*(mBuffer + (mBitNum >> 3ull)) &= ~(1 << (mBitNum & 0x7ull));

			mBitNum++;
		}

		// Set maxes
		if (mBitNum > mMaxBitNum)
			mMaxBitNum = mBitNum;

		if (((mBitNum + 7ull) >> 3ull) > mBufferLen)
			mBufferLen = ((mBitNum + 7ull) >> 3ull);

		return;
	}

	// Calculate the start & end points for what we're going to read
	const unsigned char* ptr = (unsigned char*)bitPtr;
	unsigned char* stPtr     = mBuffer + (mBitNum >> 3ull);
	unsigned char* endPtr    = mBuffer + ((bitCount + mBitNum - 1ull) >> 3ull);

	// Calculate masks & shifts
	signed long long int upShift   = mBitNum & 0x7ull;
	signed long long int downShift = 8ull - upShift;
	unsigned char lastMask         = 0xFFull >> (7ull - ((mBitNum + bitCount - 1ull) & 0x7ull));
	unsigned char startMask        = 0xFFull >> downShift;

	// Read the first byte
	unsigned char curB = *ptr++;
	*stPtr             = (curB << upShift) | (*stPtr & startMask);

	// Read everything else
	stPtr++;
	while (stPtr <= endPtr)
	{
		unsigned char nextB = *ptr++;
		*stPtr++            = (curB >> downShift) | (nextB << upShift);
		curB                = nextB;
	}

	// Done. Apply the last mask and finish up
	*endPtr &= lastMask;
	mBitNum += bitCount;

	if (((mBitNum + 7ull) >> 3ull) > mBufferLen)
		mBufferLen = ((mBitNum + 7ull) >> 3ull);

	if (mBitNum > mMaxBitNum)
		mMaxBitNum = mBitNum;
}

void SharedBitStream::WriteInt2(void* bitPtr, unsigned long long int bitCount)
{
	int val = 0;
	memcpy(&val, bitPtr, (bitCount + 7ull) >> 3ull);

	if (bitCount == 1)
	{
		WriteFlag(val);
		return;
	}

	val = convertHostToLEndian(val);
	Write(&val, bitCount);
}

bool SharedBitStream::WriteFlag(bool val)
{
	if (!ValidateAddition(((1ull + mBitNum - 1ull) >> 3ull) - (mBitNum >> 3ull)))
		return false;

	if ((mBitNum + 1ull >> 3ull) > (mBitNum >> 3ull) && !ValidateAddition(1))
		return val;

	// Manipulate the current bit
	if (val) *(mBuffer + (mBitNum >> 3ull)) |= (1 << (mBitNum & 0x7));
	else *(mBuffer + (mBitNum >> 3ull)) &= ~(1 << (mBitNum & 0x7));

	// Add to our total bit count
	mBitNum++;
	mBufferLen = ((mBitNum + 7ull) >> 3ull);

	// Done
	return val;
}

void SharedBitStream::WriteInt(int val, unsigned long long int bitCount)
{
	val = convertHostToLEndian(val);
	Write(&val, bitCount);
}

void SharedBitStream::WriteLLInt(long long int val, unsigned long long int bitCount)
{
	val = convertHostToLEndian(val);
	Write(&val, bitCount);
}

void SharedBitStream::WriteRangedInt(int val, int min, int max)
{
#ifdef _DEBUG
	if (val < min || val > max)
	{
		printf("fixme: " __FUNCTION__ "(%d, %d, %d) - val is out of range!\r\n", val, min, max);
	}
#endif

	val  = CLAMP(val, min, max);
	val -= min;

	if (min == max)
		return;

	WriteInt(val, getBinLog2(getNextPow2(max - min + 1)));
}

void SharedBitStream::WriteRangedLLInt(long long int val, long long int min, long long int max)
{
#ifdef _DEBUG
	if (val < min || val > max)
	{
		printf("fixme: " __FUNCTION__ "(%I64d, %I64d, %I64d) - val is out of range!\r\n", val, min, max);
	}
#endif

	val  = CLAMP(val, min, max);
	val -= min;

	if (min == max)
		return;

	val = convertHostToLEndian(val);
	Write(&val, getBinLog2(getNextPow2(max - min + 1)));
}

void SharedBitStream::WriteBranchingInt(int iNum, int iMin, int iMax)
{
#ifdef _DEBUG
	if (iMax < iMin || iMin > iMax)
		printf("fixme: " __FUNCTION__ "(%d, %d, %d) - min + max are invalid!\r\n", iNum, iMin, iMax);
	else if (iNum < iMin || iNum > iMax)
		printf("fixme: " __FUNCTION__ "(%d, %d, %d) - val is out of range!\r\n", iNum, iMin, iMax);
#endif
	
	int iLeft  = getPrevPow2(iMin);
	int iRight = getNextPow2(iMax);
	int iHalf  = 0;
	int iDebug = 0;

	while (++iDebug < 1024)
	{	
		iHalf = (iRight + iLeft) / 2;
		
		if (iNum < iHalf)
		{
			// Go left
			iRight = iHalf - 1;
			WriteFlag(0);
		}
		else if (iNum > iHalf)
		{
			// Go right
			iLeft = iHalf + 1;
			WriteFlag(1);
		}
		else
			break;
	}
}

void SharedBitStream::WriteString(const char* string, int max)
{
	size_t dLen = (size_t)strlen(string);

	// Limit strings to 255
	if (dLen >= max)
		dLen = max;

	// Write the length
	WriteRangedInt(dLen, 0, max);

	// Write the actual string
	if (dLen)
		Write((void*)string, dLen << 3);
}

void SharedBitStream::WriteString(const wchar_t* string, int max)
{
	size_t dLen = (size_t)wcslen(string);

	// Limit strings to 255
	if (dLen >= max)
		dLen = max;

	// Write the length
	WriteRangedInt(dLen, 0, max);

	// Write the actual string
	if (dLen)
		Write((void*)string, (sizeof(wchar_t) * dLen) << 3);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SharedBitStream::Read(void* bitPtr, unsigned long long int bitCount)
{
	if (mMaxBitNum && bitCount && mBitNum + bitCount >= mMaxBitNum)
		bitCount -= ((mBitNum + bitCount) - mMaxBitNum);

	if (!bitCount)
		return;

	if (!bitPtr)
	{
		mBitNum += bitCount;
		return;
	}

	// Calculate the start & end points for what we're going to read
	unsigned char* stPtr           = mBuffer + (mBitNum >> 3ull);
	signed long long int byteCount = (bitCount + 7ull) >> 3ull;
	unsigned char* ptr             = (unsigned char*)bitPtr;

	// Calculate the shifts
	signed long long int downShift = mBitNum & 0x7ull;
	signed long long int upShift   = 8ull - downShift;

	// Read the bytes
	unsigned char curB = *stPtr;
	while (byteCount--)
	{
		unsigned char nextB = *++stPtr;
		*ptr++              = (curB >> downShift) | (nextB << upShift);
		curB                = nextB;
	}

	// Done.
	mBitNum += bitCount;
}

void SharedBitStream::ReadInt2(void* bitPtr, unsigned long long int bitCount)
{
	int ret = 0;

	if (bitCount == 1)
	{
		ret = ReadFlag();
		memcpy(bitPtr, &ret, (bitCount + 7) >> 3);
		return;
	}

	Read(&ret, bitCount);
	ret = convertLEndianToHost(ret);
	if (bitCount != 32)
		ret &= (1 << bitCount) - 1;

	memcpy(bitPtr, &ret, (bitCount + 7) >> 3);
}

bool SharedBitStream::ReadFlag()
{
	// Calculate the mask, and then get the flag
	signed int mask = 1 << (mBitNum & 0x7);
	bool ret        = (*(mBuffer + (mBitNum >> 3)) & mask) != 0;

	// Increment our read bit
	mBitNum++;

	// Done
	return ret;
}

int SharedBitStream::ReadInt(unsigned long long int bitCount)
{
	int ret = 0;
	Read(&ret, bitCount);
	ret = convertLEndianToHost(ret);
	if (bitCount == 32)
		return ret;
	else
		ret &= (1 << bitCount) - 1;
	return ret;
}

long long int SharedBitStream::ReadIntLL(unsigned long long int bitCount)
{
	long long int ret = 0;
	Read(&ret, bitCount);
	ret = convertLEndianToHost(ret);

	return ret;
}

int SharedBitStream::ReadRangedInt(int min, int max)
{
	if (min == max)
		return 0;

	//int ret = ReadInt(getBinLog2(getNextPow2(max - min + 1)));
	//return ret + min;

	int ret = ReadInt(getBinLog2(getNextPow2(max - min + 1))) + min;

	// Make absolutely SURE that this will return a validly ranged number
	return CLAMP(ret, min, max);
}

long long int SharedBitStream::ReadRangedLLInt(long long int min, long long int max)
{
	if (min == max)
		return 0;

	long long int ret = 0;
	Read(&ret, getBinLog2(getNextPow2(max - min + 1)));
	ret = convertLEndianToHost(ret);
	//if (getBinLog2(getNextPow2(max - min + 1)) != 32)
		//ret &= (1 << getBinLog2(getNextPow2(max - min + 1))) - 1;
	
	ret += min;

	return CLAMP(ret, min, max);
}

int SharedBitStream::ReadBranchingInt(int iMin, int iMax)
{
	int iLeft  = getPrevPow2(iMin);
	int iRight = getNextPow2(iMax);
	int iDebug = 0;

	while (++iDebug < 1024)
	{
		bool iBranch = ReadFlag();
		int iHalf    = (iRight + iLeft) / 2;

		if (!iBranch)
		{
			// Went left
			iRight = iHalf - 1;
		}
		else
		{
			// Went right
			iLeft = iHalf + 1;
		}
	}

	// Done
	return (iRight + iLeft) / 2;
}

void SharedBitStream::ReadAllocateString(char*& out, int max)
{
	// Read the length 
	size_t dLen = ReadRangedInt(0, max);

	// Read the entire string into the out buffer
	if (dLen)
	{
		out = (char*)malloc(sizeof(char) * (dLen + 1));
		Read(out, dLen << 3);
	}
	else
	{
		out = NULL;
		return;
	}

	// Safety: auto null-terminate
	if (out[dLen])
		out[dLen] = 0;
}

void SharedBitStream::ReadAllocateString(wchar_t*& out, int max)
{
	// Read the length 
	size_t dLen = ReadRangedInt(0, max);

	// Read the entire string into the out buffer
	if (dLen)
	{
		out = (wchar_t*)malloc(sizeof(wchar_t) * (dLen + 1));
		Read(out, (sizeof(wchar_t) * dLen) << 3);
	}
	else
		*out = 0;

	// Safety: auto null-terminate
	if (out[dLen])
		out[dLen] = 0;
}

void SharedBitStream::ReadString(char* out, int max)
{
	// Read the length 
	size_t dLen = ReadRangedInt(0, max);

	// Read the entire string into the out buffer
	if (dLen)
		Read(out, dLen << 3);
	else if (out)
		*out = 0;

	// Safety: auto null-terminate
	if (out && out[dLen])
		out[dLen] = 0;
}

void SharedBitStream::ReadString(wchar_t* out, int max)
{
	// Read the length 
	size_t dLen = ReadRangedInt(0, max);

	// Read the entire string into the out buffer
	if (dLen)
		Read(out, (sizeof(wchar_t) * dLen) << 3);
	else
		*out = 0;

	// Safety: auto null-terminate
	if (out[dLen])
		out[dLen] = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SharedBitStream::SetBuffer(char* buffer, unsigned long long int buffer_len)
{
	Clear();

	ValidateAddition(buffer_len);

	memcpy(mBuffer, buffer, buffer_len);

	mBitNum    = 0ull;
	mMaxBitNum = mBufferLen << 3ull;
	mBufferLen = buffer_len;
}

void SharedBitStream::Clear()
{
	if (mBuffer)
	{
		// Free the packet
		free(mBuffer);
		mBuffer = NULL;
	}

	// Reset all the variables
	mMaxBitNum = 0;
	mBufferLen = 0;
	mBitNum    = 0;
	mSize      = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool SharedBitStream::LoadFromFile(const char* pFileName, bool bLargeFile)
{
	FILE* fp = NULL;

	// Make sure we load the file
	if ((fp = fopen(pFileName, "rb")) == NULL)
	{
		printf("ERROR: " __FUNCTION__ "() - Failed to open file \"%s\"\r\n", pFileName);
		return false;
	}

	// Load file
	FileLoad(fp, bLargeFile);

	// Close the file
	fclose(fp);

	return true;
}

bool SharedBitStream::SaveToFile(const char* pFileName, bool bLargeFile)
{
	FILE* fp;

	// Attempt to open the file for writing
	if ((fp = fopen(pFileName, "wb")) == NULL)
	{
		printf("ERROR: " __FUNCTION__ "() - Failed to open file \"%s\"\r\n", pFileName);
		return false;
	}

	// Write the data
	FileSave(fp, bLargeFile);

	// We're finished with the file, so close it.
	fclose(fp);

	// Done
	return true;
}

bool SharedBitStream::FileLoad(void* pFile, bool bLargeFile)
{
	unsigned long long int FullSize = 0;

	if (bLargeFile)
	{
		fread(&FullSize, sizeof(unsigned long long int), 1, (FILE*)pFile);
		FullSize = convertLEndianToHost(FullSize);
	}
	else
	{
		// Go to the end of the file
		fseek((FILE*)pFile, 0, SEEK_END);

		// Get the length of the file
		FullSize = ftell((FILE*)pFile);

		// Go back to the mStart of the file
		fseek((FILE*)pFile, 0, SEEK_SET);
	}

	// Allocate the bitstream
	this->Clear();
	this->Resize(FullSize);

	// Read the entire file into the buffer
	fread(this->mBuffer, FullSize, 1, (FILE*)pFile);
	this->mBufferLen = FullSize;

	// Reset the cursor
	this->SetCurPos(0);

	return true;
}

bool SharedBitStream::FileSave(void* pFile, bool bLargeFile)
{
	// Write the data
	if (bLargeFile)
	{
		unsigned long long int iBufferLen = convertHostToLEndian(this->mBufferLen);

		// Write the size of the buffer
		fwrite(&iBufferLen, sizeof(unsigned long long int), 1, (FILE*)pFile);
	}

	fwrite(this->mBuffer, this->mBufferLen, 1, (FILE*)pFile);

	return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------