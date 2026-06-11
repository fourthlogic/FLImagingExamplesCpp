
// GraphViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GraphView.h"
#include "GraphViewDlg.h"
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
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGraphViewDlg dialog




CGraphViewDlg::CGraphViewDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGraphViewDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bLockControls = false;
}

void CGraphViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGraphViewDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_GRAPH_VIEW, &CGraphViewDlg::OnBnClickedButtonOpenGraphView)
	ON_BN_CLICKED(IDC_BUTTON_TERMINATE_GRAPH_VIEW, &CGraphViewDlg::OnBnClickedButtonTerminateGraphView)
	ON_BN_CLICKED(IDC_BUTTON_CHART_ADD, &CGraphViewDlg::OnBnClickedButtonChartAdd)
	ON_BN_CLICKED(IDC_BUTTON_CHART_CLEAR, &CGraphViewDlg::OnBnClickedButtonChartClear)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_GRAPH, &CGraphViewDlg::OnBnClickedButtonLoadGraph)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_GRAPH, &CGraphViewDlg::OnBnClickedButtonSaveGraph)
END_MESSAGE_MAP()


// CGraphViewDlg message handlers

BOOL CGraphViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
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
	CComboBox* pComboBoxChartType = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_TYPE);
	pComboBoxChartType->Clear();

	pComboBoxChartType->AddString(_T("Bar"));
	pComboBoxChartType->AddString(_T("Line"));
	pComboBoxChartType->AddString(_T("Scatter"));

	pComboBoxChartType->SetCurSel(0);

	UpdateControls();

	SetTimer(1024, 100, nullptr);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGraphViewDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
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

void CGraphViewDlg::OnPaint()
{
	if (IsIconic())
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
HCURSOR CGraphViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGraphViewDlg::UpdateControls()
{
	if(m_bLockControls)
	{
		GetDlgItem(IDC_BUTTON_OPEN_GRAPH_VIEW)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_TERMINATE_GRAPH_VIEW)->EnableWindow(false);

		GetDlgItem(IDC_BUTTON_LOAD_GRAPH)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_SAVE_GRAPH)->EnableWindow(false);

		GetDlgItem(IDC_EDIT_CHART_NAME)->EnableWindow(false);
		GetDlgItem(IDC_COMBO_CHART_TYPE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_CHART_ADD)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_CHART_CLEAR)->EnableWindow(false);
	}
	// 그래프 뷰 유효성 체크 // Check the validity of the graph view.
	else if(!m_viewGraph.IsAvailable())
	{
		GetDlgItem(IDC_BUTTON_OPEN_GRAPH_VIEW)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_TERMINATE_GRAPH_VIEW)->EnableWindow(false);

		GetDlgItem(IDC_BUTTON_LOAD_GRAPH)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_SAVE_GRAPH)->EnableWindow(false);

		GetDlgItem(IDC_EDIT_CHART_NAME)->EnableWindow(false);
		GetDlgItem(IDC_COMBO_CHART_TYPE)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_CHART_ADD)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_CHART_CLEAR)->EnableWindow(false);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_OPEN_GRAPH_VIEW)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON_TERMINATE_GRAPH_VIEW)->EnableWindow(true);

		GetDlgItem(IDC_BUTTON_LOAD_GRAPH)->EnableWindow(true);

		// 그래프 차트 데이터/수식 데이터의 존재 유무를 얻어 온다. // Get whether graph chart data or polynomial data exists.
		if(m_viewGraph.DoesGraphExist())
		{
			GetDlgItem(IDC_BUTTON_SAVE_GRAPH)->EnableWindow(true);
			GetDlgItem(IDC_BUTTON_CHART_CLEAR)->EnableWindow(true);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_SAVE_GRAPH)->EnableWindow(false);
			GetDlgItem(IDC_BUTTON_CHART_CLEAR)->EnableWindow(false);
		}

		GetDlgItem(IDC_EDIT_CHART_NAME)->EnableWindow(true);
		GetDlgItem(IDC_COMBO_CHART_TYPE)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON_CHART_ADD)->EnableWindow(true);
	}
}

void CGraphViewDlg::LockControls(bool bLock)
{
	m_bLockControls = bLock;
	UpdateControls();
}

void CGraphViewDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case 1024:
		{
			UpdateControls();
		}
		break;

	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CGraphViewDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(1024);

	// 그래프 뷰를 종료한다. // Destroy the graph view.
	m_viewGraph.Destroy();

	return CDialogEx::DestroyWindow();
}

void CGraphViewDlg::OnBnClickedButtonOpenGraphView()
{
	// TODO: Add your control notification handler code here
	do 
	{
		// 그래프 뷰 유효성 체크 // Check the validity of the graph view.
		if(m_viewGraph.IsAvailable())
			break;

		// 그래프 뷰 생성 // Create a graph view.
		CResult res = m_viewGraph.Create(0, 0, 500, 500);

		if(res.IsFail())
			ErrorMessageBox(res);
	}
	while (false);
}


void CGraphViewDlg::OnBnClickedButtonTerminateGraphView()
{
	// TODO: Add your control notification handler code here
	do 
	{
		// 그래프 뷰 유효성 체크 // Check the validity of the graph view.
		if(!m_viewGraph.IsAvailable())
			break;

		// 그래프 뷰를 종료한다. // Destroy the graph view.
		CResult res = m_viewGraph.Destroy();

		if(res.IsFail())
			ErrorMessageBox(res);
	}
	while (false);
}


void CGraphViewDlg::OnBnClickedButtonChartAdd()
{
	// TODO: Add your control notification handler code here
	srand((unsigned int)time(nullptr));

	do 
	{
		// 그래프 뷰 유효성 체크 // Check the validity of the graph view.
		if(!m_viewGraph.IsAvailable())
			break;

		CEdit* pEditChartName = (CEdit*)GetDlgItem(IDC_EDIT_CHART_NAME);
		if(!pEditChartName)
			break;

		// 입력한 차트이름을 얻어온다. // Get the entered chart name.
		CString strChartName = _T("");
		pEditChartName->GetWindowText(strChartName);

		CFLString<wchar_t> flstrChartName(strChartName);
		if(flstrChartName.IsEmpty())
			flstrChartName = L"Chart";

		CComboBox* pComboBoxChartType = (CComboBox*)GetDlgItem(IDC_COMBO_CHART_TYPE);
		if(!pComboBoxChartType)
			break;

		// 선택한 차트타입을 얻어온다. // Get the selected chart type.
		EChartType eChartType = (EChartType)(pComboBoxChartType->GetCurSel() + 1);

		// 랜덤으로 10개의 데이터를 생성한다. // Generate 10 random data points.
		const size_t stDataCount = 10;
		double arrF64DataX1[stDataCount] = { 0., };
		double arrF64DataY1[stDataCount] = { 0., };

		for(size_t i = 0; i < stDataCount; ++i)
		{
			arrF64DataX1[i] = (double)(rand() % 100);
			arrF64DataY1[i] = (double)(rand() % 100);
		}

		// 그래프에 생성한 데이터를 추가한다. // Add the generated data points to the graph.
		m_viewGraph.Plot(arrF64DataX1, arrF64DataY1, stDataCount, eChartType, COLOR(rand()%255, rand()%255, rand()%255), flstrChartName, nullptr);

		// 그래프 뷰를 갱신한다 // Invalidate the graph view.
		m_viewGraph.Invalidate();
	}
	while (false);
}


void CGraphViewDlg::OnBnClickedButtonChartClear()
{
	// TODO: Add your control notification handler code here
	do 
	{
		// 그래프 뷰 유효성 체크 // Check the validity of the graph view.
		if(!m_viewGraph.IsAvailable())
			break;

		// 그래프 뷰의 데이터를 초기화한다 // Clear the graph view data.
		m_viewGraph.Clear();

		// 그래프 뷰를 갱신한다 // Invalidate the graph view.
		m_viewGraph.Invalidate();
	}
	while (false);
}


void CGraphViewDlg::OnBnClickedButtonLoadGraph()
{
	// TODO: Add your control notification handler code here
	do 
	{
		// 그래프 뷰 유효성 체크 // Check the validity of the graph view.
		if(!m_viewGraph.IsAvailable())
			break;

		LockControls(true);

		// 그래프 파일 로드 다이얼로그를 활성화 시킨다.
		// 가장 마지막 파라미터로 로드 옵션을 지정한다.
		// ex) EViewGraphLoadOption_Load -> 그래프 파일
		//     EViewGraphLoadOption_Load | EViewGraphLoadOption_OpenDialog 그래프 파일 로드 다이얼로그 활성화
		// Open the graph file load dialog.
		// Specify the load option using the last parameter.
		// Examples:
		// EViewGraphLoadOption_Load
		//     -> Load a graph file.
		// EViewGraphLoadOption_Load | EViewGraphLoadOption_OpenDialog
		//     -> Open the graph file load dialog.
		m_viewGraph.Load(nullptr, EViewGraphLoadOption_Load | EViewGraphLoadOption_OpenDialog);

		// 그래프 뷰를 갱신한다 // Invalidate the graph view.
		m_viewGraph.Invalidate();

		LockControls(false);
	}
	while (false);
}


void CGraphViewDlg::OnBnClickedButtonSaveGraph()
{
	// TODO: Add your control notification handler code here
	do 
	{
		// 그래프 뷰 유효성 체크 // Check the validity of the graph view.
		if(!m_viewGraph.IsAvailable())
			break;

		LockControls(true);

		// 그래프 저장 다이얼로그를 활성화 시킨다. // Open the graph save dialog.
		m_viewGraph.Save();

		LockControls(false);
	}
	while (false);
}
