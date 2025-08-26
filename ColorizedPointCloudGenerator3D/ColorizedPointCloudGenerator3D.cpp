#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliCaliSrcXYZVImage;
	CFLImage fliCaliSrcRGBImage;
	CFLImage fliExecSrcXYZVImage;
	CFLImage fliExecSrcRGBImage;
	CFLImage fliExecDstRGBImage;
	CFLImage fliSampDstRGBImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageCaliSrcXYZV;
	CGUIViewImageWrap viewImageCaliSrcRGB;
	CGUIViewImageWrap viewImageExecSrcXYZV;
	CGUIViewImageWrap viewImageExecSrcRGB;
	CGUIViewImageWrap viewImageExecDstRGB;
	CGUIViewImageWrap viewImageSampDstRGB;
	CGUIView3DWrap view3DDst;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult res = EResult_UnknownError;


		// Calibration Source XYZV 이미지 로드 // Load the calibration source XYZV image
		if((res = fliCaliSrcXYZVImage.Load(L"../../ExampleImages/ColorizedPointCloudGenerator3D/CalibXYZV.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Calibration Source XYZV 이미지 뷰 생성 // Create the calibration source XYZV image view
		if((res = viewImageCaliSrcXYZV.Create(100, 0, 400, 300)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Calibration Source XYZV 이미지 뷰에 이미지를 디스플레이 // Display the image in the calibration source XYZV image view
		if((res = viewImageCaliSrcXYZV.SetImagePtr(&fliCaliSrcXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// Calibration Source RGB 이미지 로드 // Load the calibration source RGB image
		if((res = fliCaliSrcRGBImage.Load(L"../../ExampleImages/ColorizedPointCloudGenerator3D/CalibRGB.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Calibration Source RGB 이미지 뷰 생성 // Create the calibration source RGB image view
		if((res = viewImageCaliSrcRGB.Create(100, 300, 400, 600)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Calibration Source RGB 이미지 뷰에 이미지를 디스플레이 // Display the image in the calibration source RGB image view
		if((res = viewImageCaliSrcRGB.SetImagePtr(&fliCaliSrcRGBImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// Execution Source XYZV 이미지 로드 // Load the Execution source XYZV image
		if((res = fliExecSrcXYZVImage.Load(L"../../ExampleImages/ColorizedPointCloudGenerator3D/ExecXYZV.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Execution Source XYZV 이미지 뷰 생성 // Create the Execution source XYZV image view
		if((res = viewImageExecSrcXYZV.Create(400, 0, 700, 300)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Execution Source XYZV 이미지 뷰에 이미지를 디스플레이 // Display the image in the Execution source XYZV image view
		if((res = viewImageExecSrcXYZV.SetImagePtr(&fliExecSrcXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// Execution Source RGB 이미지 로드 // Load the Execution source RGB image
		if((res = fliExecSrcRGBImage.Load(L"../../ExampleImages/ColorizedPointCloudGenerator3D/ExecRGB.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Execution Source RGB 이미지 뷰 생성 // Create the Execution source RGB image view
		if((res = viewImageExecSrcRGB.Create(400, 300, 700, 600)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Execution Source RGB 이미지 뷰에 이미지를 디스플레이 // Display the image in the Execution source RGB image view
		if((res = viewImageExecSrcRGB.SetImagePtr(&fliExecSrcRGBImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// Execution Destination RGB 이미지 뷰 생성 // Create the execution destination RGB image view
		if((res = viewImageExecDstRGB.Create(700, 0, 1000, 300)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the execution destination RGB image view
		if((res = viewImageExecDstRGB.SetImagePtr(&fliExecDstRGBImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// Execution Sampled RGB 이미지 뷰 생성 // Create the execution destination RGB image view
		if((res = viewImageSampDstRGB.Create(700, 300, 1000, 600)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the execution destination RGB image view
		if((res = viewImageSampDstRGB.SetImagePtr(&fliSampDstRGBImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// Destination 3D 이미지 뷰 생성 // Create the destination 3D image view
		if((res = view3DDst.Create(1000, 0, 1600, 600)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}


		// 두 이미지 뷰 윈도우의 Page를 동기화 한다 // Synchronize the pages of the two image view windows
		if((res = viewImageCaliSrcXYZV.SynchronizePageIndex(&viewImageCaliSrcRGB)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}


		// ColorizedPointCloudGenerator3D 객체 생성 // Create ColorizedPointCloudGenerator3D object
		CColorizedPointCloudGenerator3D colorizedPointCloudGenerator3D;

		// Calibration XYZV 이미지 설정 // Set the calibration XYZV image
		if((res = colorizedPointCloudGenerator3D.SetCalibrationImageXYZV(fliCaliSrcXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set calibration XYZV source.\n");
			break;
		}

		// Calibration RGB 이미지 설정 // Set the calibration RGB image
		if((res = colorizedPointCloudGenerator3D.SetCalibrationImageRGB(fliCaliSrcRGBImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set calibration RGB source.\n");
			break;
		}

		// Calibration의 Grid Type 설정 // Set the grid type of the calibration
		if((res = colorizedPointCloudGenerator3D.SetGridType(AdvancedFunctions::CCameraCalibrator::EGridType_ChessBoard)).IsFail())
		{
			ErrorPrint(res, L"Failed to set calibration grid type.\n");
			break;
		}

		// Calibration의 최적해 정확도 값 설정 // Set the optimal solution accuracy of the calibration
		if((res = colorizedPointCloudGenerator3D.SetOptimalSolutionAccuracy(0.00001)).IsFail())
		{
			ErrorPrint(res, L"Failed to set calibration optimal solution accuracy.\n");
			break;
		}

		// Coordinate Adjustment 자동 설정 // Coordinate Adjustment Auto Set Flag
		if((res = colorizedPointCloudGenerator3D.EnableAutoCoordinateAdjustment(true)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Coordinate Adjustment Flag.\n");
			break;
		}

		// 알고리즘 Calibration 실행 // Execute calibration of the algorithm
		if((res = colorizedPointCloudGenerator3D.Calibrate()).IsFail())
		{
			ErrorPrint(res, L"Failed to calibrate algorithm.\n");
			break;
		}


		// Calibration 결과 출력 // Print calibration results
		printf(" < Calibration Result >\n\n");

		// RGB 카메라의 Intrinsic Parameter 출력 // Print the intrinsic parameters of the RGB camera
		AdvancedFunctions::CCameraCalibrator::CCalibratorIntrinsicParameters cCalibIntrinsic;

		cCalibIntrinsic = colorizedPointCloudGenerator3D.GetIntrinsicParameters();

		printf(" < Intrinsic Parameters >\n");

		printf("Focal Length X ->\t%0.7lf\n", cCalibIntrinsic.f64FocalLengthX);
		printf("Focal Length Y ->\t%0.7lf\n", cCalibIntrinsic.f64FocalLengthY);
		printf("Principal Point X ->\t%0.7lf\n", cCalibIntrinsic.f64PrincipalPointX);
		printf("Principal Point Y ->\t%0.7lf\n", cCalibIntrinsic.f64PrincipalPointY);
		printf("Skew ->\t%0.7lf\n", cCalibIntrinsic.f64Skew);

		// RGB 카메라의 Distortion Coefficient 출력 // Print the distortion coefficients of the RGB camera
		AdvancedFunctions::CCameraCalibrator::CCalibratorDistortionCoefficients cCalibDistortion;

		cCalibDistortion = colorizedPointCloudGenerator3D.GetDistortionCoefficients();

		printf(" < Distortion Coefficients >\n");

		printf("K1 ->\t%0.7lf\n", cCalibDistortion.f64K1);
		printf("K2 ->\t%0.7lf\n", cCalibDistortion.f64K2);
		printf("P1 ->\t%0.7lf\n", cCalibDistortion.f64P1);
		printf("P2 ->\t%0.7lf\n", cCalibDistortion.f64P2);
		printf("K3 ->\t%0.7lf\n", cCalibDistortion.f64K3);

		// 두 카메라 간의 회전 행렬 출력 // Print the relative rotation matrix between both cameras
		CMatrix<double> cMatRotation;

		if((res = colorizedPointCloudGenerator3D.GetRelativeRotation(cMatRotation)).IsFail())
		{
			ErrorPrint(res, L"Failed to get relative rotation.\n");
			break;
		}

		printf(" < Relative Rotation >\n");

		printf("R00 ->\t%0.7lf\n", *cMatRotation.GetValue(0, 0));
		printf("R01 ->\t%0.7lf\n", *cMatRotation.GetValue(0, 1));
		printf("R02 ->\t%0.7lf\n", *cMatRotation.GetValue(0, 2));
		printf("R10 ->\t%0.7lf\n", *cMatRotation.GetValue(1, 0));
		printf("R11 ->\t%0.7lf\n", *cMatRotation.GetValue(1, 1));
		printf("R12 ->\t%0.7lf\n", *cMatRotation.GetValue(1, 2));
		printf("R20 ->\t%0.7lf\n", *cMatRotation.GetValue(2, 0));
		printf("R21 ->\t%0.7lf\n", *cMatRotation.GetValue(2, 1));
		printf("R22 ->\t%0.7lf\n", *cMatRotation.GetValue(2, 2));

		// 두 카메라 간의 변환 행렬 출력 // Print the relative translation matrix between both cameras
		CMatrix<double> cMatTranslation;

		if((res = colorizedPointCloudGenerator3D.GetRelativeTranslation(cMatTranslation)).IsFail())
		{
			ErrorPrint(res, L"Failed to get relative translation.\n");
			break;
		}

		printf(" < Relative Translation >\n");

		printf("TX ->\t%0.7lf\n", *cMatTranslation.GetValue(0, 0));
		printf("TY ->\t%0.7lf\n", *cMatTranslation.GetValue(1, 0));
		printf("TZ ->\t%0.7lf\n", *cMatTranslation.GetValue(2, 0));


		// 출력에 사용되는 3D 객채 생성 // Create 3D object used as output
		CFL3DObject fli3DDstObj;

		// Execution XYZV 이미지 설정 // Set the execution XYZV image
		if((res = colorizedPointCloudGenerator3D.SetSourceImageXYZV(fliExecSrcXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set execution XYZV source.\n");
			break;
		}

		// Execution RGB 이미지 설정 // Set the execution RGB image
		if((res = colorizedPointCloudGenerator3D.SetSourceImageRGB(fliExecSrcRGBImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set execution RGB source.\n");
			break;
		}

		// Destination RGB 이미지 설정 // Set the destination RGB image
		if((res = colorizedPointCloudGenerator3D.SetDestinationImageRGB(fliExecDstRGBImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set destination RGB source.\n");
			break;
		}

		// Destination Sampled RGB 이미지 설정 // Set the destination sampled RGB image
		if((res = colorizedPointCloudGenerator3D.SetSampledImageRGB(fliSampDstRGBImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set destination sampled RGB source.\n");
			break;
		}

		// Sampled 픽셀 표시 RGB 설정 // Set the color of the sampled pixels in RGB
		if((res = colorizedPointCloudGenerator3D.SetSampledRGBValue(0, 255, 255)).IsFail())
		{
			ErrorPrint(res, L"Failed to set destination sampled RGB source.\n");
			break;
		}

		// Destination 3D Object 설정 // Set the destination 3D object
		if((res = colorizedPointCloudGenerator3D.SetDestination3DObject(fli3DDstObj)).IsFail())
		{
			ErrorPrint(res, L"Failed to set destination 3D point cloud.\n");
			break;
		}

		// 알고리즘 실행 // Execute algorithm
		if((res = colorizedPointCloudGenerator3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute algorithm.\n");
			break;
		}


		// 결과 3D 객체 출력 // Print 3D Object
		if((res = view3DDst.PushObject(fli3DDstObj)).IsFail())
		{
			ErrorPrint(res, L"Failed to display the 3D object");
			break;
		}

		// 3D View 카메라 설정 // Set 3D view camera
		CFL3DCamera fl3DCam;

		fl3DCam.SetDirection(CFLPoint3<float>(0, 0, 1));
		fl3DCam.SetDirectionUp(CFLPoint3<float>(0, -1, 0));
		fl3DCam.SetPosition(CFLPoint3<float>(0, 0, -1000));

		view3DDst.SetCamera(fl3DCam);

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerImageCaliSrcXYZV = viewImageCaliSrcXYZV.GetLayer(0);
		CGUIViewImageLayerWrap layerImageCaliSrcRGB = viewImageCaliSrcRGB.GetLayer(0);
		CGUIViewImageLayerWrap layerImageExecSrcXYZV = viewImageExecSrcXYZV.GetLayer(0);
		CGUIViewImageLayerWrap layerImageExecSrcRGB = viewImageExecSrcRGB.GetLayer(0);
		CGUIViewImageLayerWrap layerImageExecDstRGB = viewImageExecDstRGB.GetLayer(0);
		CGUIViewImageLayerWrap layerImageSampDstRGB = viewImageSampDstRGB.GetLayer(0);
		CGUIView3DLayerWrap layer3DDst = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerImageCaliSrcXYZV.Clear();
		layerImageCaliSrcRGB.Clear();
		layerImageExecSrcXYZV.Clear();
		layerImageExecSrcRGB.Clear();
		layerImageExecDstRGB.Clear();
		layerImageSampDstRGB.Clear();
		layer3DDst.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerImageCaliSrcXYZV.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Calibration Source XYZV Image", YELLOW, BLACK, 15)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerImageCaliSrcRGB.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Calibration Source RGB Image", YELLOW, BLACK, 15)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerImageExecSrcXYZV.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Execution Source XYZV Image", YELLOW, BLACK, 15)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerImageExecSrcRGB.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Execution Source RGB Image", YELLOW, BLACK, 15)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerImageExecDstRGB.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Execution Destination RGB Image", YELLOW, BLACK, 15)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerImageSampDstRGB.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Execution Sampled RGB Image", YELLOW, BLACK, 15)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layer3DDst.DrawTextCanvas(&CFLPoint<double>(0, 0), L"3D Colored Point Cloud", YELLOW, BLACK, 15)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}


		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageExecDstRGB.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit of the image view.\n");
			break;
		}

		if((res = viewImageSampDstRGB.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit of the image view.\n");
			break;
		}

		// 이미지 뷰와 3D 뷰를 갱신 합니다. // Update image views and 3D view
		viewImageCaliSrcXYZV.Invalidate(true);
		viewImageCaliSrcRGB.Invalidate(true);
		viewImageExecSrcXYZV.Invalidate(true);
		viewImageExecSrcRGB.Invalidate(true);
		viewImageExecDstRGB.Invalidate(true);
		viewImageSampDstRGB.Invalidate(true);
		view3DDst.Invalidate(true);

		// 이미지 뷰와 3D 뷰가 종료될 때 까지 기다림 // Wait for the image and 3D view to close
		while(viewImageCaliSrcXYZV.IsAvailable() && viewImageCaliSrcRGB.IsAvailable() && viewImageExecSrcXYZV.IsAvailable() && viewImageExecSrcRGB.IsAvailable() && viewImageExecDstRGB.IsAvailable() && viewImageSampDstRGB.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}