#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"
#include <cfloat>
#include <cmath>
#include <iostream>
#include <limits>
#include <process.h>

bool g_bTerminated = false;
CResult g_resLearnResult;
// 
unsigned int __stdcall LearnThread(void* pParam)
{
	CStringBasedOCRDL* pStringBasedOCR = (CStringBasedOCRDL*)pParam;

	g_resLearnResult = pStringBasedOCR->Learn();
	g_bTerminated = true;

	return unsigned int(g_resLearnResult);
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliLearnImage;
	CFLImage fliSourceImage;
	CFLImage fliResultImage;

	/// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageLearn;
	CGUIViewImageWrap viewImageSource;
	CGUIViewImageWrap viewImageResult;

	// 그래프 뷰 선언 // Declare the graph view
	CGUIViewGraphWrap viewGraph;

	CResult res = EResult_UnknownError;

	do
	{
		// 라이브러리가 완전히 로드 될 때까지 기다림 // Wait for the library to fully load
		CThreadUtilities::Sleep(1000);

		// 이미지 로드 // Loads image
		if(IsFail(res = fliLearnImage.Load(L"../../ExampleImages/StringBasedOCR/Learn.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliSourceImage.Load(L"../../ExampleImages/StringBasedOCR/Source.flif")))
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

		if(IsFail(res = viewImageSource.Create(600, 0, 1100, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageResult.Create(100, 500, 600, 1000)))
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

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
		if(IsFail(res = viewImageLearn.SetImagePtr(&fliLearnImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageSource.SetImagePtr(&fliSourceImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageResult.SetImagePtr(&fliResultImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewImageSource)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewImageResult)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerLearn = viewImageLearn.GetLayer(0);
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);
		CGUIViewImageLayerWrap layerResult = viewImageResult.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerLearn.Clear();
		layerSource.Clear();
		layerResult.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerLearn.DrawTextCanvas(&CFLPoint<double>(0, 0), L"LEARN", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerSource.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Validation", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerResult.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Result", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// StringBasedOCR 객체 생성 // Create StringBasedOCR object
		CStringBasedOCRDL stringBasedOCRDL;
		
		// 학습할 이미지 설정 // Set the image to learn
		stringBasedOCRDL.SetLearningImage(fliLearnImage);
		// 검증할 이미지 설정 // Set the image to validate
		stringBasedOCRDL.SetLearningValidationImage(fliSourceImage);

		// 학습할 StringBasedOCR 모델 설정 // Set up StringBasedOCR model to learn
		stringBasedOCRDL.SetModel(CStringBasedOCRDL::EModel_FLOcrNet_S);
		// 학습할 StringBasedOCR 모델 설정 // Set up StringBasedOCR model to learn
		stringBasedOCRDL.SetModelVersion(CStringBasedOCRDL::EModelVersion_FLOcrNet_S_V1_32_256_B2);
		// 학습 epoch 값을 설정 // Set the learn epoch value 
		stringBasedOCRDL.SetLearningEpoch(500);
		// 학습 이미지 Interpolation 방식 설정 // Set Interpolation method of learn image
		stringBasedOCRDL.SetInterpolationMethod(EInterpolationMethod_Bilinear);

		// OptimizerSpec 객체 생성 // Create OptimizerSpec object
		COptimizerSpecAdamGradientDescent optSpec;

		// Optimizer의 학습률 설정 // Set learning rate of Optimizer
		optSpec.SetLearningRate(1e-3f);

		// 설정한 Optimizer를 StringBasedOCR에 적용 // Apply the Optimizer that we set up to StringBasedOCR
		stringBasedOCRDL.SetLearningOptimizerSpec(optSpec);

		// 모델의 최적의 상태를 추적 후 마지막에 최적의 상태로 적용할 지 여부 설정 // Set whether to track the optimal state of the model and apply it as the optimal state at the end.
		stringBasedOCRDL.EnableOptimalLearningStatePreservation(true);

		// AugmentationSpec 설정 // Set the AugmentationSpec
		CAugmentationSpec augSpec;

		augSpec.EnableAugmentation(true);
		augSpec.SetCommonActivationRate(0.8);
		augSpec.SetCommonIoUThreshold(0.8);
		augSpec.SetCommonInterpolationMethod(FLImaging::ImageProcessing::EInterpolationMethod_Bilinear);
		augSpec.EnableRotation(true);
		augSpec.SetRotationParam(-10., 10., false, false);

		augSpec.EnableScale(true);
		augSpec.SetScaleParam(.8, 1.2, .8, 1.2, false);

		augSpec.EnableTranslation(true);
		augSpec.SetTranslationParam(0., .1, 0., .1);

		stringBasedOCRDL.SetLearningAugmentationSpec(&augSpec);

		// 학습을 종료할 조건식 설정. Metric 값이 1.0 이상인 경우 학습 종료한다. Metric = (1-NED + mAP) / 2
		// Set Conditional Expression to End Learning. If the metric value is 1.0 or higher, end the learning. Metric = (1-NED + mAP) / 2
		stringBasedOCRDL.SetLearningStopCondition(L"metric >= 1.0");

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		CAutoSaveSpec autoSaveSpec;

		// 자동 저장 활성화 // Enable Auto-Save
		// 저장 때문에 발생하는 속도 저하를 막기 위해 예제에서는 코드 사용법만 표시하고 옵션은 끔 // To prevent performance degradation caused by saving, the examples only demonstrate how to use the code, with the saving option disabled.
		autoSaveSpec.EnableAutoSave(false);
		// 저장할 모델 경로 설정 // Set Model path to save
		autoSaveSpec.SetAutoSavePath(L"model.flsbocrdl");
		// 자동 저장 조건식 설정. 현재 Metric 값이 최대 값인 경우 저장 활성화
		// Set auto-save conditional expressions. Enable save if the current metric value is the maximum value
		autoSaveSpec.SetAutoSaveCondition(L"epoch >= 10 & metric > max('metric')");

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		stringBasedOCRDL.SetLearningAutoSaveSpec(autoSaveSpec);

		// Learn 동작을 하는 핸들 객체 선언 // Declare HANDLE object execute learn function
		HANDLE hThread;

		// StringBasedOCR learn function을 진행하는 스레드 생성 // Create the StringBasedOCR Learn function thread
		hThread = (HANDLE)_beginthreadex(NULL, 0, LearnThread, (void*)&stringBasedOCRDL, 0, nullptr);

		while(!stringBasedOCRDL.IsRunning() && !g_bTerminated)
			CThreadUtilities::Sleep(1);

		int32_t i32MaxEpoch = stringBasedOCRDL.GetLearningEpoch();
		int32_t i32PrevEpoch = 0;
		int32_t i32PrevCostCount = 0;
		int32_t i32PrevValidationCount = 0;

		while(true)
		{
			CThreadUtilities::Sleep(1);

			// 마지막 미니 배치 최대 반복 횟수 받기 // Get the last maximum number of iterations of the last mini batch 
			int i32MiniBatchCount = stringBasedOCRDL.GetActualMiniBatchCount();
			// 마지막 미니 배치 최대 반복 횟수 받기 // Get the last maximum number of iterations of the last mini batch 
			int32_t i32MaxIteration = stringBasedOCRDL.GetActualMiniBatchCount();
			// 마지막 미니 배치 반복 횟수 받기 // Get the last number of mini batch iterations
			int32_t i32Iteration = stringBasedOCRDL.GetLearningResultCurrentIteration();
			// 마지막 학습 횟수 받기 // Get the last epoch learning
			int32_t i32Epoch = stringBasedOCRDL.GetLastEpoch();

			// 학습 결과 비용과 검증 결과 기록을 받아 그래프 뷰에 출력  
			// Get the history of cost and validation and print it at graph view
			CFLArray<float> vctCosts;
			CFLArray<float> vct1MNED;
			CFLArray<float> vctMeanAP;
			CFLArray<int32_t> vctValidationEpoch;

			stringBasedOCRDL.GetLearningResultAllHistory(vctCosts, vct1MNED, vctMeanAP, vctValidationEpoch);
			
			// 미니 배치 반복이 완료되면 cost와 validation 값을 디스플레이 
			// Display cost and validation value if iterations of the mini batch is completed 
			if(vctCosts.GetCount() && i32Epoch != i32PrevEpoch && i32Iteration == i32MaxIteration && i32Epoch > 0)
			{
				// 마지막 학습 결과 비용 받기 // Get the last cost of the learning result
				float f32CurrCost = vctCosts.Back();
				// 마지막 1-NED 결과 받기 // Get the last 1-NED result
				float f321MNED = vct1MNED.GetCount() ? vct1MNED.Back() : 0;
				// 마지막 MeanAP 결과 받기 // Get the last MeanAP result
				float f32MeanAP = vctMeanAP.GetCount() ? vctMeanAP.Back() : 0;
				// 해당 epoch의 비용과 검증 결과 값 출력 // Print cost and validation value for the relevant epoch
				printf("Cost : %.6f 1-NED : %.6f mAP : %.6f Epoch %d / %d\n", f32CurrCost, f321MNED, f32MeanAP, i32Epoch, i32MaxEpoch);

				// 비용 기록이나 검증 결과 기록이 있다면 출력 // Print results if cost or validation history exists
				if((vctCosts.GetCount() && i32PrevCostCount != (int32_t)vctCosts.GetCount()) || (vct1MNED.GetCount() && i32PrevValidationCount != (int32_t)vct1MNED.GetCount()) || (vctMeanAP.GetCount() && i32PrevValidationCount != (int32_t)vctMeanAP.GetCount()))
				{
					int32_t i32Step = stringBasedOCRDL.GetLearningValidationStep();
					CFLArray<float> flaX;

					for(int64_t i = 0; i < vct1MNED.GetCount() - 1; ++i)
						flaX.PushBack((float)(i * i32Step));

					flaX.PushBack((float)(vctCosts.GetCount() - 1));

					viewGraph.LockUpdate();

					// 이전 그래프의 데이터를 삭제 // Clear previous grpah data
					viewGraph.Clear();
					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(vctCosts, EChartType_Line, RED, L"Cost");
					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(flaX, vct1MNED, EChartType_Line, BLUE, L"1-NED");
					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(flaX, vctMeanAP, EChartType_Line, GREEN, L"mAP");

					viewGraph.UnlockUpdate();
					viewGraph.Invalidate();
				}

				// 검증 결과가 1.0일 경우 학습을 중단하고 인식 진행 
				// If the validation result is 1.0, stop learning and recognize
				if(f321MNED == 1.f && f32MeanAP == 1.f || GetAsyncKeyState(VK_ESCAPE))
					stringBasedOCRDL.Stop();

				i32PrevEpoch = i32Epoch;
				i32PrevCostCount = (int32_t)vctCosts.GetCount();
				i32PrevValidationCount = (int32_t)vct1MNED.GetCount();
			}

			// epoch만큼 학습이 완료되면 종료 // End when learning progresses as much as epoch
			if(!stringBasedOCRDL.IsRunning())
			{
				// learn 동작 스레드가 완전히 종료될 까지 대기 // Wait until learning is completely terminated
				WaitForSingleObject(hThread, INFINITE);
				// 스레드 핸들 종료 // Close thread handle
				CloseHandle(hThread);
				break;
			}
		}

		if(g_resLearnResult.IsFail())
		{
			ErrorPrint(g_resLearnResult, "Failed to Learn.\n");
			break;
		}

		// 인식할 이미지 설정 // Set the image to Recognize
		stringBasedOCRDL.SetInferenceImage(fliSourceImage);
		stringBasedOCRDL.SetInferenceResultImage(fliResultImage);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = stringBasedOCRDL.Execute()))
		{
			ErrorPrint(res, "Failed to execute Learn.\n");
			break;
		}

		// 결과 이미지를 이미지 뷰에 맞게 조정합니다. // Fit the result image to the image view.
		viewImageResult.ZoomFit();

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageLearn.RedrawWindow();
		viewImageSource.RedrawWindow();
		viewImageResult.RedrawWindow();
		// 그래프 뷰를 갱신 // Update the Graph view.
		viewGraph.RedrawWindow();

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageLearn.IsAvailable() && viewImageSource.IsAvailable() && viewImageResult.IsAvailable() && viewGraph.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}