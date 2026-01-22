#include <cstdio>
#include "../CommonHeader/ErrorPrint.h"
#include <FLImaging.h>


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSrcImage;
	CFLImage fliDst1Image;
	CFLImage fliDst2Image;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageDst1;
	CGUIViewImageWrap viewImageDst2;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult res = EResult_UnknownError;

		// Source 이미지 로드 // Load the source image
		if(IsFail(res = fliSrcImage.Load(L"../../ExampleImages/NoiseImage/NoiseImage1.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Destination1 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination1 image as same as source image
		if(IsFail(res = fliDst1Image.Assign(fliSrcImage)))
		{
			ErrorPrint(res, "Failed to assign the image file.\n");
			break;
		}

		// Destination2 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination2 image as same as source image
		if(IsFail(res = fliDst2Image.Assign(fliSrcImage)))
		{
			ErrorPrint(res, "Failed to assign the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create the source image view
		if(IsFail(res = viewImageSrc.Create(100, 0, 550, 480)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if(IsFail(res = viewImageSrc.SetImagePtr(&fliSrcImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Destination1 이미지 뷰 생성 // Create the destination1 image view
		if(IsFail(res = viewImageDst1.Create(550, 0, 1000, 480)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination1 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination1 image view
		if(IsFail(res = viewImageDst1.SetImagePtr(&fliDst1Image)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Destination2 이미지 뷰 생성 // Create the destination2 image view
		if(IsFail(res = viewImageDst2.Create(1000, 0, 1450, 480)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination2 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination2 image view
		if(IsFail(res = viewImageDst2.SetImagePtr(&fliDst2Image)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImageSrc.SynchronizePointOfView(&viewImageDst1)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSrc.SynchronizeWindow(&viewImageDst1)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImageSrc.SynchronizePointOfView(&viewImageDst2)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSrc.SynchronizeWindow(&viewImageDst2)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Source Image 크기에 맞게 view의 크기를 조정 // Zoom the view to fit the image size
		if(IsFail(res = viewImageSrc.ZoomFit()))
		{
			ErrorPrint(res, "Failed to zoom fit\n");
			break;
		}

		// Destination1 Image 크기에 맞게 view의 크기를 조정 // Zoom the view to fit the image size
		if(IsFail(res = viewImageDst1.ZoomFit()))
		{
			ErrorPrint(res, "Failed to zoom fit\n");
			break;
		}

		// Destination2 Image 크기에 맞게 view의 크기를 조정 // Zoom the view to fit the image size
		if(IsFail(res = viewImageDst2.ZoomFit()))
		{
			ErrorPrint(res, "Failed to zoom fit\n");
			break;
		}

		// ROI 설정을 위한 CFLRect 객체 생성 // Create a CFLRect object for setting ROI
		CFLRect<int32_t> flrROI(200, 200, 500, 500);

		// BM3DFilter 객체 생성 // Create BM3DFilter object
		CBM3DFilter bm3dFilter;

		// Source 이미지 설정 // Set the source image
		bm3dFilter.SetSourceImage(fliSrcImage);

		// Source ROI 설정 // Set the source ROI
		bm3dFilter.SetSourceROI(flrROI);

		// Destination 이미지 설정 // Set the destination image
		bm3dFilter.SetDestinationImage(fliDst1Image);

		// Sigma (노이즈의 표준편차) 설정 // Set the sigma (standard deviation of the noise)
		bm3dFilter.SetSigma(0.2);

		// Processing Mode 설정 // Set the processing mode
		bm3dFilter.SetProcessingMode(CBM3DFilter::EProcessingMode_BasicEstimate);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = bm3dFilter.Execute()))
		{
			ErrorPrint(res, "Failed to execute BM3D Filter.");
			break;
		}

		// Destination 이미지 설정 // Set the destination image
		bm3dFilter.SetDestinationImage(fliDst2Image);

		// Processing Mode 설정 // Set the processing mode
		bm3dFilter.SetProcessingMode(CBM3DFilter::EProcessingMode_FinalEstimate);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = bm3dFilter.Execute()))
		{
			ErrorPrint(res, "Failed to execute BM3D Filter.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerDst1 = viewImageDst1.GetLayer(0);
		CGUIViewImageLayerWrap layerDst2 = viewImageDst2.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerSrc.Clear();
		layerDst1.Clear();
		layerDst2.Clear();

		// ROI 영역이 어디인지 알기 위해 디스플레이 한다 // Display to find out where ROI is
		// FLImaging의 Figure 객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능 // FLimaging's Figure objects can be displayed as a function regardless of the shape
		// 아래 함수 DrawFigureImage는 Image 좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
		// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일 경우 불투명, 0일 경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
		// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)
		if((res = layerSrc.DrawFigureImage(&flrROI, LIME)).IsFail())
			ErrorPrint(res, L"Failed to draw figure\n");

		if((res = layerDst1.DrawFigureImage(&flrROI, LIME)).IsFail())
			ErrorPrint(res, L"Failed to draw figure\n");

		if((res = layerDst2.DrawFigureImage(&flrROI, LIME)).IsFail())
			ErrorPrint(res, L"Failed to draw figure\n");

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

		if(IsFail(res = layerDst1.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination1 Image (Basic Estimate)", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDst2.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination2 Image (Final Estimate)", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc.Invalidate(true);
		viewImageDst1.Invalidate(true);
		viewImageDst2.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() && viewImageDst1.IsAvailable() && viewImageDst2.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}