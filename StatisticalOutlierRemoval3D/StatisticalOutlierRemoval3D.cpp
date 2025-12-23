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
		if((eResult = floSourceObject.Load(L"../../ExampleImages/StatisticalOutlierRemoval3D/Cube_Noisy.fl3do")).IsFail())
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

		// StatisticalOutlierRemoval3D 객체 생성 // Create StatisticalOutlierRemoval3D object
		CStatisticalOutlierRemoval3D statisticalOutlierRemoval3D;

		// Source object 설정 // Set the source object
		statisticalOutlierRemoval3D.SetSourceObject(floSourceObject);

		// Destination object 설정 // Set the destination object
		statisticalOutlierRemoval3D.SetDestinationObject(floDestinationObject);

		// 탐색할 최근접 이웃 수 설정 // Set the number of nearest neighbors to search
		statisticalOutlierRemoval3D.SetNeighborCount(40);

		// 이상치 계수 설정 // Set the outlier coefficient
		statisticalOutlierRemoval3D.SetOutlierCoefficient(1);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((eResult = statisticalOutlierRemoval3D.Execute()).IsFail())
		{
			ErrorPrint(eResult, L"Failed to execute Statistical Outlier Removal 3D.");
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

		if((eResult = layer3DSource.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		if((eResult = layer3DDst.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		// 3D 오브젝트 뷰에 Destination 오브젝트 디스플레이
		if((eResult = view3DDst.PushObject((CFL3DObject)statisticalOutlierRemoval3D.GetDestinationObject())).IsFail())
		{
			ErrorPrint(eResult, L"Failed to set object on the 3d view.\n");
			break;
		}

		view3DSource.ZoomFit();
		view3DDst.ZoomFit();

		view3DDst.SynchronizePointOfView(&view3DSource);

		// 이미지 뷰를 갱신 합니다. // Update image view
		view3DSource.Invalidate(true);
		view3DDst.Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림
		while(view3DSource.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}