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
	CDenoisingDiffusionDL* pDD = (CDenoisingDiffusionDL*)pParam;

	g_resLearnResult = pDD->Learn();
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
	CFLImage fliValidateImage;
	CFLImage fliResultImage0;
	CFLImage fliResultImage1;
	CFLImage fliResultImage2;

	/// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageLearn;
	CGUIViewImageWrap viewImageValidate;
	CGUIViewImageWrap viewImageResult0;
	CGUIViewImageWrap viewImageResult1;
	CGUIViewImageWrap viewImageResult2;

	// 그래프 뷰 선언 // Declare the graph view
	CGUIViewGraphWrap viewGraph;
	CResult res = EResult_UnknownError;

	do
	{
		// 라이브러리가 완전히 로드 될 때까지 기다림 // Wait for the library to fully load
		CThreadUtilities::Sleep(1000);

		// 이미지 로드 // Loads image
		if(IsFail(res = fliLearnImage.Load(L"../../ExampleImages/GenerativeAdversarialNetwork/mnist1000.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliValidateImage.Load(L"../../ExampleImages/GenerativeAdversarialNetwork/mnist100.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Creates image view
		if(IsFail(res = viewImageLearn.Create(100, 0, 612, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageValidate.Create(612, 0, 1124, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageResult0.Create(100, 512, 612, 1024)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageResult1.Create(612, 512, 1124, 1024)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageResult2.Create(1124, 512, 1636, 1024)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Graph 뷰 생성 // Create graph view
		if(IsFail(res = viewGraph.Create(1124, 0, 1636, 512)))
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

		if(IsFail(res = viewImageValidate.SetImagePtr(&fliValidateImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageResult0.SetImagePtr(&fliResultImage0)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageResult1.SetImagePtr(&fliResultImage1)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageResult2.SetImagePtr(&fliResultImage2)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerLearn = viewImageLearn.GetLayer(0);
		CGUIViewImageLayerWrap layerValidate = viewImageValidate.GetLayer(0);
		CGUIViewImageLayerWrap layerResult0 = viewImageResult0.GetLayer(0);
		CGUIViewImageLayerWrap layerResult1 = viewImageResult1.GetLayer(0);
		CGUIViewImageLayerWrap layerResult2 = viewImageResult2.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerLearn.Clear();
		layerValidate.Clear();
		layerResult0.Clear();
		layerResult1.Clear();
		layerResult2.Clear();

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

		if(IsFail(res = layerValidate.DrawTextCanvas(CFLPoint<double>(0, 0), L"VALIDATE", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerResult0.DrawTextCanvas(CFLPoint<double>(0, 0), L"0", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerResult1.DrawTextCanvas(CFLPoint<double>(0, 0), L"1", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerResult2.DrawTextCanvas(CFLPoint<double>(0, 0), L"2", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		viewImageLearn.Invalidate(true);
		viewImageValidate.Invalidate(true);
		viewImageResult0.Invalidate(true);
		viewImageResult1.Invalidate(true);
		viewImageResult2.Invalidate(true);

		// Denoising Diffuion 객체 생성 // Create Denoising Diffuion object
		CDenoisingDiffusionDL denoisingDiffuionDL;

		// OptimizerSpec 객체 생성 // Create OptimizerSpec object
		COptimizerSpecAdamGradientDescent optSpec;

		// 학습할 이미지 설정 // Set the image to learn
		denoisingDiffuionDL.SetLearningImage(fliLearnImage);
		// 검증할 이미지 설정 // Set the image to validate
		denoisingDiffuionDL.SetLearningValidationImage(fliValidateImage);

		// 학습할 Denoising Diffuion 모델 설정 // Set up Denoising Diffuion model to learn
		denoisingDiffuionDL.SetModel(CDenoisingDiffusionDL::EModel_FLGenNet_Diffusion_Label);
		// 학습할 Denoising Diffuion 모델 설정 // Set up Denoising Diffuion model to learn
		denoisingDiffuionDL.SetModelVersion(CDenoisingDiffusionDL::EModelVersion_FLGenNet_Diffusion_Label_V1_32);
		// 학습 epoch 값을 설정 // Set the learn epoch value 
		denoisingDiffuionDL.SetLearningEpoch(500);
		// 학습 이미지 Interpolation 방식 설정 // Set Interpolation method of learn image
		denoisingDiffuionDL.SetInterpolationMethod(EInterpolationMethod_Bilinear);

		// Optimizer의 학습률 설정 // Set learning rate of Optimizer
		optSpec.SetLearningRate(1e-4f);
		// Optimizer의 Weight Decay 설정 // Set weight decay of Optimizer
		optSpec.SetWeightDecay(0.f);

		// Gradient Clipping 옵션 적용 // Set the gradient clipping option
		denoisingDiffuionDL.EnableLearningGradientClipping(true);
		denoisingDiffuionDL.SetLearningGradientClippingThreshold(1.f);
		// 설정한 Optimizer를 GAN에 적용 // Apply the Optimizer that we set up to GAN
		denoisingDiffuionDL.SetLearningOptimizerSpec(optSpec);

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		CAutoSaveSpec autoSaveSpec;

		// 자동 저장 활성화 // Enable Auto-Save
		// 저장 때문에 발생하는 속도 저하를 막기 위해 예제에서는 코드 사용법만 표시하고 옵션은 끔 // To prevent performance degradation caused by saving, the examples only demonstrate how to use the code, with the saving option disabled.
		autoSaveSpec.EnableAutoSave(false);
		// 저장할 모델 경로 설정 // Set Model path to save
		autoSaveSpec.SetAutoSavePath(L"model.fldd");
		// 자동 저장 조건식 설정. 현재 metric값이 최대 값인 경우 저장 활성화
		// Set auto-save conditional expressions. Enable save if the current metric value is the maximum value
		autoSaveSpec.SetAutoSaveCondition(L"metric > max('metric')");

		// 자동 저장 옵션 설정 // Set Auto-Save Options
		denoisingDiffuionDL.SetLearningAutoSaveSpec(autoSaveSpec);

		// Learn 동작을 하는 핸들 객체 선언 // Declare HANDLE object execute learn function
		HANDLE hThread;

		// GAN learn function을 진행하는 스레드 생성 // Create the GAN Learn function thread
		hThread = (HANDLE)_beginthreadex(NULL, 0, LearnThread, (void*)&denoisingDiffuionDL, 0, nullptr);

		while(!denoisingDiffuionDL.IsRunning() && !g_bTerminated)
			CThreadUtilities::Sleep(1);

		int32_t i32MaxEpoch = denoisingDiffuionDL.GetLearningEpoch();
		int32_t i32PrevEpoch = 0;
		int32_t i32PrevCostCount = 0;
		int32_t i32PrevValidationCount = 0;

		while(true)
		{
			CThreadUtilities::Sleep(1);

			// 마지막 미니 배치 최대 반복 횟수 받기 // Get the last maximum number of iterations of the last mini batch 
			int32_t i32MaxIteration = denoisingDiffuionDL.GetActualMiniBatchCount();
			// 마지막 미니 배치 반복 횟수 받기 // Get the last number of mini batch iterations
			int32_t i32Iteration = denoisingDiffuionDL.GetLearningResultCurrentIteration();
			// 마지막 학습 횟수 받기 // Get the last epoch learning
			int32_t i32Epoch = denoisingDiffuionDL.GetLastEpoch();

			// 학습 결과 비용과 검증 결과 기록을 받아 그래프 뷰에 출력  
			// Get the history of cost and validation and print it at graph view
			CFLArray<float> vctCosts;
			CFLArray<float> vctSSIM;
			CFLArray<float> vctPDV;
			CFLArray<int32_t> vctValidationEpoch;

			denoisingDiffuionDL.GetLearningResultAllHistory(vctCosts, vctSSIM, vctPDV, vctValidationEpoch);

			// 미니 배치 반복이 완료되면 cost와 validation 값을 디스플레이 
			// Display cost and validation value if iterations of the mini batch is completed 
			if(vctCosts.GetCount() && i32Epoch != i32PrevEpoch && i32Iteration == i32MaxIteration && i32Epoch > 0)
			{
				// 마지막 학습 결과 비용 받기 // Get the last cost of the learning result
				float f32CurrCost = vctCosts.Back();
				// 마지막 SSIM 결과 받기 // Get the last SSIM Score result
				float f32SSIM = vctSSIM.GetCount() ? vctSSIM.Back() : 0;
				// 마지막 PDV 결과 받기 // Get the last PDV Score result
				float f32PDV = vctPDV.GetCount() ? vctPDV.Back() : 0;
				// 해당 epoch의 비용과 검증 결과 값 출력 // Print cost and validation value for the relevant epoch
				printf("Cost : %.6f SSIM : %.6f PDV : %.6f Epoch %d / %d\n", f32CurrCost, f32SSIM, f32PDV, i32Epoch, i32MaxEpoch);

				// 비용 기록이나 검증 결과 기록이 있다면 출력 // Print results if cost or validation history exists
				if((vctCosts.GetCount() && i32PrevCostCount != (int32_t)vctCosts.GetCount()) || (vctSSIM.GetCount() && i32PrevValidationCount != (int32_t)vctSSIM.GetCount()))
				{
					viewGraph.LockUpdate();

					// 이전 그래프의 데이터를 삭제 // Clear previous grpah data
					viewGraph.Clear();
					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(vctCosts, EChartType_Line, RED, L"Cost");

					int32_t i32Step = denoisingDiffuionDL.GetLearningValidationStep();
					CFLArray<float> flaX;

					for(int64_t i = 0; i < vctSSIM.GetCount() - 1; ++i)
						flaX.PushBack((float)(i * i32Step));

					flaX.PushBack((float)(vctCosts.GetCount() - 1));
					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(flaX, vctSSIM, EChartType_Line, GREEN, L"SSIM");

					flaX.Clear();

					for(int64_t i = 0; i < vctPDV.GetCount() - 1; ++i)
						flaX.PushBack((float)(i * i32Step));

					flaX.PushBack((float)(vctCosts.GetCount() - 1));
					// Graph View 데이터 입력 // Input Graph View Data
					viewGraph.Plot(flaX, vctPDV, EChartType_Line, BLUE, L"PDV");

					viewGraph.UnlockUpdate();
					viewGraph.Invalidate();
				}

				// 검증 결과가 1.0일 경우 또는 esc 키를 누른 경우 학습을 중단하고 분류 진행 
				// If the validation result is 1.0 or press ESC key, stop learning and classify images 
				if(GetAsyncKeyState(VK_ESCAPE))
					denoisingDiffuionDL.Stop();

				i32PrevEpoch = i32Epoch;
				i32PrevCostCount = (int32_t)vctCosts.GetCount();
			}

			// epoch만큼 학습이 완료되면 종료 // End when learning progresses as much as epoch
			if(!denoisingDiffuionDL.IsRunning())
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

		// 결과 이미지 개수 설정 // Set Result Image Count
		denoisingDiffuionDL.SetInferenceResultCount(10);

		// 생성할 이미지 설정 // Set the image to create
		denoisingDiffuionDL.SetInferenceResultImage(fliResultImage0);

		CFLArray<float> flaClassWeight;

		flaClassWeight.PushBack(1.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);

		// 클래스별 가중치 설정 // Set Class Weight
		denoisingDiffuionDL.SetInferenceClassWeight(flaClassWeight);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = denoisingDiffuionDL.Execute()))
		{
			ErrorPrint(res, "Failed to execute Learn.\n");
			break;
		}

		// 생성할 이미지 설정 // Set the image to create
		denoisingDiffuionDL.SetInferenceResultImage(fliResultImage1);

		flaClassWeight.Clear();
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(1.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);

		// 클래스별 가중치 설정 // Set Class Weight
		denoisingDiffuionDL.SetInferenceClassWeight(flaClassWeight);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = denoisingDiffuionDL.Execute()))
		{
			ErrorPrint(res, "Failed to execute Learn.\n");
			break;
		}

		// 생성할 이미지 설정 // Set the image to create
		denoisingDiffuionDL.SetInferenceResultImage(fliResultImage2);

		flaClassWeight.Clear();
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(1.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);
		flaClassWeight.PushBack(0.f);

		// 클래스별 가중치 설정 // Set Class Weight
		denoisingDiffuionDL.SetInferenceClassWeight(flaClassWeight);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = denoisingDiffuionDL.Execute()))
		{
			ErrorPrint(res, "Failed to execute Learn.\n");
			break;
		}

		// 결과 이미지를 이미지 뷰에 맞게 조정합니다. // Fit the result image to the image view.
		viewImageResult0.ZoomFit();
		viewImageResult1.ZoomFit();
		viewImageResult2.ZoomFit();

		// 이미지 뷰를 갱신 // Update the image view.
		viewImageLearn.Invalidate(true);
		viewImageValidate.Invalidate(true);
		viewImageResult0.Invalidate(true);
		viewImageResult1.Invalidate(true);
		viewImageResult2.Invalidate(true);
		// 그래프 뷰를 갱신 // Update the Graph view.
		viewGraph.Invalidate(true);

		wprintf(L"Inference Complete\n");

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageLearn.IsAvailable() && viewImageResult0.IsAvailable() && viewImageResult1.IsAvailable() && viewImageResult2.IsAvailable() && viewImageValidate.IsAvailable() && viewGraph.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}