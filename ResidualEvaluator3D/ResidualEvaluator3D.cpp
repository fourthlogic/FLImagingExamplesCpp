#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 객체 선언 // Declare 3D object
	CFL3DObject fl3DOReferenceObject;
	CFL3DObject fl3DOTargetObject;

	// 3D 뷰 선언 // Declare 3D view	
	CGUIView3DWrap view3DReference;
	CGUIView3DWrap view3DTarget;


	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult eResult = EResult_UnknownError;


		// Reference Object 로드 // Load the Reference object
		if((eResult = fl3DOReferenceObject.Load(L"../../ExampleImages/ResidualEvaluator3D/ReferencePoints.ply")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the object file.\n");
			break;
		}

		// Target Object 로드 // Load the Target object
		if((eResult = fl3DOTargetObject.Load(L"../../ExampleImages/ResidualEvaluator3D/MeasuredPoints.ply")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the object file.\n");
			break;
		}

		// Reference 3D 뷰 생성
		if(IsFail(eResult = view3DReference.Create(612, 0, 1124, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Destination 3D view.\n");
			break;
		}

		// Target 3D 뷰 생성
		if(IsFail(eResult = view3DTarget.Create(1124, 0, 1636, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Reference 3D view.\n");
			break;
		}

		// Reference Object 3D 뷰 생성 // Create the reference object 3D view
		if((eResult = view3DReference.PushObject(fl3DOReferenceObject)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to display the 3D object.\n");
			break;
		}

		// Target Object 3D 뷰 생성 // Create the Target object 3D view
		if((eResult = view3DTarget.PushObject(fl3DOTargetObject)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to display the 3D object.\n");
			break;
		}

		// ResidualEvaluator3D 객체 생성 // Create ResidualEvaluator3D object
		CResidualEvaluator3D residualEvaluator3D;

		// Destination object 설정 // Set the destination object
		residualEvaluator3D.SetLearnObject(fl3DOReferenceObject);
		// Target object 설정 // Set the target object
		residualEvaluator3D.SetSourceObject(fl3DOTargetObject);
		// 최대 결과 개수 설정 // Set the max count of match result
		residualEvaluator3D.SetResidualType(CResidualEvaluator3D::EResidualType_RMSE);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((eResult = residualEvaluator3D.Execute()).IsFail())
		{
			ErrorPrint(eResult, L"Failed to execute Surface Match 3D.");
			break;
		}

		view3DTarget.ZoomFit();
		view3DReference.ZoomFit();

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately		
		CGUIView3DLayerWrap layer3DReference = view3DReference.GetLayer(0);
		CGUIView3DLayerWrap layer3DTarget = view3DTarget.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer3DReference.Clear();
		layer3DTarget.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas은 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic

		if((eResult = layer3DReference.DrawTextCanvas(CFLPoint<double>(0, 0), L"Reference Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		if((eResult = layer3DTarget.DrawTextCanvas(CFLPoint<double>(0, 0), L"Target Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		CFLString<wchar_t> strText;

		strText.Format(L"Residual : %lf", residualEvaluator3D.GetResidual());
		Base::TPoint3<float> tp3Min, tp3Max;
		Base::TPoint3<double> tp3Center;

		fl3DOTargetObject.GetBoundingBox(tp3Min, tp3Max);

		tp3Center.x = (tp3Min.x + tp3Max.x) / 2;
		tp3Center.y = (tp3Min.y + tp3Max.y) / 2;
		tp3Center.z = (tp3Min.z + tp3Max.z) / 2;

		// 추정된 포즈 행렬 및 score 출력
		if((eResult = layer3DTarget.DrawText3D(tp3Center, strText, YELLOW, 0, 9)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		view3DTarget.ZoomFit();
		view3DReference.ZoomFit();

		// 이미지 뷰를 갱신 합니다. // Update image view
		view3DTarget.Invalidate(true);
		view3DReference.Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림
		while(view3DTarget.IsAvailable() && view3DReference.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}