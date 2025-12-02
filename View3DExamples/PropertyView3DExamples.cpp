#include "stdafx.h"
#include "PropertyView3DExamples.h"

using namespace FLImaging;
using namespace FLImaging::Base;
using namespace FLImaging::GUI;
using namespace FLImaging::Foundation;

CPropertyView3DExamples::CPropertyView3DExamples()
{
}

CPropertyView3DExamples::~CPropertyView3DExamples()
{
}

const CResult CPropertyView3DExamples::ConfigureMenu()
{
	CResult cReturn = EResult_UnknownError;

	do
	{
		EnableStatusMessageBox(true);
		SetMessageDisplayMode(EMessageDisplayMode_AutoScroll);

		CGUIPropertyItemCategory* pCat = new CGUIPropertyItemCategory;
		pCat->SetName(L"PushObject");
		AddItem(pCat);
		{
			CGUIPropertyItemView3DList* pIVL = new CGUIPropertyItemView3DList;
			pIVL->SetPath(L"PushObject");
			pIVL->SetName(L"3D View");
			AddItem(pIVL);

			CGUIPropertyItemButton* pPIButton = nullptr;
			CGUIPropertyItemDropdownList* pPIDD = nullptr;

			pPIDD = new CGUIPropertyItemDropdownList;
			pPIDD->SetPath(L"PushObject");
			pPIDD->SetName(L"Object Type");
			pPIDD->SetDescription(L"Select the type of object to be displayed in the 3D view.\n\
								  3D 뷰에 디스플레이 할 객체 타입을 선택합니다.");

			pPIDD->AddListItem(L"PLY File");
			pPIDD->AddListItem(L"Image(Height Map)");
			pPIDD->AddListItem(L"Image(Height Map & Texture)");
			pPIDD->AddListItem(L"3D Line");
			pPIDD->AddListItem(L"Sphere");
			pPIDD->AddListItem(L"Unselectable Axis", L"3D Axis(x,y,z) which is unselectable");

			pPIDD->SetDefaultValue(L"PLY File");
			AddItem(pPIDD);

			pPIButton = new CGUIPropertyItemButton;
			pPIButton->SetPath(L"PushObject");
			pPIButton->SetName(L"PushObject");
			pPIButton->SetButtonName(L"PushObject");
			pPIButton->SetPropertyButtonClickProcedure(PushObject());
			pPIButton->SetDescription(L"An example of adding an object to be displayed in a 3D view.\n\
									  3D 뷰에 디스플레이 할 객체를 추가하는 예제입니다.");
			AddItem(pPIButton);
		}

		pCat = new CGUIPropertyItemCategory;
		pCat->SetName(L"ROI");
		AddItem(pCat);
		{
			CGUIPropertyItemView3DList* pIVL = new CGUIPropertyItemView3DList;
			pIVL->SetPath(L"ROI");
			pIVL->SetName(L"3D View");
			AddItem(pIVL);

			CGUIPropertyItemButton* pPIButton = nullptr;

			pPIButton = new CGUIPropertyItemButton;
			pPIButton->SetPath(L"ROI");
			pPIButton->SetName(L"ROIUtilities3D");
			pPIButton->SetButtonName(L"Execute");
			pPIButton->SetDescription(L"Example of CROIUtilities3D");
			pPIButton->SetPropertyButtonClickProcedure(ROIUtilities3DTest());
			pPIButton->SetDescription(L"An example of using CROIUtilities3D.\n\
									  CROIUtilities3D 사용에 관한 예제입니다.");
			AddItem(pPIButton);

			pPIButton = new CGUIPropertyItemButton;
			pPIButton->SetPath(L"ROI");
			pPIButton->SetName(L"Frustum Collision Test");
			pPIButton->SetButtonName(L"Execute");
			pPIButton->SetPropertyButtonClickProcedure(FrustumCollisionTest());
			pPIButton->SetDescription(L"An example of testing for collisions between a frustum and 3D objects.\n\
									  절두체와 3D 객체 간의 충돌 여부 테스트 예제입니다.");
			AddItem(pPIButton);
		}

		pCat = new CGUIPropertyItemCategory;
		pCat->SetName(L"Transform");
		AddItem(pCat);
		{
			CGUIPropertyItemView3DList* pIVL = new CGUIPropertyItemView3DList;
			pIVL->SetPath(L"Transform");
			pIVL->SetName(L"Source View");
			AddItem(pIVL);

			pIVL = new CGUIPropertyItemView3DList;
			pIVL->SetPath(L"Transform");
			pIVL->SetName(L"Target View");
			AddItem(pIVL);

			CGUIPropertyItemDropdownList* pPIDD = new CGUIPropertyItemDropdownList;
			pPIDD->SetPath(L"Transform");
			pPIDD->SetName(L"Transform Type");

			pPIDD->AddListItem(L"Rotate");
			pPIDD->AddListItem(L"Rotate with Pivot");
			pPIDD->AddListItem(L"Translate");
			pPIDD->AddListItem(L"Scale");

			pPIDD->SetDefaultValue(L"Rotate");
			AddItem(pPIDD);

			CGUIPropertyItemButton* pPIButton = nullptr;

			pPIButton = new CGUIPropertyItemButton;
			pPIButton->SetPath(L"Transform");
			pPIButton->SetName(L"FL3DObjectUtilities");
			pPIButton->SetButtonName(L"Execute");
			pPIButton->SetDescription(L"Example of CFL3DObjectUtilities");
			pPIButton->SetPropertyButtonClickProcedure(FL3DObjectUtilitiesTest());
			pPIButton->SetDescription(L"An example of using CFL3DObjectUtilities, which supports transformation functions such as object rotation, scaling, and translation.\n\
									  객체 회전, 스케일, 이동 등 변환 기능을 지원하는 CFL3DObjectUtilities 사용에 관한 예제입니다.");
			AddItem(pPIButton);
		}

		pCat = new CGUIPropertyItemCategory;
		pCat->SetName(L"Camera");
		AddItem(pCat);
		{
			CGUIPropertyItemView3DList* pIVL = new CGUIPropertyItemView3DList;
			pIVL->SetPath(L"Camera");
			pIVL->SetName(L"3D View");
			AddItem(pIVL);

			CGUIPropertyItemButton* pPIButton = nullptr;

			CGUIPropertyItemCategory* pCatValue = new CGUIPropertyItemCategory;
			pCatValue->SetName(L"Position");
			pCatValue->SetPath(L"Camera");
			pCatValue->SetValueList(true);
			pCatValue->SetExpand(false);
			AddItem(pCatValue);
			{
				CGUIPropertyItemText* pText = new CGUIPropertyItemText;
				pText->SetName(L"x");
				pText->SetInputType(CGUIEditBase::EInputType_Float);
				pText->SetPath(L"Camera@Position");
				pText->SetDefaultValue(L"0");
				AddItem(pText);

				pText = new CGUIPropertyItemText;
				pText->SetName(L"y");
				pText->SetInputType(CGUIEditBase::EInputType_Float);
				pText->SetPath(L"Camera@Position");
				pText->SetDefaultValue(L"0");
				AddItem(pText);

				pText = new CGUIPropertyItemText;
				pText->SetName(L"z");
				pText->SetInputType(CGUIEditBase::EInputType_Float);
				pText->SetPath(L"Camera@Position");
				pText->SetDefaultValue(L"0");
				AddItem(pText);
			}

			pCatValue = new CGUIPropertyItemCategory;
			pCatValue->SetName(L"Direction");
			pCatValue->SetPath(L"Camera");
			pCatValue->SetValueList(true);
			pCatValue->SetExpand(false);
			AddItem(pCatValue);
			{
				CGUIPropertyItemText* pText = new CGUIPropertyItemText;
				pText->SetName(L"x");
				pText->SetInputType(CGUIEditBase::EInputType_Float);
				pText->SetPath(L"Camera@Direction");
				pText->SetDefaultValue(L"0");
				AddItem(pText);

				pText = new CGUIPropertyItemText;
				pText->SetName(L"y");
				pText->SetInputType(CGUIEditBase::EInputType_Float);
				pText->SetPath(L"Camera@Direction");
				pText->SetDefaultValue(L"0");
				AddItem(pText);

				pText = new CGUIPropertyItemText;
				pText->SetName(L"z");
				pText->SetInputType(CGUIEditBase::EInputType_Float);
				pText->SetPath(L"Camera@Direction");
				pText->SetDefaultValue(L"0");
				AddItem(pText);
			}

			pCatValue = new CGUIPropertyItemCategory;
			pCatValue->SetName(L"Up Direction");
			pCatValue->SetPath(L"Camera");
			pCatValue->SetValueList(true);
			pCatValue->SetExpand(false);
			AddItem(pCatValue);
			{
				CGUIPropertyItemText* pText = new CGUIPropertyItemText;
				pText->SetName(L"x");
				pText->SetInputType(CGUIEditBase::EInputType_Float);
				pText->SetPath(L"Camera@Up Direction");
				pText->SetDefaultValue(L"0");
				AddItem(pText);

				pText = new CGUIPropertyItemText;
				pText->SetName(L"y");
				pText->SetInputType(CGUIEditBase::EInputType_Float);
				pText->SetPath(L"Camera@Up Direction");
				pText->SetDefaultValue(L"0");
				AddItem(pText);

				pText = new CGUIPropertyItemText;
				pText->SetName(L"z");
				pText->SetInputType(CGUIEditBase::EInputType_Float);
				pText->SetPath(L"Camera@Up Direction");
				pText->SetDefaultValue(L"0");
				AddItem(pText);
			}

			pCatValue = new CGUIPropertyItemCategory;
			pCatValue->SetName(L"Target");
			pCatValue->SetPath(L"Camera");
			pCatValue->SetValueList(true);
			pCatValue->SetExpand(false);
			AddItem(pCatValue);
			{
				CGUIPropertyItemText* pText = new CGUIPropertyItemText;
				pText->SetName(L"x");
				pText->SetInputType(CGUIEditBase::EInputType_Float);
				pText->SetPath(L"Camera@Target");
				pText->SetDefaultValue(L"0");
				AddItem(pText);

				pText = new CGUIPropertyItemText;
				pText->SetName(L"y");
				pText->SetInputType(CGUIEditBase::EInputType_Float);
				pText->SetPath(L"Camera@Target");
				pText->SetDefaultValue(L"0");
				AddItem(pText);

				pText = new CGUIPropertyItemText;
				pText->SetName(L"z");
				pText->SetInputType(CGUIEditBase::EInputType_Float);
				pText->SetPath(L"Camera@Target");
				pText->SetDefaultValue(L"0");
				AddItem(pText);
			}

			CGUIPropertyItemText* pText = new CGUIPropertyItemText;
			pText->SetName(L"Angle of View(Degree)");
			pText->SetDescription(L"Input the angle of view in degree(not radian)");
			pText->SetInputType(CGUIEditBase::EInputType_Float);
			pText->SetPath(L"Camera");
			pText->SetDefaultValue(L"45");
			AddItem(pText);

			pPIButton = new CGUIPropertyItemButton;
			pPIButton->SetPath(L"Camera");
			pPIButton->SetName(L"Set Parameters");
			pPIButton->SetButtonName(L"Apply");
			pPIButton->SetPropertyButtonClickProcedure(SetCameraParameters());
			pPIButton->SetDescription(L"An example of using the camera object (CGUIView3DCamera).\n\
									  카메라 객체(CGUIView3DCamera) 사용에 관한 예제입니다.");
			AddItem(pPIButton);
		}

		cReturn = EResult_OK;
	}
	while(false);

	return cReturn;
}

CPropertyItemButtonClickProcedure* FLImaging::GUI::CPropertyView3DExamples::PushObject()
{
	CPropertyItemButtonClickProcedure* pProcedure = new CPropertyItemButtonClickProcedure;

	*pProcedure = MakePropertyItemButtonClickProcedure
	{
		do
		{
			CGUIPropertyItemView3DList* pViewList = dynamic_cast<CGUIPropertyItemView3DList*>(FindItemByFullPath(L"PushObject@3D View"));

			if(!pViewList)
				break;

			CGUIView3D* pView3D = pViewList->GetSelectedView3D();

			if(!pView3D)
			{
				SetStatusMessage(L"[Error] Select 3D view.");
				break;
			}

			CGUIPropertyItemDropdownList* pPIDD = (CGUIPropertyItemDropdownList*)FindItemByFullPath(L"PushObject@Object Type");

			if(!pPIDD)
				break;

			CFLString<wchar_t> str = pPIDD->GetValue();

			CResult res;

			if(str == L"PLY File")
				res = PushObjectPLY(pView3D);
			else if(str == L"Image(Height Map)")
				res = PushObjectHeightMap(pView3D);
			else if(str == L"Image(Height Map & Texture)")
				res = PushObjectHeightMapAndTexture(pView3D);
			else if(str == L"3D Line")
				res = PushObjectLine(pView3D);
			else if(str == L"Sphere")
				res = PushObjectSphere(pView3D);
			else if(str == L"Unselectable Axis")
				res = PushObjectUnselectableAxis(pView3D);
		}
		while(false);
	};

	return pProcedure;
}

CPropertyItemButtonClickProcedure* FLImaging::GUI::CPropertyView3DExamples::ROIUtilities3DTest()
{
	CPropertyItemButtonClickProcedure* pProcedure = new CPropertyItemButtonClickProcedure;

	*pProcedure = MakePropertyItemButtonClickProcedure
	{
		CResult res;

		do
		{
			CGUIPropertyItemView3DList* pViewList = dynamic_cast<CGUIPropertyItemView3DList*>(FindItemByFullPath(L"ROI@3D View"));

			if(!pViewList)
				break;

			CGUIView3D* pView3D = pViewList->GetSelectedView3D();

			if(!pView3D)
			{
				SetStatusMessage(L"[Error] Select 3D view.");
				break;
			}

			int32_t i32ObjectCount = pView3D->GetObjectCount();

			if(!i32ObjectCount)
			{
				SetStatusMessage(L"[Error] " + pView3D->GetTitle() + L" doesn't have any 3D objects.");
				break;
			}

			CFLFigureArray flfaFrustum;
			pView3D->GetAllROI(&flfaFrustum);
			int64_t i64ROICount = flfaFrustum.GetCount();

			if(i64ROICount == 0)
			{
				SetStatusMessage(L"[Error] There is no ROI in " + pView3D->GetTitle() + L". Press Ctrl+3 to teach ROI.");
				break;
			}

			CROIUtilities3D roiUtil3D;

			for(int64_t i = 0; i < i64ROICount; ++i)
				roiUtil3D.PushBackROI(flfaFrustum.GetAt(i));

			for(int32_t i = 0; i < i32ObjectCount; ++i)
			{
				CGUIView3DObject* pObj = (CGUIView3DObject*)pView3D->GetView3DObject(i);

				if(!pObj || !pObj->IsSelectionEnabled())
					continue;

				CFL3DObject* p3DObject = (CFL3DObject*)pObj->Get3DObject();

				if(!p3DObject)
					continue;

				roiUtil3D.PushBack3DObject(p3DObject);
			}

			CFLArray<CFLArray<int32_t>> flfaResultROIIndexInclude, flfaResultROIIndexExclude;

			roiUtil3D.SetSelectionType(CROIUtilities3D::EResultSelectionType_Include);

			if((res = roiUtil3D.Execute()).IsFail())
				break;

			if((res = roiUtil3D.GetResult(flfaResultROIIndexInclude)).IsFail())
				break;

			Foundation::CFL3DObject resultObject3D;

			if((res = roiUtil3D.GetResult(resultObject3D)).IsFail())
				break;

			roiUtil3D.SetSelectionType(CROIUtilities3D::EResultSelectionType_Exclude);

			if((res = roiUtil3D.Execute()).IsFail())
				break;

			if((res = roiUtil3D.GetResult(flfaResultROIIndexExclude)).IsFail())
				break;

			if(flfaResultROIIndexInclude.GetCount())
			{
				int32_t i32IndexRes = 0;

				for(int32_t i = 0; i < i32ObjectCount; ++i)
				{
					CGUIView3DObject* pObj = (CGUIView3DObject*)pView3D->GetView3DObject(i);

					if(!pObj || !pObj->IsSelectionEnabled())
						continue;

					CFL3DObject* p3DObject = (CFL3DObject*)pObj->Get3DObject();

					if(!p3DObject)
						continue;

					CFLArray<int32_t>& flaCollisionIndex = flfaResultROIIndexInclude.GetAt(i32IndexRes);
					i32IndexRes++;

					if(flaCollisionIndex.IsEmpty())
						continue;

					int32_t i32CollisionIndexCount = (int32_t)flaCollisionIndex.GetCount();
					CFLArray<Base::TPoint3<uint8_t>>* pFlaVertexColor = (CFLArray<Base::TPoint3<uint8_t>>*)p3DObject->GetVertexColors();

					for(int32_t j = 0; j < i32CollisionIndexCount; ++j)
						pFlaVertexColor->SetAt(flaCollisionIndex.GetAt(j), TPoint3<uint8_t>(255, 0, 0)); // RED

					pObj->UpdateAll();
					pView3D->UpdateObject(i);
				}
			}

			if(flfaResultROIIndexExclude.GetCount())
			{
				int32_t i32IndexRes = 0;

				for(int32_t i = 0; i < i32ObjectCount; ++i)
				{
					CGUIView3DObject* pObj = (CGUIView3DObject*)pView3D->GetView3DObject(i);

					if(!pObj || !pObj->IsSelectionEnabled())
						continue;

					CFL3DObject* p3DObject = (CFL3DObject*)pObj->Get3DObject();

					if(!p3DObject)
						continue;

					CFLArray<int32_t>& flaCollisionIndex = flfaResultROIIndexExclude.GetAt(i32IndexRes);
					i32IndexRes++;

					if(flaCollisionIndex.IsEmpty())
						continue;

					int32_t i32CollisionIndexCount = (int32_t)flaCollisionIndex.GetCount();
					CFLArray<Base::TPoint3<uint8_t>>* pFlaVertexColor = (CFLArray<Base::TPoint3<uint8_t>>*)p3DObject->GetVertexColors();

					for(int32_t j = 0; j < i32CollisionIndexCount; ++j)
						pFlaVertexColor->SetAt(flaCollisionIndex.GetAt(j), TPoint3<uint8_t>(0, 0, 255)); // BLUE

					pObj->UpdateAll();
					pView3D->UpdateObject(i);
				}
			}

			pView3D->UpdateScreen();

			SetStatusMessage(GetResultString(EResult_OK));
		}
		while(false);
	};

	return pProcedure;
}

CPropertyItemButtonClickProcedure* FLImaging::GUI::CPropertyView3DExamples::FrustumCollisionTest()
{
	CPropertyItemButtonClickProcedure* pProcedure = new CPropertyItemButtonClickProcedure;

	*pProcedure = MakePropertyItemButtonClickProcedure
	{
		do
		{
			CGUIPropertyItemView3DList* pViewList = dynamic_cast<CGUIPropertyItemView3DList*>(FindItemByFullPath(L"ROI@3D View"));

			if(!pViewList)
				break;

			CGUIView3D* pView3D = pViewList->GetSelectedView3D();

			if(!pView3D)
			{
				SetStatusMessage(L"[Error] Select 3D view.");
				break;
			}

			int32_t i32ObjectCount = pView3D->GetObjectCount();

			if(!i32ObjectCount)
			{
				SetStatusMessage(L"[Error] " + pView3D->GetTitle() + L" doesn't have any 3D objects.");
				break;
			}

			CFLFigureArray flfaFrustum;
			pView3D->GetAllROI(&flfaFrustum);
			int64_t i64ROICount = flfaFrustum.GetCount();

			if(i64ROICount == 0)
			{
				SetStatusMessage(L"[Error] There is no ROI in " + pView3D->GetTitle() + L". Press Ctrl+3 to teach ROI.");
				break;
			}

			for(int32_t i = 0; i < i32ObjectCount; ++i)
			{
				CGUIView3DObject* pObj = (CGUIView3DObject*)pView3D->GetView3DObject(i);

				if(!pObj)
					continue;

				CFL3DObject* p3DObject = (CFL3DObject*)pObj->Get3DObject();

				if(!p3DObject)
					continue;

				bool bUpdate = false;

				for(int64_t i = 0; i < i64ROICount; ++i)
				{
					CFLFrustum3<float>* pFrustumROI = (CFLFrustum3<float>*)flfaFrustum.GetAt(i);
					
					if(!pFrustumROI)
						continue;

					CFLArray<int32_t> flaCollisionIndex;

					if(pFrustumROI->Contains(p3DObject->GetVertices(), flaCollisionIndex).IsFail())
						continue;

					int32_t i32CollisionIndexCount = (int32_t)flaCollisionIndex.GetCount();
					bUpdate |= i32CollisionIndexCount > 0 ? true : false;

					CFLArray<Base::TPoint3<uint8_t>>* pFlaVertexColor = (CFLArray<Base::TPoint3<uint8_t>>*)p3DObject->GetVertexColors();

					for(int32_t j = 0; j < i32CollisionIndexCount; ++j)
						pFlaVertexColor->SetAt(flaCollisionIndex.GetAt(j), TPoint3<uint8_t>(0, 255, 0)); // GREEN
				}

				if(bUpdate)
				{
					pObj->UpdateAll();
					pView3D->UpdateObject(i);
				}
			}

			pView3D->UpdateScreen();

			SetStatusMessage(GetResultString(EResult_OK));
		}
		while(false);
	};

	return pProcedure;
}

CPropertyItemButtonClickProcedure* FLImaging::GUI::CPropertyView3DExamples::SetCameraParameters()
{
	CPropertyItemButtonClickProcedure* pProcedure = new CPropertyItemButtonClickProcedure;

	*pProcedure = MakePropertyItemButtonClickProcedure
	{
		do
		{
			CGUIPropertyItemView3DList* pViewList = dynamic_cast<CGUIPropertyItemView3DList*>(FindItemByFullPath(L"Camera@3D View"));

			if(!pViewList)
				break;

			CGUIView3D* pView3D = pViewList->GetSelectedView3D();

			if(!pView3D)
			{
				SetStatusMessage(L"[Error] Select 3D view.");
				break;
			}

			CGUIPropertyItemText* pTextPX = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Position@x"));
			CGUIPropertyItemText* pTextPY = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Position@y"));
			CGUIPropertyItemText* pTextPZ = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Position@z"));

			CGUIPropertyItemText* pTextDX = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Direction@x"));
			CGUIPropertyItemText* pTextDY = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Direction@y"));
			CGUIPropertyItemText* pTextDZ = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Direction@z"));

			CGUIPropertyItemText* pTextUDX = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Up Direction@x"));
			CGUIPropertyItemText* pTextUDY = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Up Direction@y"));
			CGUIPropertyItemText* pTextUDZ = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Up Direction@z"));

			CGUIPropertyItemText* pTextTX = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Target@x"));
			CGUIPropertyItemText* pTextTY = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Target@y"));
			CGUIPropertyItemText* pTextTZ = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Target@z"));

			CGUIPropertyItemText* pTextAoV = dynamic_cast<CGUIPropertyItemText*>(FindItemByFullPath(L"Camera@Angle of View(Degree)"));

			// 3D View 에서 카메라 객체 얻어 오기
			// Gets the camera object from the 3D View.
			CGUIView3DCamera* pCamera = (CGUIView3DCamera*)pView3D->GetCamera();

			if(pCamera)
			{
				{
					float f32DX = (float)_wtof(pTextDX->GetValue());
					float f32DY = (float)_wtof(pTextDY->GetValue());
					float f32DZ = (float)_wtof(pTextDZ->GetValue());

					// 카메라가 바라보는 방향을 설정
					// Sets the direction that the camera is looking.
					pCamera->SetDirection(CFLPoint3<float>(f32DX, f32DY, f32DZ));
				}

				{
					float f32UDX = (float)_wtof(pTextUDX->GetValue());
					float f32UDY = (float)_wtof(pTextUDY->GetValue());
					float f32UDZ = (float)_wtof(pTextUDZ->GetValue());

					// 카메라의 위쪽 방향 벡터를 설정
					// Sets the up direction vector of the camera.
					pCamera->SetDirectionUp(CFLPoint3<float>(f32UDX, f32UDY, f32UDZ));
				}

				{
					float f32PX = (float)_wtof(pTextPX->GetValue());
					float f32PY = (float)_wtof(pTextPY->GetValue());
					float f32PZ = (float)_wtof(pTextPZ->GetValue());

					// 카메라의 포지션을 설정
					// Sets the position of the camera.
					pCamera->SetPosition(CFLPoint3<float>(f32PX, f32PY, f32PZ));
				}

				{
					float f32TX = (float)_wtof(pTextTX->GetValue());
					float f32TY = (float)_wtof(pTextTY->GetValue());
					float f32TZ = (float)_wtof(pTextTZ->GetValue());

					// 카메라가 바라보는 물체의 좌표를 설정
					// Sets the coordinates of the object that the camera is looking at.
					pCamera->SetTarget(CFLPoint3<float>(f32TX, f32TY, f32TZ));
				}

				{
					float f32AoV = (float)_wtof(pTextAoV->GetValue());

					// 카메라의 화각 설정
					// Sets the angle of view
					pCamera->SetAngleOfViewY(f32AoV, false);
				}

				// 뷰 업데이트 // Update the view	
				pView3D->UpdateScreen();

				CFLPoint3<float> flp3D = pCamera->GetDirection();
				CFLPoint3<float> flp3UD = pCamera->GetDirectionUp();
				CFLPoint3<float> flp3P = pCamera->GetPosition();
				CFLPoint3<float> flp3T = pCamera->GetTargetPosition();
				float f32Aov = pCamera->GetAngleOfViewY();

				pTextPX->SetValue(CFLString<wchar_t>().Format(L"%f", flp3P.x));
				pTextPY->SetValue(CFLString<wchar_t>().Format(L"%f", flp3P.y));
				pTextPZ->SetValue(CFLString<wchar_t>().Format(L"%f", flp3P.z));

				pTextDX->SetValue(CFLString<wchar_t>().Format(L"%f", flp3D.x));
				pTextDY->SetValue(CFLString<wchar_t>().Format(L"%f", flp3D.y));
				pTextDZ->SetValue(CFLString<wchar_t>().Format(L"%f", flp3D.z));

				pTextUDX->SetValue(CFLString<wchar_t>().Format(L"%f", flp3UD.x));
				pTextUDY->SetValue(CFLString<wchar_t>().Format(L"%f", flp3UD.y));
				pTextUDZ->SetValue(CFLString<wchar_t>().Format(L"%f", flp3UD.z));

				pTextTX->SetValue(CFLString<wchar_t>().Format(L"%f", flp3T.x));
				pTextTY->SetValue(CFLString<wchar_t>().Format(L"%f", flp3T.y));
				pTextTZ->SetValue(CFLString<wchar_t>().Format(L"%f", flp3T.z));

				pTextAoV->SetValue(CFLString<wchar_t>().Format(L"%f", f32Aov));
			}

			SetStatusMessage(GetResultString(EResult_OK));
		}
		while(false);
	};

	return pProcedure;
}
CPropertyItemButtonClickProcedure* FLImaging::GUI::CPropertyView3DExamples::FL3DObjectUtilitiesTest()
{
	CPropertyItemButtonClickProcedure* pProcedure = new CPropertyItemButtonClickProcedure;

	*pProcedure = MakePropertyItemButtonClickProcedure
	{
		do
		{
			CGUIPropertyItemView3DList* pViewListSource = dynamic_cast<CGUIPropertyItemView3DList*>(FindItemByFullPath(L"Transform@Source View"));
			CGUIPropertyItemView3DList* pViewListTarget = dynamic_cast<CGUIPropertyItemView3DList*>(FindItemByFullPath(L"Transform@Target View"));

			if(!pViewListSource || !pViewListTarget)
				break;

			CGUIView3D* pView3DSource = pViewListSource->GetSelectedView3D();
			CGUIView3D* pView3DTarget = pViewListTarget->GetSelectedView3D();

			if(!pView3DSource)
			{
				SetStatusMessage(L"[Error] Select source view.");
				break;
			}

			if(!pView3DTarget)
			{
				SetStatusMessage(L"[Error] Select target view.");
				break;
			}

			CGUIPropertyItemDropdownList* pPIDD = (CGUIPropertyItemDropdownList*)FindItemByFullPath(L"Transform@Transform Type");

			if(!pPIDD)
				break;

			// 화살표의 길이 // Height of the arrow.
			float f32Height = 30.f;
			// 화살표에서 뾰족한 부분(원뿔)이 차지하는 비율
			// Ratio of the arrowhead (cone) to the total arrow length.
			float f32ArrowRatio = 0.15f;
			// 화살표의 파이프 반지름 // Radius of the arrow's pipe.
			float f32PipeRadius = f32Height / 40.f;
			// 화살표의 뾰족한 부분(원뿔)의 반지름 // Radius of the arrowhead (cone).
			float f32ArrowBottomRadius = f32Height / 20.f;

			// CGUIView3DObject 객체 선언(임시 변수)
			// Declare CGUIView3DObject (temporary variable)
			CGUIView3DObject mesh1, mesh2;

			// CGUIView3DObject 객체의 토폴로지 타입을 솔리드로 설정
			// Set the topology type of the CGUIView3DObject to solid
			mesh1.SetTopologyType(ETopologyType3D_Solid);
			mesh2.SetTopologyType(ETopologyType3D_Solid);

			// 화살표 객체를 생성(파란색)
			// Create the arrow object(Blue color).
			GetArrowObject(mesh1, mesh2, TPoint3<float>(0, 0, 0), f32Height, f32ArrowRatio, f32PipeRadius, f32ArrowBottomRadius, BLUE);

			// CGUIView3DObject 객체를 뷰에 디스플레이
			// Display the CGUIView3DObject in the view
			pView3DSource->Clear();
			pView3DTarget->Clear();
			pView3DSource->PushObject(mesh1);
			pView3DSource->PushObject(mesh2);
			pView3DTarget->PushObject(mesh1);
			pView3DTarget->PushObject(mesh2);
			pView3DSource->ZoomFit();
			pView3DTarget->ZoomFit();

			pView3DSource->GetLayer(0)->DrawTextCanvas(&CFLPoint<double>(0, 0), L"Original Object", YELLOW, BLACK, 30);

			CFLString<wchar_t> str = pPIDD->GetValue();
			CResult res;

			if(str == L"Rotate")
			{
				res = RotateObject(pView3DTarget);
				pView3DTarget->GetLayer(0)->DrawTextCanvas(&CFLPoint<double>(0, 0), L"Rotated Object", YELLOW, BLACK, 30);
			}
			else if(str == L"Rotate with Pivot")
			{
				res = RotateObjectWithPivot(pView3DTarget);
				pView3DTarget->GetLayer(0)->DrawTextCanvas(&CFLPoint<double>(0, 0), L"Rotated Object", YELLOW, BLACK, 30);
			}
			else if(str == L"Translate")
			{
				res = TranslateObject(pView3DTarget);
				pView3DTarget->GetLayer(0)->DrawTextCanvas(&CFLPoint<double>(0, 0), L"Translated Object", YELLOW, BLACK, 30);
			}
			else if(str == L"Scale")
			{
				res = ScaleObject(pView3DTarget);
				pView3DTarget->GetLayer(0)->DrawTextCanvas(&CFLPoint<double>(0, 0), L"Scaled Object", YELLOW, BLACK, 30);
			}
			}
		while(false);
	};

	return pProcedure;

	return nullptr;
}

const CResult FLImaging::GUI::CPropertyView3DExamples::PushObjectPLY(CGUIView3D* pView3D)
{
	CResult res;

	do
	{
		if(!pView3D)
		{
			res = EResult_NullPointer;
			break;
		}

		pView3D->Clear();

		// CGUIView3DObject 객체 선언(임시 변수)
		// Declare CGUIView3DObject (temporary variable)
		CGUIView3DObject objTemp;

		// CGUIView3DObject 객체의 토폴로지 타입을 솔리드로 설정
		// Set the topology type of the CGUIView3DObject to solid
		objTemp.SetTopologyType(ETopologyType3D_Solid);

		// CGUIView3DObject 객체를 뷰에 디스플레이 했을 때, 객체의 인덱스를 얻어 오기 위한 변수 선언
		// Declare a variable to obtain the index of the object when the CGUIView3DObject is displayed in the view
		int32_t i32Index = -1;

		// CGUIView3DObject 객체를 뷰에 디스플레이
		// Display the CGUIView3DObject in the view
		if((res = pView3D->PushObject(objTemp, &i32Index)).IsFail())
			break;

		// 뷰에 디스플레이된 CGUIView3DObject 객체를 인덱스를 통해 얻어 오기
		// Retrieve the displayed CGUIView3DObject using the index
		CGUIView3DObject* pView3DObject = (CGUIView3DObject*)pView3D->GetView3DObject(i32Index);

		if(!pView3DObject)
		{
			res = EResult_NullPointer;
			break;
		}

		// 뷰에 디스플레이된 CGUIView3DObject 객체의 내부 데이터 포인터 얻어 오기
		// 버텍스, 컬러 등의 정보는 내부 데이터 객체인 CFL3DObject 안에 있음
		// Retrieve the internal data pointer of the displayed CGUIView3DObject
		// Information such as vertices, colors, etc., are inside the internal data object CFL3DObject
		CFL3DObject* p3DObject = (CFL3DObject*)pView3DObject->Get3DObject();

		// CFL3DObject 객체의 Load() 함수 호출을 통해 PLY 파일을 로드하여 
		// 객체에 버텍스, 컬러, 노멀, 페이스 등의 정보를 저장
		// Load the PLY file through the Load() function of the CFL3DObject object
		// and store information such as vertices, colors, normals, faces, etc., in the object
		if((res = p3DObject->Load(L"../../ExampleImages/View3DExamples/Bolt.ply")).IsFail())
			break;

		// CGUIView3DObject 객체의 내부 데이터 객체인 CFL3DObject 가 변경되었으므로 
		// 뷰에서도 변경 사항을 반영해서 디스플레이 하기 위해 업데이트 플래그 설정
		// Since the internal data object of CGUIView3DObject, CFL3DObject, has changed
		// set the update flag to reflect the changes in the view and display
		pView3DObject->UpdateAll();

		// 뷰에서 해당 객체를 업데이트
		// Update the object in the view
		pView3D->UpdateObject(i32Index);

		// 업데이트된 객체의 크기를 반영하여 뷰를 ZoomFit
		// ZoomFit the view to reflect the size of the updated object
		pView3D->ZoomFit();

		res = EResult_OK;
		SetStatusMessage(GetResultString(res));
	}
	while(false);

	return res;
}

const CResult FLImaging::GUI::CPropertyView3DExamples::PushObjectSphere(CGUIView3D* pView3D)
{
	CResult res;

	do
	{
		if(!pView3D)
		{
			res = EResult_NullPointer;
			break;
		}

		// CGUIView3DObject 객체 선언(임시 변수)
		// Declare CGUIView3DObject (temporary variable)
		CGUIView3DObject objTemp;

		// CGUIView3DObject 객체의 토폴로지 타입을 솔리드로 설정
		// Set the topology type of the CGUIView3DObject to solid
		objTemp.SetTopologyType(ETopologyType3D_Solid);

		// CGUIView3DObject 객체를 뷰에 디스플레이 했을 때, 객체의 인덱스를 얻어 오기 위한 변수 선언
		// Declare a variable to obtain the index of the object when the CGUIView3DObject is displayed in the view
		int32_t i32Index = -1;

		// CGUIView3DObject 객체를 뷰에 디스플레이
		// Display the CGUIView3DObject in the view
		if((res = pView3D->PushObject(objTemp, &i32Index)).IsFail())
			break;

		// 뷰에 디스플레이된 CGUIView3DObject 객체를 인덱스를 통해 얻어 오기
		// Retrieve the displayed CGUIView3DObject using the index
		CGUIView3DObject* pView3DObject = (CGUIView3DObject*)pView3D->GetView3DObject(i32Index);

		if(!pView3DObject)
		{
			res = EResult_NullPointer;
			break;
		}

		// 구의 반지름 
		// Sets the radius of the sphere.
		float f32Radius = 30.f;

		// 구의 슬라이스 개수. 값이 클수록 부드러운 구가 만들어집니다.
		// Sets the number of slices for the sphere. Higher values create a smoother sphere.
		uint32_t u32SliceCount = 30;

		// 구의 스택 개수. 값이 클수록 부드러운 구가 만들어집니다.
		// Sets the number of stacks for the sphere. Higher values create a smoother sphere.
		uint32_t u32StackCount = 30;

		// CGUIView3DObject 객체에 버텍스, 컬러, 노말, 페이스 정보를 설정
		// Sets the vertex, color, normal, and face information to the CGUIView3DObject object.
		if((res = GetSphereObject(*pView3DObject, f32Radius, u32SliceCount, u32StackCount)).IsFail())
			break;

		// CGUIView3DObject 객체에 쉐이딩 타입을 Phong shading 으로 설정
		// Sets the shading type of the CGUIView3DObject object to Phong shading.
		pView3DObject->SetShadingType(EShadingType3D_Phong);

		// CGUIView3DObject 객체의 내부 데이터 객체인 CFL3DObject 가 변경되었으므로 
		// 뷰에서도 변경 사항을 반영해서 디스플레이 하기 위해 업데이트 플래그 설정
		// Since the internal data object of CGUIView3DObject, CFL3DObject, has changed
		// set the update flag to reflect the changes in the view and display
		pView3DObject->UpdateVertex();
		pView3DObject->UpdateNormal();
		pView3DObject->UpdateColor();

		// 뷰에서 해당 객체를 업데이트
		// Update the object in the view
		pView3D->UpdateObject(i32Index);
		pView3D->UpdateScreen();

		res = EResult_OK;
		SetStatusMessage(GetResultString(res));
	}
	while(false);

	return res;
}

const CResult FLImaging::GUI::CPropertyView3DExamples::PushObjectHeightMap(CGUIView3D* pView3D)
{
	CResult res;

	do
	{
		if(!pView3D)
		{
			res = EResult_NullPointer;
			break;
		}

		// CGUIView3DObjectHeightMap 객체 선언(임시 변수)
		// Declare CGUIView3DObjectHeightMap (temporary variable)
		CGUIView3DObjectHeightMap objTemp;

		// CGUIView3DObjectHeightMap 객체의 토폴로지 타입을 솔리드로 설정
		// Set the topology type of the CGUIView3DObjectHeightMap to solid
		objTemp.SetTopologyType(ETopologyType3D_Solid);

		// CGUIView3DObjectHeightMap 객체를 뷰에 디스플레이 했을 때, 객체의 인덱스를 얻어 오기 위한 변수 선언
		// Declare a variable to obtain the index of the object when the CGUIView3DObjectHeightMap is displayed in the view
		int32_t i32Index = -1;

		// CGUIView3DObjectHeightMap 객체를 뷰에 디스플레이
		// Display the CGUIView3DObjectHeightMap in the view
		if((res = pView3D->PushObject(objTemp, &i32Index)).IsFail())
			break;

		// 뷰에 디스플레이된 CGUIView3DObjectHeightMap 객체를 인덱스를 통해 얻어 오기
		// Retrieve the displayed CGUIView3DObjectHeightMap using the index
		CGUIView3DObjectHeightMap* pObjHeightMap = (CGUIView3DObjectHeightMap*)pView3D->GetView3DObject(i32Index);

		if(!pObjHeightMap)
			break;

		// 뷰에 디스플레이된 CGUIView3DObjectHeightMap 객체의 내부 데이터 포인터 얻어 오기
		// 버텍스, 컬러 등의 정보는 내부 데이터 객체인 CFL3DObjectHeightMap 안에 있음
		// Retrieve the internal data pointer of the displayed CGUIView3DObjectHeightMap
		// Information such as vertices, colors, etc., are inside the internal data object CFL3DObjectHeightMap
		CFL3DObjectHeightMap* p3DObject = (CFL3DObjectHeightMap*)pObjHeightMap->Get3DObject();

		// 높이 맵으로 사용할 이미지 객체 선언
		// Declares an image object to be used as a height map.
		CFLImage fliHeightMap;

		// 높이 맵 파일 로드
		// Loads the height map file.
		fliHeightMap.Load(L"../../ExampleImages/View3DExamples/mountain.flif");

		// 이미지 객체를 CFL3DObjectHeightMap 객체에 Assign
		// Assigns the image object to a CFL3DObjectHeightMap object.
		p3DObject->Assign(fliHeightMap);

		// CGUIView3DObjectHeightMap 객체의 내부 데이터 객체인 CFL3DObjectHeightMap 이 변경되었으므로 
		// 뷰에서도 변경 사항을 반영해서 디스플레이 하기 위해 업데이트 플래그 설정
		// Since the internal data object of CGUIView3DObjectHeightMap, CFL3DObjectHeightMap, has changed
		// set the update flag to reflect the changes in the view and display
		pObjHeightMap->UpdateVertex();
		pObjHeightMap->UpdateColor();

		// 뷰에서 해당 객체를 업데이트
		// Update the object in the view
		pView3D->UpdateObject(i32Index);

		// 업데이트된 객체의 크기를 반영하여 뷰를 ZoomFit
		// ZoomFit the view to reflect the size of the updated object
		pView3D->ZoomFit();

		res = EResult_OK;
		SetStatusMessage(GetResultString(res));
	}
	while(false);

	return res;
}

const CResult FLImaging::GUI::CPropertyView3DExamples::PushObjectHeightMapAndTexture(CGUIView3D* pView3D)
{
	CResult res;

	do
	{
		if(!pView3D)
		{
			res = EResult_NullPointer;
			break;
		}

		// CGUIView3DObjectHeightMap 객체 선언(임시 변수)
		// Declare CGUIView3DObjectHeightMap (temporary variable)
		CGUIView3DObjectHeightMap objTemp;

		// CGUIView3DObjectHeightMap 객체의 토폴로지 타입을 솔리드로 설정
		// Set the topology type of the CGUIView3DObjectHeightMap to solid
		objTemp.SetTopologyType(ETopologyType3D_Solid);

		// CGUIView3DObjectHeightMap 객체를 뷰에 디스플레이 했을 때, 객체의 인덱스를 얻어 오기 위한 변수 선언
		// Declare a variable to obtain the index of the object when the CGUIView3DObjectHeightMap is displayed in the view
		int32_t i32Index = -1;

		// CGUIView3DObjectHeightMap 객체를 뷰에 디스플레이
		// Display the CGUIView3DObjectHeightMap in the view
		if((res = pView3D->PushObject(objTemp, &i32Index)).IsFail())
			break;

		// 뷰에 디스플레이된 CGUIView3DObjectHeightMap 객체를 인덱스를 통해 얻어 오기
		// Retrieve the displayed CGUIView3DObjectHeightMap using the index
		CGUIView3DObjectHeightMap* pObjHeightMap = (CGUIView3DObjectHeightMap*)pView3D->GetView3DObject(i32Index);

		if(!pObjHeightMap)
		{
			res = EResult_NullPointer;
			break;
		}

		// 뷰에 디스플레이된 CGUIView3DObjectHeightMap 객체의 내부 데이터 포인터 얻어 오기
		// 버텍스, 컬러 등의 정보는 내부 데이터 객체인 CFL3DObjectHeightMap 안에 있음
		// Retrieve the internal data pointer of the displayed CGUIView3DObjectHeightMap
		// Information such as vertices, colors, etc., are inside the internal data object CFL3DObjectHeightMap
		CFL3DObjectHeightMap* p3DObject = (CFL3DObjectHeightMap*)pObjHeightMap->Get3DObject();

		// 높이 맵으로 사용할 이미지 객체 선언
		// Declares an image object to be used as a height map.
		CFLImage fliHeightMap;

		// 높이 맵 파일 로드
		// Loads the height map file.
		fliHeightMap.Load(L"../../ExampleImages/View3DExamples/mountain.flif");

		// 텍스쳐로 사용할 이미지 객체 선언
		// Declares an image object to be used as a texture
		CFLImage fliTexture;

		// 텍스쳐 파일 로드
		// Loads the texture file.
		fliTexture.Load(L"../../ExampleImages/View3DExamples/mountain_texture.flif");

		// 높이 맵 이미지 객체를 CFL3DObjectHeightMap 객체에 Assign
		// Assigns the height map image object to a CFL3DObjectHeightMap object.
		p3DObject->Assign(fliHeightMap);

		// 텍스쳐 이미지 객체를 CFL3DObjectHeightMap 객체에 Assign
		// Assigns the texture image object to a CFL3DObjectHeightMap object.
		p3DObject->SetTextureImage(fliTexture);
		p3DObject->ActivateVertexColorTexture(true);

		// CGUIView3DObjectHeightMap 객체의 내부 데이터 객체인 CFL3DObjectHeightMap 이 변경되었으므로 
		// 뷰에서도 변경 사항을 반영해서 디스플레이 하기 위해 업데이트 플래그 설정
		// Since the internal data object of CGUIView3DObjectHeightMap, CFL3DObjectHeightMap, has changed
		// set the update flag to reflect the changes in the view and display
		pObjHeightMap->UpdateVertex();
		pObjHeightMap->UpdateColor();

		// 뷰에서 해당 객체를 업데이트
		// Update the object in the view
		pView3D->UpdateObject(i32Index);

		// 업데이트된 객체의 크기를 반영하여 뷰를 ZoomFit
		// ZoomFit the view to reflect the size of the updated object
		pView3D->ZoomFit();

		res = EResult_OK;
		SetStatusMessage(GetResultString(res));
	}
	while(false);

	return res;
}

const CResult FLImaging::GUI::CPropertyView3DExamples::PushObjectLine(CGUIView3D* pView3D)
{
	CResult res;

	do
	{
		if(!pView3D)
		{
			res = EResult_NullPointer;
			break;
		}

		// 파라미터 순서 : 라인의 시작점 -> 라인의 끝점 -> 라인 색상 -> 라인 두께 -> 펜 스타일
		// Parameter Order: Start Point of Line -> End Point of Line -> Line Color -> Line Thickness -> Pen Style
		pView3D->PushObject(CGUIView3DObjectLine(TPoint3<float>(0, 10, 0), TPoint3<float>(40, 10, 0), WHITE, 6.f, EGUIViewImagePenStyle_Solid));
		pView3D->PushObject(CGUIView3DObjectLine(TPoint3<float>(0, 20, 0), TPoint3<float>(40, 20, 0), YELLOW, 2.f, EGUIViewImagePenStyle_Dash));
		pView3D->PushObject(CGUIView3DObjectLine(TPoint3<float>(0, 30, 0), TPoint3<float>(40, 30, 0), VIOLET, 3.f, EGUIViewImagePenStyle_Dot));

		// 업데이트된 객체의 크기를 반영하여 뷰를 ZoomFit
		// ZoomFit the view to reflect the size of the updated object
		pView3D->ZoomFit();

		// 3D View 에서 카메라 객체 얻어 오기
		// Gets the camera object from the 3D View.
		CGUIView3DCamera* pCamera = (CGUIView3DCamera*)pView3D->GetCamera();

		if(pCamera)
		{
			// 추가한 라인을 잘 볼 수 있는 시점으로 이동
			// Moves the viewpoint to a good position to see the added line.

			// 카메라가 바라보는 방향을 설정
			// Sets the direction that the camera is looking.
			pCamera->SetDirection(CFLPoint3<float>(0, 0, -1));

			// 카메라의 위쪽 방향 벡터를 설정
			// Sets the up direction vector of the camera.
			pCamera->SetDirectionUp(CFLPoint3<float>(0, 1, 0));

			// 카메라의 포지션을 설정
			// Sets the position of the camera.
			pCamera->SetPosition(CFLPoint3<float>(20, 20, 100));

			// 카메라가 바라보는 물체의 좌표를 설정
			// Sets the coordinates of the object that the camera is looking at.
			pCamera->SetTarget(CFLPoint3<float>(20, 20, 0));
		}

		res = EResult_OK;
		SetStatusMessage(GetResultString(res));
	}
	while(false);

	return res;
}

const CResult FLImaging::GUI::CPropertyView3DExamples::PushObjectUnselectableAxis(CGUIView3D* pView3D)
{
	CResult res;

	do
	{
		if(!pView3D)
		{
			res = EResult_NullPointer;
			break;
		}

		// 직접 추가한 3D Axis 를 띄우기 위해 기존의 디폴트 Axis 는 숨기기 처리
		// Hides the default axis to display the custom 3D axis.
		pView3D->ShowAxis(false);

		// 화살표의 길이 // Height of the arrow.
		float f32Height = 30.f;
		// 화살표에서 뾰족한 부분(원뿔)이 차지하는 비율
		// Ratio of the arrowhead (cone) to the total arrow length.
		float f32ArrowRatio = 0.15f;
		// 화살표의 파이프 반지름 // Radius of the arrow's pipe.
		float f32PipeRadius = f32Height / 40.f;
		// 화살표의 뾰족한 부분(원뿔)의 반지름 // Radius of the arrowhead (cone).
		float f32ArrowBottomRadius = f32Height / 20.f;

		// CGUIView3DObject 객체 선언(임시 변수)
		// Declare CGUIView3DObject (temporary variable)
		CGUIView3DObject mesh1, mesh2;

		// CGUIView3DObject 객체의 토폴로지 타입을 솔리드로 설정
		// Set the topology type of the CGUIView3DObject to solid
		mesh1.SetTopologyType(ETopologyType3D_Solid);
		mesh2.SetTopologyType(ETopologyType3D_Solid);

		// 이 CGUIView3DObject 객체가 ROI 또는 Crop Object 연산 시 무시되도록 선택이 불가하게 처리
		// Make these CGUIView3DObjects unselectable for ROI or Crop Object operations.
		mesh1.EnableSelection(false);
		mesh2.EnableSelection(false);

		// X 축 // Axis x
		{
			// 화살표 객체를 생성(빨간색)
			// Create the arrow object(Red color).
			GetArrowObject(mesh1, mesh2, TPoint3<float>(0, 0, 0), f32Height, f32ArrowRatio, f32PipeRadius, f32ArrowBottomRadius, RED);

			// CFL3DObjectUtilities 를 사용하여 간편하게 회전
			// 파라미터 순서 : 시작 지점 벡터 -> 종료 지점 벡터 -> 회전할 객체
			// 시작 벡터에서 종료 벡터사이의 각도만큼 회전됩니다.
			// Rotate the arrow using CFL3DObjectUtilities.
			// Parameter order: start point vector -> end point vector -> object to rotate
			// Rotates by the angle between the start and end vectors.
			CFL3DObjectUtilities::Rotate(*(Foundation::CFL3DObject*)mesh1.Get3DObject(), CFLGeometry3DVector<float>(0, 0, 1), CFLGeometry3DVector<float>(1, 0, 0));
			CFL3DObjectUtilities::Rotate(*(Foundation::CFL3DObject*)mesh2.Get3DObject(), CFLGeometry3DVector<float>(0, 0, 1), CFLGeometry3DVector<float>(1, 0, 0));

			// CGUIView3DObject 객체를 뷰에 디스플레이
			// Display the CGUIView3DObject in the view
			pView3D->PushObject(mesh1);
			pView3D->PushObject(mesh2);

			// 다음 축(Y축)을 그리기 위해 클리어
			// Clear the objects to prepare for drawing the next axis (Y axis).
			((Foundation::CFL3DObject*)mesh1.Get3DObject())->Clear();
			((Foundation::CFL3DObject*)mesh2.Get3DObject())->Clear();
		}

		// Y 축 // Axis y
		{
			// 화살표 객체를 생성(초록색)
			// Create the arrow object(Green color).
			GetArrowObject(mesh1, mesh2, TPoint3<float>(0, 0, 0), f32Height, f32ArrowRatio, f32PipeRadius, f32ArrowBottomRadius, GREEN);

			// CFL3DObjectUtilities 를 사용하여 간편하게 회전
			// 파라미터 순서 : 시작 지점 벡터 -> 종료 지점 벡터 -> 회전할 객체
			// 시작 벡터에서 종료 벡터사이의 각도만큼 회전됩니다.
			// Rotate the arrow using CFL3DObjectUtilities.
			// Parameter order: start point vector -> end point vector -> object to rotate
			// Rotates by the angle between the start and end vectors.
			CFL3DObjectUtilities::Rotate(*(Foundation::CFL3DObject*)mesh1.Get3DObject(), CFLGeometry3DVector<float>(0, 0, 1), CFLGeometry3DVector<float>(0, 1, 0));
			CFL3DObjectUtilities::Rotate(*(Foundation::CFL3DObject*)mesh2.Get3DObject(), CFLGeometry3DVector<float>(0, 0, 1), CFLGeometry3DVector<float>(0, 1, 0));

			// CGUIView3DObject 객체를 뷰에 디스플레이
			// Display the CGUIView3DObject in the view
			pView3D->PushObject(mesh1);
			pView3D->PushObject(mesh2);

			// 다음 축(Z축)을 그리기 위해 클리어
			// Clear the objects to prepare for drawing the next axis (Z axis).
			((Foundation::CFL3DObject*)mesh1.Get3DObject())->Clear();
			((Foundation::CFL3DObject*)mesh2.Get3DObject())->Clear();
		}

		// Z 축 // Axis z
		{
			// 화살표 객체를 생성(파란색)
			// Create the arrow object(Blue color).
			GetArrowObject(mesh1, mesh2, TPoint3<float>(0, 0, 0), f32Height, f32ArrowRatio, f32PipeRadius, f32ArrowBottomRadius, BLUE);

			// CGUIView3DObject 객체를 뷰에 디스플레이
			// Display the CGUIView3DObject in the view
			pView3D->PushObject(mesh1);
			pView3D->PushObject(mesh2);
		}

		pView3D->ZoomFit();

		res = EResult_OK;
		SetStatusMessage(GetResultString(res));
	}
	while(false);

	return res;
}

const CResult FLImaging::GUI::CPropertyView3DExamples::RotateObject(CGUIView3D* pView3D)
{
	CResult res;

	do
	{
		if(!pView3D)
		{
			res = EResult_NullPointer;
			break;
		}

		int32_t i32ObjectCount = pView3D->GetObjectCount();

		if(!i32ObjectCount)
		{
			SetStatusMessage(L"[Error] " + pView3D->GetTitle() + L" doesn't have any 3D objects.");
			break;
		}

		for(int32_t i = 0; i < i32ObjectCount; ++i)
		{
			CGUIView3DObject* pObj = (CGUIView3DObject*)pView3D->GetView3DObject(i);

			if(!pObj || !pObj->IsSelectionEnabled())
				continue;

			CFL3DObject* p3DObject = (CFL3DObject*)pObj->Get3DObject();

			if(!p3DObject)
				continue;

			// CFL3DObjectUtilities 를 사용하여 간편하게 회전
			// 파라미터 순서 : 시작 지점 벡터 -> 종료 지점 벡터 -> 회전할 객체
			// 시작 벡터에서 종료 벡터사이의 각도만큼 회전됩니다.
			// Rotate the object using CFL3DObjectUtilities.
			// Parameter order: start point vector -> end point vector -> object to rotate
			// Rotates by the angle between the start and end vectors.
			CFL3DObjectUtilities::Rotate(*p3DObject, CFLGeometry3DVector<float>(0, 0, 1), CFLGeometry3DVector<float>(1, 0, 0));

			pObj->UpdateVertex();
			pView3D->UpdateObject(i);
		}

		res = EResult_OK;
		SetStatusMessage(GetResultString(res));
	}
	while(false);

	return res;
}

const CResult FLImaging::GUI::CPropertyView3DExamples::RotateObjectWithPivot(CGUIView3D* pView3D)
{
	CResult res;

	do
	{
		if(!pView3D)
		{
			res = EResult_NullPointer;
			break;
		}

		int32_t i32ObjectCount = pView3D->GetObjectCount();

		if(!i32ObjectCount)
		{
			SetStatusMessage(L"[Error] " + pView3D->GetTitle() + L" doesn't have any 3D objects.");
			break;
		}

		for(int32_t i = 0; i < i32ObjectCount; ++i)
		{
			CGUIView3DObject* pObj = (CGUIView3DObject*)pView3D->GetView3DObject(i);

			if(!pObj || !pObj->IsSelectionEnabled())
				continue;

			CFL3DObject* p3DObject = (CFL3DObject*)pObj->Get3DObject();

			if(!p3DObject)
				continue;

			// 피봇을 중심으로 회전
			// An example of rotating around a pivot 
			CFL3DObjectUtilities::Rotate(*p3DObject,
										 CFLGeometry3DQuaternion<float>(CFLGeometry3DVector<float>(0.f, 1.f, 0.f),
																		CFLGeometry3DVector<float>(0.f, 0.5f, 0.5f)), 
										 CFLPoint3<float>(0, 0, 30) // Pivot
										 );

			pObj->UpdateVertex();
			pView3D->UpdateObject(i);
		}

		res = EResult_OK;
		SetStatusMessage(GetResultString(res));
	}
	while(false);

	return res;
}

const CResult FLImaging::GUI::CPropertyView3DExamples::TranslateObject(CGUIView3D* pView3D)
{
	CResult res;

	do
	{
		if(!pView3D)
		{
			res = EResult_NullPointer;
			break;
		}

		int32_t i32ObjectCount = pView3D->GetObjectCount();

		if(!i32ObjectCount)
		{
			SetStatusMessage(L"[Error] " + pView3D->GetTitle() + L" doesn't have any 3D objects.");
			break;
		}

		for(int32_t i = 0; i < i32ObjectCount; ++i)
		{
			CGUIView3DObject* pObj = (CGUIView3DObject*)pView3D->GetView3DObject(i);

			if(!pObj || !pObj->IsSelectionEnabled())
				continue;

			CFL3DObject* p3DObject = (CFL3DObject*)pObj->Get3DObject();

			if(!p3DObject)
				continue;

			// CFL3DObjectUtilities 를 사용하여 간편하게 이동
			// Translate the object using CFL3DObjectUtilities.
			CFL3DObjectUtilities::Translate(*p3DObject, CFLGeometry3DVector<float>(4, 0, 0));

			pObj->UpdateVertex();
			pView3D->UpdateObject(i);
		}

		res = EResult_OK;
		SetStatusMessage(GetResultString(res));
	}
	while(false);

	return res;
}

const CResult FLImaging::GUI::CPropertyView3DExamples::ScaleObject(CGUIView3D* pView3D)
{
	CResult res;

	do
	{
		if(!pView3D)
		{
			res = EResult_NullPointer;
			break;
		}

		int32_t i32ObjectCount = pView3D->GetObjectCount();

		if(!i32ObjectCount)
		{
			SetStatusMessage(L"[Error] " + pView3D->GetTitle() + L" doesn't have any 3D objects.");
			break;
		}

		for(int32_t i = 0; i < i32ObjectCount; ++i)
		{
			CGUIView3DObject* pObj = (CGUIView3DObject*)pView3D->GetView3DObject(i);

			if(!pObj || !pObj->IsSelectionEnabled())
				continue;

			CFL3DObject* p3DObject = (CFL3DObject*)pObj->Get3DObject();

			if(!p3DObject)
				continue;

			// CFL3DObjectUtilities 를 사용하여 간편하게 스케일
			// Scales the object using CFL3DObjectUtilities.
			CFL3DObjectUtilities::Scale(*p3DObject, 2.f);

			pObj->UpdateVertex();
			pView3D->UpdateObject(i);
		}

		res = EResult_OK;
		SetStatusMessage(GetResultString(res));
	}
	while(false);

	return res;
}

const CResult FLImaging::GUI::CPropertyView3DExamples::GetSphereObject(CGUIView3DObject& meshSphere, float f32Radius, uint32_t u32SliceCount, uint32_t u32StackCount)
{
	CResult cReturn;

	do
	{
		SFaceDataTangent sFaceData;

		if((cReturn = CGUIView3DUtils::CreateSphere(f32Radius, u32SliceCount, u32StackCount, sFaceData)).IsFail())
			break;

		CFL3DObject* pObjectData = (CFL3DObject*)meshSphere.Get3DObject();
		int32_t i32VertexCount = (int32_t)sFaceData.m_vctVertex.size();
		int32_t i32IndexCount = (int32_t)sFaceData.m_vctIndices32.size();

		CFLArray<TPoint3<float>>* pFlaVertices = (CFLArray<TPoint3<float>>*)pObjectData->GetVertices();

		if(!pFlaVertices->Resize(i32VertexCount))
		{
			cReturn = EResult_NotEnoughMemory;
			break;
		}

		CFLArray<TPoint3<uint8_t>>* pFlaVertexColors = (CFLArray<TPoint3<uint8_t>>*)pObjectData->GetVertexColors();

		if(!pFlaVertexColors->Resize(i32VertexCount))
		{
			cReturn = EResult_NotEnoughMemory;
			break;
		}

		CFLArray<TPoint3<float>>* pFlaVertexNormal = (CFLArray<TPoint3<float>>*)pObjectData->GetVertexNormals();

		if(!pFlaVertexNormal->Resize(i32VertexCount))
		{
			cReturn = EResult_NotEnoughMemory;
			break;
		}

		CFLArray<int32_t>* pFlaFaces = (CFLArray<int32_t>*)pObjectData->GetFaceIndices();
		pFlaFaces->Reserve(i32IndexCount);

		for(int32_t i = 0; i < (int32_t)sFaceData.m_vctVertex.size(); ++i)
		{
			auto& pt3 = sFaceData.m_vctVertex[i];
			TPoint3<float>& tpVertex = pFlaVertices->GetAt(i);
			TPoint3<uint8_t>& tpColor = pFlaVertexColors->GetAt(i);
			TPoint3<float>& tpNormal = pFlaVertexNormal->GetAt(i);
			tpVertex = pt3.position;
			tpNormal = pt3.normal;
			tpColor.x = 155;
			tpColor.y = 230;
			tpColor.z = 255;
		}

		for(int32_t i = 0; i < (int32_t)sFaceData.m_vctIndices32.size(); ++i)
			pFlaFaces->PushBack((int32_t)sFaceData.m_vctIndices32[i]);

		cReturn = EResult_OK;
	}
	while(false);

	return cReturn;
}

const CResult FLImaging::GUI::CPropertyView3DExamples::GetArrowObject(CGUIView3DObject& mesh1, CGUIView3DObject& mesh2, const TPoint3<float>& tp3Origin, float f32Length, float f32ArrowRatio, float f32PipeRadius, float f32ArrowBottomRadius, COLORREF clr)
{
	CResult cReturn;

	do
	{
		// color
		uint8_t r = GetRValue(clr);
		uint8_t g = GetGValue(clr);
		uint8_t b = GetBValue(clr);

		SFaceDataTangent sFaceData1;
		float ArrowHeight = f32Length * f32ArrowRatio;
		float PipeHeight = f32Length * (1.f - f32ArrowRatio);

		// origin
		float x = tp3Origin.x;
		float y = tp3Origin.y;
		float z = tp3Origin.z + (PipeHeight + ArrowHeight * 0.5f);

		uint32_t sliceCount = 20;
		uint32_t stackCount = 10;

		cReturn = CGUIView3DUtils::CreateCylinder(f32ArrowBottomRadius, 0.f, ArrowHeight, x, y, z, sliceCount, stackCount, sFaceData1);

		if(cReturn.IsFail())
			break;

		SFaceDataTangent sFaceData2;

		// origin
		x = tp3Origin.x;
		y = tp3Origin.y;
		z = tp3Origin.z + PipeHeight * 0.5f;

		cReturn = CGUIView3DUtils::CreateCylinder(f32PipeRadius, f32PipeRadius, PipeHeight, x, y, z, sliceCount, stackCount, sFaceData2);

		if(cReturn.IsFail())
			break;

		CFL3DObject* pObjectData1 = (CFL3DObject*)mesh1.Get3DObject();
		CFL3DObject* pObjectData2 = (CFL3DObject*)mesh2.Get3DObject();

		int32_t i32VertexCount1 = (int32_t)sFaceData1.m_vctVertex.size();
		int32_t i32VertexCount2 = (int32_t)sFaceData2.m_vctVertex.size();
		int32_t i32IndexCount1 = (int32_t)sFaceData1.m_vctIndices32.size() / 3;
		int32_t i32IndexCount2 = (int32_t)sFaceData2.m_vctIndices32.size() / 3;

		CFLArray<TPoint3<float>>* pFlaVertices1 = (CFLArray<TPoint3<float>>*)pObjectData1->GetVertices();

		if(!pFlaVertices1->Resize(i32VertexCount1))
		{
			cReturn = EResult_NotEnoughMemory;
			break;
		}

		CFLArray<TPoint3<uint8_t>>* pFlaVertexColors1 = (CFLArray<TPoint3<uint8_t>>*)pObjectData1->GetVertexColors();

		if(!pFlaVertexColors1->Resize(i32VertexCount1))
		{
			cReturn = EResult_NotEnoughMemory;
			break;
		}

		CFLArray<int32_t>* pFlaFaces1 = (CFLArray<int32_t>*)pObjectData1->GetFaceIndices();

		pFlaFaces1->Reserve(i32IndexCount1);

		for(int32_t i = 0; i < (int32_t)sFaceData1.m_vctVertex.size(); ++i)
		{
			auto& pt3 = sFaceData1.m_vctVertex[i];
			TPoint3<float>& tpVertex = pFlaVertices1->GetAt(i);
			tpVertex = TPoint3<float>(pt3.position.x, pt3.position.y, pt3.position.z);

			TPoint3<uint8_t>& tpColor = pFlaVertexColors1->GetAt(i);
			tpColor = TPoint3<uint8_t>(r, g, b);
		}

		for(int32_t i = 0; i < (int32_t)sFaceData1.m_vctIndices32.size(); ++i)
			pFlaFaces1->PushBack((int32_t)sFaceData1.m_vctIndices32[i]);


		CFLArray<TPoint3<float>>* pFlaVertices2 = (CFLArray<TPoint3<float>>*)pObjectData2->GetVertices();

		if(!pFlaVertices2->Resize(i32VertexCount2))
		{
			cReturn = EResult_NotEnoughMemory;
			break;
		}

		CFLArray<TPoint3<uint8_t>>* pFlaVertexColors2 = (CFLArray<TPoint3<uint8_t>>*)pObjectData2->GetVertexColors();

		if(!pFlaVertexColors2->Resize(i32VertexCount2))
		{
			cReturn = EResult_NotEnoughMemory;
			break;
		}

		CFLArray<int32_t>* pFlaFaces2 = (CFLArray<int32_t>*)pObjectData2->GetFaceIndices();

		pFlaFaces2->Reserve(i32IndexCount2);

		for(int32_t i = 0; i < (int32_t)sFaceData2.m_vctVertex.size(); ++i)
		{
			auto& pt3 = sFaceData2.m_vctVertex[i];
			TPoint3<float>& tpVertex = pFlaVertices2->GetAt(i);
			tpVertex = TPoint3<float>(pt3.position.x, pt3.position.y, pt3.position.z);

			TPoint3<uint8_t>& tpColor = pFlaVertexColors2->GetAt(i);
			tpColor = TPoint3<uint8_t>(r, g, b);
		}

		for(int32_t i = 0; i < (int32_t)sFaceData2.m_vctIndices32.size(); ++i)
			pFlaFaces2->PushBack((int32_t)sFaceData2.m_vctIndices32[i]);

		cReturn = EResult_OK;
	}
	while(false);

	return cReturn;
}
