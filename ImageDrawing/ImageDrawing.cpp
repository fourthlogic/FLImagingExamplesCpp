#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
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

		// 레이어는 따로 해제하지 않아도 View가 해제 될 때 같이 해제된다. // The layer is released together when View is released without releasing it separately.
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);

		// 이미지에 출력하기 위해 이미지 드로잉 객채에서 레이어를 얻어옴 // Gets layers from image drawing object for output to image
		CGUIViewImageLayerWrap layerDst = fliImageDrawing.GetLayer();

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerDst.Clear();


		// 이미지에 정보 표시 // Display image information
		layerSrc.DrawTextImage(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30);
		layerDst.DrawTextImage(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 30);

		CFLPoint<double> flpDraw(10., 10.);

		layerSrc.DrawFigureImage(flpDraw, RED, 3);
		layerDst.DrawFigureImage(flpDraw, RED, 3);

		CFLLine<double> fllDraw(15., 15., 80., 30.);

		layerSrc.DrawFigureImage(fllDraw, ORANGE, 3);
		layerDst.DrawFigureImage(fllDraw, ORANGE, 3);

		CFLRect<double> flrDraw(80., 80., 150., 150.);

		layerSrc.DrawFigureImage(flrDraw, YELLOW, 3);
		layerDst.DrawFigureImage(flrDraw, YELLOW, 3);

		CFLQuad<double> flqDraw(170., 170., 200., 180., 220., 210., 180., 230.);

		layerSrc.DrawFigureImage(flqDraw, GREEN, 3);
		layerDst.DrawFigureImage(flqDraw, GREEN, 3);

		CFLCircle<double> flcDraw(250., 250., 50.);

		layerSrc.DrawFigureImage(flcDraw, BLUE, 3);
		layerDst.DrawFigureImage(flcDraw, BLUE, 3);

		CFLEllipse<double> fleDraw(350., 350., 50., 80., 25.);

		layerSrc.DrawFigureImage(fleDraw, VIOLET, 3);
		layerDst.DrawFigureImage(fleDraw, VIOLET, 3);


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