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
	CAnomalyDetectionDL* pAnomalyDetection = (CAnomalyDetectionDL*)pParam;

	CResult er = pAnomalyDetection->Learn();
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
	CFLImage fliValidationImage;
	CFLImage fliResultLabelFigureImage;

	/// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageLearn;
	CGUIViewImageWrap viewImageValidation;
	CGUIViewImageWrap viewImagesLabelFigure;

	// 그래프 뷰 선언 // Declare the graph view
	CGUIViewGraphWrap viewGraph;

	CResult res = EResult_UnknownError;

	do
	{
		// 라이브러리가 완전히 로드 될 때까지 기다림 // Wait for the library to fully load
		CThreadUtilities::Sleep(1000);

		// 이미지 로드 // Loads image
		if(IsFail(res = fliLearnImage.Load(L"../../ExampleImages/AnomalyDetection/AnomalyDetectionTrainData.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliValidationImage.Load(L"../../ExampleImages/AnomalyDetection/AnomalyDetectionValidationData.flif")))
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

		if(IsFail(res = viewImageValidation.Create(600, 0, 1100, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImagesLabelFigure.Create(100, 500, 600, 1000)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewGraph.Create(600, 500, 1100, 1000)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		viewGraph.SetDarkMode();

		// 다섯 개의 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the four image view windows
		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewImageValidation)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewImagesLabelFigure)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
		if(IsFail(res = viewImageLearn.SetImagePtr(&fliLearnImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageValidation.SetImagePtr(&fliValidationImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		fliResultLabelFigureImage.Assign(fliValidationImage);
		fliResultLabelFigureImage.ClearFigures();

		if(IsFail(res = viewImagesLabelFigure.SetImagePtr(&fliResultLabelFigureImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerLearn = viewImageLearn.GetLayer(0);
		CGUIViewImageLayerWrap layerInference = viewImageValidation.GetLayer(0);
		CGUIViewImageLayerWrap layerResultLabelFigure = viewImagesLabelFigure.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerLearn.Clear();
		layerInference.Clear();
		layerResultLabelFigure.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerLearn.DrawTextCanvas(CFLPoint<double>(0, 0), L"LEARN", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerInference.DrawTextCanvas(CFLPoint<double>(0, 0), L"INFERENCE", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerResultLabelFigure.DrawTextCanvas(CFLPoint<double>(0, 0), L"RESULT", GREEN, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageLearn.RedrawWindow();
		viewImageValidation.RedrawWindow();
		viewImagesLabelFigure.RedrawWindow();

		// AnomalyDetection 객체 생성 // Create AnomalyDetection object
		CAnomalyDetectionDL anomalyDetectionDL;

		// OptimizerSpec 객체 생성 // Create OptimizerSpec object
		COptimizerSpecAdamGradientDescent optSpec;

		// 학습할 이미지 설정 // Set the image to learn
		anomalyDetectionDL.SetLearningImage(fliLearnImage);
		// 검증할 이미지 설정 // Set the image to Validation
		anomalyDetectionDL.SetLearningValidationImage(fliValidationImage);
		// 학습할 AnomalyDetection 모델 설정 // Set up AnomalyDetection model to learn
		anomalyDetectionDL.SetModel(CAnomalyDetectionDL::EModel_FLDefNet);
		// 학습할 AnomalyDetection 모델의 버전 설정 // Set up AnomalyDetection model version to learn
		anomalyDetectionDL.SetModelVersion(CAnomalyDetectionDL::EModelVersion_FLDefNet_V1_64);
		// 학습 epoch 값을 설정 // Set the learn epoch value 
		anomalyDetectionDL.SetLearningEpoch(10000);
		// 학습 이미지 Interpolation 방식 설정 // Set Interpolation method of learn image
		anomalyDetectionDL.SetInterpolationMethod(EInterpolationMethod_Bilinear);

		// Optimizer의 학습률 설정 // Set learning rate of Optimizer
		optSpec.SetLearningRate(1e-3f);

		// 설정한 Optimizer를 AnomalyDetection에 적용 // Apply the Optimizer that we set up to AnomalyDetection
		anomalyDetectionDL.SetLearningOptimizerSpec(optSpec);

		// AugmentationSpec 설정 // Set the AugmentationSpec
		CAugmentationSpec augSpec;

		augSpec.EnableAugmentation(true);
		augSpec.SetCommonActivationRate(0.700000);
		augSpec.SetCommonIoUThreshold(0.000000);
		augSpec.SetCommonInterpolationMethod(EInterpolationMethod_Bilinear);

		augSpec.EnablePerspective(true);
		augSpec.SetPerspectiveParam(0., 0.1);

		augSpec.EnableScale(true);
		augSpec.SetScaleParam(0.900000, 1.100000, 0.900000, 1.100000, true);

		augSpec.EnablePerspective(true);
		augSpec.SetPerspectiveParam(0., 0.050000);

		anomalyDetectionDL.SetLearningAugmentationSpec(&augSpec);

		// 학습을 종료할 조건식 설정. accuracy값이 0.9 이상인 경우 학습 종료한다.
		// Set Conditional Expression to End Learning. If the accuracy value is 0.9 or more, end learning.
		anomalyDetectionDL.SetLearningStopCondition(L"accuracy >= 0.9");

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		CAutoSaveSpec autoSaveSpec;

		// 자동 저장 활성화 // Enable Auto-Save
		// 저장 때문에 발생하는 속도 저하를 막기 위해 예제에서는 코드 사용법만 표시하고 옵션은 끔 // To prevent performance degradation caused by saving, the examples only demonstrate how to use the code, with the saving option disabled.
		autoSaveSpec.EnableAutoSave(false);
		// 저장할 모델 경로 설정 // Set Model path to save
		autoSaveSpec.SetAutoSavePath(L"model.flad");
		// 자동 저장 조건식 설정. 현재 cost값이 최소이고 accuracy값이 최대 값인 경우 저장 활성화
		// Set auto-save conditional expressions. Enable save if the current cost value is minimum and the accumulation value is maximum
		autoSaveSpec.SetAutoSaveCondition(L"cost < min('cost') & accuracy > max('accuracy')");

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		anomalyDetectionDL.SetLearningAutoSaveSpec(autoSaveSpec);

		// Learn 동작을 하는 핸들 객체 선언 // Declare HANDLE object execute learn function
		HANDLE hThread;

		// AnomalyDetection learn function을 진행하는 스레드 생성 // Create the AnomalyDetection Learn function thread
		hThread = (HANDLE)_beginthreadex(NULL, 0, LearnThread, (void*)&anomalyDetectionDL, 0, nullptr);

		while(!anomalyDetectionDL.IsRunning() && !g_bTerminated)
			CThreadUtilities::Sleep(1);

		int32_t i32MaxEpoch = anomalyDetectionDL.GetLearningEpoch();
		int32_t i32PrevEpoch = 0;
		int32_t i32PrevCostCount = 0;
		int32_t i32PrevValidationCount = 0;

		while(true)
		{
			CThreadUtilities::Sleep(1);

			// 마지막 미니 배치 최대 반복 횟수 받기 // Get the last maximum number of iterations of the last mini batch 
			int32_t i32MaxIteration = anomalyDetectionDL.GetActualMiniBatchCount();
			// 마지막 미니 배치 반복 횟수 받기 // Get the last number of mini batch iterations
			int32_t i32Iteration = anomalyDetectionDL.GetLearningResultCurrentIteration();
			// 마지막 학습 횟수 받기 // Get the last epoch learning
			int32_t i32Epoch = anomalyDetectionDL.GetLastEpoch();

			// 미니 배치 반복이 완료되면 cost와 validation 값을 디스플레이 
			// Display cost and validation value if iterations of the mini batch is completed 
			if(i32Epoch != i32PrevEpoch && i32Iteration == i32MaxIteration && i32Epoch > 0)
			{
				// 마지막 학습 결과 비용 받기 // Get the last cost of the learning result
				float f32CurrCost = anomalyDetectionDL.GetLearningResultLastCost();
				// 마지막 검증 결과 받기 // Get the last validation result
				float f32ValidationPa = anomalyDetectionDL.GetLearningResultLastAccuracy();

				// 해당 epoch의 비용과 검증 결과 값 출력 // Print cost and validation value for the relevant epoch
				printf("Cost : %.6f Accuracy : %.6f Epoch %d / %d\n", f32CurrCost, f32ValidationPa, i32Epoch, i32MaxEpoch);

				// 학습 결과 비용과 검증 결과 기록을 받아 그래프 뷰에 출력  
				// Get the history of cost and validation and print it at graph view
				CFLArray<float> flaCostHistory;
				CFLArray<float> flaValidationHistory;
				CFLArray<int32_t> vctValidationEpoch;

				anomalyDetectionDL.GetLearningResultAllHistory(&flaCostHistory, &flaValidationHistory, &vctValidationEpoch);

				// 비용 기록이나 검증 결과 기록이 있다면 출력 // Print results if cost or validation history exists
				if((flaCostHistory.GetCount() && i32PrevCostCount != (int32_t)flaCostHistory.GetCount()) || (flaValidationHistory.GetCount() && i32PrevValidationCount != (int32_t)flaValidationHistory.GetCount()))
				{
					int32_t i32Step = anomalyDetectionDL.GetLearningValidationStep();
					CFLArray<float> flaX;

					for(int64_t i = 0; i < flaValidationHistory.GetCount() - 1; ++i)
						flaX.PushBack((float)(i * i32Step));

					flaX.PushBack((float)(flaCostHistory.GetCount() - 1));

					// 이전 그래프의 데이터를 삭제 // Clear previous graph data
					viewGraph.LockUpdate();
					viewGraph.Clear();
					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(flaCostHistory, EChartType_Line, RED, L"Cost");

					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(flaX, flaValidationHistory, EChartType_Line, BLUE, L"Accuracy");
					viewGraph.UnlockUpdate();

					viewGraph.UpdateWindow();
					viewGraph.Invalidate();
					viewGraph.RedrawWindow();
				}

				// 검증 결과가 1.0일 경우 학습을 중단하고 분류 진행 
				// If the validation result is 1.0, stop learning and classify images 
				if(f32ValidationPa == 1.f || GetAsyncKeyState(VK_ESCAPE))
					anomalyDetectionDL.Stop();

				i32PrevEpoch = i32Epoch;
				i32PrevCostCount = (int32_t)flaCostHistory.GetCount();
				i32PrevValidationCount = (int32_t)flaValidationHistory.GetCount();
			}
			// epoch만큼 학습이 완료되면 종료 // End when learning progresses as much as epoch
			if(!anomalyDetectionDL.IsRunning() && g_bTerminated)
			{
				// learn 동작 스레드가 완전히 종료될 까지 대기 // Wait until learning is completely terminated
				WaitForSingleObject(hThread, INFINITE);
				// 스레드 핸들 종료 // Close thread handle
				CloseHandle(hThread);
				break;
			}
		}

		// Result Label Image에 피겨를 포함하지 않는 Execute
		// 분류할 이미지 설정 // Set the image to classify
		anomalyDetectionDL.SetInferenceImage(fliValidationImage);
		// 추론 결과 이미지 설정 // Set the inference result Image
		anomalyDetectionDL.SetInferenceResultImage(fliResultLabelFigureImage);
		// 추론 결과 옵션 설정 // Set the inference result options;
		// 비정상 결과 비교 Threshold 설정 // Set Anomaly Threshold
		anomalyDetectionDL.SetInferenceAnomalyThreshold(0.6f);
		// 비정상 최소 크기 설정 // Set Minimum Anomaly Area
		anomalyDetectionDL.SetInferenceMinimumAnomalyArea(4);
		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = anomalyDetectionDL.Execute()))
		{
			ErrorPrint(res, "Failed to execute.\n");
			break;
		}

		// 결과 이미지를 이미지 뷰에 맞게 조정합니다. // Fit the result image to the image view.
		viewImagesLabelFigure.ZoomFit();

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageLearn.RedrawWindow();
		viewImageValidation.RedrawWindow();
		viewImagesLabelFigure.RedrawWindow();

		// 그래프 뷰를 갱신 // Update the Graph view.
		viewGraph.RedrawWindow();

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageLearn.IsAvailable() && viewImageValidation.IsAvailable() && viewImagesLabelFigure.IsAvailable() && viewGraph.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}