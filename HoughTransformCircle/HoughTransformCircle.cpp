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
		if(IsFail(res = fliSourceImage.Load(L"../../ExampleImages/HoughTransform/coins.flif")))
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

		// HoughTransform 객체 생성 // Create HoughTransform  object
		CHoughTransform houghTransform;

		// Source 이미지 설정 // Set the source image
		houghTransform.SetSourceImage(fliSourceImage);

		// HoughTransform Circle 변환 선택 // Select HoughTransform Circle transform
		houghTransform.SetHoughShape(CHoughTransform::EHoughShape_Circle);

		// 이미지로 임계값으로 동작하는 모드 적용 // Apply the mode that operates as a threshold to the image
		houghTransform.SetExecuteMode(CHoughTransform::EExecuteMode_Image);

		// Threshold 값 설정 // Set Threshold value
		houghTransform.SetPixelThreshold(200);

		// 조건 타입 설정 Less (Threshold 값 이하의 픽셀) // Set the condition type Less (pixels below the Threshold value)
		houghTransform.SetLogicalCondition(ELogicalCondition_Less);

		// 검출할 최소 반지름 설정 // Set minimum radius to detect
		houghTransform.SetMinRadius(35);

		// 검출할 최대 반지름 설정 // Set the maximum radius to detect
		houghTransform.SetMaxRadius(40);

		// 탐색할 반지름 단위 설정 (1로 설정할 시 35(Min), 36, 37 ... 40(Max) 으로 탐색)
		// Set the radius unit to search (when set to 1, search with 35 (Min), 36, 37 ... 40 (Max))
		houghTransform.SetPixelResolution(1);

		// 탐색할 각도 단위 설정 (degree) // Set the angle unit to search (degree)
		houghTransform.SetAngleResolution(5);

		// 신뢰도 설정 (%) // set confidence (%)
		houghTransform.SetConfidence(70);

		// 최대 검출 수 설정 // Set the maximum number of detections
		houghTransform.SetMaxCount(100);

		// Canny Edge 적용유무 설정 (true 로 설정할 경우, SetPixelThreshold와 SetLogicalCondition로 설정한 값은 활용하지 않음)
		// Setting whether to apply Canny Edge (if set to true, the values set by SetPixelThreshold and SetLogicalCondition are not used)
		houghTransform.EnableCannyEdgeAppliance(false);

		// 인접하게 검출된 원을 필터링하는 옵션 // Option to filter adjacent detected circles
		houghTransform.EnableAdjacentFilterAppliance(true);


		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = houghTransform.Execute()))
		{
			ErrorPrint(res, "Failed to execute HoughTransform.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSource.Clear();

		// Result 갯수 가져오기 // Get the number of results
		int64_t i64ResultCount = houghTransform.GetResultCirclesCount();

		for(int64_t i = 0; i < i64ResultCount; i++)
		{
			CFLCircle<double> flcResult;

			houghTransform.GetResultCircle(i, flcResult);

			// 이미지 뷰에 검출된 원 객체 출력 // Output the detected original object to the image view
			if(IsFail(res = layerSource.DrawFigureImage(flcResult, LIGHTGREEN, 1)))
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