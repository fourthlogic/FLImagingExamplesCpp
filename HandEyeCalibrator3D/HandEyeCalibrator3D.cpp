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
	CGUIView3DWrap view3D;
	CGUIViewImageWrap viewImage;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult eResult = EResult_UnknownError;

		// 이미지 로드 // Load the image
		if((eResult = fliSource.Load(L"../../ExampleImages/HandEyeCalibrator3D/ChessBoard.flif")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the image.\n");
			break;
		}

		// HandEyeCalibrator3D 객체 생성 // Create HandEyeCalibrator3D object
		CHandEyeCalibrator3D handEyeCalibrator3D;

		// 처리할 이미지 설정
		handEyeCalibrator3D.SetSourceImage(fliSource);

		// 엔드 이펙터 포즈 로드 // Load the end effector pose
		if((eResult = handEyeCalibrator3D.LoadEndEffectorPose(L"../../ExampleImages/HandEyeCalibrator3D/EndEffectorPose.csv")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the file.\n");
			break;
		}

		// Camera Matrix 설정 // Set the camera matrix
		CFLPoint<double> flpFocalLength(428.668823242188, 428.268188476563);
		CFLPoint<double> flpPrincipalPoint(422.934997558594, 240.188659667969);

		handEyeCalibrator3D.SetCalibrationCameraMatrix(flpFocalLength, flpPrincipalPoint);

		// 셀 간격 설정 // Set the board cell pitch
		handEyeCalibrator3D.SetCalibrationBoardCellPitch(15, 15);

		// 캘리브레이션 객체 타입 설정 // Set the calibration object type
		handEyeCalibrator3D.SetCalibrationObjectType(ECalibrationObjectType_ChessBoard);

		// 최적화 방법 설정 // Set the optimization method
		handEyeCalibrator3D.SetOptimizationMethod(EOptimizationMethod_Nonlinear);

		// 회전 타입 설정 // Set the rotation type
		handEyeCalibrator3D.SetRotationType(ERotationType_RotationVector);

		// 엔드 이펙터 각 단위 설정 // Set the end effector angle unit
		handEyeCalibrator3D.SetEndEffectorAngleUnit(EAngleUnit_Radian);

		// 오일러 각 순서 설정 // Set the euler sequence
		handEyeCalibrator3D.SetEulerSequence(EEulerSequence_Extrinsic_XYZ);

		//왜곡 계수 설정 // Set the distortion coefficient
		CFLArray<double> flaDistortionCoefficient;

		flaDistortionCoefficient.PushBack(-0.0538526475429535);
		flaDistortionCoefficient.PushBack(0.0590364411473274);
		flaDistortionCoefficient.PushBack(0.000375126546714455);
		flaDistortionCoefficient.PushBack(0.000785713375080377);
		flaDistortionCoefficient.PushBack(-0.0189481563866138);

		handEyeCalibrator3D.SetCalibrationDistortionCoefficient(flaDistortionCoefficient);

		const int32_t i32PageCount = fliSource.GetPageCount();

		// 이미지 뷰 생성 // Create image view
		if(IsFail(eResult = viewImage.Create(100, 0, 612, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Source image view.\n");
			break;
		}

		// 결과 3D 뷰 생성 // Create result 3D view
		if(IsFail(eResult = view3D.Create(612, 0, 1124, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Result 3D view.\n");
			break;
		}

		// 이미지 포인터 설정 // Set image pointer
		viewImage.SetImagePtr(&fliSource);

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately		
		CGUIViewImageLayerWrap layerViewSource = viewImage.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerViewSource.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if((eResult = layerViewSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((eResult = handEyeCalibrator3D.Calibrate()).IsFail())
		{
			ErrorPrint(eResult, L"Failed to execute Hand Eye Calibrator 3D.");
			break;
		}

		if(view3D.IsAvailable())
		{
			CGUIView3DLayerWrap view3DLayer = view3D.GetLayer(0);

			CMatrix<double> matResultRotationVector;
			Base::TPoint3<double> tp3ResultTranslationVector;
			CFLArray<double> flaResultEulerAngle;
			CFLString<wchar_t> strDisplay;
			double f64RotationError, f64TranslationError;

			// 캘리브레이션 결과 얻어오기 // Get the calibration result
			handEyeCalibrator3D.GetResultHandToEyeRotationVector(matResultRotationVector);
			handEyeCalibrator3D.GetResultHandToEyeTranslationVector(tp3ResultTranslationVector);
			handEyeCalibrator3D.GetResultHandToEyeEulerAngle(flaResultEulerAngle);
			handEyeCalibrator3D.GetResultRotationError(f64RotationError);
			handEyeCalibrator3D.GetResultTranslationError(f64TranslationError);
			
			// 3D View의 canvas rect 영역 얻어오기 // Get the canvas rect region
			CFLRectL flrCanvasRegion = view3D.GetClientRectCanvasRegion();

			CFLPoint<double> flpImageSize(flrCanvasRegion.GetWidth(), flrCanvasRegion.GetHeight());

			strDisplay.Format(
				L"Rotation Vector(End effector to camera)\n"
				L"[%11.6lf]\n"
				L"[%11.6lf]\n"
				L"[%11.6lf]"
				, matResultRotationVector[0][0], matResultRotationVector[1][0], matResultRotationVector[2][0]);

			view3DLayer.DrawTextCanvas(CFLPoint<double>(0, 0), strDisplay, YELLOW, BLACK, 12, false, 0, EGUIViewImageTextAlignment_LEFT_TOP);

			strDisplay.Format(
				L"Translation Vector\n"
				L"     [%11.6lf]\n"
				L"     [%11.6lf]\n"
				L"     [%11.6lf]"
				, tp3ResultTranslationVector.x, tp3ResultTranslationVector.y, tp3ResultTranslationVector.z);

			view3DLayer.DrawTextCanvas(CFLPoint<double>(flpImageSize.x, flpImageSize.y), strDisplay, YELLOW, BLACK, 12, false, 0, EGUIViewImageTextAlignment_RIGHT_BOTTOM);

			strDisplay.Format(
				L"Rotation Error\n"
				L"     [%11.6lf]\n"
				L"Translation Error\n"
				L"     [%11.6lf]"
				, f64RotationError, f64TranslationError);

			view3DLayer.DrawTextCanvas(CFLPoint<double>(flpImageSize.x, 0), strDisplay, YELLOW, BLACK, 12, false, 0, EGUIViewImageTextAlignment_RIGHT_TOP);

			strDisplay.Format(
				L"Euler Angle (%s)\n"
				L"[%11.6lf]\n"
				L"[%11.6lf]\n"
				L"[%11.6lf]"
				, mapEulerString[handEyeCalibrator3D.GetEulerSequence()].c_str()
				, flaResultEulerAngle[0], flaResultEulerAngle[1], flaResultEulerAngle[2]);

			view3DLayer.DrawTextCanvas(CFLPoint<double>(0, flpImageSize.y), strDisplay, YELLOW, BLACK, 12, false, 0, EGUIViewImageTextAlignment_LEFT_BOTTOM);

			CFL3DObject fl3DOCalibrationBoard;
			Base::TPoint3<double> tp3BoardCenter;

			handEyeCalibrator3D.GetResultCalibration3DObject(fl3DOCalibrationBoard, tp3BoardCenter);
			CFLString<wchar_t> strIdx;

			strIdx.Format(L"Calibration Board");
			view3DLayer.DrawText3D(tp3BoardCenter, strIdx, RED, 0, 9);
			view3D.PushObject(fl3DOCalibrationBoard);

			for(int32_t i = 0; i < i32PageCount; i++)
			{
				Base::TPoint3<double> tp3RobotCenter, tp3CamCenter;
				CFL3DObject fl3DORobot, fl3DCam;
				CFLString<wchar_t> strIdx;

				Base::TPoint3<float> tp3Cam, tp3Board;

				// 결과 3D 객체 얻어오기 // Get the result 3D object
				
				if(handEyeCalibrator3D.GetResultCamera3DObject(i, &fl3DCam, &tp3CamCenter).IsOK())
				{
					// 카메라 포즈 추정에 실패할 경우 NOK 출력 // NOK output if camera pose estimation fails
					if((handEyeCalibrator3D.GetResultReprojectionPoint(i, tp3Cam, tp3Board)).IsFail())
					{
						strIdx.Format(L"Cam %d (NOK)", i);
						view3DLayer.DrawText3D(tp3CamCenter, strIdx, CYAN, 0, 9);
					}
					else
					{
						view3D.PushObject(CGUIView3DObjectLine(tp3Cam, tp3Board, CYAN));
						strIdx.Format(L"Cam %d", i);
						view3DLayer.DrawText3D(tp3CamCenter, strIdx, YELLOW, 0, 9);
					}

					view3D.PushObject(fl3DCam);
				}

				if(handEyeCalibrator3D.GetEndEffector3DObject(i, &fl3DORobot, &tp3RobotCenter).IsOK())
				{
					strIdx.Format(L"End Effector %d", i);
					view3DLayer.DrawText3D(tp3RobotCenter, strIdx, BLUE, 0, 9);
					view3D.PushObject(fl3DORobot);
				}
			}

			view3D.Invalidate();
			view3D.ZoomFit();
		}

		// 이미지 뷰가 종료될 때 까지 기다림
		while(viewImage.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}