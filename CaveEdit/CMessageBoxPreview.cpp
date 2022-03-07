#include "stdafx.h"
#include "CaveEditor.h"
#include "CMessageBoxPreview.h"
#include "afxdialogex.h"
#include "General.h"
#include "CaveNet_Config.h"
#include "CSMAPI_enums.h"
#include "SoundHelper.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CMessageBoxPreview, CDialog)

BEGIN_MESSAGE_MAP(CMessageBoxPreview, CDialog)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONUP()
	ON_WM_GETDLGCODE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define TIMER_ID_PROCESSLOOP	12

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

IStream* CreateStreamOnResource(LPCSTR lpName, LPCSTR lpType)
{
	IStream* ipStream = NULL;
	HRSRC hrsrc;
	DWORD dwResourceSize;
	HGLOBAL hglbImage;
	LPVOID pvSourceResourceData;
	HGLOBAL hgblResourceData;
	LPVOID pvResourceData;

	hrsrc = FindResource(GetModuleHandle(NULL), lpName, lpType);
	if (hrsrc == NULL)
		goto Return;

	dwResourceSize = SizeofResource(GetModuleHandle(NULL), hrsrc);
	hglbImage = LoadResource(GetModuleHandle(NULL), hrsrc);
	if (hglbImage == NULL)
		goto Return;

	pvSourceResourceData = LockResource(hglbImage);
	if (pvSourceResourceData == NULL)
		goto Return;

	hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
	if (hgblResourceData == NULL)
		goto Return;

	pvResourceData = GlobalLock(hgblResourceData);
	if (pvResourceData == NULL)
		goto FreeData;

	CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);

	GlobalUnlock(hgblResourceData);
	if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
		goto Return;

FreeData:
	GlobalFree(hgblResourceData);

Return:
	return ipStream;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CMessageBoxPreview::CMessageBoxPreview(CWnd* pParent) : CDialog(TSCDLG_MESSAGE_BOX, pParent)
{
	m_ScriptPtr         = NULL;
	m_Init              = false;
	memset(&m_TextInfo, 0, sizeof(m_TextInfo));
}

CMessageBoxPreview::~CMessageBoxPreview()
{
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMessageBoxPreview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define CSM_RGBA(r,g,b,a)								(COLORREF)((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))
BOOL CMessageBoxPreview::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ScriptUpdated = true;
	m_Key    = 0;
	m_KeyTrg = 0;
	m_KeyOld = 0;

	m_TextInfo.mode = 1;

	m_TextInfo.rcText.left   = 14;
	m_TextInfo.rcText.top    = 25;
	m_TextInfo.rcText.right  = 230;
	m_TextInfo.rcText.bottom = m_TextInfo.rcText.top + 48;

	// Load the background image
	IStream* pStream = CreateStreamOnResource(MAKEINTRESOURCE(ICON_MESSAGEBOX), "PNG");
	if (pStream != nullptr)
	{
		if (!m_TextBoxImage.IsNull())
			m_TextBoxImage.Destroy();
		m_TextBoxImage.Load(pStream);
		m_TextBoxImage.SetHasAlphaChannel(true);
		pStream->Release();
	}
	
	pStream = CreateStreamOnResource(MAKEINTRESOURCE(BITMAP_GAME_SHEET), "PNG");
	if (pStream != nullptr)
	{
		if (!m_SpritesheetImage.IsNull())
			m_SpritesheetImage.Destroy();
		m_SpritesheetImage.Load(pStream);
		m_SpritesheetImage.SetHasAlphaChannel(true);
		pStream->Release();
	}

	// Resize the window
	WINDOWINFO pwi;
	pwi.cbSize = sizeof(WINDOWINFO);

	this->GetWindowInfo(&pwi);

	CRect rcWin_Win;
	CRect rcWin_Client;
	GetWindowRect(&rcWin_Win);
	GetClientRect(&rcWin_Client);
	int iClW = abs(rcWin_Client.Width() - rcWin_Win.Width());
	int iClH = abs(rcWin_Client.Height() - rcWin_Win.Height());
	rcWin_Win.right  = rcWin_Win.left + m_TextBoxImage.GetWidth() + iClW;
	rcWin_Win.bottom = rcWin_Win.top + m_TextBoxImage.GetHeight() + iClH;
	SetWindowPos(NULL, 0, 0, rcWin_Win.Width(), rcWin_Win.Height(), SWP_NOZORDER | SWP_NOREPOSITION);

	if (!m_Init)
	{
		m_Init = true;
		m_CheckBox_AutoFast.Create(CsmpCheckBox::IDD, this);
	}

	SizeContents();

	// Load the font
	m_Font_Text.CreateFontA(20, 10, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Courier New");

	m_TextInfo.LINE_BMP_1 = new CImage();
	m_TextInfo.LINE_BMP_2 = new CImage();
	m_TextInfo.LINE_BMP_3 = new CImage();
	m_TextInfo.LINE_BMP_4 = new CImage();
	m_SymbolImage         = new CImage();
	m_SymbolImage->Create(6, -6, 32, CImage::createAlphaChannel);
	m_TextInfo.LINE_BMP_1->Create(432, -32, 32, CImage::createAlphaChannel);
	m_TextInfo.LINE_BMP_2->Create(432, -32, 32, CImage::createAlphaChannel);
	m_TextInfo.LINE_BMP_3->Create(432, -32, 32, CImage::createAlphaChannel);
	m_TextInfo.LINE_BMP_4->Create(432, -32, 32, CImage::createAlphaChannel);
	//m_TextInfo.LINE_BMP_1->Create(432, -32, 24, 0);
	//m_TextInfo.LINE_BMP_2->Create(432, -32, 24, 0);
	//m_TextInfo.LINE_BMP_3->Create(432, -32, 24, 0);
	m_SymbolImage->SetTransparentColor(RGB(0, 0, 0));
	m_TextInfo.LINE_BMP_1->SetTransparentColor(RGB(0, 0, 0));
	m_TextInfo.LINE_BMP_2->SetTransparentColor(RGB(0, 0, 0));
	m_TextInfo.LINE_BMP_3->SetTransparentColor(RGB(0, 0, 0));
	m_TextInfo.LINE_BMP_4->SetTransparentColor(RGB(0, 0, 0));

	CDC* dc;
	dc = CDC::FromHandle(m_TextInfo.LINE_BMP_1->GetDC()); dc->FillSolidRect(0, 0, 432, 32, RGB(0, 0, 0)); m_TextInfo.LINE_BMP_1->ReleaseDC();
	dc = CDC::FromHandle(m_TextInfo.LINE_BMP_2->GetDC()); dc->FillSolidRect(0, 0, 432, 32, RGB(0, 0, 0)); m_TextInfo.LINE_BMP_2->ReleaseDC();
	dc = CDC::FromHandle(m_TextInfo.LINE_BMP_3->GetDC()); dc->FillSolidRect(0, 0, 432, 32, RGB(0, 0, 0)); m_TextInfo.LINE_BMP_3->ReleaseDC();
	dc = CDC::FromHandle(m_TextInfo.LINE_BMP_4->GetDC()); dc->FillSolidRect(0, 0, 432, 32, RGB(0, 0, 0)); m_TextInfo.LINE_BMP_4->ReleaseDC();

	dc = CDC::FromHandle(m_SymbolImage->GetDC());
	dc->FillSolidRect(0, 0, 6, 6, RGB(255, 255, 255));
	dc->SetPixel(5, 0, RGB(0, 0, 0));
	dc->SetPixel(5, 5, RGB(0, 0, 0));
	dc->SetPixel(0, 5, RGB(0, 0, 0));
	dc->SetPixel(0, 0, RGB(0, 0, 0));
	dc->SetPixel(1, 0, RGB(160, 181, 222));
	dc->SetPixel(0, 1, RGB(160, 181, 222));
	dc->SetPixel(4, 0, RGB(160, 181, 222));
	dc->SetPixel(5, 1, RGB(160, 181, 222));
	dc->SetPixel(0, 4, RGB(160, 181, 222));
	dc->SetPixel(1, 5, RGB(160, 181, 222));
	dc->SetPixel(4, 5, RGB(160, 181, 222));
	dc->SetPixel(5, 4, RGB(160, 181, 222));
	m_SymbolImage->ReleaseDC();

	for (int i = 0; i < 4; i++)
	{
		m_TextInfo.ypos_line[i] = 16 * i;
	}

	m_ProcTimer = NULL;

	return TRUE;
}

void CMessageBoxPreview::OnDestroy()
{
	CDialog::OnDestroy();

	m_CheckBox_AutoFast.DestroyWindow();
	m_SymbolImage->Destroy();
	m_TextInfo.LINE_BMP_1->Destroy();
	m_TextInfo.LINE_BMP_2->Destroy();
	m_TextInfo.LINE_BMP_3->Destroy();
	delete m_TextInfo.LINE_BMP_1;
	delete m_TextInfo.LINE_BMP_2;
	delete m_TextInfo.LINE_BMP_3;
	m_TextInfo.LINE_BMP_1 = NULL;
	m_TextInfo.LINE_BMP_2 = NULL;
	m_TextInfo.LINE_BMP_3 = NULL;

	m_Font_Text.DeleteObject();
	m_TextBoxImage.Destroy();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned char CMessageBoxPreview::GetScriptPtr(int iOffset)
{
	if (!m_ScriptPtrGui)
		return (m_ScriptPtr ? m_ScriptPtr[iOffset] : 0);

	if (iOffset < 0 || iOffset >= m_ScriptPtrGui->GetTextLength())
		return 0;

	if (m_ScriptUpdated)
	{
		m_ScriptUpdated = false;
		char diff;
		m_ScriptString.Empty();
		m_ScriptPtrGui->GetTextRange(0, m_ScriptPtrGui->GetTextLength(), m_ScriptString);

		int i = 0;
		int j = 0;

		//this adds back in the new line carriage return pair that's removed when editing
		while (i >=0 && i < m_ScriptString.GetLength())
		{
			j = m_ScriptString.Find('\r', i) + 2;
			if (j == 1 || j < i)
				break;
			else
				i = j;

			if (m_ScriptString[i-1] != '\n')
				m_ScriptString.Insert(i-1, '\n');
		}
		i = m_ScriptString.Find('\r', i);
		diff = m_ScriptString[m_ScriptString.Find('\r', 0)+1];
	}

	return m_ScriptString[iOffset];
}

void CMessageBoxPreview::ExecuteEvent(int iEventNo)
{
	m_TextInfo.mode          = 1;
	m_TextInfo.line          = 0;
	m_TextInfo.p_write       = 0;
	m_TextInfo.wait          = 4;
	m_TextInfo.flags         = 0;
	m_TextInfo.wait_beam     = 0;
	m_TextInfo.face          = 0;
	m_TextInfo.offsetY       = 0;
	m_TextInfo.current_event = iEventNo;

	m_TextInfo.rcText.left   = 28;
	m_TextInfo.rcText.top    = 20;
	m_TextInfo.rcText.right  = 230;
	m_TextInfo.rcText.bottom = m_TextInfo.rcText.top + 48;

	//Clear gTSText lines
	CDC* dc;
	dc = CDC::FromHandle(m_TextInfo.LINE_BMP_1->GetDC()); dc->FillSolidRect(0, 0, 432, 32, RGB(0, 0, 0)); m_TextInfo.LINE_BMP_1->ReleaseDC();
	dc = CDC::FromHandle(m_TextInfo.LINE_BMP_2->GetDC()); dc->FillSolidRect(0, 0, 432, 32, RGB(0, 0, 0)); m_TextInfo.LINE_BMP_2->ReleaseDC();
	dc = CDC::FromHandle(m_TextInfo.LINE_BMP_3->GetDC()); dc->FillSolidRect(0, 0, 432, 32, RGB(0, 0, 0)); m_TextInfo.LINE_BMP_3->ReleaseDC();
	dc = CDC::FromHandle(m_TextInfo.LINE_BMP_4->GetDC()); dc->FillSolidRect(0, 0, 432, 32, RGB(0, 0, 0)); m_TextInfo.LINE_BMP_4->ReleaseDC();

	//Find where event starts
	if (m_ScriptPtrGui)
	{
		GetScriptPtr(0);
		char pBuff[6];
		sprintf(pBuff, "#%04d", iEventNo);
		m_TextInfo.p_read = m_ScriptString.Find(pBuff, 0);
		if (m_TextInfo.p_read < 0)
			m_TextInfo.p_read = 0;
	}
	else
	{
		m_TextInfo.p_read = 0;
		while (1)
		{
			//Check if we are still in the proper range
			if (GetScriptPtr(m_TextInfo.p_read) == '\0')
				return;

			//Check if we are at an event
			if (GetScriptPtr(m_TextInfo.p_read) == '#')
			{
				//Check if this is our event
				int event_no = GetTextScriptNo(++m_TextInfo.p_read);

				if (iEventNo == event_no)
					break;
				if (iEventNo < event_no)
					return;
			}

			++m_TextInfo.p_read;
		}
	}

	//Advance until new-line
	while (GetScriptPtr(m_TextInfo.p_read) != '\n' && GetScriptPtr(m_TextInfo.p_read) != 0)
		++m_TextInfo.p_read;
	++m_TextInfo.p_read;
}

int CMessageBoxPreview::GetTextScriptNo(int a)
{
	int b = 0;
	b += (GetScriptPtr(a++) - '0') * 1000;
	b += (GetScriptPtr(a++) - '0') * 100;
	b += (GetScriptPtr(a++) - '0') * 10;
	b += GetScriptPtr(a) - '0';
	return b;
}

void CMessageBoxPreview::ClearTextLine()
{
	m_TextInfo.line    = 0;
	m_TextInfo.p_write = 0;
	m_TextInfo.offsetY = 0;
	CDC* dc;

	CImage* pLineBitmaps[] = { m_TextInfo.LINE_BMP_1, m_TextInfo.LINE_BMP_2, m_TextInfo.LINE_BMP_3, m_TextInfo.LINE_BMP_4 };
	for (int i = 0; i < 4; i++)
	{
		m_TextInfo.ypos_line[i] = 16 * i;
		dc = CDC::FromHandle(pLineBitmaps[i]->GetDC());
		dc->FillSolidRect(0, 0, 432, 32, RGB(0, 0, 0));
		pLineBitmaps[i]->ReleaseDC();
		memset(&m_TextInfo.gTSText[i * 0x40], 0, 0x40);
	}
}

void CMessageBoxPreview::CheckNewLine()
{
	CImage* pLineBitmaps[] = { m_TextInfo.LINE_BMP_1, m_TextInfo.LINE_BMP_2, m_TextInfo.LINE_BMP_3, m_TextInfo.LINE_BMP_4 };

	if (m_TextInfo.ypos_line[m_TextInfo.line % 4] == 48)
	{
		m_TextInfo.mode = 3;
		CDC* dc         = CDC::FromHandle(pLineBitmaps[m_TextInfo.line % 4]->GetDC()); dc->FillSolidRect(0, 0, 432, 32, RGB(0, 0, 0)); pLineBitmaps[m_TextInfo.line % 4]->ReleaseDC();
		memset(&m_TextInfo.gTSText[m_TextInfo.line % 4 * 0x40], 0, 0x40);
	}
}

void CMessageBoxPreview::ProcessTextBox()
{
	if (!m_ScriptPtr)
		return;

	bool show_blink = (m_TextInfo.wait_beam++ % 20 > 12);
	if (show_blink != m_TextInfo.show_blink)
	{
		m_TextInfo.show_blink = show_blink;
		Invalidate(TRUE);
	}

	if (m_TextInfo.face_x < (28 * 0x200))
	{
		m_TextInfo.face_x += min((28 * 0x200) - m_TextInfo.face_x, 0x1000);
		Invalidate(TRUE);
	}

	if (!GetScriptPtr(m_TextInfo.p_read))
		return;

	m_KeyTrg = m_Key & (m_Key ^ m_KeyOld);
	m_KeyOld = m_Key;

BEGIN_PROCESS_LOOP:
	int text_offset;
	if (m_TextInfo.face)
		text_offset = 56;
	else
		text_offset = 0;

	//Set textbox position
	if (m_TextInfo.flags & 0x20)
	{
		m_TextInfo.rcText.top    = 20;
		m_TextInfo.rcText.bottom = m_TextInfo.rcText.top + 48;
	}
	else
	{
		m_TextInfo.rcText.top    = 20;
		m_TextInfo.rcText.bottom = m_TextInfo.rcText.top + 48;
	}

	BOOL bExit = FALSE;

	char c[3];
	int x, y, z, w, length;
	int dbg = 0;
	switch (m_TextInfo.mode)
	{
		case 0:
		{
			m_DesiredOffset = 0;
			break;
		}
		case 1:
		{
			++m_TextInfo.wait;

			if (m_CheckBox_AutoFast.m_bCheckBoxValue || (m_Key & (KeyPress::KP_OK | KeyPress::KP_CANCEL)))
				m_TextInfo.wait += 4;

			if (m_TextInfo.wait < 4)
				break;

			m_TextInfo.wait = 0;

			while (!bExit)
			{
				if (dbg++ >= 100)
					break;

				if (GetScriptPtr(m_TextInfo.p_read) == '<')
				{
					char pCommand[5];
					pCommand[0] = GetScriptPtr(m_TextInfo.p_read + 1);
					pCommand[1] = GetScriptPtr(m_TextInfo.p_read + 2);
					pCommand[2] = GetScriptPtr(m_TextInfo.p_read + 3);
					pCommand[3] = 0;

					if (!memcmp(pCommand, "TUR", 3))
					{
						m_TextInfo.p_read += 4;
						m_TextInfo.flags  |= 0x10;
						m_TextInfo.p_tur_x = 0;
					}
					else if (
						!memcmp(pCommand, "MSG", 3) || 
						!memcmp(pCommand, "MS2", 3) ||
						!memcmp(pCommand, "MS3", 3)
						)
					{
						m_TextInfo.p_tur_x = 0;
						m_TextInfo.flags |= 0x03;
						m_TextInfo.flags &= ~0x30;
						if (m_TextInfo.flags & 0x40)
							m_TextInfo.flags |= 0x10;
						m_TextInfo.p_read += 4;
						ClearTextLine();
					}
					else if (!memcmp(pCommand, "WAI", 3))
					{
						if (m_DesiredOffset)
						{
							m_TextInfo.p_read += 8;
							m_TextInfo.wait_next = GetTextScriptNo(m_TextInfo.p_read + 4);
							goto DECREMENT_LEFTOVER;
						}
						
						m_TextInfo.mode      = 4;
						m_TextInfo.wait_next = GetTextScriptNo(m_TextInfo.p_read + 4);
						m_TextInfo.p_read   += 8;
						bExit = TRUE;
					}
					else if (
						!memcmp(pCommand, "END", 3) ||
						!memcmp(pCommand, "ESC", 3)
						)
					{
						ClearTextLine();
						m_TextInfo.mode = 0;
						m_TextInfo.face = 0;
						m_DesiredOffset = 0;
						bExit = TRUE;
					}
					else if (!memcmp(pCommand, "NOD", 3))
					{
						if (m_DesiredOffset)
						{
							m_TextInfo.p_read += 4;
							goto DECREMENT_LEFTOVER;
						}

						m_TextInfo.mode    = 2;
						m_TextInfo.p_read += 4;
						bExit = TRUE;
					}
					else if (!memcmp(pCommand, "YNJ", 3))
					{
						if (m_DesiredOffset)
						{
							m_TextInfo.next_event = GetTextScriptNo(m_TextInfo.p_read + 4);
							m_TextInfo.wait       = 0;
							m_TextInfo.select     = 0;
							m_TextInfo.p_read    += 8;
							goto DECREMENT_LEFTOVER;
						}

						if (!m_DesiredOffset)
							SoundHelper::PlaySoundObject(SoundHelper::SoundType::ST_PXTONE, SoundHelper::PlayMode::PM_STOP_THEN_PLAY, nullptr, 5);

						m_TextInfo.next_event = GetTextScriptNo(m_TextInfo.p_read + 4);
						m_TextInfo.p_read    += 8;
						m_TextInfo.mode       = 6;
						m_TextInfo.wait       = 0;
						m_TextInfo.select     = 0;
						bExit                 = TRUE;
					}
					else if (!memcmp(pCommand, "FAC", 3))
					{
						z = GetTextScriptNo(m_TextInfo.p_read + 4);
						if (m_TextInfo.face != (signed char)z)
						{
							m_TextInfo.face   = (signed char)z;
							//(WINDOW_WIDTH / 2 - 156) * 0x200
							m_TextInfo.face_x = -34 * 0x200;
						}
						m_TextInfo.p_read += 8;
					}
					else if (!memcmp(pCommand, "CLO", 3))
					{
						m_TextInfo.flags  &= ~0x33;
						m_TextInfo.p_read += 4;
						m_TextInfo.p_tur_x = 0;
						ClearTextLine();
					}
					else if (!memcmp(pCommand, "CLR", 3))
					{
						ClearTextLine();
						m_TextInfo.p_tur_x = 0;
						m_TextInfo.p_read += 4;
					}
					else if (!memcmp(pCommand, "EVE", 3))
					{
						z = GetTextScriptNo(m_TextInfo.p_read + 4);
						ExecuteEvent(z);
						m_DesiredOffset = 0;
						m_DesiredOffset = -1;
						bExit           = TRUE;
					}
					else
					{
						pCommand[4] = 0;
						pCommand[3] = pCommand[2];
						pCommand[2] = pCommand[1];
						pCommand[1] = pCommand[0];
						pCommand[0] = '<';
						for (TSC_Info* walk = TSC_Info::first; walk; walk = walk->next)
						{
							int iFindIndex    = walk->Find(pCommand);
							TSC_struct* pInfo = (iFindIndex == -1 ? NULL : &walk->cmd[iFindIndex]);

							if (!pInfo || !pInfo->args)
								continue;

							m_TextInfo.p_read += (4 * pInfo->args) + (pInfo->args - 1);
							break;
						}

						m_TextInfo.p_read += 4;
					}

				DECREMENT_LEFTOVER:
					if (m_DesiredOffset)
						--m_DesiredOffset;
				}
				else
				{
					const char* pDataBuffer = m_ScriptPtr;
					unsigned int* pRead     = &m_TextInfo.p_read;

					if (GetScriptPtr(*pRead) == '\r')
					{
						//Go to new-line
						(*pRead)   += 2;
						m_TextInfo.p_write = 0;
						m_TextInfo.p_tur_x = 0;

						if (m_TextInfo.flags & 1)
						{
							++m_TextInfo.line;
							CheckNewLine();
						}
					}
					else if (GetScriptPtr(*pRead) == '\n')
					{
						//Go to new-line
						(*pRead)   += 1;
						m_TextInfo.p_write = 0;
						m_TextInfo.p_tur_x = 0;

						if (m_TextInfo.flags & 1)
						{
							++m_TextInfo.line;
							CheckNewLine();
						}
					}
					else if (m_TextInfo.flags & 0x10)
					{
						//SAT/CAT/TUR printing
						char str[72];

						x = (*pRead);
						//Break if reaches command, or new-line
						while (GetScriptPtr(x) != '<' && GetScriptPtr(x) != '\r' && GetScriptPtr(x) != 0)
						{
							//Skip if SHIFT-JIS
							if (GetScriptPtr(x) & 0x80)
								++x;

							++x;
						}

						//Get m_TextInfo.gTSText to copy
						length = x - (*pRead);
						for (int ll = 0; ll < length; ll++)
							str[ll] = GetScriptPtr((*pRead) + ll);
						str[length] = 0;

						if (pDataBuffer == m_ScriptPtr)
							m_TextInfo.p_write = x;
						else
							m_TextInfo.p_write += length;

						//Print m_TextInfo.gTSText
						PutLineText(m_TextInfo.p_tur_x, 0, str, RGB(0xFF, 0xFF, 0xFE), m_TextInfo.line % 4, false);
						sprintf(&m_TextInfo.gTSText[m_TextInfo.line % 4 * 0x40], str);

						//Check if should move to next line (prevent a memory overflow, come on guys, this isn't a leftover of pixel trying to make m_TextInfo.gTSText wrapping)
						m_TextInfo.p_tur_x += GetTextWidth(str, length);
						(*pRead)           += length;

						if (m_TextInfo.p_write >= 35)
							CheckNewLine();

						bExit = TRUE;
					}
					else
					{
						//Get m_TextInfo.gTSText to print
						c[0] = GetScriptPtr(*pRead);

						if (c[0] & 0x80)
						{
							c[1] = GetScriptPtr(*pRead + 1);
							c[2] = '\0';
						}
						else
						{
							c[1] = '\0';
						}

						//Print m_TextInfo.gTSText
						PutLineText(6 * m_TextInfo.p_write, 0, c, RGB(0xFF, 0xFF, 0xFE), m_TextInfo.line % 4);

						strcat(&m_TextInfo.gTSText[m_TextInfo.line % 4 * 0x40], c);
						if (!m_DesiredOffset)
							SoundHelper::PlaySoundObject(SoundHelper::SoundType::ST_PXTONE, SoundHelper::PlayMode::PM_STOP_THEN_PLAY, nullptr, 2);
					
						m_TextInfo.wait_beam = 0;

						//Offset read and write positions
						if (c[0] & 0x80)
						{
							(*pRead) += 2;
							m_TextInfo.p_write += 2;
						}
						else
						{
							(*pRead)++;
							m_TextInfo.p_write++;
						}

						if (m_TextInfo.p_write >= 35)
						{
							CheckNewLine();
							m_TextInfo.p_write = 0;
							++m_TextInfo.line;
							CheckNewLine();
						}

						bExit = TRUE;
					}
				}
			}
			break;
		}
		case 2: //NOD
		{
			if (m_KeyTrg & (KeyPress::KP_CANCEL | KeyPress::KP_OK))
				m_TextInfo.mode = 1;
			
			break;
		}
		case 3: //NEWLINE
		{
			for (int i = 0; i < 4; i++)
			{
				m_TextInfo.ypos_line[i] -= 4;

				if (!m_TextInfo.ypos_line[i])
					m_TextInfo.mode = 1;

				if (m_TextInfo.ypos_line[i] == -16)
					m_TextInfo.ypos_line[i] = 48;
			}

			Invalidate(TRUE);
			break;
		}
		case 4: //WAI
		{
			if (m_TextInfo.wait_next == 9999)
				break;

			if (m_TextInfo.wait != 9999)
				++m_TextInfo.wait;

			if (m_TextInfo.wait < m_TextInfo.wait_next)
				break;

			m_TextInfo.mode      = 1;
			m_TextInfo.wait_beam = 0;
			break;
		}
		case 6: //YNJ
		{
			if (m_TextInfo.wait < 16)
			{
				m_TextInfo.wait++;
				Invalidate(TRUE);
			}
			else
			{
				//Select option
				if (m_KeyTrg & KeyPress::KP_OK)
				{
					if (!m_DesiredOffset)
						SoundHelper::PlaySoundObject(SoundHelper::SoundType::ST_PXTONE, SoundHelper::PlayMode::PM_STOP_THEN_PLAY, nullptr, 18);

					if (m_TextInfo.select == 1)
					{
						ExecuteEvent(m_TextInfo.next_event);
					}
					else
					{
						m_TextInfo.mode = 1;
						m_TextInfo.wait_beam = 0;
					}

					Invalidate(TRUE);
				}
				//Yes
				else if (m_KeyTrg & KeyPress::KP_LEFT)
				{
					m_TextInfo.select = 0;
					if (!m_DesiredOffset)
						SoundHelper::PlaySoundObject(SoundHelper::SoundType::ST_PXTONE, SoundHelper::PlayMode::PM_STOP_THEN_PLAY, nullptr, 1);
					Invalidate(TRUE);
				}
				//No
				else if (m_KeyTrg & KeyPress::KP_RIGHT)
				{
					m_TextInfo.select = 1;
					if (!m_DesiredOffset)
						SoundHelper::PlaySoundObject(SoundHelper::SoundType::ST_PXTONE, SoundHelper::PlayMode::PM_STOP_THEN_PLAY, nullptr, 1);
					Invalidate(TRUE);
				}
			}

			break;
		}
	}

	if (m_DesiredOffset)
	{
		bExit = FALSE;
		dbg   = 0;
		goto BEGIN_PROCESS_LOOP;
	}
}

void CMessageBoxPreview::PutLineText(int x, int y, const char* text, unsigned long color, int iLineIndex, bool center)
{
	CImage* bmptr = NULL;
	switch (iLineIndex)
	{
		case 0: { bmptr = m_TextInfo.LINE_BMP_1; break; }
		case 1: { bmptr = m_TextInfo.LINE_BMP_2; break; }
		case 2: { bmptr = m_TextInfo.LINE_BMP_3; break; }
		case 3: { bmptr = m_TextInfo.LINE_BMP_4; break; }
		default: return;
	}

	// Determine the draw rect
	CRect rcDraw   = { x * 2, y * 2, 0, 0 };
	CSize txtSize  = { 0, 0 };
	CDC* dc        = CDC::FromHandle(bmptr->GetDC());
	CFont* oldfont = (CFont*)dc->SelectObject(m_Font_Text);

	// Size up the string
	txtSize = dc->GetTextExtent(text, strlen(text));

	// Set new extent
	rcDraw.right  = rcDraw.left + txtSize.cx;
	rcDraw.bottom = rcDraw.top + txtSize.cy;

	// Draw the line
	if (*text == '=')
	{
		rcDraw.right  = rcDraw.left + (6 * 2);
		rcDraw.top   += 4;
		rcDraw.bottom = rcDraw.top + (6 * 2);
		m_SymbolImage->StretchBlt(*dc, rcDraw);
	}
	else
	{
		dc->SetTextColor(color);
		dc->DrawText(text, strlen(text), rcDraw, 0);
	}

	// Done!
	bmptr->ReleaseDC();

	Invalidate(TRUE);
}

int CMessageBoxPreview::GetTextWidth(const char* pString, int iLength)
{
	CPaintDC dc(this);

	// Determine the draw rect
	CSize textSize = { 0, 0 };
	CFont* oldfont = (CFont*)dc.SelectObject(m_Font_Text);

	// Size up the string
	GetTextExtentPoint(dc, pString, iLength, &textSize);

	return textSize.cx;
}

void CMessageBoxPreview::SizeContents()
{
	CRect rcWin;
	GetWindowRect(&rcWin);
	GetParent()->ScreenToClient(&rcWin);
	
	m_CheckBox_AutoFast.SetWindowPos(NULL, rcWin.Width() - 24, rcWin.Height() - 24, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMessageBoxPreview::OnOK()
{
}

void CMessageBoxPreview::OnCancel()
{
}

void CMessageBoxPreview::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
	
	// Determine what type of event this is
	switch (nIDEvent)
	{
		case TIMER_ID_PROCESSLOOP:
		{
			ProcessTextBox();
			break;
		}
	}
}

void CMessageBoxPreview::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if (bShow && !m_ProcTimer)
	{
		m_DesiredOffset = -1;
		m_ProcTimer     = SetTimer(TIMER_ID_PROCESSLOOP, 1, NULL);
	}
	else if (!bShow && m_ProcTimer)
	{
		KillTimer(m_ProcTimer);
		m_ProcTimer = NULL;
	}
}

void CMessageBoxPreview::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (!m_Init)
		return;

	SizeContents();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct KEYPRESS_INFO
{
	char nChar;
	int iKeyFlag;
} gKeyPressInfoList[] =
{
	{ 'Z',  CMessageBoxPreview::KeyPress::KP_OK },
	{ 'X',  CMessageBoxPreview::KeyPress::KP_CANCEL },
	{ '%',  CMessageBoxPreview::KeyPress::KP_LEFT },
	{ '\'', CMessageBoxPreview::KeyPress::KP_RIGHT },
};

void CMessageBoxPreview::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	for (int i = 0; i < sizeof(gKeyPressInfoList) / sizeof(KEYPRESS_INFO); i++)
	{
		KEYPRESS_INFO* pInfo = &gKeyPressInfoList[i];

		// Skip mismatches
		if (pInfo->nChar != nChar)
			continue;

		m_Key &= ~pInfo->iKeyFlag;
		return;
	}

	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CMessageBoxPreview::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nFlags & (1 << 14))
	{
		CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	for (int i = 0; i < sizeof(gKeyPressInfoList) / sizeof(KEYPRESS_INFO); i++)
	{
		KEYPRESS_INFO* pInfo = &gKeyPressInfoList[i];

		// Skip mismatches
		if (pInfo->nChar != nChar)
			continue;

		m_Key |= pInfo->iKeyFlag;
		return;
	}

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMessageBoxPreview::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonUp(nFlags, point);

	SetFocus();
}

UINT CMessageBoxPreview::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMessageBoxPreview::RedrawTextBox(CDC* cdc, CRect* rect)
{
	// Draw the background color
	cdc->FillSolidRect(0, 0, rect->Width(), rect->Height(), RGB(240, 240, 240));

	// Draw the background image
	m_TextBoxImage.Draw(*cdc, 0, 0, m_TextBoxImage.GetWidth(), m_TextBoxImage.GetHeight(), 0, 0, m_TextBoxImage.GetWidth(), m_TextBoxImage.GetHeight());

	int text_offset;
	if (m_TextInfo.face)
		text_offset = 112;
	else
		text_offset = 0;

	//Draw NOD cursor
	if (m_TextInfo.show_blink && m_TextInfo.mode == 2)
	{
		CRect rect;

		rect.left   = 28 + 6 * (m_TextInfo.p_write * 2) + text_offset;
		rect.top    = (m_TextInfo.ypos_line[m_TextInfo.line % 4] * 2) + m_TextInfo.rcText.top + m_TextInfo.offsetY;
		rect.right  = rect.left + 10;
		rect.bottom = rect.top + 22;
		
		cdc->FillSolidRect(&rect, RGB(0xFF, 0xFF, 0xFE));
	}

	//Draw face picture
	CRect rcFace;
	rcFace.left   = 48 * (m_TextInfo.face % 6);
	rcFace.top    = 48 * (m_TextInfo.face / 6);
	rcFace.right  = rcFace.left + 48;
	rcFace.bottom = rcFace.top + 48;

	CRgn textrect;
	textrect.CreateRectRgn(m_TextInfo.rcText.left, 20, 479, 119);
	cdc->SelectClipRgn(&textrect);

	// Draw the face
	exe.spriteSheet[SURFACE_ID_FACE].Draw(*cdc, (m_TextInfo.face_x / 0x200), 14, 48 * 2, 48 * 2, rcFace.left, rcFace.top, rcFace.Width(), rcFace.Height());

	// Draw the lines
	m_TextInfo.LINE_BMP_1->Draw(*cdc, text_offset + 28, m_TextInfo.offsetY + (m_TextInfo.ypos_line[0] * 2) + m_TextInfo.rcText.top);
	m_TextInfo.LINE_BMP_2->Draw(*cdc, text_offset + 28, m_TextInfo.offsetY + (m_TextInfo.ypos_line[1] * 2) + m_TextInfo.rcText.top);
	m_TextInfo.LINE_BMP_3->Draw(*cdc, text_offset + 28, m_TextInfo.offsetY + (m_TextInfo.ypos_line[2] * 2) + m_TextInfo.rcText.top);
	m_TextInfo.LINE_BMP_4->Draw(*cdc, text_offset + 28, m_TextInfo.offsetY + (m_TextInfo.ypos_line[3] * 2) + m_TextInfo.rcText.top);
	
	cdc->SelectClipRgn(NULL);
	textrect.DeleteObject();

	// Draw the YNJ dialog
	if (m_TextInfo.mode == 6)
	{
		CRect rect_yesno(0, 20, 168, 84);
		CRect rect_cur(12, 0, 38, 20);

		int i;
		if (m_TextInfo.wait < 2)
			i = (rect->Height() - rect_yesno.Height()) + (2 - m_TextInfo.wait) * 8;
		else
			i = (rect->Height() - rect_yesno.Height());

		CRect rect_yesno_dest(rect->Width() - rect_yesno.Width() - 7 - 12, i - 7, 0, 0);
		CRect rect_cur_dest((82 * m_TextInfo.select) + rect_yesno_dest.left - 8, rect_yesno_dest.top + 24, rect_cur.Width(), rect_cur.Height());
		rect_yesno_dest.right  = rect_yesno_dest.left + rect_yesno.Width();
		rect_yesno_dest.bottom = rect_yesno_dest.top + rect_yesno.Height();
		rect_cur_dest.right    = rect_cur_dest.left + rect_cur.Width();
		rect_cur_dest.bottom   = rect_cur_dest.top + rect_cur.Height();

		// Draw it
		m_SpritesheetImage.Draw(*cdc, rect_yesno_dest, rect_yesno);
		if (m_TextInfo.wait == 16)
			m_SpritesheetImage.Draw(*cdc, rect_cur_dest, rect_cur);
	}
}

void CMessageBoxPreview::OnPaint()
{
	CRect rect;

	GetWindowRect(&rect);
	ScreenToClient(&rect);

	CPaintDC dc(this);

	CDC cdc;
	cdc.CreateCompatibleDC(&dc);

	CBitmap bmpFinal;
	int w = m_TextBoxImage.GetWidth();
	int h = m_TextBoxImage.GetHeight();
	bmpFinal.CreateBitmap(w, h, 1, 32, NULL);

	CBitmap* pOldBmp = (CBitmap*)(cdc.SelectObject(&bmpFinal));

	RedrawTextBox(&cdc, &rect);

	dc.BitBlt(0, 0, w, h, &cdc, 0, 0, SRCCOPY);

	(cdc.SelectObject(&pOldBmp));
	bmpFinal.DeleteObject();
	cdc.DeleteDC();
}

BOOL CMessageBoxPreview::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------