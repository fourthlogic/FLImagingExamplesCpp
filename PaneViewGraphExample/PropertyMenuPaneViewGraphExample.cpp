#include "stdafx.h"
#include "PropertyMenuPaneViewGraphExample.h"

using namespace FLImaging;
using namespace FLImaging::Base;
using namespace FLImaging::GUI;
using namespace FLImaging::Foundation;

CPropertyMenuPaneViewGraphExample::CPropertyMenuPaneViewGraphExample()
{
}

CPropertyMenuPaneViewGraphExample::~CPropertyMenuPaneViewGraphExample()
{
}

const CResult CPropertyMenuPaneViewGraphExample::ConfigureMenu()
{
	CResult cReturn = EResult_UnknownError;

	do
	{
		// Enable the framework's status message box.
		// 프레임워크의 상태 메시지 창을 활성화합니다.
		EnableStatusMessageBox(true);
		// Set the message display mode to automatically scroll to the latest message.
		// 메시지 표시 모드를 최신 메시지로 자동 스크롤되도록 설정합니다.
		SetMessageDisplayMode(EMessageDisplayMode_AutoScroll);

		{
			CGUIPropertyItemButton* pBtn = nullptr;

			// Add a property button item and bind its click event procedure.
			// 프로퍼티 버튼 항목을 추가하고 클릭 이벤트 프로시저를 바인딩합니다.
			pBtn = AddItem<CGUIPropertyItemButton>(L"Create PaneViewGraph");
			pBtn->SetPropertyItemButtonClickProcedure(CPropertyMenuPaneViewGraphExample::MakeButton1Implement());

			pBtn = AddItem<CGUIPropertyItemButton>(L"Show PaneViewGraph");
			pBtn->SetPropertyItemButtonClickProcedure(CPropertyMenuPaneViewGraphExample::MakeButton2Implement());

			pBtn = AddItem<CGUIPropertyItemButton>(L"Destroy PaneViewGraph");
			pBtn->SetPropertyItemButtonClickProcedure(CPropertyMenuPaneViewGraphExample::MakeButton3Implement());

			pBtn = AddItem<CGUIPropertyItemButton>(L"Rename PaneViewGraph");
			pBtn->SetPropertyItemButtonClickProcedure(CPropertyMenuPaneViewGraphExample::MakeButton4Implement());
		}

		cReturn = EResult_OK;
	}
	while(false);

	return cReturn;
}

CPropertyItemButtonClickProcedure* FLImaging::GUI::CPropertyMenuPaneViewGraphExample::MakeButton1Implement()
{
	// Create a procedure to handle the button click event asynchronously.
	// 버튼 클릭 이벤트를 비동기적으로 처리하기 위한 프로시저를 생성합니다.
	CPropertyItemButtonClickProcedure* pProcedure = new CPropertyItemButtonClickProcedure;
	*pProcedure = MakePropertyItemButtonClickProcedure
	{
		// Request the pane manager to create a new PaneViewGraph window.
		// 패널 관리자에게 새로운 PaneViewGraph 창을 생성하도록 요청합니다.
		CGUIManagerPane::CreatePaneViewGraph();
	};

	return pProcedure;
}

CPropertyItemButtonClickProcedure* FLImaging::GUI::CPropertyMenuPaneViewGraphExample::MakeButton2Implement()
{
	CPropertyItemButtonClickProcedure* pProcedure = new CPropertyItemButtonClickProcedure;
	*pProcedure = MakePropertyItemButtonClickProcedure
	{
		// Create a dynamic custom dialog and set its window title.
		// 동적 커스텀 대화 상자를 생성하고 창 제목을 설정합니다.
		CGUIDialog dlg;
		dlg.SetTitle(L"Show Pane View Graph");

		// Create a grid layout with 2 rows and 2 columns to organize controls.
		// 컨트롤들을 배치하기 위해 2행 2열 구조의 그리드 레이아웃을 생성합니다.
		CGUIGridLayout* pGrid1 = new CGUIGridLayout(2, 2);

		// Add a static text label at row 0, column 0.
		// 0행 0열에 정적 텍스트 레이블을 추가합니다.
		pGrid1->AddCtrl(0, 0, 1, 1, EControl_StaticText, L"Select Pane By:");
		// Add a ComboBox at row 0, column 1.
		// 0행 1열에 콤보 박스를 추가합니다.
		pGrid1->AddCtrl(0, 1, 1, 1, EControl_ComboBox);
		// Populate the ComboBox items.
		// 콤보 박스에 선택 항목들을 추가합니다.
		pGrid1->AddComboBoxItem(0, 1, L"Index");
		pGrid1->AddComboBoxItem(0, 1, L"Name");
		// Set the default selection to "Index".
		// 기본 선택 값을 "Index"로 설정합니다.
		pGrid1->SetCtrlValue(0, 1, L"Index");

		// Register an interactive control procedure that triggers when the ComboBox selection changes.
		// 콤보 박스의 선택 항목이 변경될 때 트리거되는 상호작용 제어 프로시저를 등록합니다.
		CWndCtrlProcedure* pProc = new CWndCtrlProcedure;
		*pProc = MakeWndCtrlProcedure
		{
			// Dynamically update the label text at row 1, column 0 based on the ComboBox choice.
			// 콤보 박스 선택 결과에 따라 1행 0열의 레이블 텍스트를 동적으로 업데이트합니다.
			pGrid1->SetCtrlValue(1, 0, (strValue == L"0") ? L"Index" : L"Name", true);
			return true;
		};
		pGrid1->SetWndCtrlProcedure(0, 1, pProc);

		// Initialize row 1 controls: a label and a text edit control.
		// 1행 컨트롤을 초기화합니다: 레이블과 텍스트 입력창.
		pGrid1->AddCtrl(1, 0, L"Index");
		pGrid1->AddCtrl(1, 1, EControl_EditCtrl);

		// Add the constructed grid layout into the dialog.
		// 구성된 그리드 레이아웃을 대화 상자에 추가합니다.
		dlg.Add(pGrid1);

		// Display the dialog as a modal window and process the result if the user clicks OK.
		// 대화 상자를 모달 창으로 띄우고 사용자가 OK를 클릭하면 결과를 처리합니다.
		if(dlg.DoModal() == IDOK)
		{
			// Check if the user opted to select the pane by index ("0").
			// 사용자가 인덱스 방식("0")으로 패널을 선택했는지 확인합니다.
			bool bSelectByIndex = pGrid1->GetCtrlValue(0, 1) == L"0";

			if(bSelectByIndex)
				// Activate and display the PaneViewGraph matching the specified index.
				// 지정된 인덱스와 일치하는 PaneViewGraph를 활성화하여 화면에 표시합니다.
				CGUIManagerPane::ShowPaneViewGraph(true, pGrid1->GetCtrlValue(1, 1).ToInt32());
			else
			{
				std::vector<CGUIPaneBase*> vctPaneViewGraph;
				// Retrieve all pane instances matching the requested name.
				// 요청된 이름과 일치하는 모든 패널 인스턴스를 검색합니다.
				CGUIManagerPane::FindPanesByName(pGrid1->GetCtrlValue(1, 1), vctPaneViewGraph);

				for(auto& pPvg : vctPaneViewGraph)
					// Safely cast to CGUIPaneViewGraph and display the pane.
					// CGUIPaneViewGraph 타입으로 안전하게 다운캐스팅한 후 패널을 화면에 표시합니다.
					CGUIManagerPane::ShowPaneViewGraph(true, dynamic_cast<CGUIPaneViewGraph*>(pPvg));
			}
		}
	};

	return pProcedure;
}

CPropertyItemButtonClickProcedure* FLImaging::GUI::CPropertyMenuPaneViewGraphExample::MakeButton3Implement()
{
	CPropertyItemButtonClickProcedure* pProcedure = new CPropertyItemButtonClickProcedure;
	*pProcedure = MakePropertyItemButtonClickProcedure
	{
		CGUIDialog dlg;
		dlg.SetTitle(L"Destroy Pane View Graph");

		CGUIGridLayout* pGrid1 = new CGUIGridLayout(2, 2);

		pGrid1->AddCtrl(0, 0, 1, 1, EControl_StaticText, L"Select Pane By:");
		pGrid1->AddCtrl(0, 1, 1, 1, EControl_ComboBox);
		pGrid1->AddComboBoxItem(0, 1, L"Index");
		pGrid1->AddComboBoxItem(0, 1, L"Name");
		pGrid1->SetCtrlValue(0, 1, L"Index");
		CWndCtrlProcedure* pProc = new CWndCtrlProcedure;
		*pProc = MakeWndCtrlProcedure
		{
			pGrid1->SetCtrlValue(1, 0, (strValue == L"0") ? L"Index" : L"Name", true);
			return true;
		};
		pGrid1->SetWndCtrlProcedure(0, 1, pProc);

		pGrid1->AddCtrl(1, 0, L"Index");
		pGrid1->AddCtrl(1, 1, EControl_EditCtrl);

		dlg.Add(pGrid1);

		if(dlg.DoModal() == IDOK)
		{
			bool bSelectByIndex = pGrid1->GetCtrlValue(0, 1) == L"0";

			if(bSelectByIndex)
				// Permanently destroy the PaneViewGraph at the specified index.
				// 지정된 인덱스의 PaneViewGraph를 완전히 소멸시킵니다.
				CGUIManagerPane::DestroyPaneViewGraph(pGrid1->GetCtrlValue(1, 1).ToInt32());
			else
			{
				std::vector<CGUIPaneBase*> vctPaneViewGraph;
				// Retrieve all pane instances matching the specified name for destruction.
				// 소멸시킬 목적으로 지정된 이름과 일치하는 모든 패널 인스턴스를 검색합니다.
				CGUIManagerPane::FindPanesByName(pGrid1->GetCtrlValue(1, 1), vctPaneViewGraph);

				for(auto& pPvg : vctPaneViewGraph)
					// Safely cast to CGUIPaneViewGraph and destroy the pane instance.
					// CGUIPaneViewGraph 타입으로 안전하게 다운캐스팅한 후 패널 인스턴스를 소멸시킵니다.
					CGUIManagerPane::DestroyPaneViewGraph(dynamic_cast<CGUIPaneViewGraph*>(pPvg));
			}
		}
	};

	return pProcedure;
}

CPropertyItemButtonClickProcedure* FLImaging::GUI::CPropertyMenuPaneViewGraphExample::MakeButton4Implement()
{
	CPropertyItemButtonClickProcedure* pProcedure = new CPropertyItemButtonClickProcedure;
	*pProcedure = MakePropertyItemButtonClickProcedure
	{
		CGUIDialog dlg;
		dlg.SetTitle(L"Set Name of Pane View Graph");

		CGUIGridLayout* pGrid1 = new CGUIGridLayout(2, 2);

		pGrid1->AddCtrl(0, 0, L"PaneViewGraph Index");
		pGrid1->AddCtrl(0, 1, EControl_EditCtrl);

		pGrid1->AddCtrl(1, 0, L"Name");
		pGrid1->AddCtrl(1, 1, EControl_EditCtrl);

		dlg.Add(pGrid1);

		if(dlg.DoModal() == IDOK)
		{
			// Fetch the specific PaneViewGraph pointer using the provided index.
			// 제공된 인덱스를 사용하여 특정 PaneViewGraph의 포인터를 가져옵니다.
			CGUIPaneViewGraph* pPaneViewGraph = CGUIManagerPane::GetPaneViewGraph(pGrid1->GetCtrlValue(0, 1).ToInt32());

			if(pPaneViewGraph)
				// Update the title or identifier name of the selected pane.
				// 선택된 패널의 타이틀 및 식별 이름을 새 이름으로 변경합니다.
				pPaneViewGraph->SetName(pGrid1->GetCtrlValue(1, 1));
		}
	};

	return pProcedure;
}