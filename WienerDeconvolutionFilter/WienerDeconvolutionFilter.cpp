﻿#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


#include <iostream>

enum EType
{
	EType_Source = 0,
	EType_Destination1,
	EType_Destination2,
	EType_Destination3,
	EType_Destination4,
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

	// 알고리즘 동작 결과 // Algorithm execution result
	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res = arrFliImage[EType_Source].Load(L"../../ExampleImages/WienerDeconvolutionFilter/bird.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		bool bError = false;

		for(int32_t i = EType_Destination1; i < ETypeCount; ++i)
		{
			// Destination 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination image as same as source image
			if((res = arrFliImage[i].Assign(arrFliImage[EType_Source])).IsFail())
			{
				ErrorPrint(res, L"Failed to assign the image file.\n");
				bError = true;
				break;
			}
		}

		if(bError)
			break;

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			int32_t x = i % 3;
			int32_t y = i / 3;

			// 이미지 뷰 생성 // Create image view
			if((res = arrViewImage[i].Create(x * 400 + 400, y * 400, x * 400 + 400 + 400, y * 400 + 400)).IsFail())
			{
				ErrorPrint(res, L"Failed to create the image view.\n");
				bError = true;
				break;
			}

			// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
			if((res = arrViewImage[i].SetImagePtr(&arrFliImage[i])).IsFail())
			{
				ErrorPrint(res, L"Failed to set image object on the image view.\n");
				bError = true;
				break;
			}
			
			if(i == EType_Source)
				continue;

			// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
			if((res = arrViewImage[EType_Source].SynchronizePointOfView(&arrViewImage[i])).IsFail())
			{
				ErrorPrint(res, L"Failed to synchronize view\n");
				bError = true;
				break;
			}

			// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
			if((res = arrViewImage[EType_Source].SynchronizeWindow(&arrViewImage[i])).IsFail())
			{
				ErrorPrint(res, L"Failed to synchronize window.\n");
				bError = true;
				break;
			}
		}

		if(bError)
			break;		

 		// WienerDeconvolutionFilter 객체 생성 // Create WienerDeconvolutionFilter filter object
 		CWienerDeconvolutionFilter wienerDeconvolutionFilter;
 		// Source 이미지 설정 // Set the source image
 		wienerDeconvolutionFilter.SetSourceImage(arrFliImage[EType_Source]); 

 		// Destination1 이미지 설정 // Set the destination1 image
 		wienerDeconvolutionFilter.SetDestinationImage(arrFliImage[EType_Destination1]);

		// Destination2 이미지 설정 // Set the destination2 image
		wienerDeconvolutionFilter.SetResultFrequency(arrFliImage[EType_Destination3]);

		// SNR 값 설정 // Set the SNR value
		wienerDeconvolutionFilter.SetSNR(0.00001);

		// Length 값 설정 // Set the Length value
		wienerDeconvolutionFilter.SetLength(135);

		// Angle 값 설정 // Set the Angle value
		wienerDeconvolutionFilter.SetAngle(45);

		// Motion Blur 값 설정 // Set the Motion Blur value
		wienerDeconvolutionFilter.SetOperationType(CWienerDeconvolutionFilter::EOperationType_Convolution);
 
 		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
 		if((res = wienerDeconvolutionFilter.Execute()).IsFail())
 		{
 			ErrorPrint(res, L"Failed to execute algorithm.");
 			break;
 		}

		// Source 이미지 설정 // Set the source image
		wienerDeconvolutionFilter.SetSourceImage(arrFliImage[EType_Destination1]); 

		// Destination2 이미지 설정 // Set the destination2 image
		wienerDeconvolutionFilter.SetDestinationImage(arrFliImage[EType_Destination2]);

		// Destination4 이미지 설정 // Set the destination2 image
		wienerDeconvolutionFilter.SetResultFrequency(arrFliImage[EType_Destination4]);

		// WienerDeconvolutionFilter Motion Blur 설정 / Set the Motion Blur value
		wienerDeconvolutionFilter.SetOperationType(CWienerDeconvolutionFilter::EOperationType_Deconvolution);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = wienerDeconvolutionFilter.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute algorithm.");
			break;
		}	

		CGUIViewImageLayerWrap arrLayer[ETypeCount];

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
			// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
			arrLayer[i] = arrViewImage[i].GetLayer(0);

			arrLayer[i].Clear();
		}

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if((res = arrLayer[EType_Source].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = arrLayer[EType_Destination1].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination1 Image (Motion Blur)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = arrLayer[EType_Destination2].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination2 Image (Deconvolution)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = arrLayer[EType_Destination3].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination3 Image (Blur Frequency)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = arrLayer[EType_Destination4].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination4 Image (Deconv Frequency)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
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