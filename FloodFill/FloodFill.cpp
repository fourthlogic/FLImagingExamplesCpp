#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


enum EImageType
{
	EImageType_Source = 0,
	EImageType_Destination1,
	EImageType_Destination2,
	EImageType_COUNT,
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage arrFliImage[EImageType_COUNT];

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap arrViewImage[EImageType_COUNT];

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = arrFliImage[EImageType_Source].Load(L"../../ExampleImages/FloodFill/Ball.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		for(int32_t i = 0; i < EImageType_COUNT; ++i)
		{
			if(IsFail(res = arrViewImage[i].Create(300 + 384 * i, 0, 300 + 384 * (i + 1), 384)))
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				break;
			}
		}

		if(IsFail(res))
			break;

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		for(int32_t i = 0; i < EImageType_COUNT; ++i)
		{
			if(IsFail(res = arrViewImage[i].SetImagePtr(&arrFliImage[i])))
			{
				ErrorPrint(res, "Failed to set image object on the image view.\n");
				break;
			}
		}

		if(IsFail(res))
			break;

		// 이미지 뷰들의 시점을 동기화 한다 // Synchronize the viewpoints of the image views.
		for(int32_t i = EImageType_Destination1; i < EImageType_COUNT; ++i)
		{
			if(IsFail(res = arrViewImage[EImageType_Source].SynchronizePointOfView(&arrViewImage[i])))
			{
				ErrorPrint(res, "Failed to synchronize view\n");
				break;
			}
		}

		if(IsFail(res))
			break;

		// 이미지 뷰 윈도우들의 위치를 동기화 한다 // Synchronize the positions of the image view windows
		for(int32_t i = EImageType_Destination1; i < EImageType_COUNT; ++i)
		{
			if(IsFail(res = arrViewImage[EImageType_Source].SynchronizeWindow(&arrViewImage[i])))
			{
				ErrorPrint(res, "Failed to synchronize window\n");
				break;
			}
		}

		if(IsFail(res))
			break;

		// Flood Fill 객체 생성 // Create Flood Fill object
		CFloodFill floodFill;

		// Source 이미지 설정 // Set source image
		floodFill.SetSourceImage(arrFliImage[EImageType_Source]);

		// 시작 지점 설정 // Set seed point
		CFLPoint<double> flpSeedPoint(9, 9);
		floodFill.SetSeedPoint(flpSeedPoint);

		// 픽셀 값 차이의 상한 설정 // Set upper bound of pixel value differences
		floodFill.SetUpperDifference(10);

		// 픽셀 값 차이의 하한 설정 // Set lower bound of pixel value differences
		floodFill.SetLowerDifference(10);

		// 연결 영역 출력값 설정 // Set output value of connected region
		floodFill.SetRangeValue(255);

		// 연결 영역이 아닌 영역의 출력값 설정 // Set output value of out of connected region
		floodFill.SetOutOfRangeValue(0);

		// Destination1 이미지 설정 // Set Destination1 image
		floodFill.SetDestinationImage(arrFliImage[EImageType_Destination1]);

		// Floating Range Mode 설정 // Set Floating Range Mode
		floodFill.SetRangeMode(CFloodFill::ERangeMode_FloatingRange);

		// 알고리즘 수행 // Execute the algorithm
		if((res = floodFill.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Flood Fill (Floating Range).");
			break;
		}

		// Destination2 이미지 설정 // Set Destination2 image
		floodFill.SetDestinationImage(arrFliImage[EImageType_Destination2]);

		// Fixed Range Mode 설정 // Set Fixed Range Mode
		floodFill.SetRangeMode(CFloodFill::ERangeMode_FixedRange);

		// 알고리즘 수행 // Execute the algorithm
		if((res = floodFill.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Flood Fill (Fixed Range).");
			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap arrLayer[EImageType_COUNT];

		for(int32_t i = 0; i < EImageType_COUNT; ++i)
		{
			arrLayer[i] = arrViewImage[i].GetLayer(0);

			// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
			arrLayer[i].Clear();
		}

		// 시작 지점을 디스플레이 합니다. // Display the seed point.
		if(IsFail(res = arrLayer[EImageType_Source].DrawFigureImage(flpSeedPoint, RED, 3)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		const wchar_t* arrViewText[EImageType_COUNT] =
		{
			L"Source Image",
			L"Floating Range",
			L"Fixed Range",
		};

		for(int32_t i = 0; i < EImageType_COUNT; ++i)
		{
			if(IsFail(res = arrLayer[i].DrawTextCanvas(CFLPoint<double>(0, 0), arrViewText[i], YELLOW, BLACK, 30)))
			{
				ErrorPrint(res, "Failed to draw text\n");
				break;
			}
		}

		if(IsFail(res))
			break;

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		for(int32_t i = 0; i < EImageType_COUNT; ++i)
			arrViewImage[i].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		bool bAvailable = true;

		while(bAvailable)
		{
			for(int32_t i = 0; i < EImageType_COUNT; ++i)
				bAvailable &= arrViewImage[i].IsAvailable();

			CThreadUtilities::Sleep(1);
		}

		for(int32_t i = 0; i < EImageType_COUNT; ++i)
			arrViewImage[i].Destroy();
	}
	while(false);

	return 0;
}
