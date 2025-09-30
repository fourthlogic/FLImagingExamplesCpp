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
	CFLImage fliDestination1Image;
	CFLImage fliDestination2Image;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewSourceImage;
	CGUIViewImageWrap viewDestination1Image;
	CGUIViewImageWrap viewDestination2Image;

	// 수행 결과 객체 선언 // Declare execution result object
	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load Source image
		if((res = fliSourceImage.Load(L"../../ExampleImages/OperationScaledDivide/Generator.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Destination1 이미지를 Source 이미지와 동일하도록 설정 // Assign Source image to Destination1 image
		if((res = fliDestination1Image.Assign(fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to assign the image.\n");
			break;
		}

		// Destination2 이미지를 Source 이미지와 동일하도록 설정 // Assign Source image to Destination2 image
		if((res = fliDestination2Image.Assign(fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to assign the image.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if((res = viewSourceImage.Create(100, 0, 612, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination1 이미지 뷰 생성 // Create Destination1 image view
		if((res = viewDestination1Image.Create(612, 0, 1124, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination2 이미지 뷰 생성 // Create Destination2 image view
		if((res = viewDestination2Image.Create(1124, 0, 1636, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display image in Source image view
		if((res = viewSourceImage.SetImagePtr(&fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination1 이미지 뷰에 이미지를 디스플레이 // Display image in Destination1 image view
		if((res = viewDestination1Image.SetImagePtr(&fliDestination1Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination2 이미지 뷰에 이미지를 디스플레이 // Display image in Destination2 image view
		if((res = viewDestination2Image.SetImagePtr(&fliDestination2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 // Synchronize viewpoints of two image views
		if((res = viewSourceImage.SynchronizePointOfView(&viewDestination1Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize point of view between image views.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 // Synchronize viewpoints of two image views
		if((res = viewSourceImage.SynchronizePointOfView(&viewDestination2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize point of view between image views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewSourceImage.SynchronizeWindow(&viewDestination1Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// 두 뷰 윈도우의 위치를 동기화 // Synchronize positions of two views
		if((res = viewSourceImage.SynchronizeWindow(&viewDestination2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window between views.\n");
			break;
		}

		// Operation Scaled Divide 객체 생성 // Create Operation Scaled Divide object
		COperationScaledDivide operationScaledDivide;

		// Source 이미지 설정 // Set Source image
		if((res = operationScaledDivide.SetSourceImage(fliSourceImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Source image.\n");
			break;
		}

		// Destination 이미지 설정 // Set Destination image
		if((res = operationScaledDivide.SetDestinationImage(fliDestination1Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination image.\n");
			break;
		}

		// 연산 방식 스칼라로 설정 // Set operation source to image
		if((res = operationScaledDivide.SetOperationSource(EOperationSource_Scalar)).IsFail())
		{
			ErrorPrint(res, L"Failed to set operation source.\n");
			break;
		}

		// 스칼라 값 지정 // Set scalar value
		if((res = operationScaledDivide.SetScalarValue(CMultiVar<double>(192, 192, 192))).IsFail())
		{
			ErrorPrint(res, L"Failed to set the scalar value.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = operationScaledDivide.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Operation Scaled Divide.\n");
			break;
		}

		// Destination 이미지 설정 // Set Destination image
		if((res = operationScaledDivide.SetDestinationImage(fliDestination2Image)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Destination image.\n");
			break;
		}

		// 스칼라 값 지정 // Set scalar value
		if((res = operationScaledDivide.SetScalarValue(CMultiVar<double>(512, 512, 512))).IsFail())
		{
			ErrorPrint(res, L"Failed to set the scalar value.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = operationScaledDivide.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Operation Scaled Divide.\n");
			break;
		}

		// 화면에 출력하기 위해 이미지 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
        // 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released
		CGUIViewImageLayerWrap layerSource = viewSourceImage.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination1 = viewDestination1Image.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination2 = viewDestination2Image.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear figures drawn on existing layer
		layerSource.Clear();
		layerDestination1.Clear();
		layerDestination2.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerSource.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 18)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerDestination1.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination1 Image(ScaledDivide 192)", YELLOW, BLACK, 18)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = layerDestination2.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination2 Image(ScaledDivide 512)", YELLOW, BLACK, 18)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		viewSourceImage.Invalidate(true);
		viewDestination1Image.Invalidate(true);
		viewDestination2Image.Invalidate(true);

		// 뷰가 닫히기 전까지 종료하지 않고 대기 // Wait until a view is closed before exiting
		while(viewSourceImage.IsAvailable() && viewDestination1Image.IsAvailable() && viewDestination2Image.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}