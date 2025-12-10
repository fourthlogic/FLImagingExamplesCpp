
// GraphViewContextMenuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GraphViewContextMenu.h"
#include "GraphViewContextMenuDlg.h"
#include "afxdialogex.h"
#include <time.h>

#include "../CommonHeader/ErrorMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum {
		IDD = IDD_ABOUTBOX
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

// CGraphViewContextMenuDlg dialog




CGraphViewContextMenuDlg::CGraphViewContextMenuDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGraphViewContextMenuDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_i32VScrollPosPrev = 0;
}

void CGraphViewContextMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCROLLBAR, m_VScrollBar);
}

BEGIN_MESSAGE_MAP(CGraphViewContextMenuDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND_RANGE(IDC_RADIO_SHOW, IDC_RADIO_HIDE, &CGraphViewContextMenuDlg::OnRangeRadioGroupHide)
	ON_COMMAND_RANGE(IDC_RADIO_ALL, IDC_RADIO_NONE, &CGraphViewContextMenuDlg::OnRangeRadioGroupSelectAll)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CGraphViewContextMenuDlg::OnBnClickedButtonApply)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CGraphViewContextMenuDlg message handlers

BOOL CGraphViewContextMenuDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if(pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if(!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	CResult res = m_viewGraph.CreateAndFitParent((size_t)GetDlgItem(IDC_STATIC_GRAPH_VIEW)->GetSafeHwnd());

	if(res.IsFail())
		ErrorMessageBox(res);

	// Check whether the unavailable context menu is displayed
	// 이용 불가능한 컨텍스트 메뉴를 디스플레이하는지 여부를 확인
	if(m_viewGraph.IsUnavailableContextMenuVisible())
	{
		if(CButton* pRadioShow = (CButton*)GetDlgItem(IDC_RADIO_SHOW))
			pRadioShow->SetCheck(BST_CHECKED);
		if(CButton* pRadioHide = (CButton*)GetDlgItem(IDC_RADIO_HIDE))
			pRadioHide->SetCheck(BST_UNCHECKED);
	}
	else
	{
		if(CButton* pRadioShow = (CButton*)GetDlgItem(IDC_RADIO_SHOW))
			pRadioShow->SetCheck(BST_UNCHECKED);
		if(CButton* pRadioHide = (CButton*)GetDlgItem(IDC_RADIO_HIDE))
			pRadioHide->SetCheck(BST_CHECKED);
	}

	CreateContextMenuCheckbox();

	SetTimer(1024, 100, nullptr);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGraphViewContextMenuDlg::OnRangeRadioGroupHide(UINT uID)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	do
	{
		if(!m_viewGraph.IsAvailable())
			break;

		switch(uID)
		{
		case IDC_RADIO_SHOW:
			// 이용 불가능한 메뉴를 디스플레이 // Display unavailable menu
			m_viewGraph.ShowUnavailableContextMenu(true); 
			break;
		case IDC_RADIO_HIDE:
			// 이용 불가능한 메뉴를 숨김 // Hide unavailable menu
			m_viewGraph.ShowUnavailableContextMenu(false); 
			break;
		}
	}
	while(false);
}

void CGraphViewContextMenuDlg::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 사용 가능한 그래프 뷰 메뉴 // Available Graph View Context Menu 
	EAvailableViewGraphContextMenu eMenu = EAvailableViewGraphContextMenu_None;

	if(CWnd* pWndStatic = GetDlgItem(IDC_STATIC_ACM))
	{
		CWnd* pChild = pWndStatic->GetWindow(GW_CHILD);
		int32_t i = 0;

		while(pChild)
		{
			BOOL bCheck = ((CButton*)pChild)->GetCheck();

			// 체크 선택된 메뉴 아이템을 eMenu 에 OR 연산하여 추가
			// Add the checked menu item to eMenu using OR operation
			if(bCheck == TRUE)
				eMenu |= (EAvailableViewGraphContextMenu)m_flaAvailableContextMenu.GetAt(i);

			pChild = pChild->GetNextWindow();
			++i;
		}
	}

	// 선택된 메뉴 아이템들을 그래프 뷰의 이용 가능한 메뉴에 적용
	// Apply the selected menu items to the available menu in the graph view
	m_viewGraph.SetAvailableViewGraphContextMenu(eMenu);

	// 팁: 아래와 같이 기존 메뉴에서 한두 개의 메뉴만 제외 가능
	// Tip: It is possible to exclude only one or two menus from the existing menu as shown below
	eMenu = m_viewGraph.GetAvailableViewGraphContextMenu();
	eMenu &= ~(EAvailableViewGraphContextMenu_IndicateMinMax | EAvailableViewGraphContextMenu_SetOpacityOfLegend);
}



////////////////////////
///  Internal functions
void CGraphViewContextMenuDlg::CreateContextMenuCheckbox()
{
	do
	{
		CFLArray<CFLString<wchar_t>> flaCheckBoxString;

		// 파일 불러오기 메뉴 // File load menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_Load);
		m_flaCheckBoxId.PushBack(IDC_CHECK_LOAD);
		flaCheckBoxString.PushBack(L"Load");

		// 불러온 파일 추가 메뉴 // File append menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_Append);
		m_flaCheckBoxId.PushBack(IDC_CHECK_APPEND);
		flaCheckBoxString.PushBack(L"Append");

		// 파일 저장 메뉴 // File save menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_Save);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SAVE);
		flaCheckBoxString.PushBack(L"Save");

		// 파일 닫기 메뉴 // File close menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_Close);
		m_flaCheckBoxId.PushBack(IDC_CHECK_CLOSE);
		flaCheckBoxString.PushBack(L"Close");

		// 그래프 클리어 메뉴 // Clear graph menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_Clear);
		m_flaCheckBoxId.PushBack(IDC_CHECK_CLEAR);
		flaCheckBoxString.PushBack(L"Clear");

		// 그래프를 클립보드에 복사 메뉴 // Copy graph to clipboard menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_Copy);
		m_flaCheckBoxId.PushBack(IDC_CHECK_COPY);
		flaCheckBoxString.PushBack(L"Copy");

		// 클립보드 데이터를 붙여넣기 메뉴 // Paste clipboard data menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ClearThenPaste);
		m_flaCheckBoxId.PushBack(IDC_CHECK_CLEAR_THEN_PASTE);
		flaCheckBoxString.PushBack(L"Clear Then Paste");

		// 그래프 데이터 추가로 붙여넣기 메뉴 // Append clipboard data menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_Paste);
		m_flaCheckBoxId.PushBack(IDC_CHECK_PASTE);
		flaCheckBoxString.PushBack(L"Paste");
		
		// 더블 클릭으로 띄운 값 지우기 메뉴 // Clear double-clicked values menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ClearPointAnnotation);
		m_flaCheckBoxId.PushBack(IDC_CHECK_CLEAR_DISPLAYED_VALUE);
		flaCheckBoxString.PushBack(L"Clear Point Annotation");

		// 차트 모양 변경 메뉴 // Change chart type menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ChangeChartType);
		m_flaCheckBoxId.PushBack(IDC_CHECK_CHANGE_CHART_TYPE);
		flaCheckBoxString.PushBack(L"Change Chart Type");

		// 툴바 보이기 메뉴 // Show toolbar menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowToolBar);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_TOOLBAR);
		flaCheckBoxString.PushBack(L"Show Toolbar");

		// 확대/축소 메뉴 // Zoom menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_Zoom);
		m_flaCheckBoxId.PushBack(IDC_CHECK_ZOOM);
		flaCheckBoxString.PushBack(L"Zoom");

		// 축 배율 기본 모드 메뉴 // Default zoom axis menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ZoomAxisNone);
		m_flaCheckBoxId.PushBack(IDC_CHECK_ZOOM_AXIS_NONE);
		flaCheckBoxString.PushBack(L"Zoom Axis None");

		// x축 배율 모드 메뉴 // Horizontal zoom axis menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ZoomAxisHorz);
		m_flaCheckBoxId.PushBack(IDC_CHECK_ZOOM_AXIS_HORZ);
		flaCheckBoxString.PushBack(L"Zoom Axis Horizontal");

		// y축 배율 모드 메뉴 // Vertical zoom axis menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ZoomAxisVert);
		m_flaCheckBoxId.PushBack(IDC_CHECK_ZOOM_AXIS_VERT);
		flaCheckBoxString.PushBack(L"Zoom Axis Vertical");

		// 패닝 메뉴 // Panning menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_Panning);
		m_flaCheckBoxId.PushBack(IDC_CHECK_PANNING);
		flaCheckBoxString.PushBack(L"Panning");

		// 그래프 뷰 설정 메뉴 // View settings menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ViewSettings);
		m_flaCheckBoxId.PushBack(IDC_CHECK_VIEW_SETTINGS);
		flaCheckBoxString.PushBack(L"View Settings");

		// 도움말 메뉴 // Help menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_Help);
		m_flaCheckBoxId.PushBack(IDC_CHECK_HELP);
		flaCheckBoxString.PushBack(L"Help");

		// 색상 변경 메뉴 // Change color menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ChangeColor);
		m_flaCheckBoxId.PushBack(IDC_CHECK_CHANGE_COLOR);
		flaCheckBoxString.PushBack(L"Change Color");

		// 그래프 이름 변경 메뉴 // Edit chart name menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_EditChartName);
		m_flaCheckBoxId.PushBack(IDC_CHECK_EDIT_CHART_NAME);
		flaCheckBoxString.PushBack(L"Edit Chart Name");

		// 십자선 표시 메뉴 // Show crosshair menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowCrosshair);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_CROSSHAIR);
		flaCheckBoxString.PushBack(L"Show Crosshair");

		// 범례 표시 메뉴 // Show legend menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowLegend);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_LEGEND);
		flaCheckBoxString.PushBack(L"Show Legend");

		// 더블 클릭으로 팝업한 값 표시 여부 설정 메뉴 // Menu item for toggling the visibility of values displayed by double-click interaction.
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowPointAnnotation);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_POINT_ANNOTATION);
		flaCheckBoxString.PushBack(L"Show Point Annotation");

		// 십자선 자석 모드 메뉴 // Magnet crosshair menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_MagnetCrosshair);
		m_flaCheckBoxId.PushBack(IDC_CHECK_MAGNET_CROSSHAIR);
		flaCheckBoxString.PushBack(L"Magnet Crosshair");

		// 그래프 순서 변경 메뉴 // Change graph order menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ChangeGraphOrder);
		m_flaCheckBoxId.PushBack(IDC_CHECK_CHANGE_GRAPH_ORDER);
		flaCheckBoxString.PushBack(L"Change Graph Order");

		// 추세선 그리기 메뉴 // Draw trend line menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_GetTrendline);
		m_flaCheckBoxId.PushBack(IDC_CHECK_GET_TREND_LINE);
		flaCheckBoxString.PushBack(L"Get Trendline");
		
		// 축 이름 변경 메뉴 // Edit axis label menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_EditAxisLabel);
		m_flaCheckBoxId.PushBack(IDC_CHECK_EDIT_AXIS_LABEL);
		flaCheckBoxString.PushBack(L"Edit Axis Label");

		// x축 y축 교체 메뉴 // Switch axis menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_SwitchAxis);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SWITCH_AXIS);
		flaCheckBoxString.PushBack(L"Switch Axis");

		// 수식 수정 메뉴 // Edit expression menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_EditExpression);
		m_flaCheckBoxId.PushBack(IDC_CHECK_EDIT_EXPRESSION);
		flaCheckBoxString.PushBack(L"Edit Expression");

		// 수식 추가 메뉴 // Add expression menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_AddExpression);
		m_flaCheckBoxId.PushBack(IDC_CHECK_ADD_EXPRESSION);
		flaCheckBoxString.PushBack(L"Add Expression");

		// 데이터 추가 메뉴 // Add data menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_AddData);
		m_flaCheckBoxId.PushBack(IDC_CHECK_ADD_DATA);
		flaCheckBoxString.PushBack(L"Add Data");

		// 클릭으로 데이터 추가 메뉴 // Add data by click menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_AddDataByClick);
		m_flaCheckBoxId.PushBack(IDC_CHECK_ADD_DATA_BY_CLICK);
		flaCheckBoxString.PushBack(L"Add Data By Click");

		// 그래프 보이기/숨기기 메뉴 // Show/hide graph menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowGraph);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_GRAPH);
		flaCheckBoxString.PushBack(L"Show Graph");

		// 그래프 삭제 메뉴 // Remove graph menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_RemoveGraph);
		m_flaCheckBoxId.PushBack(IDC_CHECK_REMOVE_GRAPH);
		flaCheckBoxString.PushBack(L"Remove Graph");

		// 데이터 삭제 메뉴 // Remove data menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_RemoveData);
		m_flaCheckBoxId.PushBack(IDC_CHECK_REMOVE_DATA);
		flaCheckBoxString.PushBack(L"Remove Data");

		// 데이터 값 수정 메뉴 // Edit data menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_EditData);
		m_flaCheckBoxId.PushBack(IDC_CHECK_EDIT_DATA);
		flaCheckBoxString.PushBack(L"Edit Data");

		// 최대/최솟값 표시 메뉴 // Indicate min/max menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_IndicateMinMax);
		m_flaCheckBoxId.PushBack(IDC_CHECK_INDICATE_MIN_MAX);
		flaCheckBoxString.PushBack(L"Indicate Min/Max");

		// 표시 범위 설정 메뉴 // Set range menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_SetRange);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SET_RANGE);
		flaCheckBoxString.PushBack(L"Set Range");

		// 범례 투명도 설정 메뉴 // Set legend opacity menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_SetOpacityOfLegend);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SET_OPACITY_OF_LEGEND);
		flaCheckBoxString.PushBack(L"Set Opacity Of Legend");

		// 레이어 보이기 메뉴 // Show layers menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowLayers);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_LAYERS);
		flaCheckBoxString.PushBack(L"Show layers");

		// 레이어 속성 창 메뉴 // Show layer properties pane dialog
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_LayerProperties);
		m_flaCheckBoxId.PushBack(IDC_CHECK_LAYER_PROPERTIES);
		flaCheckBoxString.PushBack(L"Layer Properties");

		// 레이어 정리 메뉴 // Clear layers menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ClearLayers);
		m_flaCheckBoxId.PushBack(IDC_CHECK_CLEAR_LAYERS);
		flaCheckBoxString.PushBack(L"Clear Layers");

		// 라이트 모드 테마 메뉴 // Light mode theme menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ThemeLightMode);
		m_flaCheckBoxId.PushBack(IDC_CHECK_THEME_LIGHT);
		flaCheckBoxString.PushBack(L"Theme Light Mode");

		// 다크 모드 테마 메뉴 // Dark mode theme menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ThemeDarkMode);
		m_flaCheckBoxId.PushBack(IDC_CHECK_THEME_DARK);
		flaCheckBoxString.PushBack(L"Theme Dark Mode");

		// 뷰 동기화 메뉴 // View synchronization menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_SyncView);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SYNC_VIEW);
		flaCheckBoxString.PushBack(L"Synchronize View");

		// 윈도우 동기화 메뉴 // Window synchronization menu
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_SyncWindow);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SYNC_WINDOW);
		flaCheckBoxString.PushBack(L"Synchronize Window");

		// Horizontal axis visibility
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowAxis_Horz);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_AXIS_HORZ);
		flaCheckBoxString.PushBack(L"Show Horizontal Axis");

		// Vertical axis visibility
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowAxis_Vert);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_AXIS_VERT);
		flaCheckBoxString.PushBack(L"Show Vertical Axis");

		// Horizontal axis label visibility
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowAxisLabel_Horz);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_AXIS_LABEL_HORZ);
		flaCheckBoxString.PushBack(L"Show Horizontal Axis Label");

		// Vertical axis label visibility
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowAxisLabel_Vert);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_AXIS_LABEL_VERT);
		flaCheckBoxString.PushBack(L"Show Vertical Axis Label");

		// Horizontal tick marks visibility
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowAxisTick_Horz);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_AXIS_TICK_HORZ);
		flaCheckBoxString.PushBack(L"Show Horizontal Axis Tick");

		// Vertical tick marks visibility
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowAxisTick_Vert);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_AXIS_TICK_VERT);
		flaCheckBoxString.PushBack(L"Show Vertical Axis Tick");

		// Horizontal tick labels visibility
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowAxisTickLabel_Horz);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_AXIS_TICK_LABELS_HORZ);
		flaCheckBoxString.PushBack(L"Show Horizontal Axis Tick Labels");

		// Vertical tick labels visibility
		m_flaAvailableContextMenu.PushBack(EAvailableViewGraphContextMenu_ShowAxisTickLabel_Vert);
		m_flaCheckBoxId.PushBack(IDC_CHECK_SHOW_AXIS_TICK_LABELS_VERT);
		flaCheckBoxString.PushBack(L"Show Vertical Axis Tick Labels");

		CWnd* pWndStatic = GetDlgItem(IDC_STATIC_ACM);
		pWndStatic->SetParent(this);
		pWndStatic->ModifyStyleEx(0, WS_EX_COMPOSITED);

		CRect rtWndStatic;
		pWndStatic->GetWindowRect(rtWndStatic);
		m_VScrollBar.SetParent(this);

		EAvailableViewGraphContextMenu eAvailbaleContextMenu = m_viewGraph.GetAvailableViewGraphContextMenu();

		CRect rt;
		CButton* pWndRadio = (CButton*)GetDlgItem(IDC_RADIO_NONE);
		CFont* pFont = pWndRadio->GetFont();

		pWndRadio->SetParent(this);
		pWndRadio->SetCheck(eAvailbaleContextMenu == EAvailableViewGraphContextMenu_None ? BST_CHECKED : BST_UNCHECKED);

		pWndRadio = (CButton*)GetDlgItem(IDC_RADIO_ALL);
		pWndRadio->GetWindowRect(rt);
		pWndRadio->SetParent(this);
		pWndRadio->SetCheck(eAvailbaleContextMenu == EAvailableViewGraphContextMenu_All ? BST_CHECKED : BST_UNCHECKED);

		int32_t i32Height = rt.Height() + 2;
		pWndStatic->ScreenToClient(rt);
		rt.left = 2;
		rt.top = 2;
		rt.bottom = rt.top + i32Height - 2;
		rt.right = rt.left + rtWndStatic.Width() - 2;

		for(int64_t i = 0; i < m_flaCheckBoxId.GetCount(); ++i)
		{
			CButton* pCB = new CButton;

			pCB->Create(flaCheckBoxString.GetAt(i), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFT, rt, pWndStatic, m_flaCheckBoxId.GetAt(i));

			pCB->ModifyStyleEx(0, WS_EX_COMPOSITED);
			pCB->SetFont(pFont);
			BOOL bCheck = (eAvailbaleContextMenu & (EAvailableViewGraphContextMenu)m_flaAvailableContextMenu.GetAt(i)) != EAvailableViewGraphContextMenu_None ? TRUE : FALSE;
			pCB->SetCheck(bCheck);
			rt.OffsetRect(0, i32Height);
		}

		{
			int32_t i32WndHeight = rtWndStatic.Height() - i32Height - 4;
			int32_t i32WndContentsHeight = ((int32_t)m_flaCheckBoxId.GetCount() - 1) * i32Height + 4;

			m_i32VScrollMax = 0;
			int i32VPageSize = 0;
			int i32VScrollPos = 0;

			if(i32WndHeight < i32WndContentsHeight)
			{
				m_i32VScrollMax = i32WndContentsHeight - 1;
				i32VPageSize = i32WndHeight;
				i32VScrollPos = __max(0, __min(i32VScrollPos, i32WndContentsHeight - i32VPageSize - 1));
			}

			if(i32WndHeight >= i32WndContentsHeight)
				CWnd::EnableScrollBarCtrl(SB_VERT, FALSE);
			else
			{
				SCROLLINFO si;

				si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
				si.nMin = 0;
				si.nMax = m_i32VScrollMax;
				si.nPos = i32VScrollPos;
				si.nPage = i32VPageSize;
				m_VScrollBar.SetScrollPos(i32VScrollPos);
				m_VScrollBar.SetScrollInfo(&si, TRUE);
			}
		}
	}
	while(false);
}

BOOL CGraphViewContextMenuDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CGraphViewContextMenuDlg::UpdateContentPosition(int32_t i32CurPos)
{
	if(CWnd* pWndStatic = GetDlgItem(IDC_STATIC_ACM))
	{
		CRect rectStatic;
		pWndStatic->GetClientRect(rectStatic);

		CWnd* pChild = pWndStatic->GetWindow(GW_CHILD);

		while(pChild)
		{
			CRect rect;
			pChild->GetWindowRect(&rect);
			pWndStatic->ScreenToClient(&rect);

			// 이전 위치 무효화
			pWndStatic->RedrawWindow(&rect, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW);

			// 새로운 위치 설정
			rect.OffsetRect(0, -i32CurPos);
			pChild->MoveWindow(&rect);
			pWndStatic->RedrawWindow(&rect, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW);

			pChild = pChild->GetNextWindow();
		}
	}
}

void CGraphViewContextMenuDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGraphViewContextMenuDlg::OnPaint()
{
	if(IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGraphViewContextMenuDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGraphViewContextMenuDlg::OnRangeRadioGroupSelectAll(UINT uID)
{
	int32_t i32CheckStatus = BST_CHECKED;

	switch(uID)
	{
	case IDC_RADIO_NONE:
		{
			if(CButton* pBtnNone = (CButton*)GetDlgItem(IDC_RADIO_NONE))			
				pBtnNone->SetCheck(BST_CHECKED);
			if(CButton* pBtnAll = (CButton*)GetDlgItem(IDC_RADIO_ALL))			
				pBtnAll->SetCheck(BST_UNCHECKED);

			i32CheckStatus = BST_UNCHECKED;
		}
		break;
	case IDC_RADIO_ALL:
		{
			if(CButton* pBtnNone = (CButton*)GetDlgItem(IDC_RADIO_NONE))
				pBtnNone->SetCheck(BST_UNCHECKED);
			if(CButton* pBtnAll = (CButton*)GetDlgItem(IDC_RADIO_ALL))
				pBtnAll->SetCheck(BST_CHECKED);
		}
		break;
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(CWnd* pWndStatic = GetDlgItem(IDC_STATIC_ACM))
	{
		CWnd* pChild = pWndStatic->GetWindow(GW_CHILD);

		while(pChild)
		{
			((CButton*)pChild)->SetCheck(i32CheckStatus);
			pChild = pChild->GetNextWindow();
		}
	}
}


void CGraphViewContextMenuDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if(pScrollBar && pScrollBar == &m_VScrollBar)
	{
		int32_t i32CurPos = m_VScrollBar.GetScrollPos(); // 현재 위치 가져오기

		switch(nSBCode)
		{
		case SB_LINEUP:
			i32CurPos = max(i32CurPos - 22, 0);
			break;

		case SB_LINEDOWN:
			i32CurPos = min(i32CurPos + 22, m_i32VScrollMax);
			break;

		case SB_PAGEUP:
			i32CurPos = max(i32CurPos - 66, 0);
			break;

		case SB_PAGEDOWN: 
			i32CurPos = min(i32CurPos + 66, m_i32VScrollMax);
			break;

		case SB_THUMBTRACK:
			i32CurPos = nPos;
			break;
		}

		m_VScrollBar.SetScrollPos(i32CurPos);
		int32_t i32CurDiff = i32CurPos - m_i32VScrollPosPrev;

		UpdateContentPosition(i32CurDiff);
		m_i32VScrollPosPrev = i32CurPos;
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CGraphViewContextMenuDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	do
	{
		CPoint ptGlobal;
		::GetCursorPos(&ptGlobal);
		CWnd* pWndStatic = GetDlgItem(IDC_STATIC_ACM);
		CRect rtStatic;
		pWndStatic->GetWindowRect(rtStatic);
		BOOL bPtInRect = rtStatic.PtInRect(ptGlobal);

		if(!bPtInRect)
		{
			CWnd* pWndFind = WindowFromPoint(ptGlobal);

			if(pWndFind != GetDlgItem(IDC_SCROLLBAR))
				break;
		}

		BOOL bUpward = zDelta > 0 ? TRUE : FALSE;
		int32_t i32Delta = zDelta > 0 ? zDelta : -1 * (int32_t)zDelta;

		UINT u32WheelScrollLines;

		::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &u32WheelScrollLines, 0);

		if(u32WheelScrollLines == WHEEL_PAGESCROLL)
			SendMessage(WM_VSCROLL, MAKEWPARAM((bUpward) ? SB_PAGEUP : SB_PAGEDOWN, 0), (LPARAM)m_VScrollBar.GetSafeHwnd());
		else
		{
			int32_t i32Lines = (i32Delta * u32WheelScrollLines) / WHEEL_DELTA;

			while(i32Lines--)
				SendMessage(WM_VSCROLL, MAKEWPARAM((bUpward) ? SB_LINEUP : SB_LINEDOWN, 0), (LPARAM)m_VScrollBar.GetSafeHwnd());
		}

		return TRUE;
	}
	while(false);

	return TRUE;
}

BOOL CGraphViewContextMenuDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(1024);

	m_viewGraph.Destroy();

	return CDialogEx::DestroyWindow();
}
