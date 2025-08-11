#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

struct SGridDisplay
{
	int64_t i64ImageIdx;
	CStereoCalibrator3D::SGridResult sGridData;
};

CResult DrawGridPoints(SGridDisplay& sGridDisplay, CGUIViewImageLayerWrap pLayer)
{
	CResult res = EResult_UnknownError;

	do
	{
		if(!sGridDisplay.sGridData.arrGridData.GetCount())
		{
			res = EResult_NoData;
			break;
		}

		// 그리기 색상 설정 // Set drawing color
		uint32_t colorPool[3] = { RED, LIME, CYAN };

		int64_t i64GridRow = sGridDisplay.sGridData.i64Rows;
		int64_t i64GridCol = sGridDisplay.sGridData.i64Columns;
		double f64AvgDistance = sGridDisplay.sGridData.f64AvgDistance;
		CFLQuad<double> flqBoardRegion = sGridDisplay.sGridData.flqBoardRegion;
		double f64Angle = flqBoardRegion.flpPoints[0].GetAngle(flqBoardRegion.flpPoints[1]);
		double f64Width = flqBoardRegion.flpPoints[0].GetDistance(flqBoardRegion.flpPoints[1]);

		// Grid 그리기 // Draw grid
		for(int64_t i64Row = 0; i64Row < i64GridRow; ++i64Row)
		{
			for(int64_t i64Col = 0; i64Col < i64GridCol - 1; ++i64Col)
			{
				int64_t i64GridIdx = i64Row * i64GridCol + i64Col;
				CFLPoint<double> flpGridPoint1 = (sGridDisplay.sGridData.arrGridData[i64Row][i64Col]);
				CFLPoint<double> flpGridPoint2 = (sGridDisplay.sGridData.arrGridData[i64Row][i64Col + 1]);
				CFLLine<double> fllDrawLine(flpGridPoint1, flpGridPoint2);
				pLayer.DrawFigureImage(fllDrawLine, BLACK, 5);
				pLayer.DrawFigureImage(fllDrawLine, colorPool[i64GridIdx % 3], 3);
			}

			if(i64Row < i64GridRow - 1)
			{
				CFLPoint<double> flpGridPoint1 = (sGridDisplay.sGridData.arrGridData[i64Row][i64GridCol - 1]);
				CFLPoint<double> flpGridPoint2 = (sGridDisplay.sGridData.arrGridData[i64Row + 1][0]);
				CFLLine<double> fllDrawLine(flpGridPoint1, flpGridPoint2);
				pLayer.DrawFigureImage(fllDrawLine, BLACK, 5);
				pLayer.DrawFigureImage(fllDrawLine, YELLOW, 3);
			}
		}

		uint32_t colorText = YELLOW;
		double f64PointDist = 0.;
		double f64Dx = 0.;
		double f64Dy = 0.;

		// Grid Point 인덱싱 // Index Grid Point
		for(int64_t i64Row = 0; i64Row < i64GridRow; ++i64Row)
		{
			CFLPoint<double> flpGridPoint1 = (sGridDisplay.sGridData.arrGridData[i64Row][0]);
			CFLPoint<double> flpGridPoint2 = (sGridDisplay.sGridData.arrGridData[i64Row][1]);
			double f64Angle = flpGridPoint1.GetAngle(flpGridPoint2);

			for(int64_t i64Col = 0; i64Col < i64GridCol; ++i64Col)
			{
				int64_t i64GridIdx = i64Row * i64GridCol + i64Col;

				if(i64Col < i64GridCol - 1)
				{
					flpGridPoint1 = (sGridDisplay.sGridData.arrGridData[i64Row][i64Col]);
					flpGridPoint2 = (sGridDisplay.sGridData.arrGridData[i64Row][i64Col + 1]);

					f64Dx = flpGridPoint2.x - flpGridPoint1.x;
					f64Dy = flpGridPoint2.y - flpGridPoint1.y;
					f64PointDist = sqrt(f64Dx * f64Dx + f64Dy * f64Dy);
				}

				if(i64Row)
				{
					flpGridPoint1 = (sGridDisplay.sGridData.arrGridData[i64Row][i64Col]);
					flpGridPoint2 = (sGridDisplay.sGridData.arrGridData[i64Row - 1][i64Col]);

					f64Dx = flpGridPoint2.x - flpGridPoint1.x;
					f64Dy = flpGridPoint2.y - flpGridPoint1.y;
					f64PointDist = std::min<double>(f64PointDist, sqrt(f64Dx * f64Dx + f64Dy * f64Dy));
				}
				else
				{
					flpGridPoint1 = (sGridDisplay.sGridData.arrGridData[0][i64Col]);
					flpGridPoint2 = (sGridDisplay.sGridData.arrGridData[1][i64Col]);

					f64Dx = flpGridPoint2.x - flpGridPoint1.x;
					f64Dy = flpGridPoint2.y - flpGridPoint1.y;
					f64PointDist = std::min<double>(f64PointDist, sqrt(f64Dx * f64Dx + f64Dy * f64Dy));
				}

				CFLString<wchar_t> wstrGridIdx;
				wstrGridIdx.Format(L"%d", i64GridIdx);
				colorText = colorPool[i64GridIdx % 3];

				if(i64Col == i64GridCol - 1)
					colorText = YELLOW;

				pLayer.DrawTextImage(flpGridPoint1, wstrGridIdx, colorText, BLACK, (float)(int32_t)(f64PointDist / 2), true, f64Angle);
			}
		}

		// Board Region 그리기 // Draw Board Region
		CFLString<wchar_t> wstringData = CFLString<wchar_t>().Format(L"(%d X %d)", (int32_t)i64GridCol, (int32_t)i64GridRow);
		pLayer.DrawFigureImage(flqBoardRegion, BLACK, 3);
		pLayer.DrawFigureImage(flqBoardRegion, YELLOW, 1);
		pLayer.DrawTextImage(flqBoardRegion.flpPoints[0], wstringData, YELLOW, BLACK, (float)(int32_t)(f64Width / 16), true, f64Angle, EGUIViewImageTextAlignment_LEFT_BOTTOM, nullptr, 1.f, 1.f, EGUIViewImageFontWeight_EXTRABOLD);

		res = EResult_OK;
	}
	while(false);

	return res;
}

class CMessageReceiver : public CFLBase
{
public:

	// CMessageReceiver 생성자
	CMessageReceiver(CGUIViewImageWrap* pViewImage) : m_pViewImage(pViewImage)
	{
		// 메세지를 전달 받기 위해 CBroadcastManager 에 구독 등록 //Subscribe to CBroadcast Manager to receive messages
		CBroadcastManager::Subscribe(this);
	}

	// CMessageReceiver 소멸자
	virtual ~CMessageReceiver()
	{
		// 객체가 소멸할때 메세지 수신을 중단하기 위해 구독을 해제한다. // Unsubscribe to stop receiving messages when the object disappears.
		CBroadcastManager::Unsubscribe(this);
	}

	DeclareGetClassType();

	// 메세지가 들어오면 호출되는 함수 OnReceiveBroadcast 오버라이드 하여 구현
	void OnReceiveBroadcast(const CBroadcastMessage* pMessage) override
	{
		do
		{
			// pMessage 가 nullptr 인지 확인 // Verify message is nullptr
			if(pMessage == nullptr)
				break;

			// 메세지의 채널을 확인
			switch(pMessage->GetChannel())
			{
			case EGUIBroadcast_ViewImage_PostPageChange:
				{
			        // GetCaller() 가 등록한 이미지뷰인지 확인 // Verify that GetCaller() is a registered image view
					if(pMessage->GetCaller() != (const CFLBase*)m_pViewImage->GetMessageCallerPtr())
						break;

					CFLImage* pFliLearnImage = (CFLImage*)m_pViewImage->GetImagePtr();
					int64_t i64CurPage = pFliLearnImage->GetSelectedPageIndex();

					// 이미지뷰의 0번 레이어 가져오기 // Get layer 0th of image view
					CGUIViewImageLayerWrap pLayer = m_pViewImage->GetLayer((int32_t)(i64CurPage % 10));

					for(int64_t i = 0; i < 10; ++i)
						m_pViewImage->GetLayer((int32_t)i).Clear();

					for(int64_t i64Idx = 0; i64Idx < (int64_t)pFliLearnImage->GetPageCount(); ++i64Idx)
					{
						if(m_vctGridDisplay[(int32_t)i64Idx].i64ImageIdx == i64CurPage)
							DrawGridPoints(m_vctGridDisplay[(int32_t)i64Idx], pLayer);
					}

					// 이미지뷰를 갱신 // Update the image view.
					m_pViewImage->Invalidate();
				}
				break;
			}
		}
		while(false);
	}

	SGridDisplay* m_vctGridDisplay;

protected:
	CGUIViewImageWrap* m_pViewImage;
};

bool Calibration(CStereoCalibrator3D& stereoCalibrator3D, CFLImage& fliLearnImage, CFLImage& fliLearnImage2)
{
	bool bResult = false;

	// 결과 enum 선언 // Declare the result enum
	CResult res;

	do
	{
		// Learn 이미지 설정 // Declare the learn image
		if((res = stereoCalibrator3D.SetLearnImage(&fliLearnImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image.\n");
			break;
		}

		// Learn 이미지 2 설정 // Declare the learn image
		if((res = stereoCalibrator3D.SetLearnImage2(&fliLearnImage2)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image.\n");
			break;
		}

		// Optimal solution accuracy 설정 // Set optimal solution accuracy
		if((res = stereoCalibrator3D.SetOptimalSolutionAccuracy(1e-5)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Optimal Solution Accuracy.\n");
			break;
		}

		// Grid Type 설정 // Set grid type
		if((res = stereoCalibrator3D.SetGridType(CStereoCalibrator3D::EGridType_ChessBoard)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Grid Type.\n");
			break;
		}

		// Calibration 실행 // Execute Calibration
		if((res = stereoCalibrator3D.Calibrate()).IsFail())
		{
			ErrorPrint(res, L"Calibration failed.\n");
			break;
		}

		bResult = true;
	}
	while(false);

	return bResult;
}

bool Undistortion(CStereoCalibrator3D& stereoCalibrator3D, CFLImage& fliSourceImage, CFLImage& fliSourceImage2, CFLImage& fliDestinationImage, CFLImage& fliDestinationImage2)
{
	bool bResult = false;

	// 결과 enum 선언
	CResult res;

	do
	{
		// Source 이미지 설정 // Set source image
		if((res = stereoCalibrator3D.SetSourceImage(&fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to Loads image.\n");
			break;
		}

		// Source 이미지 2 설정 // Set source image 2
		if((res = stereoCalibrator3D.SetSourceImage2(&fliSourceImage2)).IsFail())
		{
			ErrorPrint(res, L"Failed to Loads image.\n");
			break;
		}

		// Destination 이미지 설정 // Set destination image 
		if((res = stereoCalibrator3D.SetDestinationImage(&fliDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to Loads image.\n");
			break;
		}

		// Destination 이미지 2 설정 // Set destination image 2
		if((res = stereoCalibrator3D.SetDestinationImage2(&fliDestinationImage2)).IsFail())
		{
			ErrorPrint(res, L"Failed to Loads image.\n");
			break;
		}

		// Interpolation 메소드 설정 // Set interpolation method
		if((res = stereoCalibrator3D.SetInterpolationMethod(ImageProcessing::EInterpolationMethod_Bilinear)).IsFail())
		{
			ErrorPrint(res, L"Failed to set interpolation method.\n");
			break;
		}

		// Undistortion 실행 // Execute undistortion
		if((res = stereoCalibrator3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Undistortion failed.\n");
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
	CFLImage fliLearnImage2, fliSourceImage2, fliDestinationImage2;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageLearn, viewImageDestination;
	CGUIViewImageWrap viewImageLearn2, viewImageDestination2;

	// 알고리즘 동작 결과 // Algorithm execution result
	CResult res = EResult_UnknownError;

	// Message Reciever 객체 생성 // Create Message Reciever object
	CMessageReceiver msgReceiver(&viewImageLearn);
	CMessageReceiver msgReceiver2(&viewImageLearn2);

	// Stereo Calibrator 객체 생성 // Create Camera Calibrator object
	CStereoCalibrator3D stereoCalibrator3D;

	do
	{
		// Learn 이미지 로드  // Load learn image
		if((res = fliLearnImage.Load(L"../../ExampleImages/StereoCalibrator3D/Left.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Learn 이미지 2 로드 // Load learn image 2
		if((res = fliLearnImage2.Load(L"../../ExampleImages/StereoCalibrator3D/Right.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		printf("Processing.....\n");

		// Page 0 선택 // Select page 0
		fliLearnImage.SelectPage(0);
		fliLearnImage2.SelectPage(0);

		// Stereo calibration 수행 // Execute stereo calibration
		if(!Calibration(stereoCalibrator3D, fliLearnImage, fliLearnImage2))
			break;

		// Source 이미지에 Learn 이미지를 복사 (얕은 복사) // Copy the learn image to source image (Shallow Copy)
		fliSourceImage.Assign(fliLearnImage, false);

		// Source 이미지 2에 Learn 이미지 2를 복사 (얕은 복사) // Copy the learn image 2 to source image 2 (Shallow Copy)
		fliSourceImage2.Assign(fliLearnImage2, false);

		// Undistortion 수행 // Execute undistortion
		if(!Undistortion(stereoCalibrator3D, fliSourceImage, fliSourceImage2, fliDestinationImage, fliDestinationImage2))
			break;

		// 화면에 격자 탐지 결과 출력 // Display the res of grid detection
		SGridDisplay sArrGridDisplay[5];

		for(int64_t i64ImgIdx = 0; i64ImgIdx < (int64_t)fliLearnImage.GetPageCount(); ++i64ImgIdx)
		{
			SGridDisplay& sGridDisplay = sArrGridDisplay[i64ImgIdx];
			stereoCalibrator3D.GetResultGridPoints(&sGridDisplay.sGridData, i64ImgIdx);
			sGridDisplay.i64ImageIdx = i64ImgIdx;
		}

		SGridDisplay sArrGridDisplay2[5];

		for(int64_t i64ImgIdx = 0; i64ImgIdx < (int64_t)fliLearnImage2.GetPageCount(); ++i64ImgIdx)
		{
			SGridDisplay& sGridDisplay = sArrGridDisplay2[i64ImgIdx];
			stereoCalibrator3D.GetResultGridPoints2(&sGridDisplay.sGridData, i64ImgIdx);
			sGridDisplay.i64ImageIdx = i64ImgIdx;
		}

		msgReceiver.m_vctGridDisplay = sArrGridDisplay;
		msgReceiver2.m_vctGridDisplay = sArrGridDisplay2;

		// Learn 이미지 뷰 생성 // Create the learn image view
		if((res = viewImageLearn.Create(300, 0, 300 + 480 * 1, 360)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Learn 이미지 2 뷰 생성 // Create the learn image 2 view
		if((res = viewImageLearn2.Create(300 + 480, 0, 300 + 480 * 2, 360)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Learn 이미지 뷰에 이미지를 디스플레이 // Display the learn image in the image view
		if((res = viewImageLearn.SetImagePtr(&fliLearnImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Learn 이미지 2 뷰에 이미지를 디스플레이 // Display the learn image 2 in the image view
		if((res = viewImageLearn2.SetImagePtr(&fliLearnImage2)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Get the layer 0 number from image view for display
		// 레이어는 따로 해제하지 않아도 View가 해제 될 때 같이 해제된다. // The layer is released together when View is released without releasing it separately.
		CGUIViewImageLayerWrap layerLearn = viewImageLearn.GetLayer(0);
		CGUIViewImageLayerWrap layerLearn2 = viewImageLearn2.GetLayer(0);

		// Chess Board Grid 출력 // Display chess board grid
		DrawGridPoints(sArrGridDisplay[0], layerLearn);
		DrawGridPoints(sArrGridDisplay2[0], layerLearn2);

		// Source 이미지 뷰 생성 // Create Source image view
		if((res = viewImageDestination.Create(300, 360, 780, 720)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create destination image view
		if((res = viewImageDestination2.Create(780, 360, 1260, 720)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the source image in the image view
		if((res = viewImageDestination.SetImagePtr(&fliDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the destination image in the image view
		if((res = viewImageDestination2.SetImagePtr(&fliDestinationImage2)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if((res = viewImageLearn.SynchronizePointOfView(&viewImageLearn2)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다. // Synchronize the viewpoints of the two image views. 
		if((res = viewImageDestination.SynchronizePointOfView(&viewImageDestination2)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageLearn.SynchronizeWindow(&viewImageLearn2)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageLearn.SynchronizeWindow(&viewImageDestination)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageLearn.SynchronizeWindow(&viewImageDestination2)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 페이지 인덱스를 동기화 한다 // Synchronize the page index of the two image view windows
		if((res = viewImageLearn.SynchronizePageIndex(&viewImageLearn2)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view.\n");
			break;
		}

		// Calibration data 출력 // Display the calibration data
		CStereoCalibrator3D::SIntrinsicParameters sIntrinsicParam = stereoCalibrator3D.GetResultIntrinsicParameters();
		CStereoCalibrator3D::SDistortionCoefficients sDistortCoeef = stereoCalibrator3D.GetResultDistortionCoefficients();

		CStereoCalibrator3D::SIntrinsicParameters sIntrinsicParam2 = stereoCalibrator3D.GetResultIntrinsicParameters2();
		CStereoCalibrator3D::SDistortionCoefficients sDistortCoeef2 = stereoCalibrator3D.GetResultDistortionCoefficients2();

		CStereoCalibrator3D::SRotationParameters sRotationParam = stereoCalibrator3D.GetResultRotationParameters();
		CStereoCalibrator3D::SRotationParameters sRotationParam2 = stereoCalibrator3D.GetResultRotationParameters2();

		CStereoCalibrator3D::STranslationParameters sTranslationParam = stereoCalibrator3D.GetResultTranslationParameters();
		CStereoCalibrator3D::STranslationParameters sTranslationParam2 = stereoCalibrator3D.GetResultTranslationParameters2();

		double f64ReprojError = stereoCalibrator3D.GetResultReProjectionError();

		CFLString<wchar_t> strMatrix, strDistVal;
		CFLString<wchar_t> strMatrix2, strDistVal2;
		CFLString<wchar_t> strRotatMatrix, strTranslVal;
		CFLString<wchar_t> strRotatMatrix2, strTranslVal2;

		strMatrix.AppendFormat(L"%.13lf, ", sIntrinsicParam.f64FocalLengthX);
		strMatrix.AppendFormat(L"%.13lf, ", sIntrinsicParam.f64Skew);
		strMatrix.AppendFormat(L"%.13lf, ", sIntrinsicParam.f64PrincipalPointX);
		strMatrix.AppendFormat(L"%.13lf, ", 0);
		strMatrix.AppendFormat(L"%.13lf, ", sIntrinsicParam.f64FocalLengthY);
		strMatrix.AppendFormat(L"%.13lf, ", sIntrinsicParam.f64PrincipalPointY);
		strMatrix.AppendFormat(L"%.13lf, ", 0);
		strMatrix.AppendFormat(L"%.13lf, ", 0);
		strMatrix.AppendFormat(L"%.13lf", 1);

		strMatrix2.AppendFormat(L"%.13lf, ", sIntrinsicParam2.f64FocalLengthX);
		strMatrix2.AppendFormat(L"%.13lf, ", sIntrinsicParam2.f64Skew);
		strMatrix2.AppendFormat(L"%.13lf, ", sIntrinsicParam2.f64PrincipalPointX);
		strMatrix2.AppendFormat(L"%.13lf, ", 0);
		strMatrix2.AppendFormat(L"%.13lf, ", sIntrinsicParam2.f64FocalLengthY);
		strMatrix2.AppendFormat(L"%.13lf, ", sIntrinsicParam2.f64PrincipalPointY);
		strMatrix2.AppendFormat(L"%.13lf, ", 0);
		strMatrix2.AppendFormat(L"%.13lf, ", 0);
		strMatrix2.AppendFormat(L"%.13lf", 1);

		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64K1);
		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64K2);
		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64P1);
		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64P2);
		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64K3);
		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64K4);
		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64K5);
		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64K6);
		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64S1);
		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64S2);
		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64S3);
		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64S4);
		strDistVal.AppendFormat(L"%.13lf, ", sDistortCoeef.f64Gx);
		strDistVal.AppendFormat(L"%.13lf", sDistortCoeef.f64Gy);

		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64K1);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64K2);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64P1);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64P2);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64K3);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64K4);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64K5);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64K6);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64S1);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64S2);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64S3);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64S4);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64Gx);
		strDistVal2.AppendFormat(L"%.13lf", sDistortCoeef2.f64Gy);

		strRotatMatrix.AppendFormat(L"%.13lf, ", sRotationParam.f64R0);
		strRotatMatrix.AppendFormat(L"%.13lf, ", sRotationParam.f64R1);
		strRotatMatrix.AppendFormat(L"%.13lf, ", sRotationParam.f64R2);
		strRotatMatrix.AppendFormat(L"%.13lf, ", sRotationParam.f64R3);
		strRotatMatrix.AppendFormat(L"%.13lf, ", sRotationParam.f64R4);
		strRotatMatrix.AppendFormat(L"%.13lf, ", sRotationParam.f64R5);
		strRotatMatrix.AppendFormat(L"%.13lf, ", sRotationParam.f64R6);
		strRotatMatrix.AppendFormat(L"%.13lf, ", sRotationParam.f64R7);
		strRotatMatrix.AppendFormat(L"%.13lf", sRotationParam.f64R8);

		strRotatMatrix2.AppendFormat(L"%.13lf, ", sRotationParam2.f64R0);
		strRotatMatrix2.AppendFormat(L"%.13lf, ", sRotationParam2.f64R1);
		strRotatMatrix2.AppendFormat(L"%.13lf, ", sRotationParam2.f64R2);
		strRotatMatrix2.AppendFormat(L"%.13lf, ", sRotationParam2.f64R3);
		strRotatMatrix2.AppendFormat(L"%.13lf, ", sRotationParam2.f64R4);
		strRotatMatrix2.AppendFormat(L"%.13lf, ", sRotationParam2.f64R5);
		strRotatMatrix2.AppendFormat(L"%.13lf, ", sRotationParam2.f64R6);
		strRotatMatrix2.AppendFormat(L"%.13lf, ", sRotationParam2.f64R7);
		strRotatMatrix2.AppendFormat(L"%.13lf", sRotationParam2.f64R8);

		strTranslVal.AppendFormat(L"%.8lf, ", sTranslationParam.f64T0);
		strTranslVal.AppendFormat(L"%.8lf, ", sTranslationParam.f64T1);
		strTranslVal.AppendFormat(L"%.8lf, ", sTranslationParam.f64T2);
		strTranslVal.AppendFormat(L"%.8lf, ", sTranslationParam.f64T3);
		strTranslVal.AppendFormat(L"%.8lf, ", sTranslationParam.f64T4);
		strTranslVal.AppendFormat(L"%.8lf, ", sTranslationParam.f64T5);
		strTranslVal.AppendFormat(L"%.8lf, ", sTranslationParam.f64T6);
		strTranslVal.AppendFormat(L"%.8lf, ", sTranslationParam.f64T7);
		strTranslVal.AppendFormat(L"%.8lf, ", sTranslationParam.f64T8);
		strTranslVal.AppendFormat(L"%.8lf, ", sTranslationParam.f64T9);
		strTranslVal.AppendFormat(L"%.8lf, ", sTranslationParam.f64T10);
		strTranslVal.AppendFormat(L"%.8lf", sTranslationParam.f64T11);

		strTranslVal2.AppendFormat(L"%.8lf, ", sTranslationParam2.f64T0);
		strTranslVal2.AppendFormat(L"%.8lf, ", sTranslationParam2.f64T1);
		strTranslVal2.AppendFormat(L"%.8lf, ", sTranslationParam2.f64T2);
		strTranslVal2.AppendFormat(L"%.8lf, ", sTranslationParam2.f64T3);
		strTranslVal2.AppendFormat(L"%.8lf, ", sTranslationParam2.f64T4);
		strTranslVal2.AppendFormat(L"%.8lf, ", sTranslationParam2.f64T5);
		strTranslVal2.AppendFormat(L"%.8lf, ", sTranslationParam2.f64T6);
		strTranslVal2.AppendFormat(L"%.8lf, ", sTranslationParam2.f64T7);
		strTranslVal2.AppendFormat(L"%.8lf, ", sTranslationParam2.f64T8);
		strTranslVal2.AppendFormat(L"%.8lf, ", sTranslationParam2.f64T9);
		strTranslVal2.AppendFormat(L"%.8lf, ", sTranslationParam2.f64T10);
		strTranslVal2.AppendFormat(L"%.8lf", sTranslationParam2.f64T11);

		wprintf(L"Intrinsic parameters : %s\n", strMatrix.GetString());
		wprintf(L"Distortion Coefficients : %s\n", strDistVal.GetString());
		wprintf(L"Rotation parameters : %s\n", strRotatMatrix.GetString());
		wprintf(L"Translation parameters : %s\n\n", strTranslVal.GetString());
		wprintf(L"Intrinsic parameters 2 : %s\n", strMatrix2.GetString());
		wprintf(L"Distortion Coefficients 2 : %s\n", strDistVal2.GetString());
		wprintf(L"Rotation parameters 2 : %s\n", strRotatMatrix2.GetString());
		wprintf(L"Translation parameters 2 : %s\n\n", strTranslVal2.GetString());
		wprintf(L"Re-Projection Error : %.8lf", f64ReprojError);

		// Destination 이미지 뷰에 수평선을 그립니다. // Draw horizontal line at the image view
		int64_t i64Height = fliDestinationImage.GetHeight();
		int64_t i64Width = fliDestinationImage.GetWidth();

		for(int32_t i32Iter = 0; i32Iter < 2; ++i32Iter)
		{
			for(int32_t i32Index = 0; i32Index < 20; ++i32Index)
			{
				CFLLine<double> fllHorizonLine(CFLPoint<double>(0, i64Height / 20 * i32Index), CFLPoint<double>(i64Width, i64Height / 20 * i32Index));

				CGUIViewImageLayerWrap layerDst = i32Iter == 0 ? viewImageDestination.GetLayer(0) : viewImageDestination2.GetLayer(0);
				layerDst.DrawFigureImage(fllHorizonLine, LIME, 1);
			}
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageLearn.Invalidate(true);
		viewImageLearn2.Invalidate(true);
		viewImageDestination.Invalidate(true);
		viewImageDestination2.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageLearn.IsAvailable() && viewImageLearn2.IsAvailable() && viewImageDestination.IsAvailable() && viewImageDestination2.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}

