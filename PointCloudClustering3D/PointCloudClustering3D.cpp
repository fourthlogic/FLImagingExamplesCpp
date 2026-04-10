#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 객체 선언 // Declare 3D object
	CFL3DObject floDestinationObject;
	CFL3DObject floSourceObject;

	// 3D 뷰 선언 // Declare 3D view	
	CGUIView3DWrap view3DDst;
	CGUIView3DWrap view3DSource;


	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult eResult = EResult_UnknownError;


		// Source Object 로드 // Load the Source object
		if((eResult = floSourceObject.Load(L"../../ExampleImages/Smoothing3D/Smoothing3DExample.ply")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the object file.\n");
			break;
		}

		// Source 3D 뷰 생성
		if(IsFail(eResult = view3DSource.Create(612, 0, 1124, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Source 3D view.\n");
			break;
		}

		// Dst 3D 뷰 생성
		if(IsFail(eResult = view3DDst.Create(1124, 0, 1636, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Destination 3D view.\n");
			break;
		}

		// Source Object 3D 뷰 생성 // Create the source object 3D view
		if((eResult = view3DSource.PushObject(floSourceObject)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to display the 3D object.\n");
			break;
		}

		// PointCloudClustering3D 객체 생성 // Create PointCloudClustering3D object
		CPointCloudClustering3D pointCloudClustering3D;

		// Source object 설정 // Set the source object
		pointCloudClustering3D.SetSourceObject(floSourceObject);

		// 결과 최소 군집 포인트 수 설정 // Set the minimum number of points per cluster
		pointCloudClustering3D.SetMinCount(500);

		// 탐색 반경 계수 설정 // Set the radius coefficient
		pointCloudClustering3D.SetRadiusCoefficient(0.8f);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((eResult = pointCloudClustering3D.Execute()).IsFail())
		{
			ErrorPrint(eResult, L"Failed to execute Point Cloud Clustering 3D.");
			break;
		}

		view3DSource.ZoomFit();
		view3DDst.ZoomFit();


		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately		
		CGUIView3DLayerWrap layer3DDst = view3DDst.GetLayer(0);
		CGUIView3DLayerWrap layer3DSource = view3DSource.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer3DDst.Clear();
		layer3DSource.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic

		if((eResult = layer3DSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		if((eResult = layer3DDst.DrawTextCanvas(CFLPoint<double>(0, 0), L"Clustering Result", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		// 3D 오브젝트 뷰에 Destination 오브젝트 디스플레이
		int32_t i32ResultCount = pointCloudClustering3D.GetResultClusterCount();

		for(int32_t i = 0; i < i32ResultCount; ++i)
		{
			CFL3DObject flo;
			pointCloudClustering3D.GetResult3DObject(i, flo);

			TPoint3<uint8_t> tp3Color;
			Base::CColorPaletteUtilities::GetColor(i + 1, tp3Color.x, tp3Color.y, tp3Color.z);

			int32_t i32VertexCount = (int32_t)flo.GetVertices()->GetCount();
			CFLArray<TPoint3<uint8_t>>* pFlaColors = (CFLArray<TPoint3<uint8_t>>*)flo.GetVertexColors();

			pFlaColors->Clear();
			pFlaColors->Reserve(i32VertexCount);

			for(int32_t idx = 0; idx < i32VertexCount; ++idx)
				pFlaColors->PushBack(tp3Color);

			view3DDst.PushObject(flo);

			TPoint3<float> tp3Min, tp3Max;
			TPoint3<double> tp3Center;
			flo.GetBoundingBox(tp3Min, tp3Max);

			tp3Center.x = (double)((tp3Min.x + tp3Max.x) / 2);
			tp3Center.y = (double)((tp3Min.y + tp3Max.y) / 2);
			tp3Center.z = (double)((tp3Min.z + tp3Max.z) / 2);

			layer3DDst.DrawText3D(tp3Center, CFLString<wchar_t>().Format(L"%d", i), YELLOW, BLACK, 10);
		}

		view3DSource.ZoomFit();
		view3DDst.ZoomFit();

		view3DDst.SynchronizePointOfView(&view3DSource);

		// 3D 뷰를 갱신 합니다. // Update 3d view
		view3DSource.Invalidate(true);
		view3DDst.Invalidate(true);

		// 3D 뷰가 종료될 때 까지 기다림 // Wait for the 3D view to close
		while(view3DSource.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}