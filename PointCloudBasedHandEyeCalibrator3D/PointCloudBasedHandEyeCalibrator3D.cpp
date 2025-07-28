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

	CFL3DObject floLearn, floSource;
	CGUIView3DWrap view3D;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult eResult = EResult_UnknownError;

		// SurfaceMatch3D 객체 생성 // Create SurfaceMatch3D object
		CSurfaceMatch3D match;

		// Learn Object 로드 // Load learn object
		if((eResult = floLearn.Load("../../ExampleImages/PointCloudBasedHandEyeCalibrator3D/Learn.ply")).IsFail())
		{
			ErrorPrint(eResult, "Failed to load the object.\n");
			break;
		}

		// Learn object 설정 // Set the learn object.
		match.SetLearnObject(floLearn);

		// 피벗 설정 // Set the pivot point.
		CFLPoint3<double> flpPivot(-7.880958, -43.990047, 546.119202);
		match.SetLearnPivot(flpPivot);

		if((eResult = match.Learn()).IsFail())
		{
			ErrorPrint(eResult, "Failed to learn.\n");
			break;
		}

		// 오일러 각 순서 설정 // Set the euler sequence
		match.SetEulerSequence(EEulerSequence_Extrinsic_XYZ);

		// PointCloudBasedHandEyeCalibrator3D 객체 생성 // Create PointCloudBasedHandEyeCalibrator3D object
		CPointCloudBasedHandEyeCalibrator3D pointCloudBasedHandEyeCalibrator3D;

		// 3D 매치 객체 설정 // Set the 3d match object
		pointCloudBasedHandEyeCalibrator3D.Set3DMatchModel(match);

		// Source object 로드 // load the source object			
		std::vector<CFL3DObject> vctSource;
		int32_t i32SourceCount = 9;

		for(int32_t i = 0; i < i32SourceCount; ++i)
		{
			CFLString<wchar_t> flsFileName;

			flsFileName.Format(L"s%d.ply", i + 1);

			if((eResult = floSource.Load(L"../../ExampleImages/PointCloudBasedHandEyeCalibrator3D/" + flsFileName)).IsFail())
			{
				ErrorPrint(eResult, L"Failed to load the object.\n");
				break;
			}

			vctSource.push_back(floSource);
		}

		for(int32_t i = 0; i < (int32_t)vctSource.size(); ++i)
			pointCloudBasedHandEyeCalibrator3D.AddSourceObject(vctSource[i]);

		// 엔드 이펙터 포즈 로드 // Load the end effector pose
		if((eResult = pointCloudBasedHandEyeCalibrator3D.LoadEndEffectorPose(L"../../ExampleImages/PointCloudBasedHandEyeCalibrator3D/EndEffectorPose.csv")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the file.\n");
			break;
		}

		// 캘리브레이션 모드 설정 // Set the calibration mode
		pointCloudBasedHandEyeCalibrator3D.SetCalibrationMode(CHandEyeCalibrator3D::ECalibrationMode_EyeInHand);

		// 최적화 방법 설정 // Set the optimization method
		pointCloudBasedHandEyeCalibrator3D.SetOptimizationMethod(EOptimizationMethod_Nonlinear);

		// 회전 타입 설정 // Set the rotation type
		pointCloudBasedHandEyeCalibrator3D.SetRotationType(ERotationType_EulerAngle);

		// 엔드 이펙터 각 단위 설정 // Set the end effector angle unit
		pointCloudBasedHandEyeCalibrator3D.SetEndEffectorAngleUnit(EAngleUnit_Degree);

		// 오일러 각 순서 설정 // Set the euler sequence
		pointCloudBasedHandEyeCalibrator3D.SetEulerSequence(EEulerSequence_Extrinsic_XYZ);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((eResult = pointCloudBasedHandEyeCalibrator3D.Calibrate()).IsFail())
		{
			ErrorPrint(eResult, L"Failed to execute Hand Eye Calibrator 3D.");
			break;
		}

		// 결과 3D 뷰 생성 // Create result 3D view
		if(IsFail(eResult = view3D.Create(612, 0, 1124, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Result 3D view.\n");
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
			pointCloudBasedHandEyeCalibrator3D.GetResultHandToEyeRotationVector(matResultRotationVector);
			pointCloudBasedHandEyeCalibrator3D.GetResultHandToEyeTranslationVector(tp3ResultTranslationVector);
			pointCloudBasedHandEyeCalibrator3D.GetResultHandToEyeEulerAngle(flaResultEulerAngle);
			pointCloudBasedHandEyeCalibrator3D.GetResultRotationError(f64RotationError);
			pointCloudBasedHandEyeCalibrator3D.GetResultTranslationError(f64TranslationError);

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
				, mapEulerString[pointCloudBasedHandEyeCalibrator3D.GetEulerSequence()].c_str()
				, flaResultEulerAngle[0], flaResultEulerAngle[1], flaResultEulerAngle[2]);

			view3DLayer.DrawTextCanvas(CFLPoint<double>(0, flpImageSize.y), strDisplay, YELLOW, BLACK, 12, false, 0, EGUIViewImageTextAlignment_LEFT_BOTTOM);

			CFL3DObject fl3DOCalibrationBoard;
			Base::TPoint3<double> tp3BoardCenter;

			pointCloudBasedHandEyeCalibrator3D.GetResultCalibration3DObject(fl3DOCalibrationBoard, tp3BoardCenter);
			CFLString<wchar_t> strIdx;

			strIdx.Format(L"Calibration Board");
			view3DLayer.DrawText3D(tp3BoardCenter, strIdx, RED, 0, 9);
			view3D.PushObject(fl3DOCalibrationBoard);

			for(int32_t i = 0; i < (int32_t)vctSource.size(); i++)
			{
				Base::TPoint3<double> tp3RobotCenter, tp3CamCenter;
				CFL3DObject fl3DORobot, fl3DCam;
				CFLString<wchar_t> strIdx;

				Base::TPoint3<float> tp3Cam, tp3Board;

				// 결과 3D 객체 얻어오기 // Get the result 3D object

				if(pointCloudBasedHandEyeCalibrator3D.GetResultCamera3DObject(i, &fl3DCam, &tp3CamCenter).IsOK())
				{
					// 카메라 포즈 추정에 실패할 경우 NOK 출력 // NOK output if camera pose estimation fails
					if((pointCloudBasedHandEyeCalibrator3D.GetResultReprojectionPoint(i, tp3Cam, tp3Board)).IsFail())
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

				if(pointCloudBasedHandEyeCalibrator3D.GetEndEffector3DObject(i, &fl3DORobot, &tp3RobotCenter).IsOK())
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
		while(view3D.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}