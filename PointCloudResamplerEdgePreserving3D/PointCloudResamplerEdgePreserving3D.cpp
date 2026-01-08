#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 객체 선언 // Declare 3D object	
	CFL3DObject floSource;
	CFL3DObject arrFloResult[3];

	// 3D 뷰 선언 // Declare 3D view	
	CGUIView3DWrap view3DSource;
	CGUIView3DWrap arr3DView[3];
	constexpr int32_t arrI32Sensitivity[3] = { 1, 3, 5 };

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult eResult = EResult_UnknownError;


		// Source Object 로드 // Load the Source object
		if((eResult = floSource.Load(L"../../ExampleImages/PointCloudResamplerEdgePreserving3D/Box.fl3do")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the object file.\n");
			break;
		}

		// 3D 뷰 생성 // Create 3D View
		if(IsFail(eResult = view3DSource.Create(100, 0, 612, 512)))
		{
			ErrorPrint(eResult, "Failed to create the 3D view.\n");
			break;
		}

		// 3D 뷰 생성 // Create 3D View
		if(IsFail(eResult = arr3DView[0].Create(100, 512, 612, 1024)))
		{
			ErrorPrint(eResult, "Failed to create the 3D view.\n");
			break;
		}

		if(IsFail(eResult = arr3DView[1].Create(612, 512, 1124, 1024)))
		{
			ErrorPrint(eResult, "Failed to create the 3D view.\n");
			break;
		}

		if(IsFail(eResult = arr3DView[2].Create(1124, 512, 1636, 1024)))
		{
			ErrorPrint(eResult, "Failed to create the 3D view.\n");
			break;
		}

		// PointCloudResamplerEdgePreserving3D 객체 생성 // Create PointCloudResamplerEdgePreserving3D object
		CPointCloudResamplerEdgePreserving3D pointCloudResamplerEdgePreserving3D;

		// Source object 설정 // Set the source object
		pointCloudResamplerEdgePreserving3D.SetSourceObject(floSource);

		// 법선 벡터 각도 임계 설정 // Set the normal angle threshold
		pointCloudResamplerEdgePreserving3D.SetNormalAngleThreshold(15);

		// 탐색할 최근접 이웃 수 설정 // Set the number of nearest neighbors to search
		pointCloudResamplerEdgePreserving3D.SetNormalEstimationNeighborCount(20);

		// 반경 자동 계산 여부 설정 // Sets whether the radius is calculated automatically.
		pointCloudResamplerEdgePreserving3D.EnableAutoRadiusCalculation(true);

		// 결과 법선 포함 여부 설정 // Sets whether to retain result normals.
		pointCloudResamplerEdgePreserving3D.EnableNormalRetainment(false);

		// 반경 계수 설정 // Set the radius coefficient
		pointCloudResamplerEdgePreserving3D.SetRadiusCoefficient(5);

		// 입력 샘플링 개수 설정 // Set the source sampling size
		pointCloudResamplerEdgePreserving3D.SetSourceSamplingSize(2500);

		// 결과 샘플링 개수 설정 // Set the result sampling size
		pointCloudResamplerEdgePreserving3D.SetResultSamplingSize(10000);

		// 점 재배치 반복 횟수 설정 // Set the point reposition iterations
		pointCloudResamplerEdgePreserving3D.SetRepositionIterations(5);

		CFLPoint<double> flp(0, 0);

		for(int32_t i = 0; i < 3; ++i)
		{
			// Destination object 설정 // Set the destination object
			pointCloudResamplerEdgePreserving3D.SetDestinationObject(arrFloResult[i]);

			// 에지 민감도 설정 // Set the edge sensitivity
			pointCloudResamplerEdgePreserving3D.SetEdgeSensitivity(arrI32Sensitivity[i]);

			// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
			if((eResult = pointCloudResamplerEdgePreserving3D.Execute()).IsFail())
			{
				ErrorPrint(eResult, L"Failed to execute Point Cloud Resampler Edge Preserving 3D.");
				break;
			}

			// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately		
			CGUIView3DLayerWrap view3DLayer = arr3DView[i].GetLayer(0);

			// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
			view3DLayer.Clear();

			// View 정보를 디스플레이 한다. // Display view information
			// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
			// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
			// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
			//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
			// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
			//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic

			CFLString<wchar_t> flsString;

			flsString.Format(L"Edge Sensitivity %d", arrI32Sensitivity[i]);

			if((eResult = view3DLayer.DrawTextCanvas(&flp, flsString, YELLOW, BLACK, 20)).IsFail())
			{
				ErrorPrint(eResult, L"Failed to draw text.\n");
				break;
			}

			// 3D 오브젝트 뷰에 Destination 오브젝트 디스플레이
			if((eResult = arr3DView[i].PushObject(arrFloResult[i])).IsFail())
			{
				ErrorPrint(eResult, L"Failed to set object on the 3d view.\n");
				break;
			}

			view3DSource.SynchronizePointOfView(&arr3DView[i]);
		}

		CGUIView3DLayerWrap view3DLayerSource = view3DSource.GetLayer(0);
		view3DLayerSource.Clear();

		if((eResult = view3DLayerSource.DrawTextCanvas(&flp, L"Source Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		// 3D 오브젝트 뷰에 Destination 오브젝트 디스플레이
		if((eResult = view3DSource.PushObject(floSource)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to set object on the 3d view.\n");
			break;
		}

		view3DSource.ZoomFit();
		view3DSource.Invalidate(true);

		for(int32_t i = 0; i < 3; ++i)
			arr3DView[i].Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림
		while(view3DSource.IsAvailable() && arr3DView[0].IsAvailable() && arr3DView[1].IsAvailable() && arr3DView[2].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}