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
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/Gauge/Circle Array.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Creates imageview		
		if(IsFail(res = viewImage.Create(100, 100, 600, 600)))
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

		// Circle Gauge 객체 생성 // Create Circle Gauge object
		CCircleArrayGauge circleArrayGauge;

		// 처리할 이미지 설정 // Set the image to process
		circleArrayGauge.SetSourceImage(fliImage);

		// 측정할 영역을 설정합니다. // Set the area to measure.
		CFLFigureArray flfaMeasurementRegion;

		flfaMeasurementRegion.Load(L"../../ExampleImages/Gauge/Circle Array Measurement Region");

		double tolerance = 15.;
		circleArrayGauge.SetMeasurementRegion(flfaMeasurementRegion, tolerance);

		// 추출하기위한 파라미터를 설정합니다. // Set parameters for extraction.
		// 원을 추정하기위해 추출할 경계점의 변화 임계값에 대해 설정합니다. // Set the threshold change of the boundary point to be extracted to estimate the circle.
		circleArrayGauge.SetThreshold(20);
		// 원을 추정하기위해 추출할 경계점의 변화 임계값에 보정값을 설정합니다. // Set the correction value to the threshold change of the boundary point to be extracted to estimate the circle.
		circleArrayGauge.SetMinimumAmplitude(10);
		// 원을 추정하기위해 추출할 경계점들의 대표값 표본 개수를 설정합니다. // Set the number of representative sample values ??of the boundary points to be extracted to estimate the circle.
		circleArrayGauge.SetThickness(3);
		// 원을 추정하기위해 추출할 경계점들의 추출 간격을 설정합니다. // Set the extraction interval of boundary points to be extracted to estimate the circle.
		circleArrayGauge.SetSamplingStep(1.);
		// 원을 추정하기위해 추출할 경계점들의 이상치 조정을 위한 임계값을 설정합니다. // Set the threshold value for outlier adjustment of the boundary points to be extracted to estimate the circle.
		circleArrayGauge.SetOutliersThreshold(3.);
		// 원을 추정하기위해 추출할 경계점들의 이상치 조정 횟수을 설정합니다. // Set the number of outlier adjustments for boundary points to be extracted to estimate the circle.
		circleArrayGauge.SetOutliersThresholdCount(3);

		// 원을 추정하기위해 추출할 경계점 변화 방향에 대해 설정합니다. // Set the boundary point change direction to extract to estimate the circle.
		circleArrayGauge.SetTransitionType(CCircleGauge::ETransitionType_BrightToDark);
		// 원을 추정하기위해 추출한 경계점 중 사용할 경계점 유형을 선택합니다. // Select the boundary point type to use among the boundary points extracted to estimate the circle.
		circleArrayGauge.SetTransitionChoice(CCircleGauge::ETransitionChoice_LargestAmplitude);
		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = circleArrayGauge.Execute()))
		{
			ErrorPrint(res, "Failed to execute Circle gauge.\n");
			break;
		}

		// 실행 결과를 가져옵니다. // Get the execution result.
		CFLFigureArray flfaResult;
		CFLFigureArray flfaResultsValid, flfaResultsInvalid;

		// index의 경우 TransitionChoice의 Begin, End에서만 유효합니다. //Index works only at Begin and End of TransitionChoice
		// 추정된 원을 가져옵니다. // Get the estimated circle.	
		res = circleArrayGauge.GetMeasuredObject(flfaResult);
		// 추정된 원을 추출에 사용된 유효 경계점을 가져옵니다. // Get the effective boundary point used to extract the estimated circle.
		circleArrayGauge.GetMeasuredValidPoints(flfaResultsValid);
		// 추정된 원을 추출에 사용되지 못한 유효하지 않은 경계점을 가져옵니다. // Get an invalid boundary point that is not used to extract the estimated circle.
		circleArrayGauge.GetMeasuredInvalidPoints(flfaResultsInvalid);

		CGUIViewImageLayerWrap layer = viewImage.GetLayer(0);

		layer.Clear();

		// 측정 영역이 어디인지 알기 위해 디스플레이 한다 // Display to know where the measurement area is
		CFLFigureArray flfaMeasurementToleranceRegion = circleArrayGauge.GetActualMeasurementRegion();

		if(IsFail(res = layer.DrawFigureImage(&flfaMeasurementToleranceRegion, BLUE)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		if(IsFail(res = layer.DrawFigureImage(&flfaMeasurementRegion, BLUE)))
		{
			ErrorPrint(res, "Failed to draw figures objects on the image view.\n");
			break;
		}

		// 추정된 원을 디스플레이 합니다. // Display the estimated circle.
		if(IsFail(res = layer.DrawFigureImage(flfaResult, BLACK, 5)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		if(IsFail(res = layer.DrawFigureImage(flfaResult, CYAN, 3)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		// 원의 정보를 Console창에 출력합니다. // Output the original information to the console window.
		for(int64_t i = 0; i < flfaResult.GetCount(); ++i)
		{
			CFLCircle<double>* pFlcResult = (CFLCircle<double>*)flfaResult[i];

			double f64Radius = pFlcResult->GetRadius();
			CFLPoint<double> flpLineCenter = pFlcResult->GetCenter();
			printf("[%lld]Circle Center : (%.2lf, %.2lf)\nRadius : %.2lf pixels", i, flpLineCenter.x, flpLineCenter.y, f64Radius);
		}

	// 추출된 유효점이 어디인지 알기 위해 디스플레이 한다 // Display to know where the extracted valid point is
		CFLFigureArray flfaValidCrossHair;
		flfaResultsValid.MakeCrossHairElementwise(flfaValidCrossHair, 1., true);

		if(IsFail(res = layer.DrawFigureImage(&flfaValidCrossHair, LIME)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		// 추출된 유효하지 않은 점이 어디인지 알기 위해 디스플레이 한다 // Display to see where the extracted invalid points are
		CFLFigureArray flfaInvalidCrossHair;
		flfaResultsInvalid.MakeCrossHairElementwise(flfaInvalidCrossHair, 1., true);

		if(IsFail(res = layer.DrawFigureImage(&flfaInvalidCrossHair, RED)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
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