#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


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
	CResult res;

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = arrFliImage[EImageType_Source].Load(L"../../ExampleImages/Threshold/Sun.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = arrViewImage[EImageType_Source].Create(300, 0, 300 + 384, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = arrViewImage[EImageType_Destination].Create(300 + 384, 0, 300 + 384 * 2, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = arrViewImage[EImageType_Source].SetImagePtr(&arrFliImage[EImageType_Source])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = arrViewImage[EImageType_Destination].SetImagePtr(&arrFliImage[EImageType_Destination])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if(IsFail(res = arrViewImage[EImageType_Source].SynchronizePointOfView(&arrViewImage[EImageType_Destination])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = arrViewImage[EImageType_Source].SynchronizeWindow(&arrViewImage[EImageType_Destination])))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		// Adaptive Threshold Sauvola 객체 생성 // Create Adaptive Threshold Sauvola object
		CAdaptiveThresholdSauvola adaptiveThresholdSauvola;

		// Source 이미지 설정 // Set source image 
		adaptiveThresholdSauvola.SetSourceImage(arrFliImage[EImageType_Source]);

		// Destination 이미지 설정 // Set destination image
		adaptiveThresholdSauvola.SetDestinationImage(arrFliImage[EImageType_Destination]);

		// 커널 사이즈 입력 // Set kernel size
		adaptiveThresholdSauvola.SetKernel(27, 27);

		// 민감도 설정 // Set Sensitivity
		adaptiveThresholdSauvola.SetSensitivity(0.1);

		// 임계값 옵셋 설정 // Set threshold offset 
		adaptiveThresholdSauvola.SetThresholdOffset(5);

		// 알고리즘 수행 // Execute the algorithm
		if((res = adaptiveThresholdSauvola.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Adaptive Threshold Sauvola.");
			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSource = arrViewImage[EImageType_Source].GetLayer(0);
		CGUIViewImageLayerWrap layerDestination = arrViewImage[EImageType_Destination].GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerSource.Clear();
		layerDestination.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerDestination.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
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