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
	CFLImage fliIndexImage;
	CFLImage fliDestinationImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSource;
	CGUIViewImageWrap viewImageIndex;
	CGUIViewImageWrap viewImageDestination;

	// 알고리즘 동작 결과 // Algorithm execution result
	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res = fliSourceImage.Load(L"../../ExampleImages/PagePixelPicker/MultiFile_Normal.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Index 이미지 로드
		if((res = fliIndexImage.Load(L"../../ExampleImages/PagePixelPicker/IndexMap.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if((res = viewImageSource.Create(400, 0, 900, 500)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Index 이미지 뷰 생성
		if((res = viewImageIndex.Create(900, 0, 1400, 500)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create destination image view
		if((res = viewImageDestination.Create(1400, 0, 1900, 500)).IsFail())
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

		// Page Pixel Picker 객체 생성 // Create Page Pixel Picker object
		CPagePixelPicker pagePixelPicker;

		// Source 이미지 설정 // Set the source image
		// 모든 Source 이미지는 동일한 사이즈와 포맷을 가져야합니다. // All Source images must have the same size and format.
		pagePixelPicker.SetSourceImage(fliSourceImage);

		// Index 이미지 설정 // Set the index image
		// 8bit 와 16 bit 를 지원하며 반드시 입력되어야 합니다. // 8 bit and 16 bit are supported and must be entered.
		// Index 이미지는 Source 이미지나 Destination 이미지와 다르게 설정해야 합니다. // The Index image must be set differently from the source image or destination image.
		// Index 이미지의 각 픽셀 값은 입력된 Source 이미지의 페이지 인덱스를 의미합니다. // Each pixel value of the index image means a page index of the input Source image.
		pagePixelPicker.SetIndexImage(fliIndexImage);

		// Destination 이미지 설정 // Set the destination image
		pagePixelPicker.SetDestinationImage(fliDestinationImage);

		// Source ROI, Source Pivot, Destination ROI, Destination Pivot 기능을 지원합니다. // Source ROI, Source Pivot, Destination ROI and Destination Pivot are supported
		// ROI 및 Pivot 은 Image Operation Scalar 연산과 동일한 동작을 수행합니다. // ROI and Pivot perform the same behavior as the Image Operation Scalar operation.
		 
		// Blank Color 설정 // Set blank color
		// 기본적으로 Image Operation 의 Blank Color 와 동일한 동작을 합니다. // By default, it behaves the same as the Blank Color in Image Operation.
		// Index Image 픽셀 값에 해당되는 페이지 인덱스가 존재하지 않을 경우 Blank color 로 채워집니다. // If the page index corresponding to the index image pixel value does not exist, it is populated with Blank color.
		pagePixelPicker.SetBlankColor(200);

		//EnableFillBlankColorMode 기능을 지원하며, Image Operation Scalar 연산과 동일한 동작을 수행합니다. // It supports the EnableFillBlankColorMode function and performs the same behavior as the Image Operation Scalar operation.
		//pagePixelPicker.EnableFillBlankColorMode(true);
		
		// Sampling 메소드 설정 // Set sampling method
		//	- ESamplingMethod_Normal : Index 이미지 각 픽셀과 좌표에 대응되는 Source 이미지 픽셀 색상 값을 출력합니다. // - ESamplingMethod_Normal : Index Image Outputs the source image pixel color value corresponding to each pixel and coordinate.
		//	- ESamplingMethod_Gaussian : Index 이미지 각 픽셀과 좌표에 대응되는 Source 이미지와 전 후 페이지 색상 값으로 가우시안 값을 계산하여 출력합니다. // - ESamplingMethod_Gaussian : Index image calculates and outputs Gaussian values with the source image corresponding to each pixel and coordinates and the front and rear page color values.
		pagePixelPicker.SetSamplingMethod(CPagePixelPicker::ESamplingMethod_Normal);
		
		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = pagePixelPicker.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Page Pixel Picker.\n");
			
			printf("\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);
		CGUIViewImageLayerWrap layerIndex = viewImageIndex.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination = viewImageDestination.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSource.Clear();
		layerIndex.Clear();
		layerDestination.Clear();

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

		if((res = layerIndex.DrawTextCanvas(CFLPoint<double>(0, 0), L"Index Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerDestination.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSource.Invalidate(true);
		viewImageIndex.Invalidate(true);
		viewImageDestination.Invalidate(true);

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageIndex.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit\n");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageDestination.ZoomFit()).IsFail())
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