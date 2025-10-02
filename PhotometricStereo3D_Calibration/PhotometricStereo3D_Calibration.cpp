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
	CFLImage fliCalibrationImage;
	CFLImage fliDestinationImage;
	CFLImage fliCurvatureImage;
	CFLImage fliTextureImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewSourceImage;
	CGUIViewImageWrap viewCalibrationImage;
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
		if((res = viewSourceImage.Create(100, 0, 498, 398)).IsFail())
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

		// Calibration 이미지 로드 // Load Calibration image
		if((res = fliCalibrationImage.Load(L"../../ExampleImages/PhotometricStereo3D/Calibrate.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Calibration 이미지 뷰 생성 // Create Calibration image view
		if((res = viewCalibrationImage.Create(498, 0, 896, 398)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Calibration 이미지 뷰에 이미지를 디스플레이 // Display image in Calibration image view
		if((res = viewCalibrationImage.SetImagePtr(&fliCalibrationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Texture 이미지 뷰 생성 // Create Texture image view
		if((res = viewTextureImage.Create(100, 398, 498, 796)).IsFail())
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
		if((res = viewCurvatureImage.Create(498, 398, 896, 796)).IsFail())
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
		if((res = view3DDst.Create(896, 0, 1692, 796)).IsFail())
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
		if((res = viewSourceImage.SynchronizeWindow(&viewCalibrationImage)).IsFail())
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
		if((res = viewSourceImage.SynchronizeWindow(&viewTextureImage)).IsFail())
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

		// Calibration 이미지 설정 // Set Calibration image
		if((res = photometricStereo3D.SetCalibrationImage(fliCalibrationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Calibration image.\n");
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

		// Calibration Circle ROI 설정 // Set calibration circle ROI settings
		if((res = photometricStereo3D.SetCalibrationCircleROI(CFLCircle<double>(117.210526, 104.842105, 78.736842, 0.000000, 0.000000, 360.000000, EArcClosingMethod_EachOther))).IsFail())
		{
			ErrorPrint(res, L"Failed to set Calibration Circle ROI.\n");
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

		CMatrix<double> matEmpty(3, 3);

		// Angle Degrees 동작 방식으로 설정 // Set operation method as angle degrees
		if((res = photometricStereo3D.SetLightAngleDegrees(matEmpty)).IsFail())
		{
			ErrorPrint(res, L"Failed to set light angle in degrees.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 Calibration 수행 // Calibration algorithm according to previously set parameters
		if((res = photometricStereo3D.Calibrate()).IsFail())
		{
			ErrorPrint(res, L"Failed to calibrate Photometric Stereo 3D.\n");
			break;
		}

		// Calibrate 된 Angle Degree 데이터 // Calibrated angle degree data
		CMultiVar<double> mvdSlant;
		CMultiVar<double> mvdTilt;

		// Calibrate 된 Angle Degree 데이터 저장 // Save calibrated angle degree data
		if((res = photometricStereo3D.GetLightAngleDegrees(mvdSlant, mvdTilt)).IsFail())
		{
			ErrorPrint(res, L"Failed to get light angle in degrees.\n");
			break;
		}

		// 위치 데이터 동작 방식으로 설정 // Set operation method as positions
		if((res = photometricStereo3D.SetLightPositions(matEmpty)).IsFail())
		{
			ErrorPrint(res, L"Failed to set light positions.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 Calibration 수행 // Calibration algorithm according to previously set parameters
		if((res = photometricStereo3D.Calibrate()).IsFail())
		{
			ErrorPrint(res, L"Failed to calibrate Photometric Stereo 3D.\n");
			break;
		}

		// Calibrate 된 위치 데이터 // Calibrated position data
		CMatrix<double> matPosition;

		// Calibrate 된 위치 데이터 저장 // Save calibrated position data
		if((res = photometricStereo3D.GetLightPositions(matPosition)).IsFail())
		{
			ErrorPrint(res, L"Failed to get light positions.\n");
			break;
		}

		// Calibrate를 실행한 결과를 Console창에 출력 // Output calibration result to console window
		int32_t i32CalibPageNum = fliCalibrationImage.GetPageCount();

		// Angle Degrees 데이터 출력 // Print angle degrees data
		printf(" < Calibration Angle - Degrees >\n");

		for(int i = 0; i < i32CalibPageNum; i++)
			printf("Image %d ->\tSlant: %0.7lf\tTilt: %0.7lf\n", i, mvdSlant.GetAt(i), mvdTilt.GetAt(i));

		printf("\n");

		// Positions 데이터 출력 // Print positions data
		printf(" < Calibration Positions >\n");

		for(int i = 0; i < i32CalibPageNum; i++)
			printf("Image %d ->\tX: %0.7lf\tY: %0.7lf \tZ: %0.7lf\n", i, *matPosition.GetValue(i, 0), *matPosition.GetValue(i, 1), *matPosition.GetValue(i, 2));

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = photometricStereo3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Photometric Stereo 3D.\n");
			break;
		}

		// 화면에 출력하기 위해 이미지 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released
		CGUIViewImageLayerWrap layerSource = viewSourceImage.GetLayer(0);
		CGUIViewImageLayerWrap layerCalibration = viewCalibrationImage.GetLayer(0);
		CGUIViewImageLayerWrap layerCurvature = viewCurvatureImage.GetLayer(0);
		CGUIViewImageLayerWrap layerTexture = viewTextureImage.GetLayer(0);

		// 화면에 출력하기 위해 3D 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from 3D view for display
		// 이 객체는 3D 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an 3D view and does not need to be released
		CGUIView3DLayerWrap layer3DDestination = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear figures drawn on existing layer
		layerSource.Clear();
		layerCalibration.Clear();
		layerCurvature.Clear();
		layerTexture.Clear();
		layer3DDestination.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerSource.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerCalibration.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Calibration Image", YELLOW, BLACK, 20)).IsFail())
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

		// 3D View 정보 디스플레이 // Display 3D view information
		float f32CenterX = (float)fliSourceImage.GetWidth() / 2;
		float f32CenterY = (float)fliSourceImage.GetHeight() / 2;
		float f32CenterZ = (float)fliDestinationImage.GetBuffer()[(long)(f32CenterY * fliSourceImage.GetWidth() + f32CenterX)];

		TPoint3<float> tp3dFrom(f32CenterX, f32CenterY, f32CenterZ);

		float f32MulNum = 800.;

		for(long i = 0; i < i32CalibPageNum; i++)
		{
			CFLString<wchar_t> strText;

			strText.Format(L"X: %.4lf    \nY: %.4lf    \nZ: %.4lf\n", *matPosition.GetValue(i, 0), *matPosition.GetValue(i, 1), *matPosition.GetValue(i, 2));

			TPoint3<float> tp3dTo(f32MulNum * (float)*matPosition.GetValue(i, 0) + f32CenterX, f32MulNum * (float)*matPosition.GetValue(i, 1) + f32CenterY, f32MulNum * (float)*matPosition.GetValue(i, 2) + f32CenterZ);

			TPoint3<double> tp3dTod(f32MulNum * *matPosition.GetValue(i, 0) + f32CenterX, f32MulNum * *matPosition.GetValue(i, 1) + f32CenterY, f32MulNum * *matPosition.GetValue(i, 2) + f32CenterZ);

			CGUIView3DObjectLine cgui3dlineTemp(tp3dFrom, tp3dTo, YELLOW, 1);

			layer3DDestination.DrawText3D(tp3dTod, strText, BLACK, YELLOW);
			view3DDst.PushObject(cgui3dlineTemp);
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
		viewCalibrationImage.Invalidate(true);
		viewCurvatureImage.Invalidate(true);

		// 3D 뷰를 갱신 // Update 3D view
		view3DDst.Invalidate(true);

		// 뷰가 닫히기 전까지 종료하지 않고 대기 // Wait until a view is closed before exiting
		while(viewSourceImage.IsAvailable() && viewTextureImage.IsAvailable() && viewCalibrationImage.IsAvailable() && viewCurvatureImage.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}