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
	CPix2PixDL* pPix2Pix = (CPix2PixDL*)pParam;

	CResult er = pPix2Pix->Learn();
	g_bTerminated = true;

	return unsigned int(er);
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliLearnImageInput;
	CFLImage fliLearnImageTarget;
	CFLImage fliValidationImageInput;
	CFLImage fliValidationImageTarget;
	CFLImage fliResultImage;

	/// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageLearnInput;
	CGUIViewImageWrap viewImageLearnTarget;
	CGUIViewImageWrap viewImageValidationInput;
	CGUIViewImageWrap viewImageValidationTarget;
	CGUIViewImageWrap viewImagesResult;

	// 그래프 뷰 선언 // Declare the graph view
	CGUIViewGraphWrap viewGraph;

	CResult res = EResult_UnknownError;

	do
	{
		// 라이브러리가 완전히 로드 될 때까지 기다림 // Wait for the library to fully load
		CThreadUtilities::Sleep(1000);

		// 이미지 로드 // Loads image
		if(IsFail(res = fliLearnImageInput.Load(L"../../ExampleImages/Pix2Pix/Gray_Learn.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 로드 // Loads image
		if(IsFail(res = fliLearnImageTarget.Load(L"../../ExampleImages/Pix2Pix/Color_Learn.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliValidationImageInput.Load(L"../../ExampleImages/Pix2Pix/Gray_Validation.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliValidationImageTarget.Load(L"../../ExampleImages/Pix2Pix/Color_Validation.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Creates image view
		if(IsFail(res = viewImageLearnInput.Create(100, 0, 600, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰 생성 // Creates image view
		if(IsFail(res = viewImageLearnTarget.Create(600, 0, 1100, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageValidationInput.Create(100, 500, 600, 1000)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageValidationTarget.Create(600, 500, 1100, 1000)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImagesResult.Create(1100, 0, 1600, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewGraph.Create(1100, 500, 1600, 1000)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		viewGraph.SetDarkMode();

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
		if(IsFail(res = viewImageLearnInput.SetImagePtr(&fliLearnImageInput)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageValidationInput.SetImagePtr(&fliValidationImageInput)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageLearnTarget.SetImagePtr(&fliLearnImageTarget)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageValidationTarget.SetImagePtr(&fliValidationImageTarget)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImagesResult.SetImagePtr(&fliResultImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 결과와 고화질 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the Inference Result image view and High Resolution Image view

		if(IsFail(res = viewImageLearnInput.SynchronizeWindow(&viewImageLearnTarget)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageLearnInput.SynchronizeWindow(&viewImageValidationInput)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageLearnInput.SynchronizeWindow(&viewImageValidationTarget)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageLearnInput.SynchronizeWindow(&viewGraph)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageLearnInput.SynchronizePageIndex(&viewImageLearnTarget)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageValidationInput.SynchronizePageIndex(&viewImageValidationTarget)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewImageValidationTarget.SynchronizePointOfView(&viewImagesResult)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerLearnInput = viewImageLearnInput.GetLayer(0);
		CGUIViewImageLayerWrap layerLearnTarget = viewImageLearnTarget.GetLayer(0);
		CGUIViewImageLayerWrap layerInferenceInput = viewImageValidationInput.GetLayer(0);
		CGUIViewImageLayerWrap layerInferenceTarget = viewImageValidationTarget.GetLayer(0);
		CGUIViewImageLayerWrap layerResult = viewImagesResult.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerLearnInput.Clear();
		layerLearnTarget.Clear();
		layerInferenceInput.Clear();
		layerInferenceTarget.Clear();
		layerResult.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerLearnInput.DrawTextCanvas(CFLPoint<double>(0, 0), L"LEARN INPUT", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerInferenceInput.DrawTextCanvas(CFLPoint<double>(0, 0), L"VALIDATION INPUT", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerLearnTarget.DrawTextCanvas(CFLPoint<double>(0, 0), L"LEARN TARGET", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerInferenceTarget.DrawTextCanvas(CFLPoint<double>(0, 0), L"VALIDATION TARGET", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerResult.DrawTextCanvas(CFLPoint<double>(0, 0), L"INFERENCE RESULTS", GREEN, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageLearnInput.RedrawWindow();
		viewImageLearnTarget.RedrawWindow();
		viewImageValidationInput.RedrawWindow();
		viewImageValidationTarget.RedrawWindow();
		viewImagesResult.RedrawWindow();

		// Pix2Pix 객체 생성 // Create Pix2Pix object
		CPix2PixDL pix2pixDL;

		// OptimizerSpec 객체 생성 // Create OptimizerSpec object
		COptimizerSpecAdamGradientDescent optSpec;

		// 학습할 이미지 설정 // Set the image to learn
		pix2pixDL.SetLearningImage(fliLearnImageInput);
		pix2pixDL.SetLearningTargetImage(fliLearnImageTarget);
		// 학습할 이미지 설정 // Set the image to learn
		pix2pixDL.SetLearningValidationImage(fliValidationImageInput);
		pix2pixDL.SetLearningValidationTargetImage(fliValidationImageTarget);
		// 학습할 Pix2Pix 모델 설정 // Set up Pix2Pix model to learn
		pix2pixDL.SetModel(CPix2PixDL::EModel_FLGenNet_Pix2Pix);
		// 학습할 Pix2Pix 모델의 버전 설정 // Set up Pix2Pix model version to learn
		pix2pixDL.SetModelVersion(CPix2PixDL::EModelVersion_FLGenNet_Pix2Pix_V1_128);
		// 학습 epoch 값을 설정 // Set the learn epoch value 
		pix2pixDL.SetLearningEpoch(500);
		// 학습 이미지 Interpolation 방식 설정 // Set Interpolation method of learn image
		pix2pixDL.SetInterpolationMethod(EInterpolationMethod_Bilinear);

		// Optimizer의 학습률 설정 // Set learning rate of Optimizer
		optSpec.SetLearningRate(1e-4f);

		// 설정한 Optimizer를 Pix2Pix에 적용 // Apply the Optimizer that we set up to Pix2Pix
		pix2pixDL.SetLearningOptimizerSpec(optSpec);

		// 학습을 종료할 조건식 설정. accuracy값이 0.95 이상인 경우 학습 종료한다.
		// Set Conditional Expression to End Learning. If the accuracy value is 0.95 or more, end learning.
		pix2pixDL.SetLearningStopCondition(L"accuracy >= 0.95");

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		CAutoSaveSpec autoSaveSpec;

		// 자동 저장 활성화 // Enable Auto-Save
		// 저장 때문에 발생하는 속도 저하를 막기 위해 예제에서는 코드 사용법만 표시하고 옵션은 끔 // To prevent performance degradation caused by saving, the examples only demonstrate how to use the code, with the saving option disabled.
		autoSaveSpec.EnableAutoSave(false);
		// 저장할 모델 경로 설정 // Set Model path to save
		autoSaveSpec.SetAutoSavePath(L"model.flpp");
		// 자동 저장 조건식 설정. 현재 metric값이 최대 값인 경우 저장 활성화
		// Set auto-save conditional expressions. Enable save if the metric value is maximum
		autoSaveSpec.SetAutoSaveCondition(L"metric > max('metric')");

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		pix2pixDL.SetLearningAutoSaveSpec(autoSaveSpec);

		// Learn 동작을 하는 핸들 객체 선언 // Declare HANDLE object execute learn function
		HANDLE hThread;

		// Pix2Pix learn function을 진행하는 스레드 생성 // Create the Pix2Pix Learn function thread
		hThread = (HANDLE)_beginthreadex(NULL, 0, LearnThread, (void*)&pix2pixDL, 0, nullptr);

		while(!pix2pixDL.IsRunning() && !g_bTerminated)
			CThreadUtilities::Sleep(1);

		int32_t i32MaxEpoch = pix2pixDL.GetLearningEpoch();
		int32_t i32PrevEpoch = 0;
		int32_t i32PrevCostCount = 0;
		int32_t i32PrevPSNRCount = 0;
		int32_t i32PrevSSIMCount = 0;
		int32_t i32PrevValidationCount = 0;

		while(true)
		{
			CThreadUtilities::Sleep(1);

			// 마지막 미니 배치 최대 반복 횟수 받기 // Get the last maximum number of iterations of the last mini batch 
			int32_t i32MaxIteration = pix2pixDL.GetActualMiniBatchCount();
			// 마지막 미니 배치 반복 횟수 받기 // Get the last number of mini batch iterations
			int32_t i32Iteration = pix2pixDL.GetLearningResultCurrentIteration();
			// 마지막 학습 횟수 받기 // Get the last epoch learning
			int32_t i32Epoch = pix2pixDL.GetLastEpoch();

			// 미니 배치 반복이 완료되면 cost와 validation 값을 디스플레이 
			// Display cost and validation value if iterations of the mini batch is completed 
			if(i32Epoch != i32PrevEpoch && i32Iteration == i32MaxIteration && i32Epoch > 0)
			{
				// 마지막 학습 결과 비용 받기 // Get the last cost of the learning result
				float f32CurrCost = pix2pixDL.GetLearningResultLastCost();
				// 마지막 PSNR 결과 받기 // Get the last PSNR result
				float f32PSNRPa = pix2pixDL.GetLearningResultLastPSNR();
				// 마지막 SSIM 결과 받기 // Get the last SSIM result
				float f32SSIMPa = pix2pixDL.GetLearningResultLastSSIM();
				// 마지막 검증 결과 받기 // Get the last validation result
				float f32ValidationPa = pix2pixDL.GetLearningResultLastAccuracy();

				// 해당 epoch의 비용과 검증 결과 값 출력 // Print cost and validation value for the relevant epoch
				printf("Cost : %.6f PSNR : %.6f SSIM : %.6f Accuracy : %.6f Epoch %d / %d\n", f32CurrCost, f32PSNRPa, f32SSIMPa, f32ValidationPa, i32Epoch, i32MaxEpoch);

				// 학습 결과 비용과 검증 결과 기록을 받아 그래프 뷰에 출력  
				// Get the history of cost and validation and print it at graph view
				CFLArray<float> flaCostHistory;
				CFLArray<float> flaPSNRHistory;
				CFLArray<float> flaSSIMHistory;
				CFLArray<float> flaValidationHistory;
				CFLArray<int32_t> vctValidationEpoch;

				pix2pixDL.GetLearningResultAllHistory(&flaCostHistory, &flaValidationHistory, &flaPSNRHistory, &flaSSIMHistory, &vctValidationEpoch);

				// 비용 기록이나 검증 결과 기록이 있다면 출력 // Print results if cost or validation history exists
				if((flaCostHistory.GetCount() && i32PrevCostCount != (int32_t)flaCostHistory.GetCount()) || (flaPSNRHistory.GetCount() && i32PrevPSNRCount != (int32_t)flaPSNRHistory.GetCount()) || (flaSSIMHistory.GetCount() && i32PrevSSIMCount != (int32_t)flaSSIMHistory.GetCount()) || (flaValidationHistory.GetCount() && i32PrevValidationCount != (int32_t)flaValidationHistory.GetCount()))
				{
					int32_t i32Step = pix2pixDL.GetLearningValidationStep();
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
					viewGraph.Plot(flaX, flaPSNRHistory, EChartType_Line, GREEN, L"PSNR");
					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(flaX, flaSSIMHistory, EChartType_Line, YELLOW, L"SSIM");
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
					pix2pixDL.Stop();

				i32PrevEpoch = i32Epoch;
				i32PrevCostCount = (int32_t)flaCostHistory.GetCount();
				i32PrevPSNRCount = (int32_t)flaPSNRHistory.GetCount();
				i32PrevSSIMCount = (int32_t)flaSSIMHistory.GetCount();
				i32PrevValidationCount = (int32_t)flaValidationHistory.GetCount();
			}
			// epoch만큼 학습이 완료되면 종료 // End when learning progresses as much as epoch
			if(!pix2pixDL.IsRunning() && g_bTerminated)
			{
				// learn 동작 스레드가 완전히 종료될 까지 대기 // Wait until learning is completely terminated
				WaitForSingleObject(hThread, INFINITE);
				// 스레드 핸들 종료 // Close thread handle
				CloseHandle(hThread);
				break;
			}
		}

		// 분류할 이미지 설정 // Set the image to classify
		pix2pixDL.SetInferenceImage(fliValidationImageInput);
		// 추론 결과 이미지 설정 // Set the inference result Image
		pix2pixDL.SetInferenceResultImage(fliResultImage);
		// 추론 결과 옵션 설정 // Set the inference result options;
		if(IsFail(res = pix2pixDL.Execute()))
		{
			ErrorPrint(res, "Failed to execute.\n");
			break;
		}

		// 결과 이미지를 이미지 뷰에 맞게 조정합니다. // Fit the result image to the image view.
		viewImagesResult.ZoomFit();

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageLearnInput.RedrawWindow();
		viewImageValidationInput.RedrawWindow();
		viewImagesResult.RedrawWindow();

		// 그래프 뷰를 갱신 // Update the Graph view.
		viewGraph.RedrawWindow();

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageLearnInput.IsAvailable() && viewImageValidationInput.IsAvailable() && viewImageLearnTarget.IsAvailable() && viewImageValidationTarget.IsAvailable() && viewImagesResult.IsAvailable() && viewGraph.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}