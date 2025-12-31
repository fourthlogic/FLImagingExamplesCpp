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
	CFLImage fliDestinationImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewSourceImage;
	CGUIViewImageWrap viewDestinationImage;

	// 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap viewDestination3D;

	do
	{
		// 수행 결과 객체 선언 // Declare execution result object
		CResult res = EResult_UnknownError;

		// Source 이미지 로드 // Load Source image
		if((res = fliSourceImage.Load(L"../../ExampleImages/LaserTriangulation3D/SrcImage.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if((res = viewSourceImage.Create(100, 0, 600, 448)).IsFail())
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

		// Destination 이미지 뷰 생성 // Create Destination image view
		if((res = viewDestinationImage.Create(600, 0, 1100, 448)).IsFail())
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
		if((res = viewDestination3D.Create(100, 448, 1100, 896)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the 3D view.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewSourceImage.SynchronizeWindow(&viewDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewSourceImage.SynchronizeWindow(&viewDestination3D)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// Laser Triangulation 3D 객체 생성 // Create Laser Triangulation 3D object
		CLaserTriangulation3D laserTriangulation3D;

		// 출력에 사용되는 3D Height Map 객채 생성 // Create 3D height map used as output
		CFL3DObjectHeightMap fl3DOHM;

		// 레이저의 Baseline 생성 // Set base line of laser
		CFLLine<double> fllBaseLine = CFLLine<double>(0, 61, 1216, 61);

		// Source 이미지 설정 // Set Source image
		if((res = laserTriangulation3D.SetSourceImage(fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Source image.\n");
			break;
		}

		// Destination Height Map 이미지 설정 // Set Destination Height Map image
		if((res = laserTriangulation3D.SetDestinationHeightMapImage(fliDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination Height Map image.\n");
			break;
		}

		// Destination 3D Object 설정 // Set Destination 3D Object 
		if((res = laserTriangulation3D.SetDestinationObject(fl3DOHM)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination 3D Object.\n");
			break;
		}

		// 레이저의 Base Line 설정 // Set base line of laser
		if((res = laserTriangulation3D.SetBaseLine(&fllBaseLine)).IsFail())
		{
			ErrorPrint(res, L"Failed to set base line of laser.\n");
			break;
		}

		// Source 이미지 타입 설정 // Set type of Source image
		if((res = laserTriangulation3D.SetSourceType(CLaserTriangulation3D::ESourceType_Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to set source image type.\n");
			break;
		}

		// Pixel Accuracy 설정 // Set pixel accuracy
		if((res = laserTriangulation3D.SetPixelAccuracy(0.165)).IsFail())
		{
			ErrorPrint(res, L"Failed to set pixel accuracy.\n");
			break;
		}

		// Scan Accuracy 설정 // Set scan accuracy
		if((res = laserTriangulation3D.SetScanAccuracy(0.2)).IsFail())
		{
			ErrorPrint(res, L"Failed to set scan accuracy.\n");
			break;
		}

		// Working Distance 설정 // Set working distance
		if((res = laserTriangulation3D.SetWorkingDistance(214.7)).IsFail())
		{
			ErrorPrint(res, L"Failed to set working distance.\n");
			break;
		}

		// 레이저 각도 설정 // Set angle of laser
		if((res = laserTriangulation3D.SetAngleOfLaser(60)).IsFail())
		{
			ErrorPrint(res, L"Failed to set angle of laser.\n");
			break;
		}

		// 레이저 밝기 설정 // Set laser brightness threshold
		if((res = laserTriangulation3D.SetLaserThreshold(64)).IsFail())
		{
			ErrorPrint(res, L"Failed to set laser brightness threshold.\n");
			break;
		}

		// 평균 Window의 픽셀 길이 설정 // Set average window pixel length
		if((res = laserTriangulation3D.SetAverageWindowLength(5)).IsFail())
		{
			ErrorPrint(res, L"Failed to set average window length.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = laserTriangulation3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Laser Triangulation 3D.\n");
			break;
		}

		// 결과 3D 객체 출력 // Print resulting 3D Object
		if((res = viewDestination3D.PushObject(fl3DOHM)).IsFail())
		{
			ErrorPrint(res, L"Failed to display the 3D Object.\n");
			break;
		}

		// 화면에 출력하기 위해 이미지 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released
		CGUIViewImageLayerWrap layerImageSource = viewSourceImage.GetLayer(0);
		CGUIViewImageLayerWrap layerImageDestination = viewDestinationImage.GetLayer(0);

		// 화면에 출력하기 위해 3D 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from 3D view for display
		// 이 객체는 3D 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an 3D view and does not need to be released
		CGUIView3DLayerWrap layer3DDestination = viewDestination3D.GetLayer(0);
		
		// 기존에 Layer에 그려진 도형들을 삭제 // Clear figures drawn on existing layer
		layerImageSource.Clear();
		layerImageDestination.Clear();
		layer3DDestination.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerImageSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerImageDestination.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 3D 뷰 정보 표시 // Display 3D view information
		if((res = layer3DDestination.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination 3D Height Map", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 새로 생성한 이미지를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created image
		if((res = viewDestinationImage.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit image view.\n");
			break;
		}

		// 새로 생성한 3D Object를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created 3D object
		if((res = viewDestination3D.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit 3D view.\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		viewSourceImage.Invalidate(true);
		viewDestinationImage.Invalidate(true);

		// 3D 뷰를 갱신 // Update 3D view
		viewDestination3D.Invalidate(true);

		// 뷰가 닫히기 전까지 종료하지 않고 대기 // Wait until a view is closed before exiting
		while(viewSourceImage.IsAvailable() && viewDestinationImage.IsAvailable() && viewDestination3D.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}