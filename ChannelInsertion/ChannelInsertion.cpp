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
	CFLImage fliInsertionImage[2];
	CFLImage fliDestinationImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageSource;
	CGUIViewImageWrap viewImageInsertion[2];
	CGUIViewImageWrap viewImageDestination;

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = fliSourceImage.Load(L"../../ExampleImages/ChannelInsertion/Valley1.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliInsertionImage[0].Load(L"../../ExampleImages/ChannelInsertion/Valley2.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliInsertionImage[1].Load(L"../../ExampleImages/ChannelInsertion/Valley3.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImageSource.Create(100, 0, 100 + 440, 340)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageInsertion[0].Create(100 + 440, 0, 100 + 440 * 2, 340)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageInsertion[1].Create(100 + 440 * 2, 0, 100 + 440 * 3, 340)))
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
		if(IsFail(res = viewImageSource.SynchronizePointOfView(&viewImageInsertion[0])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImageInsertion[0].SynchronizePointOfView(&viewImageInsertion[1])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImageInsertion[1].SynchronizePointOfView(&viewImageDestination)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSource.SynchronizeWindow(&viewImageInsertion[0])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageInsertion[0].SynchronizeWindow(&viewImageInsertion[1])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageInsertion[1].SynchronizeWindow(&viewImageDestination)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImageSource.SetImagePtr(&fliSourceImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageInsertion[0].SetImagePtr(&fliInsertionImage[0])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageInsertion[1].SetImagePtr(&fliInsertionImage[1])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageDestination.SetImagePtr(&fliDestinationImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Channel Insertion 객체 생성 // Create Channel Insertion object
		CChannelInsertion channelInsertion;

		// Source 이미지를 저장할 Array 선언 // Declare an Array to store the source image
		Base::CFLArray<CFLImage*> vctInsertionImages;

		// 추출할 채널을 저장할 Array 선언 // Declare an Array to extract the channels
		Base::CFLArray<int64_t> vctInsertionChannels;

		// 삽입할 색인을 저장할 Array 선언 // Declare an Array to store the indices
		Base::CFLArray<int64_t> vctInsertionIndices;

		// Source 이미지 입력 // Push source images 
		vctInsertionImages.PushBack(&fliInsertionImage[0]);
		vctInsertionImages.PushBack(&fliInsertionImage[1]);

		// 삽입 이미지별 추출할 채널을 입력 // Push Insertion channels to extract 
		vctInsertionChannels.PushBack(EChannelSelection_Channel_0);
		vctInsertionChannels.PushBack(EChannelSelection_Channel_0);

		// 추출한 채널을 삽입할 색인 입력 // Push Insertion indices to insert
		vctInsertionIndices.PushBack(0);
		vctInsertionIndices.PushBack(1);

		// 소스 이미지 설정 // Set Source image
		channelInsertion.SetSourceImage(fliSourceImage);

		// 삽입할 이미지, 추출할 채널, 삽입 할 색인 설정 // Set insertion images, insertion channels, and insertion indices
		channelInsertion.SetInsertionImage(vctInsertionImages, vctInsertionChannels, vctInsertionIndices);

		// 삽입 결과를 저장할 이미지 설정 // Set destination image
		channelInsertion.SetDestinationImage(fliDestinationImage);

		// 알고리즘 수행 // Execute the algorithm
		if((res = channelInsertion.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Channel Insertion.");
			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSource[3];
		CGUIViewImageLayerWrap layerDestination = viewImageDestination.GetLayer(1);
		layerSource[0] = viewImageSource.GetLayer(0);
		layerSource[1] = viewImageInsertion[0].GetLayer(0);
		layerSource[2] = viewImageInsertion[1].GetLayer(0);

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
		if(IsFail(res = layerSource[0].DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}
		if(IsFail(res = layerSource[1].DrawTextCanvas(CFLPoint<double>(0, 0), L"Insertion Image 1", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}
		if(IsFail(res = layerSource[2].DrawTextCanvas(CFLPoint<double>(0, 0), L"Insertion Image 2", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}
		if(IsFail(res = layerDestination.DrawTextCanvas(CFLPoint<double>(0, 0), L"Insertion Image 1 +\nSource Image +\nInsertion Image 2", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageSource.Invalidate(true);
		viewImageInsertion[0].Invalidate(true);
		viewImageInsertion[1].Invalidate(true);
		viewImageDestination.Invalidate(true);

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageDestination.ZoomFit()).IsFail())
		{
			ErrorPrint(res, "Failed to zoom fit\n");
			break;
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSource.IsAvailable() && viewImageInsertion[0].IsAvailable() && viewImageInsertion[1].IsAvailable() && viewImageDestination.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}