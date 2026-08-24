#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


enum EType
{
	EType_Src0 = 0,
	EType_Src1,
	EType_Src2,
	EType_Src3,
	ETypeCount,
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage arrFliImage[ETypeCount];

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap arrViewImage[ETypeCount];
	bool bError = false;

	do
	{
		CResult res = EResult_UnknownError;

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			CFLString<wchar_t> flsFileName;

			flsFileName.Format(L"../../ExampleImages/Blob/Blob Sort %d.flif", i + 1);

			// 이미지 로드 // Load image
			if(IsFail(res = arrFliImage[i].Load(flsFileName)))
			{
				ErrorPrint(res, "Failed to load the image file.\n");
				break;
			}

			int32_t i32X = i % 2;
			int32_t i32Y = i / 2;

			// 이미지 뷰 생성 // Create image view
			if(IsFail(res = arrViewImage[i].Create(i32X * 400 + 400, i32Y * 400, i32X * 400 + 400 + 400, i32Y * 400 + 400)))
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				bError = true;
				break;
			}

			// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
			if(IsFail(res = arrViewImage[i].SetImagePtr(&arrFliImage[i])))
			{
				ErrorPrint(res, "Failed to set image object on the image view.\n");
				bError = true;
				break;
			}

			if(IsFail(res = arrViewImage[i].ZoomFit()))
			{
				ErrorPrint(res, "Failed to zoom fit\n");
				break;
			}

			// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
			if(i != EType_Src0)
			{
				if(IsFail(res = arrViewImage[EType_Src0].SynchronizePointOfView(&arrViewImage[i])))
				{
					ErrorPrint(res, "Failed to synchronize view\n");
					bError = true;
					break;
				}

				// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
				if(IsFail(res = arrViewImage[EType_Src0].SynchronizeWindow(&arrViewImage[i])))
				{
					ErrorPrint(res, "Failed to synchronize window.\n");
					bError = true;
					break;
				}
			}
		}

		if(bError)
			break;

		for(int64_t k = 0; k < ETypeCount; ++k)
		{
			// Blob 객체 생성 
			// Create Blob object
			CBlobSubsampled blob;

			// 처리할 이미지 설정
			blob.SetSourceImage(arrFliImage[k]);
			// 논리 조건 설정
			blob.SetLogicalCondition(ELogicalCondition_Greater);
			// 임계값 설정,  위의 조건과 아래의 조건이 합쳐지면 100보다 같거나 작은 객체를 검출
			blob.SetThreshold(100);

			// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
			if(IsFail(res = blob.Execute()))
			{
				ErrorPrint(res, "Failed to execute Blob.");
				break;
			}

			// Blob 결과를 얻어오기 위해 FigureArray 선언
			CFLFigureArray flfaSortClusterModeBoundaryRects;

			//////////////////////////////////////////////////////////////////////////
			// SortClusterMode 기능을 이용해서 Blob된 결과에 대해 정렬을 수행
			// 객체의 Center좌표를 바탕으로 1순위 Y오름차순, 2순위 X오름차순으로 정렬을 수행
			//////////////////////////////////////////////////////////////////////////

			// SortClusterMode 함수를 통해 영역을 정렬 (객체의 Center좌표를 바탕으로 1순위 Y오름차순, 2순위 X오름차순)
			if(IsFail(res = blob.SortClusterMode(CBlob::ESortClusterModeMethod_Center_Y_Asc_X_Asc)))
			{
				ErrorPrint(res, "Failed to get sort from the Blob object.");
				break;
			}

			// SortClusterMode한 Blob 결과들 중 Boundary Rectangle 을 얻어옴
			if(IsFail(res = blob.GetResultBoundaryRects(flfaSortClusterModeBoundaryRects)))
			{
				ErrorPrint(res, "Failed to get boundary rects from the Blob object.");
				break;
			}

			// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
			// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately

			CGUIViewImageLayerWrap layerSortClusterMode = arrViewImage[k].GetLayer(0);

			if(IsFail(res = layerSortClusterMode.DrawTextCanvas(&CFLPoint<double>(), L"(Y Asc, X Asc)", YELLOW, BLACK, 30)))
			{
				ErrorPrint(res, "Failed to draw text on the image view.\n");
				break;
			}

			// flfaOriginBoundaryRects 는 Figure들의 배열이기 때문에 Layer에 넣기만 해도 모두 드로윙이 가능하다.
			// 아래 함수 DrawFigureImage는 Image좌표를 기준으로 하는 Figure를 Drawing 한다는 것을 의미하며 // The function DrawFigureImage below means drawing a picture based on the image coordinates
			// 맨 마지막 두개의 파라미터는 불투명도 값이고 1일경우 불투명, 0일경우 완전 투명을 의미한다. // The last two parameters are opacity values, which mean opacity for 1 day and complete transparency for 0 day.
			// 여기서 0.25이므로 옅은 반투명 상태라고 볼 수 있다.
			// 파라미터 순서 : 레이어 -> Figure 객체 -> 선 색 -> 선 두께 -> 면 색 -> 펜 스타일 -> 선 알파값(불투명도) -> 면 알파값 (불투명도) // Parameter order: Layer -> Figure object -> Line color -> Line thickness -> Face color -> Pen style -> Line alpha value (opacity) -> Area alpha value (opacity)			
			if(IsFail(res = layerSortClusterMode.DrawFigureImage(&flfaSortClusterModeBoundaryRects, RED, 1, RED, EGUIViewImagePenStyle_Solid, 1, 0.25)))
			{
				ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
				break;
			}

			// Rect 정보값을 각각 확인하는 코드
			for(int64_t i = 0; i < flfaSortClusterModeBoundaryRects.GetCount(); ++i)
			{
				CFLFigureArray* pFlfa = (CFLFigureArray*)flfaSortClusterModeBoundaryRects[i];
				int64_t i64ObjectCount = pFlfa->GetCount();

				for(int64_t j = 0; j < i64ObjectCount; ++j)
				{
					CFLRect<double>* pFlrRect = (CFLRect<double>*)pFlfa->GetAt(j);

					CFLString<wchar_t> strNumber;
					strNumber.Format(L"(%d,%d)", (int32_t)i, (int32_t)j);

					if(pFlrRect)
						printf("SortClusterMode [%lld] No. (%d,%d) : (%lf,%lf,%lf,%lf)\n", k, (int32_t)i, (int32_t)j, pFlrRect->left, pFlrRect->top, pFlrRect->right, pFlrRect->bottom);

					layerSortClusterMode.DrawTextImage(&CFLPoint<double>(pFlrRect->GetCenter()), (wchar_t*)strNumber.GetString(), CYAN, BLACK, 12, false, 0, EGUIViewImageTextAlignment_CENTER_CENTER);
				}
			}

			// 이미지 뷰를 갱신 합니다. // Update image view
			arrViewImage[k].RedrawWindow();
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		bool bRun = true;

		while(bRun)
		{
			for(int32_t i = 0; i < ETypeCount; ++i)			
				bRun &= arrViewImage[i].IsAvailable();
			
			CThreadUtilities::Sleep(1);
		}

		for(int32_t i = 0; i < ETypeCount; ++i)
			arrViewImage[i].Destroy();
	}
	while(false);

	return 0;
}