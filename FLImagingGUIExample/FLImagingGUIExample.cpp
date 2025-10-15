
// FLImagingGUIExample.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "FLImagingGUIExample.h"

#include "PropertyMenuExample.h"
#include "FLImagingGUIExampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFLImagingGUIExampleApp

BEGIN_MESSAGE_MAP(CFLImagingGUIExampleApp, CWinApp)
END_MESSAGE_MAP()


// CFLImagingGUIExampleApp 생성

CFLImagingGUIExampleApp::CFLImagingGUIExampleApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CFLImagingGUIExampleApp 개체입니다.

CFLImagingGUIExampleApp theApp;


// CFLImagingGUIExampleApp 초기화

BOOL CFLImagingGUIExampleApp::InitInstance()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CWinAppEx::InitInstance();

	CGUIManager::SetAppTitle(L"FLImagingGUI Example App");
	CGUIManager::SetRegistrySubKeyName(CGUIManager::GetAppTitle());
	SetRegistryKey(L"Fourth Logic Incorporated\\Examples\\Cpp");

	// GUI Manager 초기화 전처리를 수행합니다.
	CGUIManager::PreInitialize();

	// 이미지 뷰 생성 메뉴를 추가합니다.
	CGUIManager::AddMenuItem(new CGUIMenuItemCreateViewImage);

	// 예제 프로퍼티 메뉴를 등록합니다.
	CGUIManager::RegisterMenu(CPropertyMenuExample(), L"Example", L"Menu", false);

	// 생성한 Dialog 메뉴를 등록합니다.
	CGUIMenuItemPaneDialogCustomEmbedded* pItemCustomPane = new CGUIMenuItemPaneDialogCustomEmbedded;
	pItemCustomPane->SetPath(L"Menu");
	pItemCustomPane->SetName(L"Dialog");
	pItemCustomPane->SetIcon(ETreeItemIcon_Setting);
	pItemCustomPane->SetCustomDialog(new CFLImagingGUIExampleDlg);
	CGUIManager::AddMenuItem(pItemCustomPane);



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

			switch(i32Index)
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
	CGUIManagerView::SelectFixedViewPlacementSet(0);

	CGUIManager::PreInitializePaneVisibility(true, false, false, false, false);

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

	// "Example" 이라는 이름의 메뉴 아이템 찾기
	CPropertyMenuExample* pPropertyMenu = dynamic_cast<CPropertyMenuExample*>(CGUIManager::GetMenuItem(L"Example", L"Menu"));

	// "Example" 이라는 이름의 메뉴 아이템이 있을 경우, 프로퍼티 창 열기
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

	CRect rtMainWnd;

	if(AfxGetApp() && AfxGetApp()->m_pMainWnd)
		AfxGetApp()->m_pMainWnd->GetClientRect(rtMainWnd);

	do
	{
		if(!pItemCustomPane)
			break;

		pItemCustomPane->InitializePane();

		CGUIPaneDialogCustomEmbedded* pPane = pItemCustomPane->GetPane();

		if(!pPane)
			break;

		if(pPane->IsDocked())
		{
			if(pPane->IsTabbed())
			{
				CMFCBaseTabCtrl* pParentTab = DYNAMIC_DOWNCAST(CMFCBaseTabCtrl, pPane->GetParent());

				if(pParentTab != NULL)
				{
					for(int i = 0; i < pParentTab->GetTabsNum(); i++)
					{
						CWnd* pTabWnd = pParentTab->GetTabWnd(i);

						if(pTabWnd != pPane)
							continue;

						pParentTab->DetachTab(DM_UNKNOWN, i, TRUE);
						break;
					}
				}
			}
			else
			{
				pPane->FloatPane(pPane->m_recentDockInfo.m_rectRecentFloatingRect, DM_UNKNOWN, false);
				CDockingManager* pDockManager = afxGlobalUtils.GetDockingManager(pPane->GetDockSiteFrameWnd());
				afxGlobalUtils.ForceAdjustLayout(pDockManager);
			}
		}

		pPane->SetControlBarStyle(pPane->GetControlBarStyle() & ~(AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_CLOSE));
		pPane->SetWindowPos(nullptr, 0, 0, rtMainWnd.Width(), 300, SWP_NOMOVE | SWP_NOZORDER);
		pPane->DockToFrameWindow(CBRS_ALIGN_BOTTOM, nullptr, DT_DOCK_LAST, nullptr, -1, true);
		pPane->EnableDocking(CBRS_ALIGN_ANY);
		pItemCustomPane->SetDockDone(true);
	}
	while(false);

	return TRUE;
}
