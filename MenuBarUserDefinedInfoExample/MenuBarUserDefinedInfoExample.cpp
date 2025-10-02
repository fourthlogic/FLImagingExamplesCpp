
// MenuBarUserDefinedInfoExample.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "MenuBarUserDefinedInfoExample.h"

#include "PropertyMenuBarUserDefinedInfoExample.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMenuBarUserDefinedInfoExampleApp

BEGIN_MESSAGE_MAP(CMenuBarUserDefinedInfoExampleApp, CWinApp)
END_MESSAGE_MAP()


// CMenuBarUserDefinedInfoExampleApp 생성

CMenuBarUserDefinedInfoExampleApp::CMenuBarUserDefinedInfoExampleApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CMenuBarUserDefinedInfoExampleApp 개체입니다.

CMenuBarUserDefinedInfoExampleApp theApp;


// CMenuBarUserDefinedInfoExampleApp 초기화

BOOL CMenuBarUserDefinedInfoExampleApp::InitInstance()
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
		pApp->m_pszAppName = _wcsdup(L"MenuBar UserDefinedInfo Example App");

		if(pApp->m_pMainWnd && ::IsWindow(pApp->m_pMainWnd->GetSafeHwnd()))
			pApp->m_pMainWnd->SetWindowText(L"MenuBar UserDefinedInfo Example App");
	}

	SetRegistryKey(L"Fourth Logic Incorporated\\Examples\\Cpp");

	// GUI Manager 초기화 전처리를 수행합니다.
	// Perform pre-initialization for the GUI Manager.
	CGUIManager::PreInitialize();

	// 예제 프로퍼티 메뉴를 등록합니다.
	// Register an example property menu.
	CGUIManager::RegisterMenu(CPropertyMenuBarUserDefinedInfoExample(), L"MenuBar User Defined Info Example", L"Menu", false);

	// GUI 상에서 사용될 뷰를 생성합니다.
	// Create a view to be used in the GUI.
	CGUIFixedViewDeclaration* pDeclarationCam = new CGUIFixedViewDeclaration;
	pDeclarationCam->SetMultiDocTemplateName("CGUIDocImageCGUIChildFrameImageCGUIViewImage");
	pDeclarationCam->SetViewName(L"Image View");
	CGUIManagerView::AddFixedViewDeclaration(pDeclarationCam);

	// GUI 상에서 뷰의 배열을 설정합니다.
	// Configure the arrangement of views in the GUI.
	CGUIFixedViewPlacement fvp;
	CGUIFixedViewPlacementSet fvpSet1(false);

	fvpSet1.SetName(L"Prompt View Set 1");
	fvp.SetFixedViewDeclaration(pDeclarationCam);
	fvp.SetPlacement(CFLRect<double>(0., 0., 1., 1.));
	fvpSet1.AddFixedViewPlacement(fvp);
	CGUIManagerView::AddFixedViewPlacementSet(fvpSet1);

	// 인덱스에 해당하는 뷰의 설정으로 선택합니다.
	// Select the view configuration corresponding to the index.
	CGUIManagerView::SelectFixedViewPlacementSet(0);

	CGUIManager::PreInitializePaneVisibility(true, true, false, false, false);

	// 모델 관리자를 초기화 합니다.
	// Initialize the model manager.
	CGUIManagerModel::Initialize();

	// GUI 관리자를 초기화 합니다.
	// Initialize the GUI manager.
	CGUIManager::Initialize();

	CGUIManager::LoadThemeSetting();
	CGUIManager::LoadFontSetting();
	CGUIManager::LoadWorkingPane();
	CGUIManager::LoadMenuItemExpandSetting();
	CGUIManagerModel::LoadLastUsedModel();

	// "MenuBar User Defined Info Example" 이라는 이름의 메뉴 아이템 찾기
	CPropertyMenuBarUserDefinedInfoExample* pPropertyMenu = dynamic_cast<CPropertyMenuBarUserDefinedInfoExample*>(CGUIManager::GetMenuItem(L"MenuBar User Defined Info Example", L"Menu"));

	// "MenuBar User Defined Info Example" 이라는 이름의 메뉴 아이템이 있을 경우, 프로퍼티 창 열기
	if(pPropertyMenu)
	{
		CGUIMenuItemAttributePane* pAttrPane = pPropertyMenu->GetAttributePane();

		if(pAttrPane)
		{
			// MainFrame의 Right 위치에 붙이기
			CGUIPanePlacementDockToMainFrame panePlacementDTMF;
			panePlacementDTMF.m_eDockPosition = EDockPosition_Right;
			panePlacementDTMF.m_i32Width = 400;
			pAttrPane->AddDockingPriority(panePlacementDTMF);
		}
		
		pPropertyMenu->OnLButtonDoubleClick();
	}

	return TRUE;
}
