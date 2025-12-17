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
	CFL3DObject floNoiseResult;
	CFL3DObject floSmoothingResult;
	CFL3DObject floDenoisingResult;

	// 3D 뷰 선언 // Declare 3D view	
	CGUIView3DWrap view3DNoiseResult;
	CGUIView3DWrap view3DSmoothingResult;
	CGUIView3DWrap view3DDenoisingResult;


	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult eResult = EResult_UnknownError;


		// Source Object 로드 // Load the Source object
		if((eResult = floSource.Load(L"../../ExampleImages/Denoising3D/Cube.ply")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the object file.\n");
			break;
		}

		// Noise 결과 3D 뷰 생성 // Create Noise Result 3D View
		if(IsFail(eResult = view3DNoiseResult.Create(100, 0, 612, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Noise Result 3D view.\n");
			break;
		}

		// Smoothing 결과 3D 뷰 생성 // Create Smoothing Result 3D View
		if(IsFail(eResult = view3DSmoothingResult.Create(612, 0, 1124, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Smoothing Result 3D view.\n");
			break;
		}

		// Denoising 결과 3D 뷰 생성 // Create Denoising Result 3D View	
		if(IsFail(eResult = view3DDenoisingResult.Create(1124, 0, 1636, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Denoising Result 3D view.\n");
			break;
		}

		// 노이즈 적용된 객체 생성 // Noise-applied object generation
		{
			CScannedPointCloudGenerator3D scannedPointCloudGenerator3D;
			scannedPointCloudGenerator3D.SetSourceObject(floSource);
			scannedPointCloudGenerator3D.SetDestinationObject(floNoiseResult);
			scannedPointCloudGenerator3D.SetNoiseRatio(0.5f);
			scannedPointCloudGenerator3D.EnableViewPointSetting(false);
			scannedPointCloudGenerator3D.EnableHiddenPointRemoval(false);
			scannedPointCloudGenerator3D.EnableIncludingNormalVector(false);
			scannedPointCloudGenerator3D.EnableRelativeSamplingDistance(false);
			scannedPointCloudGenerator3D.SetSamplingDistance(1);
			scannedPointCloudGenerator3D.SetTargetVertexColor({ 0, 255, 255 });

			if((eResult = scannedPointCloudGenerator3D.Execute()).IsFail())
			{
				ErrorPrint(eResult, L"Failed to execute the Scanned Point Cloud Generator 3D.\n");
				break;
			}
		}

		// Smoothing 3D 실행 // Execute Smoothing 3D
		{
			// Smoothing3D 객체 생성 // Create Smoothing3D object
			CSmoothing3D smoothing3D;

			// Source object 설정 // Set the source object
			smoothing3D.SetSourceObject(floNoiseResult);

			// Destination object 설정 // Set the destination object
			smoothing3D.SetDestinationObject(floSmoothingResult);

			// 탐색할 최근접 이웃 수 설정 // Set the number of nearest neighbors to search
			smoothing3D.SetNeighborCount(100);

			TPoint3<float> tp3ViewPoint(0, 0, 0);

			// 뷰 포인트 설정 // Set the view point
			smoothing3D.SetViewPoint(tp3ViewPoint);

			// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
			if((eResult = smoothing3D.Execute()).IsFail())
			{
				ErrorPrint(eResult, L"Failed to execute Smoothing 3D.");
				break;
			}
		}

		// Denoising 3D 실행 // Execute Denoising 3D
		{
			// Denoising3D 객체 생성 // Create Denoising3D object
			CDenoising3D denoising3D;

			// Source object 설정 // Set the source object
			denoising3D.SetSourceObject(floNoiseResult);

			// Destination object 설정 // Set the destination object
			denoising3D.SetDestinationObject(floDenoisingResult);

			// lambda 값 설정 // Set the lambda value
			denoising3D.SetLambda(0.1f);

			// 탐색할 최근접 이웃 수 설정 // Set the number of nearest neighbors to search
			denoising3D.SetNeighborCount(100);

			TPoint3<float> tp3ViewPoint(0, 0, 0);

			// 뷰 포인트 설정 // Set the view point
			denoising3D.SetViewPoint(tp3ViewPoint);

			// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
			if((eResult = denoising3D.Execute()).IsFail())
			{
				ErrorPrint(eResult, L"Failed to execute Denoising 3D.");
				break;
			}
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately		
		CGUIView3DLayerWrap layer3DNoiseResult = view3DNoiseResult.GetLayer(0);
		CGUIView3DLayerWrap layer3DDenoisingResult = view3DDenoisingResult.GetLayer(0);
		CGUIView3DLayerWrap layer3DSmoothingResult = view3DSmoothingResult.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer3DNoiseResult.Clear();
		layer3DDenoisingResult.Clear();
		layer3DSmoothingResult.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if((eResult = layer3DNoiseResult.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Noise(0.5)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		if((eResult = layer3DDenoisingResult.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Denoising Result", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		if((eResult = layer3DSmoothingResult.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Smoothing Result", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		// 3D 오브젝트 뷰에 Destination 오브젝트 디스플레이
		if((eResult = view3DNoiseResult.PushObject(floNoiseResult)).IsFail() ||
		   (eResult = view3DSmoothingResult.PushObject(floSmoothingResult)).IsFail() ||
		   (eResult = view3DDenoisingResult.PushObject(floDenoisingResult)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to set object on the 3d view.\n");
			break;
		}

		view3DNoiseResult.SynchronizePointOfView(&view3DDenoisingResult);
		view3DNoiseResult.SynchronizePointOfView(&view3DSmoothingResult);

		view3DNoiseResult.ZoomFit();

		// 이미지 뷰를 갱신 합니다. // Update image view		
		view3DNoiseResult.Invalidate(true);
		view3DSmoothingResult.Invalidate(true);
		view3DDenoisingResult.Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림
		while(view3DNoiseResult.IsAvailable() && view3DSmoothingResult.IsAvailable() && view3DDenoisingResult.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}