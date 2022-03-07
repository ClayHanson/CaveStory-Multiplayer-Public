#include "stdafx.h"

#include "CompiledMapResource.h"
#include "BitStream.h"

#include "../src/CompiledMapShared.h"
#include <Hasher.h>

#include <memory>
#include <stdio.h>
#ifdef _CAVESTORY_MULTIPLAYER
#include <Mod\List.h>
#else
#define LIST_ERASE(A, B, C, D) // do nothing
#endif

#define MAX(x, y)			((x) > (y) ? (x) : (y))

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/* 
	 FILE FORMAT FOR .PXCM:
	 * +--------+--------------------------------------------------------------------------------------------------------------------------- -  -   -     -
	 * | VER |  TYPE  |  DESCRIPTION
	 * +-----+--------+--------+------------------------------------------------------------------------------------------------------------ -  -   -     -
	 * | 100 | STRING | File header ("PXCM")
	 * | 100 | UCHAR  | File version
	 * | 100 | RANGED | Map width			(1 - 999)
	 * | 100 | RANGED | Map height			(1 - 999)
	 * | 100 | RANGED | Entity count		(0 - 512)
	 * | 100 | UINT   | TSC size
	 * | 100 | UINT   | TSC offset
	 * | 100 | RANGED | Zone count			(1 - 64)
	 * | 100 | USHORT | Highest zone ID
	 * | 100 | USHORT | Lowest tile IDX
	 * | 100 | USHORT | Highest tile IDX
	 * | 100 | UINT   | CRC Checksum
	 * | 100 |        | >>==================================== CHUNK: Map Tile Data ===================================<<
	 * | 100 | RANGED | Tile Index
	 * | 100 |        | >>===================================== CHUNK: Entity Data ====================================<<
	 * | 100 | USHORT | NPC ID
	 * | 100 | RANGED | X Position			(0 - Map Width)
	 * | 100 | RANGED | Y Position			(0 - Map Height)
	 * | 100 | RANGED | Flag ID				(0 - 9999)
	 * | 100 | USHORT | Event				(0 - 9999)
	 * | 100 | UINT   | Flags (or "bits")
	 * | 102 | UINT   | Property Count
	 * | 102 |        | >>================================== CHUNK: Entity Properties =================================<<
	 * | 102 | STRING | Property Name
	 * | 102 | STRING | Property Value
	 * | 100 |        | >>==================================== CHUNK: Map Zone Data ===================================<<
	 * | 100 | RANGED | Zone ID				(0 - Zone Count)
	 * | 100 | RANGED | X					(0 - Map Width)
	 * | 100 | RANGED | Y					(0 - Map Height)
	 * | 100 | RANGED | W					(0 - (Map Width minus X))
	 * | 100 | RANGED | H					(0 - (Map Height minus Y))
	 * | 100 |        | >>====================================== CHUNK: TSC Data ======================================<<
	 * | 100 | UCHAR  | Encrypted script byte
	 * +--------+--------------------------------------------------------------------------------------------------------------------------- -  -   -     -
*/
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CompiledMapResource::CompiledMapResource()
{
	// Zero-mem everything
	memset(&m_Chunks, 0, sizeof(m_Chunks));
	memset(&m_MapInfo, 0, sizeof(m_MapInfo));
}

CompiledMapResource::~CompiledMapResource()
{
	Reset();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CompiledMapResource::AllocateMap(int width, int height)
{
	// Reset everything
	Reset();

	// Allocate buffer
	m_MapInfo.width          = MAX(width, 1);
	m_MapInfo.height         = MAX(height, 1);
	m_Chunks.tile_data.tiles = (unsigned char*)malloc(m_MapInfo.width * m_MapInfo.height);

	// Zero-mem it
	memset(m_Chunks.tile_data.tiles, 0, m_MapInfo.width * m_MapInfo.height);

	return true;
}

bool CompiledMapResource::Load(const char* fileName, bool onlyValidate)
{
	// Temporary variables
	unsigned int tsc_offset       = 0;
	unsigned char version         = 0;
	unsigned int highest_zone_id  = 0;
	unsigned int highest_tile_idx = 0;
	unsigned int lowest_tile_idx  = 0;
	SharedBitStream stream;

	// Reset memory
	if (!onlyValidate)
	{
		Reset();
	}

	{
		FILE* fp;

		// Attempt to open the file for reading
		if ((fp = fopen(fileName, "rb")) == NULL)
		{
			printf("ERROR: " __FUNCTION__ "() - Failed to open file \"%s\"\r\n", fileName);
			return false;
		}

		// Go to the end of the file
		fseek(fp, 0, SEEK_END);

		// Get the length of the file
		unsigned int FullSize = ftell(fp);

		// Go back to the start of the file
		fseek(fp, 0, SEEK_SET);

		// Allocate the bitstream
		stream.Clear();
		stream.Resize(FullSize);

		// Read the entire file into the buffer
		fread(stream.mBuffer, FullSize, 1, fp);
		stream.mBufferLen = FullSize;

		stream.SetCurPos(0);

		// Close the file
		fclose(fp);
	}

	// Check it
	if (memcmp(stream.mBuffer, COMPILED_MAP_HEADER, 4))
	{
		printf("ERROR: " __FUNCTION__ "() - Invalid magic!\r\n");
		return false;
	}

	// Get the file version
	version = (unsigned char)stream.mBuffer[4];

	// Validate version
	if (version < COMPILED_MAP_COMPILED_VERSION)
	{
		printf("ERROR: " __FUNCTION__ "() - Map version mismatch (expected: %d, got: %d), attempting to read anyways\r\n", COMPILED_MAP_COMPILED_VERSION, version);
	}

	if (onlyValidate)
	{
		stream.Clear();
		return true;
	}

	// Skip past the header & version
	stream.SetCurPos(40);

	// Read map information
	m_MapInfo.width               = stream.ReadRangedInt(1, 999);
	m_MapInfo.height              = stream.ReadRangedInt(1, 999);
	m_Chunks.entity_data.count    = stream.ReadRangedInt(0, 512);
	m_Chunks.tsc_data.size        = stream.ReadInt(sizeof(unsigned int) << 3);
	tsc_offset                    = stream.ReadInt(sizeof(unsigned int) << 3);
	m_Chunks.zone_data.count      = stream.ReadRangedInt(1, 64);
	highest_zone_id               = stream.ReadInt(sizeof(unsigned int) << 3);
	lowest_tile_idx               = stream.ReadInt(sizeof(unsigned int) << 3);
	highest_tile_idx              = stream.ReadInt(sizeof(unsigned int) << 3);
	m_MapInfo.crc                 = stream.ReadInt(sizeof(unsigned int) << 3);

	// Allocate buffers
	m_Chunks.entity_data.entities    = (m_Chunks.entity_data.count ? (PXCM_ENTITY*)malloc(sizeof(PXCM_ENTITY) * m_Chunks.entity_data.count) : NULL);
	m_Chunks.tile_data.tiles         = (unsigned char*)malloc(m_MapInfo.width * m_MapInfo.height);
	m_Chunks.tsc_data.decrypted_data = (m_Chunks.tsc_data.size ? (unsigned char*)malloc(m_Chunks.tsc_data.size) : NULL);
	m_Chunks.tsc_data.encrypted_data = (m_Chunks.tsc_data.size ? (unsigned char*)malloc(m_Chunks.tsc_data.size) : NULL);
	m_Chunks.zone_data.zones         = (m_Chunks.zone_data.count ? (PXCM_ZONE*)malloc(sizeof(PXCM_ZONE) * m_Chunks.zone_data.count) : NULL);

	// Read map tile data
	for (int i = 0; i < m_MapInfo.width * m_MapInfo.height; i++)
		m_Chunks.tile_data.tiles[i] = (unsigned char)stream.ReadRangedInt(lowest_tile_idx, highest_tile_idx);

	// Read entity data
	for (int i = 0; i < m_Chunks.entity_data.count; i++)
	{
		PXCM_ENTITY* pEntity = &m_Chunks.entity_data.entities[i];

		pEntity->code_char      = stream.ReadInt(sizeof(unsigned short) << 3);
		pEntity->x              = stream.ReadRangedInt(0, m_MapInfo.width);
		pEntity->y              = stream.ReadRangedInt(0, m_MapInfo.height);
		pEntity->code_flag      = stream.ReadRangedInt(0, 9999);
		pEntity->code_event     = stream.ReadRangedInt(0, 9999);
		pEntity->bits           = stream.ReadInt(sizeof(unsigned int) << 3);
		pEntity->property_count = (version >= 102 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);

		// Allocate if necessary
		pEntity->property_list = (pEntity->property_count ? (PXCM_ENTITY_CUSTOM_PROPERTY*)malloc(sizeof(PXCM_ENTITY_CUSTOM_PROPERTY) * pEntity->property_count) : NULL);

		// Read property data
		for (int j = 0; j < pEntity->property_count; j++)
		{
			stream.ReadString(pEntity->property_list[j].name, MAX_ENTITY_PROPERTY_STRING_LENGTH);
			stream.ReadString(pEntity->property_list[j].value, MAX_ENTITY_PROPERTY_STRING_LENGTH);
			pEntity->property_list[j].name_crc = SuperFastHash(pEntity->property_list[j].name, strlen(pEntity->property_list[j].name));
		}
	}

	// Read zones
	for (int i = 0; i < m_Chunks.zone_data.count; i++)
	{
		PXCM_ZONE* pZone = &m_Chunks.zone_data.zones[i];

		pZone->id = stream.ReadRangedInt(0, highest_zone_id);
		pZone->x  = stream.ReadRangedInt(0, m_MapInfo.width);
		pZone->y  = stream.ReadRangedInt(0, m_MapInfo.height);
		pZone->w  = stream.ReadRangedInt(1, m_MapInfo.width - pZone->x);
		pZone->h  = stream.ReadRangedInt(1, m_MapInfo.height - pZone->y);
	}

	// Read TSC data
	stream.SetCurPos(tsc_offset);
	stream.Read(m_Chunks.tsc_data.encrypted_data, m_Chunks.tsc_data.size << 3);
	stream.Clear();

	// Copy the encrypted TSC data over to the decrypted buffer
	memcpy(m_Chunks.tsc_data.decrypted_data, m_Chunks.tsc_data.encrypted_data, m_Chunks.tsc_data.size);

	// Decrypt the TSC script
	unsigned char* start   = (unsigned char*)(m_Chunks.tsc_data.decrypted_data);
	unsigned char* end     = (unsigned char*)(m_Chunks.tsc_data.decrypted_data + m_Chunks.tsc_data.size);
	unsigned int tsc_shift = start[m_Chunks.tsc_data.size / 2];

	for (unsigned char* ptr = start; ptr != end; ptr++)
	{
		if ((ptr - start) != m_Chunks.tsc_data.size / 2)
		{
			*ptr      -= tsc_shift;
			tsc_shift += ((ptr - start) % 2);
		}
	}

	// Done!
	return true;
}

bool CompiledMapResource::Save(const char* fileName)
{
	// Temporary variables
	unsigned int tsc_offset;
	unsigned char version = COMPILED_MAP_COMPILED_VERSION;
	unsigned int highest_zone_id;
	unsigned int lowest_tile_idx;
	unsigned int highest_tile_idx;
	unsigned int tsc_offset_writepos;
	SharedBitStream stream;

	// Encrypt the TSC
	if (m_Chunks.tsc_data.decrypted_data)
	{
		// Copy decrypted TSC into the encrypted buffer
		memcpy(m_Chunks.tsc_data.encrypted_data, m_Chunks.tsc_data.decrypted_data, m_Chunks.tsc_data.size);

		// Encrypt TSC
		unsigned char* start   = (unsigned char*)(m_Chunks.tsc_data.encrypted_data);
		unsigned char* end     = (unsigned char*)(m_Chunks.tsc_data.encrypted_data + m_Chunks.tsc_data.size);
		unsigned int tsc_shift = start[m_Chunks.tsc_data.size / 2];

		for (unsigned char* ptr = start; ptr != end; ptr++)
		{
			if ((ptr - start) != m_Chunks.tsc_data.size / 2)
			{
				*ptr      += tsc_shift;
				tsc_shift += ((ptr - start) % 2);
			}
		}
	}
	else
	{
		// Make a blank TSC
		m_Chunks.tsc_data.size            = 1;
		m_Chunks.tsc_data.encrypted_data  = (unsigned char*)malloc(m_Chunks.tsc_data.size);
		m_Chunks.tsc_data.decrypted_data  = (unsigned char*)malloc(m_Chunks.tsc_data.size);
		*m_Chunks.tsc_data.encrypted_data = 0;
		*m_Chunks.tsc_data.decrypted_data = 0;
	}
	
	// Determine the tile range
	lowest_tile_idx  = 0xFFFFFFFF;
	highest_tile_idx = 0;
	for (int i = 0; i < m_MapInfo.width * m_MapInfo.height; i++)
	{
		unsigned char TileIdx = m_Chunks.tile_data.tiles[i];

		if (TileIdx > highest_tile_idx)
			highest_tile_idx = TileIdx;
		
		if (TileIdx < lowest_tile_idx)
			lowest_tile_idx = TileIdx;
	}

	highest_zone_id = 0;

	// Determine the highest id
	if (m_Chunks.zone_data.zones)
	{
		for (int i = 0; i < m_Chunks.zone_data.count; i++)
		{
			int zone_id = m_Chunks.zone_data.zones[i].id;

			if (zone_id > highest_zone_id)
				highest_zone_id = zone_id;
		}
	}
	else
	{
		// Allocate a blank zone buffer
		m_Chunks.zone_data.zones       = (PXCM_ZONE*)malloc(sizeof(PXCM_ZONE));
		m_Chunks.zone_data.count       = 1;
		m_Chunks.zone_data.zones[0].id = 0;
		m_Chunks.zone_data.zones[0].x  = 0;
		m_Chunks.zone_data.zones[0].y  = 0;
		m_Chunks.zone_data.zones[0].w  = m_MapInfo.width;
		m_Chunks.zone_data.zones[0].h  = m_MapInfo.height;
	}

	// Calculate a 'CRC'
	m_MapInfo.crc = CalculateCRC();

	stream.Write(COMPILED_MAP_HEADER, strlen(COMPILED_MAP_HEADER) << 3);
	stream.WriteInt(version, sizeof(unsigned char) << 3);

	/*

	m_Chunks.zone_data.count      = stream.ReadRangedInt(1, 64);
	highest_zone_id               = stream.ReadInt(sizeof(unsigned int) << 3);
	lowest_tile_idx               = stream.ReadInt(sizeof(unsigned int) << 3);
	highest_tile_idx              = stream.ReadInt(sizeof(unsigned int) << 3);
	m_MapInfo.crc                 = stream.ReadInt(sizeof(unsigned int) << 3);
	
	*/

	// Write map information
	stream.WriteRangedInt(m_MapInfo.width,				1, 999);
	stream.WriteRangedInt(m_MapInfo.height,				1, 999);
	stream.WriteRangedInt(m_Chunks.entity_data.count,	0, 512);
	stream.WriteInt(m_Chunks.tsc_data.size, sizeof(unsigned int) << 3);

	tsc_offset_writepos = stream.GetCurPos();
	tsc_offset          = 0;

	stream.WriteInt(tsc_offset,			sizeof(unsigned int) << 3);
	stream.WriteRangedInt(m_Chunks.zone_data.count, 1, 64);
	stream.WriteInt(highest_zone_id,	sizeof(unsigned int) << 3);
	stream.WriteInt(lowest_tile_idx,	sizeof(unsigned int) << 3);
	stream.WriteInt(highest_tile_idx,	sizeof(unsigned int) << 3);
	stream.WriteInt(m_MapInfo.crc,		sizeof(unsigned int) << 3);

	// Write map tile data
	for (int i = 0; i < (m_MapInfo.width * m_MapInfo.height); i++)
		stream.WriteRangedInt((unsigned short)m_Chunks.tile_data.tiles[i], lowest_tile_idx, highest_tile_idx);

	// Write entity data
	for (int i = 0; i < m_Chunks.entity_data.count; i++)
	{
		PXCM_ENTITY* pEntity = &m_Chunks.entity_data.entities[i];

		stream.WriteInt(pEntity->code_char,			sizeof(unsigned short) << 3);
		stream.WriteRangedInt(pEntity->x,			0, m_MapInfo.width);
		stream.WriteRangedInt(pEntity->y,			0, m_MapInfo.height);
		stream.WriteRangedInt(pEntity->code_flag,	0, 9999);
		stream.WriteRangedInt(pEntity->code_event,	0, 9999);
		stream.WriteInt(pEntity->bits,				sizeof(unsigned int) << 3);
		stream.WriteInt(pEntity->property_count,	sizeof(unsigned int) << 3);

		for (int j = 0; j < pEntity->property_count; j++)
		{
			stream.WriteString(pEntity->property_list[j].name, MAX_ENTITY_PROPERTY_STRING_LENGTH);
			stream.WriteString(pEntity->property_list[j].value, MAX_ENTITY_PROPERTY_STRING_LENGTH);
		}
	}

	// Write zones
	for (int i = 0; i < m_Chunks.zone_data.count; i++)
	{
		PXCM_ZONE* pZone = &m_Chunks.zone_data.zones[i];

		stream.WriteRangedInt(pZone->id,	0, highest_zone_id);
		stream.WriteRangedInt(pZone->x,		0, m_MapInfo.width);
		stream.WriteRangedInt(pZone->y,		0, m_MapInfo.height);
		stream.WriteRangedInt(pZone->w,		1, m_MapInfo.width - pZone->x);
		stream.WriteRangedInt(pZone->h,		1, m_MapInfo.height - pZone->y);
	}

	// Record the offset
	tsc_offset = stream.GetCurPos();

	// Write TSC data
	stream.Write(m_Chunks.tsc_data.encrypted_data, m_Chunks.tsc_data.size << 3);

	// Write the tsc offset
	stream.SetCurPos(tsc_offset_writepos);
	stream.WriteInt(tsc_offset, sizeof(unsigned int) << 3);

	{
		FILE* fp;

		// Attempt to open the file for writing
		if ((fp = fopen(fileName, "wb")) == NULL)
		{
			printf("ERROR: " __FUNCTION__ "() - Failed to open file \"%s\"\r\n", fileName);
			return false;
		}

		fwrite(stream.mBuffer, stream.mBufferLen, 1, fp);

		// We're finished with the file, so close it.
		fclose(fp);
	}

	stream.Clear();

	// Done!
	return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define HASH_CHAR(val)			CHAR_DATA  = char(val);											memcpy(HashPtr, &CHAR_DATA, sizeof(char));		HashPtr += sizeof(char);
#define HASH_SHORT(val)			SHORT_DATA = short(val);										memcpy(HashPtr, &SHORT_DATA, sizeof(short));	HashPtr += sizeof(short);
#define HASH_INT(val)			INT_DATA   = int(val);											memcpy(HashPtr, &INT_DATA, sizeof(int));		HashPtr += sizeof(int);
#define HASH_STR(val)			INT_DATA   = SuperFastHash(val, strlen(val));					memcpy(HashPtr, &INT_DATA, sizeof(int));		HashPtr += sizeof(int);
#define HASH_DATA(val, size)	INT_DATA   = SuperFastHash((const char*)val, size);				memcpy(HashPtr, &INT_DATA, sizeof(int));		HashPtr += sizeof(int);
#define PERFORM_HASH()			INT_DATA   = SuperFastHash(HashBuffer, HashPtr - HashBuffer);	memcpy(HashBuffer, &INT_DATA, sizeof(int));		HashPtr  = HashBuffer + 4;	memset(HashBuffer + 4, 0, sizeof(HashBuffer) - 4);

unsigned int CompiledMapResource::CalculateCRC()
{
	// [int]	Map width
	// [int]	Map height
	// [int]    Entities hash
	// [int]    Tiles hash
	// [int]    Decrypted TSC hash
	// [int]    Zones hash
	// [int]	Entity count
	// [int]	Zone count
	// [int]	TSC size
	//	 +36 Bytes

	// The actual data
	char HashBuffer[41];
	char* HashPtr = HashBuffer + 4;

	// Zeromem the hash buffer
	memset(HashBuffer, 0, sizeof(HashBuffer));

	// Data storage variables
	short SHORT_DATA = 0;
	char CHAR_DATA   = 0;
	int INT_DATA     = 0;

	// Hash all elements
	HASH_INT(m_MapInfo.width);
	HASH_INT(m_MapInfo.height);
	HASH_DATA(m_Chunks.entity_data.entities,		sizeof(PXCM_ENTITY) * m_Chunks.entity_data.count);
	HASH_DATA(m_Chunks.tile_data.tiles,				sizeof(unsigned char) * (m_MapInfo.width * m_MapInfo.height));
	HASH_DATA(m_Chunks.tsc_data.decrypted_data,		m_Chunks.tsc_data.size);
	HASH_DATA(m_Chunks.zone_data.zones,				sizeof(PXCM_ZONE) * m_Chunks.zone_data.count);
	HASH_INT(m_Chunks.entity_data.count);
	HASH_INT(m_Chunks.zone_data.count);
	HASH_INT(m_Chunks.tsc_data.size);
	PERFORM_HASH();

	return *((int*)HashBuffer);
}

#undef HASH_CHAR
#undef HASH_SHORT
#undef HASH_INT
#undef HASH_STR
#undef HASH_DATA
#undef PERFORM_HASH

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CompiledMapResource::Reset()
{
	// Free everything
	if (m_Chunks.entity_data.entities)
	{
		for (int i = 0; i < m_Chunks.entity_data.count; i++)
			if (m_Chunks.entity_data.entities[i].property_list)
				free(m_Chunks.entity_data.entities[i].property_list);

		free(m_Chunks.entity_data.entities);
	}

	if (m_Chunks.tile_data.tiles)
		free(m_Chunks.tile_data.tiles);

	if (m_Chunks.tsc_data.decrypted_data)
		free(m_Chunks.tsc_data.decrypted_data);
	
	if (m_Chunks.tsc_data.encrypted_data)
		free(m_Chunks.tsc_data.encrypted_data);
	
	if (m_Chunks.zone_data.zones)
		free(m_Chunks.zone_data.zones);

	// Reset memory
	memset(&m_Chunks, 0, sizeof(m_Chunks));
	memset(&m_MapInfo, 0, sizeof(m_MapInfo));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CompiledMapResource::SetTile(int tile_idx, int x, int y)
{
	if (!m_Chunks.tile_data.tiles || x < 0 || y < 0 || x >= m_MapInfo.width || y >= m_MapInfo.height)
		return;

	m_Chunks.tile_data.tiles[(y * m_MapInfo.width) + x] = tile_idx;
}

int CompiledMapResource::GetTile(int x, int y)
{
	if (x < 0 || y < 0 || x >= m_MapInfo.width || y >= m_MapInfo.height)
		return -1;

	return m_Chunks.tile_data.tiles[(y * m_MapInfo.width) + x];
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CompiledMapResource::AddEntity(int npc_id, int x, int y, int flag, int bits, int event, int index)
{
	// Entity array is full
	if (m_Chunks.entity_data.count >= 342)
		return false;

	// Resize array to fit a new entity
	if (!m_Chunks.entity_data.entities)
	{
		m_Chunks.entity_data.entities = (PXCM_ENTITY*)malloc(sizeof(PXCM_ENTITY));
		m_Chunks.entity_data.count    = 1;
	}
	else
	{
		m_Chunks.entity_data.count   += 1;
		m_Chunks.entity_data.entities = (PXCM_ENTITY*)realloc((void*)m_Chunks.entity_data.entities, sizeof(PXCM_ENTITY) * m_Chunks.entity_data.count);
	}

	PXCM_ENTITY* pEnt = NULL;

#ifdef _CAVESTORY_MULTIPLAYER
	if (index == -1)
		pEnt = &m_Chunks.entity_data.entities[m_Chunks.entity_data.count - 1];
	else
	{
		LIST_INSERT(m_Chunks.entity_data.entities, m_Chunks.entity_data.count, PXCM_ENTITY*, index);
		pEnt = &m_Chunks.entity_data.entities[index];
	}
#else
	pEnt = &m_Chunks.entity_data.entities[m_Chunks.entity_data.count - 1];
#endif

	memset(pEnt, 0, sizeof(PXCM_ENTITY));

	pEnt->code_char  = npc_id;
	pEnt->x          = x;
	pEnt->y          = y;
	pEnt->code_flag  = flag;
	pEnt->bits       = bits;
	pEnt->code_event = event;

	return true;
}

bool CompiledMapResource::RemoveEntity(int entity_index)
{
	if (!m_Chunks.entity_data.count || !m_Chunks.entity_data.entities || entity_index < 0 || entity_index >= m_Chunks.entity_data.count)
		return false;

	PXCM_ENTITY* pEntity = &m_Chunks.entity_data.entities[entity_index];

	if (pEntity->property_list)
	{
		free(pEntity->property_list);
		pEntity->property_list = NULL;
	}

	// If there is only one entity in the map now, then we can just clear all entities.
	if (m_Chunks.entity_data.count == 1)
	{
		ClearEntities();
		return true;
	}

	// Remove this entity
	LIST_ERASE(m_Chunks.entity_data.entities, m_Chunks.entity_data.count, PXCM_ENTITY*, entity_index);
	--m_Chunks.entity_data.count;

	// Done
	return true;
}

void CompiledMapResource::ClearEntities()
{
	if (!m_Chunks.entity_data.count || !m_Chunks.entity_data.entities)
		return;

	// Go through old list
	for (int i = 0; i < m_Chunks.entity_data.count; i++)
		if (m_Chunks.entity_data.entities[i].property_list)
			free(m_Chunks.entity_data.entities[i].property_list);

	// Free old array
	free(m_Chunks.entity_data.entities);
	m_Chunks.entity_data.entities = NULL;

	// Clear number of entities
	m_Chunks.entity_data.count = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned int CompiledMapResource::GetEntityPropertyNameCRC(const char* pFieldName)
{
	char pBuffer[MAX_ENTITY_PROPERTY_STRING_LENGTH];

	// Copy into the buffer
	strcpy_s(pBuffer, MAX_ENTITY_PROPERTY_STRING_LENGTH, pFieldName);

	// Lowercase the entire string
	strlwr(pBuffer);

	// Calculate the hash
	return SuperFastHash(pBuffer, strlen(pBuffer));
}

void CompiledMapResource::SetEntityProperty(int iEntityIndex, const char* pFieldName, const char* pFieldValue)
{
	if (iEntityIndex < 0 || iEntityIndex >= m_Chunks.entity_data.count || !pFieldName)
		return;

	PXCM_ENTITY_CUSTOM_PROPERTY* pProperty = NULL;
	PXCM_ENTITY* pEntity                   = &m_Chunks.entity_data.entities[iEntityIndex];
	unsigned int iCrc                      = GetEntityPropertyNameCRC(pFieldName);

	// Try to find it
	for (int i = 0; i < pEntity->property_count; i++)
	{
		if (pEntity->property_list[i].name_crc != iCrc)
			continue;

		if (!pFieldValue)
		{
			// We're deleting this field.
			if (pEntity->property_count == 1)
			{
				free(pEntity->property_list);
				pEntity->property_list  = NULL;
				pEntity->property_count = 0;
			}
			else
			{
				LIST_ERASE(pEntity->property_list, pEntity->property_count, PXCM_ENTITY_CUSTOM_PROPERTY, i);

				pEntity->property_count -= 1;
				pEntity->property_list   = (PXCM_ENTITY_CUSTOM_PROPERTY*)realloc((void*)pEntity->property_list, sizeof(PXCM_ENTITY_CUSTOM_PROPERTY) * pEntity->property_count);
				--i;
			}

			return;
		}

		// Found it! Set the value.
		strcpy(pEntity->property_list[i].value, pFieldValue);
		return;
	}

	if (!pFieldValue)
		return;

	// Allocate a new slot
	if (!pEntity->property_list)
	{
		pEntity->property_count = 1;
		pEntity->property_list  = (PXCM_ENTITY_CUSTOM_PROPERTY*)malloc(sizeof(PXCM_ENTITY_CUSTOM_PROPERTY) * pEntity->property_count);
	}
	else
	{
		pEntity->property_count += 1;
		pEntity->property_list   = (PXCM_ENTITY_CUSTOM_PROPERTY*)realloc((void*)pEntity->property_list, sizeof(PXCM_ENTITY_CUSTOM_PROPERTY) * pEntity->property_count);
	}

	// Configure the property
	pEntity->property_list[pEntity->property_count - 1].name_crc = iCrc;
	strcpy(pEntity->property_list[pEntity->property_count - 1].name, pFieldName);
	strcpy(pEntity->property_list[pEntity->property_count - 1].value, pFieldValue);
}

bool CompiledMapResource::GetEntityProperty(int iEntityIndex, int iFieldIndex, char* pOutValue)
{
	if (iEntityIndex < 0 || iEntityIndex >= m_Chunks.entity_data.count || iFieldIndex < 0 || iFieldIndex >= m_Chunks.entity_data.entities[iEntityIndex].property_count)
		return false;

	PXCM_ENTITY_CUSTOM_PROPERTY* pProperty = NULL;
	PXCM_ENTITY* pEntity                   = &m_Chunks.entity_data.entities[iEntityIndex];

	if (pOutValue)
		strcpy(pOutValue, pEntity->property_list[iFieldIndex].value);

	return true;
}

int CompiledMapResource::GetEntityPropertyFieldIndex(int iEntityIndex, const char* pFieldName)
{
	if (iEntityIndex < 0 || iEntityIndex >= m_Chunks.entity_data.count || !pFieldName || !*pFieldName)
		return -1;

	PXCM_ENTITY* pEntity = &m_Chunks.entity_data.entities[iEntityIndex];
	unsigned int iCrc    = GetEntityPropertyNameCRC(pFieldName);

	// Try to find it
	for (int i = 0; i < pEntity->property_count; i++)
		if (pEntity->property_list[i].name_crc == iCrc)
			return i;

	// Doesn't exist
	return -1;
}

int CompiledMapResource::GetEntityPropertyFieldIndexByCrc(int iEntityIndex, unsigned int iFieldCrc)
{
	if (iEntityIndex < 0 || iEntityIndex >= m_Chunks.entity_data.count)
		return -1;

	PXCM_ENTITY* pEntity = &m_Chunks.entity_data.entities[iEntityIndex];

	// Try to find it
	for (int i = 0; i < pEntity->property_count; i++)
		if (pEntity->property_list[i].name_crc == iFieldCrc)
			return i;

	// Doesn't exist
	return -1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CompiledMapResource::AddZone(int id, int x, int y, int w, int h)
{
	// Resize array to fit a new entity
	if (!m_Chunks.zone_data.zones)
	{
		m_Chunks.zone_data.count = 1;
		m_Chunks.zone_data.zones = (PXCM_ZONE*)malloc(sizeof(PXCM_ZONE));
	}
	else
	{
		m_Chunks.zone_data.count += 1;
		m_Chunks.zone_data.zones  = (PXCM_ZONE*)realloc((void*)m_Chunks.zone_data.zones, sizeof(PXCM_ZONE) * m_Chunks.zone_data.count);
	}

	memset(&m_Chunks.zone_data.zones[m_Chunks.zone_data.count - 1], 0, sizeof(PXCM_ZONE));

	// Add the zone
	PXCM_ZONE* pZone = &m_Chunks.zone_data.zones[m_Chunks.zone_data.count - 1];
	pZone->id        = id;
	pZone->x         = x;
	pZone->y         = y;
	pZone->w         = w;
	pZone->h         = h;
}

void CompiledMapResource::RemoveZone(int id)
{
	if (!m_Chunks.zone_data.count || !m_Chunks.zone_data.zones || id < 0 || id >= m_Chunks.zone_data.count)
		return;

	// Loop through all zones to populate the new array
	for (int i = 0; i < m_Chunks.zone_data.count; i++)
	{
		PXCM_ZONE* pZone = &m_Chunks.zone_data.zones[i];

		if (pZone->id != id)
			continue;

		// If there is only one entity in the map now, then we can just clear all entities.
		if (m_Chunks.zone_data.count == 1)
		{
			ClearZones();
			return;
		}

		// Remove this entity
		LIST_ERASE(m_Chunks.zone_data.zones, m_Chunks.zone_data.count, PXCM_ZONE*, id);
		--m_Chunks.zone_data.count;
		--i;
	}
}

void CompiledMapResource::ClearZones()
{
	if (!m_Chunks.zone_data.count || !m_Chunks.zone_data.zones)
		return;

	// Free old array
	free(m_Chunks.zone_data.zones);
	m_Chunks.zone_data.zones = NULL;

	// Clear number of entities
	m_Chunks.zone_data.count = 0;
}

unsigned short* CompiledMapResource::AllocateZoneArray()
{
	// We cannot do anything if there is not enough information to build a zone array.
	if (!m_MapInfo.width || !m_MapInfo.height || !m_Chunks.zone_data.count)
		return NULL;

	// Allocate the new array
	unsigned short* pZoneArray = (unsigned short*)malloc(sizeof(unsigned short) * (m_MapInfo.width * m_MapInfo.height));

	// Loop through all zones to populate the new array
	for (int i = 0; i < m_Chunks.zone_data.count; i++)
	{
		PXCM_ZONE* pZone = &m_Chunks.zone_data.zones[i];

		// Populate the array
		for (int j = 0; j < pZone->w * pZone->h; j++)
			pZoneArray[(pZone->y + (j / pZone->w)) * m_MapInfo.width + (pZone->x + (j % pZone->w))] = i;
	}

	return pZoneArray;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CompiledMapResource::TSC_AddString(const char* str, unsigned int size)
{
	int iAddPos = m_Chunks.tsc_data.size;
	int iStrLen = (size == 0xFFFFFFFF ? strlen(str) : size);

	// Resize to fit new string
	if (!m_Chunks.tsc_data.decrypted_data && !m_Chunks.tsc_data.encrypted_data)
	{
		// Allocate a new buffer
		m_Chunks.tsc_data.size           = iStrLen;
		m_Chunks.tsc_data.decrypted_data = (unsigned char*)malloc(m_Chunks.tsc_data.size + 1);
		m_Chunks.tsc_data.encrypted_data = (unsigned char*)malloc(m_Chunks.tsc_data.size + 1);
	}
	else
	{
		// Reallocate
		m_Chunks.tsc_data.size          += iStrLen;
		m_Chunks.tsc_data.decrypted_data = (unsigned char*)realloc((void*)m_Chunks.tsc_data.decrypted_data, m_Chunks.tsc_data.size + 1);
		m_Chunks.tsc_data.encrypted_data = (unsigned char*)realloc((void*)m_Chunks.tsc_data.encrypted_data, m_Chunks.tsc_data.size + 1);
	}

	// Copy into the decrypted buffer
	memset(m_Chunks.tsc_data.decrypted_data + iAddPos, 0, iStrLen);
	memcpy(m_Chunks.tsc_data.decrypted_data + iAddPos, str, iStrLen);
	m_Chunks.tsc_data.decrypted_data[m_Chunks.tsc_data.size] = 0;
}

void CompiledMapResource::TSC_Clear()
{
	if (m_Chunks.tsc_data.decrypted_data && m_Chunks.tsc_data.encrypted_data)
	{
		free(m_Chunks.tsc_data.decrypted_data);
		free(m_Chunks.tsc_data.encrypted_data);

		m_Chunks.tsc_data.decrypted_data = NULL;
		m_Chunks.tsc_data.encrypted_data = NULL;
	}

	m_Chunks.tsc_data.size = 0;
}