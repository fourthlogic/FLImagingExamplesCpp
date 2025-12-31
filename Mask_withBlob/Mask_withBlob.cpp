#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSrcImage;
	CFLImage fliSrcImageMask;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageSrcMask;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// Source 이미지 로드 // Load the source image
		if(IsFail(res = fliSrcImage.Load(L"../../ExampleImages/Mask/ChessBoard.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 원본 이미지와의 결과 비교를 위해 이미지를 복사 // Copy the image to compare the result with the original image
		if(IsFail(res = fliSrcImageMask.Assign(fliSrcImage)))
		{
			ErrorPrint(res, "Failed to assign the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create the source image view
		if(IsFail(res = viewImageSrc.Create(400, 0, 912, 612)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Mask 이미지 뷰 생성 // Create mask image view
		if(IsFail(res = viewImageSrcMask.Create(912, 0, 1424, 612)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImageSrc.SynchronizePointOfView(&viewImageSrcMask)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSrc.SynchronizeWindow(&viewImageSrcMask)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if(IsFail(res = viewImageSrc.SetImagePtr(&fliSrcImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Mask 이미지 뷰에 이미지를 디스플레이 // Display the image in the Mask image view
		if(IsFail(res = viewImageSrcMask.SetImagePtr(&fliSrcImageMask)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}


		// Blob 객체 생성 // Create Blob object
		CBlob blob;

		// 처리할 이미지 설정 // Set the image to process
		blob.SetSourceImage(fliSrcImage);
		// Threshold 모드 설정. 여기서는 2중 Threshold에 두개의 조건의 And 조건을 참으로 설정한다.
		// Threshold mode setting. Here, set the AND condition of the two conditions to true in the double threshold.
		blob.SetThresholdMode(EThresholdMode_Dual_And);
		// 논리 조건 설정 // Set logical condition
		blob.SetLogicalCondition(ELogicalCondition_Greater, ELogicalCondition_Less);
		// 임계값 설정,  위의 조건과 아래의 조건이 합쳐지면 130보다 크고 240보다 작은 객체를 검출
		// Threshold setting, when the above and below conditions are combined, an object greater than 130 and less than 240 is detected
		blob.SetThreshold(130, 240);

		// 2중 임계 값 설정을 아래와 같이 할 수도 있음. // You can set the double threshold as below.
		//		blob.SetThreshold(130, EThresholdIndex_First);
		// 		blob.SetThreshold(240, EThresholdIndex_Second);

		// 가운데 구멍난 Contour를 지원하기 위해 Perforated 모드 설정
		// Set perforated mode to support perforated contours
		blob.SetContourResultType(CBlob::EContourResultType_Perforated);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = blob.Execute()))
		{
			ErrorPrint(res, "Failed to execute Blob.");
			break;
		}

		// Blob 결과를 얻어오기 위해 FigureArray 선언 // Declare FigureArray to get blob result
		CFLFigureArray flfaContours;

		// Blob 결과들 중 Contour를 얻어옴 // Get Contour from Blob results
		if(IsFail(res = blob.GetResultContours(flfaContours)))
		{
			ErrorPrint(res, "Failed to get boundary rects from the Blob object.");
			break;
		}


		// Mask 객체 생성 // Create Mask object
		CMask mask;

		// Source 이미지 설정 // Set the source image
		mask.SetSourceImage(fliSrcImageMask);
		// Blob 결과인 flfaContours를 Src ROI로 설정 // Set blob result flfaContours as Src ROI
		mask.SetSourceROI(flfaContours);
		// Mask 색상 지정 // set mask color
		mask.SetMask(CMultiVar<double>(20., 227., 248.));

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = mask.Execute()))
		{
			ErrorPrint(res, "Failed to execute mask.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerSrcMask = viewImageSrcMask.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerSrcMask.Clear();

		// ROI영역이 어디인지 알기 위해 디스플레이 한다 // Display to find out where ROI is
		// FLImaging의 Figure 객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능 // FLimaging's Figure objects can be displayed as a function regardless of the shape
		// 아래 함수 DrawFigureImage는 Image좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
		// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일경우 불투명, 0일경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
		// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)
		if(IsFail(res = layerSrc.DrawFigureImage(&flfaContours, LIME)))
			ErrorPrint(res, "Failed to draw figure\n");

		if(IsFail(res = layerSrcMask.DrawFigureImage(&flfaContours, LIME)))
			ErrorPrint(res, "Failed to draw figure\n");


		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = layerSrc.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerSrcMask.DrawTextCanvas(CFLPoint<double>(0, 0), L"Mask Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// Contours 정보값을 각각 확인하는 코드 // Code to check each Contours information value
		for(int64_t i = 0; i < flfaContours.GetCount(); ++i)
		{
			CFLRegion* pFlrg = (CFLRegion*)flfaContours.GetAt(i);

			// 폴리곤의 정점 정보를 콘솔에 출력 // Print polygon vertex information to the console
			printf("Blob Result No. %lld : [", i);
			for(int64_t j = 0; j < pFlrg->GetCount(); ++j)
			{
				if(j)
					printf(",");

				CFLPoint<int32_t>* pFlpVertex = (CFLPoint<int32_t>*)pFlrg->GetAt(j);

				printf("(%d,%d)", pFlpVertex->x, pFlpVertex->y);
			}

			printf("]\n\n");

			CFLRect<double> flr = pFlrg->GetBoundaryRect();

			CFLString<wchar_t> strNumber;
			strNumber.Format(L"%d", i);

			layerSrc.DrawTextImage(&CFLPoint<double>(flr.left, (flr.top + flr.bottom) * .5), (wchar_t*)strNumber.GetString(), BLACK, YELLOW, 12, false, 0, EGUIViewImageTextAlignment_CENTER, nullptr, 1.f, 1.f, EGUIViewImageFontWeight_BOLD, false);
		}

		// 이미지 뷰를 갱신 // Update image view
		viewImageSrc.Invalidate(true);
		viewImageSrcMask.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() && viewImageSrcMask.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}