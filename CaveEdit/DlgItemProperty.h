#pragma once

#include "CompiledMapResource.h"

#define RECT_WIDTH(ARECT) (ARECT.right - ARECT.left)
#define RECT_HEIGHT(ARECT) (ARECT.bottom - ARECT.top)

class DlgEntPropertyEditor;

class DlgItemProperty : public CDialog
{
	DECLARE_DYNAMIC(DlgItemProperty)

	enum { IDD = DLGITEM_PROPERTY_ENTRY };

public: // Variables
	DlgEntPropertyEditor* m_pEditorOwner;
	PXCM_ENTITY_CUSTOM_PROPERTY* m_pProperty;
	CEdit m_FieldName;
	CEdit m_FieldValue;
	CButton m_MoveUp;
	CButton m_MoveDown;
	CButton m_Delete;

public:
	DlgItemProperty(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DlgItemProperty();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int GetElementX(CWnd* pWnd);
	int GetElementY(CWnd* pWnd);
	int GetElementWidth(CWnd* pWnd);
	int GetElementHeight(CWnd* pWnd);
	int GetElementRight(CWnd* pWnd);
	int GetElementBottom(CWnd* pWnd);

public:
	void ResizeElements(int iNewWidth);
	void PopulatePropInfo();
	afx_msg void OnEnChangePropName();
	afx_msg void OnEnChangePropValue();
	afx_msg void OnBnClickedPropUp();
	afx_msg void OnBnClickedPropDown();
	afx_msg void OnBnClickedPropDelete();
};
