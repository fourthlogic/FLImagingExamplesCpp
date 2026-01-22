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
		// Source View 생성 // Create Source View
		if(IsFail(res = viewImage[0].Create(200, 0, 700, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Reduce result1 View 생성 // Create Reduce result1 view
		if(IsFail(res = viewImage[1].Create(700, 0, 1200, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Reduce result2 View 생성 // Create Reduce result2 view
		if(IsFail(res = viewImage[2].Create(1200, 0, 1700, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 각 이미지 뷰의 시점을 동기화 한다. // Synchronize the viewpoint of each image view.
		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[1])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}
		if(IsFail(res = viewImage[1].SynchronizePointOfView(&viewImage[2])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 각 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the position of each image view window
		if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[1])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}
		if(IsFail(res = viewImage[1].SynchronizeWindow(&viewImage[2])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View 에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer[3];

		for(int32_t i = 0; i < 3; ++i)
			layer[i] = viewImage[i].GetLayer(0);

		// 화면상 좌표(고정 좌표)에 Source Figure View 임을 표시
		// Indicates Source Figure View on screen coordinates (fixed coordinates)
		layer[0].DrawTextCanvas(&CFLPoint<int32_t>(0, 0), L"Source Figure", YELLOW, BLACK, 30);
		// 화면상 좌표(고정 좌표)에 Reduce Result View 임을 표시
		// Indicates Reduce Result View on screen coordinates (fixed coordinates)
		layer[1].DrawTextCanvas(&CFLPoint<int32_t>(0, 0), L"Reduce Result1", YELLOW, BLACK, 30);
		layer[2].DrawTextCanvas(&CFLPoint<int32_t>(0, 0), L"Reduce Result2", YELLOW, BLACK, 30);

		CFLRegion flrgSourceFig;

		// Source Figure 불러오기 // Load source figure
		if((res = flrgSourceFig.Load(L"../../ExampleImages/Figure/RegionForReduce.fig")).IsFail())
		{
			ErrorPrint(res, "Failed to load figure.\n");
			break;
		}

		// 0번 Layer 에 Figure 와 Text 를 출력 // Draw Figure and Text to Layer 0
		layer[0].DrawFigureImage(flrgSourceFig, LIME, 3);
		layer[0].DrawFigureImage((CFLPointArray)flrgSourceFig, BLACK, 1);
		layer[0].DrawTextImage(flrgSourceFig.GetCenter(), CFLString<wchar_t>().Format(L"vertex count : %lld", flrgSourceFig.GetCount()), LIME, BLACK, 17, false, 0., EGUIViewImageTextAlignment_CENTER_CENTER);


		// Reduce 함수 실행 (Epsilon : 10.0) // Reduce function execution (Epsilon : 10.0)
		CFLRegion flrgResult1;
		double f64Epsilon1 = 10.;

		if((res = flrgSourceFig.Reduce(f64Epsilon1, EReduceCriterion_Epsilon, &flrgResult1)).IsFail())
		{
			ErrorPrint(res, "Failed to calculate.\n");
			break;
		}

		// Reduce 함수 실행 (Epsilon : 15.0) // Reduce function execution (Epsilon : 15.0)
		CFLRegion flrgResult2;
		double f64Epsilon2 = 15.;

		if((res = flrgSourceFig.Reduce(f64Epsilon2, EReduceCriterion_Epsilon, &flrgResult2)).IsFail())
		{
			ErrorPrint(res, "Failed to calculate.\n");
			break;
		}

		// View 에 결과 Region 과 정점 그리기 // Draw the resulting Region and vertices in the View
		layer[1].DrawFigureImage(flrgResult1, CYAN, 3);
		layer[2].DrawFigureImage(flrgResult2, YELLOW, 3);
		layer[1].DrawFigureImage((CFLPointArray)flrgResult1, BLACK, 1);
		layer[2].DrawFigureImage((CFLPointArray)flrgResult2, BLACK, 1);
		layer[1].DrawTextCanvas(&CFLPoint<int32_t>(0, 40), L"epsilon : 10.0", YELLOW, BLACK, 20);
		layer[2].DrawTextCanvas(&CFLPoint<int32_t>(0, 40), L"epsilon : 15.0", YELLOW, BLACK, 20);
		layer[1].DrawTextImage(flrgResult1.GetCenter(), CFLString<wchar_t>().Format(L"vertex count : %lld", flrgResult1.GetCount()), CYAN, BLACK, 17, false, 0., EGUIViewImageTextAlignment_CENTER_CENTER);
		layer[2].DrawTextImage(flrgResult2.GetCenter(), CFLString<wchar_t>().Format(L"vertex count : %lld", flrgResult2.GetCount()), YELLOW, BLACK, 17, false, 0., EGUIViewImageTextAlignment_CENTER_CENTER);

 		// Console 출력 // Console output
  		wprintf(L"\nSource Region Points : \nvertex count = %lld\n\n", flrgSourceFig.GetCount());
  
  		for(int64_t i = 0; i < flrgSourceFig.GetCount(); ++i)
  			wprintf(L"[%lld] (%.3lf,%.3lf)\n", i, flrgSourceFig.GetAt(i)->x, flrgSourceFig.GetAt(i)->y);

		wprintf(L"\n\nResult1 Region Points : \nepsilon = %.1lf\nvertex count = %lld\n\n", f64Epsilon1, flrgResult1.GetCount());

		for(int64_t i = 0; i < flrgResult1.GetCount(); ++i)
			wprintf(L"[%lld] (%.3lf,%.3lf)\n", i, flrgResult1.GetAt(i)->x, flrgResult1.GetAt(i)->y);

		wprintf(L"\n\nResult2 Region Points : \nepsilon = %.1lf\nvertex count = %lld\n\n", f64Epsilon2, flrgResult2.GetCount());

		for(int64_t i = 0; i < flrgResult2.GetCount(); ++i)
			wprintf(L"[%lld] (%.3lf,%.3lf)\n", i, flrgResult2.GetAt(i)->x, flrgResult2.GetAt(i)->y);


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