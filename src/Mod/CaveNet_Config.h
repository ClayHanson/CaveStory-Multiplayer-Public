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
  @file CaveNet_Config.h
*/

#pragma once

/*
 * CaveNet Configuration File
 *
 * Customize how CaveNet operates.
 */

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CaveNet-specific macros
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// CaveNet Version Management
#define CAVENET_GET_VERSION_PATCH(ver)							(ver & 0xFF)
#define CAVENET_GET_VERSION_MINOR(ver)							((ver >> 8) & 0xFF)
#define CAVENET_GET_VERSION_MAJOR(ver)							((ver >> 16) & 0xFF)
#define CAVENET_GET_VERSION_IDENT(ver)							((ver >> 24) & 0xFF)
#define CAVENET_IS_VERSION_OLDER(OurVer, TheirVer)				((TheirVer & 0x00FFFFFF) < (OurVer & 0x00FFFFFF))
#define CAVENET_IS_VERSION_MISMATCH(OurVer, TheirVer)			((TheirVer & 0x00FFFFFF) != (OurVer & 0x00FFFFFF))
#define CAVENET_IS_VERSION_IDENT_MISMATCH(OurVer, TheirVer)		(CAVENET_GET_VERSION_IDENT(OurVer) != CAVENET_GET_VERSION_IDENT(TheirVer))

// CaveNet Debugging
#ifdef _DEBUG
#define CAVENET_DEBUG_PACKET_DIST		1			// Debug packet distribution (like where it is built, where it was sent from & where it is processed at)
#endif

// CaveNet General
#if defined(_CAVESTORY_MULTIPLAYER_QA)
#define CAVENET_VERSION					0x0201001F 	// The current version of CaveNet
#elif defined(_DEBUG) && defined(CAVENET_DEBUG_PACKET_DIST)
#define CAVENET_VERSION					0x0101001F	// The current version of CaveNet
#else
#define CAVENET_VERSION					0x0001001F	// The current version of CaveNet
#endif

// Versions
#define CAVENET_VER_ADDED_PASSWORDS		0x0001000B
#define CAVENET_VER_ADDED_BUILD_COMP	0x0001001F // Interchangable build compatibility

// Players
#define CAVENET_ANIMNPC_INIT_ACTNO		0			// The act number for the initialization of mAnimNPCs.
#define CAVENET_ANIMNPC_DELETE_ACTNO	230			// The act number for deleting mAnimNPCs.
#define CAVENET_ANIMNPC_CL_DEL_ACTNO	231			// The act number for client-side deletion.
#define CAVENET_ANIMNPC_DEL_WAIT_ACTNO	232			// The act number for client-side waiting for deletion.
#define CAVENET_ANIMNPC_CARRY			390			// The code_char number for the carry npc.

// Pinging
#define CAVENET_PINGING_TIMEOUT			1000		// Default timeout for ping objects

// Packets
#define CAVENET_PACKETS_MAXSIZE			1500		// The max size of a packet. Must be between 8192 and 131072
#define CAVENET_PACKETS_RESERVED_BITS	64			// Amount of reserved header bits for a packet.

// Sockets
#define CAVENET_SOCKET_MAXQUEUE			1024		// The max size of a packet queue for NetSockets.
#define CAVENET_SOCKET_RETRY_MAX		10			// How many retries a failed-to-send-packet is permitted before being discarded.
#define CAVENET_SOCKET_RETRY_DELAY		1000		// Amount of time to wait before re-sending a packet that previously failed to be sent.
#define CAVENET_SOCKET_MAX_ACK_IDS		32			// The maximum number of acknowledgement IDs that a socket can hold.

// Client connection
#define CAVENET_CONNECT_ATTEMPT_TIMEOUT	30000		// Milliseconds before an unresponsive connection is timed out
#define CAVENET_CONNECT_TIMEOUT			30000		// Milliseconds before an unresponsive connection is timed out
#define CAVENET_CONNECT_LAG_THRESHOLD	5000		// Milliseconds before an unresponsive connection is considered 'lagging'
#define CAVENET_CONNECT_MAXRETRY		3			// Maximum retry count for connecting to a server

// Clients
#define CAVENET_CLIENT_PINGCACHE		4			// How many pings to store to calculate a ping average.
#define CAVENET_CLIENT_MAX_ZONES		12			// How many zones a client can occupy at once.
#define CAVENET_CLIENT_MAX_GHOSTS		128			// How many ghosts a client can have pending at once.
#define CAVENET_CLIENT_MAX_REQUESTS		32			// Max manifest requests per packet.
#define CAVENET_CLIENT_MAX_FILE_SIZE	32000		// Max file size for files transmitted across the server connection (in bytes).
#define CAVENET_CLIENT_BASE_PRIORITY	100			// The base priority level for packets that are meant to be important to a specific client.

// PacketType Parsing
#define CAVENET_PACKETPARSE_MAXARGS		32			// The maximum amount of arguments a PacketType Parse function can have.

// Teams
#define CAVENET_INVALID_TEAM_ID			0			// A null ID for teams.

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CaveNet Typedefs
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef char NetClientId;
typedef unsigned short NetPacketId;
typedef unsigned short NetTeamId;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Additional macros
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define INVALID_CLIENT_ID NetClientId(-1)
#define INVALID_PACKET_ID 0xFFFF