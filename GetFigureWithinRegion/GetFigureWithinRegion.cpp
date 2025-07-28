#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage[4];

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

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

		if(IsFail(res = viewImage[2].Create(400, 400, 912, 794)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[3].Create(912, 400, 1424, 794)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// SourceView, DstView 의 0번 레이어 가져오기 // Get Layer 0 of SourceView, DstView
		CGUIViewImageLayerWrap layerSrc0 = viewImage[0].GetLayer(0);
		CGUIViewImageLayerWrap layerDst0 = viewImage[1].GetLayer(0);
		CGUIViewImageLayerWrap layerSrc1 = viewImage[2].GetLayer(0);
		CGUIViewImageLayerWrap layerDst1 = viewImage[3].GetLayer(0);

		layerSrc0.DrawTextCanvas(TPoint<double>(0, 0), L"Source Figure And Region1", YELLOW, BLACK, 15);
		layerDst0.DrawTextCanvas(TPoint<double>(0, 0), L"Get Figure Within Region1", YELLOW, BLACK, 15);
		layerSrc1.DrawTextCanvas(TPoint<double>(0, 0), L"Source Figure And Region2", YELLOW, BLACK, 15);
		layerDst1.DrawTextCanvas(TPoint<double>(0, 0), L"Get Figure Within Region2", YELLOW, BLACK, 15);

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		for(int32_t i = 1; i < 4; ++i)
		{
			if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[i])))
			{
				ErrorPrint(res, "Failed to synchronize view\n");
				break;
			}
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		for(int32_t i = 1; i < 4; ++i)
		{
			if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[i])))
			{
				ErrorPrint(res, "Failed to synchronize window.\n");
				break;
			}
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		for(int32_t i = 1; i < 4; ++i)
		{
			if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[i])))
			{
				ErrorPrint(res, "Failed to synchronize window.\n");
				break;
			}
		}

		// Figure 생성 // Create figure
		CFLLine<double> fll(76, 300, 130, 210);

		CFLRect<double> flr(50, 50, 100, 100);

		CFLCircle<double> flc(150., 100., 30., 0., 0., 80., EArcClosingMethod_Center);

		CFLEllipse<double> fle(300, 150, 100, 50, 0, 180, 60, EArcClosingMethod_EachOther);

		CFLComplexRegion flcr;
		flcr.PushBack(CFLPoint<double>(270, 100));
		flcr.PushBack(CFLPoint<double>(420, 160));
		flcr.PushBack(CFLPoint<double>(300, 200));

		CFLFigureArray flfaSource;

		flfaSource.PushBack(fll);
		flfaSource.PushBack(flr);
		flfaSource.PushBack(flc);
		flfaSource.PushBack(fle);
		flfaSource.PushBack(flcr);

		// Region 생성 // Create region
		CFLComplexRegion flcrRegion1;

		flcrRegion1.PushBack(CFLPoint<double>(0, 0));
		flcrRegion1.PushBack(CFLPoint<double>(220, 50));
		flcrRegion1.PushBack(CFLPoint<double>(240, 100));
		flcrRegion1.PushBack(CFLPoint<double>(200, 150));
		flcrRegion1.PushBack(CFLPoint<double>(110, 170));
		flcrRegion1.PushBack(CFLPoint<double>(70, 200));

		CFLComplexRegion flcrRegion2;

		flcrRegion2.PushBack(CFLPoint<double>(150, 100));
		flcrRegion2.PushBack(CFLPoint<double>(240, 160));
		flcrRegion2.PushBack(CFLPoint<double>(430, 250));
		flcrRegion2.PushBack(CFLPoint<double>(300, 400));
		flcrRegion2.PushBack(CFLPoint<double>(200, 300));
		flcrRegion2.PushBack(CFLPoint<double>(140, 200));
		flcrRegion2.PushBack(CFLPoint<double>(110, 80));

		CFLString<wchar_t> strFigure;
		wprintf(L"Source Figure Array\n");

		strFigure.Format(L"%s\n\n", CFigureUtilities::ConvertFigureObjectToString(&flfaSource).GetString());
		wprintf(L"%s", strFigure.GetString());

		wprintf(L"Region1\n");

		strFigure.Format(L"%s\n\n", CFigureUtilities::ConvertFigureObjectToString(&flcrRegion1).GetString());
		wprintf(L"%s", strFigure.GetString());

		// SourceView1의 0번 레이어에 Source Figure, Region1 그리기 // Draw Source Figure, Region1 on Layer 0 of SourceView1
		layerSrc0.DrawFigureImage(flfaSource, CYAN);
		layerSrc0.DrawFigureImage(flcrRegion1, BLUE, 1, BLUE, EGUIViewImagePenStyle_Solid, 1, 0.2f);

		wprintf(L"Region2\n");

		strFigure.Format(L"%s\n\n", CFigureUtilities::ConvertFigureObjectToString(&flcrRegion2).GetString());
		wprintf(L"%s", strFigure.GetString());

		// SourceView2의 0번 레이어에 Source Figure, Region2 그리기 // Draw Source Figure, Region2 on Layer 0 of SourceView2
		layerSrc1.DrawFigureImage(flfaSource, CYAN);
		layerSrc1.DrawFigureImage(flcrRegion2, BLUE, 1, BLUE, EGUIViewImagePenStyle_Solid, 1, 0.2f);

		// Region1과 겹쳐지는 Figure 추출 // Get figure overlapping with Region1
		CFLFigureArray flfaResult1;
		flfaSource.GetFigureWithinRegion(flcrRegion1, &flfaResult1);

		wprintf(L"Result Figure Within Region1\n");

		strFigure.Format(L"%s\n\n", CFigureUtilities::ConvertFigureObjectToString(&flfaResult1).GetString());
		wprintf(L"%s", strFigure.GetString());

		// DstView1의 0번 레이어에 결과 그리기 // Draw the result on layer 0 of DstView1
		layerDst0.DrawFigureImage(flfaSource, CYAN);
		layerDst0.DrawFigureImage(flcrRegion1, BLUE, 1, BLUE, EGUIViewImagePenStyle_Solid, 1, 0.2f);
		layerDst0.DrawFigureImage(flfaResult1, LIME, 3, LIME, EGUIViewImagePenStyle_Solid, 1, 0.2f);

		// Region2과 겹쳐지는 Figure 추출 // Get figure overlapping with Region2
		CFLFigureArray flfaResult2;
		flfaSource.GetFigureWithinRegion(&flcrRegion2, &flfaResult2);

		wprintf(L"Result Figure Within Region2\n");

		strFigure.Format(L"%s\n\n", CFigureUtilities::ConvertFigureObjectToString(&flfaResult2).GetString());
		wprintf(L"%s", strFigure.GetString());

		// DstView1의 0번 레이어에 결과 그리기 // Draw the result on layer 0 of DstView1
		layerDst1.DrawFigureImage(flfaSource, CYAN);
		layerDst1.DrawFigureImage(flcrRegion2, BLUE, 1, BLUE, EGUIViewImagePenStyle_Solid, 1, 0.2f);
		layerDst1.DrawFigureImage(flfaResult2, LIME, 3, LIME, EGUIViewImagePenStyle_Solid, 1, 0.2f);

		// 이미지 뷰를 갱신 합니다. // Update image view
		for(int32_t i = 0; i < 4; ++i)
			viewImage[i].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage[0].IsAvailable() && viewImage[1].IsAvailable() && viewImage[2].IsAvailable() && viewImage[3].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}