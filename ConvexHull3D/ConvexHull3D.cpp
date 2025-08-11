#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"
#include <map>

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CFL3DObject floSrc;
	CFL3DObject floDst;
	CGUIView3DWrap view3DSrc;
	CGUIView3DWrap view3DDst;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Load the image
		if((res = floSrc.Load(L"../../ExampleImages/ConvexHull3D/RandomPointsOnSphere.ply")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the 3D Object.\n");
			break;
		}
		
		// CConvexHull3D 객체 생성 // Create CConvexHull3D object
		CConvexHull3D convexHull3D;


		{
			// 이미지 뷰 생성 // Create image view
			if((res = view3DSrc.Create(100, 0, 612, 512)).IsFail() ||
			   (res = view3DDst.Create(612, 0, 1124, 512)).IsFail())
			{
				ErrorPrint(res, "Failed to create 3D views.\n");
				break;
			}

			// 처리할 3D 객체 설정 // Set 3D object to process
			convexHull3D.SetSourceObject(floSrc);
			convexHull3D.SetDestinationObject(floDst);
			
			// 생성될 볼록껍질 객체의 컬러를 설정 // Set color of new 3D object
			convexHull3D.EnableVertexRecoloring(true);
			convexHull3D.SetTargetVertexColor(TPoint3<uint8_t>(255, 125, 0));

			// 3D 뷰에 표시할 3D 객체 추가 // Add 3D object to the 3D view
			view3DSrc.PushObject(floSrc);
			view3DSrc.SetPointSize(3);

			// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
			if((res = convexHull3D.Execute()).IsFail())
			{
				ErrorPrint(res, L"Failed to execute Convex Hull 3D.");
				break;
			}

			// 3D 뷰에 표시할 3D 객체 추가 // Add 3D object to the 3D view
			view3DDst.PushObject(floDst);

			// 화면에 출력하기 위해 View에서 레이어 0번을 얻어옴 // Obtain layer number 0 from view for display
			// 이 객체는 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to a view and does not need to be released separately		
			CGUIView3DLayerWrap layerViewSrc = view3DSrc.GetLayer(0);
			CGUIView3DLayerWrap layerViewDst = view3DDst.GetLayer(0);

			// View 정보를 디스플레이 한다. // Display view information
			CFLPoint<double> flpTopLeft(0, 0);
			if((res = layerViewSrc.DrawTextCanvas(flpTopLeft, L"Source Object(Point Cloud)", YELLOW, BLACK, 20)).IsFail() ||
			   (res = layerViewDst.DrawTextCanvas(flpTopLeft, L"Destination Object(Convex Hull)", YELLOW, BLACK, 20)).IsFail())
			{
				ErrorPrint(res, L"Failed to draw text.\n");
				break;
			}

			// 입력 뷰의 시점을 이동
			view3DSrc.ZoomFit();

			// 입력, 출력 뷰의 시점을 맞춤
			view3DSrc.SynchronizePointOfView(&view3DDst);

			// 입력, 출력 뷰를 하나의 창으로 취급
			view3DSrc.SynchronizeWindow(&view3DDst);

			view3DSrc.Invalidate();
			view3DDst.Invalidate();

			// 3D 뷰가 종료될 때까지 기다림
			while(view3DSrc.IsAvailable() && view3DDst.IsAvailable())
				CThreadUtilities::Sleep(1);
		}
	}
	while(false);

	return 0;
}