﻿#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage;

	do
	{
		CResult res = EResult_UnknownError;
		// 이미지 로드 // Load image
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/Blob/Perforated.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage.Create(400, 0, 1424, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		if(IsFail(res = viewImage.SetImagePtr(&fliImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Blob 객체 생성 // Create Blob object
		CBlobSubsampled blob;

		// ROI 범위 설정
		CFLRect<int32_t> flrROI(300, 250, 750, 650);

		// 처리할 이미지 설정
		blob.SetSourceImage(fliImage);
		// 처리할 ROI 설정
		blob.SetSourceROI(flrROI);
		// Threshold 모드 설정. 여기서는 2중 Threshold에 두개의 조건의 And 조건을 참으로 설정한다.
		blob.SetThresholdMode(EThresholdMode_Dual_And);
		// 논리 조건 설정
		blob.SetLogicalCondition(ELogicalCondition_Greater, ELogicalCondition_Less);
		// 임계값 설정,  위의 조건과 아래의 조건이 합쳐지면 127보다 크고 240보다 작은 객체를 검출
		blob.SetThreshold(127, 240);

		// 2중 임계 값 설정을 아래와 같이 할 수도 있음.
//		blob.SetThreshold(127, EThresholdIndex_First);
// 		blob.SetThreshold(240, EThresholdIndex_Second);

		// Subsampling Level 설정
		blob.SetSubsamplingLevel(3);
		// 가운데 구멍난 Contour를 지원하기 위해 Perforated 모드 설정
		blob.SetContourResultType(CBlob::EContourResultType_Perforated);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = blob.Execute()))
		{
			ErrorPrint(res, "Failed to execute Blob.");
			break;
		}

		// Blob 결과를 얻어오기 위해 FigureArray 선언
		CFLFigureArray flfaContours;

		// Blob 결과들 중 Contour를 얻어옴
		if(IsFail(res = blob.GetResultContours(flfaContours)))
		{
			ErrorPrint(res, "Failed to get boundary rects from the Blob object.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer = viewImage.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer.Clear();


		// ROI영역이 어디인지 알기 위해 디스플레이 한다 // Display to find out where ROI is
		// FLImaging의 Figure객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능
		if(IsFail(res = layer.DrawFigureImage(&flrROI, BLUE)))
		{
			ErrorPrint(res, "Failed to draw figures objects on the image view.\n");
			break;
		}

		// flfaContours 는 Figure들의 배열이기 때문에 Layer에 넣기만 해도 모두 드로윙이 가능하다.
		// 아래 함수 DrawFigureImage는 Image좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
		// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일경우 불투명, 0일경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
		// 여기서 0.25이므로 옅은 반투명 상태라고 볼 수 있다.
		// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)
		if(IsFail(res = layer.DrawFigureImage(&flfaContours, RED, 1, RED, EGUIViewImagePenStyle_Solid, 1, .2f)))
		{
			ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
			break;
		}

		// Contours 정보값을 각각 확인하는 코드
		for(int64_t i = 0; i < flfaContours.GetCount(); ++i)
		{
			CFLRegion* pFlrg = (CFLRegion*)flfaContours.GetAt(i);

			// 폴리곤의 정점 정보를 콘솔에 출력
			printf("No. %lld : [", i);

			for(int64_t j = 0; j < pFlrg->GetCount(); ++j)
			{
				if(j)
					printf(",");

				CFLPoint<double>* pFlpVertex = (CFLPoint<double>*)pFlrg->GetAt(j);

				printf("(%lf,%lf)", pFlpVertex->x, pFlpVertex->y);
			}

			if(pFlrg->GetExclusiveRegion())
			{
				printf("\nExclusive region\n{ ");

				CFLFigureArray* pFlfaExclusive = pFlrg->GetExclusiveRegion();

				for(int64_t j = 0; j < pFlfaExclusive->GetCount(); ++j)
				{
					CFLRegion* pFlrgExclusive = (CFLRegion*)pFlfaExclusive->GetAt(j);

					printf("No. %lld : [", j);

					for(int64_t k = 0; k < pFlrgExclusive->GetCount(); ++k)
					{
						if(k)
							printf(",");

						CFLPoint<double>* pFlpVertex = (CFLPoint<double>*)pFlrgExclusive->GetAt(k);

						printf("(%lf,%lf)", pFlpVertex->x, pFlpVertex->y);
					}

					printf("]\n");
				}

				printf(" }\n");
			}

			printf("]\n\n");

			CFLRect<double> flr = pFlrg->GetBoundaryRect();

			CFLString<wchar_t> strNumber;
			strNumber.Format(L"%d", i);

			layer.DrawTextImage(&CFLPoint<double>(flr.left, (flr.top + flr.bottom) * .5), (wchar_t*)strNumber.GetString(), BLACK, YELLOW, 12, false, 0, EGUIViewImageTextAlignment_CENTER, nullptr, 1.f, 1.f, EGUIViewImageFontWeight_BOLD, false);
		}


		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImage.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}