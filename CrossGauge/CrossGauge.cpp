#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	const int32_t i32ExampleCount = 3;
	const wchar_t* arrWcsViewText[] =
	{
		L"Dark To Bright To Dark\nClosest",
		L"Bright To Dark To Bright\nClosest",
		L"Dark To Bright To Dark Or Bright To Dark To Bright\nClosest",
	};

	CCrossGauge::ETransitionType arrTransitionType[] =
	{
		CCrossGauge::ETransitionType_DarkToBrightToDark,
		CCrossGauge::ETransitionType_BrightToDarkToBright,
		CCrossGauge::ETransitionType_DarkToBrightToDarkOrBrightToDarkToBright,
	};

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliImage[i32ExampleCount];

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImage[i32ExampleCount];
	CResult res;

	do
	{
		// 이미지 로드 // Loads image
		if(IsFail(res = fliImage[0].Load(L"../../ExampleImages/Gauge/Cross_Bright.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliImage[1].Load(L"../../ExampleImages/Gauge/Cross_Dark.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliImage[2].Load(L"../../ExampleImages/Gauge/Cross_Dark_Bright.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Creates imageview
		for(int32_t i = 0; i < i32ExampleCount; ++i)
		{
			int32_t i32X = 400 * (i % 4);

			if(IsFail(res = viewImage[i].Create(i32X, 0, i32X + 400, 400)))
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				break;
			}

			// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
			if(IsFail(res = viewImage[i].SetImagePtr(&fliImage[i])))
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

		// Cross Gauge 객체 생성 // Create Cross Gauge object
		CCrossGauge crossGauge;

		// 측정할 영역을 설정합니다. // Set the area to measure.
		CFLRect<double> measureRegion(126., 126., 400., 400.);
		double tolerance = 70.;
		crossGauge.SetMeasurementRegion(measureRegion, tolerance);

		// 추출하기위한 파라미터를 설정합니다. // Set parameters for extraction.		
		// 십자형을 추정하기위해 추출한 경계점 중 사용할 경계점 유형을 선택합니다. // Select the boundary point type to use among the boundary points extracted to estimate the crosshair.
		crossGauge.SetTransitionChoice(CCrossGauge::ETransitionChoice_Closest);
		// 십자형을 추정하기위해 추출할 경계점의 변화 임계값에 대해 설정합니다. // Set the threshold change of the boundary point to be extracted to estimate the crosshair.
		crossGauge.SetThreshold(20);
		// 십자형을 추정하기위해 추출할 경계점의 변화 임계값에 보정값을 설정합니다. // Set the correction value to the threshold change of the boundary point to be extracted to estimate the crosshair.
		crossGauge.SetMinimumAmplitude(10);
		// 십자형을 추정하기위해 추출할 경계점들의 대표값 표본 개수를 설정합니다. // Set the number of representative sample values ??of the boundary points to be extracted to estimate the crosshairs.
		crossGauge.SetThickness(1);
		// 십자형을 추정하기위해 추출할 경계점들의 추출 간격을 설정합니다. // Set the extraction interval of boundary points to be extracted to estimate the crosshair.
		crossGauge.SetSamplingStep(1.);
		// 십자형을 추정하기위해 추출할 경계점들의 이상치 조정을 위한 임계값을 설정합니다. // Set the threshold value for outlier adjustment of the boundary points to be extracted to estimate the crosshairs.
		crossGauge.SetOutliersThreshold(3.);
		// 십자형을 추정하기위해 추출할 경계점들의 이상치 조정 횟수을 설정합니다. // Set the number of outlier adjustments for boundary points to be extracted to estimate the crosshairs.
		crossGauge.SetOutliersThresholdCount(3);
		// 십자형을 추정하기위해 점 클러스터링 처리 유무에 대한 설정을 합니다. // Set whether or not to process point clustering to estimate the crosshairs.
		crossGauge.EnableClusterMode(true);
		// 십자형을 추정하기위해 마진을 설정합니다. 필요에 따라 각 구역별로 설정가능합니다. // Set the margin to estimate the crosshairs. It can be set for each zone as needed.
		crossGauge.SetMeasurementMarginRatio(0.5, 0.1);

		for(int32_t i = 0; i < i32ExampleCount; ++i)
		{
			// 처리할 이미지 설정 // Set the image to process
			crossGauge.SetSourceImage(fliImage[i]);

			// 십자형을 추정하기위해 추출할 경계점 변화 방향에 대해 설정합니다. // Set the boundary point change direction to extract to estimate the crosshair.
			crossGauge.SetTransitionType(arrTransitionType[i]);

			// 알고리즘 수행 // Execute the algorithm
			if(IsFail(res = crossGauge.Execute()))
			{
				ErrorPrint(res, "Failed to execute Cross gauge.\n");
				break;
			}

			// 실행 결과를 가져옵니다. // Get the execution result.
			CFLPoint<double> resultRegion;
			CFLFigureArray flfaResultLine;
			CFLFigureArray flfaResultsValid, flfaResultsInvalid;
			// 추정을 위한 라인을 가져옵니다. // Get the line for inference.
			crossGauge.GetMeasuredLines(flfaResultLine);
			// 추정된 십자형을 가져옵니다. // Get the estimated crosshairs.
			crossGauge.GetMeasuredObject(resultRegion);
			// 추정된 십자형을 추출에 사용된 유효 경계점을 가져옵니다. // Get the valid boundary points used to extract the estimated crosshairs.
			crossGauge.GetMeasuredValidPoints(flfaResultsValid);
			// 추정된 십자형을 추출에 사용되지 못한 유효하지 않은 경계점을 가져옵니다. // Get invalid boundary points that were not used to extract the estimated crosshairs.
			crossGauge.GetMeasuredInvalidPoints(flfaResultsInvalid);

			CGUIViewImageLayerWrap layer = viewImage[i].GetLayer(0);

			layer.Clear();

			if(IsFail(res = layer.DrawTextImage(CFLPoint<double>(0., 0.), arrWcsViewText[i], YELLOW, BLUE, 17, true)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			CFLRect<double> flrRegion = crossGauge.GetMeasurementRegion();

			CFLFigureArray flfaResult = resultRegion.MakeCrossHair(25, true);
			layer.DrawFigureImage(flfaResult, BLACK, 3);
			layer.DrawFigureImage(flfaResult, CYAN, 1);

			layer.DrawFigureImage(flfaResultLine, BLACK, 5);
			layer.DrawFigureImage(flfaResultLine, CYAN, 3);

			// 실제 검사 영역을 가져옵니다. // Gets the actual inspection area.
			CFLFigureArray flfaMeasurementRegion = crossGauge.GetActualMeasurementRegion();

			layer.DrawFigureImage(flfaMeasurementRegion, BLUE, 1, LIGHTBLUE, GUI::EGUIViewImagePenStyle_Solid, 0.5f, 0.25f);

			if(IsOK(res))
			{
				double f64ResultAngle;
				crossGauge.GetMeasuredAngle(f64ResultAngle);

				printf("Cross Center : (%.2lf, %.2lf)\nAngle : %.2lf˚\n", resultRegion.x, resultRegion.y, f64ResultAngle);
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
	}
	while(false);

	return 0;
}