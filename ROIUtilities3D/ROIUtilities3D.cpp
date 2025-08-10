#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"
#include "../CommonHeader/ErrorMessageBox.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 뷰 선언
	// Declaration of the 3D view 
	CGUIView3DWrap view3DSrc;
	CGUIView3DWrap view3DInclude;
	CGUIView3DWrap view3DExclude;
	CGUIView3DWrap view3DAdd;
	CGUIView3DWrap view3DRemove;
	CGUIView3DWrap view3DXOR;
	CGUIView3DWrap* arrView3D[6] = { &view3DSrc, &view3DInclude, &view3DExclude, &view3DAdd, &view3DRemove, &view3DXOR };

	CResult res = EResult_UnknownError;

	do
	{
		// 3D 뷰 생성 // Create the 3D view
		view3DInclude.Create(0, 0, 300, 300); // L, T, R, B(Left, Top, Right, Bottom) 
		view3DSrc.Create(300, 0, 600, 300);
		view3DExclude.Create(600, 0, 900, 300);
		view3DAdd.Create(0, 300, 300, 600);
		view3DRemove.Create(300, 300, 600, 600);
		view3DXOR.Create(600, 300, 900, 600);

		// 3D 객체 선언 // Declare a 3D object
		CFL3DObject fl3DObjLeft, fl3DObjRight;
		// 3D 객체 로드 // Load a 3D object
		res = fl3DObjLeft.Load(L"../../ExampleImages/ROIUtilities3D/Left Cam.ply");
		res = fl3DObjRight.Load(L"../../ExampleImages/ROIUtilities3D/Right Cam.ply");
		
		for(int32_t i = 0; i < 6; ++i)
		{
			// 3D 뷰에 3D 객체 추가 // Add 3D objects to the 3D view
			arrView3D[i]->PushObject(fl3DObjLeft);
			arrView3D[i]->PushObject(fl3DObjRight);

			// 추가한 3D 객체가 화면 안에 들어오도록 Zoom Fit // Perform Zoom Fit to ensure added 3D objects are within the view
			arrView3D[i]->ZoomFit();

			// 3D 뷰어의 시점(카메라) 변경 // Change the viewpoint (camera) of the 3D viewer
			arrView3D[i]->GetCamera()->SetPosition(CFLPoint3F(0.71, 0.02, 10.94));
			arrView3D[i]->GetCamera()->SetDirectionUp(CFLPoint3F(1., 0., 0.));

			if(i > 0)
			{
				// 3D 뷰 시점 동기화 // Synchronize the viewpoint of the 3D view
				arrView3D[i]->SynchronizePointOfView(arrView3D[i - 1]);
				// 윈도우 동기화 // Synchronize the window
				arrView3D[i]->SynchronizeWindow(arrView3D[i - 1]);
			}
		}

		// 절두체 ROI 선언 // Declare the frustum ROI
		CFLFrustum3<float> flfr;
		// 파일에서 절두체 ROI 로드 // Load the frustum ROI from a file
		flfr.Load(L"../../ExampleImages/ROIUtilities3D/frustumROI.fig");

		// 3D 뷰에 ROI 추가 // Add the ROI to the 3D view
		for(int32_t i = 1; i < 6; ++i)
			arrView3D[i]->PushBackROI(&flfr);

		// CROIUtilities3D 객체 선언 // Declare the CROIUtilities3D object
		CROIUtilities3D roiUtilities3D;

		// CROIUtilities3D 객체에 3D Object 추가 // Add 3D objects to the CROIUtilities3D object
		roiUtilities3D.PushBack3DObject(&fl3DObjLeft);
		roiUtilities3D.PushBack3DObject(&fl3DObjRight);

		// CROIUtilities3D 객체에 절두체 ROI 추가 // Add the frustum ROI to the CROIUtilities3D object
		roiUtilities3D.PushBackROI(flfr);

		// 선택 타입 설정 : ROI 안에 포함되는 정점만 선택 // Set the selection type to include only vertices inside the ROI
		roiUtilities3D.SetSelectionType(CROIUtilities3D::EResultSelectionType_Include);

		// CROIUtilities3D 실행 // Execute the CROIUtilities3D object
		if((res = roiUtilities3D.Execute()).IsFail())
			break;

		// CROIUtilities3D 에서 결과 얻어 오기 // Retrieve the results from CROIUtilities3D
		CFLArray<CFLArray<int32_t>> flfaResultROIIndexInclude;
		if((res = roiUtilities3D.GetResult(flfaResultROIIndexInclude)).IsFail())
			break;

		// EResultSelectionType.Add 연산을 위해 CROIUtilities3D 객체 선언 및 roiUtilities3D 를 복사 생성. Include 연산으로 얻은 결과값까지 복사됨
		// Declare and copy construct a CROIUtilities3D object for the EResultSelectionType.Add operation. 
		// The results from the Include operation are also copied.
		CROIUtilities3D roiUtilities3DAdd = roiUtilities3D;

		// 복사한 객체에서 ROI를 모두 클리어 // Clear all ROIs from the copied object
		roiUtilities3DAdd.ClearROI();

		if(flfaResultROIIndexInclude.GetCount())
		{
			int32_t i32ObjectIdx = 0;

			// 3D 뷰어에 추가된 3D 객체의 개수 // Number of 3D objects added to the 3D viewer
			int32_t i32ObjectCount = view3DInclude.GetObjectCount();

			for(int32_t i = 0; i < i32ObjectCount; ++i)
			{
				// 3D 뷰어에 추가된 i번째 3D 객체 // The i-th 3D object added to the 3D viewer
				CGUIView3DObject* pObj = (CGUIView3DObject*)view3DInclude.GetView3DObject(i);

				// 해당 객체가 없거나, 해당 객체에 대해 선택이 비활성화 되어 있다면 continue // Skip if the object is null or selection is disabled
				if(!pObj || !pObj->IsSelectionEnabled())
					continue;

				// i번째 3D 객체의 데이터(CFL3DObject) // Data of the i-th 3D object (CFL3DObject)
				CFL3DObject* pObjData = (CFL3DObject*)pObj->Get3DObject();

				// 해당 객체가 없다면 continue // Skip if the object data is null
				if(!pObjData)
					continue;

				// i번째 3D 객체에 대한 결과값 배열. 이 배열은 i번째 3D 객체에 대해, ROI 내부에 위치한 정점의 인덱스로 이루어짐 // Result array for the i-th 3D object. Contains indices of vertices within the ROI.
				CFLArray<int32_t>& flaCollisionIndex = flfaResultROIIndexInclude.GetAt(i32ObjectIdx);
				i32ObjectIdx++;

				if(flaCollisionIndex.IsEmpty())
					continue;

				int32_t i32CollisionIndexCount = (int32_t)flaCollisionIndex.GetCount();
				CFLArray<Base::TPoint3<uint8_t>>* pFlaVertexColor = (CFLArray<Base::TPoint3<uint8_t>>*)pObjData->GetVertexColors();

				// i번째 3D 객체에 대해, ROI 내부에 위치한 정점을 빨간색으로 표시 // Mark vertices within the ROI of the i-th 3D object in red
				for(int32_t j = 0; j < i32CollisionIndexCount; ++j)
					pFlaVertexColor->SetAt(flaCollisionIndex.GetAt(j), TPoint3<uint8_t>(255, 0, 0)); // RED

				// 3D 뷰어에 추가된 i번째 3D 객체에 대해 렌더링 업데이트 // Update rendering for the i-th 3D object added to the 3D viewer
				pObj->UpdateAll();
				view3DInclude.UpdateObject(i);
			}

			// 3D 뷰어 업데이트 // Update the 3D viewer
			view3DInclude.UpdateScreen();
		}

		// 선택 타입 설정 : ROI 바깥의 정점만 선택 // Set selection type: Select only vertices outside the ROI
		roiUtilities3D.SetSelectionType(CROIUtilities3D::EResultSelectionType_Exclude);

		// CROIUtilities3D 실행 // Execute CROIUtilities3D
		if((res = roiUtilities3D.Execute()).IsFail())
			break;

		// CROIUtilities3D 에서 결과 얻어 오기 // Retrieve results from CROIUtilities3D
		CFLArray<CFLArray<int32_t>> flfaResultROIIndexExclude;
		if((res = roiUtilities3D.GetResult(flfaResultROIIndexExclude)).IsFail())
			break;

		// EResultSelectionType.Remove 연산을 위해 CROIUtilities3D 객체 선언 및 roiUtilities3D 를 복사 생성. Exclude 연산으로 얻은 결과값까지 복사됨 // Declare a CROIUtilities3D object for EResultSelectionType.Remove operation and copy roiUtilities3D. Results from the Exclude operation are also copied.
		CROIUtilities3D roiUtilities3DRemove = roiUtilities3D;
		// EResultSelectionType.XOR 연산을 위해 CROIUtilities3D 객체 선언 및 roiUtilities3D 를 복사 생성. Exclude 연산으로 얻은 결과값까지 복사됨 // Declare a CROIUtilities3D object for EResultSelectionType.XOR operation and copy roiUtilities3D. Results from the Exclude operation are also copied.
		CROIUtilities3D roiUtilities3DXOR = roiUtilities3D;

		// 복사한 객체에서 ROI를 모두 클리어 // Clear all ROIs from the copied objects
		roiUtilities3DRemove.ClearROI();
		roiUtilities3DXOR.ClearROI();

		if(flfaResultROIIndexExclude.GetCount())
		{
			int32_t i32ObjectIdx = 0;

			// 3D 뷰어에 추가된 3D 객체의 개수 // Number of 3D objects added to the 3D viewer
			int32_t i32ObjectCount = view3DExclude.GetObjectCount();

			for(int32_t i = 0; i < i32ObjectCount; ++i)
			{
				// 3D 뷰어에 추가된 i번째 3D 객체 // The i-th 3D object added to the 3D viewer
				CGUIView3DObject* pObj = (CGUIView3DObject*)view3DExclude.GetView3DObject(i);

				// 해당 객체가 없거나, 해당 객체에 대해 선택이 비활성화 되어 있다면 continue // Skip if the object is null or selection is disabled
				if(!pObj || !pObj->IsSelectionEnabled())
					continue;

				// i번째 3D 객체의 데이터(CFL3DObject) // Data of the i-th 3D object (CFL3DObject)
				CFL3DObject* pObjData = (CFL3DObject*)pObj->Get3DObject();

				if(!pObjData)
					continue;

				// 해당 객체가 없다면 continue // Skip if the object data is null
				CFLArray<int32_t>& flaCollisionIndex = flfaResultROIIndexExclude.GetAt(i32ObjectIdx);
				i32ObjectIdx++;

				// i번째 3D 객체에 대한 결과값 배열. 이 배열은 i번째 3D 객체에 대해, ROI 외부에 위치한 정점의 인덱스로 이루어짐 // Result array for the i-th 3D object. Contains indices of vertices outside the ROI.
				if(flaCollisionIndex.IsEmpty())
					continue;

				int32_t i32CollisionIndexCount = (int32_t)flaCollisionIndex.GetCount();
				CFLArray<Base::TPoint3<uint8_t>>* pFlaVertexColor = (CFLArray<Base::TPoint3<uint8_t>>*)pObjData->GetVertexColors();

				// i번째 3D 객체에 대해, ROI 바깥에 위치한 정점을 파란색으로 표시 // Mark vertices outside the ROI of the i-th 3D object in blue
				for(int32_t j = 0; j < i32CollisionIndexCount; ++j)
					pFlaVertexColor->SetAt(flaCollisionIndex.GetAt(j), TPoint3<uint8_t>(0, 0, 255)); // BLUE

				// 3D 뷰어에 추가된 i번째 3D 객체에 대해 렌더링 업데이트 // Update rendering for the i-th 3D object added to the 3D viewer
				pObj->UpdateAll();
				view3DExclude.UpdateObject(i);
			}

			// 3D 뷰어 업데이트 // Update the 3D viewer
			view3DExclude.UpdateScreen();
		}

		// 기존 선택 영역(위에서 Include로 선택한 영역)에 추가로 선택할 영역을 ROI로 설정 // Set an additional ROI to be selected in the existing selection area (previously selected with Include)
		CFLFrustum3<float> flfrAdd;
		flfrAdd.Load(L"../../ExampleImages/ROIUtilities3D/frustumROI_Add.fig");
		// CROIUtilities3D 객체에 절두체 ROI 추가 // Add the frustum ROI to the CROIUtilities3D object
		roiUtilities3DAdd.PushBackROI(&flfrAdd);
		// 3D 뷰에 ROI 추가 // Add the frustum ROI to the 3D view
		view3DAdd.PushBackROI(&flfrAdd);

		// 선택 타입 설정 : 기존 결과에 ROI 안에 포함되는 정점을 추가 // Set selection type: Add vertices within the ROI to the existing results
		roiUtilities3DAdd.SetSelectionType(CROIUtilities3D::EResultSelectionType_Add);

		// CROIUtilities3D 실행 // Execute CROIUtilities3D
		if((res = roiUtilities3DAdd.Execute()).IsFail())
			break;

		// CROIUtilities3D 에서 결과 얻어 오기 // Retrieve results from CROIUtilities3D
		CFLArray<CFLArray<int32_t>> flfaResultROIIndexAdd;
		if((res = roiUtilities3DAdd.GetResult(flfaResultROIIndexAdd)).IsFail())
			break;

		if(flfaResultROIIndexAdd.GetCount())
		{
			int32_t i32ObjectIdx = 0;
			// 3D 뷰어에 추가된 3D 객체의 개수 // Number of 3D objects added to the 3D viewer
			int32_t i32ObjectCount = view3DAdd.GetObjectCount();

			for(int32_t i = 0; i < i32ObjectCount; ++i)
			{
				// 3D 뷰어에 추가된 i번째 3D 객체 // The i-th 3D object added to the 3D viewer
				CGUIView3DObject* pObj = (CGUIView3DObject*)view3DAdd.GetView3DObject(i);

				// 해당 객체가 없거나, 해당 객체에 대해 선택이 비활성화 되어 있다면 continue // Skip if the object is null or selection is disabled
				if(!pObj || !pObj->IsSelectionEnabled())
					continue;

				// i번째 3D 객체의 데이터(CFL3DObject) // Data of the i-th 3D object (CFL3DObject)
				CFL3DObject* pObjData = (CFL3DObject*)pObj->Get3DObject();

				// 해당 객체가 없다면 continue // Skip if the object data is null
				if(!pObjData)
					continue;

				// i번째 3D 객체에 대한 결과값 배열. // Result array for the i-th 3D object
				CFLArray<int32_t>& flaCollisionIndex = flfaResultROIIndexAdd.GetAt(i32ObjectIdx);
				i32ObjectIdx++;

				if(flaCollisionIndex.IsEmpty())
					continue;

				int32_t i32CollisionIndexCount = (int32_t)flaCollisionIndex.GetCount();
				CFLArray<Base::TPoint3<uint8_t>>* pFlaVertexColor = (CFLArray<Base::TPoint3<uint8_t>>*)pObjData->GetVertexColors();

				// i번째 3D 객체에 대해, ROI 내부에 위치한 정점을 빨간색으로 표시 // Mark vertices within the ROI of the i-th 3D object in red
				for(int32_t j = 0; j < i32CollisionIndexCount; ++j)
					pFlaVertexColor->SetAt(flaCollisionIndex.GetAt(j), TPoint3<uint8_t>(255, 0, 0)); // RED

				// 3D 뷰어에 추가된 i번째 3D 객체에 대해 렌더링 업데이트 // Update rendering for the i-th 3D object added to the 3D viewer
				pObj->UpdateAll();
				view3DAdd.UpdateObject(i);
			}

			// 3D 뷰어 업데이트 // Update the 3D viewer
			view3DAdd.UpdateScreen();
		}

		// 기존 선택 영역(위에서 Exclude로 선택한 영역)에서 제거할 영역을 ROI로 설정 // Set ROIs to remove areas from the existing selection (previously selected with Exclude)
		CFLFrustum3<float> flfrRemove1, flfrRemove2;
		flfrRemove1.Load(L"../../ExampleImages/ROIUtilities3D/frustumROI_Remove1.fig");
		flfrRemove2.Load(L"../../ExampleImages/ROIUtilities3D/frustumROI_Remove2.fig");
		// CROIUtilities3D 객체에 절두체 ROI 추가 // Add the frustum ROIs to the CROIUtilities3D object
		roiUtilities3DRemove.PushBackROI(flfrRemove1);
		roiUtilities3DRemove.PushBackROI(flfrRemove2);
		// 3D 뷰에 ROI 추가 // Add the frustum ROIs to the 3D view
		view3DRemove.PushBackROI(&flfrRemove1);
		view3DRemove.PushBackROI(&flfrRemove2);

		// 선택 타입 설정 : 기존 결과에서 ROI 안의 정점을 제거 // Set selection type: Remove vertices within the ROI from the existing results
		roiUtilities3DRemove.SetSelectionType(CROIUtilities3D::EResultSelectionType_Remove);

		// CROIUtilities3D 실행 // Execute CROIUtilities3D
		if((res = roiUtilities3DRemove.Execute()).IsFail())
			break;

		// CROIUtilities3D 에서 결과 얻어 오기 // Retrieve results from CROIUtilities3D
		CFLArray<CFLArray<int32_t>> flfaResultROIIndexRemove;
		if((res = roiUtilities3DRemove.GetResult(flfaResultROIIndexRemove)).IsFail())
			break;

		if(flfaResultROIIndexRemove.GetCount())
		{
			int32_t i32ObjectIdx = 0;

			// 3D 뷰어에 추가된 3D 객체의 개수 // Number of 3D objects added to the 3D viewer
			int32_t i32ObjectCount = view3DRemove.GetObjectCount();

			for(int32_t i = 0; i < i32ObjectCount; ++i)
			{
				// 3D 뷰어에 추가된 i번째 3D 객체 // The i-th 3D object added to the 3D viewer
				CGUIView3DObject* pObj = (CGUIView3DObject*)view3DRemove.GetView3DObject(i);

				// 해당 객체가 없거나, 해당 객체에 대해 선택이 비활성화 되어 있다면 continue // Skip if the object is null or selection is disabled
				if(!pObj || !pObj->IsSelectionEnabled())
					continue;

				// i번째 3D 객체의 데이터(CFL3DObject) // Data of the i-th 3D object (CFL3DObject)
				CFL3DObject* pObjData = (CFL3DObject*)pObj->Get3DObject();

				// 해당 객체가 없다면 continue // Skip if the object data is null
				if(!pObjData)
					continue;

				// i번째 3D 객체에 대한 결과값 배열. // Result array for the i-th 3D object
				CFLArray<int32_t>& flaCollisionIndex = flfaResultROIIndexRemove.GetAt(i32ObjectIdx);
				i32ObjectIdx++;

				if(flaCollisionIndex.IsEmpty())
					continue;

				int32_t i32CollisionIndexCount = (int32_t)flaCollisionIndex.GetCount();
				CFLArray<Base::TPoint3<uint8_t>>* pFlaVertexColor = (CFLArray<Base::TPoint3<uint8_t>>*)pObjData->GetVertexColors();

				// i번째 3D 객체에 대해, 기존 결과에서 ROI 안의 정점을 제거 후 선택된 정점을 파란색으로 표시 // Mark selected vertices after removing vertices within the ROI of the i-th 3D object in blue
				for(int32_t j = 0; j < i32CollisionIndexCount; ++j)
					pFlaVertexColor->SetAt(flaCollisionIndex.GetAt(j), TPoint3<uint8_t>(0, 0, 255)); // BLUE

				// 3D 뷰어에 추가된 i번째 3D 객체에 대해 렌더링 업데이트 // Update rendering for the i-th 3D object added to the 3D viewer
				pObj->UpdateAll();
				view3DRemove.UpdateObject(i);
			}

			// 3D 뷰어 업데이트 // Update the 3D viewer
			view3DRemove.UpdateScreen();
		}

		// 기존 선택 영역(위에서 Exclude로 선택한 영역)에서 XOR 선택할 영역을 ROI로 설정 // Set an ROI to perform XOR operation on the existing selection (previously selected with Exclude)
		CFLFrustum3<float> flfrXOR;
		flfrXOR.Load(L"../../ExampleImages/ROIUtilities3D/frustumROI_XOR.fig");
		// CROIUtilities3D 객체에 절두체 ROI 추가 // Add the frustum ROI to the CROIUtilities3D object
		roiUtilities3DXOR.PushBackROI(&flfrXOR);
		// 3D 뷰에 ROI 추가 // Add the frustum ROI to the 3D view
		view3DXOR.PushBackROI(&flfrXOR);

		// 선택 타입 설정 : 기존 결과에서 ROI 안의 정점을 XOR 연산하여 선택 // Set selection type: Perform XOR operation with vertices inside the ROI on the existing results
		roiUtilities3DXOR.SetSelectionType(CROIUtilities3D::EResultSelectionType_XOR);

		// CROIUtilities3D 실행 // Execute CROIUtilities3D
		if((res = roiUtilities3DXOR.Execute()).IsFail())
			break;

		// CROIUtilities3D 에서 결과 얻어 오기 // Retrieve results from CROIUtilities3D
		CFLArray<CFLArray<int32_t>> flfaResultROIIndexXOR;
		if((res = roiUtilities3DXOR.GetResult(flfaResultROIIndexXOR)).IsFail())
			break;

		if(flfaResultROIIndexXOR.GetCount())
		{
			int32_t i32ObjectIdx = 0;
			// 3D 뷰어에 추가된 3D 객체의 개수 // Number of 3D objects added to the 3D viewer
			int32_t i32ObjectCount = view3DXOR.GetObjectCount();

			for(int32_t i = 0; i < i32ObjectCount; ++i)
			{
				// 3D 뷰어에 추가된 i번째 3D 객체 // The i-th 3D object added to the 3D viewer
				CGUIView3DObject* pObj = (CGUIView3DObject*)view3DXOR.GetView3DObject(i);

				// 해당 객체가 없거나, 해당 객체에 대해 선택이 비활성화 되어 있다면 continue // Skip if the object is null or selection is disabled
				if(!pObj || !pObj->IsSelectionEnabled())
					continue;

				// i번째 3D 객체의 데이터(CFL3DObject) // Data of the i-th 3D object (CFL3DObject)
				CFL3DObject* pObjData = (CFL3DObject*)pObj->Get3DObject();

				// 해당 객체가 없다면 continue // Skip if the object data is null
				if(!pObjData)
					continue;

				// i번째 3D 객체에 대한 결과값 배열. // Result array for the i-th 3D object
				CFLArray<int32_t>& flaCollisionIndex = flfaResultROIIndexXOR.GetAt(i32ObjectIdx);
				i32ObjectIdx++;

				if(flaCollisionIndex.IsEmpty())
					continue;

				int32_t i32CollisionIndexCount = (int32_t)flaCollisionIndex.GetCount();
				CFLArray<Base::TPoint3<uint8_t>>* pFlaVertexColor = (CFLArray<Base::TPoint3<uint8_t>>*)pObjData->GetVertexColors();

				// i번째 3D 객체에 대해, 기존 결과에서 ROI 안의 정점을 XOR 연산한 결과 정점들을 파란색으로 표시 // Mark the vertices resulting from XOR operation within the ROI of the i-th 3D object in blue
				for(int32_t j = 0; j < i32CollisionIndexCount; ++j)
					pFlaVertexColor->SetAt(flaCollisionIndex.GetAt(j), TPoint3<uint8_t>(0, 0, 255)); // BLUE

				// 3D 뷰어에 추가된 i번째 3D 객체에 대해 렌더링 업데이트 // Update rendering for the i-th 3D object added to the 3D viewer
				pObj->UpdateAll();
				view3DXOR.UpdateObject(i);
			}

			// 3D 뷰어 업데이트 // Update the 3D viewer
			view3DXOR.UpdateScreen();
		}

		// 아래 함수 DrawTextCanvas는 스크린 좌표를 기준으로 문자열을 뷰어에 출력한다.
		// The function DrawTextCanvas displays a string on the viewer using screen coordinates.
		// 파라미터 순서 : 기준 좌표 Figure 객체 -> 문자열 -> 텍스트 색 -> 텍스트 테두리 색 -> 폰트 크기 -> 실제 크기로 출력 유무 -> 각도 -> 정렬 -> 폰트 이름 -> 텍스트 알파값(불투명도) -> 텍스트 테두리 알파값 (불투명도) -> 폰트 두께 -> 폰트 이탤릭 여부
		// Parameter order: reference coordinate (Figure object) -> text string -> text color -> text outline color -> font size -> render in real-world size (bool) -> angle -> alignment -> font name -> text alpha (opacity) -> text outline alpha (opacity) -> font thickness -> italic font (bool)
		CFLPoint<double> flp(0, 0);
		if(IsFail(res = view3DSrc.GetLayer(0).DrawTextCanvas(&flp, L"Source", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = view3DInclude.GetLayer(0).DrawTextCanvas(&flp, L"Include", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = view3DExclude.GetLayer(0).DrawTextCanvas(&flp, L"Exclude", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = view3DAdd.GetLayer(0).DrawTextCanvas(&flp, L"Add(Include Result+Add)", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = view3DRemove.GetLayer(0).DrawTextCanvas(&flp, L"Remove(Exclude Result-Remove)", YELLOW, BLACK, 18)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = view3DXOR.GetLayer(0).DrawTextCanvas(&flp, L"XOR(Exclude Result^XOR)", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 3D 뷰를 갱신 // Update 3D view
		for(int32_t i = 0; i < 6; ++i)
			arrView3D[i]->Invalidate(true);

		// 3D 뷰가 종료될 때 까지 기다림 // Wait for the 3D view 
		while(arrView3D[0]->IsAvailable() && arrView3D[1]->IsAvailable() && arrView3D[2]->IsAvailable() && arrView3D[3]->IsAvailable() && arrView3D[4]->IsAvailable() && arrView3D[5]->IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}