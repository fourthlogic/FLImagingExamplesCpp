#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliImageSrc;
	CFLImage fliImageDst;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageDst;

	do
	{
		CResult res = EResult_UnknownError;

		// 이미지 로드 // Load image
		if(IsFail(res = fliImageSrc.Load(L"../../ExampleImages/Mura/Region.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImageSrc.Create(100, 0, 548, 448)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		if(IsFail(res = viewImageSrc.SetImagePtr(&fliImageSrc)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImageDst.Create(548, 0, 996, 448)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		if(IsFail(res = viewImageDst.SetImagePtr(&fliImageDst)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
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
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// Mura 객체 생성 // Create Mura object
		CMura mura;

		// 처리할 이미지 설정
		mura.SetSourceImage(fliImageSrc);
		// Auto Threshold 모드 설정 // Set auto threshold mode
		mura.EnableAutoThresholdMode(false);
		// Kernel Size Rate 설정 // Set kernel size rate
		mura.SetKernelSizeRate(1);
		// Mura Color Type 설정 // Set mura color type
		mura.SetMuraColorType(CMura::EMuraColorType_BlackOnWhite);
		// 논리 조건 설정
		mura.SetLogicalCondition(ELogicalCondition_GreaterEqual);

		// 임계값 설정, 위의 조건과 아래의 조건에 따라 같거나 큰 객체를 검출
		mura.SetThreshold(CMultiVar<double>(10, 9, 9));

		// 채널 Logical Condition 설정 // Set Logical condition of channels
		mura.SetLogicalConditionOfChannels(CBlob::ELogicalConditionOfChannels_Or);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = mura.Execute()))
		{
			ErrorPrint(res, "Failed to execute Mura.");
			break;
		}

		mura.GetResultMuraImage(fliImageDst);

		// Mura 결과를 얻어오기 위해 FigureArray 선언
		CFLFigureArray flfaContours;

		mura.SetContourResultType(CBlob::EContourResultType_Perforated);

		// Mura 결과들 중 Contour를 얻어옴
		if(IsFail(res = mura.GetResultContours(flfaContours)))
		{
			ErrorPrint(res, "Failed to get boundary rects from the Mura object.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer = viewImageDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer.Clear();


		// flfaContours 는 Figure들의 배열이기 때문에 Layer에 넣기만 해도 모두 드로윙이 가능하다.
		// 아래 함수 DrawFigureImage는 Image좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
		// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일경우 불투명, 0일경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
		// 여기서 0.25이므로 옅은 반투명 상태라고 볼 수 있다.
		// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)
		if(IsFail(res = layer.DrawFigureImage(&flfaContours, RED, 1, RED, EGUIViewImagePenStyle_Solid, 1, 0.25)))
		{
			ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc.Invalidate(true);
		viewImageDst.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}