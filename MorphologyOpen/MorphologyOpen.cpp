﻿#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


enum EType
{
	EType_Src = 0,
	EType_Dst,
	ETypeCount,
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage arrFliImage[ETypeCount];

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap arrViewImage[ETypeCount];

	bool bError = false;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// Source 이미지 로드 // Load the source image
		if(IsFail(res = arrFliImage[EType_Src].Load(L"../../ExampleImages/Morphology/Chip_Noise2.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			// Destination 이미지를 Src 이미지와 동일한 이미지로 생성
			if(i != EType_Src)
			{
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

			// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
			if(IsFail(res = arrViewImage[i].SetImagePtr(&arrFliImage[i])))
			{
				ErrorPrint(res, "Failed to set image object on the image view.\n");
				bError = true;
				break;
			}

			if(i != EType_Src)
			{
				// 두개의 뷰의 시점을 동기화
				if(IsFail(res = arrViewImage[EType_Src].SynchronizePointOfView(&arrViewImage[i])))
				{
					ErrorPrint(res, "Failed to synchronize window.\n");
					bError = true;
					break;
				}

				// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
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

		// Morphology Open 객체 생성 // Create Morphology Open object
		CMorphologyOpen morphologyOpen;
		// ROI 범위 설정
		CFLRect<int32_t> flrROI(100, 190, 360, 420);
		// 처리할 이미지 설정
		morphologyOpen.SetSourceImage(arrFliImage[EType_Src]);
		// 처리할 ROI 설정
		morphologyOpen.SetSourceROI(flrROI);

		// Destination 이미지 설정 // Set the destination image
		morphologyOpen.SetDestinationImage(arrFliImage[EType_Dst]);
		// Destination ROI 설정
		morphologyOpen.SetDestinationROI(flrROI);

		// 처리할 Morphology Kernel 의 Kernel Size 설정 (Kernel Size = 7 일 경우, Kernel Size : 7x7) // Setting Kernel Size 7(Kernel Radius : 7x7)
		morphologyOpen.SetKernelShape(EKernelShape_Circle);
		morphologyOpen.SetKernel(7);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = morphologyOpen.Execute()))
		{
			ErrorPrint(res, "Failed to execute morphology open.");
			break;
		}

		CGUIViewImageLayerWrap arrLayer[ETypeCount];

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
			// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
			arrLayer[i] = arrViewImage[i].GetLayer(0);

			arrLayer[i].Clear();

			// ROI영역이 어디인지 알기 위해 디스플레이 한다 // Display to find out where ROI is
			// FLImaging의 Figure 객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능 // FLimaging's Figure objects can be displayed as a function regardless of the shape
			// 아래 함수 DrawFigureImage는 Image좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
			// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일경우 불투명, 0일경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
			// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)
			if(IsFail(res = arrLayer[i].DrawFigureImage(&flrROI, LIME)))
				ErrorPrint(res, "Failed to draw figure\n");
		}

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = arrLayer[EType_Src].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_Dst].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			// 이미지 뷰를 갱신 합니다. // Update image view
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