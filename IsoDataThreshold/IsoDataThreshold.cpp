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
	CGUIViewImageWrap viewImage[2];

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = fliISrcImage.Load(L"../../ExampleImages/Threshold/Circuit.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage[0].Create(300, 0, 300 + 384, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[1].Create(300 + 384, 0, 300 + 384 * 2, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImage[0].SetImagePtr(&fliISrcImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[1].SetImagePtr(&fliIDstImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[1])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[1])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		// IsoDataThreshold 객체 생성 // Create IsoDataThreshold object
		CIsoDataThreshold isoDataThreshold;

		// Source 이미지 설정 // Set source image 
		isoDataThreshold.SetSourceImage(fliISrcImage);

		// Destination 이미지 설정 // Set destination image
		isoDataThreshold.SetDestinationImage(fliIDstImage);

		// MultiVar 객체 생성 // Create MultiVar object
		CMultiVarL mvCondition;
		CMultiVar<double> mvThreshold;

		// 1채널 논리조건 입력 // 1Channel condition push
		mvCondition.PushBack(ELogicalCondition_Greater);

		// 논리 조건 설정 // Set condition value
		isoDataThreshold.SetLogicalCondition(mvCondition);

		// 계산된 Threshold 값을 추출 // get result threshold value
		mvThreshold = isoDataThreshold.GetResultThreshold();

		// Console창에 Threshold 값 출력 // Output the threshold value to the console window
		printf("Result Threshold Value : %d", (int)mvThreshold[0]);

		// 알고리즘 수행 // Execute the algorithm
		if((res = isoDataThreshold.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute IsoData Threshold.");
			break;
		}

		// Threshold 값 문자열 생성 // Create threshold value String
		Base::CFLString<wchar_t> flStrThresholdValue;
		flStrThresholdValue.Format(L"%d < threshold", (int)mvThreshold[0]);

		// 레이어는 따로 해제하지 않아도 View가 해제 될 때 같이 해제된다. // The layer is released together when View is released without releasing it separately.
		CGUIViewImageLayerWrap layer = viewImage[1].GetLayer(0);

		if(IsFail(res = layer.DrawTextCanvas(CFLPoint<double>(0, 0), flStrThresholdValue.GetBuffer(), YELLOW, BLACK, 30, false, 0, EGUIViewImageTextAlignment_LEFT_TOP, L"", 1.f, 1.f, EGUIViewImageFontWeight_BOLD, false)))
		{
			ErrorPrint(res, "Failed to draw text on the image view.\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImage[1].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage[0].IsAvailable() && viewImage[1].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}