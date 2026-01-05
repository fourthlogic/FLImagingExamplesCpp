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

		// BM3DFilter 객체 생성 // Create BM3DFilter object
		CBM3DFilter bm3dFilter;

		// Source 이미지 설정 // Set the source image
		bm3dFilter.SetSourceImage(fliSrcImage);

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

		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerDst1 = viewImageDst1.GetLayer(0);
		CGUIViewImageLayerWrap layerDst2 = viewImageDst2.GetLayer(0);

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

		if(IsFail(res = layerDst2.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination2 Image (Basic Estimate)", YELLOW, BLACK, 20)))
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