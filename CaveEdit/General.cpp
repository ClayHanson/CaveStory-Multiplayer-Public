//File created by David Shafer (C) 2006
#include "stdafx.h"
#include <fstream>
#include <string>
#include <stdarg.h>

#include "General.h"
#include "GeneralDefines.h"
#include "SoundLoad.h"
#include "Organya.h"
#include <CaveNet_Config.h>
#include "../src/Mod/CSMAPI_enums.h"

static char _avar_buffer[2048];
const char* avar(const char* _Format, ...) {
	va_list ptr;
	va_start(ptr, _Format);
	vsprintf(_avar_buffer, _Format, ptr);
	va_end(ptr);
	return _avar_buffer;
}

CString CS_TILE_TYPE_NAME = "CE_Tiles.png";
CString CS_NET_GRID       = "CE_NetGrid.png";

#ifdef _DEBUG
#define _CONSOLE
#endif
#define CLAMP(A, B, C) ((A) < (B) ? (B) : ((A) > (C) ? (C) : (A)))
#define CS_TILES_INI_FILE "CE_Tiles.ini"
#define CS_NPC_INI_FILE   "CE_NPC.ini"
#define CS_NPC_FILE       "CE_NPC.txt"
#define CS_TSC_INFO_FILE  "CE_TSC_Info.txt"
#define CS_TSC_ARGT_FILE  "CE_TSC_argNames.txt"
#define ERROR_FILE        "CE_error.log"

TSC_ArgType_List TSC_argTypeLists[TSC_argType::tat_count];

//************ Begin Global Variables *******************
//exe offsets
int CAVE_MAP_INFO = 0x0937B0;//this is where the map info is stored in file

CString filePath;
CString progPath;
CString EXEname;
CString EXEpath;

CaveStoryMOD exe;

//semi constants
char* CS_DEFAULT_PBM_EXT;
int  CS_DEFAULT_SCALE           = 2;
bool CS_DEFAULT_TILESET_WINDOW  = true;
bool CS_DEFAULT_WHEEL_ZOOM      = false;
int  CS_DEFAULT_TILESET_SCALE   = 2;
BYTE CS_DEFAULT_TILETYPE_ALPHA  = 128;
bool CS_DEFUALT_READ_ONLY       = false;
bool CS_DEFAULT_COMPILE_ON_SAVE = true;
bool CS_DEFAULT_USE_RECT_XYWH   = true;
bool CS_DEFAULT_PLAY_SOUND      = false;
char CS_DEFAULT_MOD_PATH[260]   = { 0 };
int CS_GRAPHIC_COLOR_DEPTH      = 32;

// Map editor config
bool CS_DEFAULT_ME_FOCUS_ON_MAP_LOAD = true;
bool CS_DEFAULT_ME_MAXIMIZE_ON_INIT  = true;
bool CS_DEFAULT_ME_SYNC_ACROSS_TABS  = true;

#include "BitStream.h"

#define CAVEEDITOR_CONFIG_VERSION 2
void SaveConfig_CaveEditor()
{
	CFile config;
	SharedBitStream stream;

	// Write everything
	stream.WriteInt(CAVEEDITOR_CONFIG_VERSION, sizeof(unsigned short) << 3);
	stream.WriteInt(CS_DEFAULT_SCALE, sizeof(unsigned int) << 3);
	stream.WriteFlag(CS_DEFAULT_TILESET_WINDOW);
	stream.WriteFlag(CS_DEFAULT_WHEEL_ZOOM);
	stream.WriteInt(CS_DEFAULT_TILESET_SCALE, sizeof(unsigned int) << 3);
	stream.WriteInt(CS_DEFAULT_TILETYPE_ALPHA, sizeof(unsigned char) << 3);
	stream.WriteFlag(CS_DEFUALT_READ_ONLY);
	stream.WriteInt(CS_GRAPHIC_COLOR_DEPTH, sizeof(unsigned int) << 3);
	stream.WriteFlag(CS_DEFAULT_COMPILE_ON_SAVE);
	stream.WriteFlag(CS_DEFAULT_USE_RECT_XYWH);
	stream.WriteFlag(CS_DEFAULT_PLAY_SOUND);
	stream.WriteString(CS_DEFAULT_MOD_PATH, sizeof(CS_DEFAULT_MOD_PATH));
	stream.WriteFlag(CS_DEFAULT_ME_FOCUS_ON_MAP_LOAD);
	stream.WriteFlag(CS_DEFAULT_ME_MAXIMIZE_ON_INIT);
	stream.WriteFlag(CS_DEFAULT_ME_SYNC_ACROSS_TABS);

	if (config.Open("CaveEditor.ini", CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyWrite))
	{
		config.Write(stream.mBuffer, stream.mBufferLen);
		config.Close();
	}
}

void LoadConfig_CaveEditor()
{
	CFile config;
	SharedBitStream stream;

	if (!config.Open("CaveEditor.ini", CFile::modeRead))
	{
		// Save defaults
		SaveConfig_CaveEditor();
		return;
	}

	// Allocate the bitstream
	stream.Clear();
	stream.Resize(config.GetLength());

	// Read the entire file into the buffer
	stream.mBufferLen = config.GetLength();
	config.Read(stream.mBuffer, stream.mBufferLen);

	stream.SetCurPos(0);
	config.Close();

	// Read everything
	int iVersion = stream.ReadInt(sizeof(unsigned short) << 3);

	if (iVersion != CAVEEDITOR_CONFIG_VERSION)
		printf("CaveEditor 2.0 Configuration Version Mismatch (expetected %d, got %d); Attempting to read anyways\r\n", CAVEEDITOR_CONFIG_VERSION, iVersion);

	CS_DEFAULT_SCALE                = stream.ReadInt(sizeof(unsigned int) << 3);
	CS_DEFAULT_TILESET_WINDOW       = stream.ReadFlag();
	CS_DEFAULT_WHEEL_ZOOM           = stream.ReadFlag();
	CS_DEFAULT_TILESET_SCALE        = stream.ReadInt(sizeof(unsigned int) << 3);
	CS_DEFAULT_TILETYPE_ALPHA       = stream.ReadInt(sizeof(unsigned char) << 3);
	CS_DEFUALT_READ_ONLY            = stream.ReadFlag();
	CS_GRAPHIC_COLOR_DEPTH          = stream.ReadInt(sizeof(unsigned int) << 3);
	CS_DEFAULT_COMPILE_ON_SAVE      = stream.ReadFlag();
	CS_DEFAULT_USE_RECT_XYWH        = stream.ReadFlag();
	CS_DEFAULT_PLAY_SOUND           = stream.ReadFlag();
	stream.ReadString(CS_DEFAULT_MOD_PATH, sizeof(CS_DEFAULT_MOD_PATH));
	CS_DEFAULT_ME_FOCUS_ON_MAP_LOAD = (iVersion < 2 ? true : stream.ReadFlag());
	CS_DEFAULT_ME_MAXIMIZE_ON_INIT  = (iVersion < 2 ? true : stream.ReadFlag());
	CS_DEFAULT_ME_SYNC_ACROSS_TABS  = (iVersion < 3 ? true : stream.ReadFlag());
}

//************* End Global Variables ********************

//REG_NAME has been created to reduce the number of switch statements in the decoding fuctions
//the coding is slightly more complicated but it reduces several lines of code to a single line
char* REG_NAME[5][8] = {
{"0AX", "0CX", "0DX", "0BX", "0SP", "0BP", "0SI", "0DI"},//0 byte registers (don't exist)
{ "AL",  "CL",  "DL",  "BL",  "AH",  "CH",  "DH",  "BH"},//1 byte registers
{ "AX",  "CX",  "DX",  "BX",  "SP",  "BP",  "SI",  "DI"},//2 byte registers
{"3AX", "3CX", "3DX", "3BX", "3SP", "3BP", "3SI", "3DI"},//3 byte registers (don't exist)
{"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"} //4 byte registers
};

static const char* GetTabToken(const char* pString, char* pBuffer, int iTokenIndex, const char* pDefaultValue = "")
{
	// Init the output buffer
	int iCurrentToken = 0;
	char* pOut        = pBuffer;
	*pOut             = 0;

	for (const char* pPtr = pString; *pPtr != 0; pPtr++)
	{
		if (*pPtr == '\t' || *pPtr == ' ')
		{
		SEEK_NEXT_TOKEN:
			if (pPtr != pString)
				++iCurrentToken;

			// Get to the next token.
			for (; *pPtr != 0; pPtr++)
				if (*pPtr > ' ')
					break;

			--pPtr;

			continue;
		}

		// Skip this token if we don't want it
		if (iCurrentToken != iTokenIndex)
		{
			for (; *pPtr != 0; pPtr++)
				if (*pPtr <= ' ')
					break;

			// Safe guard
			if (*pPtr == 0)
				break;

			goto SEEK_NEXT_TOKEN;
		}

		const char* pStart = pPtr;

		// Found it!
		for (; *pPtr != 0; pPtr++)
		{
			if (*pPtr <= ' ')
				break;

			*pOut++ = *pPtr;
		}

		// Null terminate it
		*pOut++ = 0;

		// Done!
		return pStart;
	}

	strcpy(pBuffer, pDefaultValue);

	return NULL;
}


extern CString filePath;

using namespace std;

TileKey tileKey;

char CaveBitMap::extra8[8];

int CaveTileSet::count = 0;
CaveBitMap CaveTileSet::typeMap;
CaveBitMap CaveTileSet::netGrid;

CaveMapINFO& CaveMapINFO::operator=(CaveMapINFO& info)
{
	strcpy(background, info.background);
	backType = info.backType;
	bossNum = info.bossNum;
	area = info.area;
	focus_center_x = info.focus_center_x;
	focus_center_y = info.focus_center_y;
	map_index = info.map_index;
	strcpy(caption, info.caption);
	strcpy(fileName, info.fileName);
	strcpy(NPCset1, info.NPCset1);
	strcpy(NPCset2, info.NPCset2);
	strcpy(tileset, info.tileset);
	strcpy(unknown, info.unknown);
	return info;
};

CaveMapINFO::CaveMapINFO(const PXMOD_STAGE_STRUCT& stage, ModConfigResource* pRes)
{
	strcpy(background, pRes->mBackgrounds.list[stage.background].name);
	backType = stage.bkType;
	bossNum = stage.boss_no;
	area = stage.area_index;
	focus_center_x = stage.focus_center_x;
	focus_center_y = stage.focus_center_y;
	map_index = -1;
	
	for (int i = 0; i < pRes->mStages.count; i++)
	{
		if (&pRes->mStages.list[i] != &((PXMOD_STAGE_STRUCT&)stage))
			continue;

		map_index = i;
		break;
	}

	strcpy(caption, stage.name);
	strcpy(fileName, stage.file);
	strcpy(NPCset1, pRes->mSpritesheets.list[stage.npc_sheet].name);
	strcpy(NPCset2, pRes->mSpritesheets.list[stage.boss_sheet].name);
	strcpy(tileset, pRes->mTilesets.list[stage.tileset].name);
}

void CaveMapINFO::SetModStage(PXMOD_STAGE_STRUCT* pStage, ModConfigResource* pMod)
{
	int iCache;

	strcpy_s(pStage->file, sizeof(pStage->file), fileName);
	strcpy_s(pStage->name, sizeof(pStage->name), caption);
	
	pStage->boss_no        = (PXMOD_BOSS_TYPE)bossNum;
	pStage->bkType         = (PXMOD_BK_TYPE)backType;
	pStage->area_index     = area;
	pStage->focus_center_x = focus_center_x;
	pStage->focus_center_y = focus_center_y;
	pStage->background     = ((iCache = pMod->FindBackground(background)) == -1 ? pMod->AddBackground(background) : iCache);
	pStage->npc_sheet      = ((iCache = pMod->FindSpritesheet(NPCset1))   == -1 ? pMod->AddSpritesheet(NPCset1)   : iCache);
	pStage->boss_sheet     = ((iCache = pMod->FindSpritesheet(NPCset2))   == -1 ? pMod->AddSpritesheet(NPCset2)   : iCache);
	pStage->tileset        = ((iCache = pMod->FindTileset(tileset))       == -1 ? pMod->AddTileset(tileset)       : iCache);
}

CaveBitMap::CaveBitMap()
{
	file[0] = '\0';
}

CaveBitMap::~CaveBitMap()
{
	if (!IsNull())//if bitmap already loaded destroy it first
		Destroy();
}


void CaveBitMap::Resize(int w, int h)
{
	CImage temp;//new image

	RGBQUAD RGBquad[256];

	temp.Create(w, h, GetBPP());

	GetColorTable(0, GetMaxColorTableEntries(), RGBquad);//copy palette
	temp.SetColorTable(0, GetMaxColorTableEntries(), RGBquad);

	BitBlt(temp.GetDC(), 0, 0);//copy image
	temp.ReleaseDC();

	Destroy();//destroy old bitmap and attach new one
	Attach(temp.Detach());
}


//loads Bitmap replacing any existing one
bool CaveBitMap::Load(const char* f)
{
	if (!IsNull())//if bitmap already loaded destroy it first
		Destroy();

	strcpy(file, f);
	HRESULT h = CImage::Load(f);
	if (h != S_OK)
		return false;
	return true;
}

//saves BitMap to specified file
bool CaveBitMap::Save(const char* newfile)
{
	CFile f;
	CFileException e;

	CFileStatus status;

	CFileFind findfile;
	if (CS_DEFUALT_READ_ONLY && findfile.FindFile(newfile))
	{
		CFile::GetStatus(newfile, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(newfile, status);
	}

	CImage::Save(newfile, ImageFormatPNG);//regular image save

	if (!f.Open(newfile, CFile::modeNoTruncate | CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone, &e))
		return false;
	f.Seek(f.GetLength(),0);
	f.Write(extra8, 8);//add extra "(C)Pixel" data
	f.Close();

	return true;
};

//saves BitMap to file it was loaded from
bool CaveBitMap::Save()
{
	return Save(file);
};


//draws Part of Bitmap to Device Context
void CaveBitMap::DrawPart(int x, int y, int w, int h, CDC* cdc, int xCDC, int yCDC, bool trans, int color)
{
	BLENDFUNCTION blend;
	if (trans)
	{
		switch (GetBPP())
		{
		case 4:
		case 8:
			SetTransparentColor((LONG)0);
			Draw(cdc->m_hDC, xCDC, yCDC, w, h, x, y, w, h);
			break;
		case 32:
			blend.AlphaFormat = AC_SRC_ALPHA;
			blend.BlendFlags = 0;
			blend.BlendOp = AC_SRC_OVER;
			blend.SourceConstantAlpha = 255;

			::AlphaBlend(cdc->m_hDC, xCDC, yCDC, w, h, GetDC(), x, y, w, h, blend);
			ReleaseDC();
//			TransparentBlt(cdc->m_hDC, xCDC, yCDC, w, h, GetDC(), x, y, w, h, color);
//			ReleaseDC();
			break;
		default:
			Draw(cdc->m_hDC, xCDC, yCDC, w, h, x, y, w, h);
			break;
		};
	}
	else
	if (GetBPP() == 32)//this should mean alpha channel
	{
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.BlendFlags = 0;
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = 255;
		cdc->FillSolidRect(xCDC, yCDC, w, h, 0);
		::AlphaBlend(cdc->m_hDC, xCDC, yCDC, w, h, GetDC(), x, y, w, h, blend);
		ReleaseDC();
	}
	else
		BitBlt(cdc->m_hDC, xCDC, yCDC, w, h, x, y);
};

//draws Part of Bitmap to Device Context
void CaveBitMap::DrawPart(RECT rectSrc, CDC* cdc, RECT rectDest, bool trans, int color)
{
	int xCDC = rectDest.left;
	int yCDC = rectDest.top;
	int wCDC = rectDest.right-rectDest.left;
	int hCDC = rectDest.bottom-rectDest.top;

	int x = rectSrc.left;
	int y = rectSrc.top;
	int w = rectSrc.right-rectSrc.left;
	int h = rectSrc.bottom-rectSrc.top;

	BLENDFUNCTION blend;
	if (trans)
	{
		switch (GetBPP())
		{
		case 4:
		case 8:
			SetTransparentColor((LONG)0);
			Draw(cdc->m_hDC, xCDC, yCDC, wCDC, hCDC, x, y, w, h);
			break;
		case 32:
			blend.AlphaFormat = AC_SRC_ALPHA;
			blend.BlendFlags = 0;
			blend.BlendOp = AC_SRC_OVER;
			blend.SourceConstantAlpha = 255;

			::AlphaBlend(cdc->m_hDC, xCDC, yCDC, wCDC, hCDC, GetDC(), x, y, w, h, blend);
			ReleaseDC();
			break;
//			TransparentBlt(cdc->m_hDC, xCDC, yCDC, wCDC, hCDC, GetDC(), x, y, w, h, color);
//			ReleaseDC();
		default:
			Draw(cdc->m_hDC, xCDC, yCDC, wCDC, hCDC, x, y, w, h);
			break;
		};
	}
	else
	if (GetBPP() == 32)//this should mean alpha channel
	{
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.BlendFlags = 0;
		blend.BlendOp = AC_SRC_OVER;
		blend.SourceConstantAlpha = 255;
		cdc->FillSolidRect(xCDC, yCDC, w, h, 0);
		::AlphaBlend(cdc->m_hDC, xCDC, yCDC, wCDC, hCDC, GetDC(), x, y, w, h, blend);
		ReleaseDC();
	}
	else
		BitBlt(cdc->m_hDC, xCDC, yCDC, wCDC, hCDC, x, y);
}

TileKey::TileKey()
{
	ifstream file;
	char buffer[256];

	//this preserves pathing and loads from program directory
	::GetCurrentDirectory(255, buffer);
	::SetCurrentDirectory(progPath.GetString());
	file.open(CS_TILES_INI_FILE);
	::SetCurrentDirectory(buffer);

	int i, temp;

	file.getline(buffer, 256); //remove header line

	for (i = 0; i < 256; i++)
	{
		file >> temp;
		file >> buffer;
		file >> temp;
		file >> temp;
		file >> temp;

		file >> temp;
		file >> temp;
		file >> temp;
		file >> temp;
		file >> temp;//ignore first ten entries

		file >> temp;
		sPC[i] = temp;
		file >> temp;
		sNPC[i] = temp;
		file >> temp;
		water[i] = temp;
		file >> temp;
		front[i] = temp;
		file >> temp;
		seen[i] = temp;
		file.getline(buffer, 256); //remove description
	}

	file.close();
}

NPC_extras::NPC_extras()
{
	 loaded = false;
}


NPC_extras& NPC_extras::operator=(NPC_extras& assign)
{
	loaded = assign.loaded;
	ID = assign.ID;
	shortDesc1 = assign.shortDesc1;
	shortDesc2 = assign.shortDesc2;
	type = assign.type;
	option1 = assign.option1;
	option2 = assign.option2;
	desc = assign.desc;
	notes = assign.notes;
	return assign;
}


NPCtable* NPCtable::us = NULL;

NPCtable::NPCtable()
{
	char buffer[255];
	extraCount = 0;
	for (int i = 0; i < exe.mod.mNpcs.count; i++)
	{
		PXMOD_NPC_STRUCT* pNpc = &exe.mod.mNpcs.list[i];

		extra[i].ID = i;
		strcpy(buffer, pNpc->name); extra[i].desc = buffer;
		strcpy(buffer, pNpc->shortname1); extra[i].shortDesc1 = buffer;
		strcpy(buffer, pNpc->shortname2); extra[i].shortDesc2 = buffer;
		strcpy(buffer, pNpc->option1); extra[i].option1 = buffer;
		strcpy(buffer, pNpc->option2); extra[i].option2 = buffer;
		strcpy(buffer, pNpc->option3); extra[i].option3 = buffer;
		strcpy(buffer, pNpc->category); extra[i].type = buffer;
		extra[i].notes = "";
		extra[i].loaded = true;
		++extraCount;
	}

	us = this;
}


NPCtable::~NPCtable()
{
	us = NULL;
}

bool NPCtable::reload_current_mod_npc_table()
{
	char buffer[255];
	extraCount = 0;
	for (int i = 0; i < exe.mod.mNpcs.count; i++)
	{
		PXMOD_NPC_STRUCT* pNpc = &exe.mod.mNpcs.list[i];

		extra[i].ID = i;
		strcpy(buffer, pNpc->name); extra[i].desc = buffer;
		strcpy(buffer, pNpc->shortname1); extra[i].shortDesc1 = buffer;
		strcpy(buffer, pNpc->shortname2); extra[i].shortDesc2 = buffer;
		strcpy(buffer, pNpc->option1); extra[i].option1 = buffer;
		strcpy(buffer, pNpc->option2); extra[i].option2 = buffer;
		strcpy(buffer, pNpc->option3); extra[i].option3 = buffer;
		strcpy(buffer, pNpc->category); extra[i].type = (*buffer == 0 ? "Uncategorized" : buffer);
		extra[i].notes = "";
		extra[i].loaded = true;
		++extraCount;
	}

	return true;
}

//loads NPC table
bool NPCtable::load(const char* npcName)
{
	char buffer[255];
	extraCount = 0;
	for (int i = 0; i < exe.mod.mNpcs.count; i++)
	{
		PXMOD_NPC_STRUCT* pNpc = &exe.mod.mNpcs.list[i];

		extra[i].ID = i;
		strcpy(buffer, pNpc->name); extra[i].desc = buffer;
		strcpy(buffer, pNpc->shortname1); extra[i].shortDesc1 = buffer;
		strcpy(buffer, pNpc->shortname2); extra[i].shortDesc2 = buffer;
		strcpy(buffer, pNpc->option1); extra[i].option1 = buffer;
		strcpy(buffer, pNpc->option2); extra[i].option2 = buffer;
		strcpy(buffer, pNpc->option3); extra[i].option3 = buffer;
		strcpy(buffer, pNpc->category); extra[i].type = (*buffer == 0 ? "Uncategorized" : buffer);
		extra[i].notes = "";
		extra[i].loaded = true;
		++extraCount;
	}

	int version;
	int i;

	char temp[260];
	if (*npcName == 1)
	{
		fileName = npcName + 1;
		sprintf(temp, "%s", npcName + 1);
	}
	else
	{
		fileName = npcName;
		sprintf(temp, "%s/%s", exe.mod.mModPath, npcName);
	}

	CFile file;
	if (!file.Open(temp, CFile::modeRead | CFile::shareDenyNone))
		return false;

	char code[5];
	file.Read(code, 5);
	file.Read(&NPC_COUNT, sizeof(long));

	if (!memcmp(code, "PNT4", 5)) version = 4;
	else if (!memcmp(code, "PNT3", 5)) version = 3;
	else if (!memcmp(code, "PNT2", 5)) version = 2;
	else {
		version = 1;

		file.SeekToEnd();
		ULONGLONG len = file.GetPosition();
		file.SeekToBegin();

		NPC_COUNT = len / 24;
	}

	// Set 0s
	memset(entity, 0, sizeof(entity));

	//unlike other files, each individual value is stored sequentially instead of in structures
	for (i = 0; i < NPC_COUNT; i++)//read flags
	{
		file.Read(&(entity[i].flags), version == 1 ? 2 : 4);
	}
	if (version >= 3)
	{
		for (i = 0; i < NPC_COUNT; i++)//read nodes
		{
			file.Read(&(entity[i].nodes), 8);
		}

		if (version >= 4)
		{
			for (i = 0; i < NPC_COUNT; i++)//read nodes
			{
				file.Read(&(entity[i].transmit_nodes), 8);
			}
		}
	}
	for (i = 0; i < NPC_COUNT; i++)//read hp
	{
		file.Read(&(entity[i].hp), 2);
	}
	for (i = 0; i < NPC_COUNT; i++)//tileset data ???
	{
		file.Read(&(entity[i].tileSet), 1);
    	}
	for (i = 0; i < NPC_COUNT; i++)//graphic and death sound
	{
//		file.Read(&(entity[i].hurtSound), 1);
		file.Read(&(entity[i].deathSound), 1);
	}
	for (i = 0; i < NPC_COUNT; i++)//graphic and death sound
	{
		file.Read(&(entity[i].hurtSound), 1);
//		file.Read(&(entity[i].deathSound), 1);
	}
	for (i = 0; i < NPC_COUNT; i++)//death graphic
	{
		file.Read(&(entity[i].deathPBM), 1);
	}
	for (i = 0; i < NPC_COUNT; i++)//???
	{
		file.Read(&(entity[i].weaponDrop), 4);
	}
	for (i = 0; i < NPC_COUNT; i++)//attack power
	{
		file.Read(&(entity[i].attack), 4);
	}
	for (i = 0; i < NPC_COUNT; i++)//position data
	{
		file.Read(&(entity[i].hitLeft), 1);
		file.Read(&(entity[i].hitTop), 1);
		file.Read(&(entity[i].hitRight), 1);
		file.Read(&(entity[i].hitBottom), 1);
	}
	for (i = 0; i < NPC_COUNT; i++)//hit box???
	{
		file.Read(&(entity[i].hitX), 1);
		file.Read(&(entity[i].hitY), 1);
		file.Read(&(entity[i].cenX), 1);
		file.Read(&(entity[i].cenY), 1);
	}
	file.Close();

	return true;
}


//saves NPC table
bool NPCtable::save()
{
	return save("npc.tbl");
}

//saves to new name
#include "../src/NpChar_Shared.h"
bool NPCtable::save(const char* newName)
{
	int i;
	char temp[256];
	sprintf(temp, "%s/%s", exe.mod.mModPath, newName);

	CFile file;

	CFileStatus status;
	CFileFind findfile;
	if (CS_DEFUALT_READ_ONLY && findfile.FindFile(newName))
	{
		CFile::GetStatus(temp, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(temp, status);
	}

	if (!file.Open(temp, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		return false;

	long NPC_COUNT = NPC_TYPE_COUNT;
	file.Write("PNT4", 5);
	file.Write(&NPC_COUNT, sizeof(long));

	for (i = 0; i < NPC_TYPE_COUNT; i++)//write flags
		file.Write(&(entity[i].flags), 4);
	
	for (i = 0; i < NPC_TYPE_COUNT; i++)//write nodes
		file.Write(&(entity[i].nodes), 8);
	
	for (i = 0; i < NPC_TYPE_COUNT; i++)//write transmit nodes
		file.Write(&(entity[i].transmit_nodes), 8);

	for (i = 0; i < NPC_TYPE_COUNT; i++)// life
		file.Write(&(entity[i].hp), 2);

	for (i = 0; i < NPC_TYPE_COUNT; i++)// surf
		file.Write(&(entity[i].tileSet), 1);

	for (i = 0; i < NPC_TYPE_COUNT; i++)// destroy_voice
		file.Write(&(entity[i].deathSound), 1);

	for (i = 0; i < NPC_TYPE_COUNT; i++)//hit_voice
		file.Write(&(entity[i].hurtSound), 1);

	for (i = 0; i < NPC_TYPE_COUNT; i++)// size
		file.Write(&(entity[i].deathPBM), 1);

	for (i = 0; i < NPC_TYPE_COUNT; i++) //exp
		file.Write(&(entity[i].weaponDrop), 4);

	for (i = 0; i < NPC_TYPE_COUNT; i++) //damage
		file.Write(&(entity[i].attack), 4);

	for (i = 0; i < NPC_TYPE_COUNT; i++) //view
	{
		file.Write(&(entity[i].hitLeft), 1);
		file.Write(&(entity[i].hitTop), 1);
		file.Write(&(entity[i].hitRight), 1);
		file.Write(&(entity[i].hitBottom), 1);
	}

	for (i = 0; i < NPC_TYPE_COUNT; i++)//hit box???
	{
		file.Write(&(entity[i].hitX), 1);
		file.Write(&(entity[i].hitY), 1);
		file.Write(&(entity[i].cenX), 1);
		file.Write(&(entity[i].cenY), 1);
	}

	file.Close();

	::SetCurrentDirectory(temp);
	return true;
}

//displays NPC sprite based on upper left corner of tile
void NPCtable::displaySprite(int x, int y, int index, double scale, CDC* cdc2, CaveBitMap& tileset, CaveBitMap& NPCset1, CaveBitMap& NPCset2)
{
	// Have to re-do this eventually if we need to...
	printf("Draw NPC \"%s\" @ (%04d, %04d) w/ a scale of %f\r\n", exe.mod.mNpcs.list[index].name, x, y, float(scale));
}

CaveTileSet::CaveTileSet()
{
	typData = NULL;
	tileNum = 0;
	tileName[0] = '\0';
	if (count == 0)
	{
		typeMap.Load(CS_TILE_TYPE_NAME);
		netGrid.Load(CS_NET_GRID);
	}
	count++;
}

CaveTileSet::~CaveTileSet()
{
	count--;

	clean(typData);
}


//loads tileset
bool CaveTileSet::load(const char* tileSetName)
{
	int i;
	char temp[256];

	if (typData != NULL)
		delete[] typData;

	strcpy(tileName, tileSetName);

	//loads bitmap data
	sprintf(temp, "%s/Stage/Prt%s.png", exe.mod.mModPath, tileName);
	if (!tileData.Load(temp))
		return false;

	tileNum      = (tileData.GetWidth()/16)*(tileData.GetHeight()/16);//retrieves number of tiles from bitmap data
	int t_width  = (tileData.GetWidth() >> 4);
	int t_height = (tileData.GetHeight() / 16);

	//load tile type data
	typData = new unsigned char[tileNum];
	sprintf(temp, "%s/Stage/%s.pxa", exe.mod.mModPath, tileName);
	CFile file;
	if (!file.Open(temp, CFile::modeRead | CFile::shareDenyNone))
		return false;

	//read the data
	unsigned char ignore = 0;
	for (int y = 0; y < t_height; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			if (x < t_width)
				file.Read(typData + ((y*t_width)+x), 1);
			else
				file.Read(&ignore, 1);
		}
	}

	file.Close();

	return true;
}

//saves tileset
bool CaveTileSet::save()
{
	return save(tileName);
}

//saves tileset with new name
bool CaveTileSet::save(const char* tileSetName)
{
	int i;
	char temp[256];
	
	strcpy(tileName, tileSetName);

	int t_width  = (tileData.GetWidth() / 16);
	int t_height = (tileData.GetHeight() / 16);

	//saves bitmap data
	if (!tileData.Save())
		return false;

	//save tile type data
	sprintf(temp, "%s/Stage/%s.pxa", exe.mod.mModPath, tileName);

	CFileStatus status;
	CFileFind findfile;
	if (CS_DEFUALT_READ_ONLY && findfile.FindFile(tileSetName))
	{
		CFile::GetStatus(temp, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(temp, status);
	}

	CFile file;
	if (!file.Open(temp, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		return false;

	// write the data
	unsigned char ignore = 0;
	for (int y = 0; y < t_height; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			if (x < t_width)
				file.Write(typData + ((y * t_width) + x), 1);
			else
				file.Write(&ignore, 1);
		}
	}

	file.Close();

	return true;
}



//displays tile to specified coordinates on device context
void CaveTileSet::displayTile(int index, int x, int y, CDC* cdc, bool trans)
{
	int tileWide = tileData.GetWidth() >> 4;// get how many tiles wide the pbm data is
	double tileHigh = tileData.GetHeight() / 16.0; //for some reason a tileset pbm might not be a multiple of 16 (like PrtBarr.pbm)
	int _x, _y;

	_x = index % tileWide;
	_y = index / tileWide;

	tileData.DrawPart(_x*16, _y*16, 16, 16, cdc, x, y, trans, 0x000);
}

//displays tile type as tile
void CaveTileSet::displayTileType(int index, int x, int y, CDC* cdc, bool trans)
{
	int tileWide = 16;//tile.bmp is 16x16 tiles
	int tileHigh = 16;
	int _x, _y;

	index = typData[index];

	_x = index%tileWide;
	_y = index/tileWide;

/*
	if (trans)
		typeMap.Draw(*cdc, x, y, 16, 16, _x*16, _y*16, 16, 16);
	else
//*/
		typeMap.DrawPart(_x*16, _y*16, 16, 16, cdc, x, y, trans, 0x000);
}

//resizes tile set to hold at least size number of tiles
void CaveTileSet::resize(int size)
{
	int newSize, i;
	unsigned char* newType;

	newSize = size % (tileData.GetWidth() >> 4);
	if (newSize > 0)
		newSize = size + (tileData.GetWidth() >> 4) - newSize;//round up to nearest complete tile row
	else
		newSize = size;

	newType = new unsigned char[newSize];

	for (i = 0; i < newSize; i++)
	{
		if (i >= tileNum)
			newType[i] = 0;
		else
			newType[i] = typData[i];
	}

	delete [] typData;
	typData = newType;
	tileNum = newSize;

	tileData.Resize(tileData.GetWidth(), (tileNum << 8)/tileData.GetWidth());

}


CaveMap::CaveMap()
{
	netgroup_tiles = NULL;
	tiles = NULL;
	entity = NULL;
	width = 0;
	height = 0;
}


CaveMap::~CaveMap()
{
	clean(entity);
	clean(tiles);
	clean(netgroup_tiles);
}

//reloads map files
bool CaveMap::reload()
{
	char temp[1024];
	//read tileset data
	if (!tileSet.load(mapInfo.tileset))
		return false;

	//read background
	sprintf(temp, "%s/bk%s.png", exe.mod.mModPath, mapInfo.background);
	return background.Load(temp);
}

// Print an error into pBufferOut
void CaveMap::getErrorString(int iError, char* pBufferOut, int iBufferLen)
{
	const char* pErrorStrings[] =
	{
		"Unhandled error.",
		"Failed to locate this stage's '.PXM' file.",
		"Failed to locate this stage's '.PXE' file.",
		"The included '.PXE' file is newer than we expected.",
		"Failed to load the stage tileset.",
		"Failed to load the background.",
		"Failed to load the NPC spritesheet # 1.",
		"Failed to load the NPC spritesheet # 2."
	};

	strcpy_s(pBufferOut, iBufferLen, pErrorStrings[CLAMP(iError, 0, (sizeof(pErrorStrings) / sizeof(const char*)) - 1)]);
}

//loads map
int CaveMap::load(CaveMapINFO* info)
{
	oldFileName = info->fileName;
	char temp[1024];
	if (entity != NULL)
		delete[] entity;
	if (tiles != NULL)
		delete[] tiles;
	if (netgroup_tiles != NULL)
		delete[] netgroup_tiles;

	properties.clear();
	zones.clear();

	entity = NULL;
	tiles = NULL;
	netgroup_tiles = NULL;

	mapInfo = *info;

	//Start with tile data
	sprintf(temp, "%s/Stage/%s.pxm", exe.mod.mModPath, mapInfo.fileName);
	CFile file;
	if (!file.Open(temp, CFile::modeRead | CFile::shareDenyNone))
		return 1;

	file.Read(temp, 4);//remove useless header data

	int version = temp[3];

	printf("Version = %d\r\n", version);
	
	// Set defaults
	width  = 0;
	height = 0;

	// Read the map dimensions
	file.Read(&width, 2);
	file.Read(&height, 2);

	// Allocate the tiles & netgroup tiles
	tiles          = new unsigned char[width * height];
	netgroup_tiles = new unsigned char[width * height];

	file.Read(tiles, width * height);
	file.Close();//finished reading tile data

	int zoneCount = 0;

	// PXPR is a proprietary file, unique to Cave Story Multiplayer.
	SharedBitStream stream;
	sprintf(temp, "%s/Stage/%s.pxpr", exe.mod.mModPath, mapInfo.fileName);
	if (stream.LoadFromFile(temp))
	{
		char magic[4];
		stream.Read(magic, 4 << 3);
		if (memcmp(magic, "PXPR", 4))
			goto SKIP_PXPR_LOAD;

		int iVersion = stream.ReadInt(sizeof(unsigned short) << 3);
		int iCount   = stream.ReadInt(sizeof(unsigned int) << 3);

		properties.resize(iCount);

		for (int i = 0; i < iCount; i++)
		{
			properties[i].ent_idx = stream.ReadInt(sizeof(unsigned int) << 3);
			stream.ReadString(properties[i].name, 255);
			stream.ReadString(properties[i].value, 255);
		}

	SKIP_PXPR_LOAD:
		stream.Clear();
	}
	
	// PXN is a proprietary file, unique to Cave Story Multiplayer.
	sprintf(temp, "%s/Stage/%s.pxn", exe.mod.mModPath, mapInfo.fileName);
	if (file.Open(temp, CFile::modeRead | CFile::shareDenyNone))
	{
		char magic[3];
		file.Read(magic, 3);
		if (magic[0] == 'P' &&
			magic[1] == 'X' &&
			magic[2] == 'N'
		)
		{
			char zoneVersion = 0;

			// Read the version
			file.Read(&zoneVersion, sizeof(char));

			// Read general information
			file.Read(&zoneCount, sizeof(int));

			// Resize to fit all zones
			zones.resize(zoneCount);

			// Read zones
			for (int i = 0; i < zoneCount; i++)
			{
				unsigned char STR_LEN = 0;

				// Read the zone ID
				file.Read(&zones[i].id, sizeof(int));

				// Read the length
				file.Read(&STR_LEN, sizeof(unsigned char));

				// Read the entire string
				file.Read(zones[i].name, STR_LEN);
				zones[i].name[STR_LEN] = 0;
			}

			// Read tile data
			file.Read(netgroup_tiles, width * height);
		}
		else
		{
			file.Read(netgroup_tiles, width * height);
		}

		file.Close();
	}
	else
		memset(netgroup_tiles, 0, width*height);

	// Validate the zones
	for (int i = 0; i < width * height; i++)
		netgroup_tiles[i] = CLAMP(netgroup_tiles[i], 0, zoneCount - 1);

	//begin reading entity data
	sprintf(temp, "%s/Stage/%s.pxe", exe.mod.mModPath, mapInfo.fileName);
	if (!file.Open(temp, CFile::modeRead | CFile::shareDenyNone))
		return 2;
	
	version = 1;
	char code[5];
	file.Read(code, 4);//remove useless header data
	if (memcmp(code, "PXE", 4))
	{
		file.SeekToBegin();
		file.Read(code, 5);

		if (!memcmp(code, "PXE2", 5)) version = 2;
		else
		{
			printf("Failed to load file \"%s\"; \"not a valid PXE file\"\r\n", temp);
			file.Close();
			return 3;
		}
	}

	printf("Opening PXE file \"%s\" (version = %d)\r\n", temp, version);

	entityNum = 0;
	file.Read(&entityNum, 4);

	entity = new Entity[entityNum];

	for (int i = 0; i < entityNum; i++)
	{
		Entity* eve = &entity[i];
		eve->flags  = 0;

		file.Read(&eve->x,		sizeof(unsigned short));
		file.Read(&eve->y,		sizeof(unsigned short));
		file.Read(&eve->flagID,	sizeof(unsigned short));
		file.Read(&eve->event,	sizeof(unsigned short));
		file.Read(&eve->npc,	sizeof(unsigned short));
		file.Read(&eve->flags,	version == 1 ? sizeof(unsigned short) : sizeof(unsigned int));
	}

	//file.Read(entity, entityNum*sizeof(Entity));//read it all at once
	file.Close();

	//read tileset data
	if (!tileSet.load(mapInfo.tileset))
		return 4;

	//read background
	sprintf(temp, "%s/bk%s.png", exe.mod.mModPath, mapInfo.background);
	if (!background.Load(temp))
		return 5;

	sprintf(temp, "%s/Npc/Npc%s.png", exe.mod.mModPath, mapInfo.NPCset1);
	if (!NPCset1.Load(temp))
		return 6;

	sprintf(temp, "%s/Npc/Npc%s.png", exe.mod.mModPath, mapInfo.NPCset2);
	if (!NPCset2.Load(temp))
		return 7;

	if (zones.size() == 0)
	{
		std::vector<CaveMapZone> zones;
		int highest = 0;
		buildNetGroups(zones, highest);
		for (int i = 0; i < highest; i++)
		{
			if (i == 0)
				strcpy(temp, "Main");
			else
				sprintf(temp, "Zone # %d", i);

			addMapZone(temp);
		}
	}

	if (zones.size() == 0)
		addMapZone("Main");

	return 0;
}

//creates a new map
bool CaveMap::create(CaveMapINFO* info, int w, int h)
{
	int i;
	oldFileName = info->fileName;
	char temp[1024];

	clean(entity);
	clean(tiles);
	clean(netgroup_tiles);

	properties.clear();
	zones.clear();

	entity = NULL;
	tiles = NULL;
	netgroup_tiles = NULL;

	mapInfo = *info;

	width = w;
	height = h;
	tiles = new unsigned char[width*height];
	netgroup_tiles = new unsigned char[width*height];

	//clear new map's tile data
	for (i = 0; i < width * height; i++)
	{
		tiles[i] = 0;
		netgroup_tiles[i] = 0;
	}

	addMapZone("Everything");
	
	entityNum = 1;//we'll start with one dummy entity @ 0,0
	entity = new Entity[entityNum];
	entity[0].x = 0;
	entity[0].y = 0;
	entity[0].event = 0;
	entity[0].flagID = 0;
	entity[0].flags = 0;
	entity[0].npc = 0;

	return save();
}

//saves map
bool CaveMap::save(bool compile_map)
{
	return save(mapInfo.fileName, compile_map);
}

//saves map with new name
bool CaveMap::save(const char* map, bool compile)
{
	char temp[1024];
	char temp2[1024];

	//Start with tile data
	sprintf(temp, "%s/Stage/%s.pxm", exe.mod.mModPath, map);
	CFile file;

	CFileStatus status;
	CFileFind findfile;
	if (CS_DEFUALT_READ_ONLY && findfile.FindFile(map))
	{
		CFile::GetStatus(temp, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(temp, status);
	}

	if (!file.Open(temp, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		return false;

	strcpy(temp, "PXM");
	temp[3] = 0x10;
	file.Write(temp, 4);//add header data

	file.Write(&width, 2);
	file.Write(&height, 2);

	file.Write(tiles, width * height);
	file.Close();//finish writing tile data

	if (1)
	{
		SharedBitStream stream;
		sprintf(temp, "%s/Stage/%s.pxpr", exe.mod.mModPath, map);
		// Magic
		stream.Write("PXPR", 4 << 3);

		// Version
		stream.WriteInt(1, sizeof(unsigned short) << 3);

		// Write basic info
		stream.WriteInt(properties.size(), sizeof(unsigned int) << 3);

		for (int i = 0; i < properties.size(); i++)
		{
			stream.WriteInt(properties[i].ent_idx, sizeof(unsigned int) << 3);
			stream.WriteString(properties[i].name, 255);
			stream.WriteString(properties[i].value, 255);
		}

		stream.SaveToFile(temp);
	}

	sprintf(temp, "%s/Stage/%s.pxn", exe.mod.mModPath, map);
	if (!file.Open(temp, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		return false;

	char Header[] = { 'P', 'X', 'N' };

	// Write header
	file.Write(Header, sizeof(Header));

	// VERSION
	char PXN_Version = 1;
	file.Write(&PXN_Version, sizeof(char));

	// Write general information
	int ZoneCount = zones.size();
	file.Write(&ZoneCount, sizeof(int));

	// Write zone information
	for (std::vector<CaveMapZoneDev>::iterator it = zones.begin(); it != zones.end(); it++)
	{
		CaveMapZoneDev* pZone = &(*it);

		file.Write(&pZone->id, sizeof(int));

		unsigned char Length = strlen(pZone->name);
		file.Write(&Length, sizeof(unsigned char));
		file.Write(pZone->name, Length);
	}

	file.Write(netgroup_tiles, width * height);
	file.Close();

	//begin reading entity data
	sprintf(temp, "%s/Stage/%s.pxe", exe.mod.mModPath, map);

	if (CS_DEFUALT_READ_ONLY && findfile.FindFile(map))
	{
		CFile::GetStatus(temp, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(temp, status);
	}

	if (!file.Open(temp, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone))
		return false;

	strcpy(temp, "PXE2");
	temp[4] = 0x00;
	file.Write(temp, 5);//add header data

	file.Write(&entityNum, 4);

	for (int i = 0; i < entityNum; i++)
	{
		Entity* eve = &entity[i];

		file.Write(&eve->x,			sizeof(unsigned short));
		file.Write(&eve->y,			sizeof(unsigned short));
		file.Write(&eve->flagID,	sizeof(unsigned short));
		file.Write(&eve->event,		sizeof(unsigned short));
		file.Write(&eve->npc,		sizeof(unsigned short));
		file.Write(&eve->flags,		sizeof(unsigned int));
	}

	file.Close();

	if (oldFileName != map)//new file name is different from old file name
	{
		sprintf(temp, "%s/Stage/%s.pxm", exe.mod.mModPath, (const char*)oldFileName);
		file.Remove(temp);
		sprintf(temp, "%s/Stage/%s.pxe", exe.mod.mModPath, (const char*)oldFileName);
		file.Remove(temp);
		sprintf(temp, "%s/Stage/%s.tsc", exe.mod.mModPath, (const char*)oldFileName);
		sprintf(temp2, "%s/Stage/%s.tsc", exe.mod.mModPath, map);
		file.Rename(temp, temp2);
	}
	oldFileName = map;//this is the new "old" name

	// Attempt to compile this map
	if (compile)
	{
		// Make sure it's in our map list, first..
		for (int i = 0; i < exe.mod.mStages.count; i++)
		{
			if (strcmp(exe.mod.mStages.list[i].file, map))
				continue;

			exe.CompileMaps(i, 1);
			break;
		}
	}

	return true;
}

#define CONVERT_TILE_INDEX(TIDX) \
	(TIDX) - (((TIDX) / 16) * (16 - (tileSet.tileData.GetWidth() >> 4)))
#define CONVERT_TILE_INDEX_TO_IDIOT(TIDX) \
	(TIDX) + (((TIDX) / (tileSet.tileData.GetWidth() >> 4)) * (16 - (tileSet.tileData.GetWidth() >> 4)))

//set tile to newTile and returns old tile value
char CaveMap::setTile(int x, int y, char newTile)
{
	char temp = tiles[(x+y*width)%(width*height)];
	tiles[(x+y*width)%(width*height)] = CONVERT_TILE_INDEX_TO_IDIOT(newTile);
	return temp;
}

//returns tile value
char CaveMap::getTile(int x, int y)
{
	return tiles[(x+y*width)%(width*height)];
}

void CaveMap::setNetGroupTile(int x, int y, char type)
{
	netgroup_tiles[(x+y*width)%(width*height)] = type;
}

char CaveMap::getNetGroupTile(int x, int y)
{
	return netgroup_tiles[(x + y * width) % (width * height)];
}

int CaveMap::addMapZone(const char* name)
{
	int NewId = -1;
	for (std::vector<CaveMapZoneDev>::iterator it = zones.begin(); it != zones.end(); it++)
	{
		CaveMapZoneDev* pZone = &(*it);
		if (!stricmp(pZone->name, name))
			return pZone->id;

		if (pZone->id > NewId)
			NewId = pZone->id;
	}

	// Add a new zone
	zones.resize(zones.size() + 1);

	// Configure it
	zones.back().id = NewId + 1;
	if (*name == 0)
		strcpy(zones.back().name, avar("New Zone # %d", zones.back().id));
	else
		strcpy(zones.back().name, name);

	// Done!
	return zones.back().id;
}

bool CaveMap::removeMapZone(const char* name)
{
	unsigned char iIndex = 0;
	for (std::vector<CaveMapZoneDev>::iterator it = zones.begin(); it != zones.end(); it++)
	{
		CaveMapZoneDev* pZone = &(*it);
		if (stricmp(pZone->name, name))
		{
			++iIndex;
			continue;
		}

		// Remove this zone
		for (int i = 0; i < width * height; i++)
		{
			if (netgroup_tiles[i] == iIndex)
				netgroup_tiles[i] = 0;
			else
				netgroup_tiles[i] = CLAMP(netgroup_tiles[i], 0, zones.size() - 2);
		}

		zones.erase(it);

		// Found it
		return true;
	}

	// Couldn't find it
	return false;
}

bool CaveMap::removeMapZone(int zone_id)
{
	unsigned char iIndex = 0;
	for (std::vector<CaveMapZoneDev>::iterator it = zones.begin(); it != zones.end(); it++)
	{
		CaveMapZoneDev* pZone = &(*it);
		if (pZone->id != zone_id)
		{
			++iIndex;
			continue;
		}

		// Remove this zone
		for (int i = 0; i < width * height; i++)
		{
			if (netgroup_tiles[i] == iIndex)
				netgroup_tiles[i] = 0;
			else
				netgroup_tiles[i] = CLAMP(netgroup_tiles[i], 0, zones.size() - 2);
		}

		zones.erase(it);

		// Found it
		return true;
	}

	// Couldn't find it
	return false;
}

const char* CaveMap::getMapZoneName(int zone_id)
{
	for (std::vector<CaveMapZoneDev>::iterator it = zones.begin(); it != zones.end(); it++)
	{
		CaveMapZoneDev* pZone = &(*it);
		if (pZone->id != zone_id)
			continue;

		return pZone->name;
	}

	// Couldn't find it
	return NULL;
}

bool CaveMap::isSolid(int x, int y)
{
	unsigned char a = tileSet.typData[CONVERT_TILE_INDEX(getTile(x, y))];
	return (a == 0x41 || a == 0x47 || (a >= 0x47 && a <= 0x4F) || (a >= 0x58 && a <= 0x5F) || a == 0x61 || a == 0x63 || (a >= 0x65 && a <= 0x6F) || (a >= 0x78 && a <= 0x7F));
}

bool CaveMap::buildNetGroups(std::vector<CaveMapZone>& out, int& highest_group_id)
{
	bool* TmpData = new bool[width * height];
	memset(TmpData, 0, width * height);

	int x      = 0;
	int y      = 0;
	int startX = 0;
	int startY = 0;
	int endX   = 0;
	int endY   = 0;
	int nType  = 0;
	int amount = 0;
	int taken  = 0;
	while (taken < width * height)
	{
		// Find a valid starting position
		for (x = 0; x < width; x++)
		{
			for (y = 0; y < height; y++)
			{
				nType = getNetGroupTile(x, y);

				if (!TmpData[(x + y * width) % (width * height)])
					break;

				if (nType > highest_group_id)
					highest_group_id = nType;

				nType = -1;
			}

			if (nType != -1 && y != height)
				break;
		}

		// Done!
		if (x >= width || y >= height)
			break;

		amount++;

		// Setup state
		startX = x;
		startY = y;
		endX   = x;
		endY   = y;

		// Look for the largest X value we can get
		for (x = startX; x < width; x++)
		{
			char nVal = getNetGroupTile(x, startY);
			if (nVal != nType)
				break;

			endX = x;
		}

		// Look for the largest Y value we can get
		for (y = startY; y < height; y++)
		{
			char nVal = getNetGroupTile(startX, y);
			if (nVal != nType)
				break;

			endY = y;
		}

		// Check for inconsistent netgroup types inside of the rect -- If there are any, shrink this rect
		for (x = startX; x <= endX; x++)
		{
			for (y = startY; y <= endY; y++)
			{
				if (getNetGroupTile(x, endY) != nType)
				{
					endY--;
					x = startX;
					y = startY - 1;

					continue;
				}

				if (getNetGroupTile(endX, y) != nType)
				{
					endX--;
					x = startX;
					y = startY - 1;

					continue;
				}
			}
		}

		// Set the rect we got as 'marked'
		for (x = startX; x <= endX; x++)
		{
			for (y = startY; y <= endY; y++)
			{
				TmpData[(x + y * width) % (width * height)] = 1;
				taken++;
			}
		}

		// Resize to add one
		out.resize(out.size() + 1);

		// Setup last rect
		out.back().id = nType;
		out.back().x  = startX;
		out.back().y  = startY;
		out.back().w  = (endX - startX) + 1;
		out.back().h  = (endY - startY) + 1;

		printf(__FUNCTION__ "() -    RECT! (%04d, %04d)<%04d, %04d> of NetGroup # %d\r\n",
			out.back().x,
			out.back().y,
			out.back().w,
			out.back().h,
			out.back().id
			);
	}

	printf(__FUNCTION__ "() - Created %d NetGroup RECT%s.\r\n", amount, (amount == 1 ? "" : "s"));

	delete[] TmpData;
	return true;
}

//resizes the map to the new width and height, buffering with tile 0 where needed
void CaveMap::resize(int w, int h)
{
	int i, j;
	unsigned char* newTiles;
	unsigned char* newTiles_NG;
	newTiles    = new unsigned char[w*h];
	newTiles_NG = new unsigned char[w*h];

	for (i = 0; i < w; i++)
	{
		for (j = 0; j < h; j++)
		{
			if (i >= width || j >= height)//if outside current bounds
				newTiles[i + j*w] = 0;
			else
				newTiles[i + j*w] = tiles[i + j*width];

			if (i >= width || j >= height)
				newTiles_NG[i + j*w] = 0;
			else
				newTiles_NG[i + j*w] = netgroup_tiles[i + j*width];
		}
	}

	if (tiles != NULL)
		delete[] tiles;
	if (netgroup_tiles != NULL)
		delete[] netgroup_tiles;

	netgroup_tiles = newTiles_NG;
	tiles          = newTiles;

	width = w;
	height = h;
}

//shifts map by dx and dy, filling edges with tile 0 if wrap is turned off
void CaveMap::shift(int dx, int dy, bool wrap)
{
	//I'm being lazy here, using memory to make all the math go away
	int i, j;
	unsigned char* newTiles;
	unsigned char* newTiles_NG;
	newTiles    = new unsigned char[width*height];
	newTiles_NG = new unsigned char[width*height];

	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			if (wrap)//well some math anyways...
				newTiles[(i+dx+width*100)%width + ((j+dy+height*100)%height)*width] = tiles[i + j*width];
			else
			if (i+dx < 0 || i+dx >= width || j+dy < 0 || j+dy >= height) 
				newTiles[(i+dx+width*100)%width + ((j+dy+height*100)%height)*width] = 0;
			else
				newTiles[(i+dx+width*100)%width + ((j+dy+height*100)%height)*width] = tiles[i + j*width];

			if (wrap)
				newTiles_NG[(i + dx + width * 100) % width + ((j + dy + height * 100) % height) * width] = netgroup_tiles[i + j * width];
			else
				if (i + dx < 0 || i + dx >= width || j + dy < 0 || j + dy >= height)
					newTiles_NG[(i + dx + width * 100) % width + ((j + dy + height * 100) % height) * width] = 0;
				else
					newTiles_NG[(i + dx + width * 100) % width + ((j + dy + height * 100) % height) * width] = netgroup_tiles[i + j * width];
		}
	}
	for (i = 0; i < entityNum; i++)
	{
		if (wrap)
		{
			entity[i].x = (entity[i].x+dx+width*100)%width;
			entity[i].y = (entity[i].y+dy+height*100)%height;
		}
		else
		{
			if (entity[i].x+dx < 0)
				entity[i].x = 0;
			else
			if (entity[i].x+dx >= width)
				entity[i].x = width-1;
			else
				entity[i].x += dx;

			if (entity[i].y+dy < 0)
				entity[i].y = 0;
			else
			if (entity[i].y+dy >= height)
				entity[i].y = height-1;
			else
				entity[i].y += dy;
		}
	}
	delete[] tiles;
	delete[] netgroup_tiles;
	netgroup_tiles = newTiles_NG;
	tiles = newTiles;
}

//all addEntity functions return index of added entity or -1 on failure
int CaveMap::addEntity()
{
	Entity* newEntity = new Entity[entityNum+1];

	for (int i = 0; i < entityNum; i++)
	{
		newEntity[i] = entity[i];
	}
	newEntity[entityNum].x = 0;
	newEntity[entityNum].y = 0;
	newEntity[entityNum].event = 0;
	newEntity[entityNum].flagID = 0;
	newEntity[entityNum].flags = 0;
	newEntity[entityNum].npc = 0;

	delete[] entity;
	entity = newEntity;

	entityNum++;
	return entityNum-1;
}

//all addEntity functions return index of added entity or -1 on failure
int CaveMap::addEntity(Entity e)
{
	Entity* newEntity = new Entity[entityNum+1];

	for (int i = 0; i < entityNum; i++)
	{
		newEntity[i] = entity[i];
	}
	newEntity[entityNum] = e;

	delete[] entity;
	entity = newEntity;

	entityNum++;
	return entityNum-1;
}

//all addEntity functions return index of added entity or -1 on failure
int CaveMap::addEntity(Entity e, int index)
{
	Entity* newEntity = new Entity[entityNum+1];
	if (index > entityNum)
		index = entityNum;
	if (index < 0)
		index = 0;

	for (int i = 0; i < entityNum; i++)
	{
		if (i < index)
			newEntity[i] = entity[i];
		else
			newEntity[i+1] = entity[i];
	}
	newEntity[index] = e;

	delete[] entity;
	entity = newEntity;

	entityNum++;
	return entityNum-1;
}

//all addEntity functions return index of added entity or -1 on failure
int CaveMap::addEntity(WORD x, WORD y, WORD npcType, WORD event, WORD flagID, WORD flags)
{
	if (x >= width || y >= height)
		return -1;

	Entity* newEntity = new Entity[entityNum+1];

	for (int i = 0; i < entityNum; i++)
	{
		newEntity[i] = entity[i];
	}
	newEntity[entityNum].x = x;
	newEntity[entityNum].y = y;
	newEntity[entityNum].event = event;
	newEntity[entityNum].flagID = flagID;
	newEntity[entityNum].flags = flags;
	newEntity[entityNum].npc = npcType;

	delete[] entity;
	entity = newEntity;

	entityNum++;
	return entityNum-1;
}

//all copyEntity functions return index of added entity or -1 on failure
int CaveMap::copyEntity(int index)
{
	if (index < 0 || index >= entityNum)
		return -1;

	Entity* newEntity = new Entity[entityNum+1];

	for (int i = 0; i < entityNum; i++)
	{
		newEntity[i] = entity[i];
	}
	newEntity[entityNum] = entity[index];

	delete[] entity;
	entity = newEntity;

	entityNum++;
	return entityNum-1;
}

//all copyEntity functions return index of added entity or -1 on failure
int CaveMap::copyEntity(int index, int x, int y)
{
	if (index < 0 || index >= entityNum || x >= width || y >= height)
		return -1;

	Entity* newEntity = new Entity[entityNum+1];

	for (int i = 0; i < entityNum; i++)
	{
		newEntity[i] = entity[i];
	}
	newEntity[entityNum] = entity[index];
	newEntity[entityNum].x = x;
	newEntity[entityNum].y = y;

	delete[] entity;
	entity = newEntity;

	entityNum++;
	return entityNum-1;
}

//returns number of entities after delete
int CaveMap::deleteEntity(int index)
{
	if (index < 0 || index >= entityNum)
		return -1;

	Entity* newEntity = new Entity[entityNum-1];

	for (int i = 0; i < properties.size(); i++)
	{
		if (properties[i].ent_idx == index)
		{
			properties.erase(properties.begin() + i);
			--i;
			continue;
		}
		else if (properties[i].ent_idx > index)
			--properties[i].ent_idx;
	}

	int i;
	for (i = 0; i < index; i++)
		newEntity[i] = entity[i];

	for (i = index+1; i < entityNum; i++)
		newEntity[i-1] = entity[i];

	delete[] entity;
	entity = newEntity;

	entityNum--;
	return entityNum;
}

//displays part of the map to device context, includes transparency
void CaveMap::displayMap(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY)
{
	endX++;
	endY++;
	//don't try to print outside map bounds (for loop takes care of begin > end)
	if (beginX < 0)
		beginX = 0;
	if (beginY < 0)
		beginY = 0;
	if (endX >= width)
		endX = width;
	if (endY >= height)
		endY = height;

	int i, j;
	for (i = beginX; i < endX; i++)
	{
		for (j = beginY; j < endY; j++)
		{
			tileSet.displayTile(CONVERT_TILE_INDEX(tiles[i+j*width]), (i-beginX)*16+x, (j-beginY)*16+y, cdc, true);
		}
	}
}

void CaveMap::displayTile(CDC* cdc, int x, int y, int tx, int ty, double scale)
{
	int index = CONVERT_TILE_INDEX(tiles[tx+ty*width]);

	int tileWide = tileSet.tileData.GetWidth() >> 4;// get how many tiles wide the pbm data is
	double tileHigh = tileSet.tileData.GetHeight() / 16.0; //for some reason a tileset pbm might not be a multiple of 16 (like PrtBarr.pbm)
	int _x, _y;

	_x = index%tileWide;
	_y = index/tileWide;

	tileSet.tileData.StretchBlt(cdc->m_hDC, x, y, 16*scale, 16*scale, _x*16, _y*16, 16, 16);
};

//displays map to device context, includes transparency
void CaveMap::displayMap(CDC* cdc, int x, int y)
{
	int i, j;
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			tileSet.displayTile(CONVERT_TILE_INDEX(tiles[i + j * width]), i*16+x, j*16+y, cdc, true);
		}
	}
}

//displays part of background map tiles to device context
void CaveMap::displayBackMap(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY)
{
	endX++;
	endY++;
	//don't try to print outside map bounds (for loop takes care of begin > end)
	if (beginX < 0)
		beginX = 0;
	if (beginY < 0)
		beginY = 0;
	if (endX >= width)
		endX = width;
	if (endY >= height)
		endY = height;

	int i, j;
	for (i = beginX; i < endX; i++)
	{
		for (j = beginY; j < endY; j++)
		{
			//if not a "front" tile and graphic are "seen" then display
			if (!tileKey.front[tileSet.typData[CONVERT_TILE_INDEX(tiles[i+j*width])]] && tileKey.seen[tileSet.typData[CONVERT_TILE_INDEX(tiles[i+j*width])]])
				tileSet.displayTile(CONVERT_TILE_INDEX(tiles[i + j * width]), (i-beginX)*16+x, (j-beginY)*16+y, cdc, true);
		}
	}
}

//displays background map tiles to device context
void CaveMap::displayBackMap(CDC* cdc)
{
	int i, j;
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			//if not a "front" tile and graphic are "seen" then display
			if (!tileKey.front[tileSet.typData[CONVERT_TILE_INDEX(tiles[i + j * width])]] && tileKey.seen[tileSet.typData[CONVERT_TILE_INDEX(tiles[i + j * width])]])
				tileSet.displayTile(CONVERT_TILE_INDEX(tiles[i + j * width]), i*16, j*16, cdc, true);
		}
	}
}

//displays part of background map tiles to device context
void CaveMap::displayFrontMap(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY)
{
	endX++;
	endY++;
	//don't try to print outside map bounds (for loop takes care of begin > end)
	if (beginX < 0)
		beginX = 0;
	if (beginY < 0)
		beginY = 0;
	if (endX >= width)
		endX = width;
	if (endY >= height)
		endY = height;

	int i, j;
	for (i = beginX; i < endX; i++)
	{
		for (j = beginY; j < endY; j++)
		{
			//if not a "front" tile and graphic are "seen" then display
			if (tileKey.front[tileSet.typData[CONVERT_TILE_INDEX(tiles[i + j * width])]] && tileKey.seen[tileSet.typData[CONVERT_TILE_INDEX(tiles[i + j * width])]])
				tileSet.displayTile(CONVERT_TILE_INDEX(tiles[i + j * width]), (i-beginX)*16+x, (j-beginY)*16+y, cdc, true);
		}
	}
}

//displays foreground map tiles to device context
void CaveMap::displayFrontMap(CDC* cdc)
{
	int i, j;
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			//if a "front" tile and graphic are "seen" then display
			if (tileKey.front[tileSet.typData[CONVERT_TILE_INDEX(tiles[i + j * width])]] && tileKey.seen[tileSet.typData[CONVERT_TILE_INDEX(tiles[i + j * width])]])
				tileSet.displayTile(CONVERT_TILE_INDEX(tiles[i + j * width]), i*16, j*16, cdc, true);
		}
	}
}


//displays part of background map tiles to device context
void CaveMap::displayTileTypes(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY)
{
	endX++;
	endY++;
	//don't try to print outside map bounds (for loop takes care of begin > end)
	if (beginX < 0)
		beginX = 0;
	if (beginY < 0)
		beginY = 0;
	if (endX >= width)
		endX = width;
	if (endY >= height)
		endY = height;

	int i, j;
	for (i = beginX; i < endX; i++)
		for (j = beginY; j < endY; j++)
			tileSet.displayTileType(CONVERT_TILE_INDEX(tiles[i+j*width]), (i-beginX)*16+x, (j-beginY)*16+y, cdc, true);
}

//displays foreground map tiles to device context
void CaveMap::displayTileTypes(CDC* cdc)
{
	int i, j;
	for (i = 0; i < width; i++)
		for (j = 0; j < height; j++)
			tileSet.displayTileType(CONVERT_TILE_INDEX(tiles[i+j*width]), i * 16, j * 16, cdc, true);
}

void CaveMap::displayNetGroupTiles(CDC* cdc)
{
	int x, y, _x;
	int tileWide = 128;

	CPen PEN;
	for (x = 0; x < width; x++)
	{
		for (y = 0; y < height; y++)
		{
			_x = netgroup_tiles[x + y * width] % tileWide;

			// Draw the background
			CaveTileSet::netGrid.DrawPart(_x * 16, 0, 16, 16, cdc, x * 16, y * 16, true, 0);

			// Draw the number
			int ID = netgroup_tiles[x + y * width];
			if (ID < 9)
			{
				// One digit
				CaveTileSet::netGrid.DrawPart(ID * 16, 16, 16, 16, cdc, x * 16, y * 16, true, 0);
			}
			else
			{
				// Two digits (hopefully)
				CaveTileSet::netGrid.DrawPart(144 + (8 * ((ID / 10) % 10)), 16, 8, 8, cdc, (x * 16) + 1, (y * 16) + 4, true, 0);
				CaveTileSet::netGrid.DrawPart(144 + (8 * (ID % 10)), 16, 8, 8, cdc, (x * 16) + 7, (y * 16) + 4, true, 0);
			}
		}
	}
}

void CaveMap::displayNetGroupTiles(CDC* cdc, int x, int y, int beginX, int beginY, int endX, int endY)
{
	int tileWide = 128;
	endX++;
	endY++;
	//don't try to print outside map bounds (for loop takes care of begin > end)
	if (beginX < 0)
		beginX = 0;
	if (beginY < 0)
		beginY = 0;
	if (endX >= width)
		endX = width;
	if (endY >= height)
		endY = height;

	int i, j, _x;
	for (i = beginX; i < endX; i++)
	{
		for (j = beginY; j < endY; j++)
		{
			_x = netgroup_tiles[i + j * width] % tileWide;

			CaveTileSet::netGrid.DrawPart(_x * 16, 0, 16, 16, cdc, x, y, true, 0);

			// Draw the number
			int ID = netgroup_tiles[i + j * width];
			if (ID < 9)
			{
				// One digit
				CaveTileSet::netGrid.DrawPart(ID * 16, 16, 16, 16, cdc, x, y, true, 0);
			}
			else
			{
				// Two digits (hopefully)
				CaveTileSet::netGrid.DrawPart(144 + (8 * ((ID / 10) % 10)), 16, 8, 8, cdc, x + 1, y + 4, true, 0);
				CaveTileSet::netGrid.DrawPart(144 + (8 * (ID % 10)), 16, 8, 8, cdc, x + 7, y + 4, true, 0);
			}
		}
	}
}

//displays background image to device context
void CaveMap::displayBackground(CDC* cdc)
{
	int i, j, w, h;
	//Note we're not starting at 0,0 because the background begins centered on the first tile (8,8)
	for (i = 8-background.GetWidth(); i < width*16; i += background.GetWidth())
		for (j = 8-background.GetHeight(); j < height*16; j += background.GetHeight())
		{
			w = min(background.GetWidth(), width*16-i);
			h = min(background.GetHeight(), height*16-j);
			background.Draw(*cdc, i, j, w, h, 0, 0, w, h);
		}
}

TSC_Info* TSC_Info::first = NULL;

TSC_Info::TSC_Info()
{
	if (first)
		first->prev = this;
	prev  = NULL;
	next  = first;
	first = this;
	ifstream file;

	char buffer[256];
	char typeBuffer[256];
	::GetCurrentDirectory(255, buffer);
	::SetCurrentDirectory(progPath.GetString());
	file.open(CS_TSC_INFO_FILE);
	::SetCurrentDirectory(buffer);

	memset(TSC_argTypeLists, 0, sizeof(TSC_argTypeLists));

	LoadAllCommands();
	
	::GetCurrentDirectory(255, buffer);
	::SetCurrentDirectory(progPath.GetString());
	file.open(CS_TSC_ARGT_FILE);
	::SetCurrentDirectory(buffer);

	TSC_argType argType = TSC_argType::tat_none;
	int argCount = 0;
	while (file)
	{
		if (argCount == 0)
		{
			char argNameBuffer[128];
			file.getline(argNameBuffer, 128, ' ');
			file >> argCount;
			file.get();

			argType = TSC_argType::tat_none;
			for (int i = 0; i < sizeof(TSC_argTypeNames) / sizeof(const char*); i++)
			{
				if (stricmp(TSC_argTypeNames[i], argNameBuffer))
					continue;

				argType = (TSC_argType)i;
				TSC_argTypeLists[argType].count = -1;
				break;
			}

			continue;
		}

		char _path[1024];
		char _argBuffer[1024];
		file.getline(_argBuffer, 1024, '\t');

		if (argType == TSC_argType::tat_music)
		{
			file.getline(_argBuffer, 1024, '\t');
			file.getline(_path, 1024, '\n');
		}
		else
		{
			file.getline(_argBuffer, 1024, '\n');
		}
		file.get();

		argCount--;
		if (argType == TSC_argType::tat_none)
			continue;

		TSC_argTypeLists[argType].count++;
		strcpy(TSC_argTypeLists[argType].list[TSC_argTypeLists[argType].count], _argBuffer);
	}

	file.close();
}

TSC_Info::~TSC_Info()
{
	if (first == this)
		first = next;

	if (next)
		next->prev = prev;

	if (prev)
		prev->next = next;
}

void TSC_Info::LoadAllCommands()
{
	ifstream file;

	char buffer[260];
	char typeBuffer[256];
	::GetCurrentDirectory(259, buffer);
	::SetCurrentDirectory(progPath.GetString());
	file.open(CS_TSC_INFO_FILE);
	::SetCurrentDirectory(buffer);

	count = -1;
	while (file)
	{
		count++;
		file.getline(cmd[count].command, 5, '\t');
		file >> cmd[count].command_type;
		file.get();
		file >> cmd[count].inspectDlgIdx;
		file.get();
		file >> cmd[count].args;
		file.get();
		file.getline(typeBuffer, 256, '\t');
		file.getline(cmd[count].name, 32, '\t');
		file.getline(cmd[count].desc, 256, '\n');

		for (char* ptr = typeBuffer; *ptr != 0; ptr++)
		{
			TSC_argType type = TSC_argType::tat_none;

			for (const char* _ptr = TSC_argTypeIdents; *_ptr != 0; _ptr++)
			{
				if (*ptr != *_ptr)
					continue;

				type = (TSC_argType)(_ptr - TSC_argTypeIdents);
				break;
			}

			cmd[count].types[(ptr - typeBuffer)] = type;
		}
	}

	file.close();

	sprintf(buffer, "%s/CE_Commands.txt", exe.mod.mModPath);
	file.open(buffer);
	while (file)
	{
		count++;
		file.getline(cmd[count].command, 5, '\t');
		file >> cmd[count].command_type;
		file.get();
		file >> cmd[count].inspectDlgIdx;
		file.get();
		file >> cmd[count].args;
		file.get();
		file.getline(typeBuffer, 256, '\t');
		file.getline(cmd[count].name, 32, '\t');
		file.getline(cmd[count].desc, 256, '\n');

		for (char* ptr = typeBuffer; *ptr != 0; ptr++)
		{
			TSC_argType type = TSC_argType::tat_none;

			for (const char* _ptr = TSC_argTypeIdents; *_ptr != 0; _ptr++)
			{
				if (*ptr != *_ptr)
					continue;

				type = (TSC_argType)(_ptr - TSC_argTypeIdents);
				break;
			}

			cmd[count].types[(ptr - typeBuffer)] = type;
		}
	}
	file.close();
}

//returns offset up to count-1 of matching command
int TSC_Info::Find(const char* command)
{
	for (int i = 0; i < count; i++)
		if (strcmp(command, cmd[i].command) == 0)
			return i;

	return -1;
}

//returns offset up to count-1 of matching command
int TSC_Info::Find(CString command)
{
	for (int i = 0; i < count; i++)
		if (command == cmd[i].command)
			return i;

	return -1;
}

CaveStoryMOD::CaveStoryMOD()
{
#ifdef _CONSOLE
	if (AllocConsole())
	{
		FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;

		freopen_s(&fpstdin, "CONIN$", "r", stdin);
		freopen_s(&fpstdout, "CONOUT$", "w", stdout);
		freopen_s(&fpstderr, "CONOUT$", "w", stderr);

		printf("CaveEditor\r\n");
	}
#endif
}

CaveStoryMOD::~CaveStoryMOD()
{
#ifdef _CONSOLE
	FreeConsole();
#endif
}

extern bool isSoundSystemEnabled;

#define LOAD_GENERIC_SURFACE_FILE(SURF_ID, FILE_NAME)\
	sprintf(buffer, "%s/" FILE_NAME ".png", mod.mModPath);\
	if (!spriteSheet[SURF_ID].Load(buffer)) \
	{\
		strcpy(buffer, mod.mModPath);\
		if (strrchr(buffer, '/'))\
			*strrchr(buffer, '/') = 0;\
		strcat(buffer, "/data/");\
		strcat(buffer, FILE_NAME);\
		strcat(buffer, ".png");\
		if (!spriteSheet[SURF_ID].Load(buffer))\
			printf("Failed to load default surface for \"" #SURF_ID "\"");\
	} else\
		printf("Loaded custom surface \"" #SURF_ID "\"\r\n");

//this function reads the mod
bool CaveStoryMOD::Open(const char* file)
{
	if (!mod.Load(file))
		return false;

	// Load surfaces
	char buffer[260];

	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_PIXEL,				"Resource/BITMAP/pixel");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_TITLE,				"Title");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_ARMS_IMAGE,		"ArmsImage");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_ARMS,				"Arms");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_ITEM_IMAGE,		"ItemImage");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_STAGE_ITEM,		"StageImage");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_MY_CHAR,			"MyChar");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_NPC_SYM,			"Npc/NpcSym");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_NPC_REGU,			"Npc/NpcRegu");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_TEXT_BOX,			"TextBox");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_CARET,				"Caret");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_BULLET,			"Bullet");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_FACE,				"Face");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_FACE_COLOR_MASK,	"FaceMask");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_FADE,				"Fade");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_CREDITS_IMAGE,		"Resource/BITMAP/Credit01");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_GUI,				"GUI");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_CHAT_TOPICS,		"ChatTopics");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_CONTROLS,			"Controls");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_DEMO_EDITOR,		"DemoEditor");
	LOAD_GENERIC_SURFACE_FILE(SURFACE_ID_HELP_IMAGES,		"HelpImages");

	// Determine the wave table's path
	char pWaveTable[260];
	strcpy_s(pWaveTable, exe.mod.mModPath);
	if (strrchr(pWaveTable, '/'))
		* strrchr(pWaveTable, '/') = 0;
	strcat_s(pWaveTable, "/data");

	if (!isSoundSystemEnabled)
		InitSoundSubsystem(pWaveTable);

	for (TSC_Info* walk = TSC_Info::first; walk; walk = walk->next)
		walk->LoadAllCommands();

	return true;
}

//clears file from memory (please save first)
void CaveStoryMOD::Clear()
{
	mod.Reset();
}

bool CaveStoryMOD::Save(bool bRelease)
{
	return mod.Save(bRelease);
}

bool CaveStoryMOD::AddMap(const char* name, int width, int height, int index)
{
	CaveMapINFO info;

	strcpy(info.fileName, name);
	strcpy(info.background, "bk0");
	strcpy(info.NPCset1, "0");
	strcpy(info.NPCset2, "0");
	strcpy(info.caption, name);
	info.backType = 0;
	info.bossNum = 0;

	return AddMap(&info, width, height, index);
}

bool CaveStoryMOD::AddMap(CaveMapINFO* info, int width, int height, int index)
{
	char temp[1024];
	CaveMap testing;
	testing.create(info, width, height);
	testing.save(false);

	//add tsc file
	sprintf(temp, "%s/Stage/%s.tsc", exe.mod.mModPath, info->fileName);
	CFile file;
	file.Open(temp, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);
	file.Write("\n\r\r\n", 4);
	file.Close();
	
	return mod.AddStage(info->fileName, info->caption, info->tileset, info->NPCset1, info->NPCset2, info->background, (PXMOD_BK_TYPE)info->backType, (PXMOD_BOSS_TYPE)info->bossNum) != -1;
}

bool CaveStoryMOD::CopyMap(int index, const char* name)
{
	if (index < 0 || index >= mod.mStages.count)
		return false;
	if (strlen(name) >= 32 || name[0] == '\0')//name must fit in structure
		return false;

	int i;
	for (i = 0; i < mod.mStages.count; i++)
		if (strcmp(mod.mStages.list[i].file, name) == 0)
			return false;//no duplicate names

	return mod.AddStage(name,
		mod.mStages.list[index].name,
		mod.mTilesets.list[mod.mStages.list[index].tileset].name,
		mod.mSpritesheets.list[mod.mStages.list[index].npc_sheet].name,
		mod.mSpritesheets.list[mod.mStages.list[index].boss_sheet].name,
		mod.mBackgrounds.list[mod.mStages.list[index].background].name,
		mod.mStages.list[i].bkType,
		mod.mStages.list[i].boss_no) != -1;
}

bool CaveStoryMOD::DeleteMap(int index)
{
	if (index < 0 || index >= mod.mStages.count)
		return false;

	return mod.RemoveStage(mod.mStages.list[index].file);
}

// From https://docs.microsoft.com/en-us/windows/win32/controls/create-a-tooltip-for-a-control, but modified a bit
BOOL CreateToolTipForControl(CWnd* pDlg, const char* pszText)
{
	if (!pDlg || !pszText)
		return FALSE;
	
	// Get the window of the tool.
	HWND hDlg     = (pDlg->GetParent() ? pDlg->GetParent()->GetSafeHwnd() : NULL);
	HWND hwndTool = pDlg->GetSafeHwnd();
    
	// Create the tooltip. g_hInst is the global instance handle.
	HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP |TTS_ALWAYSTIP | TTS_BALLOON, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hDlg, NULL, GetModuleHandle(NULL), NULL);
    
	if (!hwndTool || !hwndTip)
		return FALSE;

	// Associate the tooltip with the tool.
	TOOLINFO toolInfo = { 0 };
	toolInfo.cbSize   = sizeof(toolInfo);
	toolInfo.hwnd     = hDlg;
	toolInfo.uFlags   = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId      = (UINT_PTR)hwndTool;
	toolInfo.lpszText = (LPSTR)pszText;
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

	return TRUE;
}

static unsigned char GetDigitFromNumber(int iNumber, char iDigit)
{
	return unsigned char((iNumber / int(powf(10, CLAMP(3 - iDigit, 0, 4)))) % 10);
}

void GENERAL_EventTextEdit_LoseFocus(CWnd* pControl)
{
	char pBuffer[64];

	// Get the actual value
	pControl->GetWindowTextA(pBuffer, sizeof(pBuffer));

	// Get the value as an int
	short iValue = CLAMP(atoi(pBuffer), 0, 9999);

	printf("\"%s\" = \"%d\"\r\n", pBuffer, iValue);

	// Build the new buffer
	sprintf_s(pBuffer, sizeof(pBuffer), "# %04d", iValue);

	if (!iValue)
		strcpy(pBuffer, "# 0000");

	printf("  > Got \"%s\"\r\n", pBuffer);

	// Set the text
	pControl->SetWindowTextA(pBuffer);
}

void GENERAL_EventTextEdit_GainFocus(CWnd* pControl)
{
	short iValue = GENERAL_EventTextEdit_GetValue(pControl);

	// Set new text
	pControl->SetWindowTextA(avar("%d", iValue));
}

short GENERAL_EventTextEdit_GetValue(CWnd* pControl)
{
	char pBuffer[64];

	// Get the text from the control
	pControl->GetWindowTextA(pBuffer, sizeof(pBuffer));
	int iValue = 0;

	if (*pBuffer == '#' && pBuffer[1] == ' ')
		iValue = atoi(pBuffer + 2);
	else if (strlen(pBuffer) != 4)
		iValue = atoi(pBuffer);
	else if (pBuffer[0] == '0' && pBuffer[1] == '0' && pBuffer[2] == '0' && pBuffer[3] == '0')
		iValue = 0;
	else if (pBuffer[0] == '0' && pBuffer[1] == '0' && pBuffer[2] == '0')
		iValue = atoi(pBuffer + 3);
	else if (pBuffer[0] == '0' && pBuffer[1] == '0')
		iValue = atoi(pBuffer + 2);
	else if (pBuffer[0] == '0')
		iValue = atoi(pBuffer + 1);
	else
		iValue = atoi(pBuffer);

	printf("Got Value \"%d\"\r\n", iValue);

	return iValue;
}