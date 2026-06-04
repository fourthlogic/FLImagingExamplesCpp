
// MenuBarMenuItem.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "MenuBarMenuItem.h"
#include "TestDialog.h"

#include "PropertyMenuBarMenuItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMenuBarMenuItemApp

BEGIN_MESSAGE_MAP(CMenuBarMenuItemApp, CWinApp)
END_MESSAGE_MAP()


// CMenuBarMenuItemApp 생성

CMenuBarMenuItemApp::CMenuBarMenuItemApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CMenuBarMenuItemApp 개체입니다.

CMenuBarMenuItemApp theApp;


// CMenuBarMenuItemApp 초기화

BOOL CMenuBarMenuItemApp::InitInstance()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CWinAppEx::InitInstance();

	CGUIManager::SetAppTitle(L"MenuBar MenuItem Example App");
	CGUIManager::SetRegistrySubKeyName(CGUIManager::GetAppTitle());
	SetRegistryKey(L"Fourth Logic Incorporated\\Examples\\Cpp");

	// GUI Manager 초기화 전처리를 수행합니다.
	// Perform pre-initialization for the GUI Manager.
	CGUIManager::PreInitialize();

	// 예제 프로퍼티 메뉴를 등록합니다.
	// Register an example property menu.
	CGUIManager::RegisterMenu(CPropertyMenuBarMenuItem(), L"MenuBarMenuItem Example", L"Menu", false);

	// 예제 Custom Dialog Pane 메뉴를 등록합니다.
	// Register an example Custom Dialog Pane menu.
	CGUIMenuItemPaneDialogCustomEmbedded* pMIPdce = new CGUIMenuItemPaneDialogCustomEmbedded;
	pMIPdce->SetPath(L"Menu");
	pMIPdce->SetName(L"IDD_DIALOG_TEST");
	pMIPdce->SetIcon(ETreeItemIcon_Light);
	pMIPdce->SetCustomDialog(new CTestDialog);
	CGUIManager::AddMenuItem(pMIPdce);

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

	// 메뉴바에 메뉴 버튼 추가하기
	{
		// 상단 메뉴바에 "Additional Menu" 메뉴 추가
		CGUIPopupMenuItemUserDefined* pUdmiNew = CGUIManagerMainFrameMenuBar::AddUserDefinedMenuItem(
			L"Additional Menu", // 메뉴 아이템 이름
			L"", // 경로
			nullptr); // 콜백 함수 포인터

		{
			// 클릭하면 메시지박스를 띄우는 콜백 함수 만들기
			CPopupMenuItemUserDefinedCallback* pCallback = new CPopupMenuItemUserDefinedCallback;
			*pCallback = MakePopupMenuItemUserDefinedCallback
			{
				CGUIMessageBox::DoModal(L"Help menu item");
			};

			{
				// "Additional Menu" 버튼을 누르면 나오는 팝업 메뉴에 "Additional Help 1" 메뉴 삽입
				CGUIPopupMenuItemUserDefined* pUdmiNew = CGUIManagerMainFrameMenuBar::AddUserDefinedMenuItem(
					L"Additional Help 1", // 메뉴 아이템 이름
					L"Additional Menu", // 경로
					pCallback); // 콜백 함수 포인터
			}

			{
				// "Additional Menu" 버튼을 누르면 나오는 팝업 메뉴에 "Additional Help 2" 메뉴 삽입
				CGUIPopupMenuItemUserDefined* pUdmiNew = CGUIManagerMainFrameMenuBar::AddUserDefinedMenuItem(
					L"Additional Help 2", // 메뉴 아이템 이름
					L"Additional Menu", // 경로
					pCallback, false); // 콜백 함수 포인터
			}
		}
	}


	// 상단 메뉴 바의 "Window" 하위에 Separator 추가하기
	CGUIManagerMainFrameMenuBar::AddUserDefinedMenuItemSeparator(L"Window");

	// 상단 메뉴 바의 "Window" 하위 메뉴로 메뉴 추가
	{
		// "Algorithm Manager@Imaging@Server" 라는 이름의 메뉴 아이템 찾기
		CGUIMenuItemBase* pMI = CGUIManager::GetMenuItem(L"Algorithm Manager@Imaging@Server");

		// 상단 메뉴 바의 "Window" 하위 메뉴로 메뉴 추가하기
		if(pMI)
		{
			// 클릭하면 "Algorithm Manager@Imaging@Server" 메뉴를 더블클릭한 것과 동일하게 동작하는 콜백 함수 만들기
			CPopupMenuItemUserDefinedCallback* pCallback = new CPopupMenuItemUserDefinedCallback;
			*pCallback = MakePopupMenuItemUserDefinedCallback
			{
				CGUIMenuItemBase * pMI = CGUIManager::GetMenuItem(L"Algorithm Manager@Imaging@Server");

				if(pMI)
					pMI->OnLButtonDoubleClick();
			};

			CGUIManagerMainFrameMenuBar::AddUserDefinedMenuItem(
				pMI->GetName(),  // 메뉴 아이템 이름 // Menu item name
				L"Window",  // 경로 // Path
				pCallback   // 콜백 함수 포인터 // Pointer to the callback function
			);
		}
	}

	CGUIMenuBar* pMB = CGUIManagerMainFrameMenuBar::GetMenuBar();

	// 상단 메뉴 바의 "Window" 하위 메뉴로 메뉴 추가
	{
		// "Algorithm Manager@Imaging@Client" 라는 이름의 메뉴 아이템 찾기
		CGUIMenuItemBase* pMI = CGUIManager::GetMenuItem(L"Algorithm Manager@Imaging@Client");

		// 상단 메뉴 바의 "Window" 하위 메뉴로 메뉴 추가하고, 추가한 메뉴 객체의 포인터 얻어 오기
		if(pMI)
		{
			// 클릭하면 "Algorithm Manager@Imaging@Client" 메뉴를 더블클릭한 것과 동일하게 동작하는 콜백 함수 만들기
			CPopupMenuItemUserDefinedCallback* pCallback = new CPopupMenuItemUserDefinedCallback;
			*pCallback = MakePopupMenuItemUserDefinedCallback
			{
				CGUIMenuItemBase * pMI = CGUIManager::GetMenuItem(L"Algorithm Manager@Imaging@Client");

				if(pMI)
					pMI->OnLButtonDoubleClick();
			};

			CGUIPopupMenuItemUserDefined* pUdmiNew = CGUIManagerMainFrameMenuBar::AddUserDefinedMenuItem(
				pMI->GetName(),  // 메뉴 아이템 이름 // Menu item name
				L"Window",  // 경로 // Path
				pCallback,  // 콜백 함수 포인터 // Pointer to the callback function
				true, // 콜백 함수 객체 자동 삭제 여부(default 값은 true) // Whether to automatically delete the callback function object (default: true)
				true, // Enable 상태로 메뉴 아이템 표시할지 여부 // Whether to display the menu item in enabled state
				false // Checked 상태로 메뉴 아이템 표시할지 여부 // Whether to display the menu item in checked state
			);

			// 성공적으로 메뉴가 추가되었다면
			if(pUdmiNew)
			{
				// 해당 메뉴의 체크 여부를 결정하는 콜백 함수 만들기
				// "Algorithm Manager@Imaging@Client" 다이얼로그가 열려 있을 때는 체크,
				// 닫혀져 있는 경우엔 체크 해제하는 함수
				CPopupMenuItemUserDefinedCheckCallback* pCheckCallback = new CPopupMenuItemUserDefinedCheckCallback;
				*pCheckCallback = MakePopupMenuItemUserDefinedCheckCallback
				{
					bool bReturn = false;

					CGUIMenuItemPaneDialogCustomEmbedded* pMI = (CGUIMenuItemPaneDialogCustomEmbedded*)CGUIManager::GetMenuItem(L"Algorithm Manager@Imaging@Client");

					// "Algorithm Manager@Imaging@Client" 다이얼로그가 열려 있는지 여부를 확인
					if(pMI && pMI->GetPane() && CGUIManagerPane::IsPaneActive(pMI->GetPane()))
						bReturn = true;

					return bReturn;
				};

				// 체크 여부를 결정하는 콜백 함수를 설정
				pUdmiNew->SetCheckCallback(pCheckCallback);
			}
		}
	}

	// 기존 메뉴 사이에 메뉴 삽입하기
	{
		// 클릭하면 메시지박스를 띄우는 콜백 함수 만들기
		CPopupMenuItemUserDefinedCallback* pCallback = new CPopupMenuItemUserDefinedCallback;
		*pCallback = MakePopupMenuItemUserDefinedCallback
		{
			CGUIMessageBox::DoModal(L"Help1 menu item");
		};

		// 상단 메뉴 바의 "Information" 버튼을 누르면 나오는 팝업 메뉴의 인덱스 1번째에 "Help1" 메뉴 삽입
		CGUIManagerMainFrameMenuBar::AddUserDefinedMenuItem(
			L"Help1",  // 메뉴 아이템 이름 // Menu item name
			L"Information", // 경로 // Path
			pCallback, // 콜백 함수 포인터 // Pointer to the callback function
			true,      // 콜백 함수 객체 자동 삭제 여부(default 값은 true) // Whether to automatically delete the callback function object (default: true)
			true,      // Enable 상태로 메뉴 아이템 표시할지 여부 // Whether to display the menu item in enabled state
			false,     // Checked 상태로 메뉴 아이템 표시할지 여부 // Whether to display the menu item in checked state
			1);        // 삽입 위치 인덱스 // Insertion index
	}


	// 메뉴 삽입하기
	{
		// 클릭하면 메시지박스를 띄우는 콜백 함수 만들기
		CPopupMenuItemUserDefinedCallback* pCallback = new CPopupMenuItemUserDefinedCallback;
		*pCallback = MakePopupMenuItemUserDefinedCallback
		{
			CGUIMessageBox::DoModal(L"Help2 menu item");
		};

		// "Information" 버튼을 누르면 나오는 팝업 메뉴의 인덱스 2번째에 "Help2" 메뉴 삽입
		CGUIManagerMainFrameMenuBar::AddUserDefinedMenuItem(
			L"Help2",   // 메뉴 아이템 이름 // Menu item name
			L"Information",  // 경로 // Path
			pCallback,  // 콜백 함수 포인터 // Pointer to the callback function
			true, 	    // 콜백 함수 객체 자동 삭제 여부(default 값은 true) // Whether to automatically delete the callback function object (default: true)
			true, 	    // Enable 상태로 메뉴 아이템 표시할지 여부 // Whether to display the menu item in enabled state
			false, 	    // Checked 상태로 메뉴 아이템 표시할지 여부 // Whether to display the menu item in checked state
			2);		    // 삽입 위치 인덱스 // Insertion index
	}


	// 메뉴 삽입하기
	{
		// 클릭하면 메시지박스를 띄우는 콜백 함수 만들기
		CPopupMenuItemUserDefinedCallback* pCallback = new CPopupMenuItemUserDefinedCallback;
		*pCallback = MakePopupMenuItemUserDefinedCallback
		{
			CGUIMessageBox::DoModal(L"Help4 menu item");
		};

		// "Information" 버튼을 누르면 나오는 팝업 메뉴의 인덱스 4번째에 "Help4" 메뉴 삽입
		CGUIPopupMenuItemUserDefined* pUdmiNew = CGUIManagerMainFrameMenuBar::AddUserDefinedMenuItem(
			L"Help4", // 메뉴 아이템 이름 // Menu item name
			L"Information", // 경로 // Path
			pCallback); // 콜백 함수 포인터 // Pointer to the callback function

		if(pUdmiNew)
			pUdmiNew->m_i32InsertPos = 4; // 삽입 위치 인덱스 설정 // Set insertion index
	}


	// GUI 관리자를 초기화 합니다.
	// Initialize the GUI manager.
	CGUIManager::Initialize();

	// 메뉴 삭제하기 (CGUIManager::Initialize(); 호출 이후)
	// Delete menu (after calling CGUIManager::Initialize();)
	{
		CGUIManagerMainFrameMenuBar::RemoveMenuItem(L"File");
		CGUIManagerMainFrameMenuBar::RemoveMenuItem(L"Settings@Theme");
	}

	CGUIManager::LoadThemeSetting();
	CGUIManager::LoadFontSetting();
	CGUIManager::LoadWorkingPane();
	CGUIManager::LoadMenuItemExpandSetting();
	CGUIManagerModel::LoadLastUsedModel();

	return TRUE;
}
