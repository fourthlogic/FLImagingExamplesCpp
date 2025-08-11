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
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/HarrisCornerDetector/Chip.flif")))
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

		// harris 객체 생성 // Create harris object
		CHarrisCornerDetector harrisCornerDetector;

		// ROI 범위 설정
		CFLRect<int32_t> flrROI(100, 50, 450, 450);

		// 처리할 이미지 설정
		if(IsFail(res = harrisCornerDetector.SetSourceImage(fliImage)))
		{
			ErrorPrint(res, "Failed to set Source Image.");
			break;
		}

		// 처리할 ROI 설정
		if(IsFail(res = harrisCornerDetector.SetSourceROI(flrROI)))
		{
			ErrorPrint(res, "Failed to set Source ROI.");
			break;
		}

		// 코너를 검출하는 이미지의 Scale 값을 설정
		if(IsFail(res = harrisCornerDetector.SetScale(1.0)))
		{
			ErrorPrint(res, "Failed to set scale.");
			break;
		}

		// 검출할 최대 점의 개수를 설정
		if(IsFail(res = harrisCornerDetector.SetMaxPoints(500)))
		{
			ErrorPrint(res, "Failed to set max points.");
			break;
		}

		// 검출할 점수의 임계값을 설정
		if(IsFail(res = harrisCornerDetector.SetScoreThreshold(0.8f)))
		{
			ErrorPrint(res, "Failed to set score threshold.");
			break;
		}

		// 해리스 코너 디텍터의 파리미터 K를 설정
		if(IsFail(res = harrisCornerDetector.SetParamK(0.04f)))
		{
			ErrorPrint(res, "Failed to set param K.");
			break;
		}

		// 해리스 코너 디텍터 실행 함수
		if(IsFail(res = harrisCornerDetector.Execute()))
		{
			ErrorPrint(res, "Failed to execute.");
			break;
		}

		// 실행 결과를 받아오기 위한 컨테이너
		Foundation::CFLFigureArray flfaResultPoints;

		// 해리스 코너 디텍터 실행 함수
		if(IsFail(res = harrisCornerDetector.GetResultPoints(&flfaResultPoints)))
		{
			ErrorPrint(res, "Failed to get result.");
			break;
		}

		// 검출된 점의 개수를 가져오는 함수
		int64_t i64Count = harrisCornerDetector.GetResultCount();

		for(int64_t i = 0; i < i64Count; i++)
		{
			// 검출된 점을 출력
			CFLPoint<double>* pFlpTemp = dynamic_cast<CFLPoint<double>*>(flfaResultPoints.GetAt(i));
			layer.DrawFigureImage(pFlpTemp, RED, 1);
		}

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