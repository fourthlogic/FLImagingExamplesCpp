#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliSourceImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageSource;

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = fliSourceImage.Load(L"../../ExampleImages/HoughTransform/Sudoku.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImageSource.Create(100, 0, 100 + 440, 340)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImageSource.SetImagePtr(&fliSourceImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// HoughTransform  객체 생성 // Create HoughTransform object
		CHoughTransform houghTransform;

		// Source 이미지 설정 // Set source image
		houghTransform.SetSourceImage(fliSourceImage);

		// HoughTransform Line 변환 선택 // Set houghTransform line transform
		houghTransform.SetHoughShape(CHoughTransform::EHoughShape_Line);

		// 연산 방식 설정 // Set calculation method
		houghTransform.SetExecuteMode(CHoughTransform::EExecuteMode_Image);

		// Threshold 값 설정 // set threshold value
		houghTransform.SetPixelThreshold(10);

		// 조건 타입 설정 Less (Threshold 값 이하의 픽셀) // set logical condition(pixels below the threshold value)
		houghTransform.SetLogicalCondition(ELogicalCondition_Less);

		// 최소 픽셀 카운터 수 (픽셀 카운터 기준보다 낮을 경우 필터링) // Minimum number of pixel counters (Filter if lower than pixel counter criteria)
		houghTransform.SetMinPixelCount(200);

		// 인접 거리 필터링 설정 (거리 +-5, 각도 +-5 기준 가장 카운팅이 많이된 픽셀 값을 Line으로 선정) 
		// Neighbor Distance Filtering Settings(Based on distance +-5, angle +-5, the most counted pixel value is selected as a line)
		houghTransform.SetNearbyLineFilter(5);

		// 알고리즘 수행 // Execute the algorithm
		if((res = houghTransform.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute HoughTransform .");
			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerSource.Clear();

		// Result 갯수 가져오기 // get result count
		int64_t i64ResultCount = houghTransform.GetResultLinesCount();

		for(int64_t i = 0; i < i64ResultCount; i++) // 출력
		{
			CFLLine<double> fllLine;

			// line 결과 가져오기 // get result line
			houghTransform.GetResultLine(i, fllLine);

			// line 출력 // display line
			if(IsFail(res = layerSource.DrawFigureImage(fllLine, LIGHTGREEN, 1)))
			{
				ErrorPrint(res, "Failed to draw Figure\n");
				break;
			}
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageSource.Invalidate(true);

		// 이미지 뷰 / 그래프 뷰가 종료될 때 까지 기다림
		while(viewImageSource.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);


	return 0;
}