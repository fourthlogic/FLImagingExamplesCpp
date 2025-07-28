#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	const int32_t i32ViewCount = 4;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage[i32ViewCount];
	CResult res = EResult_UnknownError;

	do
	{
		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage[0].Create(400, 0, 912, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[1].Create(912, 0, 1424, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}
		if(IsFail(res = viewImage[2].Create(400, 384, 912, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[3].Create(912, 384, 1424, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		for(int32_t i = 1; i < i32ViewCount; ++i)
		{
			// 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoint of the image view
			if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[i])))
			{
				ErrorPrint(res, "Failed to synchronize view\n");
				break;
			}

			// 이미지 뷰 윈도우의 위치를 맞춤 // Align the position of the image view window
			if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[i])))
			{
				ErrorPrint(res, "Failed to synchronize window.\n");
				break;
			}
		}

		// SourceView, DstView 의 0번 레이어 가져오기 // Get Layer 0 of SourceView, DstView
		CGUIViewImageLayerWrap layerView[i32ViewCount];

		for(int32_t i = 0; i < i32ViewCount; ++i)
			layerView[i] = viewImage[i].GetLayer(0);

		// Figure 생성 // Create Figure
		CFLRect<double> flr(50, 50, 100, 100, 15);
		CFLQuad<double> flq(200, 50, 360, 50, 400, 160, 150, 110);
		CFLCircle<double> flc(100., 150., 30., 0, 30, 90, EArcClosingMethod_Center);
		CFLEllipse<double> fle(300, 250, 100, 50, 0, 30, 200, EArcClosingMethod_Center);

		CFLFigureArray flfa;

		flfa.PushBack(flr);
		flfa.PushBack(flq);
		flfa.PushBack(flc);
		flfa.PushBack(fle);

		CFLString<wchar_t> strFigure;
		wprintf(L"Figure Array\n");

		for(int32_t i = 0; i < flfa.GetCount(); ++i)
		{
			strFigure.Format(L"[%d]\n %s\n", i, CFigureUtilities::ConvertFigureObjectToString(flfa.GetAt(i)).GetString());
			wprintf(L"%s", strFigure.GetString());
		}

		wprintf(L"\n");

		// Figure 그리기 // Draw Figure
		for(int32_t i = 0; i < flfa.GetCount(); ++i)
			layerView[i].DrawFigureImage(flfa, LIME);

		//////////////////////////////// GetCenterElementwise()
		// 중심점 좌표를 담을 FigureArray 생성 // Create a FigureArray to hold the coordinates of the center point
		CFLFigureArray flfaCenter;

		// Figure Array 각 요소의 중심점 계산 // Calculate the center point of each element of Figure Array
		flfa.GetCenterElementwise(&flfaCenter);

		// 중심들을 View0의 0번 레이어에 그리기 // Draw the centers on layer 0 of View0
		layerView[0].DrawFigureImage(flfaCenter, RED);
		layerView[0].DrawTextCanvas(TPointD(0, 0), L"GetCenterElementwise() Result", YELLOW, BLACK, 15);

		// 콘솔에 중심 좌표 표시 // Print center coordinates in console
		wprintf(L"Center Point\n");

		for(int32_t i = 0; i < flfa.GetCount(); ++i)
		{
			strFigure.Format(L"[%d]\n %s\n", i, CFigureUtilities::ConvertFigureObjectToString(flfaCenter.GetAt(i)).GetString());
			wprintf(L"%s", strFigure.GetString());
		}

		wprintf(L"\n");


		//////////////////////////////// GetPerimeterElementwise()
		// 각 둘레의 길이를 저장할 CFLFigureArray 생성 // Create CFLFigureArray to store the length of each perimeter
		CFLFigureArray flfaPerimeter;

		// Figure Array 각 요소의 둘레 계산 // Calculate the perimeter of each element of the Figure Array
		flfa.GetPerimeterElementwise(&flfaPerimeter);

		// Figure Array 각 요소의 둘레 표시 // Display perimeter of each element of Figure Array
		for(int64_t i = 0; i < flfaPerimeter.GetCount(); ++i)
		{
			CFLString<wchar_t> strPerimeter;
			strPerimeter.Format(L"%lf", ((CFLScalarD*)flfaPerimeter[i])->v);
			layerView[1].DrawTextImage(flfaCenter[i], strPerimeter, BLACK);
		}
		layerView[1].DrawTextCanvas(TPointD(0, 0), L"GetPerimeterElementwise() Result", YELLOW, BLACK, 15);

		// 콘솔에 길이 표시 // Display the length in the console
		wprintf(L"Perimeter\n");

		for(int64_t i = 0; i < flfaPerimeter.GetCount(); ++i)
		{
			strFigure.Format(L"[%d]\n %lf\n", i, ((CFLScalarD*)flfaPerimeter[i])->v);
			wprintf(L"%s", strFigure.GetString());
		}

		wprintf(L"\n");


		//////////////////////////////// GetCenterOfGravityElementwise()
		// 무게중심점 좌표를 담을 FigureArray 생성 // Create a FigureArray to contain the coordinates of the center of gravity
		CFLFigureArray flfaCenterOfGravity;

		// Figure Array 각 요소의 무게중심점 계산 // Calculate the center of gravity of each element of the Figure Array
		flfa.GetCenterOfGravityElementwise(&flfaCenterOfGravity);

		// 무게중심들을 View0의 0번 레이어에 그리기 // Draw the centers of gravity on Layer 0 of View0
		layerView[2].DrawFigureImage(flfaCenterOfGravity, CYAN);
		layerView[2].DrawTextCanvas(TPointD(0, 0), L"GetCenterOfGravityElementwise() Result", YELLOW, BLACK, 15);

		// 콘솔에 무게중심 좌표 표시 // Display barycentric coordinates in console
		wprintf(L"Center Of Gravity Point\n");

		for(int32_t i = 0; i < flfa.GetCount(); ++i)
		{
			strFigure.Format(L"[%d]\n %s\n", i, CFigureUtilities::ConvertFigureObjectToString(flfaCenterOfGravity.GetAt(i)).GetString());
			wprintf(L"%s", strFigure.GetString());
		}

		wprintf(L"\n");


		//////////////////////////////// GetMinimumEnclosingRectangleElementwise()
		// 최소둘레 직사각형을 담을 FigureArray 생성 // Create a FigureArray to contain the minimum enclosing rectangle
		CFLFigureArray flfaMER;

		// Figure Array 각 요소의 최소둘레 직사각형을 계산 // Calculate the minimum enclosing rectangle of each element of the Figure Array
		flfa.GetMinimumEnclosingRectangleElementwise(&flfaMER);

		// 최소둘레 직사각형들을 View0의 0번 레이어에 그리기 // Draw the minimum enclosing rectangle on Layer 0 of View0
		layerView[3].DrawFigureImage(flfaMER, BLUE);
		layerView[3].DrawTextCanvas(TPointD(0, 0), L"GetMinimumEnclosingRectangleElementwise() Result", YELLOW, BLACK, 15);

		// 콘솔에 최소둘레 직사각형을 표시 // Display the minimum enclosing rectangle in console
		wprintf(L"Minimum Enclosing Rectangle\n");

		for(int32_t i = 0; i < flfa.GetCount(); ++i)
		{
			strFigure.Format(L"[%d]\n %s\n", i, CFigureUtilities::ConvertFigureObjectToString(flfaMER.GetAt(i)).GetString());
			wprintf(L"%s", strFigure.GetString());
		}

		wprintf(L"\n");

		// 이미지 뷰를 갱신 합니다. // Update image view
		for(int32_t i = 0; i < i32ViewCount; ++i)
		{
			viewImage[i].Invalidate(true);
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage[0].IsAvailable() && viewImage[1].IsAvailable() && viewImage[2].IsAvailable() && viewImage[3].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}