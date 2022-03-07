#pragma once

#include "NpcEditorTabBase.h"

class Tab_NpcFlags : public NpcEditorTabBase
{
	typedef NpcEditorTabBase Parent;

public:
	Tab_NpcFlags();   // standard constructor
	virtual ~Tab_NpcFlags();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_NPCEDIT_PAGE1 };
#endif

public:
	virtual void PopulateTabPage();
	virtual void SaveTabPage(PXMOD_NPC_STRUCT* ent, int act_no);
	virtual void DisplayRecord(PXMOD_NPC_STRUCT* ent, int act_no);
};
