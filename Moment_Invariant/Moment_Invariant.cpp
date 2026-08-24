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
		if(IsFail(res = viewImage.Create(400, 0, 1424, 768)))
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
		// 처리할 이미지의 이진화 이미지로 판단 유무 설정 // Set whether to judge the image to be processed as a binarized image
		// 이진화 이미지로 판단할 경우 0이 아닌 모든 화소값은 1로 처리함 // When judging as a binarized image, all non-zero pixel values are treated as 1.
		moment.EnableBinaryImage(true);

		bool bCalcGeometricMoment = true;
		bool bCalcCentroidMoment = true;
		bool bCalcCentralMoment = true;
		bool bCalcNormalizedCentralMoment= true;
		bool bCalcHuMoment= true;

		// 계산 대상에 기하학적 모멘트를 포함합니다. // Include the geometric moment in the computed object.
		moment.EnableGeometricMoment(bCalcGeometricMoment);
		// 계산 대상에 도심 모멘트를 포함합니다. // Include the centroid moment in the calculation target.
		moment.EnableCentroidMoment(bCalcCentroidMoment);
		// 계산 대상에 중심 모멘트를 포함합니다. // Include the central moment in the calculation target.
		moment.EnableCentralMoment(bCalcCentralMoment);
		// 계산 대상에 정규화된 중심 모멘트를 포함합니다. // Include the normalized central moment in the computed target.
		moment.EnableNormalizedCentralMoment(bCalcNormalizedCentralMoment);
		// 계산 대상에 휴(불변) 모멘트를 포함합니다. // Include the idle (invariant) moment in the calculation target.
		moment.EnableHuMoment(bCalcHuMoment);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = moment.Execute()))
		{
			ErrorPrint(res, "Failed to execute moment.\n");
			break;
		}

		// 모멘트 결과들을 가져옵니다. // Get the moment results.
		CMoment::SMoments results;
		moment.GetMoment(results);

		// 모멘트 결과를 Console 창에 출력 // Output the moment result to the console window
		if(bCalcGeometricMoment)
		{
			printf("< Geometric Moment > \n");
			printf(" Moment 00 : %lf\n", results.sGeometricMoments.f64GeometricM00);
			printf(" Moment 10 : %lf\n", results.sGeometricMoments.f64GeometricM10);
			printf(" Moment 01 : %lf\n", results.sGeometricMoments.f64GeometricM01);
			printf(" Moment 20 : %lf\n", results.sGeometricMoments.f64GeometricM20);
			printf(" Moment 11 : %lf\n", results.sGeometricMoments.f64GeometricM11);
			printf(" Moment 02 : %lf\n", results.sGeometricMoments.f64GeometricM02);
			printf(" Moment 30 : %lf\n", results.sGeometricMoments.f64GeometricM30);
			printf(" Moment 21 : %lf\n", results.sGeometricMoments.f64GeometricM21);
			printf(" Moment 12 : %lf\n", results.sGeometricMoments.f64GeometricM12);
			printf(" Moment 03 : %lf\n", results.sGeometricMoments.f64GeometricM03);
			printf("\n");
		}

		if(bCalcCentroidMoment)
		{
			printf("< Centroid Moment > \n");
			printf(" Moment Centroid X : %lf\n", results.sCentroidMoment.f64CentroidX);
			printf(" Moment Centroid Y : %lf\n", results.sCentroidMoment.f64CentroidY);
			printf("\n");
		}

		if(bCalcCentralMoment)
		{
			printf("< Central Moment > \n");
			printf(" Moment 00 : %lf\n", results.sCentralMoments.f64CentralM00);
			printf(" Moment 10 : %lf\n", results.sCentralMoments.f64CentralM10);
			printf(" Moment 01 : %lf\n", results.sCentralMoments.f64CentralM01);
			printf(" Moment 20 : %lf\n", results.sCentralMoments.f64CentralM20);
			printf(" Moment 11 : %lf\n", results.sCentralMoments.f64CentralM11);
			printf(" Moment 02 : %lf\n", results.sCentralMoments.f64CentralM02);
			printf(" Moment 30 : %lf\n", results.sCentralMoments.f64CentralM30);
			printf(" Moment 21 : %lf\n", results.sCentralMoments.f64CentralM21);
			printf(" Moment 12 : %lf\n", results.sCentralMoments.f64CentralM12);
			printf(" Moment 03 : %lf\n", results.sCentralMoments.f64CentralM03);
			printf("\n");
		}

		if(bCalcNormalizedCentralMoment)
		{
			printf("< Normalized Central Moment > \n");
			printf(" Moment 00 : %lf\n", results.sNormalizedCentralMoments.f64NormalizedCentralM00);
			printf(" Moment 10 : %lf\n", results.sNormalizedCentralMoments.f64NormalizedCentralM10);
			printf(" Moment 01 : %lf\n", results.sNormalizedCentralMoments.f64NormalizedCentralM01);
			printf(" Moment 20 : %lf\n", results.sNormalizedCentralMoments.f64NormalizedCentralM20);
			printf(" Moment 11 : %lf\n", results.sNormalizedCentralMoments.f64NormalizedCentralM11);
			printf(" Moment 02 : %lf\n", results.sNormalizedCentralMoments.f64NormalizedCentralM02);
			printf(" Moment 30 : %lf\n", results.sNormalizedCentralMoments.f64NormalizedCentralM30);
			printf(" Moment 21 : %lf\n", results.sNormalizedCentralMoments.f64NormalizedCentralM21);
			printf(" Moment 12 : %lf\n", results.sNormalizedCentralMoments.f64NormalizedCentralM12);
			printf(" Moment 03 : %lf\n", results.sNormalizedCentralMoments.f64NormalizedCentralM03);
			printf("\n");
		}

		if(bCalcHuMoment)
		{
			printf("< Hu Moment > \n");
			printf(" Hu 0 : %lf\n", results.sHuMoments.f64Hu0);
			printf(" Hu 1 : %lf\n", results.sHuMoments.f64Hu1);
			printf(" Hu 2 : %lf\n", results.sHuMoments.f64Hu2);
			printf(" Hu 3 : %lf\n", results.sHuMoments.f64Hu3);
			printf(" Hu 4 : %lf\n", results.sHuMoments.f64Hu4);
			printf(" Hu 5 : %lf\n", results.sHuMoments.f64Hu5);
			printf(" Hu 6 : %lf\n", results.sHuMoments.f64Hu6);
			printf("\n");
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