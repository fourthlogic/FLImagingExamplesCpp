#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

bool Undistortion(CCameraCalibrator& sCC, CFLImage& fliSourceImage, CFLImage& fliDestinationImage, CGUIViewImageWrap& viewImageSource, CGUIViewImageWrap& viewImageDestination)
{
	bool bResult = false;
	
	CResult res;

	do
	{
		// Source 이미지 로드 // Load the source image
		if(IsFail(res = fliSourceImage.Load(L"../../ExampleImages/CameraCalibrator/Undistortion.flif")))
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Destination 이미지 생성 // Create destination image
		if(IsFail(res = fliDestinationImage.Create(fliSourceImage.GetWidth(), fliSourceImage.GetHeight(), CMultiVarLL(0), fliSourceImage.GetPixelFormat())))
		{
			ErrorPrint(res, L"Failed to create the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if(IsFail(res = viewImageSource.Create(400, 480, 1040, 960)))
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Creates the Destination image view
		if(IsFail(res = viewImageDestination.Create(1040, 480, 1680, 960)))
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the Source ImageView
		if(IsFail(res = viewImageSource.SetImagePtr(&fliSourceImage)))
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the Destination image view
		if(IsFail(res = viewImageDestination.SetImagePtr(&fliDestinationImage)))
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Source 이미지 설정 // Set Source image
		if(IsFail(res = sCC.SetSourceImage(&fliSourceImage)))
		{
			ErrorPrint(res, L"Failed to Loads image\n");
			break;
		}
		 
		// Destination 이미지 설정 // Set destination image
		if(IsFail(res = sCC.SetDestinationImage(&fliDestinationImage)))
		{
			ErrorPrint(res, L"Failed to Loads image\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSource.SynchronizeWindow(&viewImageDestination)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Interpolation 알고리즘 설정 // Set the Interpolation Algorithm
		if(IsFail(res = sCC.SetInterpolationMethod(ImageProcessing::EInterpolationMethod_Bilinear)))
		{
			ErrorPrint(res, L"Failed to set interpolation method\n");
			break;
		}

		// 소요시간 측정 초기화 // Initialize time measurement
		CPerformanceCounter sPC;
		sPC.Start();

		// Undistortion 실행 // Execute Undistortion
		if(IsFail(res = sCC.Execute()))
		{
			ErrorPrint(res, L"Undistortion failed\n");
			break;
		}

		sPC.CheckPoint();

		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);

		layerSource.Clear();

		CGUIViewImageLayerWrap layerDestination = viewImageDestination.GetLayer(0);

		layerDestination.Clear();
		
		CFLPoint<double> ptTop(20,20);

		if(IsFail(res = layerDestination.DrawTextImage(&ptTop, L"Undistortion - Bilinear method", GREEN, BLACK, 20)))
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		// 소요시간 출력 // Output time required
		double f64ElapsedMS = sPC.GetCheckPointInMilliSecond();

		CFLString<wchar_t> strMS;
		strMS.Format(L"elapsed time: %.2lf ms", f64ElapsedMS);
		CFLPoint<double> ptMS(20, 50);
		
		if(IsFail(res = layerDestination.DrawTextImage(&ptMS, strMS.GetBuffer(), GREEN, BLACK, 20)))
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		viewImageDestination.Invalidate(false);

		bResult = true;
	}
	while(false);

	return bResult;
}

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CResult res;

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliSourceImage, fliDestinationImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageSource, viewImageDestination;

	// Camera Calibrator 객체 생성 // Create Camera Calibrator object
	CCameraCalibrator sCC;

	do
	{
		// Setter로 입력 // Input as setter
		double arrF64Intrinc[9] = { 605.9413643192689, 0, 325.9133439121233, 0, 605.3834974915350, 234.0647625697701, 0, 0, 1 };
		double arrF64Dist[5] = { 0.1748895907714, -1.4909467274276, -0.0070404809103, 0.0017880490098, 5.9363069879613 };

		CCameraCalibrator::CCalibratorIntrinsicParameters sIntrinc;
		sIntrinc.f64FocalLengthX = arrF64Intrinc[0];
		sIntrinc.f64Skew = arrF64Intrinc[1];
		sIntrinc.f64PrincipalPointX = arrF64Intrinc[2];
		sIntrinc.f64FocalLengthY = arrF64Intrinc[4];
		sIntrinc.f64PrincipalPointY = arrF64Intrinc[5];
		
		CCameraCalibrator::CCalibratorDistortionCoefficients sDist;
		sDist.f64K1 = arrF64Dist[0];
		sDist.f64K2 = arrF64Dist[1];
		sDist.f64P1 = arrF64Dist[2];
		sDist.f64P2 = arrF64Dist[3];
		sDist.f64K3 = arrF64Dist[4];

		if(IsFail(res = sCC.SetIntrinsicParameters(sIntrinc)))
		{
			ErrorPrint(res, L"Failed to set intrinsic parameters\n");
			break;
		}
			
		if(IsFail(res = sCC.SetDistortionCoefficients(sDist)))
		{
			ErrorPrint(res, L"Failed to set distortion coefficients\n");
			break;
		}

		if(IsFail(res = sCC.EnableAutoCalibration(false)))
		{
			ErrorPrint(res, L"Failed to auto calibration\n");
			break;
		}

		if(IsFail(res = sCC.Calibrate()))
		{
			ErrorPrint(res, L"Failed to calibration\n");
			break;
		}

		if(!Undistortion(sCC, fliSourceImage, fliDestinationImage, viewImageSource, viewImageDestination))
			break;
		
		CFLString<wchar_t> strMatrix, strDistVal;

		for(int32_t i32I = 0; i32I < 9; i32I++)
		{
			strMatrix.AppendFormat(L"%.2lf", arrF64Intrinc[i32I]);
			if(i32I < 8)
				strMatrix.Append(L", ");
		}

		for(int32_t i32I = 0; i32I < 5; i32I++)
		{
			strDistVal.AppendFormat(L"%.2lf", arrF64Dist[i32I]);
			if(i32I < 4)
				strDistVal.Append(L", ");
		}

		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);

		// 이미지 뷰 정보 표시 // Display image view information
		if(IsFail(res = layerSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Intrinsic Parameters: ", YELLOW, BLACK, 13)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerSource.DrawTextCanvas(&CFLPoint<double>(0, 20), strMatrix, YELLOW, BLACK, 13)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerSource.DrawTextCanvas(&CFLPoint<double>(0, 40), L"Distortion Coefficients: ", YELLOW, BLACK, 13)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerSource.DrawTextCanvas(&CFLPoint<double>(0, 60), strDistVal, YELLOW, BLACK, 13)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		wprintf(L"Intrinsic parameters : %s\n", strMatrix.GetString());
		wprintf(L"Distortion Coefficients : %s\n", strDistVal.GetString());

		viewImageSource.Invalidate();

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSource.IsAvailable() && viewImageDestination.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}