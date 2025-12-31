#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


enum EType
{
	EType_Src = 0,
	EType_Dst1,
	EType_Dst2,
	EType_Dst3,
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

		// Source 이미지 로드 // Load the source image
		if(IsFail(res = arrFliImage[EType_Src].Load(L"../../ExampleImages/Affine/Sunset.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			if(i != EType_Src)
			{
				// 이미지를 Copy // image copy
				if(IsFail(res = arrFliImage[i].Assign(arrFliImage[EType_Src])))
				{
					ErrorPrint(res, "Failed to assign the image file.\n");
					bError = true;
					break;
				}
			}

			int32_t i32X = i % 2;
			int32_t i32Y = i / 2;

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

			if(i != EType_Src)
			{
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

		// rotation 객체 생성 // Create rotation object
		CRotation rotation1;

		// 각도 설정 // Set Angle
		rotation1.SetAngle(30.);
		// 소스 이미지 설정 // set source image
		rotation1.SetSourceImage(arrFliImage[EType_Dst1]);

		// 알고리즘 수행 // Execute the algorithm
		if((res = rotation1.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute rotation.");
			break;
		}

		// rotation 객체 생성 // Create rotation object
		CRotation rotation2;
		// 각도 설정 // Set Angle
		rotation2.SetAngle(30.);
		// 공백 영역을 지정한 색으로 채우도록 설정 // Set fill blank color mode
		rotation2.EnableFillBlankColorMode(true);
		// 공백 영역 색상 지정 // Set blank color value
		rotation2.SetBlankColor(CMultiVar<double>(0., 0., 0.));
		// 소스 이미지 설정 // Set source image
		rotation2.SetSourceImage(arrFliImage[EType_Dst2]);

		// 알고리즘 수행 // Execute the algorithm
		if((res = rotation2.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute rotation.");
			break;
		}

		// rotation 객체 생성 // Create rotation object
		CRotation rotation3;
		// 각도 설정 // Set Angle
		rotation3.SetAngle(30.);
		// Image Resize 설정 // Set Resize mode
		rotation3.SetResizeMethod(EResizeMethod_Resize);
		// 소스 이미지 설정 // Set source image
		rotation3.SetSourceImage(arrFliImage[EType_Dst3]);

		// 알고리즘 수행 // Execute the algorithm
		if((res = rotation3.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute rotation.");
			break;
		}

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap arrLayer[ETypeCount];
		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			arrLayer[i] = arrViewImage[i].GetLayer(0);
		}

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = arrLayer[EType_Src].DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_Dst1].DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image1", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_Dst2].DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image2", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_Dst3].DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image3", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		for(int32_t i = 0; i < ETypeCount; ++i)
		{
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