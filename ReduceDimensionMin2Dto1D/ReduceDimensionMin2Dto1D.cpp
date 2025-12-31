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
	CFLImage fliDestinationImageX;
	CFLImage fliDestinationImageY;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageSource;
	CGUIViewImageWrap viewImageDestinationX;
	CGUIViewImageWrap viewImageDestinationY;

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = fliSourceImage.Load(L"../../ExampleImages/ReduceDimensionMin2Dto1D/Source.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImageSource.Create(100, 0, 612, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageDestinationX.Create(612, 0, 1124, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageDestinationY.Create(1124, 0, 1636, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSource.SynchronizeWindow(&viewImageDestinationX)))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImageSource.SynchronizeWindow(&viewImageDestinationY)))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImageSource.SetImagePtr(&fliSourceImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageDestinationX.SetImagePtr(&fliDestinationImageX)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageDestinationY.SetImagePtr(&fliDestinationImageY)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Reduce Dimension Min 2D to 1D 객체 생성 // Create Reduce Dimension Min 2D to 1D object
		CReduceDimensionMin2Dto1D reduceDimensionMin2Dto1D;

		// Source 이미지 설정 // Set source image 
		reduceDimensionMin2Dto1D.SetSourceImage(fliSourceImage);

		// Destination 이미지 설정 // Set destination image
		reduceDimensionMin2Dto1D.SetDestinationImage(fliDestinationImageX);

		// 축소 차원 설정 // Set reduction dimension
		reduceDimensionMin2Dto1D.SetReductionDimension(CReduceDimensionMin2Dto1D::EReductionDimension_X);

		// 알고리즘 수행 // Execute the algorithm
		if((res = reduceDimensionMin2Dto1D.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Reduce Dimension Min 2D to 1D.\n");

			break;
		}

		// Destination 이미지 설정 // Set destination image
		reduceDimensionMin2Dto1D.SetDestinationImage(fliDestinationImageY);

		// 축소 차원 설정 // Set reduction dimension
		reduceDimensionMin2Dto1D.SetReductionDimension(CReduceDimensionMin2Dto1D::EReductionDimension_Y);

		// 알고리즘 수행 // Execute the algorithm
		if((res = reduceDimensionMin2Dto1D.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Reduce Dimension Min 2D to 1D.\n");

			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);
		CGUIViewImageLayerWrap layerDestinationX = viewImageDestinationX.GetLayer(0);
		CGUIViewImageLayerWrap layerDestinationY = viewImageDestinationY.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerSource.Clear();
		layerDestinationX.Clear();
		layerDestinationY.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDestinationX.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image(X Dimension)", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDestinationY.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image(Y Dimension)", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageSource.Invalidate(true);
		viewImageDestinationX.Invalidate(true);
		viewImageDestinationY.Invalidate(true);

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageDestinationX.ZoomFit()).IsFail())
		{
			ErrorPrint(res, "Failed to zoom fit\n");
			break;
		}

		if((res = viewImageDestinationY.ZoomFit()).IsFail())
		{
			ErrorPrint(res, "Failed to zoom fit\n");
			break;
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSource.IsAvailable() && viewImageDestinationX.IsAvailable() && viewImageDestinationY.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}