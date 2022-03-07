#if defined(_CHAR_EDITOR) || defined(_CAVEEDITOR)
#include "framework.h"
#include "afxdialogex.h"
#endif

#include "CustomPlayer.h"
#include "SharedBitStream.h"

#ifdef _CAVESTORY_MULTIPLAYER
#include <stdarg.h>
#include "MessageBoxMode.h"
#endif

#include <memory>

#ifndef CLAMP
#define CLAMP(x, min, max)					((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#endif

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
     [ VER  ]        -> "The version the field was added in."
     [ BITS ]        -> "The size of the field, in bits. If this displays 'DEPEND', then it depends on another field (unless otherwise stated)."
	 [ TYPE ]        -> "The field type. If this displays 'RANGED', then the field is a ranged field."
	 [ DESCRIPTION ] -> "The field description."

	 FILE FORMAT FOR .PXCHAR:
	 * +-----+--------+--------+------------------------------------------------------------------------------------------------------------------- -  -   -     -
	 * | VER |  BITS  |  TYPE  |  DESCRIPTION
	 * +-----+--------+--------+------------------------------------------------------------------------------------------------------------------- -  -   -     -
	 * |  1  |   48   | STRING | File header ("PXCHAR")
	 * |  1  |   8    | UCHAR  | File version
	 * |  1  |   16   | RANGED | Length of "Character Name" string (0 - 256)
	 * |  1  | DEPEND | STRING | Character Name
	 * |  1  |   16   | RANGED | Length of "Author Name" string (0 - 256)
	 * |  1  | DEPEND | STRING | Author Name
	 * |  1  |   8    | UCHAR  | Frame count
	 * |  2  |   8    | UCHAR  | Face count
	 * |  8  |   8    | UCHAR  | Custom frame count
	 * |  8  |   8    | UCHAR  | Custom sound count
	 * |  8  |   8    | UCHAR  | Animation count
	 * |  3  |   32   | UINT   | Fish offset X
	 * |  3  |   32   | UINT   | Fish offset Y
	 * |  4  |   32   | UINT   | Fish sprite offset X
	 * |  4  |   32   | UINT   | Fish sprite offset Y
	 * |     |        | >>==================================== CHUNK: Hit box Data ====================================<<
	 * |  3  |   32   | UINT   | Hit Box LEFT
	 * |  3  |   32   | UINT   | Hit Box TOP
	 * |  3  |   32   | UINT   | Hit Box RIGHT
	 * |  3  |   32   | UINT   | Hit Box BOTTOM
	 * |     |        | >>==================================== CHUNK: Viewbox Data ====================================<<
	 * |  3  |   32   | UINT   | View Box LEFT
	 * |  3  |   32   | UINT   | View Box TOP
	 * |  3  |   32   | UINT   | View Box RIGHT
	 * |  3  |   32   | UINT   | View Box BOTTOM
	 * |     |        | >>================================= CHUNK: _Custom Frame Data =================================<<
	 * |  8  |   16   | RANGED | Length of "Frame Name" string (0 - 256)
	 * |  8  | DEPEND | STRING | Frame Name
	 * |  8  |   32   | UINT   | Frame Flags
	 * |  8  |   32   | UINT   | Rect.Left
	 * |  8  |   32   | UINT   | Rect.Top
	 * |  8  |   32   | UINT   | Rect.Right
	 * |  8  |   32   | UINT   | Rect.Bottom
	 * |  8  |   32   | UINT   | Weapon Offset X
	 * |  8  |   32   | UINT   | Weapon Offset Y
	 * |  11 |   32   | UINT   | Held Entity Offset X
	 * |  11 |   32   | UINT   | Held Entity Offset Y
	 * |  12 |   32   | UINT   | Carried Player Offset X
	 * |  12 |   32   | UINT   | Carried Player Offset Y
	 * |     |        | >>================================= CHUNK: _Custom Sound Data =================================<<
	 * |  8  |   16   | RANGED | Length of "File Name" string (0 - 256)
	 * |  8  | DEPEND | STRING | File Name
	 * |  8  |   8    | UCHAR  | Sound Type
	 * |     |        | >>===================================== CHUNK: Frame Data =====================================<<
	 * |  5  |   32   | UINT   | Frame CRC (VER 8: Store these in an array in the order they are read so we can lookup the index via CHUNK:AnimationFrameData's 'Player Frame Index')
	 * |  1  |   32   | UINT   | Rect.Left
	 * |  1  |   32   | UINT   | Rect.Top
	 * |  1  |   32   | UINT   | Rect.Right
	 * |  1  |   32   | UINT   | Rect.Bottom
	 * |  6  |   32   | UINT   | Weapon Offset X
	 * |  6  |   32   | UINT   | Weapon Offset Y
	 * |  11 |   32   | UINT   | Held Entity Offset X
	 * |  11 |   32   | UINT   | Held Entity Offset Y
	 * |  12 |   32   | UINT   | Carried Player Offset X
	 * |  12 |   32   | UINT   | Carried Player Offset Y
	 * |     |        | >>===================================== CHUNK: _Face Data =====================================<<
	 * |  5  |   32   | UINT   | Face CRC
	 * |  2  |   32   | UINT   | Rect.Left
	 * |  2  |   32   | UINT   | Rect.Top
	 * |     |        | >>=================================== CHUNK: Animation Data ===================================<<
	 * |  8  |   32   | UINT   | Animation CRC
	 * |  8  |   1    | BIT    | Loop
	 * |  9  |   1    | BIT    | Has Subsitute?
	 * | 8/9 |   16   | USHORT | Animation Frame Count (If version 8, then this field is always present. If version 9, then this field is only present if 'Has Subsitute?' is false.)
	 * |  9  |   16   | USHORT | Animation Loop Start Point (Only if 'Has Subsitute?' is false)
	 * |  9  |   32   | UINT   | Subsitute Animation CRC (Only if 'Has Subsitute?' is true)
	 * |     |        | >>================================ CHUNK: Animation Frame Data ================================<<
	 * |  8  |   16   | USHORT | Frame Number
	 * |  8  | DEPEND | RANGED | Player Frame Index (Range is '0' to 'FrameCount + CustomFrameCount')
	 * |  8  |   16   | USHORT | Sound ID
	 * |  8  |   16   | USHORT | Delay (Milliseconds)
	 * |  10 |   16   | USHORT | Sprite Offset X
	 * |  10 |   16   | USHORT | Sprite Offset Y
	 * +-----+--------+------------------------------------------------------------------------------------------------------------------- -  -   -     -
*/
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef _CAVESTORY_MULTIPLAYER
int CSM_LoadPixTone(const char* pFile) { return 0; }
void CSM_UnloadPixTone(int iSoundId) { }
int CSM_LoadCustomSound(const char* pFile) { return 0; }
void CSM_UnloadCustomSound(int iSoundId) { }

#define SET_RECT(rect, l, t, r, b) \
	rect.left = l; \
	rect.top = t; \
	rect.right = r; \
	rect.bottom = b;
#endif

static const char* gPlayerCustomSoundExtList[] =
{
	".pxt",
	".wav"
};

static struct
{
	int(*load_sound)(const char* pSoundFile);
	void(*free_sound)(int iSoundId);
} gPlayerCustomSoundFuncList[] =
{
	// .pxt
	{
		[](const char* pFile)->int { return CSM_LoadPixTone(pFile); },
		[](int iSoundId)           { CSM_UnloadPixTone(iSoundId); }
	},
	{
		[](const char* pFile)->int { return CSM_LoadCustomSound(pFile); },
		[](int iSoundId)           { CSM_UnloadCustomSound(iSoundId); }
	}
};

CustomPlayer* CustomPlayer::subsitute     = NULL;
CustomPlayer* CustomPlayer::first         = NULL;
static bool gbDidCustomPlayerRuntimeCheck = false;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CustomPlayer::CustomPlayer(bool do_link)
{
	if (!gbDidCustomPlayerRuntimeCheck)
	{
		gbDidCustomPlayerRuntimeCheck = true;

		if (PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES != (sizeof(gAnimFrameInfo) / sizeof(gAnimFrameInfo[0])))
		{
			printf("ERROR: gAnimFrameInfo's size mismatches PlayerAnimFrame's size!\r\n");
			throw;
		}
	}

	memset(mAnimList, 0, sizeof(mAnimList));
	memset(mFrameList, 0, sizeof(mFrameList));
	memset(mFaceList, 0, sizeof(mFaceList));
	memset(mBitPlanes, 0, sizeof(mBitPlanes));
	memset(mHeldEntityOffsets, 0, sizeof(mHeldEntityOffsets));
	memset(mCarriedPlayerOffsets, 0, sizeof(mCarriedPlayerOffsets));

	for (int i = 0; i < PlayerAnimation::MAX_PLAYER_ANIMATIONS; i++)
	{
		mAnimList[i].mType  = (PlayerAnimation)i;
		mAnimList[i].mFlags = gPlayerAnimationInfo[i].flags;
		mAnimList[i].mOwner = this;
	}

	mCustomSoundList   = NULL;
	mCustomFrameList   = NULL;
	mCustomSoundCount  = 0;
	mCustomFrameCount  = 0;
	mPath              = NULL;
	mName              = NULL;
	mAuthor            = NULL;
	mNormalSurfaceId   = 0;
	mMaskSurfaceId     = 0;
	mFaceMaskSurfaceId = 0;
	mFaceSurfaceId     = 0;
	mIsLoaded          = false;
	mIsLinked          = do_link;

	SET_RECT(mMinViewbox, 8, 8, 8, 8);
	SET_RECT(mMaxViewbox, 24, 24, 24, 24);

	if (mIsLinked)
	{
		next  = first;
		first = this;
	}
	else
		next = NULL;
}

CustomPlayer::~CustomPlayer()
{
	Reset();

	if (mIsLinked)
	{
		if (first == this)
		{
			first = next;
		}
		else
		{
			for (CustomPlayer* walk = CustomPlayer::first; walk; walk = walk->next)
			{
				if (walk->next != this)
					continue;

				walk->next = next;
				break;
			}
		}

		if (CustomPlayer::subsitute == this)
			CustomPlayer::subsitute = CustomPlayer::first;
	}

	if (mIsLoaded)
		UnloadAssets();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CustomPlayer::Reset()
{
	ClearCustomFrames();
	ClearCustomSounds();

	for (int i = 0; i < PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES; i++)
	{
		if (mBitPlanes[i].ready && mBitPlanes[i].bits)
			delete[] mBitPlanes[i].bits;
	}

	for (int i = 0; i < PlayerAnimation::MAX_PLAYER_ANIMATIONS; i++)
		mAnimList[i].Reset();

	memset(mAnimList, 0, sizeof(mAnimList));
	memset(mFrameList, 0, sizeof(mFrameList));
	memset(mFaceList, 0, sizeof(mFaceList));
	memset(mWeaponOffsets, 0, sizeof(mWeaponOffsets));
	memset(mBitPlanes, 0, sizeof(mBitPlanes));
	memset(mHeldEntityOffsets, 0, sizeof(mHeldEntityOffsets));
	memset(mCarriedPlayerOffsets, 0, sizeof(mCarriedPlayerOffsets));

	for (int i = 0; i < PlayerAnimation::MAX_PLAYER_ANIMATIONS; i++)
	{
		mAnimList[i].mType      = (PlayerAnimation)i;
		mAnimList[i].mFlags     = gPlayerAnimationInfo[i].flags;
		mAnimList[i].mSubsitute = PlayerAnimation::MAX_PLAYER_ANIMATIONS;
		mAnimList[i].mOwner     = this;
	}

	// Set default weapon offsets
	mWeaponOffsets[2].y  = 1;
	mWeaponOffsets[3].y  = 1;
	mWeaponOffsets[4].y  = 4;
	mWeaponOffsets[5].y  = 5;
	mWeaponOffsets[6].y  = 5;
	mWeaponOffsets[7].y  = -4;
	mWeaponOffsets[21].y = 1;
	mWeaponOffsets[22].y = 1;
	mWeaponOffsets[23].y = 4;
	mWeaponOffsets[24].y = -4;
	mWeaponOffsets[36].y = 1;
	mWeaponOffsets[37].y = 4;
	mWeaponOffsets[38].y = -4;
	mWeaponOffsets[39].y = 1;
	mWeaponOffsets[40].y = 5;
	mWeaponOffsets[41].y = -5;

	mFishOffset.x  = 0;
	mFishOffset.y  = 0;

	mHitBox.left   = 5;
	mHitBox.top    = 8;
	mHitBox.right  = 5;
	mHitBox.bottom = 8;

	mViewBox.left   = 8;
	mViewBox.top    = 8;
	mViewBox.right  = 8;
	mViewBox.bottom = 8;

	mFishSpriteOffset.x = 0;
	mFishSpriteOffset.y = 0;

	if (mPath)
	{
		delete[] mPath;
		mPath = NULL;
	}
	
	if (mName)
	{
		delete[] mName;
		mName = NULL;
	}

	if (mAuthor)
	{
		delete[] mAuthor;
		mAuthor = NULL;
	}
}

bool CustomPlayer::LoadPlayer(const char* pFileName, bool bOnlyValidate)
{
	SharedBitStream stream;
	unsigned char version;
	unsigned int mod_path_length;
	char buffer[1024];
	char* ptr = NULL;

	PlayerAnimFrame pLocalFrameList[PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES + MAX_CUSTOMPLAYER_CUSTOM_FRAMES];
	unsigned int iLocalFrameCount = 0;

	// Read the entire file into the bitstream
	{
		FILE* fp = NULL;

		// Make sure we load the file
		if ((fp = fopen(pFileName, "rb")) == NULL)
		{
			printf("ERROR: " __FUNCTION__ "() - Failed to open mod file \"%s\"\r\n", pFileName);

			return false;
		}

		// Go to the end of the file
		fseek(fp, 0, SEEK_END);

		// Get the length of the file
		unsigned int FullSize = ftell(fp);

		// Go back to the mStart of the file
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
	if (memcmp(stream.mBuffer, PXCHAR_HEADER_STRING, 5))
	{
		printf("ERROR: " __FUNCTION__ "() - Invalid magic!\r\n");
		stream.Clear();
		return false;
	}

	// Get the file version
	version = (unsigned char)stream.mBuffer[6];

	// Validate version
	if (version != PXCHAR_VERSION)
	{
		printf("ERROR: " __FUNCTION__ "() - Character version mismatch (expected: %d, got: %d) -- Attempting to read anyways\r\n", PXCHAR_VERSION, version);
	}

	// Stop here if we must
	if (bOnlyValidate)
	{
		stream.Clear();
		return true;
	}

	// Reset memory
	Reset();

	// Set the path
	SetPath(pFileName);

	// Skip past the header & version
	stream.SetCurPos(56);

	// Read basic information
	stream.ReadString(buffer, 256); SetName(buffer);
	stream.ReadString(buffer, 256); SetAuthor(buffer);
	int iFrameCount     = stream.ReadInt(sizeof(unsigned char) << 3);
	int iFaceCount      = (version >= 2 ? stream.ReadInt(sizeof(unsigned char) << 3) : 0);
	mCustomFrameCount   = (version >= 8 ? stream.ReadInt(sizeof(unsigned char) << 3) : 0);
	mCustomSoundCount   = (version >= 8 ? stream.ReadInt(sizeof(unsigned char) << 3) : 0);
	int iAnimationCount = (version >= 8 ? stream.ReadInt(sizeof(unsigned char) << 3) : 0);
	mFishOffset.x       = (version >= 3 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
	mFishOffset.y       = (version >= 3 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
	mFishSpriteOffset.x = (version >= 4 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
	mFishSpriteOffset.y = (version >= 4 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);

	// Allocate custom lists
	mCustomFrameList = (!mCustomFrameCount ? NULL : (CustomFrame*)malloc(sizeof(CustomFrame) * mCustomFrameCount));
	mCustomSoundList = (!mCustomSoundCount ? NULL : (CustomSound*)malloc(sizeof(CustomSound) * mCustomSoundCount));

	// Read hit box
	if (version >= 3)
	{
		mHitBox.left   = stream.ReadInt(sizeof(unsigned int) << 3);
		mHitBox.top    = stream.ReadInt(sizeof(unsigned int) << 3);
		mHitBox.right  = stream.ReadInt(sizeof(unsigned int) << 3);
		mHitBox.bottom = stream.ReadInt(sizeof(unsigned int) << 3);

		mHitBox.left   = CLAMP(mHitBox.left,   5, 24);
		mHitBox.right  = CLAMP(mHitBox.right,  5, 24);
		mHitBox.top    = CLAMP(mHitBox.top,    8, 24);
		mHitBox.bottom = CLAMP(mHitBox.bottom, 8, 24);
	}

	// Read view box
	if (version >= 3)
	{
		mViewBox.left   = stream.ReadInt(sizeof(unsigned int) << 3);
		mViewBox.top    = stream.ReadInt(sizeof(unsigned int) << 3);
		mViewBox.right  = stream.ReadInt(sizeof(unsigned int) << 3);
		mViewBox.bottom = stream.ReadInt(sizeof(unsigned int) << 3);
	}

	// Read custom frames
	for (int i = 0; i < mCustomFrameCount; i++)
	{
		CustomFrame* pFrame = &mCustomFrameList[i];
		
		// Read the string
		stream.ReadString(buffer, 256);

		// Copy the string into a new buffer
		pFrame->frame_name_crc = StringCRC32((void*)buffer, strlen(buffer));
		pFrame->frame_name     = new char[strlen(buffer) + 1];
		strcpy(pFrame->frame_name, buffer);

		pLocalFrameList[iFrameCount + i] = (PlayerAnimFrame)(PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES + i);

		// Read the rest of the data
		pFrame->flags                = stream.ReadInt(sizeof(unsigned int) << 3);
		pFrame->rect.left            = stream.ReadInt(sizeof(unsigned int) << 3);
		pFrame->rect.top             = stream.ReadInt(sizeof(unsigned int) << 3);
		pFrame->rect.right           = stream.ReadInt(sizeof(unsigned int) << 3);
		pFrame->rect.bottom          = stream.ReadInt(sizeof(unsigned int) << 3);
		pFrame->weapon_offset.x      = stream.ReadInt(sizeof(unsigned int) << 3);
		pFrame->weapon_offset.y      = stream.ReadInt(sizeof(unsigned int) << 3);
		pFrame->held_entity_offset.x = (version < 11 ? 4 : stream.ReadInt(sizeof(unsigned int) << 3));
		pFrame->held_entity_offset.y = (version < 11 ? 10 : stream.ReadInt(sizeof(unsigned int) << 3));
		pFrame->carry_offset.x       = (version < 12 ? 7 : stream.ReadInt(sizeof(unsigned int) << 3));
		pFrame->carry_offset.y       = (version < 12 ? 3 : stream.ReadInt(sizeof(unsigned int) << 3));

		if (version < 11)
			pFrame->flags |= PAF_SHOW_HELD_ENTITY;
	}
	
	// Read custom sounds
	for (int i = 0; i < mCustomSoundCount; i++)
	{
		CustomSound* pSound = &mCustomSoundList[i];

		// Read the string
		stream.ReadString(buffer, 256);

		// Copy the string into a new buffer
		pSound->file_path_crc = StringCRC32((void*)buffer, strlen(buffer));
		pSound->file_path     = new char[strlen(buffer) + 1];
		pSound->type          = CustomSoundType::CUSTOM_SOUND_PXT;
		strcpy(pSound->file_path, buffer);

		// Ready the custom sound ID
		pSound->custom_sound_id = -1;
		pSound->type            = (CustomSoundType)stream.ReadInt(sizeof(unsigned char) << 3);
		pSound->type            = (CustomSoundType)CLAMP(pSound->type, 0, CustomSoundType::MAX_CUSTOM_SOUND_TYPES - 1);
	}

	// Read frames
	for (int i = 0; i < iFrameCount; i++)
	{
		unsigned int iFrameCRC = stream.ReadInt(sizeof(unsigned int) << 3);
		CPLR_POINT weapon_off;
		CPLR_POINT held_off;
		CPLR_POINT carry_off;
		RECT rect;

		rect.left    = stream.ReadInt(sizeof(unsigned int) << 3);
		rect.top     = stream.ReadInt(sizeof(unsigned int) << 3);
		rect.right   = stream.ReadInt(sizeof(unsigned int) << 3);
		rect.bottom  = stream.ReadInt(sizeof(unsigned int) << 3);
		weapon_off.x = (version >= 6 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
		weapon_off.y = (version >= 6 ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
		held_off.x   = (version < 11 ? 0 : stream.ReadInt(sizeof(unsigned int) << 3));
		held_off.y   = (version < 11 ? 0 : stream.ReadInt(sizeof(unsigned int) << 3));
		carry_off.x  = (version < 12 ? 0 : stream.ReadInt(sizeof(unsigned int) << 3));
		carry_off.y  = (version < 12 ? 0 : stream.ReadInt(sizeof(unsigned int) << 3));

		// Add to local frame table
		++iLocalFrameCount;
		pLocalFrameList[iLocalFrameCount - 1] = (PlayerAnimFrame)0;

		// Loop through and find the animation that belongs to the given CRC
		for (int j = 0; j < (sizeof(gAnimFrameInfo) / sizeof(gAnimFrameInfo[0])); j++)
		{
			if (gAnimFrameInfo[j].crc != iFrameCRC)
				continue;

			if (version < 11)
			{
				held_off.x  = gAnimFrameInfo[j].default_puppy.x;
				held_off.y  = gAnimFrameInfo[j].default_puppy.y;
			}

			if (version < 12)
			{
				carry_off.x = gAnimFrameInfo[j].default_carry.x;
				carry_off.y = gAnimFrameInfo[j].default_carry.y;
			}

			memcpy(&mFrameList[j], &rect, sizeof(RECT));
			memcpy(&mWeaponOffsets[j], &weapon_off, sizeof(CPLR_POINT));
			memcpy(&mHeldEntityOffsets[j], &held_off, sizeof(CPLR_POINT));
			memcpy(&mCarriedPlayerOffsets[j], &carry_off, sizeof(CPLR_POINT));

			// Found it! Add it to the local table
			pLocalFrameList[iLocalFrameCount - 1] = (PlayerAnimFrame)j;

			break;
		}
	}

	iLocalFrameCount += mCustomFrameCount;

	// Read faces
	for (int i = 0; i < iFaceCount; i++)
	{
		// Don't allow a buffer overflow
		if (i >= (sizeof(mFaceList) / sizeof(mFaceList[0])))
			break;

		if (version >= 5)
		{
			unsigned int iFrameCRC = stream.ReadInt(sizeof(unsigned int) << 3);
			RECT rect;

			rect.left   = stream.ReadInt(sizeof(unsigned int) << 3);
			rect.top    = stream.ReadInt(sizeof(unsigned int) << 3);
			rect.right  = rect.left + 48;
			rect.bottom = rect.top  + 48;

			for (int j = 0; j < (sizeof(gAnimFaceInfo) / sizeof(gAnimFaceInfo[0])); j++)
			{
				if (gAnimFaceInfo[j].crc != iFrameCRC)
					continue;

				memcpy(&mFaceList[j], &rect, sizeof(RECT));
				break;
			}
		}
		else
		{
			// Don't allow a buffer overflow
			if (i >= (sizeof(mFaceList) / sizeof(mFaceList[0])))
				break;

			RECT* pRect   = &mFaceList[i];
			pRect->left   = stream.ReadInt(sizeof(unsigned int) << 3);
			pRect->top    = stream.ReadInt(sizeof(unsigned int) << 3);
			pRect->right  = pRect->left + 48;
			pRect->bottom = pRect->top + 48;
		}
	}

	// Read animations
	for (int i = 0; i < iAnimationCount; i++)
	{
		unsigned int iAnimCRC        = stream.ReadInt(sizeof(unsigned int) << 3);
		bool bLoop                   = stream.ReadFlag();
		bool bHasSubsitute           = (version >= 9 ? stream.ReadFlag() : false);
		unsigned int iAnimFrameCount = (!bHasSubsitute ? stream.ReadInt(sizeof(unsigned short) << 3) : 0);
		unsigned int iAnimLoopStart  = (version >= 9 && !bHasSubsitute ? stream.ReadInt(sizeof(unsigned short) << 3) : 0);
		unsigned int iSubAnimCRC     = (bHasSubsitute ? stream.ReadInt(sizeof(unsigned int) << 3) : 0);
		unsigned int iAnimIdx        = -1;
		unsigned int iSubAnimIdx     = -1;

		// Search for the animations by CRC
		for (int j = 0; j < sizeof(gPlayerAnimationInfo) / sizeof(gPlayerAnimationInfo[0]); j++)
		{
			if (gPlayerAnimationInfo[j].crc == iAnimCRC)
			{
				iAnimIdx = j;

				if (!bHasSubsitute || iSubAnimIdx != -1)
					break;
			}
			else if (gPlayerAnimationInfo[j].crc == iSubAnimCRC)
			{
				iSubAnimIdx = j;

				if (iAnimIdx != -1)
					break;
			}
		}

		// Get the corresponding animation
		CustomPlayerAnimation* pAnim = (iAnimIdx == -1 ? NULL : &mAnimList[iAnimIdx]);

		// If it loops, then set the flag appropriately.
		if (pAnim)
		{
			if (bLoop)
				pAnim->mFlags |= PlayerCustomAnimFlags::PCAF_LOOP;
			else
				pAnim->mFlags &= ~PlayerCustomAnimFlags::PCAF_LOOP;
		}

		if (bHasSubsitute)
		{
			if (pAnim)
			{
				pAnim->mSubsitute = (PlayerAnimation)iSubAnimIdx;
				pAnim->mDefault   = false;
			}

			continue;
		}

		if (pAnim)
			pAnim->Lock();

		// Loop through all of the frames
		for (int j = 0; j < iAnimFrameCount; j++)
		{
			unsigned int iFrameNo         = stream.ReadInt(sizeof(unsigned short) << 3);
			unsigned int iLocalFrameIndex = stream.ReadRangedInt(0, iFrameCount + mCustomFrameCount);
			unsigned int iSoundID         = stream.ReadInt(sizeof(unsigned short) << 3);
			unsigned int iDelay           = stream.ReadInt(sizeof(unsigned short) << 3);
			short iSpriteOffsetX          = (version >= 10 ? stream.ReadInt(sizeof(short) << 3) : 0);
			short iSpriteOffsetY          = (version >= 10 ? stream.ReadInt(sizeof(short) << 3) : 0);

			// Add the frame
			if (!pAnim)
				continue;

			pAnim->AddFrame(iFrameNo, pLocalFrameList[iLocalFrameIndex], iSoundID, iDelay, iSpriteOffsetX, iSpriteOffsetY);
		}

		if (pAnim)
		{
			pAnim->Unlock();
			pAnim->CalculateMetadata();
			pAnim->SetLoopStartFrameNo(iAnimLoopStart);
		}
	}

	stream.Clear();

	// Set the default frames for the animations that did not load
	for (int i = 0; i < PlayerAnimation::MAX_PLAYER_ANIMATIONS; i++)
	{
		if (mAnimList[i].mFrameCount)
			continue;

		mAnimList[i].SetDefaults();
	}

	// Have animations that weren't defined yet copy from other faces.
	for (int i = 0; i < (sizeof(gAnimFaceInfo) / sizeof(gAnimFaceInfo[0])); i++)
	{
		// Skip ones that we got
		if (version >= gAnimFaceInfo[i].version || gAnimFaceInfo[i].inherit_from == -1)
			continue;

		memcpy(&mFaceList[i], &mFaceList[gAnimFaceInfo[i].inherit_from], sizeof(RECT));
	}

	// Recalculate minimum and maximums
	RecalculateMeta();

	// Clamp the viewbox
	mViewBox.left   = CLAMP(mViewBox.left,   mMinViewbox.left,   mMaxViewbox.left);
	mViewBox.right  = CLAMP(mViewBox.right,  mMinViewbox.right,  mMaxViewbox.right);
	mViewBox.top    = CLAMP(mViewBox.top,    mMinViewbox.top,    mMaxViewbox.top);
	mViewBox.bottom = CLAMP(mViewBox.bottom, mMinViewbox.bottom, mMaxViewbox.bottom);

	// Done
	return true;
}

bool CustomPlayer::SavePlayer()
{
#ifdef _CAVESTORY_MULTIPLAYER
	if (!mPath)
		return false;

	int iValidSounds     = 0;
	int iValidFrames     = 0;
	int iValidAnimations = 0;
	SharedBitStream stream;
	char buffer[1024];

	// Count number of valid animations
	bool bValidAnimationList[PlayerAnimation::MAX_PLAYER_ANIMATIONS];
	unsigned int iValidSoundQuickTable[MAX_CUSTOMPLAYER_CUSTOM_SOUNDS];
	unsigned int iValidFrameQuickTable[MAX_CUSTOMPLAYER_CUSTOM_FRAMES];

	// Set all to zero
	memset(bValidAnimationList, 0, sizeof(bValidAnimationList));
	memset(iValidFrameQuickTable, 0, sizeof(iValidFrameQuickTable));
	memset(iValidSoundQuickTable, 0, sizeof(iValidSoundQuickTable));

	// Determine which animations are valid and which are not
	for (int i = 0; i < PlayerAnimation::MAX_PLAYER_ANIMATIONS; i++)
	{
		// Check for default values
		mAnimList[i].mDefault = mAnimList[i].SetDefaults(true);

		// Skip animations with no frames in them
		// Additionally, skip animations with only default frames in them (so we don't unnecessarily save data that we can restore on next load)
		if (!mAnimList[i].mFrameCount || mAnimList[i].mDefault)
			continue;

		int iValidAnimFrames = 0;

		// Count the number of valid frames
		for (CustomPlayerAnimation::Frame* walk = mAnimList[i].mFirstFrame; walk; walk = walk->next)
		{
			// Skip invalid frames
			if (walk->player_frame == PlayerAnimFrame::FRAME_INVALID && walk->sound_no >= MAX_PLAYER_SOUNDS + mCustomSoundCount)
				continue;

			++iValidAnimFrames;
		}

		// Skip this animation if it contains no valid frames
		if (!iValidAnimFrames)
			continue;

		bValidAnimationList[i] = true;
		++iValidAnimations;
	}

	// Reset sound & frame count
	for (int i = 0; i < MAX(mCustomFrameCount, mCustomSoundCount); i++)
	{
		if (i < mCustomFrameCount)
			mCustomFrameList[i].use_count = 0;

		if (i < mCustomSoundCount)
			mCustomSoundList[i].use_count = 0;
	}

	// Calculate use counts
	for (int j = 0; j < sizeof(mAnimList) / sizeof(mAnimList[0]); j++)
	{
		// Skip animations deemed invalid
		if (!bValidAnimationList[j])
			continue;

		// Look through this animation's frames to find any use cases for this sound
		for (CustomPlayerAnimation::Frame* pFrame = mAnimList[j].mFirstFrame; pFrame; pFrame = pFrame->next)
		{
			if (pFrame->sound_no >= MAX_PLAYER_SOUNDS && pFrame->sound_no < MAX_PLAYER_SOUNDS + mCustomSoundCount)
				++mCustomSoundList[pFrame->sound_no - MAX_PLAYER_SOUNDS].use_count;
			
			if (pFrame->player_frame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES && pFrame->player_frame < PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES + mCustomFrameCount)
				++mCustomFrameList[pFrame->player_frame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES].use_count;
		}
	}

	// Reset sound & frame count
	for (int i = 0; i < MAX(mCustomFrameCount, mCustomSoundCount); i++)
	{
		if (i < mCustomFrameCount)
		{
			if (mCustomFrameList[i].use_count)
				iValidFrameQuickTable[i] = PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES + iValidFrames++;
			else
				iValidFrameQuickTable[i] = PlayerAnimFrame::FRAME_IDLE;
		}

		if (i < mCustomSoundCount)
		{
			if (mCustomSoundList[i].use_count)
				iValidSoundQuickTable[i] = MAX_PLAYER_SOUNDS + iValidSounds++;
			else
				iValidSoundQuickTable[i] = MAX_CUSTOMPLAYER_CUSTOM_SOUNDS;
		}
	}

	// Write file info
	stream.Write(PXCHAR_HEADER_STRING, strlen(PXCHAR_HEADER_STRING) << 3);
	stream.WriteInt(PXCHAR_VERSION, sizeof(unsigned char) << 3);

	// Write header information
	stream.WriteString(mName);
	stream.WriteString(mAuthor);
	stream.WriteInt(PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES, sizeof(unsigned char) << 3);
	stream.WriteInt(PlayerFaceFrame::MAX_PLAYER_FACES, sizeof(unsigned char) << 3);
	stream.WriteInt(iValidFrames, sizeof(unsigned char) << 3);
	stream.WriteInt(iValidSounds, sizeof(unsigned char) << 3);
	stream.WriteInt(iValidAnimations, sizeof(unsigned char) << 3);
	stream.WriteInt(mFishOffset.x, sizeof(unsigned int) << 3);
	stream.WriteInt(mFishOffset.y, sizeof(unsigned int) << 3);
	stream.WriteInt(mFishSpriteOffset.x, sizeof(unsigned int) << 3);
	stream.WriteInt(mFishSpriteOffset.y, sizeof(unsigned int) << 3);

	// Write hit box
	stream.WriteInt(mHitBox.left,   sizeof(unsigned int) << 3);
	stream.WriteInt(mHitBox.top,    sizeof(unsigned int) << 3);
	stream.WriteInt(mHitBox.right,  sizeof(unsigned int) << 3);
	stream.WriteInt(mHitBox.bottom, sizeof(unsigned int) << 3);

	// Write view box
	stream.WriteInt(mViewBox.left,   sizeof(unsigned int) << 3);
	stream.WriteInt(mViewBox.top,    sizeof(unsigned int) << 3);
	stream.WriteInt(mViewBox.right,  sizeof(unsigned int) << 3);
	stream.WriteInt(mViewBox.bottom, sizeof(unsigned int) << 3);

	// Write custom frames
	for (int i = 0; i < mCustomFrameCount; i++)
	{
		CustomFrame* pFrame = &mCustomFrameList[i];

		if (!pFrame->use_count)
			continue;
		
		// Write the data
		stream.WriteString(pFrame->frame_name, 256);
		stream.WriteInt(pFrame->flags, sizeof(unsigned int) << 3);
		stream.WriteInt(pFrame->rect.left, sizeof(unsigned int) << 3);
		stream.WriteInt(pFrame->rect.top, sizeof(unsigned int) << 3);
		stream.WriteInt(pFrame->rect.right, sizeof(unsigned int) << 3);
		stream.WriteInt(pFrame->rect.bottom, sizeof(unsigned int) << 3);
		stream.WriteInt(pFrame->weapon_offset.x, sizeof(unsigned int) << 3);
		stream.WriteInt(pFrame->weapon_offset.y, sizeof(unsigned int) << 3);
		stream.WriteInt(pFrame->held_entity_offset.x, sizeof(unsigned int) << 3);
		stream.WriteInt(pFrame->held_entity_offset.y, sizeof(unsigned int) << 3);
		stream.WriteInt(pFrame->carry_offset.x, sizeof(unsigned int) << 3);
		stream.WriteInt(pFrame->carry_offset.y, sizeof(unsigned int) << 3);
	}
	
	// Write custom sounds
	for (int i = 0; i < mCustomSoundCount; i++)
	{
		CustomSound* pSound = &mCustomSoundList[i];

		if (!pSound->use_count)
			continue;

		// Write the data
		stream.WriteString(pSound->file_path, 256);
		stream.WriteInt(pSound->type, sizeof(unsigned char) << 3);
	}

	// Write frames
	for (int i = 0; i < PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES; i++)
	{
		RECT* pRect = &mFrameList[i];

		stream.WriteInt(gAnimFrameInfo[i].crc, sizeof(unsigned int) << 3);
		stream.WriteInt(pRect->left, sizeof(unsigned int) << 3);
		stream.WriteInt(pRect->top, sizeof(unsigned int) << 3);
		stream.WriteInt(pRect->right, sizeof(unsigned int) << 3);
		stream.WriteInt(pRect->bottom, sizeof(unsigned int) << 3);
		stream.WriteInt(mWeaponOffsets[i].x, sizeof(unsigned int) << 3);
		stream.WriteInt(mWeaponOffsets[i].y, sizeof(unsigned int) << 3);
		stream.WriteInt(mHeldEntityOffsets[i].x, sizeof(unsigned int) << 3);
		stream.WriteInt(mHeldEntityOffsets[i].y, sizeof(unsigned int) << 3);
		stream.WriteInt(mCarriedPlayerOffsets[i].x, sizeof(unsigned int) << 3);
		stream.WriteInt(mCarriedPlayerOffsets[i].y, sizeof(unsigned int) << 3);
	}

	// Write faces
	for (int i = 0; i < PlayerFaceFrame::MAX_PLAYER_FACES; i++)
	{
		RECT* pRect = &mFaceList[i];

		stream.WriteInt(gAnimFaceInfo[i].crc, sizeof(unsigned int) << 3);
		stream.WriteInt(pRect->left, sizeof(unsigned int) << 3);
		stream.WriteInt(pRect->top, sizeof(unsigned int) << 3);
	}

	// Write animations
	for (int i = 0; i < PlayerAnimation::MAX_PLAYER_ANIMATIONS; i++)
	{
		CustomPlayerAnimation* pAnim = &mAnimList[i];

		// Skip animations with no frame count.
		if (!bValidAnimationList[i])
			continue;

		// Write the animation data
		stream.WriteInt(gPlayerAnimationInfo[i].crc, sizeof(unsigned int) << 3);
		stream.WriteFlag((pAnim->mFlags & PlayerCustomAnimFlags::PCAF_LOOP) ? true : false);
		stream.WriteFlag(pAnim->mSubsitute != PlayerAnimation::MAX_PLAYER_ANIMATIONS);

		if (pAnim->mSubsitute != PlayerAnimation::MAX_PLAYER_ANIMATIONS)
		{
			stream.WriteInt(gPlayerAnimationInfo[pAnim->mSubsitute].crc, sizeof(unsigned int) << 3);
			continue;
		}

		stream.WriteInt(pAnim->mFrameCount, sizeof(unsigned short) << 3);
		stream.WriteInt(pAnim->mLoopStart, sizeof(unsigned short) << 3);

		// Write the animations
		for (CustomPlayerAnimation::Frame* walk = pAnim->mFirstFrame; walk; walk = walk->next)
		{
			stream.WriteInt(walk->frame_no, sizeof(unsigned short) << 3);
			stream.WriteRangedInt((walk->player_frame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES && walk->player_frame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES < MAX_CUSTOMPLAYER_CUSTOM_FRAMES ? iValidFrameQuickTable[walk->player_frame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES] : walk->player_frame), 0, PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES + iValidFrames);
			stream.WriteInt((walk->sound_no >= MAX_PLAYER_SOUNDS && walk->sound_no - MAX_PLAYER_SOUNDS < MAX_CUSTOMPLAYER_CUSTOM_SOUNDS ? iValidSoundQuickTable[walk->sound_no - MAX_PLAYER_SOUNDS] : walk->sound_no), sizeof(unsigned short) << 3);
			stream.WriteInt(walk->delay, sizeof(unsigned short) << 3);
			stream.WriteInt(walk->offset.x, sizeof(short) << 3);
			stream.WriteInt(walk->offset.y, sizeof(short) << 3);
		}
	}

	// Save the entire bitstream into a file
	{
		FILE* fp;

		// Build the path to the mod file
		sprintf_s(buffer, sizeof(buffer), "%s/Player.pxchar", mPath);

		// Attempt to open the file for writing
		if ((fp = fopen(buffer, "wb")) == NULL)
		{
			printf("ERROR: " __FUNCTION__ "() - Failed to open file \"%s\"\r\n", buffer);
			return false;
		}

		fwrite(stream.mBuffer, stream.mBufferLen, 1, fp);

		// We're finished with the file, so close it.
		fclose(fp);
	}

	stream.Clear();

	// Done
	return true;
#else
	return false;
#endif
}

void CustomPlayer::RecalculateMeta()
{
	//int iMinWidth  = -1;
	//int iMinHeight = -1;
	int iMaxWidth  = -1;
	int iMaxHeight = -1;
	
	// Look for a uniform height frame
	for (int i = 0; i < PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES; i++)
	{
		RECT* pFrame = &mFrameList[i];

		// Looking for a frame with a uniform height.
		if (!(gAnimFrameInfo[i].flags & PlayerAnimFlags::PAF_UNIFORM_HEIGHT))
			continue;
		
//		if (iMinWidth == -1 || iMinWidth > (pFrame->right - pFrame->left))
//			iMinWidth = (pFrame->right - pFrame->left);
		
		if (iMaxWidth == -1 || iMaxWidth < (pFrame->right - pFrame->left))
			iMaxWidth = (pFrame->right - pFrame->left);
		
//		if (iMinHeight == -1 || iMinHeight > (pFrame->bottom - pFrame->top))
//			iMinHeight = (pFrame->bottom - pFrame->top);
		
		if (iMaxHeight == -1 || iMaxHeight < (pFrame->bottom - pFrame->top))
			iMaxHeight = (pFrame->bottom - pFrame->top);
	}

	// Calculate min viewbox
	SET_RECT(
		mMinViewbox,
		8,
		8,
		8,
		8
	);

	// Calculate max viewbox
	SET_RECT(
		mMaxViewbox,
		iMaxWidth / 2,
		iMaxHeight,
		iMaxWidth / 2,
		8
	);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* CustomPlayer::GetName()
{
	return mName;
}

const char* CustomPlayer::GetAuthor()
{
	return mAuthor;
}

const char* CustomPlayer::GetPath()
{
	return mPath;
}

RECT* CustomPlayer::GetAnimationRect(PlayerAnimFrame iFrame, RECT* pOut)
{
	if (iFrame < 0 || iFrame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES + mCustomFrameCount)
		return NULL;

	if (iFrame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES)
	{
		if (pOut)
		{
			// Copy into the out buffer
			memcpy(pOut, &mCustomFrameList[iFrame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES].rect, sizeof(RECT));

			// Done
			return pOut;
		}

		return &mCustomFrameList[iFrame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES].rect;
	}

	if (pOut)
	{
		// Copy into the out buffer
		memcpy(pOut, &mFrameList[iFrame], sizeof(RECT));

		// Done
		return pOut;
	}

	return &mFrameList[iFrame];
}

RECT* CustomPlayer::GetFaceRect(PlayerFaceFrame iFace, RECT* pOut)
{
	if (iFace < 0 || iFace >= PlayerFaceFrame::MAX_PLAYER_FACES)
		return NULL;

	if (pOut)
	{
		// Copy into the out buffer
		memcpy(pOut, &mFaceList[iFace], sizeof(RECT));

		// Done
		return pOut;
	}

	return &mFaceList[iFace];
}

CustomPlayer::CPLR_POINT* CustomPlayer::GetWeaponOffset(PlayerAnimFrame iFrame, CPLR_POINT* pPoint)
{
	if (iFrame < 0 || iFrame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES + mCustomFrameCount)
		return NULL;

	if (iFrame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES)
	{
		if (pPoint)
		{
			// Copy into the out buffer
			memcpy(pPoint, &mCustomFrameList[iFrame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES].weapon_offset, sizeof(CPLR_POINT));

			// Done
			return pPoint;
		}

		return &mCustomFrameList[iFrame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES].weapon_offset;
	}

	if (pPoint)
	{
		// Copy into the out buffer
		memcpy(pPoint, &mWeaponOffsets[iFrame], sizeof(CPLR_POINT));

		// Done
		return pPoint;
	}

	return &mWeaponOffsets[iFrame];
}

CustomPlayer::CPLR_POINT* CustomPlayer::GetHeldEntityOffset(PlayerAnimFrame iFrame, CPLR_POINT* pPoint)
{
	if (iFrame < 0 || iFrame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES + mCustomFrameCount)
		return NULL;

	if (iFrame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES)
	{
		if (pPoint)
		{
			// Copy into the out buffer
			memcpy(pPoint, &mCustomFrameList[iFrame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES].held_entity_offset, sizeof(CPLR_POINT));

			// Done
			return pPoint;
		}

		return &mCustomFrameList[iFrame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES].held_entity_offset;
	}

	if (pPoint)
	{
		// Copy into the out buffer
		memcpy(pPoint, &mHeldEntityOffsets[iFrame], sizeof(CPLR_POINT));

		// Done
		return pPoint;
	}

	return &mHeldEntityOffsets[iFrame];
}

CustomPlayer::CPLR_POINT* CustomPlayer::GetCarryPlayerOffset(PlayerAnimFrame iFrame, CPLR_POINT* pPoint)
{
	if (iFrame < 0 || iFrame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES + mCustomFrameCount)
		return NULL;

	if (iFrame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES)
	{
		if (pPoint)
		{
			// Copy into the out buffer
			memcpy(pPoint, &mCustomFrameList[iFrame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES].carry_offset, sizeof(CPLR_POINT));

			// Done
			return pPoint;
		}

		return &mCustomFrameList[iFrame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES].carry_offset;
	}

	if (pPoint)
	{
		// Copy into the out buffer
		memcpy(pPoint, &mCarriedPlayerOffsets[iFrame], sizeof(CPLR_POINT));

		// Done
		return pPoint;
	}

	return &mCarriedPlayerOffsets[iFrame];
}

CustomPlayerAnimation* CustomPlayer::GetAnimation(PlayerAnimation iAnimType, bool bSubsitute)
{
	iAnimType                    = (PlayerAnimation)CLAMP(iAnimType, 0, PlayerAnimation::MAX_PLAYER_ANIMATIONS - 1);
	CustomPlayerAnimation* pAnim = &mAnimList[iAnimType];

	// Find the subsitute animation
	if (bSubsitute && pAnim->mSubsitute != PlayerAnimation::MAX_PLAYER_ANIMATIONS)
	{
		while (pAnim->mSubsitute != PlayerAnimation::MAX_PLAYER_ANIMATIONS)
		{
			pAnim = &mAnimList[pAnim->mSubsitute];

			// Stop if we've somehow come full circle, so we're not stuck in an infinite loop
			if (pAnim->mType == iAnimType)
				break;
		}
	}

	return pAnim;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CustomPlayer::SetName(const char* pNewName)
{
	if (mName)
	{
		delete[] mName;
		mName = NULL;
	}

	mName = new char[strlen(pNewName) + 1];
	strcpy(mName, pNewName);
}

void CustomPlayer::SetAuthor(const char* pNewAuthor)
{
	if (mAuthor)
	{
		delete[] mAuthor;
		mAuthor = NULL;
	}

	mAuthor = new char[strlen(pNewAuthor) + 1];
	strcpy(mAuthor, pNewAuthor);
}

void CustomPlayer::SetPath(const char* pNewPath)
{
	if (mPath)
	{
		delete[] mPath;
		mPath = NULL;
	}

	char buffer[1024];
	char* ptr;

	strcpy_s(buffer, sizeof(buffer), pNewPath);

	// Convert all backward slashes into forward slashes
	while ((ptr = strchr(buffer, '\\')) != NULL)
		*ptr = '/';

	// Remove the filename
	if ((ptr = strrchr(buffer, '/')) != NULL)
		*ptr = 0;


	mPath = new char[strlen(buffer) + 1];
	strcpy(mPath, buffer);
}

void CustomPlayer::SetAnimationRect(PlayerAnimFrame iFrame, RECT rect)
{
	if (iFrame < 0 || iFrame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES)
		return;

	mFrameList[iFrame].left   = rect.left;
	mFrameList[iFrame].top    = rect.top;
	mFrameList[iFrame].right  = rect.right;
	mFrameList[iFrame].bottom = rect.bottom;
}

void CustomPlayer::SetFaceRect(PlayerFaceFrame iFace, int iX, int iY)
{
	if (iFace < 0 || iFace >= PlayerFaceFrame::MAX_PLAYER_FACES)
		return;

	mFaceList[iFace].left   = iX;
	mFaceList[iFace].top    = iY;
	mFaceList[iFace].right  = iX + 48;
	mFaceList[iFace].bottom = iY + 48;
}

void CustomPlayer::SetWeaponOffset(PlayerAnimFrame iFrame, int iX, int iY)
{
	if (iFrame < 0 || iFrame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES)
		return;

	mWeaponOffsets[iFrame].x = 0;
	mWeaponOffsets[iFrame].y = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CustomPlayer::CustomFrame* CustomPlayer::GetCustomFrame(const char* pFrameName, bool bCreate)
{
	unsigned int iFrameCrc = StringCRC32((void*)pFrameName, strlen(pFrameName));

	// Look for it
	if (!bCreate)
	{
		for (int i = 0; i < mCustomFrameCount; i++)
		{
			// Skip mismatches
			if (mCustomFrameList[i].frame_name_crc != iFrameCrc)
				continue;

			// Found it!
			return &mCustomFrameList[i];
		}
	}

	// Stop here if the frame count exceeds maximum capacity
	if (mCustomFrameCount + 1 >= MAX_CUSTOMPLAYER_CUSTOM_FRAMES)
		return NULL;

	// We didn't find it; Allocate a new frame.
	if (!mCustomFrameList)
	{
		mCustomFrameCount = 1;
		mCustomFrameList  = (CustomFrame*)malloc(sizeof(CustomFrame) * mCustomFrameCount);
	}
	else
	{
		mCustomFrameCount += 1;
		mCustomFrameList   = (CustomFrame*)realloc((void*)mCustomFrameList, sizeof(CustomFrame) * mCustomFrameCount);
	}

	// Get the newly allocated frame
	CustomFrame* pNewFrame = &mCustomFrameList[mCustomFrameCount - 1];

	// Configure the newly allocated frame
	pNewFrame->frame_name      = new char[strlen(pFrameName) + 1];
	pNewFrame->frame_name_crc  = iFrameCrc;
	pNewFrame->rect.left       = 0;
	pNewFrame->rect.top        = 0;
	pNewFrame->rect.right      = 0;
	pNewFrame->rect.bottom     = 0;
	pNewFrame->weapon_offset.x = 0;
	pNewFrame->weapon_offset.y = 0;
	pNewFrame->flags           = 0;

	// Copy the string
	strcpy(pNewFrame->frame_name, pFrameName);

	// Done!
	return pNewFrame;
}

bool CustomPlayer::RemoveCustomFrame(const char* pFrameName)
{
	unsigned int iFrameCrc = StringCRC32((void*)pFrameName, strlen(pFrameName));
	int i;

	// Look for it
	for (i = 0; i < mCustomFrameCount; i++)
	{
		// Skip mismatches
		if (mCustomFrameList[i].frame_name_crc != iFrameCrc)
			continue;

		// Free its name
		delete[] mCustomFrameList[i].frame_name;

		// Search for animation frames that use this frame
		for (int k = 0; k < PlayerAnimation::MAX_PLAYER_ANIMATIONS; k++)
		{
			// Loop through this animation's frames
			for (CustomPlayerAnimation::Frame* walk = mAnimList[k].mFirstFrame; walk; walk = walk->next)
			{
				// Skip mismatches
				if (walk->player_frame != PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES + i)
				{
					// Decrement frames that exceed this one
					if (walk->player_frame > PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES + i)
						walk->player_frame = (PlayerAnimFrame)(walk->player_frame - 1);

					continue;
				}

				walk->player_frame = (PlayerAnimFrame)1;
			}
		}

		// Remove it from the list
#ifdef _CAVESTORY_MULTIPLAYER
		LIST_ERASE(mCustomFrameList, mCustomFrameCount, CustomFrame, i);
#endif
		--mCustomFrameCount;

		if (mCustomFrameCount <= 0)
		{
			free(mCustomFrameList);
			mCustomFrameList = NULL;
		}
		else
		{
			CustomFrame* pNewBuffer = (CustomFrame*)malloc(sizeof(CustomFrame) * mCustomFrameCount);

			// Copy the old contents over to the new buffer
			memcpy(pNewBuffer, mCustomFrameList, sizeof(CustomFrame) * mCustomFrameCount);

			// Free the old list
			free(mCustomFrameList);

			// Set the new buffer
			mCustomFrameList = pNewBuffer;
		}

		// Done!
		return true;
	}

	// Couldn't find the frame...
	return false;
}

void CustomPlayer::ClearCustomFrames()
{
	if (!mCustomFrameList)
		return;

	for (int i = 0; i < mCustomFrameCount; i++)
		delete[] mCustomFrameList[i].frame_name;

	free(mCustomFrameList);
	mCustomFrameList  = NULL;
	mCustomFrameCount = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CustomPlayer::CustomSound* CustomPlayer::GetCustomSound(const char* pFileName, bool bCreate)
{
	// Validate the filepath
	char pBuffer[2048];
	char pFileBuffer[480];
	strcpy_s(pFileBuffer, sizeof(pFileBuffer), pFileName);
	char* tmp = NULL;

	CustomSoundType iSoundType = CustomSoundType::MAX_CUSTOM_SOUND_TYPES;

	// Determine file extension
	for (int i = 0; i < sizeof(gPlayerCustomSoundExtList) / sizeof(gPlayerCustomSoundExtList[0]); i++)
	{
		if (!strstr(pFileName, gPlayerCustomSoundExtList[i]))
			continue;

		iSoundType = (CustomSoundType)i;
		break;
	}

	// File extension validation
	if (iSoundType == CustomSoundType::MAX_CUSTOM_SOUND_TYPES)
		return NULL;

	// Convert backward slashes to forward slashes
	while ((tmp = strrchr(pFileBuffer, '\\')) != NULL)
		*tmp = '/';

	// Remove path
	if ((tmp = strrchr(pFileBuffer, '/')) != NULL)
	{
		strcpy(pBuffer, tmp + 1);
		strcpy_s(pFileBuffer, sizeof(pFileBuffer), pBuffer);
	}

	// Remove file extension
	if ((tmp = strrchr(pFileBuffer, '.')) != NULL)
		*tmp = NULL;

	// Calculate the CRC for this file path.
	unsigned int iFileCrc = StringCRC32((void*)pFileBuffer, strlen(pFileBuffer));

	// Look for it
	for (int i = 0; i < mCustomSoundCount; i++)
	{
		// Skip mismatches
		if (mCustomSoundList[i].file_path_crc != iFileCrc)
			continue;

		// Found it!
		return &mCustomSoundList[i];
	}

	// We didn't find it; If we can't allocate a new sound, then we should be stopping here.
	if (!bCreate || mCustomSoundCount + 1 >= MAX_CUSTOMPLAYER_CUSTOM_SOUNDS)
		return NULL;

	// We didn't find it; Allocate a new sound.
	if (!mCustomSoundList)
	{
		mCustomSoundCount = 1;
		mCustomSoundList  = (CustomSound*)malloc(sizeof(CustomSound) * mCustomSoundCount);
	}
	else
	{
		mCustomSoundCount += 1;
		mCustomSoundList   = (CustomSound*)realloc((void*)mCustomSoundList, sizeof(CustomSound) * mCustomSoundCount);
	}

	// Get the newly allocated sound
	CustomSound* pNewSound = &mCustomSoundList[mCustomSoundCount - 1];

	// Configure the newly allocated frame
	pNewSound->file_path       = new char[strlen(pFileBuffer) + 1];
	pNewSound->type            = iSoundType;
	pNewSound->file_path_crc   = iFileCrc;
	pNewSound->custom_sound_id = -1;

	// Copy the string
	strcpy(pNewSound->file_path, pFileBuffer);

#ifdef _CAVESTORY_MULTIPLAYER
	if (mIsLoaded)
	{
		strcpy(pBuffer, mPath);
		strcat(pBuffer, "/");
		strcat(pBuffer, pNewSound->file_path);
		strcat(pBuffer, gPlayerCustomSoundExtList[pNewSound->type]);

		pNewSound->custom_sound_id = gPlayerCustomSoundFuncList[pNewSound->type].load_sound(pBuffer);
	}
#endif

	// Done!
	return pNewSound;
}

bool CustomPlayer::RemoveCustomSound(const char* pFileName)
{
	// Validate the filepath
	char pBuffer[2048];
	char pFileBuffer[480];
	strcpy_s(pFileBuffer, sizeof(pFileBuffer), pFileName);
	char* tmp = NULL;

	// Convert backward slashes to forward slashes
	while ((tmp = strrchr(pFileBuffer, '\\')) != NULL)
		*tmp = '/';

	// Remove path
	if ((tmp = strrchr(pFileBuffer, '/')) != NULL)
	{
		strcpy(pBuffer, tmp + 1);
		strcpy_s(pFileBuffer, sizeof(pFileBuffer), pBuffer);
	}

	// Remove file extension
	if ((tmp = strrchr(pFileBuffer, '.')) != NULL)
		*tmp = NULL;

	// Calculate the CRC of the file name
	unsigned int iFileCrc = StringCRC32((void*)pFileBuffer, strlen(pFileBuffer));
	int i;

	// Look for it
	for (i = 0; i < mCustomSoundCount; i++)
	{
		// Skip mismatches
		if (mCustomSoundList[i].file_path_crc != iFileCrc)
			continue;

		// Free its stuff
		delete[] mCustomSoundList[i].file_path;

#ifdef _CAVESTORY_MULTIPLAYER
		if (mCustomSoundList[i].custom_sound_id != -1)
			gPlayerCustomSoundFuncList[mCustomSoundList[i].type].free_sound(mCustomSoundList[i].custom_sound_id);

		// Remove it from the list
		LIST_ERASE(mCustomSoundList, mCustomSoundCount, CustomFrame, i);
#endif

		--mCustomSoundList;

		if (mCustomSoundCount <= 0)
		{
			free(mCustomSoundList);
			mCustomSoundList = NULL;
		}
		else
		{
			CustomSound* pNewBuffer = (CustomSound*)malloc(sizeof(CustomSound) * mCustomSoundCount);

			// Copy the old contents over to the new buffer
			memcpy(pNewBuffer, mCustomSoundList, sizeof(CustomFrame) * mCustomSoundCount);

			// Free the old list
			free(mCustomSoundList);

			// Set the new buffer
			mCustomSoundList = pNewBuffer;
		}

		// Done!
		return true;
	}

	// Couldn't find the frame...
	return false;
}

void CustomPlayer::ClearCustomSounds()
{
	if (!mCustomSoundList)
		return;

	for (int i = 0; i < mCustomSoundCount; i++)
	{
		if (mCustomSoundList[i].file_path)
			delete[] mCustomSoundList[i].file_path;

#ifdef _CAVESTORY_MULTIPLAYER
		if (mCustomSoundList[i].custom_sound_id != -1)
			gPlayerCustomSoundFuncList[mCustomSoundList[i].type].free_sound(mCustomSoundList[i].custom_sound_id);
#endif
	}

	free(mCustomSoundList);
	mCustomSoundList  = NULL;
	mCustomSoundCount = 0;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CustomPlayer::IsPixelSolid(PlayerAnimFrame iFrame, int iX, int iY)
{
#ifdef _CAVESTORY_MULTIPLAYER
	if (iFrame < 0 || iFrame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES ||!mIsLoaded || mNormalSurfaceId == SURFACE_ID_MAX)
		return false;

	// Get the sprite dimensions
	int iSpriteW = (mFrameList[iFrame].right - mFrameList[iFrame].left);
	int iSpriteH = (mFrameList[iFrame].bottom - mFrameList[iFrame].top);

	// If it's already ready, then just return the result.
	if (mBitPlanes[iFrame].ready)
	{
		if (iX < 0 || iX >= iSpriteW || iY < 0 || iY >= iSpriteH)
			return false;

		return (mBitPlanes[iFrame].bits[((iY * iSpriteW) + iX) / 8] & (1 << (((iY * iSpriteW) + iX) % 8))) != 0;
	}

	int iPenX = mFrameList[iFrame].left;
	int iPenY = mFrameList[iFrame].top;

	// Allocate the buffer
	mBitPlanes[iFrame].bits  = new unsigned char[((iSpriteW * iSpriteH) + 7) >> 3];
	mBitPlanes[iFrame].ready = true;

	// Zero-mem it
	memset(mBitPlanes[iFrame].bits, 0, ((iSpriteW * iSpriteH) + 7) >> 3);

	// Loop through the chunk to process it
	for (int y = 0; y < iSpriteH; y++)
	{
		for (int x = 0; x < iSpriteW; x++)
		{
			unsigned long iAlpha = CacheSurface::GetPixelColor(GUI_POINT(iPenX + x, iPenY + y), (Surface_Ids)mNormalSurfaceId).a;

			if (iAlpha < 127)
				continue;

			int iPixelIndex                           = (y * iSpriteW) + x;
			mBitPlanes[iFrame].bits[iPixelIndex / 8] |= (1 << (iPixelIndex % 8));
		}
	}

	int iBitIndex = ((iY * iSpriteW) + iX);

	// Determine if this is out of bounds
	if ((iBitIndex / 8) >= ((iSpriteW * iSpriteH) + 7) >> 3)
		return false;

	// Done!
	return mBitPlanes[iFrame].bits[iBitIndex / 8] & (1 << (iBitIndex % 8)) != 0;
#else
	return false;
#endif
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CustomPlayer::LoadAssets(bool bShowWarnings)
{
#ifdef _CAVESTORY_MULTIPLAYER
	if (mIsLoaded)
		return true;

	Surface_Ids iOldActive = CacheSurface::GetSurfaceID();

	char pNormalPath[260];
	char pMaskPath[260];
	char pFacePath[260];
	char pFaceMaskPath[260];

	// Build the buffers
	sprintf_s(pNormalPath,		sizeof(pNormalPath),	"%s/Normal.png", mPath);
	sprintf_s(pMaskPath,		sizeof(pMaskPath),		"%s/Mask.png", mPath);
	sprintf_s(pFacePath,		sizeof(pFacePath),		"%s/Face.png", mPath);
	sprintf_s(pFaceMaskPath,	sizeof(pFaceMaskPath),	"%s/FaceMask.png", mPath);

	// Get the next surface IDs for each asset
	if ((mNormalSurfaceId = CacheSurface::GetNextFreeSurfaceID(0)) == SURFACE_ID_MAX)
	{
		printf("ERROR: " __FUNCTION__ "() - Failed to fetch a free surface slot for file '%s' (Out of allocatable surface space!).\r\n", pNormalPath);

		return false;
	}

	if ((mMaskSurfaceId = CacheSurface::GetNextFreeSurfaceID(1)) == SURFACE_ID_MAX)
	{
		printf("ERROR: " __FUNCTION__ "() - Failed to fetch a free surface slot for file '%s' (Out of allocatable surface space!).\r\n", pMaskPath);

		return false;
	}

	if ((mFaceSurfaceId = CacheSurface::GetNextFreeSurfaceID(2)) == SURFACE_ID_MAX)
	{
		printf("ERROR: " __FUNCTION__ "() - Failed to fetch a free surface slot for file '%s' (Out of allocatable surface space!).\r\n", pFacePath);
		return false;
	}

	if ((mFaceMaskSurfaceId = CacheSurface::GetNextFreeSurfaceID(3)) == SURFACE_ID_MAX)
	{
		printf("ERROR: " __FUNCTION__ "() - Failed to fetch a free surface slot for file '%s' (Out of allocatable surface space!).\r\n", pFaceMaskPath);
		return false;
	}

	// Set the surface ID
	CacheSurface::SetSurfaceID((Surface_Ids)mNormalSurfaceId);

	// Attempt to load it
	if (!CacheSurface::LoadFileToSurface(pNormalPath))
	{
		printf("ERROR: " __FUNCTION__ "() - Failed to load file \"%s\" (File could not be read).", pNormalPath);
		return false;
	}

	// Set the surface ID
	CacheSurface::SetSurfaceID((Surface_Ids)mMaskSurfaceId);

	// Attempt to load it
	if (!CacheSurface::LoadFileToSurface(pMaskPath))
	{
		printf("ERROR: " __FUNCTION__ "() - Failed to load file \"%s\" (File could not be read).", pMaskPath);

		// Couldn't do it; Unload the previous assets
		CacheSurface::FreeSurfaceID((Surface_Ids)mNormalSurfaceId);
		return false;
	}

	// Set the surface ID
	CacheSurface::SetSurfaceID((Surface_Ids)mFaceSurfaceId);

	// Attempt to load it
	if (!CacheSurface::LoadFileToSurface(pFacePath))
	{
		printf("ERROR: " __FUNCTION__ "() - Failed to load file \"%s\" (File could not be read).", pFacePath);

		// Couldn't do it; Unload the previous assets
		CacheSurface::FreeSurfaceID((Surface_Ids)mNormalSurfaceId);
		CacheSurface::FreeSurfaceID((Surface_Ids)mMaskSurfaceId);
		return false;
	}
	
	// Set the surface ID
	CacheSurface::SetSurfaceID((Surface_Ids)mFaceMaskSurfaceId);

	// Attempt to load it
	if (!CacheSurface::LoadFileToSurface(pFaceMaskPath))
	{
		printf("ERROR: " __FUNCTION__ "() - Failed to load file \"%s\" (File could not be read).", pFaceMaskPath);

		// Couldn't do it; Unload the previous assets
		CacheSurface::FreeSurfaceID((Surface_Ids)mNormalSurfaceId);
		CacheSurface::FreeSurfaceID((Surface_Ids)mMaskSurfaceId);
		CacheSurface::FreeSurfaceID((Surface_Ids)mFaceSurfaceId);
		return false;
	}

	// Set it back to something else
	CacheSurface::SetSurfaceID(iOldActive);

	// Validate all surfaces
	if (bShowWarnings)
	{
		char pTmp[1024];
		char pMsg[1024];

		strcpy(pMsg, "WARNING:\n \n");

		if (CacheSurface::GetSurfaceIDSize((Surface_Ids)mNormalSurfaceId) != CacheSurface::GetSurfaceIDSize((Surface_Ids)mMaskSurfaceId))
		{
			strcat(pMsg, "---------------------------\n");
			sprintf(pTmp, "Normal.png's image size does not match Mask.png's image size.\nNormal.png -> %dx%d\nMask.png -> %dx%d\nPlease ensure these images are the same width & height!\n \n",
				CacheSurface::GetSurfaceIDSize((Surface_Ids)mNormalSurfaceId).x, CacheSurface::GetSurfaceIDSize((Surface_Ids)mNormalSurfaceId).y,
				CacheSurface::GetSurfaceIDSize((Surface_Ids)mMaskSurfaceId).x, CacheSurface::GetSurfaceIDSize((Surface_Ids)mMaskSurfaceId).y);
			strcat(pMsg, pTmp);
		}
		
		if (CacheSurface::GetSurfaceIDSize((Surface_Ids)mFaceSurfaceId) != CacheSurface::GetSurfaceIDSize((Surface_Ids)mFaceMaskSurfaceId))
		{
			strcat(pMsg, "---------------------------\n");
			sprintf(pTmp, "Face.png's image size does not match FaceMask.png's image size.\nFace.png -> %dx%d\nFaceMask.png -> %dx%d\nPlease ensure these images are the same width & height!\n \n",
				CacheSurface::GetSurfaceIDSize((Surface_Ids)mFaceSurfaceId).x, CacheSurface::GetSurfaceIDSize((Surface_Ids)mFaceSurfaceId).y,
				CacheSurface::GetSurfaceIDSize((Surface_Ids)mFaceMaskSurfaceId).x, CacheSurface::GetSurfaceIDSize((Surface_Ids)mFaceMaskSurfaceId).y);
			strcat(pMsg, pTmp);
		}

		// Show the error
		if (strcmp(pMsg, "WARNING:\n \n"))
		{
			int iResult = MessageBoxMode::ShowMessageBox("Warning - Character Editor", pMsg, Default_MessageBox_ButtonLayouts::LAYOUT_OK, 1);

			if (iResult == MessageBoxMode::MessageBoxReturnCodes::QUIT_GAME && BaseModeInstance::m_FocusedInstance)
				BaseModeInstance::m_FocusedInstance->EndMode(0);
		}
	}

	// Load the custom sounds
	for (int i = 0; i < mCustomSoundCount; i++)
	{
		strcpy(pNormalPath, mPath);
		strcat(pNormalPath, "/");
		strcat(pNormalPath, mCustomSoundList[i].file_path);
		strcat(pNormalPath, gPlayerCustomSoundExtList[mCustomSoundList[i].type]);

		mCustomSoundList[i].custom_sound_id = gPlayerCustomSoundFuncList[mCustomSoundList[i].type].load_sound(pNormalPath);
	}

	mIsLoaded = true;

	return true;
#else
	return true;
#endif
}

bool CustomPlayer::UnloadAssets()
{
#ifdef _CAVESTORY_MULTIPLAYER
	if (!mIsLoaded)
		return true;

	// Unload the surface ids
	CacheSurface::FreeSurfaceID((Surface_Ids)mNormalSurfaceId);
	CacheSurface::FreeSurfaceID((Surface_Ids)mMaskSurfaceId);
	CacheSurface::FreeSurfaceID((Surface_Ids)mFaceSurfaceId);
	CacheSurface::FreeSurfaceID((Surface_Ids)mFaceMaskSurfaceId);

	// Unload the custom sounds
	for (int i = 0; i < mCustomSoundCount; i++)
	{
		// Skip unloaded sounds
		if (mCustomSoundList[i].custom_sound_id == -1)
			continue;

		gPlayerCustomSoundFuncList[mCustomSoundList[i].type].free_sound(mCustomSoundList[i].custom_sound_id);
		mCustomSoundList[i].custom_sound_id = -1;
	}

	mNormalSurfaceId   = 0;
	mMaskSurfaceId     = 0;
	mFaceSurfaceId     = 0;
	mFaceMaskSurfaceId = 0;
	mIsLoaded          = false;

	for (int i = 0; i < PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES; i++)
	{
		if (mBitPlanes[i].ready && mBitPlanes[i].bits)
			delete[] mBitPlanes[i].bits;
	}

	memset(mBitPlanes, 0, sizeof(mBitPlanes));

	return true;
#else
	return true;
#endif
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------