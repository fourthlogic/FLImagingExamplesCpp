#include <cstdio>
#include "../CommonHeader/ErrorPrint.h"

#include <FLImaging.h>


enum EImageType
{
	EImageType_Source = 0,
	EImageType_Destination,
	EImageType_Count,
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage arrFliImage[EImageType_Count];

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap arrViewImage[EImageType_Count];

	do
	{
		// 알고리즘 동작 결과 // Algorithmic executation result
		CResult res = EResult_UnknownError;

		// Source 이미지 로드 // Load the source image
		if(IsFail(res = arrFliImage[EImageType_Source].Load(L"../../ExampleImages/OperationSignedSquareDifference/Gradation.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if(IsFail(res = arrViewImage[EImageType_Source].Create(100, 0, 612, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create destination image view
		if(IsFail(res = arrViewImage[EImageType_Destination].Create(612, 0, 1124, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = arrViewImage[EImageType_Source].SynchronizePointOfView(&arrViewImage[EImageType_Destination])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if(IsFail(res = arrViewImage[EImageType_Source].SetImagePtr(&arrFliImage[EImageType_Source])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if(IsFail(res = arrViewImage[EImageType_Destination].SetImagePtr(&arrFliImage[EImageType_Destination])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = arrViewImage[EImageType_Source].SynchronizeWindow(&arrViewImage[EImageType_Destination])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Operation SignedSquareDifference 객체 생성 // Create Operation SignedSquareDifference object
		COperationSignedSquareDifference operationSignedSquareDifference;

		// Source 이미지 설정 // Set the source image
		operationSignedSquareDifference.SetSourceImage(arrFliImage[EImageType_Source]);

		// Destination 이미지 설정 // Set the destination image
		operationSignedSquareDifference.SetDestinationImage(arrFliImage[EImageType_Destination]);

		// Operation Overflow Method 설정 // Set operation overflow method
		operationSignedSquareDifference.SetOverflowMethod(EOverflowMethod_Wrapping);

		// Scalar Operation 모드로 설정 // Set operation mode to scalar
		operationSignedSquareDifference.SetOperationSource(EOperationSource_Scalar);

		// 스칼라 Operand 값 지정 // Set scalar operand value
		operationSignedSquareDifference.SetScalarValue(CMultiVar<double>(0));

		// 공백 영역을 지정한 색으로 채우지 않도록 설정 // Set the blank area to not be filled with the specified color
		operationSignedSquareDifference.EnableFillBlankColorMode(false);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = res = operationSignedSquareDifference.Execute()))
		{
			ErrorPrint(res, "Failed to execute Operation Signed Square Difference.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSource = arrViewImage[EImageType_Source].GetLayer(0);
		CGUIViewImageLayerWrap layerDestination = arrViewImage[EImageType_Destination].GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSource.Clear();
		layerDestination.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		if(IsFail(res = layerSource.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDestination.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		arrViewImage[EImageType_Source].Invalidate(true);
		arrViewImage[EImageType_Destination].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(arrViewImage[EImageType_Source].IsAvailable() && arrViewImage[EImageType_Destination].IsAvailable())
			CThreadUtilities::Sleep(1);

		for(int32_t i = 0; i < EImageType_Count; ++i)
			arrViewImage[i].Destroy();
	}
	while(false);

	return 0;
}