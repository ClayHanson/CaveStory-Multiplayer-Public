#pragma once

#ifdef _CAVEEDITOR
#include <Windows.h>
#include <CaveNet_Config.h>
#else
#include "../src/WindowsWrapper.h"
#endif

#include "AbstractAssemblyParser.h"
#include "../src/Bullet.h"

class BulletAsmParser : public AbstractAssemblyParser
{
public:
	BULLET* m_Bullet;

public:
	virtual bool ResolveNameArg(const char* pValue, int& iValue, AsmCompilerState* pState);
	virtual bool GetValueFromAddress(int iAddress, unsigned int& iValue, int iByteCount, AsmExecutionState* pState);
	virtual bool SetValueAtAddress(int iAddress, const unsigned int& iValue, int iByteCount, AsmExecutionState* pState);
};