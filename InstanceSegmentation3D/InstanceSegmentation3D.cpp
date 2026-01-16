#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"
#include <cfloat>
#include <cmath>
#include <iostream>
#include <limits>
#include <process.h>

bool g_bTerminated = false;

unsigned int __stdcall LearnThread(void* pParam)
{
	CInstanceSegmentation3DDL* pInstanceSegmentation = (CInstanceSegmentation3DDL*)pParam;

	CResult er = pInstanceSegmentation->Learn();
	g_bTerminated = true;

	return unsigned int(er);
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliLearnImage;
	CFLImage fliDepthMapImage;
	CFLImage fliResultBoxContourImage;
	CFL3DObjectGroup flogResult;
	
	/// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageLearn;
	CGUIViewImageWrap viewImageDepthMap;
	CGUIViewImageWrap viewImagesBoxContour;
	CGUIView3DWrap view3DResult;

	// 그래프 뷰 선언 // Declare the graph view
	CGUIViewGraphWrap viewGraph;

	CResult res = EResult_UnknownError;

	do
	{
		// 라이브러리가 완전히 로드 될 때까지 기다림 // Wait for the library to fully load
		CThreadUtilities::Sleep(1000);

		// 이미지 로드 // Loads image
		if(IsFail(res = fliLearnImage.Load(L"../../ExampleImages/InstanceSegmentation3D/Texture(Labeled).flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliDepthMapImage.Load(L"../../ExampleImages/InstanceSegmentation3D/Depth.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Creates image view
		if(IsFail(res = viewImageLearn.Create(100, 0, 600, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageDepthMap.Create(600, 0, 1100, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImagesBoxContour.Create(100, 500, 600, 1000)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = view3DResult.Create(600, 500, 1100, 1000)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Graph 뷰 생성 // Create graph view
		if(IsFail(res = viewGraph.Create(1100, 0, 1600, 500)))
		{
			ErrorPrint(res, " Failed to create the graph view. \n");
			break;
		}

		viewGraph.SetDarkMode();

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImageLearn.SynchronizePointOfView(&viewImageDepthMap)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImageLearn.SynchronizePointOfView(&viewImagesBoxContour)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 다섯 개의 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the four image view windows
		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewImageDepthMap)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewImagesBoxContour)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageLearn.SynchronizeWindow(&view3DResult)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewGraph)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImageLearn.SetImagePtr(&fliLearnImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageDepthMap.SetImagePtr(&fliDepthMapImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImagesBoxContour.SetImagePtr(&fliResultBoxContourImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerLearn = viewImageLearn.GetLayer(0);
		CGUIViewImageLayerWrap layerDepthMap = viewImageDepthMap.GetLayer(0);
		CGUIViewImageLayerWrap layerResultLabel = viewImagesBoxContour.GetLayer(0);
		CGUIView3DLayerWrap layer3DResult = view3DResult.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerLearn.Clear();
		layerDepthMap.Clear();
		layerResultLabel.Clear();
		layer3DResult.Clear();
		
		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerLearn.DrawTextCanvas(CFLPoint<double>(0, 0), L"LEARN & VALIDATION & INFERENCE", YELLOW, BLACK, 25)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDepthMap.DrawTextCanvas(CFLPoint<double>(0, 0), L"LEARN & INFERENCE DEPTH MAP", YELLOW, BLACK, 25)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerResultLabel.DrawTextCanvas(CFLPoint<double>(0, 0), L"RESULT BOX CONTOUR", GREEN, BLACK, 25)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layer3DResult.DrawTextCanvas(CFLPoint<double>(0, 0), L"MATCH 3D RESULT", GREEN, BLACK, 25)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageLearn.RedrawWindow();
		viewImageDepthMap.RedrawWindow();
		viewImagesBoxContour.RedrawWindow();
		view3DResult.RedrawWindow();

		// instanceSegmentation3DDL 객체 생성 // Create instanceSegmentation3D object
		CInstanceSegmentation3DDL instanceSegmentation3DDL;

		// OptimizerSpec 객체 생성 // Create OptimizerSpec object
		COptimizerSpecAdamGradientDescent optSpec;

		// 학습할 이미지 설정 // Set the image to learn
		instanceSegmentation3DDL.SetLearningImage(fliLearnImage);
		// 검증할 이미지 설정 // Set the image to validate
		instanceSegmentation3DDL.SetLearningValidationImage(fliLearnImage);
		// 학습할 Depth Map 설정 // Set the depth map to learn
		instanceSegmentation3DDL.SetLearningDepthMap(fliDepthMapImage);
		// 학습할 instanceSegmentation3DDL 모델 설정 // Set up instanceSegmentation3DDL model to learn
		instanceSegmentation3DDL.SetModel(CInstanceSegmentation3DDL::EModel_R_FLSegNet);
		// 학습할 instanceSegmentation3DDL 모델의 버전 설정 // Set up instanceSegmentation3DDL model version to learn
		instanceSegmentation3DDL.SetModelVersion(CInstanceSegmentation3DDL::EModelVersion_R_FLSegNet_V1_256);
		// 학습 epoch 값을 설정 // Set the learn epoch value 
		instanceSegmentation3DDL.SetLearningEpoch(1000);
		// 학습 이미지 Interpolation 방식 설정 // Set Interpolation method of learn image
		instanceSegmentation3DDL.SetInterpolationMethod(EInterpolationMethod_Bilinear);
		// 학습을 종료할 조건식 설정. mAP, Precision값이 1 이상인 경우 학습 종료한다.mAP 와 metric는 동일한 값입니다.
		// Set Conditional Expression to End Learning. If the mAP, Precision value is 1 or higher, end the learning. mAP is same value as metric.
		instanceSegmentation3DDL.SetLearningStopCondition(L"epoch >= 300 & mAP >= 1 & Precision >= 1");
		// 검증 IoU Threshold를 0.5로 설정 // Set IoU Threshold Of Validating to 0.5
		instanceSegmentation3DDL.SetValidationIoUThreshold(0.5f);

		CFLPoint<float> flpFocalLength(1000.f, 1000.f);
		CFLPoint<float> flpPrincipalPoint(0.f, 0.f);
		CFLArray<double> flaDistortionCoefficient(5, 0.);

		// 학습 시의 Camera Calibrator 설정 // Set the camera calibrator to learn
		instanceSegmentation3DDL.SetLearningCameraCalibrator(flpFocalLength, flpPrincipalPoint, 1.f, flaDistortionCoefficient, EDirectionType_Decrement);

		// Optimizer의 학습률 설정 // Set learning rate of Optimizer
		optSpec.SetLearningRate(1e-4f);

		// 설정한 Optimizer를 instanceSegmentation3D에 적용 // Apply the Optimizer that we set up to instanceSegmentation3D
		instanceSegmentation3DDL.SetLearningOptimizerSpec(optSpec);

		// AugmentationSpec 설정 // Set the AugmentationSpec
		CAugmentationSpec augSpec;

		augSpec.EnableAugmentation(true);
		augSpec.SetCommonIoUThreshold(0.800000);
		augSpec.SetCommonInterpolationMethod(EInterpolationMethod_Bilinear);
		augSpec.EnableHorizontalFlip(true);
		augSpec.EnableVerticalFlip(true);
		
		instanceSegmentation3DDL.SetLearningAugmentationSpec(&augSpec);

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		CAutoSaveSpec autoSaveSpec;

		// 자동 저장 활성화 // Enable Auto-Save
		// 저장 때문에 발생하는 속도 저하를 막기 위해 예제에서는 코드 사용법만 표시하고 옵션은 끔 // To prevent performance degradation caused by saving, the examples only demonstrate how to use the code, with the saving option disabled.
		autoSaveSpec.EnableAutoSave(false);
		// 저장할 모델 경로 설정 // Set Model path to save
		autoSaveSpec.SetAutoSavePath(L"model.flis3");
		// 자동 저장 조건식 설정. 현재 map값이 최대 값인 경우 저장 활성화
		// Set auto-save conditional expressions. Enable save if the current map value is the maximum value
		autoSaveSpec.SetAutoSaveCondition(L"map > max('map')");

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		instanceSegmentation3DDL.SetLearningAutoSaveSpec(autoSaveSpec);

		// Learn 동작을 하는 핸들 객체 선언 // Declare HANDLE object execute learn function
		HANDLE hThread;
		// instanceSegmentation3DDL learn function을 진행하는 스레드 생성 // Create the instanceSegmentation3DDL Learn function thread
		hThread = (HANDLE)_beginthreadex(NULL, 0, LearnThread, (void*)&instanceSegmentation3DDL, 0, nullptr);

		while(!instanceSegmentation3DDL.IsRunning() && !g_bTerminated)
			CThreadUtilities::Sleep(1);

		int32_t i32MaxEpoch = instanceSegmentation3DDL.GetLearningEpoch();
		int32_t i32PrevEpoch = 0;
		int32_t i32PrevCostCount = 0;
		int32_t i32PrevValidationCount = 0;

		while(true)
		{
			CThreadUtilities::Sleep(1);

			// 마지막 미니 배치 최대 반복 횟수 받기 // Get the last maximum number of iterations of the last mini batch 
			int32_t i32MaxIteration = instanceSegmentation3DDL.GetActualMiniBatchCount();
			// 마지막 미니 배치 반복 횟수 받기 // Get the last number of mini batch iterations
			int32_t i32Iteration = instanceSegmentation3DDL.GetLearningResultCurrentIteration();
			// 마지막 학습 횟수 받기 // Get the last epoch learning
			int32_t i32Epoch = instanceSegmentation3DDL.GetLastEpoch();

			// 미니 배치 반복이 완료되면 cost와 validation 값을 디스플레이 
			// Display cost and validation value if iterations of the mini batch is completed 
			if(i32Epoch != i32PrevEpoch && i32Iteration == i32MaxIteration && i32Epoch > 0)
			{
				// 마지막 학습 결과 비용 받기 // Get the last cost of the learning result
				float f32CurrCost = instanceSegmentation3DDL.GetLearningResultLastCost();
				// 마지막 검증 결과 받기 // Get the last validation result
				float f32MeanAP = instanceSegmentation3DDL.GetLearningResultLastMeanAP();
				// 마지막 Recall 결과 받기 // Get the last recall result
				float f32Recall = instanceSegmentation3DDL.GetLearningResultLastRecall();
				// 마지막 검증 결과 받기 // Get the last validation result
				float f32Precision = instanceSegmentation3DDL.GetLearningResultLastPrecision();

				// 해당 epoch의 비용과 검증 결과 값 출력 // Print cost and validation value for the relevant epoch
				printf("Cost : %.6f mAP : %.6f recall : %.6f precision : %.6f Epoch %d / %d\n", f32CurrCost, f32MeanAP, f32Recall, f32Precision, i32Epoch, i32MaxEpoch);

				// 학습 결과 비용과 검증 결과 기록을 받아 그래프 뷰에 출력  
				// Get the history of cost and validation and print it at graph view
				CFLArray<float> vctCosts;
				CFLArray<float> vctMeanAP;
				CFLArray<float> vctRecall;
				CFLArray<float> vctPrecision;
				CFLArray<int32_t> vctValidationEpoch;

				instanceSegmentation3DDL.GetLearningResultAllHistory(&vctCosts, &vctMeanAP, &vctValidationEpoch, &vctRecall, &vctPrecision);

				// 비용 기록이나 검증 결과 기록이 있다면 출력 // Print results if cost or validation history exists
				if((vctCosts.GetCount() && i32PrevCostCount != (int32_t)vctCosts.GetCount()) || (vctMeanAP.GetCount() && i32PrevValidationCount != (int32_t)vctMeanAP.GetCount()))
				{
					int32_t i32Step = instanceSegmentation3DDL.GetLearningValidationStep();
					CFLArray<float> flaX;

					for(int64_t i = 0; i < vctMeanAP.GetCount() - 1; ++i)
						flaX.PushBack((float)(i * i32Step));

					flaX.PushBack((float)(vctCosts.GetCount() - 1));

					// 이전 그래프의 데이터를 삭제 // Clear previous grpah data
					viewGraph.LockUpdate();
					viewGraph.Clear();
					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(vctCosts, EChartType_Line, RED, L"Cost");

					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(flaX, vctMeanAP, EChartType_Line, BLUE, L"mAP");
					viewGraph.Plot(flaX, vctRecall, EChartType_Line, GREEN, L"recall");
					viewGraph.Plot(flaX, vctPrecision, EChartType_Line, PURPLE, L"precision");

					viewGraph.UnlockUpdate();

					viewGraph.UpdateWindow();
					viewGraph.Invalidate();
					viewGraph.RedrawWindow();
				}

				if(GetAsyncKeyState(VK_ESCAPE))
					instanceSegmentation3DDL.Stop();

				i32PrevEpoch = i32Epoch;
				i32PrevCostCount = (int32_t)vctCosts.GetCount();
				i32PrevValidationCount = (int32_t)vctMeanAP.GetCount();
			}
			// epoch만큼 학습이 완료되면 종료 // End when learning progresses as much as epoch
			if(!instanceSegmentation3DDL.IsRunning() && g_bTerminated)
			{
				// learn 동작 스레드가 완전히 종료될 까지 대기 // Wait until learning is completely terminated
				WaitForSingleObject(hThread, INFINITE);
				// 스레드 핸들 종료 // Close thread handle
				CloseHandle(hThread);
				break;
			}
		}

		// Result Image에 Box & Contour 모두 출력하는 Execute // Execute to print both Box& Contour in Result Image
		// 분류할 이미지 설정 // Set the image to classify
		instanceSegmentation3DDL.SetInferenceImage(fliLearnImage);
		// 3D Match에 필요한 Depth Map 이미지 설정 // Set the depth map image for 3D Match
		instanceSegmentation3DDL.SetInferenceDepthMap(fliDepthMapImage);
		// 추론 결과 이미지 설정 // Set the inference result Image
		instanceSegmentation3DDL.SetInferenceResultImage(fliResultBoxContourImage);
		// 추론 결과 옵션 설정 // Set the inference result options
		// Figure 옵션 설정 // Set the option of figures
		CInstanceSegmentation3DDL::EInferenceResultItemSettings eFigureOption = (CInstanceSegmentation3DDL::EInferenceResultItemSettings)(CInstanceSegmentation3DDL::EInferenceResultItemSettings_ClassNum | CInstanceSegmentation3DDL::EInferenceResultItemSettings_ClassName | CInstanceSegmentation3DDL::EInferenceResultItemSettings_Objectness | CInstanceSegmentation3DDL::EInferenceResultItemSettings_BoundaryRect | CInstanceSegmentation3DDL::EInferenceResultItemSettings_Region);
		instanceSegmentation3DDL.SetInferenceResultItemSettings(eFigureOption);

		// 추론 시의 Camera Calibrator 설정 // Set the camera calibrator to inference
		instanceSegmentation3DDL.SetInferenceCameraCalibrator(flpFocalLength, flpPrincipalPoint, 1.f, flaDistortionCoefficient, EDirectionType_Decrement);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = instanceSegmentation3DDL.Execute()))
		{
			ErrorPrint(res, "Failed to execute.\n");
			break;
		}

		instanceSegmentation3DDL.GetInferenceResultObjectGroup(0, flogResult);

		int64_t i64ResultCount = flogResult.GetObjectCount();

		for(int64_t i = 0; i < i64ResultCount; i++)
		{
			const CFL3DObject* pFloResult = flogResult.GetObjectByIndex(i);

			if((res = view3DResult.PushObject(*pFloResult)).IsFail())
			{
				ErrorPrint(res, L"Failed to display the 3D object.\n");
				break;
			}

			SPoseMatrixParametersMulti sResult;
			TPoint3<float> tp3Min, tp3Max;
			TPoint3<double> tp3Mid;

			pFloResult->GetBoundingBox(tp3Min, tp3Max);
			tp3Mid.Set((tp3Min.x + tp3Max.x) * .5, (tp3Min.y + tp3Max.y) * .5, (tp3Min.z + tp3Max.z) * .5);
			// Pose Matrix 정보를 Layer 에 출력합니다. // Outputs Pose Matrix information to Layer.
			instanceSegmentation3DDL.GetInferenceResultPoseMatrix(0, i, sResult);
			layer3DResult.DrawText3D(tp3Mid, CFLString<wchar_t>().Format(L"[%lld]\nName: %s\nR(%.4lf, %.4lf, %.4lf)\nRVec(%.4lf, %.4lf, %.4lf)\nT(%.4lf, %.4lf, %.4lf)\nScore: %lf\nResidual: %lf", i, sResult.strClassName.GetBuffer(), sResult.tp3Angle.x, sResult.tp3Angle.y, sResult.tp3Angle.z, sResult.tp3RotationVector.x, sResult.tp3RotationVector.y, sResult.tp3RotationVector.z, sResult.tp3TranslationVector.x, sResult.tp3TranslationVector.y, sResult.tp3TranslationVector.z, sResult.f64Score, sResult.f64Residual), YELLOW, BLACK);
		}

		// 결과 이미지를 이미지 뷰에 맞게 조정합니다. // Fit the result image to the image view.
		viewImagesBoxContour.ZoomFit();
		view3DResult.ZoomFit();

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageLearn.RedrawWindow();
		viewImageDepthMap.RedrawWindow();
		viewImagesBoxContour.RedrawWindow();
		view3DResult.RedrawWindow();

		// 그래프 뷰를 갱신 // Update the Graph view.
		viewGraph.RedrawWindow();

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageLearn.IsAvailable() && viewImagesBoxContour.IsAvailable() && viewImagesBoxContour.IsAvailable() && view3DResult.IsAvailable() && viewGraph.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}