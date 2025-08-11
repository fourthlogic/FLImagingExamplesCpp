#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CFLImage fliSrcImage;
	CFLImage fliDstImage;
	CFLImage fliCurImage;
	CFLImage fliTxtImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageTexture;
	CGUIViewImageWrap viewImageCurvature;
	CGUIView3DWrap view3DDst;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult res = EResult_UnknownError;

		// Source 이미지 로드 // Load the source image
		if((res = fliSrcImage.Load(L"../../ExampleImages/PhotometricStereo3D/Source.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		fliSrcImage.SelectPage(0);

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

		// Destination texture 이미지 뷰 생성 // Create the destination texture image view
		if((res = viewImageTexture.Create(100, 448, 548, 896)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination texture 이미지 뷰에 이미지를 디스플레이 // Display the texture image in the destination texture image view
		if((res = viewImageTexture.SetImagePtr(&fliTxtImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination curvature 이미지 뷰 생성 // Create the destination curvature image view
		if((res = viewImageCurvature.Create(548, 0, 996, 448)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination curvature 이미지 뷰에 이미지를 디스플레이 // Display the curvature image in the destination curvature image view
		if((res = viewImageCurvature.SetImagePtr(&fliCurImage)).IsFail())
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
		if((res = viewImageSrc.SynchronizeWindow(&viewImageTexture)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&viewImageCurvature)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two view windows
		if((res = viewImageSrc.SynchronizeWindow(&view3DDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		viewImageSrc.SetFixThumbnailView(true);

		// PhotometricStereo 객체 생성 // Create PhotometricStereo object
		CPhotometricStereo3D photometricStereo3D;

		CFL3DObjectHeightMap fl3DOHM;

		// Source 이미지 설정 // Set the source image
		photometricStereo3D.SetSourceImage(fliSrcImage);
		// Destination Height Map 이미지 설정 // Set the destination height map image
		photometricStereo3D.SetDestinationHeightMapImage(fliDstImage);
		// Destination Texture 이미지 설정 // Set the destination texture image
		photometricStereo3D.SetDestinationTextureImage(&fliTxtImage);
		// Destination Curvature 이미지 설정 // Set the destination curvature image
		photometricStereo3D.SetCurvatureImage(fliCurImage);
		// Destination 3D Object 설정 // Set the Destination 3D Object 
		photometricStereo3D.SetDestinationObject(fl3DOHM);
		// 동작 방식 설정 // Set Operation Mode
		photometricStereo3D.SetReconstructionMode(CPhotometricStereo3D::EReconstructionMode_Poisson_FP32);
		// Valid 픽셀의 기준 설정 // Set valid pixel ratio
		photometricStereo3D.SetValidPixelThreshold(0.25);
		// Curvature 이미지 Normalization 여부 설정 // Set curvature image normalization option
		photometricStereo3D.EnableCurvatureNormalization(true);
		
		// 각 이미지의 광원 Slant 값 입력
		CMultiVar<double> mvdSlant;

		mvdSlant.PushBack(39.831506);
		mvdSlant.PushBack(28.682381);
		mvdSlant.PushBack(20.989625);
		mvdSlant.PushBack(19.346638);
		mvdSlant.PushBack(20.785800);
		mvdSlant.PushBack(26.005273);
		mvdSlant.PushBack(19.038004);
		mvdSlant.PushBack(9.253585);
		mvdSlant.PushBack(16.425454);
		mvdSlant.PushBack(23.712574);
		mvdSlant.PushBack(26.003058);
		mvdSlant.PushBack(19.069500);
		mvdSlant.PushBack(11.801071);
		mvdSlant.PushBack(20.484473);
		mvdSlant.PushBack(25.909730);
		mvdSlant.PushBack(43.055332);
		mvdSlant.PushBack(39.043981);
		mvdSlant.PushBack(30.041029);
		mvdSlant.PushBack(26.067657);
		mvdSlant.PushBack(26.126303);

		// 각 이미지의 광원 Tilt 값 입력
		CMultiVar<double> mvdTilt;

		mvdTilt.PushBack(123.359091);
		mvdTilt.PushBack(123.952892);
		mvdTilt.PushBack(154.836215);
		mvdTilt.PushBack(-173.353324);
		mvdTilt.PushBack(-147.483507);
		mvdTilt.PushBack(109.497340);
		mvdTilt.PushBack(115.825606);
		mvdTilt.PushBack(-169.019112);
		mvdTilt.PushBack(-119.343654);
		mvdTilt.PushBack(-109.319167);
		mvdTilt.PushBack(66.944279);
		mvdTilt.PushBack(48.136896);
		mvdTilt.PushBack(-5.157068);
		mvdTilt.PushBack(-54.033519);
		mvdTilt.PushBack(-66.856636);
		mvdTilt.PushBack(60.456870);
		mvdTilt.PushBack(53.388008);
		mvdTilt.PushBack(36.447691);
		mvdTilt.PushBack(13.056294);
		mvdTilt.PushBack(-5.976723);

		photometricStereo3D.SetLightAngleDegrees(mvdSlant, mvdTilt);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = photometricStereo3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute algorithm.\n");
			break;
		}

		// Destination texture 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination texture image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageTexture.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit of the image view.\n");
			break;
		}

		// Destination curvature 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination curvature image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageCurvature.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit of the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerTxt = viewImageTexture.GetLayer(0);
		CGUIViewImageLayerWrap layerCur = viewImageCurvature.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerTxt.Clear();
		layerCur.Clear();

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

		if((res = layerTxt.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Texture Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerCur.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Curvature Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// Height Map에 Texture Image 설정
		CFL3DObjectHeightMap* pFl3DOHM = dynamic_cast<CFL3DObjectHeightMap*>(photometricStereo3D.GetDestinationObject());
		pFl3DOHM->SetTextureImage(fliTxtImage);
		pFl3DOHM->ActivateVertexColorTexture(false);

		// 3D 이미지 뷰에 Height Map (Destination Image) 이미지를 디스플레이 // Display the Height Map (Destination Image) on the 3D image view
		if((res = view3DDst.PushObject(*pFl3DOHM)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		view3DDst.ZoomFit();

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc.Invalidate(true);
		viewImageTexture.Invalidate(true);
		viewImageCurvature.Invalidate(true);
		view3DDst.Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림 // Wait for the image and 3D view to close
		while(viewImageSrc.IsAvailable() && viewImageTexture.IsAvailable() && viewImageCurvature.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}