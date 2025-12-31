#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


#include <iostream>

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

	// 이미지 객체 선언 // Declare image object
	CFLImage arrFliImage[ETypeCount];

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap arrViewImage[ETypeCount];

	// 알고리즘 동작 결과 // Algorithm execution result
	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res = arrFliImage[EType_Source].Load(L"../../ExampleImages/Threshold/Mountain.flif")).IsFail())
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

		// ROI 설정을 위한 CFLRect 객체 생성 // Create a CFLRect object for setting ROI
		CFLRect<int32_t> flrROI(300, 200, 700, 600);

		// auto threshold 객체 생성 // Create auto threshold object
		CAutoThreshold autoThreshold;
		// Source 이미지 설정 // Set the source image
		autoThreshold.SetSourceImage(arrFliImage[EType_Source]);
		// Source ROI 설정 // Set the source ROI
		autoThreshold.SetSourceROI(flrROI);

		// Destination1 이미지 설정 // Set the destination1 image
		autoThreshold.SetDestinationImage(arrFliImage[EType_Destination1]);
		// Destination1 ROI 설정 // Set the destination1 ROI
		autoThreshold.SetDestinationROI(flrROI);

		// Sigma 값 설정 // Set the sigma value
		autoThreshold.SetSigma(2);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = autoThreshold.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute algorithm.");
			break;
		}

		// Result 결과 갯수 확인 // get result count
		int64_t i64IndexCount = autoThreshold.GetResultBatchCount();

		// Threshold 값 표기를 위한 String 변수 // string variable to indicate Threshold value
		CFLArray<CFLString<wchar_t>> flaThresholdStr;

		// AutoThreshold 결과값 // auto threshold result object
		CFLArray<int32_t> flaResult;

		for(int64_t i = 0; i < i64IndexCount; ++i)
		{
			// 이전 데이터 삭제 // data clear
			flaThresholdStr.Clear();

			// Channel 값 가져오기 // get channel result
			int64_t i64ChannelCount = autoThreshold.GetChannelCount(i);

			for(int j = 0; j < i64ChannelCount; ++j)
			{
				// 이전 데이터 삭제 // data clear
				flaResult.Clear();

				// AutoThreshold 결과 값 가져오기 // get auto threshold result
				if((res = (autoThreshold.GetResultBatch(i, j, flaResult))).IsFail())
				{
					ErrorPrint(res, L"No Result.");
					break;
				}

				int64_t i64ListCount = flaResult.GetCount();

				// Threshold 값 표기를 위한 String 임시변수 // string variable to indicate threshold temporary value
				CFLString<wchar_t> strTemp;
				strTemp.Format(L"Ch%d ", j);

				for(int k = 0; k < i64ListCount; ++k)
					strTemp.AppendFormat(L"%d ", flaResult[k]);

				flaThresholdStr.PushBack(strTemp);
			}
		}

		// Destination2 이미지 설정 // Set the destination2 image
		autoThreshold.SetDestinationImage(arrFliImage[EType_Destination2]);
		// Destination2 ROI 설정 // Set the destination2 ROI
		autoThreshold.SetDestinationROI(flrROI);

		// auto threshold 커널 크기 설정
		autoThreshold.SetSigma(4);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = autoThreshold.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute algorithm.");
			break;
		}
		
		// Result 결과 갯수 확인 // get result count
		i64IndexCount = autoThreshold.GetResultBatchCount();

		// Threshold 값 표기를 위한 String 변수 // string variable to indicate Threshold value
		CFLArray<CFLString<wchar_t>> flaThresholdStr2;

		for(int64_t i = 0; i < i64IndexCount; ++i)
		{
			// 이전 데이터 삭제 // data clear
			flaThresholdStr2.Clear();

			// Channel 값 가져오기 // get channel result
			int64_t i64ChannelCount = autoThreshold.GetChannelCount(i);

			for(int j = 0; j < i64ChannelCount; ++j)
			{
				// 이전 데이터 삭제 // data clear
				flaResult.Clear();

				// AutoThreshold 결과 값 가져오기 // get auto threshold result
				if((res = (autoThreshold.GetResultBatch(i, j, flaResult))).IsFail())
				{
					ErrorPrint(res, L"No Result.");
					break;
				}

				int64_t i64ListCount = flaResult.GetCount();

				// Threshold 값 표기를 위한 String 임시변수 // string variable to indicate threshold temporary value
				CFLString<wchar_t> strTemp;
				strTemp.Format(L"Ch%d ", j);

				for(int k = 0; k < i64ListCount; ++k)
					strTemp.AppendFormat(L"%d ", flaResult[k]);

				flaThresholdStr2.PushBack(strTemp);
			}
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
			if((res = arrLayer[i].DrawFigureImage(&flrROI, LIME)).IsFail())
				ErrorPrint(res, L"Failed to draw figure\n");
		}

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic

		CFLPoint<double> flpThreshold(20, 320);

		if((res = arrLayer[EType_Source].DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = arrLayer[EType_Destination1].DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination1 Image (Sigma 2)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = arrLayer[EType_Destination1].DrawTextCanvas(flpThreshold, flaThresholdStr[0], YELLOW, BLACK, 16)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text.\n");
			break;
		}

		if((res = arrLayer[EType_Destination2].DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination2 Image (Sigma 4)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = arrLayer[EType_Destination2].DrawTextCanvas(flpThreshold, flaThresholdStr2[0], YELLOW, BLACK, 16)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text.\n");
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