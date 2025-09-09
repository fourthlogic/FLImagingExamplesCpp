#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"
#include <cfloat>
#include <cmath>
#include <iostream>
#include <limits>
#include <process.h>

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliSourceImage;
	CFLImage fliResultLearnImage;
	CFLImage fliResultValidationImage;

	/// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageSource;
	CGUIViewImageWrap viewImagesLearn;
	CGUIViewImageWrap viewImagesValidation;

	CResult res = EResult_UnknownError;

	do
	{
		// 라이브러리가 완전히 로드 될 때까지 기다림 // Wait for the library to fully load
		CThreadUtilities::Sleep(1000);

		// 이미지 로드 // Loads image
		if(IsFail(res = fliSourceImage.Load(L"../../ExampleImages/SemanticSegmentation/Train.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Creates image view
		if(IsFail(res = viewImageSource.Create(100, 0, 600, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImagesLearn.Create(600, 0, 1100, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImagesValidation.Create(1100, 0, 1700, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImageSource.SynchronizePointOfView(&viewImagesLearn)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImageSource.SynchronizePointOfView(&viewImagesValidation)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 세 개의 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the three image view windows
		if(IsFail(res = viewImageSource.SynchronizeWindow(&viewImagesLearn)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageSource.SynchronizeWindow(&viewImagesValidation)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
		if(IsFail(res = viewImageSource.SetImagePtr(&fliSourceImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImagesLearn.SetImagePtr(&fliResultLearnImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImagesValidation.SetImagePtr(&fliResultValidationImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerLearn = viewImageSource.GetLayer(0);
		CGUIViewImageLayerWrap layerValidation = viewImagesLearn.GetLayer(0);
		CGUIViewImageLayerWrap layerResultLabel = viewImagesValidation.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerLearn.Clear();
		layerValidation.Clear();
		layerResultLabel.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerLearn.DrawTextCanvas(&CFLPoint<double>(0, 0), L"SOURCE", RED, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerValidation.DrawTextCanvas(&CFLPoint<double>(0, 0), L"TRAIN(EXTRACTED (3/4))", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerResultLabel.DrawTextCanvas(&CFLPoint<double>(0, 0), L"VALIDATION(EXTRACTED (1/4)", GREEN, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageSource.RedrawWindow();
		viewImagesLearn.RedrawWindow();
		viewImagesValidation.RedrawWindow();
	
		CValidationImageExtractorDL validationImageExtractorDL;

		// 소스이미지 설정 // Set the source image
		validationImageExtractorDL.SetSourceImage(fliSourceImage);
		// 결과 학습 이미지 설정 // Set the result learning image
		validationImageExtractorDL.SetResultLearningImage(fliResultLearnImage);
		// 결과 검증 이미지 설정 // Set the result validation image
		validationImageExtractorDL.SetResultValidationImage(fliResultValidationImage);
		// 데이터 셋 타입 설정 // Set the dataset type
		validationImageExtractorDL.SetDatasetType(CValidationImageExtractorDL::EDatasetType_SemanticSegmentation);
		// Validation Image 비율 설정 // Set ratio of validation image
		validationImageExtractorDL.SetValidationRatio(0.4f);
		// Validation Set에 최소한 몇 개의 클래스가 1개 이상 씩 포함될 것인지 설정 // Set how many classes each will be included in the Validation Set
		validationImageExtractorDL.SetMinimumClassesIncluded(2);

		// Validation Extractor 실행 // Validation Extractor Execute 
		if(IsFail(res = validationImageExtractorDL.Execute()))
		{
			ErrorPrint(res, "Failed to Process\n");
			break;
		}

		// 세 개의 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the three image view windows
		if(IsFail(res = viewImageSource.SynchronizeWindow(&viewImagesLearn)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageSource.SynchronizeWindow(&viewImagesValidation)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update the image view.
		viewImagesLearn.ZoomFit();
		viewImagesValidation.ZoomFit();
		viewImageSource.RedrawWindow();
		viewImagesLearn.RedrawWindow();
		viewImagesValidation.RedrawWindow();

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSource.IsAvailable() && viewImagesLearn.IsAvailable() && viewImagesValidation.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}