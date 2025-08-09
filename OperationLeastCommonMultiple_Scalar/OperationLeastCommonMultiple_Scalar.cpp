#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


enum EType
{
	EType_Source = 0,
	EType_Destination1,
	EType_Destination2,
	ETypeCount,
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliSrcImage;
	CFLImage fliDstImage0;
	CFLImage fliDstImage1;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageDst0;
	CGUIViewImageWrap viewImageDst1;

	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if(IsFail(res = fliSrcImage.Load(L"../../ExampleImages/OperationLeastCommonMultiple/Gradient.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if((res = viewImageSrc.Create(100, 0, 600, 500)).IsFail() ||
		   (res = viewImageDst0.Create(600, 0, 1100, 500)).IsFail() ||
		   (res = viewImageDst1.Create(1100, 0, 1600, 500)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((res = viewImageSrc.SynchronizePointOfView(&viewImageDst0)).IsFail() ||
		   (res = viewImageSrc.SynchronizePointOfView(&viewImageDst1)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize view. \n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&viewImageDst0)).IsFail() ||
		   (res = viewImageSrc.SynchronizeWindow(&viewImageDst1)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window. \n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if((res = viewImageSrc.SetImagePtr(&fliSrcImage)).IsFail() ||
		   (res = viewImageDst0.SetImagePtr(&fliDstImage0)).IsFail() ||
		   (res = viewImageDst1.SetImagePtr(&fliDstImage1)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view. \n");
			break;
		}


		// 알고리즘 객체 생성 // Create algorithm object
		COperationLeastCommonMultiple operationLeastCommonMultiple;

		if((res = operationLeastCommonMultiple.SetSourceImage(fliSrcImage)).IsFail())
			break;
		if((res = operationLeastCommonMultiple.SetDestinationImage(fliDstImage0)).IsFail())
			break;
		if((res = operationLeastCommonMultiple.SetOperationSource(EOperationSource_Scalar)).IsFail())
			break;
		if((res = operationLeastCommonMultiple.SetScalarValue(CMultiVar<uint64_t>(5))).IsFail())
			break;

		// 알고리즘 수행 // Execute the algorithm
		if((res = operationLeastCommonMultiple.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute the algorithm.");
			break;
		}

		if((res = operationLeastCommonMultiple.SetDestinationImage(fliDstImage1)).IsFail())
			break;
		if((res = operationLeastCommonMultiple.SetOperationSource(EOperationSource_Scalar)).IsFail())
			break;
		if((res = operationLeastCommonMultiple.SetScalarValue(CMultiVar<uint64_t>(17))).IsFail())
			break;

		// 알고리즘 수행 // Execute the algorithm
		if((res = operationLeastCommonMultiple.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute the algorithm.");
			break;
		}


		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerDst0 = viewImageDst0.GetLayer(0);
		CGUIViewImageLayerWrap layerDst1 = viewImageDst1.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerSrc.Clear();
		layerDst0.Clear();
		layerDst1.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		CFLPoint<double> flpPoint = new CFLPoint<double>(0, 0);
		if((res = layerSrc.DrawTextCanvas(flpPoint, L"Source Image", YELLOW, BLACK, 30)).IsFail() ||
		   (res = layerDst0.DrawTextCanvas(flpPoint, L"Destination Image(LCM 5)", YELLOW, BLACK, 30)).IsFail() ||
		   (res = layerDst1.DrawTextCanvas(flpPoint, L"Destination Image(LCM 17)", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text. \n");
			break;
		}

		// Zoom Fit
		viewImageSrc.ZoomFit();
		viewImageDst0.ZoomFit();
		viewImageDst1.ZoomFit();

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageSrc.Invalidate(true);
		viewImageDst0.Invalidate(true);
		viewImageDst1.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() && viewImageDst0.IsAvailable() && viewImageDst1.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}