#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


enum EDst
{
	EDst_Gradient = 0,
	EDst_Prewitt,
	EDst_Sobel,
	EDstCount,
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSrcImage;
	CFLImage arrFliDstImage[EDstCount];

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap arrViewImageDst[EDstCount];

	bool bError = false;

	do
	{
		CResult res = EResult_UnknownError;
		// Source 이미지 로드 // Load the source image
		if(IsFail(res = fliSrcImage.Load(L"../../ExampleImages/EdgeDetection/Alphabat.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create the source image view
		if(IsFail(res = viewImageSrc.Create(400, 0, 800, 400)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if(IsFail(res = viewImageSrc.SetImagePtr(&fliSrcImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		for(int32_t i = 0; i < EDstCount; ++i)
		{
			// Destination 이미지를 Src 이미지와 동일한 이미지로 생성
			if(IsFail(res = arrFliDstImage[i].Assign(fliSrcImage)))
			{
				ErrorPrint(res, "Failed to assign the image file.\n");
				bError = true;
				break;
			}

			int32_t i32X = (i + 1) % 2;
			int32_t i32Y = (i + 1) / 2;

			// Destination 이미지 뷰 생성 // Create the destination image view
			if(IsFail(res = arrViewImageDst[i].Create(i32X * 400 + 400, i32Y * 400, i32X * 400 + 400 + 400, i32Y * 400 + 400)))
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				bError = true;
				break;
			}

			// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
			if(IsFail(res = arrViewImageDst[i].SetImagePtr(&arrFliDstImage[i])))
			{
				ErrorPrint(res, "Failed to set image object on the image view.\n");
				bError = true;
				break;
			}

			// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
			if(IsFail(res = viewImageSrc.SynchronizePointOfView(&arrViewImageDst[i])))
			{
				ErrorPrint(res, "Failed to synchronize view\n");
				bError = true;
				break;
			}

			// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
			if(IsFail(res = viewImageSrc.SynchronizeWindow(&arrViewImageDst[i])))
			{
				ErrorPrint(res, "Failed to synchronize window.\n");
				bError = true;
				break;
			}
		}

		if(bError)
			break;

		// ROI 설정을 위한 FLRectL 생성
		CFLRect<int32_t> flrROI(200, 200, 500, 500);

		// GradientFilter 객체 생성 // Create GradientFilter object
		CGradientFilter gradientFilter;

		// Source 이미지 설정 // Set the source image
		gradientFilter.SetSourceImage(fliSrcImage);
		// Source ROI 설정 // Set the source ROI
		gradientFilter.SetSourceROI(flrROI);
		// Destination 이미지 설정 // Set the destination image
		gradientFilter.SetDestinationImage(arrFliDstImage[EDst_Gradient]);
		// Destination ROI 설정
		gradientFilter.SetDestinationROI(flrROI);
		// GradientFilter 커널 연산 방법 설정
		gradientFilter.SetKernelMethod(CGradientFilter::EKernel_Gradient);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = gradientFilter.Execute()))
		{
			ErrorPrint(res, "Failed to execute gradient filter.");
			break;
		}


		// PrewittFilter 객체 생성 // Create PrewittFilter object
		CPrewittFilter prewittFilter;

		// Source 이미지 설정 // Set the source image
		prewittFilter.SetSourceImage(fliSrcImage);
		// Source ROI 설정 // Set the source ROI
		prewittFilter.SetSourceROI(flrROI);
		// Destination 이미지 설정 // Set the destination image
		prewittFilter.SetDestinationImage(arrFliDstImage[EDst_Prewitt]);
		// Destination ROI 설정
		prewittFilter.SetDestinationROI(flrROI);
		// PrewittFilter 커널 연산 방법 설정
		prewittFilter.SetKernelMethod(CPrewittFilter::EKernel_Prewitt);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = prewittFilter.Execute()))
		{
			ErrorPrint(res, "Failed to execute prewitt filter.");
			break;
		}


		// SobelFilter 객체 생성 // Create SobelFilter object
		CSobelFilter sobelFilter;

		// Source 이미지 설정 // Set the source image
		sobelFilter.SetSourceImage(fliSrcImage);
		// Source ROI 설정 // Set the source ROI
		sobelFilter.SetSourceROI(flrROI);
		// Destination 이미지 설정 // Set the destination image
		sobelFilter.SetDestinationImage(arrFliDstImage[EDst_Sobel]);
		// Destination ROI 설정
		sobelFilter.SetDestinationROI(flrROI);
		// SobelFilter 커널 연산 방법 설정
		sobelFilter.SetKernelMethod(CSobelFilter::EKernel_Sobel);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = sobelFilter.Execute()))
		{
			ErrorPrint(res, "Failed to execute sobel filter.");
			break;
		}


		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap arrLayerDst[EDstCount];

		for(int32_t i = 0; i < EDstCount; ++i)
		{
			arrLayerDst[i] = arrViewImageDst[i].GetLayer(0);
		}

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		arrLayerDst[EDst_Gradient].Clear();
		arrLayerDst[EDst_Prewitt].Clear();
		arrLayerDst[EDst_Sobel].Clear();


		// ROI영역이 어디인지 알기 위해 디스플레이 한다 // Display to find out where ROI is
		// FLImaging의 Figure 객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능 // FLimaging's Figure objects can be displayed as a function regardless of the shape
		// 아래 함수 DrawFigureImage는 Image좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
		// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일경우 불투명, 0일경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
		// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)
		if(IsFail(res = layerSrc.DrawFigureImage(&flrROI, LIME)))
			ErrorPrint(res, "Failed to draw figure\n");

		if(IsFail(res = arrLayerDst[EDst_Gradient].DrawFigureImage(&flrROI, LIME)))
			ErrorPrint(res, "Failed to draw figure\n");

		if(IsFail(res = arrLayerDst[EDst_Prewitt].DrawFigureImage(&flrROI, LIME)))
			ErrorPrint(res, "Failed to draw figure\n");

		if(IsFail(res = arrLayerDst[EDst_Sobel].DrawFigureImage(&flrROI, LIME)))
			ErrorPrint(res, "Failed to draw figure\n");


		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerSrc.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)))
			ErrorPrint(res, "Failed to draw text\n");

		if(IsFail(res = arrLayerDst[EDst_Gradient].DrawTextCanvas(CFLPoint<double>(0, 0), L"Gradient Image", YELLOW, BLACK, 20)))
			ErrorPrint(res, "Failed to draw text\n");

		if(IsFail(res = arrLayerDst[EDst_Prewitt].DrawTextCanvas(CFLPoint<double>(0, 0), L"Prewitt Image", YELLOW, BLACK, 20)))
			ErrorPrint(res, "Failed to draw text\n");

		if(IsFail(res = arrLayerDst[EDst_Sobel].DrawTextCanvas(CFLPoint<double>(0, 0), L"Sobel Image", YELLOW, BLACK, 20)))
			ErrorPrint(res, "Failed to draw text\n");


		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc.Invalidate(true);
		arrViewImageDst[EDst_Gradient].Invalidate(true);
		arrViewImageDst[EDst_Prewitt].Invalidate(true);
		arrViewImageDst[EDst_Sobel].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() && arrViewImageDst[EDst_Gradient].IsAvailable()
			&& arrViewImageDst[EDst_Prewitt].IsAvailable() && arrViewImageDst[EDst_Sobel].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}