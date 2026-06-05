
// FigureOperationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FigureOperation.h"
#include "FigureOperationDlg.h"
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


// CFigureOperationDlg dialog




CFigureOperationDlg::CFigureOperationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFigureOperationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFigureOperationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFigureOperationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_FIGURE_OBJECT_CREATE, &CFigureOperationDlg::OnBnClickedButtonFigureObjectCreate)
	ON_BN_CLICKED(IDC_BUTTON_FIGURE_OBJECT_CLEAR, &CFigureOperationDlg::OnBnClickedButtonFigureObjectClear)
	ON_BN_CLICKED(IDC_BUTTON_FIGURE_OBJECT_EXECUTE, &CFigureOperationDlg::OnBnClickedButtonFigureObjectExecute)
	ON_CBN_SELCHANGE(IDC_COMBO_FIGURE_OBJECT_SELECT1, &CFigureOperationDlg::OnCbnSelchangeComboFigureObjectSelect1)
	ON_CBN_SELCHANGE(IDC_COMBO_FIGURE_OBJECT_SELECT2, &CFigureOperationDlg::OnCbnSelchangeComboFigureObjectSelect2)
END_MESSAGE_MAP()


// CFigureOperationDlg message handlers

BOOL CFigureOperationDlg::OnInitDialog()
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
	
	// 이미지 뷰 생성 // Create image view
 	CResult res = m_viewImage.CreateAndFitParent((size_t)GetDlgItem(IDC_STATIC_IMAGE_VIEW)->GetSafeHwnd());
 
 	if(res.IsFail())
 		ErrorMessageBox(res);

	// 콤보 박스에 선택 항목을 설정한다. // Sets the selected item of the combo box.
	CComboBox* pComboBoxDeclType = (CComboBox*)GetDlgItem(IDC_COMBO_FIGURE_OBJECT_DECL_TYPE);
	pComboBoxDeclType->ResetContent();

	pComboBoxDeclType->AddString(_T("Point"));
	pComboBoxDeclType->AddString(_T("Line"));
	pComboBoxDeclType->AddString(_T("Rect"));
	pComboBoxDeclType->AddString(_T("Quad"));
	pComboBoxDeclType->AddString(_T("Circle"));
	pComboBoxDeclType->AddString(_T("Ellipse"));
	pComboBoxDeclType->AddString(_T("Cubic Spline"));
	pComboBoxDeclType->AddString(_T("Region"));
	pComboBoxDeclType->AddString(_T("Complex Region"));
	pComboBoxDeclType->AddString(_T("Doughnut"));

	pComboBoxDeclType->SetCurSel(0);

	CComboBox* pComboBoxTemplateType = (CComboBox*)GetDlgItem(IDC_COMBO_FIGURE_OBJECT_TEMPLATE_TYPE);
	pComboBoxTemplateType->ResetContent();

	pComboBoxTemplateType->AddString(_T("Int32"));
	pComboBoxTemplateType->AddString(_T("Int64"));
	pComboBoxTemplateType->AddString(_T("Float"));
	pComboBoxTemplateType->AddString(_T("Double"));

	pComboBoxTemplateType->SetCurSel(3);

	CComboBox* pComboOperation = (CComboBox*)GetDlgItem(IDC_COMBO_FIGURE_OBJECT_OPERATION);
	pComboOperation->ResetContent();

	pComboOperation->AddString(_T("Intersection"));
	pComboOperation->AddString(_T("Union"));
	pComboOperation->AddString(_T("Subtraction"));
	pComboOperation->AddString(_T("Exclusive Or"));

	pComboOperation->SetCurSel(0);

	UpdateControls();

	SetTimer(1024, 30, nullptr);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFigureOperationDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFigureOperationDlg::OnPaint()
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
HCURSOR CFigureOperationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFigureOperationDlg::UpdateControls()
{
	CWnd* pWndDeclType = GetDlgItem(IDC_COMBO_FIGURE_OBJECT_DECL_TYPE);
	CWnd* pWndTempType = GetDlgItem(IDC_COMBO_FIGURE_OBJECT_TEMPLATE_TYPE);
	CWnd* pWndCreate = GetDlgItem(IDC_BUTTON_FIGURE_OBJECT_CREATE);
	CWnd* pWndClear = GetDlgItem(IDC_BUTTON_FIGURE_OBJECT_CLEAR);
	CWnd* pWndSelect1 = GetDlgItem(IDC_COMBO_FIGURE_OBJECT_SELECT1);
	CWnd* pWndSelect2 = GetDlgItem(IDC_COMBO_FIGURE_OBJECT_SELECT2);
	CWnd* pWndOperation = GetDlgItem(IDC_COMBO_FIGURE_OBJECT_OPERATION);
	CWnd* pWndExecute = GetDlgItem(IDC_BUTTON_FIGURE_OBJECT_EXECUTE);

	// 이미지 뷰 유효성 체크 // Available the image view.
	if(!m_viewImage.IsAvailable())
	{
		pWndDeclType->EnableWindow(false);
		pWndTempType->EnableWindow(false);
		pWndCreate->EnableWindow(false);
		pWndClear->EnableWindow(false);
		pWndSelect1->EnableWindow(false);
		pWndSelect2->EnableWindow(false);
		pWndOperation->EnableWindow(false);
		pWndExecute->EnableWindow(false);
	}
	else
	{
		pWndDeclType->EnableWindow(true);
		pWndTempType->EnableWindow(true);
		pWndCreate->EnableWindow(true);

		// 이미지 뷰의 Figure object 개수를 얻어온다. // Gets the number of figure objects in the image view.
		pWndClear->EnableWindow(m_viewImage.GetFigureObjectCount() ? true : false);

		pWndSelect1->EnableWindow(true);
		pWndSelect2->EnableWindow(true);
		pWndOperation->EnableWindow(true);
		pWndExecute->EnableWindow(true);
	}

	do 
	{
		CComboBox* pCombo1 = (CComboBox*)GetDlgItem(IDC_COMBO_FIGURE_OBJECT_SELECT1);
		CComboBox* pCombo2 = (CComboBox*)GetDlgItem(IDC_COMBO_FIGURE_OBJECT_SELECT2);

		if(!pCombo1 || !pCombo2)
			break;

		// 이미지 뷰의 Figure object 개수를 얻어온다. // Gets the number of figure objects in the image view.
		size_t stCount = m_viewImage.GetFigureObjectCount();

		if(pCombo1->GetCount() == stCount && pCombo2->GetCount() == stCount)
			break;

		// 콤보 박스에 Figure Object 항목을 설정한다. // Update the figure object items in the combo box.
		UpdateFigureObjectList();
	}
	while (false);
}

EFigureDeclType CFigureOperationDlg::SelectedDeclType()
{
	// 현재 선택된 Decl Type 을 얻어온다. // Gets the currently selected Decl Type.
	EFigureDeclType eReturn = EFigureDeclType_Unknown;

	do 
	{
		CComboBox* pComboBoxDeclType = (CComboBox*)GetDlgItem(IDC_COMBO_FIGURE_OBJECT_DECL_TYPE);
		if(!pComboBoxDeclType)
			break;

		int32_t i32CurSel = pComboBoxDeclType->GetCurSel();

		switch(i32CurSel)
		{
		case 0:
			eReturn = EFigureDeclType_Point;
			break;

		case 1:
			eReturn = EFigureDeclType_Line;
			break;

		case 2:
			eReturn = EFigureDeclType_Rect;
			break;

		case 3:
			eReturn = EFigureDeclType_Quad;
			break;

		case 4:
			eReturn = EFigureDeclType_Circle;
			break;

		case 5:
			eReturn = EFigureDeclType_Ellipse;
			break;

		case 6:
			eReturn = EFigureDeclType_CubicSpline;
			break;

		case 7:
			eReturn = EFigureDeclType_Region;
			break;

		case 8:
			eReturn = EFigureDeclType_ComplexRegion;
			break;

		case 9:
			eReturn = EFigureDeclType_Doughnut;
			break;

		default:
			break;
		}
	}
	while (false);

	return eReturn;
}

void CFigureOperationDlg::UpdateFigureObjectList()
{
	do 
	{
		CComboBox* pCombo1 = (CComboBox*)GetDlgItem(IDC_COMBO_FIGURE_OBJECT_SELECT1);
		CComboBox* pCombo2 = (CComboBox*)GetDlgItem(IDC_COMBO_FIGURE_OBJECT_SELECT2);

		if(!pCombo1 || !pCombo2)
			break;

		int32_t i32Selected1 = __max(0, pCombo1->GetCurSel());
		int32_t i32Selected2 = __max(0, pCombo2->GetCurSel());

		pCombo1->ResetContent();
		pCombo2->ResetContent();

		// 이미지 뷰의 Figure object 개수를 얻어온다. // Retrieve the number of figure objects in the image view.
		size_t stCount = m_viewImage.GetFigureObjectCount();
		if(!stCount)
			break;

		for(size_t i = 0; i < stCount; ++i)
		{
			// 해당 인덱스의 Figure Object 를 얻어온다. // Retrieves the figure object at the specified index.
			CFLFigure* pFigure = m_viewImage.GetFigureObject((int32_t)i);
			if(!pFigure)
				continue;

			// Figure Object 의 이름을 설정한다. // Sets the name of the figure object.
			CFLString<wchar_t> strFigureName = MakeFigureObjectName(i, pFigure);
			if(strFigureName.IsEmpty())
				break;

			// 콤보 박스에 항목을 추가한다. // Adds an item to the combo box.
			pCombo1->AddString(strFigureName);
			pCombo2->AddString(strFigureName);

			// 얻어온 Figure Object 해제 // Releases the retrieved figure object.
			delete pFigure;
			pFigure = nullptr;
		}

		pCombo1->SetCurSel(i32Selected1);
		pCombo2->SetCurSel(i32Selected2);
	}
	while (false);
}

CFLString<wchar_t> CFigureOperationDlg::MakeFigureObjectName(size_t stIndex, CFLFigure* pFigure)
{
	CFLString<wchar_t> strReturn = L"";
	bool bError = false;

	do 
	{
		if(!pFigure)
			break;

		strReturn.Format(L"[%d] ", stIndex);

		// Figure 의 Decl Type 에 따른 이름 설정 // Set the name according to the figure's Decl Type.
		switch(pFigure->GetDeclType())
		{
		case EFigureDeclType_Point:
			strReturn += L"Point";
			break;
			
		case EFigureDeclType_Line:
			strReturn += L"Line";
			break;

		case EFigureDeclType_Rect:
			strReturn += L"Rect";
			break;
			
		case EFigureDeclType_Quad:
			strReturn += L"Quad";
			break;
			
		case EFigureDeclType_Circle:
			strReturn += L"Circle";
			break;

		case EFigureDeclType_Ellipse:
			strReturn += L"Ellipse";
			break;

		case EFigureDeclType_CubicSpline:
			strReturn += L"Cubic Spline";
			break;

		case EFigureDeclType_Region:
			strReturn += L"Region";
			break;

		case EFigureDeclType_ComplexRegion:
			strReturn += L"Complex Region";
			break;

		case EFigureDeclType_Array:
			strReturn += L"Array";
			break;

		case EFigureDeclType_Doughnut:
			strReturn += L"Doughnut";
			break;

		default:
			bError = true;
			break;
		}

		if(bError)
			break;

		// Figure 의 Template Type 에 따른 이름 설정 // Set the name based on the figure's Template Type.
		switch(pFigure->GetTemplateType())
		{
		case EFigureTemplateType_Int32:
			strReturn += L"(Int32)";
			break;

		case EFigureTemplateType_Int64:
			strReturn += L"(Int64)";
			break;

		case EFigureTemplateType_Float:
			strReturn += L"(Float)";
			break;

		case EFigureTemplateType_Double:
			strReturn += L"(Double)";
			break;

		default:
			bError = true;
			break;
		}
	}
	while (false);

	if(bError)
		strReturn = L"";

	return strReturn;
}

void CFigureOperationDlg::DrawSelectedFigure()
{
	// 선택된 Figure Object 를 얻어온다. // Retrieve the selected figure object.
	CFLFigure* pFigure1 = GetSelectedFigure1();
	CFLFigure* pFigure2 = GetSelectedFigure2();

	do 
	{
		// 이미지 뷰 유효성 체크 // Available the image view.
		if(!m_viewImage.IsAvailable())
			break;

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer = m_viewImage.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer.Clear();

		if(!pFigure1 && !pFigure2)
			break;

		if(pFigure1)
		{
			// 아래 함수 DrawFigureImage는 Image좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
			// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일경우 불투명, 0일경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
			// 여기서 0.5, 0.3이므로 옅은 반투명 상태라고 볼 수 있다.
			// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)
			layer.DrawFigureImage(pFigure1, RED, 5, RED, EGUIViewImagePenStyle_Solid, 0.5f, 0.3f);
		}

		if(pFigure2)
		{
			// 아래 함수 DrawFigureImage는 Image좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
			// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일경우 불투명, 0일경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
			// 여기서 0.5, 0.3이므로 옅은 반투명 상태라고 볼 수 있다.
			// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)
			layer.DrawFigureImage(pFigure2, BLUE, 5, BLUE, EGUIViewImagePenStyle_Solid, 0.5f, 0.3f);
		}
	}
	while (false);

	// 얻어온 Figure Object 해제 // Release the retrieved figure object.
	if(pFigure1)
	{
		delete pFigure1;
		pFigure1 = nullptr;
	}

	// 얻어온 Figure Object 해제 // Release the retrieved figure object.
	if(pFigure2)
	{
		delete pFigure2;
		pFigure2 = nullptr;
	}

	// 이미지 뷰를 갱신한다. // Update the image view.
	m_viewImage.Invalidate(true);
}

CFLFigure* CFigureOperationDlg::GetSelectedFigure1()
{
	CFLFigure* pReturn = nullptr;

	do 
	{
		// 이미지 뷰 유효성 체크 // Available the image view.
		if(!m_viewImage.IsAvailable())
			break;

		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_FIGURE_OBJECT_SELECT1);
		if(!pCombo)
			break;

		int32_t i32Selected = pCombo->GetCurSel();
		if(i32Selected < 0)
			break;

		// 해당 인덱스의 Figure Object 를 얻어온다. // Retrieve the figure object at the specified index.
		pReturn = m_viewImage.GetFigureObject(i32Selected);
	}
	while (false);

	return pReturn;
}

CFLFigure* CFigureOperationDlg::GetSelectedFigure2()
{
	CFLFigure* pReturn = nullptr;

	do 
	{
		// 이미지 뷰 유효성 체크 // Available the image view.
		if(!m_viewImage.IsAvailable())
			break;

		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_FIGURE_OBJECT_SELECT2);
		if(!pCombo)
			break;

		int32_t i32Selected = pCombo->GetCurSel();
		if(i32Selected < 0)
			break;

		// 해당 인덱스의 Figure Object 를 얻어온다. // Get the figure object at the specified index.
		pReturn = m_viewImage.GetFigureObject(i32Selected);
	}
	while (false);

	return pReturn;
}

void CFigureOperationDlg::DisplayMessage(CFLString<wchar_t> flstrMessage)
{
	do 
	{
		CEdit* pEditMessage = (CEdit*)GetDlgItem(IDC_EDIT_FIGURE_OPERATION_MESSAGE);
		if(!pEditMessage)
			break;

		pEditMessage->SetWindowText(flstrMessage);
	}
	while (false);
}

void CFigureOperationDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case 1024:
		{
			UpdateControls();
			DrawSelectedFigure();
		}
		break;

	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

HBRUSH CFigureOperationDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if(pWnd->GetDlgCtrlID() == IDC_STATIC_SOURCE_FIGURE)
		pDC->SetBkColor(RGB(255, 128, 128));
	else if(pWnd->GetDlgCtrlID() == IDC_STATIC_DESTINATION_FIGURE)
		pDC->SetBkColor(RGB(128, 128, 255));

	return hbr;
}


BOOL CFigureOperationDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(1024);

	// 이미지 뷰를 종료한다. // Destroy the image view.
	m_viewImage.Destroy();

	return CDialogEx::DestroyWindow();
}

void CFigureOperationDlg::OnBnClickedButtonFigureObjectCreate()
{
	// TODO: Add your control notification handler code here
	CFLFigure* pFlFigure = nullptr;

	do 
	{
		// 이미지 뷰 유효성 체크 // Available the image view.
		if(!m_viewImage.IsAvailable())
			break;

		CComboBox* pComboBoxTemplateType = (CComboBox*)GetDlgItem(IDC_COMBO_FIGURE_OBJECT_TEMPLATE_TYPE);

		if(!pComboBoxTemplateType)
			break;

		int32_t i32TemplateType = pComboBoxTemplateType->GetCurSel() + 1;

		// 이미지 뷰의 캔버스 영역을 얻어온다. // Get the canvas region of the image view.
		CFLRect<int32_t> flrlCanvas = m_viewImage.GetClientRectCanvasRegion();

		// 캔버스 영역의 좌표계를 이미지 영역의 좌표계로 변환한다. // Convert canvas coordinates to image coordinates.
		CFLRect<double> flrdImage =  m_viewImage.ConvertCanvasCoordToImageCoord(flrlCanvas);

		// 이미지 영역을 기준으로 생성될 Figure 의 크기와 모양을 사각형으로 설정한다. // Set the size and shape of the figure to be created as a rectangle based on the image area.
		double f64Width = flrdImage.GetWidth() / 10.;
		double f64Height = flrdImage.GetHeight() / 10.;
		double f64Size = __min(f64Width, f64Height);

		CFLPoint<double> flpdCenter(0, 0);
		flrdImage.GetCenter(flpdCenter);

		CFLRect<double> flrdFigureShape(flpdCenter.x - f64Size, flpdCenter.y - f64Size, flpdCenter.x + f64Size, flpdCenter.y + f64Size);

		// 선택한 Decl Type, Template Type 으로 Figure 를 생성한다. // Create a figure using the selected Decl Type and Template Type.
		// CubicSpline, ComplexRegion 같은 경우에는 Template Type 이 double 형으로 고정이다. // For figures such as CubicSpline and ComplexRegion, the Template Type is fixed to double.
		switch(SelectedDeclType())
		{
		case EFigureDeclType_Point:
			{
				switch(i32TemplateType)
				{
				case EFigureTemplateType_Int32:
					pFlFigure = new CFLPoint<int32_t>();
					break;

				case EFigureTemplateType_Int64:
					pFlFigure = new CFLPoint<int64_t>();
					break;

				case EFigureTemplateType_Float:
					pFlFigure = new CFLPoint<float>();
					break;

				case EFigureTemplateType_Double:
					pFlFigure = new CFLPoint<double>();
					break;

				default:
					break;
				}
			}
			break;

		case EFigureDeclType_Line:
			{
				switch(i32TemplateType)
				{
				case EFigureTemplateType_Int32:
					pFlFigure = new CFLLine<int32_t>();
					break;

				case EFigureTemplateType_Int64:
					pFlFigure = new CFLLine<int64_t>();
					break;

				case EFigureTemplateType_Float:
					pFlFigure = new CFLLine<float>();
					break;

				case EFigureTemplateType_Double:
					pFlFigure = new CFLLine<double>();
					break;

				default:
					break;
				}
			}
			break;

		case EFigureDeclType_Rect:
			{
				switch(i32TemplateType)
				{
				case EFigureTemplateType_Int32:
					pFlFigure = new CFLRect<int32_t>();
					break;

				case EFigureTemplateType_Int64:
					pFlFigure = new CFLRect<int64_t>();
					break;

				case EFigureTemplateType_Float:
					pFlFigure = new CFLRect<float>();
					break;

				case EFigureTemplateType_Double:
					pFlFigure = new CFLRect<double>();
					break;

				default:
					break;
				}
			}
			break;

		case EFigureDeclType_Quad:
			{
				switch(i32TemplateType)
				{
				case EFigureTemplateType_Int32:
					pFlFigure = new CFLQuad<int32_t>();
					break;

				case EFigureTemplateType_Int64:
					pFlFigure = new CFLQuad<int64_t>();
					break;

				case EFigureTemplateType_Float:
					pFlFigure = new CFLQuad<float>();
					break;

				case EFigureTemplateType_Double:
					pFlFigure = new CFLQuad<double>();
					break;

				default:
					break;
				}
			}
			break;

		case EFigureDeclType_Circle:
			{
				switch(i32TemplateType)
				{
				case EFigureTemplateType_Int32:
					pFlFigure = new CFLCircle<int32_t>();
					break;

				case EFigureTemplateType_Int64:
					pFlFigure = new CFLCircle<int64_t>();
					break;

				case EFigureTemplateType_Float:
					pFlFigure = new CFLCircle<float>();
					break;

				case EFigureTemplateType_Double:
					pFlFigure = new CFLCircle<double>();
					break;

				default:
					break;
				}
			}
			break;

		case EFigureDeclType_Ellipse:
			{
				switch(i32TemplateType)
				{
				case EFigureTemplateType_Int32:
					pFlFigure = new CFLEllipse<int32_t>();
					break;

				case EFigureTemplateType_Int64:
					pFlFigure = new CFLEllipse<int64_t>();
					break;

				case EFigureTemplateType_Float:
					pFlFigure = new CFLEllipse<float>();
					break;

				case EFigureTemplateType_Double:
					pFlFigure = new CFLEllipse<double>();
					break;

				default:
					break;
				}
			}
			break;

		case EFigureDeclType_CubicSpline:
			{
				pFlFigure = new CFLCubicSpline();
			}
			break;

		case EFigureDeclType_Region:
			{
				pFlFigure = new CFLRegion();
			}
			break;

		case EFigureDeclType_ComplexRegion:
			{
				pFlFigure = new CFLComplexRegion();
			}
			break;

		case EFigureDeclType_Doughnut:
			{
				switch(i32TemplateType)
				{
				case EFigureTemplateType_Int32:
					pFlFigure = new CFLDoughnut<int32_t>();
					break;

				case EFigureTemplateType_Int64:
					pFlFigure = new CFLDoughnut<int64_t>();
					break;

				case EFigureTemplateType_Float:
					pFlFigure = new CFLDoughnut<float>();
					break;

				case EFigureTemplateType_Double:
					pFlFigure = new CFLDoughnut<double>();
					break;

				default:
					break;
				}
			}

		default:
			break;
		}

		if(!pFlFigure)
			break;

		// 생성된 Figure 에 사각형을 설정함으로써 각 형상에 맞게 구성한다. // Configure the created figure by assigning a rectangle to it.
		pFlFigure->Set(flrdFigureShape);

		// 이미지 뷰에 Figure object 를 생성한다. // Create a figure object in the image view.
		// 가장 마지막 파라미터는 활성화 되는 메뉴의 구성이며, EAvailableFigureContextMenu_All 가 기본 메뉴를 활성화 한다. // The last parameter specifies the set of context menu items to be enabled. EAvailableFigureContextMenu_All enables the default context menu.
		// 활성화 하고자 하는 메뉴를 추가 혹은 제거 하기 위해서는 enum 값을 비트 연산으로 넣어주면 된다. // To add or remove specific menu items, combine enum values using bitwise operations.
		// ex) EAvailableFigureContextMenu_None -> 활성화 되는 메뉴 없음
		//     EAvailableFigureContextMenu_All -> 전체 메뉴 활성화
		//     EAvailableFigureContextMenu_DeclType | EAvailableFigureContextMenu_TemplateType -> Decl Type, Template Type 변환 메뉴 활성화
		// ex) EAvailableFigureContextMenu_None -> No menu items are enabled.
		//     EAvailableFigureContextMenu_All -> All menu items are enabled.
		//     EAvailableFigureContextMenu_DeclType | EAvailableFigureContextMenu_TemplateType -> Enables the Decl Type and Template Type conversion menu items.

		m_viewImage.PushBackFigureObject(pFlFigure, EAvailableFigureContextMenu_All);

		// 이미지 뷰를 갱신한다. // Update the image view.
		m_viewImage.Invalidate(true);

		// 콤보 박스에 Figure Object 항목을 설정한다. // Update the figure object items in the combo box.
		UpdateFigureObjectList();
	}
	while (false);

	// 생성한 Figure 객체 해제 // Release the created figure object.
	// PushBackFigureObject() 함수 내부에서 Figure 복사가 되기 때문에 생성했던 객체를 해제해 줘야한다. // The figure object must be released because PushBackFigureObject() creates an internal copy of the figure.
	if(pFlFigure)
	{
		delete pFlFigure;
		pFlFigure = nullptr;
	}
}

void CFigureOperationDlg::OnBnClickedButtonFigureObjectClear()
{
	// TODO: Add your control notification handler code here
	do 
	{
		if(!m_viewImage.IsAvailable())
			break;

		// 현재 이미지 뷰에 있는 Figure Objects 를 제거한다. // Remove all figure objects from the current image view.
		m_viewImage.ClearFigureObject();

		// 이미지 뷰를 갱신한다. // Update the image view.
		m_viewImage.Invalidate(true);

		// 콤보 박스에 Figure Object 항목을 설정한다. // Update the figure object items in the combo box.
		UpdateFigureObjectList();
	}
	while (false);
}


void CFigureOperationDlg::OnBnClickedButtonFigureObjectExecute()
{
	// TODO: Add your control notification handler code here
	CFLFigure* pFigure1 = nullptr;
	CFLFigure* pFigure2 = nullptr;
	CResult res = EResult_UnknownError;

	do 
	{
		CComboBox* pComboOperation = (CComboBox*)GetDlgItem(IDC_COMBO_FIGURE_OBJECT_OPERATION);
		if(!pComboOperation)
			break;

		// 선택된 Figure Object 를 얻어온다. // Get the selected figure object.
		pFigure1 = GetSelectedFigure1();
		pFigure2 = GetSelectedFigure2();

		if(!pFigure1 || !pFigure2)
		{
			res = EResult_InvalidObject;
			break;
		}

		// Operation 결과를 얻기 위해 FLFigureArray 를 생성한다. // Create an FLFigureArray to retrieve the operation result.
		CFLFigureArray flfa;

		switch(pComboOperation->GetCurSel())
		{
		case 0:
			// Intersection Operation 수행 // Execute the Intersection operation.
			res = pFigure1->GetRegionOfIntersection(pFigure2, &flfa);
			break;

		case 1:
			// Union Operation 수행 // Execute the Union operation.
			res = pFigure1->GetRegionOfUnion(pFigure2, &flfa);
			break;

		case 2:
			// Subtraction Operation 수행 // Execute the Subtraction operation.
			res = pFigure1->GetRegionOfSubtraction(pFigure2, &flfa);
			break;

		case 3:
			// Exclusive Or Operation 수행 // Execute the Exclusive Or operation.
			res = pFigure1->GetRegionOfExclusiveOr(pFigure2, &flfa);
			break;
		}

		// 수행 결과를 확인한다. // Check the operation result.
		if(IsFail(res))
			break;

		if(!flfa.GetCount())
			break;

		// 이미지 뷰에 Figure object 를 생성한다. // Create a figure object in the image view.
		if(flfa.GetCount() == 1)
			m_viewImage.PushBackFigureObject(flfa.GetAt(0), EAvailableFigureContextMenu_All);
		else
			m_viewImage.PushBackFigureObject(&flfa, EAvailableFigureContextMenu_All);

		// 이미지 뷰를 갱신한다. // Update the image view.
		m_viewImage.Invalidate(true);

		// 콤보 박스에 Figure Object 항목을 설정한다. // Update the figure object items in the combo box.
		UpdateFigureObjectList();
	}
	while (false);

	// 얻어온 Figure Object 해제 // Release the retrieved figure object.
	if(pFigure1)
	{
		delete pFigure1;
		pFigure1 = nullptr;
	}

	// 얻어온 Figure Object 해제 // Release the retrieved figure object.
	if(pFigure2)
	{
		delete pFigure2;
		pFigure2 = nullptr;
	}

	// 수행 결과 메세지를 표시한다. // Display the operation result message.
	DisplayMessage(res.GetString());
}

void CFigureOperationDlg::OnCbnSelchangeComboFigureObjectSelect1()
{
	// TODO: Add your control notification handler code here
	// 선택된 Figure 를 그린다. // Draw the selected figure.
	DrawSelectedFigure();
}


void CFigureOperationDlg::OnCbnSelchangeComboFigureObjectSelect2()
{
	// TODO: Add your control notification handler code here
	// 선택된 Figure 를 그린다. // Draw the selected figure.
	DrawSelectedFigure();
}
