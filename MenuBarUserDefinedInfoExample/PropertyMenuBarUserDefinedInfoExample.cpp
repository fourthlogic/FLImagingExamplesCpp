#include "stdafx.h"
#include "PropertyMenuBarUserDefinedInfoExample.h"

using namespace FLImaging;
using namespace FLImaging::Base;
using namespace FLImaging::GUI;
using namespace FLImaging::Foundation;

FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::CPropertyMenuBarUserDefinedInfoExample()
	: m_bRunning(false)
{
}

FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::~CPropertyMenuBarUserDefinedInfoExample()
{
}

const CResult FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::ConfigureMenu()
{
	CResult cReturn = EResult_UnknownError;

	do
	{
		CGUIPropertyButton* pBtn = nullptr;

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Initialize");
		pBtn->SetPropertyButtonClickProcedure(Initialize());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Run");
		pBtn->SetPropertyButtonClickProcedure(Run());
		pBtn->SetButtonWidthRatio(.5);
		AddButton(pBtn);

		pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"Stop");
		pBtn->SetPropertyButtonClickProcedure(Stop());
		pBtn->SetButtonWidthRatio(.5);
		pBtn->Enable(false);
		AddButton(pBtn);


		CGUIPropertyItemCategory* pCat = nullptr;

		pCat = new CGUIPropertyItemCategory;
		pCat->SetName(L"Add");
		AddItem(pCat);
		{
			CGUIPropertyItemButton* pPIButton = nullptr;
			CGUIPropertyItemDropdownList* pPIDD = nullptr;
			CPropertyCallback* pCallback = nullptr;

			pPIButton = new CGUIPropertyItemButton;
			pPIButton->SetPath(L"Add");
			pPIButton->SetName(L"Add");
			pPIButton->SetButtonName(L"Apply Add");
			pPIButton->SetPropertyButtonClickProcedure(Add());
			AddItem(pPIButton);

			pPIDD = new CGUIPropertyItemDropdownList;
			pPIDD->SetPath(L"Add");
			pPIDD->SetName(L"Position");
			pPIDD->AddListItem(L"PushBack");
			pPIDD->AddListItem(L"PushFront");
			pPIDD->AddListItem(L"InsertAt");
			pPIDD->SetDefaultValue(L"PushBack");
			pCallback = new CPropertyCallback;
			*pCallback = MakePropertyCallback
			{
				CGUIPropertyItemText * pPITextIndex = (CGUIPropertyItemText*)FindItemByFullPath(L"Add@Index");
				pPITextIndex->SetVisible(strValue == L"InsertAt");
				if(pPITextIndex->GetParentWndList())
					pPITextIndex->GetParentWndList()->AdjustLayoutForVisibility();
			};
			pPIDD->SetPropertyCallback(pCallback);
			AddItem(pPIDD);

			AddItemForUserDefinedInfo(L"Add");
		}

		pCat = new CGUIPropertyItemCategory;
		pCat->SetName(L"Remove");
		AddItem(pCat);
		{
			CGUIPropertyItemButton* pPIButton = nullptr;
			CGUIPropertyItemDropdownList* pPIDD = nullptr;
			CPropertyCallback* pCallback = nullptr;

			pPIButton = new CGUIPropertyItemButton;
			pPIButton->SetPath(L"Remove");
			pPIButton->SetName(L"Remove");
			pPIButton->SetButtonName(L"Apply Remove");
			pPIButton->SetPropertyButtonClickProcedure(Remove());
			AddItem(pPIButton);

			pPIDD = new CGUIPropertyItemDropdownList;
			pPIDD->SetPath(L"Remove");
			pPIDD->SetName(L"Position");
			pPIDD->AddListItem(L"PopBack");
			pPIDD->AddListItem(L"PopFront");
			pPIDD->AddListItem(L"RemoveAt");
			pPIDD->SetDefaultValue(L"PopBack");
			pCallback = new CPropertyCallback;
			*pCallback = MakePropertyCallback
			{
				CGUIPropertyItemText * pPITextIndex = (CGUIPropertyItemText*)FindItemByFullPath(L"Remove@Index");
				pPITextIndex->SetVisible(strValue == L"RemoveAt");
				if(pPITextIndex->GetParentWndList())
					pPITextIndex->GetParentWndList()->AdjustLayoutForVisibility();
			};
			pPIDD->SetPropertyCallback(pCallback);
			AddItem(pPIDD);

			CGUIPropertyItemText* pPIText = new CGUIPropertyItemText;
			pPIText->SetPath(L"Remove");
			pPIText->SetName(L"Index");
			pPIText->SetInputType(CGUIEditBase::EInputType_Number);
			pPIText->SetDefaultValue(L"0");
			AddItem(pPIText);
		}

		pCat = new CGUIPropertyItemCategory;
		pCat->SetName(L"Set");
		AddItem(pCat);
		{
			CGUIPropertyItemButton* pPIButton = nullptr;
			CGUIPropertyItemText* pPITIndex = nullptr;
			CPropertyCallback* pCallback = nullptr;

			pPIButton = new CGUIPropertyItemButton;
			pPIButton->SetPath(L"Set");
			pPIButton->SetName(L"Set");
			pPIButton->SetButtonName(L"Apply Set");
			pPIButton->SetPropertyButtonClickProcedure(Set());
			AddItem(pPIButton);

			AddItemForUserDefinedInfo(L"Set");

			pPITIndex = (CGUIPropertyItemText*)FindItemByFullPath(L"Set@Index");
			pCallback = new CPropertyCallback;
			*pCallback = MakePropertyCallback
			{
				UpdateSetParameters(strValue.ToInt32());
			};
			pPITIndex->SetPropertyCallback(pCallback);
		}

		cReturn = EResult_OK;
	}
	while(false);

	return cReturn;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::Initialize()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		do
		{
			// 모든 사용자 정의 정보를 제거합니다.
			// Clears all user-defined info items.
			CGUIManagerMainFrameMenuBar::ClearUserDefinedInfo();

			// 객체 생성 및 문자열, 배경색 설정 
			// Creating an object and setting background color and string
			CGUIMenuBarUserDefinedInfo udi(L"Stopped", RGB(128, 0, 0));

			// LButton Up 이벤트 시 동작하는 콜백 함수 설정
			// Set the callback function that executes on the LButton Up event
			CMenuBarButtonCallback* pCallback = new CMenuBarButtonCallback;
			*pCallback = MakeMenuBarButtonCallback
			{
				CGUIMessageBox::DoModal(L"[" + pItem->GetText() + L"]  LButton Up");
			};
			udi.SetLButtonUpCallback(pCallback);

			// LButtonDblClk 이벤트 시 동작하는 콜백 함수 설정
			// Set the callback function that executes on the LButtonDblClk event
			CMenuBarButtonCallback* pCallbackLBtnDblClk = new CMenuBarButtonCallback;
			*pCallbackLBtnDblClk = MakeMenuBarButtonCallback
			{
				CGUIMessageBox::DoModal(L"[" + pItem->GetText() + L"]  LButton DblClk");
			};
			udi.SetLButtonDblClkCallback(pCallbackLBtnDblClk);

			// LButtonUp 이벤트 시 동작하는 콜백 함수 설정
			// Set the callback function that executes on the LButtonUp event
			CMenuBarButtonCallback* pCallbackLBtnUp = new CMenuBarButtonCallback;
			*pCallbackLBtnUp = MakeMenuBarButtonCallback
			{
				CGUIMessageBox::DoModal(L"[" + pItem->GetText() + L"]  LButton Up");
			};
			udi.SetLButtonUpCallback(pCallbackLBtnUp);
			
			// RButtonUp 이벤트 시 동작하는 콜백 함수 설정
			// Set the callback function that executes on the RButtonUp event
			CMenuBarButtonCallback* pCallbackRBtnUp = new CMenuBarButtonCallback;
			*pCallbackRBtnUp = MakeMenuBarButtonCallback
			{
				CGUIMessageBox::DoModal(L"[" + pItem->GetText() + L"]  RButton Up");
			};
			udi.SetRButtonUpCallback(pCallbackRBtnUp);
			
			// MButtonUp 이벤트 시 동작하는 콜백 함수 설정
			// Set the callback function that executes on the MButtonUp event
			CMenuBarButtonCallback* pCallbackMBtnUp = new CMenuBarButtonCallback;
			*pCallbackMBtnUp = MakeMenuBarButtonCallback
			{
				CGUIMessageBox::DoModal(L"[" + pItem->GetText() + L"]  MButton Up");
			};
			udi.SetMButtonUpCallback(pCallbackMBtnUp);

			// Current model name 사각형의 왼쪽 방향으로 하나씩 추가됩니다.
			// Object is added in the left direction of the current model name rectangle.
			CGUIManagerMainFrameMenuBar::PushBackUserDefinedInfo(udi);

			if(CGUIMenuBar* pMenuBar = CGUIManagerMainFrameMenuBar::GetMenuBar())
				pMenuBar->Invalidate();
		}
		while(false);
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::Run()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		do
		{
			CGUIPropertyButton* pBtnRun = (CGUIPropertyButton*)FindPropertyButtonByName(L"Run");
			CGUIPropertyButton* pBtnStop = (CGUIPropertyButton*)FindPropertyButtonByName(L"Stop");

			pBtnRun->Enable(false);
			pBtnStop->Enable(true);

			// 0번 인덱스의 아이템을 얻어 오기
			// Get the item at 0
			CGUIMenuBarUserDefinedInfo* pUDI = CGUIManagerMainFrameMenuBar::GetUserDefinedInfoAt(0);

			if(pUDI && pUDI->GetText() == L"Stopped")
			{
				// 문자열 변경
				// Modify text
				pUDI->SetText(L"Running..");

				// 배경 색상 변경
				// Modify background color
				pUDI->SetBackgroundColor(RGB(0, 128, 0));
			}

			if(CGUIMenuBar* pMenuBar = CGUIManagerMainFrameMenuBar::GetMenuBar())
				pMenuBar->Invalidate();
		}
		while(false);
	};

	return pProcedure;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::Stop()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		do
		{
			CGUIPropertyButton* pBtnRun = (CGUIPropertyButton*)FindPropertyButtonByName(L"Run");
			CGUIPropertyButton* pBtnStop = (CGUIPropertyButton*)FindPropertyButtonByName(L"Stop");

			pBtnRun->Enable(true);
			pBtnStop->Enable(false);

			// 0번 인덱스의 아이템을 얻어 오기
			// Get the item at 0
			CGUIMenuBarUserDefinedInfo* pUDI = CGUIManagerMainFrameMenuBar::GetUserDefinedInfoAt(0);

			if(pUDI && pUDI->GetText() == L"Running..")
			{
				// 문자열 변경
				// Modify text
				pUDI->SetText(L"Stopped");

				// 배경 색상 변경
				// Modify background color
				pUDI->SetBackgroundColor(RGB(128, 0, 0));
			}

			if(CGUIMenuBar* pMenuBar = CGUIManagerMainFrameMenuBar::GetMenuBar())
				pMenuBar->Invalidate();
		}
		while(false);
	};

	return pProcedure;
}


CPropertyItemButtonClickProcedure* FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::Add()
{
	CPropertyItemButtonClickProcedure* pProcedure = new CPropertyItemButtonClickProcedure;

	*pProcedure = MakePropertyItemButtonClickProcedure
	{
		do
		{
			CGUIPropertyItemDropdownList* pPIDD = (CGUIPropertyItemDropdownList*)FindItemByFullPath(L"Add@Position");

			if(!pPIDD)
				break;

			Base::CFLString<wchar_t> str;
			COLORREF clrBackground;
			COLORREF clrText;
			bool bShow;
			EGUIAlignment eAlign;
			EGUIStringTrimming eStringTrimming;
			int32_t i32Index;

			// "Add" 카테고리 하위 프로퍼티에서 입력한 값 얻어 오기
			// Obtain parameters from the properties under "Add" category
			GetParameters(L"Add", i32Index, str, clrBackground, clrText, bShow, eAlign, eStringTrimming);

			// 얻어온 값으로 CGUIMenuBarUserDefinedInfo 객체 생성
			// Create CGUIMenuBarUserDefinedInfo object with obtained value
			CGUIMenuBarUserDefinedInfo udi(str, clrBackground, clrText, bShow, eAlign, eStringTrimming);

			CFLString<wchar_t> strPos = pPIDD->GetValue();

			if(strPos == L"PushBack")
				CGUIManagerMainFrameMenuBar::PushBackUserDefinedInfo(udi); // 맨 뒤에 추가 // Add to the back
			else if(strPos == L"PushFront")
				CGUIManagerMainFrameMenuBar::PushFrontUserDefinedInfo(udi); // 맨 앞에 추가 // Add to the front
			else if(strPos == L"InsertAt")
				CGUIManagerMainFrameMenuBar::InsertUserDefinedInfoAt(i32Index, udi); // 설정한 인덱스 위치에 삽입 // Insert in the index you set

			if(CGUIMenuBar* pMenuBar = CGUIManagerMainFrameMenuBar::GetMenuBar())
				pMenuBar->Invalidate();
		}
		while(false);
	};

	return pProcedure;
}

CPropertyItemButtonClickProcedure* FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::Remove()
{
	CPropertyItemButtonClickProcedure* pProcedure = new CPropertyItemButtonClickProcedure;

	*pProcedure = MakePropertyItemButtonClickProcedure
	{
		do
		{
			CGUIPropertyItemDropdownList* pPIDD = (CGUIPropertyItemDropdownList*)FindItemByFullPath(L"Remove@Position");

			if(!pPIDD)
				break;

			Base::CFLString<wchar_t> str;
			COLORREF clrBackground;
			COLORREF clrText;
			bool bShow;
			EGUIAlignment eAlign;
			EGUIStringTrimming eStringTrimming;
			int32_t i32Index;

			// "Remove" 카테고리 하위 프로퍼티에서 입력한 값 얻어 오기
			// Obtain parameters from the properties under "Remove" category
			GetParameters(L"Remove", i32Index, str, clrBackground, clrText, bShow, eAlign, eStringTrimming);

			CFLString<wchar_t> strPos = pPIDD->GetValue();

			if(strPos == L"PopBack")
			{
				// 맨 뒤의 객체 제거하며 얻어 오기
				// Obtain the last object by PopBack
				CGUIMenuBarUserDefinedInfo* pUdiReturn = CGUIManagerMainFrameMenuBar::PopBackUserDefinedInfo();

				// 메모리 누수를 방지하기 위해 얻어온 객체 제거
				// Destroy the object to prevent memory leakage
				if(pUdiReturn)
				{
					delete pUdiReturn;
					pUdiReturn = nullptr;
				}
			}
			else if(strPos == L"PopFront")
			{
				// 맨 앞의 객체 제거하며 얻어 오기
				// Obtain the first object by PopFront
				CGUIMenuBarUserDefinedInfo* pUdiReturn = CGUIManagerMainFrameMenuBar::PopFrontUserDefinedInfo();

				// 메모리 누수를 방지하기 위해 얻어온 객체 제거
				// Destroy the object to prevent memory leakage
				if(pUdiReturn)
				{
					delete pUdiReturn;
					pUdiReturn = nullptr;
				}
			}
			else if(strPos == L"RemoveAt")
				CGUIManagerMainFrameMenuBar::RemoveUserDefinedInfoAt(i32Index); // 설정한 인덱스 위치의 객체 제거 // Remove objects at the index you set

			if(CGUIMenuBar* pMenuBar = CGUIManagerMainFrameMenuBar::GetMenuBar())
				pMenuBar->Invalidate();
		}
		while(false);
	};

	return pProcedure;
}

CPropertyItemButtonClickProcedure* FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::Set()
{
	CPropertyItemButtonClickProcedure* pProcedure = new CPropertyItemButtonClickProcedure;

	*pProcedure = MakePropertyItemButtonClickProcedure
	{
		do
		{
			Base::CFLString<wchar_t> str;
			COLORREF clrBackground;
			COLORREF clrText;
			bool bShow;
			EGUIAlignment eAlign;
			EGUIStringTrimming eStringTrimming;
			int32_t i32Index;

			// "Set" 카테고리 하위 프로퍼티에서 입력한 값 얻어 오기
			// Obtain parameters from the properties under "Set" category
			GetParameters(L"Set", i32Index, str, clrBackground, clrText, bShow, eAlign, eStringTrimming);

			// i32Index 인덱스의 CGUIMenuBarUserDefinedInfo 객체 포인터 얻어 오기
			// Obtain CGUIMenuBarUserDefinedInfo object pointer from i32Index index
			CGUIMenuBarUserDefinedInfo* pUdi = CGUIManagerMainFrameMenuBar::GetUserDefinedInfoAt(i32Index);

			if(!pUdi)
				break;

			// 프로퍼티에 입력한 값을 얻어온 객체에 설정
			// Set the value entered in the property to the object
			pUdi->Set(str, clrBackground, clrText, bShow, eAlign, eStringTrimming);

			if(CGUIMenuBar* pMenuBar = CGUIManagerMainFrameMenuBar::GetMenuBar())
				pMenuBar->Invalidate();
		}
		while(false);
	};

	return pProcedure;
}

const CResult FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::AddItemForUserDefinedInfo(const Base::CFLString<wchar_t>& strPath)
{
	CResult cr;

	do
	{
		CGUIPropertyItemDropdownList* pPIDD = nullptr;
		CGUIPropertyItemText* pPIText = nullptr;
		CGUIPropertyItemColor* pPIColor = nullptr;
		CGUIPropertyItemCheckBox* pPICB = nullptr;

		pPIText = new CGUIPropertyItemText;
		pPIText->SetPath(strPath);
		pPIText->SetName(L"Index");
		pPIText->SetInputType(CGUIEditBase::EInputType_Number);
		pPIText->SetDefaultValue(L"0");
		AddItem(pPIText);

		pPIText = new CGUIPropertyItemText;
		pPIText->SetPath(strPath);
		pPIText->SetName(L"String");
		pPIText->SetDefaultValue(L"Sample Text");
		AddItem(pPIText);

		pPIDD = new CGUIPropertyItemDropdownList;
		pPIDD->SetPath(strPath);
		pPIDD->SetName(L"Text Alignment");
		pPIDD->AddListItem(L"LEFT_TOP");
		pPIDD->AddListItem(L"CENTER_TOP");
		pPIDD->AddListItem(L"RIGHT_TOP");
		pPIDD->AddListItem(L"LEFT_CENTER");
		pPIDD->AddListItem(L"CENTER_CENTER");
		pPIDD->AddListItem(L"RIGHT_CENTER");
		pPIDD->AddListItem(L"LEFT_BOTTOM");
		pPIDD->AddListItem(L"CENTER_BOTTOM");
		pPIDD->AddListItem(L"RIGHT_BOTTOM");
		pPIDD->SetDefaultValue(L"CENTER_CENTER");
		pPIDD->SetDescription(L"문자열의 정렬 옵션");
		AddItem(pPIDD);

		pPIDD = new CGUIPropertyItemDropdownList;
		pPIDD->SetPath(strPath);
		pPIDD->SetName(L"String Trimming");
		pPIDD->AddListItem(L"None");
		pPIDD->AddItemDescription(0, L"Trimming 하지 않음");
		pPIDD->AddListItem(L"Character");
		pPIDD->AddItemDescription(1, L"문자열이 레이아웃 사각형 내에 있는 마지막 문자의 경계에서 끊어지게 되도록 지정합니다.");
		pPIDD->AddListItem(L"Word");
		pPIDD->AddItemDescription(2, L"문자열이 레이아웃 사각형 내에 있는 마지막 단어의 경계에서 끊어지게 되도록 지정합니다.");
		pPIDD->AddListItem(L"EllipsisCharacter");
		pPIDD->AddItemDescription(3, L"문자열이 레이아웃 사각형 내에 있는 마지막 문자의 경계에서 끊어지고 문자 뒤에 줄임표(...)가 삽입되도록 지정합니다.");
		pPIDD->AddListItem(L"EllipsisWord");
		pPIDD->AddItemDescription(4, L"문자열이 레이아웃 사각형 안에 있는 마지막 단어의 경계에서 끊어지고 단어 뒤에 줄임표(...)가 삽입되도록 지정합니다.");
		pPIDD->AddListItem(L"EllipsisPath");
		pPIDD->AddItemDescription(5, L"가운데가 문자열에서 제거되고 줄임표로 대체되도록 지정합니다. 알고리즘은 문자열의 마지막 부분을 최대한 많이 유지합니다.");
		pPIDD->SetDefaultValue(L"EllipsisCharacter");
		pPIDD->SetDescription(L"문자열이 레이아웃 사각형을 벗어날 경우 자르기 옵션");
		AddItem(pPIDD);

		pPIColor = new CGUIPropertyItemColor;
		pPIColor->SetPath(strPath);
		pPIColor->SetName(L"Background Color");
		pPIColor->SetDefaultValueByColor(RGB(80, 80, 80));
		AddItem(pPIColor);

		pPIColor = new CGUIPropertyItemColor;
		pPIColor->SetPath(strPath);
		pPIColor->SetName(L"Text Color");
		pPIColor->SetDefaultValueByColor(RGB(255, 255, 255));
		AddItem(pPIColor);

		pPICB = new CGUIPropertyItemCheckBox;
		pPICB->SetPath(strPath);
		pPICB->SetName(L"Show");
		pPICB->SetDefaultValue(L"Checked");
		AddItem(pPICB);

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::GetParameters(const Base::CFLString<wchar_t>& strPath, int32_t& i32Index, Base::CFLString<wchar_t>& str, COLORREF& clrBackground, COLORREF& clrText, bool& bShow, EGUIAlignment& eAlign, EGUIStringTrimming& eStringTrimming)
{
	CResult cr;

	do
	{
		CGUIPropertyItemDropdownList* pPIDDTA = (CGUIPropertyItemDropdownList*)FindItemByFullPath(strPath + L"@Text Alignment");
		CGUIPropertyItemDropdownList* pPIDDST = (CGUIPropertyItemDropdownList*)FindItemByFullPath(strPath + L"@String Trimming");
		CGUIPropertyItemText* pPITextIndex = (CGUIPropertyItemText*)FindItemByFullPath(strPath + L"@Index");
		CGUIPropertyItemText* pPITextString = (CGUIPropertyItemText*)FindItemByFullPath(strPath + L"@String");
		CGUIPropertyItemColor* pPIColorBk = (CGUIPropertyItemColor*)FindItemByFullPath(strPath + L"@Background Color");
		CGUIPropertyItemColor* pPIColorText = (CGUIPropertyItemColor*)FindItemByFullPath(strPath + L"@Text Color");
		CGUIPropertyItemCheckBox* pPICB = (CGUIPropertyItemCheckBox*)FindItemByFullPath(strPath + L"@Show");

		if(pPIDDTA)
			eAlign = GetAlignment(pPIDDTA->GetValue());

		if(pPIDDST)
			eStringTrimming = GetStringTrimming(pPIDDST->GetValue());

		if(pPITextIndex)
			i32Index = pPITextIndex->GetValue().ToInt32();

		if(pPITextString)
			str = pPITextString->GetValue();

		if(pPIColorBk)
			clrBackground = pPIColorBk->GetColor();

		if(pPIColorText)
			clrText = pPIColorText->GetColor();

		if(pPICB)
			bShow = pPICB->GetValue() == L"Checked";

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::UpdateSetParameters(int32_t i32Index)
{
	CResult cr;

	do
	{
		CGUIPropertyItemDropdownList* pPIDDTA = (CGUIPropertyItemDropdownList*)FindItemByFullPath(L"Set@Text Alignment");
		CGUIPropertyItemDropdownList* pPIDDST = (CGUIPropertyItemDropdownList*)FindItemByFullPath(L"Set@String Trimming");
		CGUIPropertyItemText* pPITextIndex = (CGUIPropertyItemText*)FindItemByFullPath(L"Set@Index");
		CGUIPropertyItemText* pPITextString = (CGUIPropertyItemText*)FindItemByFullPath(L"Set@String");
		CGUIPropertyItemColor* pPIColorBk = (CGUIPropertyItemColor*)FindItemByFullPath(L"Set@Background Color");
		CGUIPropertyItemColor* pPIColorText = (CGUIPropertyItemColor*)FindItemByFullPath(L"Set@Text Color");
		CGUIPropertyItemCheckBox* pPICB = (CGUIPropertyItemCheckBox*)FindItemByFullPath(L"Set@Show");

		CGUIMenuBarUserDefinedInfo* pUdi = CGUIManagerMainFrameMenuBar::GetUserDefinedInfoAt(i32Index);

		if(!pUdi)
		{
			if(pPITextString)
				pPITextString->SetValue(pPITextString->GetDefaultValue());

			if(pPIDDTA)
				pPIDDTA->SetValue(pPIDDTA->GetDefaultValue());

			if(pPIDDST)
				pPIDDST->SetValue(pPIDDST->GetDefaultValue());

			if(pPIColorBk)
				pPIColorBk->SetValue(pPIColorBk->GetDefaultValue());

			if(pPIColorText)
				pPIColorText->SetValue(pPIColorText->GetDefaultValue());

			if(pPICB)
				pPICB->SetValue(L"Checked");
		}
		else
		{
			if(pPITextString)
				pPITextString->SetValue(pUdi->GetText());

			if(pPIDDTA)
				pPIDDTA->SetValue(GetAlignmentText(pUdi->GetTextAlignment()));

			if(pPIDDST)
				pPIDDST->SetValue(GetStringTrimmingText(pUdi->GetStringTrimming()));

			if(pPIColorBk)
				pPIColorBk->SetColor(pUdi->GetBackgroundColor());

			if(pPIColorText)
				pPIColorText->SetColor(pUdi->GetTextColor());

			if(pPICB)
				pPICB->SetValue(pUdi->IsVisible() ? L"Checked" : L"Unchecked");
		}

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

EGUIAlignment FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::GetAlignment(const Base::CFLString<wchar_t>& strVal) const
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

Base::CFLString<wchar_t> FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::GetAlignmentText(EGUIAlignment eAlign) const
{
	Base::CFLString<wchar_t> strReturn;

	do
	{
		switch(eAlign)
		{
		case EGUIAlignment_CENTER_CENTER:
			strReturn = L"CENTER_CENTER";
			break;
		case EGUIAlignment_LEFT_TOP:
			strReturn = L"LEFT_TOP";
			break;
		case EGUIAlignment_RIGHT_TOP:
			strReturn = L"RIGHT_TOP";
			break;
		case EGUIAlignment_JUSTIFIED:
			strReturn = L"JUSTIFIED";
			break;
		case EGUIAlignment_CENTER_TOP:
			strReturn = L"CENTER_TOP";
			break;
		case EGUIAlignment_LEFT_BOTTOM:
			strReturn = L"LEFT_BOTTOM";
			break;
		case EGUIAlignment_RIGHT_BOTTOM:
			strReturn = L"RIGHT_BOTTOM";
			break;
		case EGUIAlignment_CENTER_BOTTOM:
			strReturn = L"CENTER_BOTTOM";
			break;
		case EGUIAlignment_LEFT_CENTER:
			strReturn = L"LEFT_CENTER";
			break;
		case EGUIAlignment_RIGHT_CENTER:
			strReturn = L"RIGHT_CENTER";
			break;
		}
	}
	while(false);

	return strReturn;
}

EGUIStringTrimming FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::GetStringTrimming(const Base::CFLString<wchar_t>& strVal) const
{
	EGUIStringTrimming eReturn = EGUIStringTrimming_EllipsisCharacter;

	if(strVal == L"EllipsisCharacter")
		eReturn = EGUIStringTrimming_EllipsisCharacter;
	else if(strVal == L"None")
		eReturn = EGUIStringTrimming_None;
	else if(strVal == L"Character")
		eReturn = EGUIStringTrimming_Character;
	else if(strVal == L"Word")
		eReturn = EGUIStringTrimming_Word;
	else if(strVal == L"EllipsisWord")
		eReturn = EGUIStringTrimming_EllipsisWord;
	else if(strVal == L"EllipsisPath")
		eReturn = EGUIStringTrimming_EllipsisPath;

	return eReturn;
}

Base::CFLString<wchar_t> FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::GetStringTrimmingText(EGUIStringTrimming eStringTrimming) const
{
	Base::CFLString<wchar_t> strReturn;

	do
	{
		switch(eStringTrimming)
		{
		case EGUIStringTrimming_EllipsisCharacter:
			strReturn = L"EllipsisCharacter";
			break;
		case EGUIStringTrimming_None:
			strReturn = L"None";
			break;
		case EGUIStringTrimming_Character:
			strReturn = L"Character";
			break;
		case EGUIStringTrimming_Word:
			strReturn = L"Word";
			break;
		case EGUIStringTrimming_EllipsisWord:
			strReturn = L"EllipsisWord";
			break;
		case EGUIStringTrimming_EllipsisPath:
			strReturn = L"EllipsisPath";
			break;
		}
	}
	while(false);

	return strReturn;
}


void FLImaging::GUI::CPropertyMenuBarUserDefinedInfoExample::OnReceiveBroadcast(const Base::CBroadcastMessage* pMessage)
{
	if(!pMessage || (pMessage->GetChannel() & 0xf0000000) != EGUIBroadcast_Mask)
		return;

	CBroadcastMessage_GUI_MenuBar_MouseEvent* pMsg = dynamic_cast<CBroadcastMessage_GUI_MenuBar_MouseEvent*>((CBroadcastMessage*)pMessage);
	CGUIMenuBar* pCallerMenuBar = (CGUIMenuBar*)pMessage->GetCaller();
	CGUIMenuBarUserDefinedInfo* pHitUDI = pMsg ? pCallerMenuBar->GetHitUserDefinedInfo(CPoint(pMsg->GetCursor()->x, pMsg->GetCursor()->y)) : nullptr;

	switch(pMessage->GetChannel())
	{
	case EGUIBroadcast_MenuBar_PreLButtonDblClk:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PreLButtonDblClk");

			if(pMsg && !pHitUDI->GetLButtonDblClkCallback())
				pMsg->SetAcceptance(false); // LButtonDblClk 에 대한 기본 처리(MainFrame 창 최대화, Restore 등)를  하지 않고 break 하는 기능
		}
		break;
	case EGUIBroadcast_MenuBar_PostLButtonDblClk:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PostLButtonDblClk");
		}
		break;
	case EGUIBroadcast_MenuBar_PreMouseMove:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PreMouseMove");
		}
		break;
	case EGUIBroadcast_MenuBar_PostMouseMove:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PostMouseMove");
		}
		break;
	case EGUIBroadcast_MenuBar_PreMouseLeave:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PreMouseLeave");
		}
		break;
	case EGUIBroadcast_MenuBar_PostMouseLeave:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PostMouseLeave");
		}
		break;
	case EGUIBroadcast_MenuBar_PreLButtonDown:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PreLButtonDown");
		}
		break;
	case EGUIBroadcast_MenuBar_PostLButtonDown:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PostLButtonDown");
		}
		break;
	case EGUIBroadcast_MenuBar_PreLButtonUp:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PreLButtonUp");
		}
		break;
	case EGUIBroadcast_MenuBar_PostLButtonUp:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PostLButtonUp");
		}
		break;
	case EGUIBroadcast_MenuBar_PreMButtonUp:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PreMButtonUp");
		}
		break;
	case EGUIBroadcast_MenuBar_PostMButtonUp:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PostMButtonUp");
		}
		break;
	case EGUIBroadcast_MenuBar_PreRButtonUp:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PreRButtonUp");
		}
		break;
	case EGUIBroadcast_MenuBar_PostRButtonUp:
		{
			if(pHitUDI)
				CLog::Write(L"From Broadcast : [" + pHitUDI->GetText() + L"]   PostRButtonUp");
		}
		break;
	default:
		break;
	}
}
