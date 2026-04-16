#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

namespace
{
	CResult InitializeCoordinateConverter(const CSpacePlanningBaseSP& alg, CSpacePlanningCoordinateConverterSP& converter)
	{
		CResult res = EResult_UnknownError;

		do
		{
			if((res = alg.GetCoordinateConverter(converter)).IsFail())
				break;

			const int32_t i32BinCount = alg.GetBinSpecCount();
			for(int32_t i = 0; i < i32BinCount; ++i)
			{
				const Base::TPoint3<float> tpWorldPivot(16.f * i, 0.f, 0.f);
				const Base::TPoint3<float> tpBinPivot(0.f, 0.f, 0.f);
				const Base::TPoint3<float> tpDirectionZ(0.03f, 0.f, 1.f);
				const Base::TPoint3<float> tpUpY(0.0f, 1.f, 0.3f);

				if((res = converter.SetBinTransform(i, tpWorldPivot, tpBinPivot, tpDirectionZ, tpUpY)).IsFail())
					break;
			}

			if(res.IsFail())
				break;

			const int32_t i32ItemCount = alg.GetItemSpecCount();
			for(int32_t i = 0; i < i32ItemCount; ++i)
			{
				if((res = converter.SetItemPivotNormalized(i, Base::TPoint3<float>(0.5f, 0.5f, 0.5f))).IsFail())
					break;
			}

			if(res.IsFail())
				break;

			res = converter.Learn();
		}
		while(false);

		return res;
	}
}

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap view3DResult;

	CResult res = EResult_UnknownError;

	do
	{
		// 알고리즘 객체 선언 // Declare algorithm object
		CSpacePlanningDynamicSP alg;

		// Bin spec 설정 // Set the bin spec
		CSpacePlanningBaseSP::SBinSpec<float> binSpec = { 12, 9, 10 };

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

		// Random sequence 파라미터 설정 // Set the random sequence parameters
		// itemChances: 각 아이템 타입의 상대적 출현 비율 // Relative appearance ratio of each item type
		CFLArray<float> itemChances;
		itemChances.Resize(3);
		itemChances[0] = 4;
		itemChances[1] = 3;
		itemChances[2] = 2;

		// Lookahead: 다음 배치 결정 시 고려할 선택지 수 // Number of candidates to consider for next placement
		CSpacePlanningBaseSP::SRandomSequenceParameters params(itemChances, 1);

		if((res = alg.SetRandomSequenceParameters(params)).IsFail())
		{
			ErrorPrint(res, "Failed to set random sequence parameters.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 학습 수행 // Perform learning according to previously set parameters
		if((res = alg.Learn()).IsFail())
		{
			ErrorPrint(res, "Failed to learn.\n");
			break;
		}

		// 학습된 전략 중 최적 전략 선택 // Select the optimal strategy among learned strategies
		const int32_t i32OptimalStrategyIndex = alg.GetOptimalStrategyIndex();

		if((res = alg.SelectStrategy(i32OptimalStrategyIndex)).IsFail())
		{
			ErrorPrint(res, "Failed to select strategy.\n");
			break;
		}

		CSpacePlanningCoordinateConverterSP converter;
		if((res = InitializeCoordinateConverter(alg, converter)).IsFail())
		{
			ErrorPrint(res, "Failed to initialize the coordinate converter.\n");
			break;
		}

		CFL3DObjectGroup flogBins;
		if((res = converter.MakeBinObjectGroup(flogBins)).IsFail())
		{
			ErrorPrint(res, "Failed to build world-space bin objects.\n");
			break;
		}

		wprintf(L"Optimal strategy index: %d\n", i32OptimalStrategyIndex);

		// 3D 뷰 생성 // Create 3D view
		if((res = view3DResult.Create(600, 0, 1200, 600)).IsFail())
		{
			ErrorPrint(res, "Failed to create the 3D view.\n");
			break;
		}

		view3DResult.SetRenderingTransparencyMode(ERenderingTransparencyMode_DepthPeelingOIT);
		view3DResult.SetRenderingResolutionScale(2);

		const int32_t i32BinCount = alg.GetBinSpecCount();
		const int32_t i32ItemCount = alg.GetItemSpecCount();

		// 타이틀은 layer 0에 한 번만 그림 // Draw title once on layer 0
		// 매 스텝마다 갱신되는 상태 텍스트는 layer 1을 Clear 후 재작성 // Per-step status goes on layer 1, cleared each step
		view3DResult.GetLayer(0).DrawTextCanvas(CFLPoint<double>(0, 0), L"Dynamic SP - Interactive Placement", YELLOW, BLACK, 20);

		// 인터랙티브 모드 실행 // Run in interactive mode
		if((res = alg.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute the algorithm.\n");
			break;
		}

		CXorshiroRandomGenerator rng;
		rng.Seed();

		CSpacePlanningBaseSP::CFLPlacementInfoArray placements;

		// 아이템 도착 시뮬레이션 (컨베이어 벨트 상황) // Simulate item arrival (conveyor belt scenario)
		// 아이템 타입을 무작위로 생성하여 빈이 꽉 찰 때까지 계속 배치
		// Randomly generate item types and keep placing until the bin is full (EResult_FullOfCapacity)
		int32_t i32ArrivalIdx  = 0;
		int32_t i32PlacedCount = 0;

		for(;;)
		{
			if(!view3DResult.IsAvailable())
				break;

			const int32_t i32ItemType = rng.GenerateUniformRandomValueI32(0, i32ItemCount - 1);

			// 아이템을 대기열에 추가하고 권장 위치에 자동 배치
			// Push item to queue and automatically place it at the recommended position
			CSpacePlanningBaseSP::SPlacementInfo placement;

			res = alg.PushAndPlace(i32ItemType, true, placement);

			++i32ArrivalIdx;

			if(res.IsFail())
			{
				if(res == EResult_FullOfCapacity)
				{
					// 빈이 꽉 참 — 정상 종료 // Bin is full — normal termination
					wprintf(L"Arrival %d: bin is full. Stopping.\n", i32ArrivalIdx);
					break;
				}
				// 예상치 못한 오류 // Unexpected error
				ErrorPrint(res, "Failed to push and place.\n");
				break;
			}

			++i32PlacedCount;
			placements.PushBack(placement);

			Base::TPoint3<float> tpWorldPosition;
			if((res = converter.Convert(placement, tpWorldPosition)).IsFail())
			{
				ErrorPrint(res, "Failed to convert placement coordinates.\n");
				break;
			}

			wprintf(L"Arrival %d: placed item type %d at bin %d (rotation=%d, bin pos=[%.1f, %.1f, %.1f], world center=[%.1f, %.1f, %.1f])\n",
				i32ArrivalIdx, placement.i32ItemIndex, placement.i32BinIndex,
				static_cast<int32_t>(placement.eRotation),
				placement.tpPosition.x, placement.tpPosition.y, placement.tpPosition.z,
				tpWorldPosition.x, tpWorldPosition.y, tpWorldPosition.z);

			CFL3DObjectGroup flogItems;
			if((res = converter.MakeItemObjectGroup(placements, flogItems)).IsFail())
			{
				ErrorPrint(res, "Failed to build world-space item objects.\n");
				break;
			}

			view3DResult.Lock();
			// 매 스텝마다 world-space 오브젝트로 뷰를 재구성
			// Rebuild the view with world-space objects on each step
			view3DResult.ClearObjects();

			for(int32_t i = 0; i < i32PlacedCount; ++i)
			{
				int32_t i32ObjIndex = 0;
				view3DResult.PushObject(*flogItems.GetObjectByIndex(i), &i32ObjIndex);
				((CGUIView3DObject*)view3DResult.GetView3DObject(i32ObjIndex))->SetOpacity(0.6f);
			}

			for(int32_t i = 0; i < i32BinCount; ++i)
			{
				int32_t i32ObjIndex = 0;
				view3DResult.PushObject(*flogBins.GetObjectByIndex(i), &i32ObjIndex);
				((CGUIView3DObject*)view3DResult.GetView3DObject(i32ObjIndex))->SetOpacity(0.2f);
			}

			// 상태 텍스트는 layer 1에 매 스텝 Clear 후 재작성 // Clear layer 1 each step and redraw status text
			// 이 객체는 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to the view and does not need to be released separately
			CGUIView3DLayerWrap layer3DStatus = view3DResult.GetLayer(1);
			layer3DStatus.Clear();

			float f32TotalVolume = 0.f, f32UsedVolume = 0.f;
			alg.GetCurrentVolumeUsage(0, f32TotalVolume, f32UsedVolume);
			const float f32VolumeUsage = f32TotalVolume > 0.f ? 100.f * f32UsedVolume / f32TotalVolume : 0.f;

			CFLString<wchar_t> flsStatus;
			flsStatus.Format(L"Arrival %d  |  Placed: %d  |  Volume Usage: %.1f%% (%.1f / %.1f)\nWorld-space rendering enabled", i32ArrivalIdx, i32PlacedCount, f32VolumeUsage, f32UsedVolume, f32TotalVolume);

			layer3DStatus.DrawTextCanvas(CFLPoint<double>(0, 25), flsStatus, YELLOW, BLACK, 16);

			// 첫 아이템 배치 시 카메라를 전체에 맞게 조정 // Fit camera to all objects on first placement
			if(i32PlacedCount == 1)
				view3DResult.ZoomFit();

			view3DResult.Unlock();
			// 이미지 뷰를 갱신 합니다. // Update image view
			view3DResult.Invalidate(true);

			// 배치 과정을 눈으로 확인할 수 있도록 잠시 대기 // Pause briefly so the placement process is visible
			CThreadUtilities::Sleep(600);
		}

		if(!view3DResult.IsAvailable())
			break;

		// 최종 결과 요약 출력 // Print final result summary
		{
			float f32TotalVolume = 0.f, f32UsedVolume = 0.f;
			alg.GetCurrentVolumeUsage(0, f32TotalVolume, f32UsedVolume);
			const float f32VolumeUsage = f32TotalVolume > 0.f ? 100.f * f32UsedVolume / f32TotalVolume : 0.f;

			CGUIView3DLayerWrap layer3DStatus = view3DResult.GetLayer(1);
			layer3DStatus.Clear();

			CFLString<wchar_t> flsFinalInfo;
			flsFinalInfo.Format(L"Done  |  Arrivals: %d  |  Placed: %d  |  Volume Usage: %.1f%% (%.1f / %.1f)\nWorld-space rendering enabled",i32ArrivalIdx, i32PlacedCount,f32VolumeUsage, f32UsedVolume, f32TotalVolume);

			layer3DStatus.DrawTextCanvas(CFLPoint<double>(0, 25), flsFinalInfo, YELLOW, BLACK, 16);

			view3DResult.Invalidate(true);
		}

		// 3D 뷰가 종료될 때 까지 기다림 // Wait for the 3D view to close
		while(view3DResult.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}
