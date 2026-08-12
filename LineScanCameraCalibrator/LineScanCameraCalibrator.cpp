#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliImage[3];

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap arrViewImage[3];

	const wchar_t* pArrWcsViewName[3] = 
	{
		L"Calibration View",
		L"Source View",
		L"Destination View",
	};

	enum EView
	{
		EView_Calibration,
		EView_Source,
		EView_Destination,
		EView_Count
	};

	CResult res;

	do
	{
		// Learn 이미지 로드 // Load the Learn image
		if(IsFail(res = fliImage[EView_Calibration].Load(L"../../ExampleImages/LineScanCameraCalibrator/Calibration Image.flif")))
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		fliImage[EView_Source].Assign(fliImage[EView_Calibration]);

		for(int32_t i = 0; i < EView_Count; ++i)
		{
			// Learn 이미지 뷰 생성 // Create the Learn image view
			if(IsFail(res = arrViewImage[i].Create(300 + 480 * i, 0, 300 + 480 * (i + 1), 360)))
			{
				ErrorPrint(res, L"Failed to create the image view.\n");
				break;
			}

			// Learn 이미지 뷰에 이미지를 디스플레이 // Display the image in the Learn image view
			if(IsFail(res = arrViewImage[i].SetImagePtr(&fliImage[i])))
			{
				ErrorPrint(res, L"Failed to set image object on the image view.\n");
				break;
			}

			// 두 이미지 뷰의 시점을 동기화 한다. // Synchronize the viewpoints of the two image views.

			if(i)
			{
				if(IsFail(res = arrViewImage[i].SynchronizePointOfView(&arrViewImage[0])))
				{
					ErrorPrint(res, "Failed to synchronize view\n");
					break;
				}
			}
		}

		if(IsFail(res = arrViewImage[EView_Calibration].ZoomFit()))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// calibration
		// 캘리브레이션 객체 생성 // Create Calibrator object
		CLineScanCameraCalibrator lineScanCameraCalibrator;

		// 캘리브레이션 이미지 설정 // Set calibration image
		lineScanCameraCalibrator.SetCalibrationImage(fliImage[EView_Calibration]);
		// 보드의 셀 간격 설정(mm) // Sets the board cell pitch (mm).
		lineScanCameraCalibrator.SetCellPitch(10);
		// 카메라의 픽셀 정밀도 설정(mm) // Sets the camera pixel accuracy (mm).
		lineScanCameraCalibrator.SetPixelAccuracy(0.19);

		// 측정 기준 위치 설정. x축 좌표는 무시되며 y축 좌표기준으로 이미지 범위의 line을 검사
		// Sets the measurement reference position. The x-coordinate is ignored, and the image is inspected along the y-coordinate.
		CFLPoint<double> flp(0, 160);
		lineScanCameraCalibrator.SetMeasurementPosition(flp);

		// 캘리브레이션 // Calibration
		if(IsFail(res = lineScanCameraCalibrator.Calibrate()))
		{
			ErrorPrint(res, L"Undistortion failed\n");
			break;
		}

		// 캘리브레이션 결과 출력 // Display the calibration result.
		CGUIViewImageLayerWrap layer = arrViewImage[EView_Calibration].GetLayer(0);

		// 셀 간격의 픽셀 크기 // Cell pitch in pixels.
		double f64PixelPerPitch = lineScanCameraCalibrator.GetCellPitch() / lineScanCameraCalibrator.GetPixelAccuracy();

		// 실제 측정되는 범위 // Actual measurement range.
		CFLLine<double> fllMeasurementLine;

		fllMeasurementLine.flpPoints[0].x = 0;
		fllMeasurementLine.flpPoints[1].x = (double)fliImage[EView_Calibration].GetWidth();

		fllMeasurementLine.flpPoints[0].y = fllMeasurementLine.flpPoints[1].y = flp.y;

		// 측정된 결과 제어점 // Measured control points.
		CFLPointArray flpaCalibratedPoints;
		lineScanCameraCalibrator.GetCalibratedControlPoints(flpaCalibratedPoints);

		CFLFigureArray flfaCrosshair;
		flpaCalibratedPoints.MakeCrossHairElementwise(flfaCrosshair, f64PixelPerPitch / 10, true);

		layer.DrawFigureImage(fllMeasurementLine, RED, 2);
		layer.DrawFigureImage(flfaCrosshair, CYAN, 2);

		// execute (undistortion)
		// 연산 이미지, 연산 결과 이미지 설정 // Set source image, destination image
		lineScanCameraCalibrator.SetSourceImage(fliImage[EView_Source]);
		lineScanCameraCalibrator.SetDestinationImage(fliImage[EView_Destination]);

		// 왜곡 보정 동작 // Undistortion
		if(IsFail(res = lineScanCameraCalibrator.Execute()))
		{
			ErrorPrint(res, L"Undistortion failed\n");
			break;
		}

		// 뷰 이름 출력 // display view name
		for(int32_t i = 0; i < EView_Count; ++i)
		{
			CGUIViewImageLayerWrap currentLayer = arrViewImage[i].GetLayer(0);

			currentLayer.DrawTextImage(CFLPoint<double>(), pArrWcsViewName[i], YELLOW, BLACK, 20);
			arrViewImage[i].Invalidate();
		}

		// The image view is waiting until close.
		bool bAvailable = true;

		while(bAvailable)
		{
			for(int32_t i = 0; i < EView_Count; ++i)
				bAvailable &= arrViewImage[i].IsAvailable();

			CThreadUtilities::Sleep(1);
		}
	}
	while(false);

	return 0;
}