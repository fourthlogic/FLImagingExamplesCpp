﻿#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


enum EType
{
	EType_Source = 0,
	EType_MedianWeightedFilter1,
	EType_MedianWeightedFilter2,
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

	do
	{
		// 알고리즘 동작 결과 // Algorithmic executation result
		CResult res = EResult_UnknownError;

		// Source 이미지 로드 // Load the source image
		if(IsFail(res = arrFliImage[EType_Source].Load(L"../../ExampleImages/NoiseImage/NoiseImage1.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Destination1 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination1 image as same as source image
		if(IsFail(res = arrFliImage[EType_MedianWeightedFilter1].Assign(arrFliImage[EType_Source])))
		{
			ErrorPrint(res, "Failed to assign the image file.\n");
			break;
		}

		// Destination2 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination2 image as same as source image
		if(IsFail(res = arrFliImage[EType_MedianWeightedFilter2].Assign(arrFliImage[EType_Source])))
		{
			ErrorPrint(res, "Failed to assign the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if(IsFail(res = arrViewImage[EType_Source].Create(100, 0, 612, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination1 이미지 뷰 생성 // Create destination1 image view
		if(IsFail(res = arrViewImage[EType_MedianWeightedFilter1].Create(612, 0, 1124, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination2 이미지 뷰 생성 // Create destination2 image view
		if(IsFail(res = arrViewImage[EType_MedianWeightedFilter2].Create(1124, 0, 1636, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		bool bError = false;

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			if(IsFail(res = arrViewImage[i].SetImagePtr(&arrFliImage[i])))
			{
				ErrorPrint(res, "Failed to set image object on the image view.\n");
				bError = true;
				break;
			}
		}

		if(bError)
			break;

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = arrViewImage[EType_Source].SynchronizePointOfView(&arrViewImage[EType_MedianWeightedFilter1])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = arrViewImage[EType_Source].SynchronizePointOfView(&arrViewImage[EType_MedianWeightedFilter2])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if(IsFail(res = arrViewImage[EType_Source].SynchronizeWindow(&arrViewImage[EType_MedianWeightedFilter1])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if(IsFail(res = arrViewImage[EType_Source].SynchronizeWindow(&arrViewImage[EType_MedianWeightedFilter2])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Utility MedianWeightedFilter 객체 생성 // Create Utility MedianWeightedFilter object
		CMedianWeightedFilter medianWeightedFilter;
		// Source ROI 설정 // Set the source ROI
		CFLRect<int32_t> flrROI(100, 190, 360, 420);
		// Source 이미지 설정 // Set the source image
		medianWeightedFilter.SetSourceImage(arrFliImage[EType_Source]);
		// Source ROI 설정 // Set the source ROI
		medianWeightedFilter.SetSourceROI(flrROI);
		// Destination 이미지 설정 // Set the destination image
		medianWeightedFilter.SetDestinationImage(arrFliImage[EType_MedianWeightedFilter1]);
		// Destination ROI 설정
		medianWeightedFilter.SetDestinationROI(flrROI);
		// Image MedianWeightedFilter 가중 방식을 Gauss로 설정
		medianWeightedFilter.SetWeightedMethod(CMedianWeightedFilter::EWeightedMethod_Gauss);

		// 처리할 MedianWeightedFilter Kernel 의 L(0, 0, 5, 5, 45.000000) 설정  // Set MedianWeightedFilter Kernel to L(0, 0, 5, 5, 45.000000)
		CFLRect<int32_t> flfKernal(0, 0, 5, 5, 45.000000);
		medianWeightedFilter.SetKernel(&flfKernal);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = medianWeightedFilter.Execute()))
		{
			ErrorPrint(res, "Failed to execute medianWeightedFilter.");
			
			break;
		}

		// Destination 이미지 설정 // Set the destination image
		medianWeightedFilter.SetDestinationImage(arrFliImage[EType_MedianWeightedFilter2]);
		// Image MedianWeightedFilter 가중 방식을 Inner로 설정
		medianWeightedFilter.SetWeightedMethod(CMedianWeightedFilter::EWeightedMethod_Inner);
				
		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = medianWeightedFilter.Execute()))
		{
			ErrorPrint(res, "Failed to execute medianWeightedFilter.");
			
			break;
		}

		CGUIViewImageLayerWrap arrLayer[ETypeCount];

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
			// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
			arrLayer[i] = arrViewImage[i].GetLayer(0);

			// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
			arrLayer[i].Clear();

			// ROI영역이 어디인지 알기 위해 디스플레이 한다 // Display to find out where ROI is
			// FLImaging의 Figure객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능
			if(IsFail(res = arrLayer[i].DrawFigureImage(&flrROI, LIME)))
				ErrorPrint(res, "Failed to draw figure\n");

		}

		// 이미지 뷰 정보 표시 // Display image view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = arrLayer[EType_Source].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_MedianWeightedFilter1].DrawTextCanvas(&CFLPoint<double>(0, 0), L"MedianWeightedFilter1 Gauss", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_MedianWeightedFilter2].DrawTextCanvas(&CFLPoint<double>(0, 0), L"MedianWeightedFilter2 Inner", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		arrViewImage[EType_Source].Invalidate(true);
		arrViewImage[EType_MedianWeightedFilter1].Invalidate(true);
		arrViewImage[EType_MedianWeightedFilter2].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(arrViewImage[EType_Source].IsAvailable()
			  && arrViewImage[EType_MedianWeightedFilter1].IsAvailable()
			  && arrViewImage[EType_MedianWeightedFilter2].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}