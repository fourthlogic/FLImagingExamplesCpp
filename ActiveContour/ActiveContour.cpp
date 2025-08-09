#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliSrcImage;
	CFLImage fliDstImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageDst;

	CResult res = EResult_UnknownError;

	do
	{
		// 이미지 로드 // Loads image
		if(IsFail(res = fliSrcImage.Load(L"../../ExampleImages/ActiveContour/Grid Of Cross.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image views
		if((res = viewImageSrc.Create(100, 0, 600, 500)).IsFail() ||
		   (res = viewImageDst.Create(600, 0, 1100, 500)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view. \n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if(IsFail(res = viewImageSrc.SynchronizePointOfView(&viewImageDst)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSrc.SynchronizeWindow(&viewImageDst)))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if((res = viewImageSrc.SetImagePtr(&fliSrcImage)).IsFail() ||
		   (res = viewImageDst.SetImagePtr(&fliDstImage)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view. \n");
			break;
		}


		// 알고리즘 객체 생성 // Create algorithm object
		CActiveContour activeContour;

		if((res = activeContour.SetSourceImage(fliSrcImage)).IsFail())
			break;
		CFLFigure* pFlfRegion = CFigureUtilities::ConvertFigureStringToObject(L"RG[D(129.22800000000007,126.67680000000001),D(731.22800000000007,120.67680000000001),D(733.22800000000007,262.67680000000001),D(253.22800000000007,246.67680000000001),D(265.22800000000007,600.67679999999996),D(603.22800000000007,594.67679999999996),D(607.22800000000007,400.67680000000001),D(403.22800000000007,396.67680000000001),D(409.22800000000007,448.67680000000001),D(565.22800000000007,450.67680000000001),D(549.22800000000007,556.67679999999996),D(289.22800000000007,558.67679999999996),D(291.22800000000007,292.67680000000001),D(721.22800000000007,294.67680000000001),D(721.22800000000007,720.67679999999996),D(119.22800000000007,718.67679999999996),D(113.22800000000007,142.67680000000001)]");
		if((res = activeContour.SetSourceROI(pFlfRegion)).IsFail())
			break;
		if((res = activeContour.SetDestinationImage(fliDstImage)).IsFail())
			break;
		if((res = activeContour.SetPointCount(3000)).IsFail())
			break;
		if((res = activeContour.SetMaxLength(3)).IsFail())
			break;
		if((res = activeContour.SetLowThreshold(20)).IsFail())
			break;
		if((res = activeContour.SetHighThreshold(50)).IsFail())
			break;
		if((res = activeContour.SetFitMargin(5)).IsFail())
			break;

		// 알고리즘 수행 // Execute the algorithm
		if((res = activeContour.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute the algorithm.");
			break;
		}


		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageSrc.Invalidate(true);
		viewImageDst.Invalidate(true);

		for(int32_t i32Iteration = 0; i32Iteration < 20; ++i32Iteration)
		{
			activeContour.Fit();
			activeContour.Fit();
			activeContour.Fit();
			activeContour.Fit();
			activeContour.Fit();
			activeContour.Fit();
			activeContour.Fit();
			activeContour.Fit();
			activeContour.Fit();
			activeContour.Fit();
			activeContour.Spacing();
			activeContour.Spacing();
			activeContour.Spacing();
			activeContour.Spacing();
			activeContour.Spacing();

			/* PushBack Figure */
			{
				viewImageSrc.ClearFigureObject();
				viewImageSrc.PushBackFigureObject(activeContour.GetContourFigure());
				viewImageSrc.Invalidate(true);

				Sleep(50);
			}
		}

		viewImageSrc.PushBackFigureObject((CFLFigure*)activeContour.GetSourceROI());

		// 출력을 위한 이미지 레이어를 얻어옵니다. //  Gets the image layer for output.
		// 따로 해제할 필요 없음 // No need to release separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerDst = viewImageDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Delete the shapes drawn on the existing layer
		layerSrc.Clear();
		layerDst.Clear();

		// View 정보를 디스플레이 합니다. // Display View information.
		CFLPoint<double> flpPoint = new CFLPoint<double>(0, 0);
		if((res = (layerSrc.DrawTextCanvas(flpPoint, L"Source Image", YELLOW, BLACK, 30))).IsFail() ||
		   (res = layerDst.DrawTextCanvas(flpPoint, L"Destination Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text. \n");
			break;
		}

		// Zoom Fit
		viewImageSrc.ZoomFit();
		viewImageDst.ZoomFit();

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageSrc.Invalidate(true);
		viewImageDst.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() && viewImageDst.IsAvailable())
			CThreadUtilities::Sleep(1);

		delete pFlfRegion;
	}
	while(false);

	return 0;
}