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

	CCircleGauge::ETransitionType arrTransitionType[] =
	{
		CCircleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CCircleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CCircleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CCircleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CCircleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CCircleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CCircleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CCircleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CCircleGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CCircleGauge::ETransitionType_DarkToBright,
		CCircleGauge::ETransitionType_BrightToDark,
		CCircleGauge::ETransitionType_DarkToBrightToDark,
	};

	CCircleGauge::ETransitionChoice arrTransitionChoice[] =
	{
		CCircleGauge::ETransitionChoice_Begin,
		CCircleGauge::ETransitionChoice_Begin,
		CCircleGauge::ETransitionChoice_Begin,
		CCircleGauge::ETransitionChoice_LargestArea,
		CCircleGauge::ETransitionChoice_End,
		CCircleGauge::ETransitionChoice_End,
		CCircleGauge::ETransitionChoice_End,
		CCircleGauge::ETransitionChoice_LargestAmplitude,
		CCircleGauge::ETransitionChoice_Closest,
		CCircleGauge::ETransitionChoice_Closest,
		CCircleGauge::ETransitionChoice_Closest,
		CCircleGauge::ETransitionChoice_Closest,
	};

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImage[i32ExampleCount];
	CResult res;

	do
	{
		// 이미지 로드 // Loads image
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/Gauge/Circle.flif")))
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

		// Circle Gauge 객체 생성 // Create Circle Gauge object
		CCircleGauge circleGauge;

		// 처리할 이미지 설정 // Set the image to process
		circleGauge.SetSourceImage(fliImage);

		// 측정할 영역을 설정합니다. // Set the area to measure.
		CFLCircle<double> measureRegion(260., 265., 120.);
		double tolerance = 100.;
		circleGauge.SetMeasurementRegion(measureRegion, tolerance);

		// 추출하기위한 파라미터를 설정합니다. // Set parameters for extraction.
		// 원을 추정하기위해 추출할 경계점의 변화 임계값에 대해 설정합니다. // Set the threshold change of the boundary point to be extracted to estimate the circle.
		circleGauge.SetThreshold(20);
		// 원을 추정하기위해 추출할 경계점의 변화 임계값에 보정값을 설정합니다. // Set the correction value to the threshold change of the boundary point to be extracted to estimate the circle.
		circleGauge.SetMinimumAmplitude(10);
		// 원을 추정하기위해 추출할 경계점들의 대표값 표본 개수를 설정합니다. // Set the number of representative sample values ??of the boundary points to be extracted to estimate the circle.
		circleGauge.SetThickness(3);
		// 원을 추정하기위해 추출할 경계점들의 추출 간격을 설정합니다. // Set the extraction interval of boundary points to be extracted to estimate the circle.
		circleGauge.SetSamplingStep(1.);
		// 원을 추정하기위해 추출할 경계점들의 이상치 조정을 위한 임계값을 설정합니다. // Set the threshold value for outlier adjustment of the boundary points to be extracted to estimate the circle.
		circleGauge.SetOutliersThreshold(3.);
		// 원을 추정하기위해 추출할 경계점들의 이상치 조정 횟수을 설정합니다. // Set the number of outlier adjustments for boundary points to be extracted to estimate the circle.
		circleGauge.SetOutliersThresholdCount(3);

		for(int32_t i = 0; i < i32ExampleCount; ++i)
		{
			// 원을 추정하기위해 추출할 경계점 변화 방향에 대해 설정합니다. // Set the boundary point change direction to extract to estimate the circle.
			circleGauge.SetTransitionType(arrTransitionType[i]);
			// 원을 추정하기위해 추출한 경계점 중 사용할 경계점 유형을 선택합니다. // Select the boundary point type to use among the boundary points extracted to estimate the circle.
			circleGauge.SetTransitionChoice(arrTransitionChoice[i]);
			// 알고리즘 수행 // Execute the algorithm
			if(IsFail(res = circleGauge.Execute()))
			{
				ErrorPrint(res, "Failed to execute Circle gauge.\n");
				break;
			}

			// 실행 결과를 가져옵니다. // Get the execution result.
			CFLCircle<double> flcResult;
			CFLFigureArray flfaResultsValid, flfaResultsInvalid;

			// index의 경우 TransitionChoice의 Begin, End에서만 유효합니다. //Index works only at Begin and End of TransitionChoice
			// 추정된 원을 가져옵니다. // Get the estimated circle.	
			res = circleGauge.GetMeasuredObject(flcResult, i % 4);
			// 추정된 원을 추출에 사용된 유효 경계점을 가져옵니다. // Get the effective boundary point used to extract the estimated circle.
			circleGauge.GetMeasuredValidPoints(flfaResultsValid, i % 4);
			// 추정된 원을 추출에 사용되지 못한 유효하지 않은 경계점을 가져옵니다. // Get an invalid boundary point that is not used to extract the estimated circle.
			circleGauge.GetMeasuredInvalidPoints(flfaResultsInvalid, i % 4);

			CGUIViewImageLayerWrap layer = viewImage[i].GetLayer(0);

			layer.Clear();

			if(IsFail(res = layer.DrawTextImage(CFLPoint<double>(0., 0.), arrWcsViewText[i], YELLOW, BLUE, 20, true)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			// 측정 영역이 어디인지 알기 위해 디스플레이 한다 // Display to know where the measurement area is
			CFLCircle<double> flcRegion = circleGauge.GetMeasurementRegion();

			CFLCircle<double> flcInner, flcOuter;
			double f64Tolerance;
			f64Tolerance = circleGauge.GetTolerance();

			// 설정된 ROI에 대해 내부 및 외부 측정영역을 디스플레이 합니다. // Display the inner and outer measurement areas for the set ROI.
			if(flcRegion.radius < f64Tolerance)
				flcInner.Set((float)flcRegion.GetCenter().x, (float)flcRegion.GetCenter().y, 0.1f);
			else
				flcInner.Set((float)flcRegion.GetCenter().x, (float)flcRegion.GetCenter().y, flcRegion.radius - f64Tolerance);

			flcOuter.Set((float)flcRegion.GetCenter().x, (float)flcRegion.GetCenter().y, flcRegion.radius + f64Tolerance);

			if(IsFail(res = layer.DrawFigureImage(&flcInner, RED)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			if(IsFail(res = layer.DrawFigureImage(&flcOuter, RED)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			if(IsOK(res))
			{
				// 추정된 원을 디스플레이 합니다. // Display the estimated circle.
				if(IsFail(res = layer.DrawFigureImage(&flcResult, BLACK, 5)))
				{
					ErrorPrint(res, "Failed to draw figure\n");
					break;
				}

				if(IsFail(res = layer.DrawFigureImage(&flcResult, CYAN, 3)))
				{
					ErrorPrint(res, "Failed to draw figure\n");
					break;
				}

				// 원의 정보를 Console창에 출력합니다. // Output the original information to the console window.
				double f64Radius;
				flcResult.GetRadius(&f64Radius);
				CFLPoint<double> flpLineCenter = flcResult.GetCenter();
				printf("Circle Center : (%.2lf, %.2lf)\nRadius : %.2lf pixels", flpLineCenter.x, flpLineCenter.y, f64Radius);
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