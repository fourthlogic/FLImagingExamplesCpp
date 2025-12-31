#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


enum EType
{
	EType_Source = 0,
	EType_Destination,
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

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = arrFliImage[EType_Source].Load(L"../../ExampleImages/RegionInterpolation/Sky_Damaged.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		bool bError = false;

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			// 이미지 뷰 생성 // Create image view
			if(IsFail(res = arrViewImage[i].Create(i * 400 + 400, 0, i * 400 + 400 + 400, 400)))
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				bError = true;
				break;
			}

			// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
			if(IsFail(res = arrViewImage[i].SetImagePtr(&arrFliImage[i])))
			{
				ErrorPrint(res, "Failed to set image object on the image view.\n");
				bError = true;
				break;
			}

			if(i == EType_Source)
				continue;

			// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views.
			if(IsFail(res = arrViewImage[EType_Source].SynchronizePointOfView(&arrViewImage[i])))
			{
				ErrorPrint(res, "Failed to synchronize view\n");
				bError = true;
				break;
			}

			// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
			if(IsFail(res = arrViewImage[EType_Source].SynchronizeWindow(&arrViewImage[i])))
			{
				ErrorPrint(res, "Failed to synchronize window.\n");
				bError = true;
				break;
			}
		}

		if(bError)
			break;

		// Harmonic Interpolation 객체 생성 // Create  Harmonic Interpolation object
		CHarmonicInterpolation harmonicInterpolation;
		// Source 이미지 설정 // Set source image
		harmonicInterpolation.SetSourceImage(arrFliImage[EType_Source]);
		// Destination 이미지 설정 // Set destination image 
		harmonicInterpolation.SetDestinationImage(arrFliImage[EType_Destination]);
		// 정밀도 설정 // Set precision value
		harmonicInterpolation.SetPrecision(0.001);

		// 알고리즘 수행 // Execute the algorithm
		if((res = harmonicInterpolation.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Harmonic Interpolation.");
			break;
		}

		// ROI Draw를 위한 CFLRect<int32_t> 객체 생성 // Create a CFLRect<int32_t> object for ROI Draw
		CFLRect<int32_t> flrROI(164, 234, 339, 390);

		CGUIViewImageLayerWrap arrLayer[ETypeCount];

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
			// 따로 해제할 필요 없음 // No need to release separately
			arrLayer[i] = arrViewImage[i].GetLayer(0);

			arrLayer[i].Clear();

			// ROI를 디스플레이 합니다. // Display ROI range
			if(IsFail(res = arrLayer[i].DrawFigureImage(&flrROI, LIME)))
				ErrorPrint(res, "Failed to draw figure\n");
		}

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = arrLayer[EType_Source].DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_Destination].DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image (Harmonic)", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		for(int32_t i = 0; i < ETypeCount; ++i)
			arrViewImage[i].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		bool bAvailable = true;
		while(bAvailable)
		{
			for(int32_t i = 0; i < ETypeCount; ++i)
			{
				bAvailable = arrViewImage[i].IsAvailable();
				if(!bAvailable)
					break;
			}

			CThreadUtilities::Sleep(1);
		}
	}
	while(false);

	return 0;
}