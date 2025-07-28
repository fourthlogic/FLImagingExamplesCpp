#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSrcImage;
	CFLImage fliSrcImage2;
	CFLImage fliDstImage;
	CFLImage fliDstImage2;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageSrc2;
	CGUIViewImageWrap viewImageDst;
	CGUIViewImageWrap viewImageDst2;

	// 알고리즘 동작 결과 // Algorithm execution result
	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res = fliSrcImage.Load(L"../../ExampleImages/InpaintingTexture/Seville.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 2 로드 // Load the source image
		if((res = fliSrcImage2.Load(L"../../ExampleImages/InpaintingTexture/Wood.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Destination 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination image as same as source image
		if((res = fliDstImage.Assign(fliSrcImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to assign the image file.\n");
			break;
		}

		// Destination 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination image as same as source image
		if((res = fliDstImage2.Assign(fliSrcImage2)).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create the source image view
		if((res = viewImageSrc.Create(400, 0, 800, 400)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰 2 생성 // Create the source image view 2
		if((res = viewImageSrc2.Create(400, 400, 800, 800)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if((res = viewImageSrc.SetImagePtr(&fliSrcImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Source 이미지 뷰 2에 이미지를 디스플레이 // Display the image in the source image view
		if((res = viewImageSrc2.SetImagePtr(&fliSrcImage2)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create the destination image view
		if((res = viewImageDst.Create(800, 0, 1200, 400)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create the destination image view
		if((res = viewImageDst2.Create(800, 400, 1200, 800)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if((res = viewImageDst.SetImagePtr(&fliDstImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if((res = viewImageDst2.SetImagePtr(&fliDstImage2)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((res = viewImageSrc.SynchronizePointOfView(&viewImageDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&viewImageDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((res = viewImageSrc2.SynchronizePointOfView(&viewImageDst2)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc2.SynchronizeWindow(&viewImageDst2)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&viewImageSrc2)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// InpaintingTexture 객체 생성 // Create InpaintingTexture object
		CInpaintingTexture inpaintingTexture;

		// Source 이미지 설정 // Set the source image
		inpaintingTexture.SetSourceImage(fliSrcImage);
		// Destination 이미지 설정 // Set the destination image
		inpaintingTexture.SetDestinationImage(fliDstImage);
		// Patching을 진행할 Mask의 크기 설정 // Set the size of the mask for patching
		inpaintingTexture.SetMaskSize(13);
		// Patching의 Source가 되는 Mask를 찾기 위한 검색 영역의 크기 설정 // Set the size of the search area to find the mask that is the source of the patching
		inpaintingTexture.SetSearchSize(100);
		// Search step size 설정 // Set the search step size
		inpaintingTexture.SetSearchStepSize(1);
		// 매치를 위한 Gradient Value 곱 계수 설정 // Set a coefficient multiplied by gradient value for match
		inpaintingTexture.SetAnisotropy(1);

		CFLFigureArray flfaPaintingRegion;

		// 미리 그려둔 Painting region Figure Array 불러오기 // Load Pre-drawn Painting Region Figure Array
		if((res = flfaPaintingRegion.Load(L"../../ExampleImages/InpaintingTexture/PaintingRegion.fig")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the figure file.");
			break;
		}

		// Inpainting을 위한 Painting region 설정 // Set painting region for Inpainting
		inpaintingTexture.SetPaintingRegion(&flfaPaintingRegion);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = inpaintingTexture.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute InpaintingTexture.");
			break;
		}

		// Source 이미지 설정 // Set the source image
		inpaintingTexture.SetSourceImage(fliSrcImage2);
		// Destination 이미지 설정 // Set the destination image
		inpaintingTexture.SetDestinationImage(fliDstImage2);
		// Patching을 진행할 Mask의 크기 설정 // Set the size of the mask for patching
		inpaintingTexture.SetMaskSize(15);
		// Patching의 Source가 되는 Mask를 찾기 위한 검색 영역의 크기 설정 // Set the size of the search area to find the mask that is the source of the patching
		inpaintingTexture.SetSearchSize(-1);
		// Search step size 설정 // Set the search step size
		inpaintingTexture.SetSearchStepSize(1);
		// 매치를 위한 Gradient Value 곱 계수 설정 // Set a coefficient multiplied by gradient value for match
		inpaintingTexture.SetAnisotropy(0);

		CFLFigureArray flfaPaintingRegion2;

		// 미리 그려둔 Painting region Figure Array 불러오기 // Load Pre-drawn Painting Region Figure Array
		if((res = flfaPaintingRegion2.Load(L"../../ExampleImages/InpaintingTexture/PaintingRegion2.fig")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the figure file.");
			break;
		}

		// Inpainting을 위한 Painting region 설정 // Set painting region for Inpainting
		inpaintingTexture.SetPaintingRegion(&flfaPaintingRegion2);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = inpaintingTexture.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute InpaintingTexture.");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageDst2.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit of the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerSrc2 = viewImageSrc2.GetLayer(0);
		CGUIViewImageLayerWrap layerDst = viewImageDst.GetLayer(0);
		CGUIViewImageLayerWrap layerDst2 = viewImageDst2.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerSrc2.Clear();
		layerDst.Clear();
		layerDst2.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if((res = layerSrc.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerSrc2.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image 2", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerDst.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerDst2.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Image 2", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		// Painting region을 source image에 디스플레이 // Display painting region on the source image
		if(fliSrcImage.PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(flfaPaintingRegion)) == -1)
		{
			ErrorPrint(res, L"Failed to push figure on image\n");
			break;
		}

		// Painting region을 source image에 디스플레이 // Display painting region on the source image
		if(fliSrcImage2.PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(flfaPaintingRegion2)) == -1)
		{
			ErrorPrint(res, L"Failed to push figure on image\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc.Invalidate(true);
		viewImageDst.Invalidate(true);
		viewImageSrc2.Invalidate(true);
		viewImageDst2.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() && viewImageDst.IsAvailable() && viewImageSrc2.IsAvailable() && viewImageDst2.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}