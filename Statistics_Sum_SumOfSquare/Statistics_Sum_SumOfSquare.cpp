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
		if((res = fliImage.Load(L"../../ExampleImages/Statistics/StatisticsSource.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if((res = viewImage.Create(400, 0, 1150, 500)).IsFail())
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

		// Statistics 객체 생성 // Create Statistics object
		CImageStatistics imageStatistics;

		// ROI 범위 설정 // Set the ROI value
		CFLRect<double> flrROI(264, 189, 432, 364);
		// Source 이미지 설정 // Set the Source Image
		imageStatistics.SetSourceImage(fliImage);
		// Source ROI 설정 // Set the Source ROI
		imageStatistics.SetSourceROI(flrROI);

		// 결과값을 받아올 CMultiVar<double> 컨테이너 생성 // Create the CMultiVar<double> object to push the result
		CMultiVar<double> mvSum, mvSumOfSquares;

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 합을 구하는 함수 // Function that calculate the sum of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetSum(mvSum)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 제곱합을 구하는 함수 // Function that calculate the sum of squares of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetSumOfSquares(mvSumOfSquares)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// trimming 옵션 설정(Lower:0.2, Upper:0.4) // Set the Trimming value(Lower:0.2, Upper:0.4)
		imageStatistics.SetTrimming(0.2, CImageStatistics::ETrimmingLocation_Lower);
		imageStatistics.SetTrimming(0.4, CImageStatistics::ETrimmingLocation_Upper);

		// trimming 된 결과값을 받아올 CMultiVar<double> 컨테이너 생성 // Create the CMultiVar<double> object to push the trimmed result
		CMultiVar<double> mvTrimmingSum, mvTrimmingSumOfSquares;

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 합을 구하는 함수 // Function that calculate the sum of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetSum(mvTrimmingSum)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 제곱합을 구하는 함수 // Function that calculate the sum of squares of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetSumOfSquares(mvTrimmingSumOfSquares)).IsFail())
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
			ErrorPrint(res, "Failed to draw figure.");

		CFLString<wchar_t> strTrimming;
		strTrimming.Format(L"Trimming Lower : %.2lf, Upper : %.2lf", imageStatistics.GetTrimming(CImageStatistics::ETrimmingLocation_Lower), imageStatistics.GetTrimming(CImageStatistics::ETrimmingLocation_Upper));

		CFLString<wchar_t> strSumValue, strSumOfSquaresValue;
		strSumValue.Format(L"Sum Of Region : %lf", mvSum.GetAt(0));
		strSumOfSquaresValue.Format(L"Sum of Squares Of Region : %lf", mvSumOfSquares.GetAt(0));

		wprintf_s(L"%s\n", strSumValue.GetString());
		wprintf_s(L"%s\n", strSumOfSquaresValue.GetString());
		wprintf_s(L"%s\n", strTrimming.GetString());

		CFLString<wchar_t> strTrimmingSumValue, strTrimmingSumOfSquaresValue;
		strTrimmingSumValue.Format(L"Sum of Trimmed Region : %lf", mvTrimmingSum.GetAt(0));
		strTrimmingSumOfSquaresValue.Format(L"Sum of Squares of Trimmed Region : %lf", mvTrimmingSumOfSquares.GetAt(0));

		wprintf_s(L"%s\n", strTrimmingSumValue.GetString());
		wprintf_s(L"%s\n", strTrimmingSumOfSquaresValue.GetString());

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 0), strSumValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text.");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 20), strSumOfSquaresValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text.");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 60), strTrimming.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text.");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 90), strTrimmingSumValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text.");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 120), strTrimmingSumOfSquaresValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text.");
			break;
		}

		if((res = layer.DrawFigureImage(&flrROI, BLUE, 1, LIGHTBLUE, GUI::EGUIViewImagePenStyle_Solid, 0.5F, 0.5F)).IsFail())
			ErrorPrint(res, "Failed to draw figure.");

		if((res = layer.DrawFigureImage(&flrROI, LIME)).IsFail())
			ErrorPrint(res, "Failed to draw figure.");


		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImage.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}