#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage fliLearnImage[2];
	CFLImage fliFindImage;

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap viewImageLearn[2];
	CGUIViewImageWrap viewImageFind;

	CResult res = EResult_UnknownError;

	// Pattern Match Multi 객체 생성 // Create Pattern Match Multi object
	CPatternMatchMultiSparse patternMatchMultiSparseSave;

	do
	{
		CFLString<wchar_t> arrPath[2] = {L"../../ExampleImages/Matching/Pattern2 Single Learn.flif",
			L"../../ExampleImages/Matching/Pattern2 Single Learn.flif"};

		CFLString<wchar_t> arrClassName[2] = {L"A", L"B"};
		uint32_t arrColor[2] = {LIME, RED};
		CFLRect<double> arrLearnRegion[2] = { CFLRect<double>(326.6913,372.2960,477.5354,521.5354), CFLRect<double>(586.7185,566.3427,763.2982,672.1134) };

		printf(" ▷ Learn Information\n");

		for(int64_t i64DataIdx = 0; i64DataIdx < 2; ++i64DataIdx)
		{
			// 이미지 로드 // Loads image
			if(IsFail(res = fliLearnImage[i64DataIdx].Load(arrPath[i64DataIdx].GetBuffer())))
			{
				ErrorPrint(res, "Failed to load the image file.\n");
				break;
			}

			// 이미지 뷰 생성 // Creates imageview
			if(IsFail(res = viewImageLearn[i64DataIdx].Create((int32_t)(400 + 512 * i64DataIdx), 0, (int32_t)(400 + 512 * (i64DataIdx + 1)), 384)))
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				break;
			}

			// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
			if(IsFail(res = viewImageLearn[i64DataIdx].SetImagePtr(&fliLearnImage[i64DataIdx])))
			{
				ErrorPrint(res, "Failed to set image object on the image view.\n");
				break;
			}

			CGUIViewImageLayerWrap layerLearn = viewImageLearn[i64DataIdx].GetLayer(0);

			layerLearn.Clear();

			// 학습할 이미지 설정 // Set the image to learn
			patternMatchMultiSparseSave.SetLearnImage(fliLearnImage[i64DataIdx]);

			// 학습할 영역을 설정합니다. // Set the area to learn.
			CFLPoint<double> flpLearnPivot = arrLearnRegion[i64DataIdx].GetCenter();
			patternMatchMultiSparseSave.SetLearnROI(arrLearnRegion[i64DataIdx]);
			patternMatchMultiSparseSave.SetLearnPivot(flpLearnPivot);
			patternMatchMultiSparseSave.SetSampleCount(256);

		    // 알고리즘 수행 // Execute the algorithm
			if(IsFail(res = patternMatchMultiSparseSave.Learn(arrClassName[i64DataIdx])))
			{
				ErrorPrint(res, "Failed to execute Learn.\n");
				break;
			}

			// 측정 영역이 어디인지 알기 위해 디스플레이 한다 // Display to know where the measurement area is
			if(IsFail(res = layerLearn.DrawFigureImage(&arrLearnRegion[i64DataIdx], BLACK, 3)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			if(IsFail(res = layerLearn.DrawFigureImage(&arrLearnRegion[i64DataIdx], arrColor[i64DataIdx])))
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

		
			CFLString<wchar_t> strStatus;
			strStatus.Format(L"LEARN CLASS '%s'", arrClassName[i64DataIdx].GetBuffer());

			if(IsFail(res = layerLearn.DrawTextCanvas(&CFLPoint<double>(0, 0), strStatus.GetBuffer(), YELLOW, BLACK, 30)))
			{
				ErrorPrint(res, "Failed to draw text\n");
				break;
			}

			// 학습한 정보에 대해 Console창에 출력한다 // Print the learned information to the console window
			printf("  < LEARN CLASS '%ws' > \n", arrClassName[i64DataIdx].GetBuffer());
			printf("  1. ROI Shape Type : Rectangle\n");
			printf("    left   : %.3lf\n", arrLearnRegion[i64DataIdx].left);
			printf("    right  : %.3lf\n", arrLearnRegion[i64DataIdx].right);
			printf("    top    : %.3lf\n", arrLearnRegion[i64DataIdx].top);
			printf("    bottom : %.3lf\n", arrLearnRegion[i64DataIdx].bottom);
			printf("    angle  : %.3lf\n", arrLearnRegion[i64DataIdx].angle);
			printf("  2. Interest Pivot : (%.3lf, %.3lf)\n", flpLearnPivot.x, flpLearnPivot.y);
			printf("\n");


			// 이미지 뷰를 갱신 합니다. // Update the image view.
			viewImageLearn[i64DataIdx].Invalidate(true);
		}
		
		// 데이터 추가를 완료 후 Save를 진행합니다. // After completing data addition, proceed with Save.
		if(IsFail(res = patternMatchMultiSparseSave.Save(L"../../ExampleImages/Matching/Pattern Multi Learn File")))
		{
			ErrorPrint(res, "Failed to save\n");
			break;
		}

		// 이미지 로드 // Loads image
		if(IsFail(res = fliFindImage.Load(L"../../ExampleImages/Matching/Pattern2 Single Find2.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}


		// 이미지 뷰 생성 // Creates imageview
		if(IsFail(res = viewImageFind.Create(400, 384, 1168, 960)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
		if(IsFail(res = viewImageFind.SetImagePtr(&fliFindImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		for(int64_t i64DataIdx = 0; i64DataIdx < 2; ++i64DataIdx)
		{
			// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
			if(IsFail(res = viewImageFind.SynchronizeWindow(&viewImageLearn[i64DataIdx])))
			{
				ErrorPrint(res, "Failed to synchronize window.\n");
				break;
			}
		}

		CGUIViewImageLayerWrap layerFind = viewImageFind.GetLayer(1);
		layerFind.Clear();

		if(IsFail(res = layerFind.DrawTextCanvas(&CFLPoint<double>(0, 0), L"FIND", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Fai led to draw text\n");
			break;
		}

		CPatternMatchMultiSparse patternMatchMultiSparseLoad;

		// 데이터를 Load합니다. // Load data.
		if(IsFail(res = patternMatchMultiSparseLoad.Load(L"../../ExampleImages/Matching/Pattern Multi Learn File")))
		{
			ErrorPrint(res, "Failed to save\n");
			break;
		}

		// 검출할 이미지 설정 // Set image to detect
		patternMatchMultiSparseSave.SetSourceImage(fliFindImage);

		// 검출 시 사용될 파라미터를 설정합니다. // Set the parameters to be used for detection.
		// 검출 시 사용될 기본 각도를 설정합니다. // Set the default angle to be used for detection.
		patternMatchMultiSparseSave.SetAngleBias(0.);
		// 검출 시 사용될 각도의 탐색범위를 설정합니다. // Set the search range of the angle to be used for detection.
		// 각도는 기본 각도를 기준으로 (기본 각도 - AngleTolerance, 기본 각도 + AngleTolerance)가 최종 탐색범위 // The angle is based on the basic angle (default angle - AngleTolerance, basic angle + AngleTolerance) is the final search range
		patternMatchMultiSparseSave.SetAngleTolerance(15.);
		// 검출 시 최적화 정도를 설정합니다. // Set the degree of optimization for detection.
		// 검출 시 사용될 최소 탐색점수를 설정합니다. // Set the minimum search score to be used for detection.
		patternMatchMultiSparseSave.SetMinimumDetectionScore(0.7);
		// 검출 시 사용될 탐색 방식을 설정합니다. // Set the search method to be used for detection.
		patternMatchMultiSparseSave.SetMaxObjectMode(CPatternMatchMultiSparse::EMaxObjectMode_Total);
		// 검출 시 사용될 최대 탐색객체 수를 설정합니다. // Set the maximum number of search objects to be used for detection.
		patternMatchMultiSparseSave.SetMaxObjectTotal(2);
		// 검출 시 보간법 사용 유무에 대해 설정합니다. // Set whether to use interpolation when detecting.
		patternMatchMultiSparseSave.EnableInterpolation(true);
		// 검출 시 서로 다른 클래스에 대해 영역 중복을 허용 유무에 대해 설정합니다. // Set whether to allow area overlap for different classes during detection.
		patternMatchMultiSparseSave.SetConflictDetectionMethod();

		// 알고리즘 수행 // Execute the algorithm
		res = patternMatchMultiSparseSave.Execute();

		if(IsFail(res))
		{
			ErrorPrint(res, "Failed to execute\n");
			break;
		}

		// 패턴 검출 결과를 가져옵니다. // Get the pattern detection result.
		int64_t i64ResultCount = patternMatchMultiSparseSave.GetResultCount();

		printf(" ▶ Find Information\n");

		for(int64_t i = 0; i < i64ResultCount; ++i)
		{
			CPatternMatchMultiSparse::SResult results;
			CFLFigureArray flfaResultPoints;

			patternMatchMultiSparseSave.GetResult(i, results);

			float f32Score = results.f32Score;
			float f32Angle = results.f32Angle;
			float f32Scale = results.f32Scale;
			Foundation::CFLPoint<double> flpPivot = results.flpPivot;
			Foundation::CFLFigure* pFlfRegion = results.pFlfRegion;
			Foundation::CFLRect<double>* pFlrResultRegion = dynamic_cast<Foundation::CFLRect<double>*>(pFlfRegion);

			if(!pFlrResultRegion)
				break;

			Foundation::CFLRect<double> flrResultRegion = *pFlrResultRegion;

			CFLString<wchar_t> strResult;
			CFLString<wchar_t> wstrClassName = results.strClassName;

			int64_t i64Idx = 0;

			for(int64_t i64ResultIndex = 0; i64ResultIndex < 3; ++i64ResultIndex)
			{
				if(wstrClassName == arrClassName[i64ResultIndex])
				{
					i64Idx = i64ResultIndex;
					break;
				}
			}
			
			// 패턴 검출 결과를 Console창에 출력합니다. // Output the pattern detection result to the console window.
			printf(" < Instance : %lld >\n", i);
			printf(" Class Name : %ws\n", wstrClassName.GetBuffer());
			printf("  1. ROI Shape Type : Rectangle\n");
			printf("    left   : %.3lf\n", flrResultRegion.left);
			printf("    right  : %.3lf\n", flrResultRegion.right);
			printf("    top    : %.3lf\n", flrResultRegion.top);
			printf("    bottom : %.3lf\n", flrResultRegion.bottom);
			printf("    angle  : %.3lf\n", flrResultRegion.angle);
			printf("  2. Interest Pivot : (%.3lf, %.3lf)\n", flpPivot.x, flpPivot.y);
			printf("  3. Score : %0.3lf\n  4. Angle : %0.3lf\n  5. Scale : %0.3lf\n", f32Score, f32Angle, f32Scale);

			printf("\n");

			if(IsFail(res = layerFind.DrawFigureImage(&flrResultRegion, BLACK, 3)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			if(IsFail(res = layerFind.DrawFigureImage(&flrResultRegion, arrColor[i64Idx])))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

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

			TPoint<double> tpPosition;
			tpPosition.x = flpPivot.x;
			tpPosition.y = flpPivot.y;

			// 검출 결과에 해당하는 클래스명을 디스플레이 한다 // Display the class name corresponding to the detection result
			if(IsFail(res = layerFind.DrawTextImage(tpPosition, wstrClassName.GetBuffer(), YELLOW, BLACK, 30, false, 0, EGUIViewImageTextAlignment_CENTER, nullptr, 1.f, 1.f, EGUIViewImageFontWeight_BOLD, false)))
			{
				ErrorPrint(res, "Failed to draw text\n");
				break;
			}

			tpPosition.x += 10;
			CFLString<wchar_t> strText;
			strText.Format(L"Score : %.3lf\nAngle : %.3lf\nScale : x%.3lf\n", f32Score, f32Angle, f32Scale);

			if((res = layerFind.DrawTextImage(tpPosition, strText, YELLOW, BLACK, 15, false, 0, EGUIViewImageTextAlignment_LEFT_CENTER)).IsFail())
			{
				ErrorPrint(res, "Failed to draw text");
				break;
			}
		}

		viewImageFind.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageFind.IsAvailable() && viewImageLearn[0].IsAvailable() && viewImageLearn[1].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}