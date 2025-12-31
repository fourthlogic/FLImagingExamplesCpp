#include <cstdio>
#include "../CommonHeader/ErrorPrint.h"

#include <FLImaging.h>


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSourceImage;
	CFLImage fliOperandImage;
	CFLImage fliDestinationImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSource;
	CGUIViewImageWrap viewImageOperand;
	CGUIViewImageWrap viewImageDestination;

	do
	{
        // 알고리즘 동작 결과 // Algorithmic executation result
		CResult res = EResult_UnknownError;

		// Source 이미지 로드 // Load the source image
		if(IsFail(res =fliSourceImage.Load(L"../../ExampleImages/OperationDifference/siamesecat3ch.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Operand 이미지 로드 // Loads the operand image
		if(IsFail(res =fliOperandImage.Load(L"../../ExampleImages/OperationDifference/space3ch.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Destination 이미지 로드 // Load the destination image
		if(IsFail(res =fliDestinationImage.Load(L"../../ExampleImages/OperationDifference/sea3ch.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if(IsFail(res =viewImageSource.Create(100, 0, 612, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Operand 이미지 뷰 생성 // Creates operand image view
		if(IsFail(res =viewImageOperand.Create(612, 0, 1124, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create destination image view
		if(IsFail(res =viewImageDestination.Create(1124, 0,1636, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰와 Operand 이미지 뷰의 시점을 동기화 한다
		if(IsFail(res =viewImageSource.SynchronizePointOfView(&viewImageOperand)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// Source 이미지 뷰와 Destination 이미지 뷰의 시점을 동기화 한다
		if(IsFail(res =viewImageSource.SynchronizePointOfView(&viewImageDestination)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if(IsFail(res =viewImageSource.SetImagePtr(&fliSourceImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Operand 이미지 뷰에 이미지를 디스플레이
		if(IsFail(res =viewImageOperand.SetImagePtr(&fliOperandImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if(IsFail(res =viewImageDestination.SetImagePtr(&fliDestinationImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res =viewImageSource.SynchronizeWindow(&viewImageOperand)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res =viewImageSource.SynchronizeWindow(&viewImageDestination)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Operation Difference 객체 생성 // Create Operation Difference object
		COperationDifference difference;

		// Source 이미지 설정 // Set the source image
		difference.SetSourceImage(fliSourceImage);

		// Source 이미지의 ROI 범위 설정
		CFLCircle<int32_t> flcSourceROI(258, 258, 174, 0, 0, 360, EArcClosingMethod_EachOther);

		// Source 이미지의 ROI 지정
		difference.SetSourceROI(flcSourceROI);

		// Operand 이미지 설정 // Set the operand image
		difference.SetOperandImage(fliOperandImage);

		// Destination 이미지 설정 // Set the destination image
		difference.SetDestinationImage(fliDestinationImage);

		// Destination 이미지 Pivot 좌표 설정
		CFLPoint<int32_t> flpDestinationPivot(174, 169);

		// Destination 이미지 Pivot 지정
		difference.SetDestinationPivot(flpDestinationPivot);

		// Image Operation 모드로 설정 // Set operation mode to image
		difference.SetOperationSource(EOperationSource_Image);

		// 공백 색상 칠하기 모드 해제
		// 결과 이미지가 이미 존재할 경우 연산되지 않은 영역을 공백 색상으로 칠하지 않고 원본 그대로 둔다.
		difference.EnableFillBlankColorMode(false);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res =res = difference.Execute()))
		{
			ErrorPrint(res, "Failed to execute operation difference.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);
		CGUIViewImageLayerWrap layerOperand = viewImageOperand.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination = viewImageDestination.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSource.Clear();
		layerOperand.Clear();
		layerDestination.Clear();

		// FLImaging의 Figure객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능
		// Source ROI 영역이 어디인지 알기 위해 디스플레이 한다
		if(IsFail(res =layerSource.DrawFigureImage(&flcSourceROI, LIME)))
			ErrorPrint(res, "Failed to draw figure\n");

		// Destination Pivot 영역이 어디인지 알기 위해 디스플레이 한다
		CFLFigureArray flfaDestinationPivotCrossHair = flpDestinationPivot.MakeCrossHair(20, true);

		if(IsFail(res =layerDestination.DrawFigureImage(&flfaDestinationPivotCrossHair, BLACK, 3)))
			ErrorPrint(res, "Failed to draw figure\n");

		if(IsFail(res =layerDestination.DrawFigureImage(&flfaDestinationPivotCrossHair, LIME)))
			ErrorPrint(res, "Failed to draw figure\n");

		// 이미지 뷰 정보 표시 // Display image view information
		if(IsFail(res =layerSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res =layerOperand.DrawTextCanvas(CFLPoint<double>(0, 0), L"Operand Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res =layerDestination.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		viewImageSource.Invalidate(true);
		viewImageOperand.Invalidate(true);
		viewImageDestination.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSource.IsAvailable() && viewImageOperand.IsAvailable() && viewImageDestination.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}