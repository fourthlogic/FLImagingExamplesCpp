#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
    // before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSourceImage;
	CFLImage fliDestinationImage;
	CFLImage fliCurvatureImage;
	CFLImage fliTextureImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewSourceImage;
	CGUIViewImageWrap viewTextureImage;
	CGUIViewImageWrap viewCurvatureImage;

	// 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap view3DDst;

	do
	{
		// 수행 결과 객체 선언 // Declare execution result object
		CResult res = EResult_UnknownError;

		// Source 이미지 로드 // Load Source image
		if((res = fliSourceImage.Load(L"../../ExampleImages/PhotometricStereo3D/Source.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if((res = viewSourceImage.Create(100, 0, 548, 448)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display image in Source image view
		if((res = viewSourceImage.SetImagePtr(&fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Texture 이미지 뷰 생성 // Create Texture image view
		if((res = viewTextureImage.Create(100, 448, 548, 896)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Texture 이미지 뷰에 이미지를 디스플레이 // Display image in Texture image view
		if((res = viewTextureImage.SetImagePtr(&fliTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Curvature 이미지 뷰 생성 // Create Curvature image view
		if((res = viewCurvatureImage.Create(548, 0, 996, 448)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Curvature 이미지 뷰에 이미지를 디스플레이 // Display image in Curvature image view
		if((res = viewCurvatureImage.SetImagePtr(&fliCurvatureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 3D 뷰 생성 // Create Destination 3D view
		if((res = view3DDst.Create(548, 448, 996, 896)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the 3D view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 // Synchronize viewpoints of two image views
		if((res = viewSourceImage.SynchronizePointOfView(&viewTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize point of view between image views.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 // Synchronize viewpoints of two image views
		if((res = viewSourceImage.SynchronizePointOfView(&viewCurvatureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize point of view between image views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewSourceImage.SynchronizeWindow(&viewTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewSourceImage.SynchronizeWindow(&viewCurvatureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewSourceImage.SynchronizeWindow(&view3DDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// Photometric Stereo 3D 객체 생성 // Create Photometric Stereo 3D object
		CPhotometricStereo3D photometricStereo3D;

		// 출력에 사용되는 3D Height Map 객채 생성 // Create 3D height map used as output
		CFL3DObjectHeightMap fl3DOHM;

		// Source 이미지 설정 // Set Source image
		if((res = photometricStereo3D.SetSourceImage(fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Source image.\n");
			break;
		}

		// Destination Height Map 이미지 설정 // Set Destination Height Map image
		if((res = photometricStereo3D.SetDestinationHeightMapImage(fliDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination Height Map image.\n");
			break;
		}

		// Destination Texture 이미지 설정 // Set Destination Texture image
		if((res = photometricStereo3D.SetDestinationTextureImage(fliTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination Texture image.\n");
			break;
		}

		// Destination Curvature 이미지 설정 // Set Destination Curvature image
		if((res = photometricStereo3D.SetCurvatureImage(fliCurvatureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination Curvature image.\n");
			break;
		}

		// Destination 3D Object 설정 // Set Destination 3D Object 
		if((res = photometricStereo3D.SetDestinationObject(fl3DOHM)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination 3D Object.\n");
			break;
		}

		// 동작 방식 설정 // Set reconstruction mode
		if((res = photometricStereo3D.SetReconstructionMode(CPhotometricStereo3D::EReconstructionMode_Poisson_FP32)).IsFail())
		{
			ErrorPrint(res, L"Failed to set reconstruction mode.\n");
			break;
		}

		// Valid 픽셀의 기준 설정 // Set valid pixel ratio
		if((res = photometricStereo3D.SetValidPixelThreshold(0.25)).IsFail())
		{
			ErrorPrint(res, L"Failed to set valid pixel threshold.\n");
			break;
		}

		// Curvature 이미지 Normalization 여부 설정 // Set curvature image normalization option
		if((res = photometricStereo3D.EnableCurvatureNormalization(true)).IsFail())
		{
			ErrorPrint(res, L"Failed to set curvature normalization flag.\n");
			break;
		}
		
		// 각 이미지의 광원 Slant 값 입력 // Set light's slant angle
		CMultiVar<double> mvdSlant;

		mvdSlant.PushBack(39.831506);
		mvdSlant.PushBack(28.682381);
		mvdSlant.PushBack(20.989625);
		mvdSlant.PushBack(19.346638);
		mvdSlant.PushBack(20.785800);
		mvdSlant.PushBack(26.005273);
		mvdSlant.PushBack(19.038004);
		mvdSlant.PushBack(9.253585);
		mvdSlant.PushBack(16.425454);
		mvdSlant.PushBack(23.712574);
		mvdSlant.PushBack(26.003058);
		mvdSlant.PushBack(19.069500);
		mvdSlant.PushBack(11.801071);
		mvdSlant.PushBack(20.484473);
		mvdSlant.PushBack(25.909730);
		mvdSlant.PushBack(43.055332);
		mvdSlant.PushBack(39.043981);
		mvdSlant.PushBack(30.041029);
		mvdSlant.PushBack(26.067657);
		mvdSlant.PushBack(26.126303);

		// 각 이미지의 광원 Tilt 값 입력 // Set light's tilt angle
		CMultiVar<double> mvdTilt;

		mvdTilt.PushBack(123.359091);
		mvdTilt.PushBack(123.952892);
		mvdTilt.PushBack(154.836215);
		mvdTilt.PushBack(-173.353324);
		mvdTilt.PushBack(-147.483507);
		mvdTilt.PushBack(109.497340);
		mvdTilt.PushBack(115.825606);
		mvdTilt.PushBack(-169.019112);
		mvdTilt.PushBack(-119.343654);
		mvdTilt.PushBack(-109.319167);
		mvdTilt.PushBack(66.944279);
		mvdTilt.PushBack(48.136896);
		mvdTilt.PushBack(-5.157068);
		mvdTilt.PushBack(-54.033519);
		mvdTilt.PushBack(-66.856636);
		mvdTilt.PushBack(60.456870);
		mvdTilt.PushBack(53.388008);
		mvdTilt.PushBack(36.447691);
		mvdTilt.PushBack(13.056294);
		mvdTilt.PushBack(-5.976723);

		// 각 광원의 Slant, Tilt 값 입력 // Insert light's slant, tilt location
		if((res = photometricStereo3D.SetLightAngleDegrees(mvdSlant, mvdTilt)).IsFail())
		{
			ErrorPrint(res, L"Failed to set light angle in degrees.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = photometricStereo3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Photometric Stereo 3D.\n");
			break;
		}

		// 화면에 출력하기 위해 이미지 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
        // 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released
		CGUIViewImageLayerWrap layerSource = viewSourceImage.GetLayer(0);
		CGUIViewImageLayerWrap layerTexture = viewTextureImage.GetLayer(0);
		CGUIViewImageLayerWrap layerCurvature = viewCurvatureImage.GetLayer(0);

		// 화면에 출력하기 위해 3D 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from 3D view for display
		// 이 객체는 3D 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an 3D view and does not need to be released
		CGUIView3DLayerWrap layer3DDestination = view3DDst.GetLayer(0);
		
		// 기존에 Layer에 그려진 도형들을 삭제 // Clear figures drawn on existing layer
		layerSource.Clear();
		layerTexture.Clear();
		layerCurvature.Clear();
		layer3DDestination.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerSource.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerTexture.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Texture Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerCurvature.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Curvature Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 3D 뷰 정보 표시 // Display 3D view information
		if((res = layer3DDestination.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination 3D Height Map", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 3D Height Map에 Texture 적용 // Apply texture to 3D height map
		if((res = fl3DOHM.SetTextureImage(fliTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to apply texture to height map.\n");
			break;
		}

		res = fl3DOHM.ActivateVertexColorTexture(true);

		// 결과 3D 객체 출력 // Print resulting 3D Object
		if((res = view3DDst.PushObject(fl3DOHM)).IsFail())
		{
			ErrorPrint(res, L"Failed to display the 3D Object.\n");
			break;
		}

		// 새로 생성한 이미지를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created image
		if((res = viewTextureImage.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit image view.\n");
			break;
		}

		// 새로 생성한 이미지를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created image
		if((res = viewCurvatureImage.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit image view.\n");
			break;
		}

		// 새로 생성한 3D Object를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created 3D object
		if((res = view3DDst.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit 3D view.\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewSourceImage.Invalidate(true);
		viewTextureImage.Invalidate(true);
		viewCurvatureImage.Invalidate(true);

		// 3D 뷰를 갱신 // Update 3D view
		view3DDst.Invalidate(true);

		// 뷰가 닫히기 전까지 종료하지 않고 대기 // Wait until a view is closed before exiting
		while(viewSourceImage.IsAvailable() && viewTextureImage.IsAvailable() && viewCurvatureImage.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}