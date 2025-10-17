#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

struct SGridDisplay
{
	int64_t i64ImageIdx;
	CStereoCalibrator3D::SGridResult sGridResult;
};

CResult DrawGridPoints(SGridDisplay& sGridDisplay, CGUIViewImageLayerWrap pLayer)
{
	CResult res = EResult_UnknownError;

	do
	{
		if(!sGridDisplay.sGridResult.arrGridData.GetCount())
		{
			res = EResult_NoData;
			break;
		}

		// 그리기 색상 설정 // Set drawing color
		uint32_t u32ArrColor[3] = { RED, LIME, CYAN };

		int64_t i64GridRow = sGridDisplay.sGridResult.i64Rows;
		int64_t i64GridCol = sGridDisplay.sGridResult.i64Columns;
		double f64AvgDistance = sGridDisplay.sGridResult.f64AvgDistance;
		CFLQuad<double> flqBoardRegion = sGridDisplay.sGridResult.flqBoardRegion;
		double f64Angle = flqBoardRegion.flpPoints[0].GetAngle(flqBoardRegion.flpPoints[1]);
		double f64Width = flqBoardRegion.flpPoints[0].GetDistance(flqBoardRegion.flpPoints[1]);

		// Grid 그리기 // Draw grid
		for(int64_t i64Row = 0; i64Row < i64GridRow; ++i64Row)
		{
			for(int64_t i64Col = 0; i64Col < i64GridCol - 1; ++i64Col)
			{
				int64_t i64GridIdx = i64Row * i64GridCol + i64Col;
				CFLPoint<double> flpGridPoint1 = (sGridDisplay.sGridResult.arrGridData[i64Row][i64Col]);
				CFLPoint<double> flpGridPoint2 = (sGridDisplay.sGridResult.arrGridData[i64Row][i64Col + 1]);
				CFLLine<double> fllDrawLine(flpGridPoint1, flpGridPoint2);
				pLayer.DrawFigureImage(fllDrawLine, BLACK, 5);
				pLayer.DrawFigureImage(fllDrawLine, u32ArrColor[i64GridIdx % 3], 3);
			}

			if(i64Row < i64GridRow - 1)
			{
				CFLPoint<double> flpGridPoint1 = (sGridDisplay.sGridResult.arrGridData[i64Row][i64GridCol - 1]);
				CFLPoint<double> flpGridPoint2 = (sGridDisplay.sGridResult.arrGridData[i64Row + 1][0]);
				CFLLine<double> fllDrawLine(flpGridPoint1, flpGridPoint2);
				pLayer.DrawFigureImage(fllDrawLine, BLACK, 5);
				pLayer.DrawFigureImage(fllDrawLine, YELLOW, 3);
			}
		}

		uint32_t u32ColorText = YELLOW;
		double f64PointDist = 0.;
		double f64Dx = 0.;
		double f64Dy = 0.;

		// Grid Point 인덱싱 // Index Grid Point
		for(int64_t i64Row = 0; i64Row < i64GridRow; ++i64Row)
		{
			CFLPoint<double> flpGridPoint1 = (sGridDisplay.sGridResult.arrGridData[i64Row][0]);
			CFLPoint<double> flpGridPoint2 = (sGridDisplay.sGridResult.arrGridData[i64Row][1]);
			double f64TempAngle = flpGridPoint1.GetAngle(flpGridPoint2);

			for(int64_t i64Col = 0; i64Col < i64GridCol; ++i64Col)
			{
				int64_t i64GridIdx = i64Row * i64GridCol + i64Col;

				if(i64Col < i64GridCol - 1)
				{
					flpGridPoint1 = (sGridDisplay.sGridResult.arrGridData[i64Row][i64Col]);
					flpGridPoint2 = (sGridDisplay.sGridResult.arrGridData[i64Row][i64Col + 1]);

					f64Dx = flpGridPoint2.x - flpGridPoint1.x;
					f64Dy = flpGridPoint2.y - flpGridPoint1.y;
					f64PointDist = sqrt(f64Dx * f64Dx + f64Dy * f64Dy);
				}

				if(i64Row)
				{
					flpGridPoint1 = (sGridDisplay.sGridResult.arrGridData[i64Row][i64Col]);
					flpGridPoint2 = (sGridDisplay.sGridResult.arrGridData[i64Row - 1][i64Col]);

					f64Dx = flpGridPoint2.x - flpGridPoint1.x;
					f64Dy = flpGridPoint2.y - flpGridPoint1.y;
					f64PointDist = std::min<double>(f64PointDist, sqrt(f64Dx * f64Dx + f64Dy * f64Dy));
				}
				else
				{
					flpGridPoint1 = (sGridDisplay.sGridResult.arrGridData[0][i64Col]);
					flpGridPoint2 = (sGridDisplay.sGridResult.arrGridData[1][i64Col]);

					f64Dx = flpGridPoint2.x - flpGridPoint1.x;
					f64Dy = flpGridPoint2.y - flpGridPoint1.y;
					f64PointDist = std::min<double>(f64PointDist, sqrt(f64Dx * f64Dx + f64Dy * f64Dy));
				}

				CFLString<wchar_t> wstrGridIdx;
				wstrGridIdx.Format(L"%d", i64GridIdx);
				u32ColorText = u32ArrColor[i64GridIdx % 3];

				if(i64Col == i64GridCol - 1)
					u32ColorText = YELLOW;

				pLayer.DrawTextImage(flpGridPoint1, wstrGridIdx, u32ColorText, BLACK, (float)(int32_t)(f64PointDist / 2), true, f64TempAngle);
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

	// CMessageReceiver 생성자 // CMessageReceiver constructor
	CMessageReceiver(CGUIViewImageWrap* pViewImage) : m_pViewImage(pViewImage)
	{
		// 메세지를 전달 받기 위해 CBroadcastManager 에 구독 등록 // Subscribe to CBroadcast Manager to receive messages
		CBroadcastManager::Subscribe(this);
	}

	// CMessageReceiver 소멸자 // CMessageReceiver destructor
	virtual ~CMessageReceiver()
	{
		// 객체가 소멸할때 메세지 수신을 중단하기 위해 구독을 해제한다. // Unsubscribe to stop receiving messages when object disappears.
		CBroadcastManager::Unsubscribe(this);
	}

	DeclareGetClassType();

	// 메세지가 들어오면 호출되는 함수 OnReceiveBroadcast 오버라이드 하여 구현 // Override OnReceiveBroadcast that is called when a message in called
	void OnReceiveBroadcast(const CBroadcastMessage* pMessage) override
	{
		do
		{
			// Message 가 nullptr 인지 확인 // Verify message is nullptr
			if(!pMessage)
				break;

			// 설정된 뷰가 nullptr 인지 확인 // Verify view is nullptr
			if(!m_pViewImage)
				break;

			// 메세지의 채널을 확인 // Check message's channel
			switch(pMessage->GetChannel())
			{
			case EGUIBroadcast_ViewImage_PostPageChange:
				{
			        // GetCaller 가 등록한 이미지뷰인지 확인 // Verify that GetCaller is a registered image view
					if(pMessage->GetCaller() != (const CFLBase*)m_pViewImage->GetMessageCallerPtr())
						break;

					CFLImage* pFliLearnImage = (CFLImage*)m_pViewImage->GetImagePtr();

					if(!pFliLearnImage)
						break;

					int64_t i64CurPage = pFliLearnImage->GetSelectedPageIndex();

					// 이미지뷰의 0번 레이어 가져오기 // Get layer 0th of image view
					CGUIViewImageLayerWrap layer = m_pViewImage->GetLayer((int32_t)(i64CurPage % 10));

					for(int64_t i = 0; i < 10; ++i)
						m_pViewImage->GetLayer((int32_t)i).Clear();

					for(int64_t i64Idx = 0; i64Idx < (int64_t)pFliLearnImage->GetPageCount(); ++i64Idx)
					{
						if(m_psGridDisplay[(int32_t)i64Idx].i64ImageIdx == i64CurPage)
							DrawGridPoints(m_psGridDisplay[(int32_t)i64Idx], layer);
					}

					// 이미지 뷰를 갱신 // Update image view
					m_pViewImage->Invalidate(true);
				}
				break;
			}
		}
		while(false);
	}

	SGridDisplay* m_psGridDisplay;

protected:
	CGUIViewImageWrap* m_pViewImage;
};

CResult Calibration(CStereoCalibrator3D& stereoCalibrator3D, CFLImage& fliLearnImage, CFLImage& fliLearn2Image)
{
	// 수행 결과 객체 선언 // Declare execution result object
	CResult res = EResult_UnknownError;

	do
	{
		// Learn 이미지 설정 // Set Learn image
		if((res = stereoCalibrator3D.SetLearnImage(fliLearnImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Learn image.\n");
			break;
		}

		// Learn 2 이미지 설정 // Set Learn 2 image
		if((res = stereoCalibrator3D.SetLearnImage2(fliLearn2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Learn 2 image.\n");
			break;
		}

		// Calibration의 최적해 정확도 값 설정 // Set optimal solution accuracy of calibration
		if((res = stereoCalibrator3D.SetOptimalSolutionAccuracy(1e-5)).IsFail())
		{
			ErrorPrint(res, L"Failed to set calibration optimal solution accuracy.\n");
			break;
		}

		// Calibration에 사용되는 Grid Type 설정 // Set grid type used in calibration
		if((res = stereoCalibrator3D.SetGridType(CStereoCalibrator3D::EGridType_ChessBoard)).IsFail())
		{
			ErrorPrint(res, L"Failed to set calibration grid type.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 Calibration 수행 // Calibration algorithm according to previously set parameters
		if((res = stereoCalibrator3D.Calibrate()).IsFail())
		{
			ErrorPrint(res, L"Failed to calibrate Stereo Calibrator 3D.\n");
			break;
		}
	}
	while(false);

	return res;
}

CResult Undistortion(CStereoCalibrator3D& stereoCalibrator3D, CFLImage& fliSourceImage, CFLImage& fliSource2Image, CFLImage& fliDestinationImage, CFLImage& fliDestination2Image)
{
	// 수행 결과 객체 선언 // Declare execution result object
	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 설정 // Set Source image
		if((res = stereoCalibrator3D.SetSourceImage(fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Source image.\n");
			break;
		}

		// Source 이미지 2 설정 // Set Source 2 image
		if((res = stereoCalibrator3D.SetSourceImage2(fliSource2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Source 2 image.\n");
			break;
		}

		// Destination 이미지 설정 // Set Destination image
		if((res = stereoCalibrator3D.SetDestinationImage(fliDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination image.\n");
			break;
		}

		// Destination 이미지 2 설정 // Set Destination 2 image
		if((res = stereoCalibrator3D.SetDestinationImage2(fliDestination2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination 2 image.\n");
			break;
		}

		// Interpolation 메소드 설정 // Set interpolation method
		if((res = stereoCalibrator3D.SetInterpolationMethod(ImageProcessing::EInterpolationMethod_Bilinear)).IsFail())
		{
			ErrorPrint(res, L"Failed to set interpolation method.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = stereoCalibrator3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Stereo Calibrator 3D.\n");
			break;
		}
	}
	while(false);

	return res;
}

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliLearnImage;
	CFLImage fliSourceImage;
	CFLImage fliDestinationImage;
	CFLImage fliLearn2Image;
	CFLImage fliSource2Image;
	CFLImage fliDestination2Image;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewLearnImage;
	CGUIViewImageWrap viewDestinationImage;
	CGUIViewImageWrap viewLearn2Image;
	CGUIViewImageWrap viewDestination2Image;

	// 수행 결과 객체 선언 // Declare execution result object
	CResult res = EResult_UnknownError;

	do
	{
		// Learn 이미지 로드 // Load Learn image
		if((res = fliLearnImage.Load(L"../../ExampleImages/StereoCalibrator3D/Left.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Learn2 이미지 로드 // Load Learn2 image
		if((res = fliLearn2Image.Load(L"../../ExampleImages/StereoCalibrator3D/Right.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Learn 이미지 뷰 생성 // Create Learn image view
		if((res = viewLearnImage.Create(300, 0, 300 + 480 * 1, 360)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Learn 2 이미지 뷰 생성 // Create Learn 2 image view
		if((res = viewLearn2Image.Create(300 + 480, 0, 300 + 480 * 2, 360)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create Destination image view
		if((res = viewDestinationImage.Create(300, 360, 780, 720)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 2 이미지 뷰 생성 // Create Destination 2 image view
		if((res = viewDestination2Image.Create(780, 360, 1260, 720)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Learn 이미지 뷰에 이미지를 디스플레이 // Display image in Learn image view
		if((res = viewLearnImage.SetImagePtr(&fliLearnImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Learn 2 이미지 뷰에 이미지를 디스플레이 // Display image in Learn 2 image view
		if((res = viewLearn2Image.SetImagePtr(&fliLearn2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display image in Destination image view
		if((res = viewDestinationImage.SetImagePtr(&fliDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 2 이미지 뷰에 이미지를 디스플레이 // Display image in Destination 2 image view
		if((res = viewDestination2Image.SetImagePtr(&fliDestination2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewLearnImage.SynchronizeWindow(&viewLearn2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewLearnImage.SynchronizeWindow(&viewDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewLearnImage.SynchronizeWindow(&viewDestination2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 Page를 동기화 한다 // Synchronize pages of two image views
		if((res = viewLearnImage.SynchronizePageIndex(&viewLearn2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize page index between image views.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 Page를 동기화 한다 // Synchronize pages of two image views
		if((res = viewLearnImage.SynchronizePageIndex(&viewDestinationImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize page index between image views.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 Page를 동기화 한다 // Synchronize pages of two image views
		if((res = viewLearnImage.SynchronizePageIndex(&viewDestination2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize page index between image views.\n");
			break;
		}

		printf("Processing.....\n");

	    // Stereo Calibrator 3D 객체 생성 // Create Stereo Calibrator 3D object
		CStereoCalibrator3D stereoCalibrator3D;

		// Stereo Calibrator 3D Calibration 수행 // Calibrate Stereo Calibrator 3D
		if(Calibration(stereoCalibrator3D, fliLearnImage, fliLearn2Image).IsFail())
			break;

		// Source 이미지를 Learn 이미지와 동일하도록 설정 (얕은 복사) // Assign Learn image to Source image (Shallow Copy)
		if((res = fliSourceImage.Assign(fliLearnImage, false)).IsFail())
		{
			ErrorPrint(res, L"Failed to assign the image.\n");
			break;
		}

		// Source 2 이미지를 Learn 2 이미지와 동일하도록 설정 (얕은 복사) // Assign Learn 2 image to Source 2 image (Shallow Copy)
		if((res = fliSource2Image.Assign(fliLearn2Image, false)).IsFail())
		{
			ErrorPrint(res, L"Failed to assign the image.\n");
			break;
		}

		// Stereo Calibrator 3D Undistortion 수행 // Undistort Stereo Calibrator 3D
		if(Undistortion(stereoCalibrator3D, fliSourceImage, fliSource2Image, fliDestinationImage, fliDestination2Image).IsFail())
			break;

		// 뷰에 격자 탐지 결과 출력 // Display grid detection result in view
		SGridDisplay sArrGridDisplay[5];

		for(int64_t i64ImgIdx = 0; i64ImgIdx < (int64_t)fliLearnImage.GetPageCount(); ++i64ImgIdx)
		{
			SGridDisplay& sGridDisplay = sArrGridDisplay[i64ImgIdx];
			stereoCalibrator3D.GetResultGridPoints(&sGridDisplay.sGridResult, i64ImgIdx);
			sGridDisplay.i64ImageIdx = i64ImgIdx;
		}

		SGridDisplay sArrGridDisplay2[5];

		for(int64_t i64ImgIdx = 0; i64ImgIdx < (int64_t)fliLearn2Image.GetPageCount(); ++i64ImgIdx)
		{
			SGridDisplay& sGridDisplay = sArrGridDisplay2[i64ImgIdx];
			stereoCalibrator3D.GetResultGridPoints2(&sGridDisplay.sGridResult, i64ImgIdx);
			sGridDisplay.i64ImageIdx = i64ImgIdx;
		}

	    // Message Receiver 객체 생성 // Create Message Receiver object
		CMessageReceiver msgReceiver(&viewLearnImage);
		CMessageReceiver msgReceiver2(&viewLearn2Image);

		msgReceiver.m_psGridDisplay = sArrGridDisplay;
		msgReceiver2.m_psGridDisplay = sArrGridDisplay2;

		// 화면에 출력하기 위해 이미지 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released
		CGUIViewImageLayerWrap layerLearn = viewLearnImage.GetLayer(0);
		CGUIViewImageLayerWrap layerLearn2 = viewLearn2Image.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination = viewDestinationImage.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination2 = viewDestination2Image.GetLayer(0);

		// Chess Board Grid 출력 // Display chess board grid
		DrawGridPoints(sArrGridDisplay[0], layerLearn);
		DrawGridPoints(sArrGridDisplay2[0], layerLearn2);

		// Calibration data 출력 // Display calibration data
		CStereoCalibrator3D::SIntrinsicParameters sIntrinsicParam = stereoCalibrator3D.GetResultIntrinsicParameters();
		CStereoCalibrator3D::SDistortionCoefficients sDistortCoeef = stereoCalibrator3D.GetResultDistortionCoefficients();

		CStereoCalibrator3D::SIntrinsicParameters sIntrinsicParam2 = stereoCalibrator3D.GetResultIntrinsicParameters2();
		CStereoCalibrator3D::SDistortionCoefficients sDistortCoeef2 = stereoCalibrator3D.GetResultDistortionCoefficients2();

		CStereoCalibrator3D::SRotationParameters sRotationParam = stereoCalibrator3D.GetResultRotationParameters();
		CStereoCalibrator3D::SRotationParameters sRotationParam2 = stereoCalibrator3D.GetResultRotationParameters2();

		CStereoCalibrator3D::STranslationParameters sTranslationParam = stereoCalibrator3D.GetResultTranslationParameters();
		CStereoCalibrator3D::STranslationParameters sTranslationParam2 = stereoCalibrator3D.GetResultTranslationParameters2();

		double f64ReprojError = stereoCalibrator3D.GetResultReProjectionError();

		CFLString<wchar_t> strMatrix;
		CFLString<wchar_t> strDistVal;
		CFLString<wchar_t> strMatrix2;
		CFLString<wchar_t> strDistVal2;
		CFLString<wchar_t> strRotatMatrix;
		CFLString<wchar_t> strTranslVal;
		CFLString<wchar_t> strRotatMatrix2;
		CFLString<wchar_t> strTranslVal2;

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
		strDistVal.AppendFormat(L"%.13lf", sDistortCoeef.f64K3);

		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64K1);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64K2);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64P1);
		strDistVal2.AppendFormat(L"%.13lf, ", sDistortCoeef2.f64P2);
		strDistVal2.AppendFormat(L"%.13lf", sDistortCoeef2.f64K3);

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

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerLearn.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Learn Image", YELLOW, BLACK, 15)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerLearn2.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Learn 2 Image", YELLOW, BLACK, 15)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerDestination.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 15)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerDestination2.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination 2 Image", YELLOW, BLACK, 15)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}


		// 새로 생성한 이미지를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created image
		if((res = viewDestinationImage.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit image view.\n");
			break;
		}

		// 새로 생성한 이미지를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created image
		if((res = viewDestination2Image.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit image view.\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		viewLearnImage.Invalidate(true);
		viewLearn2Image.Invalidate(true);
		viewDestinationImage.Invalidate(true);
		viewDestination2Image.Invalidate(true);

		// 뷰가 닫히기 전까지 종료하지 않고 대기 // Wait until a view is closed before exiting
		while(viewLearnImage.IsAvailable() && viewLearn2Image.IsAvailable() && viewDestinationImage.IsAvailable() && viewDestination2Image.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}

