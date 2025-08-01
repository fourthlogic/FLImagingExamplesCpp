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
	CGUIViewImageWrap viewImageSorted;
	CGUIViewImageWrap viewImageRecover;

	do
	{
		CResult res = EResult_UnknownError;
		// 이미지 로드 // Load image
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/Blob/Ball.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImageSorted.Create(200, 0, 968, 576)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageRecover.Create(968, 0, 1736, 576)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageSorted.SetImagePtr(&fliImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageRecover.SetImagePtr(&fliImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Source 이미지 뷰와 Destination 이미지 뷰의 시점을 동기화 한다
		if(IsFail(res = viewImageSorted.SynchronizePointOfView(&viewImageRecover)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// Image 크기에 맞게 view의 크기를 조정 // Zoom the view to fit the image size
		if(IsFail(res = viewImageSorted.ZoomFit()))
		{
			ErrorPrint(res, "Failed to zoom fit\n");
			break;
		}

		if(IsFail(res = viewImageRecover.ZoomFit()))
		{
			ErrorPrint(res, "Failed to zoom fit\n");
			break;
		}

		// Blob 객체 생성 // Create Blob object
		CBlobSubsampled blob;

		// ROI 범위 설정
		CFLRect<int32_t> flrROI(450, 425, 1024, 800);

		// 처리할 이미지 설정
		blob.SetSourceImage(fliImage);
		// 처리할 ROI 설정
		blob.SetSourceROI(flrROI);
		// 논리 조건 설정
		blob.SetLogicalCondition(ELogicalCondition_GreaterEqual);
		// 임계값 설정,  위의 조건과 아래의 조건이 합쳐지면 100보다 같거나 큰 객체를 검출
		blob.SetThreshold(100);
		// Subsampling Level 설정
		blob.SetSubsamplingLevel(3);
		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = blob.Execute()))
		{
			ErrorPrint(res, "Failed to execute Blob.");
			break;
		}

		// 결과 객체들 중 해당되는 조건을 가진 객체를 제거

		// 50보다 같거나 큰 장변 길이를 가진 객체들을 제거
		if(IsFail(res = blob.Filter(CBlob::EFilterItem_MinimumEnclosingRectangleLongSideLength, 50, ELogicalCondition_GreaterEqual)))
		{
			ErrorPrint(res, "Blob filtering algorithm error occurs.");
			break;
		}

		// 50보다 같거나 큰 단변 길이를 가진 객체들을 제거
		if(IsFail(res = blob.Filter(CBlob::EFilterItem_MinimumEnclosingRectangleShortSideLength, 50, ELogicalCondition_GreaterEqual)))
		{
			ErrorPrint(res, "Blob filtering algorithm error occurred.");
			break;
		}

		// 면적이 50보다 작은 객체들을 제거
		if(IsFail(res = blob.Filter(CBlob::EFilterItem_Area, 50, ELogicalCondition_LessEqual)))
		{
			ErrorPrint(res, "Blob filtering algorithm error occurred.");
			break;
		}

		// Blob 결과를 얻어오기 위해 FigureArray 선언
		CFLFigureArray flfaSortedBoundaryRects;
		CFLFigureArray flfaRecoverBoundaryRects;

		CFLArray<int32_t> flaItem;
		CFLArray<int32_t> flaOrder;

		// 첫 번째 조건을 Bound rect center y좌표, 내림차순 정렬
		flaItem.PushBack(CBlob::EFilterItem_BoundaryRectCenterY);
		flaOrder.PushBack(CBlob::EOrder_Descending);

		// 두 번째 조건을 Bound rect center x좌표, 내림차순 정렬
		flaItem.PushBack(CBlob::EFilterItem_BoundaryRectCenterX);
		flaOrder.PushBack(CBlob::EOrder_Descending);

		// Blob 결과를 정렬
		if(IsFail(res = blob.Sort(flaItem, flaOrder)))
		{
			ErrorPrint(res, "Failed to sort from the Blob object.");
			break;
		}

		// 정렬된 Blob 결과들 중 Boundary Rectangle 을 얻어옴
		if(IsFail(res = blob.GetResultBoundaryRects(flfaSortedBoundaryRects)))
		{
			ErrorPrint(res, "Failed to get boundary rects from the Blob object.");
			break;
		}

		// Blob 정렬 상태를 초기 상태로 복구
		if(IsFail(res = blob.Sort(CBlob::EFilterItem_Unselected)))
		{
			ErrorPrint(res, "Failed to get boundary rects from the Blob object.");
			break;
		}

		// 복구된 Blob 결과들 중 Boundary Rectangle 을 얻어옴
		if(IsFail(res = blob.GetResultBoundaryRects(flfaRecoverBoundaryRects)))
		{
			ErrorPrint(res, "Failed to get boundary rects from the Blob object.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately

		CGUIViewImageLayerWrap layerSorted = viewImageSorted.GetLayer(0);

		CGUIViewImageLayerWrap layerRecover = viewImageRecover.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSorted.Clear();
		layerRecover.Clear();

		if(IsFail(res = layerSorted.DrawTextCanvas(&CFLPoint<double>(), L"Sort", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text on the image view.\n");
			break;
		}

		if(IsFail(res = layerRecover.DrawTextCanvas(&CFLPoint<double>(), L"No Sort", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text on the image view.\n");
			break;
		}

		// ROI영역이 어디인지 알기 위해 디스플레이 한다 // Display to find out where ROI is
		// FLImaging의 Figure객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능
		if(IsFail(res = layerSorted.DrawFigureImage(&flrROI, BLUE)))
		{
			ErrorPrint(res, "Failed to draw figures objects on the image view.\n");
			break;
		}

		if(IsFail(res = layerRecover.DrawFigureImage(&flrROI, BLUE)))
		{
			ErrorPrint(res, "Failed to draw figures objects on the image view.\n");
			break;
		}

		// flfaOriginBoundaryRects 는 Figure들의 배열이기 때문에 Layer에 넣기만 해도 모두 드로윙이 가능하다.
		// 아래 함수 DrawFigureImage는 Image좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
		// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일경우 불투명, 0일경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
		// 여기서 0.25이므로 옅은 반투명 상태라고 볼 수 있다.
		// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)
		if(IsFail(res = layerSorted.DrawFigureImage(&flfaSortedBoundaryRects, RED, 1, RED, EGUIViewImagePenStyle_Solid, 1, 0.25)))
		{
			ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
			break;
		}

		if(IsFail(res = layerRecover.DrawFigureImage(&flfaRecoverBoundaryRects, RED, 1, RED, EGUIViewImagePenStyle_Solid, 1, 0.25)))
		{
			ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
			break;
		}

		// Rect 정보값을 각각 확인하는 코드
		for(int64_t i = 0; i < flfaSortedBoundaryRects.GetCount(); ++i)
		{
			CFLRect<int32_t>* pFlrRect = (CFLRect<int32_t>*)flfaSortedBoundaryRects.GetAt(i);

			if(pFlrRect)
				printf("Sorted No. %lld : (%d,%d,%d,%d)\n", i, pFlrRect->left, pFlrRect->top, pFlrRect->right, pFlrRect->bottom);

			CFLString<wchar_t> strNumber;
			strNumber.Format(L"%d", i);

			layerSorted.DrawTextImage(&CFLPoint<double>(pFlrRect->GetCenter()), (wchar_t*)strNumber.GetString(), CYAN);

			pFlrRect = (CFLRect<int32_t>*)flfaRecoverBoundaryRects.GetAt(i);

			if(pFlrRect)
				printf("Recover No. %lld : (%d,%d,%d,%d)\n", i, pFlrRect->left, pFlrRect->top, pFlrRect->right, pFlrRect->bottom);

			layerRecover.DrawTextImage(&CFLPoint<double>(pFlrRect->GetCenter()), (wchar_t*)strNumber.GetString(), CYAN);
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSorted.Invalidate(true);
		viewImageRecover.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSorted.IsAvailable() && viewImageRecover.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}