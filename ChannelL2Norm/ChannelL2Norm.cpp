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
		if(IsFail(res = arrFliImage[EImageType_Source].Load(L"../../ExampleImages/ChannelL2Norm/Coord.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if((res = arrViewImage[EImageType_Source].Create(100, 0, 600, 500)).IsFail() ||
		   (res = arrViewImage[EImageType_Destination].Create(600, 0, 1100, 500)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if(IsFail(res = arrViewImage[EImageType_Source].SynchronizePointOfView(&arrViewImage[EImageType_Destination])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = arrViewImage[EImageType_Source].SynchronizeWindow(&arrViewImage[EImageType_Destination])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if((res = arrViewImage[EImageType_Source].SetImagePtr(&arrFliImage[EImageType_Source])).IsFail() ||
		   (res = arrViewImage[EImageType_Destination].SetImagePtr(&arrFliImage[EImageType_Destination])).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view. \n");
			break;
		}


		// 알고리즘 객체 생성 // Create algorithm object
		CChannelL2Norm channelL2Norm;

		if((res = channelL2Norm.SetSourceImage(arrFliImage[EImageType_Source])).IsFail())
			break;
		if((res = channelL2Norm.SetDestinationImage(arrFliImage[EImageType_Destination])).IsFail())
			break;

		// 알고리즘 수행 // Execute the algorithm
		if((res = channelL2Norm.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute the algorithm.");
			break;
		}


		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSrc = arrViewImage[EImageType_Source].GetLayer(0);
		CGUIViewImageLayerWrap layerDst = arrViewImage[EImageType_Destination].GetLayer(0);

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

		// Zoom Fit
		arrViewImage[EImageType_Source].ZoomFit();
		arrViewImage[EImageType_Destination].ZoomFit();

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