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
	CGUIViewImageWrap viewSrcImage;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res = EResult_UnknownError;

	do
	{
		// 이미지 로드 // Load image
		if((res = fliImage.Load(L"../../ExampleImages/OperationCompare/candle.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if((res = viewSrcImage.Create(400, 0, 912, 484)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		if((res = viewSrcImage.SetImagePtr(&fliImage)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer = viewSrcImage.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer.Clear();

		// HOG 객체 생성 // Create HOG object
		CHistogramsOfOrientedGradients histogramsOfOrientedGradients;

		// ROI 범위 생성 // Create ROI area
		CFLRect<int32_t> flrROI(200, 10, 300, 200);

		// 연산할 이미지 설정 // Set Image to Calculate
		if(IsFail(res = histogramsOfOrientedGradients.SetSourceImage(fliImage)))
		{
			ErrorPrint(res, "Failed to set Source Image.");
			break;
		}

		// 연산할 ROI 설정 // Set ROI to Calculate
		if(IsFail(res = histogramsOfOrientedGradients.SetSourceROI(flrROI)))
		{
			ErrorPrint(res, "Failed to set Source ROI.");
			break;
		}

		// 알고리즘 수행 // Execute the algorithm
		if((res = histogramsOfOrientedGradients.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Histograms Of Oriented Gradients.");
			break;
		}

		// 실행 결과를 받아오기 위한 컨테이너 // Container to get Calculated results
		CFLFigureArray flfaPeakVectors;

		// 피크 벡터 추출 // Get Peak Vectors
		if((res = histogramsOfOrientedGradients.GetPeakVectorsFigure(0, flfaPeakVectors)).IsFail())
		{
			ErrorPrint(res, "Failed to get result.");
			break;
		}

		// 피크 벡터를 출력 // Print Peak Vectors
		layer.DrawFigureImage(flfaPeakVectors[0], BLUE, 3, BLUE, EGUIViewImagePenStyle_Solid, 0.3f, 0.3f);
		layer.DrawFigureImage(flfaPeakVectors[1], GREEN, 3, GREEN, EGUIViewImagePenStyle_Solid, 0.3f, 0.3f);
		layer.DrawFigureImage(flfaPeakVectors[2], RED, 3, RED, EGUIViewImagePenStyle_Solid, 0.3f, 0.3f);

		// ROI영역이 어디인지 알기 위해 디스플레이 한다 // Display to find out where ROI is
		if(IsFail(res = layer.DrawFigureImage(&flrROI, LIME)))
		{
			ErrorPrint(res, "Failed to draw figures objects on the image view.\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewSrcImage.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewSrcImage.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}