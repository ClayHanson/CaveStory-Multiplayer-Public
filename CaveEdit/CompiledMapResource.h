#pragma once

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAX_ENTITY_PROPERTY_STRING_LENGTH		256

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct PXCM_ENTITY_CUSTOM_PROPERTY
{
	unsigned int name_crc;
	char name[MAX_ENTITY_PROPERTY_STRING_LENGTH];
	char value[MAX_ENTITY_PROPERTY_STRING_LENGTH];
};

struct PXCM_ENTITY
{
	unsigned short code_char;
	unsigned short x;
	unsigned short y;
	unsigned short code_flag;
	unsigned short code_event;
	unsigned int bits;

	PXCM_ENTITY_CUSTOM_PROPERTY* property_list;
	unsigned int property_count;
};

struct PXCM_ZONE
{
	unsigned short id;
	unsigned short x;
	unsigned short y;
	unsigned short w;
	unsigned short h;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct PXCM_MAP_INFO
{
	unsigned short width;
	unsigned short height;
	unsigned int crc;
};

struct PXCM_TILE_CHUNK
{
	unsigned char* tiles;
};

struct PXCM_ENTITY_CHUNK
{
	PXCM_ENTITY* entities;
	unsigned short count;
};

struct PXCM_TSC_CHUNK
{
	unsigned char* encrypted_data;
	unsigned char* decrypted_data;
	unsigned int size;
};

struct PXCM_ZONE_CHUNK
{
	PXCM_ZONE* zones;
	unsigned short count;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CompiledMapResource
{
public: // Public map info
	PXCM_MAP_INFO m_MapInfo;

	struct
	{
		PXCM_TILE_CHUNK   tile_data;
		PXCM_ENTITY_CHUNK entity_data;
		PXCM_TSC_CHUNK    tsc_data;
		PXCM_ZONE_CHUNK   zone_data;
	} m_Chunks;

public: // Public constructor
	CompiledMapResource();
	~CompiledMapResource();

public: // Map manipulation

	/// @brief Allocate a new map.
	///
	/// @param width The width of the map.
	/// @param height The height of the map.
	///
	/// @returns Returns true if the map was allocated successfully, false otherwise.
	bool AllocateMap(int width, int height);

	/// @brief Load a map from a file on the disc.
	///
	/// @param fileName The filename of the map.
	/// @param onlyValidate If set to true, then this function will not load the file, but will instead validate that it is a valid map file.
	///
	/// @returns Returns true if the map could be loaded, false otherwise.
	bool Load(const char* fileName, bool onlyValidate = false);

	/// @brief Saves the map to a file.
	///
	/// @param fileName The filename of the map.
	///
	/// @returns Returns true if the map could be saved, false otherwise.
	bool Save(const char* fileName);

public: // Map information generation

	/// @brief Get the CRC checksum for this map.
	///
	/// @returns Returns the CRC.
	unsigned int CalculateCRC();

public: // General cleanup

	/// @brief Clear & free all buffers.
	void Reset();

public: // Tile editing

	/// @brief Set a tile at the specified coordinates.
	///
	/// @param tile_idx The index of the tile.
	/// @param x The X position of the tile to replace.
	/// @param y The Y position of the tile to replace.
	void SetTile(int tile_idx, int x, int y);
	
	/// @brief Get a tile at the specified coordinates.
	///
	/// @param x The X position of the tile to replace.
	/// @param y The Y position of the tile to replace.
	///
	/// @returns Returns the tile index at the given position.
	int GetTile(int x, int y);

public: // Entity editing

	/// @brief Add an entity.
	///
	/// @param npc_id The npc ID of the entity.
	/// @param x The X tile position of the entity.
	/// @param y The Y tile position of the entity.
	/// @param flag The flag code for this entity.
	/// @param bits The flags of this entity.
	/// @param event THe event code for this entity.
	/// @param index The index to insert the entity at.
	///
	/// @returns Returns true if the entity was successfully added.
	bool AddEntity(int npc_id, int x, int y, int flag, int bits, int event, int index = -1);

	/// @brief Remove an entity.
	///
	/// @param entity_idx The index of the entity.
	///
	/// @returns Returns true if the entity was successfully added.
	bool RemoveEntity(int entity_index);

	/// @brief Clear all entities from the map.
	///
	void ClearEntities();

public: // Entity property editing

	/// @brief Calculates an entity property's name_crc value.
	///
	/// @param pFieldName The name of the field.
	///
	/// @return Returns the calculated name_crc value.
	static unsigned int GetEntityPropertyNameCRC(const char* pFieldName);

	/// @brief Add a property to an entity.
	///
	/// @param iEntityIndex The index of the entity.
	/// @param pFieldName The name of the property field.
	/// @param pFieldVaule The value of the property field.
	void SetEntityProperty(int iEntityIndex, const char* pFieldName, const char* pFieldValue);

	/// @brief Get a property value.
	///
	/// @param iEntityIndex The index of the entity.
	/// @param iFieldIndex The index of the field we want.
	///
	/// @return Returns true if the property exists, false otherwise.
	bool GetEntityProperty(int iEntityIndex, int iFieldIndex, char* pOutValue);
	
	/// @brief Get a property field's index.
	///
	/// @param iEntityIndex The index of the entity.
	/// @param pFieldName The name of the property field.
	///
	/// @return Returns the index of the field if it exists, -1 if it does not.
	int GetEntityPropertyFieldIndex(int iEntityIndex, const char* pFieldName);
	
	/// @brief Get a property field's index by its name_crc value.
	///
	/// @param iEntityIndex The index of the entity.
	/// @param iFieldCrc The name_crc value we're looking up.
	///
	/// @return Returns the index of the field if it exists, -1 if it does not.
	int GetEntityPropertyFieldIndexByCrc(int iEntityIndex, unsigned int iFieldCrc);

public: // Zone editing

	/// @brief Add a map zone.
	/// 
	/// @param id The ID of the zone.
	/// @param x
	/// @param y
	/// @param w
	/// @param h
	void AddZone(int id, int x, int y, int w, int h);

	/// @brief Remove a map zone.
	/// 
	/// @param id The ID of the zone.
	void RemoveZone(int id);

	/// @brief Clear all zones.
	void ClearZones();

	/// @brief Allocate a zone tile array.
	///
	/// @return Returns an array allocated with 'malloc' on success, otherwise returns NULL.
	unsigned short* AllocateZoneArray();

public: // TSC editing

	/// @brief Add a basic string to the TSC.
	///
	/// @param str The string to be added.
	/// @param size The size of the string to be added.
	void TSC_AddString(const char* str, unsigned int size = 0xFFFFFFFF);

	/// @brief Clear the text script.
	void TSC_Clear();
};