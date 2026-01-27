#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 객체 선언 // Declare 3D object
	CFL3DObject floDestinationObject;
	CFL3DObject floSourceObject;

	// 3D 뷰 선언 // Declare 3D view	
	CGUIView3DWrap view3DDst;


	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult eResult = EResult_UnknownError;


		// Source Object 로드 // Load the Source object
		if((eResult = floSourceObject.Load(L"../../ExampleImages/Skeleton3D/Source.fl3do")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the object file.\n");
			break;
		}

		// Source 3D 뷰 생성
		if(IsFail(eResult = view3DDst.Create(612, 0, 1124, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Source 3D view.\n");
			break;
		}

		// Source Object 3D 뷰 생성 // Create the source object 3D view
		if((eResult = view3DDst.PushObject(floSourceObject)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to display the 3D object.\n");
			break;
		}

		// Skeleton3D 객체 생성 // Create Skeleton3D object
		CSkeleton3D skeleton3D;

		// Source object 설정 // Set the source object
		skeleton3D.SetSourceObject(floSourceObject);

		// Destination object 설정 // Set the destination object
		skeleton3D.SetDestinationObject(floDestinationObject);

		// sigma 계수 설정 // Set the sigma coefficient value
		skeleton3D.SetRadiusCoefficient(16);

		// Skeleton 샘플링 점 개수 설정 // Sets the number of skeleton sampling points
		skeleton3D.SetSkeletonSamplingSize(1000);

		// Source 샘플링 점 개수 설정 // Sets the number of source sampling points
		skeleton3D.SetSourceSamplingSize(10000);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((eResult = skeleton3D.Execute()).IsFail())
		{
			ErrorPrint(eResult, L"Failed to execute Skeleton 3D.");
			break;
		}

		view3DDst.ZoomFit();

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately		
		CGUIView3DLayerWrap layer3DDst = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer3DDst.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic


		if((eResult = layer3DDst.DrawTextCanvas(CFLPoint<double>(0, 0), L"Result", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		// 결과 색상 부여 // Result color assignment
		floDestinationObject = skeleton3D.GetDestinationObject();
		int32_t i32SegmentCount = (int32_t)floDestinationObject.GetSegmentElementCountInformation()->GetCount();
		CFLArray<TPoint3<uint8_t>>* pFlaSegmentColors = (CFLArray<TPoint3<uint8_t>>*)floDestinationObject.GetSegmentColors();
		TPoint3<uint8_t> tp3Color(0, 255, 255);

		pFlaSegmentColors->Reserve(i32SegmentCount);

		for(int32_t i = 0; i < i32SegmentCount; ++i)
			pFlaSegmentColors->PushBack(tp3Color);

		CGUIView3DObject viewObj(floDestinationObject);
		viewObj.SetTopologyType(ETopologyType3D_Segment);

		// 3D 오브젝트 뷰에 Destination 오브젝트 디스플레이
		if((eResult = view3DDst.PushObject(viewObj)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to set object on the 3d view.\n");
			break;
		}

		view3DDst.ZoomFit();
		
		// 뷰를 갱신 합니다. // Update view
		view3DDst.Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림
		while(view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}