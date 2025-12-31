#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSourceImage;
	CFLImage fliOperandImage1;
	CFLImage fliOperandImage2;
	CFLImage fliDestinationImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSource;
	CGUIViewImageWrap viewImageOperand1;
	CGUIViewImageWrap viewImageOperand2;
	CGUIViewImageWrap viewImageDestination;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res = fliSourceImage.Load(L"../../ExampleImages/OperationLinear/Space.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Operand1 이미지 로드 // Loads the operand image
		if((res = fliOperandImage1.Load(L"../../ExampleImages/OperationLinear/circle.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if((res = fliOperandImage2.Load(L"../../ExampleImages/OperationLinear/Sky.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Destination 이미지 로드 // Loads the destination image
		if((res = fliDestinationImage.Create(250, 250)).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if((res = viewImageSource.Create(100, 0, 400, 300)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Operand1 이미지 뷰 생성 // Creates operand image view
		if((res = viewImageOperand1.Create(400, 0, 700, 300)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Operand2 이미지 뷰 생성 // Creates operand image view
		if((res = viewImageOperand2.Create(700, 0, 1000, 300)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create destination image view
		if((res = viewImageDestination.Create(1000, 0, 1300, 300)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰와 Operand1 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the source view and the operand view
		if((res = viewImageSource.SynchronizePointOfView(&viewImageOperand1)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// Source 이미지 뷰와 Operand2 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the source view and the operand view
		if((res = viewImageSource.SynchronizePointOfView(&viewImageOperand2)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// Source 이미지 뷰와 Destination 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the source view and the destination view
		if((res = viewImageSource.SynchronizePointOfView(&viewImageDestination)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if((res = viewImageSource.SetImagePtr(&fliSourceImage)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Operand1 이미지 뷰에 이미지를 디스플레이 // Display the image in the operand image view
		if((res = viewImageOperand1.SetImagePtr(&fliOperandImage1)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Operand2 이미지 뷰에 이미지를 디스플레이 // Display the image in the operand image view
		if((res = viewImageOperand2.SetImagePtr(&fliOperandImage2)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if((res = viewImageDestination.SetImagePtr(&fliDestinationImage)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if((res = viewImageSource.SynchronizeWindow(&viewImageOperand1)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if((res = viewImageSource.SynchronizeWindow(&viewImageOperand2)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if((res = viewImageSource.SynchronizeWindow(&viewImageDestination)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Operation Linear 객체 생성 // Create Operation Linear object
		COperationLinear operationLinear;

		// Source 이미지 설정 // Set the source image
		operationLinear.SetSourceImage(fliSourceImage);

		// Operand 이미지 설정 // Set the operand image
		operationLinear.SetOperandImage(fliOperandImage1);
		operationLinear.SetOperandImage2(fliOperandImage2);


		operationLinear.SetOperationOrder(EOperationOrder_CoefficientConstant);
		operationLinear.SetOverflowMethod(EOverflowMethod_Wrapping);

		// Destination 이미지 설정 // Set the destination image
		operationLinear.SetDestinationImage(fliDestinationImage);

		// Image Operation 소스로 설정 // Set Operation Source to image
		operationLinear.SetOperationSource(EOperationSource_Image);

		// 공백 색상 칠하기 모드 해제 // Set the Fill blank color mode false
		// 결과 이미지가 이미 존재할 경우 연산되지 않은 영역을 공백 색상으로 칠하지 않고 원본 그대로 둔다. // If the destination image already exists, the uncomputed area is left intact without being painted in a blank color.
		operationLinear.EnableFillBlankColorMode(false);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = operationLinear.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute operation Linear.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);
		CGUIViewImageLayerWrap layerOperand1 = viewImageOperand1.GetLayer(0);
		CGUIViewImageLayerWrap layerOperand2 = viewImageOperand2.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination = viewImageDestination.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSource.Clear();
		layerOperand1.Clear();
		layerOperand2.Clear();
		layerDestination.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if((res = layerOperand1.DrawTextCanvas(CFLPoint<double>(0, 0), L"Operand1 Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if((res = layerOperand2.DrawTextCanvas(CFLPoint<double>(0, 0), L"Operand2 Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if((res = layerDestination.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		viewImageSource.Invalidate(true);
		viewImageOperand1.Invalidate(true);
		viewImageOperand2.Invalidate(true);
		viewImageDestination.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSource.IsAvailable() && viewImageOperand1.IsAvailable() && viewImageOperand2.IsAvailable() && viewImageDestination.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}