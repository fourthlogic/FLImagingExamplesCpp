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
	CInstanceSegmentationDL* pInstanceSegmentation = (CInstanceSegmentationDL*)pParam;

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
	CFLImage fliValidationImage;
	CFLImage fliResultBoxContourImage;
	CFLImage fliResultContourImage;

	/// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageLearn;
	CGUIViewImageWrap viewImageValidation;
	CGUIViewImageWrap viewImagesBoxContour;
	CGUIViewImageWrap viewImagesContour;

	// 그래프 뷰 선언 // Declare the graph view
	CGUIViewGraphWrap viewGraph;

	CResult res = EResult_UnknownError;

	do
	{
		// 라이브러리가 완전히 로드 될 때까지 기다림 // Wait for the library to fully load
		CThreadUtilities::Sleep(1000);

		// 이미지 로드 // Loads image
		if(IsFail(res = fliLearnImage.Load(L"../../ExampleImages/InstanceSegmentation/Bolt.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliValidationImage.Load(L"../../ExampleImages/InstanceSegmentation/Bolt.flif")))
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

		if(IsFail(res = viewImagesBoxContour.Create(100, 500, 600, 1000)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImagesContour.Create(600, 500, 1100, 1000)))
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
		if(IsFail(res = viewImageValidation.SynchronizePointOfView(&viewImagesContour)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImageValidation.SynchronizePointOfView(&viewImagesBoxContour)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 다섯 개의 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the four image view windows
		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewImageValidation)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewImagesBoxContour)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewImagesContour)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewGraph)))
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

		if(IsFail(res = viewImagesBoxContour.SetImagePtr(&fliResultBoxContourImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImagesContour.SetImagePtr(&fliResultContourImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerLearn = viewImageLearn.GetLayer(0);
		CGUIViewImageLayerWrap layerValidation = viewImageValidation.GetLayer(0);
		CGUIViewImageLayerWrap layerResultLabel = viewImagesBoxContour.GetLayer(0);
		CGUIViewImageLayerWrap layerResultLabelFigure = viewImagesContour.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerLearn.Clear();
		layerValidation.Clear();
		layerResultLabel.Clear();
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

		if(IsFail(res = layerValidation.DrawTextCanvas(CFLPoint<double>(0, 0), L"VALIDATION", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerResultLabel.DrawTextCanvas(CFLPoint<double>(0, 0), L"RESULT BOX CONTOUR", GREEN, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerResultLabelFigure.DrawTextCanvas(CFLPoint<double>(0, 0), L"RESULT CONTOUR", GREEN, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageLearn.RedrawWindow();
		viewImageValidation.RedrawWindow();
		viewImagesBoxContour.RedrawWindow();
		viewImagesContour.RedrawWindow();

		// instanceSegmentationDL 객체 생성 // Create intanceSegmentation object
		CInstanceSegmentationDL instanceSegmentationDL;

		// OptimizerSpec 객체 생성 // Create OptimizerSpec object
		COptimizerSpecAdamGradientDescent optSpec;

		// 학습할 이미지 설정 // Set the image to learn
		instanceSegmentationDL.SetLearningImage(fliLearnImage);
		// 검증할 이미지 설정 // Set the image to validate
		instanceSegmentationDL.SetLearningValidationImage(fliValidationImage);
		// 학습할 instanceSegmentationDL 모델 설정 // Set up instanceSegmentationDL model to learn
		instanceSegmentationDL.SetModel(CInstanceSegmentationDL::EModel_R_FLSegNet_V2);
		// 학습할 instanceSegmentationDL 모델의 버전 설정 // Set up instanceSegmentationDL model version to learn
		instanceSegmentationDL.SetModelVersion(CInstanceSegmentationDL::EModelVersion_R_FLSegNet_V2_256);
		// 학습 epoch 값을 설정 // Set the learn epoch value 
		instanceSegmentationDL.SetLearningEpoch(500);
		// 학습 이미지 Interpolation 방식 설정 // Set Interpolation method of learn image
		instanceSegmentationDL.SetInterpolationMethod(EInterpolationMethod_Bilinear);
		// 학습을 종료할 조건식 설정. mAP값이 0.85 이상인 경우 학습 종료한다. metric와 동일한 값입니다.
		// Set Conditional Expression to End Learning. If the mAP value is 0.85 or higher, end the learning. Same value as metric.
		instanceSegmentationDL.SetLearningStopCondition(L"mAP >= 0.85");
		// 검증 IoU Threshold를 0.7로 설정 // Set IoU Threshold Of Validating to 0.7 
		instanceSegmentationDL.SetValidationIoUThreshold(0.7f);

		// Optimizer의 학습률 설정 // Set learning rate of Optimizer
		optSpec.SetLearningRate(1e-4f);

		// 설정한 Optimizer를 intanceSegmentation에 적용 // Apply the Optimizer that we set up to intanceSegmentation
		instanceSegmentationDL.SetLearningOptimizerSpec(optSpec);

		// AugmentationSpec 설정 // Set the AugmentationSpec
		CAugmentationSpec augSpec;

		augSpec.EnableAugmentation(true);
		augSpec.SetCommonActivationRate(0.700000);
		augSpec.SetCommonIoUThreshold(0.800000);
		augSpec.SetCommonInterpolationMethod(EInterpolationMethod_Bilinear);
		augSpec.EnableHorizontalFlip(true);
		augSpec.EnableVerticalFlip(true);
		augSpec.EnableScale(true);
		augSpec.SetScaleParam(1.000000, 1.000000, 1.000000, 1.000000, true);

		instanceSegmentationDL.SetLearningAugmentationSpec(&augSpec);

		// 학습을 종료할 조건식 설정. mAP값이 0.95 이상인 경우 학습 종료한다. metric와 동일한 값입니다.
		// Set Conditional Expression to End Learning. If the mAP value is 0.95 or higher, end the learning. Same value as metric.
		instanceSegmentationDL.SetLearningStopCondition(L"map >= 0.95");

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		CAutoSaveSpec autoSaveSpec;

		// 자동 저장 활성화 // Enable Auto-Save
		// 저장 때문에 발생하는 속도 저하를 막기 위해 예제에서는 코드 사용법만 표시하고 옵션은 끔 // To prevent performance degradation caused by saving, the examples only demonstrate how to use the code, with the saving option disabled.
		autoSaveSpec.EnableAutoSave(false);
		// 저장할 모델 경로 설정 // Set Model path to save
		autoSaveSpec.SetAutoSavePath(L"model.flis");
		// 자동 저장 조건식 설정. 현재 map값이 최대 값인 경우 저장 활성화
		// Set auto-save conditional expressions. Enable save if the current map value is the maximum value
		autoSaveSpec.SetAutoSaveCondition(L"map > max('map')");

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		instanceSegmentationDL.SetLearningAutoSaveSpec(autoSaveSpec);

		// Learn 동작을 하는 핸들 객체 선언 // Declare HANDLE object execute learn function
		HANDLE hThread;
		// instanceSegmentationDL learn function을 진행하는 스레드 생성 // Create the instanceSegmentationDL Learn function thread
		hThread = (HANDLE)_beginthreadex(NULL, 0, LearnThread, (void*)&instanceSegmentationDL, 0, nullptr);

		while(!instanceSegmentationDL.IsRunning() && !g_bTerminated)
			CThreadUtilities::Sleep(1);

		int32_t i32MaxEpoch = instanceSegmentationDL.GetLearningEpoch();
		int32_t i32PrevEpoch = 0;
		int32_t i32PrevCostCount = 0;
		int32_t i32PrevValidationCount = 0;

		while(true)
		{
			CThreadUtilities::Sleep(1);

			// 마지막 미니 배치 최대 반복 횟수 받기 // Get the last maximum number of iterations of the last mini batch 
			int32_t i32MaxIteration = instanceSegmentationDL.GetActualMiniBatchCount();
			// 마지막 미니 배치 반복 횟수 받기 // Get the last number of mini batch iterations
			int32_t i32Iteration = instanceSegmentationDL.GetLearningResultCurrentIteration();
			// 마지막 학습 횟수 받기 // Get the last epoch learning
			int32_t i32Epoch = instanceSegmentationDL.GetLastEpoch();

			// 미니 배치 반복이 완료되면 cost와 validation 값을 디스플레이 
			// Display cost and validation value if iterations of the mini batch is completed 
			if(i32Epoch != i32PrevEpoch && i32Iteration == i32MaxIteration && i32Epoch > 0)
			{
				// 마지막 학습 결과 비용 받기 // Get the last cost of the learning result
				float f32CurrCost = instanceSegmentationDL.GetLearningResultLastCost();
				// 마지막 검증 결과 받기 // Get the last validation result
				float f32MeanAP = instanceSegmentationDL.GetLearningResultLastMeanAP();
				// 마지막 Recall 결과 받기 // Get the last recall result
				float f32Recall = instanceSegmentationDL.GetLearningResultLastRecall();
				// 마지막 검증 결과 받기 // Get the last validation result
				float f32Precision = instanceSegmentationDL.GetLearningResultLastPrecision();

				// 해당 epoch의 비용과 검증 결과 값 출력 // Print cost and validation value for the relevant epoch
				printf("Cost : %.6f mAP : %.6f recall : %.6f precision : %.6f Epoch %d / %d\n", f32CurrCost, f32MeanAP, f32Recall, f32Precision, i32Epoch, i32MaxEpoch);

				// 학습 결과 비용과 검증 결과 기록을 받아 그래프 뷰에 출력  
				// Get the history of cost and validation and print it at graph view
				CFLArray<float> vctCosts;
				CFLArray<float> vctMeanAP;
				CFLArray<float> vctRecall;
				CFLArray<float> vctPrecision;
				CFLArray<int32_t> vctValidationEpoch;

				instanceSegmentationDL.GetLearningResultAllHistory(&vctCosts, &vctMeanAP, &vctValidationEpoch, &vctRecall, &vctPrecision);

				// 비용 기록이나 검증 결과 기록이 있다면 출력 // Print results if cost or validation history exists
				if((vctCosts.GetCount() && i32PrevCostCount != (int32_t)vctCosts.GetCount()) || (vctMeanAP.GetCount() && i32PrevValidationCount != (int32_t)vctMeanAP.GetCount()))
				{
					int32_t i32Step = instanceSegmentationDL.GetLearningValidationStep();
					CFLArray<float> flaX;

					for(int64_t i = 0; i < vctMeanAP.GetCount() - 1; ++i)
						flaX.PushBack((float)(i * i32Step));

					flaX.PushBack((float)(vctCosts.GetCount() - 1));

					// 이전 그래프의 데이터를 삭제 // Clear previous graph data
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

				// 검증 결과가 1.0일 경우 학습을 중단하고 분류 진행 
				// If the validation result is 1.0, stop learning and classify images 
				if(f32MeanAP == 1.f || GetAsyncKeyState(VK_ESCAPE))
					instanceSegmentationDL.Stop();

				i32PrevEpoch = i32Epoch;
				i32PrevCostCount = (int32_t)vctCosts.GetCount();
				i32PrevValidationCount = (int32_t)vctMeanAP.GetCount();
			}
			// epoch만큼 학습이 완료되면 종료 // End when learning progresses as much as epoch
			if(!instanceSegmentationDL.IsRunning() && g_bTerminated)
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
		instanceSegmentationDL.SetInferenceImage(fliValidationImage);
		// 추론 결과 이미지 설정 // Set the inference result Image
		instanceSegmentationDL.SetInferenceResultImage(fliResultBoxContourImage);
		// 추론 결과 옵션 설정 // Set the inference result options
		// Figure 옵션 설정 // Set the option of figures
		CInstanceSegmentationDL::EInferenceResultItemSettings eFigureOption = (CInstanceSegmentationDL::EInferenceResultItemSettings)(CInstanceSegmentationDL::EInferenceResultItemSettings_ClassNum | CInstanceSegmentationDL::EInferenceResultItemSettings_ClassName | CInstanceSegmentationDL::EInferenceResultItemSettings_Objectness);
		instanceSegmentationDL.SetInferenceResultItemSettings(eFigureOption);
		CInstanceSegmentationDL::EInferenceResultRegionFigureType eFigureType = (CInstanceSegmentationDL::EInferenceResultRegionFigureType)(CInstanceSegmentationDL::EInferenceResultRegionFigureType_Region | CInstanceSegmentationDL::EInferenceResultRegionFigureType_BoundaryRectangle);
		instanceSegmentationDL.SetInferenceResultRegionFigureType(eFigureType);

		// Objectness Threshold 설정 // Set the obectness threshold
		instanceSegmentationDL.SetInferenceResultObjectnessThreshold(0.5f);
		// Mask Threshold 설정 // Set The mask threshold
		instanceSegmentationDL.SetInferenceResultMaskThreshold(0.5f);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = instanceSegmentationDL.Execute()))
		{
			ErrorPrint(res, "Failed to execute.\n");
			break;
		}

		// Result Image에 Contour 만 출력하는 Execute // Execute to print both Box& Contour in Result Image
		// 분류할 이미지 설정 // Set the image to classify
		instanceSegmentationDL.SetInferenceImage(fliValidationImage);
		// 추론 결과 이미지 설정 // Set the inference result Image
		instanceSegmentationDL.SetInferenceResultImage(fliResultContourImage);
		// 추론 결과 옵션 설정 // Set the inference result options
		// Figure 옵션 설정 // Set the option of figures
		eFigureOption = CInstanceSegmentationDL::EInferenceResultItemSettings_None;
		eFigureType = CInstanceSegmentationDL::EInferenceResultRegionFigureType_Region;
		instanceSegmentationDL.SetInferenceResultItemSettings(eFigureOption);
		instanceSegmentationDL.SetInferenceResultRegionFigureType(eFigureType);
		// Objectness Threshold 설정 // Set the obectness threshold
		instanceSegmentationDL.SetInferenceResultObjectnessThreshold(0.5f);
		// Mask Threshold 설정 // Set The mask threshold
		instanceSegmentationDL.SetInferenceResultMaskThreshold(0.5f);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = instanceSegmentationDL.Execute()))
		{
			ErrorPrint(res, "Failed to execute.\n");
			break;
		}

		// 결과 이미지를 이미지 뷰에 맞게 조정합니다. // Fit the result image to the image view.
		viewImagesBoxContour.ZoomFit();
		viewImagesContour.ZoomFit();

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageLearn.RedrawWindow();
		viewImageValidation.RedrawWindow();
		viewImagesBoxContour.RedrawWindow();
		viewImagesContour.RedrawWindow();

		// 그래프 뷰를 갱신 // Update the Graph view.
		viewGraph.RedrawWindow();

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageLearn.IsAvailable() && viewImageValidation.IsAvailable() && viewImagesBoxContour.IsAvailable() && viewImagesContour.IsAvailable() && viewGraph.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}