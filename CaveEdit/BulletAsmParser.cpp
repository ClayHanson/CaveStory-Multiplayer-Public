#include "BulletAsmParser.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define ADDRESS_OFFSET 0xFFFF

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define DEFINE_FIELD(CLASS, FIELD_NAME, ASM_NAME) { #ASM_NAME, sizeof(CLASS::FIELD_NAME), offsetof(struct CLASS, FIELD_NAME) }
#define FIELD_COUNT (sizeof(gFieldList) / sizeof(gFieldList[0]))

static struct
{
	const char* field_name;
	unsigned int iByteSize;
	unsigned int offset;
} gFieldList[] =
{
	DEFINE_FIELD(BULLET, flag,				B_FLAG),
	DEFINE_FIELD(BULLET, code_bullet,		B_BULLET_CODE),
	DEFINE_FIELD(BULLET, code_arms,			B_ARMS_CODE),
	DEFINE_FIELD(BULLET, bbits,				B_BBITS),
	DEFINE_FIELD(BULLET, cond,				B_COND),
	DEFINE_FIELD(BULLET, x,					B_X),
	DEFINE_FIELD(BULLET, y,					B_Y),
	DEFINE_FIELD(BULLET, xm,				B_XM),
	DEFINE_FIELD(BULLET, ym,				B_YM),
	DEFINE_FIELD(BULLET, tgt_x,				B_TGT_X),
	DEFINE_FIELD(BULLET, tgt_y,				B_TGT_Y),
	DEFINE_FIELD(BULLET, act_no,			B_ACT_NO),
	DEFINE_FIELD(BULLET, act_wait,			B_ACT_WAIT),
	DEFINE_FIELD(BULLET, ani_wait,			B_ANI_WAIT),
	DEFINE_FIELD(BULLET, ani_no,			B_ANI_NO),
	DEFINE_FIELD(BULLET, direct,			B_DIRECT),
	DEFINE_FIELD(BULLET, rect.left,			B_RECT_LEFT),
	DEFINE_FIELD(BULLET, rect.top,			B_RECT_TOP),
	DEFINE_FIELD(BULLET, rect.right,		B_RECT_RIGHT),
	DEFINE_FIELD(BULLET, rect.bottom,		B_RECT_BOTTOM),
	DEFINE_FIELD(BULLET, count1,			B_COUNT_ONE),
	DEFINE_FIELD(BULLET, count2,			B_COUNT_TWO),
	DEFINE_FIELD(BULLET, life_count,		B_LIFE_COUNT),
	DEFINE_FIELD(BULLET, damage,			B_DAMAGE),
	DEFINE_FIELD(BULLET, life,				B_LIFE),
	DEFINE_FIELD(BULLET, enemyXL,			B_ENEMY_XL),
	DEFINE_FIELD(BULLET, enemyYL,			B_ENEMY_YL),
	DEFINE_FIELD(BULLET, blockXL,			B_BLOCK_XL),
	DEFINE_FIELD(BULLET, blockYL,			B_BLOCK_YL),
	DEFINE_FIELD(BULLET, view.left,			B_VIEW_LEFT),
	DEFINE_FIELD(BULLET, view.top,			B_VIEW_TOP),
	DEFINE_FIELD(BULLET, view.right,		B_VIEW_RIGHT),
	DEFINE_FIELD(BULLET, view.bottom,		B_VIEW_BOTTOM)
};

#undef DEFINE_FIELD

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool BulletAsmParser::ResolveNameArg(const char* pValue, int& iValue, AsmCompilerState* pState)
{
	if (AbstractAssemblyParser::ResolveNameArg(pValue, iValue, pState))
		return true;

	// Attempt to resolve it in our field list
	for (int i = 0; i < FIELD_COUNT; i++)
	{
		if (strcmp(pValue, gFieldList[i].field_name))
			continue;

		iValue = ADDRESS_OFFSET + i;
		return true;
	}

	// Couldn't resolve it
	return false;
}

bool BulletAsmParser::GetValueFromAddress(int iAddress, unsigned int& iValue, int iByteCount, AsmExecutionState* pState)
{
	if (iAddress >= ADDRESS_OFFSET && iAddress < ADDRESS_OFFSET + FIELD_COUNT)
	{
		iAddress -= ADDRESS_OFFSET;
		iValue    = 0;
		memcpy(&iValue, (int*)(((unsigned int)m_Bullet) + gFieldList[iAddress].offset), gFieldList[iAddress].iByteSize);
		return true;
	}

	return AbstractAssemblyParser::GetValueFromAddress(iAddress, iValue, iByteCount, pState);
}

bool BulletAsmParser::SetValueAtAddress(int iAddress, const unsigned int& iValue, int iByteCount, AsmExecutionState* pState)
{
	if (iAddress >= ADDRESS_OFFSET && iAddress < ADDRESS_OFFSET + FIELD_COUNT)
	{
		iAddress -= ADDRESS_OFFSET;
		memcpy((int*)(((unsigned int)m_Bullet) + gFieldList[iAddress].offset), &iValue, gFieldList[iAddress].iByteSize);
		return true;
	}

	return AbstractAssemblyParser::SetValueAtAddress(iAddress, iValue, iByteCount, pState);
}