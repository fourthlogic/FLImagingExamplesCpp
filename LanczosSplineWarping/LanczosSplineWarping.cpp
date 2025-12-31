#include <cstdio>
#include "../CommonHeader/ErrorPrint.h"
#include <FLImaging.h>


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage arrFliImage[4];

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap arrViewImage[4];

	do
	{
		// 알고리즘 동작 결과 // Algorithmic executation result
		CResult res = EResult_UnknownError;

		int32_t arrLeft[4] = { 400, 912, 400, 912 };
		int32_t arrTop[4] = { 0, 0, 500, 500 };
		int32_t arrRight[4] = { 912, 1424, 912, 1424 };
		int32_t arrBottom[4] = { 500, 500, 1000, 1000 };

		// Source 이미지 로드 // Load the source image
		if(IsFail(res = arrFliImage[0].Load(L"../../ExampleImages/LanczosWarping/chess.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		int32_t i = 0;

		for(; i < 4; ++i)
		{
			// 이미지 뷰 생성 // Create image view
			if(IsFail(res = arrViewImage[i].Create(arrLeft[i], arrTop[i], arrRight[i], arrBottom[i])))
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				break;
			}
			//이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
			if(IsFail(res = arrViewImage[i].SetImagePtr(&arrFliImage[i])))
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				break;
			}
		}

		if(i < 4)
			break;

		i = 0;

		for(; i < 4; i += 2)
		{
			// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
			if(IsFail(res = arrViewImage[i].SynchronizePointOfView(&arrViewImage[i + 1])))
			{
				ErrorPrint(res, "Failed to synchronize view\n");
				break;
			}

			// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
			if(IsFail(res = arrViewImage[i].SynchronizeWindow(&arrViewImage[i + 1])))
			{
				ErrorPrint(res, "Failed to synchronize window.\n");
				break;
			}
		}

		if(i < 4)
			break;

		// LanczosSplineWarping 객체 생성 // Create LanczosSplineWarping object
		CLanczosSplineWarping lanczosSplineWarping;

		// Source 이미지 설정 // Set the source image
		lanczosSplineWarping.SetSourceImage(arrFliImage[0]);
		// Destination 이미지 설정 // Set the destination image
		lanczosSplineWarping.SetDestinationImage(arrFliImage[1]);
		// Interpolation Method 설정 // Set the interpolation method
		lanczosSplineWarping.SetInterpolationMethod(EInterpolationMethod_Bilinear);

		// 그리드를 (5,5)로 초기화 // Initialize the grid to (5,5)
		CFLPoint<int32_t> flpGridSize(5, 5);

		CFLPoint<int32_t> flpGridIndex;
		CFLPointArray flpaSourcePoints;
		CFLPointArray flpaDestinationPoints;

		double f64ScaleX = arrFliImage[0].GetWidth() / 4.;
		double f64ScaleY = arrFliImage[0].GetHeight() / 4.;

		for(int y = 0; y < flpGridSize.y; ++y)
		{
			flpGridIndex.y = y;

			for(int x = 0; x < flpGridSize.x; ++x)
			{
				flpGridIndex.x = x;

				// Grid Index와 같은 좌표로 Source 좌표를 설정 // Set Source coordinates to the same coordinates as Grid Index
				CFLPoint<double> flpSource(flpGridIndex.x * f64ScaleX, flpGridIndex.y * f64ScaleY);


				double f64RandomX = CRandomGenerator::Double(-0.2, 0.2);
				double f64RandomY = CRandomGenerator::Double(-0.2, 0.2);

				// 외곽의 좌표는 안쪽으로 변형 되도록 설정 // Set the outer coordinates to be transformed inward
				if(y == 0)
					f64RandomY = f64RandomY < 0 ? -f64RandomY : f64RandomY;

				if(x == 0)
					f64RandomX = f64RandomX < 0 ? -f64RandomX : f64RandomX;

				if(y == flpGridSize.y - 1)
					f64RandomY = f64RandomY > 0 ? -f64RandomY : f64RandomY;

				if(x == flpGridSize.x - 1)
					f64RandomX = f64RandomX > 0 ? -f64RandomX : f64RandomX;

			   // Grid Index와 같은 좌표에서 미세한 랜덤 값을 부여해서 좌표를 왜곡 // Distort coordinates by giving fine random values at the same coordinates as Grid Index 
				CFLPoint<double> flpDistortion = new CFLPoint<double>((flpGridIndex.x + f64RandomX) * f64ScaleX, (flpGridIndex.y + f64RandomY) * f64ScaleY);

				flpaSourcePoints.PushBack(flpSource);
				flpaDestinationPoints.PushBack(flpDistortion);
			}
		}

		lanczosSplineWarping.SetCalibrationPointArray(flpaSourcePoints, flpaDestinationPoints);

		// 앞서 설정된 Source Image, Calibration Point Array를 기반으로 Calibrate 수행 // Calibrate based on the previously set Source Image and Calibration Point Array
		if(IsFail(res = lanczosSplineWarping.Calibrate()))
		{
			ErrorPrint(res, "Failed to execute LanczosSplineWarping.");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = lanczosSplineWarping.Execute()))
		{
			ErrorPrint(res, "Failed to execute LanczosSplineWarping.");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if(IsFail(res = arrViewImage[1].ZoomFit()))
		{
			ErrorPrint(res, "Failed to zoom fit of the image view.\n");
			break;
		}

		//변환 결과를 원래대로 변환하는 과정 수행 // Perform the process of converting the conversion result back to its original state
		arrFliImage[2].Assign(arrFliImage[1]);
		// 컨트롤 포인트 설정 // Set control points
		lanczosSplineWarping.SetCalibrationPointArray(flpaDestinationPoints, flpaSourcePoints);
		// Source 이미지 설정 // Set the source image
		lanczosSplineWarping.SetSourceImage(arrFliImage[2]);
		// Destination 이미지 설정 // Set the destination image
		lanczosSplineWarping.SetDestinationImage(arrFliImage[3]);
		// Interpolation Method 설정 // Set the interpolation method
		lanczosSplineWarping.SetInterpolationMethod(EInterpolationMethod_Bilinear);

		// 앞서 설정된 Source Image, Calibration Point Array를 기반으로 Calibrate 수행
		if(IsFail(res = lanczosSplineWarping.Calibrate()))
		{
			ErrorPrint(res, "Failed to execute LanczosSplineWarping.");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = lanczosSplineWarping.Execute()))
		{
			ErrorPrint(res, "Failed to execute LanczosSplineWarping.");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if(IsFail(res = arrViewImage[3].ZoomFit()))
		{
			ErrorPrint(res, "Failed to zoom fit of the image view.\n");
			break;
		}

		// LanczosTrnasform 클래스에 설정된 Vertex 정보를 화면에 Display // Display the vertex information set in the LanczosTrnasform class on the screen

		CGUIViewImageLayerWrap layer;
		CGUIViewImageLayerWrap layer2;

		layer = arrViewImage[0].GetLayer(0);
		layer2 = arrViewImage[2].GetLayer(0);

		for(int64_t i = 0; i < flpaSourcePoints.GetCount(); ++i)
		{
			CFLPoint<double> flpSource = flpaSourcePoints.GetAt(i);
			CFLPoint<double> flpDestination = flpaDestinationPoints.GetAt(i);

			CFLLine<double> fllDrawLine(flpSource, flpDestination);
			CFLFigureArray flfaArrow = fllDrawLine.MakeArrowWithRatio(0.25, true, 20);

			// Source Vertex를 좌/우측 View Layer에 Drawing // Drawing the source vertex on the left/right view layer
			if(IsFail(res = layer.DrawFigureImage(&flpSource, RED, 1)))
			{
				ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
				break;
			}

			if(IsFail(res = layer.DrawFigureImage(&flpDestination, BLUE, 1)))
			{
				ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
				break;
			}

			if(IsFail(res = layer.DrawFigureImage(&flfaArrow, YELLOW, 1)))
			{
				ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
				break;
			}

			fllDrawLine = CFLLine<double>(flpDestination, flpSource);
			flfaArrow = fllDrawLine.MakeArrowWithRatio(0.25, true, 20);

			// Source Vertex를 좌/우측 View Layer에 Drawing // Drawing the source vertex on the left/right view layer
			if(IsFail(res = layer2.DrawFigureImage(&flpSource, RED, 1)))
			{
				ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
				break;
			}

			if(IsFail(res = layer2.DrawFigureImage(&flpDestination, BLUE, 1)))
			{
				ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
				break;
			}

			if(IsFail(res = layer2.DrawFigureImage(&flfaArrow, YELLOW, 1)))
			{
				ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
				break;
			}
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = arrViewImage[0].GetLayer(1);
		CGUIViewImageLayerWrap layerDst = arrViewImage[1].GetLayer(1);

		CGUIViewImageLayerWrap layerSrc2 = arrViewImage[2].GetLayer(1);
		CGUIViewImageLayerWrap layerDst2 = arrViewImage[3].GetLayer(1);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerDst.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerSrc.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDst.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerSrc2.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image2", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDst2.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image2", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		for(int32_t i = 0; i < 4; ++i)
			arrViewImage[i].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(arrViewImage[0].IsAvailable() && arrViewImage[1].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}