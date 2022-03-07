#pragma once
#include "afxwin.h"

#include "CaveMapInfoDlg.h"
#include "TileSelectDlg.h"
#include "CaveMapZoneInfo.h"
#include "ScriptEditorDlg.h"

#define ENTITY_SORT_CATEGORY           0
#define ENTITY_SORT_ALPHA              1
#define ENTITY_SORT_ID                 2

#define MAP_BACKGROUND  0
#define MAP_BACK_TILES  1
#define MAP_FRONT_TILES 2
#define MAP_TILE_TYPES  3
#define MAP_NET_GROUPS  4

#define MAP_DISPLAY_LAST 5

#define MAX_UNDO       100


struct EntityStack
{
	Entity enity;
	int index;
	char item;
};

struct MapStack
{
	int width;//width of map (in tiles)
	int height;//height of map (in tiles)

	unsigned char* tiles;//list of all tiles in map
};

struct Stack
{
	Stack();
	~Stack();

	CString desc;
	EntityStack* ePtr;
	MapStack* mPtr;
};



// CaveMapEditDlg dialog

class MapTabManager;
class CaveMapEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CaveMapEditDlg)

public:
	CaveMapEditDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CaveMapEditDlg();

// Dialog Data
	enum { IDD = DIALOG_MAP_EDIT };

	unsigned char copied_tiles[1024];
	bool useCopyTiles;
	int copied_xsize;
	int copied_ysize;
	int copy_valid_sec_num;

	bool oldEntityMode;
	bool oldUseCopyTiles;
	int oldcopied_xsize;
	int oldcopied_ysize;
	
	CMenu popup;//used for popup menu selections
	CMenu conMenu;//the real context menu

	static CaveMapEditDlg* us;

	NPCtable npcTable;//list all sorts of useful NPC info
	bool move;//used for ordering mouse down, mouse move functions
	bool esc;//used to prevent [Esc] from closing window but still allowing the [X] to be used

	bool input;//used to prevent an initial mouse down from registering
	bool lclick;//left click
	bool rclick;//right click
	bool oldRClick;

	int oldX;//old Window coords used to move tileWindow
	int oldY;

	bool entityMoved;//used for dragging and dropping enitites
	bool movingEntity;
	int selectedEntity;
	int entitySort;

	Entity copyEntity;//used to store entity for copying

	int mapIndex;
	bool loadMap(CaveMapINFO* info, int index);//load map
	void reloadMap();//reload map data
	TileSelectDlg tileWindow;
	void Refresh(bool regen = 0);
	bool redraw;//should the map be redrawn(true) or just repainted(false)
	bool changes;//have changes been made to map
	CaveMapInfoDlg m_MapInfo;
	CaveMapInfoDlg m_EntityInfo;
	CaveMapZoneInfo m_NetGroupInfo;
	ScriptEditorDlg m_ScriptEditorDlg;
	CaveMap caveMap;
	CMenu m_OurMenu;

	Stack undoStack[MAX_UNDO];
	Stack redoStack[MAX_UNDO];
	int undoPtr;
	int undoStart;
	int undoEnd;

	CMenu* GetRealMenu();
	void AddUndo(int type, CString desc);
	void Undo();
	void Redo();
	bool CanUndo();
	bool CanRedo();
	void FixUndoMenu();
	MapTabManager* m_MapTabMgr;
protected:
	HACCEL m_hAccel; // accelerator table
	HICON m_hIcon;

	bool init;//has dialog been initialized?

	int offX;//map offset in pixels
	int offY;//map offset in pixels

	int xCursor, yCursor;//used to display tile highlighter
	int oldXC, oldYC;//used to track old cursor position
	int CopyStartX, CopyStartY;//used to track old cursor position

	double scale;//current zoom scale of map in display
	CBitmap bmpMem[5];//used for flicker free display (sort of)

	void UpdateScrollBars();
	void PaintTiles(int xC, int yC);//helper for placing new tiles
	void PaintNetGroup(int xC, int yC);//helper for setting net group
	void FindEntity(CPoint point);//used to find entity that has been clicked on
	void FillTiles(int x, int y, char match, int oX = 0, int oY = 0);//helper for filling area with tiles
	void ReplaceTileBlock(int x, int y, int x2, int y2);
	void ReplaceTiles(int x, int y, char oldTile);//helper for replacing tiles
	void RedrawMap(CDC &dc, bool regen = 0);
	void DisplayEntity(CDC& cdc2, CBrush& brush, int index);
	void DisplayEntitySprite(CDC& cdc2, int index);
	void ListEntityTypes();//fills the Entity type combo box

	afx_msg void OnPaint();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
	void SetChanges(bool bValue);
	void UpdateEntity();//called by child to update entity info that's be changed
	CScrollBar m_ScrollVert;
	CScrollBar m_ScrollHorz;

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedFrontTiles();
	afx_msg void OnBnClickedBackTiles();
	afx_msg void OnBnClickedBackground();
	afx_msg void OnBnClickedEntity();
	afx_msg void OnBnClickedMap();
	afx_msg void OnBnClickedEntities();
	afx_msg void OnBnClickedTileGrid();
	afx_msg void OnBnClickedTileTypes();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedProperties();
	CStatic m_ViewFrame;//this is the map view frame
	CStatic view;//this is a child of the map view frame and is the actual image
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedMapScript();
	afx_msg void OnBnClickedSave();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnMapShiftmap();
	afx_msg void OnEntityAddentity();
	afx_msg void OnEntityDeleteentity();
	afx_msg void OnEntityCopyentity();
	afx_msg void OnEntityPasteentity();
	afx_msg void OnSpawnHere();
	afx_msg void OnZoom05x();
	afx_msg void OnZoom1x();
	afx_msg void OnZoom2x();
	afx_msg void OnZoom3x();
	afx_msg void OnZoom4x();
	afx_msg void OnMapEditscript();
	afx_msg void OnMapProperties();
	afx_msg void OnMapSave();
	virtual void OnOK();
	afx_msg void OnMove(int x, int y);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMapSetstartlocation();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnBnClickedEntitySprites();
	afx_msg void OnBnClickedEntityText();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnSortentitiiesbyIdnumber();
	afx_msg void OnSortentitiiesbyCategory();
	afx_msg void OnSortentitiiesbyName();
	afx_msg void OnBnClickedNetgroup();

	inline double GetMapZoom() { return scale; }
	inline void SetMapZoom(double newscale) { if (newscale == scale) return; scale = newscale; Refresh(); }
};
