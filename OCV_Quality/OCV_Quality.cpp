#include <cstdio>
#include <iostream>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliImage1;
	CFLImage fliImage2;
	CFLImage fliImage3;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage1;
	CGUIViewImageWrap viewImage2;
	CGUIViewImageWrap viewImage3;
	CResult res;

	do
	{
		// 이미지 로드 // Load image
		if((res = fliImage1.Load(L"../../ExampleImages/OCV/A.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		if((res = fliImage2.Load(L"../../ExampleImages/OCV/A_Demaged1.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		if((res = fliImage3.Load(L"../../ExampleImages/OCV/A_Demaged2.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if((res = viewImage1.Create(200, 0, 712, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		if((res = viewImage2.Create(712, 0, 1224, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		if((res = viewImage3.Create(1224, 0, 1736, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if((res = viewImage1.SetImagePtr(&fliImage1)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		if((res = viewImage2.SetImagePtr(&fliImage2)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		if((res = viewImage3.SetImagePtr(&fliImage3)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if((res = viewImage1.SynchronizeWindow(&viewImage2)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		if((res = viewImage1.SynchronizeWindow(&viewImage3)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((res = viewImage1.SynchronizePointOfView(&viewImage2)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		if((res = viewImage1.SynchronizePointOfView(&viewImage3)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer1 = viewImage1.GetLayer(0);
		CGUIViewImageLayerWrap layer2 = viewImage2.GetLayer(1);
		CGUIViewImageLayerWrap layer3 = viewImage3.GetLayer(2);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer1.Clear();
		layer2.Clear();
		layer3.Clear();

		// OCV 객체 생성 // Create OCV object
		COCV ocv;

		// OCR Font 파일을 로드
		if(IsFail(res = ocv.LoadFontData(L"../../ExampleImages/OCV/A.flocr")))
		{
			ErrorPrint(res, L"Failed to load Font file.");
			break;
		}

		// 문자를 검증할 이미지 설정
		if(IsFail(res = ocv.SetSourceImage(fliImage1)))
		{
			ErrorPrint(res, L"Failed to set Source Image.");
			break;
		}

		// 이미지에서 문자를 검증하는 기능을 수행
		if(IsFail(res = ocv.Execute()))
		{
			ErrorPrint(res, res.GetString());
			break;
		}

		if((res = layer1.DrawTextCanvas(&CFLPoint<double>(0, 0), ocv.GetResultVerificationState() == COCV::EVerificationState_OK ? L"Verify" : L"Fail", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text");
			break;
		}

		// 찾은 문자의 개수를 받아오는 함수
		int64_t i64ResultCount = ocv.GetResultCount();

		// 찾은 문자의 정보를 받아올 컨테이너
		COCV::COCVVerificationCharacterInfo resultChar;

		for(int64_t i = 0; i < i64ResultCount; ++i)
		{
			ocv.GetResultVerificationCharactersInfo(i, resultChar);

			CFLString<wchar_t> flsResultString;
			CFLString<wchar_t> flsResultString2 = L"";
			CFLRect<double> flrBoundary = resultChar.flrBoundary;
			CFLLine<double> fllBlankSpaceWidth = resultChar.fllBlankSpaceWidthLine;

			flsResultString.Format(L"[%s] Quality: %d%%\nScale: %.2lf\nAngle: %.1lf\nLighting: %.2lf\nContrast: %.2lf", resultChar.flfaCharacter.GetName(), (int32_t)(resultChar.f64Quality * 100), resultChar.f64ScaleWidth * resultChar.f64ScaleHeight, resultChar.f64Rotation, resultChar.f64Lighting, resultChar.f64Contrast);
			flsResultString2.Format(L"Space Width: %.2lf", resultChar.f64BlankSpaceWidth);

			if(IsFail(layer1.DrawTextImage(CFLPoint<double>(flrBoundary.left, flrBoundary.top), flsResultString, YELLOW, BLACK, 10, false, 0., EGUIViewImageTextAlignment_LEFT_BOTTOM)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			if(IsFail(layer1.DrawFigureImage(resultChar.flfaCharacter, LIME, 1, LIME, EGUIViewImagePenStyle_Solid, 1.f, 0.35f)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			if(IsFail(layer1.DrawFigureImage(flrBoundary, resultChar.bVerified ? GREEN : RED, 3, resultChar.bVerified ? GREEN : RED, EGUIViewImagePenStyle_Solid, 1.f, 0.0f)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			layer1.DrawFigureImage(resultChar.flfaIntrusion, YELLOW, 1, YELLOW, EGUIViewImagePenStyle_Solid, 1.f, 0.3f);
			layer1.DrawFigureImage(resultChar.flfaExtrusion, BLUE, 1, BLUE, EGUIViewImagePenStyle_Solid, 1.f, 0.3f);

			if(IsFail(layer1.DrawFigureImage(fllBlankSpaceWidth, BLACK, 3, BLACK, EGUIViewImagePenStyle_Solid, 1.f, 0.35f)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			if(IsFail(layer1.DrawTextImage(CFLPointArray(fllBlankSpaceWidth)[0], flsResultString2, YELLOW, BLACK, 10, false, 0., EGUIViewImageTextAlignment_LEFT_BOTTOM)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}
		}

		// 문자를 검증할 이미지 설정
		if(IsFail(res = ocv.SetSourceImage(fliImage2)))
		{
			ErrorPrint(res, L"Failed to set Source Image.");
			break;
		}

		// 이미지에서 문자를 검증하는 기능을 수행
		if(IsFail(res = ocv.Execute()))
		{
			ErrorPrint(res, res.GetString());
			break;
		}

		if((res = layer2.DrawTextCanvas(&CFLPoint<double>(0, 0), ocv.GetResultVerificationState() == COCV::EVerificationState_OK ? L"Verify" : L"Fail", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text");
			break;
		}

		// 찾은 문자의 개수를 받아오는 함수
		i64ResultCount = ocv.GetResultCount();

		for(int64_t i = 0; i < i64ResultCount; ++i)
		{
			ocv.GetResultVerificationCharactersInfo(i, resultChar);

			CFLString<wchar_t> flsResultString;
			CFLString<wchar_t> flsResultString2 = L"";
			CFLRect<double> flrBoundary = resultChar.flrBoundary;
			CFLLine<double> fllBlankSpaceWidth = resultChar.fllBlankSpaceWidthLine;

			flsResultString.Format(L"[%s] Quality: %d%%\nScale: %.2lf\nAngle: %.1lf\nLighting: %.2lf\nContrast: %.2lf", resultChar.flfaCharacter.GetName(), (int32_t)(resultChar.f64Quality * 100), resultChar.f64ScaleWidth * resultChar.f64ScaleHeight, resultChar.f64Rotation, resultChar.f64Lighting, resultChar.f64Contrast);
			flsResultString2.Format(L"Space Width: %.2lf", resultChar.f64BlankSpaceWidth);

			if(IsFail(layer2.DrawTextImage(CFLPoint<double>(flrBoundary.left, flrBoundary.top), flsResultString, YELLOW, BLACK, 10, false, 0., EGUIViewImageTextAlignment_LEFT_BOTTOM)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			if(IsFail(layer2.DrawFigureImage(resultChar.flfaCharacter, LIME, 1, LIME, EGUIViewImagePenStyle_Solid, 1.f, 0.35f)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			if(IsFail(layer2.DrawFigureImage(flrBoundary, resultChar.bVerified ? GREEN : RED, 3, resultChar.bVerified ? GREEN : RED, EGUIViewImagePenStyle_Solid, 1.f, 0.0f)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			layer2.DrawFigureImage(resultChar.flfaIntrusion, YELLOW, 1, YELLOW, EGUIViewImagePenStyle_Solid, 1.f, 0.3f);
			layer2.DrawFigureImage(resultChar.flfaExtrusion, BLUE, 1, BLUE, EGUIViewImagePenStyle_Solid, 1.f, 0.3f);

			if(IsFail(layer2.DrawFigureImage(fllBlankSpaceWidth, BLACK, 3, BLACK, EGUIViewImagePenStyle_Solid, 1.f, 0.35f)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			if(IsFail(layer2.DrawTextImage(CFLPointArray(fllBlankSpaceWidth)[0], flsResultString2, YELLOW, BLACK, 10, false, 0., EGUIViewImageTextAlignment_LEFT_BOTTOM)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}
		}

		// 문자를 검증할 이미지 설정
		if(IsFail(res = ocv.SetSourceImage(fliImage3)))
		{
			ErrorPrint(res, L"Failed to set Source Image.");
			break;
		}

		// 이미지에서 문자를 검증하는 기능을 수행
		if(IsFail(res = ocv.Execute()))
		{
			ErrorPrint(res, res.GetString());
			break;
		}

		if((res = layer3.DrawTextCanvas(&CFLPoint<double>(0, 0), ocv.GetResultVerificationState() == COCV::EVerificationState_OK ? L"Verify" : L"Fail", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text");
			break;
		}

		// 찾은 문자의 개수를 받아오는 함수
		i64ResultCount = ocv.GetResultCount();

		for(int64_t i = 0; i < i64ResultCount; ++i)
		{
			ocv.GetResultVerificationCharactersInfo(i, resultChar);

			CFLString<wchar_t> flsResultString;
			CFLString<wchar_t> flsResultString2 = L"";
			CFLRect<double> flrBoundary = resultChar.flrBoundary;
			CFLLine<double> fllBlankSpaceWidth = resultChar.fllBlankSpaceWidthLine;

			flsResultString.Format(L"[%s] Quality: %d%%\nScale: %.2lf\nAngle: %.1lf\nLighting: %.2lf\nContrast: %.2lf", resultChar.flfaCharacter.GetName(), (int32_t)(resultChar.f64Quality * 100), resultChar.f64ScaleWidth * resultChar.f64ScaleHeight, resultChar.f64Rotation, resultChar.f64Lighting, resultChar.f64Contrast);
			flsResultString2.Format(L"Space Width: %.2lf", resultChar.f64BlankSpaceWidth);

			if(IsFail(layer3.DrawTextImage(CFLPoint<double>(flrBoundary.left, flrBoundary.top), flsResultString, YELLOW, BLACK, 10, false, 0., EGUIViewImageTextAlignment_LEFT_BOTTOM)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			if(IsFail(layer3.DrawFigureImage(resultChar.flfaCharacter, LIME, 1, LIME, EGUIViewImagePenStyle_Solid, 1.f, 0.35f)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			if(IsFail(layer3.DrawFigureImage(flrBoundary, resultChar.bVerified ? GREEN : RED, 3, resultChar.bVerified ? GREEN : RED, EGUIViewImagePenStyle_Solid, 1.f, 0.0f)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			layer3.DrawFigureImage(resultChar.flfaIntrusion, YELLOW, 1, YELLOW, EGUIViewImagePenStyle_Solid, 1.f, 0.3f);
			layer3.DrawFigureImage(resultChar.flfaExtrusion, BLUE, 1, BLUE, EGUIViewImagePenStyle_Solid, 1.f, 0.3f);

			if(IsFail(layer3.DrawFigureImage(fllBlankSpaceWidth, BLACK, 3, BLACK, EGUIViewImagePenStyle_Solid, 1.f, 0.35f)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}

			if(IsFail(layer3.DrawTextImage(CFLPointArray(fllBlankSpaceWidth)[0], flsResultString2, YELLOW, BLACK, 10, false, 0., EGUIViewImageTextAlignment_LEFT_BOTTOM)))
			{
				printf("Failed to draw recognized character : %lld", i);
				break;
			}
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImage1.Invalidate(true);
		viewImage2.Invalidate(true);
		viewImage3.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage1.IsAvailable() && viewImage2.IsAvailable() && viewImage3.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}