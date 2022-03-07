#pragma once

#if !defined(_CHAR_EDITOR) && !defined(_CAVEEDITOR) && !defined(NO_WINDOWS)
#include "WindowsWrapper.h"
#else
#include <Windows.h>
#endif

#ifdef _CAVESTORY_MULTIPLAYER
#include "Sound.h"
#endif

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct CUSTOM_PLAYER_ANIMATION_INFO;
class CustomPlayerAnimation;
class CustomPlayer;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAX_CUSTOMPLAYER_CUSTOM_SOUNDS		64
#define MAX_CUSTOMPLAYER_CUSTOM_FRAMES		64
#define NULL 0

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _CAVEEDITOR
/// Animation information for CustomPlayerAnimation.
///
struct CUSTOM_PLAYER_ANIMATION_INFO
{
	/// The current animation type.
	///
	int current_anim_type;

	/// This player's current animation frame.
	///
	int curr_anim;

	/// The expire point of the current delay that must be passed in order for frame_no to continue ticking.
	///
	unsigned int delay_expire;

	/// Current frame number.
	///
	unsigned int frame_no;

	/// Current animation frame.
	///
	void* current_frame_ptr;
};
#endif

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#if defined(_CHAR_EDITOR) || defined(_CAVEEDITOR)
#define BEGIN_ANIMATION_LIST()								static struct\
															{\
															RECT DefaultRect;\
															const char* Name;\
															} gAnimFrames[] = { { { 0, 0, 0, 0 }, "FRAME_INVALID" },
#define DEFINE_ANIMATION_FRAME(X, Y, W, H, AFR_NAME)			{ { X, Y, X + W, Y + H }, "FRAME_" #AFR_NAME },
#define FINISH_ANIMATION_LIST()								}; \
															enum PlayerAnimFrame { MAX_PLAYER_ANIM_FRAMES = (sizeof(gAnimFrames) / sizeof(gAnimFrames[0])) };

#define BEGIN_FACE_LIST()									static struct\
															{\
																int DefaultX;\
																int DefaultY;\
																const char* FaceName;\
															} gAnimFaces[] = { { 0, 0, "FACE_INVALID" },
#define DEFINE_FACE_FRAME(X, Y, AFR_NAME)		{ X, Y, "FACE_" #AFR_NAME },
#define FINISH_FACE_LIST()									}; enum PlayerFaceFrame { MAX_PLAYER_FACES = (sizeof(gAnimFaces) / sizeof(gAnimFaces[0])) };
#else
#define BEGIN_ANIMATION_LIST()								enum PlayerAnimFrame : unsigned short { FRAME_INVALID = 0,
#define DEFINE_ANIMATION_FRAME(X, Y, W, H, AFR_NAME)			FRAME_##AFR_NAME,
#define FINISH_ANIMATION_LIST()								MAX_PLAYER_ANIM_FRAMES };

#define BEGIN_FACE_LIST()									enum PlayerFaceFrame : unsigned short { FACE_INVALID = 0,
#define DEFINE_FACE_FRAME(X, Y, AFR_NAME)						FACE_##AFR_NAME,
#define FINISH_FACE_LIST()									MAX_PLAYER_FACES };
#endif

#define DEFINE_ANIMATION(ANIM_NAME)							ANIMATION_##ANIM_NAME,

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// DEFINE_FACE_FRAME(<Inherit From -- Will copy from this face's index if version is lower
BEGIN_FACE_LIST()
	DEFINE_FACE_FRAME(0, 0, NORMAL)
	DEFINE_FACE_FRAME(48, 0, HURT)
	DEFINE_FACE_FRAME(96, 0, LOW_HEALTH)
	DEFINE_FACE_FRAME(144, 0, DEAD)

	DEFINE_FACE_FRAME(0, 0, MIMIGA_NORMAL)
	DEFINE_FACE_FRAME(48, 0, MIMIGA_HURT)
	DEFINE_FACE_FRAME(96, 0, MIMIGA_LOW_HEALTH)
	DEFINE_FACE_FRAME(144, 0, MIMIGA_DEAD)
FINISH_FACE_LIST()

BEGIN_ANIMATION_LIST()
	DEFINE_ANIMATION_FRAME(0, 0, 16, 16, IDLE)
	DEFINE_ANIMATION_FRAME(16, 0, 16, 16, WALK_1)
	DEFINE_ANIMATION_FRAME(32, 0, 16, 16, WALK_2)
	DEFINE_ANIMATION_FRAME(48, 0, 16, 16, LOOK_UP)
	DEFINE_ANIMATION_FRAME(64, 0, 16, 16, LOOK_UP_WALK_1)
	DEFINE_ANIMATION_FRAME(80, 0, 16, 16, LOOK_UP_WALK_2)
	DEFINE_ANIMATION_FRAME(96, 0, 16, 16, LOOK_DOWN)
	DEFINE_ANIMATION_FRAME(112, 0, 16, 16, SEARCH)
	DEFINE_ANIMATION_FRAME(128, 0, 16, 16, SITTING)

	DEFINE_ANIMATION_FRAME(144, 0, 16, 16, COLLAPSED)
	DEFINE_ANIMATION_FRAME(160, 0, 16, 16, FALLING_UPSIDE_DOWN)
	DEFINE_ANIMATION_FRAME(176, 0, 16, 16, TACKLE_FLY_FORWARD)
	DEFINE_ANIMATION_FRAME(192, 0, 16, 16, TACKLE_ON_GROUND)
	DEFINE_ANIMATION_FRAME(208, 0, 16, 16, TACKLE_ON_GROUND_CHARGE)
	DEFINE_ANIMATION_FRAME(224, 0, 16, 16, TACKLE_ON_GROUND_LOOK_UP)
	DEFINE_ANIMATION_FRAME(240, 0, 16, 16, TACKLE_ON_GROUND_LOOK_UP_CHARGE)
	DEFINE_ANIMATION_FRAME(256, 0, 16, 16, FLY_DEAD_BACKWARDS)
	DEFINE_ANIMATION_FRAME(272, 0, 16, 16, FLY_DEAD_LOOK_DOWN)
	DEFINE_ANIMATION_FRAME(288, 0, 16, 16, FLY_DEAD_LOOK_UP)
	DEFINE_ANIMATION_FRAME(304, 0, 16, 16, FLY_DEAD_FORWARD)

	DEFINE_ANIMATION_FRAME(320, 0, 16, 16, CARRIED_DEAD)
	DEFINE_ANIMATION_FRAME(336, 0, 16, 16, CARRIED_IDLE)
	DEFINE_ANIMATION_FRAME(352, 0, 16, 16, CARRIED_LOOK_UP)
	DEFINE_ANIMATION_FRAME(368, 0, 16, 16, CARRIED_LOOK_DOWN)

	DEFINE_ANIMATION_FRAME(0, 64, 23, 13, FISH_LEFT)
	DEFINE_ANIMATION_FRAME(0, 77, 23, 13, FISH_RIGHT)
	DEFINE_ANIMATION_FRAME(0, 90, 22, 13, FISH_MIMIGAMASK_LEFT)
	DEFINE_ANIMATION_FRAME(1, 103, 22, 13, FISH_MIMIGAMASK_RIGHT)

	DEFINE_ANIMATION_FRAME(23, 64, 11, 17, RIDE_DRAGON_1)
	DEFINE_ANIMATION_FRAME(34, 64, 11, 17, RIDE_DRAGON_2)
	DEFINE_ANIMATION_FRAME(23, 81, 11, 17, RIDE_DRAGON_MASK_1)
	DEFINE_ANIMATION_FRAME(34, 81, 11, 17, RIDE_DRAGON_MASK_2)

	DEFINE_ANIMATION_FRAME(384, 0, 8, 8,  STAR_1)
	DEFINE_ANIMATION_FRAME(384, 8, 8, 8,  STAR_2)
	DEFINE_ANIMATION_FRAME(384, 16, 8, 8, STAR_3)

	DEFINE_ANIMATION_FRAME(16, 0, 16, 16, JUMP)
	DEFINE_ANIMATION_FRAME(64, 0, 16, 16, JUMP_LOOK_UP)
	DEFINE_ANIMATION_FRAME(96, 0, 16, 16, JUMP_LOOK_DOWN)

	DEFINE_ANIMATION_FRAME(32, 0, 16, 16, FALL)
	DEFINE_ANIMATION_FRAME(80, 0, 16, 16, FALL_LOOK_UP)
	DEFINE_ANIMATION_FRAME(96, 0, 16, 16, FALL_LOOK_DOWN)
FINISH_ANIMATION_LIST();

enum PlayerAnimation
{
	DEFINE_ANIMATION(IDLE)
	DEFINE_ANIMATION(IDLE_SHOOT)
	DEFINE_ANIMATION(IDLE_LOOK_UP)
	DEFINE_ANIMATION(IDLE_LOOK_UP_SHOOT)
	
	// Animation Group "N/A"
	DEFINE_ANIMATION(SEARCH)
	
	// Animation Group "WALKING"
	DEFINE_ANIMATION(WALK)
	DEFINE_ANIMATION(WALK_SHOOT)
	DEFINE_ANIMATION(WALK_LOOK_UP)
	DEFINE_ANIMATION(WALK_LOOK_UP_SHOOT)
	
	// Animation Group "FALLING"
	DEFINE_ANIMATION(FALL)
	DEFINE_ANIMATION(FALL_SHOOT)
	DEFINE_ANIMATION(FALL_LOOK_UP)
	DEFINE_ANIMATION(FALL_LOOK_UP_SHOOT)
	DEFINE_ANIMATION(FALL_LOOK_DOWN)
	DEFINE_ANIMATION(FALL_LOOK_DOWN_SHOOT)
	
	// Animation Group "JUMPING"
	DEFINE_ANIMATION(JUMP)
	DEFINE_ANIMATION(JUMP_SHOOT)
	DEFINE_ANIMATION(JUMP_LOOK_UP)
	DEFINE_ANIMATION(JUMP_LOOK_UP_SHOOT)
	DEFINE_ANIMATION(JUMP_LOOK_DOWN)
	DEFINE_ANIMATION(JUMP_LOOK_DOWN_SHOOT)
	
	// Animation Group "N/A"
	DEFINE_ANIMATION(FISH_LEFT_IDLE)
	DEFINE_ANIMATION(FISH_RIGHT_IDLE)
	DEFINE_ANIMATION(LAND_ON_GROUND)
	
	// Animation Group "CARRIED"
	DEFINE_ANIMATION(CARRIED_IDLE)
	DEFINE_ANIMATION(CARRIED_IDLE_SHOOT)
	DEFINE_ANIMATION(CARRIED_WALKING)
	DEFINE_ANIMATION(CARRIED_WALKING_SHOOT)
	DEFINE_ANIMATION(CARRIED_LOOK_UP)
	DEFINE_ANIMATION(CARRIED_LOOK_UP_SHOOT)
	DEFINE_ANIMATION(CARRIED_LOOK_DOWN)
	DEFINE_ANIMATION(CARRIED_LOOK_DOWN_SHOOT)
	
	// Animation Group "CARRIED_DEAD"
	DEFINE_ANIMATION(CARRIED_DEAD_IDLE)
	DEFINE_ANIMATION(CARRIED_DEAD_WALKING)
	
	// Animation Group "N/A"
	DEFINE_ANIMATION(WHIMSICAL_STAR_1)
	DEFINE_ANIMATION(WHIMSICAL_STAR_2)
	DEFINE_ANIMATION(WHIMSICAL_STAR_3)
	DEFINE_ANIMATION(TACKLE_HIT_WALL)
	DEFINE_ANIMATION(TACKLE_POUNCE)
	DEFINE_ANIMATION(TACKLE_CHARGE)
	DEFINE_ANIMATION(TACKLE_CHARGE_LOOK_UP)
	DEFINE_ANIMATION(TACKLE_LAND_ON_GROUND)
	DEFINE_ANIMATION(TACKLE_SLIDING_ON_GROUND)
	DEFINE_ANIMATION(TACKLE_STAND_UP)
	DEFINE_ANIMATION(COLLAPSED_ON_GROUND)
	DEFINE_ANIMATION(STUNNED_FLY_FORWARD)
	DEFINE_ANIMATION(STUNNED_FLY_UP)
	DEFINE_ANIMATION(STUNNED_FLY_DOWN)
	DEFINE_ANIMATION(STUNNED_FLY_BACKWARD)
	DEFINE_ANIMATION(DROWNED)
	DEFINE_ANIMATION(RIDING_DRAGON)
	
	// Animation Group "IDLE"
	DEFINE_ANIMATION(MASK_IDLE)
	DEFINE_ANIMATION(MASK_IDLE_SHOOT)
	DEFINE_ANIMATION(MASK_IDLE_LOOK_UP)
	DEFINE_ANIMATION(MASK_IDLE_LOOK_UP_SHOOT)
	
	// Animation Group "N/A"
	DEFINE_ANIMATION(MASK_SEARCH)
	
	// Animation Group "WALKING"
	DEFINE_ANIMATION(MASK_WALK)
	DEFINE_ANIMATION(MASK_WALK_SHOOT)
	DEFINE_ANIMATION(MASK_WALK_LOOK_UP)
	DEFINE_ANIMATION(MASK_WALK_LOOK_UP_SHOOT)
	
	// Animation Group "FALLING"
	DEFINE_ANIMATION(MASK_FALL)
	DEFINE_ANIMATION(MASK_FALL_SHOOT)
	DEFINE_ANIMATION(MASK_FALL_LOOK_UP)
	DEFINE_ANIMATION(MASK_FALL_LOOK_UP_SHOOT)
	DEFINE_ANIMATION(MASK_FALL_LOOK_DOWN)
	DEFINE_ANIMATION(MASK_FALL_LOOK_DOWN_SHOOT)
	
	// Animation Group "JUMPING"
	DEFINE_ANIMATION(MASK_JUMP)
	DEFINE_ANIMATION(MASK_JUMP_SHOOT)
	DEFINE_ANIMATION(MASK_JUMP_LOOK_UP)
	DEFINE_ANIMATION(MASK_JUMP_LOOK_UP_SHOOT)
	DEFINE_ANIMATION(MASK_JUMP_LOOK_DOWN)
	DEFINE_ANIMATION(MASK_JUMP_LOOK_DOWN_SHOOT)
	
	// Animation Group "N/A"
	DEFINE_ANIMATION(MASK_FISH_LEFT_IDLE)
	DEFINE_ANIMATION(MASK_FISH_RIGHT_IDLE)
	DEFINE_ANIMATION(MASK_LAND_ON_GROUND)
	
	// Animation Group "CARRIED"
	DEFINE_ANIMATION(MASK_CARRIED_IDLE)
	DEFINE_ANIMATION(MASK_CARRIED_IDLE_SHOOT)
	DEFINE_ANIMATION(MASK_CARRIED_WALKING)
	DEFINE_ANIMATION(MASK_CARRIED_WALKING_SHOOT)
	DEFINE_ANIMATION(MASK_CARRIED_LOOK_UP)
	DEFINE_ANIMATION(MASK_CARRIED_LOOK_UP_SHOOT)
	DEFINE_ANIMATION(MASK_CARRIED_LOOK_DOWN)
	DEFINE_ANIMATION(MASK_CARRIED_LOOK_DOWN_SHOOT)
	
	// Animation Group "CARRIED_DEAD"
	DEFINE_ANIMATION(MASK_CARRIED_DEAD_IDLE)
	DEFINE_ANIMATION(MASK_CARRIED_DEAD_WALKING)
	
	// Animation Group "N/A"
	DEFINE_ANIMATION(MASK_TACKLE_HIT_WALL)
	DEFINE_ANIMATION(MASK_TACKLE_POUNCE)
	DEFINE_ANIMATION(MASK_TACKLE_CHARGE)
	DEFINE_ANIMATION(MASK_TACKLE_CHARGE_LOOK_UP)
	DEFINE_ANIMATION(MASK_TACKLE_LAND_ON_GROUND)
	DEFINE_ANIMATION(MASK_TACKLE_SLIDING_ON_GROUND)
	DEFINE_ANIMATION(MASK_TACKLE_STAND_UP)
	DEFINE_ANIMATION(MASK_COLLAPSED_ON_GROUND)
	DEFINE_ANIMATION(MASK_STUNNED_FLY_FORWARD)
	DEFINE_ANIMATION(MASK_STUNNED_FLY_UP)
	DEFINE_ANIMATION(MASK_STUNNED_FLY_DOWN)
	DEFINE_ANIMATION(MASK_STUNNED_FLY_BACKWARD)
	DEFINE_ANIMATION(MASK_DROWNED)
	DEFINE_ANIMATION(MASK_RIDING_DRAGON)
	
	MAX_PLAYER_ANIMATIONS
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CRCs for all action names

enum PlayerAnimFlags
{
	PAF_UNIFORM_HEIGHT      = 0x00000001,
	PAF_HIDE_HITBOX         = 0x00000002,
	PAF_HIDE_VIEWBOX        = 0x00000004,
	PAF_SHOW_WEAPON         = 0x00000008,
	PAF_SPECIFIC_DIRECTION  = 0x00000010,
	PAF_FORCE_UP            = 0x00000020,
	PAF_FORCE_LEFT          = 0x00000040,
	PAF_FORCE_RIGHT         = 0x00000080,
	PAF_FORCE_DOWN          = 0x00000100,
	PAF_FISHING             = 0x00000200,
	PAF_MIMIGA_MASK         = 0x00000400,
	PAF_SHOW_HELD_ENTITY    = 0x00000800,
};

enum PlayerCustomAnimFlags
{
	PCAF_MIMIGA_MASK = 0x00000001, //<< If set, then this animation will use mimiga mask frames.
	PCAF_LOOP        = 0x00000002, //<< If set, then this animation will loop.
	PCAF_FINISH      = 0x00000004  //<< If set, then this animation will be required to finish before it can switch to another animation (unless the other animation has a different group than this animation OR if priority is higher than the currently playing animation.)
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _CAVESTORY_MULTIPLAYER
// todo: store these in an easily editable file
static struct
{
	const char* name;
	unsigned int sound_id;
} gPlayerSoundInfoList[] =
{
	// 0
	{ "Player Hits Head",			SOUND_IDS::sou_HitHead			},
	{ "Player Changes Weapon",		SOUND_IDS::sou_ChangeWeapon		},
	{ "Player Jumps",				SOUND_IDS::sou_QuoteJump		},
	{ "Player Takes Damage",		SOUND_IDS::sou_TakeDamage		},
	{ "Player Dies",				SOUND_IDS::sou_QuoteDie			},

	// 5
	{ "Player Footstep",			SOUND_IDS::sou_Footstep			},
	{ "Player Lands on Ground",		SOUND_IDS::sou_LandOnGround		},
	{ "Player Hits Wall (Agility)",	SOUND_IDS::sou_TackleHitWall	},
	{ "Ting",						SOUND_IDS::sou_Ting				},
	{ "Critter Jump (Small)",		SOUND_IDS::sou_SmallJump		},

	// 10
	{ "Critter Jump (Medium)",		SOUND_IDS::sou_MediumJump		},
	{ "Critter Fly",				SOUND_IDS::sou_CritterFly		},
	{ "Critter Fly (Medium)",		SOUND_IDS::sou_CritterFlyMed	},
	{ "Computer Beep",				SOUND_IDS::sou_Computer			},
	{ "Bubbler Pop",				SOUND_IDS::sou_Bubblin_Pop		},

	// 15
	{ "Squeak",						SOUND_IDS::sou_Squeak			},
	{ "Squeak 2",					SOUND_IDS::sou_Squeek			},
	{ "Gaudi Squeal",				SOUND_IDS::sou_Labyr_Bug_Squeal	},
	{ "Squeal",						SOUND_IDS::sou_Squeal			},
	{ "Roar",						SOUND_IDS::sou_Roar				},

	// 20
	{ "Thud (Loud)",				SOUND_IDS::sou_LoudThud			},
	{ "Thud",						SOUND_IDS::sou_Thud				},
	{ "Whud",						SOUND_IDS::sou_Whud				},
	{ "KAPOW!",						SOUND_IDS::sou_Kapow			},
	{ "Generic Explosion",			SOUND_IDS::sou_Explosion		},

	// 25
	{ "Tiny Explosion",				SOUND_IDS::sou_Tiny_Explosion	},
	{ "Small Explosion",			SOUND_IDS::sou_Small_Explosion	},
	{ "Custom Sound",				SOUND_IDS::sou_ExtraStart		}
};

#define MAX_PLAYER_SOUNDS (sizeof(gPlayerSoundInfoList) / sizeof(gPlayerSoundInfoList[0]))
#else
#define MAX_PLAYER_SOUNDS 0
#endif

// todo: store these in an easily editable file
static struct ANIM_FACE_INFO_STRUCT
{
	const char* name;
	unsigned char version;
	char inherit_from;
	unsigned int crc;
	unsigned int flags;
	struct
	{
		int x;
		int y;
	} default_pos;
} gAnimFaceInfo[] =
{
	{ "FACE_INVALID",           1, -1, 0x87735235, 0x00000000, { 0,    0    } },
	{ "FACE_NORMAL",            1, -1, 0x17426A0A, 0x00000000, { 0,    0    } },
	{ "FACE_HURT",              1, -1, 0xEEDF36C3, 0x00000000, { 48,   0    } },
	{ "FACE_LOW_HEALTH",        1, -1, 0x2096A3C7, 0x00000000, { 96,   0    } },
	{ "FACE_DEAD",              1, -1, 0xC4777BFD, 0x00000000, { 144,  0    } },
	{ "FACE_MIMIGA_NORMAL",     7,  1, 0x44A16A9E, 0x00000000, { 0,    0    } },
	{ "FACE_MIMIGA_HURT",       7,  2, 0x780AF8B3, 0x00000000, { 48,   0    } },
	{ "FACE_MIMIGA_LOW_HEALTH", 7,  3, 0x95857555, 0x00000000, { 96,   0    } },
	{ "FACE_MIMIGA_DEAD",       7,  4, 0x52A2B58D, 0x00000000, { 144,  0    } }
};

// todo: store these in an easily editable file
static struct ANIM_FRAME_INFO_STRUCT
{
	const char* name;
	unsigned int crc;
	unsigned int flags;
	RECT default_rect;
	struct
	{
		int x;
		int y;
	} default_carry;
	struct
	{
		int x;
		int y;
	} default_puppy;
} gAnimFrameInfo[] =
{
	{ "FRAME_INVALID",                         0xFF6D7A9E,	0x00000000,	{ 0,    0,    0,    0    },	{ 0,    0     },	{ 0,    0     } },
	{ "FRAME_IDLE",                            0xF312031D,	0x00000809,	{ 0,    0,    16,   16   },	{ 7,    3     },	{ 4,    10    } },
	{ "FRAME_WALK_1",                          0xB0E35C7B,	0x00000809,	{ 16,   0,    32,   16   },	{ 7,    4     },	{ 4,    11    } },
	{ "FRAME_WALK_2",                          0x29EA0DC1,	0x00000809,	{ 32,   0,    48,   16   },	{ 7,    4     },	{ 4,    11    } },
	{ "FRAME_LOOK_UP",                         0x86890702,	0x00000829,	{ 48,   0,    64,   16   },	{ 7,    3     },	{ 4,    10    } },
	{ "FRAME_LOOK_UP_WALK_1",                  0x0347D106,	0x00000829,	{ 64,   0,    80,   16   },	{ 7,    4     },	{ 4,    11    } },
	{ "FRAME_LOOK_UP_WALK_2",                  0x9A4E80BC,	0x00000829,	{ 80,   0,    96,   16   },	{ 7,    4     },	{ 4,    11    } },
	{ "FRAME_LOOK_DOWN",                       0x76CA10E4,	0x00000909,	{ 96,   0,    112,  16   },	{ 7,    4     },	{ 4,    11    } },
	{ "FRAME_SEARCH",                          0x9BD6D001,	0x00000809,	{ 112,  0,    128,  16   },	{ 7,    3     },	{ 4,    10    } },
	{ "FRAME_SITTING",                         0xEC4278FD,	0x00000801,	{ 128,  0,    144,  16   },	{ 7,    1     },	{ 4,    8     } },
	{ "FRAME_COLLAPSED",                       0x0828DFE2,	0x00000801,	{ 144,  0,    160,  16   },	{ 3,    2     },	{ 2,    3     } },
	{ "FRAME_FALLING_UPSIDE_DOWN",             0x2014F6D5,	0x00000001,	{ 160,  0,    176,  16   },	{ 0,    0     },	{ 16,   0     } },
	{ "FRAME_TACKLE_FLY_FORWARD",              0x2015FC6C,	0x00000801,	{ 176,  0,    192,  16   },	{ 7,    3     },	{ 4,    10    } },
	{ "FRAME_TACKLE_ON_GROUND",                0x6FF0850C,	0x00000801,	{ 192,  0,    208,  16   },	{ 7,    1     },	{ 4,    8     } },
	{ "FRAME_TACKLE_ON_GROUND_CHARGE",         0xFE0CDC26,	0x00000801,	{ 208,  0,    224,  16   },	{ 7,    1     },	{ 4,    8     } },
	{ "FRAME_TACKLE_ON_GROUND_LOOK_UP",        0x4A0131E4,	0x00000821,	{ 224,  0,    240,  16   },	{ 7,    1     },	{ 4,    8     } },
	{ "FRAME_TACKLE_ON_GROUND_LOOK_UP_CHARGE", 0xADA043D4,	0x00000821,	{ 240,  0,    256,  16   },	{ 7,    1     },	{ 4,    8     } },
	{ "FRAME_FLY_DEAD_BACKWARDS",              0x35678D8B,	0x00000801,	{ 256,  0,    272,  16   },	{ 7,    3     },	{ 4,    10    } },
	{ "FRAME_FLY_DEAD_LOOK_DOWN",              0x3E3688C4,	0x00000901,	{ 272,  0,    288,  16   },	{ 7,    3     },	{ 4,    11    } },
	{ "FRAME_FLY_DEAD_LOOK_UP",                0x2C61F4E0,	0x00000821,	{ 288,  0,    304,  16   },	{ 7,    3     },	{ 4,    11    } },
	{ "FRAME_FLY_DEAD_FORWARD",                0x498DDC7A,	0x00000801,	{ 304,  0,    320,  16   },	{ 7,    3     },	{ 4,    10    } },
	{ "FRAME_CARRIED_DEAD",                    0x55230E7A,	0x00000801,	{ 320,  0,    336,  16   },	{ 7,    3     },	{ 4,    10    } },
	{ "FRAME_CARRIED_IDLE",                    0x6422F24B,	0x00000809,	{ 336,  0,    352,  16   },	{ 7,    3     },	{ 4,    10    } },
	{ "FRAME_CARRIED_LOOK_UP",                 0x24A33A63,	0x00000829,	{ 352,  0,    368,  16   },	{ 7,    3     },	{ 4,    10    } },
	{ "FRAME_CARRIED_LOOK_DOWN",               0x5244A439,	0x00000909,	{ 368,  0,    384,  16   },	{ 7,    3     },	{ 4,    11    } },
	{ "FRAME_FISH_LEFT",                       0x65F960BA,	0x00000A51,	{ 0,    64,   23,   77   },	{ 15,   1     },	{ 12,   8     } },
	{ "FRAME_FISH_RIGHT",                      0xE76678A3,	0x00000A91,	{ 0,    77,   23,   90   },	{ 8,    1     },	{ 5,    8     } },
	{ "FRAME_FISH_MIMIGAMASK_LEFT",            0xA7DC937F,	0x00000E51,	{ 0,    90,   23,   103  },	{ 15,   1     },	{ 12,   8     } },
	{ "FRAME_FISH_MIMIGAMASK_RIGHT",           0x0CAA6B6F,	0x00000E91,	{ 1,    103,  24,   116  },	{ 8,    1     },	{ 5,    8     } },
	{ "FRAME_RIDE_DRAGON_1",                   0x7B202E4A,	0x00000856,	{ 23,   64,   34,   81   },	{ -9,   11    },	{ -11,  18    } },
	{ "FRAME_RIDE_DRAGON_2",                   0xE2297FF0,	0x00000856,	{ 34,   64,   45,   81   },	{ -9,   11    },	{ -11,  18    } },
	{ "FRAME_RIDE_DRAGON_MASK_1",              0xB0F3B6AA,	0x00000C56,	{ 23,   81,   34,   98   },	{ -9,   11    },	{ -11,  18    } },
	{ "FRAME_RIDE_DRAGON_MASK_2",              0x29FAE710,	0x00000C56,	{ 34,   81,   45,   98   },	{ -9,   11    },	{ -11,  18    } },
	{ "FRAME_STAR_1",                          0x09F8FAF6,	0x00000056,	{ 384,  0,    392,  8    },	{ 0,    0     },	{ 0,    0     } },
	{ "FRAME_STAR_2",                          0x90F1AB4C,	0x00000056,	{ 384,  8,    392,  16   },	{ 0,    0     },	{ 0,    0     } },
	{ "FRAME_STAR_3",                          0xE7F69BDA,	0x00000056,	{ 384,  16,   392,  24   },	{ 0,    0     },	{ 0,    0     } },
	{ "FRAME_JUMP",                            0xB9D557D0,	0x00000809,	{ 16,   0,    32,   16   },	{ 7,    4     },	{ 4,    11    } },
	{ "FRAME_JUMP_LOOK_UP",                    0x763B5EA7,	0x00000829,	{ 64,   0,    80,   16   },	{ 7,    4     },	{ 4,    11    } },
	{ "FRAME_JUMP_LOOK_DOWN",                  0xB70D41AA,	0x00000909,	{ 96,   0,    112,  16   },	{ 7,    4     },	{ 4,    11    } },
	{ "FRAME_FALL",                            0xE5368ECA,	0x00000809,	{ 32,   0,    48,   16   },	{ 7,    4     },	{ 4,    11    } },
	{ "FRAME_FALL_LOOK_UP",                    0xC8287E34,	0x00000829,	{ 80,   0,    96,   16   },	{ 7,    4     },	{ 4,    11    } },
	{ "FRAME_FALL_LOOK_DOWN",                  0xD60F06A8,	0x00000909,	{ 96,   0,    112,  16   },	{ 7,    4     },	{ 4,    11    } }
};

// todo: store these in an easily editable file
static struct
{
	char group_id;
	unsigned char priority;
	const char* name;
	unsigned int crc;
	unsigned int flags;
	unsigned int mask_anim_index;
	unsigned int subsitute_index;
	const char* desc;
} gPlayerAnimationInfo[MAX_PLAYER_ANIMATIONS] =
{
	/* 0    */ {  0,  0, "ANIMATION_IDLE",                          0x64C33FDB,	0x00000002,	0x00000033,	0x00000063,	"Played when the player is standing still." },
	/* 1    */ {  0,  2, "ANIMATION_IDLE_SHOOT",                    0x7D86333D,	0x00000004,	0x00000034,	0x00000000,	"Played when the player is standing still and shooting." },
	/* 2    */ {  0,  0, "ANIMATION_IDLE_LOOK_UP",                  0x37E21A1D,	0x00000002,	0x00000035,	0x00000063,	"Played when the player is looking up and standing still." },
	/* 3    */ {  0,  2, "ANIMATION_IDLE_LOOK_UP_SHOOT",            0xC5623652,	0x00000004,	0x00000036,	0x00000002,	"Played when the player is looking up and shooting while standing still." },
	/* 4    */ { -1,  1, "ANIMATION_SEARCH",                        0x420EF7D4,	0x00000002,	0x00000037,	0x00000063,	"Played when the player presses the interaction button (Down Arrow)." },
	/* 5    */ {  1,  1, "ANIMATION_WALK",                          0x3576F09B,	0x00000002,	0x00000038,	0x00000063,	"Played when the player is walking." },
	/* 6    */ {  1,  2, "ANIMATION_WALK_SHOOT",                    0x5E49BC74,	0x00000004,	0x00000039,	0x00000005,	"Played when the player is walking and shooting." },
	/* 7    */ {  1,  1, "ANIMATION_WALK_LOOK_UP",                  0x6B5E532F,	0x00000002,	0x0000003A,	0x00000063,	"Played when the player is walking while looking up." },
	/* 8    */ {  1,  2, "ANIMATION_WALK_LOOK_UP_SHOOT",            0xEB318C3A,	0x00000004,	0x0000003B,	0x00000007,	"Played when the player is walking, looking up and shooting." },
	/* 9    */ {  2,  1, "ANIMATION_FALL",                          0x43B755C2,	0x00000002,	0x0000003C,	0x00000063,	"Played when the player is in mid-air and is going down." },
	/* 10   */ {  2,  2, "ANIMATION_FALL_SHOOT",                    0x5382682C,	0x00000004,	0x0000003D,	0x00000009,	"Played when the player is falling and shooting." },
	/* 11   */ {  2,  1, "ANIMATION_FALL_LOOK_UP",                  0x98829F75,	0x00000002,	0x0000003E,	0x00000063,	"Played when the player is in mid-air, is going down and is looking up." },
	/* 12   */ {  2,  2, "ANIMATION_FALL_LOOK_UP_SHOOT",            0xB1C169D9,	0x00000004,	0x0000003F,	0x0000000B,	"Played when the player is in mid-air, looking up and shooting." },
	/* 13   */ {  2,  1, "ANIMATION_FALL_LOOK_DOWN",                0xE860FAA8,	0x00000002,	0x00000040,	0x00000063,	"Played when the player is in mid-air, is going down and is looking down." },
	/* 14   */ {  2,  2, "ANIMATION_FALL_LOOK_DOWN_SHOOT",          0x66347FF6,	0x00000004,	0x00000041,	0x0000000D,	"Played when the player is in mid-air, looking down and shooting." },
	/* 15   */ {  3,  1, "ANIMATION_JUMP",                          0x1F548CD8,	0x00000002,	0x00000042,	0x00000063,	"Played when the player is in mid-air and is going up." },
	/* 16   */ {  3,  2, "ANIMATION_JUMP_SHOOT",                    0xD4466DFB,	0x00000004,	0x00000043,	0x0000000F,	"Played when the player is in mid-air, going up and shooting." },
	/* 17   */ {  3,  1, "ANIMATION_JUMP_LOOK_UP",                  0x2691BFE6,	0x00000002,	0x00000044,	0x00000063,	"Played when the player is in mid-air, is going up and is looking up." },
	/* 18   */ {  3,  2, "ANIMATION_JUMP_LOOK_UP_SHOOT",            0x669E095B,	0x00000004,	0x00000045,	0x00000011,	"Played when the player is in mid-air, going up, looking up and shooting." },
	/* 19   */ {  3,  1, "ANIMATION_JUMP_LOOK_DOWN",                0x8962BDAA,	0x00000002,	0x00000046,	0x00000063,	"Played when the player is in mid-air, is going up and is looking down." },
	/* 20   */ {  3,  2, "ANIMATION_JUMP_LOOK_DOWN_SHOOT",          0x22333338,	0x00000004,	0x00000047,	0x00000013,	"Played when the player is in mid-air, going up, looking down and shooting." },
	/* 21   */ { -1,  2, "ANIMATION_FISH_LEFT_IDLE",                0x4A35E4B1,	0x00000002,	0x00000048,	0x00000063,	"Played when the player is fishing facing left." },
	/* 22   */ { -1,  2, "ANIMATION_FISH_RIGHT_IDLE",               0x73C3F8CA,	0x00000002,	0x00000049,	0x00000063,	"Played when the player is fishing facing right." },
	/* 23   */ { -1,  1, "ANIMATION_LAND_ON_GROUND",                0xCC611A17,	0x00000004,	0x0000004A,	0x00000063,	"Played when the player lands on the ground. Should not loop." },
	/* 24   */ {  4,  3, "ANIMATION_CARRIED_IDLE",                  0x3488130A,	0x00000002,	0x0000004B,	0x00000063,	"The player is being carried, and their carrier is standing still." },
	/* 25   */ {  4,  4, "ANIMATION_CARRIED_IDLE_SHOOT",            0x807CCF3E,	0x00000004,	0x0000004C,	0x00000018,	"The player is being carried, their carrier is standing still, and they\nare shooting." },
	/* 26   */ {  4,  3, "ANIMATION_CARRIED_WALKING",               0x0EA01794,	0x00000002,	0x0000004D,	0x00000018,	"The player is being carried, and their carrier is moving." },
	/* 27   */ {  4,  4, "ANIMATION_CARRIED_WALKING_SHOOT",         0x1EEAF95A,	0x00000004,	0x0000004E,	0x00000018,	"The player is being carried, their carrier is moving, and they\nare shooting." },
	/* 28   */ {  4,  3, "ANIMATION_CARRIED_LOOK_UP",               0x249D559F,	0x00000002,	0x0000004F,	0x00000063,	"The player is being carried and looking up." },
	/* 29   */ {  4,  4, "ANIMATION_CARRIED_LOOK_UP_SHOOT",         0x6185E1FA,	0x00000004,	0x00000050,	0x0000001C,	"The player is being carried, looking up and shooting." },
	/* 30   */ {  4,  3, "ANIMATION_CARRIED_LOOK_DOWN",             0x3740767F,	0x00000002,	0x00000051,	0x00000063,	"The player is being carried and looking down." },
	/* 31   */ {  4,  2, "ANIMATION_CARRIED_LOOK_DOWN_SHOOT",       0x3BFED3C1,	0x00000004,	0x00000052,	0x0000001E,	"The player is being carried, looking down and shooting." },
	/* 32   */ {  5,  3, "ANIMATION_CARRIED_DEAD_IDLE",             0x7CEA2409,	0x00000002,	0x00000053,	0x00000063,	"The player is dead, being carried, and their carrier is standing still." },
	/* 33   */ {  5,  3, "ANIMATION_CARRIED_DEAD_WALKING",          0x3F5D09C5,	0x00000002,	0x00000054,	0x00000063,	"The player is dead, being carried, and their carrier is moving." },
	/* 34   */ { -1,  0, "ANIMATION_WHIMSICAL_STAR_1",              0xB4518C74,	0x00000002,	0x00000063,	0x00000063,	"Animation loop for the first Whimsical Star." },
	/* 35   */ { -1,  0, "ANIMATION_WHIMSICAL_STAR_2",              0x2D58DDCE,	0x00000002,	0x00000063,	0x00000063,	"Animation loop for the second Whimsical Star." },
	/* 36   */ { -1,  0, "ANIMATION_WHIMSICAL_STAR_3",              0x5A5FED58,	0x00000002,	0x00000063,	0x00000063,	"Animation loop for the third Whimsical Star." },
	/* 37   */ { -1,  3, "ANIMATION_TACKLE_HIT_WALL",               0xF2DDFC53,	0x00000000,	0x00000055,	0x00000063,	"Played when the player hits a wall with Agility." },
	/* 38   */ { -1,  3, "ANIMATION_TACKLE_POUNCE",                 0xA5C1A328,	0x00000002,	0x00000056,	0x00000063,	"Played after the player charges Agility." },
	/* 39   */ { -1,  3, "ANIMATION_TACKLE_CHARGE",                 0x3C8850D5,	0x00000002,	0x00000057,	0x00000063,	"Played when the player is charging Agility." },
	/* 40   */ { -1,  3, "ANIMATION_TACKLE_CHARGE_LOOK_UP",         0x326A4A86,	0x00000002,	0x00000058,	0x00000063,	"Played when the player is charging Agility and looking up." },
	/* 41   */ { -1,  3, "ANIMATION_TACKLE_LAND_ON_GROUND",         0xD5F6453D,	0x00000000,	0x00000059,	0x00000063,	"Played when the player lands on the ground after performing a successful tackle." },
	/* 42   */ { -1,  3, "ANIMATION_TACKLE_SLIDING_ON_GROUND",      0x0391E092,	0x00000002,	0x0000005A,	0x00000063,	"Played when the player is waiting to stop sliding on the ground\nafter landing from a successful tackle." },
	/* 43   */ { -1,  3, "ANIMATION_TACKLE_STAND_UP",               0x008B8266,	0x00000000,	0x0000005B,	0x00000063,	"Played when the player gets up from a successful tackle." },
	/* 44   */ { -1,  3, "ANIMATION_COLLAPSED_ON_GROUND",           0x8ADA8FF6,	0x00000002,	0x0000005C,	0x00000063,	"Played when the player is touching the ground after being stunned mid-air." },
	/* 45   */ { -1,  3, "ANIMATION_STUNNED_FLY_FORWARD",           0xAFB76C5F,	0x00000002,	0x0000005D,	0x00000063,	"Played when the player is stunned whilst going forwards in mid-air." },
	/* 46   */ { -1,  3, "ANIMATION_STUNNED_FLY_UP",                0xD94D8286,	0x00000002,	0x0000005E,	0x00000063,	"Played when the player is stunned whilst going up in mid-air." },
	/* 47   */ { -1,  3, "ANIMATION_STUNNED_FLY_DOWN",              0xB4F765C0,	0x00000002,	0x0000005F,	0x00000063,	"Played when the player is stunned whilst going down in mid-air." },
	/* 48   */ { -1,  3, "ANIMATION_STUNNED_FLY_BACKWARD",          0x96ADEFAA,	0x00000002,	0x00000060,	0x00000063,	"Played when the player is stunned whilst going backwards in mid-air." },
	/* 49   */ { -1,  8, "ANIMATION_DROWNED",                       0xAB7D4110,	0x00000000,	0x00000061,	0x0000002C,	"Played when the player runs out of air." },
	/* 50   */ { -1, 12, "ANIMATION_RIDING_DRAGON",                 0xA9CF9E33,	0x00000002,	0x00000062,	0x00000063,	"Player is riding the dragon in the neutral ending." },
	/* 51   */ {  0,  0, "ANIMATION_MASK_IDLE",                     0xCD11DA80,	0x00000003,	0x00000063,	0x00000000,	"Played when the player is standing still.\nPlayer has a Mimiga Mask equipped." },
	/* 52   */ {  0,  2, "ANIMATION_MASK_IDLE_SHOOT",               0xA10F945B,	0x00000005,	0x00000063,	0x00000001,	"Played when the player is standing still and shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 53   */ {  0,  0, "ANIMATION_MASK_IDLE_LOOK_UP",             0x4CF73CFE,	0x00000003,	0x00000063,	0x00000002,	"Played when the player is looking up and standing still.\nPlayer has a Mimiga Mask equipped." },
	/* 54   */ {  0,  2, "ANIMATION_MASK_IDLE_LOOK_UP_SHOOT",       0x2450A5C6,	0x00000005,	0x00000063,	0x00000003,	"Played when the player is looking up and shooting while standing still.\nPlayer has a Mimiga Mask equipped." },
	/* 55   */ { -1,  1, "ANIMATION_MASK_SEARCH",                   0xCB21CC6E,	0x00000003,	0x00000063,	0x00000004,	"Played when the player presses the interaction button (Down Arrow).\nPlayer has a Mimiga Mask equipped." },
	/* 56   */ {  1,  1, "ANIMATION_MASK_WALK",                     0x9CA415C0,	0x00000003,	0x00000063,	0x00000005,	"Played when the player is walking.\nPlayer has a Mimiga Mask equipped." },
	/* 57   */ {  1,  2, "ANIMATION_MASK_WALK_SHOOT",               0x82C01B12,	0x00000005,	0x00000063,	0x00000006,	"Played when the player is walking and shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 58   */ {  1,  1, "ANIMATION_MASK_WALK_LOOK_UP",             0x104B75CC,	0x00000003,	0x00000063,	0x00000007,	"Played when the player is walking while looking up.\nPlayer has a Mimiga Mask equipped." },
	/* 59   */ {  1,  2, "ANIMATION_MASK_WALK_LOOK_UP_SHOOT",       0x0A031FAE,	0x00000005,	0x00000063,	0x00000008,	"Played when the player is walking, looking up and shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 60   */ {  2,  1, "ANIMATION_MASK_FALL",                     0xEA65B099,	0x00000003,	0x00000063,	0x00000009,	"Played when the player is in mid-air and is going down.\nPlayer has a Mimiga Mask equipped." },
	/* 61   */ {  2,  2, "ANIMATION_MASK_FALL_SHOOT",               0x8F0BCF4A,	0x00000005,	0x00000063,	0x0000000A,	"Played when the player is falling and shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 62   */ {  2,  1, "ANIMATION_MASK_FALL_LOOK_UP",             0xE397B996,	0x00000003,	0x00000063,	0x0000000B,	"Played when the player is in mid-air, is going down and is looking up.\nPlayer has a Mimiga Mask equipped." },
	/* 63   */ {  2,  2, "ANIMATION_MASK_FALL_LOOK_UP_SHOOT",       0x50F3FA4D,	0x00000005,	0x00000063,	0x0000000C,	"Played when the player is in mid-air, looking up and shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 64   */ {  2,  1, "ANIMATION_MASK_FALL_LOOK_DOWN",           0x4F3EA46F,	0x00000003,	0x00000063,	0x0000000D,	"Played when the player is in mid-air, is going down and is looking down.\nPlayer has a Mimiga Mask equipped." },
	/* 65   */ {  2,  2, "ANIMATION_MASK_FALL_LOOK_DOWN_SHOOT",     0x1A1F1324,	0x00000005,	0x00000063,	0x0000000E,	"Played when the player is in mid-air, looking down and shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 66   */ {  3,  1, "ANIMATION_MASK_JUMP",                     0xB6866983,	0x00000003,	0x00000063,	0x0000000F,	"Played when the player is in mid-air and is going up.\nPlayer has a Mimiga Mask equipped." },
	/* 67   */ {  3,  2, "ANIMATION_MASK_JUMP_SHOOT",               0x08CFCA9D,	0x00000005,	0x00000063,	0x00000010,	"Played when the player is in mid-air, going up and shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 68   */ {  3,  1, "ANIMATION_MASK_JUMP_LOOK_UP",             0x5D849905,	0x00000003,	0x00000063,	0x00000011,	"Played when the player is in mid-air, is going up and is looking up.\nPlayer has a Mimiga Mask equipped." },
	/* 69   */ {  3,  2, "ANIMATION_MASK_JUMP_LOOK_UP_SHOOT",       0x87AC9ACF,	0x00000005,	0x00000063,	0x00000012,	"Played when the player is in mid-air, going up, looking up and shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 70   */ {  3,  1, "ANIMATION_MASK_JUMP_LOOK_DOWN",           0x2E3CE36D,	0x00000003,	0x00000063,	0x00000013,	"Played when the player is in mid-air, is going up and is looking down.\nPlayer has a Mimiga Mask equipped." },
	/* 71   */ {  3,  2, "ANIMATION_MASK_JUMP_LOOK_DOWN_SHOOT",     0x5E185FEA,	0x00000005,	0x00000063,	0x00000014,	"Played when the player is in mid-air, going up, looking down and shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 72   */ { -1,  2, "ANIMATION_MASK_FISH_LEFT_IDLE",           0xED6BBA76,	0x00000003,	0x00000063,	0x00000015,	"Played when the player is fishing facing left.\nPlayer has a Mimiga Mask equipped." },
	/* 73   */ { -1,  2, "ANIMATION_MASK_FISH_RIGHT_IDLE",          0x7664F187,	0x00000003,	0x00000063,	0x00000016,	"Played when the player is fishing facing right.\nPlayer has a Mimiga Mask equipped." },
	/* 74   */ { -1,  1, "ANIMATION_MASK_LAND_ON_GROUND",           0x6B3F44D0,	0x00000005,	0x00000063,	0x00000017,	"Played when the player lands on the ground. Should not loop.\nPlayer has a Mimiga Mask equipped." },
	/* 75   */ {  4,  3, "ANIMATION_MASK_CARRIED_IDLE",             0x4F9D35E9,	0x00000003,	0x00000063,	0x00000018,	"The player is being carried, and their carrier is standing still.\nPlayer has a Mimiga Mask equipped." },
	/* 76   */ {  4,  4, "ANIMATION_MASK_CARRIED_IDLE_SHOOT",       0x614E5CAA,	0x00000005,	0x00000063,	0x00000019,	"The player is being carried, their carrier is standing still, and they\nare shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 77   */ {  4,  3, "ANIMATION_MASK_CARRIED_WALKING",          0x0B071ED9,	0x00000003,	0x00000063,	0x0000001A,	"The player is being carried, and their carrier is moving.\nPlayer has a Mimiga Mask equipped." },
	/* 78   */ {  4,  4, "ANIMATION_MASK_CARRIED_WALKING_SHOOT",    0x764B61CE,	0x00000005,	0x00000063,	0x0000001B,	"The player is being carried, their carrier is moving, and they\nare shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 79   */ {  4,  3, "ANIMATION_MASK_CARRIED_LOOK_UP",          0x213A5CD2,	0x00000003,	0x00000063,	0x0000001C,	"The player is being carried and looking up.\nPlayer has a Mimiga Mask equipped." },
	/* 80   */ {  4,  4, "ANIMATION_MASK_CARRIED_LOOK_UP_SHOOT",    0x0924796E,	0x00000005,	0x00000063,	0x0000001D,	"The player is being carried, looking up and shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 81   */ {  4,  3, "ANIMATION_MASK_CARRIED_LOOK_DOWN",        0x0B4BFA34,	0x00000003,	0x00000063,	0x0000001E,	"The player is being carried and looking down.\nPlayer has a Mimiga Mask equipped." },
	/* 82   */ {  4,  2, "ANIMATION_MASK_CARRIED_LOOK_DOWN_SHOOT",  0xD007EF08,	0x00000005,	0x00000063,	0x0000001F,	"The player is being carried, looking down and shooting.\nPlayer has a Mimiga Mask equipped." },
	/* 83   */ {  5,  3, "ANIMATION_MASK_CARRIED_DEAD_IDLE",        0x40E1A842,	0x00000003,	0x00000063,	0x00000020,	"The player is dead, being carried, and their carrier is standing still.\nPlayer has a Mimiga Mask equipped." },
	/* 84   */ {  5,  3, "ANIMATION_MASK_CARRIED_DEAD_WALKING",     0x43766517,	0x00000003,	0x00000063,	0x00000021,	"The player is dead, being carried, and their carrier is moving.\nPlayer has a Mimiga Mask equipped." },
	/* 85   */ { -1,  3, "ANIMATION_MASK_TACKLE_HIT_WALL",          0xF77AF51E,	0x00000001,	0x00000063,	0x00000025,	"Played when the player hits a wall with Agility.\nPlayer has a Mimiga Mask equipped." },
	/* 86   */ { -1,  3, "ANIMATION_MASK_TACKLE_POUNCE",            0x9CB905CC,	0x00000003,	0x00000063,	0x00000026,	"Played after the player charges Agility.\nPlayer has a Mimiga Mask equipped." },
	/* 87   */ { -1,  3, "ANIMATION_MASK_TACKLE_CHARGE",            0x05F0F631,	0x00000003,	0x00000063,	0x00000027,	"Played when the player is charging Agility.\nPlayer has a Mimiga Mask equipped." },
	/* 88   */ { -1,  3, "ANIMATION_MASK_TACKLE_CHARGE_LOOK_UP",    0x5ACBD212,	0x00000003,	0x00000063,	0x00000028,	"Played when the player is charging Agility and looking up.\nPlayer has a Mimiga Mask equipped." },
	/* 89   */ { -1,  3, "ANIMATION_MASK_TACKLE_LAND_ON_GROUND",    0xBD57DDA9,	0x00000001,	0x00000063,	0x00000029,	"Played when the player lands on the ground after performing a successful tackle.\nPlayer has a Mimiga Mask equipped." },
	/* 90   */ { -1,  3, "ANIMATION_MASK_TACKLE_SLIDING_ON_GROUND", 0xE1C263BA,	0x00000003,	0x00000063,	0x0000002A,	"Played when the player is waiting to stop sliding on the ground\nafter landing from a successful tackle.\nPlayer has a Mimiga Mask equipped." },
	/* 91   */ { -1,  3, "ANIMATION_MASK_TACKLE_STAND_UP",          0x052C8B2B,	0x00000001,	0x00000063,	0x0000002B,	"Played when the player gets up from a successful tackle.\nPlayer has a Mimiga Mask equipped." },
	/* 92   */ { -1,  3, "ANIMATION_MASK_COLLAPSED_ON_GROUND",      0x7D59EA38,	0x00000003,	0x00000063,	0x0000002C,	"Played when the player is touching the ground after being stunned mid-air.\nPlayer has a Mimiga Mask equipped." },
	/* 93   */ { -1,  3, "ANIMATION_MASK_STUNNED_FLY_FORWARD",      0x58340991,	0x00000003,	0x00000063,	0x0000002D,	"Played when the player is stunned whilst going forwards in mid-air.\nPlayer has a Mimiga Mask equipped." },
	/* 94   */ { -1,  3, "ANIMATION_MASK_STUNNED_FLY_UP",           0x7E13DC41,	0x00000003,	0x00000063,	0x0000002E,	"Played when the player is stunned whilst going up in mid-air.\nPlayer has a Mimiga Mask equipped." },
	/* 95   */ { -1,  3, "ANIMATION_MASK_STUNNED_FLY_DOWN",         0xBC9FFFE4,	0x00000003,	0x00000063,	0x0000002F,	"Played when the player is stunned whilst going down in mid-air.\nPlayer has a Mimiga Mask equipped." },
	/* 96   */ { -1,  3, "ANIMATION_MASK_STUNNED_FLY_BACKWARD",     0xEA868378,	0x00000003,	0x00000063,	0x00000030,	"Played when the player is stunned whilst going backwards in mid-air.\nPlayer has a Mimiga Mask equipped." },
	/* 97   */ { -1,  8, "ANIMATION_MASK_DROWNED",                  0x804034B9,	0x00000001,	0x00000063,	0x00000031,	"Played when the player runs out of air.\nPlayer has a Mimiga Mask equipped." },
	/* 98   */ { -1, 12, "ANIMATION_MASK_RIDING_DRAGON",            0x90B738D7,	0x00000003,	0x00000063,	0x00000032,	"Player is riding the dragon in the neutral ending.\nPlayer has a Mimiga Mask equipped." },
};

#define MAX_PLAYER_ANIMATION_INFO (sizeof(gPlayerAnimationInfo) / sizeof(gPlayerAnimationInfo[0]))

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#undef BEGIN_ANIMATION_LIST
#undef DEFINE_ANIMATION_FRAME
#undef FINISH_ANIMATION_LIST
#undef BEGIN_FACE_LIST
#undef DEFINE_FACE_FRAME
#undef FINISH_FACE_LIST
#undef DEFINE_ANIMATION

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define PXCHAR_HEADER_STRING	"PXCHAR"
#define PXCHAR_VERSION			12

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// @brief Animation info.
///
class CustomPlayerAnimation
{
	friend class CustomPlayer;
public: // Structs

	/// Frame information for an animation.
	///
	struct Frame
	{
		/// This frame's sequence number.
		///
		int frame_no;

		/// The player frame to display on this frame.
		///
		PlayerAnimFrame player_frame;

		/// The sound to play on this frame.
		///
		unsigned short sound_no;

		/// The delay, in FPS, before the next frame.
		///
		unsigned int delay;

		/// A pointer to the next frame.
		///
		Frame* next;

		/// The drawing offset for this frame.
		struct
		{
			short x;
			short y;
		} offset;

		/// @brief The default constructor for this struct.
		///
		Frame()
		{
			frame_no     = 0;
			player_frame = PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES;
			sound_no     = 0xFFFF;
			delay        = 0;
			offset.x     = 0;
			offset.y     = 0;
		}

		/// @brief The default constructor for this struct.
		///
		Frame(int iFrameNo, PlayerAnimFrame iPlayerFrame, unsigned short iSoundId, unsigned int iFrameDelay, char iOffsetX, char iOffsetY)
		{
			frame_no     = iFrameNo;
			player_frame = iPlayerFrame;
			sound_no     = iSoundId;
			delay        = iFrameDelay;
			offset.x     = iOffsetX;
			offset.y     = iOffsetY;
		}

		/// @brief Check to see if two frames have the same data.
		///
		/// @return Returns true if the assessment is true.
		inline bool Equals(Frame* pOther)
		{
			return (pOther->player_frame == player_frame && pOther->sound_no == sound_no && pOther->delay == delay && pOther->offset.x == offset.x && pOther->offset.y == offset.y);
		}
	};

private: // Private variables

	/// Stores whether the AllocateFrame() function should call CalculateMetadata() afterwards (false) or not (true).
	///
	bool mLocked;

	/// Stores whether this animation is dirty.
	///
	bool mDirty;

public: // Variables

	/// The owner for this animation.
	///
	CustomPlayer* mOwner;

	/// The type of animation this object represents.
	///
	PlayerAnimation mType;
	
	/// The subsitute for this animation.
	///
	PlayerAnimation mSubsitute;

	/// The first frame that belongs to this animation.
	///
	Frame* mFirstFrame;
	
	/// The last frame that belongs to this animation.
	///
	Frame* mLastFrame;
	
	/// The last frame that belongs to this animation.
	///
	Frame* mLoopStartFrame;

	/// Number of frame objects allocated.
	///
	unsigned int mFrameCount;

	/// The flags for this animation.
	///
	unsigned int mFlags;

	/// The loop frame for this animation.
	///
	unsigned int mLoopStart;

	/// Set if this has the default frames or not.
	///
	bool mDefault;

	/// The metadata for this animation.
	///
	struct
	{
		/// The length of this animation, in frames. Does not include the frames caused by frame delays.
		///
		unsigned int mAnimLength_Frames;

		/// The total length of this animation, in frame. Includes the frames caused by frame delays.
		///
		unsigned int mAnimLength_TotalFrames;

	} mMetadata;

public: // C++ Stuff

	/// Constructor
	///
	CustomPlayerAnimation();

	/// Deconstructor
	///
	~CustomPlayerAnimation();

protected: // Protected methods

	/// @brief Allocate a frame.
	///
	/// @param iFrameNo The frame number to allocate at.
	///
	/// @return Returns the newly allocated frame.
	Frame* AllocateFrame(int iFrameNo);

public: // Frame Management

	/// @brief This function recalculates this animation's metadata.
	///
	void CalculateMetadata();

	/// @brief Add a frame.
	///
	/// @param iFrameNo The frame number to insert the new frame at.
	/// @param iAnimFrame The animation frame to use. Set to MAX_PLAYER_ANIM_FRAMES to have no animation on this frame.
	/// @param iSoundNo The sound number to play. Set to 0xFFFF to not play any sound.
	/// @param iDelay The frame delay in FPS. Set to 0 to have no delay.
	/// @param iSpriteOffsetX The sprite's X offset for this frame.
	/// @param iSpriteOffsetY The sprite's Y offset for this frame.
	///
	/// @return Returns true on success, otherwise returns false.
	bool AddFrame(int iFrameNo, PlayerAnimFrame iAnimFrame = PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES, unsigned short iSoundNo = 0xFFFF, unsigned int iDelay = 0, short iSpriteOffsetX = 0, short iSpriteOffsetY = 0);

	/// @brief Remove a frame at a given number.
	///
	/// @return Returns true on success, false on failure.
	bool RemoveFrame(int iFrameNo);

	/// @brief Find a frame by number.
	///
	/// @param iFrameNo The frame number.
	///
	/// @return Returns a Frame* object if it exists.
	Frame* FindFrame(int iFrameNo);
	
	/// @brief Reset this animation.
	///
	void Reset();

public: // Methods

	/// @brief Set the loop start point.
	///
	/// @param iFrameNo The frame number to start the loop at.
	void SetLoopStartFrameNo(int iFrameNo);

	/// @brief Execute a frame.
	///
	/// @param pFrame The frame to execute.
	/// @param pMC The player we should execute the frame on.
	/// @param bPlaySound Play the sound for this frame.
	/// @param iSubPixelX The X position at which to play the sound.
	/// @param iSubPixelY The Y position at which to play the sound.
	void ExecuteFrame(Frame* pFrame, CUSTOM_PLAYER_ANIMATION_INFO* pInfo, bool bPlaySound = true, int iSubPixelX = -1, int iSubPixelY = -1);

	/// @brief Advance this animation by one frame.
	///
	/// @param pMC The player to tick.
	/// @param bPlaySound Play sounds.
	/// @param bKeepFrame Do not advance a frame. If this is set to '-1', then let the animation system have control over this aspect.
	/// @param iSubPixelX The X position at which to play the sound.
	/// @param iSubPixelY The Y position at which to play the sound.
	///
	/// @return Returns false if the animation has ended, true otherwise.
	bool Tick(CUSTOM_PLAYER_ANIMATION_INFO* pInfo, bool bPlaySound = true, char bKeepFrame = -1, int iSubPixelX = -1, int iSubPixelY = -1);

	/// @brief Set the frame number for a play information object.
	///
	/// @param pInfo The info object.
	/// @param iFrameNo The frame number to set.
	/// @param bPlaySound Play sounds.
	/// @param iSubPixelX The X position at which to play the sound.
	/// @param iSubPixelY The Y position at which to play the sound.
	///
	/// @return Returns the frame number that was set.
	int SetFrameNo(CUSTOM_PLAYER_ANIMATION_INFO* pInfo, int iFrameNo, bool bPlaySound = true, int iSubPixelX = -1, int iSubPixelY = -1);

	/// @brief Restart the animation from the beginning.
	///
	/// @param pMC The player to initialize.
	/// @param bKeepFrame Keep the current frame number if it's within range. If this is set to '-1', then let the animation system have control over this aspect.
	void Start(CUSTOM_PLAYER_ANIMATION_INFO* pInfo, char bKeepFrame = -1);

	/// @brief Check to see if the animation has completed 'n' cycles.
	///
	/// @param pInfo The info object.
	/// @param iCycleCount The amount of cycles we're checking for.
	///
	/// @return Returns true if the animation has played the given amount of times, false otherwise.
	bool HasPlayed(CUSTOM_PLAYER_ANIMATION_INFO* pInfo, unsigned short iCycleCount = 1);

	/// @brief Get the sprite offset for a frame.
	///
	/// @param pInfo The info object.
	/// @param iOffsetX A pointer to an integer that will hold the X offset.
	/// @param iOffsetY A pointer to an integer that will hold the Y offset.
	///
	/// @return Returns true on success, false on failure.
	bool GetFrameSpriteOffset(CUSTOM_PLAYER_ANIMATION_INFO* pInfo, int* iOffsetX, int* iOffsetY);

	/// @brief Lock this animation.
	///
	inline void Lock()
	{
		mLocked = true;
	}
	
	/// @brief Unlock this animation.
	///
	inline void Unlock()
	{
		mLocked = true;

		if (mDirty)
			CalculateMetadata();
	}

	/// @brief Set default configuration for this animation.
	///
	/// @param bCheckOnly If set to 'true', no frames will be added. Instead, the default frames will be checked against the current frames to determine if this animation has default frames.
	///
	/// @return Returns true if the default frames are set.
	bool SetDefaults(bool bCheckOnly = false);

public: // Debug methods

	/// @brief Dump this object's frames.
	///
	void Dump();
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CustomPlayer
{
	friend class CustomPlayerAnimation;
public: // Linking

	static CustomPlayer* subsitute;
	static CustomPlayer* first;
	CustomPlayer* next;

public: // Structs

	enum CustomSoundType
	{
		CUSTOM_SOUND_PXT = 0,
		CUSTOM_SOUND_WAV = 1,

		MAX_CUSTOM_SOUND_TYPES
	};

	struct CPLR_POINT
	{
		union
		{
			int x;
			int w;
		};

		union
		{
			int y;
			int h;
		};
	};

	struct CPLR_BITPLANE
	{
		bool ready;
		unsigned char* bits;
	};

	/// Custom sound information
	///
	struct CustomSound
	{
		/// The type of custom sound this is.
		///
		CustomSoundType type;

		/// The file path of the pixtone file.
		///
		char* file_path;

		/// The CRC of this sound's filepath (for faster lookup).
		///
		unsigned int file_path_crc;

		/// The loaded custom sound ID.
		///
		int custom_sound_id;

		/// Use count
		///
		int use_count;
	};
	
	/// Custom frame information
	///
	struct CustomFrame
	{
		/// The name of this custom frame.
		///
		char* frame_name;

		/// The CRC of this frame's name (for faster lookup).
		///
		unsigned int frame_name_crc;

		/// The source rect for this frame.
		///
		RECT rect;

		/// The weapon offset for this frame.
		///
		CPLR_POINT weapon_offset;
		
		/// The offset for held entities (like puppies).
		///
		CPLR_POINT held_entity_offset;
		
		/// The offset for carried players.
		///
		CPLR_POINT carry_offset;

		/// The flags for this frame.
		///
		unsigned int flags;

		/// Use count
		///
		int use_count;
	};

public: // Data

	/// Full path to the character folder. String characteristics:
	///   - All backward slashes (\) are converted to forward slashes (/)
	///   - Does not end in a slash
	///   - Is a full path (ex. "H:/dev/CaveStory/build_en/data/Players/Quote")
	char* mPath;

	/// The character name for this player.
	char* mName;

	/// The author for this player.
	char* mAuthor;

	/// Stores all frame data
	RECT mFrameList[PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES];

	/// Stores all face data
	RECT mFaceList[PlayerFaceFrame::MAX_PLAYER_FACES];

	/// Stores all weapon offsets for all player frames
	CPLR_POINT mWeaponOffsets[PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES];
	
	/// Stores all held entity offsets for all player frames
	CPLR_POINT mHeldEntityOffsets[PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES];
	
	/// Stores all carry player offsets for all player frames
	CPLR_POINT mCarriedPlayerOffsets[PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES];

	/// Stores all bitplanes for all player frames
	CPLR_BITPLANE mBitPlanes[PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES];

	/// Stores all custom sounds
	CustomSound* mCustomSoundList;

	/// Stores the number of custom sounds
	unsigned int mCustomSoundCount;
	
	/// Stores all custom frames
	CustomFrame* mCustomFrameList;

	/// Stores the number of custom sounds
	unsigned int mCustomFrameCount;

	/// Minimum viewbox rect
	RECT mMinViewbox;

	/// Maximum viewbox rect
	RECT mMaxViewbox;

	/// Fishing offset
	CPLR_POINT mFishOffset;
	CPLR_POINT mFishSpriteOffset;
	RECT mHitBox;
	RECT mViewBox;

	/// In-game stuff
	int mNormalSurfaceId;
	int mMaskSurfaceId;
	int mFaceMaskSurfaceId;
	int mFaceSurfaceId;
	
	/// Animations
	CustomPlayerAnimation mAnimList[PlayerAnimation::MAX_PLAYER_ANIMATIONS];

	/// Whether this custom player is loaded or not
	bool mIsLinked;
	bool mIsLoaded;

public: // Constructor & deconstructor

	CustomPlayer(bool do_link = true);
	~CustomPlayer();

public: // Methods

	/// @brief Reset this custom player.
	void Reset();

	/// @brief Load a player configuration.
	///
	/// @param pFileName The file name to load.
	/// @param bOnlyValidate 
	///
	/// @return Returns true on success, false otherwise.
	bool LoadPlayer(const char* pFileName, bool bOnlyValidate = false);

	/// @brief Save a player configuration.
	///
	/// @param pFileName The file name to load.
	///
	/// @return Returns true on success, false otherwise.
	bool SavePlayer();

	/// @brief Recalculate meta information about this player.
	///
	void RecalculateMeta();

public: // Custom player information

	/// @brief Get the name of this character.
	///
	/// @return Returns a string pointer to the character's name.
	const char* GetName();

	/// @brief Get the author for this character.
	///
	/// @return Returns a string pointer to the author's name.
	const char* GetAuthor();

	/// @brief Get the path for this character.
	///
	/// @return Returns a string pointer to the path.
	const char* GetPath();

	/// @brief Fetch a rect with an animation frame
	///
	/// @param iFrame The frame of animation to fetch.
	/// @param pOut Pointer to the rect. If NULL, then this will return the actual rect.
	///
	/// @return Returns the rect.
	RECT* GetAnimationRect(PlayerAnimFrame iFrame, RECT* pOut);

	/// @brief Fetch a face rect
	///
	/// @param iFace The face to fetch.
	/// @param pOut Pointer to the rect. If NULL, then this will return the actual rect.
	///
	/// @return Returns the rect.
	RECT* GetFaceRect(PlayerFaceFrame iFace, RECT* pOut);

	/// @brief Fetch a weapon offset for a frame
	///
	/// @param iFrame The frame which we should get the weapon offset of.
	/// @param pPoint A pointer to a CPLR_POINT that will hold the offset.
	///
	/// @return Returns pPoint on success, NULL otherwise.
	CPLR_POINT* GetWeaponOffset(PlayerAnimFrame iFrame, CPLR_POINT* pPoint);
	
	/// @brief Fetch a held entity offset
	///
	/// @param iFrame The frame which we should get the held entity offset of.
	/// @param pPoint A pointer to a CPLR_POINT that will hold the offset.
	///
	/// @return Returns pPoint on success, NULL otherwise.
	CPLR_POINT* GetHeldEntityOffset(PlayerAnimFrame iFrame, CPLR_POINT* pPoint);
	
	/// @brief Fetch a carry offset
	///
	/// @param iFrame The frame which we should get the carried player offset of.
	/// @param pPoint A pointer to a CPLR_POINT that will hold the offset.
	///
	/// @return Returns pPoint on success, NULL otherwise.
	CPLR_POINT* GetCarryPlayerOffset(PlayerAnimFrame iFrame, CPLR_POINT* pPoint);

	/// @brief Get an animation by its identifier.
	///
	/// It's important that this is used instead of using mAnimList, since this also calculates animation subsitution.
	///
	/// @param iAnimType The animation type.
	/// @param bSubsitute If true, then this function determines the subsituted animation (if there is one).
	///
	/// @return Returns the animation object.
	CustomPlayerAnimation* GetAnimation(PlayerAnimation iAnimType, bool bSubsitute = true);

public: // Setting information

	/// @brief Set the new name for this character.
	/// 
	/// @param pNewName The new character name.
	void SetName(const char* pNewName);

	/// @brief Set the author for this character.
	/// 
	/// @param pNewAuthor The new author name.
	void SetAuthor(const char* pNewAuthor);

	/// @brief Get the path for this character.
	///
	/// @param pNewPath The path for this avatar.
	void SetPath(const char* pNewPath);

	/// @brief Set an animation frame for this character.
	///
	/// @param iFrame The frame index for the animation.
	/// @param pRect The new rect.
	void SetAnimationRect(PlayerAnimFrame iFrame, RECT rect);

	/// @brief Set a face frame for this character.
	///
	/// @param iFace The face index.
	/// @param iX The new X position.
	/// @param iY The new Y position.
	void SetFaceRect(PlayerFaceFrame iFace, int iX, int iY);

	/// @brief Set a weapon offset for a frame.
	///
	/// @param iFrame The frame index for the animation.
	/// @param iX The X weapon offset.
	/// @param iY The Y weapon offset.
	void SetWeaponOffset(PlayerAnimFrame iFrame, int iX, int iY);

public: // Custom frame methods

	/// @brief Get a custom frame of animation by name.
	///
	/// @param pFrameName The name to search for.
	/// @param bCreate If the frame could not be found and this is set to 'true', then a new frame with the given name will be allocated.
	///
	/// @return Returns a CustomFrame object on success, otherwise returns NULL.
	CustomFrame* GetCustomFrame(const char* pFrameName, bool bCreate = false);

	/// @brief Remove a custom frame of animation by name.
	///
	/// @param pFrameName The name of the frame to be removed.
	///
	/// @return Returns true on success, false on failure.
	bool RemoveCustomFrame(const char* pFrameName);

	/// @brief Clear all custom frame objects.
	///
	void ClearCustomFrames();

public: // Custom sound methods

	/// @brief Get a custom frame of animation by name.
	///
	/// @param pFileName The PXT's filename to search for. \
	///        This string will be stripped of path & file extension, and then it will be concatenated with the player's path. \
	///        Ex: If I pass "C:/MyPlayer/Lol" to 'pFileName', then it will be expanded to "H:/My/Player/Path/Lol.pxt"
	/// @param bCreate If the sound could not be found and this is set to 'true', then a new sound with the given name will be allocated.
	///
	/// @return Returns a CustomSound object on success, otherwise returns NULL.
	CustomSound* GetCustomSound(const char* pFileName, bool bCreate = false);

	/// @brief Remove a custom sound by name.
	///
	/// @param pFileName The filename of the sound to be removed.
	///
	/// @return Returns true on success, false on failure.
	bool RemoveCustomSound(const char* pFileName);

	/// @brief Clear all custom sound objects.
	///
	void ClearCustomSounds();

public: // Bitplanes methods

	/// @brief Check to see if a pixel in Normal.png is opaque or not.
	///
	/// @param iFrame The frame to check for a pixel on.
	/// @param iX The X position of the pixel.
	/// @param iY The Y position of the pixel.
	///
	/// @return Returns true if the pixel has an alpha value of 255, false otherwise.
	bool IsPixelSolid(PlayerAnimFrame iFrame, int iX, int iY);

	/// @brief Get the flags for a frame.
	///
	/// @return Returns the flags for the specified frame.
	inline unsigned int GetFrameFlags(PlayerAnimFrame iFrame)
	{
		if (iFrame >= PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES)
			return (iFrame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES < mCustomFrameCount ? mCustomFrameList[iFrame - PlayerAnimFrame::MAX_PLAYER_ANIM_FRAMES].flags : 0);

		return gAnimFrameInfo[iFrame].flags;
	}

public: // Game methods

	/// @brief Load this player's assets.
	///
	/// @param bShowErrors Show errors in a message box.
	///
	/// @return Returns true on success, false otherwise.
	bool LoadAssets(bool bShowWarnings = false);

	/// @brief Unload this player's assets.
	///
	/// @return Returns true on success, false otherwise.
	bool UnloadAssets();
};