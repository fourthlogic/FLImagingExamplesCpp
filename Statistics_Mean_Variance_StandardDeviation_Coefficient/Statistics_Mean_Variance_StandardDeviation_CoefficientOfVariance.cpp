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
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if((res = viewImage.Create(400, 0, 1150, 500)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

        // 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		if((res = viewImage.SetImagePtr(&fliImage)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
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
		CMultiVar<double> mvMean, mvVariance, mvStandardDeviation, mvCoefficientOfVariance;

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 평균을 구하는 함수 // Function that calculate the mean of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetMean(mvMean)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 분산을 구하는 함수 // Function that calculate the variance of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetVariance(mvVariance)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 표준편차를 구하는 함수 // Function that calculate the standard deviation of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetStandardDeviation(mvStandardDeviation)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 변동계수을 구하는 함수 // Function that calculate the coefficient of variance of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetCoefficientOfVariance(mvCoefficientOfVariance)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// trimming 옵션 설정(Lower:0.2, Upper:0.4) // Set the trimming value(Lower:0.2, Upper:0.4)
		imageStatistics.SetTrimming(0.2, CImageStatistics::ETrimmingLocation_Lower);
		imageStatistics.SetTrimming(0.4, CImageStatistics::ETrimmingLocation_Upper);

		// trimming 된 결과값을 받아올 CMultiVar<double> 컨테이너 생성 // Create the CMultiVar<double> object to push the trimmed result
		CMultiVar<double> mvTrimmingMean, mvTrimmingVariance, mvTrimmingStandardDeviation, mvTrimmingCoefficientOfVariance;

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 평균을 구하는 함수 // Function that calculate the mean of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetMean(mvTrimmingMean)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 분산을 구하는 함수 // Function that calculate the variance of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetVariance(mvTrimmingVariance)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 표준편차를 구하는 함수 // Function that calculate the standard deviation of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetStandardDeviation(mvTrimmingStandardDeviation)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 변동계수을 구하는 함수 // Function that calculate the coefficient of variance of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetCoefficientOfVariance(mvTrimmingCoefficientOfVariance)).IsFail())
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

		CFLString<wchar_t> strMeanValue, strVarianceValue, strStandardDeviationValue, strCoefficientOfVariance;
		strMeanValue.Format(L"Mean Of Region : %lf", mvMean.GetAt(0));
		strStandardDeviationValue.Format(L"StandardDeviation  Of Region : %lf", mvStandardDeviation.GetAt(0));
		strVarianceValue.Format(L"Variance  Of Region : %lf", mvVariance.GetAt(0));
		strCoefficientOfVariance.Format(L"Coefficient of Variance Of Region : %lf", mvCoefficientOfVariance.GetAt(0));

		CFLString<wchar_t> strTrimmingMeanValue, strTrimmingVarianceValue, strTrimmingStandardDeviationValue, strTrimmingCoefficientOfVariance;
		strTrimmingMeanValue.Format(L"Mean Of Trimmed Region : %lf", mvTrimmingMean.GetAt(0));
		strTrimmingStandardDeviationValue.Format(L"StandardDeviation  Of Trimmed Region : %lf", mvTrimmingStandardDeviation.GetAt(0));
		strTrimmingVarianceValue.Format(L"Variance  Of Trimmed Region : %lf", mvTrimmingVariance.GetAt(0));
		strTrimmingCoefficientOfVariance.Format(L"Coefficient of Variance Of Trimmed Region : %lf", mvTrimmingCoefficientOfVariance.GetAt(0));

		wprintf_s(L"%s\n", strMeanValue.GetString());
		wprintf_s(L"%s\n", strStandardDeviationValue.GetString());
		wprintf_s(L"%s\n", strVarianceValue.GetString());
		wprintf_s(L"%s\n", strCoefficientOfVariance.GetString());

		wprintf_s(L"%s\n", strTrimming.GetString());

		wprintf_s(L"%s\n", strTrimmingMeanValue.GetString());
		wprintf_s(L"%s\n", strTrimmingStandardDeviationValue.GetString());
		wprintf_s(L"%s\n", strTrimmingVarianceValue.GetString());
		wprintf_s(L"%s\n", strTrimmingCoefficientOfVariance.GetString());

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(CFLPoint<double>(0, 0), strMeanValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 20), strStandardDeviationValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 60), strVarianceValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 90), strCoefficientOfVariance.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 120), strTrimming.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 150), strTrimmingMeanValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 180), strTrimmingStandardDeviationValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 210), strTrimmingVarianceValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 240), strTrimmingCoefficientOfVariance.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if((res = layer.DrawFigureImage(&flrROI, BLUE, 1, LIGHTBLUE, GUI::EGUIViewImagePenStyle_Solid, 0.5F, 0.5F)).IsFail())
			ErrorPrint(res, "Failed to draw figure\n");

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