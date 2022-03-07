#pragma once

#include "General.h"
#include "CEventListBox.h"

struct TSCTmp_Event;

struct TSCTmp_Command {
	unsigned short args[32];
	TSC_struct* info;
	TSCTmp_Event* event;
	char name[4];
	char* text;
};

class TSCTmp_Event {
public:
	int event_no;
	TSCTmp_Command* cmd_list;
	int cmd_count;
	int cmd_size;

public:
	void Clear();

public:
	TSCTmp_Command* AllocateCommand();
};

struct SelectionRange {
	TSCTmp_Event* ptr;
	int start;
	int size;
};

class EventSelectorDialog : public CDialog {
	DECLARE_DYNAMIC(EventSelectorDialog)
public: // Variables
	CEventListBox mEventList;
	
	TSC_Info command;
	SelectionRange* mListRanges;
	TSCTmp_Event* mEventObjects;
	int mEventCount;
	int mEventSize;
	int mInitEvent;

public: // Constructor & deconstructor
	EventSelectorDialog(CWnd* pParent = nullptr);
	virtual ~EventSelectorDialog();

public: // Enums
	enum { IDD = DIALOG_EVENT_SELECTOR };

public: // Event list manipulation
	TSC_struct* FindCommand(const char* cmd_name);
	TSCTmp_Event* AllocateEvent();
	void ClearEvents();

protected: // Protected methods
	virtual void DoDataExchange(CDataExchange* pDX);
	void PopulateEventList();

protected: // message map.
	DECLARE_MESSAGE_MAP()

public: // Public methods
	bool LoadEvents(const char* file);

public: // Stuff for the dialog
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeEvents();
	afx_msg void OnBnClickedEveselAccept();
	afx_msg void OnBnClickedEveselBack();
};
