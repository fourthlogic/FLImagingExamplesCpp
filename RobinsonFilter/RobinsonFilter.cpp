#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSrcImage;
	CFLImage fliAmpDstImage;
	CFLImage fliDirDstImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageAmpDst;
	CGUIViewImageWrap viewImageDirDst;

	// 알고리즘 동작 결과 // Algorithm execution result
	CResult result = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((result = fliSrcImage.Load(L"../../ExampleImages/EdgeDetection/Alphabat.flif")).IsFail())
		{
			ErrorPrint(result, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create the source image view
		if((result = viewImageSrc.Create(400, 0, 912, 612)).IsFail())
		{
			ErrorPrint(result, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if((result = viewImageSrc.SetImagePtr(&fliSrcImage)).IsFail())
		{
			ErrorPrint(result, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create the destination image view
		if((result = viewImageAmpDst.Create(912, 0, 1424, 612)).IsFail())
		{
			ErrorPrint(result, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if((result = viewImageAmpDst.SetImagePtr(&fliAmpDstImage)).IsFail())
		{
			ErrorPrint(result, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create the destination image view
		if((result = viewImageDirDst.Create(1424, 0, 1936, 612)).IsFail())
		{
			ErrorPrint(result, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if((result = viewImageDirDst.SetImagePtr(&fliDirDstImage)).IsFail())
		{
			ErrorPrint(result, L"Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((result = viewImageSrc.SynchronizePointOfView(&viewImageAmpDst)).IsFail())
		{
			ErrorPrint(result, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((result = viewImageSrc.SynchronizePointOfView(&viewImageDirDst)).IsFail())
		{
			ErrorPrint(result, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((result = viewImageSrc.SynchronizeWindow(&viewImageAmpDst)).IsFail())
		{
			ErrorPrint(result, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((result = viewImageSrc.SynchronizeWindow(&viewImageDirDst)).IsFail())
		{
			ErrorPrint(result, L"Failed to synchronize window.\n");
			break;
		}

		// CRobinsonFilter 객체 생성 // Create CRobinsonFilter object
		CRobinsonFilter robinsonFilter;

		// Source 이미지 설정 // Set source image
		robinsonFilter.SetSourceImage(fliSrcImage);
		// Destination 이미지 설정 // Set destination image
		robinsonFilter.SetDestinationImage(fliAmpDstImage);
		// 연산 모드 설정 // Set calculating mode
		robinsonFilter.SetRobinsonMode(CRobinsonFilter::ERobinsonMode_AMP);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((result = robinsonFilter.Execute()).IsFail())
		{
			ErrorPrint(result, L"Failed to execute RobinsonFilter.");
			break;
		}

		// Destination 이미지 설정 // Set destination image
		robinsonFilter.SetDestinationImage(fliDirDstImage);
		// 연산 모드 설정 // Set calculating mode
		robinsonFilter.SetRobinsonMode(CRobinsonFilter::ERobinsonMode_DIR);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((result = robinsonFilter.Execute()).IsFail())
		{
			ErrorPrint(result, L"Failed to execute RobinsonFilter.");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((result = viewImageAmpDst.ZoomFit()).IsFail())
		{
			ErrorPrint(result, L"Failed to zoom fit of the image view.\n");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((result = viewImageDirDst.ZoomFit()).IsFail())
		{
			ErrorPrint(result, L"Failed to zoom fit of the image view.\n");
			break;
		}

		if((result = viewImageDirDst.SetFloatingImageValueRange(-180.f, 180.f)).IsFail())
		{
			ErrorPrint(result, L"Failed to set value range of the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerAmpDst = viewImageAmpDst.GetLayer(0);
		CGUIViewImageLayerWrap layerDirDst = viewImageDirDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerAmpDst.Clear();
		layerDirDst.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if((result = layerSrc.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(result, L"Failed to draw text\n");
			break;
		}

		if((result = layerAmpDst.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image(AMP)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(result, L"Failed to draw text\n");
			break;
		}

		if((result = layerDirDst.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image(DIR)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(result, L"Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc.Invalidate(true);
		viewImageAmpDst.Invalidate(true);
		viewImageDirDst.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to exit
		while(viewImageSrc.IsAvailable() && viewImageAmpDst.IsAvailable() && viewImageDirDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}