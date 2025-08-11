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
		if(IsFail(res = fliLearnImage.Load(L"../../ExampleImages/Matching/Rectangle Array_0.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = fliFindImage.Load(L"../../ExampleImages/Matching/Rectangle Array_1.flif")))
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

		if(IsFail(res = layerLearn.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Measurement Array", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = layerFind.DrawTextCanvas(&CFLPoint<double>(0, 0), L"FIND", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// Rectangle Array Match 객체 생성 // Create a Rectangle Array Match object
		CRectangleArrayMatch rectangleArrayMatch;

		// 학습할 영역을 설정합니다. // Set the area to learn.
		CFLFigureArray flfaMeasurement;
		CFLRect<double> flrRect00(587.479194, 364.452004, 929.550836, 616.575019);
		CFLRect<double> flrRect01(583.464651, 1215.493013, 924.560595, 1467.566788);
		CFLRect<double> flrRect02(1531.503352, 655.504324, 1872.516362, 908.626989);
		CFLRect<double> flrRect03(1241.471070, 1222.460787, 1580.517129, 1474.488487);

		flfaMeasurement.PushBack(flrRect00);
		flfaMeasurement.PushBack(flrRect01);
		flfaMeasurement.PushBack(flrRect02);
		flfaMeasurement.PushBack(flrRect03);

		CFLPoint<double> flpCameraPivot(0., 0.);

		// 검출 시 사용될 파라미터를 설정합니다. // Set the parameters to be used for detection.
		// 탐색할 이미지를 설정합니다. // Set the image to browse.
		rectangleArrayMatch.SetSourceImage(fliFindImage);
		// 측정 배열을 설정합니다. // Set up the measurement array.
		rectangleArrayMatch.SetArray(flfaMeasurement); 
		// 탐색 시, MeasurementArray의 기본 각도를 설정합니다. // On navigation, set the default angle of the MeasurementArray.
		rectangleArrayMatch.SetBaseAngle(0.);
		// 중심의 초기값을 이미지 중심으로 할지 설정합니다. // Set whether the initial value of the center is the center of the image.
		rectangleArrayMatch.EnablePivotImageCenter(true);
		// 중심 오프셋을 설정합니다. // Set the center offset.
		rectangleArrayMatch.SetPivotOffset(flpCameraPivot);
		// 최소 스코어 점수를 설정합니다. // Set the minimum score score.
		rectangleArrayMatch.SetMinScore(0.5);
		// 탐색 시, 각도 탐색 범위를 설정합니다. // When searching, set the angle search range.
		rectangleArrayMatch.SetObjectAngleTolerance(180);
		// 탐색 시, Fitting Enable/Disable을 설정합니다. // When searching, set Fitting Enable/Disable.
		rectangleArrayMatch.SetFitting();
		// 탐색 시, 허용 이동량 범위를 설정합니다. // When searching, set the allowable movement range.
		rectangleArrayMatch.SetAllowingObjectDistanceError();
		


		// 배열 측정 영역이 어디인지 알기 위해 디스플레이 한다 // Display to see where the array measurement area is
		if((res = layerLearn.DrawFigureImage(flfaMeasurement, BLUE, 3, BLUE, GUI::EGUIViewImagePenStyle_Solid, 0.25, 0.25)).IsFail())
		{
			ErrorPrint(res, "Failed to draw figure\n");
			break;
		}

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = rectangleArrayMatch.Execute()))
		{
			ErrorPrint(res, "Failed to execute.\n");
			break;
		}


		// 검출 결과 배열의 개수를 가져옵니다. // Get the number of detection result arrays.
		int64_t i64ResultCount = rectangleArrayMatch.GetResultCount();
		double f64Score, f64Angle;
		// 검출 결과 배열의 점수를 가져옵니다. // Get the score of the detection result array.
		rectangleArrayMatch.GetResultArrayScore(f64Score);
		// 검출 결과 배열의 각도를 가져옵니다. // Get the angle of the detection result array.
		rectangleArrayMatch.GetResultArrayAngle(f64Angle);

		for(int32_t i = 0; i < i64ResultCount; ++i)
		{
			CRectangleArrayMatch::SResult sResult;

			// 검출 결과 중 배열 하나를 가져옵니다. // Get an array of detection results.
			rectangleArrayMatch.GetResult(i, sResult);
			CFLPoint<double> flpRegionCenter = sResult.flrMeasuredRegion.GetCenter();
			CFLString<wchar_t> strDisplayResult;
			strDisplayResult.Format(L"Array Element ID : %lld\n Score : %.3lf\n Angle : %.3lf", sResult.i64Index, sResult.f64Score, sResult.f64Angle);

			// 검출 결과 중 배열 하나의 결과를 디스플레이 합니다. // Display the result of one array among the detection results.
			if((res = layerFind.DrawFigureImage(sResult.flrMeasuredRegion, BLACK, 3)).IsFail())
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			// 검출 결과 중 배열 하나의 결과를 디스플레이 합니다. // Display the result of one array among the detection results.
			if((res = layerFind.DrawFigureImage(sResult.flrMeasuredRegion, CYAN, 1)).IsFail())
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			// 검출 결과 중 배열 하나의 결과의 중심점을 디스플레이 합니다. // Display the center point of the result of one of the detection results.
			if((res = layerFind.DrawTextImage(flpRegionCenter, strDisplayResult, YELLOW, BLACK, 11)).IsFail())
			{
				ErrorPrint(res, "Failed to draw text\n");
				break;
			}

			CFLString<wchar_t> strDisplayResultArray;

			if(i == 0)
			{
				strDisplayResultArray.Format(L"Array Score : % .3lf  Array Angle : % .3lf", f64Score, f64Angle);
				wprintf(L"%s\n", strDisplayResultArray.GetString());

				strDisplayResult.Format(L"Array Score : % .3lf\n Array Angle : % .3lf", f64Score, f64Angle);

				CFLQuad<double> flqRegion(sResult.flrMeasuredRegion);

				double f64MaxY = INT_MAX, f64MaxX = INT_MAX;

				for(int32_t i32Search = 0; i32Search < 4; ++i32Search)
				{
					if(f64MaxY > flqRegion.flpPoints[i32Search].y)
						f64MaxY = flqRegion.flpPoints[i32Search].y;
				}

				for(int32_t i32Search = 0; i32Search < 4; ++i32Search)
				{
					if(f64MaxY == flqRegion.flpPoints[i32Search].y)
					{
						if(f64MaxX > flqRegion.flpPoints[i32Search].x)
							f64MaxX = flqRegion.flpPoints[i32Search].x;
					}
				}

				CFLPoint<double> flpArrayResult(f64MaxX, f64MaxY - 10);

			    // 검출 결과 중 배열 하나의 결과의 정보를 디스플레이 합니다. // Display the information of the result of one of the detection results.
				if((res = layerFind.DrawTextImage(flpArrayResult, strDisplayResult, GOLD, BLACK, 14)).IsFail())
				{
					ErrorPrint(res, "Failed to draw text\n");
					break;
				}
			}

			CFLString<wchar_t> strDisplayResultElement;
			strDisplayResultElement.Format(L"Array Element ID : %lld Score : %.3lf Angle : %.3lf", sResult.i64Index, sResult.f64Score, sResult.f64Angle);
			wprintf(L" - %s\n", strDisplayResultElement.GetString());
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