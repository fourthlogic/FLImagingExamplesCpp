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
		CMultiVar<double> mvMin, mvMax;

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 최소값을 구하는 함수 // Function that calculate the min of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetMin(mvMin)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 최대값을 구하는 함수 // Function that calculate the max of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetMax(mvMax)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// trimming 옵션 설정(Lower:0.2, Upper:0.4) // Set the trimming value(Lower:0.2, Upper:0.4)
		imageStatistics.SetTrimming(0.2, CImageStatistics::ETrimmingLocation_Lower);
		imageStatistics.SetTrimming(0.4, CImageStatistics::ETrimmingLocation_Upper);

		// trimming 된 결과값을 받아올 CMultiVar<double> 컨테이너 생성 // Create the CMultiVar<double> object to push the trimmed result
		CMultiVar<double> mvTrimmingMin, mvTrimmingMax;

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 최소값을 구하는 함수 // Function that calculate the min of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetMin(mvTrimmingMin)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 최대값을 구하는 함수 // Function that calculate the max of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetMax(mvTrimmingMax)).IsFail())
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

		CFLString<wchar_t> strTrimming;
		strTrimming.Format(L"Trimming Lower : %.2lf, Upper : %.2lf", imageStatistics.GetTrimming(CImageStatistics::ETrimmingLocation_Lower), imageStatistics.GetTrimming(CImageStatistics::ETrimmingLocation_Upper));

		CFLString<wchar_t> strMinValue, strMaxValue, strTrimmingMinValue, strTrimmingMaxValue;
		strMinValue.Format(L"Min Of Region : %lf", mvMin.GetAt(0));
		strMaxValue.Format(L"Max Of Region : %lf", mvMax.GetAt(0));
		strTrimmingMinValue.Format(L"Min Of Trimmed Region : %lf", mvTrimmingMin.GetAt(0));
		strTrimmingMaxValue.Format(L"Max Of Trimmed Region : %lf", mvTrimmingMax.GetAt(0));

		wprintf_s(L"%s\n", strMinValue.GetString());
		wprintf_s(L"%s\n", strMaxValue.GetString());
		wprintf_s(L"%s\n", strTrimming.GetString());
		wprintf_s(L"%s\n", strTrimmingMinValue.GetString());
		wprintf_s(L"%s\n", strTrimmingMaxValue.GetString());

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(CFLPoint<double>(0, 0), strMinValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 20), strMaxValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 60), strTrimming.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 90), strTrimmingMinValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 120), strTrimmingMaxValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		CFLRect<double> flrMinRegion(429, 336, 430, 337);
		CFLRect<double> flrMaxRegion(492, 370, 493, 371);
		flrMinRegion.Inflate(0.5);
		flrMaxRegion.Inflate(0.5);

		if((res = layer.DrawFigureImage(&flrROI, LIME)).IsFail())
			ErrorPrint(res, "Failed to draw figure\n");

		if((res = layer.DrawFigureImage(&flrMinRegion, BLUE, 1, LIGHTBLUE, GUI::EGUIViewImagePenStyle_Solid, 0.5F, 0.5F)).IsFail())
			ErrorPrint(res, "Failed to draw figure\n");

		if((res = layer.DrawFigureImage(&flrMaxRegion, RED, 1, RED, GUI::EGUIViewImagePenStyle_Solid, 0.5F, 0.5F)).IsFail())
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