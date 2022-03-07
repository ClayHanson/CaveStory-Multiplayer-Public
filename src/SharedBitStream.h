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

class SharedBitStream
{
public:
	unsigned char* mBuffer;
	unsigned long long int mBufferLen;
	unsigned long long int mMaxBitNum;
	unsigned long long int mBitNum;
	unsigned long long int mSize;

public:
	SharedBitStream();
	~SharedBitStream();

public:

	static int GetRangedIntBits(int min, int max);

public: // Protected methods
	bool ValidateAddition(unsigned long long int bitCount);
	void Resize(unsigned long long int newSize);

public: // Write methods
	void Write(const void* bitPtr, unsigned long long int bitCount);
	void WriteInt2(void* bitPtr, unsigned long long int bitCount);
	bool WriteFlag(bool val);
	void WriteInt(int val, unsigned long long int bitCount);
	void WriteLLInt(long long int val, unsigned long long int bitCount);
	void WriteRangedInt(int val, int min, int max);
	void WriteRangedLLInt(long long int val, long long int min, long long int max);
	void WriteBranchingInt(int val, int min, int max);
	void WriteString(const char* str, int max = 256);
	void WriteString(const wchar_t* str, int max = 256);

public: // Read methods
	void Read(void* bitPtr, unsigned long long int bitCount);
	void ReadInt2(void* bitPtr, unsigned long long int bitCount);
	bool ReadFlag();
	int ReadInt(unsigned long long int bitCount);
	long long int ReadIntLL(unsigned long long int bitCount);
	int ReadRangedInt(int min, int max);
	long long int ReadRangedLLInt(long long int min, long long int max);
	int ReadBranchingInt(int min, int max);
	void ReadAllocateString(char*& out, int max = 256);
	void ReadAllocateString(wchar_t*& out, int max = 256);
	void ReadString(char* out, int max = 256);
	void ReadString(wchar_t* out, int max = 256);

public: // Other methods
	void SetBuffer(char* buffer, unsigned long long int buffer_len);
	void Clear();

public: // File methods
	bool LoadFromFile(const char* pFileName, bool bLargeFile = false);
	bool SaveToFile(const char* pFileName, bool bLargeFile = false);
	bool FileLoad(void* pFile, bool bLargeFile = false);
	bool FileSave(void* pFile, bool bLargeFile = false);

public:
	inline unsigned long long int GetCurPos() { return mBitNum; }
	inline void SetCurPos(unsigned long long int x) { mBitNum = x; }
};