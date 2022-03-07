#pragma once

#include "NpcEditorTabBase.h"

class Tab_NpcInfo : public NpcEditorTabBase
{
	typedef NpcEditorTabBase Parent;

public:
	Tab_NpcInfo();
	virtual ~Tab_NpcInfo();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_NPCEDIT_PAGE5 };
#endif

public:
	virtual void PopulateTabPage();
	virtual void SaveTabPage(PXMOD_NPC_STRUCT* ent, int act_no);
	virtual void DisplayRecord(PXMOD_NPC_STRUCT* ent, int act_no);
	DECLARE_MESSAGE_MAP()
};
