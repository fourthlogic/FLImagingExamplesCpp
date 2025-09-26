#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSourceImage;
	CFLImage fliResultImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSource;
	CGUIViewImageWrap viewImageResult;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// 이미지 로드 // Load image
		if((res = fliSourceImage.Load(L"../../ExampleImages/Matching/DrawingImage.flif")).IsFail() ||
		   (res = fliResultImage.Assign(fliSourceImage)).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성(이미지 1) // Create the Source image view
		if((res = viewImageSource.Create(0, 0, 600, 600)).IsFail() ||
		   (res = viewImageResult.Create(600, 0, 1200, 600)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 뷰에 이미지를 디스플레이 // Display the image in the view
		if((res = viewImageSource.SetImagePtr(&fliSourceImage)).IsFail() ||
		   (res = viewImageResult.SetImagePtr(&fliResultImage)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 알고리즘 객체 생성 //create algorithm object
		CLineSegmentDetector lineSegmentDetector;
		lineSegmentDetector.SetSourceImage(&fliSourceImage);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = lineSegmentDetector.Execute()))
		{
			ErrorPrint(res, "Failed to execute.");
			break;
		}

		CFLArray<double> arrScores;
		CFLFigureArray flfaResults;

		double f64ScoreThreshold = lineSegmentDetector.GetScoreThreshold();

		lineSegmentDetector.GetResultLineSegments(flfaResults);
		lineSegmentDetector.GetResultScores(arrScores);

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer = viewImageResult.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer.Clear();

		// Layer에 이미지 좌표에 맞게 도형을 그림 // draw figure on the layer based on image coordinates.
		layer.DrawFigureImage(flfaResults, RED);

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSource.Invalidate(true);
		viewImageResult.Invalidate(true);

		// 이미지 뷰 / 그래프 뷰가 종료될 때 까지 기다림
		while(viewImageSource.IsAvailable() && viewImageResult.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}