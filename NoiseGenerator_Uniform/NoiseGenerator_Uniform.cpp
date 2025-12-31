#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

// 이미지 객체 선언 // Declare the image object
	CFLImage fliImage;
	CFLImage fliDestinationImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImage[2];
	CResult res;

	do
	{
		// 이미지 로드 // Loads image
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/NoiseGenerator/Plate.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliDestinationImage.Load(L"../../ExampleImages/NoiseGenerator/Plate.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Creates imageview
		if(IsFail(res = viewImage[0].Create(400, 0, 912, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[1].Create(912, 0, 1424, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다. // Synchronize the viewpoints of the two image views.
		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[1])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
		if(IsFail(res = viewImage[0].SetImagePtr(&fliImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[1].SetImagePtr(&fliDestinationImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[1])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		CResult res = EResult_UnknownError;

		// Noise Generator 객체 생성 // Create Noise Generator object
		CNoiseGenerator noiseGenerator;

		// ROI 범위 설정 // Set ROI range
		CFLRect<int32_t> flrROI(61, 63, 583, 376);

		// 처리할 이미지 설정 // Set the image to process
		noiseGenerator.SetSourceImage(fliImage);
		noiseGenerator.SetDestinationImage(fliDestinationImage);
		// 처리할 ROI 설정 // Set the ROI to be processed
		noiseGenerator.SetSourceROI(flrROI);
		noiseGenerator.SetDestinationROI(flrROI);

		// 생성할 노이즈 설정 // Set the noise to generate
		noiseGenerator.SetNoiseType(CNoiseGenerator::ENoiseType_UniformNoise);
		// 균등 분포 잡음의 크기 비율 설정 // Set the magnitude ratio of uniformly distributed noise
		// 범위 0 ~ 1사이에서 1에 가까울 수록 이미지 포맷에서의 각 픽셀 잡음의 변동성이 커지며, 0에 가까울 수록 잡음의 변동성이 작아짐 // From 0 to 1, the closer to 1, the greater the variability of each pixel noise in the image format, and the closer to 0, the less the variability of the noise.
		noiseGenerator.SetUniformDistNoise(0.05);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = noiseGenerator.Execute()))
		{
			ErrorPrint(res, "Failed to execute noise generator.\n");
			break;
		}

		CGUIViewImageLayerWrap layer = viewImage[0].GetLayer(0);
		CGUIViewImageLayerWrap layerDst = viewImage[1].GetLayer(0);

		layer.Clear();
		layerDst.Clear();

		// ROI영역이 어디인지 알기 위해 디스플레이 한다 // Display to know where the ROI area is
		if(IsFail(res = layer.DrawFigureImage(&flrROI, LIME)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		if(IsFail(res = layerDst.DrawFigureImage(&flrROI, LIME)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if(IsFail(res = layer.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDst.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImage[0].Invalidate(true);
		viewImage[1].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage[0].IsAvailable() && viewImage[1].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}