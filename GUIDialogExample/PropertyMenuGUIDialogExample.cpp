#include "stdafx.h"
#include "PropertyMenuGUIDialogExample.h"

using namespace FLImaging;
using namespace FLImaging::Base;
using namespace FLImaging::GUI;
using namespace FLImaging::Foundation;

CPropertyMenuGUIDialogExample::CPropertyMenuGUIDialogExample()
{
	m_vctLogicalCondition.push_back(L"Greater");
	m_vctLogicalCondition.push_back(L"Less");
}

CPropertyMenuGUIDialogExample::~CPropertyMenuGUIDialogExample()
{
}

const CResult CPropertyMenuGUIDialogExample::ConfigureMenu()
{
	CResult cReturn = EResult_UnknownError;

	do
	{
		CGUIPropertyButton* pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Create ModelessDialog");
		pBtn->SetPropertyButtonClickProcedure(MakeButton1Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);
		SetShortcut(EKeyType_F6, pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Create FloatingPointSlider Dialog");
		pBtn->SetPropertyButtonClickProcedure(MakeButton2Implement());
		pBtn->SetButtonWidthRatio(1.);
		SetShortcut(EKeyType_F7, pBtn);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Complex Layout");
		pBtn->SetPropertyButtonClickProcedure(MakeButton3Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Sized Complex Layout");
		pBtn->SetPropertyButtonClickProcedure(MakeButton4Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Box Layout Vertical");
		pBtn->SetPropertyButtonClickProcedure(MakeButton5Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Box Layout Horizontal");
		pBtn->SetPropertyButtonClickProcedure(MakeButton6Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Grid Layout Like Vertical Box");
		pBtn->SetPropertyButtonClickProcedure(MakeButton7Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Grid Layout Like Horizontal Box");
		pBtn->SetPropertyButtonClickProcedure(MakeButton8Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Sized Box Layout Vertical");
		pBtn->SetPropertyButtonClickProcedure(MakeButton9Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Sized Box Layout Horizontal");
		pBtn->SetPropertyButtonClickProcedure(MakeButton10Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Sized Grid Layout Like Vertical Box");
		pBtn->SetPropertyButtonClickProcedure(MakeButton11Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Sized Grid Layout Like Horizontal Box");
		pBtn->SetPropertyButtonClickProcedure(MakeButton12Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Complex Layout 2");
		pBtn->SetPropertyButtonClickProcedure(MakeButton13Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Complex Layout 3");
		pBtn->SetPropertyButtonClickProcedure(MakeButton15Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"BoxLayout Static Alignment");
		pBtn->SetPropertyButtonClickProcedure(MakeButton16Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"GridLayout Static Alignment");
		pBtn->SetPropertyButtonClickProcedure(MakeButton17Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"ListCtrl Dialog1");
		pBtn->SetPropertyButtonClickProcedure(MakeButton18Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"ListCtrl Dialog2");
		pBtn->SetPropertyButtonClickProcedure(MakeButton19Implement());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		CGUIPropertyItemCategory* pCat = new CGUIPropertyItemCategory;
		pCat->SetName(L"Parameters");
		AddItem(pCat);

		pCat = new CGUIPropertyItemCategory;
		pCat->SetName(L"Fundamental");
		pCat->SetPath(L"Parameters");
		AddItem(pCat);

		CGUIPropertyItemDropdownList* pThreshMode = new CGUIPropertyItemDropdownList;
		pThreshMode->SetName(L"Threshold Mode");
		pThreshMode->SetPath(L"Parameters@Fundamental");
		pThreshMode->AddListItem(L"Single");
		pThreshMode->AddListItem(L"Dual And");
		pThreshMode->AddListItem(L"Dual Or");
		pThreshMode->SetDefaultValue(L"Single");
		AddItem(pThreshMode);

		CGUIPropertyItemDropdownList* pChannelCondition = new CGUIPropertyItemDropdownList;
		pChannelCondition->SetName(L"Logical Condition of Channels");
		pChannelCondition->SetPath(L"Parameters@Fundamental");
		pChannelCondition->AddListItem(L"And");
		pChannelCondition->AddListItem(L"Or");
		pChannelCondition->SetDefaultValue(L"And");
		AddItem(pChannelCondition);

		CGUIPropertyItemCheckBox* pAbsolMode = new CGUIPropertyItemCheckBox;
		pAbsolMode->SetName(L"Absolute Mode");
		pAbsolMode->SetPath(L"Parameters@Fundamental");
		AddItem(pAbsolMode);

		cReturn = EResult_OK;
	}
	while(false);

	return cReturn;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton1Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		if(!m_pModelessDlg)
		{
			m_pModelessDlg = CGUIDialog::CreateModelessDialog();
			m_pModelessDlg->SetTitle(L"Threshold (" + GetName() + L")");

			int32_t i32Width = GetSystemMetrics(SM_CXSCREEN) >> 2;
			int32_t i32Height = GetSystemMetrics(SM_CYSCREEN) >> 1;

			int32_t i32StartX = i32Width - 200;
			int32_t i32StartY = i32Height - 300;

			if(i32Width < 200)
			{
				int32_t i32NewWidth = (int32_t)(i32Width * .75);
				i32StartX = i32Width - (i32NewWidth >> 1);
				i32Width = i32NewWidth;
			}

			if(i32Height < 300)
			{
				int32_t i32NewHeight = (int32_t)(i32Height * .75);
				i32StartY = i32Height - (i32NewHeight >> 1);
				i32Height = i32NewHeight;
			}
			//EControl_Button;

			CGUIGridLayout* pGridLayoutGeometric = new CGUIGridLayout(1, 2, true);
			{
				pGridLayoutGeometric->AddCtrl(0, 0, EControl_StaticText, L"Mode");
				pGridLayoutGeometric->AddCtrl(0, 1, EControl_ComboBox, L"Auto"); // L"Auto" 는 해당 ComboBox 의 default value
				pGridLayoutGeometric->AddComboBoxItem(0, 1, L"Auto");
				pGridLayoutGeometric->AddComboBoxItem(0, 1, L"Absolute");
				pGridLayoutGeometric->AddComboBoxItem(0, 1, L"Relative (%)");
				pGridLayoutGeometric->AddComboBoxItem(0, 1, L"Minimum Residue");
				pGridLayoutGeometric->AddComboBoxItem(0, 1, L"Maximum Entropy");
				pGridLayoutGeometric->AddComboBoxItem(0, 1, L"Iso-Data");
			}
			m_pModelessDlg->Add(pGridLayoutGeometric);

			CGUIGridLayout* m_pGridLayout1 = new CGUIGridLayout(2, 2, true, L"Preview Mode");
			{
				m_pGridLayout1->AddCtrl(0, 0, L"Above or on threshold");
				m_pGridLayout1->AddCtrl(0, 1, EControl_ComboBox, L"White"); // L"White" 는 해당 ComboBox 의 default value
				m_pGridLayout1->AddComboBoxItem(0, 1, L"White");
				m_pGridLayout1->AddComboBoxItem(0, 1, L"Reddish");
				m_pGridLayout1->AddComboBoxItem(0, 1, L"Gray");
				m_pGridLayout1->AddCtrl(1, 0, L"Below threshold");
				m_pGridLayout1->AddCtrl(1, 1, EControl_ComboBox, L"Black"); // L"Black" 은 해당 ComboBox 의 default value
				m_pGridLayout1->AddComboBoxItem(1, 1, L"Black");
				m_pGridLayout1->AddComboBoxItem(1, 1, L"Blush");
				m_pGridLayout1->AddComboBoxItem(1, 1, L"Gray");
			}
			m_pModelessDlg->Add(m_pGridLayout1);

			CGUIBoxLayout* pBoxLayout = new CGUIBoxLayout(EOrientation_Horizontal, true);
			{
				pBoxLayout->AddCtrl(EControl_Slider);
				CGUISlider* pSlider = (CGUISlider*)pBoxLayout->GetCtrl(0);
				pSlider->SetRange(0, 255);

				CWndCtrlProcedure* pSliderHandler = new CWndCtrlProcedure;
				*pSliderHandler = MakeWndCtrlProcedure
				{
					return OnSliderPosChanged(_wtoi(strValue));
				};
				pBoxLayout->SetWndCtrlProcedure(0, pSliderHandler);
			}
			m_pModelessDlg->Add(pBoxLayout);

			m_pModelessDlg->OnInitDialog();
			CGUISlider* pSlider = (CGUISlider*)pBoxLayout->GetCtrl(0);
			pSlider->SetTic(1);
		}
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton2Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		if(!m_pModelessDlg)
		{
			m_pModelessDlg = CGUIDialog::CreateModelessDialog();

			m_pModelessDlg->SetTitle(L"Slider Test");

			// 실수형 슬라이더
			GUI::CGUIGridLayout* pGrid = new CGUIGridLayout(2, 3, true, L"test");

			pGrid->AddCtrl(0, 0, L"test");
			pGrid->AddCtrl(0, 1, 1, 2, EControl_ComboBox, L"test");

			CGUISliderFloatingPoint* pSliderFloatingPoint = new CGUISliderFloatingPoint;

			pSliderFloatingPoint->SetRange(0., 1.);
			pSliderFloatingPoint->SetUnit(0.01); // SliderFloatingPoint 1틱당 증가량
			pSliderFloatingPoint->SetDecimalPlaces(2); // 소수점 자리 표시


			pGrid->AddCtrl(1, 0, L"Val=  ");
			pGrid->AddCtrl(1, 1, 1, 2, EControl_SliderFloatingPoint, L"", pSliderFloatingPoint);

			pGrid->SetCtrlVisible(1, 0, false);
			pGrid->SetCtrlVisible(1, 1, false);

			CGUIBoxLayout* pBox = new CGUIBoxLayout(EOrientation_Horizontal);
			pBox->Add(pGrid);

			m_pModelessDlg->Add(pBox);

			m_pModelessDlg->OnInitDialog();

			pGrid->SetCtrlVisible(1, 0, true);
			pGrid->SetCtrlVisible(1, 1, true);
			m_pModelessDlg->AdjustLayout();
		}
	};
	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton3Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateComplexLayout1();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton4Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateComplexLayout1Sized();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton5Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateBoxLayoutVertical();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton6Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateBoxLayoutHorizontal();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton7Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateGridLayoutLikeVerticalBox();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton8Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateGridLayoutLikeHorizontalBox();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton9Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateSizedBoxLayoutVertical();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton10Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateSizedBoxLayoutHorizontal();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton11Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateSizedGridLayoutLikeVerticalBox();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton12Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateSizedGridLayoutLikeHorizontalBox();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton13Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateComplexLayout2();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton15Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateComplexLayout3();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton16Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateBoxLayoutStaticTextAlignment();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton17Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateGridLayoutStaticTextAlignment();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton18Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateListCtrlDialog1();
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuGUIDialogExample::MakeButton19Implement()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CreateListCtrlDialog2();
	};

	return pProcedure;
}

void FLImaging::GUI::CPropertyMenuGUIDialogExample::OnReceiveBroadcast(const Base::CBroadcastMessage* pMessage)
{
	do
	{
		if(!AfxGetApp() || !AfxGetApp()->GetMainWnd() || !AfxGetApp()->GetMainWnd()->GetSafeHwnd())
			break;

		CGUIPaneProperties* pActiveProp = GetPaneProperties();

		if(!pActiveProp || !pActiveProp->IsVisible() || !pActiveProp->IsWindowVisible())
			break;

		switch(pMessage->GetChannel())
		{
		case EGUIBroadcast_GUIDialog_CloseWindow:
			{
				if(pMessage->GetCaller() != m_pModelessDlg)
					break;

				m_pModelessDlg = nullptr;
			}
			break;
		default:
			break;
		}
	}
	while(false);
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::OnSliderPosChanged(int32_t i32SliderPos)
{
	CResult cr = EResult_UnknownError;

	do
	{
		CLog::Write(L"%d", i32SliderPos);
		cr = EResult_OK;
	}
	while(false);

	return cr;
}


const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateBoxLayoutVertical()
{
	CResult cr;

	do
	{
		if(m_pModelessDlg)
			m_pModelessDlg->DestroyWindow();

		m_pModelessDlg = CGUIDialog::CreateModelessDialog();
		m_pModelessDlg->SetTitle(L"BoxLayout Vertical");

		CGUIBoxLayout* pBoxLayout = new CGUIBoxLayout(EOrientation_Vertical);
		pBoxLayout->AddCtrl(0, 2, EControl_EditCtrl, L"ABC");
		pBoxLayout->AddCtrl(EControl_EditCtrl, L"ABCDEF");
		pBoxLayout->AddCtrl(3, 3, EControl_EditCtrl, L"ABCDEFG");

		m_pModelessDlg->Add(pBoxLayout);
		m_pModelessDlg->OnInitDialog();

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateBoxLayoutHorizontal()
{
	CResult cr;

	do
	{
		if(m_pModelessDlg)
			m_pModelessDlg->DestroyWindow();

		m_pModelessDlg = CGUIDialog::CreateModelessDialog();
		m_pModelessDlg->SetTitle(L"BoxLayout Horizontal");

		CGUIBoxLayout* pBoxLayout = new CGUIBoxLayout(EOrientation_Horizontal);
		pBoxLayout->AddCtrl(0, 2, EControl_EditCtrl, L"ABC");
		pBoxLayout->AddCtrl(EControl_EditCtrl, L"ABCDEF");
		pBoxLayout->AddCtrl(3, 3, EControl_EditCtrl, L"ABCDEFG");

		m_pModelessDlg->Add(pBoxLayout);
		m_pModelessDlg->OnInitDialog();

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateGridLayoutLikeVerticalBox()
{
	CResult cr;

	do
	{
		if(m_pModelessDlg)
			m_pModelessDlg->DestroyWindow();

		m_pModelessDlg = CGUIDialog::CreateModelessDialog();
		m_pModelessDlg->SetTitle(L"Grid Layout Like Vertical Box");

		CGUIGridLayout* pGridLayout = new CGUIGridLayout(6, 1);
		pGridLayout->AddCtrl(0, 0, 2, 1, EControl_EditCtrl, L"ABC");
		pGridLayout->AddCtrl(2, 0, 1, 1, EControl_EditCtrl, L"ABCDEF");
		pGridLayout->AddCtrl(3, 0, 3, 1, EControl_EditCtrl, L"ABCDEFG");

		m_pModelessDlg->Add(pGridLayout);
		m_pModelessDlg->OnInitDialog();

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateGridLayoutLikeHorizontalBox()
{
	CResult cr;

	do
	{
		if(m_pModelessDlg)
			m_pModelessDlg->DestroyWindow();

		m_pModelessDlg = CGUIDialog::CreateModelessDialog();
		m_pModelessDlg->SetTitle(L"Grid Layout Like Horizontal Box");

		CGUIGridLayout* pGridLayout = new CGUIGridLayout(1, 6);
		pGridLayout->AddCtrl(0, 0, 1, 2, EControl_EditCtrl, L"ABC");
		pGridLayout->AddCtrl(0, 2, 1, 1, EControl_EditCtrl, L"ABCDEF");
		pGridLayout->AddCtrl(0, 3, 1, 3, EControl_EditCtrl, L"ABCDEFG");

		m_pModelessDlg->Add(pGridLayout);
		m_pModelessDlg->OnInitDialog();

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateSizedBoxLayoutVertical()
{
	CResult cr;

	do
	{
		CGUIDialog dlg;
		dlg.SetTitle(L"Sized Box Layout Vertical");

		CGUIBoxLayout* pLayout = new CGUIBoxLayout(EOrientation_Vertical, 350, 320);
		pLayout->AddCtrl(L"Figure Name :  ");
		pLayout->AddCtrl(EControl_EditCtrl, L"NAME_FIGURE_");
		pLayout->AddCtrl(L"Figure Template :  ");
		pLayout->AddCtrl(EControl_ComboBox);
		pLayout->AddComboBoxItem(3, L"Int32");
		pLayout->AddComboBoxItem(3, L"Int64");
		pLayout->AddComboBoxItem(3, L"Float");
		pLayout->AddComboBoxItem(3, L"Double");

		dlg.Add(pLayout);

		if(dlg.DoModal() != IDOK)
			break;

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateSizedBoxLayoutHorizontal()
{
	CResult cr;

	do
	{
		CGUIDialog dlg;
		dlg.SetTitle(L"Sized Box Layout Horizontal");

		CGUIBoxLayout* pLayout = new CGUIBoxLayout(EOrientation_Horizontal, 650, 50);
		pLayout->AddCtrl(L"Figure Name :  ");
		pLayout->AddCtrl(EControl_EditCtrl, L"NAME_FIGURE_");
		pLayout->AddCtrl(L"Figure Template :  ");
		pLayout->AddCtrl(EControl_ComboBox);
		pLayout->AddComboBoxItem(3, L"Int32");
		pLayout->AddComboBoxItem(3, L"Int64");
		pLayout->AddComboBoxItem(3, L"Float");
		pLayout->AddComboBoxItem(3, L"Double");

		dlg.Add(pLayout);

		if(dlg.DoModal() != IDOK)
			break;

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateSizedGridLayoutLikeVerticalBox()
{
	CResult cr;

	do
	{
		CGUIDialog dlg;
		dlg.SetTitle(L"Sized Grid Layout Like Vertical Box");

		CGUIGridLayout* pLayout = new CGUIGridLayout(4, 1, 350, 320);
		pLayout->AddCtrl(0, 0, L"Figure Name :  ");
		pLayout->AddCtrl(1, 0, EControl_EditCtrl, L"NAME_FIGURE_");
		pLayout->AddCtrl(2, 0, L"Figure Template :  ");
		pLayout->AddCtrl(3, 0, EControl_ComboBox);
		pLayout->AddComboBoxItem(3, 0, L"Int32");
		pLayout->AddComboBoxItem(3, 0, L"Int64");
		pLayout->AddComboBoxItem(3, 0, L"Float");
		pLayout->AddComboBoxItem(3, 0, L"Double");

		dlg.Add(pLayout);

		if(dlg.DoModal() != IDOK)
			break;

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateSizedGridLayoutLikeHorizontalBox()
{
	CResult cr;

	do
	{
		CGUIDialog dlg;
		dlg.SetTitle(L"Sized Grid Layout Like Horizontal Box");

		CGUIGridLayout* pLayout = new CGUIGridLayout(1, 4, 650, 50);
		pLayout->AddCtrl(0, 0, L"Figure Name :  ");
		pLayout->AddCtrl(0, 1, EControl_EditCtrl, L"NAME_FIGURE_");
		pLayout->AddCtrl(0, 2, L"Figure Template :  ");
		pLayout->AddCtrl(0, 3, EControl_ComboBox);
		pLayout->AddComboBoxItem(0, 3, L"Int32");
		pLayout->AddComboBoxItem(0, 3, L"Int64");
		pLayout->AddComboBoxItem(0, 3, L"Float");
		pLayout->AddComboBoxItem(0, 3, L"Double");

		dlg.Add(pLayout);

		if(dlg.DoModal() != IDOK)
			break;

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateComplexLayout1()
{
	CResult cr;

	do
	{
		if(!m_pModelessDlg)
		{
			m_pModelessDlg = CGUIDialog::CreateModelessDialog();
			m_pModelessDlg->SetTitle(L"Threshold (" + GetName() + L")");

			CGUIGridLayout* pGridLayout0 = new CGUIGridLayout(3, 2, true);
			{
				// Threshold Mode
				CGUIPropertyItemBase* pThreshMode = dynamic_cast<CGUIPropertyItemBase*>(FindItemByFullPath(L"Parameters@Fundamental@Threshold Mode"));
				pGridLayout0->AddCtrl(0, 0, L"Threshold Mode");

				if(!pThreshMode->GetValue().Compare(L"Single"))
					m_i32ThresholdIndex = 0;
				else if(!pThreshMode->GetValue().Compare(L"Dual And"))
					m_i32ThresholdIndex = 1;
				else
					m_i32ThresholdIndex = 2;

				pGridLayout0->AddCtrl(0, 1, EControl_ComboBox, L"Single");
				pGridLayout0->AddComboBoxItem(0, 1, L"Single");
				pGridLayout0->AddComboBoxItem(0, 1, L"Dual And");
				pGridLayout0->AddComboBoxItem(0, 1, L"Dual Or");
				pGridLayout0->SetCtrlValue(0, 1, m_i32ThresholdIndex);
				CWndCtrlProcedure* pVCP = new CWndCtrlProcedure;
				*pVCP = MakeWndCtrlProcedure
				{
					switch(_wtoi(strValue))
					{
					case 0:
						{
							m_pGridLayout1->SetVisible(true);
							m_pGridLayout2->SetVisible(false);
						}
						break;
					case 1:
					case 2:
						{
							m_pGridLayout1->SetVisible(true);
							m_pGridLayout2->SetVisible(true);
						}
						break;
					}
				m_pModelessDlg->AdjustLayout();
				return true;
				};
				pGridLayout0->SetWndCtrlProcedure(0, 1, pVCP);

				pGridLayout0->AddCtrl(1, 0, EControl_CheckBox, L"Absolute Mode");
				CWndCtrlProcedure* pVCP2 = new CWndCtrlProcedure;
				*pVCP2 = MakeWndCtrlProcedure
				{
					if(strValue == L"Checked")
					{
						m_pGridLayout1->SetVisible(true);
						m_pGridLayout2->SetVisible(true);

						if(m_pGridLayout1->m_bShow)
						{
							for(int r = 0; r < m_pGridLayout1->m_i32RowCnt; r++)
							{
								for(int c = 0; c < m_pGridLayout1->m_i32ColCnt; c++)
									m_pGridLayout1->SetCtrlVisible(r, c, true);
							}
						}
						if(m_pGridLayout2->m_bShow)
						{
							for(int r = 0; r < m_pGridLayout2->m_i32RowCnt; r++)
							{
								for(int c = 0; c < m_pGridLayout2->m_i32ColCnt; c++)
									m_pGridLayout2->SetCtrlVisible(r, c, true);
							}
						}
					}
					else
					{
						m_pGridLayout1->SetVisible(false);
						m_pGridLayout2->SetVisible(false);

						if(m_pGridLayout1->m_bShow)
						{
							for(int r = 0; r < m_pGridLayout1->m_i32RowCnt; r++)
							{
								for(int c = 0; c < m_pGridLayout1->m_i32ColCnt; c++)
									m_pGridLayout1->SetCtrlVisible(r, c, false);
							}
						}
						if(m_pGridLayout2->m_bShow)
						{
							for(int r = 0; r < m_pGridLayout2->m_i32RowCnt; r++)
							{
								for(int c = 0; c < m_pGridLayout2->m_i32ColCnt; c++)
									m_pGridLayout2->SetCtrlVisible(r, c, false);
							}
						}
					}

					m_pModelessDlg->AdjustLayout();
					return true;
				};
				pGridLayout0->SetWndCtrlProcedure(1, 0, pVCP2);

				// Absolute mode
				CGUIPropertyItemBase* pAbsolMode = dynamic_cast<CGUIPropertyItemBase*>(FindItemByFullPath(L"Parameters@Fundamental@Absolute Mode"));
				CGUICheckBox* pCheck = (CGUICheckBox*)pGridLayout0->GetCtrl(1, 0);

				pCheck->SetCheck(!pAbsolMode->GetValue().Compare(L"Checked"));

				// Channel Condition
				pGridLayout0->AddCtrl(2, 0, L"Channel Condition");
				pGridLayout0->AddCtrl(2, 1, EControl_ComboBox);
				pGridLayout0->AddComboBoxItem(2, 1, L"And");
				pGridLayout0->AddComboBoxItem(2, 1, L"Or");

				CGUIPropertyItemBase* pChannelCondition = dynamic_cast<CGUIPropertyItemBase*>(FindItemByFullPath(L"Parameters@Fundamental@Logical Condition of Channels"));

				if(!pChannelCondition->GetValue().Compare(L"And"))
					m_i32ChannelConditionIndex = 0;
				else
					m_i32ChannelConditionIndex = 1;

				pGridLayout0->SetCtrlValue(2, 1, m_i32ChannelConditionIndex);
			}
			m_pModelessDlg->Add(pGridLayout0);

			m_pGridLayout1 = new CGUIGridLayout(2, 2, true, L"Threshold 1");
			{
				m_pGridLayout1->AddCtrl(0, 0, L"test");
				m_pGridLayout1->AddCtrl(0, 1, EControl_ComboBox, std::get<0>(m_vctLogicalCondition[0]));

				for(auto& iter : m_vctLogicalCondition)
					m_pGridLayout1->AddComboBoxItem(0, 1, std::get<0>(iter));

				CGUISliderFloatingPoint* pSliderFloatingPoint = new CGUISliderFloatingPoint;

				pSliderFloatingPoint->SetRange(0, 1);
				pSliderFloatingPoint->SetUnit(0.01);

				m_pGridLayout1->AddCtrl(1, 0, EControl_EditCtrl);
				m_pGridLayout1->AddCtrl(1, 1, EControl_SliderFloatingPoint, L"", pSliderFloatingPoint);
			}

			m_pGridLayout2 = new CGUIGridLayout(2, 2, true, L"Threshold 2");
			{
				m_pGridLayout2->AddCtrl(0, 0, L"test2");
				m_pGridLayout2->AddCtrl(0, 1, EControl_ComboBox, std::get<0>(m_vctLogicalCondition[0]));

				for(auto& iter : m_vctLogicalCondition)
					m_pGridLayout2->AddComboBoxItem(0, 1, std::get<0>(iter));

				m_pGridLayout2->SetCtrlVisible(0, 0, false);
				m_pGridLayout2->SetCtrlVisible(0, 1, false);

				CGUISliderFloatingPoint* pSliderFloatingPoint = new CGUISliderFloatingPoint;

				pSliderFloatingPoint->SetRange(0, 1);
				pSliderFloatingPoint->SetUnit(0.01);

				m_pGridLayout2->AddCtrl(1, 0, EControl_EditCtrl);
				m_pGridLayout2->AddCtrl(1, 1, EControl_SliderFloatingPoint, L"", pSliderFloatingPoint);

				m_pGridLayout2->SetCtrlVisible(1, 0, false);
				m_pGridLayout2->SetCtrlVisible(1, 1, false);
			}
			m_pGridLayout2->SetVisible(false);

			CGUIBoxLayout* pBox = new CGUIBoxLayout(EOrientation_Horizontal);
			pBox->Add(m_pGridLayout1);
			pBox->Add(m_pGridLayout2);
			m_pModelessDlg->Add(pBox);

			m_pModelessDlg->OnInitDialog();
			m_pModelessDlg->AdjustLayout();
		}

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateComplexLayout1Sized()
{
	CResult cr;

	do
	{
		if(!m_pModelessDlg)
		{
			m_pModelessDlg = CGUIDialog::CreateModelessDialog();
			m_pModelessDlg->SetTitle(L"Threshold (" + GetName() + L")");

			CGUIGridLayout* pGridLayout0 = new CGUIGridLayout(3, 2, 600, 110, true);
			{
				// Threshold Mode
				CGUIPropertyItemBase* pThreshMode = dynamic_cast<CGUIPropertyItemBase*>(FindItemByFullPath(L"Parameters@Fundamental@Threshold Mode"));
				pGridLayout0->AddCtrl(0, 0, L"Threshold Mode");

				if(!pThreshMode->GetValue().Compare(L"Single"))
					m_i32ThresholdIndex = 0;
				else if(!pThreshMode->GetValue().Compare(L"Dual And"))
					m_i32ThresholdIndex = 1;
				else
					m_i32ThresholdIndex = 2;

				pGridLayout0->AddCtrl(0, 1, EControl_ComboBox, L"Single");
				pGridLayout0->AddComboBoxItem(0, 1, L"Single");
				pGridLayout0->AddComboBoxItem(0, 1, L"Dual And");
				pGridLayout0->AddComboBoxItem(0, 1, L"Dual Or");
				pGridLayout0->SetCtrlValue(0, 1, m_i32ThresholdIndex);
				CWndCtrlProcedure* pVCP = new CWndCtrlProcedure;
				*pVCP = MakeWndCtrlProcedure
				{
					switch(_wtoi(strValue))
					{
					case 0:
						{
							m_pGridLayout1->SetVisible(true);
							m_pGridLayout2->SetVisible(false);
						}
						break;
					case 1:
					case 2:
						{
							m_pGridLayout1->SetVisible(true);
							m_pGridLayout2->SetVisible(true);
						}
						break;
					}
				m_pModelessDlg->AdjustLayout();
				return true;
				};
				pGridLayout0->SetWndCtrlProcedure(0, 1, pVCP);

				pGridLayout0->AddCtrl(1, 0, EControl_CheckBox, L"Absolute Mode");
				CWndCtrlProcedure* pVCP2 = new CWndCtrlProcedure;
				*pVCP2 = MakeWndCtrlProcedure
				{
					if(0)
					{
						if(strValue == L"Checked")
						{
							m_pGridLayout1->SetVisible(true);
							m_pGridLayout2->SetVisible(true);
						}
						else
						{
							m_pGridLayout1->SetVisible(false);
							m_pGridLayout2->SetVisible(false);
						}
					}

					if(strValue == L"Checked")
					{
						if(m_pGridLayout1->m_bShow)
						{
							for(int r = 0; r < m_pGridLayout1->m_i32RowCnt; r++)
							{
								for(int c = 0; c < m_pGridLayout1->m_i32ColCnt; c++)
									m_pGridLayout1->SetCtrlVisible(r, c, true);
							}
						}
						if(m_pGridLayout2->m_bShow)
						{
							for(int r = 0; r < m_pGridLayout2->m_i32RowCnt; r++)
							{
								for(int c = 0; c < m_pGridLayout2->m_i32ColCnt; c++)
									m_pGridLayout2->SetCtrlVisible(r, c, true);
							}
						}
					}
					else
					{
						if(m_pGridLayout1->m_bShow)
						{
							for(int r = 0; r < m_pGridLayout1->m_i32RowCnt; r++)
							{
								for(int c = 0; c < m_pGridLayout1->m_i32ColCnt; c++)
									m_pGridLayout1->SetCtrlVisible(r, c, false);
							}
						}
						if(m_pGridLayout2->m_bShow)
						{
							for(int r = 0; r < m_pGridLayout2->m_i32RowCnt; r++)
							{
								for(int c = 0; c < m_pGridLayout2->m_i32ColCnt; c++)
									m_pGridLayout2->SetCtrlVisible(r, c, false);
							}
						}
					}

					m_pModelessDlg->AdjustLayout();
					return true;
				};
				pGridLayout0->SetWndCtrlProcedure(1, 0, pVCP2);

				// Absolute mode
				CGUIPropertyItemBase* pAbsolMode = dynamic_cast<CGUIPropertyItemBase*>(FindItemByFullPath(L"Parameters@Fundamental@Absolute Mode"));
				CGUICheckBox* pCheck = (CGUICheckBox*)pGridLayout0->GetCtrl(1, 0);

				pCheck->SetCheck(!pAbsolMode->GetValue().Compare(L"Checked"));

				// Channel Condition
				pGridLayout0->AddCtrl(2, 0, L"Channel Condition");
				pGridLayout0->AddCtrl(2, 1, EControl_ComboBox);
				pGridLayout0->AddComboBoxItem(2, 1, L"And");
				pGridLayout0->AddComboBoxItem(2, 1, L"Or");

				CGUIPropertyItemBase* pChannelCondition = dynamic_cast<CGUIPropertyItemBase*>(FindItemByFullPath(L"Parameters@Fundamental@Logical Condition of Channels"));

				if(!pChannelCondition->GetValue().Compare(L"And"))
					m_i32ChannelConditionIndex = 0;
				else
					m_i32ChannelConditionIndex = 1;

				pGridLayout0->SetCtrlValue(2, 1, m_i32ChannelConditionIndex);
			}
			m_pModelessDlg->Add(pGridLayout0);

			m_pGridLayout1 = new CGUIGridLayout(2, 2, true, L"Threshold 1");
			{
				m_pGridLayout1->AddCtrl(0, 0, L"test");
				m_pGridLayout1->AddCtrl(0, 1, EControl_ComboBox, std::get<0>(m_vctLogicalCondition[0]));

				for(auto& iter : m_vctLogicalCondition)
					m_pGridLayout1->AddComboBoxItem(0, 1, std::get<0>(iter));

				CGUISliderFloatingPoint* pSliderFloatingPoint = new CGUISliderFloatingPoint;

				pSliderFloatingPoint->SetRange(0, 1);
				pSliderFloatingPoint->SetUnit(0.01);

				m_pGridLayout1->AddCtrl(1, 0, EControl_EditCtrl);
				m_pGridLayout1->AddCtrl(1, 1, EControl_SliderFloatingPoint, L"", pSliderFloatingPoint);
			}

			m_pGridLayout2 = new CGUIGridLayout(2, 2, true, L"Threshold 2");
			{
				m_pGridLayout2->AddCtrl(0, 0, L"test2");
				m_pGridLayout2->AddCtrl(0, 1, EControl_ComboBox, std::get<0>(m_vctLogicalCondition[0]));

				for(auto& iter : m_vctLogicalCondition)
					m_pGridLayout2->AddComboBoxItem(0, 1, std::get<0>(iter));

				m_pGridLayout2->SetCtrlVisible(0, 0, false);
				m_pGridLayout2->SetCtrlVisible(0, 1, false);

				CGUISliderFloatingPoint* pSliderFloatingPoint = new CGUISliderFloatingPoint;

				pSliderFloatingPoint->SetRange(0, 1);
				pSliderFloatingPoint->SetUnit(0.01);

				m_pGridLayout2->AddCtrl(1, 0, EControl_EditCtrl);
				m_pGridLayout2->AddCtrl(1, 1, EControl_SliderFloatingPoint, L"", pSliderFloatingPoint);

				m_pGridLayout2->SetCtrlVisible(1, 0, false);
				m_pGridLayout2->SetCtrlVisible(1, 1, false);
			}
			m_pGridLayout2->SetVisible(false);

			CGUIBoxLayout* pBox = new CGUIBoxLayout(EOrientation_Horizontal);
			pBox->Add(m_pGridLayout1);
			pBox->Add(m_pGridLayout2);
			m_pModelessDlg->Add(pBox);

			m_pModelessDlg->OnInitDialog();
			m_pModelessDlg->AdjustLayout();
		}

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateComplexLayout2()
{
	CResult cr;

	do
	{
		if(m_pModelessDlg)
			m_pModelessDlg->DestroyWindow();

		m_pModelessDlg = CGUIDialog::CreateModelessDialog();
		m_pModelessDlg->SetTitle(L"BoxLayout Horizontal");

		CGUIGridLayout* pGridLayout = new CGUIGridLayout(2, 2);
		{
			CGUIGridLayout* m_pGridLayout1 = new CGUIGridLayout(3, 4, true, L"GridLayout 1");
			{
				CGUIGridLayout* pGridLayoutG1_1 = new CGUIGridLayout(3, 3, true, L"GridLayout G1-1");
				{
					pGridLayoutG1_1->AddCtrl(0, 0, EControl_EditCtrl, L"ABC");
					pGridLayoutG1_1->AddCtrl(0, 1, EControl_EditCtrl, L"ABC");
					pGridLayoutG1_1->AddCtrl(0, 2, EControl_EditCtrl, L"ABC");

					pGridLayoutG1_1->AddCtrl(1, 0, EControl_EditCtrl, L"ABCDEFGW'");
					pGridLayoutG1_1->AddCtrl(1, 1, EControl_EditCtrl, L"AB'");
					pGridLayoutG1_1->AddCtrl(1, 2, EControl_EditCtrl, L"ABCDEFW'");

					pGridLayoutG1_1->AddCtrl(2, 0, EControl_EditCtrl, L"ABCDEFGW'");
					pGridLayoutG1_1->AddCtrl(2, 1, EControl_EditCtrl, L"ABGW'");
					pGridLayoutG1_1->AddCtrl(2, 2, EControl_EditCtrl, L"FGW'");
				}

				m_pGridLayout1->Add(0, 0, 3, 3, pGridLayoutG1_1);
				m_pGridLayout1->AddCtrl(0, 3, EControl_EditCtrl, L"ABC");
				m_pGridLayout1->AddCtrl(1, 3, EControl_EditCtrl, L"ABCD");
				m_pGridLayout1->AddCtrl(2, 3, EControl_EditCtrl, L"FGW'");
			}

			CGUIBoxLayout* pBoxLayout1 = new CGUIBoxLayout(EOrientation_Vertical, true, L"BoxLayout 1");
			{
				CGUIGridLayout* pGridLayoutB1_1 = new CGUIGridLayout(3, 3, true, L"GridLayout B1-1");
				{
					pGridLayoutB1_1->AddCtrl(0, 0, EControl_EditCtrl, L"ABC");
					pGridLayoutB1_1->AddCtrl(0, 1, EControl_EditCtrl, L"ABC");
					pGridLayoutB1_1->AddCtrl(0, 2, EControl_EditCtrl, L"ABC");

					pGridLayoutB1_1->AddCtrl(1, 0, EControl_EditCtrl, L"ABC");
					pGridLayoutB1_1->AddCtrl(1, 1, EControl_EditCtrl, L"AB'");
					pGridLayoutB1_1->AddCtrl(1, 2, EControl_EditCtrl, L"FGW");

					pGridLayoutB1_1->AddCtrl(2, 0, EControl_EditCtrl, L"ABC'");
					pGridLayoutB1_1->AddCtrl(2, 1, EControl_EditCtrl, L"ABG");
					pGridLayoutB1_1->AddCtrl(2, 2, EControl_EditCtrl, L"FGW'");
				}
				pBoxLayout1->Add(pGridLayoutB1_1);
			}

			CGUIGridLayout* m_pGridLayout2 = new CGUIGridLayout(3, 6, true, L"GridLayout 2");
			{
				m_pGridLayout2->AddCtrl(0, 0, 1, 6, EControl_EditCtrl, L"ABCDEEEEEEEEEE");
				m_pGridLayout2->AddCtrl(1, 0, 1, 6, EControl_EditCtrl, L"ABCDEEEEEEEEEE");
				m_pGridLayout2->AddCtrl(2, 0, 1, 6, EControl_EditCtrl, L"ABCDEEEEEEEEEE");
			}

			pGridLayout->Add(0, 0, 1, 2, m_pGridLayout1);
			pGridLayout->Add(1, 0, pBoxLayout1);
			pGridLayout->Add(1, 1, m_pGridLayout2);
		}

		m_pModelessDlg->Add(pGridLayout);
		m_pModelessDlg->OnInitDialog();

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateComplexLayout3()
{
	CResult cr;

	do
	{
		m_pModelessDlg = CGUIDialog::CreateModelessDialog();
		m_pModelessDlg->SetTitle(L"Threshold (" + GetName() + L")");
		m_pModelessDlg->EnableScroll(true);

		CGUIGridLayout* m_pGridLayout0 = new CGUIGridLayout(3, 3, true);
		{
			// Threshold Mode
			CGUIPropertyItemBase* pThreshMode = dynamic_cast<CGUIPropertyItemBase*>(FindItemByFullPath(L"Parameters@Fundamental@Threshold Mode"));
			m_pGridLayout0->AddCtrl(0, 0, L"Threshold Mode");

			if(!pThreshMode->GetValue().Compare(L"Single"))
				m_i32ThresholdIndex = 0;
			else if(!pThreshMode->GetValue().Compare(L"Dual And"))
				m_i32ThresholdIndex = 1;
			else
				m_i32ThresholdIndex = 2;

			m_pGridLayout0->AddCtrl(0, 1, 1, 2, EControl_ComboBox, L"Single");
			m_pGridLayout0->AddComboBoxItem(0, 1, L"Single");
			m_pGridLayout0->AddComboBoxItem(0, 1, L"Dual And");
			m_pGridLayout0->AddComboBoxItem(0, 1, L"Dual Or");
			m_pGridLayout0->SetCtrlValue(0, 1, m_i32ThresholdIndex);

			CWndCtrlProcedure* pVCP = new CWndCtrlProcedure;
			*pVCP = MakeWndCtrlProcedure
			{

			switch(_wtoi(strValue))
			{
			case 0:
				{
					m_pGridLayout1->SetVisible(true);
					m_pGridLayout2->SetVisible(false);
					int32_t i32Start = 0;

					for(; i32Start < 3; ++i32Start)
					{
						m_pGridLayout1->SetCtrlVisible(2 * i32Start, 0, true);
						m_pGridLayout1->SetCtrlVisible(2 * i32Start, 1, true);
						m_pGridLayout1->SetCtrlVisible(2 * i32Start + 1, 0, true);
						m_pGridLayout1->SetCtrlVisible(2 * i32Start + 1, 1, true);
					}

					for(; i32Start < 10; ++i32Start)
					{
						m_pGridLayout1->SetCtrlVisible(2 * i32Start, 0, false);
						m_pGridLayout1->SetCtrlVisible(2 * i32Start, 1, false);
						m_pGridLayout1->SetCtrlVisible(2 * i32Start + 1, 0, false);
						m_pGridLayout1->SetCtrlVisible(2 * i32Start + 1, 1, false);
					}
				}
				break;
			case 1:
			case 2:
				{
					m_pGridLayout1->SetVisible(true);
					m_pGridLayout2->SetVisible(true);
					int32_t i32Start = 0;

					for(; i32Start < 3; ++i32Start)
					{
						m_pGridLayout1->SetCtrlVisible(2 * i32Start, 0, true);
						m_pGridLayout1->SetCtrlVisible(2 * i32Start, 1, true);
						m_pGridLayout1->SetCtrlVisible(2 * i32Start + 1, 0, true);
						m_pGridLayout1->SetCtrlVisible(2 * i32Start + 1, 1, true);
						m_pGridLayout2->SetCtrlVisible(2 * i32Start, 0, true);
						m_pGridLayout2->SetCtrlVisible(2 * i32Start, 1, true);
						m_pGridLayout2->SetCtrlVisible(2 * i32Start + 1, 0, true);
						m_pGridLayout2->SetCtrlVisible(2 * i32Start + 1, 1, true);
					}

					for(; i32Start < 10; ++i32Start)
					{
						m_pGridLayout1->SetCtrlVisible(2 * i32Start, 0, false);
						m_pGridLayout1->SetCtrlVisible(2 * i32Start, 1, false);
						m_pGridLayout1->SetCtrlVisible(2 * i32Start + 1, 0, false);
						m_pGridLayout1->SetCtrlVisible(2 * i32Start + 1, 1, false);
						m_pGridLayout2->SetCtrlVisible(2 * i32Start, 0, false);
						m_pGridLayout2->SetCtrlVisible(2 * i32Start, 1, false);
						m_pGridLayout2->SetCtrlVisible(2 * i32Start + 1, 0, false);
						m_pGridLayout2->SetCtrlVisible(2 * i32Start + 1, 1, false);
					}
				}
				break;
			}
			m_pModelessDlg->AdjustLayout();
			return true;
			};

			m_pGridLayout0->SetWndCtrlProcedure(0, 1, pVCP);

			m_pGridLayout0->AddCtrl(1, 0, 1, 1, EControl_CheckBox, L"Absolute Mode");

			// Absolute mode
			CGUIPropertyItemBase* pAbsolMode = dynamic_cast<CGUIPropertyItemBase*>(FindItemByFullPath(L"Parameters@Fundamental@Absolute Mode"));
			CGUICheckBox* pCheck = (CGUICheckBox*)m_pGridLayout0->GetCtrl(1, 0);

			pCheck->SetCheck(!pAbsolMode->GetValue().Compare(L"Checked"));

			// Channel Condition
			m_pGridLayout0->AddCtrl(2, 0, L"Channel Condition");
			m_pGridLayout0->AddCtrl(2, 1, 1, 2, EControl_ComboBox);
			m_pGridLayout0->AddComboBoxItem(2, 1, L"And");
			m_pGridLayout0->AddComboBoxItem(2, 1, L"Or");

			CGUIPropertyItemBase* pChannelCondition = dynamic_cast<CGUIPropertyItemBase*>(FindItemByFullPath(L"Parameters@Fundamental@Logical Condition of Channels"));

			if(!pChannelCondition->GetValue().Compare(L"And"))
				m_i32ChannelConditionIndex = 0;
			else
				m_i32ChannelConditionIndex = 1;

			m_pGridLayout0->SetCtrlValue(2, 1, m_i32ChannelConditionIndex);
		}
		m_pModelessDlg->Add(m_pGridLayout0);

		m_pGridLayout1 = new CGUIGridLayout(20, 3, true, L"Threshold 1");
		{
			CFLString<wchar_t> fls;

			for(int32_t i = 0; i < 10; ++i)
			{
				fls.Format(L"Condition %d", i);
				m_pGridLayout1->AddCtrl(2 * i, 0, fls);
				m_pGridLayout1->AddCtrl(2 * i, 1, 1, 2, EControl_ComboBox, std::get<0>(m_vctLogicalCondition[0]));

				for(auto& iter : m_vctLogicalCondition)
					m_pGridLayout1->AddComboBoxItem(2 * i, 1, std::get<0>(iter));

				m_pGridLayout1->SetCtrlVisible(2 * i, 0, false);
				m_pGridLayout1->SetCtrlVisible(2 * i, 1, false);

				CGUISliderFloatingPoint* pSliderFloatingPoint = new CGUISliderFloatingPoint;

				pSliderFloatingPoint->SetRange(0, 1);
				pSliderFloatingPoint->SetUnit(0.01);

				m_pGridLayout1->AddCtrl(2 * i + 1, 0, EControl_EditCtrl);
				m_pGridLayout1->AddCtrl(2 * i + 1, 1, 1, 2, EControl_SliderFloatingPoint, L"", pSliderFloatingPoint);

				m_pGridLayout1->SetCtrlVisible(2 * i + 1, 0, false);
				m_pGridLayout1->SetCtrlVisible(2 * i + 1, 1, false);
			}

			for(int32_t i = 0; i < 3; ++i)
			{
				m_pGridLayout1->SetCtrlVisible(2 * i, 0, true);
				m_pGridLayout1->SetCtrlVisible(2 * i, 1, true);
				m_pGridLayout1->SetCtrlVisible(2 * i + 1, 0, true);
				m_pGridLayout1->SetCtrlVisible(2 * i + 1, 1, true);
				/*

				CGUIPropertyItemBase* pThresh = FindItemByFullPath(CFLString<wchar_t>().Format(L"Parameters@Fundamental@Threshold Value1@Channel %d", i));

				m_pGridLayout1->SetCtrlValue(2 * i + 1, 0, pThresh->GetValue());
				m_pGridLayout1->SetCtrlValue(2 * i + 1, 1, pThresh->GetValue());

				CGUIPropertyItemDropdownList* pCondition = dynamic_cast<CGUIPropertyItemDropdownList*>(FindItemByFullPath(CFLString<wchar_t>().Format(L"Parameters@Fundamental@Threshold Condition1@Channel %d", i)));
				int32_t i32ConditionIndex = pCondition->GetIndex((CFLString<wchar_t>&)pCondition->GetValue());
*/
			}
		}

		m_pGridLayout2 = new CGUIGridLayout(20, 3, true, L"Threshold 2");
		{
			CFLString<wchar_t> fls;

			for(int32_t i = 0; i < 10; ++i)
			{
				fls.Format(L"Condition %d", i);
				m_pGridLayout2->AddCtrl(2 * i, 0, fls);
				m_pGridLayout2->AddCtrl(2 * i, 1, 1, 2, EControl_ComboBox, std::get<0>(m_vctLogicalCondition[0]));

				for(auto& iter : m_vctLogicalCondition)
					m_pGridLayout2->AddComboBoxItem(2 * i, 1, std::get<0>(iter));

				m_pGridLayout2->SetCtrlVisible(2 * i, 0, false);
				m_pGridLayout2->SetCtrlVisible(2 * i, 1, false);

				CGUIPropertyItemBase* pThresholdValue = dynamic_cast<CGUIPropertyItemBase*>(FindItemByFullPath(CFLString<wchar_t>().Format(L"Parameters@Fundamental@Threshold Value1@Channel %d", i)));
				CGUISliderFloatingPoint* pSliderFloatingPoint = new CGUISliderFloatingPoint;

				pSliderFloatingPoint->SetRange(0, 1);
				pSliderFloatingPoint->SetUnit(0.01);

				m_pGridLayout2->AddCtrl(2 * i + 1, 0, EControl_EditCtrl);
				m_pGridLayout2->AddCtrl(2 * i + 1, 1, 1, 2, EControl_SliderFloatingPoint, L"", pSliderFloatingPoint);

				m_pGridLayout2->SetCtrlVisible(2 * i + 1, 0, false);
				m_pGridLayout2->SetCtrlVisible(2 * i + 1, 1, false);
			}

			for(int32_t i = 0; i < 3; ++i)
			{
				m_pGridLayout2->SetCtrlVisible(2 * i, 0, true);
				m_pGridLayout2->SetCtrlVisible(2 * i, 1, true);
				m_pGridLayout2->SetCtrlVisible(2 * i + 1, 0, true);
				m_pGridLayout2->SetCtrlVisible(2 * i + 1, 1, true);


			}
		}
		m_pGridLayout2->SetVisible(false);

		CGUIBoxLayout* pBox = new CGUIBoxLayout(EOrientation_Horizontal);
		pBox->Add(m_pGridLayout1);
		pBox->Add(m_pGridLayout2);
		m_pModelessDlg->Add(pBox);

		m_pModelessDlg->OnInitDialog();
		m_pModelessDlg->AdjustLayout();
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateBoxLayoutStaticTextAlignment()
{
	CResult cr;

	do
	{
		if(m_pModelessDlg)
			m_pModelessDlg->DestroyWindow();

		m_pModelessDlg = CGUIDialog::CreateModelessDialog();
		m_pModelessDlg->SetTitle(L"BoxLayout");

		CGUIBoxLayout* pBoxLayout = new CGUIBoxLayout(EOrientation_Vertical);
		pBoxLayout->AddCtrl(EControl_ComboBox);
		pBoxLayout->AddComboBoxItem(0, L"LEFT_TOP");
		pBoxLayout->AddComboBoxItem(0, L"CENTER_TOP");
		pBoxLayout->AddComboBoxItem(0, L"RIGHT_TOP");
		pBoxLayout->AddComboBoxItem(0, L"LEFT_CENTER");
		pBoxLayout->AddComboBoxItem(0, L"CENTER_CENTER");
		pBoxLayout->AddComboBoxItem(0, L"RIGHT_CENTER");
		pBoxLayout->AddComboBoxItem(0, L"LEFT_BOTTOM");
		pBoxLayout->AddComboBoxItem(0, L"CENTER_BOTTOM");
		pBoxLayout->AddComboBoxItem(0, L"RIGHT_BOTTOM");
		pBoxLayout->SetCtrlValue(0, L"LEFT_TOP");

		CWndCtrlProcedure* pWCP = new CWndCtrlProcedure;
		*pWCP = MakeWndCtrlProcedure
		{
			CGUIComboBox * pComboBox = (CGUIComboBox*)m_pModelessDlg->GetAt(0)->GetCtrl(0);
			CString str;
			pComboBox->GetLBText(_wtoi(strValue), str);
			m_pModelessDlg->GetAt(0)->SetCtrlAlignment(1, GetAlignment(CFLString<wchar_t>(str)));
			m_pModelessDlg->AdjustLayout();
			return true;
		};
		pBoxLayout->SetWndCtrlProcedure(0, pWCP);

		pBoxLayout->AddCtrl(1, 9, EControl_StaticText, L"In a general sense, continuity \ndescribes how two items \ncome together. In ACIS, \nthese items may be two curves \nthat meet in some way, \ntwo portions of the same curve, etc. (In the latter case, \none is usually describing \nthe smoothness of a curve, \nwhich is a global property, \nin terms of a local property.)");
		pBoxLayout->SetCtrlAlignment(1, EGUIAlignment_LEFT_TOP);

		m_pModelessDlg->Add(pBoxLayout);
		m_pModelessDlg->OnInitDialog();

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateGridLayoutStaticTextAlignment()
{
	CResult cr;

	do
	{
		if(m_pModelessDlg)
			m_pModelessDlg->DestroyWindow();

		m_pModelessDlg = CGUIDialog::CreateModelessDialog();
		m_pModelessDlg->SetTitle(L"GridLayout");

		CGUIGridLayout* pGridLayout = new CGUIGridLayout(10, 1, EOrientation_Vertical);
		pGridLayout->AddCtrl(0, 0, EControl_ComboBox);
		pGridLayout->AddComboBoxItem(0, 0, L"LEFT_TOP");
		pGridLayout->AddComboBoxItem(0, 0, L"CENTER_TOP");
		pGridLayout->AddComboBoxItem(0, 0, L"RIGHT_TOP");
		pGridLayout->AddComboBoxItem(0, 0, L"LEFT_CENTER");
		pGridLayout->AddComboBoxItem(0, 0, L"CENTER_CENTER");
		pGridLayout->AddComboBoxItem(0, 0, L"RIGHT_CENTER");
		pGridLayout->AddComboBoxItem(0, 0, L"LEFT_BOTTOM");
		pGridLayout->AddComboBoxItem(0, 0, L"CENTER_BOTTOM");
		pGridLayout->AddComboBoxItem(0, 0, L"RIGHT_BOTTOM");
		pGridLayout->SetCtrlValue(0, 0, L"LEFT_TOP");

		CWndCtrlProcedure* pWCP = new CWndCtrlProcedure;
		*pWCP = MakeWndCtrlProcedure
		{
			CGUIComboBox * pComboBox = (CGUIComboBox*)m_pModelessDlg->GetAt(0)->GetCtrl(0);
			CString str;
			pComboBox->GetLBText(_wtoi(strValue), str);
			m_pModelessDlg->GetAt(0)->SetCtrlAlignment(1, GetAlignment(CFLString<wchar_t>(str)));
			m_pModelessDlg->AdjustLayout();
			return true;
		};
		pGridLayout->SetWndCtrlProcedure(0, 0, pWCP);

		pGridLayout->AddCtrl(1, 0, 9, 1, EControl_StaticText, L"In a general sense, continuity \ndescribes how two items \ncome together. In ACIS, \nthese items may be two curves \nthat meet in some way, \ntwo portions of the same curve, etc. (In the latter case, \none is usually describing \nthe smoothness of a curve, \nwhich is a global property, \nin terms of a local property.)");
		pGridLayout->SetCtrlAlignment(1, 0, EGUIAlignment_LEFT_TOP);

		m_pModelessDlg->Add(pGridLayout);
		m_pModelessDlg->OnInitDialog();

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateListCtrlDialog1()
{
	CResult cr;

	do
	{

		Base::CFLString<wchar_t> strText;
		Base::CFLString<wchar_t> strTextName;
		Base::CFLString<wchar_t> strTextValue;

		CGUIDialog* pDlg = CGUIDialog::CreateModelessDialog();
		pDlg->SetTitle(L"Moment");

		CGUIListCtrl* pListCtrl = new CGUIListCtrl;
		pListCtrl->CreateListCtrl(GetPaneProperties());
		pListCtrl->InsertColumn(0, _T("Moment"), LVCFMT_CENTER, 120, -1);

		for(int32_t i32Ch = 0; i32Ch < 1; ++i32Ch)
		{
			strText.Format(L"Channel %d", i32Ch);
			pListCtrl->InsertColumn(i32Ch + 1, strText.GetBuffer(), LVCFMT_CENTER, 120, -1);
		}

		CRect rtListCtrl = pListCtrl->GetBoundaryRect();
		CGUIGridLayout* pGridLayoutGeometric = new CGUIGridLayout(1, 1, rtListCtrl.Width() + GetSystemMetrics(SM_CYMENU), 10 * 24, true, L"< Geometric Moments >");

		for(int32_t i = 9; i >= 0; --i)
		{
			strTextName = L"Hi";
			pListCtrl->InsertItem(0, strTextName);
		}

		double arr[9] = { 1,2,3,4,5,6,7,8,9 };

		for(int32_t i32Ch = 0; i32Ch < 1; ++i32Ch)
		{
			for(int32_t i = 9; i >= 0; --i)
			{
				strTextValue.Format(L"%e", arr[i]);
				pListCtrl->SetItem(i, i32Ch + 1, LVIF_TEXT, strTextValue, 0, 0, 0, NULL);
			}
		}
		pGridLayoutGeometric->AddCtrl(0, 0, 1, 1, EControl_ListCtrl, L"", pListCtrl);
		pDlg->Add(pGridLayoutGeometric);
		pDlg->OnInitDialog();

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuGUIDialogExample::CreateListCtrlDialog2()
{
	CResult cr;

	do
	{
		CGUIDialog* pDlg = CGUIDialog::CreateModelessDialog();
		pDlg->SetTitle(L"Geometric Learn Class List");

		CGUIListCtrl* pListCtrl = new CGUIListCtrl;
		pListCtrl->CreateListCtrl(GetPaneProperties());
		pListCtrl->InsertColumn(0, L" [ Page ]", LVCFMT_CENTER, 80, -1);
		pListCtrl->InsertColumn(1, L" Class", LVCFMT_CENTER, 140, -1);
		pListCtrl->InsertColumn(2, L" Feature Count", LVCFMT_CENTER, 120, -1);
		pListCtrl->InsertColumn(3, L" Feature Filter", LVCFMT_CENTER, 120, -1);
		pListCtrl->InsertColumn(4, L" Threshold Coeff", LVCFMT_CENTER, 120, -1);
		pListCtrl->InsertColumn(5, L" Pivot", LVCFMT_CENTER, 140, -1);

		CRect rtListCtrl = pListCtrl->GetBoundaryRect();
		CGUIGridLayout* pGridLayoutGeometric = new CGUIGridLayout(1, 1, rtListCtrl.Width() + GetSystemMetrics(SM_CYMENU), ((int32_t)10) * 24 + 20, true, L"");

		for(int32_t i32Index = (int32_t)10; i32Index >= 0; --i32Index)
		{
			CFLString<wchar_t> strTextName = CFLString<wchar_t>().Format(L"Page %d", i32Index);
			pListCtrl->InsertItem(0, strTextName);

			CFLString<wchar_t> strClassName = L"Hi";
			pListCtrl->SetItem(0, 1, LVIF_TEXT, strClassName, 0, 0, 0, NULL);


			CFLString<wchar_t> strTextValue;
			int32_t i32FeatureCount = 1;
			strTextValue.Format(L"%lld", i32FeatureCount);
			pListCtrl->SetItem(0, 2, LVIF_TEXT, strTextValue, 0, 0, 0, NULL);

			double f64FeatureFilter = 2;
			strTextValue.Format(L"%d", int32_t(f64FeatureFilter * 100));
			pListCtrl->SetItem(0, 3, LVIF_TEXT, strTextValue, 0, 0, 0, NULL);

			double f64LearnThreshold = 3;
			strTextValue.Format(L"%d", int32_t(f64LearnThreshold * 100));
			pListCtrl->SetItem(0, 4, LVIF_TEXT, strTextValue, 0, 0, 0, NULL);

			CFLPoint<double> flpPivot(5, 5);
			strTextValue.Format(L"(%.3lf, %.3lf)", flpPivot.x, flpPivot.y);
			pListCtrl->SetItem(0, 5, LVIF_TEXT, strTextValue, 0, 0, 0, NULL);

			pGridLayoutGeometric->AddCtrl(0, 0, 1, 1, EControl_ListCtrl, L"", pListCtrl);
		}

		pDlg->Add(pGridLayoutGeometric);
		pDlg->OnInitDialog();

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

EGUIAlignment FLImaging::GUI::CPropertyMenuGUIDialogExample::GetAlignment(const Base::CFLString<wchar_t>& strVal) const
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