#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


enum EType
{
	EType_Src = 0,
	EType_Opr,
	EType_Dst,
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

	bool bError = false;

	do
	{
		// 동작 결과 // operation result
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Loads image
		if(IsFail(res = arrFliImage[EType_Src].Load(L"../../ExampleImages/Affine/Generator.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = arrFliImage[EType_Opr].Load(L"../../ExampleImages/Affine/Sunset.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			int32_t i32X = i % ETypeCount;
			int32_t i32Y = i / ETypeCount;

			// 이미지 뷰 생성 // Create image view
			if(IsFail(res = arrViewImage[i].Create(i32X * 400 + 400, i32Y * 400, i32X * 400 + 400 + 400, i32Y * 400 + 400)))
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

			if(i != EType_Src && i != EType_Dst)
			{
				// 두개의 뷰의 시점을 동기화  // Synchronize the viewpoints of the two image views. 
				if(IsFail(res = arrViewImage[EType_Src].SynchronizePointOfView(&arrViewImage[i])))
				{
					ErrorPrint(res, "Failed to synchronize window.\n");
					bError = true;
					break;
				}

				// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
				if(IsFail(res = arrViewImage[EType_Src].SynchronizeWindow(&arrViewImage[i])))
				{
					ErrorPrint(res, "Failed to synchronize window.\n");
					bError = true;
					break;
				}
			}
		}

		if(bError)
			break;

		// ImageConcatenator 객체 생성 // Create ImageConcatenator object
		CImageConcatenator imageConcatenator;

		// Source 이미지 설정 // Set source image 
		imageConcatenator.SetSourceImage(arrFliImage[EType_Src]);

		// Operand 이미지 설정 // Set operand image 
		imageConcatenator.SetOperandImage(arrFliImage[EType_Opr]);

		// Destination 이미지 설정 // Set destination image 
		imageConcatenator.SetDestinationImage(arrFliImage[EType_Dst]);

		// Operand ROI 지정 // Create ROI range
		CFLRect<int32_t> flrROI(arrFliImage[EType_Opr]);

		flrROI.left = (int32_t)(flrROI.GetWidth() * 0.7);

		// Operand 이미지 설정 // Set operand image 
		imageConcatenator.SetOperandROI(flrROI);

		// 알고리즘 수행 // Execute the algorithm
		if((res = imageConcatenator.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute imageConcatenator.");
			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap arrLayer[ETypeCount];

		for(int32_t i = 0; i < ETypeCount; ++i)
			arrLayer[i] = arrViewImage[i].GetLayer(0);

		// View 정보를 디스플레이 합니다. // Display View information.
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다.// The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = arrLayer[EType_Src].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_Opr].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Operand Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_Dst].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// ImageConcatenator 영역 표기 // ImageConcatenator Area draw
		if(IsFail(res = arrLayer[EType_Opr].DrawFigureImage(&flrROI, LIME)))
			ErrorPrint(res, "Failed to draw figure\n");

		for(int32_t i = ETypeCount - 1; i >= 0; --i)
		{
			// 이미지 뷰를 갱신 합니다. // Update the image view.
			arrViewImage[i].ZoomFit();
			arrViewImage[i].Invalidate(true);
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		bool bRun = true;
		while(bRun)
		{
			for(int32_t i = 0; i < ETypeCount; ++i)
			{
				bRun &= arrViewImage[i].IsAvailable();
			}

			CThreadUtilities::Sleep(1);
		}
	}
	while(false);

	return 0;
}