#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 뷰 선언 // Declare image view
	CGUIView3DWrap view3DSrc;
	CGUIView3DWrap view3DDst[2];
	CFL3DObject fl3DObjectSrc;
	CFL3DObject fl3DObjectDst1;
	CFL3DObject fl3DObjectDst2;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult res = EResult_UnknownError;

		// Source 3D 뷰 생성 // Create the Source 3D view
		if((res = view3DSrc.Create(100, 0, 600, 500)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 1 3D 뷰 생성 // Create the destination 3D view
		if((res = view3DDst[0].Create(600, 0, 1100, 500)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 2 3D 뷰 생성 // Create the destination 3D view
		if((res = view3DDst[1].Create(1100, 0, 1600, 500)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		fl3DObjectSrc.Load(L"../../ExampleImages/SphericalHarmonicsTransform3D/Airplane.fl3do");

		// 객체 생성 // Create object
		CSphericalHarmonicsTransform3D sphericalHarmonicsTransform3D;

		// 파라미터 설정 // Set parameter
		sphericalHarmonicsTransform3D.SetSourceObject(fl3DObjectSrc);
		sphericalHarmonicsTransform3D.SetDestinationObject(fl3DObjectDst1);
		sphericalHarmonicsTransform3D.SetDirectionType(CSphericalHarmonicsTransform3D::ETransformDirection_Forward);
		sphericalHarmonicsTransform3D.SetMaxDegree(15);
		
		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = sphericalHarmonicsTransform3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to Forward.");
			break;
		}

		// 파라미터 설정 // Set parameter
		sphericalHarmonicsTransform3D.SetSourceObject(fl3DObjectDst1);
		sphericalHarmonicsTransform3D.SetDestinationObject(fl3DObjectDst2);
		sphericalHarmonicsTransform3D.SetDirectionType(CSphericalHarmonicsTransform3D::ETransformDirection_Inverse);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = sphericalHarmonicsTransform3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to Inverse.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIView3DLayerWrap layer3DSrc = view3DSrc.GetLayer(0);
		CGUIView3DLayerWrap layer3DDst1 = view3DDst[0].GetLayer(0);
		CGUIView3DLayerWrap layer3DDst2 = view3DDst[1].GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer3DSrc.Clear();
		layer3DDst1.Clear();
		layer3DDst2.Clear();

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		view3DSrc.PushObject(fl3DObjectSrc);
		view3DSrc.ZoomFit();

		view3DDst[0].PushObject(fl3DObjectDst1);
		view3DDst[0].SetPointSize(10);
		view3DDst[0].ZoomFit();

		view3DDst[1].PushObject(fl3DObjectDst2);
		view3DDst[1].SetPointSize(2);
		view3DDst[1].SetShadingType(EShadingType3D_Shadeless);
		view3DDst[1].SynchronizePointOfView(&view3DSrc);
		view3DDst[1].ZoomFit();
		
		CFLPoint<double> flpTopLeft(0, 0);

		if((res = layer3DSrc.DrawTextCanvas(flpTopLeft, L"Source Object", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DDst1.DrawTextCanvas(flpTopLeft, L"Forward Result", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DDst2.DrawTextCanvas(flpTopLeft, L"Inverse Result", YELLOW, BLACK, 20)).IsFail() )
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		view3DSrc.Invalidate(true);
		view3DDst[0].Invalidate(true);
		view3DDst[1].Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림
		while(view3DSrc.IsAvailable() && view3DDst[0].IsAvailable() && view3DDst[1].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}