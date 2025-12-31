#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliFindImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageLearn;
	CGUIViewImageWrap viewImageFind;

	CResult res = EResult_UnknownError;
	CFLFigureArray flfLearnedDrawing;

	do
	{
		if(IsFail(res = fliFindImage.Load(L"../../ExampleImages/Matching/DrawingImage.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Creates imageview
		if(IsFail(res = viewImageLearn.Create(400, 0, 912, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageFind.Create(912, 0, 1680, 576)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImageFind.SetImagePtr(&fliFindImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		CGUIViewImageLayerWrap layerLearn = viewImageLearn.GetLayer(0);
		CGUIViewImageLayerWrap layerFind = viewImageFind.GetLayer(1);

		layerLearn.Clear();
		layerFind.Clear();

		if(IsFail(res = layerLearn.DrawTextCanvas(CFLPoint<double>(0, 0), L"LEARN", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerFind.DrawTextCanvas(CFLPoint<double>(0, 0), L"FIND", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// Geometric Match 객체 생성 // Create Geometric Match object
		CDrawingMatch drawingMatch;

		// 학습할 이미지 설정 // Set the image to learn
		// 학습할 도면을 설정합니다. // Set the drawing to learn.
		drawingMatch.SetDrawing(L"../../ExampleImages/Matching/Drawing.gbr");
		// 도면에 대한 분해능 단위를 설정합니다. // Set unit of pixel accuracy.
		drawingMatch.SetDistanceUnit(CDrawingMatch::EDistanceUnit_Millimeter);
		// 도면에 대한 분해능을 설정합니다. // Set  pixel accuracy.
		drawingMatch.SetPixelAccuracy(0.1, 0.1);

		// 학습 파라미터를 설정합니다. // Set the learning parameters.
		// 추출할 특징점 개수를 설정합니다. // Set the number of feature points to be extracted.
		drawingMatch.SetFeatureCount();
		// 추출할 특징점 처리과정에서의 노이즈 필터링 정도를 설정합니다. // Set the noise filtering degree in the process of processing the feature points to be extracted.
		drawingMatch.SetFeatureFiltering();
		// 추출할 특징점 처리과정에서의 허용 임계값을 설정합니다. // Set the allowable threshold in the feature point processing process to be extracted.
		drawingMatch.SetLearnThresholdCoefficient();

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = drawingMatch.Learn()))
		{
			ErrorPrint(res, "Failed to execute Learn.\n");
			break;
		}

		// 학습한 특징점을 디스플레이 한다 // Display the learned feature point
		// 학습한 특징점이 찾고자하는 객체를 나타내기에 충분하게 잘 뽑혔는지 확인하고, 그렇지 않다면 학습 파라미터를 재조정함으로써 재확인하면 검출 시 더 효과적입니다. // Check whether the learned feature points are selected well enough to represent the object to be found.
		flfLearnedDrawing = drawingMatch.GetLearnedDrawing();

		if(IsFail(res = layerLearn.DrawFigureImage(flfLearnedDrawing, BLUE)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}


		// 검출할 이미지 설정 // Set image to detect
		drawingMatch.SetSourceImage(fliFindImage);

		// 검출 시 사용될 파라미터를 설정합니다. // Set the parameters to be used for detection.
		// 검출 시 사용될 기본 각도를 설정합니다. // Set the default angle to be used for detection.
		drawingMatch.SetAngleBias(0.);
		// 검출 시 사용될 각도의 탐색범위를 설정합니다. // Set the search range of the angle to be used for detection.
		// 각도는 기본 각도를 기준으로 (기본 각도 - AngleTolerance, 기본 각도 + AngleTolerance)가 최종 탐색범위 // The angle is based on the basic angle (default angle - AngleTolerance, basic angle + AngleTolerance) is the final search range
		drawingMatch.SetAngleTolerance(5.);
		// 검출 시 사용될 스케일 탐색범위를 설정합니다. // Set the scale search range to be used for detection.
		drawingMatch.SetScaleRange(0.9, 1.1);
		// 검출 시 사용될 최소 탐색점수를 설정합니다. // Set the minimum search score to be used for detection.
		drawingMatch.SetMinimumDetectionScore(0.5);
		// 검출 시 사용될 최대 탐색객체 수를 설정합니다. // Set the maximum number of search objects to be used for detection.
		drawingMatch.SetMaxObject(1);

		// 검출 시 보간법 사용 유무에 대해 설정합니다. // Set whether to use interpolation when detecting.
		drawingMatch.EnableInterpolation();
		// 검출 시 최적화 정도에 대해 설정합니다. // Set the degree of optimization for detection.
		drawingMatch.SetOptimizationOption();
		// 검출 시 대비정도에 대해 설정합니다. // Set the contrast level for detection.
		drawingMatch.SetContrastOption();
		// 검출 시 이미지 영역밖의 탐색 정도를 설정합니다. // Set the degree of search outside the image area when detecting.
		drawingMatch.SetContrastOption();
		// 검출 시 처리과정에서의 허용 임계값을 설정합니다. // Set the allowable threshold in the process of detection.
		drawingMatch.SetFindThresholdCoefficient();
		// 검출 시 겹쳐짐 허용 정도를 설정합니다. // Set the allowable degree of overlap during detection.
		drawingMatch.SetObjectOverlap();
		// 검출 시 이미지 전처리 유무를 설정합니다. // Set whether or not to pre-process the image during detection.

		// 알고리즘 수행 // Execute the algorithm
		res = drawingMatch.Execute();

		if(IsFail(res))
		{
			ErrorPrint(res, "Failed to Execute\n");
			break;
		}


		// 기하학적 패턴 검출 결과를 가져옵니다. // Get the geometric pattern detection result.
		std::vector<CDrawingMatch::SResult> vctResults;
		int64_t i64ResultCount = drawingMatch.GetResultCount();

		printf(" ▶ Find Information\n");

		for(int64_t i = 0; i < i64ResultCount; ++i)
		{
			CDrawingMatch::SResult results;
			drawingMatch.GetResult(i, results);

			float f32Score = results.f32Score;
			float f32Angle = results.f32Angle;
			float f32Scale = results.f32Scale;
			Foundation::CFLFigure* pFlfRegion = results.pFlfRegion;
			Foundation::CFLPoint<double> flpLocation = results.flpLocation;
			Foundation::CFLPoint<double> flpPivot = results.flpPivot;

			CFLString<wchar_t> strResult;

			CFLRect<double> flrResultRegion = pFlfRegion->GetBoundaryRect();

			// 기하학적 패턴 검출 결과를 Console창에 출력합니다. // Output the geometric pattern detection result to the console window.
			printf(" < Instance : %lld >\n", i);
			printf("  1. ROI Shape Type : Rectangle\n");
			printf("    left   : %.3lf\n", flrResultRegion.left);
			printf("    right  : %.3lf\n", flrResultRegion.right);
			printf("    top    : %.3lf\n", flrResultRegion.top);
			printf("    bottom : %.3lf\n", flrResultRegion.bottom);
			printf("    angle  : %.3lf\n", f32Angle);
			printf("  2. Interest Pivot : (%.3lf, %.3lf)\n", flpPivot.x, flpPivot.y);
			printf("  3. Score : %0.3lf\n  4. Angle : %0.3lf\n  5. Scale : %0.3lf\n", f32Score, f32Angle, f32Scale);

			printf("\n");

			// 검출 결과의 중심점을 디스플레이 한다 // Display the center point of the detection result
			CFLFigureArray flfaPoint = flpPivot.MakeCrossHair(3, false);
			flfaPoint.Rotate(f32Angle, &flpPivot);

			if(IsFail(res = layerFind.DrawFigureImage(&flfaPoint, BLACK, 3)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			if(IsFail(res = layerFind.DrawFigureImage(&flfaPoint, LIME)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			if(IsFail(res = layerFind.DrawFigureImage(pFlfRegion, BLACK, 3)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			if(IsFail(res = layerFind.DrawFigureImage(pFlfRegion, CYAN)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			TPoint<double> tpPosition;
			tpPosition.x = flpPivot.x;
			tpPosition.y = flpPivot.y;

			CFLString<wchar_t> strText;
			strText.Format(L"Score : %.3lf\nAngle : %.3lf\nScale : x%.3lf\n", f32Score, f32Angle, f32Scale);

			if((res = layerFind.DrawTextImage(tpPosition, strText, YELLOW, BLACK, 15, false, 0, EGUIViewImageTextAlignment_LEFT_CENTER)).IsFail())
			{
				ErrorPrint(res, "Failed to draw text");
				break;
			}
		}

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageLearn.ZoomFitToLayer(0);
		viewImageLearn.Invalidate(true);
		viewImageFind.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageLearn.IsAvailable() && viewImageFind.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}