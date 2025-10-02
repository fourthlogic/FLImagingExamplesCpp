#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSourceImage;
	CFLImage fliSource2Image;
	CFLImage fliDestinationImage;
	CFLImage fliTextureImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewSourceImage;
	CGUIViewImageWrap viewSource2Image;
	CGUIViewImageWrap viewDestinationImage;

    // 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap view3DDst;

	do
	{
		// 수행 결과 객체 선언 // Declare execution result object
		CResult res = EResult_UnknownError;

		// Source 이미지 로드 // Load Source image
		if((res = fliSourceImage.Load(L"../../ExampleImages/StereoDisparity3D/Left.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 2 이미지 로드 // Load Source 2 image
		if((res = fliSource2Image.Load(L"../../ExampleImages/StereoDisparity3D/Right.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if((res = viewSourceImage.Create(100, 0, 548, 448)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display image in Source image view
		if((res = viewSourceImage.SetImagePtr(&fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Source 2 이미지 뷰 생성 // Create Source 2 image view
		if((res = viewSource2Image.Create(548, 0, 996, 448)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 2 이미지 뷰에 이미지를 디스플레이 // Display image in Source 2 image view
		if((res = viewSource2Image.SetImagePtr(&fliSource2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create Destination image view
		if((res = viewDestinationImage.Create(100, 448, 548, 896)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display image in Destination image view
		if((res = viewDestinationImage.SetImagePtr(&fliDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 3D 뷰 생성 // Create Destination 3D view
		if((res = view3DDst.Create(548, 448, 996, 896)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewSourceImage.SynchronizeWindow(&viewSource2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewSourceImage.SynchronizeWindow(&viewDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewSourceImage.SynchronizeWindow(&view3DDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}


		// Stereo Disparity 3D 객체 생성 // Create Stereo Disparity 3D object
		CStereoDisparity3D stereoDisparity3D;

		// 출력에 사용되는 3D Height Map 객채 생성 // Create 3D height map used as output
		CFL3DObjectHeightMap fl3DOHM;

		// Source 이미지 설정 // Set Source image
		if((res = stereoDisparity3D.SetSourceImage(fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Source image.\n");
			break;
		}

		// Source 2 이미지 설정 // Set Source 2 image
		if((res = stereoDisparity3D.SetSourceImage2(fliSource2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Source 2 image.\n");
			break;
		}

		// Destination Height Map 이미지 설정 // Set Destination Height Map image
		if((res = stereoDisparity3D.SetDestinationHeightMapImage(fliDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination Height Map image.\n");
			break;
		}

		// Destination Texture 이미지 설정 // Set Destination Texture image
		if((res = stereoDisparity3D.SetDestinationTextureImage(&fliTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination Texture image.\n");
			break;
		}

		// Destination 3D Object 설정 // Set Destination 3D Object 
		if((res = stereoDisparity3D.SetDestinationObject(fl3DOHM)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination 3D Object.\n");
			break;
		}

		// 최소 허용 Disparity 값 설정 // Set minimum allowed disparity value
		if((res = stereoDisparity3D.SetMinimumDisparity(-20)).IsFail())
		{
			ErrorPrint(res, L"Failed to set the minimum allowed disparity value.\n");
			break;
		}

		// 최대 허용 Disparity 값 설정 // Set maximum allowed disparity value
		if((res = stereoDisparity3D.SetMaximumDisparity(0)).IsFail())
		{
			ErrorPrint(res, L"Failed to set the maximum allowed disparity value.\n");
			break;
		}

		// Matched Block 크기 설정 // Set matched block size
		if((res = stereoDisparity3D.SetMatchBlockSize(3)).IsFail())
		{
			ErrorPrint(res, L"Failed to set the matched block size.\n");
			break;
		}

		// 좌우 간 최대 허용 차이 값 설정 // Set maximum allowed difference value between left and right
		if((res = stereoDisparity3D.SetMaximumDifference(30)).IsFail())
		{
			ErrorPrint(res, L"Failed to set the maximum allowed difference.\n");
			break;
		}

		// 고유비 값 설정 // Set uniqueness ratio value
		if((res = stereoDisparity3D.SetUniquenessRatio(0)).IsFail())
		{
			ErrorPrint(res, L"Failed to set the uniqueness ratio value.\n");
			break;
		}

		// P1 값 설정 // Set P1 Value
		if((res = stereoDisparity3D.SetP1(200)).IsFail())
		{
			ErrorPrint(res, L"Failed to set the P1 Value.\n");
			break;
		}

		// P2 값 설정 // Set P2 Value
		if((res = stereoDisparity3D.SetP2(800)).IsFail())
		{
			ErrorPrint(res, L"Failed to set the P2 Value.\n");
			break;
		}

		// Median Morphology 커널 사이즈 설정 // Set median morphology kernel size
		if((res = stereoDisparity3D.SetFilterSize(5)).IsFail())
		{
			ErrorPrint(res, L"Failed to set the median kernel size.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = stereoDisparity3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Stereo Disparity 3D.\n");
			break;
		}

		// 화면에 출력하기 위해 이미지 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released
		CGUIViewImageLayerWrap layerSrc = viewSourceImage.GetLayer(0);
		CGUIViewImageLayerWrap layerSrc2 = viewSource2Image.GetLayer(0);
		CGUIViewImageLayerWrap layerDst = viewDestinationImage.GetLayer(0);

		// 화면에 출력하기 위해 3D 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from 3D view for display
		// 이 객체는 3D 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an 3D view and does not need to be released
		CGUIView3DLayerWrap layer3DDestination = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerSrc2.Clear();
		layerDst.Clear();
		layer3DDestination.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerSrc.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerSrc2.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image 2", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerDst.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 3D 뷰 정보 표시 // Display 3D view information
		if((res = layer3DDestination.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination 3D Height Map", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 3D Height Map에 Texture 적용 // Apply texture to 3D height map
		if((res = fl3DOHM.SetTextureImage(fliTextureImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to apply texture to height map.\n");
			break;
		}

		res = fl3DOHM.ActivateVertexColorTexture(true);

		// 결과 3D 객체 출력 // Print resulting 3D Object
		if((res = view3DDst.PushObject(fl3DOHM)).IsFail())
		{
			ErrorPrint(res, L"Failed to display the 3D Object.\n");
			break;
		}

		// 새로 생성한 이미지를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created image
		if((res = viewDestinationImage.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit image view.\n");
			break;
		}

		// 새로 생성한 3D Object를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created 3D object
		if((res = view3DDst.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit 3D view.\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewSourceImage.Invalidate(true);
		viewSource2Image.Invalidate(true);
		viewDestinationImage.Invalidate(true);

		// 3D 뷰를 갱신 // Update 3D view
		view3DDst.Invalidate(true);

		// 뷰가 닫히기 전까지 종료하지 않고 대기 // Wait until a view is closed before exiting
		while(viewSourceImage.IsAvailable() && viewSource2Image.IsAvailable() && viewDestinationImage.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}