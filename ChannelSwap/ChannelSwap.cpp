#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


int main()
{
	// You must call the following function once
    // before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSourceImage;
	CFLImage fliDestinationImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewSourceImage;
	CGUIViewImageWrap viewDestinationImage;

	do
	{
		// 수행 결과 객체 선언 // Declare execution result object
		CResult res = EResult_UnknownError;

		// Source 이미지 로드 // Load Source image
		if((res = fliSourceImage.Load(L"../../ExampleImages/channelSwap/deer.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if((res = viewSourceImage.Create(400, 0, 1052, 427)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create Destination image view
		if((res = viewDestinationImage.Create(1052, 0, 1692, 427)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 // Synchronize viewpoints of two image views
		if((res = viewSourceImage.SynchronizePointOfView(&viewDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize point of view between image views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewSourceImage.SynchronizeWindow(&viewDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display image in Source image view
		if((res = viewSourceImage.SetImagePtr(&fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display image in Destination image view
		if((res = viewDestinationImage.SetImagePtr(&fliDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Channel Swap 객체 생성 // Create Channel Swap object
		CChannelSwap channelSwap;

		// Source 이미지 설정 // Set Source image
		if((res = channelSwap.SetSourceImage(fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Source image.\n");
			break;
		}

		// Destination 이미지 설정 // Set Destination image
		if((res = channelSwap.SetDestinationImage(fliDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination image.\n");
			break;
		}

		// 교환할 채널 인덱스 설정 // Set channel indexes to swap
		if((res = channelSwap.SetSwapChannelFrom(EChannelSelection_Channel_0)).IsFail())
		{
			ErrorPrint(res, L"Failed to set swap channel from.\n");
			break;
		}

		if((res = channelSwap.SetSwapChannelTo(EChannelSelection_Channel_1)).IsFail())
		{
			ErrorPrint(res, L"Failed to set swap channel to.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = channelSwap.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Channel Swap.\n");
			break;
		}

		// 화면에 출력하기 위해 이미지 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
        // 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released
		CGUIViewImageLayerWrap layerSource = viewSourceImage.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination = viewDestinationImage.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear figures drawn on existing layer
		layerSource.Clear();
		layerDestination.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerDestination.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		viewSourceImage.Invalidate(true);
		viewDestinationImage.Invalidate(true);

		// 뷰가 닫히기 전까지 종료하지 않고 대기 // Wait until a view is closed before exiting
		while(viewSourceImage.IsAvailable() && viewDestinationImage.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}