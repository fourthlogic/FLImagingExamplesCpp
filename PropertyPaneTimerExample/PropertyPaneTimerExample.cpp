﻿
// PropertyPaneTimerExample.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "PropertyPaneTimerExample.h"

#include "PropertyTimerExample.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPropertyPaneTimerExampleApp

BEGIN_MESSAGE_MAP(CPropertyPaneTimerExampleApp, CWinApp)
END_MESSAGE_MAP()


// CPropertyPaneTimerExampleApp 생성

CPropertyPaneTimerExampleApp::CPropertyPaneTimerExampleApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CPropertyPaneTimerExampleApp 개체입니다.

CPropertyPaneTimerExampleApp theApp;


// CPropertyPaneTimerExampleApp 초기화

BOOL CPropertyPaneTimerExampleApp::InitInstance()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CWinAppEx::InitInstance();
	SetRegistryKey(L"Fourth Logic Incorporated");

	// GUI Manager 초기화 전처리를 수행합니다.
	CGUIManager::PreInitialize();

	// 예제 프로퍼티 메뉴를 등록합니다.
	CGUIManager::RegisterMenu(CPropertyTimerExample(), L"Property Pane Timer Example", L"Menu", false);


	// GUI 상에서 사용될 뷰를 생성합니다.
	std::vector<CGUIFixedViewDeclaration*> vctFixedViewDecl;

	for(int32_t i = 0; i < 4; ++i)
	{
		CGUIFixedViewDeclaration* pDeclarationCam = new CGUIFixedViewDeclaration;
		pDeclarationCam->SetMultiDocTemplateName("CGUIDocImageCGUIChildFrameImageCGUIViewImage");
		pDeclarationCam->SetViewName(L"Image View");
		CGUIManagerView::AddFixedViewDeclaration(pDeclarationCam);
		vctFixedViewDecl.push_back(pDeclarationCam);
	}

	// GUI 상에서 뷰의 배열을 설정합니다.
	CGUIFixedViewPlacement fvp;

	CGUIFixedViewPlacementSet fvpSet1(false);
	CGUIFixedViewPlacementSet fvpSet2(false);
	CGUIFixedViewPlacementSet fvpSet3(false);
	CGUIFixedViewPlacementSet fvpSet4(false);

	fvpSet1.SetName(L"Prompt Image View Set 1");
	fvpSet2.SetName(L"Prompt Image View Set 2");
	fvpSet3.SetName(L"Prompt Image View Set 3");
	fvpSet4.SetName(L"Prompt Image View Set 4");

	for(int32_t x = 0; x < 2; ++x)
	{
		for(int32_t y = 0; y < 2; ++y)
		{
			int32_t i32Index = (y * 2) + x;

			CGUIFixedViewDeclaration* pDeclarationCam = vctFixedViewDecl[i32Index];
			fvp.SetFixedViewDeclaration(pDeclarationCam);

			switch (i32Index)
			{
			case 0:
				{
					fvp.SetPlacement(CFLRect<double>(1.0 * x, 1.0 * y, 1.0 * (x + 1), 1.0 * (y + 1)));
					fvpSet1.AddFixedViewPlacement(fvp);

					fvp.SetPlacement(CFLRect<double>(0.5 * x, 1.0 * y, 0.5 * (x + 1), 1.0 * (y + 1)));
					fvpSet2.AddFixedViewPlacement(fvp);

					fvp.SetPlacement(CFLRect<double>(0.5 * x, 0.5 * y, 0.5 * (x + 1), 0.5 * (y + 1)));
					fvpSet3.AddFixedViewPlacement(fvp);
					fvpSet4.AddFixedViewPlacement(fvp);
				}
				break;

			case 1:
				{
					fvp.SetPlacement(CFLRect<double>(0.5 * x, 1.0 * y, 0.5 * (x + 1), 1.0 * (y + 1)));
					fvpSet2.AddFixedViewPlacement(fvp);

					fvp.SetPlacement(CFLRect<double>(0.5 * x, 0.5 * y, 0.5 * (x + 1), 0.5 * (y + 1)));
					fvpSet3.AddFixedViewPlacement(fvp);
					fvpSet4.AddFixedViewPlacement(fvp);
				}
				break;

			case 2:
				{
					fvp.SetPlacement(CFLRect<double>(1.0 * x, 0.5 * y, 1.0 * (x + 1), 0.5 * (y + 1)));
					fvpSet3.AddFixedViewPlacement(fvp);

					fvp.SetPlacement(CFLRect<double>(0.5 * x, 0.5 * y, 0.5 * (x + 1), 0.5 * (y + 1)));
					fvpSet4.AddFixedViewPlacement(fvp);
				}
				break;

			case 3:
				{
					fvp.SetPlacement(CFLRect<double>(0.5 * x, 0.5 * y, 0.5 * (x + 1), 0.5 * (y + 1)));
					fvpSet4.AddFixedViewPlacement(fvp);
				}
				break;
			}
		}
	}

	CGUIManagerView::AddFixedViewPlacementSet(fvpSet1);
	CGUIManagerView::AddFixedViewPlacementSet(fvpSet2);
	CGUIManagerView::AddFixedViewPlacementSet(fvpSet3);
	CGUIManagerView::AddFixedViewPlacementSet(fvpSet4);

	// 인덱스에 해당하는 뷰의 설정으로 선택합니다.
	CGUIManagerView::SelectFixedViewPlacementSet(3);

	CGUIManager::PreInitializePaneVisibility(true, true, false, false, false);

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
		pMF->SetWindowPos(NULL, 0, 0, 1280, 1024, NULL);
	}

	// "MenuBar User Defined Info Example" 이라는 이름의 메뉴 아이템 찾기
	CPropertyTimerExample* pPropertyMenu = dynamic_cast<CPropertyTimerExample*>(CGUIManager::GetMenuItem(L"Property Pane Timer Example", L"Menu"));

	// "MenuBar User Defined Info Example" 이라는 이름의 메뉴 아이템이 있을 경우, 프로퍼티 창 열기
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
