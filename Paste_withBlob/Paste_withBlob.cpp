#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


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

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// Source 이미지 로드 // Load the source image
		if(IsFail(res = arrFliImage[EType_Source].Load(L"../../ExampleImages/Paste/ChessBoard.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Destination1 이미지 로드. // Load the destination1 image
		if(IsFail(res = arrFliImage[EType_Destination1].Load(L"../../ExampleImages/Paste/Floor.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Destination2 이미지를 Destination1 이미지와 동일한 이미지로 생성
		if(IsFail(res = arrFliImage[EType_Destination2].Assign(arrFliImage[EType_Destination1])))
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
		if(IsFail(res = arrViewImage[EType_Destination1].Create(612, 0, 1124, 512)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination2 이미지 뷰 생성 // Create destination2 image view
		if(IsFail(res = arrViewImage[EType_Destination2].Create(1124, 0, 1636, 512)))
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
		if(IsFail(res = arrViewImage[EType_Source].SynchronizePointOfView(&arrViewImage[EType_Destination1])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = arrViewImage[EType_Source].SynchronizePointOfView(&arrViewImage[EType_Destination2])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if(IsFail(res = arrViewImage[EType_Source].SynchronizeWindow(&arrViewImage[EType_Destination1])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if(IsFail(res = arrViewImage[EType_Source].SynchronizeWindow(&arrViewImage[EType_Destination2])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Blob 객체 생성 // Create Blob object
		CBlob blob;

		// 처리할 이미지 설정 // Set the image to process
		blob.SetSourceImage(arrFliImage[EType_Source]);
		// Threshold 모드 설정. 여기서는 2중 Threshold에 두개의 조건의 And 조건을 참으로 설정한다.
		// Threshold mode setting. Here, set the AND condition of the two conditions to true in the double threshold.
		blob.SetThresholdMode(EThresholdMode_Dual_And);
		// 논리 조건 설정 // Set logical condition
		blob.SetLogicalCondition(ELogicalCondition_Greater, ELogicalCondition_Less);
		// 임계값 설정,  위의 조건과 아래의 조건이 합쳐지면 130보다 크고 240보다 작은 객체를 검출
		// Set the threshold, when the above and below conditions are combined, objects greater than 130 and less than 240 are detected
		blob.SetThreshold(130, 240);

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


		// Paste 객체 생성 // Create Paste object
		CPaste paste;

		// Source 이미지 설정 // Set the source image
		paste.SetSourceImage(arrFliImage[EType_Source]);
		// Blob 결과인 flfaContours를 Src ROI로 설정 // Set blob result flfaContours as Src ROI
		paste.SetSourceROI(flfaContours);
		// Destination 이미지 설정 // Set the destination image
		paste.SetDestinationImage(arrFliImage[EType_Destination2]);
		// Destination Pivot 설정 // Set the destination pivot
		paste.SetDestinationPivot(flfaContours.GetCenter());
		// 항상 공백 영역을 지정한 색으로 채우도록 설정할 것인지 선택 // Choose whether to always fill blank areas with the specified color
		paste.EnableFillBlankColorMode(false);
		// 공백 영역 색상 지정 // Set the blank color
		paste.SetBlankColor(CMultiVar<double>(0., 0., 0.));

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = paste.Execute()))
		{
			ErrorPrint(res, "Failed to execute paste.");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if(IsFail(res = arrViewImage[EType_Destination2].ZoomFit()))
		{
			ErrorPrint(res, "Failed to zoom fit of the image view.\n");
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
		}

		// ROI영역이 어디인지 알기 위해 디스플레이 한다 // Display to find out where ROI is
		// FLImaging의 Figure 객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능 // FLimaging's Figure objects can be displayed as a function regardless of the shape
		// 아래 함수 DrawFigureImage는 Image좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
		// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일경우 불투명, 0일경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
		// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)
		if(IsFail(res = arrLayer[EType_Source].DrawFigureImage(&flfaContours, LIME)))
			ErrorPrint(res, "Failed to draw figure\n");

		if(IsFail(res = arrLayer[EType_Destination2].DrawFigureImage(&flfaContours, LIME)))
			ErrorPrint(res, "Failed to draw figure\n");


		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = arrLayer[EType_Source].DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_Destination1].DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_Destination2].DrawTextCanvas(CFLPoint<double>(0, 0), L"Result Image", YELLOW, BLACK, 20)))
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

			arrLayer[EType_Source].DrawTextImage(&CFLPoint<double>(flr.left, (flr.top + flr.bottom) * .5), (wchar_t*)strNumber.GetString(), BLACK, YELLOW, 12, false, 0, EGUIViewImageTextAlignment_CENTER, nullptr, 1.f, 1.f, EGUIViewImageFontWeight_BOLD, false);
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