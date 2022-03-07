

//offset into exe to PE header info
#define PE_OFFSET               0x3c

//**** offsets into .text section ****   -0x001000

#define CS_TITLE_MAP_X_OFFSET            0x00E761//unsigned char
#define CS_TITLE_MAP_Y_OFFSET            0x00E75F//unsigned char
#define CS_TITLE_MAP_EVENT_OFFSET        0x00E763//unsigned char
#define CS_TITLE_MAP_OFFSET              0x00E765//unsigned char

#define CS_START_HEALTH_CURRENT_OFFSET   0x013BCF//unsigned short int
#define CS_START_HEALTH_MAX_OFFSET       0x013BD8//unsigned short int
#define CS_START_PLAYER_FACING           0x013B74//int

#define CS_MAXIMUM_MAP_COUNT_OFFSET      0x013B24//unsigned char

#define CS_START_UNKOWN1                 0x013B6D//unsigned char

#define CS_START_PLAYER_X_OFFSET         0x01C592//unsigned char
#define CS_START_PLAYER_Y_OFFSET         0x01C590//unsigned char
#define CS_START_EVENT_OFFSET            0x01C594//int?
#define CS_START_MAP_OFFSET              0x01C599//unsigned char

#define CS_SCRIPT_LIMT                   0x020545  //0x021545

//0x8FC0 - Weapon behaivor jump routine
/*/  Weapon routines

0x004160 - Snake
0x0043F0
0x0043F0

0x0047B0 - Polar Star
0x0047B0
0x0047B0

0x004B30 - Fireball
0x004B30
0x004B30

0x005120 - Machinegun
0x005120
0x005120

0x0055A0 - Missile Launcher
0x0055A0
0x0055A0

0x005D80 - Missiles
0x005D80
0x005D80

0x005F30 - Bubbler
0x006190
0x0064D0

0x0068B0 - [nothing]
0x0075E0
0x0078A0

0x006BB0 - Blade
0x006E60
0x007110

0x007910 - Super Missile Launcher
0x007910
0x007910

0x008080 - Super Missile
0x008080
0x008080

0x008230 - Nemesis
0x008230
0x008230

0x008710 - Spur
0x008710
0x008710

0x008AE0 - 
0x008AE0
0x008AE0

0x008230 - 
0x008F40
0x008F90
//*/

//These are the most important offsets
//these offsets reference the map data and must be updated to expand the map area
#define CS_MAP_JMP1_OFFSET1              0x020C2F
#define CS_MAP_JMP1_OFFSET2              0x020C73

//these reference 0x0937D0 not 0x0937B0 (file name)
#define CS_MAP_JMP2_OFFSET1              0x020CB5
#define CS_MAP_JMP2_OFFSET2              0x020CF6
#define CS_MAP_JMP2_OFFSET3              0x020D38

//these reference 0x0937F4 not 0x0937B0 (background)
#define CS_MAP_JMP3_OFFSET1              0x020D7A

//these reference 0x0937F0 not 0x0937B0 (background type)
#define CS_MAP_JMP8_OFFSET1              0x020D9E

//these reference 0x093814 not 0x0937B0 (npc tileset 1)
#define CS_MAP_JMP4_OFFSET1              0x020DD9

//these reference 0x093834 not 0x0937B0 (npc tileset 2)
#define CS_MAP_JMP5_OFFSET1              0x020E1C

//these reference 0x093855 not 0x0937B0 (caption)
#define CS_MAP_JMP6_OFFSET1              0x020E6A

//these reference 0x093854 not 0x0937B0 (boss number)
#define CS_MAP_JMP7_OFFSET1              0x020EA8


//**** offsets into .data section ****
#define CS_HEALTH_CURRENT_OFFSET         0x00F6CC
#define CS_HEALTH_MAX_OFFSET             0x00F6D0

#define CS_ARMSITEM_TSC_STRING_OFFSET         624
#define CS_PBM_FOLDER_STRING_OFFSET           640
#define CS_DOUKUTSU_TIMESTAMP_STRING_OFFSET   656
#define CS_CONFIG_DAT_STRING_OFFSET           676
#define CS_VERSION_FORMAT_STRING_OFFSET       700
#define CS_000_DAT_STRING_OFFSET              764
#define CS_PBM_FOLDER_STRING_OFFSET2          772
#define CS_SURFACE_STRING_OFFSET              784
#define CS_PBM_FOLDER_STRING_OFFSET3          808
#define CS_DEBUG_OUT_STRING_OFFSET            840
#define CS_CREDIT_STRING_OFFSET               860
#define CS_CREDIT_TSC_STRING_OFFSET           872
#define CS_CASTS_STRING_OFFSET                896
#define CS_NPC_TBL_STRING_OFFSET              936
#define CS_XXX_STRING_OFFSET                  992
#define CS_C_PIXEL_STRING_OFFSET             1240
#define CS_PIXEL_STRING_OFFSET               1256
#define CS_MYCHAR_STRING_OFFSET              1264
#define CS_TITLE_STRING_OFFSET               1272
#define CS_ARMSIMAGE_STRING_OFFSET           1280
#define CS_ARMS_STRING_OFFSET                1292
#define CS_ITEMIMAGE_STRING_OFFSET           1300
#define CS_STAGEIMAGE_STRING_OFFSET          1312
#define CS_NPC_NPCSYM_STRING_OFFSET          1324
#define CS_NPC_NPCREGU_STRING_OFFSET         1336
#define CS_TEXTBOX_STRING_OFFSET             1348
#define CS_CARET_STRING_OFFSET               1356
#define CS_BULLET_STRING_OFFSET              1364
#define CS_FACE_STRING_OFFSET                1372
#define CS_FADE_STRING_OFFSET                1380
#define CS_CREDIT01_STRING_OFFSET            1388
#define CS_PIXTONE_STRING_OFFSET             1400
#define CS_DOUKUTSU_STRING_OFFSET            1456
#define CS_DATA_STRING_OFFSET                1468
#define CS_0_STRING_OFFSET                   1504
#define CS_PXM_STRING_OFFSET                 1660
#define CS_PROFILE_DAT_STRING_OFFSET         1792
#define CS_STAGESELECT_TSC_STRING_OFFSET     1904
#define CS_STAGE_STRING_OFFSET               2004

#define CS_PRT_FOLDER_STRING_OFFSET          2012
#define CS_PXA_FOLDER_STRING_OFFSET          2024

#define CS_PXM_FOLDER_STRING_OFFSET          2036
#define CS_PXE_FOLDER_STRING_OFFSET          2048
#define CS_TSC_FOLDER_STRING_OFFSET          2060

#define CS_NPC_STRING_OFFSET                 2076
#define CS_NPC_FOLDER_STRING_OFFSET          2080
#define CS_NPC_FOLDER_STRING_OFFSET2         2092
#define CS_XXXX_STRING_OFFSET                2104
// ORG name offsets here
#define CS_HEAD_TSC_STRING_OFFSET            2472
#define CS_WINDOW_STRING_OFFSET             12240

#define CS_LOADING_STRING_OFFSET             1532


//ORG name offsets
#define CS_ORG_WANPAKU_STRING_OFFSET         2112
#define CS_ORG_ANZEN_STRING_OFFSET           2120
#define CS_ORG_GAMEOVER_STRING_OFFSET        2128
#define CS_ORG_GAVITY_STRING_OFFSET          2140
#define CS_ORG_WEED_STRING_OFFSET            2148
#define CS_ORG_MDOWN2_STRING_OFFSET          2156
#define CS_ORG_FIREEYE_STRING_OFFSET         2164
#define CS_ORG_VIVI_STRING_OFFSET            2172
#define CS_ORG_MURA_STRING_OFFSET            2180
#define CS_ORG_FANFALE1_STRING_OFFSET        2188
#define CS_ORG_GINSUKE_STRING_OFFSET         2200
#define CS_ORG_CEMETERY_STRING_OFFSET        2208
#define CS_ORG_PLANT_STRING_OFFSET           2220
#define CS_ORG_KODOU_STRING_OFFSET           2228
#define CS_ORG_FANFALE3_STRING_OFFSET        2236
#define CS_ORG_FANFALE2_STRING_OFFSET        2248
#define CS_ORG_DR_STRING_OFFSET              2260
#define CS_ORG_ESCAPE_STRING_OFFSET          2264
#define CS_ORG_JENKA_STRING_OFFSET           2272
#define CS_ORG_MAZE_STRING_OFFSET            2280
#define CS_ORG_ACCESS_STRING_OFFSET          2288
#define CS_ORG_IRONH_STRING_OFFSET           2296
#define CS_ORG_GRAND_STRING_OFFSET           2304
#define CS_ORG_CURLY_STRING_OFFSET           2312
#define CS_ORG_OSIDE_STRING_OFFSET           2320
#define CS_ORG_REQUIEM_STRING_OFFSET         2328
#define CS_ORG_WANPAK2_STRING_OFFSET         2336
#define CS_ORG_QUIET_STRING_OFFSET           2344
#define CS_ORG_LASTCAVE_STRING_OFFSET        2352
#define CS_ORG_BALCONY_STRING_OFFSET         2364
#define CS_ORG_LASTBTL_STRING_OFFSET         2372
#define CS_ORG_LASTBT3_STRING_OFFSET         2380
#define CS_ORG_ENDING_STRING_OFFSET          2388
#define CS_ORG_ZONBIE_STRING_OFFSET          2396
#define CS_ORG_BDOWN_STRING_OFFSET           2404
#define CS_ORG_HELL_STRING_OFFSET            2412
#define CS_ORG_JENKA2_STRING_OFFSET          2420
#define CS_ORG_MARINE_STRING_OFFSET          2428
#define CS_ORG_BALLOS_STRING_OFFSET          2436
#define CS_ORG_TOROKO_STRING_OFFSET          2444
#define CS_ORG_WHITE_STRING_OFFSET           2452

//these are offsets into the .data section (0x08F000)
#define CS_SHOT_DATA_OFFSET                   116 //0x08F074
#define CS_WEAPON_LEVEL_OFFSET             0x466C //0x09366C

#define SC_WEAPON_JUMP_TABLE_OFFSET        
#define SC_ENTITY_JUMP_TABLE_OFFSET        0x9548

#define offset(x, y) (((char*)&x.y)-((char*)&x))

//for some reason these isn't being found by the compiler so I added it
DEFINE_GUID(ImageFormatBMP, 0xb96b3cab,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
DEFINE_GUID(ImageFormatPNG, 0xb96b3caf, 0x0728, 0x11d3, 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e);

