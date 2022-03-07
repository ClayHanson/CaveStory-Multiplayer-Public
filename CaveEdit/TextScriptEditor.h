#pragma once

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define TSC_INSERT_FRONT 0
#define TSC_INSERT_BACK  -1

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class TextScriptEditor
{
public: // Public structs

	enum TSC_DATA_TYPE : unsigned char
	{
		TDT_NONE    = 0,
		TDT_COMMAND = 1,
		TDT_TEXT    = 2,
		TDT_EVENT   = 3
	};

public: // TSC Data types

	struct TSC_ABSTRACT
	{
	protected:
		/// The type of data this struct represents
		TSC_DATA_TYPE data_type;
		
	public:
		// Methods
		inline TSC_DATA_TYPE GetDataType() { return data_type; }

		TSC_ABSTRACT()
		{
			data_type = TDT_NONE;
		}
	};

	struct TSC_COMMAND : TSC_ABSTRACT
	{
		/// The name of the command.
		char name[3];

		/// The arguments of this command.
		unsigned short args[4];

		/// Arg count for this command.
		unsigned char argc;

		TSC_COMMAND()
		{
			data_type = TDT_COMMAND;
		}
	};

	struct TSC_TEXT : TSC_ABSTRACT
	{
		/// The text.
		char* text;
		int text_len;

		TSC_TEXT()
		{
			data_type = TDT_TEXT;
		}
	};
	
	struct TSC_EVENT : TSC_ABSTRACT
	{
		/// The ID of this event. 0 - 9999
		unsigned short event_id;

		TSC_EVENT()
		{
			data_type = TDT_EVENT;
		}
	};

public: // Data

	TSC_ABSTRACT** mDataList;
	int mDataCount;
	int mDataSize;

public: // Constructor & deconstructor

	TextScriptEditor();
	~TextScriptEditor();

public: // Data methods

	/// @brief Parse TSC-syntax data.
	///
	/// @param bData The data to parse
	/// @param bDataLen The length of the data to parse
	/// @param bAdd If set to true, then the parsed data will be added to our event list.
	///
	/// @return bool Returns true if the data was parsed successfully.
	bool ParseData(char* pData, int iDataLen, bool bAdd = true);

	/// @brief Build the script buffer with the elements in the list.
	///
	/// @return char* Returns a pointer to the allocated buffer containing the built script.
	char* BuildScript();

public: // Addition methods

	/// @brief Insert a command element at the given index.
	///
	/// @param pCommand The command to insert.
	/// @param iArg0 The first argument to this command. Can be -1 to not be added.
	/// @param iArg1 The second argument to this command. Can be -1 to not be added.
	/// @param iArg2 The third argument to this command. Can be -1 to not be added.
	/// @param iArg3 The fourth argument to this command. Can be -1 to not be added.
	/// @param iInsertIndex The index to insert the element at.
	///
	/// @return bool Returns true on success.
	bool InsertCommand(char pCommand[4], int iArg0 = -1, int iArg1 = -1, int iArg2 = -1, int iArg3 = -1, int iInsertIndex = TSC_INSERT_BACK);

	/// @brief Insert a text element at the given index.
	///
	/// @param pText The text to add.
	/// @param iInsertIndex The index to insert the element at.
	///
	/// @return bool Returns true on success.
	bool InsertText(char* pText, int iInsertIndex = TSC_INSERT_BACK);

	/// @brief Insert an event element at the given index.
	///
	/// @param iEventId The ID of the event.
	/// @param iInsertIndex The index to insert the element at.
	///
	/// @return bool Returns true on success.
	bool InsertEvent(unsigned short iEventId, int iInsertIndex = TSC_INSERT_BACK);

public: // Removal methods

	/// @brief Remove a TSC element at the index.
	///
	/// @param iIndex The index which the element resides at.
	void RemoveElement(int iIndex);
};