// ScriptEditorDlg.cpp : implementation file
//


#include "stdafx.h"
#include "CaveEditor.h"
#include "General.h"
#include "ScriptEditorDlg.h"
#include ".\scripteditordlg.h"
#include "Sound.h"
#include "Organya.h"
#include "SoundLoad.h"
#include "MapTransferDialog.h"
#include "CaveMapEditDlg.h"

//any color
#define EVENT_COLOR    RGB(  0,  0,  0)
#define COMMAND_COLOR  RGB(  0,  0,255)
#define NUMBER_COLOR   RGB(128,  0,128)
#define SPACER_COLOR   RGB(128,128,128)
#define INVALID_COLOR  RGB(255,  0,  0)

//bold or not bold
#define EVENT_STYLE    CFM_BOLD
#define COMMAND_STYLE  0
#define NUMBER_STYLE   0
#define SPACER_STYLE   CFM_BOLD
#define INVALID_STYLE  0

// ScriptEditorDlg dialog

IMPLEMENT_DYNAMIC(ScriptEditorDlg, CDialog)
ScriptEditorDlg::ScriptEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ScriptEditorDlg::IDD, pParent)
{
	currentCommandSize = 0;
	currentCommandIndex = 0;
	currentCommandPos = 0;
	scriptMapIndex = 0;
	scriptText = NULL;
	scriptTextLen = 0;
	init = false;
	help = true;
	width = 0;
	height = 0;
}

ScriptEditorDlg::~ScriptEditorDlg()
{
	if (scriptText != NULL)
		delete[] scriptText;
}

void ScriptEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, EDIT_SCRIPT, scriptControl);
	DDX_Control(pDX, LIST_COMMANDS, commandListControl);
}


BEGIN_MESSAGE_MAP(ScriptEditorDlg, CDialog)
	ON_BN_CLICKED(TEXT_SCRDET_BUTTON0, OnBnClickedScript0)
	ON_BN_CLICKED(TEXT_SCRDET_BUTTON1, OnBnClickedScript1)
	ON_BN_CLICKED(TEXT_SCRDET_BUTTON2, OnBnClickedScript2)
	ON_BN_CLICKED(BUTTON_SYNTAX, OnBnClickedSyntax)
	ON_BN_CLICKED(BUTTON_SAVE, OnBnClickedSave)
	ON_LBN_SELCHANGE(LIST_COMMANDS, OnLbnSelchangeCommands)
	ON_LBN_DBLCLK(LIST_COMMANDS, OnLbnDblclkCommands)
	ON_BN_CLICKED(BUTTON_DETAILS, OnBnClickedDetails)
	ON_EN_CHANGE(EDIT_SCRIPT, OnEnChangeScript)
	ON_NOTIFY(EN_SELCHANGE, EDIT_SCRIPT, OnEnSelchangeScript)
	ON_WM_DROPFILES()
	ON_EN_UPDATE(EDIT_SCRIPT, OnEnUpdateScript)
	ON_WM_SIZE()
	ON_BN_CLICKED(BUTTON_EDIT_COMMAND, &ScriptEditorDlg::OnBnClickedEditCommand)
	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	ON_BN_CLICKED(BUTTON_SHOWMSGBOX, &ScriptEditorDlg::OnBnClickedShowmsgbox)
END_MESSAGE_MAP()


BOOL ScriptEditorDlg::SetArgBoxImage(int index, const char* dataPath, int img_index, int w, int h)
{
	extern CString EXEpath;
	CImage* bmpPtr  = NULL;
	CString ourStr  = EXEpath;
	ourStr         += dataPath;
	CStatic* box    = (CStatic*)GetDlgItem(IDC_SCREDT_ARGBOX0 + index);
	CRect boxsize;
	CBitmap bitmap;
	CImage image;
	CImage img;
	int startX;
	int startY;

	box->GetClientRect(&boxsize);

	// Pick the bitmap pointer location
	if (index == 0) bmpPtr = &argBoxImage0;
	else if (index == 1) bmpPtr = &argBoxImage1;
	else if (index == 2) bmpPtr = &argBoxImage2;
	else return FALSE;

	// Clear the old image
	if (!bmpPtr->IsNull())
		bmpPtr->Destroy();

	// Load the image
	if (FAILED(image.Load(_T(ourStr.GetString()))))
	{
		printf("Failed to load \"%s\"!\r\n", ourStr.GetBuffer());
		return FALSE;
	}

	// Calculate some variables
	startX = (img_index % (image.GetWidth() / w)) * w;
	startY = (img_index / (image.GetWidth() / w)) * h;

	bmpPtr->Create(boxsize.Width(), boxsize.Height(), image.GetBPP(), 0);

	if (w != h)
	{
		image.StretchBlt(bmpPtr->GetDC(), 0, (boxsize.Height() / 2) - (boxsize.Width() / 2), boxsize.Width(), boxsize.Width() / 2, startX, startY, w, h, SRCCOPY);
	}
	else
	{
		image.StretchBlt(bmpPtr->GetDC(), 0, 0, boxsize.Width(), boxsize.Height(), startX, startY, w, h, SRCCOPY);
	}

	bmpPtr->ReleaseDC();
	image.Destroy();

	// Set the bitmap
	box->ModifyStyle(0xF, SS_BITMAP, SWP_NOSIZE);
	box->SetBitmap(*bmpPtr);

	// Done!
	return TRUE;
}

int atoin(const char* pBuffer, int iLength)
{
	char buf[128];

	*buf = 0;
	strncat(buf, pBuffer, iLength);

	return atoi(buf);
}

bool TSC_GetEventBuffer(const char* scriptText, int iEventNo, char* pOut)
{
	char pEventToFind[6];
	sprintf(pEventToFind, "#%04d", iEventNo);

	const char* pStart = strstr((const char*)scriptText, pEventToFind);
	const char* pEnd   = (!pStart ? NULL : strchr(pStart + 1, '#'));

	if (pStart && !pEnd)
		pEnd = (const char*)scriptText + strlen((const char*)scriptText);

	if (!pStart || !pEnd)
		return false;

	*pOut = 0;
	strncat(pOut, pStart, pEnd - pStart);

	return true;
}

//#define FIND_COMMAND_DEBUG

#ifdef FIND_COMMAND_DEBUG
static int ifindIndent = 0;
#endif
bool TSC_FindCommandType(TSC_Info* pCmdInfo, const char* scriptText, int iEventNo, int iCommandType, char* pCommandOut)
{
	char pEventBuffer[2048];

	if (!TSC_GetEventBuffer(scriptText, iEventNo, pEventBuffer))
		return false;

	const char* pCmd_Start = strchr(pEventBuffer, '<');
	std::vector<int> pJumpEventList;

	pJumpEventList.clear();

#ifdef FIND_COMMAND_DEBUG
	for (int i = 0; i < ifindIndent; i++)
		printf(" ");
	printf("> Searching event %d for command_type %d:\r\n", iEventNo, iCommandType);

	ifindIndent += 3;
#endif

	// Loop through all commands
	while (pCmd_Start)
	{
		const char* pCmd_End = NULL;

		if (
			(pCmd_End = strchr(pCmd_Start + 1, '<')) == NULL &&
			(pCmd_End = strchr(pCmd_Start + 1, '\r')) == NULL &&
			(pCmd_End = strchr(pCmd_Start + 1, '\n')) == NULL
			)
				pCmd_End = pCmd_Start + strlen(pCmd_Start);

		for (int i = 0; i < pCmdInfo->count; i++)
		{
			TSC_struct* pInfo = &pCmdInfo->cmd[i];

			if (memcmp(pInfo->command, pCmd_Start, 4))
				continue;

#ifdef FIND_COMMAND_DEBUG
			for (int indent_idx = 0; indent_idx < ifindIndent; indent_idx++)
				printf(" ");
			printf("   > Trying \"%s\"...\r\n", pInfo->command);
#endif

			if (pInfo->command_type == iCommandType)
			{
				// Found it
				*pCommandOut = 0;
				strncat(pCommandOut, pCmd_Start, pCmd_End - pCmd_Start);
#ifdef FIND_COMMAND_DEBUG
				ifindIndent -= 3;
#endif
				return true;
			}

			if (pInfo->command_type == 1)
			{
				// Jump command! Add it to the test list
				int iJumpEventNo = 0;

				if (pInfo->args >= 1 && pInfo->types[0] == TSC_argType::tat_event)			iJumpEventNo = atoin(pCmd_Start + 4, 4);
				else if (pInfo->args >= 2 && pInfo->types[1] == TSC_argType::tat_event)		iJumpEventNo = atoin(pCmd_Start + 9, 4);
				else if (pInfo->args >= 3 && pInfo->types[2] == TSC_argType::tat_event)		iJumpEventNo = atoin(pCmd_Start + 14, 4);
				else if (pInfo->args >= 4 && pInfo->types[3] == TSC_argType::tat_event)		iJumpEventNo = atoin(pCmd_Start + 19, 4);

				if (iJumpEventNo)
					pJumpEventList.push_back(iJumpEventNo);
			}

			break;
		}

		// Find the next command
		if (*pCmd_End == '<')
			pCmd_Start = pCmd_End;
		else
			pCmd_Start = strchr(pCmd_End, '<');
	}

	// Check jump locations
	for (std::vector<int>::iterator it = pJumpEventList.begin(); it != pJumpEventList.end(); it++)
	{
		int iJumpEventNo = *it;

		if (TSC_FindCommandType(pCmdInfo, scriptText, iJumpEventNo, iCommandType, pCommandOut))
		{
#ifdef FIND_COMMAND_DEBUG
			ifindIndent -= 3;
#endif
			return true;
		}
	}

#ifdef FIND_COMMAND_DEBUG
	ifindIndent -= 3;
#endif

	return false;
}

int ScriptEditorDlg::Get_TRA_MapDest_FromEventNo(int iEventNo)
{
	if (!scriptText)
		return -1;

	char pCommand[128];
	if (!TSC_FindCommandType(&command, (const char*)scriptText, iEventNo, 2, pCommand))
		return -1;

	for (int i = 0; i < command.count; i++)
	{
		TSC_struct* pInfo = &command.cmd[i];

		if (memcmp(pInfo->command, pCommand, 4))
			continue;

		if (pInfo->args >= 1 && pInfo->types[0] == TSC_argType::tat_map)			return atoin(pCommand + 4, 4);
		else if (pInfo->args >= 2 && pInfo->types[1] == TSC_argType::tat_map)		return atoin(pCommand + 9, 4);
		else if (pInfo->args >= 3 && pInfo->types[2] == TSC_argType::tat_map)		return atoin(pCommand + 14, 4);
		else if (pInfo->args >= 4 && pInfo->types[3] == TSC_argType::tat_map)		return atoin(pCommand + 19, 4);
	}

	return -1;
}

// ScriptEditorDlg message handlers
BOOL ScriptEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, TRUE);		  // Set small icon

	RECT rect;
	GetClientRect(&rect);

	mMapEditor = NULL;
	sound0 = 0;
	sound1 = 0;
	sound2 = 0;
	width = rect.right;
	height = rect.bottom;

	GetWindowRect(&rect);
	ScreenToClient(&rect);

	m_MinWindowSize.x = rect.right - rect.left;
	m_MinWindowSize.y = rect.bottom - rect.top;

	mTextboxPreview.Create(CMessageBoxPreview::IDD, this);
	mTextboxPreview.ShowWindow(SW_HIDE);

	//m_Font.CreatePointFont(100, _T("Lucida Console"));
	scriptControl.SetEventMask(ENM_UPDATE | ENM_CHANGE | ENM_SELCHANGE);
	scriptControl.LimitText(500000);//maximum script length
	//scriptControl.SetFont(&m_Font);

	CHARFORMAT cf;
	scriptControl.GetDefaultCharFormat(cf);
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_FACE;
	strcpy(cf.szFaceName, "Lucida Console");
	scriptControl.SetDefaultCharFormat(cf);
	scriptControl.SetWordCharFormat(cf);

	format = true;
	init = true;

	char buffer[256];

	for (int i = 0; i < command.count; i++)
	{
		sprintf(buffer, "%s  -  %s", command.cmd[i].command, command.cmd[i].name);

		commandListControl.AddString(buffer);
	}

	SetDlgItemText(TEXT_SCRDET_TEXT0, "");
	SetDlgItemText(TEXT_SCRDET_TEXT1, "");
	SetDlgItemText(TEXT_SCRDET_TEXT2, "");
	SetDlgItemText(TEXT_SCRDET_INFO0, "");
	SetDlgItemText(TEXT_SCRDET_INFO1, "");
	SetDlgItemText(TEXT_SCRDET_INFO2, "");
	return TRUE;
}

void ScriptEditorDlg::OnBnClickedSyntax()
{
	int offset = 0;
	while (true)
	{
		try//<-- first time using these things
		{
			if (CheckSyntax(offset))
			{
				if (offset == 0)
					MessageBox("No Syntax Errors.");
				break;
			}
		}
		catch (ScriptError err)
		{
			scriptControl.SetSel(err.offset, err.offset+err.size);
			if (MessageBox(err.error, "Syntax Error", MB_OKCANCEL) == IDCANCEL)
				break;
			offset = err.offset + err.size;
		}
	}
	scriptControl.SetFocus();
}

void ScriptEditorDlg::OnBnClickedSave()
{
	Save();
	scriptControl.SetModify(FALSE);
}

void ScriptEditorDlg::OnLbnSelchangeCommands()
{
	displayCommand(commandListControl.GetCurSel());
}

void ScriptEditorDlg::OnLbnDblclkCommands()
{
	//paste command
	scriptControl.ReplaceSel(command.cmd[commandListControl.GetCurSel()].command);
	FormatText(true);
	//switch focus to edit box for immediate typing
	scriptControl.SetFocus();
}

void ScriptEditorDlg::OnBnClickedDetails()
{
	CString temp;
	RECT rect, rect2;
	int i;

	GetDlgItemText(BUTTON_DETAILS, temp);
	GetWindowRect(&rect2);

	GetDlgItem(FRAME_SCRIPT_HELP)->GetWindowRect(&rect);

	if (!help)
	{
		help = true;
		SetDlgItemText(BUTTON_DETAILS, "Hide Command Details");
		CalcWindowRect(&rect);
		rect2.right = rect.right;
	}
	else
	{
		help = false;
		SetDlgItemText(BUTTON_DETAILS, "Show Command Details");
		rect2.right = rect.left-rect.right;
		CalcWindowRect(&rect);
		rect2.right += rect.right;
	}

	MoveWindow(&rect2);
}

void ScriptEditorDlg::Open(char* file, int mapIndex)
{
	int i;
	format = false;
	strcpy(fileName, file);
	CFile scrFile;
	int shift = 0;
	char buffer[1024];

	mMapEditor = NULL;

	sprintf(buffer, "Script Editor - %s", file);
	SetWindowText(buffer);

	scrFile.Open(fileName, CFile::modeRead | CFile::shareDenyNone);
	size = scrFile.GetLength();

	if (mTextboxPreview.IsWindowVisible())
		OnBnClickedShowmsgbox();

	//resize string to hold entire script
	if (scriptText != NULL)
		delete[] scriptText;
	scriptText = new unsigned char [size+1];//+1 for the terminating character
	scriptTextLen = size;

	scrFile.Read(scriptText, size);
	scrFile.Close();

	shift = scriptText[size/2];//now shift all OTHER bytes by this amount

	for (i = 0; i < size; i++)//decode file
		if (i != size/2)
			scriptText[i] -= shift;

	scriptMapIndex = mapIndex;
	if (scriptMapIndex >= 0 && scriptMapIndex < exe.mod.mStages.count - 1)
		mCurrentMap.load(&CaveMapINFO(exe.mod.mStages.list[scriptMapIndex], &exe.mod));

	scriptText[size] = '\0';
	scriptTextLen = size;
	scriptControl.LineScroll(-scriptControl.GetFirstVisibleLine(), 0);//scroll to top
	scriptControl.SetWindowText((char*)scriptText);
	format = true;
	FormatText();
	
	mTextboxPreview.m_ScriptPtr     = (const char*)scriptText;
	mTextboxPreview.m_ScriptPtrGui  = &scriptControl;
	mTextboxPreview.m_ScriptUpdated = true;
	mTextboxPreview.m_DesiredOffset = 0;
}

void ScriptEditorDlg::Save()
{
	//this is an attempt to "throw" errors to see if it make more sense that passing them some other way
	try
	{
		CheckSyntax();
	}
	catch (ScriptError err)
	{
		char pErrorMsg[1024];

		sprintf(pErrorMsg, "Found a script error:\n\n\"%s\"\n\nAre you sure you want to save?", err.error.GetBuffer(), err.offset);

		if (MessageBox(pErrorMsg, 0, MB_YESNO) != IDYES)
			return;
	}


//1,461
	int i, j;
	CString buffer;

	char diff;
	scriptControl.GetTextRange(0, scriptControl.GetTextLength(), buffer);

	i = 0;
	//this adds back in the new line carriage return pair that's removed when editing
	while(i >=0 && i < buffer.GetLength())
	{
		j = buffer.Find('\r', i)+2;
		if (j == 1 || j < i)
			break;
		else
			i = j;

		if (buffer[i - 1] != '\n')
			buffer.Insert(i-1, '\n');
	}
	i = buffer.Find('\r', i);
	diff = buffer[buffer.Find('\r', 0)+1];

	size = buffer.GetLength();


	//resize string to hold entire script
	if (scriptText != NULL)
		delete[] scriptText;
	scriptText = new unsigned char [size+1];//+1 for the terminating character
	scriptTextLen = size;

	strcpy((char*)scriptText, buffer.GetString());

	int shift = scriptText[size/2];

	for (i = 0; i < size; i++)//encode file
		if (i != size/2)
			scriptText[i] += shift;

	CFileStatus status;
	if (CS_DEFUALT_READ_ONLY)
	{
		CFile::GetStatus(fileName, status);
		status.m_attribute = status.m_attribute & 0xFE;//this should remove read-only from file
		CFile::SetStatus(fileName, status);
	}

	CFile scrFile;
	CFileException e;
	if (scrFile.Open(fileName, CFile::modeWrite | CFile::modeCreate, &e))
	{
		scrFile.Write(scriptText, size);
		scrFile.Close();

		if (mMapEditor)
			mMapEditor->changes = true;
	}
	else
		MessageBox("Error writing to file!");

	for (i = 0; i < size; i++)//decode file
		if (i != size/2)
			scriptText[i] -= shift;

	// Get the map's name
	char pMapName[260];

	if (strstr(fileName, "Stage"))
	{
		char* pStart = (strrchr(fileName, '/') != NULL ? strrchr(fileName, '/') + 1 : (strrchr(fileName, '\\') != NULL ? strrchr(fileName, '\\') + 1 : fileName));
		strcpy(pMapName, pStart);

		if (strchr(pMapName, '.'))
			* strchr(pMapName, '.') = 0;

		// Look for the map in the stage list
		for (int i = 0; i < exe.mod.mStages.count; i++)
		{
			if (stricmp(exe.mod.mStages.list[i].file, pMapName))
				continue;

			// Compile it
			exe.CompileMaps(i, 1);
			break;
		}
	}
}

void ScriptEditorDlg::FormatText(bool line)
{
	//this first if statement is to prevent this function from being called after
	//every syntanx highlighting that's done which would certainly cause an infinite loop
	if (format == false)
		return;
	format = false;

	int i, j, k, end;
	CString buffer;
	CString temp;
	char extra[5];

	//prevent flicker
	int eventMask = ::SendMessage(scriptControl.m_hWnd, EM_SETEVENTMASK, 0, 0);
	scriptControl.HideSelection(TRUE, FALSE);
	scriptControl.SetRedraw(FALSE);

	/********* basic formating ***************
	All commands will appear in blue
	All text will appear in black
	All numbers will appear in cyan
	All spacers will appear in gray
	All invalid commands or improperly formated ones
	will appear in red
	events will appear in bold
	********** basic formating **************/

	CHARRANGE oldSel;
	scriptControl.GetSel(oldSel);

//	CHARRANGE sel;

	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);

	scriptControl.GetTextRange(0, scriptControl.GetTextLength(), buffer);

	if (!line)
	{

		scriptControl.SetSel(0, -1);
	//	strcpy(cf.szFaceName, "Courier New");
		strcpy(cf.szFaceName, "Lucida Console");//I think this looks best.
		//strcpy(cf.szFaceName, "Lucida Sans Unicode");//I think this looks best.
	//	strcpy(cf.szFaceName, "Fixedsys");
		cf.dwMask = CFM_FACE | CFM_BOLD | CFM_COLOR | CFM_SIZE;
		cf.yHeight = 180;
		cf.crTextColor =  RGB(0,0,0);
		cf.dwEffects = 0;
		scriptControl.SetSelectionCharFormat(cf);
		i = 0;
		end = buffer.GetLength();
	}
	else
	{
		i = scriptControl.LineIndex(scriptControl.LineFromChar(oldSel.cpMin)); 
		end = min(oldSel.cpMax+20, buffer.GetLength());

		cf.dwMask = CFM_FACE | CFM_BOLD | CFM_COLOR | CFM_SIZE;
		cf.yHeight = 180;
		scriptControl.SetSel(i, oldSel.cpMax+20);
		strcpy(cf.szFaceName, "Lucida Console");//I think this looks best.
		cf.crTextColor =  RGB(0,0,0);
		cf.dwEffects = 0;
		scriptControl.SetSelectionCharFormat(cf);
	}

	for (; i < end; i++)
	{
		if (buffer[i] < 32) continue;

		//if a # is found this is the begining of an event ID
		if (buffer[i] == '#' && i + 5 <= end)
		{
			scriptControl.SetSel(i+1,i+5);//select whole ID
			k = -1;
			temp = scriptControl.GetSelText();
			temp = CString((CStringW)temp);

			//this checks to see if there is a number for the event between the numbers 0 and 9999
			//if not then this event is incorrectly written and is flagged red
			if (1 != sscanf(temp, "%d%s", &k, extra) || k < 0 || k > 9999)
				cf.crTextColor =  INVALID_COLOR;
			else
				cf.crTextColor =  EVENT_COLOR;
			cf.dwEffects = EVENT_STYLE;
			scriptControl.SetSelectionCharFormat(cf);
			i += 4;
		}
		else if (buffer[i] == '<' && i + 4 <= end)//It's a command
		{
			scriptControl.SetSel(i,i+4);//select whole ID
			temp = scriptControl.GetSelText();
			temp = CString((CStringW)temp);
			j = command.Find(temp);

			if (j == -1)//command not found
			{
				cf.crTextColor =  INVALID_COLOR;
				cf.dwEffects = INVALID_STYLE;
				scriptControl.SetSelectionCharFormat(cf);
				i += 3;
			}
			else//Command Found
			{
				cf.crTextColor =  COMMAND_COLOR;
				cf.dwEffects = COMMAND_STYLE;
				scriptControl.SetSelectionCharFormat(cf);
				i += 3;
				j = command.cmd[j].args;//get number and convert to a number
				for (; j > 0; j--)
				{
					scriptControl.SetSel(i+1,i+5);//select whole ID
					k = -1;
					temp = scriptControl.GetSelText();

					if (1 != sscanf(temp, "%d%s", &k, extra) || k < 0 || k > 9999)
						cf.crTextColor =  INVALID_COLOR;
					else
						cf.crTextColor =  NUMBER_COLOR;
					cf.dwEffects = NUMBER_STYLE;
					scriptControl.SetSelectionCharFormat(cf);
					i+=4;
					if (j > 1)//account for spacer
					{
						i++;
						scriptControl.SetSel(i,i+1);//select whole ID
						cf.crTextColor =  SPACER_COLOR;
						cf.dwEffects = SPACER_STYLE;
						scriptControl.SetSelectionCharFormat(cf);
					}
				}
			}
		}
	}

	//Select old selection
	scriptControl.SetSel(oldSel);
	scriptControl.HideSelection(FALSE, FALSE);//show selection
	::SendMessage(scriptControl.m_hWnd, EM_SETEVENTMASK, 0, eventMask);
	scriptControl.SetRedraw(true);//allow redraw
	scriptControl.RedrawWindow();

	scriptControl.EmptyUndoBuffer();//prevent undo for now

	format = true;
}

bool ScriptEditorDlg::CheckSyntax(int i)
{
	int j, k, l, end;
	int last_event = 0;
	CString buffer;
	CString temp;
	char extra[32];

	ScriptError err;

	scriptControl.GetTextRange(0, scriptControl.GetTextLength(), buffer);

	end = buffer.GetLength();

	if (i != 0)
	{
		for (l = 0; l < i; l++)
		{
			//if a # is found this is the begining of an event ID
			if (buffer[l] == '#')
			{
				scriptControl.GetTextRange(l + 1, l + 5, temp);
				k = -1;
				if (1 == sscanf(temp, "%d%s", &k, extra))
					last_event = k;
			}
		}
	}

	for (; i < end; i++)
	{
		//if a # is found this is the begining of an event ID
		if (buffer[i] == '#')
		{
			scriptControl.GetTextRange(i+1, i+5, temp);
			k = -1;
			if (1 == sscanf(temp, "%d%s", &k, extra))
			{
				if (k < 0 || k > 9999)
				{
					err.offset = i;
					sprintf(extra, "%d", scriptControl.LineFromChar(err.offset));
					err.error = "Invalid Event Number @ Line # ";
					err.error += extra;
					err.size = 5;
					throw err;
					return false;
				}

				if (k < last_event)
				{
					err.offset = i;
					sprintf(extra, "%d", scriptControl.LineFromChar(err.offset));
					err.error = "Events should be ordered from least to greatest @ Line # ";
					err.error += extra;
					err.size = 5;
					throw err;
					return false;
				}

				last_event = k;
			}
			else
			{
				err.offset = i;
				sprintf(extra, "%d", scriptControl.LineFromChar(err.offset));
				err.error = "Invalid Event Number @ Line # ";
				err.error += extra;
				err.size = 5;
				throw err;
				return false;
			}
			
			i += 4;
		}
		else
		if (buffer[i] == '<')
		{
			scriptControl.GetTextRange(i,i+4, temp);//select whole ID
			j = command.Find(temp);

			if (j == -1)//command not found
			{
				err.offset = i;
				sprintf(extra, "%d", scriptControl.LineFromChar(err.offset));
				err.error = "Unknown Script Command @ Line # ";
				err.error += extra;
				err.size = 4;
				throw err;
				return false;
			}
			else//Command Found
			{
				i += 3;
				j = command.cmd[j].args;//get number and convert to a number
				for (; j > 0; j--)
				{
					scriptControl.GetTextRange(i+1,i+5, temp);//select whole ID
					k = -1;
					if (1 != sscanf(temp, "%d%s", &k, extra) || k < 0 || k > 9999)
					{
						err.offset = i+1;
						sprintf(extra, "%d", scriptControl.LineFromChar(err.offset));
						err.error = "Invalid Command Parameter @ Line # ";
						err.error += extra;
						err.size = 4;
						throw err;
						return false;
					}
					i+=4;
					if (j > 1)//account for spacer
						i++;
				}
			}
		}
	}
	return true;
}


void ScriptEditorDlg::OnEnChangeScript()
{
	if (mTextboxPreview.IsWindowVisible())
	{
		mTextboxPreview.m_ScriptUpdated = true;
		mTextboxPreview.ExecuteEvent(mTextboxPreview.m_TextInfo.current_event);
	}

	FormatText(true);
}

void ScriptEditorDlg::OnEnUpdateScript()
{
}

void ScriptEditorDlg::OnEnSelchangeScript(NMHDR *pNMHDR, LRESULT *pResult)
{
	SELCHANGE *pSelChange = reinterpret_cast<SELCHANGE *>(pNMHDR);
	*pResult = 0;

	CString temp;

	int i = pSelChange->chrg.cpMin;
	scriptControl.GetTextRange(i, i, temp);
	while (i >= 0 && temp[0] != '#')//not beginning of command
	{
		i--;
		scriptControl.GetTextRange(i, i+1, temp);
	};
	if (i != -1)
	{
		// Selected an event
		scriptControl.GetTextRange(i + 1, i + 5, temp);
		mTextboxPreview.ExecuteEvent(atoi(temp.GetBuffer()));
	}

	int eventstart = i;
	mTextboxPreview.m_DesiredOffset = 0;

	i = pSelChange->chrg.cpMin;
	scriptControl.GetTextRange(i, i, temp);
	while (i >= 0 && temp[0] != '<')//not beginning of command
	{
		i--;
		scriptControl.GetTextRange(i, i+1, temp);
	};
	if (i == -1)
		return;//no command found

	scriptControl.GetTextRange(i, i+4, temp);
	int argsStart = i + 4;

	i = command.Find(temp);
	if (i == -1)
		return;//not a command

	scriptControl.GetTextRange(eventstart, pSelChange->chrg.cpMin, temp);

	int iOffset = 0;
	int iTempN  = 0;
	while ((iTempN = temp.Find('<', iOffset)) != -1)
	{
		iOffset = iTempN + 1;
		++mTextboxPreview.m_DesiredOffset;
	}

	currentCommandPos   = argsStart - 4;
	currentCommandIndex = i;

	displayCommand(i, argsStart);
}

void ScriptEditorDlg::OnOK()
{
	CDialog::OnOK();
	OnCancel();
}

void ScriptEditorDlg::OnCancel()
{
	if (mTextboxPreview.IsWindowVisible())
	{
		OnBnClickedShowmsgbox();
		mTextboxPreview.ShowWindow(SW_HIDE);
	}

	CDialog::OnCancel();
}

void ScriptEditorDlg::OnBnClickedScript0()
{
	PlaySound(sound0);
}

void ScriptEditorDlg::OnBnClickedScript1()
{
	PlaySound(sound1);
}

void ScriptEditorDlg::OnBnClickedScript2()
{
	PlaySound(sound2);
}

void ScriptEditorDlg::PlaySound(int sound)
{
	if (sound >= gPtpTableCount)
	{
		if (IsOrganyaPlaying())
		{
			StopOrganyaMusic();
			return;
		}

		PlayOrganyaTrack(sound - gPtpTableCount);
	} else PlaySoundObject(sound, 1);
}

void ScriptEditorDlg::displayCommand(int i, int textStart)
{
	char buffer[26];

	SetDlgItemText(TEXT_COMMAND, command.cmd[i].name);
	SetDlgItemText(TEXT_DESC, command.cmd[i].desc);
	SetDlgItemText(TEXT_SCRDET_TEXT0, "");
	SetDlgItemText(TEXT_SCRDET_TEXT1, "");
	SetDlgItemText(TEXT_SCRDET_TEXT2, "");
	SetDlgItemText(TEXT_SCRDET_INFO0, "");
	SetDlgItemText(TEXT_SCRDET_INFO1, "");
	SetDlgItemText(TEXT_SCRDET_INFO2, "");
	SetDlgItemText(TEXT_SCRDET_BUTTON0, "");
	SetDlgItemText(TEXT_SCRDET_BUTTON1, "");
	SetDlgItemText(TEXT_SCRDET_BUTTON2, "");
	GetDlgItem(BUTTON_EDIT_COMMAND)->EnableWindow(FALSE);
	GetDlgItem(TEXT_SCRDET_BUTTON0)->ShowWindow(SW_HIDE);
	GetDlgItem(TEXT_SCRDET_BUTTON1)->ShowWindow(SW_HIDE);
	GetDlgItem(TEXT_SCRDET_BUTTON2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SCREDT_ARGBOX0)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SCREDT_ARGBOX1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SCREDT_ARGBOX2)->ShowWindow(SW_HIDE);
	GetDlgItem(TEXT_SCRDET_INFO0)->ShowWindow(SW_SHOW);
	GetDlgItem(TEXT_SCRDET_INFO1)->ShowWindow(SW_SHOW);
	GetDlgItem(TEXT_SCRDET_INFO2)->ShowWindow(SW_SHOW);

	if (textStart != -1) {
		if (command.cmd[i].inspectDlgIdx > 0) {
			// There is no case 0 here because 0 means there is no dialog associated with the command.
			GetDlgItem(BUTTON_EDIT_COMMAND)->EnableWindow(TRUE);
		}

		CString argStr0;
		CString argStr1;

		memset(currentCommandArgs, 0, sizeof(currentCommandArgs));

		int textOffset = textStart;
		int dispOffset = -1;
		int count      = (3 > command.cmd[i].args ? command.cmd[i].args : 3);
		for (int j = 0; j < count; j++)
		{
			TSC_argType type = command.cmd[i].types[j];
			dispOffset++;

			if (type == TSC_argType::tat_y_coord)
			{
				dispOffset--;
				count++;

				if (j + 1 >= command.cmd[i].args)
					break;

				continue;
			}

			// Get the arg string
			scriptControl.GetTextRange(textOffset, textOffset + 4, argStr0);
			textOffset += 5;

			currentCommandArgs[j] = atoi(argStr0.GetString());

			// Set label
			if (type == TSC_argType::tat_sound)
			{
				// Sound
				GetDlgItem(TEXT_SCRDET_BUTTON0 + dispOffset)->ShowWindow(SW_SHOW);
				SetDlgItemText(TEXT_SCRDET_BUTTON0 + dispOffset, TSC_argTypeNames[type]);

				if (dispOffset == 0) sound0 = atoi(argStr0.GetString());
				else if (dispOffset == 1) sound1 = atoi(argStr0.GetString());
				else if (dispOffset == 2) sound2 = atoi(argStr0.GetString());

				if (CS_DEFAULT_PLAY_SOUND)
					PlaySound(atoi(argStr0.GetString()));
			}
			else if (type == TSC_argType::tat_music)
			{
				// Music
				GetDlgItem(TEXT_SCRDET_BUTTON0 + dispOffset)->ShowWindow(SW_SHOW);
				SetDlgItemText(TEXT_SCRDET_BUTTON0 + dispOffset, TSC_argTypeNames[type]);

				if (dispOffset == 0) sound0 = gPtpTableCount + atoi(argStr0.GetString());
				else if (dispOffset == 1) sound1 = gPtpTableCount + atoi(argStr0.GetString());
				else if (dispOffset == 2) sound2 = gPtpTableCount + atoi(argStr0.GetString());
			}
			else if (type == TSC_argType::tat_x_coord)
			{
				// X, Y
				SetDlgItemText(TEXT_SCRDET_TEXT0 + dispOffset, "(x, y)");
			}
			else if(type == TSC_argType::tat_face)
			{
				// FACE
				GetDlgItem(IDC_SCREDT_ARGBOX0 + dispOffset)->ShowWindow(SW_SHOW);
				char Buffer[1024];
				sprintf(Buffer, "%s/Face.png", exe.mod.mModPath);
				SetArgBoxImage(dispOffset, Buffer, atoi(argStr0.GetString()), 48, 48);
				GetDlgItem(TEXT_SCRDET_INFO0 + dispOffset)->ShowWindow(SW_HIDE);
				break;
			}
			else if (type == TSC_argType::tat_graphic)
			{
				// FACE
				GetDlgItem(IDC_SCREDT_ARGBOX0 + dispOffset)->ShowWindow(SW_SHOW);
				char Buffer[1024];
				int iIndex = atoi(argStr0.GetString());
				if (iIndex >= 1000)
				{
					sprintf(Buffer, "%s/ItemImage.png", exe.mod.mModPath);
					iIndex -= 1000;
					SetArgBoxImage(dispOffset, Buffer, iIndex, 32, 16);
				}
				else
				{
					sprintf(Buffer, "%s/ArmsImage.png", exe.mod.mModPath);
					SetArgBoxImage(dispOffset, Buffer, iIndex, 16, 16);
				}

				GetDlgItem(TEXT_SCRDET_INFO0 + dispOffset)->ShowWindow(SW_HIDE);
				break;
			}
			else
			{
				SetDlgItemText(TEXT_SCRDET_TEXT0 + dispOffset, TSC_argTypeNames[type]);
			}
			
			switch (type)
			{
				case TSC_argType::tat_music:
				{
					int argNum = atoi(argStr0.GetString());
					SetDlgItemText(TEXT_SCRDET_INFO0 + dispOffset, (argNum >= exe.mod.mMusic.count ? "???" : exe.mod.mMusic.list[argNum].name));
					break;
				}
				case TSC_argType::tat_map:
				{
					int argNum = atoi(argStr0.GetString());
					SetDlgItemText(TEXT_SCRDET_INFO0 + dispOffset, argNum >= exe.mod.mStages.count ? "???" : exe.mod.mStages.list[argNum].name);
					break;
				}
				case TSC_argType::tat_npc_specific:
				{
					int argNum = atoi(argStr0.GetString());

					// Search for the entity with the code
					SetDlgItemText(TEXT_SCRDET_INFO0 + dispOffset, "???");
					for (int m = 0; m < mCurrentMap.entityNum; m++)
					{
						if (mCurrentMap.entity[m].event != argNum)
							continue;

						SetDlgItemText(TEXT_SCRDET_INFO0 + dispOffset, avar("%s\n%s", mNpcTable.extra[mCurrentMap.entity[m].npc].shortDesc1.GetString(), mNpcTable.extra[mCurrentMap.entity[m].npc].shortDesc2.GetString()));
						break;
					}
					break;
				}
				case TSC_argType::tat_npc_type:
				{
					int argNum = atoi(argStr0.GetString());

					if (argNum >= NPC_TYPE_COUNT * 2)
					{
						SetDlgItemText(TEXT_SCRDET_INFO0 + dispOffset, "(null)\n(null)");
					}
					else
					{
						SetDlgItemText(TEXT_SCRDET_INFO0 + dispOffset, avar("%s\n%s", mNpcTable.extra[argNum].shortDesc1.GetString(), mNpcTable.extra[argNum].shortDesc2.GetString()));
					}

					break;
				}
				case TSC_argType::tat_x_coord:
				{
					// Get Y COORD
					scriptControl.GetTextRange(textOffset, textOffset + 4, argStr1);
					textOffset += 5;
					currentCommandArgs[j + 1] = atoi(argStr1.GetString());

					SetDlgItemText(TEXT_SCRDET_INFO0 + dispOffset, avar("%d, %d", atoi(argStr0.GetString()), atoi(argStr1.GetString())));
					break;
				}
				case TSC_argType::tat_number:
				{
					int argNum = atoi(argStr0.GetString());
					SetDlgItemText(TEXT_SCRDET_INFO0 + dispOffset, avar("%04d", argNum));
					break;
				}
				default:
					if (TSC_argTypeLists[type].count == 0)
					{
						SetDlgItemText(TEXT_SCRDET_INFO0 + dispOffset, argStr0.GetString());
					}
					else
					{
						int argNum = atoi(argStr0.GetString());

						if (argNum > TSC_argTypeLists[type].count)
						{
							SetDlgItemText(TEXT_SCRDET_INFO0 + dispOffset, "???");
						}
						else
						{
							SetDlgItemText(TEXT_SCRDET_INFO0 + dispOffset, TSC_argTypeLists[type].list[argNum]);
						}
					}

					break;
			}
		}

		currentCommandSize = (textOffset + 3) - textStart;
	}

	switch (command.cmd[i].args)
	{
	case 0:
		strcpy(buffer, command.cmd[i].command);
		break;
	case 1:
		sprintf(buffer, "%sXXXX", command.cmd[i].command);
		break;
	case 2:
		sprintf(buffer, "%sXXXX:YYYY", command.cmd[i].command);
		break;
	case 3:
		sprintf(buffer, "%sXXXX:YYYY:ZZZZ", command.cmd[i].command);
		break;
	case 4:
		sprintf(buffer, "%sXXXX:YYYY:ZZZZ:WWWW", command.cmd[i].command);
		break;
	}

	SetDlgItemText(TEXT_SYNTAX, buffer);
}

void ScriptEditorDlg::OnDropFiles(HDROP hDropInfo)
{
	char fileName[512];
	CFile file;
	
	if (::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0) == 1)//only works with one file
	{
		::DragQueryFile(hDropInfo, 0, fileName, 512);

		Open(fileName, 0);
	}

	CDialog::OnDropFiles(hDropInfo);
}

#define MOVE_CONTROL_X(NID) \
			GetDlgItem(NID)->GetWindowRect(&rect);\
			ScreenToClient(&rect);\
			rect.MoveToX(rect.left + k);\
			GetDlgItem(NID)->MoveWindow(&rect);

#define MOVE_CONTROL_XH(NID) \
			GetDlgItem(NID)->GetWindowRect(&rect);\
			ScreenToClient(&rect);\
			rect.bottom += cy-height;\
			rect.MoveToX(rect.left + k);\
			GetDlgItem(NID)->MoveWindow(&rect);

#define MOVE_CONTROL_Y(NID) \
			GetDlgItem(NID)->GetWindowRect(&rect);\
			ScreenToClient(&rect);\
			rect.MoveToY(rect.top + cy - height);\
			GetDlgItem(NID)->MoveWindow(&rect);

void ScriptEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (cx == 0 || cy == 0)
		return;

	int i, j, k;
	CRect rect, rect2;
	if (init)
	{

		GetDlgItem(FRAME_SCRIPT_HELP)->GetWindowRect(&rect);
		ScreenToClient(&rect);

		k = rect.left;
		if (!help)
		{
			i = 0;
			j = rect.right-rect.left;
		}
		else
		{
			i = -(rect.right-rect.left);
			j = 0;
		}
		rect.left = cx+i;
		rect.right = cx+j;
		rect.bottom += cy - height;
		GetDlgItem(FRAME_SCRIPT_HELP)->MoveWindow(&rect);

		k = rect.left - k;//this is the horizontal shift for all help controls

		MOVE_CONTROL_XH(TEXT_SCRIPT_FRAME2);
		MOVE_CONTROL_XH(LIST_COMMANDS);

		if (k != 0)
		{
			MOVE_CONTROL_X(TEXT_SCRIPT_FRAME1);
			MOVE_CONTROL_X(TEXT_SCRIPT_COMMAND);
			MOVE_CONTROL_X(TEXT_COMMAND);
			MOVE_CONTROL_X(TEXT_SCRIPT_DESC);
			MOVE_CONTROL_X(TEXT_DESC);
			MOVE_CONTROL_X(TEXT_SYNTAX);
			MOVE_CONTROL_X(TEXT_SCRIPT_DETAILS);
			MOVE_CONTROL_X(TEXT_SCRDET_TEXT0);
			MOVE_CONTROL_X(TEXT_SCRDET_TEXT1);
			MOVE_CONTROL_X(TEXT_SCRDET_TEXT2);
			MOVE_CONTROL_X(TEXT_SCRDET_INFO0);
			MOVE_CONTROL_X(TEXT_SCRDET_INFO1);
			MOVE_CONTROL_X(TEXT_SCRDET_INFO2);
			MOVE_CONTROL_X(TEXT_SCRDET_BUTTON0);
			MOVE_CONTROL_X(TEXT_SCRDET_BUTTON1);
			MOVE_CONTROL_X(TEXT_SCRDET_BUTTON2);
			MOVE_CONTROL_X(IDC_SCREDT_ARGBOX0);
			MOVE_CONTROL_X(IDC_SCREDT_ARGBOX1);
			MOVE_CONTROL_X(IDC_SCREDT_ARGBOX2);

			GetDlgItem(TEXT_SCRIPT_FRAME1)->ShowWindow(SW_HIDE);
			GetDlgItem(TEXT_SCRIPT_FRAME1)->ShowWindow(SW_SHOW);
		}

		GetDlgItem(TEXT_SCRIPT_FRAME2)->ShowWindow(SW_HIDE);
		GetDlgItem(TEXT_SCRIPT_FRAME2)->ShowWindow(SW_SHOW);

		Invalidate(TRUE);

		GetDlgItem(EDIT_SCRIPT)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right += k;
		rect.bottom += cy - height;
		GetDlgItem(EDIT_SCRIPT)->MoveWindow(&rect);

		GetDlgItem(BUTTON_SYNTAX)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.MoveToY(rect.top + cy - height);
		GetDlgItem(BUTTON_SYNTAX)->MoveWindow(&rect);

		GetDlgItem(BUTTON_SAVE)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.MoveToY(rect.top + cy - height);
		GetDlgItem(BUTTON_SAVE)->MoveWindow(&rect);

		GetDlgItem(IDCANCEL)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.MoveToY(rect.top + cy - height);
		GetDlgItem(IDCANCEL)->MoveWindow(&rect);

		GetDlgItem(BUTTON_SHOWMSGBOX)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.left += k;
		rect.right += k;
		rect.MoveToY(rect.top + cy - height);
		GetDlgItem(BUTTON_SHOWMSGBOX)->MoveWindow(&rect);
		GetDlgItem(BUTTON_SHOWMSGBOX)->Invalidate(TRUE);

		GetDlgItem(BUTTON_EDIT_COMMAND)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.left += k;
		rect.right += k;
		rect.MoveToY(rect.top + cy - height);
		GetDlgItem(BUTTON_EDIT_COMMAND)->MoveWindow(&rect);
		GetDlgItem(BUTTON_EDIT_COMMAND)->Invalidate(TRUE);

		GetDlgItem(BUTTON_DETAILS)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.left += k;
		rect.right += k;
		rect.MoveToY(rect.top + cy - height);
		GetDlgItem(BUTTON_DETAILS)->MoveWindow(&rect);
		GetDlgItem(BUTTON_DETAILS)->Invalidate(TRUE);

		if (mTextboxPreview.IsWindowVisible())
		{
			CRect rcTextBox;
			CRect rcEdit;

			GetDlgItem(EDIT_SCRIPT)->GetWindowRect(&rcEdit);
			mTextboxPreview.GetWindowRect(&rcTextBox);
			ScreenToClient(&rcEdit);
			ScreenToClient(&rcTextBox);

			int iWidth       = rcTextBox.Width();
			rcTextBox.right  = rcEdit.left + ((rcEdit.Width() / 2) - (iWidth / 2)) + iWidth;
			rcTextBox.left   = rcEdit.left + ((rcEdit.Width() / 2) - (iWidth / 2));
			mTextboxPreview.SetWindowPos(NULL, rcTextBox.left, rcTextBox.top, rcTextBox.Width(), rcTextBox.Height(), SWP_NOZORDER);
			//mTextboxPreview.m_CheckBox_AutoFast.SetWindowPos(NULL, rcTextBox.left + rcTextBox.Width(), rcTextBox.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		}

		width = cx;
		height = cy;
	}
}


void ScriptEditorDlg::OnBnClickedEditCommand()
{
	CString Replacement = "";

	switch (command.cmd[currentCommandIndex].inspectDlgIdx)
	{
		case 1:
		{
			// Map transfer dialog
			CaveMap map;

			int err = 0;
			if ((err = map.load(&CaveMapINFO(exe.mod.mStages.list[scriptMapIndex], &exe.mod))) != 0)
			{
				printf("fixme: ScriptEditorDlg::displayCommand() - Failed to load map # %04d (\"%s\") -- Error Code # %04d\r\n", scriptMapIndex, exe.mod.mStages.list[scriptMapIndex].file, err);
				return;
			}

			// Ready the dialog
			MapTransferDialog TRAInspect;

			// Set the dialog's info
			TRAInspect.mMap              = &map;
			TRAInspect.mOldValues.mapidx = currentCommandArgs[0];
			TRAInspect.mOldValues.eveno  = currentCommandArgs[1];
			TRAInspect.mOldValues.xpos   = currentCommandArgs[2];
			TRAInspect.mOldValues.ypos   = currentCommandArgs[3];

			// Open it
			if (!TRAInspect.DoModal())
				return;

			Replacement.Format("%s%04d:%04d:%04d:%04d", command.cmd[currentCommandIndex].command, TRAInspect.mOldValues.mapidx, TRAInspect.mOldValues.eveno, TRAInspect.mOldValues.xpos, TRAInspect.mOldValues.ypos);
			break;
		}
		default:
		{
			printf("fixme: ScriptEditorDlg::displayCommand() - Command \"%s\" has unhandled inspectDlgIdx '%d'.\r\n", command.cmd[currentCommandIndex].name, command.cmd[currentCommandIndex].inspectDlgIdx);
			return;
		}
	}

	scriptControl.SetSel(currentCommandPos, currentCommandPos + currentCommandSize);
	scriptControl.ReplaceSel(Replacement.GetBuffer(), TRUE);
}


void ScriptEditorDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CDialog::OnGetMinMaxInfo(lpMMI);

	lpMMI->ptMinTrackSize.x = m_MinWindowSize.x;
	lpMMI->ptMinTrackSize.y = m_MinWindowSize.y;
}

void ScriptEditorDlg::OnBnClickedShowmsgbox()
{
	CRect rcTextBox;
	CRect rcEdit;

	GetDlgItem(EDIT_SCRIPT)->GetWindowRect(&rcEdit);
	mTextboxPreview.GetWindowRect(&rcTextBox);
	ScreenToClient(&rcEdit);
	ScreenToClient(&rcTextBox);

	if (mTextboxPreview.IsWindowVisible())
	{
		// Hide it
		rcEdit.top = rcTextBox.top;

		GetDlgItem(EDIT_SCRIPT)->SetWindowPos(NULL, rcEdit.left, rcEdit.top, rcEdit.Width(), rcEdit.Height(), SWP_NOZORDER);
		mTextboxPreview.ShowWindow(SW_HIDE);
		return;
	}
	
	// Show it
	int iWidth = rcTextBox.Width();
	rcTextBox.right  = rcEdit.left + ((rcEdit.Width() / 2) - (iWidth / 2)) + iWidth;
	rcTextBox.left   = rcEdit.left + ((rcEdit.Width() / 2) - (iWidth / 2));
	rcTextBox.bottom = rcEdit.top + rcTextBox.Height();
	rcTextBox.top    = rcEdit.top;
	rcEdit.top       = rcTextBox.bottom + 8;
	GetDlgItem(EDIT_SCRIPT)->SetWindowPos(NULL, rcEdit.left, rcEdit.top, rcEdit.Width(), rcEdit.Height(), SWP_NOZORDER);
	mTextboxPreview.SetWindowPos(NULL, rcTextBox.left, rcTextBox.top, iWidth, rcTextBox.Height(), SWP_NOZORDER);
	//mTextboxPreview.m_CheckBox_AutoFast.SetWindowPos(NULL, rcTextBox.left, rcTextBox.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	mTextboxPreview.ShowWindow(SW_SHOW);
}