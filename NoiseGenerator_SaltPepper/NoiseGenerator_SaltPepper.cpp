#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

enum EImageType
{
	EImageType_Source = 0,
	EImageType_Destination,
	EImageType_Count,
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage arrFliImage[EImageType_Count];

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap arrViewImage[EImageType_Count];
	CResult res;

	do
	{
		// 이미지 로드 // Loads image
		if(IsFail(res = arrFliImage[EImageType_Source].Load(L"../../ExampleImages/NoiseGenerator/Plate.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = arrFliImage[EImageType_Destination].Load(L"../../ExampleImages/NoiseGenerator/Plate.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Creates imageview
		if(IsFail(res = arrViewImage[EImageType_Source].Create(400, 0, 912, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = arrViewImage[EImageType_Destination].Create(912, 0, 1424, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다. // Synchronize the viewpoints of the two image views.
		if(IsFail(res = arrViewImage[EImageType_Source].SynchronizePointOfView(&arrViewImage[EImageType_Destination])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
		if(IsFail(res = arrViewImage[EImageType_Source].SetImagePtr(&arrFliImage[EImageType_Source])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = arrViewImage[EImageType_Destination].SetImagePtr(&arrFliImage[EImageType_Destination])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = arrViewImage[EImageType_Source].SynchronizeWindow(&arrViewImage[EImageType_Destination])))
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
		noiseGenerator.SetSourceImage(arrFliImage[EImageType_Source]);
		noiseGenerator.SetDestinationImage(arrFliImage[EImageType_Destination]);
		// 처리할 ROI 설정 // Set the ROI to be processed
		noiseGenerator.SetSourceROI(flrROI);
		noiseGenerator.SetDestinationROI(flrROI);

		// 생성할 노이즈 설정 // Set the noise to generate
		noiseGenerator.SetNoiseType(CNoiseGenerator::ENoiseType_SaltAndPepper);
		// 소금&후추 잡음을 적용할 면적 비율 설정 // Set salt & pepper noise Area ratio
		noiseGenerator.SetAreaRatio(0.1);

		// 소금&후추 잡음 값 설정.// Set salt & pepper noise Value
		noiseGenerator.SetMinimumRange(0.0);
		noiseGenerator.SetMaximumRange(255.0);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = noiseGenerator.Execute()))
		{
			ErrorPrint(res, "Failed to execute noise generator.\n");
			break;
		}

		CGUIViewImageLayerWrap layer = arrViewImage[EImageType_Source].GetLayer(0);
		CGUIViewImageLayerWrap layerDst = arrViewImage[EImageType_Destination].GetLayer(0);

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
		arrViewImage[EImageType_Source].Invalidate(true);
		arrViewImage[EImageType_Destination].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(arrViewImage[EImageType_Source].IsAvailable() && arrViewImage[EImageType_Destination].IsAvailable())
			CThreadUtilities::Sleep(1);

		for(int32_t i = 0; i < EImageType_Count; ++i)
			arrViewImage[i].Destroy();
	}
	while(false);

	return 0;
}