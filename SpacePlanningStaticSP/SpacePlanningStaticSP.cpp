#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();


	// 3D 뷰 선언 // Declare 3D views
	CGUIView3DWrap view3DResult;

	CResult res = EResult_UnknownError;

	do
	{
		// 알고리즘 객체 선언 // Declare algorithm object
		CSpacePlanningStaticSP alg;

		// Bin spec 설정 // Set the bin spec
		CSpacePlanningBaseSP::SBinSpec<float> binSpec = { 12,9,10 };

		if((res = alg.AddBinSpec(binSpec)).IsFail())
		{
			ErrorPrint(res, "Failed to add bin spec.\n");
			break;
		}

		// Item spec 설정 (회전 없음) // Set the item specs (no rotation)
		CSpacePlanningBaseSP::SItemSpec<float> itemSpec1 = { 3, 3, 4, 1, CSpacePlanningBaseSP::ERotationType_NoRotation };
		CSpacePlanningBaseSP::SItemSpec<float> itemSpec2 = { 4, 3, 3, 1, CSpacePlanningBaseSP::ERotationType_NoRotation };
		CSpacePlanningBaseSP::SItemSpec<float> itemSpec3 = { 5, 3, 2, 1, CSpacePlanningBaseSP::ERotationType_NoRotation };

		if((res = alg.AddItemSpec(itemSpec1)).IsFail() ||
		   (res = alg.AddItemSpec(itemSpec2)).IsFail() ||
		   (res = alg.AddItemSpec(itemSpec3)).IsFail())
		{
			ErrorPrint(res, "Failed to add item spec.\n");
			break;
		}

		// Static list 파라미터 설정 // Set the static list parameters
		CSpacePlanningBaseSP::SStaticListParameters params;
		params.itemCounts.Resize(3);
		params.itemCounts[0] = 4;
		params.itemCounts[1] = 3;
		params.itemCounts[2] = 2;

		if((res = alg.SetStaticListParameters(params)).IsFail())
		{
			ErrorPrint(res, "Failed to set static list parameters.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 학습 수행 // Perform learning according to previously set parameters
		if((res = alg.Learn()).IsFail())
		{
			ErrorPrint(res, "Failed to learn.\n");
			break;
		}

		// 배치 결과 3D 오브젝트 그룹 취득 // Get the placement result 3D object group
		// 구조: [0, ItemCount) = 배치된 아이템, [ItemCount, end) = 빈(bin) * 2개씩 (속 채움, 외곽선)
		// Structure: [0, ItemCount) = placed items, [ItemCount, end) = bins * 2 each (filled, wireframe)
		CFL3DObjectGroup flog;

		if((res = alg.Get3DObject(flog)).IsFail())
		{
			ErrorPrint(res, "Failed to get 3D object.\n");
			break;
		}

		const int32_t i32BinCount  = alg.GetBinSpecCount();
		const int32_t i32ItemCount = alg.GetItemSpecCount();

		if((res = view3DResult.Create(600, 0, 1100, 500)).IsFail())
		{
			ErrorPrint(res, "Failed to create the 3D view.\n");
			break;
		}

		view3DResult.SetRenderingTransparencyMode(ERenderingTransparencyMode_DepthPeelingOIT);
		view3DResult.SetRenderingResolutionScale(2);

		// 결과 뷰에 아이템 및 Bin 오브젝트 추가 // Push item and bin objects to the result view
		for(int32_t i = 0; i < i32ItemCount; ++i)
		{
			int32_t i32ObjIndex = 0;
			view3DResult.PushObject(*flog.GetObjectByIndex(i), &i32ObjIndex);
			((CGUIView3DObject*)view3DResult.GetView3DObject(i32ObjIndex))->SetOpacity(0.6f);
		}

		for(int32_t i = 0; i < i32BinCount; ++i)
		{
			int32_t i32ObjIndex = 0;

			view3DResult.PushObject(*flog.GetObjectByIndex(i32ItemCount + 2 * i), &i32ObjIndex);
			((CGUIView3DObject*)view3DResult.GetView3DObject(i32ObjIndex))->SetOpacity(0.2f);

			view3DResult.PushObject(*flog.GetObjectByIndex(i32ItemCount + 2 * i + 1), &i32ObjIndex);
			((CGUIView3DObject*)view3DResult.GetView3DObject(i32ObjIndex))->SetOpacity(0.6f);
		}

		// 화면에 출력하기 위해 3D 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 from the 3D view for display
		// 이 객체는 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to the view and does not need to be released separately
		CGUIView3DLayerWrap layer3DResult = view3DResult.GetLayer(0);

		CFLPoint<double> flpLeftTop(0, 0);
		layer3DResult.DrawTextCanvas(flpLeftTop, L"Result 3D View", YELLOW, BLACK, 20);

		// 결과 정보를 3D 뷰에 텍스트로 표시 // Draw result summary text on the 3D view
		float f32TotalVolume = 0.f, f32UsedVolume = 0.f;
		alg.GetCurrentVolumeUsage(0, f32TotalVolume, f32UsedVolume);
		const float f32VolumeUsage = f32TotalVolume > 0.f ? 100.f * f32UsedVolume / f32TotalVolume : 0.f;

		CFLString<wchar_t> flsResultInfo;
		flsResultInfo.Format(
			L"Optimal strategy index: %d\n"
			L"Volume Usage: %.1f%%(%.1f/%.1f)",
			alg.GetOptimalStrategyIndex(),
			f32VolumeUsage,
			f32UsedVolume,
			f32TotalVolume);

		layer3DResult.DrawTextCanvas(CFLPoint<double>(0, 25), flsResultInfo, YELLOW, BLACK, 16);

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다.
		// With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		view3DResult.ZoomFit();

		// 이미지 뷰를 갱신 합니다. // Update image view
		view3DResult.Invalidate(true);

		// 3D 뷰가 종료될 때 까지 기다림 // Wait for the 3D view to close
		while(view3DResult.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}
