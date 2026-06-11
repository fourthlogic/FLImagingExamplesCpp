
// GraphViewIntoDialogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GraphViewIntoDialog.h"
#include "GraphViewIntoDialogDlg.h"
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


// CGraphViewIntoDialogDlg dialog




CGraphViewIntoDialogDlg::CGraphViewIntoDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGraphViewIntoDialogDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGraphViewIntoDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGraphViewIntoDialogDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_GRAPH_ADD, &CGraphViewIntoDialogDlg::OnBnClickedButtonGraphAdd)
	ON_BN_CLICKED(IDC_BUTTON_GRAPH_CLEAR, &CGraphViewIntoDialogDlg::OnBnClickedButtonGraphClear)
END_MESSAGE_MAP()


// CGraphViewIntoDialogDlg message handlers

BOOL CGraphViewIntoDialogDlg::OnInitDialog()
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

	// 그래프 뷰 생성 // Create a graph view.
	CResult res = m_viewGraph.CreateAndFitParent((size_t)GetDlgItem(IDC_STATIC_GRAPH_VIEW)->GetSafeHwnd());

	if(res.IsFail())
		ErrorMessageBox(res);

	SetDlgItemText(IDC_EDIT_GRAPH_EQUATION_A, L"0.003");
	SetDlgItemText(IDC_EDIT_GRAPH_EQUATION_B, L"-0.003");
	SetDlgItemText(IDC_EDIT_GRAPH_EQUATION_C, L"-1");
	SetDlgItemText(IDC_EDIT_GRAPH_EQUATION_D, L"1");
	SetDlgItemText(IDC_EDIT_GRAPH_EQUATION_E, L"0");

	OnBnClickedButtonGraphAdd();

	UpdateControls();

	SetTimer(1024, 100, nullptr);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CGraphViewIntoDialogDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			if(pMsg->wParam == VK_RETURN)
				OnBnClickedButtonGraphAdd();

			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CGraphViewIntoDialogDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGraphViewIntoDialogDlg::OnPaint()
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
HCURSOR CGraphViewIntoDialogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGraphViewIntoDialogDlg::UpdateControls()
{
	// 그래프 뷰 유효성 체크 // Check the validity of the graph view.
	GetDlgItem(IDC_BUTTON_GRAPH_ADD)->EnableWindow(m_viewGraph.IsAvailable());

	// 그래프 뷰 유효성 체크, 그래프 뷰 데이터 존재 여부 체크 // Check the validity of the graph view and the existence of graph data.
	GetDlgItem(IDC_BUTTON_GRAPH_CLEAR)->EnableWindow(m_viewGraph.IsAvailable() && m_viewGraph.DoesGraphExist());
}

void CGraphViewIntoDialogDlg::OnTimer(UINT_PTR nIDEvent)
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


BOOL CGraphViewIntoDialogDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(1024);

	// 그래프 뷰를 종료한다. // Destroy the graph view.
	m_viewGraph.Destroy();

	return CDialogEx::DestroyWindow();
}


void CGraphViewIntoDialogDlg::OnBnClickedButtonGraphAdd()
{
	// TODO: Add your control notification handler code here
	CFLString<wchar_t> flstrInfo = L"";

	srand((unsigned int)time(nullptr));

	do 
	{
		// 그래프 뷰 유효성 체크 // Check the validity of the graph view.
		if(!m_viewGraph.IsAvailable())
			break;

		CEdit* pEditEquation[5] = { nullptr, };

		pEditEquation[0] = (CEdit*)GetDlgItem(IDC_EDIT_GRAPH_EQUATION_A);
		pEditEquation[1] = (CEdit*)GetDlgItem(IDC_EDIT_GRAPH_EQUATION_B);
		pEditEquation[2] = (CEdit*)GetDlgItem(IDC_EDIT_GRAPH_EQUATION_C);
		pEditEquation[3] = (CEdit*)GetDlgItem(IDC_EDIT_GRAPH_EQUATION_D);
		pEditEquation[4] = (CEdit*)GetDlgItem(IDC_EDIT_GRAPH_EQUATION_E);

		bool bError = false;

		for(int32_t i = 0; i < 5; ++i)
		{
			if(pEditEquation[i])
				continue;

			bError = true;
			break;
		}

		if(bError)
			break;

		double arrF64Coef[5] = { 0, };
		CFLString<wchar_t> flstrEquation = L"";

		for(int32_t i = 0; i < 5; ++i)
		{
			CString strCoef = _T("");
			pEditEquation[i]->GetWindowText(strCoef);

			arrF64Coef[i] = _wtof(strCoef);
			if(arrF64Coef[i] == 0.)
				continue;

			if(!flstrEquation.IsEmpty() && arrF64Coef[i] > 0.)
				flstrEquation += L" + ";

			CFLString<wchar_t> flstrFormat = L"";
			if(i == 3)
			{
				flstrFormat.Format(L"%lf*x", arrF64Coef[i]);
			}
			else if(i == 4)
			{
				flstrFormat.Format(L"%lf", arrF64Coef[i]);
			}
			else
			{
				flstrFormat.Format(L"%lf*x^%d", arrF64Coef[i], 4 - i);
			}

			flstrEquation += flstrFormat;
		}

		if(flstrEquation.IsEmpty())
			break;

		// 방정식의 해를 얻어올 객체 생성 // Create an object to receive the roots of the equation.
		CFLArray<std::complex<double>> flaEquationResult;

		// 4차 방정식의 해를 얻어온다. // Get the roots of the quartic equation.
		if(IsOK(CEquation::Quartic(arrF64Coef[0], arrF64Coef[1], arrF64Coef[2], arrF64Coef[3], arrF64Coef[4], &flaEquationResult)))
		{
			for(int64_t i = 0; i < flaEquationResult.GetCount(); ++i)
			{
				std::complex<double> cp = flaEquationResult.GetAt(i);
				CFLString<wchar_t> flstrCP = L"";
				if(cp.imag() == 0.)
					flstrCP.Format(L"%lf", cp.real());
				else if(cp.imag() > 0)
					flstrCP.Format(L"%lf+%lfi", cp.real(), cp.imag());
				else
					flstrCP.Format(L"%lf%lfi", cp.real(), cp.imag());

				flstrInfo += flstrCP + L"\r\n";
			}
		}

		// 수식 객체 생성 // Create an expression object.
		CExpression exp;
		// 수식 문자열을 설정한다 // Set the expression string.
		exp.SetExpression(flstrEquation);

		// 그래프 뷰에 수식 데이터를 추가한다 // Add the expression data to the graph view.
		m_viewGraph.Plot(&exp, COLOR(rand()%256, rand()%256, rand()%256));

		// 그래프 뷰를 갱신한다 // Invalidate the graph view.
		m_viewGraph.Invalidate();
	}
	while(false);

	GetDlgItem(IDC_EDIT_EQUATION_INFO)->SetWindowText(flstrInfo);
}


void CGraphViewIntoDialogDlg::OnBnClickedButtonGraphClear()
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
	while(false);
}
