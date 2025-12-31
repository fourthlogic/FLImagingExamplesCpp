#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliLearnImage;
	CFLImage fliFindImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageLearn;
	CGUIViewImageWrap viewImageFind;

	CResult res = EResult_UnknownError;

	do
	{
		// 이미지 로드 // Loads image
		if(IsFail(res = fliLearnImage.Load(L"../../ExampleImages/Matching/Pattern Single Learn.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliFindImage.Load(L"../../ExampleImages/Matching/Pattern Single Find.flif")))
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

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
		if(IsFail(res = viewImageLearn.SetImagePtr(&fliLearnImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		if(IsFail(res = viewImageFind.SetImagePtr(&fliFindImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageLearn.SynchronizeWindow(&viewImageFind)))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
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

		// Pattern Match 객체 생성 // Create Pattern Match object
		CPatternMatch patternMatch;

		// 학습할 이미지 설정 // Set the image to learn
		patternMatch.SetLearnImage(fliLearnImage);

		// 학습할 영역을 설정합니다. // Set the area to learn.
		CFLRect<double> learnRegion(174.7086, 272.2204, 799.0551, 601.3228);
		CFLPoint<double> flpLearnPivot = learnRegion.GetCenter();
		patternMatch.SetLearnROI(learnRegion);
		patternMatch.SetLearnPivot(flpLearnPivot);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = patternMatch.Learn()))
		{
			ErrorPrint(res, "Failed to execute Learn.\n");
			break;
		}

		// 측정 영역이 어디인지 알기 위해 디스플레이 한다 // Display to know where the measurement area is
		if(IsFail(res = layerLearn.DrawFigureImage(&learnRegion, BLACK, 3)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		if(IsFail(res = layerLearn.DrawFigureImage(&learnRegion, CYAN)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		// 설정된 중심점의 위치를 디스플레이 한다 // Display the position of the set center point
		CFLFigureArray flfaPointPivot = flpLearnPivot.MakeCrossHair(3, false);

		if(IsFail(res = layerLearn.DrawFigureImage(&flfaPointPivot, BLACK, 3)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		if(IsFail(res = layerLearn.DrawFigureImage(&flfaPointPivot, LIME)))
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		// 학습한 정보에 대해 Console창에 출력한다 // Print the learned information to the console window
		printf(" ▷ Learn Information\n");
		printf("  1. ROI Shape Type : Rectangle\n");
		printf("    left   : %.3lf\n", learnRegion.left);
		printf("    right  : %.3lf\n", learnRegion.right);
		printf("    top    : %.3lf\n", learnRegion.top);
		printf("    bottom : %.3lf\n", learnRegion.bottom);
		printf("    angle  : %.3lf\n", learnRegion.angle);
		printf("  2. Interest Pivot : (%.3lf, %.3lf)\n", flpLearnPivot.x, flpLearnPivot.y);
		printf("\n");

		// 검출할 이미지 설정 // Set image to detect
		patternMatch.SetSourceImage(fliFindImage);

		// 검출 시 사용될 파라미터를 설정합니다. // Set the parameters to be used for detection.
		// 검출 시 사용될 유효 변경 크기범위를 설정합니다. // Set the effective change size range to be used for detection.
		patternMatch.SetScaleRange(0.95, 1.05);
		// 검출 시 사용될 기본 각도를 설정합니다. // Set the default angle to be used for detection.
		patternMatch.SetAngleBias(0.);
		// 검출 시 사용될 각도의 탐색범위를 설정합니다. // Set the search range of the angle to be used for detection.
		// 각도는 기본 각도를 기준으로 (기본 각도 - AngleTolerance, 기본 각도 + AngleTolerance)가 최종 탐색범위 // The angle is based on the basic angle (default angle - AngleTolerance, basic angle + AngleTolerance) is the final search range
		patternMatch.SetAngleTolerance(10.);
		// 검출 시 최적화 정도를 설정합니다. // Set the degree of optimization for detection.
		// 0 ~ 1범위에서 0에 가까울수록 정확성은 낮아질 수 있으나, 속도가 상향됩니다. // From 0 to 1, the closer to 0, the lower the accuracy, but the higher the speed.
		patternMatch.SetAccuracy(0.5);
		// 검출 시 사용될 최소 탐색점수를 설정합니다. // Set the minimum search score to be used for detection.
		patternMatch.SetMinimumDetectionScore(0.7);
		// 검출 시 사용될 최대 탐색객체 수를 설정합니다. // Set the maximum number of search objects to be used for detection.
		patternMatch.SetMaxObject(1);
		// 검출 시 보간법 사용 유무에 대해 설정합니다. // Set whether to use interpolation when detecting.
		patternMatch.EnableInterpolation(true);

		// 알고리즘 수행 // Execute the algorithm
		res = patternMatch.Execute();

		if(IsFail(res))
		{
			ErrorPrint(res, "Failed to execute\n");
			break;
		}

		// 패턴 검출 결과를 가져옵니다. // Get the pattern detection result.
		int64_t i64ResultCount = patternMatch.GetResultCount();

		printf(" ▶ Find Information\n");

		for(int64_t i = 0; i < i64ResultCount; ++i)
		{
			CPatternMatch::SResult results;

			patternMatch.GetResult(i, results);

			float f32Score = results.f32Score;
			float f32Angle = results.f32Angle;
			float f32Scale = results.f32Scale;
			Foundation::CFLPoint<double> flpPivot = results.flpPivot;
			Foundation::CFLFigure* pFlfRegion = results.pFlfRegion;
			Foundation::CFLRect<double>* pFlrResultRegion = dynamic_cast<Foundation::CFLRect<double>*>(pFlfRegion);

			if(!pFlrResultRegion)
				break;

			Foundation::CFLRect<double> flrResultRegion = *pFlrResultRegion;

			CFLString<wchar_t> strData;
			CFLPoint<double> flpResultRegion = CFLPoint<double>(flrResultRegion.left, flrResultRegion.top);

			// 패턴 검출 결과를 Console창에 출력합니다. // Output the pattern detection result to the console window.
			printf(" < Instance : %lld >\n", i);
			printf("  1. ROI Shape Type : Rectangle\n");
			printf("    left   : %.3lf\n", flrResultRegion.left);
			printf("    right  : %.3lf\n", flrResultRegion.right);
			printf("    top    : %.3lf\n", flrResultRegion.top);
			printf("    bottom : %.3lf\n", flrResultRegion.bottom);
			printf("    angle  : %.3lf\n", flrResultRegion.angle);
			printf("  2. Interest Pivot : (%.4lf, %.4lf)\n", flpResultRegion.x, flpResultRegion.y);
			printf("  3. Score : %0.3lf\n  4. Angle : %0.3lf\n  5. Scale : %0.3lf\n", f32Score, flrResultRegion.angle, f32Scale);
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
			
			// 결과 영역을 디스플레이 한다 // Display the result area
			if(IsFail(res = layerFind.DrawFigureImage(&flrResultRegion, BLACK, 3)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			if(IsFail(res = layerFind.DrawFigureImage(&flrResultRegion, LIME)))
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
		viewImageLearn.Invalidate(true);
		viewImageFind.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageLearn.IsAvailable() && viewImageFind.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}