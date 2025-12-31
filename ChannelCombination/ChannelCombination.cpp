#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliSourceImage[3];
	CFLImage fliDestinationImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageSource[3];
	CGUIViewImageWrap viewImageDestination;

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = fliSourceImage[0].Load(L"../../ExampleImages/ChannelCombination/Valley1.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliSourceImage[1].Load(L"../../ExampleImages/ChannelCombination/Valley2.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliSourceImage[2].Load(L"../../ExampleImages/ChannelCombination/Valley3.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImageSource[0].Create(100, 0, 100 + 440, 340)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageSource[1].Create(100 + 440, 0, 100 + 440 * 2, 340)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageSource[2].Create(100 + 440 * 2, 0, 100 + 440 * 3, 340)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageDestination.Create(100 + 440 * 3, 0, 100 + 440 * 4, 340)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if(IsFail(res = viewImageSource[0].SynchronizePointOfView(&viewImageSource[1])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImageSource[1].SynchronizePointOfView(&viewImageSource[2])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImageSource[2].SynchronizePointOfView(&viewImageDestination)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSource[0].SynchronizeWindow(&viewImageSource[1])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageSource[1].SynchronizeWindow(&viewImageSource[2])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageSource[2].SynchronizeWindow(&viewImageDestination)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImageSource[0].SetImagePtr(&fliSourceImage[0])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageSource[1].SetImagePtr(&fliSourceImage[1])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageSource[2].SetImagePtr(&fliSourceImage[2])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageDestination.SetImagePtr(&fliDestinationImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Channel Combination 객체 생성 // Create Channel Combination object
		CChannelCombination channelCombination;

		// Source 이미지를 저장할 Array 선언 // Declare an Array to store the source image
		Base::CFLArray<CFLImage*> vctSrcImages;

		// 결합할 채널을 저장할 Array 선언 // Declare an Array to store the channels
		Base::CFLArray<int64_t> vctSrcChannels;

		// Source 이미지 입력 // source images push
		vctSrcImages.PushBack(&fliSourceImage[0]);
		vctSrcImages.PushBack(&fliSourceImage[1]);
		vctSrcImages.PushBack(&fliSourceImage[2]);

		// 이미지별 결합할 채널을 입력 // channels push
		vctSrcChannels.PushBack(EChannelSelection_Channel_0);
		vctSrcChannels.PushBack(EChannelSelection_Channel_0);
		vctSrcChannels.PushBack(EChannelSelection_Channel_0);

		// 결합할 이미지 및 채널입력 // Set images, channels
		channelCombination.SetSourceImage(vctSrcImages, vctSrcChannels);

		// 결합 결과를 저장할 이미지 설정 // Set destination image
		channelCombination.SetDestinationImage(fliDestinationImage);

		// 알고리즘 수행 // Execute the algorithm
		if((res = channelCombination.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Channel Combination.");
			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSource[3];
		CGUIViewImageLayerWrap layerDestination = viewImageDestination.GetLayer(1);
		layerSource[0] = viewImageSource[0].GetLayer(0);
		layerSource[1] = viewImageSource[1].GetLayer(0);
		layerSource[2] = viewImageSource[2].GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerSource[0].Clear();
		layerSource[1].Clear();
		layerSource[2].Clear();
		layerDestination.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerSource[0].DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image 1", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}
		if(IsFail(res = layerSource[1].DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image 2", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}
		if(IsFail(res = layerSource[2].DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image 3", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}
		if(IsFail(res = layerDestination.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image 1 + 2 + 3", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageSource[0].Invalidate(true);
		viewImageSource[1].Invalidate(true);
		viewImageSource[2].Invalidate(true);
		viewImageDestination.Invalidate(true);

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageDestination.ZoomFit()).IsFail())
		{
			ErrorPrint(res, "Failed to zoom fit\n");
			break;
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSource[0].IsAvailable() && viewImageSource[1].IsAvailable() && viewImageSource[2].IsAvailable() && viewImageDestination.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}