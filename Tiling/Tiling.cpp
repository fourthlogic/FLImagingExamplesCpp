#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	const int32_t i32SrcImageCount = 4;

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSrcImage;
	CFLImage fliDstImage;
	CFLImage fliSrcImages[4];

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap arrViewImageSrc[i32SrcImageCount];
	CGUIViewImageWrap viewImageDst;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// Source 이미지 로드 // Load the source image
		for(int32_t i = 0; i<i32SrcImageCount; ++i)
		{
			CFLStringW flsFileName;

			flsFileName.Format(L"TilingSourceImage%d.flif", i);

			if(IsFail(res = fliSrcImages[i].Load(L"../../ExampleImages/Tiling/" + flsFileName)))
			{
				ErrorPrint(res, "Failed to load the image file.\n");
				break;
			}
		}		

		// 여러 장의 이미지를 하나의 FLImage로 생성 // Create multiple images into one FLImage
		fliSrcImage = fliSrcImages[0];

		for(int32_t i = 1; i < i32SrcImageCount; ++i)
		{
			fliSrcImage.PushBackPage(fliSrcImages[i]);
		}

		// Destination 이미지 로드 // Load the destination image
		if(IsFail(res = fliDstImage.Load(L"../../ExampleImages/Tiling/TilingDestinationImage.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create the source image view
		for(int32_t i = 0; i < i32SrcImageCount; ++i)
		{
			if(i < 2)
			{
				if(IsFail(res = arrViewImageSrc[i].Create(100 + 300 * i, 0, 400 + 300 * i, 300)))
				{
					ErrorPrint(res, "Failed to create the image view.\n");
					break;
				}
			}
			else
			{
				if(IsFail(res = arrViewImageSrc[i].Create(100 + 300 * (i - 2), 300, 400 + 300 * (i - 2), 600)))
				{
					ErrorPrint(res, "Failed to create the image view.\n");
					break;
				}
			}

			arrViewImageSrc[i].SetFixThumbnailView(true);
		}

		// Source 이미지 뷰에 페이지에 각각 존재하는 이미지를 각각에 뷰에 디스플레이 하기 위해 얕은 복사로 각각의 이미지 객체에 할당.
		// Assign each image object to each image object by shallow copying to display each image on the page in the source image view to each view.
		for(int32_t i = 0; i < i32SrcImageCount; ++i)
		{
			// 얕은 복사된 해당 페이지를 선택한 이미지를 뷰에 디스플레이
			// Display the selected image of the shallow-copied page in the view
			if(IsFail(res = arrViewImageSrc[i].SetImagePtr(&fliSrcImages[i])))
			{
				ErrorPrint(res, "Failed to set image object on the image view.\n");
				break;
			}
		}

		// Destination 이미지 뷰 생성 // Create the destination image view
		if(IsFail(res = viewImageDst.Create(912, 0, 1424, 612)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if(IsFail(res = viewImageDst.SetImagePtr(&fliDstImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 이미지 뷰 윈도우의 위치를 맞춤 // Align the position of the image view window
		for(int32_t i = 1; i< i32SrcImageCount; ++i)
		{
			if(IsFail(res = arrViewImageSrc[0].SynchronizeWindow(&arrViewImageSrc[i])))
			{
				ErrorPrint(res, "Failed to synchronize window.\n");
				break;
			}
		}

		if(IsFail(res = arrViewImageSrc[0].SynchronizeWindow(&viewImageDst)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}
		
		// Source 이미지에 ROI 추가 // Add ROI to source image
		CFLRect<double> flRect(30, 68, 200, 235);
		
		flRect.SetName(L"0");
		fliSrcImage.SelectPage(0);
		fliSrcImage.PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(flRect));
		fliSrcImages[0].PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(flRect));

		flRect = CFLRect<double>(260, 135, 415, 440);
		flRect.SetName(L"1");
		fliSrcImage.PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(flRect));
		fliSrcImages[0].PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(flRect));

		flRect = CFLRect<double>(280, 250, 480, 480);
		flRect.SetName(L"0");
		fliSrcImage.SelectPage(1);
		fliSrcImage.PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(flRect));
		fliSrcImages[1].PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(flRect));

		flRect = CFLRect<double>(110, 150, 350, 440);
		flRect.SetName(L"0");
		fliSrcImage.SelectPage(2);
		fliSrcImage.PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(flRect));
		fliSrcImages[2].PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(flRect));

		flRect = CFLRect<double>(220, 230, 470, 450);
		flRect.SetName(L"0");
		fliSrcImage.SelectPage(3);
		fliSrcImage.PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(flRect));
		fliSrcImages[3].PushBackFigure(CFigureUtilities::ConvertFigureObjectToString(flRect));

		// Destination 이미지에 ROI 추가 // Add ROI to destination image
		fliDstImage.PushBackFigure(L"D(79.292035, 67.964602, 292.247788, 267.327434, INFO[NAME(0_0)])");
		fliDstImage.PushBackFigure(L"D(296.778761, 271.858407, 459.893805, 444.035398, INFO[NAME(0_1)])");
		fliDstImage.PushBackFigure(L"D(88.353982, 738.548673, 337.557522, 956.035398, INFO[NAME(1_0)])");
		fliDstImage.PushBackFigure(L"D(482.548673, 457.628319, 659.256637, 675.115044, INFO[NAME(2_0)])");
		fliDstImage.PushBackFigure(L"D(659.256638, 222.017700, 835.964602, 439.504425, INFO[NAME(3_0)])");

		// Tiling 객체 생성 // Create Tiling object
		CTiling tiling;

		// Source 이미지 설정 // Set the source image
		tiling.SetSourceImage(fliSrcImage);
		// Destination 이미지 설정 // Set the destination image
		tiling.SetDestinationImage(fliDstImage);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = tiling.Execute()))
		{
			ErrorPrint(res, "Failed to execute tiling.");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if(IsFail(res = viewImageDst.ZoomFit()))
		{
			ErrorPrint(res, "Failed to zoom fit of the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc[i32SrcImageCount];
		CGUIViewImageLayerWrap layerDst = viewImageDst.GetLayer(0);
		
		for(int32_t i = 0; i < i32SrcImageCount; ++i)
		{
			layerSrc[i] = arrViewImageSrc[i].GetLayer(0);

			// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
			layerSrc[i].Clear();
		}

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerDst.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		for(int32_t i = 0 ; i< i32SrcImageCount; ++i)
		{
			CFLString<wchar_t> fls;
			fls.Format(L"Source Image #%d", i);

			if(IsFail(res = layerSrc[i].DrawTextCanvas(CFLPoint<double>(0, 0), fls, YELLOW, BLACK, 20)))
			{
				ErrorPrint(res, "Failed to draw text\n");
				break;
			}
		}

		if(IsFail(res = layerDst.DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신한다. // Update the image view.
		for(int32_t i = 0 ; i< i32SrcImageCount; ++i)
			arrViewImageSrc[i].Invalidate(true);

		viewImageDst.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		bool bRun = true;
		while(bRun)
		{
			for(int32_t i = 0 ; i < 4; ++i)
			{
				bRun &= arrViewImageSrc[i].IsAvailable();
			}

			bRun &= viewImageDst.IsAvailable();

			CThreadUtilities::Sleep(1);
		}
	}
	while(false);

	return 0;
}