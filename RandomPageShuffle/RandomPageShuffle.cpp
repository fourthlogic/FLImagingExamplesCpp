#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliAfterSrcImage;
	CFLImage fliBeforeSrcImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageAfterSrc;
	CGUIViewImageWrap viewImageBeforeSrc;

	do
	{
		CResult res = EResult_UnknownError;

		// 알고리즘을 수행할 Source 이미지 로드 // Load the source image to execute algorithm
		if(IsFail(res = fliAfterSrcImage.Load(L"../../ExampleImages/RandomPageShuffle/Landscape.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 알고리즘 수행 결과와 비교할 Source 이미지 로드 // Load the source image to compare executing result of algorithm
		if(IsFail(res = fliBeforeSrcImage.Load(L"../../ExampleImages/RandomPageShuffle/Landscape.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 알고리즘을 수행 할 Source 이미지 뷰 생성 // Create the After Source image view to execute algorithm
		if(IsFail(res = viewImageAfterSrc.Create(912, 0, 1424, 612)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 알고리즘을 수행 할 Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view to execute algorithm
		if(IsFail(res = viewImageAfterSrc.SetImagePtr(&fliAfterSrcImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 알고리즘 수행 결과와 비교할 Source 이미지 뷰 생성 // Create the source image view to compare executing result of algorithm
		if(IsFail(res = viewImageBeforeSrc.Create(400, 0, 912, 612)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 알고리즘 수행 결과와 비교할 Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view to compare executing result of algorithm
		if(IsFail(res = viewImageBeforeSrc.SetImagePtr(&fliBeforeSrcImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageAfterSrc.SynchronizeWindow(&viewImageBeforeSrc)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImageAfterSrc.SynchronizePointOfView(&viewImageBeforeSrc)))
		{
			ErrorPrint(res, "Failed to synchronize view.\n");
			break;
		}

		// 두 이미지 뷰의 페이지 색인을 동기화 한다 // Synchronize the page index of the two image views
		if(IsFail(res = viewImageAfterSrc.SynchronizePageIndex(&viewImageBeforeSrc)))
		{
			ErrorPrint(res, "Failed to synchronize view.\n");
			break;
		}

		// Random Page Shuffle 객체 생성 // Create Random Page Shuffle object
		CRandomPageShuffle randomPageShuffle;

		// Source 이미지 설정 // Set the source image
		randomPageShuffle.SetSourceImage(fliAfterSrcImage);

		// 랜덤하게 섞을 페이지의 시작 인덱스와 페이지 개수 설정  // Set the start page index and page counts to shuffle pages
		randomPageShuffle.SetSelection(0, 5);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = randomPageShuffle.Execute()))
		{
			ErrorPrint(res, "Failed to execute Random Page Shuffle.");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if(IsFail(res = viewImageBeforeSrc.ZoomFit()))
		{
			ErrorPrint(res, "Failed to zoom fit of the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerAfterSrc = viewImageAfterSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerBeforeSrc = viewImageBeforeSrc.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerAfterSrc.Clear();
		layerBeforeSrc.Clear();

		CFLPoint<double> flpDrawingPoint(0, 0);

		// View 정보를 디스플레이 한다. // Display view information
			// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
			// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
			// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
			//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
			// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
			//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerAfterSrc.DrawTextCanvas(&flpDrawingPoint, L"Source Image After Shuffle", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerBeforeSrc.DrawTextCanvas(&flpDrawingPoint, L"Source Image Before Shuffle", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageAfterSrc.Invalidate(true);
		viewImageBeforeSrc.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageAfterSrc.IsAvailable() && viewImageBeforeSrc.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}