#pragma once

#include <memory>
#include <vector>

//------------------------------------------------------------------------------------------------//

#define ASM_MAX_STACK 2048

#define ASM_INSERT(LISTV, LISTC, LISTTYPE, INDEX)		memmove(&LISTV[INDEX + 1], &LISTV[INDEX], (LISTC - INDEX - 1) * sizeof(LISTTYPE))
#define ASM_ERASE(LISTV, LISTC, LISTTYPE, INDEX)		memmove(&LISTV[INDEX], &LISTV[INDEX + 1], (LISTC - INDEX - 1) * sizeof(LISTTYPE))

//------------------------------------------------------------------------------------------------//

class AbstractAssemblyParser
{
public: // Structs

	enum TokenType : unsigned char
	{
		TOKEN_TYPE_NONE = 0,

		// Only use these
		TOKEN_TYPE_NAME,
		TOKEN_TYPE_NUMBER,
		TOKEN_TYPE_SYMBOL,

		// Don't use these
		TOKEN_TYPE_NUMBER_HEX,
		TOKEN_TYPE_NUMBER_BIN,
	};

	class AsmExecutionState;
	class AsmInstruction;
	
	struct AsmStack
	{
		int data[ASM_MAX_STACK];
	};

	class AsmOperation
	{
	public:

		/// String hash
		char name[6];

		/// Arguments
		unsigned char arg_count;

		/// Execute this opcode
		void(*exec)(AbstractAssemblyParser* pParser, AsmInstruction* pInst, AsmExecutionState* pState, AsmStack* pStack);
	};

	class AsmInstruction
	{
	public:

		/// The OP Code
		const AsmOperation* op;

		/// Arguments
		int* arg_list;
		unsigned char arg_count;
		int* arg_lookup_list;
		unsigned char arg_lookup_count;

	public:

		inline AsmInstruction()
		{
			op               = NULL;
			arg_list         = NULL;
			arg_lookup_list  = NULL;
			arg_count        = 0;
			arg_lookup_count = 0;
		}

		inline ~AsmInstruction()
		{
			if (arg_list)
				free(arg_list);

			if (arg_lookup_list)
				free(arg_lookup_list);
		}

	public:

		inline void AddArg(int val, bool arg)
		{
			// Make sure we have an op code
			if (!op)
				return;
			
			// Check to see if there are too many arguments
			if (arg_lookup_count >= op->arg_count && arg)
				return;

			// Allocate the list accordingly
			if (!arg_list)
			{
				arg_count = 1;
				arg_list  = (int*)malloc(sizeof(int) * arg_count);
			}
			else
			{
				arg_count += 1;
				arg_list   = (int*)realloc((void*)arg_list, sizeof(int) * arg_count);
			}
			
			// Set the value
			arg_list[arg_count - 1] = val;

			if (arg)
			{
				// Allocate the list accordingly
				if (!arg_lookup_list)
				{
					arg_lookup_count = 1;
					arg_lookup_list  = (int*)malloc(sizeof(int) * arg_lookup_count);
				}
				else
				{
					arg_lookup_count += 1;
					arg_lookup_list   = (int*)realloc((void*)arg_lookup_list, sizeof(int) * arg_lookup_count);
				}

				arg_lookup_list[arg_lookup_count - 1] = arg_count - 1;
			}
		}
		
		inline void InsertArg(int idx, int val)
		{
			// Make sure we have an op code
			if (!op)
				return;
			
			// Check to see if there are too many arguments
			if (arg_lookup_count >= op->arg_count)
				return;

			// Allocate the list accordingly
			if (!arg_list)
			{
				arg_count = 1;
				arg_list  = (int*)malloc(sizeof(int) * arg_count);
			}
			else
			{
				arg_count += 1;
				arg_list   = (int*)realloc((void*)arg_list, sizeof(int) * arg_count);
			}

			// Insert the value
			ASM_INSERT(arg_list, arg_count, int, idx);
			
			// Set the value
			arg_list[idx] = val;
		}

		inline int GetArg(unsigned char index)
		{
			return (arg_list ? arg_list[index % arg_count] : 0);
		}
	};

	struct TOKEN_STRUCT
	{
		/// This token's type.
		TokenType type;

		/// A pointer to an offset of pText where this token is at.
		const char* text_pointer;

		/// The line number for this token.
		int line_number;

		/// The value of this token.
		void* value;
	};

	struct ASM_LABEL
	{
		char label_name[32];
		AsmInstruction* prev_inst;
	};

	struct AsmCompilerState
	{
		std::vector<AsmInstruction*>* pLineTable;
		std::vector<ASM_LABEL*>* pLabelTable;
	};

	// Update this enum according to 'AsmExecutionState' AND 'gRegisterOffsets'
	enum RegisterAddresses
	{
		RA_EAX = -1,
		RA_EBX = -2,
		RA_ECX = -3,
		RA_EDX = -4,
		RA_ESI = -5,
		RA_EDI = -6,
		RA_ESP = -7,
		RA_EBP = -8,
		RA_R0  = -9,
		RA_R1  = -10,
		RA_R2  = -11,
		RA_R3  = -12,
		RA_R4  = -13,
		RA_R5  = -14,
		RA_R6  = -15,
		RA_R7  = -16,
		RA_PC  = -17,

		RA_PTR        = -18,
		RA_MATH_BEGIN = -19,
		RA_MATH_ADD   = -20,
		RA_MATH_SUB   = -21,
		RA_MATH_MUL   = -22,
		RA_MATH_DIV   = -23,
		RA_MATH_END   = -24,
		RA_DWORD_PTR  = -25,
		RA_WORD_PTR   = -26,
		RA_BYTE_PTR   = -27
	};

	struct AsmExecutionState
	{
		// Machine status word
		int msw;

		// Current execution
		int pc;

		// accumulator
		int eax;

		// ...
		int ebx;

		// counter
		int ecx;

		// data
		int edx;

		// ...
		int esi;

		// ...
		int edi;

		// stack pointer
		int esp;

		// base pointer
		int ebp;

		// registers
		int r[8];

		int get(unsigned int idx)
		{
			int* pTbl[] = { &eax, &ebx, &ecx, &edx, &esi, &edi, &esp, &ebp, &r[0], &r[1], &r[2], &r[3], &r[4], &r[5], &r[6], &r[7], &pc };
			return *pTbl[idx % 17];
		}
		
		void set(unsigned int idx, int val)
		{
			int* pTbl[]    = { &eax, &ebx, &ecx, &edx, &esi, &edi, &esp, &ebp, &r[0], &r[1], &r[2], &r[3], &r[4], &r[5], &r[6], &r[7], &pc };
			*pTbl[idx % 17] = val;
		}
	};

public:

	AsmExecutionState m_ExecState;
	AsmStack m_ExecStack;
	int m_MainInst;

protected: // Variables

	AsmInstruction** m_InstSet;
	int m_InstCount;

public: // C++ stuff

	AbstractAssemblyParser();
	~AbstractAssemblyParser();

public: // Public methods

	/// 

	/// @brief Parse a string
	///
	/// @param pText The text to parse.
	/// @param iTextLen The length of the text to parse.
	///
	/// @returns Returns non-zero if the parser ran into an error.
	///
	/// @errorcode 0	No error
	/// @errorcode -1	Unexpected EOL
	/// @errorcode -2	Unexpected character
	/// @errorcode -3	Number too big
	/// @errorcode -4	Syntax error
	/// @errorcode -5	Got non-int as argument for op
	/// @errorcode -6	An instruction couldn't accept an argument that was provided
	int ParseAsm(const char* pText, unsigned int iTextLen = 0xFFFFFFFF);

	/// @brief Execute opcodes at the given address until done.
	/// 
	/// @returns Returns true if the parser has more to execute, false if otherwise
	bool Call(int iAddress);

	/// @brief Reset this parser.
	void Reset();

	/// @brief Clear this parser of instructions.
	void ClearInstSet();

public:

	/// @brief Expand an operand of this instruction
	int __get_operand_value(AsmInstruction* pInst, int iArgIndex, bool bGetAddress = false);
	void __set_operand_value(AsmInstruction* pInst, int iArgIndex, int iArgValue);

protected: // Protected virtual methods

	/// @brief Fetch an operation by name.
	///
	/// @param The name of the operation
	///
	/// @returns Returns the operation data structure if it could be found.
	virtual const AsmOperation* FetchOpInfoByName(const char* name);

	/// @brief Parse one line of psuedo-assembly into the instruction set.
	///
	/// @param pInst The instruction to add the arg to.
	/// @param iArgIndex The argument index.
	/// @param pStr The string to parse.
	///
	/// @returns Returns an instruction if the line could be parsed successfully.
	virtual bool AddArgToInstruction(AsmInstruction* pInst, int iArgValue, AsmCompilerState* pState);

	/// @brief Resolve a name arg token to an int.
	virtual bool ResolveNameArg(const char* pValue, int& iValue, AsmCompilerState* pState);

	/// @brief Get a value at the specified 'address'
	virtual bool GetValueFromAddress(int iAddress, unsigned int& iValue, int iByteCount, AsmExecutionState* pState);

	/// @brief Set the value at the specified 'address'
	virtual bool SetValueAtAddress(int iAddress, const unsigned int& iValue, int iByteCount, AsmExecutionState* pState);
};