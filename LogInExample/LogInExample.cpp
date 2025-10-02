
// LogInExample.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "LogInExample.h"

#include "PropertyLogInExample.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLogInExampleApp

BEGIN_MESSAGE_MAP(CLogInExampleApp, CWinApp)
END_MESSAGE_MAP()


// CLogInExampleApp 생성

CLogInExampleApp::CLogInExampleApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CLogInExampleApp 개체입니다.

CLogInExampleApp theApp;


// CLogInExampleApp 초기화

BOOL CLogInExampleApp::InitInstance()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CWinAppEx::InitInstance();

	if(CWinApp* pApp = AfxGetApp())
	{
		if(pApp->m_pszAppName)
		{
			free((void*)pApp->m_pszAppName);
			pApp->m_pszAppName = nullptr;
		}
		pApp->m_pszAppName = _wcsdup(L"LogIn Example App");

		if(pApp->m_pMainWnd && ::IsWindow(pApp->m_pMainWnd->GetSafeHwnd()))
			pApp->m_pMainWnd->SetWindowText(L"LogIn Example App");
	}

	SetRegistryKey(L"Fourth Logic Incorporated\\Examples\\Cpp");

	// GUI Manager 초기화 전처리를 수행합니다.
	CGUIManager::PreInitialize();

	// 로그인 메뉴를 추가합니다.
	CGUIManager::AddMenuItem(new CGUIMenuItemLogIn);

	// 추가한 로그인 메뉴의 상위 폴더의 이름을 얻어 옵니다.
	Base::CFLString<wchar_t> strMenuItemParentFullPath;
	int64_t i64MenuItemCount = GUI::CGUIManager::GetMenuItemCount();

	for(int64_t i = i64MenuItemCount - 1; i >= 0; --i)
	{
		CGUIMenuItemLogIn* pMenuItemLogIn = dynamic_cast<CGUIMenuItemLogIn*>(GUI::CGUIManager::GetMenuItem(i));

		if(pMenuItemLogIn)
		{
			Base::CFLString<wchar_t> strMenuItemName = pMenuItemLogIn->GetName();
			strMenuItemParentFullPath = L"FLImaging@" + pMenuItemLogIn->GetFullPath();
			strMenuItemParentFullPath.Replace(L"@" + strMenuItemName, L"");
			break;
		}
	}

	// 로그인 메뉴를 트리에서 펼칩니다.
	if(!strMenuItemParentFullPath.IsEmpty())
	{
		bool bExpand = true;
		bool bRecursive = true;
		bool bAllowToLoad = false;
		CGUIManager::AddPredefinedMenuItemExpandSetting(strMenuItemParentFullPath, bExpand, bRecursive, bAllowToLoad);
	}

	CGUIManager::AddPredefinedMenuItemExpandSetting(L"FLImaging@Menu", true, true, false);
	CGUIManager::AddPredefinedMenuItemExpandSetting(L"FLImaging@View Manager", false, true, false);
	CGUIManager::AddPredefinedMenuItemExpandSetting(L"FLImaging@Model Manager", false, true, false);

	// 예제 프로퍼티 메뉴를 등록합니다.
	CGUIManager::RegisterMenu(CPropertyLogInExample(), L"Log In Property Example", L"Menu", false);

	// GUI 상에서 사용될 뷰를 생성합니다.
	CGUIFixedViewDeclaration* pDeclarationCam = new CGUIFixedViewDeclaration;
	pDeclarationCam->SetMultiDocTemplateName("CGUIDocImageCGUIChildFrameImageCGUIViewImage");
	pDeclarationCam->SetViewName(L"Image View");
	CGUIManagerView::AddFixedViewDeclaration(pDeclarationCam);

	// GUI 상에서 뷰의 배열을 설정합니다.
	CGUIFixedViewPlacement fvp;
	fvp.SetFixedViewDeclaration(pDeclarationCam);
	fvp.SetPlacement(CFLRect<double>(0., 0., 1., 1.));

	CGUIFixedViewPlacementSet fvpSet1(false);
	fvpSet1.SetName(L"Prompt Image View Set 1");
	fvpSet1.AddFixedViewPlacement(fvp);

	CGUIManagerView::AddFixedViewPlacementSet(fvpSet1);

	// 인덱스에 해당하는 뷰의 설정으로 선택합니다.
	CGUIManagerView::SelectFixedViewPlacementSet(0);

	CGUIManager::PreInitializePaneVisibility(true, false, false, false, false);

	// 모델 관리자를 초기화 합니다.
	CGUIManagerModel::Initialize();

	// GUI 관리자를 초기화 합니다.
	CGUIManager::Initialize();

	// Menu tree 의 expand 세팅을 불러 옵니다.
	CGUIManager::LoadMenuItemExpandSetting();

	// Main Frame 위치를 설정합니다.
	CGUIMainFrame* pMF = nullptr;

	if(AfxGetApp() && AfxGetApp()->m_pMainWnd)
		pMF = dynamic_cast<CGUIMainFrame*>(AfxGetApp()->m_pMainWnd);

	if(pMF)
	{
 		pMF->ShowWindow(SW_SHOWMAXIMIZED);
		pMF->ModifyStyle(WS_MAXIMIZEBOX, 0, 0);
		pMF->SetWindowPos(NULL, 0, 0, 1280, 1024, NULL);
	}

	// "Log In Property Example" 이라는 이름의 메뉴 아이템 찾기
	CPropertyLogInExample* pPropertyMenu = dynamic_cast<CPropertyLogInExample*>(CGUIManager::GetMenuItem(L"Log In Property Example", L"Menu"));

	// "Log In Property Example" 이라는 이름의 메뉴 아이템이 있을 경우, 프로퍼티 창 열기
	if(pPropertyMenu)
	{
		if(!pPropertyMenu->OnLButtonDoubleClick())
			pPropertyMenu->OnLButtonDoubleClick();

		pPropertyMenu->GetPaneProperties()->UndockPane();
		pPropertyMenu->GetPaneProperties()->SetWindowPos(nullptr, 0, 0, 400, 1000, SWP_NOMOVE | SWP_NOZORDER);
		pPropertyMenu->GetPaneProperties()->DockToFrameWindow(CBRS_ALIGN_RIGHT, nullptr, DT_DOCK_FIRST, nullptr, -1, true);
		pPropertyMenu->GetPaneProperties()->GetPropertyGridCtrl()->Invalidate();
		pPropertyMenu->ShowPaneWindow();
	}

	return TRUE;
}
