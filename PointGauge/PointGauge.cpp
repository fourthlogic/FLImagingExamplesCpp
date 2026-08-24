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

	CPointGauge::ETransitionType arrTransitionType[] =
	{
		CPointGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CPointGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CPointGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CPointGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CPointGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CPointGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CPointGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CPointGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CPointGauge::ETransitionType_DarkToBrightOrBrightToDark,
		CPointGauge::ETransitionType_DarkToBright,
		CPointGauge::ETransitionType_BrightToDark,
		CPointGauge::ETransitionType_DarkToBrightToDark,
	};

	CPointGauge::ETransitionChoice arrTransitionChoice[] =
	{
		CPointGauge::ETransitionChoice_Begin,
		CPointGauge::ETransitionChoice_Begin,
		CPointGauge::ETransitionChoice_Begin,
		CPointGauge::ETransitionChoice_LargestArea,
		CPointGauge::ETransitionChoice_End,
		CPointGauge::ETransitionChoice_End,
		CPointGauge::ETransitionChoice_End,
		CPointGauge::ETransitionChoice_LargestAmplitude,
		CPointGauge::ETransitionChoice_Closest,
		CPointGauge::ETransitionChoice_Closest,
		CPointGauge::ETransitionChoice_Closest,
		CPointGauge::ETransitionChoice_Closest,
	};

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImage[i32ExampleCount];
	CResult res;

	do
	{
		// 이미지 로드 // Loads image
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/Gauge/Stripe.flif")))
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

		// Point Gauge 객체 생성 // Create Point Gauge object
		CPointGauge pointGauge;

		// 처리할 이미지 설정 // Set the image to process
		pointGauge.SetSourceImage(fliImage);

		// 측정할 영역을 설정합니다. // Set the area to measure.
		CFLPoint<double> measureCenter(267, 240);
		double tolerance = 400.;
		double angle = 25.;
		pointGauge.SetMeasurementRegion(measureCenter, tolerance, angle);

		// 추출하기위한 파라미터를 설정합니다. // Set parameters for extraction.
		// 점을 추정하기위해 추출할 경계점의 변화 임계값에 대해 설정합니다. // Set the threshold change of the boundary point to be extracted to estimate the point.
		pointGauge.SetThreshold(20);
		// 점을 추정하기위해 추출할 경계점의 변화 임계값에 보정값을 설정합니다. // Set the correction value to the threshold change of the boundary point to be extracted to estimate the point.
		pointGauge.SetMinimumAmplitude(10);
		// 점을 추정하기위해 추출할 경계점들의 대표값 표본 개수를 설정합니다. // Set the number of representative sample values ??of the boundary points to be extracted to estimate the points.
		pointGauge.SetThickness(1);


		for(int32_t i = 0; i < i32ExampleCount; ++i)
		{
			// 점을 추정하기위해 추출할 경계점 변화 방향에 대해 설정합니다. // Set the boundary point change direction to extract to estimate the point.
			pointGauge.SetTransitionType(arrTransitionType[i]);
			// 추출한 경계점 중 최종적으로 얻고자하는 경계점 유형을 선택합니다. // Select the boundary point type you want to finally get among the extracted boundary points.
			pointGauge.SetTransitionChoice(arrTransitionChoice[i]);

			// 알고리즘 수행 // Execute the algorithm
			if(IsFail(res = pointGauge.Execute()))
			{
				ErrorPrint(res, "Failed to execute Line gauge.\n");
				break;
			}

			// 실행 결과를 가져옵니다. // Get the execution result.
			int64_t i64Count = pointGauge.GetMeasuredObjectCount();
			// 추정과정에 사용된 선을 가져옵니다. // Get the line used in the estimation process.
			CFLLine<double> fllLine = pointGauge.GetMeasurementRegion();

			CGUIViewImageLayerWrap layer = viewImage[i].GetLayer(0);

			layer.Clear();

			if(IsFail(res = layer.DrawTextImage(CFLPoint<double>(0., 0.), arrWcsViewText[i], YELLOW, BLUE, 20, true)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			// 측정 중심 위치를 디스플레이한다. // Display the measurement center position.
			if(IsFail(res = layer.DrawFigureImage(measureCenter.MakeCrossHair(10), RED)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			// 추출된 점이 어디인지 알기 위해 디스플레이 한다 // Display to know where the extracted point is
			for(int32_t i32Index = 0; i32Index < (int32_t)i64Count; ++i32Index)
			{
				CFLPoint<double> flp;
				pointGauge.GetMeasuredObject(flp, i32Index);

				if(IsFail(res = layer.DrawFigureImage(flp.MakeCrossHair(10, true), BLACK, 3)))
				{
					ErrorPrint(res, "Failed to draw figure\n");
					break;
				}

				// begin, end인 경우 설정된 index(0~2)의 point는 CYAN색상으로 표시하고, 나머지 index는 YELLOW로 표시한다. // In the case of begin, end, the point of the set index (0-2) is displayed in CYAN color, and the remaining index is displayed in YELLOW.
				COLORREF col = (arrTransitionChoice[i] == CPointGauge::ETransitionChoice_Begin || arrTransitionChoice[i] == CPointGauge::ETransitionChoice_End) && i32Index != i % 4 ? YELLOW : CYAN;

				if(IsFail(res = layer.DrawFigureImage(flp.MakeCrossHair(10, true), col, 3)))
				{
					ErrorPrint(res, "Failed to draw figure\n");
					break;
				}

				printf("Index %d : (%lf, %lf)\n", i32Index, flp.x, flp.y);
			}

			// 측정 영역이 어디인지 알기 위해 디스플레이 한다 // Display to know where the measurement area is
			if(IsFail(res = layer.DrawFigureImage(&fllLine, BLUE)))
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