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
	CFLImage fliSourceDestinationImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageDst;

	 // 수행 결과 객체 선언 // Declare the execution result object
	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res = fliSourceImage.Load(L"../../ExampleImages/OperationLogicalNor/circle.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Destination 이미지를 Source 이미지와 동일하게 설정
		if((res = fliSourceDestinationImage.Assign(fliSourceImage)).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if((res = viewImageSrc.Create(100, 0, 612, 512)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create Destination image view
		if((res = viewImageDst.Create(612, 0, 1124, 512)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((res = viewImageSrc.SynchronizePointOfView(&viewImageDst)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if((res = viewImageSrc.SetImagePtr(&fliSourceImage)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if((res = viewImageDst.SetImagePtr(&fliSourceDestinationImage)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&viewImageDst)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Operation Logical Nor 객체 생성 // Create Operation Logical Nor object
		COperationLogicalNor operationLogicalNor;

		// Source 이미지 설정 // Set the source image
		operationLogicalNor.SetSourceImage(fliSourceDestinationImage);

		// Source 이미지의 ROI 범위 설정 // Set the Source ROI value
		CFLCircle<int32_t> flcSourceROI(128, 128, 80, 0, 0, 360, EArcClosingMethod_EachOther);

		// Source 이미지의 ROI 지정 // Set the Source ROI
		operationLogicalNor.SetSourceROI(flcSourceROI);

		// Scalar Operation 소스로 설정 // Set Operation Source to scalar
		operationLogicalNor.SetOperationSource(EOperationSource_Scalar);

		// 스칼라 값 지정 // Set the Scalar value
		operationLogicalNor.SetScalarValue(CMultiVar<double>(111));

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = operationLogicalNor.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute operation logical Nor.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSource = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination = viewImageDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSource.Clear();
		layerDestination.Clear();

		// FLImaging의 Figure객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능
		// Source ROI 영역이 어디인지 알기 위해 디스플레이 한다
		if((res = layerSource.DrawFigureImage(&flcSourceROI, LIME)).IsFail())
			ErrorPrint(res, "Failed to draw figure\n");

		if((res = layerDestination.DrawFigureImage(&flcSourceROI, LIME)).IsFail())
			ErrorPrint(res, "Failed to draw figure\n");

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = layerSource.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if((res = layerDestination.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		viewImageSrc.Invalidate(true);
		viewImageDst.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() && viewImageDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}