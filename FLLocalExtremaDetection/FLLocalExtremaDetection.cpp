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

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// 이미지 로드 // Load image
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/FLLocalExtremaDetection/FLLocalExtremaDetection.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage.Create(400, 0, 1168, 540)))
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

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer = viewImage.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer.Clear();

		// 객체 생성 // Create object
		CFLLocalExtremaDetection flLocalExtremaDetection;

		// ROI 범위 설정
		CFLRect<int32_t> flrROI(100, 50, 450, 450);

		// 처리할 이미지 설정
		if(IsFail(res = flLocalExtremaDetection.SetSourceImage(fliImage)))
		{
			ErrorPrint(res, "Failed to set Source Image.");
			break;
		}

		// 처리할 ROI 설정
		if(IsFail(res = flLocalExtremaDetection.SetSourceROI(flrROI)))
		{
			ErrorPrint(res, "Failed to set Source ROI.");
			break;
		}

		// 검출을 위한 Suppression Radius 설정
		if(IsFail(res = flLocalExtremaDetection.SetSuppressionRadius(11)))
		{
			ErrorPrint(res, "Failed to set Suppression Radius.");
			break;
		}

		// Reference Surface Check 설정
		if(IsFail(res = flLocalExtremaDetection.EnableReferenceSurfaceCheck(true)))
		{
			ErrorPrint(res, "Failed to set Reference Surface Check.");
			break;
		}

		// 알고리즘 실행
		if(IsFail(res = flLocalExtremaDetection.Execute()))
		{
			ErrorPrint(res, "Failed to execute.");
			break;
		}

		// 실행 결과를 받아오기 위한 컨테이너
		Foundation::CFLFigureArray flfaResultPoints;

		// 검출 결과 받아오기
		if(IsFail(res = flLocalExtremaDetection.GetResultPoints(flfaResultPoints)))
		{
			ErrorPrint(res, "Failed to get result.");
			break;
		}

		// 검출된 점을 출력
		if(flfaResultPoints.GetCount() > 0)
			layer.DrawFigureImage(flfaResultPoints, RED, 2);


		// ROI영역이 어디인지 알기 위해 디스플레이 한다 // Display to find out where ROI is
		// FLImaging의 Figure객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능
		if(IsFail(res = layer.DrawFigureImage(&flrROI, BLUE)))
		{
			ErrorPrint(res, "Failed to draw figures objects on the image view.\n");
			break;
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