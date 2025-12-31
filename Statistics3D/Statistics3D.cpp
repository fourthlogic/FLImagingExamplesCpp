#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 객체 선언 // Declare 3D object
	CFL3DObject floSource;

	// 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap view3DSrc;

	do
	{
		// 수행 결과 객체 선언 // Declare execution result object
		CResult res = EResult_UnknownError;

		// Source 3D Object 로드 // Load Source 3D Object
		if((res = floSource.Load(L"../../ExampleImages/Statistics3D/Sphere.ply")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the 3D object file.\n");
			break;
		}

		// Source 3D 뷰 생성 // Create Source 3D view
		if((res = view3DSrc.Create(100, 0, 612, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the 3D view.\n");
			break;
		}

		// Statistics 3D 객체 생성 // Create Statistics 3D object
		CStatistics3D statistics3D;

		// Source 3D Object 설정 // Set Source 3D Object
		if((res = statistics3D.SetSourceObject(floSource)).IsFail())
		{
			ErrorPrint(res, L"Failed to set Source 3D Object.\n");
			break;
		}

		// 사전 계산 여부 설정 // Set pre calculated value hold flag
		if((res = statistics3D.EnablePreCalculatedHold(true)).IsFail())
		{
			ErrorPrint(res, L"Failed to set pre calculated flag.\n");
			break;
		}

		// 위치 데이터 불러오기 // Get position data
		TPoint3<double> tpPositionMin;
		TPoint3<double> tpPositionMax;
		TPoint3<double> tpPositionSum;
		TPoint3<double> tpPositionSumOfSquares;
		TPoint3<double> tpPositionMean;
		TPoint3<double> tpPositionMedian;
		TPoint3<double> tpPositionVariance;
		TPoint3<double> tpPositionStandardDeviation;
		TPoint3<double> tpPositionCoefficientOfVariance;
		TPoint3<double> tpPositionLowerQuartile;
		TPoint3<double> tpPositionUpperQuartile;

		double f64PositionCovarianceXY;
		double f64PositionCovarianceXZ;
		double f64PositionCovarianceYZ;

		double f64PositionCorrelationCoefficientXY;
		double f64PositionCorrelationCoefficientXZ;
		double f64PositionCorrelationCoefficientYZ;

		if((res = statistics3D.GetPointPositionMin(tpPositionMin)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's min");
			break;
		}

		if((res = statistics3D.GetPointPositionMax(tpPositionMax)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's max");
			break;
		}

		if((res = statistics3D.GetPointPositionSum(tpPositionSum)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's sum");
			break;
		}

		if((res = statistics3D.GetPointPositionSumOfSquares(tpPositionSumOfSquares)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's sum of squares");
			break;
		}

		if((res = statistics3D.GetPointPositionMean(tpPositionMean)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's mean");
			break;
		}

		if((res = statistics3D.GetPointPositionMedian(tpPositionMedian)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's median");
			break;
		}

		if((res = statistics3D.GetPointPositionVariance(tpPositionVariance)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's variance");
			break;
		}

		if((res = statistics3D.GetPointPositionStandardDeviation(tpPositionStandardDeviation)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's standard deviation");
			break;
		}

		if((res = statistics3D.GetPointPositionCoefficientOfVariance(tpPositionCoefficientOfVariance)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's coefficient of variance");
			break;
		}

		if((res = statistics3D.GetPointPositionLowerQuartile(tpPositionLowerQuartile)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's lower quartile");
			break;
		}

		if((res = statistics3D.GetPointPositionUpperQuartile(tpPositionUpperQuartile)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's upper quartile");
			break;
		}

		statistics3D.SetCorrelatedPointPosition(CStatistics3D::EPointPosition_X, CStatistics3D::EPointPosition_Y);

		if((res = statistics3D.GetPointPositionCovariance(f64PositionCovarianceXY)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's covariance");
			break;
		}

		statistics3D.SetCorrelatedPointPosition(CStatistics3D::EPointPosition_X, CStatistics3D::EPointPosition_Z);

		if((res = statistics3D.GetPointPositionCovariance(f64PositionCovarianceXZ)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's covariance");
			break;
		}

		statistics3D.SetCorrelatedPointPosition(CStatistics3D::EPointPosition_Y, CStatistics3D::EPointPosition_Z);

		if((res = statistics3D.GetPointPositionCovariance(f64PositionCovarianceYZ)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's covariance");
			break;
		}

		statistics3D.SetCorrelatedPointPosition(CStatistics3D::EPointPosition_X, CStatistics3D::EPointPosition_Y);

		if((res = statistics3D.GetPointPositionCorrelationCoefficient(f64PositionCorrelationCoefficientXY)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's correlation coefficient");
			break;
		}

		statistics3D.SetCorrelatedPointPosition(CStatistics3D::EPointPosition_X, CStatistics3D::EPointPosition_Z);

		if((res = statistics3D.GetPointPositionCorrelationCoefficient(f64PositionCorrelationCoefficientXZ)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's correlation coefficient");
			break;
		}

		statistics3D.SetCorrelatedPointPosition(CStatistics3D::EPointPosition_Y, CStatistics3D::EPointPosition_Z);

		if((res = statistics3D.GetPointPositionCorrelationCoefficient(f64PositionCorrelationCoefficientYZ)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point position's correlation coefficient");
			break;
		}

		// 색 데이터 불러오기 // Get color data
		CMultiVar<double> mvColorMin;
		CMultiVar<double> mvColorMax;
		CMultiVar<double> mvColorSum;
		CMultiVar<double> mvColorSumOfSquares;
		CMultiVar<double> mvColorMean;
		CMultiVar<double> mvColorMedian;
		CMultiVar<double> mvColorVariance;
		CMultiVar<double> mvColorStandardDeviation;
		CMultiVar<double> mvColorCoefficientOfVariance;
		CMultiVar<double> mvColorLowerQuartile;
		CMultiVar<double> mvColorUpperQuartile;

		double f64ColorCovarianceBG;
		double f64ColorCovarianceBR;
		double f64ColorCovarianceGR;

		double f64ColorCorrelationCoefficientBG;
		double f64ColorCorrelationCoefficientBR;
		double f64ColorCorrelationCoefficientGR;

		if((res = statistics3D.GetPointColorMin(mvColorMin)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's min");
			break;
		}

		if((res = statistics3D.GetPointColorMax(mvColorMax)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's max");
			break;
		}

		if((res = statistics3D.GetPointColorSum(mvColorSum)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's sum");
			break;
		}

		if((res = statistics3D.GetPointColorSumOfSquares(mvColorSumOfSquares)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's sum of squares");
			break;
		}

		if((res = statistics3D.GetPointColorMean(mvColorMean)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's mean");
			break;
		}

		if((res = statistics3D.GetPointColorMedian(mvColorMedian)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's median");
			break;
		}

		if((res = statistics3D.GetPointColorVariance(mvColorVariance)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's variance");
			break;
		}

		if((res = statistics3D.GetPointColorStandardDeviation(mvColorStandardDeviation)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's standard deviation");
			break;
		}

		if((res = statistics3D.GetPointColorCoefficientOfVariance(mvColorCoefficientOfVariance)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's coefficient of variance");
			break;
		}

		if((res = statistics3D.GetPointColorLowerQuartile(mvColorLowerQuartile)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's lower quartile");
			break;
		}

		if((res = statistics3D.GetPointColorUpperQuartile(mvColorUpperQuartile)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's upper quartile");
			break;
		}

		statistics3D.SetCorrelatedPointColor(CStatistics3D::EPointColor_B, CStatistics3D::EPointColor_G);

		if((res = statistics3D.GetPointColorCovariance(f64ColorCovarianceBG)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's covariance");
			break;
		}

		statistics3D.SetCorrelatedPointColor(CStatistics3D::EPointColor_B, CStatistics3D::EPointColor_R);

		if((res = statistics3D.GetPointColorCovariance(f64ColorCovarianceBR)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's covariance");
			break;
		}

		statistics3D.SetCorrelatedPointColor(CStatistics3D::EPointColor_G, CStatistics3D::EPointColor_R);

		if((res = statistics3D.GetPointColorCovariance(f64ColorCovarianceGR)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's covariance");
			break;
		}

		statistics3D.SetCorrelatedPointColor(CStatistics3D::EPointColor_B, CStatistics3D::EPointColor_G);

		if((res = statistics3D.GetPointColorCorrelationCoefficient(f64ColorCorrelationCoefficientBG)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's correlation coefficient");
			break;
		}

		statistics3D.SetCorrelatedPointColor(CStatistics3D::EPointColor_B, CStatistics3D::EPointColor_R);

		if((res = statistics3D.GetPointColorCorrelationCoefficient(f64ColorCorrelationCoefficientBR)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's correlation coefficient");
			break;
		}

		statistics3D.SetCorrelatedPointColor(CStatistics3D::EPointColor_G, CStatistics3D::EPointColor_R);

		if((res = statistics3D.GetPointColorCorrelationCoefficient(f64ColorCorrelationCoefficientGR)).IsFail())
		{
			ErrorPrint(res, L"Failed to get point color's correlation coefficient");
			break;
		}

		// 면 데이터 불러오기 // Get face data
		double f64SurfaceArea;

		if((res = statistics3D.GetSurfaceArea(f64SurfaceArea)).IsFail())
		{
			ErrorPrint(res, "Failed to get face's surface area");
			break;
		}

		// 콘솔에 데이터 출력 // Print data to console
		printf(" < Point Position Data >\n");

		printf("Min ->\tX: %0.7lf\tY: %0.7lf\tZ: %0.7lf\n", tpPositionMin.x, tpPositionMin.y, tpPositionMin.z);
		printf("Max ->\tX: %0.7lf\tY: %0.7lf\tZ: %0.7lf\n", tpPositionMax.x, tpPositionMax.y, tpPositionMax.z);
		printf("Sum ->\tX: %0.7lf\tY: %0.7lf\tZ: %0.7lf\n", tpPositionSum.x, tpPositionSum.y, tpPositionSum.z);
		printf("Sum Of Squares ->\tX: %0.7lf\tY: %0.7lf\tZ: %0.7lf\n", tpPositionSumOfSquares.x, tpPositionSumOfSquares.y, tpPositionSumOfSquares.z);
		printf("Mean ->\tX: %0.7lf\tY: %0.7lf\tZ: %0.7lf\n", tpPositionMean.x, tpPositionMean.y, tpPositionMean.z);
		printf("Median ->\tX: %0.7lf\tY: %0.7lf\tZ: %0.7lf\n", tpPositionMedian.x, tpPositionMedian.y, tpPositionMedian.z);
		printf("Variance ->\tX: %0.7lf\tY: %0.7lf\tZ: %0.7lf\n", tpPositionVariance.x, tpPositionVariance.y, tpPositionVariance.z);
		printf("Standard Deviation ->\tX: %0.7lf\tY: %0.7lf\tZ: %0.7lf\n", tpPositionStandardDeviation.x, tpPositionStandardDeviation.y, tpPositionStandardDeviation.z);
		printf("Coefficient Of Variance ->\tX: %0.7lf\tY: %0.7lf\tZ: %0.7lf\n", tpPositionCoefficientOfVariance.x, tpPositionCoefficientOfVariance.y, tpPositionCoefficientOfVariance.z);
		printf("Lower Quartile ->\tX: %0.7lf\tY: %0.7lf\tZ: %0.7lf\n", tpPositionLowerQuartile.x, tpPositionLowerQuartile.y, tpPositionLowerQuartile.z);
		printf("Upper Quartile ->\tX: %0.7lf\tY: %0.7lf\tZ: %0.7lf\n", tpPositionUpperQuartile.x, tpPositionUpperQuartile.y, tpPositionUpperQuartile.z);

		printf("Covariance ->\tXY: %0.7lf\tXZ: %0.7lf\tYZ: %0.7lf\n", f64PositionCovarianceXY, f64PositionCovarianceXZ, f64PositionCovarianceYZ);
		printf("Correlation Coefficient ->\tXY: %0.7lf\tXZ: %0.7lf\tYZ: %0.7lf\n", f64PositionCorrelationCoefficientXY, f64PositionCorrelationCoefficientXZ, f64PositionCorrelationCoefficientYZ);

		printf("\n");

		printf(" < Point Color Data >\n");

		printf("Min ->\tB: %0.7lf\tG: %0.7lf\tR: %0.7lf\n", mvColorMin.GetAt(0), mvColorMin.GetAt(1), mvColorMin.GetAt(2));
		printf("Max ->\tB: %0.7lf\tG: %0.7lf\tR: %0.7lf\n", mvColorMax.GetAt(0), mvColorMax.GetAt(1), mvColorMax.GetAt(2));
		printf("Sum ->\tB: %0.7lf\tG: %0.7lf\tR: %0.7lf\n", mvColorSum.GetAt(0), mvColorSum.GetAt(1), mvColorSum.GetAt(2));
		printf("Sum Of Squares ->\tB: %0.7lf\tG: %0.7lf\tR: %0.7lf\n", mvColorSumOfSquares.GetAt(0), mvColorSumOfSquares.GetAt(1), mvColorSumOfSquares.GetAt(2));
		printf("Mean ->\tB: %0.7lf\tG: %0.7lf\tR: %0.7lf\n", mvColorMean.GetAt(0), mvColorMean.GetAt(1), mvColorMean.GetAt(2));
		printf("Median ->\tB: %0.7lf\tG: %0.7lf\tR: %0.7lf\n", mvColorMedian.GetAt(0), mvColorMedian.GetAt(1), mvColorMedian.GetAt(2));
		printf("Variance ->\tB: %0.7lf\tG: %0.7lf\tR: %0.7lf\n", mvColorVariance.GetAt(0), mvColorVariance.GetAt(1), mvColorVariance.GetAt(2));
		printf("Standard Deviation ->\tB: %0.7lf\tG: %0.7lf\tR: %0.7lf\n", mvColorStandardDeviation.GetAt(0), mvColorStandardDeviation.GetAt(1), mvColorStandardDeviation.GetAt(2));
		printf("Coefficient Of Variance ->\tB: %0.7lf\tG: %0.7lf\tR: %0.7lf\n", mvColorCoefficientOfVariance.GetAt(0), mvColorCoefficientOfVariance.GetAt(1), mvColorCoefficientOfVariance.GetAt(2));
		printf("Lower Quartile ->\tB: %0.7lf\tG: %0.7lf\tR: %0.7lf\n", mvColorLowerQuartile.GetAt(0), mvColorLowerQuartile.GetAt(1), mvColorLowerQuartile.GetAt(2));
		printf("Upper Quartile ->\tB: %0.7lf\tG: %0.7lf\tR: %0.7lf\n", mvColorUpperQuartile.GetAt(0), mvColorUpperQuartile.GetAt(1), mvColorUpperQuartile.GetAt(2));

		printf("Covariance ->\tBG: %0.7lf\tBR: %0.7lf\tGR: %0.7lf\n", f64ColorCovarianceBG, f64ColorCovarianceBR, f64ColorCovarianceGR);
		printf("Correlation Coefficient ->\tBG: %0.7lf\tBR: %0.7lf\tGR: %0.7lf\n", f64ColorCorrelationCoefficientBG, f64ColorCorrelationCoefficientBR, f64ColorCorrelationCoefficientGR);

		printf("\n");

		printf(" < Face Data >\n");
		
		printf("Surface Area: %0.7lf\n", f64SurfaceArea);

		printf("\n");


		// 화면에 출력하기 위해 3D 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 number from 3D view for display
		// 이 객체는 3D 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an 3D view and does not need to be released
		CGUIView3DLayerWrap layer3DSource = view3DSrc.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear figures drawn on existing layer
		layer3DSource.Clear();

		// 3D 뷰 정보 표시 // Display 3D view information
		if((res = layer3DSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source 3D Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 입력 3D 객체 출력 // Print input 3D Object
		if((res = view3DSrc.PushObject(floSource)).IsFail())
		{
			ErrorPrint(res, L"Failed to display the 3D Object.\n");
			break;
		}

		// 새로 생성한 3D Object를 가지는 뷰 Zoom Fit 실행 // Activate Zoom Fit for view with newly created 3D object
		if((res = view3DSrc.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit 3D view.\n");
			break;
		}

		// 3D 뷰를 갱신 // Update 3D view
		view3DSrc.Invalidate(true);

		// 뷰가 닫히기 전까지 종료하지 않고 대기 // Wait until a view is closed before exiting
		while(view3DSrc.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}