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
		if((res = fliImage.Load(L"../../ExampleImages/PeripheralLuminance/Bolt.flif")).IsFail())
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

		// CPeripheralLuminance 객체 생성 // Create CPeripheralLuminance object
		CPeripheralLuminance peripheralLuminance;

		CFLFigureArray flfaMeasurementRegion;

		// 피겨 파일 로드 // Figure file Load
		if((res = flfaMeasurementRegion.Load(L"../../ExampleImages/PeripheralLuminance/Measurement Region.fig")).IsFail())
		{
			ErrorPrint(res, "Failed to execute Peripheral Luminance.");
			break;
		}

		// Source 이미지 설정 // Set the Source Image
		peripheralLuminance.SetSourceImage(fliImage);
		// Measurement Region 설정 // Set the Measurement Region
		peripheralLuminance.SetMeasurementRegion(flfaMeasurementRegion);
		// Thickness 설정 // Set Thickness
		peripheralLuminance.SetThickness(2.0);

		// 알고리즘 수행 // Execute the algorithm
		if((res = peripheralLuminance.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Peripheral Luminance.");
			break;
		}

		CFLArray<double> flaResult;

		// 측정 영역들의 휘도 차이에 결과 // Results of luminance differences in measurement areas
		if((res = peripheralLuminance.GetResult(flaResult)).IsFail())
		{
			ErrorPrint(res, "No Result");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer = viewImage.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer.Clear();

		CFLString<wchar_t> strText;

		for(int32_t i32Idx = 0; i32Idx < flaResult.GetCount(); ++i32Idx)
		{
			CFLFigure* pflfSrc = flfaMeasurementRegion.GetAt(i32Idx);

			strText.Format(L"%lf ", flaResult[i32Idx]);

			// 측정 영역이 어디인지 알기 위해 디스플레이 한다 // Display to find out where Measurement is
			if((res = layer.DrawFigureImage(pflfSrc, LIME, 1, LIME, EGUIViewImagePenStyle_Solid, 0.3f, 0.3f)).IsFail())
			{
				ErrorPrint(res, "Failed to draw Figure\n");
				break;
			}

			// 이미지 뷰 정보 표시 // Display image view information
			if((res = layer.DrawTextImage(pflfSrc, strText.GetString(), YELLOW, BLACK, 13, false, 0., EGUIViewImageTextAlignment_LEFT_TOP)).IsFail())
			{
				ErrorPrint(res, "Failed to draw text\n");
				break;
			}
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