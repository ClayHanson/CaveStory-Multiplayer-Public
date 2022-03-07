#include "stdafx.h"
#include "CaveEditor.h"
#include "DlgItemProperty.h"
#include "DlgEntityEditor.h"
#include "afxdialogex.h"

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(DlgItemProperty, CDialog)

BEGIN_MESSAGE_MAP(DlgItemProperty, CDialog)
	ON_EN_CHANGE(EDIT_PROP_NAME,		OnEnChangePropName)
	ON_EN_CHANGE(EDIT_PROP_VALUE,		OnEnChangePropValue)
	ON_BN_CLICKED(BUTTON_PROP_UP,		OnBnClickedPropUp)
	ON_BN_CLICKED(BUTTON_PROP_DOWN,		OnBnClickedPropDown)
	ON_BN_CLICKED(BUTTON_PROP_DELETE,	OnBnClickedPropDelete)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DlgItemProperty::DlgItemProperty(CWnd* pParent)
	: CDialog(DLGITEM_PROPERTY_ENTRY, pParent)
{
	m_pEditorOwner = NULL;
	m_pProperty    = NULL;
}

DlgItemProperty::~DlgItemProperty()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgItemProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, EDIT_PROP_NAME,		m_FieldName);
	DDX_Control(pDX, EDIT_PROP_VALUE,		m_FieldValue);
	DDX_Control(pDX, BUTTON_PROP_UP,		m_MoveUp);
	DDX_Control(pDX, BUTTON_PROP_DOWN,		m_MoveDown);
	DDX_Control(pDX, BUTTON_PROP_DELETE,	m_Delete);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int DlgItemProperty::GetElementX(CWnd* pWnd)
{
	RECT rc;

	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);

	return rc.left;
}

int DlgItemProperty::GetElementY(CWnd* pWnd)
{
	RECT rc;

	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);

	return rc.top;
}

int DlgItemProperty::GetElementWidth(CWnd* pWnd)
{
	RECT rc;

	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);

	return RECT_WIDTH(rc);
}

int DlgItemProperty::GetElementHeight(CWnd* pWnd)
{
	RECT rc;

	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);

	return RECT_HEIGHT(rc);
}

int DlgItemProperty::GetElementRight(CWnd* pWnd)
{
	RECT rc;

	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);

	return rc.right;
}

int DlgItemProperty::GetElementBottom(CWnd* pWnd)
{
	RECT rc;

	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);

	return rc.bottom;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgItemProperty::ResizeElements(int iNewWidth)
{
	static const int iMargin         = 2;
	static const int iItemSizeConst  = 24;
	static const int iFieldNameWidth = 119;

	SetWindowPos(NULL, 0, 0, iNewWidth, iItemSizeConst + ((iMargin + 1) * 2) + 2, SWP_NOZORDER | SWP_NOMOVE | SWP_NOREDRAW);

	// Resize all elements
	m_FieldName.SetWindowPos	(NULL, iMargin + 1, iMargin + 1, iFieldNameWidth, iItemSizeConst, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOREPOSITION | SWP_NOREDRAW);
	m_Delete.SetWindowPos		(NULL, iNewWidth - (iMargin + 3) - iItemSizeConst, iMargin + 1, iItemSizeConst, iItemSizeConst, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOREPOSITION | SWP_NOREDRAW);
	m_MoveDown.SetWindowPos		(NULL, GetElementX(&m_Delete) - iMargin - iItemSizeConst, iMargin + 1, iItemSizeConst, iItemSizeConst, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOREPOSITION | SWP_NOREDRAW);
	m_MoveUp.SetWindowPos		(NULL, GetElementX(&m_MoveDown) - iMargin - iItemSizeConst, iMargin + 1, iItemSizeConst, iItemSizeConst, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOREPOSITION | SWP_NOREDRAW);
	m_FieldValue.SetWindowPos	(NULL, GetElementRight(&m_FieldName) + iMargin, iMargin + 1, (GetElementX(&m_MoveUp) - iMargin) - (GetElementRight(&m_FieldName) + iMargin), iItemSizeConst, SWP_NOZORDER | SWP_SHOWWINDOW | SWP_NOREDRAW);
	//printf("(%d, %d)<%d, %d>\r\n", ThisRect.left, ThisRect.top, RECT_WIDTH(ThisRect), RECT_HEIGHT(ThisRect));
}

void DlgItemProperty::PopulatePropInfo()
{
	if (!m_pProperty || !m_pEditorOwner)
		return;

	bool bChange = m_pEditorOwner->m_Changes;

	m_FieldName.SetWindowTextA(m_pProperty->name);
	m_FieldValue.SetWindowTextA(m_pProperty->value);

	m_pEditorOwner->m_Changes = bChange;
}

void DlgItemProperty::OnEnChangePropName()
{
	if (!m_pProperty)
		return;

	m_FieldName.GetWindowTextA(m_pProperty->name, MAX_ENTITY_PROPERTY_STRING_LENGTH);
	m_pEditorOwner->m_Changes = true;
}

void DlgItemProperty::OnEnChangePropValue()
{
	if (!m_pProperty)
		return;

	m_FieldValue.GetWindowTextA(m_pProperty->value, MAX_ENTITY_PROPERTY_STRING_LENGTH);
	m_pEditorOwner->m_Changes = true;
}

void DlgItemProperty::OnBnClickedPropUp()
{
	if (!m_pProperty || !m_pEditorOwner)
		return;

	m_pEditorOwner->EntProp_MoveUp(m_pProperty->name);
}

void DlgItemProperty::OnBnClickedPropDown()
{
	if (!m_pProperty || !m_pEditorOwner)
		return;

	m_pEditorOwner->EntProp_MoveDown(m_pProperty->name);
}

void DlgItemProperty::OnBnClickedPropDelete()
{
	if (!m_pProperty || !m_pEditorOwner)
		return;

	m_pEditorOwner->EntProp_Delete(m_pProperty->name);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------