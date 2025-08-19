#include <cstdio>
#include <locale.h>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	setlocale(LC_ALL, "Korean");

	// 이미지 객체 선언 // Declare image object
	CFLImage fliImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage;

	do
	{
		CResult res = EResult_UnknownError;

		// 문자열을 입력 받을 wchar_t 형 배열 선언
		wchar_t arrWcEncoding[4096] = { 0, };

		printf("Please input encoding message.: ");

		// 문자열을 입력 받는다
		fgetws(arrWcEncoding, 4096, stdin);

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage.Create(400, 0, 1424, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		int32_t i32StrLen = (int32_t)wcslen(arrWcEncoding);

		if(i32StrLen >= 1)
		{
			if(arrWcEncoding[i32StrLen - 1] == '\n')
			{
				if(i32StrLen >= 2 && arrWcEncoding[wcslen(arrWcEncoding) - 2] == '\r' )				
					arrWcEncoding[wcslen(arrWcEncoding) - 2] = '\0';

				arrWcEncoding[wcslen(arrWcEncoding) - 1] = '\0';
			}
			else if(arrWcEncoding[wcslen(arrWcEncoding) - 1] == '\r')
				arrWcEncoding[wcslen(arrWcEncoding) - 1] = '\0';
		}

		// Data Matrix Encoder 객체 선언
		CDataMatrixEncoder dataMatrixEncoder;
		CDataMatrixSpec codeSpec;

		// 처리할 이미지 설정
		dataMatrixEncoder.SetSourceImage(fliImage);
		// Encoding Message 설정
		dataMatrixEncoder.SetEncodingMessage(CFLString<wchar_t>(arrWcEncoding));
		// Encoder 데이터 영역 색상 설정
		// EDataCodeColor_Auto 로 설정 시 BlackOnWhite와 동일한 옵션으로 처리된다.
		codeSpec.SetColorMode(EDataCodeColor_WhiteOnBlack);

		// Encoding 스펙 설정
		// 설정하지 않을 시에는 기본 값으로 동작한다
		dataMatrixEncoder.SetDataMatrixEncodingSpec(codeSpec);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = dataMatrixEncoder.Execute()))
		{
			ErrorPrint(res, "Failed to execute data matrix encoder.");
			
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		if(IsFail(res = viewImage.SetImagePtr(&fliImage)))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Data Matrix Decoder 객체 생성 // Create Data Matrix Decoder object
		CDataMatrixDecoder dataMatrixDecoder;

		// 처리할 이미지 설정
		dataMatrixDecoder.SetSourceImage(fliImage);

		// Decode 데이터 영역 색상 설정
		// EDataCodeColor_Auto 로 설정 시 자동으로 Decode 된다.
		dataMatrixDecoder.SetColorMode(EDataCodeColor_WhiteOnBlack);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = dataMatrixDecoder.Execute()))
		{
			ErrorPrint(res, "Failed to execute data matrix decoder.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer = viewImage.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer.Clear();

		// Data Matrix Decoder 결과 개수를 얻는다.
		int64_t i64Results = dataMatrixDecoder.GetResultCount();

		for(int64_t i = 0; i < i64Results; ++i)
		{
			// Data Matrix Decoder 결과를 얻어오기 위해 FLQuadD 선언
			CFLQuad<double> flqdRegion;

			// Data Matrix Decoder 결과들 중 Data Region 을 얻어옴
			if(IsFail(res = dataMatrixDecoder.GetResultDataRegion(i, flqdRegion)))
			{
				ErrorPrint(res, "Failed to get data region from the data matrix decoder object.");
				continue;
			}
		
			// Data Matrix 의 영역을 디스플레이 한다.
			// FLImaging의 Figure 객체들은 어떤 도형모양이든 상관없이 하나의 함수로 디스플레이가 가능 // FLimaging's Figure objects can be displayed as a function regardless of the shape
			// 아래 함수 DrawFigureImage는 Image좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
			// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일경우 불투명, 0일경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
			// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)
			if(IsFail(res = layer.DrawFigureImage(&flqdRegion, LIME, 2)))
			{
				ErrorPrint(res, "Failed to draw figure object on the image view.\n");
				continue;
			}

			// Data Matrix Decoder 결과를 얻어오기 위해 FigureArray 선언
			CFLFigureArray flfaGridRegion;

			// Data Matrix Decoder 결과들 중 Grid Region 을 얻어옴
			if(IsFail(res = dataMatrixDecoder.GetResultGridRegion(i, flfaGridRegion)))
			{
				ErrorPrint(res, "Failed to get grid region from the data matrix decoder object.");
				continue;
			}

			// Data Matrix 의 Grid Region 을 디스플레이 한다.
			if(IsFail(res = layer.DrawFigureImage(&flfaGridRegion, LIME, 2)))
			{
				ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
				continue;
			}

			// Data Matrix Decoder 결과를 얻어오기 위해 FLStringW 선언
			CFLString<wchar_t> flstrDecoded;

			// Data Matrix Decoder 결과들 중 Decoded String 을 얻어옴
			if(IsFail(res = dataMatrixDecoder.GetResultDecodedString(i, flstrDecoded)))
			{
				ErrorPrint(res, "Failed to get decoded string from the data matrix decoder object.");
				continue;
			}

			// String 을 디스플레이 하기 위한 기준 좌표 FLPointL 선언
			CFLPoint<int32_t> flplPos(flqdRegion.flpPoints[3]);

			// Decoded String 을 디스플레이 한다.
			// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
			// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
			// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
			//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
			// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
			//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
			if(IsFail(res = layer.DrawTextImage(&flplPos, flstrDecoded, CYAN, BLACK, 20)))
			{
				ErrorPrint(res, "Failed to draw string object on the image view.\n");
				continue;
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