#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"
#include <map>

int main()
{
	// You must call the following function once
    // before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSrcXYZVImage;
	CFLImage fliSrcTextureImage;

	// 3D 객체 선언 // Declare 3D object
	CFL3DObject floDestination;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewSrcXYZVImage;
	CGUIViewImageWrap viewSrcTextureImage;

	// 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap view3DDst;

	do
	{
		// 수행 결과 객체 선언 // Declare execution result object
		CResult res = EResult_UnknownError;

		// Source XYZV 이미지 로드 // Load Source XYZV image
		if((res = fliSrcXYZVImage.Load(L"../../ExampleImages/XYZImageToPointCloudConverter3D/XYZV.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source Texture 이미지 로드 // Load Source Texture image
		if((res = fliSrcTextureImage.Load(L"../../ExampleImages/XYZImageToPointCloudConverter3D/Texture.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source XYZV 이미지 뷰 생성 // Create Source XYZV image view
		if((res = viewSrcXYZVImage.Create(100, 0, 612, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source Texture 이미지 뷰 생성 // Create Source Texture image view
		if((res = viewSrcTextureImage.Create(612, 0, 1124, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 3D 뷰 생성 // Create Destination 3D view
		if((res = view3DDst.Create(100, 512, 612, 1024)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the 3D view.\n");
			break;
		}

		// Source XYZV 이미지 뷰에 이미지를 디스플레이 // Display image in Source XYZV image view
		if((res = viewSrcXYZVImage.SetImagePtr(&fliSrcXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Source Texture 이미지 뷰에 이미지를 디스플레이 // Display image in Source Texture image view
		if((res = viewSrcTextureImage.SetImagePtr(&fliSrcTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 // Synchronize viewpoints of two image views
		if((res = viewSrcXYZVImage.SynchronizePointOfView(&viewSrcTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize point of view between image views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = view3DDst.SynchronizeWindow(&viewSrcXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = view3DDst.SynchronizeWindow(&viewSrcTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// XYZ Image To Point Cloud Converter 3D 객체 생성 // Create XYZ Image To Point Cloud Converter 3D object
		CXYZImageToPointCloudConverter3D xyzImageToPointCloudConverter3D;

		// Source XYZV 이미지 설정 // Set Source XYZV image
		if((res = xyzImageToPointCloudConverter3D.SetSourceImage(fliSrcXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Source XYZV image.\n");
			break;
		}

		// Source Texture 이미지 설정 // Set Source Texture image
		if((res = xyzImageToPointCloudConverter3D.SetTextureImage(fliSrcTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Source Texture image.\n");
			break;
		}

		// Destination Point Cloud 설정 // Set Destination Point Cloud
		if((res = xyzImageToPointCloudConverter3D.SetDestinationObject(floDestination)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination 3D object.\n");
			break;
		}

		// Coordinate Adjustment Scale 설정 // Set coordinate adjustment scale
		if((res = xyzImageToPointCloudConverter3D.SetCoordinateAdjustmentScale(1, -1, -1)).IsFail())
		{
			ErrorPrint(res, L"Failed to set coordinate adjustment scale.\n");
			break;
		}

		// Coordinate Adjustment Offset 설정 // Set coordinate adjustment offset
		if((res = xyzImageToPointCloudConverter3D.SetCoordinateAdjustmentOffset(-41, -5, 900)).IsFail())
		{
			ErrorPrint(res, L"Failed to set coordinate adjustment offset.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = xyzImageToPointCloudConverter3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute XYZ Image To Point Cloud Converter 3D.\n");
			break;
		}


		// 결과 3D 객체 출력 // Print resulting 3D Object
		if((res = view3DDst.PushObject(floDestination)).IsFail())
		{
			ErrorPrint(res, L"Failed to display the 3D Object.\n");
			break;
		}

		// 3D View 카메라 설정 // Set 3D view camera
		CFL3DCamera fl3DCam;

		fl3DCam.SetDirection(CFLPoint3<float>(0, 0, -1));
		fl3DCam.SetDirectionUp(CFLPoint3<float>(0, 1, 0));
		fl3DCam.SetPosition(CFLPoint3<float>(10, -20, 750));

		view3DDst.SetCamera(fl3DCam);

		// 화면에 출력하기 위해 이미지 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released
		CGUIViewImageLayerWrap layerViewXYZV = viewSrcXYZVImage.GetLayer(0);
		CGUIViewImageLayerWrap layerViewTexture = viewSrcTextureImage.GetLayer(0);

		// 화면에 출력하기 위해 3D 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from 3D view for display
		// 이 객체는 3D 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an 3D view and does not need to be released
		CGUIView3DLayerWrap layerView3D = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear figures drawn on existing layer
		layerViewXYZV.Clear();
		layerViewTexture.Clear();
		layerView3D.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerViewXYZV.DrawTextCanvas(&CFLPoint<double>(0, 0), L"XYZV Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerViewTexture.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Texture Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerView3D.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Point Cloud", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		viewSrcXYZVImage.Invalidate(true);
		viewSrcTextureImage.Invalidate(true);

		// 3D 뷰를 갱신 // Update 3D view
		view3DDst.Invalidate(true);

		// 뷰가 닫히기 전까지 종료하지 않고 대기 // Wait until a view is closed before exiting
		while(viewSrcXYZVImage.IsAvailable() && viewSrcTextureImage.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}