#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 뷰 선언 // Declare image view
	CGUIView3DWrap view3DDst;
	
	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult res = EResult_UnknownError;

		// Destination 3D 뷰 생성 // Create the destination 3D view
		if((res = view3DDst.Create(600, 0, 1100, 500)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		view3DDst.PushObject(CFL3DObject());
		CGUIView3DObject* pViewObject = (CGUIView3DObject *)view3DDst.GetView3DObject(0);
		CFL3DObject& floDst = *(CFL3DObject*)pViewObject->Get3DObject();

		// Perspective Transform 3D 객체 생성 // Create Perspective Transform 3D object
		CPointCloudGenerator3D alg;

		// 파라미터 설정 // Set parameter
		alg.SetDestinationObject(floDst);
		alg.EnableColorGeneration(true);
		alg.EnableNormalGeneration(false);

		int64_t arrI64count[3] = {};

		alg.AddPredefinedObject(true, arrI64count, EPredefinedObject_Regular_DodecaHedron, TPoint3<uint8_t>(255, 255, 255));

		arrI64count[0] = 4000;
		alg.AddPredefinedObject(false, arrI64count, EPredefinedObject_Regular_DodecaHedron, TPoint3<uint8_t>(255, 0, 0));
		arrI64count[0] = 0;

		arrI64count[1] = 20000;
		alg.AddPredefinedObject(false, arrI64count, EPredefinedObject_Regular_DodecaHedron, TPoint3<uint8_t>(0, 255, 0));
		arrI64count[1] = 0;

		arrI64count[2] = 100000;
		alg.AddPredefinedObject(false, arrI64count, EPredefinedObject_Regular_DodecaHedron, TPoint3<uint8_t>(0, 0, 255));
		arrI64count[2] = 0;

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIView3DLayerWrap layer3DDst = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer3DDst.Clear();

		CFLPoint<double> flpTopLeft(0, 0);

		if((res = layer3DDst.DrawTextCanvas(flpTopLeft, L"Destination Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = alg.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute.");
			break;
		}

		pViewObject->UpdateAll();
		view3DDst.UpdateObject(0);
		view3DDst.ZoomFit();

		// 뷰가 종료될 때까지 계속 실행
		while(view3DDst.IsAvailable())
		{
			if((res = alg.Execute()).IsFail())
			{
				ErrorPrint(res, L"Failed to execute.");
				break;
			}
			
			if(!view3DDst.IsAvailable())
				break;

			pViewObject->UpdateVertex();
			view3DDst.UpdateObject(0);

			view3DDst.UpdateScreen();
			CThreadUtilities::Sleep(100);
		}
	}
	while(false);

	return 0;
}