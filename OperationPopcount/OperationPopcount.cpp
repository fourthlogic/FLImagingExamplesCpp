#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliSrcImage;
	CFLImage fliDstImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageDst;

	CResult res = EResult_UnknownError;

	do
	{
		uint16_t arrU16[16];

		for(int32_t i = 0; i < 16; ++i)
			arrU16[i] = ~(0xffff << i);

		// 버퍼로부터 Source 이미지 생성 // Create the source image from the buffer
		if((res = fliSrcImage.Create(4, 4, (uint8_t*)arrU16, EPixelFormat_C1_U16)).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if((res = viewImageSrc.Create(100, 0, 600, 500)).IsFail() ||
		   (res = viewImageDst.Create(600, 0, 1100, 500)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if(IsFail(res = viewImageSrc.SynchronizePointOfView(&viewImageDst)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSrc.SynchronizeWindow(&viewImageDst)))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if((res = viewImageSrc.SetImagePtr(&fliSrcImage)).IsFail() ||
		   (res = viewImageDst.SetImagePtr(&fliDstImage)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view. \n");
			break;
		}


		// 알고리즘 객체 생성 // Create algorithm object
		COperationPopcount operationPopcount;

		// 이미지 설정 // Set the images
		if((res = operationPopcount.SetSourceImage(fliSrcImage)).IsFail())
			break;
		if((res = operationPopcount.SetDestinationImage(fliDstImage)).IsFail())
			break;

		// 알고리즘 수행 // Execute the algorithm
		if((res = operationPopcount.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute the algorithm.");
			break;
		}


		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerDst = viewImageDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerSrc.Clear();
		layerDst.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		CFLPoint<double> flpPoint = new CFLPoint<double>(0, 0);
		if((res = (layerSrc.DrawTextCanvas(flpPoint, L"Source Image", YELLOW, BLACK, 30))).IsFail() ||
		   (res = layerDst.DrawTextCanvas(flpPoint, L"Destination Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text. \n");
			break;
		}

		viewImageSrc.SetPixelNumberMode(EPixelNumberMode_Hexadecimal);

		// Zoom Fit
		viewImageSrc.ZoomFit();
		viewImageDst.ZoomFit();

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageSrc.Invalidate(true);
		viewImageDst.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() && viewImageDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}