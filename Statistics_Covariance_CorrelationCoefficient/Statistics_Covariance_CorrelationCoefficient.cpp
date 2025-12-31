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
		if((res = fliImage.Load(L"../../ExampleImages/Statistics/MultiChannelSource.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if((res = viewImage.Create(400, 0, 1150, 700)).IsFail())
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

		// Source 이미지 설정 // Set the Source Image
		imageStatistics.SetSourceImage(fliImage);
		// 상관관계를 구할 채널을 설정 // Set the Correlation Channels
		imageStatistics.SetCorrelatedChannel(0, 1);

		// 결과값을 받아올 CMultiVar<double> 컨테이너 생성 // Create the CMultiVar<double> object to push the result
		double f64Covariance, f64CorrelationCoeff;

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 공분산을 구하는 함수 // Function that calculate the covariance of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetCovariance(f64Covariance)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 상관계수를 구하는 함수 // Function that calculate the correlation coefficient of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetCorrelationCoefficient(f64CorrelationCoeff)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 상관관계를 구할 채널을 설정 // Set the Correlation Channels
		imageStatistics.SetCorrelatedChannel(0, 2);

		// 결과값을 받아올 CMultiVar<double> 컨테이너 생성 // Create the CMultiVar<double> object to push the result
		double f64Covariance2, f64CorrelationCoeff2;

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 공분산을 구하는 함수 // Function that calculate the covariance of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetCovariance(f64Covariance2)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 이미지 전체(혹은 ROI 영역) 픽셀값의 상관계수를 구하는 함수 // Function that calculate the correlation coefficient of the pixel value of the image(or the region of ROI)
		if((res = imageStatistics.GetCorrelationCoefficient(f64CorrelationCoeff2)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer = viewImage.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer.Clear();

		CFLString<wchar_t> strCorrChannel, strCovarianceValue, strCorrelationCoeffValue;
		strCorrChannel.Format(L"Correlation Channel : Channel 0 - Channel 1");
		strCovarianceValue.Format(L"Covariance : %lf", f64Covariance);
		strCorrelationCoeffValue.Format(L"Correlation Coefficient : %lf", f64CorrelationCoeff);

		wprintf_s(L"%s\n", strCorrChannel.GetString());
		wprintf_s(L"%s\n", strCovarianceValue.GetString());
		wprintf_s(L"%s\n", strCorrelationCoeffValue.GetString());

		CFLString<wchar_t> strCorrChannel2, strCovarianceValue2, strCorrelationCoeffValue2;
		strCorrChannel2.Format(L"Correlation Channel : Channel 0 - Channel 2");
		strCovarianceValue2.Format(L"Covariance : %lf", f64Covariance2);
		strCorrelationCoeffValue2.Format(L"Correlation Coefficient : %lf", f64CorrelationCoeff2);

		wprintf_s(L"%s\n", strCorrChannel2.GetString());
		wprintf_s(L"%s\n", strCovarianceValue2.GetString());
		wprintf_s(L"%s\n", strCorrelationCoeffValue2.GetString());

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(CFLPoint<double>(0, 0), strCorrChannel.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 20), strCovarianceValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 60), strCorrelationCoeffValue.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 90), strCorrChannel2.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 120), strCovarianceValue2.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 150), strCorrelationCoeffValue2.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}


		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImage.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}