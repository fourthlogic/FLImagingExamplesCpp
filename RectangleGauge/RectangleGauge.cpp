#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	const int32_t i32ExampleCount = 12;
	const wchar_t* arrWcsViewText[] =
	{
		L"Dark To Bright Or Bright To Dark\nBegin 0",
		L"Dark To Bright Or Bright To Dark\nBegin 1",
		L"Dark To Bright Or Bright To Dark\nBegin 2",
		L"Dark To Bright Or Bright To Dark\nLargest Area",
		L"Dark To Bright Or Bright To Dark\nEnd 0",
		L"Dark To Bright Or Bright To Dark\nEnd 1",
		L"Dark To Bright Or Bright To Dark\nEnd 2",
		L"Dark To Bright Or Bright To Dark\nLargest Amplitude",
		L"Dark To Bright Or Bright To Dark\nClosest",
		L"Dark To Bright\nClosest",
		L"Bright To Dark\nClosest",
		L"Dark To Bright To Dark\nClosest",
	};

	CRectangleGauge::ETransitionType arrTransitionType[] =
	{
		CRectangleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CRectangleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CRectangleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CRectangleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CRectangleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CRectangleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CRectangleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CRectangleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CRectangleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CRectangleGauge::ETransitionType_DarkToBright,
		CRectangleGauge::ETransitionType_BrightToDark,
		CRectangleGauge::ETransitionType_DarkToBrightToDark,
	};

	CRectangleGauge::ETransitionChoice arrTransitionChoice[] =
	{
		CRectangleGauge::ETransitionChoice_Begin,
		CRectangleGauge::ETransitionChoice_Begin,
		CRectangleGauge::ETransitionChoice_Begin,
		CRectangleGauge::ETransitionChoice_LargestArea,
		CRectangleGauge::ETransitionChoice_End,
		CRectangleGauge::ETransitionChoice_End,
		CRectangleGauge::ETransitionChoice_End,
		CRectangleGauge::ETransitionChoice_LargestAmplitude,
		CRectangleGauge::ETransitionChoice_Closest,
		CRectangleGauge::ETransitionChoice_Closest,
		CRectangleGauge::ETransitionChoice_Closest,
		CRectangleGauge::ETransitionChoice_Closest,
	};

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImage[i32ExampleCount];
	CResult res;

	do
	{
		// 이미지 로드 // Loads image
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/Gauge/Rect.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Creates imageview
		for(int32_t i = 0; i < i32ExampleCount; ++i)
		{
			int32_t i32X = 300 * (i % 4);
			int32_t i32Y = 300 * (i / 4);

			if(IsFail(res = viewImage[i].Create(i32X, i32Y, i32X + 300, i32Y + 300)))
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				break;
			}

			// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
			if(IsFail(res = viewImage[i].SetImagePtr(&fliImage)))
			{
				ErrorPrint(res, "Failed to set image object on the image view.\n");
				break;
			}

			// 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the all image views. 
			if(i)
			{
				if(IsFail(res = viewImage[i].SynchronizePointOfView(&viewImage[0])))
				{
					ErrorPrint(res, "Failed to set image object on the image view.\n");
					break;
				}
			}
		}

		CResult res = EResult_UnknownError;

		// Rectangle Gauge 객체 생성 // Create a Rectangle Gauge object
		CRectangleGauge rectangleGauge;

		// 처리할 이미지 설정 // Set the image to process
		rectangleGauge.SetSourceImage(fliImage);

		// 측정할 영역을 설정합니다. // Set the area to measure.
		CFLRect<double> measureRegion(213.577428,262.324155,295.020437,348.179290);
		double tolerance = 50.;
		rectangleGauge.SetMeasurementRegion(measureRegion, tolerance);

		// 추출하기위한 파라미터를 설정합니다. // Set parameters for extraction.		
		// 사각형을 추정하기위해 추출할 경계점의 변화 임계값에 대해 설정합니다. // Set the threshold change of the boundary point to be extracted to estimate the rectangle.
		rectangleGauge.SetThreshold(20);
		// 사각형을 추정하기위해 추출할 경계점의 변화 임계값에 보정값을 설정합니다. // Set the correction value to the threshold change of the boundary point to be extracted to estimate the rectangle.
		rectangleGauge.SetMinimumAmplitude(10);
		// 사각형을 추정하기위해 추출할 경계점들의 대표값 표본 개수를 설정합니다. // Set the number of representative sample values ??of the boundary points to be extracted to estimate the rectangle.
		rectangleGauge.SetThickness(1);
		// 사각형을 추정하기위해 추출할 경계점들의 추출 간격을 설정합니다. // Set the extraction interval of boundary points to be extracted to estimate the rectangle.
		rectangleGauge.SetSamplingStep(1.);
		// 사각형을 추정하기위해 추출할 경계점들의 이상치 조정을 위한 임계값을 설정합니다. // Set the threshold value for outlier adjustment of the boundary points to be extracted to estimate the rectangle.
		rectangleGauge.SetOutliersThreshold(3.);
		// 사각형을 추정하기위해 추출할 경계점들의 이상치 조정 횟수을 설정합니다. // Set the number of outlier adjustments for boundary points to be extracted to estimate the rectangle.
		rectangleGauge.SetOutliersThresholdCount(3);
		// 사각형을 추정하기위해 점 클러스터링 처리 유무에 대한 설정을 합니다. // Set whether or not to process point clustering to estimate the rectangle.
		rectangleGauge.EnableClusterMode(true);
		// 사각형을 추정하기위해 마진을 설정합니다. 필요에 따라 각 구역별로 설정가능합니다. // Set the margin to estimate the rectangle. It can be set for each zone as needed.
		rectangleGauge.SetMeasurementMarginRatio(0., CRectangleGauge::EMargin_All);
		// 사각형을 추정하기위한 Tolerance를 설정합니다. 필요에 따라 각 구역별로 설정가능합니다. // Set the Tolerance for estimating the rectangle. It can be set for each zone as needed.
		rectangleGauge.SetTolerance(tolerance, CRectangleGauge::ETolerance_All);

		for(int32_t i = 0; i < i32ExampleCount; ++i)
		{
			// 사각형을 추정하기위해 추출할 경계점 변화 방향에 대해 설정합니다. // Set the boundary point change direction to extract to estimate the rectangle.
			rectangleGauge.SetTransitionType(arrTransitionType[i]);
			// 사각형을 추정하기위해 추출한 경계점 중 사용할 경계점 유형을 선택합니다. // Select the boundary point type to use among the boundary points extracted to estimate the rectangle.
			rectangleGauge.SetTransitionChoice(arrTransitionChoice[i]);
			// 알고리즘 수행 // Execute the algorithm
			if(IsFail(res = rectangleGauge.Execute()))
			{
				ErrorPrint(res, "Failed to execute Rectangle gauge.\n");
				break;
			}

			// 실행 결과를 가져옵니다. // Get the execution result.
			CFLRect<double> flrResult;
			CFLFigureArray flfaResultsValid, flfaResultsInvalid;
			// 추정된 사각형을 가져옵니다. // Get the estimated rectangle.
			res = rectangleGauge.GetMeasuredObject(flrResult, i % 4);
			// 추정된 사각형을 추출에 사용된 유효 경계점을 가져옵니다. // Get the valid bounding point used to extract the estimated rectangle.
			rectangleGauge.GetMeasuredValidPoints(flfaResultsValid, i % 4);
			// 추정된 사각형을 추출에 사용되지 못한 유효하지 않은 경계점을 가져옵니다. // Get an invalid bounding point that was not used to extract the estimated rectangle.
			rectangleGauge.GetMeasuredInvalidPoints(flfaResultsInvalid, i % 4);

			CGUIViewImageLayerWrap layer = viewImage[i].GetLayer(0);

			layer.Clear();

			if(IsFail(res = layer.DrawTextImage(CFLPoint<double>(0., 0.), arrWcsViewText[i], YELLOW, BLUE, 20, true)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			// 측정 영역이 어디인지 알기 위해 디스플레이 한다 // Display to know where the measurement area is
			CFLRect<double> flrRegion = rectangleGauge.GetMeasurementRegion();
			CFLArray<double> arrTolerance = rectangleGauge.GetTolerance();
			double f64Tolerance = arrTolerance[0];

			double arrF64Tolerance[4];
			double f64WidthTolerance, f64HeightTolerance;
			double f64Ratio;

			double f64Height = flrRegion.GetHeight();
			double f64Width = flrRegion.GetWidth();

			// 설정된 ROI에 대해 내부 및 외부 측정영역을 디스플레이 합니다. // Display the inner and outer measurement areas for the set ROI.
			if(f64Height >= f64Width)
			{
				f64Ratio = f64Width / f64Height;

				double f64MinTolerance = f64Height / 2.;

				if(f64Tolerance >= f64MinTolerance)
					f64Tolerance = f64MinTolerance;

				f64HeightTolerance = f64Tolerance;
				f64WidthTolerance = f64HeightTolerance * f64Ratio;
			}

			if(f64Height < f64Width)
			{
				f64Ratio = f64Height / f64Width;

				double f64MinTolerance = f64Width / 2.;

				if(f64Tolerance >= f64MinTolerance)
					f64Tolerance = f64MinTolerance;

				f64WidthTolerance = f64Tolerance;
				f64HeightTolerance = f64WidthTolerance * f64Ratio;
			}

			for(int32_t j = 0; j < 2; ++j)
			{
				arrF64Tolerance[2 * j] = f64WidthTolerance;
				arrF64Tolerance[2 * j + 1] = f64HeightTolerance;
			}

			CFLPoint<double> flpCent = flrRegion.GetCenter();
			CFLRect<double>  flrInner = flrRegion, flrOuter = flrRegion;

			if(flrInner.GetWidth() / 2. <= arrF64Tolerance[0] || flrInner.GetHeight() / 2. <= arrF64Tolerance[1])
			{
				if(IsFail(res = layer.DrawFigureImage(&CFLPoint<double>(flrInner.GetCenter()), RED)))
				{
					ErrorPrint(res, "Failed to draw figure\n");
					break;
				}
			}
			else
			{
				flrInner.Offset(-flpCent.x, -flpCent.y);
				flrInner.Multiply(((double)flrRegion.GetWidth() - arrF64Tolerance[0] * 2.) / (double)flrRegion.GetWidth(), ((double)flrRegion.GetHeight() - arrF64Tolerance[1] * 2.) / (double)flrRegion.GetHeight());
				flrInner.Offset(flpCent);

				if(IsFail(res = layer.DrawFigureImage(&flrInner, RED)))
				{
					ErrorPrint(res, "Failed to draw figure\n");
					break;
				}
			}

			flrOuter.Offset(-flpCent.x, -flpCent.y);
			flrOuter.Multiply(((double)flrRegion.GetWidth() + arrF64Tolerance[0] * 2.) / (double)flrRegion.GetWidth(), ((double)flrRegion.GetHeight() + arrF64Tolerance[1] * 2.) / (double)flrRegion.GetHeight());
			flrOuter.Offset(flpCent);

			if(IsFail(res = layer.DrawFigureImage(&flrOuter, RED)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			if(IsOK(res))
			{
				// 추정된 사각형을 디스플레이 합니다. // Display the estimated rectangle.
				if(IsFail(res = layer.DrawFigureImage(&flrResult, BLACK, 5)))
				{
					ErrorPrint(res, "Failed to draw figure\n");
					break;
				}

				if(IsFail(res = layer.DrawFigureImage(&flrResult, CYAN, 3)))
				{
					ErrorPrint(res, "Failed to draw figure\n");
					break;
				}

				// 사각형의 정보를 Console창에 출력합니다. // Output the square information to the console window.
				double f64ResultWidth, f64ResultHeight, f64ResultAngle;
				f64ResultWidth = flrResult.GetWidth();
				f64ResultHeight = flrResult.GetHeight();
				f64ResultAngle = flrResult.GetAngle();
				CFLPoint<double> flpLineCenter = flrResult.GetCenter();
				printf("Rectangle Center : (%.2lf, %.2lf)\nWidth : %.2lf pixels\nHeight : %.2lf pixels\nAngle : %.2lf˚\n", flpLineCenter.x, flpLineCenter.y, f64ResultWidth, f64ResultHeight, f64ResultAngle);
			}

			// 추출된 유효점이 어디인지 알기 위해 디스플레이 한다 // Display to know where the extracted valid point is
			for(int32_t i32Index = 0; i32Index < (int32_t)flfaResultsValid.GetCount(); ++i32Index)
			{
				if(flfaResultsValid.GetAt(i32Index)->GetDeclType() != EFigureDeclType_Point)
					break;

				CFLPoint<double>* pFlp = static_cast<CFLPoint<double>*>(flfaResultsValid.GetAt(i32Index));

				CFLFigureArray flfaPoint = CFLPoint<double>(pFlp->x, pFlp->y).MakeCrossHair(1, true);

				if(IsFail(res = layer.DrawFigureImage(&flfaPoint, LIME)))
				{
					ErrorPrint(res, "Failed to draw figure\n");
					break;
				}
			}

			// 추출된 유효하지 않은 점이 어디인지 알기 위해 디스플레이 한다 // Display to see where the extracted invalid points are
			for(int32_t i32Index = 0; i32Index < (int32_t)flfaResultsInvalid.GetCount(); ++i32Index)
			{
				if(flfaResultsInvalid.GetAt(i32Index)->GetDeclType() != EFigureDeclType_Point)
					break;

				CFLPoint<double>* pFlp = static_cast<CFLPoint<double>*>(flfaResultsInvalid.GetAt(i32Index));

				CFLFigureArray flfaPoint = CFLPoint<double>(pFlp->x, pFlp->y).MakeCrossHair(1, true);

				if(IsFail(res = layer.DrawFigureImage(&flfaPoint, RED)))
				{
					ErrorPrint(res, "Failed to draw figure\n");
					break;
				}
			}

			// 측정 영역이 어디인지 알기 위해 디스플레이 한다 // Display to know where the measurement area is
			if(IsFail(res = layer.DrawFigureImage(&measureRegion, BLUE)))
			{
				ErrorPrint(res, "Failed to draw figures objects on the image view.\n");
				break;
			}

			// 이미지 뷰를 갱신 합니다. // Update the image view.
			viewImage[i].Invalidate(true);
		}

		bool bTerminated = false;
		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(!bTerminated)
		{
			for(int32_t i = 0; i < i32ExampleCount; ++i)
				bTerminated |= !viewImage[i].IsAvailable();

			CThreadUtilities::Sleep(1);
		}

		for(int32_t i = 0; i < i32ExampleCount; ++i)
			viewImage[i].Destroy();
	}
	while(false);

	return 0;
}