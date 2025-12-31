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
	CFLImage fliConvertedImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageSource;
	CGUIViewImageWrap viewImageConverted;

	do
	{
		// 동작 결과 // operation result
		CResult eResult = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(eResult = fliSourceImage.Load(L"../../ExampleImages/ColorBoosting/Butterfly.flif")))
		{
			ErrorPrint(eResult, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(eResult = viewImageSource.Create(112, 0, 912, 534)))
		{
			ErrorPrint(eResult, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(eResult = viewImageConverted.Create(913, 0, 1713, 534)))
		{
			ErrorPrint(eResult, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(eResult = viewImageSource.SetImagePtr(&fliSourceImage)))
		{
			ErrorPrint(eResult, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(eResult = viewImageConverted.SetImagePtr(&fliConvertedImage)))
		{
			ErrorPrint(eResult, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if(IsFail(eResult = viewImageSource.SynchronizePointOfView(&viewImageConverted)))
		{
			ErrorPrint(eResult, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(eResult = viewImageSource.SynchronizeWindow(&viewImageConverted)))
		{
			ErrorPrint(eResult, "Failed to synchronize window\n");
			break;
		}

		// FL Color Boosting Type 1 객체 생성 // Create FL Color Boosting Type 1 object
		CFLColorBoostingType1 flColorBoostingType1;

		// Source 이미지 설정 // Set source image 
		if(IsFail(eResult = flColorBoostingType1.SetSourceImage(fliSourceImage)))
		{
			ErrorPrint(eResult, "Failed to set Source Image.");
			break;
		}

		// Destination 이미지 설정 // Set destination image
		if(IsFail(eResult = flColorBoostingType1.SetDestinationImage(fliConvertedImage)))
		{
			ErrorPrint(eResult, "Failed to set Destination Image.");
			break;
		}

		// Factor 설정 // Set Factor.
		if(IsFail(eResult = flColorBoostingType1.SetFactor(3)))
		{
			ErrorPrint(eResult, "Failed to set Factor.");
			break;
		}

		// 알고리즘 수행 // Execute the algorithm
		if((eResult = flColorBoostingType1.Execute()).IsFail())
		{
			ErrorPrint(eResult, "Failed to execute.");
			break;
		}

		// 이미지 뷰의 zoom fit // image view zoom fit
		if((eResult = viewImageConverted.ZoomFit()).IsFail())
		{
			ErrorPrint(eResult, "Failed to zoom fit\n");
			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);
		CGUIViewImageLayerWrap layerConverted = viewImageConverted.GetLayer(1);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerSource.Clear();
		layerConverted.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		if(IsFail(eResult = layerSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(eResult, "Failed to draw text\n");
			break;
		}

		if(IsFail(eResult = layerConverted.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(eResult, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageSource.Invalidate(true);
		viewImageConverted.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSource.IsAvailable() && viewImageConverted.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}