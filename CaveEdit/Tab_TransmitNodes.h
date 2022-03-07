#pragma once

#include "NpcEditorTabBase.h"

class Tab_TransmitNodes : public NpcEditorTabBase
{
	typedef NpcEditorTabBase Parent;

public:
	Tab_TransmitNodes();
	virtual ~Tab_TransmitNodes();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = TAB_NPCEDIT_PAGE4 };
#endif

public:
	virtual void PopulateTabPage();
	virtual void SaveTabPage(PXMOD_NPC_STRUCT* ent, int act_no);
	virtual void DisplayRecord(PXMOD_NPC_STRUCT* ent, int act_no);
};
