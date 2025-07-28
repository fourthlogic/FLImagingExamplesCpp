#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res = EResult_UnknownError;

	do
	{
		// 이미지 로드 // Load image
		if((res = fliImage.Load(L"../../ExampleImages/FuzzyEntropy/FuzzyEntropySource.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if((res = viewImage.Create(400, 0, 912, 612)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		if((res = viewImage.SetImagePtr(&fliImage)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.");
			break;
		}

		// Fuzzy Entropy 객체 생성 // Create Fuzzy Entropy object
		CFuzzyEntropy fuzzyEntropy;

		// ROI 범위 설정 // Set the ROI value		
		CFLCircle<double> flrROI(310.466830, 81.769042, 81.769042, 0.000000, 0.000000, 360.000000, EArcClosingMethod_EachOther);
		// Source 이미지 설정 // Set the Source Image
		fuzzyEntropy.SetSourceImage(fliImage);
		// Source ROI 설정 // Set the Source ROI
		fuzzyEntropy.SetSourceROI(flrROI);

		// Parameter 설정(A: 0, C: 255) // Set the parameter(A: 0, C: 255)
		fuzzyEntropy.SetParameterA(0);
		fuzzyEntropy.SetParameterC(255);

		// 알고리즘 수행 // Execute the algorithm
		if((res = fuzzyEntropy.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Fuzzy Entropy.");
			break;
		}

		// 결과값을 받아올 CMultiVar<double> 컨테이너 생성 // Create the CMultiVar<double> object to push the result
		CMultiVar<double> mvFuzzyEntropy;

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 Fuzzy Entropy를 구하는 함수 // Function that calculate the fuzzy entropy of the image(or the region of ROI)
		if((res = fuzzyEntropy.GetResultFuzzyEntropy(mvFuzzyEntropy)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer = viewImage.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer.Clear();

		// ROI영역이 어디인지 알기 위해 디스플레이 한다 // Display to find out where ROI is
		if((res = layer.DrawFigureImage(&flrROI, LIME)).IsFail())
			ErrorPrint(res, "Failed to draw figure\n");

		CFLString<wchar_t> strFuzzyEntropy;
		strFuzzyEntropy.Format(L"Fuzzy Entropy Of Region : %lf", mvFuzzyEntropy.GetAt(0));

		wprintf_s(L"%s\n", strFuzzyEntropy.GetString());

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 0), strFuzzyEntropy.GetString(), YELLOW, BLACK, 25)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if((res = layer.DrawFigureImage(&flrROI, LIME)).IsFail())
			ErrorPrint(res, "Failed to draw figure\n");

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImage.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}