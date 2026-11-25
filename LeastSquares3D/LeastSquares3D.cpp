#include <cstdio>
#include <cmath>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap view3D[2];

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// 3D 뷰 생성 // Create 3D views
		if((res = view3D[0].Create(100, 0, 700, 600)).IsFail() || (res = view3D[1].Create(700, 0, 1300, 600)).IsFail())
		{
			ErrorPrint(res, "Failed to create the 3D view.\n");
			break;
		}

		// 곡선 (100s, 40sin(πs), 20s^2), s = 0 ~ 1 위의 점들에 노이즈를 추가하여 데이터 생성
		// Generate data by adding noise to points on the curve (100s, 40sin(πs), 20s^2), s = 0 ~ 1
		const double f64PI = 3.14159265358979323846;

		CFLPointArray3<double> flpaData;

		for(int32_t i = 0; i <= 40; ++i)
		{
			double f64S = i / 40.;
			double f64X = 100. * f64S + CRandomGenerator::Double(-1., 1.);
			double f64Y = 40. * sin(f64PI * f64S) + CRandomGenerator::Double(-1., 1.);
			double f64Z = 20. * f64S * f64S + CRandomGenerator::Double(-1., 1.);

			// 일부 점은 곡선에서 크게 벗어나도록 만든다. // Make some points deviate greatly from the curve.
			if(i == 8)
				f64Z += 25.;
			else if(i == 22)
				f64Y -= 30.;
			else if(i == 35)
				f64Z -= 25.;

			flpaData.PushBack(CFLPoint3<double>(f64X, f64Y, f64Z));
		}

		// CLeastSquares3D<double> 객체 생성 // Create CLeastSquares3D<double> object
		CLeastSquares3D<double> leastSquares3D;

		// 데이터를 할당 // Assign data
		if((res = leastSquares3D.Assign(flpaData)).IsFail())
		{
			ErrorPrint(res, "Failed to assign data.\n");
			break;
		}

		// 피팅할 다항식의 차수 // Degree of the polynomial to fit
		const int32_t i32Degree = 3;

		// 결과 점을 얻을 매개변수 t 목록 (-0.3 ~ 1.3, 0.01 간격)
		// 데이터의 t 는 누적 거리 비율(0 ~ 1)로 지정되므로, 범위 밖의 t 는 데이터 끝부분 너머의 점을 추정합니다.
		// List of parameter t to get result points (-0.3 to 1.3, 0.01 interval)
		// Since t of the data is the cumulative distance ratio (0 to 1), t outside the range gives points beyond the ends of the data.
		CFLArray<double> flaTargetT;

		for(int32_t i = -30; i <= 130; ++i)
			flaTargetT.PushBack(i / 100.);

		// 각 뷰에서 사용할 Outlier 파라미터 // Outlier parameters for each view
		const CFLString<wchar_t> arrFlsViewName[2] = { L"Default", L"Outlier Exclusion" };
		const int64_t arrI64OutlierThresholdCount[2] = { 0, 3 };
		const double arrF64OutlierThreshold[2] = { 3., 3. };

		// 점 크기 및 색상 (R, G, B) // Point size and colors (R, G, B)
		const float f32PointSize = 5.f;
		const TPoint3<uint8_t> tpColorData(0, 255, 255);    // CYAN
		const TPoint3<uint8_t> tpColorOutlier(255, 0, 0);   // RED
		const TPoint3<uint8_t> tpColorInside(0, 255, 0);    // LIME
		const TPoint3<uint8_t> tpColorOutside(0, 0, 255);   // BLUE

		for(int32_t i = 0; i < 2; ++i)
		{
			// 각 축의 다항식 계수를 얻는다. (x(t), y(t), z(t), 내림차순)
			// Get polynomial coefficients of each axis. (x(t), y(t), z(t), descending order)
			CFLArray<double> flaCoefX, flaCoefY, flaCoefZ;
			CFLArray<int64_t> flaOutlierIndices;

			if((res = leastSquares3D.GetParametricPoly(i32Degree, flaCoefX, flaCoefY, flaCoefZ, ECoefficientOrder_Descending, arrI64OutlierThresholdCount[i], arrF64OutlierThreshold[i], &flaOutlierIndices)).IsFail())
			{
				ErrorPrint(res, "Failed to calculate.\n");
				break;
			}

			// 매개변수 t 에 대한 결과 점들을 얻는다. // Get result points for parameter t
			CFLPointArray3<double> flpaResult;

			if((res = leastSquares3D.GetParametricPoints(i32Degree, flaTargetT, flpaResult, arrI64OutlierThresholdCount[i], arrF64OutlierThreshold[i])).IsFail())
			{
				ErrorPrint(res, "Failed to calculate.\n");
				break;
			}

			// 결과 점 : 데이터 범위(t = 0 ~ 1) 안은 LIME, 밖은 BLUE
			// Result points : LIME inside the data range (t = 0 to 1), BLUE outside
			CFLArray<TPoint3<float>> flaResultVertices;
			CFLArray<TPoint3<uint8_t>> flaResultColors;

			for(int64_t j = 0; j < flpaResult.GetCount(); ++j)
			{
				const CFLPoint3<double>* pFlpResult = flpaResult.GetAt(j);
				const double f64T = flaTargetT.GetAt(j);
				const bool bInside = f64T >= 0. && f64T <= 1.;

				flaResultVertices.PushBack(TPoint3<float>((float)pFlpResult->x, (float)pFlpResult->y, (float)pFlpResult->z));
				flaResultColors.PushBack(bInside ? tpColorInside : tpColorOutside);
			}

			// 데이터 점 : CYAN, 제외된 Outlier 점 : RED // Data points : CYAN, Excluded outlier points : RED
			CFLArray<TPoint3<float>> flaDataVertices;
			CFLArray<TPoint3<uint8_t>> flaDataColors;

			for(int64_t j = 0; j < flpaData.GetCount(); ++j)
			{
				const CFLPoint3<double>* pFlpData = flpaData.GetAt(j);

				flaDataVertices.PushBack(TPoint3<float>((float)pFlpData->x, (float)pFlpData->y, (float)pFlpData->z));
				flaDataColors.PushBack(tpColorData);
			}

			for(int64_t j = 0; j < flaOutlierIndices.GetCount(); ++j)
				flaDataColors.SetAt(flaOutlierIndices.GetAt(j), tpColorOutlier);

			// 점 크기를 지정하여 3D 뷰에 점들을 추가 // Add points to the 3D view with the specified point size
			CGUIView3DObject view3DObjectResult(CFL3DObject(flaResultVertices, flaResultColors));
			view3DObjectResult.SetPointSize(f32PointSize);

			CGUIView3DObject view3DObjectData(CFL3DObject(flaDataVertices, flaDataColors));
			view3DObjectData.SetPointSize(f32PointSize);

			if((res = view3D[i].PushObject(view3DObjectResult)).IsFail() || (res = view3D[i].PushObject(view3DObjectData)).IsFail())
			{
				ErrorPrint(res, "Failed to push the object.\n");
				break;
			}

			// 화면에 출력하기 위해 View 에서 레이어 0번을 얻어옴 // Obtain layer number 0 from view for display
			// 이 객체는 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to a view and does not need to be released separately
			CGUIView3DLayerWrap layer = view3D[i].GetLayer(0);

			CFLString<wchar_t> flsParameter;

			if(arrI64OutlierThresholdCount[i])
				flsParameter.Format(L"Outlier Threshold Count : %lld, Outlier Threshold : %.3lf", arrI64OutlierThresholdCount[i], arrF64OutlierThreshold[i]);
			else
				flsParameter.Format(L"Outlier Threshold Count : %lld", arrI64OutlierThresholdCount[i]);

			CFLString<wchar_t> flsOutlierCount;
			flsOutlierCount.Format(L"Outlier Count : %lld", flaOutlierIndices.GetCount());

			layer.DrawTextCanvas(CFLPoint<double>(0, 0), arrFlsViewName[i], YELLOW, BLACK, 20);
			layer.DrawTextCanvas(CFLPoint<double>(0, 30), flsParameter, YELLOW, BLACK, 13);
			layer.DrawTextCanvas(CFLPoint<double>(0, 50), flsOutlierCount, YELLOW, BLACK, 13);
			layer.DrawTextCanvas(CFLPoint<double>(0, 70), L"Data : CYAN, Outlier : RED", YELLOW, BLACK, 13);
			layer.DrawTextCanvas(CFLPoint<double>(0, 90), L"Result (t = 0 ~ 1) : LIME, Result (extrapolated) : BLUE", YELLOW, BLACK, 13);

			// Console 출력 // Console output
			wprintf(L"[%s]\n", arrFlsViewName[i].GetString());
			wprintf(L"%s\n", flsParameter.GetString());
			wprintf(L"%s\n", flsOutlierCount.GetString());

			const CFLString<wchar_t> arrFlsAxis[3] = { L"x", L"y", L"z" };
			const CFLArray<double>* arrPFlaCoef[3] = { &flaCoefX, &flaCoefY, &flaCoefZ };

			for(int32_t k = 0; k < 3; ++k)
			{
				const CFLArray<double>& flaCoef = *arrPFlaCoef[k];
				wprintf(L"%s(t) = %.6lft^3 %+.6lft^2 %+.6lft %+.6lf\n", arrFlsAxis[k].GetString(), flaCoef.GetAt(0), flaCoef.GetAt(1), flaCoef.GetAt(2), flaCoef.GetAt(3));
			}

			wprintf(L"\n");
		}

		if(res.IsFail())
			break;

		// 두 뷰의 시점을 맞춤 // Fit the viewpoint of the two views
		view3D[0].ZoomFit();
		view3D[0].SynchronizePointOfView(&view3D[1]);

		// 두 뷰를 하나의 창으로 취급 // Treat the two views as one window
		view3D[0].SynchronizeWindow(&view3D[1]);

		for(int32_t i = 0; i < 2; ++i)
			view3D[i].Invalidate();

		// 3D 뷰가 하나라도 꺼지면 종료로 간주 // Consider closed when any of the 3D views are turned off
		while(view3D[0].IsAvailable() && view3D[1].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}
