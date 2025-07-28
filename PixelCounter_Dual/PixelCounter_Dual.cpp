#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliISrcImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImage1;

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image 
		if(IsFail(res = fliISrcImage.Load(L"../../ExampleImages/PixelCounter/Semiconductor.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage1.Create(300, 0, 300 + 512, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImage1.SetImagePtr(&fliISrcImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Pixel Counter 객체 생성 // Create Pixel Counter object
		CPixelCounter pixelCounter;

		// Source 이미지 설정 // Set source image 
		pixelCounter.SetSourceImage(fliISrcImage);

		// Source ROI 설정 // Set source ROI 
		CFLQuad<double> flfSourceROI(170.550171, 102.400000, 380.243003, 135.950853, 341.100341, 312.092833, 124.417747, 265.960410);
		pixelCounter.SetSourceROI(flfSourceROI);

		// PixelCounter 모드 설정(Dual) // Set Threshold Mode(Dual)
		pixelCounter.SetThresholdMode(EThresholdMode_Dual_And);

		// MultiVar 객체 생성 // Create MultiVar object
		CMultiVar<double> mvThresholdValue1, mvThresholdValue2;
		CMultiVarL mvCondition1, mvCondition2;

		// 임계값 입력 // PixelCounter value push
		mvThresholdValue1.PushBack(120);
		mvThresholdValue2.PushBack(230);

		// 임계값 설정 (다채널 경우 CMultiVar<double> 사용) // Set PixelCounter value(Use CMultiVar<double> for multi-channel)
		pixelCounter.SetThreshold(mvThresholdValue1, mvThresholdValue2);

		// 1채널 논리조건 입력 // 1Channel condition push
		mvCondition1.PushBack(ELogicalCondition_Greater);
		mvCondition2.PushBack(ELogicalCondition_Less);

		// 논리 조건 설정 // Set condition value
		pixelCounter.SetLogicalCondition(mvCondition1, mvCondition2);

		// 알고리즘 수행 // Execute the algorithm
		if((res = pixelCounter.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Pixel Counter.");
			break;
		}

		int64_t i64TotalPixel = pixelCounter.GetResultTotalPixelCount();
		int64_t i64ValidPixel = pixelCounter.GetResultValidPixelCount();
		int64_t i64InvalidPixel = pixelCounter.GetResultInvalidPixelCount();

		// 전체 픽셀, 유효한 픽셀, 유효하지 않은 픽셀 갯수 출력 // display Total, Valid, Invalid Pixel Count
		{
			printf("Total Pixel Count : %lld\n", i64TotalPixel);
			printf("Valid Pixel Count : %lld\n", i64ValidPixel);
			printf("Invalid Pixel Count : %lld\n", i64InvalidPixel);
		}

		// Text 출력 // Draw Text 
		CFLString<wchar_t> flsDrawText;
		flsDrawText.Format(L"Source Image\n120 < threshold < 230\nTotal Pixel Count: %lld\nValid Pixel Count: %lld\nInvalid Pixel Count: %lld", i64TotalPixel, i64ValidPixel, i64InvalidPixel);

		// 레이어는 따로 해제하지 않아도 View가 해제 될 때 같이 해제된다. // The layer is released together when View is released without releasing it separately.
		CGUIViewImageLayerWrap layer = viewImage1.GetLayer(0);

		// PixelCounter 값을 디스플레이 합니다. // Display PixelCounter value.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layer.DrawTextCanvas(&CFLPoint<double>(0, 0), flsDrawText, YELLOW, BLACK, 15, false, 0, EGUIViewImageTextAlignment_LEFT_TOP, L"", 1.f, 1.f, EGUIViewImageFontWeight_BOLD, false)))
		{
			ErrorPrint(res, "Failed to draw text on the image view.\n");
			break;
		}

		// Source ROI 그리기 // Draw Source ROI
		if(IsFail(res = layer.DrawFigureImage(flfSourceROI, LIME)))
		{
			ErrorPrint(res, "Failed to draw Source ROI on the image view.\n");
			break;
		}

		viewImage1.Invalidate(false);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage1.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}