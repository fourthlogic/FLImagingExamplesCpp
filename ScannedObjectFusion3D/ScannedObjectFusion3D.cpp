#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 뷰 선언 // Declare 3d view	
	const int32_t i32SourceCount = 6;
	CGUIView3DWrap view3DDst;
	CGUIView3DWrap arrSourceView[6];

	// 3D 객체 선언 // Declare 3D object
	CFL3DObject arrSourceObjects[6];
	CFL3DObject floDst;

	CResult res = EResult_UnknownError;

	do
	{
		// ScannedObjectFusion3D 객체 생성 // Create ScannedObjectFusion3D object
		CScannedObjectFusion3D scannedObjectFusion3D;

		// Source object 로드 // load the source object									
		for(int32_t i = 0; i < i32SourceCount; ++i)
		{
			CFLString<wchar_t> flsPath(L"../../ExampleImages/ScannedObjectFusion3D");
			flsPath.AppendFormat(L"/%d.fl3do", i);

			if((res = arrSourceObjects[i].Load(flsPath)).IsFail())
			{
				ErrorPrint(res, L"Failed to load the object.\n");
				break;
			}

			scannedObjectFusion3D.AddSourceObject(arrSourceObjects[i]);
		}

		// Source Object 3D 뷰 생성 // Create the source object 3D view
		int32_t i32WindowWidth = 300;
		int32_t i32WindowHeight = 300;

		for(int32_t i = 0; i < i32SourceCount / 3; ++i)
		{
			int32_t i32Height = i32WindowHeight * i;

			for(int32_t j = 0; j < i32SourceCount / 2; ++j)
			{
				int32_t i32Width = i32WindowWidth * j;

				arrSourceView[i * 3 + j].Create(10 + i32Width, i32Height, 10 + i32Width + i32WindowWidth, i32Height + i32WindowHeight);
			}
		}

		// Destination Object 3D 뷰 생성 // Create the destination object 3D view
		if((res = view3DDst.Create(910, 10, 1510, 610)).IsFail())
			break;

		for(int32_t i = 1; i < i32SourceCount; ++i)
			arrSourceView[0].SynchronizeWindow(&arrSourceView[i]);

		CFLPoint<double> flpTopLeft(0, 0);

		for(int32_t i = 0; i < i32SourceCount; ++i)
		{
			arrSourceView[i].PushObject(arrSourceObjects[i]);
			arrSourceView[i].ZoomFit();

			CGUIView3DLayerWrap layer3DSrc = arrSourceView[i].GetLayer(0);
			CFLString<wchar_t> flsName(L"Scene");
			flsName.AppendFormat(L" %d", i);

			if(layer3DSrc.DrawTextCanvas(flpTopLeft, flsName, YELLOW, BLACK, 20).IsFail())
				continue;
		}

		// 샘플링 거리 설정 // Set the sampling distance
		scannedObjectFusion3D.SetSamplingDistance(0.03f);
		// 기준 좌표계 설정 // Set the base coordinate system
		scannedObjectFusion3D.SetBaseCoordinateFrame(0);
		// 데이터 취득 타입 설정 // Set the data acquisition type
		scannedObjectFusion3D.SetAcquisitionType(CScannedObjectFusion3D::EAcquisitionType_Unordered);
		// Destination object 설정 // Set the destination object
		scannedObjectFusion3D.SetDestinationObject(floDst);

		if((res = scannedObjectFusion3D.Calibrate()).IsFail())
		{
			ErrorPrint(res, L"Failed to Calibrate\n");
			break;
		}

		view3DDst.PushObject(floDst);

		CGUIView3DLayerWrap layer3DDst = view3DDst.GetLayer(0);

		if((res = layer3DDst.DrawTextCanvas(flpTopLeft, L"Calibration Result", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text.\n");
			break;
		}

		// 뷰를 갱신 // Update view
		for(int32_t i = 0; i < i32SourceCount; ++i)
			arrSourceView[i].Invalidate(true);

		view3DDst.ZoomFit();
		view3DDst.Invalidate(true);

		// 뷰가 종료될 때까지 기다림 // Wait for the view to close
		while(view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}