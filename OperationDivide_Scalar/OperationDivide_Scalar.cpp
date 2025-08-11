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
	CFLImage fliDestinationImage1;
	CFLImage fliDestinationImage2;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSource;
	CGUIViewImageWrap viewImageDestination1;
	CGUIViewImageWrap viewImageDestination2;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// Source 이미지 로드 // Load the source image
		if(IsFail(res = fliSourceImage.Load(L"../../ExampleImages/OperationDivide/FlowerWall.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Destination 이미지를 Src 이미지와 동일한 이미지로 생성
		if(IsFail(res = fliDestinationImage1.Assign(fliSourceImage)))
		{
			ErrorPrint(res, "Failed to assign the image file.\n");
			break;
		}

		if(IsFail(res = fliDestinationImage2.Assign(fliSourceImage)))
		{
			ErrorPrint(res, "Failed to assign the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if(IsFail(res = viewImageSource.Create(100, 0, 612, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination1 이미지 뷰 생성 // Create destination1 image view
		if(IsFail(res = viewImageDestination1.Create(612, 0,1124, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination2 이미지 뷰 생성 // Create destination2 image view
		if(IsFail(res = viewImageDestination2.Create(1124, 0,1636, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImageSource.SynchronizePointOfView(&viewImageDestination1)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImageSource.SynchronizePointOfView(&viewImageDestination2)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if(IsFail(res = viewImageSource.SetImagePtr(&fliSourceImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Destination1 이미지 뷰에 이미지를 디스플레이
		if(IsFail(res = viewImageDestination1.SetImagePtr(&fliDestinationImage1)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Destination2 이미지 뷰에 이미지를 디스플레이
		if(IsFail(res = viewImageDestination2.SetImagePtr(&fliDestinationImage2)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSource.SynchronizeWindow(&viewImageDestination1)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSource.SynchronizeWindow(&viewImageDestination2)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Operation Divide 객체 생성 // Create Operation Divide object
		COperationDivide operationDivide;
		// Source 이미지 설정 // Set the source image
		operationDivide.SetSourceImage(fliSourceImage);
		// 연산 방식 설정 // Set operation source
		operationDivide.SetOperationSource(EOperationSource_Scalar);
		// Destination1 이미지 설정 // Set the destination1 image
		operationDivide.SetDestinationImage(fliDestinationImage1);
		// Scalar 값 설정 // Set Scalar value
		operationDivide.SetScalarValue(CMultiVar<double>(1.5, 1.5, 1.5));

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = operationDivide.Execute()))
		{
			ErrorPrint(res, "Failed to execute operation divide.");
			break;
		}

		// Destination1 이미지 설정 // Set the destination1 image
		operationDivide.SetDestinationImage(fliDestinationImage2);
		// Scalar 값 설정 // Set Scalar value
		operationDivide.SetScalarValue(CMultiVar<double>(3.0, 3.0, 3.0));

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = operationDivide.Execute()))
		{
			ErrorPrint(res, "Failed to execute operation divide.");
			break;
		}
		

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSource = viewImageSource.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination1 = viewImageDestination1.GetLayer(0);
		CGUIViewImageLayerWrap layerDestination2 = viewImageDestination2.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSource.Clear();
		layerDestination1.Clear();
		layerDestination2.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerSource.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDestination1.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination1 Image(Divide 1.5)", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDestination2.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination2 Image(Divide 3.0)", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		viewImageSource.Invalidate(true);
		viewImageDestination1.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSource.IsAvailable()
			&& viewImageDestination1.IsAvailable()
			&& viewImageDestination2.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}