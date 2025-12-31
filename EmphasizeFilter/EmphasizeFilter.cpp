#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

enum EType
{
	EType_Source = 0,
	EType_Destination1,
	EType_Destination2,
	ETypeCount,
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage arrFliImage[ETypeCount];

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap arrViewImage[ETypeCount];

	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res =arrFliImage[EType_Source].Load(L"../../ExampleImages/EmphasizeFilter/houses.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Destination1 이미지를 Source 이미지와 동일한 이미지로 생성 // Create the Destination1 image as the Source image.
		if((res =arrFliImage[EType_Destination1].Assign(arrFliImage[EType_Source])).IsFail())
		{
			ErrorPrint(res, L"Failed to assign the image file.\n");
			break;
		}

		// Destination2 이미지를 Source 이미지와 동일한 이미지로 생성 // Create the Destination2 image as the Source image.
		if((res =arrFliImage[EType_Destination2].Assign(arrFliImage[EType_Source])).IsFail())
		{
			ErrorPrint(res, L"Failed to assign the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create the source image view
		if((res =arrViewImage[EType_Source].Create(100, 0, 612, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination1 이미지 뷰 생성  // Create the destination1 image
		if((res =arrViewImage[EType_Destination1].Create(612, 0,1124, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination2 이미지 뷰 생성 // Create the destination2 image
		if((res =arrViewImage[EType_Destination2].Create(1124, 0,1636, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		bool bError = false;

		// 이미지 뷰에 이미지를 디스플레이  Displays the image in the image view
		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			if((res =arrViewImage[i].SetImagePtr(&arrFliImage[i])).IsFail())
			{
				ErrorPrint(res, L"Failed to set image object on the image view.\n");
				bError = true;
				break;
			}
		}

		if(bError)
			break;

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((res =arrViewImage[EType_Source].SynchronizePointOfView(&arrViewImage[EType_Destination1])).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views.
		if((res =arrViewImage[EType_Source].SynchronizePointOfView(&arrViewImage[EType_Destination2])).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res =arrViewImage[EType_Source].SynchronizeWindow(&arrViewImage[EType_Destination1])).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res =arrViewImage[EType_Source].SynchronizeWindow(&arrViewImage[EType_Destination2])).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// EmphasizeFilter 객체 생성 // Create EmphasizeFilter object
		CEmphasizeFilter emphasizeFilter;
		// Source 이미지 설정 // Set the source Image
		emphasizeFilter.SetSourceImage(arrFliImage[EType_Source]);
		// Destination 이미지 설정 // Set the destination Image
		emphasizeFilter.SetDestinationImage(arrFliImage[EType_Destination1]);
		// 파라미터 값 설정 // Set parameters
		emphasizeFilter.SetKernel(7);
 		emphasizeFilter.SetFactor(1.5);
		emphasizeFilter.SetPaddingMethod(EPaddingMethod_DecreasingKernel);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute the algorithm according to the previously set parameters
		if((res =emphasizeFilter.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute operation rolling.");
			break;
		}

		// Destination 이미지 설정 // Set the destination Image
		emphasizeFilter.SetDestinationImage(arrFliImage[EType_Destination2]);
		// 파라미터 값 설정 // Set parameters
		emphasizeFilter.SetKernel(3);
		emphasizeFilter.SetFactor(2.5);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute the algorithm according to the previously set parameters
		if((res =emphasizeFilter.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute operation rolling.");
			break;
		}

		CGUIViewImageLayerWrap arrLayer[ETypeCount];

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Get layer 0 from Image View to display on the screen
			// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // Since this object belongs to the ImageView, it does not need to be freed.
			arrLayer[i] = arrViewImage[i].GetLayer(0);

			// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
			arrLayer[i].Clear();
		}

		// 이미지 뷰 정보 표시 // Display image view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas draws a String based on screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color. // If the color parameter is set as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by treating it as a background color
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if((res =arrLayer[EType_Source].DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res =arrLayer[EType_Destination1].DrawTextCanvas(CFLPoint<double>(0, 0), L"EmphasizeFilter Mask: 7x7 Factor: 1.50", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res =arrLayer[EType_Destination2].DrawTextCanvas(CFLPoint<double>(0, 0), L"EmphasizeFilter Mask: 3x3 Factor: 2.50", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image views
		arrViewImage[EType_Source].Invalidate(true);
		arrViewImage[EType_Destination1].Invalidate(true);
		arrViewImage[EType_Destination2].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the imageview to exit.
		while(arrViewImage[EType_Source].IsAvailable()
			&& arrViewImage[EType_Destination1].IsAvailable()
			&& arrViewImage[EType_Destination2].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}