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
	CFLImage fliIDstImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImage1;
	CGUIViewImageWrap viewImage2;

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = fliISrcImage.Load(L"../../ExampleImages/Threshold/Mountain.flif")))
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

		if(IsFail(res = viewImage2.Create(300 + 512, 0, 300 + 512 * 2, 384)))
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

		if(IsFail(res = viewImage2.SetImagePtr(&fliIDstImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if(IsFail(res = viewImage1.SynchronizePointOfView(&viewImage2)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImage1.SynchronizeWindow(&viewImage2)))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		// Absolute Threshold 객체 생성 // Create Absolute Threshold object
		CAbsoluteThreshold absoluteThreshold;

		// Source 이미지 설정 // Set source image 
		absoluteThreshold.SetSourceImage(fliISrcImage);

		// Destination 이미지 설정 // Set destination image
		absoluteThreshold.SetDestinationImage(fliIDstImage);

		// threshold 모드 설정(Single) // Set threshold mode(Single)
		absoluteThreshold.SetThresholdMode(EThresholdMode_Single);

		// MultiVar 객체 생성 // Create MultiVar object
		CMultiVar<double> mvThresholdValue;
		CMultiVarL mvCondition;

		// 임계값 입력 // threshold value push
		mvThresholdValue.PushBack(100);

		// 임계값 설정 (다채널 경우 CMultiVar<double> 사용) // Set threshold value(Use CMultiVar<double> for multi-channel)
		absoluteThreshold.SetThreshold(mvThresholdValue);

		// 1채널 논리조건 입력 // 1Channel condition push
		mvCondition.PushBack(ELogicalCondition_Greater);

		// 논리 조건 설정 // Set condition value
		absoluteThreshold.SetLogicalCondition(mvCondition);

		// 알고리즘 수행 // Execute the algorithm
		if((res = absoluteThreshold.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Absolute Threshold.");
			break;
		}

		// 레이어는 따로 해제하지 않아도 View가 해제 될 때 같이 해제된다. // The layer is released together when View is released without releasing it separately.
		CGUIViewImageLayerWrap layer = viewImage2.GetLayer(0);

		// threshold 값을 디스플레이 합니다. // Display threshold value.
		// 아래 함수 DrawTextCanvas은 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layer.DrawTextCanvas(&CFLPoint<double>(0, 0), L"100 < threshold", YELLOW, BLACK, 30, false, 0, EGUIViewImageTextAlignment_LEFT_TOP, L"", 1.f, 1.f, EGUIViewImageFontWeight_BOLD, false)))
		{
			ErrorPrint(res, "Failed to draw text on the image view.\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImage2.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage1.IsAvailable() && viewImage2.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}