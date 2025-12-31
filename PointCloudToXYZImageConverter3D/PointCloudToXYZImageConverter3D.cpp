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
	CFLImage fliDstXYZVImage;
	CFLImage fliDstTextureImage;

    // 3D 객체 선언 // Declare 3D object
	CFL3DObject floSource;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewDstXYZVImage;
	CGUIViewImageWrap viewDstTextureImage;

	// 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap view3DSrc;

	do
	{
		// 수행 결과 객체 선언 // Declare execution result object
		CResult res = EResult_UnknownError;

		// Source Point Cloud 로드 // Load Source Point Cloud
		if((res = floSource.Load(L"../../ExampleImages/PointCloudToXYZImageConverter3D/3DSrc.ply")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the point cloud.\n");
			break;
		}

		// Source 3D 뷰 생성 // Create Source 3D view
		if((res = view3DSrc.Create(100, 0, 612, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the 3D view.\n");
			break;
		}

		// Destination XYZV 이미지 뷰 생성 // Create Destination XYZV image view
		if((res = viewDstXYZVImage.Create(100, 512, 612, 1024)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination Texture 이미지 뷰 생성 // Create Destination Texture image view
		if((res = viewDstTextureImage.Create(612, 512, 1124, 1024)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination XYZV 이미지 뷰에 이미지를 디스플레이 // Display image in Destination XYZV image view
		if((res = viewDstXYZVImage.SetImagePtr(&fliDstXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination Texture 이미지 뷰에 이미지를 디스플레이 // Display image in Destination Texture image view
		if((res = viewDstTextureImage.SetImagePtr(&fliDstTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 // Synchronize viewpoints of two image views
		if((res = viewDstXYZVImage.SynchronizePointOfView(&viewDstTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize point of view between image views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = view3DSrc.SynchronizeWindow(&viewDstXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = view3DSrc.SynchronizeWindow(&viewDstTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// Point Cloud To XYZ Image Converter 3D 객체 생성 // Create Point Cloud To XYZ Image Converter 3D object
		CPointCloudToXYZImageConverter3D pointCloudToXYZImageConverter3D;

		// Source Point Cloud 설정 // Set Source Point Cloud.
		if((res = pointCloudToXYZImageConverter3D.SetSourceObject(floSource)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Source 3D object.\n");
			break;
		}

		// Destination XYZV 이미지 설정 // Set Destination XYZV image
		if((res = pointCloudToXYZImageConverter3D.SetDestinationImage(fliDstXYZVImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination XYZV image.\n");
			break;
		}

		// Destination Texture 이미지 설정 // Set Destination Texture image.
		if((res = pointCloudToXYZImageConverter3D.SetDestinationTextureImage(fliDstTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination Texture image.\n");
			break;
		}

		// Destination 이미지 크기 설정 // Set size of destination image
		if((res = pointCloudToXYZImageConverter3D.SetImageSize(140, 200)).IsFail())
		{
			ErrorPrint(res, L"Failed to set destination image size.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = pointCloudToXYZImageConverter3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Point Cloud To XYZ Image Converter 3D.\n");
			break;
		}


		// 입력 3D 객체 출력 // Print source 3D Object
		if((res = view3DSrc.PushObject(floSource)).IsFail())
		{
			ErrorPrint(res, L"Failed to display the 3D Object.\n");
			break;
		}

		// 3D View 카메라 설정 // Set 3D view camera
		CFL3DCamera fl3DCam;

		fl3DCam.SetDirection(CFLPoint3<float>(0, 0, -1));
		fl3DCam.SetDirectionUp(CFLPoint3<float>(0, 1, 0));
		fl3DCam.SetPosition(CFLPoint3<float>(10, -20, 750));

		view3DSrc.SetCamera(fl3DCam);

		// 화면에 출력하기 위해 이미지 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released
		CGUIViewImageLayerWrap layerViewXYZV = viewDstXYZVImage.GetLayer(0);
		CGUIViewImageLayerWrap layerViewTexture = viewDstTextureImage.GetLayer(0);

		// 화면에 출력하기 위해 3D 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from 3D view for display
		// 이 객체는 3D 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an 3D view and does not need to be released
		CGUIView3DLayerWrap layerView3D = view3DSrc.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear figures drawn on existing layer
		layerView3D.Clear();
		layerViewXYZV.Clear();
		layerViewTexture.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerViewXYZV.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination XYZV Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerViewTexture.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Texture Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 3D 뷰 정보 표시 // Display 3D view information
		if((res = layerView3D.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Point Cloud", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}


		// 새로 생성한 이미지를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created image
		if((res = viewDstXYZVImage.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit image view.\n");
			break;
		}

		// 새로 생성한 이미지를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created image
		if((res = viewDstTextureImage.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit image view.\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		viewDstXYZVImage.Invalidate(true);
		viewDstTextureImage.Invalidate(true);

		// 3D 뷰를 갱신 // Update 3D view
		view3DSrc.Invalidate(true);

		// 뷰가 닫히기 전까지 종료하지 않고 대기 // Wait until a view is closed before exiting
		while(viewDstXYZVImage.IsAvailable() && viewDstTextureImage.IsAvailable() && view3DSrc.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}