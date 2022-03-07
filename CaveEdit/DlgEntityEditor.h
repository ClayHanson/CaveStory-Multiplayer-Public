#pragma once

#include "ModConfigResource.h"
#include "CompiledMapResource.h"
#include "DlgItemProperty.h"

struct DLG_ENT_PROPERTY_EDITOR_ITEM
{
	PXCM_ENTITY_CUSTOM_PROPERTY* prop;
	DlgItemProperty* gui_item;
};

class DlgEntPropertyEditor : public CDialog
{
	DECLARE_DYNAMIC(DlgEntPropertyEditor)

	enum { IDD = DIALOG_ENTITY_PROPERTIES };

protected:
	bool m_Initialized;

public: // Variables
	CScrollBar m_ScrollBar;
	CWnd m_ListBox;
	int m_iMinSizeX;
	int m_iMinSizeY;

public: // More variables

	DLG_ENT_PROPERTY_EDITOR_ITEM* m_PropList;
	int m_PropCount;
	bool m_Changes;

public: // C++ stuff
	DlgEntPropertyEditor(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DlgEntPropertyEditor();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	DLG_ENT_PROPERTY_EDITOR_ITEM* EntProp_Get(const char* pName, int* iIndex = nullptr);
	void EntProp_Set(const char* pName, const char* pValue);
	void EntProp_Delete(const char* pName);
	void EntProp_MoveUp(const char* pName);
	void EntProp_MoveDown(const char* pName);
	void EntProp_Calculate();

public:
	afx_msg void Clicked_Ok();
	afx_msg void Clicked_Cancel();
	afx_msg void Clicked_Add();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnDestroy();
};
