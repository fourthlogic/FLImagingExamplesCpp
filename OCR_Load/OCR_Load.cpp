#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliLearnImage;
	CFLImage fliRecognizeImage;
	CFLImage fliRecognizeImageUnicode;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageRecognize;
	CGUIViewImageWrap viewImageRecognizeUnicode;
	CResult res;

	do
	{
		if((res = fliLearnImage.Load(L"../../ExampleImages/OCR/FourthLogic Inc_Learn.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		if((res = fliRecognizeImage.Load(L"../../ExampleImages/OCR/OCR_Recognition.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		if((res = fliRecognizeImageUnicode.Load(L"../../ExampleImages/OCR/OCR_Recognition_Unicode2.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Converted 이미지 뷰 생성
		if((res = viewImageRecognize.Create(200, 0, 712, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Converted 이미지 뷰 생성
		if((res = viewImageRecognizeUnicode.Create(712, 0, 1224, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Converted 이미지 뷰에 이미지를 디스플레이
		if((res = viewImageRecognize.SetImagePtr(&fliRecognizeImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Converted 이미지 뷰에 이미지를 디스플레이
		if((res = viewImageRecognizeUnicode.SetImagePtr(&fliRecognizeImageUnicode)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerRecognize = viewImageRecognize.GetLayer(1);
		CGUIViewImageLayerWrap layerRecognizeUnicode = viewImageRecognizeUnicode.GetLayer(1);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerRecognize.Clear();
		layerRecognizeUnicode.Clear();

		if((res = layerRecognize.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Recognition1", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text");
			break;
		}

		if((res = layerRecognizeUnicode.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Recognition2", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text");
			break;
		}

		// 학습을 진행할 OCR 객체 생성 // Create OCR object to Learn
		COCR ocrLearn;

		// 문자를 학습할 이미지 설정
		if(IsFail(res = ocrLearn.SetLearnImage(fliLearnImage)))
		{
			ErrorPrint(res, L"Failed to set Learn Image.");
			break;
		}

		// 학습할 이미지에 저장되어있는 Figure 학습
		if(IsFail(res = ocrLearn.Learn()))
		{
			ErrorPrint(res, L"Failed to train.");
			break;
		}

		// 인식할 문자의 각도 범위를 설정
		if(IsFail(res = ocrLearn.SetRecognizingAngleTolerance(9.)))
		{
			ErrorPrint(res, L"Failed to set recognizing angle tolerance.");
			break;
		}

		// 인식할 문자의 색상을 설정
		if(IsFail(res = ocrLearn.SetRecognizingCharacterColorType(ECharacterColorType_All)))
		{
			ErrorPrint(res, L"Failed to set recognizing character color.");
			break;
		}

		// 인식할 최소 점수를 설정
		if(IsFail(res = ocrLearn.SetRecognizingMinimumScore(0.7)))
		{
			ErrorPrint(res, L"Failed to set minimum score.");
			break;
		}

		// 인식할 최대 개수를 설정
		if(IsFail(res = ocrLearn.SetRecognizingMaximumCharacterCount(20)))
		{
			ErrorPrint(res, L"Failed to set maximum character count.");
			break;
		}

		// 인식할 문자의 유니코드 여부를 설정
		if(IsFail(res = ocrLearn.EnableRecognizingUnicodeByteCharacter(true)))
		{
			ErrorPrint(res, L"Failed to Enable unicode byte character.");
			break;
		}

		 // 학습 정보 파일 및 입력 파라미터를 저장
		if(IsFail(res = ocrLearn.Save(L"../../ExampleImages/OCR/OCR_FourthLogic.flocr")))
		{
			ErrorPrint(res, L"Failed to save learned file.");
			break;
		}

		// 학습 파일을 로드할 OCR 객체 생성 // Create OCR object to load file
		COCR ocrLoad;

		// 학습 정보 파일을 로드
		if(IsFail(res = ocrLoad.Load(L"../../ExampleImages/OCR/OCR_FourthLogic.flocr")))
		{
			ErrorPrint(res, L"Failed to load learned file.");
			break;
		}

	    // 문자를 인식할 이미지 설정
		if(IsFail(res = ocrLoad.SetSourceImage(fliRecognizeImage)))
		{
			ErrorPrint(res, L"Failed to set Source Image.");
			break;
		}

		// 인식할 이미지에서 문자를 찾는 기능을 수행
		if(IsFail(res = ocrLoad.Execute()))
		{
			ErrorPrint(res, res.GetString());
			break;
		}

		// 찾은 문자의 개수를 받아오는 함수
		int64_t i64ResultCount = ocrLoad.GetResultCount();

		// 찾은 문자의 정보를 받아올 컨테이너
		COCR::COCRRecognitionCharacterInfo resultChar;

		for(int64_t i = 0; i < i64ResultCount; ++i)
		{
			ocrLoad.GetResultRecognizedCharactersInfo(i, resultChar);

			CFLString<wchar_t> flsResultString;
			CFLRect<double> flrBoundary;

			flsResultString.Format(L"[%s] Score: %d%%\nScale: %.2lf\nAngle : %.1lf", resultChar.flfaCharacter.GetName(), (int32_t)(resultChar.f64Score * 100), resultChar.f64ScaleWidth * resultChar.f64ScaleHeight, resultChar.f64Rotation);
			resultChar.flfaCharacter.GetBoundaryRect(flrBoundary);

			if(IsFail(layerRecognize.DrawTextImage(CFLPoint<double>(flrBoundary.left, flrBoundary.top), flsResultString, YELLOW, BLACK, 12, false, 0., EGUIViewImageTextAlignment_LEFT_BOTTOM)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			if(IsFail(layerRecognize.DrawFigureImage(resultChar.flfaCharacter, LIME, 1, LIME, EGUIViewImagePenStyle_Solid, 1.f, 0.35f)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}
		}

		// 문자를 인식할 이미지 설정
		if(IsFail(res = ocrLoad.SetSourceImage(fliRecognizeImageUnicode)))
		{
			ErrorPrint(res, L"Failed to set Source Image.");
			break;
		}

		// 인식할 이미지에서 문자를 찾는 기능을 수행
		if(IsFail(res = ocrLoad.Execute()))
		{
			ErrorPrint(res, res.GetString());
			break;
		}

		// 찾은 문자의 개수를 받아오는 함수
		i64ResultCount = ocrLoad.GetResultCount();

		for(int64_t i = 0; i < i64ResultCount; ++i)
		{
			ocrLoad.GetResultRecognizedCharactersInfo(i, resultChar);

			CFLString<wchar_t> flsResultString;
			CFLRect<double> flrBoundary;

			flsResultString.Format(L"[%s] Score: %d%%\nScale: %.2lf\nAngle : %.1lf", resultChar.flfaCharacter.GetName(), (int32_t)(resultChar.f64Score * 100), resultChar.f64ScaleWidth * resultChar.f64ScaleHeight, resultChar.f64Rotation);
			resultChar.flfaCharacter.GetBoundaryRect(flrBoundary);

			if(IsFail(layerRecognizeUnicode.DrawTextImage(CFLPoint<double>(flrBoundary.left, flrBoundary.top), flsResultString, YELLOW, BLACK, 15, false, 0., EGUIViewImageTextAlignment_LEFT_BOTTOM)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			if(IsFail(layerRecognizeUnicode.DrawFigureImage(resultChar.flfaCharacter, LIME, 1, LIME, EGUIViewImagePenStyle_Solid, 1.f, 0.35f)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageRecognize.Invalidate(true);
		viewImageRecognizeUnicode.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageRecognize.IsAvailable() && viewImageRecognizeUnicode.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}