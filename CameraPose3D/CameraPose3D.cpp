#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"
#include <map>

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	std::map<Foundation::EEulerSequence, std::wstring> mapEulerString;

	mapEulerString[Foundation::EEulerSequence_Extrinsic_XYZ] = L"Ext_XYZ";
	mapEulerString[Foundation::EEulerSequence_Extrinsic_XZY] = L"Ext_XZY";
	mapEulerString[Foundation::EEulerSequence_Extrinsic_YZX] = L"Ext_YZX";
	mapEulerString[Foundation::EEulerSequence_Extrinsic_YXZ] = L"Ext_YXZ";
	mapEulerString[Foundation::EEulerSequence_Extrinsic_ZXY] = L"Ext_ZXY";
	mapEulerString[Foundation::EEulerSequence_Extrinsic_ZYX] = L"Ext_ZYX";
	mapEulerString[Foundation::EEulerSequence_Extrinsic_XYX] = L"Ext_XYX";
	mapEulerString[Foundation::EEulerSequence_Extrinsic_XZX] = L"Ext_XZX";
	mapEulerString[Foundation::EEulerSequence_Extrinsic_YZY] = L"Ext_YZY";
	mapEulerString[Foundation::EEulerSequence_Extrinsic_YXY] = L"Ext_YXY";
	mapEulerString[Foundation::EEulerSequence_Extrinsic_ZYZ] = L"Ext_ZYZ";
	mapEulerString[Foundation::EEulerSequence_Extrinsic_ZXZ] = L"Ext_ZXZ";

	mapEulerString[Foundation::EEulerSequence_Intrinsic_XYZ] = L"Int_XYZ";
	mapEulerString[Foundation::EEulerSequence_Intrinsic_XZY] = L"Int_XZY";
	mapEulerString[Foundation::EEulerSequence_Intrinsic_YZX] = L"Int_YZX";
	mapEulerString[Foundation::EEulerSequence_Intrinsic_YXZ] = L"Int_YXZ";
	mapEulerString[Foundation::EEulerSequence_Intrinsic_ZXY] = L"Int_ZXY";
	mapEulerString[Foundation::EEulerSequence_Intrinsic_ZYX] = L"Int_ZYX";
	mapEulerString[Foundation::EEulerSequence_Intrinsic_XYX] = L"Int_XYX";
	mapEulerString[Foundation::EEulerSequence_Intrinsic_XZX] = L"Int_XZX";
	mapEulerString[Foundation::EEulerSequence_Intrinsic_YZY] = L"Int_YZY";
	mapEulerString[Foundation::EEulerSequence_Intrinsic_YXY] = L"Int_YXY";
	mapEulerString[Foundation::EEulerSequence_Intrinsic_ZYZ] = L"Int_ZYZ";
	mapEulerString[Foundation::EEulerSequence_Intrinsic_ZXZ] = L"Int_ZXZ";
	
	CFLImage fliSource;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult eResult = EResult_UnknownError;

		// 이미지 로드 // Load the image
		if((eResult = fliSource.Load(L"../../ExampleImages/CameraPose3D/ChessBoard(9).flif")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the image.\n");
			break;
		}
		
		// CameraPose3D 객체 생성 // Create CameraPose3D object
		CCameraPose3D cameraPose3D;

		// Camera Matrix 설정 // Set the camera matrix
		CFLPoint<double> flpFocalLength(617.8218, 618.2815);
		CFLPoint<double> flpPrincipalPoint(319.05237, 243.0472);
		cameraPose3D.SetCameraMatrix(flpFocalLength, flpPrincipalPoint);

		// 셀 간격 설정 // Set the board cell pitch
		cameraPose3D.SetBoardCellPitch(5, 5);

		// 캘리브레이션 객체 타입 설정 // Set the calibration object type
		cameraPose3D.SetCalibrationObjectType(ECalibrationObjectType_ChessBoard);

		// 이미지 전처리 타입 설정 // Set the image preprocessing method
		cameraPose3D.SetPreprocessingMethod(ECalibrationPreprocessingMethod_ShadingCorrection);
		
		const int32_t i32PageCount = fliSource.GetPageCount();

		CGUIViewImageWrap arrViewWrap[9];
		int32_t i32WindowWidth = 300;
		int32_t i32WindowHeight = 300;

		for(int32_t i = 0; i < i32PageCount / 3; ++i)
		{
			int32_t i32Height = i32WindowHeight * i;

			for(int32_t j = 0 ; j < i32PageCount / 3; ++j)
			{
				int32_t i32Width = i32WindowWidth * j;

				arrViewWrap[i * 3 + j].Create(10 + i32Height, i32Width, 10 + i32Height + i32WindowHeight, i32Width + i32WindowWidth);
			}
		}

		for(int32_t i = 1; i < i32PageCount; ++i)
			arrViewWrap[0].SynchronizeWindow(&arrViewWrap[i]);

		CFLImage fliPage[9];

		for(int32_t i = 0; i < i32PageCount; i++)
		{			
			// 페이지 선택
			fliPage[i].Assign(fliSource.GetPage(i));

			// 처리할 이미지 설정
			cameraPose3D.SetSourceImage(fliPage[i]);

			// 이미지 포인터 설정 // Set image pointer
			arrViewWrap[i].SetImagePtr(&fliPage[i]);

			// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
			if((eResult = cameraPose3D.Execute()).IsFail())
			{
				ErrorPrint(eResult, L"Failed to execute Camera Pose 3D.");
				break;
			}

			// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
			// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately		
			CGUIViewImageLayerWrap layerViewSource = arrViewWrap[i].GetLayer(0);

			// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
			layerViewSource.Clear();

			// View 정보를 디스플레이 한다. // Display view information
			// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
			// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
			// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
			//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
			// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
			//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
			if((eResult = layerViewSource.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 15)).IsFail())
			{
				ErrorPrint(eResult, L"Failed to draw text.\n");
				break;
			}

			// 결과 객체 영역 가져오기 // Get the result board region
			CFLQuad<double> flqBoardRegion;
			cameraPose3D.GetResultBoardRegion(flqBoardRegion);

			// 결과 코너점 가져오기 // Get the result corner points
			CFLFigureArray flfaCornerPoints;
			cameraPose3D.GetResultCornerPoints(flfaCornerPoints);

			// 결과 객체 영역 그리기 // Draw the result board region
			layerViewSource.DrawFigureImage(flqBoardRegion, BLUE, 3);

			// 결과 코너점 그리기 // Draw the result corner points
			flfaCornerPoints.Flatten();

			for(int64_t k = 0; k < flfaCornerPoints.GetCount(); ++k)
				layerViewSource.DrawFigureImage(flfaCornerPoints[k]->GetCenter().MakeCrossHair(5, true), ORANGE, 1);

			CFLString<wchar_t> strDisplay;

			// 결과 가져오기 // Get the results
			CFLArray<double> flaResultRotationVector, flaResultTranslationVector, flaResultEulerAngle;
			CMatrix<double> matResultRotationMatrix;

			// 오일러 각 순서 설정 // Set the euler sequence
			EEulerSequence eEulerSequence = EEulerSequence_Extrinsic_XYZ;

			cameraPose3D.GetResultRotationVector(flaResultRotationVector);
			cameraPose3D.GetResultRotationMatrix(matResultRotationMatrix);
			cameraPose3D.GetResultTranslationVector(flaResultTranslationVector);
			cameraPose3D.GetResultEulerAngle(eEulerSequence, flaResultEulerAngle);

			CFLPoint<double> flpImageSize(fliSource);
			flpImageSize *= 2;

			strDisplay.Format(
				L"Rotation Vector\n"
				L"[%11.6lf]\n"
				L"[%11.6lf]\n"
				L"[%11.6lf]"
				, flaResultRotationVector[0], flaResultRotationVector[1], flaResultRotationVector[2]);
			layerViewSource.DrawTextImage(CFLPoint<double>(), strDisplay, YELLOW, BLACK, 11, false, 0, EGUIViewImageTextAlignment_LEFT_TOP, L"Courier New");

			strDisplay.Format(
				L"Translation Vector\n"
				L"     [%11.6lf]\n"
				L"     [%11.6lf]\n"
				L"     [%11.6lf]"
				, flaResultTranslationVector[0], flaResultTranslationVector[1], flaResultTranslationVector[2]);

			layerViewSource.DrawTextImage(CFLPoint<double>(flpImageSize.x, 0), strDisplay, YELLOW, BLACK, 11, false, 0, EGUIViewImageTextAlignment_RIGHT_TOP, L"Courier New");

			strDisplay.Format(
				L"                  Rotation Matrix\n"
				L"[%9.6lf, %9.6lf, %9.6lf]\n"
				L"[%9.6lf, %9.6lf, %9.6lf]\n"
				L"[%9.6lf, %9.6lf, %9.6lf]"
				, matResultRotationMatrix[0][0], matResultRotationMatrix[0][1], matResultRotationMatrix[0][2]
				, matResultRotationMatrix[1][0], matResultRotationMatrix[1][1], matResultRotationMatrix[1][2]
				, matResultRotationMatrix[2][0], matResultRotationMatrix[2][1], matResultRotationMatrix[2][2]);

			layerViewSource.DrawTextImage(CFLPoint<double>(flpImageSize.x, flpImageSize.y), strDisplay, YELLOW, BLACK, 11, false, 0, EGUIViewImageTextAlignment_RIGHT_BOTTOM, L"Courier New");

			strDisplay.Format(
				L"Euler Angle (%s)\n"
				L"[%11.6lf]\n"
				L"[%11.6lf]\n"
				L"[%11.6lf]"
				, mapEulerString[eEulerSequence].c_str()
				, flaResultEulerAngle[0], flaResultEulerAngle[1], flaResultEulerAngle[2]);

			layerViewSource.DrawTextImage(CFLPoint<double>(0, flpImageSize.y), strDisplay, YELLOW, BLACK, 11, false, 0, EGUIViewImageTextAlignment_LEFT_BOTTOM, L"Courier New");
		}

		if(arrViewWrap[0].IsAvailable())
			arrViewWrap[0].Invalidate();

		// 이미지 뷰가 종료될 때 까지 기다림
		while(arrViewWrap[0].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}