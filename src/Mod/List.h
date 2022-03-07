#pragma once

#ifndef LIST_INSERT
#define LIST_INSERT(LISTV, LISTC, LISTTYPE, INDEX)		memmove(&LISTV[INDEX + 1], &LISTV[INDEX], (LISTC - INDEX - 1) * sizeof(LISTTYPE))
#endif
#ifndef LIST_ERASE
#define LIST_ERASE(LISTV, LISTC, LISTTYPE, INDEX)		memmove(&LISTV[INDEX], &LISTV[INDEX + 1], (LISTC - INDEX - 1) * sizeof(LISTTYPE))
#endif

template<class T>
class List
{
public:
	typedef T* iterator;

public:
	T* m_List;
	int m_ListSize;
	int m_ListCount;

public:
	List()
	{
		m_List = NULL;
		m_ListSize = 0;
		m_ListCount = 0;
	}
	~List()
	{
		if (m_List)
			free(m_List);
	}

public:
	/// Returns -1 when value couldn't be found.
	inline int FindIndex(const T& value)
	{
		if (!m_ListCount)
			return -1;

		for (int i = 0; i < m_ListCount; i++)
		{
			if (m_List[i] == value)
				return i;
		}

		return -1;
	}

	inline bool IsInArray(const T& value)
	{
		return (FindIndex(value) != -1);
	}

	/// Self-explanatory.
	inline void Clear()
	{
		m_ListCount = 0;
	}

	/// Self-explanatory.
	inline void Free()
	{
		m_ListCount = 0;
		m_ListSize  = 0;

		if (m_List)
		{
			free(m_List);
			m_List = NULL;
		}
	}

	/// Returns less-than-zero value on error; Otherwise, no error.
	/// -1 : Already in index
	/// -2 : Over capacity
	inline int Insert(const T& value)
	{
		// Make room for more if we have to
		if (!m_List || m_ListCount + 1 >= m_ListSize)
		{
			if (!m_List)
			{
				m_ListSize = 1;
				m_List     = (T*)malloc(sizeof(T) * m_ListSize);
			}
			else
			{
				m_ListSize += 1;
				m_List      = (T*)realloc((void*)m_List, sizeof(T) * m_ListSize);
			}
		}

		// Find where to insert it
		memcpy(&m_List[m_ListCount++], &value, sizeof(T));

		// Done
		return m_ListCount - 1;
	}
	
	/// Returns less-than-zero value on error; Otherwise, no error.
	/// -1 : Already in index
	/// -2 : Over capacity
	inline int InsertAt(const T& value, int index)
	{
		// Make room for more if we have to
		if (!m_List || m_ListCount + 1 >= m_ListSize)
		{
			if (!m_List)
			{
				m_ListSize = 1;
				m_List     = (T*)malloc(sizeof(T) * m_ListSize);
			}
			else
			{
				m_ListSize += 1;
				m_List      = (T*)realloc((void*)m_List, sizeof(T) * m_ListSize);
			}
		}

		index = (index > m_ListSize - 1 ? m_ListSize - 1 : (index < 0 ? 0 : index));

		++m_ListCount;
		LIST_INSERT(m_List, m_ListCount, T, index);
		memcpy(&m_List[index], &value, sizeof(T));

		// Done
		return m_ListCount - 1;
	}

	/// Returns less-than-zero value on error.
	/// -1 : Not in array
	/// -2 : Array is empty
	inline int Remove(const T& value)
	{
		if (!m_ListCount)
			return -2;

		int INDEX = 0;
		if ((INDEX = FindIndex(value)) == -1)
			return -1;

		LIST_ERASE(m_List, m_ListCount, T, INDEX);
		m_ListCount--;

		return INDEX;
	}

	/// Returns less-than-zero value on error.
	/// -1 : Array is empty
	/// -2 : Index out of range
	inline int RemoveIndex(int index)
	{
		if (!m_ListCount)
			return -1;

		if (index < 0 || index >= m_ListCount)
			return -2;

		LIST_ERASE(m_List, m_ListCount, T, index);
		m_ListCount--;

		return index;
	}

	/// Shrink the array's list.
	inline void Shrink()
	{
		if (!m_List || m_ListSize == m_ListCount)
			return;

		if (m_ListCount == 0)
		{
			free((void*)m_List);
			m_ListSize = 0;
			m_List     = NULL;

			return;
		}

		m_List     = (T*)realloc((void*)m_List, sizeof(T) * m_ListCount);
		m_ListSize = m_ListCount;
	}

	/// Add a new T to the list.
	inline T* Increment()
	{
		T ToAdd;
		Insert(ToAdd);
		return &m_List[m_ListCount - 1];
	}

	/// Set the size of the list.
	inline void SetSize(unsigned int iNewSize)
	{
		if (iNewSize == 0)
		{
			// Free ourselves
			Free();

			// Done
			return;
		}

		// Re-allocate (or allocate) the list
		if (!m_List)
		{
			m_ListSize = iNewSize;
			m_List     = (T*)malloc(sizeof(T) * m_ListSize);

			// Done
			return;
		}

		m_ListSize = iNewSize;
		m_List     = (T*)realloc((void*)m_List, sizeof(T) * m_ListSize);

		if (m_ListCount >= iNewSize)
			m_ListCount = iNewSize - 1;
	}

	inline int Size()
	{
		return m_ListCount;
	}

	inline int Capacity()
	{
		return m_ListSize;
	}

	inline T* back()
	{
		return (m_List ? &m_List[m_ListCount - 1] : NULL);
	}
	
	inline T* front()
	{
		return (m_List ? &m_List[0] : NULL);
	}

	inline T* begin()
	{
		return m_List;
	}

	inline T* end()
	{
		return m_List + m_ListCount;
	}

public:
	inline operator T*& () { return m_List; }
	inline operator T* () { return m_List; }
	inline T operator[](int idx) { return m_List[idx]; }
};