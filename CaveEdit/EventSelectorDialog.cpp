// EventSelectorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "EventSelectorDialog.h"
#include "afxdialogex.h"
#include "General.h"

#include <memory>

IMPLEMENT_DYNAMIC(EventSelectorDialog, CDialog)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void TSCTmp_Event::Clear() {
	if (!cmd_list)
		return;

	for (int i = 0; i < cmd_count; i++) {
		TSCTmp_Command* cmd = &cmd_list[i];

		// Free the text (if there is any)
		if (cmd->text) delete[] cmd->text;
	}

	delete[] cmd_list;
	cmd_list  = nullptr;
	cmd_count = 0;
}

TSCTmp_Command* TSCTmp_Event::AllocateCommand() {
	// Allocate a new command
	if (!cmd_list) {
		cmd_list = new TSCTmp_Command[8];
		cmd_size = 8;
	} else if (cmd_count + 1 >= cmd_size) {
		cmd_size += 8;

		TSCTmp_Command* newAlloc = new TSCTmp_Command[cmd_size];

		//Copy the old block into this new one
		memcpy(newAlloc, cmd_list, (cmd_size - 8) * sizeof(TSCTmp_Command));

		// Free the old block
		delete[] cmd_list;

		// Set the new block
		cmd_list = newAlloc;
	}

	TSCTmp_Command* ret = &cmd_list[cmd_count++];
	
	// Act as the command's constructor
	memset(ret, 0, sizeof(TSCTmp_Command));

	// Configure it
	ret->event = this;

	// Done!
	return ret;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

EventSelectorDialog::EventSelectorDialog(CWnd* pParent) : CDialog(EventSelectorDialog::IDD, pParent) {
	mEventObjects = NULL;
	mEventCount   = 0;
	mEventSize    = 0;
	mListRanges   = NULL;
	mInitEvent    = -1;
}

EventSelectorDialog::~EventSelectorDialog() {
	ClearEvents();

	if (mListRanges)
		delete[] mListRanges;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

TSC_struct* EventSelectorDialog::FindCommand(const char* cmd_name) {
	int index = command.Find(cmd_name);

	// If it's not valid, then return NULL.
	if(index == -1)
		return NULL;

	return &command.cmd[index];
}

TSCTmp_Event* EventSelectorDialog::AllocateEvent() {
	// Allocate a new command
	if (!mEventObjects) {
		mEventObjects = new TSCTmp_Event[8];
		mEventSize    = 8;
	} else if (mEventCount + 1 >= mEventSize) {
		mEventSize += 8;

		TSCTmp_Event* newAlloc = new TSCTmp_Event[mEventSize];

		//Copy the old block into this new one
		memcpy(newAlloc, mEventObjects, (mEventSize - 8) * sizeof(TSCTmp_Event));

		// Free the old block
		delete[] mEventObjects;

		// Set the new block
		mEventObjects = newAlloc;
	}
	
	// Init the new object
	memset(&mEventObjects[mEventCount], 0, sizeof(TSCTmp_Event));

	// Done!
	return &mEventObjects[mEventCount++];
}

void EventSelectorDialog::ClearEvents() {
	// Delete cache'd events
	if (!mEventObjects)
		return;

	// Free all event memory
	for (int i = 0; i < mEventCount; i++)
		mEventObjects[i].Clear();

	// Free the list
	delete[] mEventObjects;

	mEventObjects = nullptr;
	mEventCount   = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void EventSelectorDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_EVENTS, mEventList);
}


BEGIN_MESSAGE_MAP(EventSelectorDialog, CDialog)
	ON_LBN_SELCHANGE(LIST_EVENTS, &EventSelectorDialog::OnLbnSelchangeEvents)
	ON_BN_CLICKED(BUTTON_EVESEL_ACCEPT, &EventSelectorDialog::OnBnClickedEveselAccept)
	ON_BN_CLICKED(BUTTON_EVESEL_BACK, &EventSelectorDialog::OnBnClickedEveselBack)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void EventSelectorDialog::PopulateEventList() {
	if (!mEventObjects)
		return;
	
	if (mListRanges)
		delete[] mListRanges;

	// Clear the list
	mEventList.ResetContent();
	GetDlgItem(BUTTON_EVESEL_ACCEPT)->EnableWindow(FALSE);

	// Init the buffer
	char buffer[1024];

	// Init the ranges
	mListRanges = new SelectionRange[mEventCount];

	// Populate the list
	for (int i = 0; i < mEventCount; i++) {
		TSCTmp_Event* eve = &mEventObjects[i];

		// Add this to the list
		sprintf(buffer, "#%04d", eve->event_no);
		mEventList.AppendString(buffer, true, RGB(128, 0, 128));
		mListRanges[i].ptr   = eve;
		mListRanges[i].start = mEventList.GetCount() - 1;

		// Append all the commands for this event
		for (int j = 0; j < eve->cmd_count; j++) {
			TSCTmp_Command* cmd = &eve->cmd_list[j];

			sprintf(buffer, "  <%s", cmd->name);

			// Append arguments onto the string
			for (int k = 0; k < cmd->info->args; k++) {
				char num[5];
				sprintf(num, "%04d", cmd->args[k]);
				strcat(buffer, num);

				if (k != cmd->info->args - 1)
					strcat(buffer, ":");
			}

			mEventList.AppendString(buffer, false, RGB(0, 0, 255));

			// Add the text string if it exists
			if (cmd->text) {
				sprintf(buffer, "      %s", cmd->text);
				mEventList.AppendString(buffer, false, RGB(0, 0, 0));
			}
		}

		mListRanges[i].size = mEventList.GetCount() - mListRanges[i].start;
	}
}

bool EventSelectorDialog::LoadEvents(const char* file) {
	CFile f;

	// Attempt to open the file
	if (!f.Open(file, CFile::modeRead)) {
		printf("ERROR: LoadEvents() - Failed to open \"%s\".\r\n", file);
		return false;
	}

	ULONGLONG size = f.GetLength();

	// Allocate a buffer for the script
	unsigned char* scriptText = new unsigned char[size + 1];//+1 for the terminating character

	// Read the entire file
	f.Read(scriptText, size);
	f.Close();

	// Decode the file
	int shift = scriptText[size / 2];
	for (int i = 0; i < size; i++)
		if (i != size / 2)
			scriptText[i] -= shift;

	// Parse the script
	const char* text_start = NULL;
	TSCTmp_Event* lastEvent   = NULL;
	TSCTmp_Command* lastCmd   = NULL;
	for (const char* ptr = (const char*)scriptText; ptr < (const char*)(scriptText + size); ptr++) {
		if (*ptr == '#') {
			char num[5];
			sprintf(num, "%.4s", ptr + 1);

			// Allocate a new event
			lastEvent           = AllocateEvent();
			lastEvent->event_no = atoi(num);
			ptr                += 4;
			text_start          = ptr + 1;
		} else if (*ptr == '<') {
			if (!lastEvent) {
				printf("Found a command outside of an event???\r\n");
				continue;
			}

			// Pass the '<'
			ptr++;

			// Configure it
			char cmdname[4];
			sprintf(cmdname, "%.3s", ptr);
			ptr += 3;

			// Find the command
			TSC_struct* info = NULL;

			char cmd_name_full[5];
			sprintf(cmd_name_full, "<%s", cmdname);
			if ((info = FindCommand(cmd_name_full)) == NULL) {
				printf("ERROR: LoadEvents() - Failed to find event '%s'\r\n", cmdname);
				continue;
			}

			// Allocate the new command
			TSCTmp_Command* newcmd = lastEvent->AllocateCommand();

			// Configure the command
			strcpy(newcmd->name, cmdname);
			newcmd->info = info;

			// Append the text if applicable
			if (text_start != ptr - 4 && lastCmd) {
				char fmt[156]; sprintf(fmt, "%%.%ds", (ptr - 4) - text_start);

				lastCmd->text = new char[((ptr - 4) - text_start) + 1];

				char* mptr = lastCmd->text;
				for (const char* tptr = text_start; tptr < text_start + ((ptr - 4) - text_start); tptr++) {
					if (*tptr == '\r' || *tptr == '\n')
						continue;
					
					*mptr++ = *tptr;
				}

				*mptr = 0;
			}

			// Set the last CMD
			lastCmd = newcmd;

			// Get the arguments
			for (int k = 0; k < info->args; k++) {
				char num[5];
				sprintf(num, "%.4s", ptr);
				ptr += 4;

				newcmd->args[k] = atoi(num);

				if (k != info->args - 1) {
					if (*ptr != ':') {
						printf("'\r\n  > ERROR: LoadEvents() - Invalid amount of arguments for '%s'!\r\n", cmdname);
						break;
					}

					ptr++;
				}
			}

			// Set the new text_start position
			text_start = ptr;

			// Account for the increment step
			ptr--;
		} else if (text_start && text_start == ptr && *ptr < ' ') {
			text_start++;
		}
	}

	// Cleanup
	delete[] scriptText;

	// Done!
	return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BOOL EventSelectorDialog::OnInitDialog() {
	if (!CDialog::OnInitDialog())
		return FALSE;

	PopulateEventList();

	// Set selected event
	if (mInitEvent == -1) {
		mEventList.SetCurSel(0);
		return TRUE;
	}

	for (int i = 0; i < mEventCount; i++) {
		if (mListRanges[i].ptr->event_no != mInitEvent)
			continue;

		mEventList.SetCurSel(mListRanges[i].start);
		break;
	}

	return TRUE;
}

void EventSelectorDialog::OnLbnSelchangeEvents() {
	// Enable the 'accept' button
	GetDlgItem(BUTTON_EVESEL_ACCEPT)->EnableWindow(TRUE);

	// Get the event this selection is tied to
	int sel = mEventList.GetCurSel();

	// Find the selection
	for (int i = 0; i < mEventCount; i++) {
		SelectionRange* range = &mListRanges[i];

		// Ignore this object if we're out of range
		if (sel < range->start || sel >= range->start + range->size)
			continue;

		if (sel == range->start)
			return;

		mEventList.SetCurSel(range->start);
	}
}

void EventSelectorDialog::OnBnClickedEveselAccept() {
	// Get the event this selection is tied to
	int sel = mEventList.GetCurSel();

	if (sel == LB_ERR) {
		this->EndDialog(-1);
		return;
	}

	// Find the selection
	for (int i = 0; i < mEventCount; i++) {
		SelectionRange* range = &mListRanges[i];

		// Ignore this object if we're out of range
		if (sel < range->start || sel >= range->start + range->size)
			continue;

		this->EndDialog(range->ptr->event_no);
		return;
	}

	this->EndDialog(-1);
}


void EventSelectorDialog::OnBnClickedEveselBack() {
	this->EndDialog(-1);
}