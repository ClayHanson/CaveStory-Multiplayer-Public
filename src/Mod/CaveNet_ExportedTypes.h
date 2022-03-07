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
  @file CaveNet_ExportedTypes.h
*/

#pragma once

#include <CaveNet_Config.h>
#include <CaveNet_EnumData.h>

#include <SortedBinList.h>
#include <List.h>

#include <CSMAPI_begincode.h>

class GameResource;
struct PXDEMO_PLAYER_STRUCT;

#ifdef CAVESTORY_MOD_EXPORTS
#include <SDL_net.h>
#else
typedef struct
{
	unsigned int host;            /* 32-bit IPv4 host address */
	unsigned short port;            /* 16-bit protocol port */
} IPaddress;
#endif

/// @addtogroup CaveNet
/// @{
///
namespace CaveNet
{
	namespace DataStructures
	{
		class PSSortableClient;
		class NetClientVector;
		class SortableClient;
		class AI_ActionBase;
		class NetDownload;
		class NetPacket;
		class NetSocket;
		class NetClient;
		class NetPlayer;
		class AI_Brain;
		class NetTeam;

	};
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Create a team object.
///
/// @return Returns the newly created team object.
extern CAVESTORY_MOD_API void CaveNet_CreateTeam(int iTeamId, bool bClientSide, void* pOut);

/// @brief Get the global timer.
///
/// @return Returns the global timer.
extern CAVESTORY_MOD_API unsigned int CaveNet_Client_GetGlobalTimerOffset();

/// @brief Check if we're in a GUI.
///
/// @return bool Returns true if we're in a server-controlled menu.
extern CAVESTORY_MOD_API bool CaveNet_Client_IsInGui();

/// @brief Check if we're connected to a server.
///
/// @return bool Returns true if we're connected to a server.
extern CAVESTORY_MOD_API bool CaveNet_Client_IsConnected();

/// @brief Get our ghost ID.
///
/// @return bool Returns the local client's ghost ID.
extern CAVESTORY_MOD_API int CaveNet_Client_GetGhostId();

/// @brief Get a client object
///
/// @param int index The index of the client.
///
/// @return NetClient* The client object at the index.
extern CAVESTORY_MOD_API CaveNet::DataStructures::NetClient* CaveNet_Client_ClientGroup_GetClient(int index);

/// @brief Get a client object by their ghost id
///
/// @param int ghost_id The ghost ID.
///
/// @return NetClient* The client object.
extern CAVESTORY_MOD_API CaveNet::DataStructures::NetClient* CaveNet_Client_ClientGroup_GetClientByGhostId(int ghost_id);

/// @brief Get the amount of clients currently cached
///
/// @return int The amount of clients cached
extern CAVESTORY_MOD_API int CaveNet_Client_ClientGroup_GetClientCount();

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Get the server-side global timer.
///
/// @return Returns the global timer.
extern CAVESTORY_MOD_API unsigned int CaveNet_Server_GetGlobalTimerOffset();

/// @brief Check if we're hosting a server.
///
/// @return bool Returns true if we're hosting a server.
extern CAVESTORY_MOD_API bool CaveNet_Server_IsHosting();

/// @brief Check if we're in the lobby.
///
/// @return bool Returns true if we're in the lobby.
extern CAVESTORY_MOD_API bool CaveNet_Server_InLobby();

/// @brief Create a bot.
///
/// @param pBotName The name for this bot.
/// @param pCharName The character name for this bot. Leave as 'NULL' to pick a random character.
/// @param iColorR The bot's custom red value.
/// @param iColorG The bot's custom green value.
/// @param iColorB The bot's custom blue value.
/// @param bRandomColor Whether to use a random color.
///
/// @return Returns the newly created client on success, otherwise returns NULL.
extern CAVESTORY_MOD_API CaveNet::DataStructures::NetClient* CaveNet_Server_ClientGroup_AddBot(const char* pBotName = NULL, const char* pCharName = NULL, unsigned char iColorR = 0, unsigned char iColorG = 0, unsigned char iColorB = 0, bool bRandomColor = false);

/// @brief Get a client object
///
/// @param int index The index of the client.
///
/// @return NetClient* The client object at the index.
extern CAVESTORY_MOD_API CaveNet::DataStructures::NetClient* CaveNet_Server_ClientGroup_GetClient(int index);


/// @brief Get a client object by their ghost id
///
/// @param int ghost_id The ghost ID.
///
/// @return NetClient* The client object.
extern CAVESTORY_MOD_API CaveNet::DataStructures::NetClient* CaveNet_Server_ClientGroup_GetClientByGhostId(int ghost_id);

/// @brief Get the amount of clients currently cached
///
/// @return int The amount of clients cached
extern CAVESTORY_MOD_API int CaveNet_Server_ClientGroup_GetClientCount();

/// @brief Transmit a system message.
///
/// @param pFormat The format of the message.
/// @param ... Additional arguments.
extern CAVESTORY_MOD_API void CaveNet_Server_ClientGroup_TransmitSystemMessage(const char* pFormat, ...);

/// @brief Revive all dead players.
///
extern CAVESTORY_MOD_API void CaveNet_Server_ClientGroup_ReviveAllDeadPlayers();

//------------------------------------------------------------

/// @brief Process networking.
///
/// @param type The type of networking to process. Just use -1.
extern CAVESTORY_MOD_API void CaveNet_ProcessNet(int type = -1);

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		
namespace CaveNet
{
	namespace DataStructures
	{
		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		/// Enumerations

		/// todo: FlagOperation
		///
		enum FlagOperation
		{
			/// Overwrite all flags
			FLAG_OVERWRITE,

			/// Set flags
			FLAG_SET,

			/// Clear flags
			FLAG_CLEAR
		};

		/// todo: ClientState
		///
		enum ClientState
		{
			/// Not connected
			CLIENT_NOT_CONNECTED = 0,

			/// Connecting
			CLIENT_CONNECTING,

			/// Downloading files
			CLIENT_DOWNLOADING,

			/// Connected
			CLIENT_CONNECTED
		};

		/// todo: GUI_Type
		///
		enum GUI_Type
		{
			/// No GUI
			GUI_NONE = 0,

			/// Inventory
			GUI_CAMP,		// Arms, items aka inventory

			/// Map system
			GUI_MAP,		// The portable map system

			/// Max GUI types
			GUI_COUNT
		};

		/// todo: NetDownloadTypes
		///
		enum NetDownloadTypes
		{
			/// Generic file
			NET_DOWNLOAD_GENERIC = 0,

			/// Map download
			NET_DOWNLOAD_MAP = 1,

			/// Max download types
			NET_DOWNLOAD_MAX,
		};

		/// Commands that can be sent to the server
		///
		enum UserCommands
		{
			/// Respawn a player.
			USER_COMMAND_RESPAWN_PLAYER,

			/// Teleport a player to us.
			USER_COMMAND_FETCH_PLAYER,

			/// Teleport to a player.
			USER_COMMAND_FIND_PLAYER,

			/// Spectate a player.
			USER_COMMAND_SPECTATE_PLAYER,

			/// Kick a player.
			USER_COMMAND_KICK_PLAYER,

			/// reserved
			USER_COMMAND_RESERVED_5,

			/// reserved
			USER_COMMAND_RESERVED_6,

			/// reserved
			USER_COMMAND_RESERVED_7,

			/// reserved
			USER_COMMAND_RESERVED_8,

			/// reserved
			USER_COMMAND_RESERVED_9,

			/// reserved
			USER_COMMAND_RESERVED_10,

			/// Number of user commands.
			__USER_COMMAND__COUNT__
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// @brief A sortable client class, to be used with SortedBinList.
		///
		class SortableClient
		{
		public: // Variables

			/// A pointer to the client.
			///
			NetClient* m_Client;

			/// The key.
			///
			int m_Key;

		public: // Constructor

			/// Constructor
			///
			SortableClient(int key)
			{
				m_Client = NULL;
				m_Key    = key;
			}

			/// Deconstructor
			///
			SortableClient()
			{
				m_Key    = 0;
				m_Client = NULL;
			}

		public: // Operators

			/// @protected
			///
			inline bool operator< (const SortableClient& other) const { return m_Key < other.m_Key; }

			/// @protected
			///
			inline bool operator<=(const SortableClient& other) const { return m_Key <= other.m_Key; }

			/// @protected
			///
			inline bool operator> (const SortableClient& other) const { return m_Key > other.m_Key; }

			/// @protected
			///
			inline bool operator>=(const SortableClient& other) const { return m_Key >= other.m_Key; }

			/// @protected
			///
			inline bool operator!=(const SortableClient& other) const { return m_Key != other.m_Key; }

			/// @protected
			///
			inline bool operator==(const SortableClient& other) const { return m_Key == other.m_Key; }
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// @brief A pointer-safe sortable client.
		///
		/// This class is to be used with DynamicSortedBinList and SortedBinList.
		class PSSortableClient
		{
		public: // Variables

			/// The client's ghost id.
			///
			NetClientId m_GhostId;

			/// The key for this.
			///
			int m_Key;

		public: // Constructor

			/// Constructor used to find an entry
			///
			/// @param key ...
			PSSortableClient(int key)
			{
				m_GhostId = NULL;
				m_Key = key;
			}

			/// Real constructor.
			///
			/// @param ghostId ...
			/// @param key ...
			PSSortableClient(NetClientId ghostId, int key)
			{
				m_GhostId = ghostId;
				m_Key     = key;
			}

			/// Default constructor
			///
			PSSortableClient()
			{
				m_GhostId = NULL;
				m_Key = 0;
			}

		public: // Methods

			/// @brief Get the client associated with this.
			///
			/// @param Server Whether or not to check the server-side client container instead of the client-side client container.
			///
			/// @returns NetClient* Returns the found client. Returns NULL if no client with this ghost ID could be found.
			NetClient* GetClient(bool Server);

			/// @brief Get the client associated with this.
			///
			/// @param Server Whether or not to check the server-side client container instead of the client-side client container.
			///
			/// @returns NetClient* Returns the found client. Returns NULL if no client with this ghost ID could be found.
			NetClient* GetClient(bool Server) const;

		public: // Operators
			/// @protected
			///
			inline bool operator< (const PSSortableClient& other) const { return m_Key < other.m_Key; }

			/// @protected
			///
			inline bool operator<=(const PSSortableClient& other) const { return m_Key <= other.m_Key; }

			/// @protected
			///
			inline bool operator> (const PSSortableClient& other) const { return m_Key > other.m_Key; }

			/// @protected
			///
			inline bool operator>=(const PSSortableClient& other) const { return m_Key >= other.m_Key; }

			/// @protected
			///
			inline bool operator!=(const PSSortableClient& other) const { return m_Key != other.m_Key; }

			/// @protected
			///
			inline bool operator==(const PSSortableClient& other) const { return m_Key == other.m_Key; }
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// @brief Player teams.
		///
		class CAVESTORY_MOD_API NetTeam
		{
			friend class NetClient;
			friend class NetPlayer;

		public: // Enumeration

			enum TeamType
			{
				TEAM_YELLOW = 0,
				TEAM_GREEN  = 1,
				TEAM_RED    = 2,
				TEAM_BLUE   = 3
			};

		public: // Structs

			/// Stores information for spawning weapons.
			///
			struct NetTeamSpawnWeapon
			{
				/// The ID of the weapon.
				///
				int weapon_id;

				/// The level of the weapon.
				///
				unsigned char level;

				/// The starting ammo.
				///
				int ammo;

				/// The maximum ammo.
				///
				int max_ammo;
			};

			/// Controls NetTeam configuration.
			///
			struct NetTeamConfig
			{
				/// Whether to allow friendly fire or not.  Default is false.
				///
				bool friendly_fire;

				/// Force outfit color
				///
				bool force_outfit_color;

				/// Enable leveling up weapons. Default is true.
				///
				bool enable_leveling_weapons;

				/// Damage multiplier for the amount of damage team members deal to other entities / players. Default is 1.
				///
				float damage_out_mult;

				/// Damage multiplier for the amount of damage team members take. Default is 1.
				///
				float damage_in_mult;

				/// The weapons team members spawn with.
				///
				NetTeamSpawnWeapon spawn_weapons[ARMS_MAX];

				/// Initial equip flags for team members.
				///
				unsigned int equip_flags;

				/// The maximum life points that team members spawn with.
				///
				unsigned int max_life;

				/// The score for this team.
				///
				int score;

				/// Maximum amount of members for this team.
				///
				int max_members;

				/// Whether this team can be autobalanced to / from.
				///
				bool allow_autobalance;
			};

		public: // Linkage

			/// First team in the client-sided link.
			///
			static NetTeam* client_first;

			/// First team in the server-sided link.
			///
			static NetTeam* server_first;

			/// Total count of server-side teams.
			///
			static unsigned int server_count;

			/// Total count of client-side teams.
			///
			static unsigned int client_count;

			/// Next team in the link.
			///
			NetTeam* next;

			/// Previous team in the link.
			///
			NetTeam* prev;

		private: // Private variables

			/// The newest team id.
			///
			static NetTeamId next_team_id;

		protected: // Variables

			/// The team net ID. This is sent over the network instead of the team name (if we can help it).
			///
			NetTeamId mTeamId;

			/// The team name.
			///
			char* mName;
			
			/// The team color.
			///
			GUI_COLOR mColor;

			/// The team color in hex.
			///
			char mHexColor[7];

			/// The clients in this team.
			///
			List<NetClient*> mClientList;

			/// Stores whether this team is clientside or serverside.
			///
			bool mIsClientSide;

			/// The NPC index of the spawn object.
			///
			int mSpawnNpcIndex;

		public: // NPCs

			/// The team type.
			///
			TeamType mTeamType;

			/// A list of NPCs that represent spawn points.
			///
			List<NPCHAR*> mSpawnPoints;

			/// This team's CTP flag.
			///
			NPCHAR* mFlagNpc;

			/// This team's CTP capture zone.
			///
			NPCHAR* mCaptureZoneNpc;

		public: // Public configuration

			/// This team's configuration.
			///
			NetTeamConfig mConfig;

		public: // C++ Stuff

			/// Constructor
			///
			NetTeam();
			
			/// Client-side constructor
			///
			NetTeam(NetTeamId iTeamId, bool bClientSide = true);

			/// Deconstructor
			///
			~NetTeam();

		protected: // Link methods

			/// @brief Link this team to the rest.
			///
			void Link();

			/// @brief Unlink this team from the rest.
			///
			void Unlink();

		public: // Team info methods

			/// @brief Reset this team's configuration.
			///
			void ResetConfig();

			/// @brief Set the team's name.
			///
			/// @param pNewName The new team name.
			void SetName(const char* pNewName);

			/// @brief Set the team's color.
			///
			/// @param pColor The new team color.
			void SetColor(GUI_COLOR pColor);

			/// @brief Set the NPC spawn index.
			///
			/// @param iIndex The index of the spawn NPC.
			void SetNpcSpawnIndex(int iIndex);

		public: // Inline team info methods

			/// @brief Get this team's name.
			///
			/// @return Returns this team's color.
			inline const char* GetName() { return mName; }

			/// @brief Get this team's color.
			///
			/// @return Returns this team's color.
			inline GUI_COLOR GetColor() { return mColor; }

			/// @brief Get this team's color in hexidecimal.
			///
			/// @return Returns this team's color in hexidecimal format.
			inline const char* GetHexColor() { return mHexColor; }

			/// @brief Get this team's ID.
			///
			/// @return Returns this team's ID.
			inline NetTeamId GetTeamId() { return mTeamId; }

			/// @brief Get whether this team is client-sided or not.
			///
			/// @return Returns true if this team exists client-side, false if it exists server-side.
			inline bool IsClientSide() { return mIsClientSide; }

			/// @brief Get the spawn index.
			///
			/// @return Returns the spawn npc index.
			inline int GetNpcSpawnIndex() { return mSpawnNpcIndex; }

		public: // Member management methods

			/// @brief Add a spawn point to this team.
			///
			/// @param iNpcIdx The index of the NPC in gNPC.
			///
			/// @return Returns true on success, false otherwise.
			bool AddSpawnPoint(int iNpcIdx);

			/// @brief Get the spawn position for this team.
			///
			/// @return Returns a spawn position.
			GUI_POINT GetSpawnPosition();

			/// @brief Spawn a player.
			///
			/// @param pClient The team member to spawn.
			void SpawnClient(NetClient* pClient);

		public: // Member management methods

			/// @brief Check if a client is a member of this team.
			///
			/// @param pClient The client to check.
			///
			/// @return Returns true if the given client is apart of this team.
			bool IsMember(NetClient* pClient);

			/// @brief Add a client to this team.
			///
			/// @param pClient The client to add.
			/// @param bAnnounce Whether to announce the client's join or not.
			///
			/// @return Returns true on success, false otherwise.
			bool AddMember(NetClient* pClient, bool bAnnounce = true);

			/// @brief Remove a member for this team.
			///
			/// @param pClient The client to remove.
			/// @param bAnnounce Whether to announce the client's departure or not.
			///
			/// @return Returns true on success, false otherwise.
			bool RemoveMember(NetClient* pClient, bool bAnnounce = true);

			/// @brief Get this team's member count.
			///
			/// @return Returns this team's member count.
			int GetMemberCount();

			/// @brief Get a member of this team.
			///
			/// @param iIndex The index of the member.
			///
			/// @return Returns a pointer to a NetClient* on success, else returns NULL if index is out of bounds.
			NetClient* GetMember(int iIndex);

			/// @brief Clear all members from this team.
			///
			void ClearMembers();

		public: // Static methods
			
			/// @brief Delete all teams.
			///
			static void ClearAllTeams();

			/// @brief Find a team by name.
			///
			/// @param pName The name of the team we want to find.
			/// @param bClientSide Whether to search the client-sided array or the server-sided array.
			///
			/// @return Returns the team if it exists, NULL otherwise.
			static NetTeam* FindTeamByName(const char* pName, bool bClientSide);
			
			/// @brief Find a team by id.
			///
			/// @param iTeamId The ID of the team we want to find.
			/// @param bClientSide Whether to search the client-sided array or the server-sided array.
			///
			/// @return Returns the team if it exists, NULL otherwise.
			static NetTeam* FindTeamById(NetTeamId iTeamId, bool bClientSide);
			
			/// @brief Find a team by type.
			///
			/// @param iTeamType The type of the team we want to find.
			/// @param bClientSide Whether to search the client-sided array or the server-sided array.
			///
			/// @return Returns the team if it exists, NULL otherwise.
			static NetTeam* FindTeamByType(TeamType iTeamType, bool bClientSide);
			
			/// @brief Fetch a team by index.
			///
			/// @param iTeamIndex The index of the team to get.
			/// @param bClientSide Whether to search the client-sided array or the server-sided array.
			///
			/// @return Returns the team if it exists, NULL otherwise.
			static NetTeam* GetTeamByIndex(int iTeamIndex, bool bClientSide);

			/// @brief Check to see if teams are unbalanced.
			///
			/// @param iHighestTeam A reference to an int that will store the heaviest team's ID.
			/// @param iHighestTeam A reference to an int that will store the lightest team's ID.
			///
			/// @return Returns true if teams are unbalanced, false otherwise.
			static bool AreTeamsUnbalanced(NetTeamId& iHighestTeam, NetTeamId& iLightestTeam);

			/// @brief Autobalance teams.
			///
			/// @param bOnlySwitchDeadPlayers Only switch dead players.
			static void BalanceTeams(bool bOnlySwitchDeadPlayers = false);
			
			/// @brief Get the next free team ID.
			///
			/// @param bClientSide Whether to reference the client-side array or the server-side array.
			///
			/// @return Returns the next free Team ID.
			static NetTeamId GetNextFreeTeamId(bool bClientSide);

		public: // Networking

			/// @brief Write a team packet.
			///
			/// @param pPacket The packet to write to.
			void WritePacket(NetPacket* pPacket);

			/// @brief Size up a team packet.
			///
			/// @return Returns the size of the packet, in bits.
			unsigned int SizePacket();
			
#ifdef _DEBUG
			/// @brief Query a packet.
			///
			/// @param pPacket The packet to read.
			void QueryPacket(NetPacket* pPacket, void* pAddArgFunc);
#endif

			/// @brief Read a team packet.
			///
			/// @param pPacket The packet to read from.
			void ReadPacket(NetPacket* pPacket);

			/// @brief Send a team update to all clients.
			///
			void UpdateTeam();

		public: // Gui

			/// @brief Opens an easy-to-use dialog that will allow the host to go through teams.
			///
			/// @return Returns the resulting error code.
			static int OpenTeamListMenu();
			
			/// @brief Opens an easy-to-use dialog that will allow the host to add or edit a team.
			///
			/// @param pTeam The team to edit. If NULL, a new team will be created.
			///
			/// @return Returns the resulting error code.
			static int OpenTeamEditMenu(NetTeam* pTeam = NULL);
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// Stores changed map tile information
		///
		struct CAVESTORY_MOD_API NetClient_MapTileChange
		{
			/// The index in gMap.tiles of the tile.
			///
			int tile_index;

			/// The new tile index to be set
			///
			short new_tile_id;

			/// @brief For SortedBinList compatibility
			///
			/// @param other The other class to compare ourselves to.
			///
			/// @return Returns the result of the comparison.
			inline bool operator==(const NetClient_MapTileChange& other) const { return tile_index == other.tile_index; }

			/// @brief For SortedBinList compatibility
			///
			/// @param other The other class to compare ourselves to.
			///
			/// @return Returns the result of the comparison.
			inline bool operator!=(const NetClient_MapTileChange& other) const { return tile_index != other.tile_index; }

			/// @brief For SortedBinList compatibility
			///
			/// @param other The other class to compare ourselves to.
			///
			/// @return Returns the result of the comparison.
			inline bool operator< (const NetClient_MapTileChange& other) const { return tile_index < other.tile_index; }

			/// @brief For SortedBinList compatibility
			///
			/// @param other The other class to compare ourselves to.
			///
			/// @return Returns the result of the comparison.
			inline bool operator<=(const NetClient_MapTileChange& other) const { return tile_index <= other.tile_index; }

			/// @brief For SortedBinList compatibility
			///
			/// @param other The other class to compare ourselves to.
			///
			/// @return Returns the result of the comparison.
			inline bool operator> (const NetClient_MapTileChange& other) const { return tile_index > other.tile_index; }

			/// @brief For SortedBinList compatibility
			///
			/// @param other The other class to compare ourselves to.
			///
			/// @return Returns the result of the comparison.
			inline bool operator>=(const NetClient_MapTileChange& other) const { return tile_index >= other.tile_index; }

			/// Constructor
			///
			NetClient_MapTileChange()
			{
				tile_index = 0;
				new_tile_id = 0;
			}

			/// Constructor
			///
			NetClient_MapTileChange(int index)
			{
				NetClient_MapTileChange();
				tile_index = index;
			}
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// Stores statistics of a connection.
		///
		struct CAVESTORY_MOD_API NetStats
		{
			/// Total number of packets received over the lifetime of a connection.
			///
			int total_packets;

			/// Total number of bytes received overall
			///
			int total_bytes;

			/// Number of packets parsed last read.
			///
			int step_packets;

			/// Numbers of bytes received last read.
			///
			int step_bytes;

			/// Reset the statistics
			///
			inline void Reset()
			{
				total_packets = 0;
				total_bytes   = 0;
				step_packets  = 0;
				step_bytes    = 0;
			}
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// Uniform ping calculation
		///
		class CAVESTORY_MOD_API NetPing
		{
		public: // Variables
			/// Last ping time.
			///
			int mLastTime;
			
			/// Last sent ping time.
			///
			int mLastPingTime;

			/// Last packet receive time
			///
			int mLastReceiveTime;

			/// Expecting a pong?
			///
			bool mExpectingPong;

			/// The current index in #mAmount.
			///
			int mCurrent;

			/// The average ping.
			///
			int mAverage;

			/// List of the amount of time taken for round-trip pings.
			///
			int mAmount[CAVENET_CLIENT_PINGCACHE];

			/// Number of pings.
			///
			int mCount;

		public: // Constructor

			/// Constructor
			///
			NetPing();

		public: // Manipulation methods

			/// @brief Reset.
			///
			void Reset();

			/// @brief Notify of ping.
			///
			void DoPing();

			/// @brief Notify of missed ping.
			///
			void MissPing();

		public: // Data query methods

			/// @brief Check if this object is considered 'lagging'.
			///
			/// @return bool Returns true if this object is considered lagging.
			bool IsLagging();

			/// @brief Check if this object can be timed out.
			///
			/// @return bool Returns true if this object can be timed out.
			bool IsTimedOut();

			/// @brief Get the average ping.
			///
			/// @return int Returns the average ping.
			int GetAverage();
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		/// Stores ghosting information for a single NPC.

		/// A class for storing client-specific ghost information for NPCs.
		///
		class CAVESTORY_MOD_API GhostInfo
		{
		public: // Variables

			/// The NPC index in #gNPC.
			///
			short npc_idx;

			/// How many ticks have passed since this ghost info was created. Used to score which ghost object gets scoped next.
			///
			int ticks_alive;

		public: // Constructor

			/// Constructor
			///
			GhostInfo(int iNpcIDX, float iDistance)
			{
				ticks_alive = 0;
				npc_idx     = iNpcIDX;
			}

			/// Constructor 2
			///
			GhostInfo()
			{
				ticks_alive = 0;
				npc_idx     = 0;
			}

		public: // Operators

			/// @brief For SortedBinList compatibility
			///
			/// @param other The other class to compare ourselves to.
			///
			/// @return Returns true if our ticks_alive are less than the other ghost's ticks_alive.
			inline bool operator< (const GhostInfo& other) const { return ticks_alive  < other.ticks_alive; }

			/// @brief For SortedBinList compatibility
			///
			/// @param other The other class to compare ourselves to.
			///
			/// @return Returns true if our ticks_alive are less than or equal to the other ghost's ticks_alive.
			inline bool operator<=(const GhostInfo& other) const { return ticks_alive <= other.ticks_alive; }

			/// @brief For SortedBinList compatibility
			///
			/// @param other The other class to compare ourselves to.
			///
			/// @return Returns true if our ticks_alive are greater than the other ghost's ticks_alive.
			inline bool operator> (const GhostInfo& other) const { return ticks_alive  > other.ticks_alive; }

			/// @brief For SortedBinList compatibility
			///
			/// @param other The other class to compare ourselves to.
			///
			/// @return Returns true if our ticks_alive are greater than or equal to the other ghost's ticks_alive.
			inline bool operator>=(const GhostInfo& other) const { return ticks_alive >= other.ticks_alive; }

			/// @brief For SortedBinList compatibility
			///
			/// @param other The other class to compare ourselves to.
			///
			/// @return Returns true if our ticks_alive are not equal to the other ghost's ticks_alive.
			inline bool operator!=(const GhostInfo& other) const { return ticks_alive != other.ticks_alive; }

			/// @brief For SortedBinList compatibility
			///
			/// @param other The other class to compare ourselves to.
			///
			/// @return Returns true if our ticks_alive are equal to the other ghost's ticks_alive.
			inline bool operator==(const GhostInfo& other) const { return ticks_alive == other.ticks_alive; }
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// AI Controls
		///
		struct AI_Controls
		{
			/// Go left
			///
			bool bLeft;

			/// Go right
			///
			bool bRight;

			/// Look up
			///
			bool bUp;

			/// Look down / search
			///
			bool bDown;

			/// Jump
			///
			bool bJump;

			/// Shoot
			///
			bool bShoot;

			/// Previous weapon
			///
			bool bArmsPrev;

			/// Next weapon
			///
			bool bArmsNext;
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// AI Player's action base
		///
		class CAVESTORY_MOD_API AI_ActionBase
		{
		public:

			/// The brain this action is performing for
			///
			AI_Brain* pBrain;

		public:

			/// @brief Constructor
			///
			AI_ActionBase()
			{
				pBrain = NULL;
			}

		public:

			/// @brief Called when this action is started
			///
			virtual bool OnBegin() = 0;

			/// @brief Called when this action finishes
			///
			virtual void OnFinish() = 0;

			/// @brief Whether this action is complete or not
			///
			virtual bool IsDone() = 0;

			/// @brief Called every frame
			///
			virtual void Tick(AI_Controls* pControls) = 0;
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// AI Player's brain
		///
		class CAVESTORY_MOD_API AI_Brain
		{
			friend class NetPlayer;

		public: // Callbacks

			void(*m_OnCleanup)(AI_Brain* pBrain);

		public: // Variables

			/// ... 
			///
			void* ptr_1;

			/// ... 
			///
			void* ptr_2;

			/// ... 
			///
			int act_no;

			/// ... 
			///
			int act_wait;
			
			/// ... 
			///
			int count1;
			
			/// ... 
			///
			int count2;

			/// ... 
			///
			int count3;

			/// ... 
			///
			int count4;

			/// ... 
			///
			int count5;

			/// ... 
			///
			int count6;

			/// ... 
			///
			int count7;

			/// ... 
			///
			int count8;

			/// ... 
			///
			int timer_1;

			/// ... 
			///
			int timer_2;

			/// ... 
			///
			int tgt_x;

			/// ... 
			///
			int tgt_y;

			/// ... 
			///
			int start_time;

			/// ...
			///
			void* m_CustomVarList[32];

		public: // Actions

			/// The action queue for this brain.
			///
			List<AI_ActionBase*> m_ActionQueue;

			/// The player attached to this brain.
			///
			NetPlayer* m_Player;

			/// Selected weapon
			///
			int m_SelectedWeapon;

		public:

			/// @brief Constructor
			///
			AI_Brain();

			/// @brief Deconstructor
			///
			~AI_Brain();

		public: // Methods

			/// @brief Add an action to the queue.
			///
			AI_ActionBase* AddAction(AI_ActionBase* pAction);

		public: // Bot methods

			/// Think for this bot.
			///
			void Think();

			/// Tick this bot for one frame.
			///
			int Tick();
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// @brief Networking client
		///
		/// Handles a client that is currently connected to us.
		class CAVESTORY_MOD_API NetClient
		{
		public: // Friends
			friend class NetClientVector;
			friend class NetPlayer;
			friend class NetTeam;

		public: // Enumerations
			enum ClientState
			{
				/// This client isn't connected to anything
				CLIENT_DISCONNECTED = 0,

				/// This client is a client connected to us
				CLIENT_CONNECTED,

				/// This client is a client-side ghost of another client
				CLIENT_GHOST
			};

			struct ClientColor
			{
				/// Red
				///
				char r;

				/// Green
				///
				char g;

				/// Blue
				///
				char b;
			};

		public: // Client cache
			struct CLIENT_CACHE
			{
				/// This client's cached playercolor
				///
				ClientColor char_color;

				/// Whether to use this client's cached playercolor
				///
				bool use_color;

				/// do not use
				///
				char ghostlist_lookup_table[((NPC_MAX + BOSS_MAX) + 7) >> 3];

				/// do not use
				///
				List<NetClient_MapTileChange> tile_change_list;

				/// This client's ghosting queue.
				///
				List<GhostInfo> ghostlist;

				/// A list of zones this client's player is currently occupying.
				///
				SortedBinList<int, CAVENET_CLIENT_MAX_ZONES> zonelist;

				/// This client's permit stages.
				///
				PERMIT_STAGE permitStage[PERMIT_STAGE_COUNT];

				/// deprecated
				///
				///BULLET bulletData[BULLET_MAX];

				/// An array of this client's weapons.
				///
				ARMS armsData[ARMS_MAX];

				/// An array of this client's items.
				///
				ITEM itemData[ITEM_MAX];

				/// The name of this client's preferred character.
				///
				char character_name[256];

				/// Whether this client is initialized or not
				///
				bool is_initialized;

				/// Whether this client is spectating or not.
				///
				bool is_spectating;

				/// Whether this client was spectating or not.
				///
				bool is_old_spectator;

				/// This client's spectate target.
				///
				NetClientId current_spectate_trg;

				/// This client's old spectate target.
				///
				NetClientId old_spectate_trg;

				/// This client's currently selected weapon.
				///
				int currentWeapon;

				/// Last ping lag time
				///
				int lastPingLagTime;

				/// Soft rensha; for weapons
				///
				int soft_rensha;

				/// This client's game flags.
				///
				/// @see GameFlags
				int gameFlags;

				/// Old game flags
				///
				/// @see GameFlags
				int oldGameFlags;

				/// Empty
				///
				int empty;

				/// Current map index
				///
				int curr_map;

				/// When to send a ping packet
				///
				char sendPingPacket;

				/// Whimsical star data
				///
				WHIMSICAL_STAR star[MAX_WHIMSICAL_STARS];

				/// Star update
				///
				int star_update;
				
				/// Cache star count
				///
				int star_cached;

				/// This client's spur charge.
				///
				int spur_charge;

				/// Whether this client's spur charge is at maximum.
				///
				bool spur_max;

				/// Whether this client has finalized initialization.
				///
				bool has_finalized;

				/// Last movement time (using SDL_GetTicks())
				///
				long long int lastMoveTime;
				
				/// Last time input changed (using CaveNet::Server::GetGlobalTimerOffset())
				///
				unsigned long long int lastInputChangeTime;

				/// Last move input keys
				///
				unsigned int lastMoveInputKeys;

				/// HP bar timer before it dissapears
				int hp_bar_wait;

				/// Chat emote index
				///
				int chat_emote_index;

				/// Chat emote timer
				///
				int chat_emote_timer;

				/// Chat emote spam amount
				///
				int chat_emote_spam_amount;

				/// Chat emote spam cooldown
				///
				int chat_emote_spam_cooldown;

				/// Chat emote Y offset
				///
				int chat_emote_yoffset;

				/// The client being carried by us.
				///
				NetClientId carried_by_ghost_id;

				/// The client we're carrying.
				///
				NetClientId carrying_ghost_id;

				/// This client's predicted camera X
				///
				int frame_x;

				/// This client's predicted camera Y
				///
				int frame_y;

				/// This client's name
				///
				char net_name[127];

				/// Whether this is a client-side client or a server-side client.
				///
				bool is_ghost;

				/// Whether this is our local player.
				///
				bool is_our_player;

				/// The hash of CaveNet::DataStructures::NetClient::CLIENT_CACHE::net_name.
				///
				int net_name_hash;

				/// Total bytes received.
				///
				int bytes_received;

				/// Total bytes sent.
				///
				int bytes_sent;

				/// Total Bytes lost.
				///
				int bytes_lost;

				/// Old key
				///
				int key_old;

				/// Key inputs (trg)
				///
				int keyTrg;

				/// Key inputs
				///
				int key;

				/// Old game keys
				///
				int gameKeyOld;

				/// KeyTrg
				///
				/// @see CSM_KEY_DESC::CSM_KEY_DESC_KEYTRG
				int gameKeyTrg;

				/// Key
				///
				/// @see CSM_KEY_DESC::CSM_KEY_DESC_KEY
				int gameKey;

				/// The index number of the map where this client died.
				///
				int death_map;

				/// Persistent death NPC.
				///
				GUI_POINT death_position;

				/// Ping object for this client
				///
				NetPing ping;

				/// Got packet on this tick
				///
				bool got_packet;

				/// Which GUI this client has opened.
				///
				/// @see CaveNet::DataStructures::GUI_Type
				int OpenedGUI;

				/// This client's calculated ping (for client-side clients only)
				///
				int ClientSide_AveragePing;

				/// Whether this client is deemed as 'lagging' (for client-side clients only)
				///
				bool ClientSide_Lagging;

				/// Whether this client is downloading files (for client-side clients only)
				///
				bool ClientSide_Downloading;

				/// Download files left for this client (for client-side clients only)
				///
				int ClientSide_Download_FilesLeft;

				/// This client's name rect in SURFACE_ID_NET_NAME_PLANE.
				///
				struct
				{
					/// X position
					///
					int x;

					/// Y position
					///
					int y;

					/// Width
					///
					int w;

					/// Height
					///
					int h;
				} name_rect;

				/// "Signal Strength Index". Index in GUI_SourceRects::rc_IMAGE_SignalStrength .
				///
				unsigned char ssidx;

				/// Our player that's being recorded
				///
				PXDEMO_PLAYER_STRUCT* recorder_player;

				/// The last time we switched teams
				///
				int last_team_switch_time;

				/// Our creation time
				///
				int create_time;

				/// Allow char move
				///
				char allow_char_move;

				/// Collectable count
				///
				int collectable_count;

				/// Collectables shown
				///
				bool collectables_shown;

				/// ... 
				///
				int puppy_spawn_time;

				/// ...
				///
				int basil_spawn_time;

				/// ...
				///
				unsigned int respawn_time;

				/// Last time we interacted with a supply cabinet
				///
				unsigned int last_supply_time;
			} mCache;

			struct
			{
				/// Whether this client wants to download resources or not
				///
				bool DownloadResources;
			} mClientConfig;

			struct
			{
				/// Download queue
				///
				NetDownload** list;

				/// How many files are in the queue
				///
				int count;

				/// Whether the client has already started downloading or not
				///
				bool is_initialized;

				/// Whether the client is ready for the next file chunk
				///
				bool is_ready;
			} mDownloadQueue;

			struct
			{
				/// Resource scope queue
				///
				GameResource** list;

				/// How many resources are left to scope
				///
				int count;

				/// Whether or not we're done scoping resources
				///
				bool is_done;

				/// Whether we sent a resource
				///
				bool was_sent;

				/// Whether we started already
				///
				bool did_start;

				/// Clean list
				///
				char* clean_list;

				/// Size of clean list
				///
				int clean_list_size;
			} mManifestQueue;

			/// Our connection port. NEVER change this.
			///
			int mConn_Port;

			/// Whether this client is local (same-application) or not.
			///
			bool mIsLocal;

			/// Whether this client is a bot or a real user.
			///
			bool mIsBot;

			/// Whether this client is an admin or not.
			///
			bool mIsAdmin;

		protected: // Variables

			/// The team we're apart of.
			/// @protected
			NetTeam* mTeam;

			/// This client's player object.
			/// @protected
			NetPlayer* mPlayer;

			/// The socket this client is using to communicate to us.
			/// @protected
			NetSocket* mSocket;

			/// This client's IP address.
			/// @protected
			IPaddress mAddress;

			/// A pointer to the clientgroup that this client is stored within.
			/// @protected
			NetClientVector* mVector;

			/// This client's Ghost ID.
			/// @protected
			///
			/// @see CaveNet::DataStructures::NetClient::GetGhostId()
			NetClientId mGhostId;

			/// The state of this client.
			/// @protected
			ClientState mState;

			/// The channel number to communicate on.
			/// @protected
			int mChannelNum;

			/// The safe client interface that belongs to this client.
			///
			SafeClientInterface mInterface;

		public: // Gameplay related variables

			/// I'm ready!
			///
			bool mIsReady;

		public: // Constructors

			/// Default constructor
			///
			NetClient();

			/// Default deconstructor
			///
			~NetClient();

		protected: // Protected methods

			/// @brief Set the state of the client.
			///
			/// @param state The new state of the client.
			inline void SetState(ClientState state) { mState = state; }

			/// @brief Set the vector pointer
			///
			inline void SetVector(NetClientVector* ptr) { mVector = ptr; }

			/// @brief Set the player pointer
			///
			inline void SetPlayer(NetPlayer* ptr) { mPlayer = ptr; }

		public: // Resource management

			/// @brief Queue a resource to be checked w/ this client
			///
			/// @param pResource The resource to queue.
			///
			/// @return Returns true if the resource was queue'd successfully, false otherwise.
			bool QueueResource(GameResource* pResource);

			/// @brief Check if a resource is dirty for this client.
			///
			/// @param pResource The resource to check.
			/// @param iIndex The index to start searching at.
			///
			/// @return Returns true if the specified resource is dirty.
			/// @note Dirty resources will be sent to the client to be updated.
			bool IsResourceDirty(GameResource* pResource, int iIndex = -1);

			/// @brief Set a resource as dirty
			///
			/// @param pResource The resource to set the dirtiness of.
			/// @param bValue If true, dirty pResource. If false, clean pResource.
			///
			/// @return Returns true if the operation was performed successfully, false otherwise.
			bool SetResourceDirty(GameResource* pResource, bool bValue, int iIndex = -1);

			/// @brief Send this client all still-dirty manifest items
			///
			void SendDirtyManifest();

			/// @brief Clear this client's manifest
			///
			void ClearManifest();

			/// @brief Set all items in this client's manifest cache as 'dirty'
			///
			void SetManifestDirty();

			/// @brief Download the next resource
			///
			/// @param deleteFirst Whether to delete the next resource in the queue or not.
			///
			/// @return Returns true if there is/was a resource in the queue.
			bool NextResource(bool deleteFirst = true);

			/// @brief Send a chunk of the download manifest to the client.
			///
			void TickManifestQueue();

		public: // Netdownloading

			/// @brief Queue a download
			///
			/// @param ptr The download to queue.
			///
			/// @param Returns true if the download was queued successfully.
			bool QueueDownload(NetDownload* ptr);

			/// @brief Clear the client's download queue
			///
			void ClearDownloadQueue();

			/// @brief Finish the current download & go to the next one
			///
			/// @returns Returns true if another download has been started, false if there are no downloads left.
			bool NextDownload(bool deleteFirst = true);

			/// @brief Tick downloading
			///
			void TickDownloadQueue();

			/// @brief Check to see if this client is downloading.
			///
			/// @return Returns true if this client is downloading files, false otherwise.
			bool IsDownloading();

			/// @brief Get the progress of the current download in string form
			///
			/// @param out A pointer to a buffer to store the string.
			/// @param out_size The size of ' out '.
			void BuildDownloadString(char* out, int out_size);

		public: // Safe methods

			/// @brief Get an interface for this client.
			///
			/// @return Returns an interface for this client.
			/// @warning Do not keep a reference to this interface! It will point to an unallocated area when the client leaves!
			SafeClientInterface* GetInterface();

		public: // Team methods

			/// @brief Calculate a team balance score.
			///
			/// @return Returns a score.
			int GetTeamBalanceScore();

			/// @brief Determine whether this client can be auto balanced or not.
			///
			/// @return Returns true if they can be autobalanced, false otherwise.
			bool CanBeAutoBalanced();

			/// @brief Get this client's team.
			///
			/// @return Returns a NetTeam* pointer if the client is in a team, NULL otherwise.
			inline NetTeam* GetTeam() { return mTeam; }

			/// @brief Get this client's team id.
			///
			/// @return Returns this client's team id.
			inline int GetTeamId() { return (!mTeam ? -1 : mTeam->GetTeamId()); }

		public: // Game methods

			/// @brief Has max EXP for the current weapon?
			///
			/// @return Returns true if this client is at max EXP & level for their held weapon.
			bool IsMaxExp();

			/// @brief Update gamekey and gamekeytrg.
			///
			void GetTrg();

			/// @brief Spawn this client at a set of coordinates.
			///
			/// @param int x The X coordinate.
			/// @param int y The Y coordinate.
			void Spawn(int x, int y);

			/// @brief Find a suitable place to spawn & spawn there.
			///
			void SpawnAuto();

			/// @brief Respawn this client in a fixed amount of time.
			///
			/// @param iTime The amount of time (in milliseconds) to wait before respawning.
			void Respawn(int iTime);

			/// @brief Clear all arms data
			///
			void ClearArmsData();

			/// @brief Clear all item data
			///
			void ClearItemData();

			/// @brief Add arms data
			///
			/// @param code The weapon ID to add
			/// @param max_num The maximum ammo for this weapon.
			///
			/// @return Returns true on success, false otherwise.
			bool AddArmsData(long code, long max_num);
			
			/// @brief Add arms data
			///
			/// @param code The weapon ID to add
			/// @param num The amount of ammo for this weapon.
			/// @param max_num The maximum ammo for this weapon.
			/// @param level The level of the weapon.
			///
			/// @return Returns true on success, false otherwise.
			bool AddArmsData2(long code, long num, long max_num, long level);

			/// @brief Check to see if this client has a weapon.
			///
			/// @param code The weapon to check for
			///
			/// @return Returns true if the client has the weapon, false otherwise.
			bool HasArmsData(long code);

			/// Add arms data (exact)
			///
			bool SetArmsDataExact(long index, long code, long max_num, long exp, long level);

			/// Set arms level & experience
			///
			bool SetArmsInfo(long code, long exp, long level, long max_num);

			/// @brief Remove arms data
			///
			/// @param code The weapon ID to remove.
			///
			/// @return Returns true if the weapon was found & removed, false otherwise.
			bool SubArmsData(long code);

			/// @brief Trade arms
			///
			/// @param code1 The weapon ID to trade in
			/// @param code2 The weapon ID to replace code1 with.
			/// @param max_num The maximum ammo for the new weapon.
			///
			/// @return Returns true on success.
			bool TradeArms(long code1, long code2, long max_num);

			/// @brief Add an item to inventory
			///
			/// @param code The item ID
			///
			/// @return Returns true on success.
			bool AddItemData(long code);

			/// @brief Remove an item
			///
			/// @param code The item ID
			///
			/// @return Returns true on success
			bool SubItemData(long code);

			/// @brief Check this client's inventory for an item.
			///
			/// @param code The code of the item to check for.
			///
			/// @return Returns true if they have it, false otherwise.
			bool CheckItem(long code);
			
			/// @brief Check this client's inventory for a weapon.
			///
			/// @param code The code of the weapon to check for.
			///
			/// @return Returns true if they have it, false otherwise.
			bool CheckArms(long code);

			/// @brief Add experience to the held weapon
			///
			/// @param x Amount of EXP to add
			void AddExp(int x, BOOL spur = false);

			/// @brief Add ammo to the held weapon.
			///
			/// @param no Amount of ammo to add.
			/// @param val
			void AddBullet(int no, int val);

			/// @brief Check if the missile launcher is full.
			///
			/// @return Returns true if it is.
			bool IsAmmoFull();

			/// @brief Set all weapons in this client's inventory to be at full ammo.
			///
			void FullArmsEnergy();

			/// @brief Zero-exp and reset the levels of all weapons in this client's inventory.
			///
			void ZeroArmsEnergy_All();

			/// @brief Set / clear equip flags.
			///
			/// @param flag The flag to set / clear.
			/// @param val Whether the flag should be set (true) or cleared (false).
			void EquipItem(int flag, bool val);

			/// @brief Backstep the player.
			///
			/// @param code_event What to backstep from.
			///
			/// @note If code_event = 0, the player will always backstep to the right.
			/// @note If code_event = 1, the player will always backstep to the left.
			/// @note Otherwise, the player will backstep from the NPC tied to the given code_event.
			void BackStep(int code_event);

			/// @brief Set the player's absolute position.
			///
			/// @param x The X position
			/// @param y The Y position
			void SetPlayerPosition(int x, int y);

			/// @param Set the direction of the player
			///
			/// @param direction The direction the player should face.
			///
			/// @see Direction
			void SetDirect(int direction);

			/// @brief Reset spur charge.
			///
			void ResetSpurCharge();

			/// @brief Reset experience for the currently selected weapon.
			///
			void ZeroExpMyChar();

			/// @brief Switch to the next weapon in our inventory.
			///
			/// @return Returns the new selected weapon index.
			int ArmsNext();

			/// @brief Switch to the previous weapon in our inventory
			///
			/// @return Returns the new selected weapon index.
			int ArmsPrev();

			/// @brief Add a stage permission.
			///
			/// @param index The UI position of the stage in the teleportation GUI.
			/// @param event The event to execute when selected.
			///
			/// @return Returns true when successfully added.
			bool AddPermitStage(int index, int event);

			/// @brief Remove a stage permission.
			///
			/// @param index The index of the permitted stage.
			///
			/// @return Returns true if successfully removed.
			bool SubPermitStage(int index);

			/// @brief Clear stage permissions.
			///
			void ClearPermitStage();

			/// @brief Set client color.
			///
			/// @param r Red value.
			/// @param g Green value.
			/// @param b Blue value.
			///
			/// @note If r, g and b are set to 0, then the character will not display a custom color.
			void SetColor(unsigned char r, unsigned char g, unsigned char b);

			/// @brief Play an emote.
			///
			/// @param index The index of the emote.
			void ChatEmote(int index);

			/// @brief Draw the emote.
			///
			/// @param fx Camera X position.
			/// @param fy Camera Y position.
			void DrawChatEmote(int fx, int fy);

			/// @brief Cache our death NPC if we can.
			///
			void CacheDeath();

			/// @brief Clear death cache.
			///
			void ClearDeathCache();

			/// @brief Transmit a tile change to a client.
			///
			/// @param x The tile's X position.
			/// @param y The tile's Y position.
			/// @param new_tile The new tile index.
			void SendTileChange(int x, int y, int new_tile);

			/// @brief Callback for when the map loads.
			///
			void OnMapLoad();

			/// @brief Get display focus for mAnimNPCs.
			///
			/// @return Returns DFocus.
			MYCHAR* GetDFocus();

			/// @brief Choose this client's character.
			///
			void ChooseCharacter();

			/// @brief Get the face info for this client.
			///
			/// @param face_src_rect A pointer to the GUI_RECT that will hold the face rect.
			/// @param surf_id A pointer to a Surface_Ids variable that will hold the face's surface ID.
			/// @param mask_id A pointer to a Surface_Ids variable that will hold the face mask's surface ID.
			void GetFaceInfo(GUI_RECT* face_src_rect, Surface_Ids* surf_id, Surface_Ids* mask_id);

		public: // Movement prediction

			/// @brief Simulate character movement.
			///
			/// @param iNumFrames How many frames ahead to simulate.
			/// @param pMC A pointer to a MYCHAR* structure to store the simulated data.
			/// @param iKeyMask The key mask to allow. Use 'gKey' values. For example, 'gKeyShot' will allow the shoot key.
			///
			/// @return Returns number of frames simulated.
			int SimulateMovement(int iNumFrames, MYCHAR* pMC, unsigned int iKeyMask = gKeyMaxVal);

			/// @brief Predict this client's movement.
			///
			/// @param iNewInput The new input for this client.
			/// @param iInputTime The server time at which the new input was used on the client's side.
			/// @param iClientX The client's X position.
			/// @param iClientY The client's Y position.
			void PredictMovement(int iNewInput, long long int iInputTime, int iClientX, int iClientY);

		public: // GUI management

			/// @brief Open a GUI for this client.
			///
			/// @param gui_type The type of GUI to open.
			/// @param DisableControl Disable control for this client when the GUI opens.
			///
			/// @return Returns true if the GUI was opened.
			bool OpenGUI(CaveNet::DataStructures::GUI_Type gui_type, bool DisableControl = true);

			/// @brief Close any open GUI for this client.
			/// 
			/// @param EnableControl Enable control when the GUI closes.
			///
			/// @return Returns true if the client was taken out of a GUI.
			bool CloseGUI(bool EnableControl = true);

			/// @brief Handle any GUI interactions.
			///
			/// @param ActionIndex ...
			/// @param Arg1 ...
			/// @param Arg2 ...
			/// @param Arg3 ...
			void HandleGuiAction(int ActionIndex, int Arg1, int Arg2, int Arg3);

			/// @brief Self-explanatory.
			///
			/// @return Returns true if the client is currently in a GUI.
			bool IsInGUI();

			/// @brief Check to see if this client can open a type of GUI.
			///
			/// @param gui_type The type of GUI to check.
			///
			/// @return Returns true if this client can open the specified gui_type or not.
			bool CanOpenGUI(CaveNet::DataStructures::GUI_Type gui_type);

		public: // Carrying

			/// @brief Carry a player.
			///
			/// @param id The client ID of the person we should carry.
			///
			/// @return Returns true if we started carrying them.
			bool CarryPlayer(NetClientId id);

			/// @brief Dismount from the player we're mounted on.
			///
			/// @param deleteAnim Delete the carry animation.
			///
			/// @return Returns true if dismounted successfully.
			bool Dismount(bool deleteAnim = true);

			/// @brief Get the next client in the carry stack.
			///
			/// @return Returns the client we're carrying.
			NetClient* GetCarryStackUp();

			/// @brief Get the previous carried client.
			///
			/// @return Returns the client carrying us.
			NetClient* GetCarryStackDown();

			/// @brief Check if we're being carried.
			///
			/// @return Returns true if we're being carried.
			bool IsBeingCarried();

			/// @brief Check if we're carrying anybody.
			///
			/// @return Returns true if we're carrying anybody.
			bool IsCarrying();

			/// @brief Check to see if we're carrying a specific client.
			///
			/// @param id The ID of the client we're checking for.
			///
			/// @return Returns true if we're carrying any client that matches this ghost id.
			/// @note This function checks the entire carry stack.
			bool IsCarryingPlayer(NetClientId id);

			/// @brief Check to see if we're being carried by a specific client.
			///
			/// @param id The ID of the client we're checking for.
			///
			/// @return Returns true if we're being carried by the specified client.
			/// @note This function checks the entire carry stack.
			bool IsBeingCarriedBy(NetClientId id);

			/// @brief Check if we can be carried by a client.
			/// 
			/// @param other The client to check.
			///
			/// @return Returns true if we can be carried by the given client.
			bool CanBeCarriedBy(NetClient* other);

			/// @brief Check if we can carry a client.
			/// 
			/// @param other The client to check.
			///
			/// @return Returns true if we can carry the given client.
			bool CanCarry(NetClient* other);

		public: // Whimsical star

			/// @brief Re-initialize the whimsical stars.
			///
			void InitStar();

			/// @brief Process the whimsical stars.
			///
			void ActStar();

			/// @brief Draw the whimsical stars.
			///
			/// @param fx Camera X.
			/// @param fy Camera Y.
			void PutStar(int fx, int fy);

			/// @brief Transmit the whimsical stars.
			///
			void TransmitStars();

		public: // Spectation
			/// @brief Spectate another client
			///
			/// @param ghostId The ghost ID of the client to observe.
			void SpectatePlayer(NetClientId ghostId);

			/// @brief Takes this client out of spectator mode.
			///
			void ClearSpectate();

			/// @brief Cache the current spectator target
			///
			void CacheSpectate();

			/// @brief Restore the old spectator target
			///
			void RestoreSpectate();

			/// @brief Update the spectation target, taking into account text script's focusing.
			///
			void UpdateSpectator();

			/// @brief Get the next spectator target
			///
			/// @param iDirect The direction in the clientgroup to get the next target. Should only be Directions::DIRECT_LEFT or Directions::DIRECT_RIGHT .
			///
			/// @return Returns the next client to spectate on success, or NULL if we couldn't find anyone to spectate.
			NetClient* GetNextSpectateTarget(Directions iDirect);

			/// @brief Process spectation controls.
			///
			void ProcessSpectator();

			/// @brief Determine whether or not the specified client is spectatable.
			///
			/// @param pTarget The client to validate.
			///
			/// @return Returns true if the client can be spectated.
			bool IsTargetSpectatable(NetClient* pTarget);

		public: // Frame management

			/// @brief Validate this client's frame.
			///
			void ValidateFrame();

			/// @brief Get this client's screen rect.
			///
			/// @param x_out A pointer to an int that will contain the X value. Can be NULL.
			/// @param y_out A pointer to an int that will contain the Y value. Can be NULL.
			/// @param w_out A pointer to an int that will contain the width value. Can be NULL.
			/// @param h_out A pointer to an int that will contain the height value. Can be NULL.
			///
			/// @note This only sets rect_out to a non-magnified, non-subpixel rect. (x, y, w, h).
			void GetScreenRect(int* x_out, int* y_out, int* w_out, int* h_out);

		protected: // Protected zone management

			/// @brief Find the zone in our array
			///
			/// @param zone_id The ID of the zone
			///
			/// @return Returns the index of the zone in mCache.zones, or -1 if it couldn't be found.
			inline int GetZoneIndex(int zone_id);

		public: // Zone management

			/// @brief Add ourselves to a zone
			///
			/// @param zone_id The IDX of the zone
			void AddToZone(int zone_idx);

			/// @brief Remove ourselves from a zone.
			///
			/// @param zone_id The IDX of the zone
			void RemoveFromZone(int zone_idx);

			/// @brief Check to see if we're in a zone.
			///
			/// @param zone_id The IDX of the zone
			bool IsInZone(int zone_idx);

			/// @brief Clear our occupied zones.
			///
			/// @param do_remove Do the removal process for every zone. Setting this to 'false' will make it just memset the zones array & set zone_count to 0.
			void ClearZones(bool do_remove = true);

			/// @brief Calculate & build a list of zone candidates.
			///
			/// @param zones The zone list to populate with zone candidates.
			/// @param count A pointer to an int that will hold the number of valid zones in 'zones'.
			void CalculateZoneCandidates(int zones[CAVENET_CLIENT_MAX_ZONES], int* count);

			/// @brief Re-calculate what zones we're in.
			///
			void OccupyZones();

		public: // Methods

			/// @brief Assign this client to a socket & address.
			///
			/// @param sock The socket to bind the client to.
			/// @param addr The address to bind the client to.
			///
			/// @return Returns false on error
			bool Assign(NetSocket* sock, IPaddress* addr, int channel = -2);

			/// @brief Disconnect this client from the server.
			///
			void Disconnect();

		public: // Packet communication

			/// @brief Send a packet to this client.
			///
			/// @param NetPacket* packet The packet to be sent.
			///
			/// @return Returns true on success.
			bool Send(NetPacket* packet);

			/// @brief Read a packet from this client.
			///
			/// @param NetPacket* packet A packet buffer.
			///
			/// @return Returns true if a packet was read.
			bool Read(NetPacket* packet);

		public: // Server methods

			/// @brief Get this client's ping average.
			///
			/// @return Returns this client's ping average.
			inline int GetPing() { return mCache.ping.GetAverage(); }

			/// @brief Check for administrator privileges for this client
			///
			/// @return Returns TRUE if the client is an admin, otherwise returns FALSE.
			void AutoAdminCheck();

			/// @brief Check to see if this client is an administrator or not.
			///
			/// @return Returns TRUE if the client is an admin, otherwise returns FALSE.
			BOOL IsAdmin();

		public: // Miscallaneous Methods

			/// @brief Check to see if this object is a ghost.
			///
			/// @return Returns true if this client belongs to the client vector.
			inline bool IsGhost() { return mCache.is_ghost; }

			/// @brief Get the state of the client.
			///
			/// @return Returns the current state of the client.
			inline const ClientState GetState() { return mState; }

			/// @brief Get this client's player object.
			///
			/// @return Returns this client's player object.
			inline NetPlayer* GetPlayer() { return mPlayer; }

			/// @brief Get this client's MYCHAR object.
			///
			/// @param bRequired If this is set to 'true', then if this function returns 'NULL', it will instead return a pointer to a garbage MYCHAR.
			///
			/// @return Returns this client's MYCHAR object.
			MYCHAR* GetMyChar(bool bRequired = false);

			/// @brief Get our custom player.
			///
			/// @return Returns this client's custom player.
			CustomPlayer* GetCharacter();

			/// @brief Get this client's ghost ID.
			///
			/// @return Returns this client's ghost ID.
			inline NetClientId GetGhostId() { return mGhostId; }

			/// @brief Get the channel this client is communicating on.
			///
			/// @return Returns this client's channel
			inline int GetChannel() { return mChannelNum; }

			/// @brief Get the channel this client's address.
			///
			/// @return Returns this client's IPaddress.
			inline IPaddress* GetAddress() { return &mAddress; }

			/// @brief Get this client's netname
			///
			/// @return Returns this client's name.
			inline const char* GetNetName() { return mCache.net_name; }

			/// @brief Get this client's NetSocket
			///
			/// @return Returns this client's current NetSocket.
			inline NetSocket* GetSocket() { return mSocket; }

			/// @brief Get this client's IP into a string.
			///
			/// @param out The buffer to hold the IP.
			/// @param out_size The size of the buffer.
			///
			/// @return Returns 'out' param if successful.
			char* GetStringIP(char* out, int out_size);

		public: // Server-side methods

			/// @brief Set the ghost ID for this client.
			///
			/// @param id The ghost ID.
			inline void SetGhostId(NetClientId id) { mGhostId = id; }

			/// @brief Send the player list to this client.
			///
			void SendPlayerList();

			/// @brief Send the current map.
			///
			/// @param event_id The event to execute upon loading the map.
			void SendMap(int event_id = 90);

			/// @brief Send server information to this client.
			///
			void SendServerInfo();

			/// @brief Send flags.
			///
			void SendFlags();

			/// @brief Send boss life.
			///
			void SendBossLife();

			/// @brief Send the current text script state.
			///
			void SendTextScriptState();

			/// @brief Transmit the 'start event' packet.
			///
			void SendStartEvent();

			/// @brief Transmit the 'team update' packet.
			///
			/// @param pTeam The team to update the client about. If NULL, then it will update every team.
			///
			void SendTeamData(NetTeam* pTeam = NULL);

			/// @brief Self-explanatory.
			///
			void FreezeIfEventIsRunning();

			/// @brief Finalize this client.
			///
			void FinalizeClient();

			/// @brief Paste the current game state to this client.
			///
			void PasteState();

			/// @brief Initialization.
			///
			void InitClient();

			/// @brief Synchronize configuration variables.
			///
			void SyncConfigVars();

			/// @brief Transmit the physics for this player.
			///
			void TransmitPlayerPhysics();

			/// @brief Transmit an update packet to all clients.
			///
			void TransmitUpdate();

			/// @brief Transmit a name update packet to all clients.
			///
			void TransmitName();

			/// @brief Transmit an update packet to all clients.
			///
			void TransmitMapTileUpdate();

			/// @brief Transmit a player update packet to all clients.
			///
			void TransmitPlayerUpdate();

			/// @brief Transmit this player's inventory
			///
			/// @param iTypeFlags What to transmit.
			///
			/// @see CaveNet::UpdateInventoryFlags
			void TransmitInventory(unsigned int iTypeFlags);

			/// @brief Enforce name uniqueness.
			///
			void ValidateName();

			/// @brief Build the event context for this client.
			///
			/// @param ptr A pointer to the TEXT_SCRIPT_PTR_DATA that will hold this client's script context.
			///
			/// @return Returns ptr.
			TEXT_SCRIPT_PTR_DATA* GetScriptContext(TEXT_SCRIPT_PTR_DATA* ptr);

			/// @brief Have the client say something in chat.
			///
			/// @param pMessage The message to say.
			void Say(const char* pMessage);

		public: // Server-side packet building methods

			/// @brief Construct a full-on connection packet.
			///
			/// @param out The packet to build the output in.
			///
			/// @return Returns true if it was built successfully.
			bool BuildFullPacket_Join(NetPacket* out);

			/// @brief Construct a full-on move packet.
			///
			/// @param out The packet to build the output in.
			///
			/// @return Returns true if it was built successfully.
			bool BuildFullPacket_MovePacket(NetPacket* out);

		public: // Server-side packet building methods (for stems)

			/// @brief Build a 'join' packet.
			///
			/// @param out The packet to hold the output.
			///
			/// @return Returns true if built successfully.
			bool BuildStemPacket_Join(NetPacket* out);

			/// @brief Build a 'ready' packet.
			///
			/// @param out The packet to hold the output.
			///
			/// @return Returns true if built successfully.
			bool BuildStemPacket_Ready(NetPacket* out);

			/// @brief Build a 'whimsical star update' packet.
			///
			/// @param out The packet to hold the output.
			///
			/// @return Returns 0 on success, non-zero on error.
			int BuildStemPacket_WhimsicalStar(NetPacket* out);

			/// @brief Build a 'player update' packet.
			///
			/// @param out The packet to hold the output.
			///
			/// @return Returns 0 on success, non-zero on error.
			int BuildStemPacket_PlayerUpdate(NetPacket* out);

			/// @brief Build a 'player physics' packet.
			///
			/// @param out The packet to hold the output.
			///
			/// @return Returns 0 on success, non-zero on error.
			int BuildStemPacket_PlayerPhysicsUpdate(NetPacket* out);

			/// @brief Build an 'update' packet.
			///
			/// @param out The packet to hold the output.
			///
			/// @return Returns 0 on success, non-zero on error.
			int BuildStemPacket_Update(NetPacket* out);

			/// @brief Build an 'update ping' packet.
			///
			/// @param out The packet to hold the output.
			///
			/// @return Returns 0 on success, non-zero on error.
			int BuildStemPacket_UpdatePing(NetPacket* out);

		public: // Client-side methods

			/// @brief Get the SURFACE_ID_GUI rect for the signal strength of this client.
			///
			/// @param x A pointer to the X value.
			/// @param y A pointer to the Y value.
			/// @param w A pointer to the width value.
			/// @param h A pointer to the height value.
			void GetSignalStrengthRect(int* x, int* y, int* w, int* h, bool IncrementFrame = true);

			/// @brief Set this client as a ghosted object.
			///
			/// @param id The ghost ID.
			inline void SetGhostedClient(NetClientId id) { mGhostId = id; SetState(ClientState::CLIENT_GHOST); }

			/// @brief Generate a name on the name plate surface for this user
			///
			void GenerateNamePlate();

			/// @brief Get the rect for the name plate of this client
			///
			/// @param x A pointer to the X value.
			/// @param y A pointer to the Y value.
			/// @param w A pointer to the width value.
			/// @param h A pointer to the height value.
			void GetNamePlateRect(int* x, int* y, int* w, int* h);

		public: // Server-side ghosting methods

			/// @brief Reset ghosting for this client.
			///
			void ResetGhosting();

			/// @brief Build the initial dirty list; Don't include NPCs that don't exist.
			///
			void InitializeDirtyList();

			/// @brief Mark an NPC at the given index as 'dirty'. It will be queued for re-ghosting.
			///
			/// @param index The index of the NPC.
			void DirtyNPC(int index);

			/// @brief Mark an NPC at the given index as 'clean'. Removes it from the ghost queue if it's in it.
			///
			/// @param index The index of the NPC.
			void CleanNPC(int index);

			/// @brief Get the index of a dirty NPC.
			///
			/// @param index The index of the NPC in gNPC.
			///
			/// @returns int Returns the index of the NPC in our dirty list. Returns -1 if the given NPC is not queued.
			int GetNPCDirtyIndex(int index);

		public: // Processing Methods

			/// @brief Process ghosting on this client. Should only be called server-side.
			///
			void ProcessGhosting();
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		
		/// @brief Movement prediction information.
		///
		/// A struct for holding information related to movement prediction.
		///
		struct NetPlayerFrameInfo
		{
			/// The global time at which this frame was saved.
			///
			unsigned long long int time;
			
			/// The game tick that we were at when this was recorded.
			///
			unsigned long long int game_tick;

			/// Whether the player is dead.
			///
			bool dead;

			/// The information for this frame.
			///
			MYCHAR info;

			/// This client's spur charge.
			///
			int spur_charge;

			/// Whether this client's spur charge is at maximum.
			///
			bool spur_max;

			/// The current select weapon
			///
			int current_weapon;

			/// Soft rensha; for weapons
			///
			int soft_rensha;
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// @brief A networking player.
		///
		/// Handles a client's player object.
		class CAVESTORY_MOD_API NetPlayer
		{
			friend class AI_Brain;
			friend class AI_ActionBase;
			friend class NetClient;

		protected: // Variables

			/// @protected
			///
			NetClient* mClient;

		public: // Movement prediction

			/// Movement prediction information
			///
			NetPlayerFrameInfo* mMoveHistory;

			/// Stores how many movement frames are stored in mMoveHistory.
			///
			unsigned int mMoveHistoryCount;

		public:
			/// Player data
			struct
			{
				/// AI Brain
				///
				AI_Brain* brain;

				/// AI enabled
				///
				bool enabled;
			} m_Ai;

			struct
			{
				/// This player's MYCHAR representation.
				///
				MYCHAR npc;

				/// Whether this client is dead or not.
				///
				bool dead;
			} m_Player;

		public: // Constructors & deconsctructors

			NetPlayer(NetClient* client);
			~NetPlayer();

		public:

			/// @brief Clear all prediction frames.
			///
			void ClearPredictionFrames();

			/// @brief Remember the current playerstate for movement prediction.
			///
			void AddPredictionFrame();

			/// @brief Apply the closest frame to the given global timestamp.
			///
			/// @param iGlobalTime The time that the frame should be the closest to.
			/// @param iClientX The client's X position.
			/// @param iClientY The client's Y position.
			///
			/// @return Returns true on success, false otherwise.
			bool ApplyClosestPredictionFrame(unsigned long long int iGlobalTime, int iClientX, int iClientY);

			/// @brief Get the closest movement frame to the given timestamp.
			///
			/// @param iGlobalTime The time that the frame should be the closest to.
			/// @param iClientX The client's X position.
			/// @param iClientY The client's Y position.
			///
			/// @return Returns the frame closest to the given time, otherwise returns NULL if no frames are cached.
			NetPlayerFrameInfo* FindClosestPredictionFrame(unsigned long long int iGlobalTime, int iClientX, int iClientY);

			/// @brief End the current animation
			///
			void EndAnimation();

			/// @brief Begin fishing.
			///
			/// @return Returns true if the playerstate was successfully set, otherwise returns false.
			bool BeginFishing();

			/// @brief Begin tackling.
			///
			/// @return Returns true if the playerstate was successfully set, otherwise returns false.
			bool Tackle(int level);

			/// @brief Teleports the player in.
			///
			/// @return Returns true if the playerstate was successfully set, otherwise returns false.
			bool TeleportIn();

			/// @brief Teleports the player out.
			///
			/// @return Returns true if the playerstate was successfully set, otherwise returns false.
			bool TeleportOut();

			/// @brief Death animation.
			///
			/// @return Returns true if the playerstate was successfully set, otherwise returns false.
			bool BecomeDead();

			/// @brief Become a carried NPC
			///
			/// @return Returns true if the playerstate was successfully set, otherwise returns false.
			bool BecomeCarried(CaveNet::DataStructures::NetClient* pClient);

			/// @brief Check to see if this player is currently being animated.
			///
			/// @return Returns true if we're being controlled by an animation NPC.
			inline bool IsAnimating() { return (m_Player.npc.netanim != -1); }

		public: // Params

			/// @brief Deal damage to this player. This method <i>can</i> kill the player.
			///
			/// @param damage The amount of damage to deal.
			/// @param bExactDamage If set to true, then the damage dealt is not run through any modifier settings.
			/// @param iDamageType The type of damage done.
			/// @param pKillerClient The client who is damaging us.
			/// @param pKillerNpc The NPC who is damaging us.
			void Damage(int iDamage, bool bExactDamage = false, PlayerDeathType iDamageType = PlayerDeathType::DEATHTYPE_UNKNOWN, CaveNet::DataStructures::NetClient* pKillerClient = NULL, NPCHAR* pKillerNpc = NULL);

		public: // Acts

			/// @brief Process the camera
			///
			/// @param bKey Whether input is allowed or not
			void Act_Camera(bool bKey);

			/// @brief Process underwater air
			///
			void AirProcess();

		public: // Collision

			/// @private
			///
			void PutlittleStar();

			/// @private
			///
			int JudgeHitMyCharBlock(int x, int y);

			/// @private
			///
			int JudgeHitMyCharTriangleA(int x, int y);

			/// @private
			///
			int JudgeHitMyCharTriangleB(int x, int y);

			/// @private
			///
			int JudgeHitMyCharTriangleC(int x, int y);

			/// @private
			///
			int JudgeHitMyCharTriangleD(int x, int y);

			/// @private
			///
			int JudgeHitMyCharTriangleE(int x, int y);

			/// @private
			///
			int JudgeHitMyCharTriangleF(int x, int y);

			/// @private
			///
			int JudgeHitMyCharTriangleG(int x, int y);

			/// @private
			///
			int JudgeHitMyCharTriangleH(int x, int y);

			/// @private
			///
			int JudgeHitMyCharWater(int x, int y);

			/// @private
			///
			int JudgeHitMyCharDamage(int x, int y);

			/// @private
			///
			int JudgeHitMyCharDamageW(int x, int y);

			/// @private
			///
			int JudgeHitMyCharVectLeft(int x, int y);

			/// @private
			///
			int JudgeHitMyCharVectUp(int x, int y);

			/// @private
			///
			unsigned char JudgeHitMyCharNPC3(NPCHAR* npc);

			/// @private
			///
			int JudgeHitMyCharVectRight(int x, int y);

			/// @private
			///
			int JudgeHitMyCharVectDown(int x, int y);

			/// @private
			///
			int JudgeHitMyCharNPC(NPCHAR* npc);

			/// @private
			///
			int JudgeHitMyCharNPC4(NPCHAR* npc);

			/// @private
			///
			int JudgeHitMyCharPlayer(MYCHAR* npc);

		public: // Public methods

			/// @brief Check to see if this player has max EXP in their held weapon.
			///
			/// @return Returns true if we have max EXP and level for the held weapon
			bool IsMaxExp();

			/// @brief Check to see if this player can be revived right now.
			///
			/// @return Returns true if we can be revived
			bool CanBeRevived();

			/// @brief Check to see if this player can shoot.
			///
			/// @return Returns true if we can shoot
			int CanShoot();

			/// @brief Update this client's gameKey, gameKeyTrg, key and keyTrg
			///
			void GetTrg();

			/// @brief Tick this player's movement logic
			///
			void Act(bool bKey);

			/// @brief 
			///
			/// @param x
			void AddExp(int x);

			/// @brief 
			///
			/// @param x
			void AddLife(int x);

			/// @brief 
			///
			/// @param x
			void AddMaxLife(int x);

			/// @brief 
			///
			/// @param x
			/// @param y
			void SetPosition(int x, int y);

			/// @brief Show/hide this player
			///
			/// @param val Boolean
			void ShowMyChar(bool val);

			/// @brief 
			///
			/// @param bKey Allow control
			void Animate(bool bKey);

			/// @brief Revive this player.
			///
			/// @param restoreControl Restore control after revival.
			/// @param health How much health to revive them with.
			/// @param pSavior Our saviour. Can be NULL.
			/// @param bAnnounce Announce this revival to everyone on the server.
			void Revive(bool restoreControl, int health = 1, NetClient * pSavior = NULL, bool bAnnounce = true);

			/// @brief Kill this player.
			///
			/// @param bResetVelocity Reset the player's velocity to zero.
			void Kill(bool bResetVelocity = true, PlayerDeathType iDeathType = PlayerDeathType::DEATHTYPE_UNKNOWN, NPCHAR* pKillerNpc = NULL, CaveNet::DataStructures::NetClient* pKillerClient = NULL);

			/// @brief Reset the collision flags for this player.
			///
			void ResetFlag();

			/// @brief Player collision checking against map tiles whilst in noclip mode.
			///
			void HitMyCharMapNoclip();
			
			/// @brief Player collision checking against map tiles.
			///
			void HitMyCharMap();

			/// @brief Player collision checking against NPCs.
			///
			void HitMyCharNpChar();

			/// @brief Player collision checking against bullets.
			///
			void HitMyCharBullet();

			/// @brief Player collision checking against bosses.
			///
			void HitMyCharBoss();

			/// @brief Draw this player.
			///
			/// @param fx Camera X
			/// @param fy Camera Y
			void Put(int fx, int fy);

		public:

			/// @brief Start AI routine (if this is a bot).
			///
			void StartAI();

			/// @brief End AI routine (if this is a bot).
			///
			void EndAI();

			/// @brief loop
			///
			void ProcessAI();

		public: // Miscallaneous methods

			/// @brief Get the client that owns this player.
			///
			/// @return
			inline NetClient* GetClient() { return mClient; }
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// Clientgroup iterator.
		///
		class NetClientIterator
		{
		private:
			/// The index we're at.
			/// @private
			int m_iIndex;

			/// A pointer to the 'get client' function.
			/// @private
			NetClient*(*m_pGetClientFunc)(int);

		public:

			/// @brief Default constructor
			/// 
			inline NetClientIterator()
			{
				m_iIndex         = 0;
				m_pGetClientFunc = NULL;
			}

		public:

			/// @brief Get the first client in a clientgroup.
			///
			/// @param bUseServerClientGroup Whether to use the server-side clientgroup (true) or the client-side clientgroup (false).
			///
			/// @return The first client in the given list.
			inline NetClient* first(bool bUseServerClientGroup)
			{
				m_pGetClientFunc = (!bUseServerClientGroup ? CaveNet_Client_ClientGroup_GetClient : CaveNet_Server_ClientGroup_GetClient);
				m_iIndex         = 0;

				return m_pGetClientFunc(0);
			}

			/// @brief Get the first client in a clientgroup.
			///
			/// @return The first client in the given list.
			inline NetClient* next()
			{
				return m_pGetClientFunc(++m_iIndex);
			}
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		/// Stores multiple clients.
		///
		class CAVESTORY_MOD_API NetClientVector
		{
		public: // Friends
			/// Friends with NetClient because that's what we'll be storing
			///
			friend class NetClient;

		public: // Typedefs

			/// The iterator for NetClientVector
			///
			typedef NetClient** iterator;

		protected: // Variables

			/// @protected
			///
			NetClient** mList;

			/// @protected
			///
			int mCount;

			/// @protected
			///
			int mSize;

		public: // Constructors & deconstructors

			/// Constructor
			///
			NetClientVector();

			/// Deconstructor
			///
			~NetClientVector();

		protected: // Protected methods

			/// @brief Resize the vector
			/// @protected
			///
			/// @param size The new size of the vector.
			void Resize(int newSize);

		public: // Vector manipulation methods

			/// @brief Add a client into this vector.
			///
			/// @param client The client object to add.
			///
			/// @return bool Returns true if the client was added to the vector.
			bool Add(NetClient* client);

			/// @brief Remove a client from this vector.
			///
			/// @param client The client object to remove.
			///
			/// @return bool Returns true if the client was removed from the vector.
			bool Remove(NetClient* client);

			/// @brief Remove a client from this vector.
			///
			/// @param itr The client pointer to remove.
			///
			/// @return bool Returns true if the client was removed from the vector.
			bool Remove(NetClient** itr);

			/// @brief Clear the vector
			///
			void Clear();

			/// @brief Clear the vector, whilst also freeing all clients
			///
			void FreeClients();

			/// @brief Shrink the vector down to fit the current amount of clients in it
			///
			void Shrink();

		public: // Miscallaneous methods

			/// @brief Get the number of clients in the vector.
			///
			/// @return int Returns the amount of clients currently in the vector.
			int Count();

			/// @brief Get the size of the vector.
			///
			/// @return int Returns the size of the vector.
			int Size();

#ifdef _DEBUG
			/// @brief Dump data (debug only)
			/// @private
			///
			void Dump();
#endif

		public: // Operator overloads

			/// @brief Array operator -- Returns a net client at the given index
			///
			/// @param index The index of the client
			NetClient* operator[](int index);

			/// @brief Array operator -- Returns a net client at the given index
			///
			/// @param index The index of the client
			NetClient* operator[](int index) const;

		public: // Iterating the array

			/// @brief Get the first entry
			///
			/// @return Returns the first entry
			NetClient* first();

			/// @brief Get the last entry
			///
			/// @return Returns the last entry
			NetClient* last();

			/// @brief Get the first iterator entry
			///
			/// @return Returns the first iterator entry
			NetClient** begin();

			/// @brief Get the last iterator entry
			///
			/// @return Returns the last iterator entry
			NetClient** end();
		};

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		inline MYCHAR* NetClient::GetMyChar(bool bRequired)
		{
			return (mCache.is_ghost && mCache.is_our_player ? &gMC : (mPlayer ? &mPlayer->m_Player.npc : (bRequired ? &gNOT_USED : NULL)));
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	};

	using namespace DataStructures;

};

/// @}
///

#include <CSMAPI_endcode.h>