#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImage;
	CResult res;

	do
	{ 
		// 이미지 로드 // Load image
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/Moment/airEdge.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage.Create(600, 0, 1424, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if(IsFail(res = viewImage.SetImagePtr(&fliImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		CResult res = EResult_UnknownError;

		// Moment 객체 생성 // Create Moment object
		CMoment moment;

		// ROI 범위 설정 // Set ROI range
		CFLRect<int32_t> flrROI(15, 150, 420, 280);

		// 처리할 이미지 설정 // Set the image to process
		moment.SetSourceImage(fliImage);
		// 처리할 ROI 설정 // Set the ROI to be processed
		moment.SetSourceROI(flrROI);
		// 처리할 이미지의 이진화이미지로 판단 유무 설정 // Set whether to judge the image to be processed as a binarized image
		moment.EnableBinaryImage(true);

		// 계산 대상에 저니키 모멘트 N, M 파라미터를 추가합니다. // Add Zernike Moment N, M parameters to the calculation target.
 		moment.AddZernike(1, -1);
 		moment.AddZernike(1, 1);
 		moment.AddZernike(3, -3);
 		moment.AddZernike(3, -1);
 		moment.AddZernike(3, 1);
 		moment.AddZernike(3, 3);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = moment.Execute()))
		{
			ErrorPrint(res, "Failed to execute moment.\n");
			break;
		}

		// 모멘트 결과들을 가져옵니다. // Get the moment results.
		CMoment::SZernike zernike;
		int64_t i64ZernikeCount = (int64_t)moment.GetZernikeCount();

		for(int i = 0; i < i64ZernikeCount; ++i)
		{
			moment.GetZernike(&zernike, i);
			printf("Zernike N = %d, M = %d, RealValue : %.4lf, Imaginary : %.4lf\n", zernike.i32N, zernike.i32M, zernike.f64ZernikeReal, zernike.f64ZernikeImag);
		}
		
		CGUIViewImageLayerWrap layer = viewImage.GetLayer(0);

		layer.Clear();
		
		// ROI 영역이 어디인지 알기 위해 디스플레이 한다 // Display to know where the ROI area is
		if(IsFail(res = layer.DrawFigureImage(&flrROI, BLUE)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImage.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage.IsAvailable())
			CThreadUtilities::Sleep(1);

		viewImage.Destroy();
	}
	while(false);

	return 0;
}