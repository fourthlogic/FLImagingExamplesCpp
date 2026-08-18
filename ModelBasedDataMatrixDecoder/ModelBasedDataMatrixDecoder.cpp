#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CResult res;

	do
	{
		enum EViewList
		{
			EViewList_Learn,
			EViewList_ModelBaseFind,
			EViewList_NormalFind,

			EViewList_Count
		};

		// Declaration of the image view 
		CGUIViewImageWrap arrViewImage[EViewList_Count];

		CFLImage arrFliImage[EViewList_Count];
		CGUIViewImageLayerWrap arrLayer[EViewList_Count];


		const wchar_t* arrWcsViewText[EViewList_Count] =
		{
			L"Learn View",
			L"Model Based Decoder Result View",
			L"Decoder Result View",
		};

		// 이미지 로드 // Loads image
		if(IsFail(res = arrFliImage[EViewList_Learn].Load(L"../../ExampleImages/DataMatrix/Learn.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = arrFliImage[EViewList_ModelBaseFind].Load(L"../../ExampleImages/DataMatrix/Find.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		if(IsFail(res = arrFliImage[EViewList_NormalFind].Load(L"../../ExampleImages/DataMatrix/Find.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		// Create the image view 
		const int32_t i32ViewSize = 450;
		const int32_t i32Start = 100;

		for(int32_t i = 0; i < EViewList_Count; ++i)
		{
			int32_t i32X = i32ViewSize * i;
			int32_t i32Y = i32Start;

			if(IsFail(res = arrViewImage[i].Create(i32X + i32Start, i32Y, i32X + i32ViewSize + i32Start, i32Y + i32ViewSize)))
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				break;
			}

			// 이미지 뷰에 이미지를 디스플레이 // Display the image in the imageview
			if(IsFail(res = arrViewImage[i].SetImagePtr(&arrFliImage[i])))
			{
				ErrorPrint(res, "Failed to set image object on the image view.\n");
				break;
			}

			// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
			// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
			arrLayer[i] = arrViewImage[i].GetLayer(0);

			if(IsFail(res = arrLayer[i].DrawTextCanvas(CFLPoint<double>(0., 0.), arrWcsViewText[i], YELLOW, BLUE, 20)))
			{
				ErrorPrint(res, "Failed to draw figure\n");
				break;
			}
		}

		// 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the all image views. 
		if(IsFail(res = arrViewImage[EViewList_ModelBaseFind].SynchronizePointOfView(&arrViewImage[EViewList_NormalFind])))
		{
			ErrorPrint(res, "Failed to set image object on the image view.\n");
			break;
		}

		// Model Based Data Matrix Decoder 객체 생성 // Create Model Based Data Matrix Decoder object
		CModelBasedDataMatrixDecoder modelBasedDataMatrixDecoder;

		// 학습 이미지 설정 // Sets the learn image.
		modelBasedDataMatrixDecoder.SetLearnImage(arrFliImage[EViewList_Learn]);
		// 코드 색상 설정 // Sets the code color.
		modelBasedDataMatrixDecoder.SetColorMode(EDataCodeColor_WhiteOnBlack);

		// 학습 동작 // Learn
		if((res = modelBasedDataMatrixDecoder.Learn()).IsFail())
		{
			ErrorPrint(res, "Failed to learn data matrix decoder.");
			break;
		}

		// 동작 이미지 설정 // Set source image
		modelBasedDataMatrixDecoder.SetSourceImage(arrFliImage[EViewList_ModelBaseFind]);
		// 디코딩 결과 개수 설정 // Sets the number of decoding results.
		modelBasedDataMatrixDecoder.SetDetectingCount(EDataCodeDecoderDetectingCount_All);
		modelBasedDataMatrixDecoder.SetMaximumDetectingCount(3);

		// 학습 이미지 기준 탐색 각도 설정 // Sets the search angle relative to the learn data.
		modelBasedDataMatrixDecoder.SetAngleTolerance(30);

		// 동작 // Execute
		if((res = modelBasedDataMatrixDecoder.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute data matrix decoder.");
			break;
		}


		// Learn 동작 결과를 얻어온다 // Gets the result of the learn result.
		CModelBasedDataMatrixDecoder::CDataMatrixLearnInformation datamatrixLearnInfo;
		modelBasedDataMatrixDecoder.GetLearnResult(datamatrixLearnInfo);
		{
			wprintf(L"\n[Model Based Learn Result]\n");

			CFLQuad<double>& flqLearnedCodeRegion = datamatrixLearnInfo.decodedDataMatrixInformation.flqRegion;
			CFLString<wchar_t>& flsLearnedCode = datamatrixLearnInfo.decodedDataMatrixInformation.flsCode;

			// Learn 동작 결과 영역 및 코드 출력 // Outputs the regions and codes from the learn operation results.
			arrLayer[EViewList_Learn].DrawFigureImage(flqLearnedCodeRegion, LIME, 2);
			arrLayer[EViewList_Learn].DrawTextImage(flqLearnedCodeRegion.flpPoints[3], flsLearnedCode, CYAN, BLACK, 20, false, flqLearnedCodeRegion.flpPoints[3].GetAngle(flqLearnedCodeRegion.flpPoints[2]));

			wprintf(L"Code Size : %d x %d\n", datamatrixLearnInfo.i32Rows, datamatrixLearnInfo.i32Cols);

			CFLString<wchar_t> flsCodeColor;

			if(datamatrixLearnInfo.eColor == EDataCodeColor_BlackOnWhite)
				flsCodeColor = L"Black On White";
			else if(datamatrixLearnInfo.eColor == EDataCodeColor_WhiteOnBlack)
				flsCodeColor = L"White On Black";

			wprintf(L"Code Color : %s\n", flsCodeColor.GetString());

			CFLString<wchar_t> flsFlip = datamatrixLearnInfo.bFlip ? L"Yes" : L"No";

			wprintf(L"Flip : %s\n", flsFlip.GetString());
			wprintf(L"Code : %s\n", flsLearnedCode.GetString());
		}

		// Data Matrix Decoder 결과 개수를 얻는다. // Gets the number of results from the Data Matrix decoder.
		int64_t i64Results = modelBasedDataMatrixDecoder.GetResultCount();

		wprintf(L"\n[Model Based Decoded Result]\n");
		for(int64_t i = 0; i < i64Results; ++i)
		{
			CFLQuad<double> flqDecodedCodeRegion;
			CFLString<wchar_t> flsDecodedCode;

			// Data Matrix Decoder 결과들 중 Data Region 을 얻어옴 // Gets the Data Region from the results of the Data Matrix decoder.
			if(IsFail(res = modelBasedDataMatrixDecoder.GetResultDataRegion(i, flqDecodedCodeRegion)))
			{
				ErrorPrint(res, "Failed to get data region from the data matrix decoder object.");
				continue;
			}

			// Data Matrix Decoder 결과들 중 Decoded String 을 얻어옴 // Gets the decoded string from the results of the Data Matrix decoder.
			if(IsFail(res = modelBasedDataMatrixDecoder.GetResultDecodedString(i, flsDecodedCode)))
			{
				ErrorPrint(res, "Failed to get decoded string from the data matrix decoder object.");
				continue;
			}

			wprintf(L"No. %lld Code : ", i);
			wprintf(L"%s\n", flsDecodedCode.GetString());

			arrLayer[EViewList_ModelBaseFind].DrawFigureImage(flqDecodedCodeRegion, LIME, 2);
			arrLayer[EViewList_ModelBaseFind].DrawTextImage(flqDecodedCodeRegion.flpPoints[3], flsDecodedCode, CYAN, BLACK, 16, false, flqDecodedCodeRegion.flpPoints[3].GetAngle(flqDecodedCodeRegion.flpPoints[2]));
		}

		// 일반 Data Matrix Decoder 결과와 비교하기 위한 동작 // Operation for comparing with standard Data Matrix decoder results.

		// Data Matrix Decoder 객체 생성 // Create Data Matrix Decoder object
		CDataMatrixDecoder datamatrixDecoder;

		// 동작 이미지 설정 // Set source image
		datamatrixDecoder.SetSourceImage(arrFliImage[EViewList_NormalFind]);
		// 코드 색상 설정 // Sets the code color.
		datamatrixDecoder.SetColorMode(EDataCodeColor_WhiteOnBlack);
		// 디코딩 결과 개수 설정 // Sets the number of decoding results.
		datamatrixDecoder.SetDetectingCount(EDataCodeDecoderDetectingCount_All);
		datamatrixDecoder.SetMaximumDetectingCount(3);

		// 동작 // Execute
		if((res = datamatrixDecoder.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute data matrix decoder.");
			break;
		}

		// Data Matrix Decoder 결과 개수를 얻는다. // Gets the number of results from the Data Matrix decoder.
		i64Results = datamatrixDecoder.GetResultCount();

		wprintf(L"\n[Normal Decoded Result]\n");
		for(int64_t i = 0; i < i64Results; ++i)
		{
			CFLQuad<double> flqDecodedCodeRegion;
			CFLString<wchar_t> flsDecodedCode;

			// Data Matrix Decoder 결과들 중 Data Region 을 얻어옴 // Gets the Data Region from the results of the Data Matrix decoder.
			if(IsFail(res = datamatrixDecoder.GetResultDataRegion(i, flqDecodedCodeRegion)))
			{
				ErrorPrint(res, "Failed to get data region from the data matrix decoder object.");
				continue;
			}

			// Data Matrix Decoder 결과들 중 Decoded String 을 얻어옴 // Gets the decoded string from the results of the Data Matrix decoder.
			if(IsFail(res = datamatrixDecoder.GetResultDecodedString(i, flsDecodedCode)))
			{
				ErrorPrint(res, "Failed to get decoded string from the data matrix decoder object.");
				continue;
			}

			wprintf(L"No. %lld Code : ", i);
			wprintf(L"%s\n", flsDecodedCode.GetString());

			arrLayer[EViewList_NormalFind].DrawFigureImage(flqDecodedCodeRegion, LIME, 2);
			arrLayer[EViewList_NormalFind].DrawTextImage(flqDecodedCodeRegion.flpPoints[3], flsDecodedCode, CYAN, BLACK, 16, false, flqDecodedCodeRegion.flpPoints[3].GetAngle(flqDecodedCodeRegion.flpPoints[2]));
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		for(int32_t i = 0; i < EViewList_Count; ++i)
			arrViewImage[i].Invalidate();

		// The image view is waiting until close.
		while(arrViewImage[EViewList_Learn].IsAvailable() && arrViewImage[EViewList_ModelBaseFind].IsAvailable() && arrViewImage[EViewList_NormalFind].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}