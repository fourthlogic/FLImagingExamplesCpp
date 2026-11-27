#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage[3];

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// 이미지 뷰 생성 // Create image views
		for(int32_t i = 0; i < 3; ++i)
		{
			if((res = viewImage[i].Create(200 + i * 500, 0, 700 + i * 500, 500)).IsFail())
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				break;
			}
		}

		if(res.IsFail())
			break;

		// 각 이미지 뷰의 시점을 동기화 한다. // Synchronize the viewpoint of each image view.
		if((res = viewImage[0].SynchronizePointOfView(&viewImage[1])).IsFail() || (res = viewImage[1].SynchronizePointOfView(&viewImage[2])).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 각 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the position of each image view window
		if((res = viewImage[0].SynchronizeWindow(&viewImage[1])).IsFail() || (res = viewImage[1].SynchronizeWindow(&viewImage[2])).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 곡선 y = 0.004 * (x - 250)^2 + 200 위의 점들에 노이즈를 추가하여 데이터 생성
		// Generate data by adding noise to points on the curve y = 0.004 * (x - 250)^2 + 200
		CFLPointArray flpaData;

		for(int32_t i = 0; i <= 40; ++i)
		{
			double f64X = 50. + i * 10.;
			double f64Y = 0.004 * (f64X - 250.) * (f64X - 250.) + 200. + CRandomGenerator::Double(-5., 5.);

			// 일부 점은 곡선에서 크게 벗어나도록 만든다. // Make some points deviate greatly from the curve.
			if(i == 6)
				f64Y -= 90.;
			else if(i == 20)
				f64Y += 110.;
			else if(i == 33)
				f64Y += 100.;

			// 일부 점은 곡선에서 조금 벗어나도록 만든다. (임계값이 작을 때만 제외됨)
			// Make some points deviate slightly from the curve. (Excluded only when the threshold is small)
			if(i == 13)
				f64Y -= 25.;
			else if(i == 27)
				f64Y += 25.;

			flpaData.PushBack(CFLPoint<double>(f64X, f64Y));
		}

		// CLeastSquares<double> 객체 생성 // Create CLeastSquares<double> object
		CLeastSquares<double> leastSquares;

		// 데이터를 할당 // Assign data
		if((res = leastSquares.Assign(flpaData)).IsFail())
		{
			ErrorPrint(res, "Failed to assign data.\n");
			break;
		}

		// 피팅할 다항식의 차수 // Degree of the polynomial to fit
		const int32_t i32Degree = 2;

		// 각 뷰에서 사용할 Outlier 파라미터 // Outlier parameters for each view
		const CFLString<wchar_t> arrFlsViewName[3] = { L"Default", L"Parameter 1", L"Parameter 2" };
		const int64_t arrI64OutlierThresholdCount[3] = { 0, 1, 3 };
		const double arrF64OutlierThreshold[3] = { 3., 3., 1.5 };

		wprintf(L"Source Points\n");
		wprintf(L"%s\n\n", CFigureUtilities::ConvertFigureObjectToString(&flpaData).GetString());

		for(int32_t i = 0; i < 3; ++i)
		{
			// 계수 값을 받기 위해 CFLArray 생성 // Create CFLArray to receive coefficient values
			CFLArray<double> flaCoef;
			// R square 값을 받기 위해 double 선언 // Declare double to receive R square value
			double f64RSquare = 0.;
			// 제외된 Outlier 점들의 인덱스를 받기 위해 CFLArray 생성 // Create CFLArray to receive the indices of excluded outlier points
			CFLArray<int64_t> flaOutlierIndices;

			// 다항식 계수를 얻는다. (내림차순 : a, b, c → y = ax^2 + bx + c)
			// Get polynomial coefficients. (Descending order : a, b, c -> y = ax^2 + bx + c)
			if((res = leastSquares.GetPoly(i32Degree, flaCoef, &f64RSquare, ECoefficientOrder_Descending, ELeastSqauresAccuracy_TemplateType, arrI64OutlierThresholdCount[i], arrF64OutlierThreshold[i], &flaOutlierIndices)).IsFail())
			{
				ErrorPrint(res, "Failed to calculate.\n");
				break;
			}

			// Outlier 인덱스로 Outlier PointArray 생성 // Create Outlier PointArray from the outlier indices
			CFLPointArray flpaOutlier;

			for(int64_t j = 0; j < flaOutlierIndices.GetCount(); ++j)
				flpaOutlier.PushBack(flpaData.GetAt(flaOutlierIndices.GetAt(j)));

			// 얻어온 계수로 곡선을 계산하여 짧은 선분들로 표현 // Calculate the curve with the obtained coefficients and represent it with short line segments
			CFLFigureArray flfaCurve;
			double f64PrevX = 0.;
			double f64PrevY = 0.;

			for(int32_t x = 30; x <= 470; x += 4)
			{
				double f64X = (double)x;
				double f64Y = 0.;

				for(int64_t j = 0; j < flaCoef.GetCount(); ++j)
					f64Y = f64Y * f64X + flaCoef.GetAt(j);

				if(x != 30)
					flfaCurve.PushBack(CFLLine<double>(f64PrevX, f64PrevY, f64X, f64Y));

				f64PrevX = f64X;
				f64PrevY = f64Y;
			}

			// 화면에 출력하기 위해 Image View 에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
			// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
			CGUIViewImageLayerWrap layer = viewImage[i].GetLayer(0);

			CFLString<wchar_t> flsParameter;

			if(arrI64OutlierThresholdCount[i])
				flsParameter.Format(L"Outlier Threshold Count : %lld\r\nOutlier Threshold : %.3lf", arrI64OutlierThresholdCount[i], arrF64OutlierThreshold[i]);
			else
				flsParameter.Format(L"Outlier Threshold Count : %lld", arrI64OutlierThresholdCount[i]);

			CFLString<wchar_t> flsResult;
			flsResult.Format(L"y = %.6lfx^2 %+.6lfx %+.6lf\r\nR square : %.6lf\r\nOutlier Count : %lld", flaCoef.GetAt(0), flaCoef.GetAt(1), flaCoef.GetAt(2), f64RSquare, flaOutlierIndices.GetCount());

			// 레이어에 Figure 와 Text 를 출력 // Draw Figure and Text to the layer
			// 피팅 결과 곡선 : CYAN, 데이터 : LIME, 제외된 Outlier 점 : RED
			// Fitted curve : CYAN, Data : LIME, Excluded outlier points : RED
			layer.DrawFigureImage(flfaCurve, BLACK, 5);
			layer.DrawFigureImage(flfaCurve, CYAN, 3);
			layer.DrawFigureImage(flpaData, BLACK, 3);
			layer.DrawFigureImage(flpaData, LIME, 1);
			layer.DrawFigureImage(flpaOutlier, RED, 1);
			layer.DrawTextCanvas(&CFLPoint<int32_t>(0, 0), arrFlsViewName[i], YELLOW, BLACK, 30);
			layer.DrawTextCanvas(&CFLPoint<int32_t>(0, 40), flsParameter, YELLOW, BLACK, 15);
			layer.DrawTextCanvas(&CFLPoint<int32_t>(0, 80), flsResult, YELLOW, BLACK, 15);

			// Console 출력 // Console output
			wprintf(L"[%s]\n", arrFlsViewName[i].GetString());
			wprintf(L"%s\n", flsParameter.GetString());
			wprintf(L"%s\n\n", flsResult.GetString());
		}

		if(res.IsFail())
			break;

		// 이미지 뷰들을 갱신 합니다. // Update the image views.
		for(int32_t i = 0; i < 3; ++i)
			viewImage[i].Invalidate(true);

		// 이미지 뷰가 셋중에 하나라도 꺼지면 종료로 간주 // Consider closed when any of the three image views are turned off
		while(viewImage[0].IsAvailable() && viewImage[1].IsAvailable() && viewImage[2].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}
