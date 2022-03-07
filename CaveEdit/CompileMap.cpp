#include "stdafx.h"
#include <fstream>
#include <string>

#include "General.h"
#include "GeneralDefines.h"
#include "SoundLoad.h"
#include "Organya.h"
#include "ProgressBar.h"

#include "CompiledMapResource.h"

#include "../src/CompiledMapShared.h"
#include "../src/BitStream.h"
#include "CRC32.h"

#define DECLARE_COMPILE_TASK(id, enum_name, text) enum { enum_name = id }; static const char* MapCompile_Task_##id = text; static const char* MapCompile_Task_##enum_name = text;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                   ID         Enumeration Name				    Task Description
DECLARE_COMPILE_TASK(0,			CTASK_READ_SCRIPT,					"Loading text script");
DECLARE_COMPILE_TASK(1,			CTASK_DECRYPT_SCRIPT,				"Decrypting text script");
DECLARE_COMPILE_TASK(2,			CTASK_ENCRYPT_SCRIPT,				"Encrypting text script");
DECLARE_COMPILE_TASK(3,			CTASK_CALCULATE_NETGROUP_RECTS,		"Calculating netgroup rects");

DECLARE_COMPILE_TASK(4,			CTASK_WRITE_MAP_INFO,				"Writing map info");
DECLARE_COMPILE_TASK(5,			CTASK_WRITE_TILE_DATA,				"Writing tile data");
DECLARE_COMPILE_TASK(6,			CTASK_WRITE_ENTITY_INFO,			"Writing entity info");
DECLARE_COMPILE_TASK(7,			CTASK_WRITE_NETGROUPS,				"Writing net group rects");
DECLARE_COMPILE_TASK(8,			CTASK_CALCULATE_CHECKSUM,			"Calculating CRC checksum");

#define MAX_CTASK_COUNT CTASK_CALCULATE_CHECKSUM

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define COMPILETASK_LIST_FORMAT(idx) "%s - (Step %d / %d)", MapCompile_Task_##idx, idx + 1, MAX_CTASK_COUNT
#define INCREMENT_COMPILETASK_LIST(task_ident) \
	if (update_progress)\
	{\
		CString tmp_str;\
		if (task_ident < MAX_CTASK_COUNT)\
		{\
			tmp_str.Format(COMPILETASK_LIST_FORMAT(task_ident));\
			ProgressWindow::UpdateProgressCurrent(task_ident, tmp_str.GetString());\
		}\
	}
	

static int gCompileMapStart   = 0;
static int gCurrentCompileMap = 0;
static int gCompileMapCount   = 0;
static bool gCompilingMapList = false;
DECLARE_PROGRESS_TASK(CompileAllMaps, "Compiling maps...", "Compile All Maps - CaveStory Editor")
{
	CaveStoryMOD* exe = (CaveStoryMOD*)(user_data[0]);
	CString tmp_str   = "";

	// Set max values
	SetCurrentProgressMax(MAX_CTASK_COUNT);
	SetOverallProgressMax(exe->mod.mStages.count);

	// Update overall progress
	tmp_str.Format("Compiling map %04d / %04d (%d%%)...", gCurrentCompileMap, exe->mod.mStages.count, 100 * gCurrentCompileMap / gCompileMapCount);
	UpdateProgressOverall(gCurrentCompileMap, tmp_str.GetString());

	// Update current progress
	tmp_str.Format(COMPILETASK_LIST_FORMAT(0));
	UpdateProgressCurrent(0, tmp_str.GetString());

	// Compile the map
	exe->CompileMap(gCompileMapStart + gCurrentCompileMap, true);

	// Done
	gCurrentCompileMap++;
	if (gCurrentCompileMap == gCompileMapCount)
	{
		gCompilingMapList = false;
		EndTask();
	}
}

bool CaveStoryMOD::CompileMaps(int start, int count)
{
	gCurrentCompileMap = 0;
	gCompileMapStart   = start;
	gCompileMapCount   = count == -1 ? mod.mStages.count : count;
	gCompilingMapList  = true;

	START_PROGRESS_TASK(CompileAllMaps, this);
	return true;
}

bool CaveStoryMOD::CompileMap(int index, bool update_progress)
{
	CaveMapINFO* mInfo = &CaveMapINFO(exe.mod.mStages.list[index % mod.mStages.count], &exe.mod);
	if (!strcmp(mInfo->fileName, "\\empty"))
		return false;

	CompiledMapResource pMapOut;

	std::vector<CaveMapZone> CompiledZoneList;
	unsigned char* scriptText = NULL;
	char filename[128];
	char tsc_file[128];
	int highest_netgroup_id = 0;
	int tsc_shift           = 0;
	int tsc_size            = 0;
	int net_group_list_size = 0;
	CaveMap map;
	CFile scrFile;

	// Load the map
	if (map.load(mInfo))
		return false;

	// Allocate the map
	if (!pMapOut.AllocateMap(map.width, map.height))
		return false;

	// Load information
	highest_netgroup_id = map.zones.size();

	// Build file name
	sprintf(filename, "%s/Stage/%s.pxcm", exe.mod.mModPath, mInfo->fileName);
	sprintf(tsc_file, "%s/Stage/%s.tsc", exe.mod.mModPath, mInfo->fileName);

	// Read the script file
	if (scrFile.Open(tsc_file, CFile::modeRead | CFile::shareDenyNone))
	{
		INCREMENT_COMPILETASK_LIST(CTASK_READ_SCRIPT);
		tsc_size = scrFile.GetLength();

		// Allocate the buffer for the script
		scriptText = new unsigned char[tsc_size + 1];

		// Read into the buffer & close the file
		scrFile.Read(scriptText, tsc_size);
		scrFile.Close();

		INCREMENT_COMPILETASK_LIST(CTASK_DECRYPT_SCRIPT);

		// Decode the file first...
		tsc_shift = scriptText[tsc_size / 2];
		for (int i = 0; i < tsc_size; i++)
			if (i != tsc_size / 2)
				scriptText[i] -= tsc_shift;

		// Just add it the map
		pMapOut.TSC_AddString((const char*)scriptText, tsc_size);
	}
	else
	{
		// Bypass all three tasks
		INCREMENT_COMPILETASK_LIST(CTASK_READ_SCRIPT);
		INCREMENT_COMPILETASK_LIST(CTASK_DECRYPT_SCRIPT);
		INCREMENT_COMPILETASK_LIST(CTASK_ENCRYPT_SCRIPT);
	}

	INCREMENT_COMPILETASK_LIST(CTASK_CALCULATE_NETGROUP_RECTS);

	int unused;
	if (!map.buildNetGroups(CompiledZoneList, unused))
	{
		CompiledZoneList.clear();
		CompiledZoneList.push_back({ 0, 0, 0, map.width, map.height });
	}

	net_group_list_size = CompiledZoneList.size();

	INCREMENT_COMPILETASK_LIST(CTASK_WRITE_MAP_INFO);

	// Ready some variables
	const unsigned char FileVersion  = COMPILED_MAP_COMPILED_VERSION;
	const unsigned short EntityCount = map.entityNum;

	if (!gCompilingMapList)
	{
		printf("Saving map \"%s\"\r\n", filename);
		printf("  >     Map size = (%d, %d)\r\n", map.width, map.height);
		printf("  >      TscSize = %d\r\n", tsc_size);
		printf("  > Entity Count = %d\r\n", EntityCount);
	}

	printf("  > Entity Count = %d\r\n", EntityCount);
	printf("  > Rects        = %d\r\n", CompiledZoneList.size());

	INCREMENT_COMPILETASK_LIST(CTASK_WRITE_TILE_DATA);

	// Copy the tiles into the buffer
	memcpy(pMapOut.m_Chunks.tile_data.tiles, map.tiles, map.width * map.height);

	INCREMENT_COMPILETASK_LIST(CTASK_WRITE_ENTITY_INFO);

	// Write entities
	for (int i = 0; i < EntityCount; i++)
	{
		Entity* ent = &map.entity[i];

		pMapOut.AddEntity(ent->npc, ent->x, ent->y, ent->flagID, ent->flags, ent->event);

		for (int j = 0; j < map.properties.size(); j++)
		{
			if (map.properties[j].ent_idx != i)
				continue;

			pMapOut.SetEntityProperty(pMapOut.m_Chunks.entity_data.count - 1, map.properties[j].name, map.properties[j].value);
		}
	}

	// Write rects
	INCREMENT_COMPILETASK_LIST(CTASK_WRITE_NETGROUPS);

	for (std::vector<CaveMapZone>::iterator it = CompiledZoneList.begin(); it != CompiledZoneList.end(); it++)
	{
		CaveMapZone* pRect = &(*it);

		pMapOut.AddZone(pRect->id, pRect->x, pRect->y, pRect->w, pRect->h);
	}

	// Begin checksum calculation
	INCREMENT_COMPILETASK_LIST(CTASK_CALCULATE_CHECKSUM);

	if (!pMapOut.Save(filename))
	{
		printf("ERROR: Failed to save map to \"%s\"!\r\n", filename);

		CompiledZoneList.clear();
		if (scriptText)
			delete[] scriptText;

		return false;
	}

	printf("  > CRC Result   = %d\r\n", pMapOut.m_MapInfo.crc);

	// Free things
	CompiledZoneList.clear();
	if (scriptText)
		delete[] scriptText;

	// Done!
	return true;
}