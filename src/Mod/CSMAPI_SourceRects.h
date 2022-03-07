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
  @file CSMAPI_SourceRects.h
*/

#pragma once

#define FRAME_BG_TILE_MARGIN 3

namespace GUI_SourceRects
{
	static GUI_RECT rc_BUTTON_Tab_Normal[]       = { { 63, 103, 3, 14 }, { 81, 103, 3, 14 }, { 72, 103, 3, 14 } /* Left, middle, right */ };
	static GUI_RECT rc_BUTTON_Tab_Hover[]        = { { 60, 103, 3, 14 }, { 78, 103, 3, 14 }, { 69, 103, 3, 14 } /* Left, middle, right */ };
	static GUI_RECT rc_BUTTON_Tab_Selected[]     = { { 57, 103, 3, 14 }, { 75, 103, 3, 14 }, { 66, 103, 3, 14 } /* Left, middle, right */ };
	static GUI_RECT rc_BUTTON_Generic_Selected[] = { { 18, 65, 1, 15 }, { 19, 65, 15, 15 }, { 34, 65, 2, 15 } /* Left, middle, right */ };
	static GUI_RECT rc_BUTTON_Generic_Normal[]   = { { 0, 65, 1, 15 }, { 1, 65, 15, 15 }, { 16, 65, 2, 15 } /* Left, middle, right */ };
	static GUI_RECT rc_BUTTON_Sizable_Normal[]   = { { 0, 65, 2, 2 }, { 2, 65, 14, 2 }, { 16, 65, 2, 2 }, { 0, 67, 2, 1 }, { 2, 67, 14, 1 }, { 16, 67, 2, 1 }, { 0, 78, 2, 2 }, { 2, 78, 14, 2 }, { 16, 78, 2, 2 } };
	static GUI_RECT rc_BUTTON_Sizable_Selected[] = { { 18, 65, 2, 2 }, { 20, 65, 14, 2 }, { 34, 65, 2, 2 }, { 18, 67, 2, 1 }, { 20, 67, 14, 1 }, { 34, 67, 2, 1 }, { 18, 78, 2, 2 }, { 20, 78, 14, 2 }, { 34, 78, 2, 2 } };
	static GUI_RECT rc_BUTTON_Sizable_Disabled[] = { { 18, 30, 2, 2 }, { 20, 30, 14, 2 }, { 34, 30, 2, 2 }, { 18, 32, 2, 1 }, { 20, 32, 14, 1 }, { 34, 32, 2, 1 }, { 18, 43, 2, 2 }, { 20, 43, 14, 2 }, { 34, 43, 2, 2 } };
	static GUI_RECT rc_BUTTON_RecordBar_Play[]   = { { 22, 0, 11, 11 }, { 22, 11, 11, 11 }, { 22, 22, 11, 11 }, { 22, 33, 11, 11 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_BUTTON_RecordBar_Pause[]  = { { 33, 0, 11, 11 }, { 33, 11, 11, 11 }, { 33, 22, 11, 11 }, { 33, 33, 11, 11 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_BUTTON_RecordBar_Save[]   = { { 44, 0, 11, 11 }, { 44, 11, 11, 11 }, { 44, 22, 11, 11 }, { 44, 33, 11, 11 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_BUTTON_RecordBar_Load[]   = { { 55, 0, 11, 11 }, { 55, 11, 11, 11 }, { 55, 22, 11, 11 }, { 55, 33, 11, 11 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_BUTTON_RecordBar_Delete[] = { { 121, 0, 11, 11 }, { 121, 11, 11, 11 }, { 121, 22, 11, 11 }, { 121, 33, 11, 11 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_BUTTON_RecordBar_FR[]     = { { 66, 0, 11, 11 }, { 66, 11, 11, 11 }, { 66, 22, 11, 11 }, { 66, 33, 11, 11 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_BUTTON_RecordBar_FF[]     = { { 77, 0, 11, 11 }, { 77, 11, 11, 11 }, { 77, 22, 11, 11 }, { 77, 33, 11, 11 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_BUTTON_RecordBar_Record[] = { { 88, 0, 11, 11 }, { 88, 11, 11, 11 }, { 88, 22, 11, 11 }, { 88, 33, 11, 11 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_BUTTON_RecordBar_Stop[]   = { { 99, 0, 11, 11 }, { 99, 11, 11, 11 }, { 99, 22, 11, 11 }, { 99, 33, 11, 11 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_BUTTON_TitleModSelect[]   = { { 142, 43, 7, 7 }, { 142, 50, 7, 7 } }; // SURFACE_ID_GUI
	static GUI_RECT rc_BUTTON_Options[]          = { { 136, 237, 13, 14 }, { 136, 251, 13, 14 } }; // SURFACE_ID_GUI
	static GUI_RECT rc_FRAME_RecordTimeline_L[]  = { { 11, 0, 4, 4 }, { 15, 0, 3, 4 }, { 18, 0, 4, 4 }, { 11, 4, 4, 3 }, { 15, 4, 3, 3 }, { 18, 4, 4, 3 }, { 11, 7, 4, 4 }, { 15, 7, 3, 4 }, { 18, 7, 4, 4 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_FRAME_RecordTimeline_R[]  = { { 0, 33, 3, 3 }, { 3, 33, 1, 3 }, { 4, 33, 3, 3 }, { 0, 36, 3, 5 }, { 3, 36, 1, 5 }, { 4, 36, 3, 5 }, { 0, 41, 3, 3 }, { 3, 41, 1, 3 }, { 4, 41, 3, 3 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_FRAME_RecordTimeline_PG[] = { { 11, 11, 4, 4 }, { 15, 11, 3, 4 }, { 18, 11, 4, 4 }, { 11, 15, 4, 3 }, { 15, 15, 3, 3 }, { 18, 15, 4, 3 }, { 11, 18, 4, 4 }, { 15, 18, 3, 4 }, { 18, 18, 4, 4 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_FRAME_RecordTimeline_CT[] = { { 12, 34, 2, 9 }, { 14, 34, 3, 9 }, { 19, 34, 2, 9 } }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_FRAME_Window[]            = { { 0, 0, 244, 8 }, { 0, 8, 244, 8 }, { 0, 16, 244, 8 } }; // Top, middle, bottom
	static GUI_RECT rc_FRAME_MessageBox[]        = { { 0, 45, 10, 10 }, { 10, 45, 10, 10 }, { 20, 45, 10, 10 }, { 0, 55, 10, 7 }, { 10, 55, 10, 7 }, { 20, 55, 10, 7 }, { 0, 62, 10, 3 }, { 10, 62, 10, 3 }, { 20, 62, 10, 3 } };
	static GUI_RECT rc_FRAME_MessageBox_Top[]    = { { 0, 45, 3, 3 }, { 3, 45, 24, 3 }, { 27, 45, 3, 3 }, { 0, 48, 3, 1 }, { 3, 48, 24, 1 }, { 27, 48, 3, 1 }, { 0, 54, 3, 1 }, { 3, 54, 24, 1 }, { 27, 54, 3, 1 } };
	static GUI_RECT rc_FRAME_MessageBox_Bottom[] = { { 0, 55, 3, 3 }, { 3, 55, 24, 3 }, { 27, 55, 3, 3 }, { 0, 55, 3, 1 }, { 3, 55, 24, 1 }, { 27, 55, 3, 1 }, { 0, 62, 3, 3 }, { 3, 62, 24, 3 }, { 27, 62, 3, 3 } };
	static GUI_RECT rc_FRAME_TextBox[]           = { { 0, 0, 244, 8 }, { 0, 8, 244, 8 }, { 0, 16, 244, 8 } };
	static GUI_RECT rc_FRAME_Tooltip[]           = { { 46, 103, 2, 1}, { 48, 103, 7, 1 }, { 55, 103, 2, 1 }, { 46, 104, 2, 5 }, { 48, 104, 7, 5 }, { 55, 104, 2, 5 }, { 46, 109, 2, 2 }, { 48, 109, 7, 2 }, { 55, 109, 2, 2 } };
	static GUI_RECT rc_FRAME_EscapeBox[]         = { { 88, 32, 3, 3 }, { 91, 32, 24, 3 }, { 115, 32, 3, 3 }, { 88, 35, 3, 14 }, { 91, 35, 24, 14 }, { 115, 35, 3, 14 }, { 88, 49, 3, 3 }, { 91, 49, 24, 3 }, { 115, 49, 3, 3 } };
	static GUI_RECT rc_FRAME_ListEntryBox[]      = { { 140, 32, 2, 2 }, { 142, 32, 7, 2 }, { 149, 32, 2, 2 }, { 140, 34, 2, 1 }, { 142, 34, 7, 1 }, { 149, 34, 2, 1 }, { 140, 41, 2, 2 }, { 142, 41, 7, 2 }, { 149, 41, 2, 2 } };
	static GUI_RECT rc_FRAME_ListEntryBox_Sel[]  = { { 73, 92, 2, 2 }, { 75, 92, 7, 2 }, { 82, 92, 2, 2 }, { 73, 94, 2, 1 }, { 75, 94, 7, 1 }, { 82, 94, 2, 1 }, { 73, 101, 2, 2 }, { 75, 101, 7, 2 }, { 82, 101, 2, 2 } };
	static GUI_RECT rc_FRAME_EmoteWindow[]       = { { 98, 14, 2, 2 }, { 99, 14, 1, 2 }, { 100, 14, 2, 2 }, { 98, 16, 2, 1 }, { 99, 16, 1, 1 }, { 100, 16, 2, 1 }, { 98, 17, 2, 2 }, { 99, 17, 1, 2 }, { 100, 17, 2, 2 } };
	static GUI_RECT rc_IMAGE_SignalStrength[]    = { { 0, 0, 11, 8 }, { 55, 0, 11, 8 }, { 66, 0, 11, 8 }, { 77, 0, 11, 8 }, { 88, 0, 11, 8 } };
	static GUI_RECT rc_IMAGE_LocatingSignal[]    = { { 11, 0, 11, 8 }, { 22, 0, 11, 8 }, { 33, 0, 11, 8 }, { 44, 0, 11, 8 }, { 33, 0, 11, 8 }, { 22, 0, 11, 8 } };
	static GUI_RECT rc_IMAGE_NoSignal            = { 99, 0, 11, 8 }; // SURFACE_ID_GUI
	static GUI_RECT rc_IMAGE_LagIcon             = { 118, 32, 11, 11 }; // SURFACE_ID_GUI
	static GUI_RECT rc_IMAGE_DeleteButton[]      = { { 91, 63, 11, 11 }, { 102, 63, 11, 11 } /* Not selected, Selected*/ };
	static GUI_RECT rc_IMAGE_TeleportUpButton[]  = { { 22, 117, 11, 11 }, { 33, 117, 11, 11 } /* Not selected, Selected*/ };
	static GUI_RECT rc_IMAGE_TeleportDnButton[]  = { { 113, 52, 11, 11 }, { 124, 52, 11, 11 } /* Not selected, Selected*/ };
	static GUI_RECT rc_IMAGE_ReviveButton[]      = { { 113, 63, 11, 11 }, { 124, 63, 11, 11 } /* Not selected, Selected*/ };
	static GUI_RECT rc_IMAGE_SpectateButton[]    = { { 0, 117, 11, 11 }, { 11, 117, 11, 11 } /* Not selected, Selected*/ };
	static GUI_RECT rc_IMAGE_HealthBarBG[]       = { { 0, 40, 24, 8 }, { 24, 40, 39, 8 }, { 63, 40, 1, 8 } }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_HealthBarFill[]     = { { 0, 24, 2, 6 }, { 2, 24, 227, 6 }, { 229, 24, 2, 6 } }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_EnergyBarBG[]       = { { 0, 72, 2, 8 }, { 2, 72, 36, 8 }, { 38, 72, 2, 8 } }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_EnergyBarFill[]     = { { 0, 81, 2, 5 }, { 2, 81, 35, 5 }, { 37, 81, 2, 5 } }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_EnergyFull          = { 40, 72, 39, 7 }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_HealthBarDead       = { 128, 160, 39, 7 }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_Zero                = { 0, 56, 8, 8 }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_Lvl                 = { 81, 81, 24, 7 }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_Slash               = { 72, 48, 8, 8 }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_Dashes              = { 81, 48, 15, 16 }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_FrameBGTile         = { 167, 160, 8, 8 }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_TooltipArrow        = { 49, 111, 5, 5 }; // SURFACE_ID_GUI
	static GUI_RECT rc_IMAGE_Pointer             = { 113, 90, 13, 10 }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_PointerDark         = { 129, 90, 13, 10 }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_MenuIcon            = { 129, 32, 11, 11 }; // SURFACE_ID_GUI
	static GUI_RECT rc_IMAGE_BtnDelete[]         = { { 91, 63, 11, 11 }, { 102, 63, 11, 11 } }; // SURFACE_ID_GUI
	static GUI_RECT rc_IMAGE_BtnEdit[]           = { { 91, 52, 11, 11 }, { 102, 52, 11, 11 } }; // SURFACE_ID_GUI
	static GUI_RECT rc_IMAGE_BtnJoin[]           = { { 91, 74, 11, 11 }, { 102, 74, 11, 11 } }; // SURFACE_ID_GUI
	static GUI_RECT rc_IMAGE_ChatBubble          = { 0, 0, 16, 14 }; // SURFACE_ID_CHAT_TOPICS
	static GUI_RECT rc_IMAGE_InMenu              = { 157, 32, 9, 9 }; // SURFACE_ID_GUI
	static GUI_RECT rc_IMAGE_IsAdmin             = { 157, 68, 9, 9 }; // SURFACE_ID_GUI
	static GUI_RECT rc_IMAGE_MinControls_Sel     = { 73, 28, 15, 11 }; // SURFACE_ID_CONTROLS
	static GUI_RECT rc_IMAGE_MinControls_Tab     = { 63, 18, 15, 11 }; // SURFACE_ID_CONTROLS
	static GUI_RECT rc_IMAGE_MinControls_None    = { 77, 18, 11, 11 }; // SURFACE_ID_CONTROLS
	static GUI_RECT rc_IMAGE_InfoIcon            = { 157, 41, 9, 9 }; // SURFACE_ID_GUI
	static GUI_RECT rc_IMAGE_DownloadingIcon     = { 157, 50, 9, 9 }; // SURFACE_ID_GUI
	static GUI_RECT rc_IMAGE_RecordBar_LeftTrim  = { 0, 11, 6, 11 }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_IMAGE_RecordBar_RightTrim = { 0, 22, 6, 11 }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_IMAGE_BigRecordDot        = { 0, 0, 11, 11 }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_IMAGE_RecTimeline_Marker  = { 11, 22, 11, 11 }; // SURFACE_ID_DEMO_EDITOR
	static GUI_RECT rc_IMAGE_PasswordIcon        = { 0, 8, 11, 11 }; // SURFACE_ID_GUI
	static GUI_RECT rc_IMAGE_ArmsTitle           = { 80, 48, 64, 8 }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_ItemTitle           = { 80, 56, 64, 8 }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_LevelAbbrev         = { 80, 80, 16, 8 }; // SURFACE_ID_TEXT_BOX
	static GUI_RECT rc_IMAGE_TextBoxTop          = { 80, 80, 16, 8 }; // SURFACE_ID_TEXT_BOX
}