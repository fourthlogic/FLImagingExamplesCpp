#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage arrFliImage[2];

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap arrViewImage[2];

	// 알고리즘 동작 결과 // Algorithm execution result
	CResult result = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((result = arrFliImage[0].Load(L"../../ExampleImages/OperationCompare/nature.flif")).IsFail())
		{
			ErrorPrint(result, L"Failed to load the image file.\n");
			break;
		}

		// Destination1 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination1 image as same as source image
		if((result = arrFliImage[1].Assign(arrFliImage[0])).IsFail())
		{
			ErrorPrint(result, L"Failed to assign the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if((result = arrViewImage[0].Create(100, 0, 612, 512)).IsFail())
		{
			ErrorPrint(result, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create destination image view
		if((result = arrViewImage[1].Create(612, 0, 1124, 512)).IsFail())
		{
			ErrorPrint(result, L"Failed to create the image view.\n");
			break;
		}

		bool bError = false;

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		for(int32_t i = 0; i < 2; ++i)
		{
			if((result = arrViewImage[i].SetImagePtr(&arrFliImage[i])).IsFail())
			{
				ErrorPrint(result, L"Failed to set image object on the image view.\n");
				bError = true;
				break;
			}
		}

		if(bError)
			break;

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((result = arrViewImage[0].SynchronizePointOfView(&arrViewImage[1])).IsFail())
		{
			ErrorPrint(result, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if((result = arrViewImage[0].SynchronizeWindow(&arrViewImage[1])).IsFail())
		{
			ErrorPrint(result, L"Failed to synchronize window.\n");
			break;
		}

		// COperationCompare 객체 생성 // Create COperationCompare object
		COperationCompare operationCompare;
		// Source 이미지 설정 // Set source image
		operationCompare.SetSourceImage(arrFliImage[0]);
		// Destination 이미지 설정 // Set destination image
		operationCompare.SetDestinationImage(arrFliImage[1]);
		// Scalar Operation 모드로 설정 // Set operation mode to scalar
		operationCompare.SetOperationSource(EOperationSource_Scalar);
		// Compare Scalar 값 설정 // Set comparison value of Compare operation
		operationCompare.SetScalarValue(CMultiVar<double>(110,110,110));

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((result = operationCompare.Execute()).IsFail())
		{
			ErrorPrint(result, L"Failed to execute operation compare.");
			
			break;
		}

		CGUIViewImageLayerWrap arrLayer[2];

		for(int32_t i = 0; i < 2; ++i)
		{
			// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
			// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
			arrLayer[i] = arrViewImage[i].GetLayer(0);

			// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
			arrLayer[i].Clear();
		}

		// 이미지 뷰 정보 표시 // Display image view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if((result = arrLayer[0].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(result, L"Failed to draw text\n");
			break;
		}

		if((result = arrLayer[1].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Image(Compare 110,110,110)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(result, L"Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		arrViewImage[0].Invalidate(true);
		arrViewImage[1].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to exit
		while(arrViewImage[0].IsAvailable()
			  && arrViewImage[1].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}