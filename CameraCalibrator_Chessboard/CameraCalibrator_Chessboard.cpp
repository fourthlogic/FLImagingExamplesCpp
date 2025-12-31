#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

struct SGridDisplay
{
	int64_t i64ImageIdx;
	int64_t i64ObjectIdx;
	AdvancedFunctions::CCameraCalibrator::CCalibratorGridResult sGridData;
};
 
bool Calibration(CCameraCalibrator& sCC, CFLImage& fliLearnImage)
{
	bool bResult = false;

	CResult res;

	do
	{
		// Learn 이미지 설정 // Learn image settings
		if(IsFail(res = sCC.SetCalibrationImage(&fliLearnImage)))
		{
			ErrorPrint(res, L"Failed to set image\n");
			break;
		}

		// Calibator할 대상 종류를 설정합니다. // Set the target type for Calibator.
		sCC.SetGridType(CCameraCalibrator::EGridType_ChessBoard);
		// 결과에 대한 학습률을 설정합니다. // Set the learning rate for the result.
		sCC.SetOptimalSolutionAccuracy();

		// Calibration 실행 // Execute Calibration
		if(IsFail(res = sCC.Calibrate()))
		{
			ErrorPrint(res, L"Calibration failed\n");
			break;
		}

		bResult = true;
	}
	while(false);

	return bResult;
}

bool Undistortion(CCameraCalibrator& sCC, CFLImage& fliSourceImage, CFLImage& fliDestinationImage, CGUIViewImageWrap& viewImageSource, CGUIViewImageWrap& viewImageDestination)
{
	bool bResult = false;

	CResult res;

	do
	{
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

		// Interpolation 알고리즘 설정 // Set the Interpolation Algorithm
		if(IsFail(res = sCC.SetInterpolationMethod(ImageProcessing::EInterpolationMethod_Bilinear)))
		{
			ErrorPrint(res, L"Failed to set interpolation method\n");
			break;
		}
		 
		// Undistortion 실행 // Execute Undistortion
		if(IsFail(res = sCC.Execute()))
		{
			ErrorPrint(res, L"Undistortion failed\n");
			break;
		}

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

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliLearnImage, fliSourceImage, fliDestinationImage;
	CFLImage fliDisplay;
	CFLImage arrFliDisplay[3];

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageLearn[3], viewImageSource, viewImageDestination;

	// Camera Calibrator 객체 생성 // Create Camera Calibrator object
	CCameraCalibrator sCC;
	CResult res;

	do
	{
		// Learn 이미지 로드 // Load the Learn image
		if(IsFail(res = fliLearnImage.Load(L"../../ExampleImages/CameraCalibrator/ChessBoard/Multiple ChessBoard.flif")))
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		printf("Processing.....\n");
		fliDisplay = fliLearnImage;
		fliDisplay.SwapPage(&arrFliDisplay[0], 0);
		fliDisplay.SwapPage(&arrFliDisplay[1], 1);
		fliDisplay.SwapPage(&arrFliDisplay[2], 2);

		for(int32_t i = 0; i < 3; ++i)
		{
			// Learn 이미지 뷰 생성 // Create the Learn image view
			if(IsFail(res = viewImageLearn[i].Create(300 + 480 * i, 0, 300 + 480 * (i + 1), 360)))
			{
				ErrorPrint(res, L"Failed to create the image view.\n");
				break;
			}

			// Learn 이미지 뷰에 이미지를 디스플레이 // Display the image in the Learn image view
			if(IsFail(res = viewImageLearn[i].SetImagePtr(&arrFliDisplay[i])))
			{
				ErrorPrint(res, L"Failed to set image object on the image view.\n");
				break;
			}
		}

		if(!Calibration(sCC, fliLearnImage))
			break;

		// Source 이미지 로드 // Load the source image
		if(IsFail(res = fliSourceImage.Load(L"../../ExampleImages/CameraCalibrator/ChessBoard/UndistortionImage/Undistortion_Image.flif")))
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		if(!Undistortion(sCC, fliSourceImage, fliDestinationImage, viewImageSource, viewImageDestination))
			break;

		// 화면에 격자 탐지 결과 출력 // Output the grid detection result on the screen
		SGridDisplay sArrGridDisplay[3];

		for(int64_t i64ImgIdx = 0; i64ImgIdx < (int64_t)fliLearnImage.GetPageCount(); ++i64ImgIdx)
		{
			int64_t i64ObjectCount = sCC.GetResultGridPointsObjectCnt(i64ImgIdx);

			for(int64_t i64ObjectIdx = 0; i64ObjectIdx < i64ObjectCount; ++i64ObjectIdx)
			{
				SGridDisplay& sGridDisplay = sArrGridDisplay[i64ImgIdx];
				sCC.GetResultGridPoints(i64ObjectIdx, i64ImgIdx, &sGridDisplay.sGridData);
				sGridDisplay.i64ImageIdx = i64ImgIdx;
				sGridDisplay.i64ObjectIdx = sGridDisplay.sGridData.i64ID;
			}
		}

		for(int32_t i = 0; i < 3; ++i)
		{
			CGUIViewImageLayerWrap layerLearn = viewImageLearn[i].GetLayer(0);

			layerLearn.Clear();

			uint32_t colorPool[3] = { RED, LIME, BLUE };

			int64_t i64GridRow = sArrGridDisplay[i].sGridData.i64Rows;
			int64_t i64GridCol = sArrGridDisplay[i].sGridData.i64Columns;
			int64_t i64ColorPoolIdx = 0;

			for(int64_t i64Row = 0; i64Row < i64GridRow; ++i64Row)
			{
				for(int64_t i64Col = 0; i64Col < i64GridCol - 1; ++i64Col)
				{
					int64_t i64GridIdx = i64Row * i64GridCol + i64Col;

					CFLPoint<double>* pFlpGridPoint1 = ((CFLPoint<double>*)((*(CFLFigureArray*)sArrGridDisplay[i].sGridData.flfaGridData[i64Row])[i64Col]));
					CFLPoint<double>* pFlpGridPoint2 = ((CFLPoint<double>*)((*(CFLFigureArray*)sArrGridDisplay[i].sGridData.flfaGridData[i64Row])[i64Col + 1]));
					CFLLine<double> fllDrawLine(pFlpGridPoint1, pFlpGridPoint2);

					if(IsFail(res = layerLearn.DrawFigureImage(fllDrawLine, BLACK, 5)))
					{
						ErrorPrint(res, L"Failed to draw figure\n");
						break;
					}

					if(IsFail(res = layerLearn.DrawFigureImage(fllDrawLine, colorPool[i64GridIdx % 3], 3)))
					{
						ErrorPrint(res, L"Failed to draw figure\n");
						break;
					}


					if(i64Row < i64GridRow - 1)
					{
						CFLPoint<double>* pFlpGridPoint1 = ((CFLPoint<double>*)((*(CFLFigureArray*)sArrGridDisplay[i].sGridData.flfaGridData[i64Row])[i64GridCol - 1]));
						CFLPoint<double>* pFlpGridPoint2 = ((CFLPoint<double>*)((*(CFLFigureArray*)sArrGridDisplay[i].sGridData.flfaGridData[i64Row + 1])[0]));
						CFLLine<double> fllDrawLine(pFlpGridPoint1, pFlpGridPoint2);

						if(IsFail(res = layerLearn.DrawFigureImage(fllDrawLine, BLACK, 5)))
						{
							ErrorPrint(res, L"Failed to draw figure\n");
							break;
						}

						if(IsFail(res = layerLearn.DrawFigureImage(fllDrawLine, YELLOW, 3)))
						{
							ErrorPrint(res, L"Failed to draw figure\n");
							break;
						}
					}
				}
			}

			uint32_t colorText = YELLOW;
			colorPool[2] = CYAN;
			double f64PointDist = 0.;
			double f64Dx = 0.;
			double f64Dy = 0.;

			for(int64_t i64Row = 0; i64Row < i64GridRow; ++i64Row)
			{
				CFLPoint<double>* pFlpGridPoint1 = ((CFLPoint<double>*)((*(CFLFigureArray*)sArrGridDisplay[i].sGridData.flfaGridData[i64Row])[0]));
				CFLPoint<double>* pFlpGridPoint2 = ((CFLPoint<double>*)((*(CFLFigureArray*)sArrGridDisplay[i].sGridData.flfaGridData[i64Row])[1]));
				double f64Angle = pFlpGridPoint1->GetAngle(pFlpGridPoint2);

				for(int64_t i64Col = 0; i64Col < i64GridCol; ++i64Col)
				{
					int64_t i64GridIdx = i64Row * i64GridCol + i64Col;

					if(i64Col < i64GridCol - 1)
					{
						pFlpGridPoint1 = ((CFLPoint<double>*)((*(CFLFigureArray*)sArrGridDisplay[i].sGridData.flfaGridData[i64Row])[i64Col]));
						pFlpGridPoint2 = ((CFLPoint<double>*)((*(CFLFigureArray*)sArrGridDisplay[i].sGridData.flfaGridData[i64Row])[i64Col + 1]));

						f64Dx = pFlpGridPoint2->x - pFlpGridPoint1->x;
						f64Dy = pFlpGridPoint2->y - pFlpGridPoint1->y;
						f64PointDist = sqrt(f64Dx * f64Dx + f64Dy * f64Dy);
					}

					if(i64Row)
					{
						pFlpGridPoint1 = ((CFLPoint<double>*)((*(CFLFigureArray*)sArrGridDisplay[i].sGridData.flfaGridData[i64Row])[i64Col]));
						pFlpGridPoint2 = ((CFLPoint<double>*)((*(CFLFigureArray*)sArrGridDisplay[i].sGridData.flfaGridData[i64Row - 1])[i64Col]));

						f64Dx = pFlpGridPoint2->x - pFlpGridPoint1->x;
						f64Dy = pFlpGridPoint2->y - pFlpGridPoint1->y;
						f64PointDist = std::min<double>(f64PointDist, sqrt(f64Dx * f64Dx + f64Dy * f64Dy));
					}
					else
					{
						pFlpGridPoint1 = ((CFLPoint<double>*)((*(CFLFigureArray*)sArrGridDisplay[i].sGridData.flfaGridData[0])[i64Col]));
						pFlpGridPoint2 = ((CFLPoint<double>*)((*(CFLFigureArray*)sArrGridDisplay[i].sGridData.flfaGridData[1])[i64Col]));

						f64Dx = pFlpGridPoint2->x - pFlpGridPoint1->x;
						f64Dy = pFlpGridPoint2->y - pFlpGridPoint1->y;
						f64PointDist = std::min<double>(f64PointDist, sqrt(f64Dx * f64Dx + f64Dy * f64Dy));
					}

					CFLString<wchar_t> wstrGridIdx;
					wstrGridIdx.Format(L"%d", i64GridIdx);
					colorText = colorPool[i64GridIdx % 3];

					if(i64Col == i64GridCol - 1)
						colorText = YELLOW;

					if(IsFail(res = layerLearn.DrawTextImage(pFlpGridPoint1, wstrGridIdx, colorText, BLACK, (float)(f64PointDist / 2), true, f64Angle)))
					{
						ErrorPrint(res, L"Failed to draw figure\n");
						break;
					}
				}
			}

			CFLQuad<double> flqBoardRegion = sArrGridDisplay[i].sGridData.flqBoardRegion;
			double f64Angle = flqBoardRegion.flpPoints[0].GetAngle(flqBoardRegion.flpPoints[1]);
			CFLString<wchar_t> wstringData = CFLString<wchar_t>().Format(L"[%d] (%d X %d)", (int32_t)sArrGridDisplay[i].i64ObjectIdx, (int32_t)i64GridCol, (int32_t)i64GridRow);

			if(IsFail(res = layerLearn.DrawFigureImage(flqBoardRegion, YELLOW, 3)))
			{
				ErrorPrint(res, L"Failed to draw figure\n");
				break;
			}

			if(IsFail(res = layerLearn.DrawTextImage(flqBoardRegion.flpPoints[0], wstringData, YELLOW, BLACK, 15, false, f64Angle, EGUIViewImageTextAlignment_LEFT_BOTTOM, nullptr, 1.f, 1.f, EGUIViewImageFontWeight_EXTRABOLD)))
			{
				ErrorPrint(res, L"Failed to draw text\n");
				break;
			}

			viewImageLearn[i].Invalidate();
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if(IsFail(res = viewImageSource.Create(300, 360, 780, 720)))
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Creates the Destination image view
		if(IsFail(res = viewImageDestination.Create(780, 360, 1260, 720)))
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

		for(int32_t i = 0; i < 3; ++i)
		{
			// 두 이미지 뷰의 시점을 동기화 한다. // Synchronize the viewpoints of the two image views.
			if(IsFail(res = viewImageLearn[i].SynchronizePointOfView(&viewImageSource)))
			{
				ErrorPrint(res, "Failed to synchronize view\n");
				break;
			}

			// 두 이미지 뷰의 시점을 동기화 한다. // Synchronize the viewpoints of the two image views.
			if(IsFail(res = viewImageLearn[i].SynchronizePointOfView(&viewImageDestination)))
			{
				ErrorPrint(res, "Failed to synchronize view\n");
				break;
			}

			// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
			if(IsFail(res = viewImageLearn[i].SynchronizeWindow(&viewImageSource)))
			{
				ErrorPrint(res, "Failed to synchronize window.\n");
				break;
			}

			// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
			if(IsFail(res = viewImageLearn[i].SynchronizeWindow(&viewImageDestination)))
			{
				ErrorPrint(res, "Failed to synchronize window.\n");
				break;
			}
		}

		// calibration data 출력 // Output calibration data  // Output calibration data
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);
		CCameraCalibrator::CCalibratorIntrinsicParameters sIntrinsicParam = sCC.GetResultIntrinsicParameters();
		CCameraCalibrator::CCalibratorDistortionCoefficients sDistortCoeef = sCC.GetResultDistortionCoefficients();

		CFLString<wchar_t> strMatrix, strDistVal;

		strMatrix.AppendFormat(L"%.2lf, ", sIntrinsicParam.f64FocalLengthX);
		strMatrix.AppendFormat(L"%.2lf, ", sIntrinsicParam.f64Skew);
		strMatrix.AppendFormat(L"%.2lf, ", sIntrinsicParam.f64PrincipalPointX);
		strMatrix.AppendFormat(L"%.2lf, ", 0);
		strMatrix.AppendFormat(L"%.2lf, ", sIntrinsicParam.f64FocalLengthY);
		strMatrix.AppendFormat(L"%.2lf, ", sIntrinsicParam.f64PrincipalPointY);
		strMatrix.AppendFormat(L"%.2lf, ", 0);
		strMatrix.AppendFormat(L"%.2lf, ", 0);
		strMatrix.AppendFormat(L"%.2lf", 1);

		strDistVal.AppendFormat(L"%.2lf, ", sDistortCoeef.f64K1);
		strDistVal.AppendFormat(L"%.2lf, ", sDistortCoeef.f64K2);
		strDistVal.AppendFormat(L"%.2lf, ", sDistortCoeef.f64P1);
		strDistVal.AppendFormat(L"%.2lf, ", sDistortCoeef.f64P2);
		strDistVal.AppendFormat(L"%.2lf", sDistortCoeef.f64K3);

		wprintf(L"Intrinsic parameters : %s\n", strMatrix.GetString());
		wprintf(L"Distortion Coefficients : %s\n", strDistVal.GetString());

		CFLPoint<double> ptIntrTitle(80, 300);
		CFLPoint<double> ptIntr(100, 320);
		CFLPoint<double> ptDistTitle(80, 350);
		CFLPoint<double> ptDist(100, 370);

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

		CGUIViewImageLayerWrap layerDestination = viewImageDestination.GetLayer(0);

		CFLPoint<double> ptTop(20, 20);

		if(IsFail(res = layerDestination.DrawTextImage(&ptTop, L"Undistortion - Bilinear method", GREEN, BLACK, 20)))
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		viewImageSource.Invalidate(true);
		viewImageDestination.Invalidate(true);

	    // 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageLearn[0].IsAvailable() && viewImageLearn[1].IsAvailable() && viewImageLearn[2].IsAvailable() && viewImageSource.IsAvailable() && viewImageDestination.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}