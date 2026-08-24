#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


enum EType
{
	EType_Src = 0,
	EType_FFT,
	EType_Filter,
	EType_Multiply,
	EType_Dst,
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
		if(IsFail(res = arrImage[EType_Src].Load(L"../../ExampleImages/FilterGeneratorFD/Sea1Ch.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage[EType_Src].Create(300, 0, 300 + 384, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_FFT].Create(300 + 384, 0, 300 + 384 * 2, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Filter].Create(300 + 384 * 2, 0, 300 + 384 * 3, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Multiply].Create(300, 0 + 384, 300 + 384, 384 + 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Dst].Create(300 + 384 * 1, 0 + 384, 300 + 384 * 2, 384 + 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImage[EType_Src].SetImagePtr(&arrImage[EType_Src])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_FFT].SetImagePtr(&arrImage[EType_FFT])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Filter].SetImagePtr(&arrImage[EType_Filter])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Multiply].SetImagePtr(&arrImage[EType_Multiply])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Dst].SetImagePtr(&arrImage[EType_Dst])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if(IsFail(res = viewImage[EType_Src].SynchronizePointOfView(&viewImage[EType_FFT])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Src].SynchronizePointOfView(&viewImage[EType_Filter])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Src].SynchronizePointOfView(&viewImage[EType_Multiply])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Src].SynchronizePointOfView(&viewImage[EType_Dst])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImage[EType_Src].SynchronizeWindow(&viewImage[EType_FFT])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Src].SynchronizeWindow(&viewImage[EType_Filter])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Src].SynchronizeWindow(&viewImage[EType_Multiply])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImage[EType_Src].SynchronizeWindow(&viewImage[EType_Dst])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		// Fourier Transform 객체 생성 // Create Fourier Transform object
		CFourierTransform fourierTransform;

		// Source 이미지 설정 // Set source image 
		fourierTransform.SetSourceImage(arrImage[EType_Src]);

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

		// FilterGeneratorButterworthFD 객체 생성 // Create FilterGeneratorButterworthFD object
		CFilterGeneratorButterworthFD filterGeneratorButterworthFD;

		// Source 이미지 설정 // Set source image 
		filterGeneratorButterworthFD.SetSourceImage(arrImage[EType_Src]);

		// Destination 이미지 설정 // Set destination image
		filterGeneratorButterworthFD.SetDestinationImage(arrImage[EType_Filter]);

		// 정밀도 설정 // Set Accuracy
		filterGeneratorButterworthFD.SetAccuracy(EFloatingPointAccuracy_Bit32);

		// 필터 타입 설정 // set Filter type
		filterGeneratorButterworthFD.SetType(CFilterGeneratorButterworthFD::EFilterBaseFDType_FFT_Shift);

		// Distance 설정 // set Distance
		filterGeneratorButterworthFD.SetDistance(50);

		// Degree 설정 // set Degree
		filterGeneratorButterworthFD.SetDegree(2);

		// 알고리즘 수행 // Execute the algorithm
		if((res = filterGeneratorButterworthFD.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute FilterGeneratorButterworthFD.");
			break;
		}

		// Operation Multiply 객체 생성 // Create Operation Multiply object
		COperationMultiply operationMultiply;
		// Source 이미지 설정 // Set the source image
		operationMultiply.SetSourceImage(arrImage[EType_FFT]);
		// Operand 이미지 설정 // Set the operand image
		operationMultiply.SetOperandImage(arrImage[EType_Filter]);
		// Destination 이미지 설정 // Set the destination image
		operationMultiply.SetDestinationImage(arrImage[EType_Multiply]);
		// 연산 방식 설정 // Set operation source
		operationMultiply.SetOperationSource(EOperationSource_Image);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = operationMultiply.Execute()))
		{
			ErrorPrint(res, "Failed to execute operation multiply.");
			break;
		}

		// Source 이미지 설정(FFT image) // Set source image (FFT image)
		fourierTransform.SetSourceImage(arrImage[EType_Multiply]);

		// Destination 이미지 설정(IFFT image) // Set destination image(IFFT image)
		fourierTransform.SetDestinationImage(arrImage[EType_Dst]);

		// 알고리즘 수행(IFFT) // Execute the algorithm(IFFT)
		if((res = fourierTransform.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Fourier Transform.");
			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSource = viewImage[EType_Src].GetLayer(0);
		CGUIViewImageLayerWrap layerFFT = viewImage[EType_FFT].GetLayer(0);
		CGUIViewImageLayerWrap layerFilter = viewImage[EType_Filter].GetLayer(0);
		CGUIViewImageLayerWrap layerMultiply = viewImage[EType_Multiply].GetLayer(0);
		CGUIViewImageLayerWrap layerDestination = viewImage[EType_Dst].GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerSource.Clear();
		layerFFT.Clear();
		layerFilter.Clear();
		layerMultiply.Clear();
		layerDestination.Clear();

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

		if(IsFail(res = layerFilter.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image(Filter)", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerMultiply.DrawTextCanvas(CFLPoint<double>(0, 0), L"Filtering FFT Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDestination.DrawTextCanvas(CFLPoint<double>(0, 0), L"Filtering Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImage[EType_Src].Invalidate(true);
		viewImage[EType_FFT].Invalidate(true);
		viewImage[EType_Filter].Invalidate(true);
		viewImage[EType_Multiply].Invalidate(true);
		viewImage[EType_Dst].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage[EType_Src].IsAvailable() && viewImage[EType_FFT].IsAvailable() && viewImage[EType_Filter].IsAvailable() && viewImage[EType_Multiply].IsAvailable() && viewImage[EType_Dst].IsAvailable())
			CThreadUtilities::Sleep(1);

		for(int32_t i = 0; i < ETypeCount; i++)
			viewImage[i].Destroy();
	}
	while(false);

	return 0;
}