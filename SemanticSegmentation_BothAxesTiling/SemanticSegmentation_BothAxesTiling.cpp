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
	CSemanticSegmentationDL* pSemanticSegmentation = (CSemanticSegmentationDL*)pParam;

	CResult er = pSemanticSegmentation->Learn();
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
	CFLImage fliResultLabelImage;
	CFLImage fliResultLabelFigureImage;

	/// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageLearn;
	CGUIViewImageWrap viewImageValidation;
	CGUIViewImageWrap viewImagesLabel;
	CGUIViewImageWrap viewImagesLabelFigure;

	// 그래프 뷰 선언 // Declare the graph view
	CGUIViewGraphWrap viewGraph;

	CResult res = EResult_UnknownError;

	do
	{
		// 라이브러리가 완전히 로드 될 때까지 기다림 // Wait for the library to fully load
		CThreadUtilities::Sleep(1000);

		// 이미지 로드 // Loads image
		if(IsFail(res = fliLearnImage.Load(L"../../ExampleImages/SemanticSegmentation/Cityscape.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliValidationImage.Load(L"../../ExampleImages/SemanticSegmentation/Cityscape.flif")))
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

		if(IsFail(res = viewImagesLabel.Create(100, 500, 600, 1000)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImagesLabelFigure.Create(600, 500, 1100, 1000)))
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

		// 다섯 개의 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the four image view windows
		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewImageValidation)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewImagesLabel)))
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

		viewImagesLabel.EnablePixelSegmentationMode(true);

		if(IsFail(res = viewImagesLabel.SetImagePtr(&fliResultLabelImage)))
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
		CGUIViewImageLayerWrap layerValidation = viewImageValidation.GetLayer(0);
		CGUIViewImageLayerWrap layerResultLabel = viewImagesLabel.GetLayer(0);
		CGUIViewImageLayerWrap layerResultLabelFigure = viewImagesLabelFigure.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerLearn.Clear();
		layerValidation.Clear();
		layerResultLabel.Clear();
		layerResultLabelFigure.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 
		//                 폰트 크기 -> 실제 크기 유무 -> 각도 -> 얼라인 -> 폰트 이름 -> 
		//                 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> 
		//                  Area color -> font size -> actual size -> angle -> Align -> Font Name -> 
		//                  Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> 
		//                  Font Thickness -> Font Italic
		if(IsFail(res = layerLearn.DrawTextCanvas(&CFLPoint<double>(0, 0), L"LEARN", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerValidation.DrawTextCanvas(&CFLPoint<double>(0, 0), L"VALIDATION", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerResultLabel.DrawTextCanvas(&CFLPoint<double>(0, 0), L"RESULT", GREEN, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerResultLabelFigure.DrawTextCanvas(&CFLPoint<double>(0, 0), L"RESULT FIGURE", GREEN, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageLearn.RedrawWindow();
		viewImageValidation.RedrawWindow();
		viewImagesLabel.RedrawWindow();
		viewImagesLabelFigure.RedrawWindow();

		// SemanticSegmentation 객체 생성 // Create SemanticSegmentation object
		CSemanticSegmentationDL semanticSegmentation;

		// OptimizerSpec 객체 생성 // Create OptimizerSpec object
		COptimizerSpecAdamGradientDescent optSpec;

		// 학습할 이미지 설정 // Set the image to learn
		semanticSegmentation.SetLearningImage(fliLearnImage);
		// 검증할 이미지 설정 // Set the image to validate
		semanticSegmentation.SetLearningValidationImage(fliValidationImage);
		// 학습할 SemanticSegmentation 모델 설정 // Set up SemanticSegmentation model to learn
		semanticSegmentation.SetModel(CSemanticSegmentationDL::EModel_FLSegNet);
		// 학습할 SemanticSegmentation 모델의 버전 설정 // Set up SemanticSegmentation model version to learn
		semanticSegmentation.SetModelVersion(CSemanticSegmentationDL::EModelVersion_FLSegNet_V1_256_B1);
		// 학습 epoch 값을 설정 // Set the learn epoch value 
		semanticSegmentation.SetLearningEpoch(1000);
		// 학습 이미지 Interpolation 방식 설정 // Set Interpolation method of learn image
		semanticSegmentation.SetInterpolationMethod(EInterpolationMethod_Bilinear);
		// 모델의 최적의 상태를 추적 후 마지막에 최적의 상태로 적용할 지 여부 설정 // Set whether to track the optimal state of the model and apply it as the optimal state at the end.
		semanticSegmentation.EnableOptimalLearningStatePreservation(true);

		// Optimizer의 학습률 설정 // Set learning rate of Optimizer
		optSpec.SetLearningRate(1e-3f);

		// 설정한 Optimizer를 SemanticSegmentation에 적용 // Apply the Optimizer that we set up to SemanticSegmentation
		semanticSegmentation.SetLearningOptimizerSpec(optSpec);

		// 학습을 종료할 조건식 설정. miou.ze값이 0.85 이상인 경우 학습 종료한다. metric.ze와 동일한 값입니다.
		// Set Conditional Expression to End Learning. If the miou.ze value is 0.85 or higher, end the learning. Same value as metric.ze.
		semanticSegmentation.SetLearningStopCondition(L"mIoU.ze >= 0.85");

		// 학습 이미지 분할 모드 설정.
		// 정사각형 비율을 유지하여 모델 크기로 이미지를 나누어 처리한다.
		// Set the training image segmentation mode.
		// The original image is divided and processed by the model size by maintaining the square ratio.
		semanticSegmentation.SetImageTilingMode(ETilingMode_BothAxesTiling_NoFit);
		// 학습 이미지 분할 모드의 겹치는 비율을 설정 // Set the overlap ratio of the learning image in tiling mode
		semanticSegmentation.SetImageTilingOverlapRatio(0.25);

		// 학습을 종료할 조건식 설정. mIoU.ze값이 0.85 이상인 경우 학습 종료한다. metric.ze와 동일한 값입니다.
		// Set Conditional Expression to End Learning. If the mIoU.ze value is 0.85 or higher, end the learning. Same value as metric.ze.
		semanticSegmentation.SetLearningStopCondition(L"miou.ze >= 0.85");

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		CAutoSaveSpec autoSaveSpec;

		// 자동 저장 활성화 // Enable Auto-Save
		// 저장 때문에 발생하는 속도 저하를 막기 위해 예제에서는 코드 사용법만 표시하고 옵션은 끔 // To prevent performance degradation caused by saving, the examples only demonstrate how to use the code, with the saving option disabled.
		autoSaveSpec.EnableAutoSave(false);
		// 저장할 모델 경로 설정 // Set Model path to save
		autoSaveSpec.SetAutoSavePath(L"model.flss");
		// 자동 저장 조건식 설정. 현재 miou.ze값이 최대 값인 경우 저장 활성화
		// Set auto-save conditional expressions. Enable save if the current miou.ze value is the maximum value
		autoSaveSpec.SetAutoSaveCondition(L"miou.ze > max('miou.ze')");

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		semanticSegmentation.SetLearningAutoSaveSpec(autoSaveSpec);

		// Learn 동작을 하는 핸들 객체 선언 // Declare HANDLE object execute learn function
		HANDLE hThread;

		// SemanticSegmentation learn function을 진행하는 스레드 생성 // Create the SemanticSegmentation Learn function thread
		hThread = (HANDLE)_beginthreadex(NULL, 0, LearnThread, (void*)&semanticSegmentation, 0, nullptr);

		while(!semanticSegmentation.IsRunning() && !g_bTerminated)
			CThreadUtilities::Sleep(1);

		int32_t i32MaxEpoch = semanticSegmentation.GetLearningEpoch();
		int32_t i32PrevEpoch = 0;
		int32_t i32PrevCostCount = 0;
		int32_t i32PrevValidationCount = 0;

		while(true)
		{
			CThreadUtilities::Sleep(1);

			// 마지막 미니 배치 최대 반복 횟수 받기 // Get the last maximum number of iterations of the last mini batch 
			int32_t i32MaxIteration = semanticSegmentation.GetActualMiniBatchCount();
			// 마지막 미니 배치 반복 횟수 받기 // Get the last number of mini batch iterations
			int32_t i32Iteration = semanticSegmentation.GetLearningResultCurrentIteration();
			// 마지막 학습 횟수 받기 // Get the last epoch learning
			int32_t i32Epoch = semanticSegmentation.GetLastEpoch();

			// 미니 배치 반복이 완료되면 cost와 validation 값을 디스플레이 
			// Display cost and validation value if iterations of the mini batch is completed 
			if(i32Epoch != i32PrevEpoch && i32Iteration == i32MaxIteration && i32Epoch > 0)
			{
				// 마지막 학습 결과 비용 받기 // Get the last cost of the learning result
				float f32CurrCost = semanticSegmentation.GetLearningResultLastCost();
				// 마지막 검증 결과 받기 // Get the last validation result
				float f32ValidationPa = semanticSegmentation.GetLearningResultLastAccuracy();
				float f32ValidationMeanIoU = semanticSegmentation.GetLearningResultLastMeanIoU();

				// 해당 epoch의 비용과 검증 결과 값 출력 // Print cost and validation value for the relevant epoch
				printf("Cost : %.6f Validation : %.6f mIoU : %.6f Epoch %d / %d\n", f32CurrCost, f32ValidationPa, f32ValidationMeanIoU, i32Epoch, i32MaxEpoch);

				// 학습 결과 비용과 검증 결과 기록을 받아 그래프 뷰에 출력  
				// Get the history of cost and validation and print it at graph view
				CFLArray<float> vctCosts;
				CFLArray<float> vctVadliationPixelAccuracy;
				CFLArray<float> vctMeanIoU;
				CFLArray<float> vctVadliationPixelAccuracyZE;
				CFLArray<float> vctMeanIoUZE;
				CFLArray<int32_t> vctValidationEpoch;

				semanticSegmentation.GetLearningResultAllHistory(&vctCosts, &vctVadliationPixelAccuracy, &vctMeanIoU, &vctVadliationPixelAccuracyZE, &vctMeanIoUZE, &vctValidationEpoch);

				// 비용 기록이나 검증 결과 기록이 있다면 출력 // Print results if cost or validation history exists
				if((vctCosts.GetCount() && i32PrevCostCount != (int32_t)vctCosts.GetCount()) || (vctVadliationPixelAccuracy.GetCount() && i32PrevValidationCount != (int32_t)vctVadliationPixelAccuracy.GetCount()))
				{
					int32_t i32Step = semanticSegmentation.GetLearningValidationStep();
					CFLArray<float> flaX;

					for(int64_t i = 0; i < vctVadliationPixelAccuracy.GetCount() - 1; ++i)
						flaX.PushBack((float)(i * i32Step));

					flaX.PushBack((float)(vctCosts.GetCount() - 1));

					// 이전 그래프의 데이터를 삭제 // Clear previous grpah data
					viewGraph.LockUpdate();
					viewGraph.Clear();
					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(vctCosts, EChartType_Line, RED, L"Cost");

					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(flaX, vctVadliationPixelAccuracy, EChartType_Line, BLUE, L"PixelAccuracy(Zero Exception)");
					viewGraph.Plot(flaX, vctMeanIoU, EChartType_Line, PINK, L"mIoU");
					viewGraph.UnlockUpdate();

					viewGraph.UpdateWindow();
					viewGraph.Invalidate();
					viewGraph.RedrawWindow();
				}

				// 검증 결과가 1.0일 경우 학습을 중단하고 분류 진행 
				// If the validation result is 1.0, stop learning and classify images 
				if(f32ValidationPa == 1.f || GetAsyncKeyState(VK_ESCAPE))
					semanticSegmentation.Stop();

				i32PrevEpoch = i32Epoch;
				i32PrevCostCount = (int32_t)vctCosts.GetCount();
				i32PrevValidationCount = (int32_t)vctVadliationPixelAccuracy.GetCount();
			}
			// epoch만큼 학습이 완료되면 종료 // End when learning progresses as much as epoch
			if(!semanticSegmentation.IsRunning() && g_bTerminated)
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
		semanticSegmentation.SetInferenceImage(fliValidationImage);
		// 추론 결과 이미지 설정 // Set the inference result Image
		semanticSegmentation.SetInferenceResultImage(fliResultLabelImage);
		// 추론 결과 옵션 설정 // Set the inference result options;
		// Result 결과를 Label Image로 받을지 여부 설정 // Set whether to receive the result as a Label Image
		semanticSegmentation.EnableInferenceResultLabelImage(true);
		// Result 결과에 Region Figure를 포함 여부 설정 // Set whether to include region figure in result
		semanticSegmentation.EnableInferenceResultIncludingRegionFigures(false);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = semanticSegmentation.Execute()))
		{
			ErrorPrint(res, "Failed to execute.\n");
			break;
		}

		// Result Label Image에 피겨를 포함한 Execute
		// 추론 결과 이미지 설정 // Set the inference result Image
		semanticSegmentation.SetInferenceResultImage(fliResultLabelFigureImage);
		// 추론 결과 옵션 설정 // Set the inference result options;
		// Result 결과를 Label Image로 받을지 여부 설정 // Set whether to receive the result as a Label Image
		semanticSegmentation.EnableInferenceResultLabelImage(false);
		// Result 결과에 Region Figure를 포함 여부 설정 // Set whether to include region figure in result
		semanticSegmentation.EnableInferenceResultIncludingRegionFigures(true);
		// Result item settings enum 설정 // Set the result item settings
		semanticSegmentation.SetInferenceResultItemSettings(CSemanticSegmentationDL::EInferenceResultItemSettings_ClassNum_ClassName_ConfidenceScore_RegionType_Contour);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = semanticSegmentation.Execute()))
		{
			ErrorPrint(res, "Failed to execute.\n");
			break;
		}


		// 결과 이미지를 이미지 뷰에 맞게 조정합니다. // Fit the result image to the image view.
		viewImagesLabel.ZoomFit();
		viewImagesLabelFigure.ZoomFit();

		// ResultLabel 뷰에 Floating Value Range를 설정 // Set Floating Value Range in ResultLabel view
		viewImagesLabel.SetFloatingImageValueRange(0.f, (float)semanticSegmentation.GetLearningResultClassCount());

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageLearn.RedrawWindow();
		viewImageValidation.RedrawWindow();
		viewImagesLabel.RedrawWindow();
		viewImagesLabelFigure.RedrawWindow();

		// 그래프 뷰를 갱신 // Update the Graph view.
		viewGraph.RedrawWindow();

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageLearn.IsAvailable() && viewImageValidation.IsAvailable() && viewImagesLabel.IsAvailable() && viewImagesLabelFigure.IsAvailable() && viewGraph.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}