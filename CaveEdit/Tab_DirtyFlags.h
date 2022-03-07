#pragma once

#include "NpcEditorTabBase.h"

class Tab_DirtyFlags : public NpcEditorTabBase
{
	typedef NpcEditorTabBase Parent;

public:
	Tab_DirtyFlags();
	virtual ~Tab_DirtyFlags();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_NPCEDIT_PAGE2 };
#endif

public:
	virtual void PopulateTabPage();
	virtual void SaveTabPage(PXMOD_NPC_STRUCT* ent, int act_no);
	virtual void DisplayRecord(PXMOD_NPC_STRUCT* ent, int act_no);
};
