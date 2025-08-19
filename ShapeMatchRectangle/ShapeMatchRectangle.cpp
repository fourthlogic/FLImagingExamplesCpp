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

	bool bError = false;

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// Source 이미지 로드 // Load the source image
		if(IsFail(res = fliImage.Load(L"../../ExampleImages/ShapeMatch/Rect Match.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage.Create(400, 0, 912, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			bError = true;
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		if(IsFail(res = viewImage.SetImagePtr(&fliImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			bError = true;
			break;
		}

		if(bError)
			break;

		// Shape Match 객체 생성 // Create Shape Match object
		CShapeMatchRectangle shapeMatch;

		// 학습할 사각형의 반지름 설정 // Set the radius of the rectangle to learn
		shapeMatch.SetSize(60, 365);
		// 학습할 사각형의 각도 설정 // Set the angle of the rectangle to learn
		shapeMatch.SetAngle(0);

		// 검출할 객체의 색상을 설정합니다. // Sets the color of the object to be detected.
		shapeMatch.SetObjectColor(EShapeMatchObjectColor_Bright);

		// 도형 학습 // Learn shape
		if(IsFail(res = shapeMatch.Learn()))
		{
			ErrorPrint(res, "Failed to Learn.\n");
			break;
		}

		// 검출할 이미지 설정 // Set image to detect
		shapeMatch.SetSourceImage(fliImage);
		// 검출 시 사용될 유효 변경 크기범위를 설정합니다. // Set the effective change size range to be used for detection.
		shapeMatch.SetScaleRange(0.9, 1.1);
		// 검출할 객체 개수 설정. 0이하의 값을 입력 시 모든 객체 검출 
		// Set the number of objects to detect. If a value less than or equal to 0 is entered, all objects will be detected.
		shapeMatch.SetMaxObject(0);

		// 알고리즘 수행 // Execute the algorithm
		if(IsFail(res = shapeMatch.Execute()))
		{
			ErrorPrint(res, "Failed to execute.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer = viewImage.GetLayer(0);

		int64_t i64ResultCount = shapeMatch.GetResultCount();

		for(int64_t i = 0; i < i64ResultCount; ++i)
		{
			CShapeMatchRectangleResult matchResult;

			shapeMatch.GetResult(i, matchResult);

			// 도형 검출 결과를 Console창에 출력합니다. // Output the shape detection result to the console window.
			printf(" < Instance : %lld >\n", i);
			printf("  1. Shape Type : Rectangle\n");
			printf("    Pivot X: %.3lf\n", matchResult.flpPivot.x);
			printf("    Pivot Y: %.3lf\n", matchResult.flpPivot.y);
			printf("    Width    : %.3lf\n", matchResult.flrResultObject.GetWidth());
			printf("    Height    : %.3lf\n", matchResult.flrResultObject.GetHeight());
			printf("    Angle    : %.3lf\n", matchResult.flrResultObject.angle);
			printf("  2. Score : %.3lf\n  3. Angle : %.3lf\n  4. Scale : %.3lf\n\n", matchResult.f32Score, matchResult.f32Angle, matchResult.f32Scale);

			if(IsFail(res = layer.DrawFigureImage(matchResult.flrResultObject, CYAN, 3)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}

			if(IsFail(res = layer.DrawTextImage(matchResult.flpPivot, CFLString<wchar_t>().Format(L"Score : %.3lf\nAngle : %.3lf\nScale : %.3lf\nPivot : (%.2lf, %.2lf)", matchResult.f32Score, matchResult.f32Angle, matchResult.f32Scale, matchResult.flpPivot.x, matchResult.flpPivot.y), YELLOW, BLACK, 15)))
			{
				ErrorPrint(res, "Failed to draw text\n");
				break;
			}
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