#include "AbstractAssemblyParser.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define TOKENIZER_IS_WHITESPACE(chr)	(chr == ' ' || chr == '\t' || chr == '\r' || chr == '\n')
#define TOKENIZER_IS_NAME(chr)			((chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z') || chr == '_')
#define TOKENIZER_IS_NUMBER(chr)		(chr >= '0' && chr <= '9')
#define TOKENIZER_IS_SYMBOL(chr)		(chr == ',' || chr == ';' || chr == '@' || chr == ':' || chr == '#' || chr == '[' || chr == ']' || chr == '+' || chr == '-' || chr == '*' || chr == '*' || chr == '$')
#define TOKENIZER_IS_HEX(chr)			((chr >= 'A' && chr <= 'F') || (chr >= 'a' && chr <= 'f') || (chr >= '0' && chr <= '9'))
#define TOKENIZER_IS_BIN(chr)			(chr == '0' || chr == '1')

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* gTokenTypeNames[] =
{
	" NONE ",
	" NAME ",
	"NUMBER",
	"SYMBOL"
};

static const char* gRegisterNames[] =
{
	/*
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
		*/

	"EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "ESP", "EBP", "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "PC", "PTR", "MATH_BEGIN", "MATH_ADD", "MATH_SUB", "MATH_MUL", "MATH_DIV", "MATH_END", "DWORD_PTR", "WORD_PTR", "BYTE_PTR"
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// HELPER: Convert a binary string to int
static unsigned int BinStringToInt(const char* str)
{
	register unsigned char* p = (unsigned char*)str;
	register unsigned int   r = 0;

	while (p && *p)
	{
		r <<= 1;
		r  += (unsigned int)((*p++) & 0x01);
	}

	return (unsigned int)r;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AbstractAssemblyParser::AbstractAssemblyParser()
{
	m_InstSet   = NULL;
	m_InstCount = 0;

	Reset();
}

AbstractAssemblyParser::~AbstractAssemblyParser()
{
	Reset();
	ClearInstSet();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define ERROR_CODE(ECODE, EMSG, ...) { printf(EMSG, __VA_ARGS__); iRetErrorCode = ECODE; goto FREE_TOKENS; }

int AbstractAssemblyParser::ParseAsm(const char* pText, unsigned int iTextLen)
{
	if (iTextLen == 0 || !pText || !*pText)
		return 0;

	if (iTextLen == 0xFFFFFFFF)
		iTextLen = strlen(pText);

	AsmCompilerState pCState;

	m_MainInst = 0;

	// List of tokens.
	std::vector<TOKEN_STRUCT*> pTokenList;

	// Use the index of instructions in the m_InstList set as the key for this vector.
	std::vector<int> pTokenLookupTable;

	// Allocate the new compiler state's variables
	pCState.pLineTable  = new std::vector<AsmInstruction*>;
	pCState.pLabelTable = new std::vector<ASM_LABEL*>;

	char buffer[128];
	int iRetErrorCode = 0;

	// Tokenize everything
	TokenType iLastTokenType = TokenType::TOKEN_TYPE_NONE;
	const char* pTokenStart  = NULL;
	int iLineNo              = 0;

	for (const char* ptr = pText; ptr <= pText + iTextLen; ptr++)
	{
		TokenType iCurrentTokenType = TokenType::TOKEN_TYPE_NONE;

		// Determine the token type
		if (ptr == (pText + iTextLen))
			iCurrentTokenType = iLastTokenType;
		else if (*ptr == ';')
		{
			for (;ptr < pText + iTextLen; ptr++)
			{
				if (*ptr == '\n')
					break;
			}

			ptr--;

			iCurrentTokenType = TokenType::TOKEN_TYPE_NONE;
		}
		else if (TOKENIZER_IS_WHITESPACE(*ptr))
		{
			// WHITESPACE
			if (*ptr == '\n')
				++iLineNo;

			// Don't tokenize whitespace
			iCurrentTokenType = TokenType::TOKEN_TYPE_NONE;
		}
		else if (TOKENIZER_IS_WHITESPACE(*ptr))
			iCurrentTokenType = TokenType::TOKEN_TYPE_NAME;
		else if (TOKENIZER_IS_HEX(*ptr) && iLastTokenType == TokenType::TOKEN_TYPE_NUMBER_HEX)
			iCurrentTokenType = TokenType::TOKEN_TYPE_NUMBER_HEX;
		else if (ptr[0] == '0' && ptr[1] == 'x' && iLastTokenType != TokenType::TOKEN_TYPE_NUMBER_HEX)
		{
			iCurrentTokenType = TokenType::TOKEN_TYPE_NUMBER_HEX;
			ptr              += 2;
				
			if (!TOKENIZER_IS_HEX(*ptr))
				ERROR_CODE(-1, "Line # %d: Unexpected EOL (Expected a hexidecimal character, got '%.1s' instead)\r\n", iLineNo, ptr);
		}
		else if (TOKENIZER_IS_BIN(*ptr) && iLastTokenType == TokenType::TOKEN_TYPE_NUMBER_BIN)
			iCurrentTokenType = TokenType::TOKEN_TYPE_NUMBER_BIN;
		else if (ptr[0] == '0' && ptr[1] == 'b' && iLastTokenType != TokenType::TOKEN_TYPE_NUMBER_BIN)
		{
			iCurrentTokenType = TokenType::TOKEN_TYPE_NUMBER_BIN;
			ptr              += 2;
				
			if (!TOKENIZER_IS_BIN(*ptr))
				ERROR_CODE(-1, "Line # %d: Unexpected EOL (Expected 0 or 1, got '%.1s' instead)\r\n", iLineNo, ptr);
		}
		else if (TOKENIZER_IS_NAME(*ptr))
			iCurrentTokenType = TokenType::TOKEN_TYPE_NAME;
		else if (TOKENIZER_IS_NUMBER(*ptr) && iLastTokenType != TokenType::TOKEN_TYPE_NUMBER_HEX)
		{
			iCurrentTokenType = TokenType::TOKEN_TYPE_NUMBER;
		}
		else if (TOKENIZER_IS_SYMBOL(*ptr))
			iCurrentTokenType = TokenType::TOKEN_TYPE_SYMBOL;
		else
		{
			// Unhandled character
			ERROR_CODE(-2, "Line # %d: Unhandled character '%.1s'\r\n", iLineNo, ptr);
		}

		// If it's a different token type, then save the last token
		if (ptr >= pText + iTextLen || iCurrentTokenType != iLastTokenType || (iLastTokenType == TokenType::TOKEN_TYPE_SYMBOL && (ptr - pTokenStart) == 1))
		{
			int iTokenLen = ptr - pTokenStart;

			// Add new token
			if (iTokenLen && iLastTokenType != TokenType::TOKEN_TYPE_NONE)
			{
				// Allocate the new token
				TOKEN_STRUCT* pNewToken = new TOKEN_STRUCT();
				pNewToken->type         = (iLastTokenType == TokenType::TOKEN_TYPE_NUMBER_BIN || iLastTokenType == TokenType::TOKEN_TYPE_NUMBER_HEX ? TokenType::TOKEN_TYPE_NUMBER : iLastTokenType);
				pNewToken->line_number  = iLineNo;
				pNewToken->text_pointer = pTokenStart;
				pNewToken->value        = 0;

				// Make sure there is an entry for this line
				while (pCState.pLineTable->size() < pNewToken->line_number + 1)
					pCState.pLineTable->push_back(NULL);

				// Allocate differently per token type
				switch (iLastTokenType)
				{
					case TokenType::TOKEN_TYPE_NAME:
					{
						// Allocate a new buffer
						pNewToken->value           = new char[iTokenLen + 1];
						*((char*)pNewToken->value) = 0;

						// Copy the token into the buffer
						strncat(((char*)pNewToken->value), pTokenStart, iTokenLen);
						strupr((char*)pNewToken->value);
						break;
					}
					case TokenType::TOKEN_TYPE_NUMBER:
					{
						// Copy the token into a buffer
						*buffer = 0;
						strncat_s(buffer, sizeof(buffer), pTokenStart, iTokenLen);

						// Set the value
						*((int*)&pNewToken->value) = atoi(buffer);
						break;
					}
					case TokenType::TOKEN_TYPE_SYMBOL:
					{
						// Copy the token into a buffer
						*buffer = 0;
						strncat_s(buffer, sizeof(buffer), pTokenStart, iTokenLen);

						// Set the value
						*((int*)&pNewToken->value) = *pTokenStart;
						ptr                        = pTokenStart + 1;

						break;
					}
					case TokenType::TOKEN_TYPE_NUMBER_HEX:
					{
						if (iTokenLen > 8)
						{
							delete pNewToken;
							ERROR_CODE(-3, "Line # %d: Number too long (Numbers can only be 4 bytes long)\r\n", iLineNo);
						}

						// Copy the token into a buffer
						*buffer = 0;
						strncat_s(buffer, sizeof(buffer), pTokenStart, iTokenLen);

						// Set the value
						*((int*)&pNewToken->value) = strtol(buffer, NULL, 16);
						break;
					}
					case TokenType::TOKEN_TYPE_NUMBER_BIN:
					{
						if (iTokenLen > 32)
						{
							delete pNewToken;
							ERROR_CODE(-3, "Line # %d: Number too long (Numbers can only be 4 bytes long)\r\n", iLineNo);
						}

						// Copy the token into a buffer
						*buffer = 0;
						strncat_s(buffer, sizeof(buffer), pTokenStart, iTokenLen);

						// Set the value
						*((int*)&pNewToken->value) = BinStringToInt(buffer);
							
						break;
					}
				}

				pTokenList.push_back(pNewToken);
			}

			pTokenStart    = ptr;
			iLastTokenType = iCurrentTokenType;
		}
	}

	// FIRST PASS: Remove comments
	for (int i = 0; i < pTokenList.size(); i++)
	{
		TOKEN_STRUCT* pThisToken = pTokenList[i];
		int iThisLineNo          = pThisToken->line_number;

		if (i + 1 < pTokenList.size() && pThisToken->type == TokenType::TOKEN_TYPE_NAME && *((char*)pThisToken->value) == 'R' && pTokenList[i + 1]->type == TokenType::TOKEN_TYPE_NUMBER)
		{
			// Register name -- Combine these things
			char* pNewStr = new char[3];
			pNewStr[0]    = 'R';
			pNewStr[1]    = '0' + (((int)pTokenList[i + 1]->value) % 9);
			pNewStr[2]    = 0;

			delete[] (char*)pThisToken->value;
			pThisToken->value = pNewStr;
			delete pTokenList[i + 1];
			pTokenList.erase(pTokenList.begin() + i + 1);
		}

		if (i + 1 < pTokenList.size() && pThisToken->type == TokenType::TOKEN_TYPE_SYMBOL && (char)pThisToken->value == '-' && pTokenList[i + 1]->type == TokenType::TOKEN_TYPE_NUMBER)
		{
			// Negative number
			char b[128];

			// Build it
			sprintf_s(b, sizeof(b), "-%d", (int)pTokenList[i + 1]);

			pThisToken->type  = TokenType::TOKEN_TYPE_NUMBER;
			pThisToken->value = (void*)atoi(b);
			delete pTokenList[i + 1];
			pTokenList.erase(pTokenList.begin() + i + 1);
		}

		// Process this based on token type
		printf("LINE # %04d [ %s ]: ", pThisToken->line_number, gTokenTypeNames[pThisToken->type]);

		switch (pThisToken->type)
		{
			case TokenType::TOKEN_TYPE_NAME:
			{
				printf("\"%s\"\r\n", (char*)pThisToken->value);
				break;
			}
			case TokenType::TOKEN_TYPE_NUMBER:
			{
				printf("0x%08X (%d)\r\n", (int)pThisToken->value, (int)pThisToken->value);
				break;
			}
			case TokenType::TOKEN_TYPE_SYMBOL:
			{
				printf("'%.1s' (%d)\r\n", (char*)& pThisToken->value, (int)pThisToken->value);
				break;
			}
			default:
			{
				printf("<n/a>\r\n");
				break;
			}
		}
	}

	// SECOND PASS: Generate instructions
	for (std::vector<TOKEN_STRUCT*>::iterator it = pTokenList.begin(); it != pTokenList.end(); it++)
	{
		TOKEN_STRUCT* pThisToken = *it;

		// Expect an opname
		if (pThisToken->type != TokenType::TOKEN_TYPE_NAME)
			ERROR_CODE(-4, "Line # %d: Expected an opcode / label name\r\n", pThisToken->line_number);

		// Label!!
		if (it + 1 != pTokenList.end() && (*(it + 1))->type == TokenType::TOKEN_TYPE_SYMBOL && (char)((*(it + 1))->value) == ':')
		{
			ASM_LABEL* pLabel = new ASM_LABEL();
			pLabel->prev_inst = (!m_InstCount ? NULL : m_InstSet[m_InstCount - 1]);
			strcpy_s(pLabel->label_name, sizeof(pLabel->label_name), (char*)pThisToken->value);

			if (!stricmp(pLabel->label_name, "main"))
				m_MainInst = m_InstCount;

			pCState.pLabelTable->push_back(pLabel);
			it++;

			continue;
		}

		// Fetch the opcode for this operation
		const AsmOperation* OpInfo = FetchOpInfoByName((char*)pThisToken->value);
		if (!OpInfo)
			ERROR_CODE(-4, "Line # %d: Unknown operation \"%s\".\r\n", pThisToken->line_number, (char*)pThisToken->value);

		// Create the instruction
		AsmInstruction* pNewInst = new AsmInstruction();
		pNewInst->op             = OpInfo;

		// Allocate new space for the instruction
		if (!m_InstSet)
		{
			m_InstCount = 1;
			m_InstSet   = (AsmInstruction**)malloc(sizeof(AsmInstruction*) * m_InstCount);
		}
		else
		{
			m_InstCount += 1;
			m_InstSet    = (AsmInstruction**)realloc((void*)m_InstSet, sizeof(AsmInstruction*) * m_InstCount);
		}

		// Set the instruction at the specified location
		m_InstSet[m_InstCount - 1] = pNewInst;

		// Setup line pointers
		if ((*pCState.pLineTable)[pThisToken->line_number] == NULL)
			(*pCState.pLineTable)[pThisToken->line_number] = pNewInst;

		pTokenLookupTable.push_back(it - pTokenList.begin());
		std::vector<TOKEN_STRUCT*>::iterator last_it = it;

		// Look for the next token
		for (; it != pTokenList.end(); it++)
		{
			if (pThisToken->line_number != ((TOKEN_STRUCT*)*it)->line_number)
				break;

			last_it = it;
		}

		it = last_it;

		// Stop here; we've reached the end.
		if (it == pTokenList.end())
			break;
	}

	// THIRD PASS: Add instruction sets' arguments
	for (int i = 0; i < m_InstCount; i++)
	{
		AsmInstruction* pInst = m_InstSet[i];
		int iArgTokenStart    = pTokenLookupTable[i] + 1;
		int iTokenIndex       = iArgTokenStart;
		bool bInPointer       = false;
		int iArgIndex         = 0;

		// 0 : Not in math OP
		// 1 : Expecting pointer symbol, number OR name
		// 2 : Expecting math symbol OR closing symbol
		char bInMathOp = 0;

		// Get all arguments
		while (iArgIndex < pInst->op->arg_count || bInMathOp || bInPointer)
		{
			if (iTokenIndex >= pTokenList.size())
				ERROR_CODE(-1, "Line # %d: Unexpected EOL (Expected paremeter # %d)\r\n", pTokenList[iArgTokenStart - 1]->line_number, iArgIndex);

			TOKEN_STRUCT* pArgToken = pTokenList[iTokenIndex];
			int iArgValue           = 0;

			if ((!bInMathOp || bInMathOp == 1) && !bInPointer && pArgToken->type == TokenType::TOKEN_TYPE_SYMBOL && (char)pArgToken->value == '$')
			{
				// Pointers
				++iTokenIndex;
				pInst->AddArg(RegisterAddresses::RA_PTR, true);

				bInPointer = true;
				continue;
			}

			if (!bInPointer && !bInMathOp && pArgToken->type == TokenType::TOKEN_TYPE_NAME && (!strcmp((char*)pArgToken->value, "DWORD") || !strcmp((char*)pArgToken->value, "WORD") || !strcmp((char*)pArgToken->value, "BYTE")) &&
				iTokenIndex + 2 < pTokenList.size() && pTokenList[iTokenIndex + 1]->type == TokenType::TOKEN_TYPE_NAME && !strcmp((char*)pTokenList[iTokenIndex + 1]->value, "PTR") &&
				pTokenList[iTokenIndex + 2]->type == TokenType::TOKEN_TYPE_SYMBOL && (char)pTokenList[iTokenIndex + 2]->value == '[')
			{
				// Math ptr with specifier
				iTokenIndex += 3;

				if (!strcmp((char*)pArgToken->value, "DWORD"))
					pInst->AddArg(RegisterAddresses::RA_DWORD_PTR, true);
				else if (!strcmp((char*)pArgToken->value, "WORD"))
					pInst->AddArg(RegisterAddresses::RA_WORD_PTR, true);
				else if (!strcmp((char*)pArgToken->value, "BYTE"))
					pInst->AddArg(RegisterAddresses::RA_BYTE_PTR, true);

				pInst->AddArg(RegisterAddresses::RA_MATH_BEGIN, false);

				// Go until we hit the end
				bInMathOp = 1;
				continue;
			}
			else if (!bInPointer && !bInMathOp && pArgToken->type == TokenType::TOKEN_TYPE_SYMBOL && (char)pArgToken->value == '[')
			{
				// Math ptr
				iTokenIndex++;
				pInst->AddArg(RegisterAddresses::RA_MATH_BEGIN, true);

				// Go until we hit the end
				bInMathOp = 1;
				continue;
			}

			if (bInMathOp == 2)
			{
				if (pArgToken->type == TokenType::TOKEN_TYPE_SYMBOL && (char)pArgToken->value == ']')
				{
					pInst->AddArg(RegisterAddresses::RA_MATH_END, false);
					bInMathOp = 0;
					goto GOTO_NEXT_ARG;
				}
				else if (pArgToken->type == TokenType::TOKEN_TYPE_SYMBOL && (char)pArgToken->value == '+')
				{
					pInst->AddArg(RegisterAddresses::RA_MATH_ADD, false);
					bInMathOp = 1;
					++iTokenIndex;
					continue;
				}
				else if (pArgToken->type == TokenType::TOKEN_TYPE_SYMBOL && (char)pArgToken->value == '-')
				{
					pInst->AddArg(RegisterAddresses::RA_MATH_SUB, false);
					bInMathOp = 1;
					++iTokenIndex;
					continue;
				}
				else if (pArgToken->type == TokenType::TOKEN_TYPE_SYMBOL && (char)pArgToken->value == '*')
				{
					pInst->AddArg(RegisterAddresses::RA_MATH_MUL, false);
					bInMathOp = 1;
					++iTokenIndex;
					continue;
				}
				else if (pArgToken->type == TokenType::TOKEN_TYPE_SYMBOL && (char)pArgToken->value == '/')
				{
					pInst->AddArg(RegisterAddresses::RA_MATH_DIV, false);
					bInMathOp = 1;
					++iTokenIndex;
					continue;
				}
			}

			if (pArgToken->type == TokenType::TOKEN_TYPE_NAME)
			{
				// Look for the label
				if (!ResolveNameArg((const char*)pArgToken->value, iArgValue, &pCState))
					ERROR_CODE(-5, "Line # %d: Could not resolve \"%s\" for instruction \"%s\"\r\n", pTokenList[iArgTokenStart - 1]->line_number, (char*)pArgToken->value, pInst->op->name);
			}
			else if (pArgToken->type == TokenType::TOKEN_TYPE_NUMBER)
				iArgValue = (int)pArgToken->value;
			else
				ERROR_CODE(-5, "Line # %d: Got a non-int argument for instruction \"%s\"\r\n", pTokenList[iArgTokenStart - 1]->line_number, pInst->op->name);

			// Add this argument to this instruction's argument list
			if (bInMathOp)
			{
				if (bInMathOp == 1)
				{
					if (bInPointer)
						bInPointer = false;

					bInMathOp = 2;
					pInst->AddArg(iArgValue, false);
				}
				else
					ERROR_CODE(-6, "Line # %d: Encountered an unexpected %s (bInMathOp=%d, bInPointer=%d).\r\n", pTokenList[iArgTokenStart - 1]->line_number, pInst->op->name, bInMathOp, bInPointer);
			}
			else if (bInPointer)
			{
				bInPointer = false;
				pInst->AddArg(iArgValue, false);
			}
			else if (!AddArgToInstruction(pInst, iArgValue, &pCState))
				ERROR_CODE(-6, "Line # %d: Invalid operand '%d' for operation \"%s\"!\r\n", pTokenList[iArgTokenStart - 1]->line_number, (int)iArgValue, pInst->op->name);

			if (!bInMathOp)
			{
			GOTO_NEXT_ARG:
				if (++iArgIndex >= pInst->op->arg_count)
					break;

				if (iTokenIndex + 1 >= pTokenList.size())
					ERROR_CODE(-1, "Line # %d: Unexpected EOL (Expected paremeter # %d)\r\n", pTokenList[iArgTokenStart - 1]->line_number, iArgIndex);

				if (iTokenIndex + 1 >= pTokenList.size() || pTokenList[iTokenIndex + 1]->type != TokenType::TOKEN_TYPE_SYMBOL || (char)pTokenList[iTokenIndex + 1]->value != ',')
					ERROR_CODE(-1, "Line # %d: Unexpected EOL (Expected paremeter # %d)\r\n", pTokenList[iArgTokenStart - 1]->line_number, iArgIndex);
			}

			iTokenIndex += (!bInMathOp ? 2 : 1);
		}
	}

	// DEBUG
	printf("Generated %d instruction%s:\r\n", m_InstCount, m_InstCount == 1 ? "" : "s");
	for (int i = 0; i < m_InstCount; i++)
	{
		AsmInstruction* pInst = m_InstSet[i];

		printf("[%04d / %04d] LINE # %04d > %s ", i, m_InstCount, pTokenList[pTokenLookupTable[i]]->line_number, pInst->op->name);
		for (int j = 0; j < pInst->arg_lookup_count; j++)
		{
			int iArgStart = pInst->arg_lookup_list[j];
			int iArgEnd    = (j + 1 >= pInst->arg_lookup_count ? pInst->arg_count : pInst->arg_lookup_list[j + 1]);

			if ((iArgEnd - iArgStart) > 1)
			{
				printf("{ ");
				for (int l = iArgStart; l < iArgEnd; l++)
				{
					if (pInst->arg_list[l] < 0)
						printf("%s%s", gRegisterNames[-(pInst->arg_list[l] + 1)], (l == (iArgEnd - 1) ? " " : ", "));
					else
						printf("%d%s", pInst->arg_list[l], (l == (iArgEnd - 1) ? " " : ", "));
				}
				printf("}%s", j == pInst->arg_lookup_count - 1 ? "\r\n" : ", ");
				continue;
			}

			if (pInst->arg_list[iArgStart] < 0)
				printf("%s%s", gRegisterNames[-(pInst->arg_list[iArgStart] + 1)], j == pInst->arg_lookup_count - 1 ? "\r\n" : ", ");
			else
				printf("%d%s", pInst->arg_list[iArgStart], j == pInst->arg_lookup_count -1 ? "\r\n" : ", ");
		}
		if (!pInst->arg_lookup_count)
			printf("\r\n");
	}
	printf("\r\n");

	if (0 == 1)
	{
FREE_TOKENS:
		free(m_InstSet);
		m_InstSet   = NULL;
		m_InstCount = 0;
	}

	for (int i = 0; i < pCState.pLabelTable->size(); i++)
		delete (*pCState.pLabelTable)[i];

	// Delete the instruction lookup table
	delete pCState.pLineTable;
	delete pCState.pLabelTable;

	// Delete the tokens
	for (std::vector<TOKEN_STRUCT*>::iterator it = pTokenList.begin(); it != pTokenList.end(); it++)
	{
		TOKEN_STRUCT* pToken = *it;

		// Free the string
		if (pToken->type == TokenType::TOKEN_TYPE_NAME)
			delete[] (char*)pToken->value;

		// Free the token
		delete pToken;
	}

	// Done!
	return iRetErrorCode;
}

bool AbstractAssemblyParser::Call(int iAddress)
{
	if (m_ExecState.pc >= m_InstCount)
		return false;

	// Store the current address
	m_ExecState.esp -= 4;
	m_ExecStack.data[(m_ExecState.esp / 4) % ASM_MAX_STACK] = m_ExecState.pc;

	// Reset
	m_ExecState.pc = iAddress;

	while (true)
	{
		AsmInstruction* pInst = m_InstSet[m_ExecState.pc];

		// Execute this opcode's func
		pInst->op->exec(this, pInst, &m_ExecState, &m_ExecStack);

		// If the PC is the same as it was, then we've halted.
		if (++m_ExecState.pc >= m_InstCount || pInst == m_InstSet[m_ExecState.pc])
			break;
	}

	// Restore it
	m_ExecState.pc   = m_ExecStack.data[(m_ExecState.esp / 4) % ASM_MAX_STACK];
	m_ExecState.esp += 4;

	return false;
}

void AbstractAssemblyParser::Reset()
{
	memset(&m_ExecState, 0, sizeof(m_ExecState));
	memset(&m_ExecStack, 0, sizeof(m_ExecStack));
	m_ExecState.esp = (ASM_MAX_STACK * 4);
}

void AbstractAssemblyParser::ClearInstSet()
{
	if (m_InstSet)
	{
		free(m_InstSet);
		m_InstSet = NULL;
	}

	m_InstCount = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int AbstractAssemblyParser::__get_operand_value(AsmInstruction* pInst, int iArgIndex, bool bGetAddress)
{
	if (
		pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] == RegisterAddresses::RA_DWORD_PTR ||
		pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] == RegisterAddresses::RA_WORD_PTR ||
		pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] == RegisterAddresses::RA_BYTE_PTR ||
		pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] == RegisterAddresses::RA_MATH_BEGIN
		)
	{
		int iArgStart  = pInst->arg_lookup_list[iArgIndex];
		int iArgEnd    = (iArgIndex + 1 >= pInst->arg_lookup_count ? pInst->arg_count : pInst->arg_lookup_list[iArgIndex + 1]);
		int iReturn    = 0;
		int iByteCount = 4;

		if (pInst->arg_list[iArgStart] == RegisterAddresses::RA_DWORD_PTR ||
			pInst->arg_list[iArgStart] == RegisterAddresses::RA_WORD_PTR ||
			pInst->arg_list[iArgStart] == RegisterAddresses::RA_BYTE_PTR)
		{
			iByteCount = (pInst->arg_list[iArgStart] == RegisterAddresses::RA_DWORD_PTR ? 4 : (pInst->arg_list[iArgStart] == RegisterAddresses::RA_WORD_PTR ? 2 : 1));
			iArgStart += 1;
		}

		// Process the operand stipulations
		for (int i = iArgStart + 1; i < iArgEnd - 1; i++)
		{
			int operand = pInst->arg_list[i];

			// Get this operand's value
			if (i == iArgStart + 1)
			{
				if (operand < 0)
					operand = m_ExecState.get(-(operand + 1));

				iReturn = operand;
			}

			switch (operand)
			{
				case RegisterAddresses::RA_MATH_ADD:
				{
					operand = pInst->arg_list[++i];

					if (operand < 0)
						iReturn += m_ExecState.get(-(operand + 1));
					else
						iReturn += operand;

					break;
				}
				case RegisterAddresses::RA_MATH_SUB:
				{
					operand = pInst->arg_list[++i];

					if (operand < 0)
						iReturn -= m_ExecState.get(-(operand + 1));
					else
						iReturn -= operand;

					break;
				}
				case RegisterAddresses::RA_MATH_MUL:
				{
					operand = pInst->arg_list[++i];

					if (operand < 0)
						iReturn *= m_ExecState.get(-(operand + 1));
					else
						iReturn *= operand;

					break;
				}
				case RegisterAddresses::RA_MATH_DIV:
				{
					operand = pInst->arg_list[++i];

					if (operand < 0)
						iReturn /= m_ExecState.get(-(operand + 1));
					else
						iReturn /= operand;

					break;
				}
			}
		}

		if (bGetAddress)
			return iReturn;

		unsigned int iValue = 0;

		// Get the value
		GetValueFromAddress(iReturn, iValue, iByteCount, &m_ExecState);

		return iValue;
	}
	else if (pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] == RegisterAddresses::RA_PTR)
	{
		int iAddress        = (pInst->arg_list[pInst->arg_lookup_list[iArgIndex] + 1] < 0 ? m_ExecState.get(-(pInst->arg_list[pInst->arg_lookup_list[iArgIndex] + 1] + 1)) : pInst->arg_list[pInst->arg_lookup_list[iArgIndex] + 1]);
		unsigned int iValue = 0;

		if (bGetAddress)
			return iAddress;

		GetValueFromAddress(iAddress, iValue, 4, &m_ExecState);

		return iValue;
	}

	if (bGetAddress)
		return pInst->arg_list[pInst->arg_lookup_list[iArgIndex]];

	return (pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] < 0 ? m_ExecState.get(-(pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] + 1)) : pInst->arg_list[pInst->arg_lookup_list[iArgIndex]]);
}

void AbstractAssemblyParser::__set_operand_value(AsmInstruction* pInst, int iArgIndex, int iArgValue)
{
	if (
		pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] == RegisterAddresses::RA_DWORD_PTR ||
		pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] == RegisterAddresses::RA_WORD_PTR ||
		pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] == RegisterAddresses::RA_BYTE_PTR ||
		pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] == RegisterAddresses::RA_MATH_BEGIN
		)
	{
		int iArgStart  = pInst->arg_lookup_list[iArgIndex];
		int iArgEnd    = (iArgIndex + 1 >= pInst->arg_lookup_count ? pInst->arg_count : pInst->arg_lookup_list[iArgIndex + 1]);
		int iReturn    = 0;
		int iByteCount = 4;

		if (pInst->arg_list[iArgStart] == RegisterAddresses::RA_DWORD_PTR ||
			pInst->arg_list[iArgStart] == RegisterAddresses::RA_WORD_PTR ||
			pInst->arg_list[iArgStart] == RegisterAddresses::RA_BYTE_PTR)
		{
			iByteCount = (pInst->arg_list[iArgStart] == RegisterAddresses::RA_DWORD_PTR ? 4 : (pInst->arg_list[iArgStart] == RegisterAddresses::RA_WORD_PTR ? 2 : 1));
			iArgStart += 1;
		}

		// Process the operand stipulations
		for (int i = iArgStart + 1; i < iArgEnd - 1; i++)
		{
			int operand = pInst->arg_list[i];

			if (i == iArgStart + 1)
			{
				// Get this operand's value
				if (operand < 0)
					operand = m_ExecState.get(-(operand + 1));

				iReturn = operand;
				continue;
			}

			switch (operand)
			{
				case RegisterAddresses::RA_MATH_ADD:
				{
					operand = pInst->arg_list[++i];

					printf("add operand '%d' to iReturn('%d')\r\n", operand, iReturn);
					if (operand < 0)
						iReturn += m_ExecState.get(-(operand + 1));
					else
						iReturn += operand;

					break;
				}
				case RegisterAddresses::RA_MATH_SUB:
				{
					operand = pInst->arg_list[++i];

					if (operand < 0)
						iReturn -= m_ExecState.get(-(operand + 1));
					else
						iReturn -= operand;

					break;
				}
				case RegisterAddresses::RA_MATH_MUL:
				{
					operand = pInst->arg_list[++i];

					if (operand < 0)
						iReturn *= m_ExecState.get(-(operand + 1));
					else
						iReturn *= operand;

					break;
				}
				case RegisterAddresses::RA_MATH_DIV:
				{
					operand = pInst->arg_list[++i];

					if (operand < 0)
						iReturn /= m_ExecState.get(-(operand + 1));
					else
						iReturn /= operand;

					break;
				}
			}
		}

		// Get the value
		SetValueAtAddress(iReturn, iArgValue, iByteCount, &m_ExecState);
	}
	else if (pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] == RegisterAddresses::RA_PTR)
	{
		int iAddress = (pInst->arg_list[pInst->arg_lookup_list[iArgIndex] + 1] < 0 ? m_ExecState.get(-(pInst->arg_list[pInst->arg_lookup_list[iArgIndex] + 1] + 1)) : pInst->arg_list[pInst->arg_lookup_list[iArgIndex] + 1]);
		SetValueAtAddress(iAddress, iArgValue, 4, &m_ExecState);
	}
	else if (pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] < 0)
		m_ExecState.set(-(pInst->arg_list[pInst->arg_lookup_list[iArgIndex]] + 1), iArgValue);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define DEFINE_ASM_FUNC(NAME) static void ASMEXEC_##NAME(AbstractAssemblyParser* pParser, AbstractAssemblyParser::AsmInstruction* pInst, AbstractAssemblyParser::AsmExecutionState* pState, AbstractAssemblyParser::AsmStack* pStack)
#define GET_PARAM(PARAM) (PARAM < 0 ? pState->get((unsigned int)(-(PARAM + 1))) : PARAM)
#define SET_PARAM(PARAM, VALUE) pState->set((unsigned int)(-(PARAM + 1)), VALUE);

static int szArg0 = 0;
static int szArg1 = 0;
static int szArg2 = 0;
static int szArg3 = 0;
static int szArg4 = 0;
static int szArg5 = 0;

DEFINE_ASM_FUNC(CMP)
{
	szArg0      = pParser->__get_operand_value(pInst, 0);
	szArg1      = pParser->__get_operand_value(pInst, 1);
	pState->msw = 0;

	// Only xor first bit if it equals the value.
	if (szArg0 == szArg1)
		pState->msw |= 0b0001;
	
	if (szArg0 > szArg1)
		pState->msw |= 0b0010;
	
	if (szArg0 < szArg1)
		pState->msw |= 0b0100;
}

DEFINE_ASM_FUNC(JMP)
{
	szArg0     = pParser->__get_operand_value(pInst, 0);
	pState->pc = szArg0 - 1;
}

DEFINE_ASM_FUNC(JE)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);

	if (pState->msw & 0b0001)
		pState->pc = szArg0 - 1;
}

DEFINE_ASM_FUNC(JNE)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);

	if (!(pState->msw & 0b0001))
		pState->pc = szArg0 - 1;
}

DEFINE_ASM_FUNC(JG)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);

	if (pState->msw & 0b0010)
		pState->pc = szArg0 - 1;
}

DEFINE_ASM_FUNC(JGE)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);

	if (pState->msw & 0b0011)
		pState->pc = szArg0 - 1;
}

DEFINE_ASM_FUNC(JL)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);

	if (pState->msw & 0b0100)
		pState->pc = szArg0 - 1;
}

DEFINE_ASM_FUNC(JLE)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);

	if (pState->msw & 0b0101)
		pState->pc = szArg0 - 1;
}

DEFINE_ASM_FUNC(INC)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);

	pParser->__set_operand_value(pInst, 0, szArg0 + 1);
}

DEFINE_ASM_FUNC(DEC)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);

	pParser->__set_operand_value(pInst, 0, szArg0 - 1);
}

DEFINE_ASM_FUNC(ADD)
{
	szArg0      = pParser->__get_operand_value(pInst, 0);
	szArg1      = pParser->__get_operand_value(pInst, 1);

	pParser->__set_operand_value(pInst, 0, szArg0 + szArg1);
}

DEFINE_ASM_FUNC(SUB)
{
	szArg0      = pParser->__get_operand_value(pInst, 0);
	szArg1      = pParser->__get_operand_value(pInst, 1);
	
	pParser->__set_operand_value(pInst, 0, szArg0 - szArg1);
}

DEFINE_ASM_FUNC(IMUL)
{
	szArg0      = pParser->__get_operand_value(pInst, 0);
	szArg1      = pParser->__get_operand_value(pInst, 1);
	
	pParser->__set_operand_value(pInst, 0, szArg0 * szArg1);
}

DEFINE_ASM_FUNC(IDIV)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);

	if (!szArg0)
	{
		pState->edx = 0;
		pState->eax = 0;

		return;
	}

	pState->edx = (pState->eax % szArg0);
	pState->eax = (pState->eax / szArg0);
}

DEFINE_ASM_FUNC(PUSH)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);

	// Store the current address
	pState->esp -= 4;
	pStack->data[(pState->esp / 4) % ASM_MAX_STACK] = szArg0;
}

DEFINE_ASM_FUNC(POP)
{
	if (!pState->esp)
		return;
	
	szArg0 = pStack->data[(pState->esp / 4) % ASM_MAX_STACK];
	pParser->__set_operand_value(pInst, 0, szArg0);
	pState->esp += 4;
}

DEFINE_ASM_FUNC(LEA)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);
	szArg1 = pParser->__get_operand_value(pInst, 1, true);

	pParser->__set_operand_value(pInst, 0, szArg1);
}

DEFINE_ASM_FUNC(CALL)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);
	pParser->Call(szArg0);
}

DEFINE_ASM_FUNC(MOV)
{
	szArg0 = pParser->__get_operand_value(pInst, 1);

	pParser->__set_operand_value(pInst, 0, szArg0);
}

DEFINE_ASM_FUNC(RET)
{
	--pState->pc;
}

DEFINE_ASM_FUNC(PRINT)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);
	printf("PRINT: %d\r\n", szArg0);
}

DEFINE_ASM_FUNC(SHL)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);
	szArg1 = pParser->__get_operand_value(pInst, 1);

	pParser->__set_operand_value(pInst, 0, szArg0 << szArg1);
}

DEFINE_ASM_FUNC(SHR)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);
	szArg1 = pParser->__get_operand_value(pInst, 1);

	pParser->__set_operand_value(pInst, 0, szArg0 >> szArg1);
}

DEFINE_ASM_FUNC(AND)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);
	szArg1 = pParser->__get_operand_value(pInst, 1);

	pParser->__set_operand_value(pInst, 0, szArg0 & szArg1);
}

DEFINE_ASM_FUNC(OR)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);
	szArg1 = pParser->__get_operand_value(pInst, 1);

	pParser->__set_operand_value(pInst, 0, szArg0 | szArg1);
}

DEFINE_ASM_FUNC(XOR)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);
	szArg1 = pParser->__get_operand_value(pInst, 1);

	pParser->__set_operand_value(pInst, 0, szArg0 ^ szArg1);
}

DEFINE_ASM_FUNC(NOT)
{
	szArg0 = pParser->__get_operand_value(pInst, 0);

	pParser->__set_operand_value(pInst, 0, szArg0 ^ 0xFFFFFFFF);
}

#undef DEFINE_ASM_FUNC

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static AbstractAssemblyParser::AsmOperation gBaseOps[] =
{
	// Jumps
	//							  : ARGS		: DESCRIPTION
	{ "CMP", 2, ASMEXEC_CMP }, // : A, B		: Compare 'A' against 'B'.
	{ "JMP", 1, ASMEXEC_JMP }, // : A			: Unconditionally jump to address 'A'.
	{ "JE",  1, ASMEXEC_JE  }, // : A			: Jump to address 'C' if last result equaled
	{ "JNE", 1, ASMEXEC_JNE }, // : A			: Jump to address 'C' if last result didn't equal
	{ "JG",  1, ASMEXEC_JG  }, // : A			: Jump to address 'C' if last result was greater than
	{ "JGE", 1, ASMEXEC_JGE }, // : A			: Jump to address 'C' if last result was greater than or equal to
	{ "JL",  1, ASMEXEC_JL  }, // : A			: Jump to address 'C' if last result was less than
	{ "JLE", 1, ASMEXEC_JLE }, // : A			: Jump to address 'C' if last result was less than or equal to

	// Bitwise math
	//							    : ARGS		: DESCRIPTION
	{ "SHL",  2, ASMEXEC_SHL },  // : A, B		: Shifts 'A' left by 'B'.
	{ "SHR",  2, ASMEXEC_SHR },  // : A, B		: Shifts 'A' right by 'B'.
	{ "AND",  2, ASMEXEC_AND },  // : A, B		: Bitwise and
	{ "OR",   2, ASMEXEC_OR  },  // : A, B		: Bitwise or
	{ "XOR",  2, ASMEXEC_XOR },  // : A, B		: Bitwise exclusive or
	{ "NOT",  1, ASMEXEC_NOT },  // : A			: Bitwise not

	// Math
	//							    : ARGS		: DESCRIPTION
	{ "INC",  1, ASMEXEC_INC  },  // : A		: Increment 'A' by one.
	{ "DEC",  1, ASMEXEC_DEC  },  // : A		: Decrement 'B' by one.
	{ "ADD",  2, ASMEXEC_ADD  },  // : A, B		: Add 'A' to 'B'. Stores result in first operand.
	{ "SUB",  2, ASMEXEC_SUB  },  // : A, B		: Subtract 'A' by 'B'. Stores result in first operand.
	{ "IMUL", 2, ASMEXEC_IMUL },  // : A, B		: Multiply 'A' by 'B'. Stores result in first operand.
	{ "IDIV", 1, ASMEXEC_IDIV },  // : A		: Divide EAX by 'A'.

	// Stack
	//							    : ARGS		: DESCRIPTION
	{ "PUSH", 1, ASMEXEC_PUSH }, // : A			: Pushes 'A' onto the stack.
	{ "POP",  1, ASMEXEC_POP  }, // : A			: Retrieves the item at the top of the stack & sets the first operand to it.

	// Common
	//							  : ARGS		: DESCRIPTION
	{ "LEA",  2, ASMEXEC_LEA },  // : A, B		: Set 'A' to the pointer address of 'B'.
	{ "CALL", 1, ASMEXEC_CALL }, // : A			: Call 'A' and return to this position when finished.
	{ "MOV",  2, ASMEXEC_MOV  }, // : A, B		: Store value 'A' in 'B'. Same as "B = A;" in C++.
	{ "RET",  0, ASMEXEC_RET  }, // :			: Stop execution here
	{ "PRINT",1, ASMEXEC_PRINT}, // :			: 
};

const AbstractAssemblyParser::AsmOperation* AbstractAssemblyParser::FetchOpInfoByName(const char* name)
{
	for (int i = 0; i < (sizeof(gBaseOps) / sizeof(gBaseOps[0])); i++)
	{
		// Skip mismatches
		if (strcmp(name, gBaseOps[i].name))
			continue;

		// Found it
		return &gBaseOps[i];
	}

	// Didn't find it
	return NULL;
}

bool AbstractAssemblyParser::AddArgToInstruction(AsmInstruction* pInst, int iArgValue, AsmCompilerState* pState)
{
	// Set it
	pInst->AddArg(iArgValue, true);

	// Done
	return true;
}

//int AbstractAssemblyParser::ResolveNameArg(const char* pValue, AsmCompilerState* pState)
bool AbstractAssemblyParser::ResolveNameArg(const char* pValue, int& iValue, AsmCompilerState* pState)
{
	if (!strcmp(pValue, "EAX")) { iValue = RegisterAddresses::RA_EAX; return true; }
	else if (!strcmp(pValue, "EBX")) { iValue = RegisterAddresses::RA_EBX; return true; }
	else if (!strcmp(pValue, "ECX")) { iValue = RegisterAddresses::RA_ECX; return true; }
	else if (!strcmp(pValue, "EDX")) { iValue = RegisterAddresses::RA_EDX; return true; }
	else if (!strcmp(pValue, "ESI")) { iValue = RegisterAddresses::RA_ESI; return true; }
	else if (!strcmp(pValue, "EDI")) { iValue = RegisterAddresses::RA_EDI; return true; }
	else if (!strcmp(pValue, "ESP")) { iValue = RegisterAddresses::RA_ESP; return true; }
	else if (!strcmp(pValue, "EBP")) { iValue = RegisterAddresses::RA_EBP; return true; }
	else if (!strcmp(pValue, "R0")) { iValue = RegisterAddresses::RA_R0; return true; }
	else if (!strcmp(pValue, "R1")) { iValue = RegisterAddresses::RA_R1; return true; }
	else if (!strcmp(pValue, "R2")) { iValue = RegisterAddresses::RA_R2; return true; }
	else if (!strcmp(pValue, "R3")) { iValue = RegisterAddresses::RA_R3; return true; }
	else if (!strcmp(pValue, "R4")) { iValue = RegisterAddresses::RA_R4; return true; }
	else if (!strcmp(pValue, "R5")) { iValue = RegisterAddresses::RA_R5; return true; }
	else if (!strcmp(pValue, "R6")) { iValue = RegisterAddresses::RA_R6; return true; }
	else if (!strcmp(pValue, "R7")) { iValue = RegisterAddresses::RA_R7; return true; }
	else if (!strcmp(pValue, "PC")) { iValue = RegisterAddresses::RA_PC; return true; }
	
	for (int j = 0; j < pState->pLabelTable->size(); j++)
	{
		ASM_LABEL* pLabel = (*pState->pLabelTable)[j];
		if (strcmp(pLabel->label_name, pValue))
			continue;

		if (pLabel->prev_inst == NULL)
		{
			iValue = 0;
			return true;
		}

		// Find this instruction
		for (int k = 0; k < m_InstCount; k++)
		{
			if (m_InstSet[k] != pLabel->prev_inst)
				continue;

			if (k + 1 >= m_InstCount)
				break;

			iValue = k + 1;
			return true;
		}
	}

	return false;
}

bool AbstractAssemblyParser::GetValueFromAddress(int iAddress, unsigned int& iValue, int iByteCount, AsmExecutionState* pState)
{
	if (iAddress >= 0 && iAddress < (ASM_MAX_STACK * sizeof(int)))
	{
		memcpy(&iValue, ((char*)m_ExecStack.data) + iAddress, iByteCount);
		return true;
	}

	printf("UNHANDLED GET VALUE @ 0x%08X (%d) <%d BYTE%s>\r\n", iAddress, iAddress, iByteCount, iByteCount == 1 ? "" : "s");

	return false;
}

bool AbstractAssemblyParser::SetValueAtAddress(int iAddress, const unsigned int& iValue, int iByteCount, AsmExecutionState* pState)
{
	if (iAddress >= 0 && iAddress < (ASM_MAX_STACK * sizeof(int)))
	{
		memcpy(((char*)m_ExecStack.data) + iAddress, &iValue, iByteCount);
		return true;
	}

	printf("UNHANDLED SET VALUE '%d' @ 0x%08X <%d BYTE%s>\r\n", iValue, iAddress, iByteCount, iByteCount == 1 ? "" : "s");

	return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------