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

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSource;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// 이미지 1 로드 // Load image 1
		if(IsFail(res = fliSourceImage.Load(L"../../ExampleImages/HoughTransform/PatternExample.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성(이미지 1) // Create the Source image view (Image 1)
		if(IsFail(res = viewImageSource.Create(100, 0, 100 + 600, 600)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 뷰에 이미지를 디스플레이 // Display the image in the view
		if(IsFail(res = viewImageSource.SetImagePtr(&fliSourceImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Generalized Hough Transform 객체 생성 // Create Generalized Hough Transform  object
		CGeneralizedHoughTransform generalizedHoughTransform;

		// Source 이미지 설정 // Set the source image
		generalizedHoughTransform.SetSourceImage(fliSourceImage);

		CFLCircle<int64_t> flfPatternROI(575, 755, 71, 0, 0, 360, EArcClosingMethod_EachOther);
		generalizedHoughTransform.SetPatternROI(&flfPatternROI);

		// Threshold 값 설정 // Set Threshold value
		generalizedHoughTransform.SetPixelThreshold(128);

		// 신뢰도 설정 // set confidence
		generalizedHoughTransform.SetConfidence(0.5);

		// 탐색할 각도 단위 설정 (degree) // Set the angle unit to search (degree)
		generalizedHoughTransform.SetAngleTolerance(90);

		// 탐색할 크기 설정 (percent) // Set the scale tolerance to search (percent)
		generalizedHoughTransform.SetScaleTolerance(10);

		// 최대 검출 수 설정 // Set the maximum number of detections
		generalizedHoughTransform.SetMaxObjectCount(20);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = generalizedHoughTransform.Execute()))
		{
			ErrorPrint(res, "Failed to execute.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSource.Clear();

		// Result 갯수 체크 // Check the number of results
		if(generalizedHoughTransform.GetDetectedObjectCount() > 0)
		{
			CFLFigureArray flfaDetectedObjects;
			generalizedHoughTransform.GetDetectedObjects(&flfaDetectedObjects);

			// 이미지 뷰에 검출된 객체 출력 // Output the detected object to the image view
			if(IsFail(res = layerSource.DrawFigureImage(flfaDetectedObjects, BRIGHTCYAN, 2)))
			{
				ErrorPrint(res, "Failed to draw Figure\n");
				break;
			}
		}
		
		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSource.Invalidate(true);

		// 이미지 뷰 / 그래프 뷰가 종료될 때 까지 기다림
		while(viewImageSource.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}