#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

bool Calibration(COrthogonalCalibrator& orthogonalCalibrator, const CFLImage& fliBoardImage)
{
	bool bResult = false;
	CResult res;

	do
	{
		// Learn 이미지 설정 // Learn image settings
		if((res = orthogonalCalibrator.SetCalibrationImage(&fliBoardImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image\n");
			break;
		}

		// 직교 보정 계산을 할 Learn 이미지 설정 // Learn image settings for orthogonal correction
		if((res = orthogonalCalibrator.SetOrthogonalCorrectionImage(fliBoardImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image\n");
			break;
		}

		// Calibator할 대상 종류를 설정합니다. // Set the target type for Calibator.
		orthogonalCalibrator.SetGridType(CCameraCalibrator::EGridType_ChessBoard);
		// 결과에 대한 학습률을 설정합니다. // Set the learning rate for the result.
		orthogonalCalibrator.SetOptimalSolutionAccuracy();

		// Calibration 실행 // Execute Calibration
		if((res = orthogonalCalibrator.Calibrate()).IsFail())
		{
			ErrorPrint(res, L"Calibration failed\n");
			break;
		}

		bResult = true;
	}
	while(false);

	return bResult;
}

bool Undistortion(COrthogonalCalibrator& orthogonalCalibrator, CFLImage& fliSourceImage, CFLImage& fliDestinationImage)
{
	bool bResult = false;
	CResult res;

	do
	{
		// Source 이미지 설정 // Set Source image
		orthogonalCalibrator.SetSourceImage(&fliSourceImage);

		// Destination 이미지 설정 // Set destination image
		orthogonalCalibrator.SetDestinationImage(&fliDestinationImage);

		// Interpolation 알고리즘 설정 // Set the Interpolation Algorithm
		if((res = orthogonalCalibrator.SetInterpolationMethod(ImageProcessing::EInterpolationMethod_Bilinear)).IsFail())
		{
			ErrorPrint(res, L"Failed to set interpolation method\n");
			break;
		}

		// Undistortion 실행 // Execute Undistortion
		if((res = orthogonalCalibrator.Execute()).IsFail())
		{
			ErrorPrint(res, L"Undistortion failed\n");
			break;
		}

		bResult = true;
	}
	while(false);

	return bResult;
}

bool CalculatePixelAccuracy(COrthogonalCalibrator& orthogonalCalibrator, const double f64BoardCellPitch, double& f64PixelAccuracy)
{
	bool bResult = false;

	do
	{
		COrthogonalCalibrator::CCalibratorGridResult gridResult;

		orthogonalCalibrator.GetResultGridPoints(orthogonalCalibrator.GetSourceImage()->GetSelectedPageIndex(), 0, &gridResult);

		if(CFLFigureArray* pFlfa = (CFLFigureArray*)gridResult.flfaGridData[0])
		{
			CFLPoint<double> flp1, flp2;
			orthogonalCalibrator.ConvertCoordinate((CFLPoint<double>*)pFlfa->Front(), &flp1);
			orthogonalCalibrator.ConvertCoordinate((CFLPoint<double>*)pFlfa->Back(), &flp2);

			CFLLine<double> fliLine2(flp1, flp2);

			CFLImage* pFliDst = (CFLImage*)orthogonalCalibrator.GetDestinationImage();

			pFliDst->PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(fliLine2));
			f64PixelAccuracy = f64BoardCellPitch / (fliLine2.GetLength() / (double)(pFlfa->GetCount() - 1));
			bResult = true;
		}
		else
			bResult = false;
		
	}
	while(false);

	return bResult;
}

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliDistortionChessBoard, fliUndistortedChessBoard, fliDistortedMeasurementImage, fliUndistortedMeasurementImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageDistortionChessBoard, viewImageUndistortionChessBoard, viewImageDistortionMeasurement, viewImageUndistortionMeasurement;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		{
			// Image View 생성 // Create image view
			if((res = viewImageDistortionChessBoard.Create(0, 0, 500, 500)).IsFail())
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				break;
			}

			if((res = viewImageUndistortionChessBoard.Create(500, 0, 1000, 500)).IsFail())
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				break;
			}

			if((res = viewImageDistortionMeasurement.Create(0, 500, 500, 1000)).IsFail())
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				break;
			}

			if((res = viewImageUndistortionMeasurement.Create(500, 500, 1000, 1000)).IsFail())
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				break;
			}

			// 이미지 포인터 설정 // Image pointer settings
			viewImageDistortionChessBoard.SetImagePtr(&fliDistortionChessBoard);
			viewImageUndistortionChessBoard.SetImagePtr(&fliUndistortedChessBoard);

			viewImageDistortionMeasurement.SetImagePtr(&fliDistortedMeasurementImage);
			viewImageUndistortionMeasurement.SetImagePtr(&fliUndistortedMeasurementImage);
		}

		// Orthogonal Calibrator 클래스 선언 // Declare COrthogonal Calibrator class instance.
		COrthogonalCalibrator orthogonalCalibrator;

		// Learn 이미지 로드 // Load the Learn image
		if(IsFail(res = fliDistortionChessBoard.Load(L"../../ExampleImages/Measurement/ChessBoard.flif")))
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		if(!Calibration(orthogonalCalibrator, fliDistortionChessBoard))
			break;

		if(!Undistortion(orthogonalCalibrator, fliDistortionChessBoard, fliUndistortedChessBoard))
			break;

		// Board cell pitch 설정 // Board cell pitch settings
		const double f64BoardCellPitch = 15;
		double f64PixelAccuracy;

		if(!CalculatePixelAccuracy(orthogonalCalibrator, f64BoardCellPitch, f64PixelAccuracy))
			break;

		// 측정 이미지 로드 // Load the measurement image
		if((res = fliDistortedMeasurementImage.Load(L"../../ExampleImages/Measurement/Measurement.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		if(!Undistortion(orthogonalCalibrator, fliDistortedMeasurementImage, fliUndistortedMeasurementImage))
			break;

		// Rectangle Gauge 클래스 선언 // Declare Rectangle Gauge class instance.
		CRectangleGauge rectangleGauge;

		// Source 이미지 설정 // Set Source image			
		rectangleGauge.SetSourceImage(fliUndistortedMeasurementImage);

		// 측정할 영역을 설정합니다. // Set the area to measure.
		CFLRect<double> flrMeasureRegion(274, 335, 493, 481, -8.06731650598383);
		rectangleGauge.SetMeasurementRegion(flrMeasureRegion, 20.000000);

		// 알고리즘 수행 // Execute the algorithm
		if((res = rectangleGauge.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Rectangle gauge.\n");
			break;
		}

		// 실행 결과를 가져옵니다. // Get the execution result.
		CFLRect<double> flrResult;
		rectangleGauge.GetMeasuredObject(flrResult, 0);

		CFLPoint<double> flpLeftTop(flrResult.left, flrResult.top);
		CFLPoint<double> flpRightTop(flrResult.right, flrResult.top);
		CFLPoint<double> flpLeftBottom(flrResult.left, flrResult.bottom);
		CFLPoint<double> flpRightBottom(flrResult.right, flrResult.bottom);

		CFLLine<double> fliTop(flpLeftTop, flpRightTop);
		CFLLine<double> fliRight(flpRightTop, flpRightBottom);
		CFLLine<double> fliBottom(flpLeftBottom, flpRightBottom);
		CFLLine<double> fliLeft(flpLeftTop, flpLeftBottom);

		// 측정된 사각형의 실제 길이를 계산합니다. // Calculate the actual length of the measured rectangle.
		double f64LeftLength = fliLeft.GetLength() * f64PixelAccuracy;
		double f64TopLength = fliTop.GetLength() * f64PixelAccuracy;
		double f64RightLength = fliRight.GetLength() * f64PixelAccuracy;
		double f64BottomLength = fliBottom.GetLength() * f64PixelAccuracy;

		// 이미지 뷰 정보 표시 // Display image view information		
		CGUIViewImageLayerWrap layerDistortionChessBoard = viewImageDistortionChessBoard.GetLayer(0);
		CGUIViewImageLayerWrap layerUndistortionChessBoard = viewImageUndistortionChessBoard.GetLayer(0);
		CGUIViewImageLayerWrap layerDistortionMeasurement = viewImageDistortionMeasurement.GetLayer(0);
		CGUIViewImageLayerWrap layerUndistortionMeasurement = viewImageUndistortionMeasurement.GetLayer(0);
		CFLPoint<double> flpPoint(0, 0);

		if((res = layerDistortionChessBoard.DrawTextImage(&flpPoint, L"Distortion ChessBoard", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layerUndistortionChessBoard.DrawTextImage(&flpPoint, CFLStringW().Format(L"Undistortion ChessBoard \nBoard Cell Pitch : %lf \nPixel Accuracy : %lf", f64BoardCellPitch, f64PixelAccuracy), YELLOW, BLACK, 20)).IsFail() ||
		   (res = layerDistortionMeasurement.DrawTextImage(&flpPoint, L"Distortion Measurement", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layerUndistortionMeasurement.DrawTextImage(&flpPoint, L"Undistortion & Measurement Result", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layerUndistortionMeasurement.DrawTextImage(&fliLeft.GetCenter(), CFLStringW().Format(L"%lf (mm)", f64LeftLength), YELLOW, BLACK, 12)).IsFail() ||
		   (res = layerUndistortionMeasurement.DrawTextImage(&fliTop.GetCenter(), CFLStringW().Format(L"%lf (mm)", f64TopLength), YELLOW, BLACK, 12)).IsFail() ||
		   (res = layerUndistortionMeasurement.DrawTextImage(&fliRight.GetCenter(), CFLStringW().Format(L"%lf (mm)", f64RightLength), YELLOW, BLACK, 12)).IsFail() ||
		   (res = layerUndistortionMeasurement.DrawTextImage(&fliBottom.GetCenter(), CFLStringW().Format(L"%lf (mm)", f64BottomLength), YELLOW, BLACK, 12)).IsFail() ||
		   (res = layerUndistortionMeasurement.DrawFigureImage(flrResult, CYAN, 5)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다.
		viewImageDistortionChessBoard.Invalidate(true);
		viewImageDistortionMeasurement.Invalidate(true);
		viewImageUndistortionChessBoard.Invalidate(true);
		viewImageUndistortionMeasurement.Invalidate(true);

		viewImageDistortionChessBoard.ZoomFit();
		viewImageDistortionMeasurement.ZoomFit();
		viewImageUndistortionChessBoard.ZoomFit();
		viewImageUndistortionMeasurement.ZoomFit();

		// 이미지 뷰가 꺼지면 종료로 간주
		while(viewImageDistortionChessBoard.IsAvailable() && viewImageUndistortionChessBoard.IsAvailable() && viewImageDistortionMeasurement.IsAvailable() && viewImageUndistortionMeasurement.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}