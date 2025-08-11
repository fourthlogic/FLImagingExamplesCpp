#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CFLImage fliSrcImage;
	CFLImage fliCalImage;
	CFLImage fliDstImage;
	CFLImage fliTxtImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageCal;
	CGUIViewImageWrap viewImageTxt;
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
		if((res = viewImageSrc.Create(100, 0, 498, 398)).IsFail())
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

		// Calibration 이미지 로드 // Load the calibration image
		if((res = fliCalImage.Load(L"../../ExampleImages/PhotometricStereo3D/Calibrate.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		fliCalImage.SelectPage(0);

		// Calibration 이미지 뷰 생성 // Create the calibration image view
		if((res = viewImageCal.Create(498, 0, 896, 398)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Calibration 이미지 뷰에 이미지를 디스플레이 // Display the image in the calibration image view
		if((res = viewImageCal.SetImagePtr(&fliCalImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create the destination image view
		if((res = viewImageTxt.Create(100, 398, 498, 796)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if((res = viewImageTxt.SetImagePtr(&fliTxtImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 3D 이미지 뷰 생성 // Create the destination 3D image view
		if((res = view3DDst.Create(896, 0, 1692, 769)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&viewImageCal)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&viewImageTxt)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&view3DDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		viewImageSrc.SetFixThumbnailView(true);

		// PhotometricStereo 객체 생성 // Create PhotometricStereo object
		CPhotometricStereo3D photometricStereo;

		CFL3DObjectHeightMap fl3DOHM;

		// Source 이미지 설정 // Set the source image
		photometricStereo.SetSourceImage(fliSrcImage);
		// Calibration 이미지 설정 // Set the calibration image
		photometricStereo.SetCalibrationImage(fliCalImage);
		// Destination Height Map 이미지 설정 // Set the destination height map image
		photometricStereo.SetDestinationHeightMapImage(fliDstImage);
		// Destination Texture 이미지 설정 // Set the destination texture image
		photometricStereo.SetDestinationTextureImage(fliTxtImage);
		// Destination 3D Object 설정 // Set the destination 3D object 
		photometricStereo.SetDestinationObject(fl3DOHM);
		// Calibration 데이터 설정 // Set Calibration Settings
		photometricStereo.SetCalibrationCircleROI(CFLCircle<double>(117.210526, 104.842105, 78.736842, 0.000000, 0.000000, 360.000000, EArcClosingMethod_EachOther));
		// 동작 방식 설정 // Set operation mode
		photometricStereo.SetReconstructionMode(CPhotometricStereo3D::EReconstructionMode_Poisson_FP32);
		// Valid 픽셀의 기준 설정 // Set valid pixel ratio
		photometricStereo.SetValidPixelThreshold(0.25);

		CMatrix<double> cmatdTemp(3, 3);

		// Angle Degrees 동작 방식으로 설정 // Set operation method as angle degrees
		photometricStereo.SetLightAngleDegrees(cmatdTemp);

		// 알고리즘 Calibration 실행 // Execute calibration of the algorithm
		if((res = photometricStereo.Calibrate()).IsFail())
		{
			ErrorPrint(res, "Failed to calibrate algorithm.\n");
			break;
		}

		// Calibrate 된 Angle Degree 데이터 저장 // Save calibrated angle degree data
		CMultiVar<double> cmvdSlant;
		CMultiVar<double> cmvdTilt;

		photometricStereo.GetLightAngleDegrees(cmvdSlant, cmvdTilt);

		// 위치 데이터 동작 방식으로 설정 // Set operation method as positions
		photometricStereo.SetLightPositions(cmatdTemp);

		// 알고리즘 Calibration 실행 // Execute calibration of the algorithm
		if((res = photometricStereo.Calibrate()).IsFail())
		{
			ErrorPrint(res, "Failed to calibrate algorithm.\n");
			break;
		}

		// Calibrate 된 위치 데이터 저장 // Save calibrated position data
		CMatrix<double> cmatdPosition;

		photometricStereo.GetLightPositions(cmatdPosition);

		// Calibrate를 실행한 결과를 Console창에 출력합니다. // Output the calibration result to the console window.
		int32_t i32CalibPageNum = fliCalImage.GetPageCount();

		// Angle Degrees 데이터 출력
		printf(" < Calibration Angle - Degrees >\n");

		for(int i = 0; i < i32CalibPageNum; i++)
			printf("Image %d ->\tSlant: %0.7lf\tTilt: %0.7lf\n", i, cmvdSlant.GetAt(i), cmvdTilt.GetAt(i));

		printf("\n");

		// Positions 데이터 출력
		printf(" < Calibration Positions >\n");

		for(int i = 0; i < i32CalibPageNum; i++)
			printf("Image %d ->\tX: %0.7lf\tY: %0.7lf \tZ: %0.7lf\n", i, *cmatdPosition.GetValue(i, 0), *cmatdPosition.GetValue(i, 1), *cmatdPosition.GetValue(i, 2));

		// 알고리즘 실행 // Execute algorithm
		if((res = photometricStereo.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to calibrate algorithm.\n");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageTxt.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit of the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerCal = viewImageCal.GetLayer(0);
		CGUIViewImageLayerWrap layerTxt = viewImageTxt.GetLayer(0);
		CGUIView3DLayerWrap layer3D = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerCal.Clear();
		layerTxt.Clear();
		layer3D.Clear();

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

		if((res = layerCal.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Calibration Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerTxt.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Texture Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 3D View 정보 디스필레이 // Display 3D view information
		float f32CenterX = (float)fliSrcImage.GetWidth() / 2;
		float f32CenterY = (float)fliSrcImage.GetHeight() / 2;
		float f32CenterZ = (float)fliDstImage.GetBuffer()[(long)(f32CenterY * fliSrcImage.GetWidth() + f32CenterX)];

		TPoint3<float> tp3dFrom(f32CenterX, f32CenterY, f32CenterZ);

		float f32MulNum = 800.;

		for(long i = 0; i < i32CalibPageNum; i++)
		{
			CFLString<wchar_t> strText;

			strText.Format(L"X: %.4lf    \nY: %.4lf    \nZ: %.4lf\n", *cmatdPosition.GetValue(i, 0), *cmatdPosition.GetValue(i, 1), *cmatdPosition.GetValue(i, 2));

			TPoint3<float> tp3dTo(f32MulNum * (float)*cmatdPosition.GetValue(i, 0) + f32CenterX, f32MulNum * (float)*cmatdPosition.GetValue(i, 1) + f32CenterY, f32MulNum * (float)*cmatdPosition.GetValue(i, 2) + f32CenterZ);

			TPoint3<double> tp3dTod(f32MulNum * *cmatdPosition.GetValue(i, 0) + f32CenterX, f32MulNum * *cmatdPosition.GetValue(i, 1) + f32CenterY, f32MulNum * *cmatdPosition.GetValue(i, 2) + f32CenterZ);

			CGUIView3DObjectLine cgui3dlineTemp(tp3dFrom, tp3dTo, YELLOW, 1);

			layer3D.DrawText3D(tp3dTod, strText, BLACK, YELLOW);
			view3DDst.PushObject(cgui3dlineTemp);
		}

		// Height Map에 Texture Image 설정
		CFL3DObjectHeightMap* pFl3DOHM = dynamic_cast<CFL3DObjectHeightMap*>(photometricStereo.GetDestinationObject());
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
		viewImageTxt.Invalidate(true);
		viewImageCal.Invalidate(true);
		view3DDst.Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림 // Wait for the image and 3D view to close
		while(viewImageSrc.IsAvailable() && viewImageTxt.IsAvailable() && viewImageCal.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}