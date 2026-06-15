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
		// 복잡한 다이얼로그의 레이아웃의 row 인덱스 // Row index of the complex dialog layout.
		enum class ERowLayout : int32_t
		{
			Main = 0,           // Main 레이아웃 // Main layout
			Button = 4,         // Button 레이아웃 // Button layout
			Details = 0,        // Details 레이아웃 // Details layout

			Count = 5,          // 행 개수 // Number of rows
		};

		// 복잡한 다이얼로그의 레이아웃의 col 인덱스 // Column index of the complex dialog layout.
		enum class EColLayout : int32_t
		{
			Main = 0,           // Main 레이아웃 // Main layout
			Button = 0,         // Button 레이아웃 // Button layout
			Details = 1,        // Details 레이아웃 // Details layout

			Count = 2,          // 컬럼 개수 // Number of columns
		};

		// 복잡한 다이얼로그의 메인 그리드 내부 컨트롤의 row 인덱스 // Row index of the control in the main grid of the complex dialog.
		enum class ERowMainComplex : int32_t
		{
			Message = 0,        // 메세지 // Message
			ProgressCtrl = 3,   // 프로그레스 바 // Progress bar
			ElapsedTime = 4,    // 경과 시간 // Elapsed Time

			Count = 5,          // 행 개수 // Number of rows
		};

		// 복잡한 다이얼로그의 메인 그리드 내부 컨트롤의 col 인덱스 // Column index of the control in the main grid of the complex dialog.
		enum class EColMainComplex : int32_t
		{
			Message = 1,        // 메세지 // Message
			ProgressCtrl = 1,   // 프로그레스 바 // Progress bar
			ElapsedTime = 1,    // 경과 시간 // Elapsed Time

			Count = 6,          // 컬럼 개수 // Number of columns
		};

		// 단순한 다이얼로그의 메인 그리드 내부 컨트롤의 row 인덱스 // Row index of the control in the main grid of the simple dialog.
		enum class ERowMainSimple : int32_t
		{
			Message = 0,       // 메세지 // Message
			ProgressCtrl = 2,  // 프로그레스 바 // Progress bar
			ElapsedTime = 3,   // 경과 시간 // Elapsed Time

			Count = 4,         // 행 개수 // Number of rows
		};

		// 단순한 다이얼로그의 메인 그리드 내부 컨트롤의 col 인덱스 // Column index of the control in the main grid of the simple dialog.ㄴ
		enum class EColMainSimple : int32_t
		{
			Message = 0,       // 메세지 // Message
			ProgressCtrl = 0,  // 프로그레스 바 // Progress bar
			ElapsedTime = 0,   // 경과 시간 // Elapsed Time

			Count = 1,         // 컬럼 개수 // Number of columns
		};

		// 복잡한 다이얼로그의 버튼 그리드의 row 인덱스 // Row index of the button grid in the complex dialog.
		enum class ERowButtonComplex : int32_t
		{
			Details = 0,       // 세부 사항 버튼 // Details button
			Stop = 1,          // 중지 버튼 // Stop button

			Count = 2,         // 행 개수 // Number of rows
		};

		// 단순한 다이얼로그의 버튼 그리드의 row 인덱스 // Row index of the button grid in the simple dialog.
		enum class ERowButtonSimple : int32_t
		{
			Stop = 0,          // 중지 버튼 // Stop button

			Count = 1,         // 행 개수 // Number of rows
		};

		// 다이얼로그의 버튼 그리드의 col 인덱스 // Column index of the button grid in the dialog.
		enum class EColButton : int32_t
		{
			Button = 2,        // 버튼 // Button

			Count = 3,         // 컬럼 개수 // Number of columns
		};
	}
}

FLImaging::GUI::CPropertyMenuDialogProgressExample::CPropertyMenuDialogProgressExample()
	: CGUIPropertyMenuBase()
	, m_pDlgProgress(nullptr)
	, m_pThread(nullptr)
	, m_pProgressCtrlFP(nullptr)
	, m_pStopButton(nullptr)
	, m_pGridMain(nullptr)
	, m_pGridButtons(nullptr)
	, m_pGridDetails(nullptr)
	, m_i32BtnStopClickCount(0)
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
	CloseProgressDialog();

	if(m_pThread)
	{
		delete m_pThread;
		m_pThread = nullptr;
	}
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::ConfigureMenu()
{
	CResult cReturn = EResult_UnknownError;

	do
	{
		CGUIPropertyButton* pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Simple Dialog");
		pBtn->SetPropertyButtonClickProcedure(ButtonCreateSimpleDialog());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);
		SetShortcut(EKeyType_F6, pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Dialog With Details");
		pBtn->SetPropertyButtonClickProcedure(ButtonCreateComplexDialog());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);
		SetShortcut(EKeyType_F7, pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Param(Pivot, Alignment, ..) Applied Dialog");
		pBtn->SetPropertyButtonClickProcedure(ButtonCreateSimpleDialogWithSettings());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Dialog With CFLSheet - BoxLayout");
		pBtn->SetPropertyButtonClickProcedure(ButtonCreateComplexDialogWithSheetBoxLayout());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Dialog With CFLSheet - GridLayout");
		pBtn->SetPropertyButtonClickProcedure(ButtonCreateComplexDialogWithSheetGridLayout());
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

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuDialogProgressExample::ButtonCreateSimpleDialog()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateSimpleDialog();
		// 다이얼로그 창 생성 // Create the dialog window.
		m_pDlgProgress->OnInitDialog();

		// 작업 스레드 생성 // Create a worker thread.
		m_pThread = new std::future<void>(std::async(CPropertyMenuDialogProgressExample::AlgorithmThreadForSimpleDialog, this, &m_pThread));
		SetThreadPriority(m_pThread, THREAD_PRIORITY_LOWEST);

		// 스레드 파라미터 초기화 // Reset the thread parameters.
		ResetThreadParams();

		// 작업 스레드에서 업데이트 한 값을 반영하여 다이얼로그 창 업데이트 // Update the dialog window with the values updated by the worker thread.
		UpdateSimpleDialog();

		// 다이얼로그 닫기 // Close the dialog.
		CloseProgressDialog();

		if(m_pThread)
		{
			delete m_pThread;
			m_pThread = nullptr;
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

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuDialogProgressExample::ButtonCreateSimpleDialogWithSettings()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateSimpleDialogWithSettings();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuDialogProgressExample::ButtonCreateComplexDialogWithSheetBoxLayout()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		bool bBoxLayout = true;
		CreateComplexProgressDialogWithSheet(bBoxLayout);
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuDialogProgressExample::ButtonCreateComplexDialogWithSheetGridLayout()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		bool bBoxLayout = false;
		CreateComplexProgressDialogWithSheet(bBoxLayout);
	};

	return pProcedure;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::CreateSimpleDialog()
{
	CResult cr;

	do
	{
		CloseProgressDialog();

		// 다이얼로그 창 생성 // Create the dialog window.
		if(!m_pDlgProgress)
			m_pDlgProgress = CGUIDialogProgress::CreateModelessDialog(AfxGetApp()->m_pMainWnd);

		CFLString<wchar_t> strMessage;

		// 메시지, 프로그레스 바, 시간 등이 표시되는 메인 그리드 레이아웃 생성 // Create the main grid layout for displaying messages, the progress bar, and time.
		m_pGridMain = new CGUIGridLayout((int32_t)ERowMainSimple::Count, (int32_t)EColMainSimple::Count);
		{
			strMessage.Format
			(
				L"CGUIDialogProgress example thread is working...\n(%d/%d)",
				m_i32CurrentProgress, m_i32TotalProgress
			);

			// 메시지를 그리드 레이아웃에 추가 // Add the message to the grid layout.
			m_pGridMain->AddCtrl((int32_t)ERowMainSimple::Message, (int32_t)EColMainSimple::Message, 2, 1, strMessage);
			// 프로그레스 바를 그리드 레이아웃에 추가 // Add the progress bar to the grid layout.
			m_pGridMain->AddCtrl((int32_t)ERowMainSimple::ProgressCtrl, (int32_t)EColMainSimple::ProgressCtrl, EControl_ProgressBarFloatingPoint);
			// 소요 시간을 그리드 레이아웃에 추가 // Add the elapsed time to the grid layout.
			m_pGridMain->AddCtrl((int32_t)ERowMainSimple::ElapsedTime, (int32_t)EColMainSimple::ElapsedTime, L"Elapsed Time      00 : 00 : 00");
			// 소요 시간의 텍스트 정렬 옵션을 수평, 수직 모두 중간 정렬로 설정 // Set the text alignment of the elapsed time to center both horizontally and vertically.
			m_pGridMain->SetCtrlAlignment((int32_t)ERowMainSimple::ElapsedTime, (int32_t)EColMainSimple::ElapsedTime, EGUIAlignment_CENTER_CENTER);
			// 소요 시간이 m_pGridMain 레이아웃의 (int32_t)ERowMainSimple::ElapsedTime, (int32_t)EColMainSimple::ElapsedTime 위치에 있다는 것을 m_pDlgProgress 에 알려줌
			// Inform m_pDlgProgress that the elapsed time is located at (int32_t)ERowMainSimple::ElapsedTime and (int32_t)EColMainSimple::ElapsedTime in the m_pGridMain layout.
			m_pDlgProgress->SetElapsedTimePosition(m_pGridMain, (int32_t)ERowMainSimple::ElapsedTime, (int32_t)EColMainSimple::ElapsedTime);
			// 진행률 업데이트를 위해 프로그레스 바 객체 포인터를 얻어 오기 // Get a pointer to the progress bar object for updating the progress.
			m_pProgressCtrlFP = (CGUIProgressCtrlFloatingPoint*)m_pGridMain->GetCtrl((int32_t)ERowMainSimple::ProgressCtrl, (int32_t)EColMainSimple::ProgressCtrl);
		}

		// 버튼이 표시되는 버튼 그리드 레이아웃 생성 // Create the button grid layout for displaying buttons.
		m_pGridButtons = new CGUIGridLayout((int32_t)ERowButtonSimple::Count, (int32_t)EColButton::Count);
		{
			// 중지 버튼을 그리드 레이아웃에 추가 // Add the stop button to the grid layout.
			m_pGridButtons->AddCtrl((int32_t)ERowButtonSimple::Stop, (int32_t)EColButton::Button, EControl_Button, L"Stop");
			// 중지 버튼 객체 포인터를 얻어 오기 // Get a pointer to the stop button object.
			m_pStopButton = (CGUIButton*)m_pGridButtons->GetCtrl((int32_t)ERowButtonSimple::Stop, (int32_t)EColButton::Button);
		}

		// 다이얼로그에 메인 그리드 레이아웃 추가 // Add the main grid layout to the dialog.
		m_pDlgProgress->Add(m_pGridMain);
		// 다이얼로그에 버튼 그리드 레이아웃 추가 // Add the button grid layout to the dialog.
		m_pDlgProgress->Add(m_pGridButtons);

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
		CloseProgressDialog();

		CGUIPropertyItemDropdownList* pDropTextAlignment = dynamic_cast<CGUIPropertyItemDropdownList*>(FindItemByName(L"Text Alignment"));
		CGUIPropertyItemDropdownList* pDropResizePivot = dynamic_cast<CGUIPropertyItemDropdownList*>(FindItemByName(L"Resize Pivot"));
		CGUIPropertyItemCheckBox* pCheckKeepMaxWidth = dynamic_cast<CGUIPropertyItemCheckBox*>(FindItemByName(L"Keep Maximum Width"));
		CGUIPropertyItemCheckBox* pCheckKeepMaxHeight = dynamic_cast<CGUIPropertyItemCheckBox*>(FindItemByName(L"Keep Maximum Height"));

		EGUIAlignment eTextAlignment = GetAlignment(pDropTextAlignment->GetValue());
		EGUIAlignment eResizePivot = GetAlignment(pDropResizePivot->GetValue());
		bool bKeepMaxWidth = pCheckKeepMaxWidth->GetValue() == L"Checked";
		bool bKeepMaxHeight = pCheckKeepMaxHeight->GetValue() == L"Checked";

		// 다이얼로그 창 생성 // Create the dialog window.
		if(!m_pDlgProgress)
			m_pDlgProgress = CGUIDialogProgress::CreateModelessDialog(AfxGetApp()->m_pMainWnd);

		CFLString<wchar_t> strMessage;
		CFLString<wchar_t> strMessageFormat =
			CFLString<wchar_t>(L"[Step 3/3] Learning...\n\n") +
			CFLString<wchar_t>(L"<Cost>\n") +
			CFLString<wchar_t>(L"0.012132\n\n") +
			CFLString<wchar_t>(L"<Iteration>\n") +
			CFLString<wchar_t>(L"(%d/%d)\n\n") + // m_i32CurrentProgress/m_i32TotalProgress
			CFLString<wchar_t>(L"<Epoch>\n") +
			CFLString<wchar_t>(L"1/100\n\n") +
			CFLString<wchar_t>(L"<Validation>\n") +
			CFLString<wchar_t>(L"96.12%%at 1 epoch\n\n\n");

		// 메시지, 프로그레스 바, 시간 등이 표시되는 메인 그리드 레이아웃 생성 // Create the main grid layout for displaying messages, the progress bar, and time.
		m_pGridMain = new CGUIGridLayout((int32_t)ERowMainComplex::Count, (int32_t)EColMainComplex::Count);
		{
			strMessage.Format
			(
				strMessageFormat,
				m_i32CurrentProgress, m_i32TotalProgress
			);

			// 메시지를 그리드 레이아웃에 추가 // Add the message to the grid layout.
			m_pGridMain->AddCtrl((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, 3, 4, strMessage);
			// 메시지의 텍스트 정렬 옵션을 "Text Alignment" 에서 얻어 온 값으로 설정
			m_pGridMain->SetCtrlAlignment((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, eTextAlignment);
			// 프로그레스 바를 그리드 레이아웃에 추가 // Add the progress bar to the grid layout.
			m_pGridMain->AddCtrl((int32_t)ERowMainComplex::ProgressCtrl, (int32_t)EColMainComplex::ProgressCtrl, 1, 4, EControl_ProgressBarFloatingPoint);
			// 소요 시간을 그리드 레이아웃에 추가 // Add the elapsed time to the grid layout.
			m_pGridMain->AddCtrl((int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime, 1, 4, L"Elapsed Time      00 : 00 : 00");
			// 소요 시간의 텍스트 정렬 옵션을 수평, 수직 모두 중간 정렬로 설정 // Set the text alignment of the elapsed time to center both horizontally and vertically.
			m_pGridMain->SetCtrlAlignment((int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime, EGUIAlignment_CENTER_CENTER);
			// 소요 시간이 m_pGridMain 레이아웃의 (int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime 위치에 있다는 것을 m_pDlgProgress 에 알려줌
			// Inform m_pDlgProgress that the elapsed time is located at (int32_t)ERowMainSimple::ElapsedTime and (int32_t)EColMainSimple::ElapsedTime in the m_pGridMain layout.
			m_pDlgProgress->SetElapsedTimePosition(m_pGridMain, (int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime);
			// 진행률 업데이트를 위해 프로그레스 바 객체 포인터를 얻어 오기 // Get a pointer to the progress bar object for updating the progress.
			m_pProgressCtrlFP = (CGUIProgressCtrlFloatingPoint*)m_pGridMain->GetCtrl((int32_t)ERowMainComplex::ProgressCtrl, (int32_t)EColMainComplex::ProgressCtrl);
		}

		// 버튼이 표시되는 버튼 그리드 레이아웃 생성 // Create the button grid layout for displaying buttons.
		m_pGridButtons = new CGUIGridLayout((int32_t)ERowButtonComplex::Count, (int32_t)EColButton::Count);
		{
			// 상세 보기 버튼을 그리드 레이아웃에 추가 // Add the details button to the grid layout.
			m_pGridButtons->AddCtrl((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, EControl_Button, L"See Details");
			// "See Details" 버튼 클릭 이벤트 처리기 생성 // Create the click event handler for the "See Details" button.
			CWndCtrlProcedure* pVCP = new CWndCtrlProcedure;
			// "See Details" 버튼 클릭 시 수행되는 내용 정의 // Define the action to be performed when the "See Details" button is clicked.
			*pVCP = MakeWndCtrlProcedure
			{
				// "See Details" 버튼을 홀수 번 누른 경우 // If the "See Details" button has been clicked an odd number of times.
				if(((CGUIButton*)m_pGridButtons->GetCtrl((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button))->GetClickCount() % 2 == 1)
				{
					// 디테일 그리드 레이아웃을 show // Show the detail grid layout.
					m_pGridDetails->SetVisible(true);
					// "See Details" 버튼의 텍스트를 "Close Details" 로 변경 // Change the text of the "See Details" button to "Close Details".
					m_pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, L"Close Details");
				}
				// "See Details" 버튼을 짝수 번 누른 경우 // If the "See Details" button has been clicked an even number of times.
				else
				{
					// 디테일 그리드 레이아웃을 hide // Hide the detail grid layout.
					m_pGridDetails->SetVisible(false);
					// "Close Details" 버튼의 텍스트를 "See Details" 로 변경 // Change the text of the "Close Details" button to "See Details".
					m_pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, L"See Details");
				}

				// 다이얼로그의 레이아웃을 업데이트 // Adjust the dialog layout.
				m_pDlgProgress->AdjustLayout();

				return true;
			};
			// "See Details" 버튼에 클릭 이벤트 처리기 추가 // Add a click event handler to the "See Details" button.
			m_pGridButtons->SetWndCtrlProcedure((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, pVCP);

			// 중지 버튼을 그리드 레이아웃에 추가 // Add the stop button to the grid layout.
			m_pGridButtons->AddCtrl((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button, EControl_Button, L"Stop");
			// 중지 버튼 객체 포인터를 얻어 오기 // Get a pointer to the stop button object.
			m_pStopButton = (CGUIButton*)m_pGridButtons->GetCtrl((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button);
		}

		// 상세 정보가 표시되는 그리드 레이아웃 생성 // Create the grid layout for displaying detailed information.
		m_pGridDetails = new CGUIGridLayout(7, 2, true);
		{
			// 내부 컨트롤들 추가 // Add internal controls.
			m_pGridDetails->AddCtrl(0, 0, 1, 2, L"In a general sense, continuity \ndescribes how two items \ncome together. In ACIS, \nthese items may be two curves \nthat meet in some way, \ntwo portions of the same curve, etc. (In the latter case, \none is usually describing \nthe smoothness of a curve, \nwhich is a global property, \nin terms of a local property.)");
			m_pGridDetails->SetCtrlAlignment(0, 0, EGUIAlignment_RIGHT_TOP);
			m_pGridDetails->AddCtrl(1, 0, 1, 2, EControl_Slider);
			m_pGridDetails->AddCtrl(2, 0, EControl_CheckBox, L"ACIS");
			m_pGridDetails->AddCtrl(2, 1, EControl_ComboBox);
			m_pGridDetails->AddComboBoxItem(3, 1, L"C0");
			m_pGridDetails->AddComboBoxItem(3, 1, L"C1");
			m_pGridDetails->AddComboBoxItem(3, 1, L"C2");
			m_pGridDetails->AddComboBoxItem(3, 1, L"G1");
			m_pGridDetails->AddComboBoxItem(3, 1, L"G2");
			m_pGridDetails->AddCtrl(4, 0, L"Expression");
			m_pGridDetails->AddCtrl(4, 1, EControl_EditCtrl);
			m_pGridDetails->AddCtrl(5, 0, L"Slider2");
			m_pGridDetails->AddCtrl(5, 1, EControl_SliderFloatingPoint);

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
			m_pGridDetails->Add(6, 0, pBoxInner);
			m_pGridDetails->Add(6, 1, pGridInner);
		}

		// 레이아웃들을 배치하기 위한 배경 레이아웃 생성 // Create a background layout for arranging the layouts.
		CGUIGridLayout* pGridBackground = new CGUIGridLayout((int32_t)ERowLayout::Count, (int32_t)EColLayout::Count);

		// 메인 그리드 레이아웃을 ERowLayout::Main, EColLayout::Main 위치에서 row span = 4, col span = 1 을 차지하도록 배치
		// Place the main grid layout at ERowLayout::Main and EColLayout::Main, occupying a row span of 4 and a column span of 1.
		pGridBackground->Add((int32_t)ERowLayout::Main, (int32_t)EColLayout::Main, 4, 1, m_pGridMain);
		// 버튼 그리드 레이아웃을 ERowLayout::Button, EColLayout::Main 위치에서 row span = 1, col span = 1 을 차지하도록 배치
		// Place the button grid layout at (ERowLayout::Button, EColLayout::Main) with a row span of 1 and a column span of 1.
		pGridBackground->Add((int32_t)ERowLayout::Button, (int32_t)EColLayout::Button, m_pGridButtons);
		// 메인 그리드 레이아웃을 ERowLayout::Main, EColLayout::Main 위치에서 row span = 5, col span = 1 을 차지하도록 배치
		// Place the main grid layout at (ERowLayout::Main, EColLayout::Main) with a row span of 5 and a column span of 1.
		pGridBackground->Add((int32_t)ERowLayout::Details, (int32_t)EColLayout::Details, 5, 1, m_pGridDetails);
		// 상세 정보 레이아웃 숨기기 처리 // Hide the detailed information layout.
		m_pGridDetails->SetVisible(false);

		// 다이얼로그에 배경 레이아웃 추가 // Add the background layout to the dialog.
		m_pDlgProgress->Add(pGridBackground);

		// 다이얼로그 창의 크기가 변경될 때 어느 위치를 기준으로 창을 확대 또는 축소할지 기준 위치를 정합니다.
		// Set the anchor position used when resizing the dialog window.
		m_pDlgProgress->SetResizeWindowPivot(eResizePivot);
		// 다이얼로그 창의 크기가 변경될 때 변경되기 전 창의 너비가 더 크면, 창의 너비를 변경하지 않고 이전 너비를 유지할지 여부 설정
		// Set whether to keep the previous width when the dialog window is resized and the previous window width is greater than the new width.
		m_pDlgProgress->KeepPreviousMaximumWidth(bKeepMaxWidth);
		// 다이얼로그 창의 크기가 변경될 때 변경되기 전 창의 높이가 더 크면, 창의 높이를 변경하지 않고 이전 높이를 유지할지 여부 설정
		// Set whether to preserve the previous height if it is greater than the new height when the dialog window is resized.
		m_pDlgProgress->KeepPreviousMaximumHeight(bKeepMaxHeight);
		// 다이얼로그 창 생성 // Create the dialog window.
		m_pDlgProgress->OnInitDialog();

		// 작업 스레드 생성 // Create a worker thread.
		m_pThread = new std::future<void>(std::async(CPropertyMenuDialogProgressExample::AlgorithmThreadForComplexDialog, this, &m_pThread));
		SetThreadPriority(m_pThread, THREAD_PRIORITY_LOWEST);

		// 스레드 파라미터 초기화 // Reset the thread parameters.
		ResetThreadParams();

		// 작업 스레드에서 업데이트 한 값을 반영하여 다이얼로그 창 업데이트 // Update the dialog window to reflect the values updated by the worker thread.
		UpdateComplexDialog();

		// 다이얼로그 닫기 // Close the dialog.
		CloseProgressDialog();

		cr = EResult_OK;
	}
	while(false);

	if(m_pThread)
	{
		delete m_pThread;
		m_pThread = nullptr;
	}

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::CreateSimpleDialogWithSettings()
{
	CResult cr;

	do
	{
		CGUIPropertyItemDropdownList* pDropTextAlignment = dynamic_cast<CGUIPropertyItemDropdownList*>(FindItemByName(L"Text Alignment"));
		CGUIPropertyItemDropdownList* pDropResizePivot = dynamic_cast<CGUIPropertyItemDropdownList*>(FindItemByName(L"Resize Pivot"));
		CGUIPropertyItemCheckBox* pCheckKeepMaxWidth = dynamic_cast<CGUIPropertyItemCheckBox*>(FindItemByName(L"Keep Maximum Width"));
		CGUIPropertyItemCheckBox* pCheckKeepMaxHeight = dynamic_cast<CGUIPropertyItemCheckBox*>(FindItemByName(L"Keep Maximum Height"));

		EGUIAlignment eTextAlignment = GetAlignment(pDropTextAlignment->GetValue());
		EGUIAlignment eResizePivot = GetAlignment(pDropResizePivot->GetValue());
		bool bKeepMaxWidth = pCheckKeepMaxWidth->GetValue() == L"Checked";
		bool bKeepMaxHeight = pCheckKeepMaxHeight->GetValue() == L"Checked";

		// 단순한 진행률 표시 다이얼로그 생성 // Create a simple progress dialog.
		CreateSimpleDialog();

		// 다이얼로그의 메시지 텍스트 정렬을 설정 // Set the dialog message text alignment.
		m_pGridMain->SetCtrlAlignment((int32_t)ERowMainSimple::Message, (int32_t)ERowMainSimple::Message, eTextAlignment);
		// 다이얼로그 창의 크기가 변경될 때 어느 위치를 기준으로 창을 확대 또는 축소할지 기준 위치를 정합니다.
		// Set the anchor position used when resizing the dialog window.
		m_pDlgProgress->SetResizeWindowPivot(eResizePivot);
		// 다이얼로그 창의 크기가 변경될 때 변경되기 전 창의 너비가 더 크면, 창의 너비를 변경하지 않고 이전 너비를 유지할지 여부 설정
		// Set whether to keep the previous width when the dialog window is resized and the previous window width is greater than the new width.
		m_pDlgProgress->KeepPreviousMaximumWidth(bKeepMaxWidth);
		// 다이얼로그 창의 크기가 변경될 때 변경되기 전 창의 높이가 더 크면, 창의 높이를 변경하지 않고 이전 높이를 유지할지 여부 설정
		// Set whether to preserve the previous height if it is greater than the new height when the dialog window is resized.
		m_pDlgProgress->KeepPreviousMaximumHeight(bKeepMaxHeight);
		// 다이얼로그 창 생성 // Create the dialog window.
		m_pDlgProgress->OnInitDialog();

		// 작업 스레드 생성 // Create a worker thread.
		m_pThread = new std::future<void>(std::async(CPropertyMenuDialogProgressExample::AlgorithmThreadForSimpleDialog, this, &m_pThread));
		SetThreadPriority(m_pThread, THREAD_PRIORITY_LOWEST);

		// 스레드 파라미터 초기화 // Reset the thread parameters.
		ResetThreadParams();

		// 작업 스레드에서 업데이트 한 값을 반영하여 다이얼로그 창 업데이트 // Update the dialog window to reflect the values updated by the worker thread.
		UpdateSimpleDialogWithSettings();

		// 다이얼로그 닫기 // Close the dialog.
		CloseProgressDialog();

		cr = EResult_OK;
	}
	while(false);

	if(m_pThread)
	{
		delete m_pThread;
		m_pThread = nullptr;
	}

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::UpdateSimpleDialog()
{
	CResult cr;

	do
	{
		while(true)
		{
			// 스레드가 완료되면 break // Break if the thread has completed.
			if(m_bThreadDone)
				break;

			// 중지 버튼을 1회 이상 클릭 시 break // Break if the stop button has been clicked at least once.
			if(m_pStopButton->GetClickCount() >= 1)
				break;
			else
			{
				// 가변 길이의 문자열을 설정 // Set a variable-length string.
				CFLString<wchar_t> strMessage;
				strMessage.Format
				(
					L"CGUIDialogProgress example thread is working...\n(%d/%d)",
					m_i32CurrentProgress, m_i32TotalProgress
				);

				// m_pGridMain 의 ERowMainSimple::Message, ERowMainSimple::Message 위치에 문자열 값 설정하여 다이얼로그 상에서 보이는 메세지 설정
				// Set the message displayed in the dialog by assigning a string value to (ERowMainSimple::Message, EColMainSimple::Message) in m_pGridMain.
				m_pGridMain->SetCtrlValue((int32_t)ERowMainSimple::Message, (int32_t)ERowMainSimple::Message, strMessage);

				// 프로그레스 바에 진행률 표시 // Display the progress on the progress bar.
				m_pProgressCtrlFP->SetPos((double)m_i32CurrentProgress / (double)m_i32TotalProgress);
				// 다이얼로그 창 크기 업데이트 // Adjust the dialog window size.
				m_pDlgProgress->AdjustLayout();
			}
			// 메시지 펌프 // Message pump
			CGUIManager::PeekAndPump();
			// Sleep 시간을 1로 설정 // Set the sleep interval to 1 ms.
			Sleep(1);
		}

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::UpdateSimpleDialogWithSettings()
{
	CResult cr;

	do
	{
		while(true)
		{
			// 스레드가 완료되면 break // Break if the thread has completed.
			if(m_bThreadDone)
				break;

			// 중지 버튼을 1회 이상 클릭 시 break // Break if the stop button has been clicked at least once.
			if(m_pStopButton->GetClickCount() >= 1)
				break;
			else
			{
				// 가변 길이의 문자열을 설정 // Set a variable-length string.
				CFLString<wchar_t> strMessage;
				strMessage.Format
				(
					m_vctVariousLengthMessages[m_i32CurrentProgress % (int32_t)m_vctVariousLengthMessages.size()] + L"\n(%d/%d)",
					m_i32CurrentProgress, m_i32TotalProgress
				);

				// m_pGridMain 의 ERowMainSimple::Message, ERowMainSimple::Message 위치에 문자열 값 설정하여 다이얼로그 상에서 보이는 메세지 설정
				// Set the message displayed in the dialog by assigning a string value to (ERowMainSimple::Message, EColMainSimple::Message) in m_pGridMain.
				m_pGridMain->SetCtrlValue((int32_t)ERowMainSimple::Message, (int32_t)ERowMainSimple::Message, strMessage);

				// 프로그레스 바에 진행률 표시 // Display the progress on the progress bar.
				m_pProgressCtrlFP->SetPos((double)m_i32CurrentProgress / (double)m_i32TotalProgress);
				// 다이얼로그 창 크기 업데이트 // Adjust the dialog window size.
				m_pDlgProgress->AdjustLayout();
			}
			// 메시지 펌프 // Message pump
			CGUIManager::PeekAndPump();
			// Sleep 시간을 100 으로 설정 // Set the sleep interval to 100 ms.
			Sleep(100);
		}

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::UpdateComplexDialog()
{
	CResult cr;

	do
	{
		CFLString<wchar_t> strMessage;
		CFLString<wchar_t> strMessageFormat =
			CFLString<wchar_t>(L"[Step 3/3] Learning...\n\n") +
			CFLString<wchar_t>(L"<Cost>\n") +
			CFLString<wchar_t>(L"0.012132\n\n") +
			CFLString<wchar_t>(L"<Iteration>\n") +
			CFLString<wchar_t>(L"(%d/%d)\n\n") + // m_i32CurrentProgress/m_i32TotalProgress
			CFLString<wchar_t>(L"<Epoch>\n") +
			CFLString<wchar_t>(L"1/100\n\n") +
			CFLString<wchar_t>(L"<Validation>\n") +
			CFLString<wchar_t>(L"96.12%%at 1 epoch\n\n\n");

		while(true)
		{
			// 스레드가 완료되면 break // Break if the thread has completed.
			if(m_bThreadDone)
				break;

			// 중지 버튼 클릭 횟수가 변경된 경우 // If the stop button click count has changed.
			if(m_pStopButton->GetClickCount() != m_i32BtnStopClickCount)
			{
				// 중지 버튼을 한 번 클릭한 경우 // If the stop button has been clicked once.
				if(m_pStopButton->GetClickCount() == 1)
				{
					// 중지 버튼 내 문자열을 "Continue.." 로 설정 // Set the text of the stop button to "Continue...".
					m_pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button, L"Continue ..");
				}
				// 중지 버튼을 두 번 클릭한 경우 ("Continue.." 버튼을 클릭한 경우) // If the stop button has been clicked twice ("Continue..." button clicked).
				else if(m_pStopButton->GetClickCount() == 2)
				{
					// 중지 버튼 숨기기 // Hide the stop button.
					m_pGridButtons->SetCtrlVisible((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button, false);
				}

				// "Details" 버튼 숨기기 // Hide the "Details" button.
				m_pGridButtons->SetCtrlVisible((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, false);
				// 상세 정보 레이아웃 숨기기 // Hide the detailed information layout.
				m_pGridDetails->SetVisible(false);
				// 중지 버튼 클릭 횟수 업데이트 // Update the stop button click count.
				m_i32BtnStopClickCount = m_pStopButton->GetClickCount();
				// 다이얼로그의 레이아웃 업데이트 // Adjust the dialog layout.
				m_pDlgProgress->AdjustLayout();
			}

			// 중지 버튼을 한 번 클릭한 경우 // If the stop button has been clicked once.
			if(m_pStopButton->GetClickCount() == 1)
			{
				// 메세지 설정 // Set the message.
				strMessage.Format(L"[Step 3/3] Temp Message...\n\n<Temp Progress>");

				// m_pGridMain 의 ERowMainComplex::Message, EColMainComplex::Message 위치에 문자열 값 설정하여 다이얼로그 상에서 보이는 메세지 설정
				// Set the message displayed in the dialog by assigning a string value to (ERowMainComplex::Message, EColMainComplex::Message) in m_pGridMain.
				m_pGridMain->SetCtrlValue((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, strMessage);

				// 프로그레스 바가 100% 로 채워지도록 설정 // Set the progress bar to 100%.
				m_pProgressCtrlFP->SetPos(1.);
				// 다이얼로그 화면 갱신 // Invalidate the dialog.
				m_pDlgProgress->Invalidate();
				// 메시지 펌프 // Message pump
				CGUIManager::PeekAndPump();
				Sleep(1);
				continue;
			}
			// 중지 버튼을 두 번 클릭한 경우 // If the stop button has been clicked twice.
			else if(m_pStopButton->GetClickCount() == 2)
			{
				// 중지 프로세스 진행이 아직 완료되지 않은 경우 // If the stop process has not yet completed.
				if(m_i32CurrentStopProgress <= m_i32TotalStopProgress)
				{
					// 메세지 설정 // Set the message.
					strMessage.Format(L"[Step 3/3] Learning is being stopped...\n\n<Stop Progress>\n(%d/%d)", m_i32CurrentStopProgress, m_i32TotalStopProgress);
					m_pGridMain->SetCtrlValue((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, strMessage);

					// 프로그레스 바에 진행률 설정 // Set the progress on the progress bar.
					m_pProgressCtrlFP->SetPos((double)m_i32CurrentStopProgress / (double)m_i32TotalStopProgress);

					// 다이얼로그 화면 갱신 // Invalidate the dialog.
					m_pDlgProgress->Invalidate();
				}
				else // 중지 프로세스 진행이 아직 완료된 경우 while 문에서 break // Break out of the loop if the stop process has completed.
					break;

				// 메시지 펌프 // Message pump
				CGUIManager::PeekAndPump();
				Sleep(1);
				continue;
			}

			// 중지 버튼을 한 번도 누르지 않은 경우 // If the stop button has not been clicked.

			// 메세지 설정 // Set the message.
			strMessage.Format
			(
				strMessageFormat,
				m_i32CurrentProgress, m_i32TotalProgress
			);
			m_pGridMain->SetCtrlValue((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, strMessage);

			// 프로그레스 바에 진행률 설정 // Set the progress on the progress bar.
			m_pProgressCtrlFP->SetPos((double)m_i32CurrentProgress / (double)m_i32TotalProgress);

			// 다이얼로그 화면 갱신 // Invalidate the dialog.
			m_pDlgProgress->Invalidate();
			// 메시지 펌프 // Message pump
			CGUIManager::PeekAndPump();
			Sleep(1);
		}

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::CreateComplexProgressDialogWithSheet(bool bBoxLayout)
{
	CResult cr;

	do
	{
		CloseProgressDialog();

		CGUIPropertyItemDropdownList* pDropTextAlignment = dynamic_cast<CGUIPropertyItemDropdownList*>(FindItemByName(L"Text Alignment"));
		CGUIPropertyItemDropdownList* pDropResizePivot = dynamic_cast<CGUIPropertyItemDropdownList*>(FindItemByName(L"Resize Pivot"));
		CGUIPropertyItemCheckBox* pCheckKeepMaxWidth = dynamic_cast<CGUIPropertyItemCheckBox*>(FindItemByName(L"Keep Maximum Width"));
		CGUIPropertyItemCheckBox* pCheckKeepMaxHeight = dynamic_cast<CGUIPropertyItemCheckBox*>(FindItemByName(L"Keep Maximum Height"));

		EGUIAlignment eTextAlignment = GetAlignment(pDropTextAlignment->GetValue());
		EGUIAlignment eResizePivot = GetAlignment(pDropResizePivot->GetValue());
		bool bKeepMaxWidth = pCheckKeepMaxWidth->GetValue() == L"Checked";
		bool bKeepMaxHeight = pCheckKeepMaxHeight->GetValue() == L"Checked";

		// 다이얼로그 창 생성 // Create the dialog window.
		if(!m_pDlgProgress)
			m_pDlgProgress = CGUIDialogProgress::CreateModelessDialog(AfxGetApp()->m_pMainWnd);

		CFLString<wchar_t> strMessage;
		CFLString<wchar_t> strMessageFormat =
			CFLString<wchar_t>(L"[Step 3/3] Learning...\n\n") +
			CFLString<wchar_t>(L"<Cost>\n") +
			CFLString<wchar_t>(L"0.012132\n\n") +
			CFLString<wchar_t>(L"<Iteration>\n") +
			CFLString<wchar_t>(L"(%d/%d)\n\n") + // m_i32CurrentProgress/m_i32TotalProgress
			CFLString<wchar_t>(L"<Epoch>\n") +
			CFLString<wchar_t>(L"1/100\n\n") +
			CFLString<wchar_t>(L"<Validation>\n") +
			CFLString<wchar_t>(L"96.12%%at 1 epoch\n\n\n");

		// 메시지, 프로그레스 바, 시간 등이 표시되는 메인 그리드 레이아웃 생성 // Create the main grid layout for displaying messages, the progress bar, and time.
		m_pGridMain = new CGUIGridLayout((int32_t)ERowMainComplex::Count, (int32_t)EColMainComplex::Count);
		{
			strMessage.Format
			(
				strMessageFormat,
				m_i32CurrentProgress, m_i32TotalProgress
			);

			// 메시지를 그리드 레이아웃에 추가 // Add the message to the grid layout.
			m_pGridMain->AddCtrl((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, 3, 4, strMessage);
			// 메시지의 텍스트 정렬 옵션을 "Text Alignment" 에서 얻어 온 값으로 설정
			m_pGridMain->SetCtrlAlignment((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, eTextAlignment);
			// 프로그레스 바를 그리드 레이아웃에 추가 // Add the progress bar to the grid layout.
			m_pGridMain->AddCtrl((int32_t)ERowMainComplex::ProgressCtrl, (int32_t)EColMainComplex::ProgressCtrl, 1, 4, EControl_ProgressBarFloatingPoint);
			// 소요 시간을 그리드 레이아웃에 추가 // Add the elapsed time to the grid layout.
			m_pGridMain->AddCtrl((int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime, 1, 4, L"Elapsed Time      00 : 00 : 00");
			// 소요 시간의 텍스트 정렬 옵션을 수평, 수직 모두 중간 정렬로 설정 // Set the text alignment of the elapsed time to center both horizontally and vertically.
			m_pGridMain->SetCtrlAlignment((int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime, EGUIAlignment_CENTER_CENTER);
			// 소요 시간이 m_pGridMain 레이아웃의 (int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime 위치에 있다는 것을 m_pDlgProgress 에 알려줌
			// Inform m_pDlgProgress that the elapsed time is located at (int32_t)ERowMainSimple::ElapsedTime and (int32_t)EColMainSimple::ElapsedTime in the m_pGridMain layout.
			m_pDlgProgress->SetElapsedTimePosition(m_pGridMain, (int32_t)ERowMainComplex::ElapsedTime, (int32_t)EColMainComplex::ElapsedTime);
			// 진행률 업데이트를 위해 프로그레스 바 객체 포인터를 얻어 오기 // Get a pointer to the progress bar object for updating the progress.
			m_pProgressCtrlFP = (CGUIProgressCtrlFloatingPoint*)m_pGridMain->GetCtrl((int32_t)ERowMainComplex::ProgressCtrl, (int32_t)EColMainComplex::ProgressCtrl);
		}

		// 버튼이 표시되는 버튼 그리드 레이아웃 생성 // Create the button grid layout for displaying buttons.
		m_pGridButtons = new CGUIGridLayout((int32_t)ERowButtonComplex::Count, (int32_t)EColButton::Count);
		{
			// 상세 보기 버튼을 그리드 레이아웃에 추가 // Add the details button to the grid layout.
			m_pGridButtons->AddCtrl((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, EControl_Button, L"See Details");
			// "See Details" 버튼 클릭 이벤트 처리기 생성 // Create the click event handler for the "See Details" button.
			CWndCtrlProcedure* pVCP = new CWndCtrlProcedure;
			// "See Details" 버튼 클릭 시 수행되는 내용 정의 // Define the action to be performed when the "See Details" button is clicked.
			*pVCP = MakeWndCtrlProcedure
			{
				// "See Details" 버튼을 홀수 번 누른 경우 // If the "See Details" button has been clicked an odd number of times.
				if(((CGUIButton*)m_pGridButtons->GetCtrl((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button))->GetClickCount() % 2 == 1)
				{
					// 시트가 포함된 레이아웃을 show // Show the layout containing the sheet.
					m_pLayoutForSheet->SetVisible(true);
					// "See Details" 버튼의 텍스트를 "Close Details" 로 변경 // Change the text of the "See Details" button to "Close Details".
					m_pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, L"Close Details");
				}
				// "See Details" 버튼을 짝수 번 누른 경우 // If the "See Details" button has been clicked an even number of times.
				else
				{
					// 시트가 포함된 레이아웃을 hide // Hide the layout containing the sheet.
					m_pLayoutForSheet->SetVisible(false);
					// "Close Details" 버튼의 텍스트를 "See Details" 로 변경 // Change the text of the "Close Details" button to "See Details".
					m_pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, L"See Details");
				}

				// 다이얼로그의 레이아웃을 업데이트 // Adjust the dialog layout.
				m_pDlgProgress->AdjustLayout();

				return true;
			};
			// "See Details" 버튼에 클릭 이벤트 처리기 추가 // Add a click event handler to the "See Details" button.
			m_pGridButtons->SetWndCtrlProcedure((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, pVCP);

			// 중지 버튼을 그리드 레이아웃에 추가 // Add the stop button to the grid layout.
			m_pGridButtons->AddCtrl((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button, EControl_Button, L"Stop");
			// 중지 버튼 객체 포인터를 얻어 오기 // Get a pointer to the stop button object.
			m_pStopButton = (CGUIButton*)m_pGridButtons->GetCtrl((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button);
		}

		// CFLSheet 가 포함된 박스 레이아웃 // Box layout containing a CFLSheet.
		if(bBoxLayout)
		{
			m_pLayoutForSheet = new CGUIBoxLayout(EOrientation_Vertical, 300, 500, true);

			// 시트 추가 // Add the sheet.
			((CGUIBoxLayout*)m_pLayoutForSheet)->AddCtrl(EControl_Sheet);
			((CGUIBoxLayout*)m_pLayoutForSheet)->AddCtrl(EControl_Sheet);
		}
		else
		{
			m_pLayoutForSheet = new CGUIGridLayout(14, 2, 300, 500, true);

			((CGUIGridLayout*)m_pLayoutForSheet)->AddCtrl(0, 0, L"Detail 1");
			((CGUIGridLayout*)m_pLayoutForSheet)->AddCtrl(1, 0, L"Detail 2");
			((CGUIGridLayout*)m_pLayoutForSheet)->AddCtrl(2, 0, L"Detail 3");
			((CGUIGridLayout*)m_pLayoutForSheet)->AddCtrl(0, 1, EControl_EditCtrl);
			((CGUIGridLayout*)m_pLayoutForSheet)->AddCtrl(1, 1, EControl_EditCtrl);
			((CGUIGridLayout*)m_pLayoutForSheet)->AddCtrl(2, 1, EControl_EditCtrl);

			// 시트 추가 // Add the sheet.
			((CGUIGridLayout*)m_pLayoutForSheet)->AddCtrl(3, 0, 6, 2, EControl_Sheet);
			((CGUIGridLayout*)m_pLayoutForSheet)->AddCtrl(9, 0, 5, 2, EControl_Sheet);
		}

		{
			// 0 번째 시트 // Sheet at index 0
			{
				// 추가한 시트 얻어 오기 // Get the added sheet
				CGUISheetInterface* pSheetInterface = nullptr;

				if(bBoxLayout)
					pSheetInterface = ((CGUIBoxLayout*)m_pLayoutForSheet)->GetSheetInterface(0);
				else
					pSheetInterface = ((CGUIGridLayout*)m_pLayoutForSheet)->GetSheetInterface(3, 0);

				// 시트의 데이터 포인터 얻어 오기 // Get the data pointer of the sheet
				CFLArray<CFLArray<CFLString<wchar_t>>>* pData = pSheetInterface->GetDataPtr();

				int32_t i32ColCnt = 10; // column 개수 // Number of columns
				int32_t i32RowCnt = 10; // row 개수 // Number of rows
				int32_t i32FixedColCnt = 1; // 고정 column 개수 // Number of fixed columns
				int32_t i32FixedRowCnt = 1; // 고정 row 개수 // Number of fixed rows

				// 시트에 디스플레이 할 내용 추가 // Add content to be displayed in the sheet
				for(int32_t r = 0; r < i32FixedRowCnt; r++)
				{
					pData->Append(CFLArray<CFLString<wchar_t>>());
					CFLArray<CFLString<wchar_t>>& arrRow = pData->Back();

					for(int32_t c = 0; c < i32ColCnt; c++)
						arrRow.Append(CFLString<wchar_t>().Format(L"Fixed (%d,%d)", r, c));
				}

				for(int32_t r = i32FixedRowCnt; r < i32RowCnt; r++)
				{
					pData->Append(CFLArray<CFLString<wchar_t>>());
					CFLArray<CFLString<wchar_t>>& arrRow = pData->Back();

					for(int32_t c = 0; c < i32FixedColCnt; c++)
						arrRow.Append(CFLString<wchar_t>().Format(L"Fixed (%d,%d)", r, c));

					for(int32_t c = i32FixedColCnt; c < i32ColCnt; c++)
						arrRow.Append(CFLString<wchar_t>().Format(L"(%d,%d)", r, c));
				}

				// 데이터에 추가한 내용을 시트에 업데이트 // Update the sheet with the added data
				pSheetInterface->UpdateSheetData();

				// 고정 column, row 개수 설정 // Set the number of fixed columns and rows
				pSheetInterface->SetFixedRowCount(i32FixedRowCnt);
				pSheetInterface->SetFixedColumnCount(i32FixedColCnt);
			}

			// 1 번째 시트 // Sheet at index 1
			{
				// 추가한 시트 얻어 오기 // Get the added sheet
				CGUISheetInterface* pSheetInterface = nullptr;

				if(bBoxLayout)
					pSheetInterface = ((CGUIBoxLayout*)m_pLayoutForSheet)->GetSheetInterface(1);
				else
					pSheetInterface = ((CGUIGridLayout*)m_pLayoutForSheet)->GetSheetInterface(9, 0);

				// 시트의 데이터 포인터 얻어 오기 // Get the data pointer of the sheet
				CFLArray<CFLArray<CFLString<wchar_t>>>* pData = pSheetInterface->GetDataPtr();

				int32_t i32ColCnt = 10; // column 개수 // Number of columns
				int32_t i32RowCnt = 10; // row 개수 // Number of rows
				int32_t i32FixedColCnt = 1; // 고정 column 개수 // Number of fixed columns
				int32_t i32FixedRowCnt = 1; // 고정 row 개수 // Number of fixed rows

				// 시트에 디스플레이 할 내용 추가 // Add content to be displayed in the sheet
				for(int32_t r = 0; r < i32FixedRowCnt; r++)
				{
					pData->Append(CFLArray<CFLString<wchar_t>>());
					CFLArray<CFLString<wchar_t>>& arrRow = pData->Back();

					for(int32_t c = 0; c < i32ColCnt; c++)
						arrRow.Append(CFLString<wchar_t>().Format(L"Fixed (%d,%d)", r, c));
				}

				for(int32_t r = i32FixedRowCnt; r < i32RowCnt; r++)
				{
					pData->Append(CFLArray<CFLString<wchar_t>>());
					CFLArray<CFLString<wchar_t>>& arrRow = pData->Back();

					for(int32_t c = 0; c < i32FixedColCnt; c++)
						arrRow.Append(CFLString<wchar_t>().Format(L"Fixed (%d,%d)", r, c));

					for(int32_t c = i32FixedColCnt; c < i32ColCnt; c++)
						arrRow.Append(CFLString<wchar_t>().Format(L"(%d,%d)", r, c));
				}

				// 데이터에 추가한 내용을 시트에 업데이트 // Update the sheet with the added data
				pSheetInterface->UpdateSheetData();

				// 고정 column, row 개수 설정 // Set the number of fixed columns and rows
				pSheetInterface->SetFixedRowCount(i32FixedRowCnt);
				pSheetInterface->SetFixedColumnCount(i32FixedColCnt);
			}
		}

		// 레이아웃들을 배치하기 위한 배경 레이아웃 생성 // Create a background layout for arranging the layouts.
		CGUIGridLayout* pGridBackground = new CGUIGridLayout((int32_t)ERowLayout::Count, (int32_t)EColLayout::Count);

		// 메인 그리드 레이아웃을 ERowLayout::Main, EColLayout::Main 위치에서 row span = 4, col span = 1 을 차지하도록 배치
		// Place the main grid layout at ERowLayout::Main and EColLayout::Main, occupying a row span of 4 and a column span of 1.
		pGridBackground->Add((int32_t)ERowLayout::Main, (int32_t)EColLayout::Main, 4, 1, m_pGridMain);
		// 버튼 그리드 레이아웃을 ERowLayout::Button, EColLayout::Main 위치에서 row span = 1, col span = 1 을 차지하도록 배치
		// Place the button grid layout at (ERowLayout::Button, EColLayout::Main) with a row span of 1 and a column span of 1.
		pGridBackground->Add((int32_t)ERowLayout::Button, (int32_t)EColLayout::Button, m_pGridButtons);
		// 메인 그리드 레이아웃을 ERowLayout::Main, EColLayout::Main 위치에서 row span = 5, col span = 1 을 차지하도록 배치
		// Place the main grid layout at (ERowLayout::Main, EColLayout::Main) with a row span of 5 and a column span of 1.
		pGridBackground->Add((int32_t)ERowLayout::Details, (int32_t)EColLayout::Details, 5, 1, m_pLayoutForSheet);
		// 상세 정보 레이아웃 숨기기 처리 // Hide the detailed information layout.
		m_pLayoutForSheet->SetVisible(false);

		// 다이얼로그에 배경 레이아웃 추가 // Add the background layout to the dialog.
		m_pDlgProgress->Add(pGridBackground);

		// 다이얼로그 창의 크기가 변경될 때 어느 위치를 기준으로 창을 확대 또는 축소할지 기준 위치를 정합니다.
		// Set the anchor position used when resizing the dialog window.
		m_pDlgProgress->SetResizeWindowPivot(eResizePivot);
		// 다이얼로그 창의 크기가 변경될 때 변경되기 전 창의 너비가 더 크면, 창의 너비를 변경하지 않고 이전 너비를 유지할지 여부 설정
		// Set whether to keep the previous width when the dialog window is resized and the previous window width is greater than the new width.
		m_pDlgProgress->KeepPreviousMaximumWidth(bKeepMaxWidth);
		// 다이얼로그 창의 크기가 변경될 때 변경되기 전 창의 높이가 더 크면, 창의 높이를 변경하지 않고 이전 높이를 유지할지 여부 설정
		// Set whether to preserve the previous height if it is greater than the new height when the dialog window is resized.
		m_pDlgProgress->KeepPreviousMaximumHeight(bKeepMaxHeight);
		// 다이얼로그 창 생성 // Create the dialog window.
		m_pDlgProgress->OnInitDialog();

		// 작업 스레드 생성 // Create a worker thread.
		m_pThread = new std::future<void>(std::async(CPropertyMenuDialogProgressExample::AlgorithmThreadForComplexDialog, this, &m_pThread));
		SetThreadPriority(m_pThread, THREAD_PRIORITY_LOWEST);

		// 스레드 파라미터 초기화 // Reset the thread parameters.
		ResetThreadParams();

		// 작업 스레드에서 업데이트 한 값을 반영하여 다이얼로그 창 업데이트 // Update the dialog window to reflect the values updated by the worker thread.
		UpdateComplexDialogWithSheet();

		// 다이얼로그 닫기 // Close the dialog.
		CloseProgressDialog();

		cr = EResult_OK;
	}
	while(false);

	if(m_pThread)
	{
		delete m_pThread;
		m_pThread = nullptr;
	}

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::UpdateComplexDialogWithSheet()
{
	CResult cr;

	do
	{
		CFLString<wchar_t> strMessage;
		CFLString<wchar_t> strMessageFormat =
			CFLString<wchar_t>(L"[Step 3/3] Learning...\n\n") +
			CFLString<wchar_t>(L"<Cost>\n") +
			CFLString<wchar_t>(L"0.012132\n\n") +
			CFLString<wchar_t>(L"<Iteration>\n") +
			CFLString<wchar_t>(L"(%d/%d)\n\n") + // m_i32CurrentProgress/m_i32TotalProgress
			CFLString<wchar_t>(L"<Epoch>\n") +
			CFLString<wchar_t>(L"1/100\n\n") +
			CFLString<wchar_t>(L"<Validation>\n") +
			CFLString<wchar_t>(L"96.12%%at 1 epoch\n\n\n");

		while(true)
		{
			// 스레드가 완료되면 break // Break if the thread has completed.
			if(m_bThreadDone)
				break;

			// 중지 버튼 클릭 횟수가 변경된 경우 // If the stop button click count has changed.
			if(m_pStopButton->GetClickCount() != m_i32BtnStopClickCount)
			{
				// 중지 버튼을 한 번 클릭한 경우 // If the stop button has been clicked once.
				if(m_pStopButton->GetClickCount() == 1)
				{
					// 중지 버튼 내 문자열을 "Continue.." 로 설정 // Set the text of the stop button to "Continue...".
					m_pGridButtons->SetCtrlValue((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button, L"Continue ..");
				}
				// 중지 버튼을 두 번 클릭한 경우 ("Continue.." 버튼을 클릭한 경우) // If the stop button has been clicked twice ("Continue..." button clicked).
				else if(m_pStopButton->GetClickCount() == 2)
				{
					// 중지 버튼 숨기기 // Hide the stop button.
					m_pGridButtons->SetCtrlVisible((int32_t)ERowButtonComplex::Stop, (int32_t)EColButton::Button, false);
				}

				// "Details" 버튼 숨기기 // Hide the "Details" button.
				m_pGridButtons->SetCtrlVisible((int32_t)ERowButtonComplex::Details, (int32_t)EColButton::Button, false);
				// Sheet 가 포함된 레이아웃 숨기기 // Hide the layout containing the sheet
				m_pLayoutForSheet->SetVisible(false);
				// 중지 버튼 클릭 횟수 업데이트 // Update the stop button click count.
				m_i32BtnStopClickCount = m_pStopButton->GetClickCount();
				// 다이얼로그의 레이아웃 업데이트 // Adjust the dialog layout.
				m_pDlgProgress->AdjustLayout();
			}

			// 중지 버튼을 한 번 클릭한 경우 // If the stop button has been clicked once.
			if(m_pStopButton->GetClickCount() == 1)
			{
				// 메세지 설정 // Set the message.
				strMessage.Format(L"[Step 3/3] Temp Message...\n\n<Temp Progress>");

				// m_pGridMain 의 ERowMainComplex::Message, EColMainComplex::Message 위치에 문자열 값 설정하여 다이얼로그 상에서 보이는 메세지 설정
				// Set the message displayed in the dialog by assigning a string value to (ERowMainComplex::Message, EColMainComplex::Message) in m_pGridMain.
				m_pGridMain->SetCtrlValue((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, strMessage);

				// 프로그레스 바가 100% 로 채워지도록 설정 // Set the progress bar to 100%
				m_pProgressCtrlFP->SetPos(1.);
				// 다이얼로그 화면 갱신 // Invalidate the dialog.
				m_pDlgProgress->Invalidate();
				// 메시지 펌프 // Message pump
				CGUIManager::PeekAndPump();
				Sleep(1);
				continue;
			}
			// 중지 버튼을 두 번 클릭한 경우 // If the stop button has been clicked twice.
			else if(m_pStopButton->GetClickCount() == 2)
			{
				// 중지 프로세스 진행이 아직 완료되지 않은 경우 // If the stop process has not yet completed.
				if(m_i32CurrentStopProgress <= m_i32TotalStopProgress)
				{
					// 메세지 설정 // Set the message.
					strMessage.Format(L"[Step 3/3] Learning is being stopped...\n\n<Stop Progress>\n(%d/%d)", m_i32CurrentStopProgress, m_i32TotalStopProgress);
					m_pGridMain->SetCtrlValue((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, strMessage);

					// 프로그레스 바에 진행률 설정 // Set the progress on the progress bar.
					m_pProgressCtrlFP->SetPos((double)m_i32CurrentStopProgress / (double)m_i32TotalStopProgress);

					// 다이얼로그 화면 갱신 // Invalidate the dialog.
					m_pDlgProgress->Invalidate();
				}
				else // 중지 프로세스 진행이 아직 완료된 경우 while 문에서 break // Break out of the loop if the stop process has completed.
					break;

				// 메시지 펌프 // Message pump
				CGUIManager::PeekAndPump();
				Sleep(1);
				continue;
			}

			// 중지 버튼을 한 번도 누르지 않은 경우 // If the stop button has not been clicked.

			// 메세지 설정 // Set the message.
			strMessage.Format
			(
				strMessageFormat,
				m_i32CurrentProgress, m_i32TotalProgress
			);
			m_pGridMain->SetCtrlValue((int32_t)ERowMainComplex::Message, (int32_t)EColMainComplex::Message, strMessage);

			// 프로그레스 바에 진행률 설정 // Set the progress on the progress bar.
			m_pProgressCtrlFP->SetPos((double)m_i32CurrentProgress / (double)m_i32TotalProgress);

			// 다이얼로그 화면 갱신 // Invalidate the dialog.
			m_pDlgProgress->Invalidate();
			// 메시지 펌프 // Message pump
			CGUIManager::PeekAndPump();
			Sleep(1);
		}

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuDialogProgressExample::CloseProgressDialog()
{
	CResult cr;

	do
	{
		if(m_pDlgProgress)
		{
			m_pDlgProgress->DestroyWindow();
			m_pDlgProgress = nullptr;
		}

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

void FLImaging::GUI::CPropertyMenuDialogProgressExample::AlgorithmThreadForComplexDialog(CPropertyMenuDialogProgressExample* pInstance, std::future<void>** pContext)
{
	// 복잡한 다이얼로그에서 사용되는 스레드 // Thread used by the complex dialog

	if(pInstance->m_pDlgProgress)
	{
		while(true)
		{
			// 현재 종료 작업 진행량이 전체 종료 작업 진행량 이상이 된 경우 // If the current shutdown progress reaches or exceeds the total shutdown progress
			if(pInstance->m_i32CurrentStopProgress >= pInstance->m_i32TotalStopProgress)
			{
				// 스레드가 종료되었음을 알리는 플래그를 true 로 설정 후 break // Set the thread termination flag to true and break
				pInstance->m_bThreadDone = true;
				break;
			}

			// 버튼을 2회 이상 클릭한 경우 // If the button has been clicked two or more times
			if(pInstance->m_pStopButton->GetClickCount() >= 2)
			{
				Sleep(1);
				// 중지 작업 진행량 값을 증가시킴 // Increase the shutdown progress value
				++pInstance->m_i32CurrentStopProgress;
			}
			else
			{
				// 현재 작업 진행량이 전체 작업 진행량 이상이 된 경우 // If the current progress reaches or exceeds the total progress
				if(pInstance->m_i32CurrentProgress >= pInstance->m_i32TotalProgress)
				{
					// 스레드가 종료되었음을 알리는 플래그를 true 로 설정 후 break // Set the thread termination flag to true and break
					pInstance->m_bThreadDone = true;
					break;
				}

				Sleep(1);
				// 작업 진행량의 값을 증가시킴 // Increment the progress value
				++pInstance->m_i32CurrentProgress;
			}
		}
	}
}

void FLImaging::GUI::CPropertyMenuDialogProgressExample::AlgorithmThreadForSimpleDialog(CPropertyMenuDialogProgressExample* pInstance, std::future<void>** pContext)
{
	// 단순한 다이얼로그에서 사용되는 스레드 // Thread used by the simple dialog

	if(pInstance && pInstance->m_pDlgProgress)
	{
		while(true)
		{
			// 버튼을 1회 이상 클릭한 경우 // If the button has been clicked one or more times
			if(pInstance->m_pStopButton->GetClickCount() >= 1)
			{
				// 스레드가 종료되었음을 알리는 플래그를 true 로 설정 후 break // Set the thread termination flag to true and break
				pInstance->m_bThreadDone = true;
				break;
			}
			else
			{
				// 현재 작업 진행량이 전체 작업 진행량 이상이 된 경우 // If the current progress reaches or exceeds the total progress
				if(pInstance->m_i32CurrentProgress >= pInstance->m_i32TotalProgress)
				{
					// 스레드가 종료되었음을 알리는 플래그를 true 로 설정 후 break // Set the thread termination flag to true and break
					pInstance->m_bThreadDone = true;
					break;
				}

				Sleep(1);
				// 작업 진행량의 값을 증가시킴 // Increment the progress value
				++pInstance->m_i32CurrentProgress;
			}
		}
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

void FLImaging::GUI::CPropertyMenuDialogProgressExample::ResetThreadParams()
{
	// 스레드 파라미터 값을 초기화 합니다. // Reset the thread parameter values
	m_bThreadDone = false;
	m_i32CurrentStopProgress = 0;
	m_i32CurrentProgress = 0;
	m_i32TotalProgress = 2000;
	m_i32TotalStopProgress = 20;
	m_i32BtnStopClickCount = 0;
}
