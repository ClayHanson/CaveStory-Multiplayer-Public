#pragma once

/*
	Some opcodes' argument sizes are dependent on how many groups there are, what the maximum group byte size is, etc
*/

enum PxLangOpCode
{
	// No arguments
	PXL_OP_NUL = 0,

	// 01 (Name CRC - Index # to int table) (Group Byte Size)
	PXL_OP_GRP = 1,

	// 02 (Name CRC - Index # to int table) (String Content - Index # to string table)
	PXL_OP_STR = 2
};