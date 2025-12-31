#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliImage;

	// 이미지 드로잉 객체 선언 // Declare image drawing object
	CFLImageDrawingWrap fliImageDrawing;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageDst;

	do
	{
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Load image
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/Blob/AlignBall.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Drawing 이미지를 Src 이미지와 동일한 이미지로 생성
		if(IsFail(res = fliImageDrawing.Assign(fliImage)))
		{
			ErrorPrint(res, "Failed to assign the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImageSrc.Create(400, 0, 800, 400)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImageDst.Create(800, 0, 1200, 400)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다. // Synchronize the viewpoints of the two image views.
		if(IsFail(res = viewImageSrc.SynchronizePointOfView(&viewImageDst)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSrc.SynchronizeWindow(&viewImageDst)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		if(IsFail(res = viewImageSrc.SetImagePtr(&fliImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Blob 객체 생성 // Create Blob object
		CBlob blob;

		// 처리할 이미지 설정
		blob.SetSourceImage(fliImage);
		// 논리 조건 설정
		blob.SetLogicalCondition(ELogicalCondition_Less);
		// 임계값 설정,  위의 조건과 아래의 조건이 합쳐지면 127보다 작은 객체를 검출
		blob.SetThreshold(127);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = blob.Execute()))
		{
			ErrorPrint(res, "Failed to execute Blob.");
			break;
		}

		// 결과 객체들 중 해당되는 조건을 가진 객체를 제거

		// 면적이 500보다 작은 객체들을 제거
		if(IsFail(res = blob.Filter(CBlob::EFilterItem_Area, 500, ELogicalCondition_LessEqual)))
		{
			ErrorPrint(res, "Blob filtering algorithm error occurred.");
			break;
		}

		// Blob 결과를 얻어오기 위해 FigureArray 선언
		CFLFigureArray flfaBoundaryRects;

		// Blob 결과들 중 Boundary Rectangle 을 얻어옴
		if(IsFail(res = blob.GetResultBoundaryRects(flfaBoundaryRects)))
		{
			ErrorPrint(res, "Failed to get boundary rects from the Blob object.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어를 얻어옴 // Obtain layer from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer1 = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layer2 = viewImageSrc.GetLayer(1);
		CGUIViewImageLayerWrap layer3 = viewImageSrc.GetLayer(2);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer1.Clear();
		layer2.Clear();
		layer3.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		if(IsFail(res = layer3.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// flfaBoundaryRects 는 Figure들의 배열이기 때문에 Layer에 넣기만 해도 모두 드로윙이 가능하다.
		// 아래 함수 DrawFigureImage는 Image좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
		// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일경우 불투명, 0일경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
		// 여기서 0.25이므로 옅은 반투명 상태라고 볼 수 있다.
		// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)
		if(IsFail(res = layer1.DrawFigureImage(&flfaBoundaryRects, RED, 1, RED, EGUIViewImagePenStyle_Solid, 1, 0.25)))
		{
			ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
			break;
		}

		// Rect 정보값을 각각 확인하는 코드
		for(int64_t i = 0; i < flfaBoundaryRects.GetCount(); ++i)
		{
			CFLRect<int32_t>* pFlrRect = (CFLRect<int32_t>*)flfaBoundaryRects.GetAt(i);

			if(pFlrRect)
			{
				printf("No. [%lld]\n", i);
				printf("LeftTop     : (%d,%d)\n", pFlrRect->left, pFlrRect->top);
				printf("RightBottom : (%d,%d)\n", pFlrRect->right, pFlrRect->bottom);
				printf("Width  : %.lf\n", pFlrRect->GetWidth());
				printf("Height : %.lf\n", pFlrRect->GetHeight());
				printf("Center : (%.1lf, %.1lf)\n\n", pFlrRect->GetCenter().x, pFlrRect->GetCenter().y);

				CFLString<wchar_t> strNumber;
				strNumber.Format(L"[%d]", i);

				CFLString<wchar_t> strLeftTop, strRightBottom, strInfo;
				strLeftTop.Format(L"(%d, %d)", pFlrRect->left, pFlrRect->top);
				strRightBottom.Format(L"(%d, %d)", pFlrRect->right, pFlrRect->bottom);
				strInfo.Format(L"Width : %.lf\nHeight : %.lf\nCenter : (%.1lf, %.1lf)", pFlrRect->GetWidth(), pFlrRect->GetHeight(), pFlrRect->GetCenter().x, pFlrRect->GetCenter().y);

				// 아래 함수 DrawTextImage는 Image좌표를 기준으로 하는 Text를 Drawing 한다는 것을 의미한다.
				// 파라미터 순서 : 레이어 -> 문자열 좌표 -> 문자열 지정 -> 문자열 색 -> 면 색 -> 폰트 크기 -> 실제 크기로 그릴지의 여부 -> 각도 -> 문자열의 위치 기준
				layer2.DrawTextImage(&CFLPoint<double>(pFlrRect->GetCenter()), strNumber, CYAN, BLACK, 12, false, 0., FLImaging::GUI::EGUIViewImageTextAlignment_CENTER_CENTER);
				layer2.DrawTextImage(&CFLPoint<double>(pFlrRect->left, pFlrRect->top), strLeftTop, YELLOW, BLACK, 12, false, 0., FLImaging::GUI::EGUIViewImageTextAlignment_RIGHT_BOTTOM);
				layer2.DrawTextImage(&CFLPoint<double>(pFlrRect->right, pFlrRect->bottom), strRightBottom, YELLOW, BLACK);
				layer2.DrawTextImage(&CFLPoint<double>(pFlrRect->right, pFlrRect->top), strInfo, LIME, BLACK, 12, false, 0., FLImaging::GUI::EGUIViewImageTextAlignment_LEFT_BOTTOM);
			}
		}

		// 이미지에 출력하기 위해 이미지 드로잉 객채에서 레이어를 얻어옴 // Gets layers from image drawing object for output to image
		CGUIViewImageLayerWrap layer = fliImageDrawing.GetLayer();

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer.Clear();

		// 기존 레이어에 드로잉된 원소들을 해당 레이어 뒤쪽에 추가합니다. // Add elements drawn from an existing layer to the back of that layer.
		if(IsFail(res = layer.PushBack(layer1)))
		{
			ErrorPrint(res, "Failed to push back.\n");
			break;
		}

		if(IsFail(res = layer.PushBack(layer2)))
		{
			ErrorPrint(res, "Failed to push back.\n");
			break;
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if(IsFail(res = layer.DrawTextImage(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지에 그립니다. // Draw in the image.
		if(IsFail(res = fliImageDrawing.Draw()))
		{
			ErrorPrint(res, "Failed to draw.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		if(IsFail(res = viewImageDst.SetImagePtr(&fliImageDrawing)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc.Invalidate();
		viewImageDst.Invalidate();

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() || viewImageDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}