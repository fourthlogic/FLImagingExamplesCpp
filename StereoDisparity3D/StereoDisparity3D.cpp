#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CFLImage fliSrcImage;
	CFLImage fliSrcImage2;
	CFLImage fliDstImage;
	CFLImage fliTxtImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageSrc2;
	CGUIViewImageWrap viewImageDst;
	CGUIView3DWrap view3DDst;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult res = EResult_UnknownError;

		// Source 이미지 로드 // Load the source image
		if((res = fliSrcImage.Load(L"../../ExampleImages/StereoDisparity3D/Left.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 2 로드 // Load the source image 2
		if((res = fliSrcImage2.Load(L"../../ExampleImages/StereoDisparity3D/Right.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		fliSrcImage.SelectPage(0);
		fliSrcImage2.SelectPage(0);

		// Source 이미지 뷰 생성 // Create the source image view
		if((res = viewImageSrc.Create(100, 0, 548, 448)).IsFail())
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

		// Source 이미지 2 뷰 생성 // Create the source image view 2
		if((res = viewImageSrc2.Create(548, 0, 996, 448)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 2 뷰에 이미지를 디스플레이 // Display the image in the source image view 2
		if((res = viewImageSrc2.SetImagePtr(&fliSrcImage2)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create the destination image view
		if((res = viewImageDst.Create(100, 448, 548, 896)).IsFail())
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

		// Destination 3D 이미지 뷰 생성 // Create the destination 3D image view
		if((res = view3DDst.Create(548, 448, 996, 896)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&viewImageSrc2)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&viewImageDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two view windows
		if((res = viewImageSrc.SynchronizeWindow(&view3DDst)).IsFail())
		{
			ErrorPrint(res,L"Failed to synchronize window.\n");
			break;
		}

		viewImageSrc.SetFixThumbnailView(true);
		viewImageSrc2.SetFixThumbnailView(true);

		// StereoDisparity 객체 생성 // Create StereoDisparity object
		CStereoDisparity3D stereoDisparity;

		CFL3DObjectHeightMap fl3DOHM;

		// Source 이미지 설정 // Set the source image
		stereoDisparity.SetSourceImage(fliSrcImage);
		// Source 이미지 2 설정
		stereoDisparity.SetSourceImage2(fliSrcImage2);
		// Destination Height Map 이미지 설정 // Set the destination height map image
		stereoDisparity.SetDestinationHeightMapImage(fliDstImage);
		// Destination Texture 이미지 설정 // Set the destination texture image
		stereoDisparity.SetDestinationTextureImage(&fliTxtImage);
		// Destination 3D Object 설정 // Set the Destination 3D Object 
		stereoDisparity.SetDestinationObject(fl3DOHM);
		// 최소 허용 Disparity 값 설정 // Set the minimum allowed stereoDisparity value
		stereoDisparity.SetMinimumDisparity(-20);
		// Disparity 범위 설정 // Set the range of stereoDisparity
		stereoDisparity.SetMaximumDisparity(0);
		// Matched Block 크기 설정 // Set the matched block size
		stereoDisparity.SetMatchBlockSize(3);
		// 좌우 간 최대 허용 차이 값 설정 // Set maximum allowed difference value between left and right
		stereoDisparity.SetMaximumDifference(30);
		// 고유비 값 설정 // Set the uniqueness ratio value
		stereoDisparity.SetUniquenessRatio(0);
		// P1 값 설정 // Set P1 Value
		stereoDisparity.SetP1(200);
		// P2 값 설정 // Set P2 Value
		stereoDisparity.SetP2(800);
		// Median Morphology 커널 사이즈 설정 // Set the median morphology kernel size
		stereoDisparity.SetFilterSize(5);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = stereoDisparity.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute algorithm.\n");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageDst.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit of the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerSrc2 = viewImageSrc2.GetLayer(0);
		CGUIViewImageLayerWrap layerDst = viewImageDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerSrc2.Clear();
		layerDst.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if((res = layerSrc.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerSrc2.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image 2", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerDst.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// Height Map에 Texture Image 설정
		CFL3DObjectHeightMap* pFl3DOHM = dynamic_cast<CFL3DObjectHeightMap*>(stereoDisparity.GetDestinationObject());
		pFl3DOHM->SetTextureImage(fliTxtImage);
		pFl3DOHM->ActivateVertexColorTexture(true);

		// 3D 이미지 뷰에 Height Map (Destination Image) 이미지를 디스플레이 // Display the Height Map (Destination Image) on the 3D image view
		if((res = view3DDst.PushObject(*pFl3DOHM)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		view3DDst.ZoomFit();

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc.Invalidate(true);
		viewImageSrc2.Invalidate(true);
		viewImageDst.Invalidate(true);
		view3DDst.Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림 // Wait for the image and 3D view to close
		while(viewImageSrc.IsAvailable() && viewImageSrc2.IsAvailable() && viewImageDst.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}