#include "stdafx.h"
#include "PropertyMenuDialogProgressExample.h"

using namespace FLImaging;
using namespace FLImaging::Base;
using namespace FLImaging::Foundation;
using namespace FLImaging::GUI;

namespace FLImaging
{
	namespace GUI
	{
		// 복잡한 다이얼로그의 레이아웃의 row 인덱스
		// Row indices of the layout of a complex dialog
		enum class ERowLayout : int32_t
		{
			Main = 0,           // Main 레이아웃 / Main layout
			Button = 4,         // Button 레이아웃 / Button layout
			Details = 0,        // Details 레이아웃 / Details layout

			Count = 5,          // 행 개수 / Row count
		};

		// 복잡한 다이얼로그의 레이아웃의 col 인덱스
		// Column indices of the layout of a complex dialog
		enum class EColLayout : int32_t
		{
			Main = 0,           // Main 레이아웃 / Main layout
			Button = 0,         // Button 레이아웃 / Button layout
			Details = 1,        // Details 레이아웃 / Details layout

			Count = 2,          // 컬럼 개수 / Column count
		};

		// 복잡한 다이얼로그의 메인 그리드 내부 컨트롤의 row 인덱스
		// Row indices of the inner controls of the main grid of a complex dialog
		enum class ERowMainComplex : int32_t
		{
			Message = 0,        // 메세지 / Message
			ProgressCtrl = 3,   // 프로그레스 바 / Progress bar
			ElapsedTime = 4,    // 시간 / Time

			Count = 5,          // 행 개수 / Row count
		};

		// 복잡한 다이얼로그의 메인 그리드 내부 컨트롤의 col 인덱스
		// Column indices of the inner controls of the main grid of a complex dialog
		enum class EColMainComplex : int32_t
		{
			Message = 1,        // 메세지 / Message
			ProgressCtrl = 1,   // 프로그레스 바 / Progress bar
			ElapsedTime = 1,    // 시간 / Time

			Count = 6,          // 컬럼 개수 / Column count
		};

		// 단순한 다이얼로그의 메인 그리드 내부 컨트롤의 row 인덱스
		// Row indices of the inner controls of the main grid of a simple dialog
		enum class ERowMainSimple : int32_t
		{
			Message = 0,       // 메세지 / Message
			ProgressCtrl = 2,  // 프로그레스 바 / Progress bar
			ElapsedTime = 3,   // 시간 / Time

			Count = 4,         // 행 개수 / Row count
		};

		// 단순한 다이얼로그의 메인 그리드 내부 컨트롤의 col 인덱스
		// Column indices of the inner controls of the main grid of a simple dialog
		enum class EColMainSimple : int32_t
		{
			Message = 0,       // 메세지 / Message
			ProgressCtrl = 0,  // 프로그레스 바 / Progress bar
			ElapsedTime = 0,   // 시간 / Time

			Count = 1,         // 컬럼 개수 / Column count
		};

		// 복잡한 다이얼로그의 버튼 그리드의 row 인덱스
		// Row indices of the button grid of a complex dialog
		enum class ERowButtonComplex : int32_t
		{
			Details = 0,       // 세부 사항 버튼 / Details button
			Stop = 1,          // 중지 버튼 / Stop button

			Count = 2,         // 행 개수 / Row count
		};

		// 단순한 다이얼로그의 버튼 그리드의 row 인덱스
		// Row indices of the button grid of a simple dialog
		enum class ERowButtonSimple : int32_t
		{
			Stop = 0,          // 중지 버튼 / Stop button

			Count = 1,         // 행 개수 / Row count
		};

		// 다이얼로그의 버튼 그리드의 col 인덱스
		// Column indices of the button grid of a dialog
		enum class EColButton : int32_t
		{
			Button = 2,        // 버튼 / Button

			Count = 3,         // 컬럼 개수 / Column count
		};
	}
}

FLImaging::GUI::sProgressThreadContext::sProgressThreadContext()
	: pDlgProgress(nullptr)
	, pThread(nullptr)
	, bThreadDone(false)
	, bThreadStopRequested(false)
	, i32CurrentProgress(0)
	, i32TotalProgress(1000)
	, i32CurrentStopProgress(0)
	, i32TotalStopProgress(20)
	, i32BtnStopClickCount(0)
	, pProgressCtrlFP(nullptr)
	, pStopButton(nullptr)
	, pGridMain(nullptr)
	, pGridButtons(nullptr)
	, pGridDetails(nullptr)
	, pLayoutForSheet(nullptr)
{
}

FLImaging::GUI::sProgressThreadContext::~sProgressThreadContext()
{
}

void FLImaging::GUI::sProgressThreadContext::TerminateProgressThread()
{
	// 워커 스레드에 종료 요청
	// Request termination to the worker thread
	bThreadStopRequested = true;

	if(pThread && pThread->valid())
	{
		// 스레드가 종료(ready)될 때까지 루프
		// Loop until the thread is terminated (ready)
		while(!bThreadDone && pThread->wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
		{
			// 메시지 펌핑을 하여 UI가 굳는 것을 방지
			// Prevent UI freezing by pumping messages
			CGUIManager::PeekAndPump();

			// 과도한 CPU 점유 방지
			// Prevent excessive CPU usage
			::Sleep(10);
		}
	}

	if(pThread)
	{
		delete pThread;
		pThread = nullptr;
	}
}

void FLImaging::GUI::sProgressThreadContext::ResetThreadParams()
{
	pDlgProgress = nullptr;
	pThread = nullptr;
	bThreadDone = false;
	bThreadStopRequested = false;
	i32CurrentProgress = 0;
	i32CurrentStopProgress = 0;
	i32BtnStopClickCount = 0;
	pProgressCtrlFP = nullptr;
	pStopButton = nullptr;
	pGridMain = nullptr;
	pGridButtons = nullptr;
	pGridDetails = nullptr;
	pLayoutForSheet = nullptr;
}

const CResult FLImaging::GUI::sProgressThreadContext::CloseProgressDialog()
{
	CResult cr;

	do
	{
		if(pDlgProgress && ::IsWindow(pDlgProgress->GetSafeHwnd()))
			pDlgProgress->PostMessage(WM_COMMAND, IDOK);
// 
// 		if(pDlgProgress)
// 		{
// 			delete pDlgProgress;
// 			pDlgProgress = nullptr;
// 		}

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

FLImaging::GUI::CPropertyMenuDialogProgressExample::CPropertyMenuDialogProgressExample()
	: CGUIPropertyMenuBase()
	, m_pSContext1(nullptr)
	, m_pSContext2(nullptr)
	, m_pSContext3(nullptr)
	, m_pSContext4(nullptr)
	, m_pSContext5(nullptr)
	, m_pSContext6(nullptr)
{
	m_vctVariousLengthMessages.push_back(L"consectetur adipiscing elit, sed do eiusmod tempor incididunt \nut labore et dolore magna aliqua. \nUt enim ad minim veniam, \nquis nostrud exercitation ullamco laboris nisi \nut aliquip ex ea commodo consequat. ");
	m_vctVariousLengthMessages.push_back(L"Duis");
	m_vctVariousLengthMessages.push_back(L"Lorem ipsum dolor sit amet,");
	m_vctVariousLengthMessages.push_back(L"aute irure dolor in reprehenderit in ");
	m_vctVariousLengthMessages.push_back(L"voluptate velit esse");
	m_vctVariousLengthMessages.push_back(L"cillum dolore eu fugiat nulla pariatur. Excepteur ");
	m_vctVariousLengthMessages.push_back(L"sint occaecat \ncupidatat non proident, \nsunt in culpa qui  \nofficia deserunt \n\n\n\n\n\nmollit anim id est laborum.\n");
}

FLImaging::GUI::CPropertyMenuDialogProgressExample::~CPropertyMenuDialogProgressExample()
{
	// 모든 스레드 컨텍스트 안전하게 해제
	// Release all thread contexts safely
	SProgressThreadContext* arrContexts[] = { m_pSContext1, m_pSContext2, m_pSContext3, m_pSContext4, m_pSContext5, m_pSContext6 };
	for(int i = 0; i < 6; ++i)
	{
		if(arrContexts[i])
		{
			arrContexts[i]->TerminateProgressThread();
			delete arrContexts[i];
		}
	}
	m_pSContext1 = m_pSContext2 = m_pSContext3 = m_pSContext4 = m_pSContext5 = m_pSContext6 = nullptr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::ConfigureMenu()
{
	CResult cReturn = EResult_UnknownError;

	do
	{
		CGUIPropertyButton* pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"간단한 다이얼로그");
		pBtn->SetPropertyButtonClickProcedure(ButtonCreateSimpleDialog());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);
		SetShortcut(EKeyType_F6, pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"멤버 변수 없이 생성하는 간단한 다이얼로그");
		pBtn->SetPropertyButtonClickProcedure(ButtonCreateSimpleDialogWithoutMembers());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"디테일 창 + 제목이 있는 다이얼로그");
		pBtn->SetPropertyButtonClickProcedure(ButtonCreateComplexDialog());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);
		SetShortcut(EKeyType_F7, pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Modal 디테일 창 + 제목");
		pBtn->SetPropertyButtonClickProcedure(ButtonCreateComplexDialogModal());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Pivot, Alignment 등 파라미터 적용 다이얼로그");
		pBtn->SetPropertyButtonClickProcedure(ButtonCreateSimpleDialogWithSettings());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"CFLSheet 창이 있는 다이얼로그 1");
		pBtn->SetPropertyButtonClickProcedure(ButtonCreateComplexDialogWithSheet1());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"CFLSheet 창이 있는 다이얼로그 2");
		pBtn->SetPropertyButtonClickProcedure(ButtonCreateComplexDialogWithSheet2());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		CGUIPropertyItemDropdownList* pDrop = new CGUIPropertyItemDropdownList;
		pDrop->SetName(L"Text Alignment");
		pDrop->AddListItem(L"LEFT_TOP");
		pDrop->AddListItem(L"CENTER_TOP");
		pDrop->AddListItem(L"RIGHT_TOP");
		pDrop->AddListItem(L"LEFT_CENTER");
		pDrop->AddListItem(L"CENTER_CENTER");
		pDrop->AddListItem(L"RIGHT_CENTER");
		pDrop->AddListItem(L"LEFT_BOTTOM");
		pDrop->AddListItem(L"CENTER_BOTTOM");
		pDrop->AddListItem(L"RIGHT_BOTTOM");
		pDrop->SetDefaultValue(L"CENTER_CENTER");
		pDrop->SetDescription(L"다이얼로그 창 내부 텍스트 정렬 옵션을 설정합니다.");
		AddItem(pDrop);

		pDrop = new CGUIPropertyItemDropdownList;
		pDrop->SetName(L"Resize Pivot");
		pDrop->AddListItem(L"LEFT_TOP");
		pDrop->AddListItem(L"CENTER_TOP");
		pDrop->AddListItem(L"RIGHT_TOP");
		pDrop->AddListItem(L"LEFT_CENTER");
		pDrop->AddListItem(L"CENTER_CENTER");
		pDrop->AddListItem(L"RIGHT_CENTER");
		pDrop->AddListItem(L"LEFT_BOTTOM");
		pDrop->AddListItem(L"CENTER_BOTTOM");
		pDrop->AddListItem(L"RIGHT_BOTTOM");
		pDrop->SetDefaultValue(L"LEFT_TOP");
		pDrop->SetDescription(L"다이얼로그 창의 크기가 변경될 때 어느 위치를 기준으로 창을 확대 또는 축소할지 기준 위치를 정합니다.");
		AddItem(pDrop);

		CGUIPropertyItemCheckBox* pCheck = new CGUIPropertyItemCheckBox;
		pCheck->SetName(L"Keep Maximum Width");
		pCheck->SetDescription(L"다이얼로그 창의 크기가 변경될 때 변경되기 전 창의 너비가 더 크면, 창의 너비를 변경하지 않고 이전 너비를 유지할지 여부 설정");
		AddItem(pCheck);

		pCheck = new CGUIPropertyItemCheckBox;
		pCheck->SetName(L"Keep Maximum Height");
		pCheck->SetDescription(L"다이얼로그 창의 크기가 변경될 때 변경되기 전 창의 높이가 더 크면, 창의 높이를 변경하지 않고 이전 높이를 유지할지 여부 설정");
		AddItem(pCheck);

		cReturn = EResult_OK;
	}
	while(false);

	return cReturn;
}

void FLImaging::GUI::CPropertyMenuDialogProgressExample::GetDialogSettings(EGUIAlignment& eTextAlignment, EGUIAlignment& eResizePivot, bool& bKeepMaxWidth, bool& bKeepMaxHeight) const
{
	CGUIPropertyItemDropdownList* pDropTextAlignment = dynamic_cast<CGUIPropertyItemDropdownList*>(FindItemByName(L"Text Alignment"));
	CGUIPropertyItemDropdownList* pDropResizePivot = dynamic_cast<CGUIPropertyItemDropdownList*>(FindItemByName(L"Resize Pivot"));
	CGUIPropertyItemCheckBox* pCheckKeepMaxWidth = dynamic_cast<CGUIPropertyItemCheckBox*>(FindItemByName(L"Keep Maximum Width"));
	CGUIPropertyItemCheckBox* pCheckKeepMaxHeight = dynamic_cast<CGUIPropertyItemCheckBox*>(FindItemByName(L"Keep Maximum Height"));

	if(pDropTextAlignment) eTextAlignment = GetAlignment(pDropTextAlignment->GetValue());
	if(pDropResizePivot) eResizePivot = GetAlignment(pDropResizePivot->GetValue());
	if(pCheckKeepMaxWidth) bKeepMaxWidth = pCheckKeepMaxWidth->GetValue() == L"Checked";
	if(pCheckKeepMaxHeight) bKeepMaxHeight = pCheckKeepMaxHeight->GetValue() == L"Checked";
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuDialogProgressExample::ButtonCreateSimpleDialog()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		do
		{
			// 워커 스레드에 종료 요청
			// Request termination to the worker thread
			if(m_pSContext1)
				m_pSContext1->TerminateProgressThread();

			if(!m_pSContext1)
				m_pSContext1 = new SProgressThreadContext;

			if(!m_pSContext1)
				break;

			m_pSContext1->ResetThreadParams();

			CreateSimpleDialog();

			// 다이얼로그 창 생성
			// Create dialog window
			m_pSContext1->pDlgProgress->OnInitDialog();

			// 작업 스레드 생성 (인자 정리)
			// Create worker thread (arguments cleaned up)
			m_pSContext1->pThread = new std::future<void>(std::async(std::launch::async, CPropertyMenuDialogProgressExample::AlgorithmThreadForSimpleDialog, this, m_pSContext1, false));
			SetThreadPriority(m_pSContext1->pThread, THREAD_PRIORITY_LOWEST);
		}
		while(false);
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuDialogProgressExample::ButtonCreateSimpleDialogWithoutMembers()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		// 다이얼로그 창 생성
		// Create dialog window
		CGUIDialogProgress * pDlgProgress = CGUIDialogProgress::CreateModelessDialog(AfxGetApp()->m_pMainWnd);

		CFLString<wchar_t> strMessage;

		// 메시지, 프로그레스 바, 시간 등이 표시되는 메인 그리드 레이아웃 생성
		// Create main grid layout displaying messages, progress bar, time, etc.
		CGUIGridLayout* pGridMain = new CGUIGridLayout(3, 1);
		{
			// 메시지를 그리드 레이아웃에 추가
			// Add message to grid layout
			pGridMain->AddCtrl(0, 0, L"");
		}

		// 다이얼로그에 메인 그리드 레이아웃 추가
		// Add main grid layout to dialog
		pDlgProgress->Add(pGridMain);

		pDlgProgress->OnInitDialog();

		// 작업 스레드 생성
		// Create worker thread
		bool bThreadDone = false;
		int32_t i32CurrentLoadingAlgorithm = 0;
		int32_t i32TotalAlgorithm = 10;

		std::future<void>* pThread = new std::future<void>(std::async(std::launch::async, [&]()
																	  {
																		  for(int i = 0; i < i32TotalAlgorithm; ++i)
																		  {
																			  // load algorithms..
																			  i32CurrentLoadingAlgorithm = i;
																			  Sleep(250);
																		  }

																		  bThreadDone = true;
																	  }));
		SetThreadPriority(pThread, THREAD_PRIORITY_LOWEST);
		while(true)
		{
			// 스레드가 완료되면 break
			// Break if thread is done
			if(bThreadDone)
				break;

			// 가변 길이의 문자열을 설정
			// Set variable length string
			CFLString<wchar_t> strMessage;
			strMessage.Format
			(
				L"Algorithm Recipe Loading...\n(%d/%d)",
				i32CurrentLoadingAlgorithm, i32TotalAlgorithm
			);

			// 다이얼로그 상에서 보이는 메세지 설정
			// Set message displayed on dialog
			pGridMain->SetCtrlValue(0, 0, strMessage);

			// 다이얼로그 창 업데이트
			// Update dialog window
			pDlgProgress->AdjustLayout();

			// 메시지 펌프
			// Message pump
			CGUIManager::PeekAndPump();
			// Sleep 시간을 1로 설정
			// Set sleep time to 1
			Sleep(1);
		}

		// 다이얼로그 닫기
		// Close dialog
		if(pDlgProgress)
		{
			pDlgProgress->DestroyWindow();
			pDlgProgress = nullptr;
		}

		if(pThread)
		{
			delete pThread;
			pThread = nullptr;
		}
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuDialogProgressExample::ButtonCreateComplexDialog()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateComplexProgressDialog();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuDialogProgressExample::ButtonCreateComplexDialogModal()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateComplexProgressDialogModal();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuDialogProgressExample::ButtonCreateComplexDialogWithSheet1()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		// 워커 스레드에 종료 요청 및 초기화
		if(m_pSContext5) m_pSContext5->TerminateProgressThread();
		if(!m_pSContext5) m_pSContext5 = new SProgressThreadContext;
		m_pSContext5->ResetThreadParams();

		CreateComplexProgressDialogWithSheet(true, m_pSContext5);
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuDialogProgressExample::ButtonCreateComplexDialogWithSheet2()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		// 워커 스레드에 종료 요청 및 초기화
		if(m_pSContext6) m_pSContext6->TerminateProgressThread();
		if(!m_pSContext6) m_pSContext6 = new SProgressThreadContext;
		m_pSContext6->ResetThreadParams();

		CreateComplexProgressDialogWithSheet(false, m_pSContext6);
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuDialogProgressExample::ButtonCreateSimpleDialogWithSettings()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateSimpleDialogWithSettings();
	};

	return pProcedure;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::CreateSimpleDialog()
{
	CResult cr;

	do
	{
		// 다이얼로그 생성
		// Create dialog
		if(!m_pSContext1->pDlgProgress)
			m_pSContext1->pDlgProgress = CGUIDialogProgress::CreateModelessDialog(AfxGetApp()->m_pMainWnd);

		CFLString<wchar_t> strMessage;

		// 메시지, 프로그레스 바, 시간 등이 표시되는 메인 그리드 레이아웃 생성
		// Create main grid layout displaying messages, progress bar, time, etc.
		m_pSContext1->pGridMain = new CGUIGridLayout((int32_t)ERowMainSimple::Count, (int32_t)EColMainSimple::Count);
		{
			strMessage.Format
			(
				L"CGUIDialogProgress example thread is working...\n(%d/%d)",
				m_pSContext1->i32CurrentProgress, m_pSContext1->i32TotalProgress
			);

			// 메시지를 그리드 레이아웃에 추가
			// Add message to grid layout
			m_pSContext1->pGridMain->AddCtrl((int32_t)ERowMainSimple::Message, (int32_t)EColMainSimple::Message, 2, 1, strMessage);
			// 프로그레스 바를 그리드 레이아웃에 추가
			// Add progress bar to grid layout
			m_pSContext1->pGridMain->AddCtrl((int32_t)ERowMainSimple::ProgressCtrl, (int32_t)EColMainSimple::ProgressCtrl, EControl_ProgressBarFloatingPoint);
			// 소요 시간을 그리드 레이아웃에 추가
			// Add elapsed time to grid layout
			m_pSContext1->pGridMain->AddCtrl((int32_t)ERowMainSimple::ElapsedTime, (int32_t)EColMainSimple::ElapsedTime, L"Elapsed Time      00 : 00 : 00");
			// 소요 시간의 텍스트 정렬 옵션을 수평, 수직 모두 중간 정렬로 설정
			// Set elapsed time text alignment to center-center
			m_pSContext1->pGridMain->SetCtrlAlignment((int32_t)ERowMainSimple::ElapsedTime, (int32_t)EColMainSimple::ElapsedTime, EGUIAlignment_CENTER_CENTER);
			// 소요 시간 위치 설정
			// Set elapsed time position
			m_pSContext1->pDlgProgress->SetElapsedTimePosition(m_pSContext1->pGridMain, (int32_t)ERowMainSimple::ElapsedTime, (int32_t)EColMainSimple::ElapsedTime);
			// 진행률 업데이트를 위해 프로그레스 바 객체 포인터를 얻어 오기
			// Get progress bar object pointer for progress update
			m_pSContext1->pProgressCtrlFP = (CGUIProgressCtrlFloatingPoint*)m_pSContext1->pGridMain->GetCtrl((int32_t)ERowMainSimple::ProgressCtrl, (int32_t)EColMainSimple::ProgressCtrl);
		}

		// 버튼이 표시되는 버튼 그리드 레이아웃 생성
		// Create button grid layout displaying buttons
		m_pSContext1->pGridButtons = new CGUIGridLayout((int32_t)ERowButtonSimple::Count, (int32_t)EColButton::Count);
		{
			// 중지 버튼을 그리드 레이아웃에 추가
			// Add stop button to grid layout
			m_pSContext1->pGridButtons->AddCtrl((int32_t)ERowButtonSimple::Stop, (int32_t)EColButton::Button, EControl_Button, L"Stop");
			// 중지 버튼 객체 포인터를 얻어 오기
			// Get stop button object pointer
			m_pSContext1->pStopButton = (CGUIButton*)m_pSContext1->pGridButtons->GetCtrl((int32_t)ERowButtonSimple::Stop, (int32_t)EColButton::Button);
		}

		// 다이얼로그에 메인 그리드 레이아웃 추가
		// Add main grid layout to dialog
		m_pSContext1->pDlgProgress->Add(m_pSContext1->pGridMain);
		// 다이얼로그에 버튼 그리드 레이아웃 추가
		// Add button grid layout to dialog
		m_pSContext1->pDlgProgress->Add(m_pSContext1->pGridButtons);

		m_pSContext1->i32TotalProgress = 100;

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::CreateComplexProgressDialog()
{
	CResult cr;

	do
	{
		// 워커 스레드에 종료 요청
		// Request termination to the worker thread
		if(m_pSContext2)
			m_pSContext2->TerminateProgressThread();

		if(!m_pSContext2)
			m_pSContext2 = new SProgressThreadContext;

		if(!m_pSContext2)
			break;

		m_pSContext2->ResetThreadParams();

		EGUIAlignment eTextAlignment = EGUIAlignment_CENTER_CENTER;
		EGUIAlignment eResizePivot = EGUIAlignment_LEFT_TOP;
		bool bKeepMaxWidth = false, bKeepMaxHeight = false;
		GetDialogSettings(eTextAlignment, eResizePivot, bKeepMaxWidth, bKeepMaxHeight);

		// 다이얼로그 생성
		// Create dialog
		if(!m_pSContext2->pDlgProgress)
		{
			m_pSContext2->pDlgProgress = CGUIDialogProgress::CreateModelessDialog(AfxGetApp()->m_pMainWnd);
			m_pSContext2->pDlgProgress->SetTitle(L"MyTitle");
			m_pSContext2->pDlgProgress->ShowCaption(true);
			m_pSContext2->pDlgProgress->ShowMinimizeButtonOnCaption(true);
			m_pSContext2->pDlgProgress->SetMinimizeWidth(300);
			CWndCtrlProcedure* pCb = new CWndCtrlProcedure;
			*pCb = MakeWndCtrlProcedure
			{
				return true; // Do nothing
			};
			m_pSContext2->pDlgProgress->SetCloseButtonOnCaptionCallback(pCb);
		}

		CFLString<wchar_t> strMessage;
		CFLString<wchar_t> strMessageFormat =
			CFLString<wchar_t>(L"[Step 3/3] Learning...\n\n") +
			CFLString<wchar_t>(L"<Cost>\n") +
			CFLString<wchar_t>(L"0.012132\n\n") +
			CFLString<wchar_t>(L"<Iteration>\n") +
			CFLString<wchar_t>(L"(%d/%d)\n\n") +
			CFLString<wchar_t>(L"<Epoch>\n") +
			CFLString<wchar_t>(L"1/100\n\n") +
			CFLString<wchar_t>(L"<Validation>\n") +
			CFLString<wchar_t>(L"96.12%%at 1 epoch\n\n\n");

		// 메시지, 프로그레스 바, 시간 등이 표시되는 메인 그리드 레이아웃 생성
		// Create main grid layout displaying messages, progress bar, time, etc.
		m_pSContext2->pGridMain = new CGUIGridLayout((int32_t)ERowMainComplex::Count, (int32_t)EColMainComplex::Count, true, L"m_pSContext2->pGridMain");
		{
			strMessage.Format
			(
				strMessageFormat,
				m_pSContext2->i32CurrentProgress, m_pSContext2->i32TotalProgress
			);

			// 메시지를 그리드 레이아웃에 추가
			// Add message to grid layout
			m_pSContext2->pGridMain->AddCtrl((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, 3, 4, strMessage);
			// 메시지의 텍스트 정렬 옵션을 설정
			// Set text alignment option of message
			m_pSContext2->pGridMain->SetCtrlAlignment((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, eTextAlignment);
			// 프로그레스 바를 그리드 레이아웃에 추가
			// Add progress bar to grid layout
			m_pSContext2->pGridMain->AddCtrl((int32_t)ERowMainComplex::ProgressCtrl, (int32_t)EColMainComplex::ProgressCtrl, 1, 4, EControl_ProgressBarFloatingPoint);
			// 소요 시간을 그리드 레이아웃에 추가
			// Add elapsed time to grid layout
			m_pSContext2->pGridMain->AddCtrl((int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime, 1, 4, L"Elapsed Time      00 : 00 : 00");
			// 소요 시간의 텍스트 정렬 옵션을 설정
			// Set elapsed time text alignment
			m_pSContext2->pGridMain->SetCtrlAlignment((int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime, EGUIAlignment_CENTER_CENTER);
			// 소요 시간 위치 설정
			// Set elapsed time position
			m_pSContext2->pDlgProgress->SetElapsedTimePosition(m_pSContext2->pGridMain, (int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime);
			// 진행률 업데이트를 위해 프로그레스 바 객체 포인터를 얻어 오기
			// Get progress bar object pointer for progress update
			m_pSContext2->pProgressCtrlFP = (CGUIProgressCtrlFloatingPoint*)m_pSContext2->pGridMain->GetCtrl((int32_t)ERowMainComplex::ProgressCtrl, (int32_t)EColMainComplex::ProgressCtrl);
		}

		// 버튼이 표시되는 버튼 그리드 레이아웃 생성
		// Create button grid layout displaying buttons
		m_pSContext2->pGridButtons = new CGUIGridLayout((int32_t)ERowButtonComplex::Count, (int32_t)EColButton::Count, true, L"m_pSContext2->pGridButtons");
		{
			// 상세 보기 버튼을 그리드 레이아웃에 추가
			// Add details button to grid layout
			m_pSContext2->pGridButtons->AddCtrl((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, EControl_Button, L"See Details");

			CWndCtrlProcedure* pVCP = new CWndCtrlProcedure;
			*pVCP = MakeWndCtrlProcedure
			{
				if(((CGUIButton*)m_pSContext2->pGridButtons->GetCtrl((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button))->GetClickCount() % 2 == 1)
				{
					m_pSContext2->pGridDetails->SetVisible(true);
					m_pSContext2->pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, L"Close Details");
				}
				else
				{
					m_pSContext2->pGridDetails->SetVisible(false);
					m_pSContext2->pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, L"See Details");
				}
				m_pSContext2->pDlgProgress->AdjustLayout();
				return true;
			};
			m_pSContext2->pGridButtons->SetWndCtrlProcedure((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, pVCP);

			// 중지 버튼을 그리드 레이아웃에 추가
			// Add stop button to grid layout
			m_pSContext2->pGridButtons->AddCtrl((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button, EControl_Button, L"Stop");
			m_pSContext2->pStopButton = (CGUIButton*)m_pSContext2->pGridButtons->GetCtrl((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button);
		}

		// 상세 정보가 표시되는 그리드 레이아웃 생성
		// Create detail info grid layout
		m_pSContext2->pGridDetails = new CGUIGridLayout(7, 2, true, L"m_pSContext2->pGridDetails");
		{
			m_pSContext2->pGridDetails->AddCtrl(0, 0, 1, 2, L"In a general sense, continuity \ndescribes how two items \ncome together. In ACIS, \nthese items may be two curves \nthat meet in some way, \ntwo portions of the same curve, etc. (In the latter case, \none is usually describing \nthe smoothness of a curve, \nwhich is a global property, \nin terms of a local property.)");
			m_pSContext2->pGridDetails->SetCtrlAlignment(0, 0, EGUIAlignment_RIGHT_TOP);
			m_pSContext2->pGridDetails->AddCtrl(1, 0, 1, 2, EControl_Slider);
			m_pSContext2->pGridDetails->AddCtrl(2, 0, EControl_CheckBox, L"ACIS");
			m_pSContext2->pGridDetails->AddCtrl(2, 1, EControl_ComboBox);
			m_pSContext2->pGridDetails->AddComboBoxItem(3, 1, L"C0");
			m_pSContext2->pGridDetails->AddComboBoxItem(3, 1, L"C1");
			m_pSContext2->pGridDetails->AddComboBoxItem(3, 1, L"C2");
			m_pSContext2->pGridDetails->AddComboBoxItem(3, 1, L"G1");
			m_pSContext2->pGridDetails->AddComboBoxItem(3, 1, L"G2");
			m_pSContext2->pGridDetails->AddCtrl(4, 0, L"Expression");
			m_pSContext2->pGridDetails->AddCtrl(4, 1, EControl_EditCtrl);
			m_pSContext2->pGridDetails->AddCtrl(5, 0, L"Slider2");
			m_pSContext2->pGridDetails->AddCtrl(5, 1, EControl_SliderFloatingPoint);

			CGUIBoxLayout* pBoxInner = new CGUIBoxLayout(EOrientation_Vertical, true, L"Box");
			CGUIGridLayout* pGridInner = new CGUIGridLayout(3, 2, true, L"Grid");
			pBoxInner->AddCtrl(EControl_EditCtrl);
			pBoxInner->AddCtrl(EControl_EditCtrl);
			pBoxInner->AddCtrl(EControl_EditCtrl);
			pGridInner->AddCtrl(0, 0, L"Edit1");
			pGridInner->AddCtrl(1, 0, L"Edit2");
			pGridInner->AddCtrl(2, 0, L"Edit2");
			pGridInner->AddCtrl(0, 1, EControl_EditCtrl);
			pGridInner->AddCtrl(1, 1, EControl_EditCtrl);
			pGridInner->AddCtrl(2, 1, EControl_EditCtrl);
			m_pSContext2->pGridDetails->Add(6, 0, pBoxInner);
			m_pSContext2->pGridDetails->Add(6, 1, pGridInner);
		}

		// 레이아웃들을 배치하기 위한 배경 레이아웃 생성
		// Create background layout to place layouts
		CGUIGridLayout* pGridBackground = new CGUIGridLayout((int32_t)ERowLayout::Count, (int32_t)EColLayout::Count, true, L"pGridBackground");

		pGridBackground->Add((int32_t)ERowLayout::Main, (int32_t)EColLayout::Main, 4, 1, m_pSContext2->pGridMain);
		pGridBackground->Add((int32_t)ERowLayout::Button, (int32_t)EColLayout::Button, m_pSContext2->pGridButtons);
		pGridBackground->Add((int32_t)ERowLayout::Details, (int32_t)EColLayout::Details, 5, 1, m_pSContext2->pGridDetails);
		m_pSContext2->pGridDetails->SetVisible(false);

		m_pSContext2->pDlgProgress->Add(pGridBackground);

		m_pSContext2->pDlgProgress->SetResizeWindowPivot(eResizePivot);
		m_pSContext2->pDlgProgress->KeepPreviousMaximumWidth(bKeepMaxWidth);
		m_pSContext2->pDlgProgress->KeepPreviousMaximumHeight(bKeepMaxHeight);
		m_pSContext2->pDlgProgress->OnInitDialog();

		m_pSContext2->pThread = new std::future<void>(std::async(std::launch::async, CPropertyMenuDialogProgressExample::AlgorithmThreadForComplexDialog, this, m_pSContext2));
		SetThreadPriority(m_pSContext2->pThread, THREAD_PRIORITY_LOWEST);

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::CreateComplexProgressDialogModal()
{
	CResult cr;

	do
	{
		// 워커 스레드에 종료 요청
		// Request termination to the worker thread
		if(m_pSContext3)
			m_pSContext3->TerminateProgressThread();

		if(!m_pSContext3)
			m_pSContext3 = new SProgressThreadContext;

		if(!m_pSContext3)
			break;

		m_pSContext3->ResetThreadParams();

		EGUIAlignment eTextAlignment = EGUIAlignment_CENTER_CENTER;
		EGUIAlignment eResizePivot = EGUIAlignment_LEFT_TOP;
		bool bKeepMaxWidth = false, bKeepMaxHeight = false;
		GetDialogSettings(eTextAlignment, eResizePivot, bKeepMaxWidth, bKeepMaxHeight);

		// 다이얼로그 생성 (Modal)
		// Create dialog (Modal)
		if(!m_pSContext3->pDlgProgress)
		{
			m_pSContext3->pDlgProgress = new CGUIDialogProgress(AfxGetApp()->m_pMainWnd);
			m_pSContext3->pDlgProgress->SetTitle(L"MyTitle");
			m_pSContext3->pDlgProgress->ShowCaption(true);
			m_pSContext3->pDlgProgress->ShowMinimizeButtonOnCaption(true);
			m_pSContext3->pDlgProgress->SetMinimizeWidth(300);
			CWndCtrlProcedure* pCb = new CWndCtrlProcedure;
			*pCb = MakeWndCtrlProcedure
			{
				return true; // Do nothing
			};
			m_pSContext3->pDlgProgress->SetCloseButtonOnCaptionCallback(pCb);
		}

		CFLString<wchar_t> strMessage;
		CFLString<wchar_t> strMessageFormat =
			CFLString<wchar_t>(L"[Step 3/3] Learning...\n\n") +
			CFLString<wchar_t>(L"<Cost>\n") +
			CFLString<wchar_t>(L"0.012132\n\n") +
			CFLString<wchar_t>(L"<Iteration>\n") +
			CFLString<wchar_t>(L"(%d/%d)\n\n") +
			CFLString<wchar_t>(L"<Epoch>\n") +
			CFLString<wchar_t>(L"1/100\n\n") +
			CFLString<wchar_t>(L"<Validation>\n") +
			CFLString<wchar_t>(L"96.12%%at 1 epoch\n\n\n");

		// 메인 그리드 및 컨트롤 설정 (생략된 주석 및 코드는 Complex와 동일, 컨텍스트만 SContext3 사용)
		m_pSContext3->pGridMain = new CGUIGridLayout((int32_t)ERowMainComplex::Count, (int32_t)EColMainComplex::Count, true, L"m_pSContext3->pGridMain");
		{
			strMessage.Format(strMessageFormat, m_pSContext3->i32CurrentProgress, m_pSContext3->i32TotalProgress);

			m_pSContext3->pGridMain->AddCtrl((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, 3, 4, strMessage);
			m_pSContext3->pGridMain->SetCtrlAlignment((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, eTextAlignment);
			m_pSContext3->pGridMain->AddCtrl((int32_t)ERowMainComplex::ProgressCtrl, (int32_t)EColMainComplex::ProgressCtrl, 1, 4, EControl_ProgressBarFloatingPoint);
			m_pSContext3->pGridMain->AddCtrl((int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime, 1, 4, L"Elapsed Time      00 : 00 : 00");
			m_pSContext3->pGridMain->SetCtrlAlignment((int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime, EGUIAlignment_CENTER_CENTER);
			m_pSContext3->pDlgProgress->SetElapsedTimePosition(m_pSContext3->pGridMain, (int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime);
			m_pSContext3->pProgressCtrlFP = (CGUIProgressCtrlFloatingPoint*)m_pSContext3->pGridMain->GetCtrl((int32_t)ERowMainComplex::ProgressCtrl, (int32_t)EColMainComplex::ProgressCtrl);
		}

		m_pSContext3->pGridButtons = new CGUIGridLayout((int32_t)ERowButtonComplex::Count, (int32_t)EColButton::Count, true, L"m_pSContext3->pGridButtons");
		{
			m_pSContext3->pGridButtons->AddCtrl((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, EControl_Button, L"See Details");
			CWndCtrlProcedure* pVCP = new CWndCtrlProcedure;
			*pVCP = MakeWndCtrlProcedure
			{
				if(((CGUIButton*)m_pSContext3->pGridButtons->GetCtrl((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button))->GetClickCount() % 2 == 1)
				{
					m_pSContext3->pGridDetails->SetVisible(true);
					m_pSContext3->pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, L"Close Details");
				}
				else
				{
					m_pSContext3->pGridDetails->SetVisible(false);
					m_pSContext3->pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, L"See Details");
				}
				m_pSContext3->pDlgProgress->AdjustLayout();
				return true;
			};
			m_pSContext3->pGridButtons->SetWndCtrlProcedure((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, pVCP);

			m_pSContext3->pGridButtons->AddCtrl((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button, EControl_Button, L"Stop");
			m_pSContext3->pStopButton = (CGUIButton*)m_pSContext3->pGridButtons->GetCtrl((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button);
		}

		m_pSContext3->pGridDetails = new CGUIGridLayout(7, 2, true, L"m_pSContext3->pGridDetails");
		{
			m_pSContext3->pGridDetails->AddCtrl(0, 0, 1, 2, L"In a general sense, continuity \ndescribes how two items \ncome together. In ACIS, \nthese items may be two curves \nthat meet in some way, \ntwo portions of the same curve, etc. (In the latter case, \none is usually describing \nthe smoothness of a curve, \nwhich is a global property, \nin terms of a local property.)");
			m_pSContext3->pGridDetails->SetCtrlAlignment(0, 0, EGUIAlignment_RIGHT_TOP);
			m_pSContext3->pGridDetails->AddCtrl(1, 0, 1, 2, EControl_Slider);
			m_pSContext3->pGridDetails->AddCtrl(2, 0, EControl_CheckBox, L"ACIS");
			m_pSContext3->pGridDetails->AddCtrl(2, 1, EControl_ComboBox);
			m_pSContext3->pGridDetails->AddComboBoxItem(3, 1, L"C0");
			m_pSContext3->pGridDetails->AddComboBoxItem(3, 1, L"C1");
			m_pSContext3->pGridDetails->AddComboBoxItem(3, 1, L"C2");
			m_pSContext3->pGridDetails->AddComboBoxItem(3, 1, L"G1");
			m_pSContext3->pGridDetails->AddComboBoxItem(3, 1, L"G2");
			m_pSContext3->pGridDetails->AddCtrl(4, 0, L"Expression");
			m_pSContext3->pGridDetails->AddCtrl(4, 1, EControl_EditCtrl);
			m_pSContext3->pGridDetails->AddCtrl(5, 0, L"Slider2");
			m_pSContext3->pGridDetails->AddCtrl(5, 1, EControl_SliderFloatingPoint);

			CGUIBoxLayout* pBoxInner = new CGUIBoxLayout(EOrientation_Vertical, true, L"Box");
			CGUIGridLayout* pGridInner = new CGUIGridLayout(3, 2, true, L"Grid");
			pBoxInner->AddCtrl(EControl_EditCtrl);
			pBoxInner->AddCtrl(EControl_EditCtrl);
			pBoxInner->AddCtrl(EControl_EditCtrl);
			pGridInner->AddCtrl(0, 0, L"Edit1");
			pGridInner->AddCtrl(1, 0, L"Edit2");
			pGridInner->AddCtrl(2, 0, L"Edit2");
			pGridInner->AddCtrl(0, 1, EControl_EditCtrl);
			pGridInner->AddCtrl(1, 1, EControl_EditCtrl);
			pGridInner->AddCtrl(2, 1, EControl_EditCtrl);
			m_pSContext3->pGridDetails->Add(6, 0, pBoxInner);
			m_pSContext3->pGridDetails->Add(6, 1, pGridInner);
		}

		CGUIGridLayout* pGridBackground = new CGUIGridLayout((int32_t)ERowLayout::Count, (int32_t)EColLayout::Count, true, L"pGridBackground");

		pGridBackground->Add((int32_t)ERowLayout::Main, (int32_t)EColLayout::Main, 4, 1, m_pSContext3->pGridMain);
		pGridBackground->Add((int32_t)ERowLayout::Button, (int32_t)EColLayout::Button, m_pSContext3->pGridButtons);
		pGridBackground->Add((int32_t)ERowLayout::Details, (int32_t)EColLayout::Details, 5, 1, m_pSContext3->pGridDetails);
		m_pSContext3->pGridDetails->SetVisible(false);

		m_pSContext3->pDlgProgress->Add(pGridBackground);

		m_pSContext3->pDlgProgress->SetResizeWindowPivot(eResizePivot);
		m_pSContext3->pDlgProgress->KeepPreviousMaximumWidth(bKeepMaxWidth);
		m_pSContext3->pDlgProgress->KeepPreviousMaximumHeight(bKeepMaxHeight);

		// 작업 스레드 생성 (통합된 스레드 함수 활용)
		// Create worker thread (using unified thread function)
		m_pSContext3->pThread = new std::future<void>(std::async(std::launch::async, CPropertyMenuDialogProgressExample::AlgorithmThreadForComplexDialog, this, m_pSContext3));
		SetThreadPriority(m_pSContext3->pThread, THREAD_PRIORITY_LOWEST);

		// 다이얼로그 창 실행
		// Execute dialog window
		m_pSContext3->pDlgProgress->DoModal();

		if(m_pSContext3->pDlgProgress)
		{
			delete m_pSContext3->pDlgProgress;
			m_pSContext3->pDlgProgress = nullptr;
		}

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::CreateComplexProgressDialogWithSheet(bool bBoxLayout, SProgressThreadContext* pContext)
{
	CResult cr;

	do
	{
		if(!pContext)
			break;

		EGUIAlignment eTextAlignment = EGUIAlignment_CENTER_CENTER;
		EGUIAlignment eResizePivot = EGUIAlignment_LEFT_TOP;
		bool bKeepMaxWidth = false, bKeepMaxHeight = false;
		GetDialogSettings(eTextAlignment, eResizePivot, bKeepMaxWidth, bKeepMaxHeight);

		// 다이얼로그 생성
		// Create dialog
		if(!pContext->pDlgProgress)
			pContext->pDlgProgress = CGUIDialogProgress::CreateModelessDialog(AfxGetApp()->m_pMainWnd);

		CFLString<wchar_t> strMessage;
		CFLString<wchar_t> strMessageFormat =
			CFLString<wchar_t>(L"[Step 3/3] Learning...\n\n") +
			CFLString<wchar_t>(L"<Cost>\n") +
			CFLString<wchar_t>(L"0.012132\n\n") +
			CFLString<wchar_t>(L"<Iteration>\n") +
			CFLString<wchar_t>(L"(%d/%d)\n\n") +
			CFLString<wchar_t>(L"<Epoch>\n") +
			CFLString<wchar_t>(L"1/100\n\n") +
			CFLString<wchar_t>(L"<Validation>\n") +
			CFLString<wchar_t>(L"96.12%%at 1 epoch\n\n\n");

		// 메시지, 프로그레스 바, 시간 등이 표시되는 메인 그리드 레이아웃 생성
		// Create main grid layout
		pContext->pGridMain = new CGUIGridLayout((int32_t)ERowMainComplex::Count, (int32_t)EColMainComplex::Count, true, L"pContext->pGridMain");
		{
			strMessage.Format(strMessageFormat, pContext->i32CurrentProgress, pContext->i32TotalProgress);
			pContext->pGridMain->AddCtrl((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, 3, 4, strMessage);
			pContext->pGridMain->SetCtrlAlignment((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, eTextAlignment);
			pContext->pGridMain->AddCtrl((int32_t)ERowMainComplex::ProgressCtrl, (int32_t)EColMainComplex::ProgressCtrl, 1, 4, EControl_ProgressBarFloatingPoint);
			pContext->pGridMain->AddCtrl((int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime, 1, 4, L"Elapsed Time      00 : 00 : 00");
			pContext->pGridMain->SetCtrlAlignment((int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime, EGUIAlignment_CENTER_CENTER);
			pContext->pDlgProgress->SetElapsedTimePosition(pContext->pGridMain, (int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime);
			pContext->pProgressCtrlFP = (CGUIProgressCtrlFloatingPoint*)pContext->pGridMain->GetCtrl((int32_t)ERowMainComplex::ProgressCtrl, (int32_t)EColMainComplex::ProgressCtrl);
		}

		// 버튼이 표시되는 버튼 그리드 레이아웃 생성
		// Create button grid layout
		pContext->pGridButtons = new CGUIGridLayout((int32_t)ERowButtonComplex::Count, (int32_t)EColButton::Count, true, L"pContext->pGridButtons");
		{
			pContext->pGridButtons->AddCtrl((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, EControl_Button, L"See Details");
			CWndCtrlProcedure* pVCP = new CWndCtrlProcedure;
			if(pContext == m_pSContext5)
			{
				*pVCP = MakeWndCtrlProcedure
				{
					if(((CGUIButton*)m_pSContext5->pGridButtons->GetCtrl((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button))->GetClickCount() % 2 == 1)
					{
						m_pSContext5->pLayoutForSheet->SetVisible(true);
						m_pSContext5->pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, L"Close Details");
					}
					else
					{
						m_pSContext5->pLayoutForSheet->SetVisible(false);
						m_pSContext5->pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, L"See Details");
					}
					m_pSContext5->pDlgProgress->AdjustLayout();
					return true;
				};
			}
			else
			{
				*pVCP = MakeWndCtrlProcedure
				{
					if(((CGUIButton*)m_pSContext6->pGridButtons->GetCtrl((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button))->GetClickCount() % 2 == 1)
					{
						m_pSContext6->pLayoutForSheet->SetVisible(true);
						m_pSContext6->pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, L"Close Details");
					}
					else
					{
						m_pSContext6->pLayoutForSheet->SetVisible(false);
						m_pSContext6->pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, L"See Details");
					}
					m_pSContext6->pDlgProgress->AdjustLayout();
					return true;
				};
			}
			pContext->pGridButtons->SetWndCtrlProcedure((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, pVCP);

			pContext->pGridButtons->AddCtrl((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button, EControl_Button, L"Stop");
			pContext->pStopButton = (CGUIButton*)pContext->pGridButtons->GetCtrl((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button);
		}

		// CFLSheet 가 포함된 레이아웃 생성
		// Create layout containing CFLSheet
		if(bBoxLayout)
		{
			pContext->pLayoutForSheet = new CGUIBoxLayout(EOrientation_Vertical, 300, 500, true, L"pContext->pLayoutForSheet");
			((CGUIBoxLayout*)pContext->pLayoutForSheet)->AddCtrl(EControl_Sheet);
			((CGUIBoxLayout*)pContext->pLayoutForSheet)->AddCtrl(EControl_Sheet);
		}
		else
		{
			pContext->pLayoutForSheet = new CGUIGridLayout(14, 2, 300, 500, true, L"pContext->pLayoutForSheet");
			((CGUIGridLayout*)pContext->pLayoutForSheet)->AddCtrl(0, 0, L"Detail 1");
			((CGUIGridLayout*)pContext->pLayoutForSheet)->AddCtrl(1, 0, L"Detail 2");
			((CGUIGridLayout*)pContext->pLayoutForSheet)->AddCtrl(2, 0, L"Detail 3");
			((CGUIGridLayout*)pContext->pLayoutForSheet)->AddCtrl(0, 1, EControl_EditCtrl);
			((CGUIGridLayout*)pContext->pLayoutForSheet)->AddCtrl(1, 1, EControl_EditCtrl);
			((CGUIGridLayout*)pContext->pLayoutForSheet)->AddCtrl(2, 1, EControl_EditCtrl);

			((CGUIGridLayout*)pContext->pLayoutForSheet)->AddCtrl(3, 0, 6, 2, EControl_Sheet);
			((CGUIGridLayout*)pContext->pLayoutForSheet)->AddCtrl(9, 0, 5, 2, EControl_Sheet);
		}

		for(int i = 0; i < 2; ++i)
		{
			CGUISheetInterface* pSheetInterface = nullptr;

			if(bBoxLayout)
				pSheetInterface = ((CGUIBoxLayout*)pContext->pLayoutForSheet)->GetSheetInterface(i);
			else
				pSheetInterface = ((CGUIGridLayout*)pContext->pLayoutForSheet)->GetSheetInterface(i == 0 ? 3 : 9, 0);

			CFLArray<CFLArray<CFLString<wchar_t>>>* pData = pSheetInterface->GetDataPtr();

			int32_t i32ColCnt = 10;
			int32_t i32RowCnt = 10;
			int32_t i32FixedColCnt = 1;
			int32_t i32FixedRowCnt = 1;

			for(int32_t r = 0; r < i32FixedRowCnt; r++)
			{
				pData->Append(CFLArray<CFLString<wchar_t>>());
				CFLArray<CFLString<wchar_t>>& arrRow = pData->Back();
				for(int32_t c = 0; c < i32ColCnt; c++) arrRow.Append(CFLString<wchar_t>().Format(L"Fixed (%d,%d)", r, c));
			}

			for(int32_t r = i32FixedRowCnt; r < i32RowCnt; r++)
			{
				pData->Append(CFLArray<CFLString<wchar_t>>());
				CFLArray<CFLString<wchar_t>>& arrRow = pData->Back();
				for(int32_t c = 0; c < i32FixedColCnt; c++) arrRow.Append(CFLString<wchar_t>().Format(L"Fixed (%d,%d)", r, c));
				for(int32_t c = i32FixedColCnt; c < i32ColCnt; c++) arrRow.Append(CFLString<wchar_t>().Format(L"(%d,%d)", r, c));
			}

			pSheetInterface->UpdateSheetData();
			pSheetInterface->SetFixedRowCount(i32FixedRowCnt);
			pSheetInterface->SetFixedColumnCount(i32FixedColCnt);
		}

		CGUIGridLayout* pGridBackground = new CGUIGridLayout((int32_t)ERowLayout::Count, (int32_t)EColLayout::Count, true, L"pGridBackground");

		pGridBackground->Add((int32_t)ERowLayout::Main, (int32_t)EColLayout::Main, 4, 1, pContext->pGridMain);
		pGridBackground->Add((int32_t)ERowLayout::Button, (int32_t)EColLayout::Button, pContext->pGridButtons);
		pGridBackground->Add((int32_t)ERowLayout::Details, (int32_t)EColLayout::Details, 5, 1, pContext->pLayoutForSheet);
		pContext->pLayoutForSheet->SetVisible(false);

		pContext->pDlgProgress->Add(pGridBackground);
		pContext->pDlgProgress->SetResizeWindowPivot(eResizePivot);
		pContext->pDlgProgress->KeepPreviousMaximumWidth(bKeepMaxWidth);
		pContext->pDlgProgress->KeepPreviousMaximumHeight(bKeepMaxHeight);
		pContext->pDlgProgress->OnInitDialog();

		// 통합 스레드 실행
		// Run unified thread
		pContext->pThread = new std::future<void>(std::async(std::launch::async, CPropertyMenuDialogProgressExample::AlgorithmThreadForComplexDialog, this, pContext));
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_LOWEST);

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::CreateSimpleDialogWithSettings()
{
	CResult cr;

	do
	{
		// 워커 스레드에 종료 요청
		// Request termination to the worker thread
		if(m_pSContext4)
			m_pSContext4->TerminateProgressThread();

		if(!m_pSContext4)
			m_pSContext4 = new SProgressThreadContext;

		if(!m_pSContext4)
			break;

		m_pSContext4->ResetThreadParams();

		EGUIAlignment eTextAlignment = EGUIAlignment_CENTER_CENTER;
		EGUIAlignment eResizePivot = EGUIAlignment_LEFT_TOP;
		bool bKeepMaxWidth = false, bKeepMaxHeight = false;
		GetDialogSettings(eTextAlignment, eResizePivot, bKeepMaxWidth, bKeepMaxHeight);

		// 다이얼로그 생성 (내부 구현은 기존 재활용)
		// Create dialog (reusing internal implementation)
		SProgressThreadContext* pTemp = m_pSContext1;
		m_pSContext1 = m_pSContext4; // 임시 스와핑 / Temporary swapping
		CreateSimpleDialog();
		m_pSContext1 = pTemp;

		// 다이얼로그 설정 적용
		// Apply dialog settings
		m_pSContext4->pGridMain->SetCtrlAlignment((int32_t)ERowMainSimple::Message, (int32_t)ERowMainSimple::Message, eTextAlignment);
		m_pSContext4->pDlgProgress->SetResizeWindowPivot(eResizePivot);
		m_pSContext4->pDlgProgress->KeepPreviousMaximumWidth(bKeepMaxWidth);
		m_pSContext4->pDlgProgress->KeepPreviousMaximumHeight(bKeepMaxHeight);
		m_pSContext4->pDlgProgress->OnInitDialog();

		// 통합 작업 스레드 생성 (bWithSettings 플래그 true)
		// Create unified worker thread (bWithSettings flag true)
		m_pSContext4->pThread = new std::future<void>(std::async(std::launch::async, CPropertyMenuDialogProgressExample::AlgorithmThreadForSimpleDialog, this, m_pSContext4, true));
		SetThreadPriority(m_pSContext4->pThread, THREAD_PRIORITY_LOWEST);

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::UpdateSimpleDialog(SProgressThreadContext* pSContext, bool bWithSettings)
{
	CResult cr;

	do
	{
		if(!pSContext)
		{
			cr = EResult_NullPointer;
			break;
		}

		// 가변 길이의 문자열을 설정
		// Set variable length string
		CFLString<wchar_t> strMessage;

		if(bWithSettings)
		{
			strMessage.Format
			(
				m_vctVariousLengthMessages[pSContext->i32CurrentProgress % (int32_t)m_vctVariousLengthMessages.size()] + L"\n(%d/%d)",
				pSContext->i32CurrentProgress, pSContext->i32TotalProgress
			);
		}
		else
		{
			strMessage.Format
			(
				L"CGUIDialogProgress example thread is working...\n(%d/%d)",
				pSContext->i32CurrentProgress, pSContext->i32TotalProgress
			);
		}

		// 메세지 설정
		// Set message
		pSContext->pGridMain->SetCtrlValue((int32_t)ERowMainSimple::Message, (int32_t)ERowMainSimple::Message, strMessage);

		// 프로그레스 바에 진행률 표시
		// Display progress rate on the progress bar
		pSContext->pProgressCtrlFP->SetPos((double)pSContext->i32CurrentProgress / (double)pSContext->i32TotalProgress);
		// 다이얼로그 창 크기 업데이트
		// Update dialog window size
		pSContext->pDlgProgress->AdjustLayout();

		CGUIManagerDialogProgress::Redraw(pSContext->pDlgProgress);
		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::UpdateComplexDialog(SProgressThreadContext* pSContext, int32_t i32StopClickCount)
{
	CResult cr;

	do
	{
		if(!pSContext)
		{
			cr = EResult_NullPointer;
			break;
		}

		CFLString<wchar_t> strMessage;
		CFLString<wchar_t> strMessageFormat =
			CFLString<wchar_t>(L"[Step 3/3] Learning...\n\n") +
			CFLString<wchar_t>(L"<Cost>\n") +
			CFLString<wchar_t>(L"0.012132\n\n") +
			CFLString<wchar_t>(L"<Iteration>\n") +
			CFLString<wchar_t>(L"(%d/%d)\n\n") +
			CFLString<wchar_t>(L"<Epoch>\n") +
			CFLString<wchar_t>(L"1/100\n\n") +
			CFLString<wchar_t>(L"<Validation>\n") +
			CFLString<wchar_t>(L"96.12%%at 1 epoch\n\n\n");

		// 중지 버튼 클릭 횟수가 변경된 경우
		// When stop button click count has changed
		if(i32StopClickCount != pSContext->i32BtnStopClickCount)
		{
			// 중지 버튼을 한 번 클릭한 경우
			// When stop button is clicked once
			if(i32StopClickCount == 1)
			{
				pSContext->pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button, L"Continue ..");
			}
			// 중지 버튼을 두 번 클릭한 경우 ("Continue.." 버튼을 클릭한 경우)
			// When stop button is clicked twice (When "Continue.." button is clicked)
			else if(i32StopClickCount == 2)
			{
				pSContext->pGridButtons->SetCtrlVisible((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button, false);
			}

			// "Details" 버튼 숨기기
			// Hide "Details" button
			pSContext->pGridButtons->SetCtrlVisible((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, false);

			// 컨텍스트에 따라 상세 정보 또는 Sheet 레이아웃 숨기기
			// Hide details or sheet layout depending on context
			if(pSContext->pGridDetails) pSContext->pGridDetails->SetVisible(false);
			if(pSContext->pLayoutForSheet) pSContext->pLayoutForSheet->SetVisible(false);

			// 중지 버튼 클릭 횟수 업데이트
			// Update stop button click count
			pSContext->i32BtnStopClickCount = i32StopClickCount;
			// 다이얼로그의 레이아웃 업데이트
			// Update layout of the dialog
			pSContext->pDlgProgress->AdjustLayout();
		}

		// 중지 버튼을 한 번 클릭한 경우
		// When stop button is clicked once
		if(i32StopClickCount == 1)
		{
			// 메세지 설정
			// Set message
			strMessage.Format(L"[Step 3/3] Temp Message...\n\n<Temp Progress>");
			pSContext->pGridMain->SetCtrlValue((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, strMessage);

			// 프로그레스 바가 100% 로 채워지도록 설정
			// Set progress bar to 100%
			pSContext->pProgressCtrlFP->SetPos(1.);

			CGUIManagerDialogProgress::Redraw(pSContext->pDlgProgress);
			Sleep(1);
			continue;
		}
		// 중지 버튼을 두 번 클릭한 경우
		// When stop button is clicked twice
		else if(i32StopClickCount == 2)
		{
			// 중지 프로세스 진행이 아직 완료되지 않은 경우
			// If stop process is not yet complete
			if(pSContext->i32CurrentStopProgress <= pSContext->i32TotalStopProgress)
			{
				strMessage.Format(L"[Step 3/3] Learning is being stopped...\n\n<Stop Progress>\n(%d/%d)", pSContext->i32CurrentStopProgress, pSContext->i32TotalStopProgress);
				pSContext->pGridMain->SetCtrlValue((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, strMessage);

				// 프로그레스 바에 진행률 설정
				// Set progress rate on the progress bar
				pSContext->pProgressCtrlFP->SetPos((double)pSContext->i32CurrentStopProgress / (double)pSContext->i32TotalStopProgress);
			}
			else // 중지 프로세스 진행이 아직 완료된 경우 while 문에서 break / Break while loop if stop process is complete
				break;

			CGUIManagerDialogProgress::Redraw(pSContext->pDlgProgress);
			Sleep(1);
			continue;
		}

		// 중지 버튼을 한 번도 누르지 않은 경우
		// When the stop button has never been clicked

		// 메시지 설정
		// Set message
		strMessage.Format
		(
			strMessageFormat,
			pSContext->i32CurrentProgress, pSContext->i32TotalProgress
		);
		pSContext->pGridMain->SetCtrlValue((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, strMessage);

		// 프로그레스 바에 진행률 설정 및 색상 변경
		// Set progress rate on the progress bar and change color
		double f64Progress = (double)pSContext->i32CurrentProgress / (double)pSContext->i32TotalProgress;
		pSContext->pProgressCtrlFP->SetPos(f64Progress);

		if(f64Progress > 0.4 && f64Progress < 0.7)
			pSContext->pProgressCtrlFP->SetBarColor(Foundation::YELLOW);
		else if(f64Progress >= 0.7)
		{
			pSContext->pProgressCtrlFP->SetBarColor(Foundation::RED);
			pSContext->pProgressCtrlFP->SetTextOverColor(Foundation::WHITE);
		}

		CGUIManagerDialogProgress::Redraw(pSContext->pDlgProgress);
		cr = EResult_OK;
	}
	while(false);

	return cr;
}

void FLImaging::GUI::CPropertyMenuDialogProgressExample::AlgorithmThreadForComplexDialog(CPropertyMenuDialogProgressExample* pInstance, SProgressThreadContext* pContext)
{
	// 복잡한 다이얼로그(Sheet, Modal 포함)에서 공통으로 사용되는 통합 스레드
	// Unified thread commonly used in complex dialogs (including Sheet, Modal)
	if(pInstance && pContext && pContext->pDlgProgress)
	{
		while(true)
		{
			// 중지 요청 확인
			// Check stop request
			if(pContext->bThreadStopRequested)
				break;

			// 현재 종료 작업 진행량이 전체 종료 작업 진행량 이상이 된 경우
			// If current stop progress is greater than or equal to total stop progress
			if(pContext->i32CurrentStopProgress >= pContext->i32TotalStopProgress)
			{
				pContext->bThreadStopRequested = true;
				break;
			}

			int32_t i32StopClickCount = 0;

			if(pContext->pStopButton)
				i32StopClickCount += pContext->pStopButton->GetClickCount();

			if(pContext->pDlgProgress)
				i32StopClickCount += pContext->pDlgProgress->GetCloseButtonOnCaptionClickCount();

			// 버튼을 2회 이상 클릭한 경우
			// When button is clicked 2 or more times
			if(i32StopClickCount >= 2)
			{
				Sleep(1);
				// 중지 작업 진행량 값을 증가시킴
				// Increase stop progress value
				++pContext->i32CurrentStopProgress;
			}
			else
			{
				// 현재 작업 진행량이 전체 작업 진행량 이상이 된 경우
				// If current progress is greater than or equal to total progress
				if(pContext->i32CurrentProgress >= pContext->i32TotalProgress)
					break;

				Sleep(1);
				// 작업 진행량의 값을 증가시킴
				// Increase progress value
				++pContext->i32CurrentProgress;
			}

			// 통합 업데이트 함수 호출
			// Call unified update function
			pInstance->UpdateComplexDialog(pContext, i32StopClickCount);
		}

		pContext->CloseProgressDialog();
		pContext->bThreadDone = true;
	}
}

void FLImaging::GUI::CPropertyMenuDialogProgressExample::AlgorithmThreadForSimpleDialog(CPropertyMenuDialogProgressExample* pInstance, SProgressThreadContext* pContext, bool bWithSettings)
{
	// 단순한 다이얼로그에서 공통으로 사용되는 스레드
	// Thread commonly used in simple dialogs
	if(pInstance && pContext && pContext->pDlgProgress)
	{
		while(true)
		{
			int32_t i32StopClickCount = 0;

			if(pContext->pStopButton)
				i32StopClickCount += pContext->pStopButton->GetClickCount();

			if(pContext->pDlgProgress)
				i32StopClickCount += pContext->pDlgProgress->GetCloseButtonOnCaptionClickCount();

			// 버튼을 1회 이상 클릭한 경우
			// When button is clicked 1 or more times
			if(i32StopClickCount >= 1)
				break;
			else
			{
				// 현재 작업 진행량이 전체 작업 진행량 이상이 된 경우
				// If current progress is greater than or equal to total progress
				if(pContext->i32CurrentProgress >= pContext->i32TotalProgress)
					break;

				Sleep(40);
				// 작업 진행량의 값을 증가시킴
				// Increase progress value
				++pContext->i32CurrentProgress;
			}

			// 작업 스레드에서 업데이트 한 값을 반영하여 다이얼로그 창 업데이트
			// Update dialog window with the updated value from worker thread
			pInstance->UpdateSimpleDialog(pContext, bWithSettings);
			Sleep(100);
		}

		pContext->CloseProgressDialog();
		pContext->bThreadDone = true;
	}
}

EGUIAlignment FLImaging::GUI::CPropertyMenuDialogProgressExample::GetAlignment(const Base::CFLString<wchar_t>& strVal) const
{
	EGUIAlignment eReturn = EGUIAlignment_CENTER_CENTER;

	if(strVal == L"CENTER_CENTER")
		eReturn = EGUIAlignment_CENTER_CENTER;
	else if(strVal == L"LEFT_TOP")
		eReturn = EGUIAlignment_LEFT_TOP;
	else if(strVal == L"RIGHT_TOP")
		eReturn = EGUIAlignment_RIGHT_TOP;
	else if(strVal == L"JUSTIFIED")
		eReturn = EGUIAlignment_JUSTIFIED;
	else if(strVal == L"CENTER_TOP")
		eReturn = EGUIAlignment_CENTER_TOP;
	else if(strVal == L"LEFT_BOTTOM")
		eReturn = EGUIAlignment_LEFT_BOTTOM;
	else if(strVal == L"RIGHT_BOTTOM")
		eReturn = EGUIAlignment_RIGHT_BOTTOM;
	else if(strVal == L"CENTER_BOTTOM")
		eReturn = EGUIAlignment_CENTER_BOTTOM;
	else if(strVal == L"LEFT_CENTER")
		eReturn = EGUIAlignment_LEFT_CENTER;
	else if(strVal == L"RIGHT_CENTER")
		eReturn = EGUIAlignment_RIGHT_CENTER;

	return eReturn;
}
