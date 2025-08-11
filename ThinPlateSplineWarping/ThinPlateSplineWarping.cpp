#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSrcImage;
	CFLImage fliDstImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageDst;

	// 알고리즘 동작 결과 // Algorithm execution result
	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res = fliSrcImage.Load(L"../../ExampleImages/ThinPlateSplineWarping/Undistortion.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create the source image view
		if((res = viewImageSrc.Create(400, 0, 912, 612)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if((res = viewImageSrc.SetImagePtr(&fliSrcImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create the destination image view
		if((res = viewImageDst.Create(912, 0, 1424, 612)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if((res = viewImageDst.SetImagePtr(&fliDstImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((res = viewImageSrc.SynchronizePointOfView(&viewImageDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&viewImageDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// Thin Plate Spline Warping 객체 생성 // Create Thin Plate Spline Warping object
		CThinPlateSplineWarping thinPlateSplineWarping;

		// Source 이미지 설정 // Set the source image
		thinPlateSplineWarping.SetSourceImage(fliSrcImage);
		// Destination 이미지 설정 // Set the destination image
		thinPlateSplineWarping.SetDestinationImage(fliDstImage);
		// Interpolation Method 설정 // Set the interpolation method
		thinPlateSplineWarping.SetInterpolationMethod(EInterpolationMethod_Bilinear);

		// 그리드를 (5,5)로 초기화 // Initialize grid to (5, 5)
		CFLPoint<int32_t> flpGridSize(5, 5);

		CFLPoint<int32_t> flpGridIndex;
		CFLPointArray flpaSourcePoints;
		CFLPointArray flpaDestinationPoints;

		double f64ScaleX = fliSrcImage.GetWidth() / 4.;
		double f64ScaleY = fliSrcImage.GetHeight() / 4.;

		for(int y = 0; y < flpGridSize.y; ++y)
		{
			flpGridIndex.y = y;

			for(int x = 0; x < flpGridSize.x; ++x)
			{
				flpGridIndex.x = x;

				// Grid Index와 같은 좌표로 Source 좌표를 설정 // Set source vertex same as the grid index
				CFLPoint<double> flpSource(flpGridIndex.x * f64ScaleX, flpGridIndex.y * f64ScaleY);
				// Grid Index와 같은 좌표에서 미세한 랜덤 값을 부여해서 왜곡된 Destination 좌표 설정 // Set distorted destination coordinates by giving fine random values in coordinates such as Grid Index
				CFLPoint<double> flpDistortion((flpGridIndex.x + CRandomGenerator::Double(-.2, .2)) * f64ScaleX, (flpGridIndex.y + CRandomGenerator::Double(-.2, .2)) * f64ScaleY);

				// 위에서 설정한 좌표들을 바탕으로 Calibration Point Array에 Point 추가 // Add points to the calibration point array based on the coordinates set above
				flpaSourcePoints.PushBack(flpSource);
				flpaDestinationPoints.PushBack(flpDistortion);
			}
		}

		thinPlateSplineWarping.SetCalibrationPointArray(flpaSourcePoints, flpaDestinationPoints);

		// Calibration Point Array를 화면에 Display // Display calibration point array

		CGUIViewImageLayerWrap layer;

		layer = viewImageSrc.GetLayer(0);

		for(int64_t i = 0; i < flpaSourcePoints.GetCount(); ++i)
		{
			CFLPoint<double> flpSource = flpaSourcePoints.GetAt(i);
			CFLPoint<double> flpDestination = flpaDestinationPoints.GetAt(i);

			// Source Vertex를 Source 이미지 뷰 Layer에 그리기 // Draw the source vertex on the source image view layer
			CFLLine<double> fllDrawLine(flpSource, flpDestination);
			// 선분을 화살표로 변경 // Change a line to an arrow
			CFLFigureArray flfaArrow = fllDrawLine.MakeArrowWithRatio(0.25, true, 20);

			if((res = layer.DrawFigureImage(&flpSource, RED, 1)).IsFail())
			{
				ErrorPrint(res, L"Failed to draw figure objects on the image view.\n");
				break;
			}

			if((res = layer.DrawFigureImage(&flpDestination, BLUE, 1)).IsFail())
			{
				ErrorPrint(res, L"Failed to draw figure objects on the image view.\n");
				break;
			}

			if((res = layer.DrawFigureImage(&flfaArrow, YELLOW, 1)).IsFail())
			{
				ErrorPrint(res, L"Failed to draw figure objects on the image view.\n");
				break;
			}
		}

		// 앞서 설정된 Source Image, Calibration Point Array를 기반으로 Calibrate 수행 // Calibrate based on previously set Source Image, Calibration Point Array
		if((res = thinPlateSplineWarping.Calibrate()).IsFail())
		{
			ErrorPrint(res, L"Failed to Calibrate.");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = thinPlateSplineWarping.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to Execute.");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageDst.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit of the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(1);
		CGUIViewImageLayerWrap layerDst = viewImageDst.GetLayer(1);

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
		if((res = layerSrc.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerDst.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc.Invalidate(true);
		viewImageDst.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() && viewImageDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}