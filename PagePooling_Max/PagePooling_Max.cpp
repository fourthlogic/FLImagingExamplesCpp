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
	CFLImage fliIndexImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSource;
	CGUIViewImageWrap viewImageDestination;
	CGUIViewImageWrap viewImageIndex;

	// 알고리즘 동작 결과 // Algorithm execution result
	CResult res = EResult_UnknownError;

	do
	{
		// 이미지 로드 // Load image
		if((res = fliSourceImage.Load(L"../../ExampleImages/PagePooling/Multiple File_Max.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if((res = viewImageSource.Create(200, 0, 712, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create destination image view
		if((res = viewImageDestination.Create(712, 0, 1224, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Index 이미지 뷰 생성
		if((res = viewImageIndex.Create(1224, 0, 1736, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((res = viewImageSource.SynchronizePointOfView(&viewImageDestination)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((res = viewImageDestination.SynchronizePointOfView(&viewImageIndex)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if((res = viewImageSource.SynchronizeWindow(&viewImageDestination)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if((res = viewImageDestination.SynchronizeWindow(&viewImageIndex)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if((res = viewImageSource.SetImagePtr(&fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if((res = viewImageDestination.SetImagePtr(&fliDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Index 이미지 뷰에 이미지를 디스플레이 // Display the image in the Index image view
		if((res = viewImageIndex.SetImagePtr(&fliIndexImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		viewImageSource.SetFixThumbnailView(true);

		// Page Pooling 객체 생성 // Create Page Pooling object
		CPagePooling pagePooling;

		// Source 이미지 설정 // Set the source image
		pagePooling.SetSourceImage(fliSourceImage);

		// Destination 이미지 설정 // Set the destination image
		pagePooling.SetDestinationImage(fliDestinationImage);

		// Index 이미지 // Index image
		//	- 각 픽셀별로 추출한 결과 값이 위치한 페이지 인덱스 값을 대응되는 좌표의 픽셀로 출력합니다. // For each pixel, output the page index value where the resulting value is located to the pixel of the corresponding coordinates.
		//	- Sampling 메소드가 Min Gaussian, Max Gaussian 모드인 경우 출력되는 인덱스 이미지는 각각 Min, Max 모드로 동작할 때 출력되는 인덱스 이미지와 동일합니다. // If the sampling method is in Min Gaussian and Max Gaussian modes, the output index image is the same as the output index image when operating in Min and Max modes, respectively.
		//	- Sampling 메소드가 Mean 모드인 경우는 인덱스 이미지 출력을 지원하지 않습니다. // Index image output is not supported when the Sampling method is in Mean mode.
		//	- 추출할 결과 값이 여러 페이지에 동일하게 존재할 경우, 가장 앞의 인덱스를 출력합니다. // Outputs the leading index if the resulting values to be extracted are equally present on multiple pages.
		//	- SetIndexImage 는 SetSourceImage 나 SetDestinationImage 에서 설정한 이미지와 동일하면 동작하지 않습니다. // - SetIndexImage will not work if it is the same as the image set in SetSourceImage or SetDestinationImage.
		//	- Index Image 를 지정하지 않을 경우 인덱스 이미지를 출력하지 않는 모드로 동작합니다. // If SetIndexImage is not specified, it operates in a mode that does not output the index image.
		//	- 인덱스 이미지 추출은 최대 65535 장 까지만 지원됩니다. // Index image extraction is supported up to 65535 pages
		//	- Source ROI 영역 밖에 해당하는 인덱스는 무효 값으로 8bit 인덱스 이미지에서는 255, 16bit 인덱스 이미지에서는 65535 가 입력됩니다. // Indexes outside the Source ROI area are invalid values, with 255 for an 8-bit index image and 65535 for a 16-bit index image.

		// Index 이미지 설정
		pagePooling.SetIndexImage(fliIndexImage);

		// Sampling 메소드 설정 // Set the sampling method
		//	- Max : 입력된 이미지 가운데 최대 값을 출력합니다. // Max : Outputs the maximum value of the entered image.
		//	- MaxGaussian : 입력된 이미지 가운데 가장 앞 쪽 인덱스에 위치한 최대 값을 기준으로 가우시안 값을 출력합니다. // MaxGaussian : Outputs the Gaussian value based on the maximum value located in the leading index of the entered image.
		//	- Min : 입력된 이미지 가운데 최소 값을 출력합니다. // Min : Outputs the minimum value of the entered image.
		//	- MinGaussian : 입력된 이미지 가운데 가장 앞 쪽 인덱스에 위치한 최소 값을 기준으로 가우시안 값을 출력합니다. // MinGaussian : Outputs the Gaussian value based on the minimum value located in the leading index of the entered image.
		//	- Mean : 입력된 이미지들의 평균 값을 출력합니다. (최대 16843009 장 까지 지원됩니다.) // Mean: Outputs the average value of the entered images. (Up to 16843009 pages are supported.)
		pagePooling.SetSamplingMethod(CPagePooling::ESamplingMethod_Max);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = pagePooling.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Page Pooling.\n");
			
			printf("\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination = viewImageDestination.GetLayer(0);
		CGUIViewImageLayerWrap layerIndex = viewImageIndex.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSource.Clear();
		layerDestination.Clear();
		layerIndex.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if((res = layerSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerDestination.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerIndex.DrawTextCanvas(CFLPoint<double>(0, 0), L"Index Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSource.Invalidate(true);
		viewImageDestination.Invalidate(true);
		viewImageIndex.Invalidate(true);

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageDestination.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit\n");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageIndex.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit\n");
			break;
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSource.IsAvailable() && viewImageDestination.IsAvailable()&& viewImageIndex.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}