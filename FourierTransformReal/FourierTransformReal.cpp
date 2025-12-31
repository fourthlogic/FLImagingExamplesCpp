#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliImage;
	CFLImage fliRFTImage;
	CFLImage fliIRFTImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageOriginal;
	CGUIViewImageWrap viewImageRFT;
	CGUIViewImageWrap viewImageIRFT;

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/FourierTransform/TempleNoise.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImageOriginal.Create(300, 0, 300 + 512, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageRFT.Create(300 + 512, 0, 300 + 512 * 2, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageIRFT.Create(300 + 512 * 2, 0, 300 + 512 * 3, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImageOriginal.SetImagePtr(&fliImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageIRFT.SetImagePtr(&fliIRFTImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageOriginal.SynchronizeWindow(&viewImageRFT)))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		if(IsFail(res = viewImageOriginal.SynchronizeWindow(&viewImageIRFT)))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		// FourierTransformReal 객체 생성 // Create FourierTransformReal object
		CFourierTransformReal fourierTransformReal;
		
		// Source 이미지 설정 // Set source image 
		fourierTransformReal.SetSourceImage(fliImage);

		// Destination 이미지 설정(RFT image) // Set destination image(RFT image) 
		fourierTransformReal.SetDestinationImage(fliRFTImage);

		// 결과 이미지 포멧 설정 (RFT image, 32/64 bit Floating Point 설정 가능) // Set Result image format(RFT image, 32/64 bit Floating Point) 
		fourierTransformReal.SetResultType(EFloatingPointAccuracy_Bit32);

		// 알고리즘 수행(RFT) // Execute the algorithm(RFT)
		if((res = fourierTransformReal.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Fourier Transform Real.");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImageRFT.SetImagePtr(&fliRFTImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 이미지의 노이즈를 감소하기(Mask 사용) // Reduce the noise in the image (using Mask)
		// Mask 객체 생성 // Create Mask object
		CMask imageMask;

		// 변환 이미지를 설정(RFT) // Set source image(RFT image)
		imageMask.SetSourceImage(fliRFTImage);

		// CFLFigureArray 객체를 생성 // Create CFLFigureArray object
		CFLFigureArray flaROIs;

		// 미리 그려둔 Mask region Figure Array 불러오기 // Load Pre-drawn Mask Region Figure Array
		if((res = flaROIs.Load(L"../../ExampleImages/FourierTransform/RFTRegion.fig")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the figure file.");
			break;
		}

		// 지정한 ROI를 입력 // Set mask ROI
		imageMask.SetSourceROI(&flaROIs);

		// 마스크 값을 입력 // set mask value
		imageMask.SetMask(CMultiVar<double>(0.0));

		// 알고리즘 수행(mask) // Execute the algorithm(mask)		
		if((res = imageMask.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Image Mask.");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageRFT.Invalidate(true);

		// Source 이미지 설정(RFT image) // Set source image (RFT image)
		fourierTransformReal.SetSourceImage(fliRFTImage);

		// Destination 이미지 설정(IRFT image) // Set destination image(IRFT image)
		fourierTransformReal.SetDestinationImage(fliIRFTImage);

		// 알고리즘 수행(IRFT) // Execute the algorithm(IRFT)
		if((res = fourierTransformReal.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Fourier Transform.");
			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layer1 = viewImageOriginal.GetLayer(0);
		CGUIViewImageLayerWrap layer2 = viewImageRFT.GetLayer(0);
		CGUIViewImageLayerWrap layer3 = viewImageIRFT.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layer1.Clear();

		// Mask ROI 영역을 출력 // Display the mask ROI area
		if(IsFail(res = layer2.DrawFigureImage(&flaROIs, LIME)))
			ErrorPrint(res, "Failed to draw figure\n");

		// View 정보를 디스플레이 합니다. // Display View information.
		if(IsFail(res = layer1.DrawTextCanvas(CFLPoint<double>(0, 0), L"Spatial Domain", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layer2.DrawTextCanvas(CFLPoint<double>(0, 0), L"Frequency Domain", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layer3.DrawTextCanvas(CFLPoint<double>(0, 0), L"Inverse RFT Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageOriginal.ZoomFit();
		viewImageRFT.ZoomFit();
		viewImageIRFT.ZoomFit();
		viewImageOriginal.Invalidate(true);
		viewImageRFT.Invalidate(true);
		viewImageIRFT.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageOriginal.IsAvailable() && viewImageRFT.IsAvailable() && viewImageIRFT.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}