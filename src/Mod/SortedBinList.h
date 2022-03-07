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
  @file SortedBinList.h
*/


#ifndef _SORTED_BIN_LIST_H_
#define _SORTED_BIN_LIST_H_

#ifndef LIST_INSERT
#define LIST_INSERT(LISTV, LISTC, LISTTYPE, INDEX)		memmove(&LISTV[INDEX + 1], &LISTV[INDEX], (LISTC - INDEX - 1) * sizeof(LISTTYPE))
#endif
#ifndef LIST_ERASE
#define LIST_ERASE(LISTV, LISTC, LISTTYPE, INDEX)		memmove(&LISTV[INDEX], &LISTV[INDEX + 1], (LISTC - INDEX - 1) * sizeof(LISTTYPE))
#endif

#include <memory>

template<class T, int LISTSIZE>
class SortedBinList
{
public:
	T mList[LISTSIZE];
	int mListCount;

public:
	SortedBinList()
	{
		mListCount = 0;
	}

public:
	/// Returns -1 when value couldn't be found.
	inline int FindIndex(const T& value)
	{
		if (!mListCount)                    return -1;
		if (mList[0] == value)              return 0;
		if (mList[mListCount - 1] == value) return (mListCount - 1);

		int L = 0;
		int R = mListCount - 1;
		int m = 0;
		while (L < R)
		{
			m = int(floorf(float(L + R) / 2.f));
			if (mList[m] < value)
				L = m + 1;
			else if (mList[m] > value)
				R = m - 1;
			else
				return m;
		}

		// Try the alternative method
		L = 0;
		R = mListCount - 1;
		while (L != R)
		{
			m = int(ceil(float(L + R) / 2.f));
			if (mList[m] > value)
				R = m - 1;
			else
				L = m;
		}

		if (mList[L] == value)
			return L;

		return -1;
	}

	inline bool IsInArray(const T& value)
	{
		return (FindIndex(value) != -1);
	}

	/// Self-explanatory.
	inline void Clear()
	{
		mListCount = 0;
	}

	/// Returns less-than-zero value on error; Otherwise, no error.
	/// -1 : Over capacity
	inline int Insert(const T& value)
	{
		int iIndex = 0;

		// Found the index. Just return it.
		if ((iIndex = FindIndex(value)) != -1)
			return iIndex;

		// Don't insert over capacity
		if (mListCount + 1 >= LISTSIZE)
			return -1;

		// Find where to insert it
		if (mListCount == 0 || value >= mList[mListCount - 1])
		{
			// Insert at the back of the array
			mList[mListCount] = value;
			mListCount++;

			// Done
			return (mListCount - 1);
		}

		if (value <= mList[0])
		{
			// Insert at beginning of array
			mListCount++;
			LIST_INSERT(mList, mListCount, T, 0);
			mList[0] = value;

			// Done
			return 0;
		}

		// Determine where to insert this element at
		int i;
		for (i = mListCount; i >= 0 && mList[i - 1] > value; i--);

		// Insert at array
		mListCount++;
		LIST_INSERT(mList, mListCount, T, i);
		mList[i] = value;

		// Done
		return i;
	}

	/// Returns less-than-zero value on error.
	/// -1 : Not in array
	/// -2 : Array is empty
	inline int Remove(const T& value)
	{
		if (!mListCount)
			return -2;

		int INDEX = 0;
		if ((INDEX = FindIndex(value)) == -1)
			return -1;

		LIST_ERASE(mList, mListCount, T, INDEX);
		mListCount--;

		return INDEX;
	}

	/// Returns less-than-zero value on error.
	/// -1 : Array is empty
	/// -2 : Index out of range
	inline int RemoveIndex(int index)
	{
		if (!mListCount)
			return -1;

		if (index < 0 || index >= mListCount)
			return -2;

		LIST_ERASE(mList, mListCount, T, index);
		mListCount--;

		return index;
	}

	inline int Size()
	{
		return mListCount;
	}

	inline int Capacity()
	{
		return LISTSIZE;
	}

public:
	inline operator T*& () { return mList; }
	inline operator T* () { return mList; }
	inline int operator[](int idx) { return mList[idx]; }
};

//---------------

template<class T, int ALLOC_STEP_AMT = 4, bool FORCE_UNIQUE_VALUES = true>
class DynamicSortedBinList
{
public:
	T* mList;
	int mListSize;
	int mListCount;

public:
	DynamicSortedBinList()
	{
		mList      = NULL;
		mListSize  = 0;
		mListCount = 0;
	}
	~DynamicSortedBinList()
	{
		if (mList)
			free(mList);
	}

public:
	/// Returns -1 when value couldn't be found.
	inline int FindIndex(const T& value)
	{
		if (!mListCount)                    return -1;
		if (mList[0] == value)              return 0;
		if (mList[mListCount - 1] == value) return (mListCount - 1);

		int L = 0;
		int R = mListCount - 1;
		int m = 0;
		while (L < R)
		{
			m = int(floorf(float(L + R) / 2.f));
			if (mList[m] < value)
				L = m + 1;
			else if (mList[m] > value)
				R = m - 1;
			else
				return m;
		}

		// Try the alternative method
		L = 0;
		R = mListCount - 1;
		while (L != R)
		{
			m = int(ceil(float(L + R) / 2.f));
			if (mList[m] > value)
				R = m - 1;
			else
				L = m;
		}

		if (mList[L] == value)
			return L;

		return -1;
	}

	inline bool IsInArray(const T& value)
	{
		return (FindIndex(value) != -1);
	}

	/// Self-explanatory.
	inline void Clear()
	{
		mListCount = 0;
	}

	/// Self-explanatory.
	inline void Free()
	{
		mListCount = 0;
		mListSize  = 0;

		if (mList)
		{
			free(mList);
			mList = NULL;
		}
	}

	/// Returns the index at which the value resides at.
	inline int Insert(const T& value, bool error_if_found = false)
	{
		int iIndex = 0;

		if (FORCE_UNIQUE_VALUES && (iIndex = FindIndex(value)) != -1)
			return (error_if_found ? -1 : iIndex);

		// Make room for more if we have to
		if (!mList || mListCount + 1 >= mListSize)
		{
			if (!mList)
			{
				mListSize += ALLOC_STEP_AMT;
				mList      = (T*)malloc(sizeof(T) * mListSize);
			}
			else
			{
				mListSize += ALLOC_STEP_AMT;
				mList      = (T*)realloc((void*)mList, sizeof(T) * mListSize);
			}
		}

		// Find where to insert it
		if (mListCount == 0 || (const T)value >= (const T)mList[mListCount - 1])
		{
			// Insert at the back of the array
			memcpy(&mList[mListCount], &value, sizeof(T));
			mListCount++;

			// Done
			return (mListCount - 1);
		}

		if ((const T)value <= (const T)mList[0])
		{
			// Insert at beginning of array
			mListCount++;
			LIST_INSERT(mList, mListCount, T, 0);
			memcpy(&mList[0], &value, sizeof(T));

			// Done
			return 0;
		}

		// Determine where to insert this element at
		int i;
		for (i = mListCount; i >= 0 && (const T)mList[i - 1] > (const T)value; i--);

		// Insert at array
		mListCount++;
		LIST_INSERT(mList, mListCount, T, i);
		memcpy(&mList[i], &value, sizeof(T));

		// Done
		return i;
	}

	/// Returns less-than-zero value on error.
	/// -1 : Not in array
	/// -2 : Array is empty
	inline int Remove(const T& value)
	{
		if (!mListCount)
			return -2;

		int INDEX = 0;
		if ((INDEX = FindIndex(value)) == -1)
			return -1;

		LIST_ERASE(mList, mListCount, T, INDEX);
		mListCount--;

		return INDEX;
	}

	/// Returns less-than-zero value on error.
	/// -1 : Array is empty
	/// -2 : Index out of range
	inline int RemoveIndex(int index)
	{
		if (!mListCount)
			return -1;

		if (index < 0 || index >= mListCount)
			return -2;

		LIST_ERASE(mList, mListCount, T, index);
		mListCount--;

		return index;
	}

	/// Shrink the array's list.
	inline void Shrink()
	{
		if (!mList || mListSize == mListCount)
			return;

		if (mListCount == 0)
		{
			free((void*)mList);
			mListSize = 0;
			mList     = NULL;

			return;
		}

		mList     = (T*)realloc((void*)mList, sizeof(T) * mListCount);
		mListSize = mListCount;
	}

	/// Resort the list contents.
	inline void Resort()
	{
		DynamicSortedBinList<T, ALLOC_STEP_AMT, false> TemporaryList;

		// Inesrt into temp list
		for (int i = 0; i < mListCount; i++)
			TemporaryList.Insert(mList[i]);

		// Clear our own list
		Clear();

		// Re-insert back into ourselves
		for (int i = 0; i < TemporaryList.mListCount; i++)
			Insert(TemporaryList.mList[i]);

		TemporaryList.Clear();
		TemporaryList.Shrink();
	}

	/// Resort the list contents.
	inline void FastResort()
	{
		// Just sort. Lol
		T pTmp;

		for (int i = 1; i < mListCount; i++)
		{
			if (mList[i - 1] > mList[i])
			{
				// Store the temporary variable
				memcpy(&pTmp, &mList[i], sizeof(T));

				// Swap places
				memcpy(&mList[i], &mList[i - 1], sizeof(T));
				memcpy(&mList[i - 1], &pTmp, sizeof(T));

				// Restart at the beginning of the array
				i = 0;
			}
		}
	}

	inline int Size()
	{
		return mListCount;
	}

	inline int Capacity()
	{
		return mListSize;
	}

public:
	inline operator T*& () { return mList; }
	inline operator T* () { return mList; }
	inline T* operator[](int idx) { return &mList[idx]; }
};

//---------------

template<class T, int ALLOC_STEP_AMT = 4, bool FORCE_UNIQUE_VALUES = true>
class DynamicSortedPointerBinList
{
public:
	T* mList;
	int mListSize;
	int mListCount;

public:
	DynamicSortedPointerBinList()
	{
		mList      = NULL;
		mListSize  = 0;
		mListCount = 0;
	}
	~DynamicSortedPointerBinList()
	{
		if (mList)
			free(mList);
	}

public:
	/// Returns -1 when value couldn't be found.
	inline int FindIndex(const T& value)
	{
		if (!mListCount)				          return -1;
		if ((*mList[0]) == (*value))              return 0;
		if ((*mList[mListCount - 1]) == (*value)) return (mListCount - 1);

		int L = 0;
		int R = mListCount - 1;
		int m = 0;
		while (L < R)
		{
			m = int(floorf(float(L + R) / 2.f));
			if (*mList[m] < *value)
				L = m + 1;
			else if (*mList[m] > *value)
				R = m - 1;
			else
				return m;
		}

		// Try the alternative method
		L = 0;
		R = mListCount - 1;
		while (L != R)
		{
			m = int(ceil(float(L + R) / 2.f));
			if (*mList[m] > * value)
				R = m - 1;
			else
				L = m;
		}

		if (*mList[L] == *value)
			return L;

		return -1;
	}

	inline bool IsInArray(const T& value)
	{
		return (FindIndex(value) != -1);
	}

	/// Self-explanatory.
	inline void Clear()
	{
		mListCount = 0;
	}

	/// Self-explanatory.
	inline void Free()
	{
		mListCount = 0;
		mListSize  = 0;

		if (mList)
		{
			free(mList);
			mList = NULL;
		}
	}

	/// Returns the index at which the value resides at.
	inline int Insert(const T& value, bool error_if_found = false)
	{
		int iIndex = 0;

		if (FORCE_UNIQUE_VALUES && (iIndex = FindIndex(value)) != -1)
			return (error_if_found ? -1 : iIndex);

		// Make room for more if we have to
		if (!mList || mListCount + 1 >= mListSize)
		{
			if (!mList)
			{
				mListSize += ALLOC_STEP_AMT;
				mList      = (T*)malloc(sizeof(T) * mListSize);
			}
			else
			{
				mListSize += ALLOC_STEP_AMT;
				mList      = (T*)realloc((void*)mList, sizeof(T) * mListSize);
			}
		}

		// Find where to insert it
		if (mListCount == 0 || *((const T)value) >= *((const T)mList[mListCount - 1]))
		{
			// Insert at the back of the array
			memcpy(&mList[mListCount], &value, sizeof(T));
			mListCount++;

			// Done
			return (mListCount - 1);
		}

		if (*((const T)value) <= *((const T)mList[0]))
		{
			// Insert at beginning of array
			mListCount++;
			LIST_INSERT(mList, mListCount, T, 0);
			memcpy(&mList[0], &value, sizeof(T));

			// Done
			return 0;
		}

		// Determine where to insert this element at
		int i;
		for (i = mListCount; i >= 0 && *((const T)mList[i - 1]) > *((const T)value); i--);

		// Insert at array
		mListCount++;
		LIST_INSERT(mList, mListCount, T, i);
		memcpy(&mList[i], &value, sizeof(T));

		// Done
		return i;
	}

	/// Returns less-than-zero value on error.
	/// -1 : Not in array
	/// -2 : Array is empty
	inline int Remove(const T& value)
	{
		if (!mListCount)
			return -2;

		int INDEX = 0;
		if ((INDEX = FindIndex(value)) == -1)
			return -1;

		LIST_ERASE(mList, mListCount, T, INDEX);
		mListCount--;

		return INDEX;
	}

	/// Returns less-than-zero value on error.
	/// -1 : Array is empty
	/// -2 : Index out of range
	inline int RemoveIndex(int index)
	{
		if (!mListCount)
			return -1;

		if (index < 0 || index >= mListCount)
			return -2;

		LIST_ERASE(mList, mListCount, T, index);
		mListCount--;

		return index;
	}

	/// Shrink the array's list.
	inline void Shrink()
	{
		if (!mList || mListSize == mListCount)
			return;

		if (mListCount == 0)
		{
			free((void*)mList);
			mListSize = 0;
			mList     = NULL;

			return;
		}

		mList     = (T*)realloc((void*)mList, sizeof(T) * mListCount);
		mListSize = mListCount;
	}

	/// Resort the list contents.
	inline void Resort()
	{
		DynamicSortedPointerBinList<T, ALLOC_STEP_AMT, false> TemporaryList;

		// Inesrt into temp list
		for (int i = 0; i < mListCount; i++)
			TemporaryList.Insert(mList[i]);

		// Clear our own list
		Clear();

		// Re-insert back into ourselves
		for (int i = 0; i < TemporaryList.mListCount; i++)
			Insert(TemporaryList.mList[i]);

		TemporaryList.Clear();
		TemporaryList.Shrink();
	}

	/// Resort the list contents.
	inline void FastResort()
	{
		// Just sort. Lol
		T pTmp;

		for (int i = 1; i < mListCount; i++)
		{
			if (mList[i - 1] > mList[i])
			{
				// Store the temporary variable
				memcpy(&pTmp, &mList[i], sizeof(T));

				// Swap places
				memcpy(&mList[i], &mList[i - 1], sizeof(T));
				memcpy(&mList[i - 1], &pTmp, sizeof(T));

				// Restart at the beginning of the array
				i = 0;
			}
		}
	}

	inline int Size()
	{
		return mListCount;
	}

	inline int Capacity()
	{
		return mListSize;
	}

public:
	inline operator T*& () { return mList; }
	inline operator T* () { return mList; }
	inline T* operator[](int idx) { return &mList[idx]; }
};

#endif