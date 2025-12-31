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
	CGUIViewGraphWrap viewGraph;

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 1 로드
		if(IsFail(res = fliSourceImage.Load(L"../../ExampleImages/Projection/mountains.flif")))
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

		// Graph 뷰 생성 // Create graph view
		if(IsFail(res = viewGraph.Create(100 + 440 * 1, 0, 100 + 440 * 2, 340)))
		{
			ErrorPrint(res, " Failed to create the graph view. \n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImageSource.SetImagePtr(&fliSourceImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 윈도우의 위치를 동기화 한다 // / Synchronize the positions of windows
		if(IsFail(res = viewImageSource.SynchronizeWindow(&viewGraph)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Projection 객체 생성 // Create Projection object
		CProjection projection;

		// Source 이미지 설정 // Set source image
		projection.SetSourceImage(fliSourceImage);

		// 연산 방향 설정 // Set operation direction
		projection.SetProjectionMode(CProjection::EProjectionDirection_Column);

		// 알고리즘 수행 // Execute the algorithm
		if((res = projection.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Projection.");
			break;
		}

		// Result 결과 갯수 확인 // get result count
		int64_t i64Count = projection.GetResultCount();

		// Channel 값 표기를 위한 String 변수 // string variable to indicate Channel value
		Base::CFLString<wchar_t> strChannel;

		// 그래프 선 색상 // Graph line color
		uint32_t arrColor[10] = { GRAPHVIEW_RED , GRAPHVIEW_GREEN , GRAPHVIEW_BLUE ,VIOLET,CYAN ,MAGENTA,AQUA,ORANGE,BLACK,YELLOW };

		// Projection 결과값 // Projection Result Object
		Base::CFLArray<double> flaResult;

		for(int64_t i = 0; i < i64Count; ++i)
		{
			// 이전 데이터 삭제 // data clear
			flaResult.Clear();

			// Projection 결과 값 가져오기 // get projection result
			if(IsFail(res = projection.GetResult(i, flaResult)))
				break;

			// 채널 String // Channel String
			strChannel.Format(L"Ch%d", i);

			// Graph View 데이터 입력 // Input Graph View Data
			viewGraph.Plot(flaResult, GUI::EChartType_Line, arrColor[i], strChannel, nullptr);
		}

		// 그래프 뷰를 갱신 합니다. // Update the Graph view.
		viewGraph.Invalidate(true);

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerSource.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		if(IsFail(res = layerSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageSource.Invalidate(true);

		// 이미지 뷰 / 그래프 뷰가 종료될 때 까지 기다림
		while(viewImageSource.IsAvailable() && viewGraph.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}