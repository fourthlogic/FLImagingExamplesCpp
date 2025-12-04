
// DialogProgressExample.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "FullyConnected_XOR.h"

#include "PropertyMenuFullyConnected_XOR.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFullyConnectedXORApp

BEGIN_MESSAGE_MAP(CFullyConnectedXORApp, CWinApp)
END_MESSAGE_MAP()


// CFullyConnectedXORApp 생성

CFullyConnectedXORApp::CFullyConnectedXORApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CFullyConnectedXORApp 개체입니다.

CFullyConnectedXORApp theApp;


// CFullyConnectedXORApp 초기화

BOOL CFullyConnectedXORApp::InitInstance()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CWinAppEx::InitInstance();

	CGUIManager::SetAppTitle(L"Fully-Connected XOR Example App");
	CGUIManager::SetRegistrySubKeyName(CGUIManager::GetAppTitle());
	SetRegistryKey(L"Fourth Logic Incorporated");

	// GUI Manager 초기화 전처리를 수행합니다.
	CGUIManager::PreInitialize();

	// 예제 프로퍼티 메뉴를 등록합니다.
	CGUIManager::RegisterMenu(CPropertyMenuFullyConnectedXOR(), L"Fully-Connected XOR Example", L"Menu", false);

	// 이미지 뷰 생성 메뉴를 추가합니다.
	CGUIManager::ModifyPaneCreationOnInitialize(EPaneType_None, EPaneType_ViewGraph);

	// 인덱스에 해당하는 뷰의 설정으로 선택합니다.
	CGUIManager::PreInitializePaneVisibility(true, false, false, false, false, true, false);

	// 모델 관리자를 초기화 합니다.
	CGUIManagerModel::Initialize();

	// GUI 관리자를 초기화 합니다.
	CGUIManager::Initialize();

	// Main Frame 위치를 설정합니다.
	CGUIMainFrame* pMF = nullptr;

	if(AfxGetApp() && AfxGetApp()->m_pMainWnd)
		pMF = dynamic_cast<CGUIMainFrame*>(AfxGetApp()->m_pMainWnd);

	if(pMF)
	{
		CGUIPaneMenu* pPaneMenu = pMF->GetPaneMenu();

		if(pPaneMenu)
		{
			if(pPaneMenu->CanAutoHide())
				pPaneMenu->SetAutoHideMode(TRUE, CBRS_ALIGN_LEFT);
		}

		pMF->ShowWindow(SW_SHOWMAXIMIZED);
		pMF->ModifyStyle(WS_MAXIMIZEBOX, 0, 0);
		pMF->SetWindowPos(NULL, 0, 0, 1200, 600, NULL);
	}

	CGUIPaneViewGraph* pPaneGraph = CGUIManagerPane::GetPaneViewGraph();
	CGUIViewGraph* pViewGraph = nullptr;

	if(pPaneGraph)
	{
		pViewGraph = pPaneGraph->GetViewGraph();
		pViewGraph->SetWindowPos(nullptr, 0, 0, 800, 600, SWP_NOMOVE | SWP_NOZORDER);
		pPaneGraph->DockToFrameWindow(CBRS_ALIGN_TOP, nullptr, DT_DOCK_FIRST, nullptr, -1, true);
	}

	CPropertyMenuFullyConnectedXOR* pPropertyMenu = dynamic_cast<CPropertyMenuFullyConnectedXOR*>(CGUIManager::GetMenuItem(L"Fully-Connected XOR Example", L"Menu"));

	if(pPropertyMenu)
	{
		if(!pPropertyMenu->OnLButtonDoubleClick())
			pPropertyMenu->OnLButtonDoubleClick();

		pPropertyMenu->GetPaneProperties()->UndockPane();
		pPropertyMenu->GetPaneProperties()->SetWindowPos(nullptr, 0, 0, 400, 600, SWP_NOMOVE | SWP_NOZORDER);
		pPropertyMenu->GetPaneProperties()->DockToFrameWindow(CBRS_ALIGN_RIGHT, nullptr, DT_DOCK_FIRST, nullptr, -1, true);
		pPropertyMenu->GetPaneProperties()->GetPropertyGridCtrl()->Invalidate();
		pPropertyMenu->ShowPaneWindow();
	}

	return TRUE;
}
