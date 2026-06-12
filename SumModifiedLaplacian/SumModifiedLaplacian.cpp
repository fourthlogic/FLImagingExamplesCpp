#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSrcImage1;
	CFLImage fliSrcImage2;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc1;
	CGUIViewImageWrap viewImageSrc2;

	do
	{
		CResult res = EResult_UnknownError;
		// Source 이미지 로드 // Load the source image
		if(IsFail(res = fliSrcImage1.Load(L"../../ExampleImages/FocusMeasurement/Focus1.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliSrcImage2.Load(L"../../ExampleImages/FocusMeasurement/Focus2.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create the source image view
		if(IsFail(res = viewImageSrc1.Create(400, 0, 912, 612)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageSrc2.Create(912, 0, 1424, 612)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if(IsFail(res = viewImageSrc1.SetImagePtr(&fliSrcImage1)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageSrc2.SetImagePtr(&fliSrcImage2)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImageSrc1.SynchronizePointOfView(&viewImageSrc2)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSrc1.SynchronizeWindow(&viewImageSrc2)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Sum Modified Laplacian 객체 생성 // Create Sum Modified Laplacian object
		CSumModifiedLaplacian sumModifiedLaplacian;

		// Source 이미지 1 설정 // Set the source1 image
		sumModifiedLaplacian.SetSourceImage(fliSrcImage1);

		//Threshold 설정 //Set Threshold
		sumModifiedLaplacian.SetThreshold(5.0);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = sumModifiedLaplacian.Execute()))
		{
			ErrorPrint(res, "Failed to execute Sum Modified Laplacian.");
			break;
		}

		// 결과 점수 획득 // Get Result Score
		double f64Score1 = sumModifiedLaplacian.GetResultScore();

		// Source 이미지 2 설정 // Set the source2 image
		sumModifiedLaplacian.SetSourceImage(fliSrcImage2);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = sumModifiedLaplacian.Execute()))
		{
			ErrorPrint(res, "Failed to execute Sum Modified Laplacian.");
			break;
		}

		// 결과 점수 획득 // Get Result Score
		double f64Score2 = sumModifiedLaplacian.GetResultScore();

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc1 = viewImageSrc1.GetLayer(0);
		CGUIViewImageLayerWrap layerSrc2 = viewImageSrc2.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc1.Clear();
		layerSrc2.Clear();

		// 점수를 디스플레이 한다. // Display score
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic

		CFLString<wchar_t> flsLayer1 = CFLString<wchar_t>().Format(L"Score : %lf", f64Score1);
		CFLString<wchar_t> flsLayer2 = CFLString<wchar_t>().Format(L"Score : %lf", f64Score2);

		if(IsFail(res = layerSrc1.DrawTextCanvas(&CFLPoint<double>(0, 0), flsLayer1, YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerSrc2.DrawTextCanvas(&CFLPoint<double>(0, 0), flsLayer2, YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc1.Invalidate(true);
		viewImageSrc2.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc1.IsAvailable() && viewImageSrc2.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}