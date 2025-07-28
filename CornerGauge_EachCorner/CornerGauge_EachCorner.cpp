#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImage;
	CResult res;

	do
	{
		// 이미지 로드 // Loads image
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/Gauge/Rect.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Loads image view
		if(IsFail(res = viewImage.Create(400, 0, 1424, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
		if(IsFail(res = viewImage.SetImagePtr(&fliImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		CResult res = EResult_UnknownError;

		// Corner Gauge 객체 생성 // Create Corner Gauge object
		CCornerGauge cornerGauge;

		// 처리할 이미지 설정 // Set the image to process
		cornerGauge.SetSourceImage(fliImage);

		// 측정할 영역을 설정합니다. // Set the area to measure.
		CFLRect<double> measureRegion(213.577428, 262.324155, 295.020437, 348.179290);
		double tolerance = 50.;
		cornerGauge.SetMeasurementRegion(measureRegion, tolerance);

		// 추출하기위한 파라미터를 설정합니다. // Set parameters for extraction.
		// 코너를 추정하기위해 추출할 경계점 변화 방향에 대해 설정합니다. // Set the boundary point change direction to extract to estimate the corner.
		cornerGauge.SetTransitionType(CCornerGauge::ETransitionType_DarkToBrightOrBrightToDark);
		// 코너를 추정하기위해 추출한 경계점 중 사용할 경계점 유형을 선택합니다. // Select the boundary point type to use among the boundary points extracted to estimate the corner.
		cornerGauge.SetTransitionChoice(CCornerGauge::ETransitionChoice_Closest);
		// 코너를 추정하기위해 추출할 경계점의 변화 임계값에 대해 설정합니다. // Set the threshold change of the boundary point to be extracted to estimate the corner.
		cornerGauge.SetThreshold(20);
		// 코너를 추정하기위해 추출할 경계점의 변화 임계값에 보정값을 설정합니다. // Set the correction value to the threshold change of the boundary point to be extracted to estimate the corner.
		cornerGauge.SetMinimumAmplitude(10);
		// 코너를 추정하기위해 추출할 경계점들의 대표값 표본 개수를 설정합니다. // Set the number of representative sample values ??of the boundary points to be extracted to estimate the corner.
		cornerGauge.SetThickness(1);
		// 코너를 추정하기위해 추출할 경계점들의 추출 간격을 설정합니다. // Set the extraction interval of boundary points to be extracted to estimate the corner.
		cornerGauge.SetSamplingStep(1.);
		// 코너를 추정하기위해 추출할 경계점들의 이상치 조정을 위한 임계값을 설정합니다. // Set the threshold value for outlier adjustment of the boundary points to be extracted to estimate the corner.
		cornerGauge.SetOutliersThreshold(1.);
		// 코너를 추정하기위해 추출할 경계점들의 이상치 조정 횟수을 설정합니다. // Set the number of outlier adjustments for boundary points to be extracted to estimate the corner.
		cornerGauge.SetOutliersThresholdCount(3);
		// 코너를 추정하기위해 점 클러스터링 처리 유무에 대한 설정을 합니다. // Set whether or not to process point clustering to estimate the corner.
		cornerGauge.EnableClusterMode(true);
		// 코너를 추정하기위해 마진을 설정합니다. 필요에 따라 각 구역별로 설정가능합니다. // Set the margin to estimate the corner. It can be set for each zone as needed.
		cornerGauge.SetMeasurementMarginRatio(0., CCornerGauge::EMargin_All);
		// 코너를 추정하기위한 Tolerance를 설정합니다. 필요에 따라 각 구역별로 설정가능합니다. // Set the Tolerance for estimating the corner. It can be set for each zone as needed.
		cornerGauge.SetTolerance(tolerance, CCornerGauge::ETolerance_All);
		// 코너를 측정하기위한 영역을 설정합니다. // Set the area for measuring corners.
		cornerGauge.SetCorner(CCornerGauge::ECorner_All);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = cornerGauge.Execute()))
		{
			ErrorPrint(res, "Failed to execute Corner gauge.\n");
			break;
		}


		CGUIViewImageLayerWrap layer = viewImage.GetLayer(0);

		layer.Clear();

		if(IsOK(res))
		{
			// 실행 결과를 가져옵니다. // Get the execution result.
			CFLFigureArray flfaResultLine;
			// 추정된 선을 가져옵니다. // Get the estimated line.
			cornerGauge.GetMeasuredLines(flfaResultLine);

			layer.DrawFigureImage(flfaResultLine, BLACK, 5);
			layer.DrawFigureImage(flfaResultLine, CYAN, 3);

			CFLLine<double> arrLines[2];
			arrLines[0] = dynamic_cast<CFLLine<double>*>(flfaResultLine.GetAt(0));
			arrLines[1] = dynamic_cast<CFLLine<double>*>(flfaResultLine.GetAt(1));

			// 추정된 코너를 가져옵니다. // Get the estimated corner.
			for(int32_t i32CornerIndex = 0; i32CornerIndex < 4; ++i32CornerIndex)
			{
				CFLPoint<double> flpResultCorner;
				cornerGauge.GetMeasuredObject(flpResultCorner, i32CornerIndex);

				layer.DrawFigureImage(flpResultCorner, BLACK, 3);
				layer.DrawFigureImage(flpResultCorner, CYAN, 1);

				printf("#%d : Corner X : %.2lf, Corner Y : %.2lf\n", i32CornerIndex, flpResultCorner.x, flpResultCorner.y);
			}

			for(int32_t i = 0; i < 2; ++i)
			{
				double f64ResultAngle;
				f64ResultAngle = arrLines[i].GetAngle();
				printf("Line Angle : %.2lf˚\n", f64ResultAngle);
			}
		}

		CFLFigureArray flfaResultsValid, flfaResultsInvalid;
		// 추정된 코너를 추출에 사용된 유효 경계점을 가져옵니다. // Get the effective boundary point used to extract the estimated corner.
		cornerGauge.GetMeasuredValidPoints(flfaResultsValid);
		// 추정된 코너를 추출에 사용되지 못한 유효하지 않은 경계점을 가져옵니다. // Get an invalid boundary point that is not used to extract the estimated corner.
		cornerGauge.GetMeasuredInvalidPoints(flfaResultsInvalid);


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
		viewImage.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}