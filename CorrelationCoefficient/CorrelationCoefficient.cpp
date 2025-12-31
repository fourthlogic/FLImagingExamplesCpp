#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSource;
	CFLImage fliOperand1;
	CFLImage fliOperand2;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewSrcImage;
	CGUIViewImageWrap viewOpr1Image;
	CGUIViewImageWrap viewOpr2Image;

    // 수행 결과 객체 선언 // Declare the execution result object
	CResult res = EResult_UnknownError;

	do
	{
		// 이미지 로드 // Load image
		if((res = fliSource.Load(L"../../ExampleImages/CorrelationCoefficient/Source.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.");
			break;
		}

		if((res = fliOperand1.Load(L"../../ExampleImages/CorrelationCoefficient/Operand1.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.");
			break;
		}

		if((res = fliOperand2.Load(L"../../ExampleImages/CorrelationCoefficient/Operand2.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if((res = viewSrcImage.Create(400, 0, 950, 550)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.");
			break;
		}

		if((res = viewOpr1Image.Create(950, 0, 1500, 550)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.");
			break;
		}

		if((res = viewOpr2Image.Create(1500, 0, 2050, 550)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.");
			break;
		}

        // 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		if((res = viewSrcImage.SetImagePtr(&fliSource)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.");
			break;
		}

		if((res = viewOpr1Image.SetImagePtr(&fliOperand1)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.");
			break;
		}

		if((res = viewOpr2Image.SetImagePtr(&fliOperand2)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views.
		if(IsFail(res = viewSrcImage.SynchronizePointOfView(&viewOpr1Image)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		if(IsFail(res = viewSrcImage.SynchronizePointOfView(&viewOpr2Image)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewSrcImage.SynchronizeWindow(&viewOpr1Image)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		if(IsFail(res = viewSrcImage.SynchronizeWindow(&viewOpr2Image)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Correlation Coefficient 객체 생성 // Create Correlation Coefficient object
		CCorrelationCoefficient correlationCoefficient;

		// Source 이미지 설정 // Set the Source Image
		correlationCoefficient.SetSourceImage(fliSource);

		// Operand 이미지 설정 // Set the Operand Image
		correlationCoefficient.SetOperandImage(fliOperand1);

		// 알고리즘 수행 // Execute the algorithm
		if((res = correlationCoefficient.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Correlation Coefficient.");
			break;
		}

		// 결과값을 받아올 CMultiVar<double> 컨테이너 생성 // Create the CMultiVar<double> object to push the result
		CMultiVar<double> mvResult1;

		// 결과 값을 가져오는 함수 // Function that get result value
		if((res = correlationCoefficient.GetResult(mvResult1)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// Operand 이미지 설정 // Set the Operand Image
		correlationCoefficient.SetOperandImage(fliOperand2);

		// 알고리즘 수행 // Execute the algorithm
		if((res = correlationCoefficient.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Correlation Coefficient.");
			break;
		}

		// 결과값을 받아올 CMultiVar<double> 컨테이너 생성 // Create the CMultiVar<double> object to push the result
		CMultiVar<double> mvResult2;

		// 결과 값을 가져오는 함수 // Function that get result value
		if((res = correlationCoefficient.GetResult(mvResult2)).IsFail())
		{
			ErrorPrint(res, "Failed to process.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewSrcImage.GetLayer(0);
		CGUIViewImageLayerWrap layerOpr1 = viewOpr1Image.GetLayer(0);
		CGUIViewImageLayerWrap layerOpr2 = viewOpr2Image.GetLayer(0);

		// 기존에 layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerOpr1.Clear();
		layerOpr2.Clear();

		CFLString<wchar_t> strResult1, strResult2;
		strResult1.Format(L"Operand1\nCorrelation Coefficient : %lf", mvResult1.GetAt(0));
		strResult2.Format(L"Operand2\nCorrelation Coefficient : %lf", mvResult2.GetAt(0));

		wprintf_s(L"%s\n", strResult1.GetString());
		wprintf_s(L"%s\n", strResult2.GetString());

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerSrc.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if((res = layerOpr1.DrawTextCanvas(CFLPoint<double>(0, 0), strResult1.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerOpr2.DrawTextCanvas(CFLPoint<double>(0, 0), strResult2.GetString(), YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewSrcImage.Invalidate(true);
		viewOpr1Image.Invalidate(true);
		viewOpr2Image.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewSrcImage.IsAvailable() && viewOpr1Image.IsAvailable() && viewOpr2Image.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}