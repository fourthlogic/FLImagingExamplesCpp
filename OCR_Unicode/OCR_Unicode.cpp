﻿#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliImage;
	CFLImage fliRecognizeImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage;
	CGUIViewImageWrap viewImageRecognize;
	CResult res;

	do
	{
		// 이미지 로드 // Load image
		if((res = fliImage.Load(L"../../ExampleImages/OCR/OCR_Learn_Unicode.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		if((res = fliRecognizeImage.Load(L"../../ExampleImages/OCR/OCR_Recognition_Unicode.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if((res = viewImage.Create(200, 0, 712, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Converted 이미지 뷰 생성
		if((res = viewImageRecognize.Create(712, 0, 1224, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if((res = viewImage.SetImagePtr(&fliImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Converted 이미지 뷰에 이미지를 디스플레이
		if((res = viewImageRecognize.SetImagePtr(&fliRecognizeImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if((res = viewImage.SynchronizeWindow(&viewImageRecognize)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((res = viewImage.SynchronizePointOfView(&viewImageRecognize)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer = viewImage.GetLayer(0);
		CGUIViewImageLayerWrap layerRecognize = viewImageRecognize.GetLayer(1);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer.Clear();
		layerRecognize.Clear();

		if((res = layer.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Learn", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text");
			break;
		}

		if((res = layerRecognize.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Recognition", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text");
			break;
		}

		// OpticalCharacterReader 객체 생성 // Create OpticalCharacterReader object
		COCR ocr;

		// 문자를 학습할 이미지 설정
		if(IsFail(res = ocr.SetLearnImage(fliImage)))
		{
			ErrorPrint(res, L"Failed to set Source Image.");
			break;
		}

		// 학습할 이미지에 저장되어있는 Figure 학습
		if(IsFail(res = ocr.Learn()))
		{
			ErrorPrint(res, L"Failed to train.");
			break;
		}

		// 학습한 문자의 개수를 받아오는 함수
		int64_t i64LearnedCount = ocr.GetLearnedDataCount();
		CFLFigureArray flfaLearned;

		// 학습한 문자의 모양를 받아오는 함수
		ocr.GetLearnedCharacter(flfaLearned);

		for(int64_t i = 0; i < i64LearnedCount; ++i)
		{
			CFLString<wchar_t> flsResultString;
			CFLRect<double> flrBoundary;

			flsResultString.Format(L"%s", flfaLearned.GetAt(i)->GetName());
			flfaLearned.GetAt(i)->GetBoundaryRect(flrBoundary);

			if(IsFail(layer.DrawTextImage(CFLPoint<double>(flrBoundary.left, flrBoundary.top), flsResultString, YELLOW, BLACK, 15, false, 0., EGUIViewImageTextAlignment_LEFT_BOTTOM)))
			{
				printf("Failed to draw learnt character : %lld", i);
				break;
			}

			if(IsFail(layer.DrawFigureImage(flfaLearned.GetAt(i), LIME, 1, LIME, EGUIViewImagePenStyle_Solid, 1.f, 0.35f)))
			{
				printf("Failed to draw learnt character : %lld", i);
				break;
			}
		}

	   // 문자를 인식할 이미지 설정
		if(IsFail(res = ocr.SetSourceImage(fliRecognizeImage)))
		{
			ErrorPrint(res, L"Failed to set Source Image.");
			break;
		}

		// 인식할 문자의 각도 범위를 설정
		if(IsFail(res = ocr.SetRecognizingAngleTolerance(50.)))
		{
			ErrorPrint(res, L"Failed to set recognizing angle tolerance.");
			break;
		}

		// 인식할 이미지의 잡음 제거 여부를 설정
		if(IsFail(res = ocr.EnableRecognizingNoiseReduction(true)))
		{
			ErrorPrint(res, L"Failed to set recognizing Noise Reduction.");
			break;
		}

		// 인식할 문자의 색상을 설정
		if(IsFail(res = ocr.SetRecognizingCharacterColorType(ECharacterColorType_WhiteOnBlack)))
		{
			ErrorPrint(res, L"Failed to set recognizing character color.");
			break;
		}

		// 인식할 최소 점수를 설정
		if(IsFail(res = ocr.SetRecognizingMinimumScore(0.7)))
		{
			ErrorPrint(res, L"Failed to set minimum score.");
			break;
		}

		// 인식할 최대 개수를 설정
		if(IsFail(res = ocr.SetRecognizingMaximumCharacterCount(14)))
		{
			ErrorPrint(res, L"Failed to set maximum character count.");
			break;
		}

		// 인식할 문자의 유니코드 여부를 설정
		if(IsFail(res = ocr.EnableRecognizingUnicodeByteCharacter(true)))
		{
			ErrorPrint(res, L"Failed to Enable unicode byte character.");
			break;
		}

		// 인식할 이미지에서 문자를 찾는 기능을 수행
		if(IsFail(res = ocr.Execute()))
		{
			ErrorPrint(res, res.GetString());
			break;
		}

		// 찾은 문자의 개수를 받아오는 함수
		int64_t i64ResultCount = ocr.GetResultCount();

		// 찾은 문자의 정보를 받아올 컨테이너
		COCR::COCRRecognitionCharacterInfo resultChar;

		for(int64_t i = 0; i < i64ResultCount; ++i)
		{
			ocr.GetResultRecognizedCharactersInfo(i, resultChar);

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

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImage.Invalidate(true);
		viewImageRecognize.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage.IsAvailable() && viewImageRecognize.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}