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
	CGUIViewImageWrap viewImageSource;
	CGUIViewImageWrap viewImageDestination;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// 이미지 로드 // Load image
		if(IsFail(res = fliSourceImage.Load(L"../../ExampleImages/Perspective/calendar.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliDestinationImage.Load(L"../../ExampleImages/Perspective/space.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if(IsFail(res = viewImageSource.Create(400, 0, 912, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create destination image view
		if(IsFail(res = viewImageDestination.Create(912, 0,1424, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if(IsFail(res = viewImageSource.SetImagePtr(&fliSourceImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if(IsFail(res = viewImageDestination.SetImagePtr(&fliDestinationImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSource.SynchronizeWindow(&viewImageDestination)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Perspective 객체 생성 // Create Perspective object
		CPerspective perspective;

		// Source 이미지 설정 // Set the source image
		perspective.SetSourceImage(fliSourceImage);

		// Source 이미지의 투영 영역 범위 설정 // Set the range of the projection area of the Source image
		CFLQuad<double> flqSourceProjection(290.87, 65.73, 531.69, 192.5, 169.68, 406.66, 34.59, 170.22);

		// Source 이미지의 투영 영역 지정 // Set the projection area of the Source image
		perspective.SetSourceProjection(flqSourceProjection);

		// Destination 이미지 설정 // Set the destination image
		perspective.SetDestinationImage(fliDestinationImage);

		// Destination 이미지의 출력 대상 영역 범위 설정 // Set the output destination area range of Destination image
		CFLCircle<int32_t> flcDestinationROI(243, 261, 188, 0, 0, 360, EArcClosingMethod_EachOther);

		// Destination 이미지의 출력 대상 영역 지정 // Destination Specify the output target area of the image
		perspective.SetDestinationROI(flcDestinationROI);

		// Destination 이미지의 투영 영역 범위 설정 // Set the range of the projection area of the destination image
		CFLRect<int32_t> flrDestinationProjection(192, 208, 332, 346);

		// Destination 이미지의 투영 영역 지정 // Set the projection area of the destination image
		perspective.SetDestinationProjection(flrDestinationProjection);

		// 보간법 설정 (Bicubic / Bilinear / NearestNeighbor) // Set interpolation method (Bicubic / Bilinear / NearestNeighbor)
		perspective.SetInterpolationMethod(EInterpolationMethod_Bicubic);

		// 공백 영역 색상 값 설정 // Set blank area color value
		CMultiVar<double> mvBlankColor(10, 160, 20);

		// 공백 영역 색상 지정 // color the blank area
		perspective.SetBlankColor(mvBlankColor);

		// 항상 공백 영역을 지정한 색으로 채우도록 설정 // Always set blank areas to be filled with the specified color
		perspective.EnableFillBlankColorMode(true);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = perspective.Execute()))
		{
			ErrorPrint(res, "Failed to execute perspective.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination = viewImageDestination.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSource.Clear();
		layerDestination.Clear();

		// FLImaging의 Figure객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능 // FLImaging's figure objects can be displayed with a single function, regardless of the shape of the figure
		// Source Projection 영역이 어디인지 알기 위해 디스플레이 한다 // Display to know where the Source Projection area is
		if(IsFail(res = layerSource.DrawFigureImage(&flqSourceProjection, LIME, 3)))
			ErrorPrint(res, "Failed to draw figure\n");

		// Destination Projection 영역이 어디인지 알기 위해 디스플레이한다. // Display to know where the Destination Projection area is.
		if(IsFail(res = layerDestination.DrawFigureImage(&flrDestinationProjection, LIME, 3)))
			ErrorPrint(res, "Failed to draw figure\n");

		// Destination ROI 영역이 어디인지 알기 위해 디스플레이한다. // Display to know where the Destination ROI area is.
		if(IsFail(res = layerDestination.DrawFigureImage(&flcDestinationROI, RED, 3)))
			ErrorPrint(res, "Failed to draw figure\n");

		// 이미지 뷰 정보 표시 // Display image view information
		if(IsFail(res = layerSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDestination.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		viewImageSource.Invalidate(true);
		viewImageDestination.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSource.IsAvailable() && viewImageDestination.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}