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

	 // 수행 결과 객체 선언 // Declare the execution result object
	CResult res = EResult_UnknownError;

	do
	{
		uint16_t arrU16[16];

		for(int32_t i = 0; i < 16; ++i)
			arrU16[i] = ~(1 << i);

		// 버퍼로부터 Source 이미지 생성 // Create the source image from the buffer
		if ((res = fliSrcImage.Create(4, 4, (uint8_t *)arrU16, EPixelFormat_C1_U16)).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image views
		if ((res = viewImageSrc.Create(100, 0,  600, 545)).IsFail() ||
			(res = viewImageDst.Create(600, 0, 1100, 545)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화한다 // Synchronize the viewpoints of the two image views
		if ((res = viewImageSrc.SynchronizePointOfView(&viewImageDst)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the images in the image views
		if ((res = viewImageSrc.SetImagePtr(&fliSrcImage)).IsFail() ||
			(res = viewImageDst.SetImagePtr(&fliDstImage)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&viewImageDst)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Operation Leading Ones 객체 생성 // Create Operation Leading Ones object
		COperationLeadingOnes operationLeadingOnes;

		// 이미지 설정 // Set the images
		operationLeadingOnes.SetSourceImage(fliSrcImage);
		operationLeadingOnes.SetDestinationImage(fliDstImage);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = operationLeadingOnes.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute operation leading ones.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerDst = viewImageDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerDst.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		CFLPoint<double> flpPoint = CFLPoint<double>(0, 0);

		if ((res = layerSrc.DrawTextCanvas(&flpPoint, L"Source Image", YELLOW, BLACK, 20)).IsFail() ||
			(res = layerDst.DrawTextCanvas(&flpPoint, L"Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		viewImageSrc.SetPixelNumberMode(EPixelNumberMode_Hexadecimal);

		// 이미지 뷰를 갱신 // Update image view
		viewImageSrc.Invalidate(true);
		viewImageDst.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() && viewImageDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}