#pragma once

/*
 * CaveNet Enumeration Data
 *
 * Holds important enumerations for CaveNet.
 */

namespace CaveNet
{
	enum NetInputFlag
	{
		NET_INPUT_FLAG_JUMP      = 0b000000001,
		NET_INPUT_FLAG_SHOOT     = 0b000000010,
		NET_INPUT_FLAG_LEFT      = 0b000000100,
		NET_INPUT_FLAG_RIGHT     = 0b000001000,
		NET_INPUT_FLAG_UP        = 0b000010000,
		NET_INPUT_FLAG_DOWN      = 0b000100000,
		NET_INPUT_FLAG_ARMS_NEXT = 0b001000000,
		NET_INPUT_FLAG_ARMS_PREV = 0b010000000,
		NET_INPUT_FLAG_COUNT     = 0b100000000

		// Please update this to equal the last enum
	};

	enum UpdateInventoryFlags : unsigned char
	{
		UPDATE_INV_ALL           = (0x01 | 0x02 | 0x04 | 0x08),
		UPDATE_INV_WEAPONS       = 0x01,
		UPDATE_INV_ITEM          = 0x02,
		UPDATE_INV_PERMIT_STAGES = 0x04,
		UPDATE_INV_COLLECTABLES  = 0x08,
	};

	enum VersionIdentifier
	{
		RELEASE = 0,
		DEBUG   = 1
	};

	static const char* VersionIdentStrings[] =
	{
		"", "_d", "_qa"
	};
};