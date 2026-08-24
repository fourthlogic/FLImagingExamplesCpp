#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


enum EType
{
	EType_Source = 0,
	EType_FFT,
	EType_IdealFilter,
	EType_IdealDst,
	EType_ButterworthFilter,
	EType_ButterworthDst,
	EType_GaussianFilter,
	EType_GaussianDst,
	ETypeCount,
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage arrImage[ETypeCount];

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImage[ETypeCount];

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = arrImage[EType_Source].Load(L"../../ExampleImages/FilterGeneratorFD/Sea1Ch.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage[EType_Source].Create(300, 0, 300 + 384, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_IdealFilter].Create(300 + 384, 0, 300 + 384 * 2, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_ButterworthFilter].Create(300 + 384 * 2, 0, 300 + 384 * 3, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_GaussianFilter].Create(300 + 384 * 3, 0, 300 + 384 * 4, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_FFT].Create(300, 384, 300 + 384, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_IdealDst].Create(300 + 384, 384, 300 + 384 * 2, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_ButterworthDst].Create(300 + 384 * 2, 384, 300 + 384 * 3, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_GaussianDst].Create(300 + 384 * 3, 384, 300 + 384 * 4, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImage[EType_Source].SetImagePtr(&arrImage[EType_Source])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_IdealFilter].SetImagePtr(&arrImage[EType_IdealFilter])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_ButterworthFilter].SetImagePtr(&arrImage[EType_ButterworthFilter])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_GaussianFilter].SetImagePtr(&arrImage[EType_GaussianFilter])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_FFT].SetImagePtr(&arrImage[EType_FFT])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_IdealDst].SetImagePtr(&arrImage[EType_IdealDst])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_ButterworthDst].SetImagePtr(&arrImage[EType_ButterworthDst])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_GaussianDst].SetImagePtr(&arrImage[EType_GaussianDst])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if(IsFail(res = viewImage[EType_Source].SynchronizePointOfView(&viewImage[EType_IdealFilter])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Source].SynchronizePointOfView(&viewImage[EType_ButterworthFilter])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Source].SynchronizePointOfView(&viewImage[EType_GaussianFilter])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Source].SynchronizePointOfView(&viewImage[EType_FFT])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Source].SynchronizePointOfView(&viewImage[EType_IdealDst])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Source].SynchronizePointOfView(&viewImage[EType_ButterworthDst])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Source].SynchronizePointOfView(&viewImage[EType_GaussianDst])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImage[EType_Source].SynchronizeWindow(&viewImage[EType_IdealFilter])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Source].SynchronizeWindow(&viewImage[EType_ButterworthFilter])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Source].SynchronizeWindow(&viewImage[EType_GaussianFilter])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Source].SynchronizeWindow(&viewImage[EType_FFT])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Source].SynchronizeWindow(&viewImage[EType_IdealDst])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Source].SynchronizeWindow(&viewImage[EType_ButterworthDst])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Source].SynchronizeWindow(&viewImage[EType_GaussianDst])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		// Fourier Transform 객체 생성 // Create Fourier Transform object
		CFourierTransform fourierTransform;

		// Source 이미지 설정 // Set source image 
		fourierTransform.SetSourceImage(arrImage[EType_Source]);

		// Destination 이미지 설정(FFT image) // Set destination image(FFT image) 
		fourierTransform.SetDestinationImage(arrImage[EType_FFT]);

		// 결과 이미지 포멧 설정 (FFT image, 32/64 bit Floating Point 설정 가능) // Set Result image format(FFT image, 32/64 bit Floating Point) 
		fourierTransform.SetResultType(EFloatingPointAccuracy_Bit32);

		// 푸리에 변환 결과 이미지를 쉬프트해서 받도록 설정 // Set to receive a shifted image of the Fourier transform result
		fourierTransform.SetShiftSpectrum(EFourierTransformShiftSpectrum_Shift);

		// 알고리즘 수행(FFT) // Execute the algorithm(FFT)
		if((res = fourierTransform.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Fourier Transform.");
			break;
		}

		// FilterGeneratorBandrejectFD 객체 생성 // Create FilterGeneratorBandrejectFD object
		CFilterGeneratorBandrejectFD filterGeneratorBandrejectFD;

		// Source 이미지 설정 // Set source image 
		filterGeneratorBandrejectFD.SetSourceImage(arrImage[EType_FFT]);

		// Destination 이미지 설정 // Set destination image
		filterGeneratorBandrejectFD.SetDestinationImage(arrImage[EType_IdealFilter]);

		// 정밀도 설정 // Set Accuracy
		filterGeneratorBandrejectFD.SetAccuracy(EFloatingPointAccuracy_Bit32);

		// MinFrequency 설정 // set MinFrequency
		filterGeneratorBandrejectFD.SetMinFrequency(0.1);

		// MaxFrequency 설정 // set MaxFrequency
		filterGeneratorBandrejectFD.SetMaxFrequency(0.6);

		// Filter Shape 설정 // set Filter Shape
		filterGeneratorBandrejectFD.SetFilterShape(CFilterGeneratorBandrejectFD::EFilterShape_Ideal);

		// 알고리즘 수행 // Execute the algorithm
		if((res = filterGeneratorBandrejectFD.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute FilterGeneratorBandrejectFD.");
			break;
		}

		// Destination 이미지 설정 // Set destination image
		filterGeneratorBandrejectFD.SetDestinationImage(arrImage[EType_ButterworthFilter]);

		// Filter Shape 설정 // set Filter Shape
		filterGeneratorBandrejectFD.SetFilterShape(CFilterGeneratorBandrejectFD::EFilterShape_Butterworth);

		// Distance 설정 // set Distance
		filterGeneratorBandrejectFD.SetDistance(256);

		// Degree 설정 // set Degree
		filterGeneratorBandrejectFD.SetDegree(2);

		// 알고리즘 수행 // Execute the algorithm
		if((res = filterGeneratorBandrejectFD.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute FilterGeneratorBandrejectFD.");
			break;
		}

		// Destination 이미지 설정 // Set destination image
		filterGeneratorBandrejectFD.SetDestinationImage(arrImage[EType_GaussianFilter]);

		// Filter Shape 설정 // set Filter Shape
		filterGeneratorBandrejectFD.SetFilterShape(CFilterGeneratorBandrejectFD::EFilterShape_Gaussian);

		// Sigma1 설정 // set Sigma1
		filterGeneratorBandrejectFD.SetSigma1(1);

		// Sigma2 설정 // set Sigma2
		filterGeneratorBandrejectFD.SetSigma2(1);

		// Phi 설정 // set Phi
		filterGeneratorBandrejectFD.SetPhi(0);

		// 알고리즘 수행 // Execute the algorithm
		if((res = filterGeneratorBandrejectFD.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute FilterGeneratorBandrejectFD.");
			break;
		}

		// Operation Multiply 객체 생성 // Create Operation Multiply object
		COperationMultiply operationMultiply;
		// Source 이미지 설정 // Set the source image
		operationMultiply.SetSourceImage(arrImage[EType_FFT]);
		// Operand 이미지 설정 // Set the operand image
		operationMultiply.SetOperandImage(arrImage[EType_IdealFilter]);
		// Destination 이미지 설정 // Set the destination image
		operationMultiply.SetDestinationImage(arrImage[EType_IdealDst]);
		// 연산 방식 설정 // Set operation source
		operationMultiply.SetOperationSource(EOperationSource_Image);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = operationMultiply.Execute()))
		{
			ErrorPrint(res, "Failed to execute operation multiply.");
			break;
		}

		// Operand 이미지 설정 // Set the operand image
		operationMultiply.SetOperandImage(arrImage[EType_ButterworthFilter]);
		// Destination 이미지 설정 // Set the destination image
		operationMultiply.SetDestinationImage(arrImage[EType_ButterworthDst]);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = operationMultiply.Execute()))
		{
			ErrorPrint(res, "Failed to execute operation multiply.");
			break;
		}

		// Operand 이미지 설정 // Set the operand image
		operationMultiply.SetOperandImage(arrImage[EType_GaussianFilter]);
		// Destination 이미지 설정 // Set the destination image
		operationMultiply.SetDestinationImage(arrImage[EType_GaussianDst]);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = operationMultiply.Execute()))
		{
			ErrorPrint(res, "Failed to execute operation multiply.");
			break;
		}

		// Source 이미지 설정(FFT image) // Set source image (FFT image)
		fourierTransform.SetSourceImage(arrImage[EType_IdealDst]);

		// Destination 이미지 설정(IFFT image) // Set destination image(IFFT image)
		fourierTransform.SetDestinationImage(arrImage[EType_IdealDst]);

		// 알고리즘 수행(IFFT) // Execute the algorithm(IFFT)
		if((res = fourierTransform.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Fourier Transform.");
			break;
		}

		// Source 이미지 설정(FFT image) // Set source image (FFT image)
		fourierTransform.SetSourceImage(arrImage[EType_ButterworthDst]);

		// Destination 이미지 설정(IFFT image) // Set destination image(IFFT image)
		fourierTransform.SetDestinationImage(arrImage[EType_ButterworthDst]);

		// 알고리즘 수행(IFFT) // Execute the algorithm(IFFT)
		if((res = fourierTransform.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Fourier Transform.");
			break;
		}

		// Source 이미지 설정(FFT image) // Set source image (FFT image)
		fourierTransform.SetSourceImage(arrImage[EType_GaussianDst]);

		// Destination 이미지 설정(IFFT image) // Set destination image(IFFT image)
		fourierTransform.SetDestinationImage(arrImage[EType_GaussianDst]);

		// 알고리즘 수행(IFFT) // Execute the algorithm(IFFT)
		if((res = fourierTransform.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Fourier Transform.");
			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSource = viewImage[EType_Source].GetLayer(0);
		CGUIViewImageLayerWrap layerFFT = viewImage[EType_FFT].GetLayer(0);
		CGUIViewImageLayerWrap layerIdealFilter = viewImage[EType_IdealFilter].GetLayer(0);
		CGUIViewImageLayerWrap layerButterworthFilter = viewImage[EType_ButterworthFilter].GetLayer(0);
		CGUIViewImageLayerWrap layerGaussianFilter = viewImage[EType_GaussianFilter].GetLayer(0);
		CGUIViewImageLayerWrap layerIdealDst = viewImage[EType_IdealDst].GetLayer(0);
		CGUIViewImageLayerWrap layerButterworthDst = viewImage[EType_ButterworthDst].GetLayer(0);
		CGUIViewImageLayerWrap layerGaussianDst = viewImage[EType_GaussianDst].GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerSource.Clear();
		layerFFT.Clear();
		layerIdealFilter.Clear();
		layerButterworthFilter.Clear();
		layerGaussianFilter.Clear();
		layerIdealDst.Clear();
		layerButterworthDst.Clear();
		layerGaussianDst.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerFFT.DrawTextCanvas(CFLPoint<double>(0, 0), L"FFT Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerIdealFilter.DrawTextCanvas(CFLPoint<double>(0, 0), L"Ideal, Min = 0.1, Max = 0.6", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerButterworthFilter.DrawTextCanvas(CFLPoint<double>(0, 0), L"Butterworth, Distance = 256, Degree = 2", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerGaussianFilter.DrawTextCanvas(CFLPoint<double>(0, 0), L"Gaussian, Sigma1 = Sigma2 = 1, Phi = 0", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerIdealDst.DrawTextCanvas(CFLPoint<double>(0, 0), L"Ideal Filtering Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerButterworthDst.DrawTextCanvas(CFLPoint<double>(0, 0), L"Butterworth Filtering Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerGaussianDst.DrawTextCanvas(CFLPoint<double>(0, 0), L"Gaussian Filtering Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImage[EType_Source].Invalidate(true);
		viewImage[EType_IdealFilter].Invalidate(true);
		viewImage[EType_ButterworthFilter].Invalidate(true);
		viewImage[EType_GaussianFilter].Invalidate(true);
		viewImage[EType_FFT].Invalidate(true);
		viewImage[EType_IdealDst].Invalidate(true);
		viewImage[EType_ButterworthDst].Invalidate(true);
		viewImage[EType_GaussianDst].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage[EType_Source].IsAvailable() && viewImage[EType_IdealFilter].IsAvailable() && viewImage[EType_ButterworthFilter].IsAvailable() && viewImage[EType_GaussianFilter].IsAvailable() && viewImage[EType_FFT].IsAvailable() && viewImage[EType_IdealDst].IsAvailable() && viewImage[EType_ButterworthDst].IsAvailable() && viewImage[EType_GaussianDst].IsAvailable())
			CThreadUtilities::Sleep(1);
		
		for(int32_t i = 0; i < ETypeCount; i++)
			viewImage[i].Destroy();
	}
	while(false);

	return 0;
}