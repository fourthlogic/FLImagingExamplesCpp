#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
    // before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliCaliSrcXYZVImage;
	CFLImage fliCaliSrcColorImage;
	CFLImage fliExecSrcXYZVImage;
	CFLImage fliExecSrcColorImage;
	CFLImage fliExecDstColorImage;
	CFLImage fliSampDstColorImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewCaliSrcXYZVImage;
	CGUIViewImageWrap viewCaliSrcColorImage;
	CGUIViewImageWrap viewExecSrcXYZVImage;
	CGUIViewImageWrap viewExecSrcColorImage;
	CGUIViewImageWrap viewExecDstColorImage;
	CGUIViewImageWrap viewSampDstColorImage;

	// 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap view3DDst;

	do
	{
		// 수행 결과 객체 선언 // Declare execution result object
		CResult res = EResult_UnknownError;

		// Calibration Source XYZV 이미지 로드 // Load Calibration Source XYZV image
		if((res = fliCaliSrcXYZVImage.Load(L"../../ExampleImages/ColorizedPointCloudGenerator3D/CalibXYZV.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Calibration Source XYZV 이미지 뷰 생성 // Create Calibration Source XYZV image view
		if((res = viewCaliSrcXYZVImage.Create(100, 0, 400, 300)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Calibration Source XYZV 이미지 뷰에 이미지를 디스플레이 // Display image in Calibration Source XYZV image view
		if((res = viewCaliSrcXYZVImage.SetImagePtr(&fliCaliSrcXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// Calibration Source Color 이미지 로드 // Load Calibration Source Color image
		if((res = fliCaliSrcColorImage.Load(L"../../ExampleImages/ColorizedPointCloudGenerator3D/CalibRGB.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Calibration Source Color 이미지 뷰 생성 // Create Calibration Source Color image view
		if((res = viewCaliSrcColorImage.Create(100, 300, 400, 600)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Calibration Source Color 이미지 뷰에 이미지를 디스플레이 // Display image in Calibration Source Color image view
		if((res = viewCaliSrcColorImage.SetImagePtr(&fliCaliSrcColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// Execution Source XYZV 이미지 로드 // Load Execution Source XYZV image
		if((res = fliExecSrcXYZVImage.Load(L"../../ExampleImages/ColorizedPointCloudGenerator3D/ExecXYZV.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Execution Source XYZV 이미지 뷰 생성 // Create Execution Source XYZV image view
		if((res = viewExecSrcXYZVImage.Create(400, 0, 700, 300)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Execution Source XYZV 이미지 뷰에 이미지를 디스플레이 // Display image in Execution Source XYZV image view
		if((res = viewExecSrcXYZVImage.SetImagePtr(&fliExecSrcXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// Execution Source Color 이미지 로드 // Load Execution Source Color image
		if((res = fliExecSrcColorImage.Load(L"../../ExampleImages/ColorizedPointCloudGenerator3D/ExecRGB.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Execution Source Color 이미지 뷰 생성 // Create Execution Source Color image view
		if((res = viewExecSrcColorImage.Create(400, 300, 700, 600)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Execution Source Color 이미지 뷰에 이미지를 디스플레이 // Display image in Execution Source Color image view
		if((res = viewExecSrcColorImage.SetImagePtr(&fliExecSrcColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// Execution Destination Color 이미지 뷰 생성 // Create Execution Destination Color image view
		if((res = viewExecDstColorImage.Create(700, 0, 1000, 300)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Execution Destination Color 이미지 뷰에 이미지를 디스플레이 // Display image in Execution Destination Color image view
		if((res = viewExecDstColorImage.SetImagePtr(&fliExecDstColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// Execution Sampled Destination 이미지 뷰 생성 // Create Execution Sampled Destination image view
		if((res = viewSampDstColorImage.Create(700, 300, 1000, 600)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Execution Sampled Destination 이미지 뷰에 이미지를 디스플레이 // Display image in Execution Sampled Destination image view
		if((res = viewSampDstColorImage.SetImagePtr(&fliSampDstColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// Destination 3D 뷰 생성 // Create Destination 3D view
		if((res = view3DDst.Create(1000, 0, 1600, 600)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the 3D view.\n");
			break;
		}


		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewCaliSrcXYZVImage.SynchronizeWindow(&viewCaliSrcColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewCaliSrcXYZVImage.SynchronizeWindow(&viewExecSrcColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewCaliSrcXYZVImage.SynchronizeWindow(&viewExecSrcXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewCaliSrcXYZVImage.SynchronizeWindow(&viewExecDstColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewCaliSrcXYZVImage.SynchronizeWindow(&viewSampDstColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewCaliSrcXYZVImage.SynchronizeWindow(&view3DDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}


		// 두 이미지 뷰 윈도우의 Page를 동기화 한다 // Synchronize pages of two image views
		if((res = viewCaliSrcXYZVImage.SynchronizePageIndex(&viewCaliSrcColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize page index between image views.\n");
			break;
		}


		// Colorized Point Cloud Generator 3D 객체 생성 // Create Colorized Point Cloud Generator 3D object
		CColorizedPointCloudGenerator3D colorizedPointCloudGenerator3D;

		// Calibration Source XYZV 이미지 설정 // Set Calibration Source XYZV image
		if((res = colorizedPointCloudGenerator3D.SetCalibrationXYZVImage(fliCaliSrcXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Calibration Source XYZV image.\n");
			break;
		}

		// Calibration Source Color 이미지 설정 // Set Calibration Source Color image
		if((res = colorizedPointCloudGenerator3D.SetCalibrationColorImage(fliCaliSrcColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Calibration Source Color image.\n");
			break;
		}

		// Calibration에 사용되는 Grid Type 설정 // Set grid type used in calibration
		if((res = colorizedPointCloudGenerator3D.SetGridType(AdvancedFunctions::CCameraCalibrator::EGridType_ChessBoard)).IsFail())
		{
			ErrorPrint(res, L"Failed to set calibration grid type.\n");
			break;
		}

		// Calibration의 최적해 정확도 값 설정 // Set optimal solution accuracy of calibration
		if((res = colorizedPointCloudGenerator3D.SetOptimalSolutionAccuracy(0.00001)).IsFail())
		{
			ErrorPrint(res, L"Failed to set calibration optimal solution accuracy.\n");
			break;
		}

		// 자동 Coordinate Adjustment 사용 여부 설정 // Set auto coordinate adjustment flag
		if((res = colorizedPointCloudGenerator3D.EnableAutoCoordinateAdjustment(true)).IsFail())
		{
			ErrorPrint(res, L"Failed to set coordinate adjustment flag.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 Calibration 수행 // Calibration algorithm according to previously set parameters
		if((res = colorizedPointCloudGenerator3D.Calibrate()).IsFail())
		{
			ErrorPrint(res, L"Failed to calibrate Colorized Point Cloud Generator 3D.\n");
			break;
		}


		// Calibration 결과 출력 // Print calibration results
		printf(" < Calibration Result >\n\n");

		// Color 카메라의 Intrinsic Parameter 출력 // Print intrinsic parameters of color camera
		AdvancedFunctions::CCameraCalibrator::CCalibratorIntrinsicParameters calibIntrinsic;

		calibIntrinsic = colorizedPointCloudGenerator3D.GetIntrinsicParameters();

		printf(" < Intrinsic Parameters >\n");

		printf("Focal Length X ->\t%0.7lf\n", calibIntrinsic.f64FocalLengthX);
		printf("Focal Length Y ->\t%0.7lf\n", calibIntrinsic.f64FocalLengthY);
		printf("Principal Point X ->\t%0.7lf\n", calibIntrinsic.f64PrincipalPointX);
		printf("Principal Point Y ->\t%0.7lf\n", calibIntrinsic.f64PrincipalPointY);
		printf("Skew ->\t%0.7lf\n", calibIntrinsic.f64Skew);

		printf("\n");

		// Color 카메라의 Distortion Coefficient 출력 // Print distortion coefficients of color camera
		AdvancedFunctions::CCameraCalibrator::CCalibratorDistortionCoefficients calibDistortion;

		calibDistortion = colorizedPointCloudGenerator3D.GetDistortionCoefficients();

		printf(" < Distortion Coefficients >\n");

		printf("K1 ->\t%0.7lf\n", calibDistortion.f64K1);
		printf("K2 ->\t%0.7lf\n", calibDistortion.f64K2);
		printf("P1 ->\t%0.7lf\n", calibDistortion.f64P1);
		printf("P2 ->\t%0.7lf\n", calibDistortion.f64P2);
		printf("K3 ->\t%0.7lf\n", calibDistortion.f64K3);

		printf("\n");

		// 두 카메라 간의 회전 행렬 출력 // Print relative rotation matrix between both cameras
		CMatrix<double> matRotation;

		if((res = colorizedPointCloudGenerator3D.GetRelativeRotation(matRotation)).IsFail())
		{
			ErrorPrint(res, L"Failed to get relative rotation.\n");
			break;
		}

		printf(" < Relative Rotation >\n");

		printf("R00 ->\t%0.7lf\n", *matRotation.GetValue(0, 0));
		printf("R01 ->\t%0.7lf\n", *matRotation.GetValue(0, 1));
		printf("R02 ->\t%0.7lf\n", *matRotation.GetValue(0, 2));
		printf("R10 ->\t%0.7lf\n", *matRotation.GetValue(1, 0));
		printf("R11 ->\t%0.7lf\n", *matRotation.GetValue(1, 1));
		printf("R12 ->\t%0.7lf\n", *matRotation.GetValue(1, 2));
		printf("R20 ->\t%0.7lf\n", *matRotation.GetValue(2, 0));
		printf("R21 ->\t%0.7lf\n", *matRotation.GetValue(2, 1));
		printf("R22 ->\t%0.7lf\n", *matRotation.GetValue(2, 2));

		printf("\n");

		// 두 카메라 간의 변환 행렬 출력 // Print relative translation matrix between both cameras
		CMatrix<double> matTranslation;

		if((res = colorizedPointCloudGenerator3D.GetRelativeTranslation(matTranslation)).IsFail())
		{
			ErrorPrint(res, L"Failed to get relative translation.\n");
			break;
		}

		printf(" < Relative Translation >\n");

		printf("TX ->\t%0.7lf\n", *matTranslation.GetValue(0, 0));
		printf("TY ->\t%0.7lf\n", *matTranslation.GetValue(1, 0));
		printf("TZ ->\t%0.7lf\n", *matTranslation.GetValue(2, 0));

		printf("\n");


		// 출력에 사용되는 3D 객채 생성 // Create 3D object used as output
		CFL3DObject fl3DDstObj;

		// Execution Source XYZV 이미지 설정 // Set Execution Source XYZV image
		if((res = colorizedPointCloudGenerator3D.SetSourceXYZVImage(fliExecSrcXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Execution Source XYZV image.\n");
			break;
		}

		// Execution Source Color 이미지 설정 // Set Execution Source Color image
		if((res = colorizedPointCloudGenerator3D.SetSourceColorImage(fliExecSrcColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Execution Source Color image.\n");
			break;
		}

		// Execution Destination Color 이미지 설정 // Set Execution Destination Color image
		if((res = colorizedPointCloudGenerator3D.SetDestinationColorImage(fliExecDstColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Execution Destination Color image.\n");
			break;
		}

		// Execution Destination Sampled Color 이미지 설정 // Set Execution Destination Sampled Color image
		if((res = colorizedPointCloudGenerator3D.SetDestinationSampledColorImage(fliSampDstColorImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Execution Destination Sampled Color image.\n");
			break;
		}

		// Sampled 픽셀 표시 Color 설정 // Set color of the sampled pixels in BGR
		if((res = colorizedPointCloudGenerator3D.SetSampledBGRValue(255, 255, 0)).IsFail())
		{
			ErrorPrint(res, L"Failed to set sampled pixel BGR value.\n");
			break;
		}

		// Execution Destination 3D Object 설정 // Set Execution Destination 3D Object
		if((res = colorizedPointCloudGenerator3D.SetDestination3DObject(fl3DDstObj)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Execution Destination 3D Object.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = colorizedPointCloudGenerator3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Colorized Point Cloud Generator 3D.\n");
			break;
		}


		// 결과 3D 객체 출력 // Print resulting 3D Object
		if((res = view3DDst.PushObject(fl3DDstObj)).IsFail())
		{
			ErrorPrint(res, L"Failed to display the 3D Object.\n");
			break;
		}

		// 3D View 카메라 설정 // Set 3D view camera
		CFL3DCamera fl3DCam;

		fl3DCam.SetDirection(CFLPoint3<float>(0, 0, 1));
		fl3DCam.SetDirectionUp(CFLPoint3<float>(0, -1, 0));
		fl3DCam.SetPosition(CFLPoint3<float>(0, 0, -1000));

		view3DDst.SetCamera(fl3DCam);

		// 화면에 출력하기 위해 이미지 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
	    // 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released
		CGUIViewImageLayerWrap layerImageCaliSrcXYZV = viewCaliSrcXYZVImage.GetLayer(0);
		CGUIViewImageLayerWrap layerImageCaliSrcColor = viewCaliSrcColorImage.GetLayer(0);
		CGUIViewImageLayerWrap layerImageExecSrcXYZV = viewExecSrcXYZVImage.GetLayer(0);
		CGUIViewImageLayerWrap layerImageExecSrcColor = viewExecSrcColorImage.GetLayer(0);
		CGUIViewImageLayerWrap layerImageExecDstColor = viewExecDstColorImage.GetLayer(0);
		CGUIViewImageLayerWrap layerImageSampDstColor = viewSampDstColorImage.GetLayer(0);

		// 화면에 출력하기 위해 3D 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from 3D view for display
		// 이 객체는 3D 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an 3D view and does not need to be released
		CGUIView3DLayerWrap layer3DDst = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear figures drawn on existing layer
		layerImageCaliSrcXYZV.Clear();
		layerImageCaliSrcColor.Clear();
		layerImageExecSrcXYZV.Clear();
		layerImageExecSrcColor.Clear();
		layerImageExecDstColor.Clear();
		layerImageSampDstColor.Clear();
		layer3DDst.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerImageCaliSrcXYZV.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Calibration Source XYZV Image", YELLOW, BLACK, 15)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerImageCaliSrcColor.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Calibration Source Color Image", YELLOW, BLACK, 15)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerImageExecSrcXYZV.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Execution Source XYZV Image", YELLOW, BLACK, 15)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerImageExecSrcColor.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Execution Source Color Image", YELLOW, BLACK, 15)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerImageExecDstColor.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Color Image", YELLOW, BLACK, 15)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerImageSampDstColor.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Sampled Color Image", YELLOW, BLACK, 15)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 3D 뷰 정보 표시 // Display 3D view information
		if((res = layer3DDst.DrawTextCanvas(&CFLPoint<double>(0, 0), L"3D Destination Colored Point Cloud", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}


		// 새로 생성한 이미지를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created image
		if((res = viewExecDstColorImage.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit image view.\n");
			break;
		}

		// 새로 생성한 이미지를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created image
		if((res = viewSampDstColorImage.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit image view.\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		viewCaliSrcXYZVImage.Invalidate(true);
		viewCaliSrcColorImage.Invalidate(true);
		viewExecSrcXYZVImage.Invalidate(true);
		viewExecSrcColorImage.Invalidate(true);
		viewExecDstColorImage.Invalidate(true);
		viewSampDstColorImage.Invalidate(true);

		// 3D 뷰를 갱신 // Update 3D view
		view3DDst.Invalidate(true);
		

		// 뷰가 닫히기 전까지 종료하지 않고 대기 // Wait until a view is closed before exiting
		while(viewCaliSrcXYZVImage.IsAvailable() && viewCaliSrcColorImage.IsAvailable() && viewExecSrcXYZVImage.IsAvailable() && viewExecSrcColorImage.IsAvailable() && viewExecDstColorImage.IsAvailable() && viewSampDstColorImage.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}