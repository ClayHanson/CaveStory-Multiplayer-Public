// DlgBulletEditor.cpp : implementation file
//

#include "stdafx.h"
#include "CaveEditor.h"
#include "DlgBulletEditor.h"
#include "afxdialogex.h"
#include "General.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(DlgBulletEditor, CDialog)

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define CLAMP(X, AMIN, AMAX) ((X) > (AMAX) ? (AMAX) : ((X) < (AMIN) ? (AMIN) : (X)))
#define GET_CENTER_OFFSET(w1, w2) ((MAX((w1), (w2)) / 2) - (MIN((w1), (w2)) / 2))
#define RECT_WIDTH(ARECT) (ARECT.right - ARECT.left)
#define RECT_HEIGHT(ARECT) (ARECT.bottom - ARECT.top)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(DlgBulletEditor, CDialog)
	ON_WM_PAINT()
	ON_EN_CHANGE(EDIT_BULLET_NAME, OnEnChangeBulletName)
	ON_EN_CHANGE(EDIT_BULLET_DAMAGE, OnEnChangeBulletDamage)
	ON_EN_CHANGE(EDIT_BULLET_LIFE, OnEnChangeBulletLife)
	ON_EN_CHANGE(EDIT_BULLET_LIFE_COUNT, OnEnChangeBulletLifeCount)
	ON_LBN_SELCHANGE(LIST_BULLETS, OnLbnSelchangeBullets)
	ON_BN_CLICKED(BUTTON_BULLET_ADD, OnBnClickedBulletAdd)
	ON_BN_CLICKED(BUTTON_BULLET_DUPLICATE, OnBnClickedBulletDuplicate)
	ON_BN_CLICKED(BUTTON_BULLET_DELETE, OnBnClickedBulletDelete)
	ON_BN_CLICKED(BUTTON_BULLET_SAVE, OnBnClickedBulletSave)
	ON_BN_CLICKED(CHECK_BULLET_FLAG_1, OnBnClickedCheck1)
	ON_BN_CLICKED(CHECK_BULLET_FLAG_2, OnBnClickedCheck2)
	ON_BN_CLICKED(CHECK_BULLET_FLAG_3, OnBnClickedCheck3)
	ON_BN_CLICKED(CHECK_BULLET_FLAG_4, OnBnClickedCheck4)
	ON_BN_CLICKED(CHECK_BULLET_FLAG_5, OnBnClickedCheck5)
	ON_EN_CHANGE(EDIT_BULLET_BOUNDS_W, OnEnChangeBulletBoundsW)
	ON_EN_CHANGE(EDIT_BULLET_BOUNDS_H, OnEnChangeBulletBoundsH)
	ON_EN_CHANGE(EDIT_BULLET_COL_BOX_W, OnEnChangeBulletColW)
	ON_EN_CHANGE(EDIT_BULLET_COL_BOX_H, OnEnChangeBulletColH)
	ON_EN_CHANGE(EDIT_BULLET_DISP_X, OnEnChangeBulletDispX)
	ON_EN_CHANGE(EDIT_BULLET_DISP_Y, OnEnChangeBulletDispY)
	ON_EN_CHANGE(EDIT_BULLET_DISP_W, OnEnChangeBulletDispW)
	ON_EN_CHANGE(EDIT_BULLET_DISP_H, OnEnChangeBulletDispH)
	ON_BN_CLICKED(CHECK_BULLET_COPY_BEHAVIOR, OnBnClickedBulletCopyBehavior)
	ON_BN_CLICKED(CHECK_BULLET_COPY_BOUNDS, OnBnClickedBulletCopyBounds)
	ON_BN_CLICKED(CHECK_BULLET_COPY_COLLISION, OnBnClickedBulletCopyCollision)
	ON_BN_CLICKED(CHECK_BULLET_COPY_FLAGS, OnBnClickedBulletCopyFlags)
	ON_BN_CLICKED(CHECK_BULLET_COPY_SPRITE, OnBnClickedBulletCopySprite)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DlgBulletEditor::DlgBulletEditor(CWnd* pParent) : CDialog(DIALOG_GAMEEDIT_BULLET, pParent)
{
	m_IgnoreInput = false;

	memset(&m_Copy, 0, sizeof(m_Copy));
}

DlgBulletEditor::~DlgBulletEditor()
{
	for (int i = 0; i < m_BulletData.GetCount(); i++)
		delete m_BulletData[i];

	m_BulletImage.Destroy();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgBulletEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_BULLETS, m_BulletList);
}

BOOL DlgBulletEditor::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Initialize the bullet data list
	for (int i = 0; i < exe.mod.mBullets.count; i++)
	{
		PXMOD_BULLET_STRUCT* pNewBul = new PXMOD_BULLET_STRUCT();
		memcpy(pNewBul, &exe.mod.mBullets.list[i], sizeof(PXMOD_BULLET_STRUCT));
		m_BulletData.Add(pNewBul);
	}

	// Load the bullet list
	Refresh();

	m_BulletList.SetCurSel(0);
	LoadBullet();

	char buffer[1024];
	sprintf(buffer, "%s/Bullet.png", exe.mod.mModPath);

	if (!m_BulletImage.Load(buffer))
	{
		sprintf(buffer, "%s/../data/Bullet.png", exe.mod.mModPath);
		if (!m_BulletImage.Load(buffer))
		{
			printf("failed to load the bullet image!!!\r\n");
		}
	}

	SetIcon(m_hIcon, TRUE);		 // Set big icon
	SetIcon(m_hIcon, TRUE);	   // Set small icon

	Invalidate(TRUE);

	// Done!
	return TRUE;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgBulletEditor::OnLbnSelchangeBullets()
{
	LoadBullet();
}

void DlgBulletEditor::OnBnClickedBulletAdd()
{
	PXMOD_BULLET_STRUCT* pNewWpn = new PXMOD_BULLET_STRUCT;

	// Configure the new bullet
	memset(pNewWpn, 0, sizeof(PXMOD_BULLET_STRUCT));
	strcpy(pNewWpn->name, "Unnamed Bullet");

	// Add it to the list
	m_BulletData.Add(pNewWpn);
	Refresh();
	m_BulletList.SetCurSel(m_BulletList.GetCount() - 1);
	LoadBullet();

	CDC* cdc;
	cdc = GetDC();
	RenderBullet(*cdc);
	ReleaseDC(cdc);
}

void DlgBulletEditor::OnBnClickedBulletDuplicate()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR)
	{
		MessageBox("Select a bullet first.", "Error");
		return;
	}

	PXMOD_BULLET_STRUCT* pNewWpn = new PXMOD_BULLET_STRUCT;

	// Configure the new bullet
	memcpy(pNewWpn, m_BulletData[iCurBul], sizeof(PXMOD_BULLET_STRUCT));
	strcpy(pNewWpn->name, m_BulletData[iCurBul]->name);
	strcat(pNewWpn->name, " (Duplicated)");

	// Add it to the list
	m_BulletData.Add(pNewWpn);

	Refresh();
	m_BulletList.SetCurSel(m_BulletData.GetCount() - 1);
	LoadBullet();
}

void DlgBulletEditor::OnBnClickedBulletDelete()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR)
	{
		MessageBox("Select a bullet first.", "Error");
		return;
	}

	delete m_BulletData[iCurBul];
	m_BulletData.RemoveAt(iCurBul, 1);
	Refresh();
	m_BulletList.SetCurSel(m_BulletData.GetCount() - 1);
	LoadBullet();
}

void DlgBulletEditor::OnBnClickedBulletSave()
{
	SaveBullets();
	OnOK();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgBulletEditor::OnEnChangeBulletName()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	GetDlgItem(EDIT_BULLET_NAME)->GetWindowTextA(m_BulletData[iCurBul]->name, sizeof(m_BulletData[iCurBul]->name));
	
	char buffer[256];
	sprintf_s(buffer, sizeof(buffer), "[%.3i] %s", iCurBul, m_BulletData[iCurBul]->name);
	m_BulletList.DeleteString((UINT)iCurBul);

	if (!m_BulletList.GetCount())
		m_BulletList.AddString(buffer);
	else
		m_BulletList.InsertString((UINT)iCurBul, buffer);

	m_BulletList.SetCurSel(iCurBul);
}

void DlgBulletEditor::OnEnChangeBulletBoundsW()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_BULLET_BOUNDS_W)->GetWindowTextA(buffer, sizeof(buffer));
	m_BulletData[iCurBul]->bullet_size.w = atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderBullet(*cdc);
	ReleaseDC(cdc);
}

void DlgBulletEditor::OnEnChangeBulletBoundsH()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_BULLET_BOUNDS_H)->GetWindowTextA(buffer, sizeof(buffer));
	m_BulletData[iCurBul]->bullet_size.h = atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderBullet(*cdc);
	ReleaseDC(cdc);
}

void DlgBulletEditor::OnEnChangeBulletColW()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_BULLET_COL_BOX_W)->GetWindowTextA(buffer, sizeof(buffer));
	m_BulletData[iCurBul]->collision_size.w = atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderBullet(*cdc);
	ReleaseDC(cdc);
}

void DlgBulletEditor::OnEnChangeBulletColH()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_BULLET_COL_BOX_H)->GetWindowTextA(buffer, sizeof(buffer));
	m_BulletData[iCurBul]->collision_size.h = atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderBullet(*cdc);
	ReleaseDC(cdc);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DlgBulletEditor::OnEnChangeBulletDamage()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_BULLET_DAMAGE)->GetWindowTextA(buffer, sizeof(buffer));
	m_BulletData[iCurBul]->damage = atoi(buffer);
}

void DlgBulletEditor::OnEnChangeBulletLife()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_BULLET_LIFE)->GetWindowTextA(buffer, sizeof(buffer));
	m_BulletData[iCurBul]->hit_life = atoi(buffer);
}

void DlgBulletEditor::OnEnChangeBulletLifeCount()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_BULLET_LIFE_COUNT)->GetWindowTextA(buffer, sizeof(buffer));
	m_BulletData[iCurBul]->range = atoi(buffer);
}

void DlgBulletEditor::OnEnChangeBulletDispX()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_BULLET_DISP_X)->GetWindowTextA(buffer, sizeof(buffer));
	m_BulletData[iCurBul]->sprite.right = atoi(buffer) + (m_BulletData[iCurBul]->sprite.right - m_BulletData[iCurBul]->sprite.left);
	m_BulletData[iCurBul]->sprite.left  = atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderBullet(*cdc);
	ReleaseDC(cdc);
}

void DlgBulletEditor::OnEnChangeBulletDispY()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_BULLET_DISP_Y)->GetWindowTextA(buffer, sizeof(buffer));
	m_BulletData[iCurBul]->sprite.bottom = atoi(buffer) + (m_BulletData[iCurBul]->sprite.bottom - m_BulletData[iCurBul]->sprite.top);
	m_BulletData[iCurBul]->sprite.top    = atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderBullet(*cdc);
	ReleaseDC(cdc);
}

void DlgBulletEditor::OnEnChangeBulletDispW()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_BULLET_DISP_W)->GetWindowTextA(buffer, sizeof(buffer));
	m_BulletData[iCurBul]->sprite.right = m_BulletData[iCurBul]->sprite.left + atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderBullet(*cdc);
	ReleaseDC(cdc);
}

void DlgBulletEditor::OnEnChangeBulletDispH()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	char buffer[256];
	GetDlgItem(EDIT_BULLET_DISP_H)->GetWindowTextA(buffer, sizeof(buffer));
	m_BulletData[iCurBul]->sprite.bottom = m_BulletData[iCurBul]->sprite.top + atoi(buffer);

	CDC* cdc;
	cdc = GetDC();
	RenderBullet(*cdc);
	ReleaseDC(cdc);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgBulletEditor::OnBnClickedCheck1()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	bool bValue                  = (((CButton*)GetDlgItem(CHECK_BULLET_FLAG_1))->GetState() & BST_CHECKED);
	m_BulletData[iCurBul]->flags = (bValue ? m_BulletData[iCurBul]->flags | PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_IGNORE_SOLID : m_BulletData[iCurBul]->flags & ~PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_IGNORE_SOLID);
}

void DlgBulletEditor::OnBnClickedCheck2()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	bool bValue                  = (((CButton*)GetDlgItem(CHECK_BULLET_FLAG_2))->GetState() & BST_CHECKED);
	m_BulletData[iCurBul]->flags = (bValue ? m_BulletData[iCurBul]->flags | PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_DONT_DESTROY_SOLID : m_BulletData[iCurBul]->flags & ~PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_DONT_DESTROY_SOLID);
}

void DlgBulletEditor::OnBnClickedCheck3()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	bool bValue                  = (((CButton*)GetDlgItem(CHECK_BULLET_FLAG_3))->GetState() & BST_CHECKED);
	m_BulletData[iCurBul]->flags = (bValue ? m_BulletData[iCurBul]->flags | PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_PROJECTILE : m_BulletData[iCurBul]->flags & ~PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_PROJECTILE);
}

void DlgBulletEditor::OnBnClickedCheck4()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	bool bValue                  = (((CButton*)GetDlgItem(CHECK_BULLET_FLAG_4))->GetState() & BST_CHECKED);
	m_BulletData[iCurBul]->flags = (bValue ? m_BulletData[iCurBul]->flags | PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_BREAK_BLOCKS : m_BulletData[iCurBul]->flags & ~PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_BREAK_BLOCKS);
}

void DlgBulletEditor::OnBnClickedCheck5()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_IgnoreInput)
		return;

	bool bValue                  = (((CButton*)GetDlgItem(CHECK_BULLET_FLAG_5))->GetState() & BST_CHECKED);
	m_BulletData[iCurBul]->flags = (bValue ? m_BulletData[iCurBul]->flags | PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_PIERCE_BLOCKS : m_BulletData[iCurBul]->flags & ~PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_PIERCE_BLOCKS);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgBulletEditor::OnBnClickedBulletCopyBehavior()
{
	m_Copy.behavior = (((CButton*)GetDlgItem(CHECK_BULLET_COPY_BEHAVIOR))->GetState() & BST_CHECKED);
}

void DlgBulletEditor::OnBnClickedBulletCopyBounds()
{
	m_Copy.bounds = (((CButton*)GetDlgItem(CHECK_BULLET_COPY_BOUNDS))->GetState() & BST_CHECKED);
}

void DlgBulletEditor::OnBnClickedBulletCopyCollision()
{
	m_Copy.collision = (((CButton*)GetDlgItem(CHECK_BULLET_COPY_COLLISION))->GetState() & BST_CHECKED);
}

void DlgBulletEditor::OnBnClickedBulletCopyFlags()
{
	m_Copy.flags = (((CButton*)GetDlgItem(CHECK_BULLET_COPY_FLAGS))->GetState() & BST_CHECKED);
}

void DlgBulletEditor::OnBnClickedBulletCopySprite()
{
	m_Copy.sprite = (((CButton*)GetDlgItem(CHECK_BULLET_COPY_SPRITE))->GetState() & BST_CHECKED);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgBulletEditor::Refresh()
{
	int iOldSel = m_BulletList.GetCurSel();

	while (m_BulletList.GetCount()) m_BulletList.DeleteString(0);

	char Buffer[256];
	for (int i = 0; i < m_BulletData.GetCount(); i++)
	{
		sprintf_s(Buffer, sizeof(Buffer), "[%.3i] %s", i, m_BulletData[i]->name);
		m_BulletList.AddString(Buffer);
	}
}

void DlgBulletEditor::LoadBullet()
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR)
		return;

	PXMOD_BULLET_STRUCT* pBullet = m_BulletData[iCurBul];
	char buffer[128];

	m_IgnoreInput = true;

	// Set the display rect
	GetDlgItem(EDIT_BULLET_NAME)->SetWindowTextA(pBullet->name);

	//------------------------------
	if (m_Copy.behavior)
	{
		m_IgnoreInput = false;
		OnEnChangeBulletDamage();
		OnEnChangeBulletLife();
		OnEnChangeBulletLifeCount();
		m_IgnoreInput = true;
	}
	else
	{
		sprintf_s(buffer, sizeof(buffer), "%d", pBullet->damage); GetDlgItem(EDIT_BULLET_DAMAGE)->SetWindowTextA(buffer);
		sprintf_s(buffer, sizeof(buffer), "%d", pBullet->hit_life); GetDlgItem(EDIT_BULLET_LIFE)->SetWindowTextA(buffer);
		sprintf_s(buffer, sizeof(buffer), "%d", pBullet->range); GetDlgItem(EDIT_BULLET_LIFE_COUNT)->SetWindowTextA(buffer);
	}

	//------------------------------
	if (m_Copy.bounds)
	{
		m_IgnoreInput = false;
		OnEnChangeBulletBoundsW();
		OnEnChangeBulletBoundsH();
		m_IgnoreInput = true;
	}
	else
	{
		sprintf_s(buffer, sizeof(buffer), "%d", pBullet->bullet_size.w); GetDlgItem(EDIT_BULLET_BOUNDS_W)->SetWindowTextA(buffer);
		sprintf_s(buffer, sizeof(buffer), "%d", pBullet->bullet_size.h); GetDlgItem(EDIT_BULLET_BOUNDS_H)->SetWindowTextA(buffer);
	}

	//------------------------------
	if (m_Copy.collision)
	{
		m_IgnoreInput = false;
		OnEnChangeBulletColW();
		OnEnChangeBulletColH();
		m_IgnoreInput = true;
	}
	else
	{
		sprintf_s(buffer, sizeof(buffer), "%d", pBullet->collision_size.w); GetDlgItem(EDIT_BULLET_COL_BOX_W)->SetWindowTextA(buffer);
		sprintf_s(buffer, sizeof(buffer), "%d", pBullet->collision_size.h); GetDlgItem(EDIT_BULLET_COL_BOX_H)->SetWindowTextA(buffer);
	}

	//------------------------------
	if (m_Copy.sprite)
	{
		m_IgnoreInput = false;
		OnEnChangeBulletDispX();
		OnEnChangeBulletDispY();
		OnEnChangeBulletDispW();
		OnEnChangeBulletDispH();
		m_IgnoreInput = true;
	}
	else
	{
		sprintf_s(buffer, sizeof(buffer), "%d", pBullet->sprite.left); GetDlgItem(EDIT_BULLET_DISP_X)->SetWindowTextA(buffer);
		sprintf_s(buffer, sizeof(buffer), "%d", pBullet->sprite.top); GetDlgItem(EDIT_BULLET_DISP_Y)->SetWindowTextA(buffer);
		sprintf_s(buffer, sizeof(buffer), "%d", pBullet->sprite.right - pBullet->sprite.left); GetDlgItem(EDIT_BULLET_DISP_W)->SetWindowTextA(buffer);
		sprintf_s(buffer, sizeof(buffer), "%d", pBullet->sprite.bottom - pBullet->sprite.top); GetDlgItem(EDIT_BULLET_DISP_H)->SetWindowTextA(buffer);
	}

	//------------------------------
	if (m_Copy.flags)
	{
		m_IgnoreInput = false;
		OnBnClickedCheck1();
		OnBnClickedCheck2();
		OnBnClickedCheck3();
		OnBnClickedCheck4();
		OnBnClickedCheck5();
		m_IgnoreInput = true;
	}
	else
	{
		((CButton*)GetDlgItem(CHECK_BULLET_FLAG_1))->SetCheck(pBullet->flags & PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_IGNORE_SOLID);
		((CButton*)GetDlgItem(CHECK_BULLET_FLAG_2))->SetCheck(pBullet->flags & PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_DONT_DESTROY_SOLID);
		((CButton*)GetDlgItem(CHECK_BULLET_FLAG_3))->SetCheck(pBullet->flags & PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_PROJECTILE);
		((CButton*)GetDlgItem(CHECK_BULLET_FLAG_4))->SetCheck(pBullet->flags & PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_BREAK_BLOCKS);
		((CButton*)GetDlgItem(CHECK_BULLET_FLAG_5))->SetCheck(pBullet->flags & PXMOD_BULLET_FLAGS::PXMOD_BULLET_FLAG_PIERCE_BLOCKS);
	}

	// Redraw some stuff
	CDC* cdc;
	cdc = GetDC();
	RenderBullet(*cdc);
	ReleaseDC(cdc);

	m_IgnoreInput = false;
}

void DlgBulletEditor::SaveBullets()
{
	while (exe.mod.mBullets.count) exe.mod.RemoveBullet(exe.mod.mBullets.list[0].name);

	for (int i = 0; i < m_BulletData.GetCount(); i++)
	{
		int iInsertIndex = exe.mod.AddBullet(m_BulletData[i]->name);
		memcpy(&exe.mod.mBullets.list[iInsertIndex], m_BulletData[i], sizeof(PXMOD_BULLET_STRUCT));
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgBulletEditor::OnPaint()
{
	int x, y;
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		x = (rect.Width() - cxIcon + 1) / 2;
		y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}

	CPaintDC dc(this); // device context for painting
	CDialog::OnPaint();

	// Render everything else
	RenderBullet(dc);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DlgBulletEditor::RenderBullet(CDC& dc)
{
	int iCurBul = m_BulletList.GetCurSel();
	if (iCurBul == LB_ERR || m_BulletImage.IsNull())
		return;

	PXMOD_BULLET_STRUCT* pBullet = m_BulletData[iCurBul];
	CRect EntireRect;
	CRect DstRect;
	CRect SrcRect = { pBullet->sprite.left, pBullet->sprite.top, pBullet->sprite.right, pBullet->sprite.bottom };
	RECT rect;

	int max_dim = 0;
	float scale = 0.f;

	// Get the size of the picture box
	GetDlgItem(PICTURE_BULLET)->GetWindowRect(&EntireRect);
	GetDlgItem(PICTURE_BULLET)->GetWindowRect(&DstRect);

	// Get the rect of the picture box
	GetDlgItem(PICTURE_BULLET)->GetClientRect(&rect);
	GetDlgItem(PICTURE_BULLET)->ClientToScreen(&rect);
	ScreenToClient(&rect);

	// Draw the background
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	FillRect(dc.GetSafeHdc(), &rect, brush);

	{
		max_dim = (RECT_WIDTH(SrcRect) >= RECT_HEIGHT(SrcRect)) ? RECT_WIDTH(SrcRect) : RECT_HEIGHT(SrcRect);
		scale   = ((float)RECT_WIDTH(EntireRect)) / float(max_dim);

		struct
		{
			int x;
			int y;
			int w;
			int h;
		} NewRect;

		if (RECT_WIDTH(SrcRect) >= RECT_HEIGHT(SrcRect))
		{
			NewRect.x = 0;
			NewRect.w = RECT_WIDTH(EntireRect);
			NewRect.h = RECT_HEIGHT(SrcRect) * scale;
			NewRect.y = (RECT_WIDTH(EntireRect) - NewRect.h) / 2;
		}
		else
		{
			NewRect.y = 0;
			NewRect.h = RECT_WIDTH(EntireRect);
			NewRect.w = RECT_WIDTH(SrcRect) * scale;
			NewRect.x = (RECT_WIDTH(EntireRect) - NewRect.w) / 2;
		}

		DstRect.left   = rect.left + GET_CENTER_OFFSET(RECT_WIDTH(EntireRect), NewRect.w);
		DstRect.top    = rect.top + GET_CENTER_OFFSET(RECT_HEIGHT(EntireRect), NewRect.h);
		DstRect.right  = DstRect.left + NewRect.w;
		DstRect.bottom = DstRect.top + NewRect.h;
	}

	// Draw the bullet
	m_BulletImage.AlphaBlend(dc.GetSafeHdc(), DstRect, SrcRect);

	// Draw the hitboxes
	float PixelW = float(RECT_WIDTH(DstRect)) / float(RECT_WIDTH(SrcRect));
	float PixelH = float(RECT_HEIGHT(DstRect)) / float(RECT_HEIGHT(SrcRect));
	int CenterX  = (DstRect.left + int((float)RECT_WIDTH(SrcRect) * PixelW)) - (0x8 * PixelW);
	int CenterY  = (DstRect.top + int((float)RECT_HEIGHT(SrcRect) * PixelH)) - (0x8 * PixelH);

	HBRUSH red   = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH green = CreateSolidBrush(RGB(0, 255, 0));
	HBRUSH fish  = CreateSolidBrush(RGB(156, 173, 193));

	// View box
	{
		RECT ViewBoxRc =
		{
			CenterX - (LONG)roundf(float(pBullet->bullet_size.w) * PixelW),
			CenterY - (LONG)roundf(float(pBullet->bullet_size.h) * PixelH),
			CenterX + (LONG)roundf(float(pBullet->bullet_size.w) * PixelW),
			CenterY + (LONG)roundf(float(pBullet->bullet_size.h) * PixelH)
		};

		if (ViewBoxRc.left   < rect.left)   ViewBoxRc.left   = rect.left;
		if (ViewBoxRc.top    < rect.top)    ViewBoxRc.top    = rect.top;
		if (ViewBoxRc.right  > rect.right)  ViewBoxRc.right  = rect.right;
		if (ViewBoxRc.bottom > rect.bottom) ViewBoxRc.bottom = rect.bottom;

		FrameRect(dc, &ViewBoxRc, green);
	}

	// Hit box
	{
		RECT HitBoxRc =
		{
			CenterX - (LONG)roundf(float(pBullet->collision_size.w) * PixelW),
			CenterY - (LONG)roundf(float(pBullet->collision_size.h) * PixelH),
			CenterX + (LONG)roundf(float(pBullet->collision_size.w) * PixelW),
			CenterY + (LONG)roundf(float(pBullet->collision_size.h) * PixelH)
		};

		if (HitBoxRc.left   < rect.left)   HitBoxRc.left   = rect.left;
		if (HitBoxRc.top    < rect.top)    HitBoxRc.top    = rect.top;
		if (HitBoxRc.right  > rect.right)  HitBoxRc.right  = rect.right;
		if (HitBoxRc.bottom > rect.bottom) HitBoxRc.bottom = rect.bottom;

		FrameRect(dc, &HitBoxRc, red);
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------