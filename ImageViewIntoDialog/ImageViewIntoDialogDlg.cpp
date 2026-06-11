
// ImageViewIntoDialogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageViewIntoDialog.h"
#include "ImageViewIntoDialogDlg.h"
#include "afxdialogex.h"

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
	enum
	{
		IDD = IDD_ABOUTBOX
	};

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


// CImageViewIntoDialogDlg dialog




CImageViewIntoDialogDlg::CImageViewIntoDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImageViewIntoDialogDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageViewIntoDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageViewIntoDialogDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_FIGURE_OBJECT_CREATE, &CImageViewIntoDialogDlg::OnBnClickedButtonFigureObjectCreate)
	ON_BN_CLICKED(IDC_BUTTON_FIGURE_OBJECT_POP_FRONT, &CImageViewIntoDialogDlg::OnBnClickedButtonFigureObjectPopFront)
END_MESSAGE_MAP()


// CImageViewIntoDialogDlg message handlers

BOOL CImageViewIntoDialogDlg::OnInitDialog()
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

	// 이미지 뷰 생성 // Create image view
	CResult res = m_viewImage.CreateAndFitParent((size_t)GetDlgItem(IDC_STATIC_IMAGE_VIEW)->GetSafeHwnd());

	if(res.IsFail())
		ErrorMessageBox(res);

	UpdateControls();

	SetTimer(1024, 100, nullptr);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CImageViewIntoDialogDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CImageViewIntoDialogDlg::OnPaint()
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
HCURSOR CImageViewIntoDialogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CImageViewIntoDialogDlg::UpdateControls()
{
	bool bEnable = false;

	do
	{
		// 이미지 뷰 유효성 체크 // Available the image view.
		if(!m_viewImage.IsAvailable())
			break;

		// 이미지 뷰의 Figure object 개수를 얻어온다. // Get the number of figure objects in the image view.
		if(!m_viewImage.GetFigureObjectCount())
			break;

		bEnable = true;
	}
	while(false);

	GetDlgItem(IDC_BUTTON_FIGURE_OBJECT_POP_FRONT)->EnableWindow(bEnable);
}

void CImageViewIntoDialogDlg::OnTimer(UINT_PTR nIDEvent)
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


BOOL CImageViewIntoDialogDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(1024);

	// 이미지 뷰를 종료한다. // Destroy the image view.
	m_viewImage.Destroy();

	return CDialogEx::DestroyWindow();
}

void CImageViewIntoDialogDlg::OnBnClickedButtonFigureObjectCreate()
{
	// TODO: Add your control notification handler code here

	do
	{
		// 이미지 뷰 유효성 체크 // Available the image view.
		if(!m_viewImage.IsAvailable())
			break;

		// 이미지 뷰의 캔버스 영역을 얻어온다. // Get the canvas region of the image view.
		CFLRect<int32_t> flrlCanvas = m_viewImage.GetClientRectCanvasRegion();

		// 캔버스 영역의 좌표계를 이미지 영역의 좌표계로 변환한다. // Convert the canvas region coordinates to image region coordinates.
		CFLRect<double> flrdImage = m_viewImage.ConvertCanvasCoordToImageCoord(flrlCanvas);

		// 이미지 영역을 기준으로 생성될 Figure 의 크기와 모양을 사각형으로 설정한다. // Set the size and shape of the figure to be created as a rectangle based on the image region.
		double f64Width = flrdImage.GetWidth() / 10.;
		double f64Height = flrdImage.GetHeight() / 10.;
		double f64Size = __min(f64Width, f64Height);

		CFLPoint<double> flpdCenter(0, 0);
		flrdImage.GetCenter(flpdCenter);

		CFLRect<double> flrdFigure(flpdCenter.x - f64Size, flpdCenter.y - f64Size, flpdCenter.x + f64Size, flpdCenter.y + f64Size);

		// 이미지 뷰에 Figure object 를 생성한다.
		// 가장 마지막 파라미터는 활성화 되는 메뉴의 구성이며, EAvailableFigureContextMenu_All 가 기본 메뉴를 활성화 한다.
		// 활성화 하고자 하는 메뉴를 추가 혹은 제거 하기 위해서는 enum 값을 비트 연산으로 넣어주면 된다.
		// ex) EAvailableFigureContextMenu_None -> 활성화 되는 메뉴 없음
		//     EAvailableFigureContextMenu_All -> 전체 메뉴 활성화
		//     EAvailableFigureContextMenu_DeclType | EAvailableFigureContextMenu_TemplateType -> Decl Type, Template Type 변환 메뉴 활성화
		// Create a figure object in the image view.
		// The last parameter specifies the configuration of the enabled context menus.
		// EAvailableFigureContextMenu_All enables the default context menus.
		// To add or remove specific menus, combine enum values using bitwise operations.
		// Examples:
		// EAvailableFigureContextMenu_None
		//     -> No context menus are enabled.
		// EAvailableFigureContextMenu_All
		//     -> All context menus are enabled.
		// EAvailableFigureContextMenu_DeclType | EAvailableFigureContextMenu_TemplateType
		//     -> Enables the Decl Type and Template Type conversion menus.
		m_viewImage.PushBackFigureObject(&flrdFigure, EAvailableFigureContextMenu_All);
	}
	while(false);
}


void CImageViewIntoDialogDlg::OnBnClickedButtonFigureObjectPopFront()
{
	// TODO: Add your control notification handler code here
	CFLFigure* pFlFigure = nullptr;
	CString strFigureInfo = _T("Error");

	do
	{
		// 이미지 뷰 유효성 체크 // Available the image view.
		if(!m_viewImage.IsAvailable())
			break;

		// 이미지 뷰의 맨 앞의 Figure 를 제거하면서 얻어온다. // Get and remove the first figure object from the image view.
		pFlFigure = m_viewImage.PopFrontFigureObject();
		if(!pFlFigure)
			break;

		// Figure 를 문자열로 변환합니다. // Convert the figure to a string.
		CFLString<wchar_t> flStrFigure = CFigureUtilities::ConvertFigureObjectToString(pFlFigure);

		strFigureInfo = flStrFigure;
	}
	while(false);

	GetDlgItem(IDC_EDIT_FIGURE_OBEJCT_INFO)->SetWindowText(strFigureInfo);

	// 얻어온 Figure 객체 해제 // Delete the retrieved figure object.
	if(pFlFigure)
	{
		delete pFlFigure;
		pFlFigure = nullptr;
	}
}