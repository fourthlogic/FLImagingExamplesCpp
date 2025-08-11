#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliSrcImage;
	CFLImage fliFFTImage;
	CFLImage fliIdealFilter;
	CFLImage fliIdealDst;
	CFLImage fliButterworthFilter;
	CFLImage fliButterworthDst;
	CFLImage fliGaussianFilter;
	CFLImage fliGaussianDst;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImage[8];

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = fliSrcImage.Load(L"../../ExampleImages/FilterGeneratorFD/Sea1Ch.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage[0].Create(300, 0, 300 + 384, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[1].Create(300 + 384, 0, 300 + 384 * 2, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[2].Create(300 + 384 * 2, 0, 300 + 384 * 3, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[3].Create(300 + 384 * 3, 0, 300 + 384 * 4, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[4].Create(300, 384, 300 + 384, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[5].Create(300 + 384, 384, 300 + 384 * 2, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[6].Create(300 + 384 * 2, 384, 300 + 384 * 3, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[7].Create(300 + 384 * 3, 384, 300 + 384 * 4, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImage[0].SetImagePtr(&fliSrcImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[1].SetImagePtr(&fliIdealFilter)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[2].SetImagePtr(&fliButterworthFilter)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[3].SetImagePtr(&fliGaussianFilter)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[4].SetImagePtr(&fliFFTImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[5].SetImagePtr(&fliIdealDst)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[6].SetImagePtr(&fliButterworthDst)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[7].SetImagePtr(&fliGaussianDst)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[1])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[2])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[3])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[4])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[5])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[6])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[7])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[1])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[2])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[3])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[4])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[5])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[6])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[7])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		// Fourier Transform 객체 생성 // Create Fourier Transform object
		CFourierTransform fourierTransform;

		// Source 이미지 설정 // Set source image 
		fourierTransform.SetSourceImage(fliSrcImage);

		// Destination 이미지 설정(FFT image) // Set destination image(FFT image) 
		fourierTransform.SetDestinationImage(fliFFTImage);

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
		filterGeneratorBandrejectFD.SetSourceImage(fliFFTImage);

		// Destination 이미지 설정 // Set destination image
		filterGeneratorBandrejectFD.SetDestinationImage(fliIdealFilter);

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
		filterGeneratorBandrejectFD.SetDestinationImage(fliButterworthFilter);

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
		filterGeneratorBandrejectFD.SetDestinationImage(fliGaussianFilter);

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
		operationMultiply.SetSourceImage(fliFFTImage);
		// Operand 이미지 설정 // Set the operand image
		operationMultiply.SetOperandImage(fliIdealFilter);
		// Destination 이미지 설정 // Set the destination image
		operationMultiply.SetDestinationImage(fliIdealDst);
		// 연산 방식 설정 // Set operation source
		operationMultiply.SetOperationSource(EOperationSource_Image);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = operationMultiply.Execute()))
		{
			ErrorPrint(res, "Failed to execute operation multiply.");
			break;
		}

		// Operand 이미지 설정 // Set the operand image
		operationMultiply.SetOperandImage(fliButterworthFilter);
		// Destination 이미지 설정 // Set the destination image
		operationMultiply.SetDestinationImage(fliButterworthDst);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = operationMultiply.Execute()))
		{
			ErrorPrint(res, "Failed to execute operation multiply.");
			break;
		}

		// Operand 이미지 설정 // Set the operand image
		operationMultiply.SetOperandImage(fliGaussianFilter);
		// Destination 이미지 설정 // Set the destination image
		operationMultiply.SetDestinationImage(fliGaussianDst);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = operationMultiply.Execute()))
		{
			ErrorPrint(res, "Failed to execute operation multiply.");
			break;
		}

		// Source 이미지 설정(FFT image) // Set source image (FFT image)
		fourierTransform.SetSourceImage(fliIdealDst);

		// Destination 이미지 설정(IFFT image) // Set destination image(IFFT image)
		fourierTransform.SetDestinationImage(fliIdealDst);

		// 알고리즘 수행(IFFT) // Execute the algorithm(IFFT)
		if((res = fourierTransform.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Fourier Transform.");
			break;
		}

		// Source 이미지 설정(FFT image) // Set source image (FFT image)
		fourierTransform.SetSourceImage(fliButterworthDst);

		// Destination 이미지 설정(IFFT image) // Set destination image(IFFT image)
		fourierTransform.SetDestinationImage(fliButterworthDst);

		// 알고리즘 수행(IFFT) // Execute the algorithm(IFFT)
		if((res = fourierTransform.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Fourier Transform.");
			break;
		}

		// Source 이미지 설정(FFT image) // Set source image (FFT image)
		fourierTransform.SetSourceImage(fliGaussianDst);

		// Destination 이미지 설정(IFFT image) // Set destination image(IFFT image)
		fourierTransform.SetDestinationImage(fliGaussianDst);

		// 알고리즘 수행(IFFT) // Execute the algorithm(IFFT)
		if((res = fourierTransform.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Fourier Transform.");
			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSource = viewImage[0].GetLayer(0);
		CGUIViewImageLayerWrap layerFFT = viewImage[4].GetLayer(0);
		CGUIViewImageLayerWrap layerIdealFilter = viewImage[1].GetLayer(0);
		CGUIViewImageLayerWrap layerButterworthFilter = viewImage[2].GetLayer(0);
		CGUIViewImageLayerWrap layerGaussianFilter = viewImage[3].GetLayer(0);
		CGUIViewImageLayerWrap layerIdealDst = viewImage[5].GetLayer(0);
		CGUIViewImageLayerWrap layerButterworthDst = viewImage[6].GetLayer(0);
		CGUIViewImageLayerWrap layerGaussianDst = viewImage[7].GetLayer(0);

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
		if(IsFail(res = layerSource.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerFFT.DrawTextCanvas(&CFLPoint<double>(0, 0), L"FFT Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerIdealFilter.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Ideal, Min = 0.1, Max = 0.6", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerButterworthFilter.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Butterworth, Distance = 256, Degree = 2", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerGaussianFilter.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Gaussian, Sigma1 = Sigma2 = 1, Phi = 0", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerIdealDst.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Ideal Filtering Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerButterworthDst.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Butterworth Filtering Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerGaussianDst.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Gaussian Filtering Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImage[0].Invalidate(true);
		viewImage[1].Invalidate(true);
		viewImage[2].Invalidate(true);
		viewImage[3].Invalidate(true);
		viewImage[4].Invalidate(true);
		viewImage[5].Invalidate(true);
		viewImage[6].Invalidate(true);
		viewImage[7].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage[0].IsAvailable() && viewImage[1].IsAvailable() && viewImage[2].IsAvailable() && viewImage[3].IsAvailable() && viewImage[4].IsAvailable() && viewImage[5].IsAvailable() && viewImage[6].IsAvailable() && viewImage[7].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}