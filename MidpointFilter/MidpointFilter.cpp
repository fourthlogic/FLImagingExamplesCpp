#include <cstdio>

#include <FLImaging.h>


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

	do
	{
		// Source 이미지 로드 // Load the source image
		if(IsFail(fliSrcImage.Load(L"../../ExampleImages/NoiseImage/NoiseImage1.flif")))
		{
			printf("Failed to load the image file.\n");
			break;
		}
		
		// Destination이미지를 Src 이미지와 동일한 이미지로 생성 // Create destination image as same as source image
		if(IsFail(fliDstImage.Assign(fliSrcImage)))
		{
			printf("Failed to assign the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create the source image view
		if(IsFail(viewImageSrc.Create(400, 0, 912, 612)))
		{
			printf("Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if(IsFail(viewImageSrc.SetImagePtr(&fliSrcImage)))
		{
			printf("Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create the destination image view
		if(IsFail(viewImageDst.Create(912, 0, 1424, 612)))
		{
			printf("Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if(IsFail(viewImageDst.SetImagePtr(&fliDstImage)))
		{
			printf("Failed to set image object on the image view.\n");
			break;
		}

		// Source이미지 뷰와 Dst 이미지 뷰의 초점을 맞춤
		if(IsFail(viewImageSrc.SynchronizePointOfView(&viewImageDst)))
		{
			printf("Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(viewImageSrc.SynchronizeWindow(&viewImageDst)))
		{
			printf("Failed to synchronize window.\n");
			break;
		}

		// Source Image 크기에 맞게 view의 크기를 조정
		if(IsFail(viewImageSrc.ZoomFit()))
		{
			printf("Failed to zoom fit\n");
			break;
		}

		// Destination Image 크기에 맞게 view의 크기를 조정
		if(IsFail(viewImageDst.ZoomFit()))
		{
			printf("Failed to zoom fit\n");
			break;
		}

		// MidpointFilter 객체 생성 // Create MidpointFilter object
		CMidpointFilter midpointFilter;

		// Source 이미지 설정 // Set the source image
		midpointFilter.SetSourceImage(fliSrcImage);

		// Destination 이미지 설정 // Set the destination image
		midpointFilter.SetDestinationImage(fliDstImage);

		// 처리할 Filter의 Kernel Size 설정 // Set the kernel size of the filter
		midpointFilter.SetKernel(5);

		// Kernel 영역이 Source ROI의 Boundary Rect를 벗어나는 영역에 대한 처리 방법 설정
		// Set the padding method for areas where the kernel area extends beyond the boundary rect of the source ROI.
		midpointFilter.SetPaddingMethod(EPaddingMethod_DecreasingKernel);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(midpointFilter.Execute()))
		{
			printf("Failed to execute midpoint filter.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerDst = viewImageDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerDst.Clear();

		if(IsFail(layerSrc.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)))
		{
			printf("Failed to draw text\n");
			break;
		}

		if(IsFail(layerDst.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 20)))
		{
			printf("Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc.Invalidate(true);
		viewImageDst.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() && viewImageDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}