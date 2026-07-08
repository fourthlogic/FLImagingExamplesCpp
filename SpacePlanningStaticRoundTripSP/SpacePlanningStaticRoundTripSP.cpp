#include <cstdio>
#include <vector>
#include <functional>
#include <sys/stat.h>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

namespace SpacePlanning = FLImaging::ThreeDim::SpacePlanning;

// SpacePlanningStaticSP 고급 예제: top item 만 한 개씩 반대편 상자로 옮긴 뒤 다시 되돌립니다.
// Advanced SpacePlanningStaticSP example: move only top-pickable items to the other bin and back.
// 일부 물품은 XZ 평면 회전, 일부 물품은 모든 축 회전을 허용합니다.
// Some items allow XZ-plane rotation, and one item allows full axis rotation.

namespace
{
	// 배치된 물품 하나. tpMin/tpMax 는 회전 적용 후 영역 // Placed item; tpMin/tpMax store rotated bounds
	struct SItemInstance
	{
		int32_t i32ItemSpecIndex;
		SpacePlanning::EAxisRotation eRotation;
		Base::TPoint3<float> tpMin;
		Base::TPoint3<float> tpMax;
	};

	Base::TPoint3<float> GetRotatedItemSize(const SpacePlanning::SItemSpec<float>& itemSpec, SpacePlanning::EAxisRotation eRotation)
	{
		switch(eRotation)
		{
		case SpacePlanning::EAxisRotation_XYZ:
			return Base::TPoint3<float>(itemSpec.width, itemSpec.height, itemSpec.depth);
		case SpacePlanning::EAxisRotation_ZYX:
			return Base::TPoint3<float>(itemSpec.depth, itemSpec.height, itemSpec.width);
		case SpacePlanning::EAxisRotation_XZY:
			return Base::TPoint3<float>(itemSpec.width, itemSpec.depth, itemSpec.height);
		case SpacePlanning::EAxisRotation_ZXY:
			return Base::TPoint3<float>(itemSpec.depth, itemSpec.width, itemSpec.height);
		case SpacePlanning::EAxisRotation_YXZ:
			return Base::TPoint3<float>(itemSpec.height, itemSpec.width, itemSpec.depth);
		case SpacePlanning::EAxisRotation_YZX:
			return Base::TPoint3<float>(itemSpec.height, itemSpec.depth, itemSpec.width);
		}

		return Base::TPoint3<float>(itemSpec.width, itemSpec.height, itemSpec.depth);
	}

	SItemInstance MakeItemInstance(const std::vector<SpacePlanning::SItemSpec<float>>& vctItemSpecs, const SpacePlanning::SPlacementInfo& placement)
	{
		const Base::TPoint3<float> tpSize = GetRotatedItemSize(vctItemSpecs[placement.i32ItemIndex], placement.eRotation);

		SItemInstance instance;
		instance.i32ItemSpecIndex = placement.i32ItemIndex;
		instance.eRotation = placement.eRotation;
		instance.tpMin = placement.tpPosition;
		instance.tpMax = Base::TPoint3<float>(
			placement.tpPosition.x + tpSize.x,
			placement.tpPosition.y + tpSize.y,
			placement.tpPosition.z + tpSize.z);

		return instance;
	}

	// lower 위에 upper 가 얹혀 있는지 판정 // Check whether upper rests on lower
	bool IsBelow(const SItemInstance& lower, const SItemInstance& upper)
	{
		const bool bXOverlap = (lower.tpMin.x < upper.tpMax.x) && (upper.tpMin.x < lower.tpMax.x);
		const bool bZOverlap = (lower.tpMin.z < upper.tpMax.z) && (upper.tpMin.z < lower.tpMax.z);
		const bool bUpperIsAbove = upper.tpMin.y >= lower.tpMax.y - 0.001f;

		return bXOverlap && bZOverlap && bUpperIsAbove;
	}

	// 한 상자의 배치 상태와 상단 물품 판정용 countAbove // Bin placement state and countAbove for top-item checks
	struct SBinState
	{
		std::vector<SItemInstance> vctItems;
		std::vector<int32_t> vctCountAbove;

		void Clear()
		{
			vctItems.clear();
			vctCountAbove.clear();
		}

		// 물품을 추가하고 아래 물품들의 countAbove 갱신 // Add an item and update countAbove of items below it
		void AddInstance(const SItemInstance& instance)
		{
			for(size_t i = 0; i < vctItems.size(); ++i)
			{
				if(IsBelow(vctItems[i], instance))
					vctCountAbove[i]++;
			}

			vctItems.push_back(instance);
			vctCountAbove.push_back(0);
		}

		// 지정 타입의 상단 물품을 제거하고 새로 드러난 상단 인덱스를 반환 // Remove one top item and return newly exposed tops
		CResult RemovePickableOfType(int32_t i32ItemSpecIndex, std::vector<int32_t>& vctNewlyPickable)
		{
			int32_t i32Found = -1;
			for(size_t j = 0; j < vctItems.size(); ++j)
			{
				if(vctItems[j].i32ItemSpecIndex == i32ItemSpecIndex && vctCountAbove[j] == 0)
				{
					i32Found = static_cast<int32_t>(j);
					break;
				}
			}

			if(i32Found < 0)
				return EResult_DoesNotExist;

			const SItemInstance removed = vctItems[i32Found];
			vctItems.erase(vctItems.begin() + i32Found);
			vctCountAbove.erase(vctCountAbove.begin() + i32Found);

			for(size_t i = 0; i < vctItems.size(); ++i)
			{
				if(IsBelow(vctItems[i], removed))
				{
					const int32_t i32Old = vctCountAbove[i];
					vctCountAbove[i]--;

					if(i32Old == 1)
						vctNewlyPickable.push_back(static_cast<int32_t>(i));
				}
			}

			return EResult_OK;
		}
	};

	// 실행 파일 경로 // Executable path used as the cache reference
	CFLString<wchar_t> GetExecutablePath()
	{
		wchar_t wcsPath[MAX_PATH];
		wcsPath[0] = L'\0';
		GetModuleFileNameW(NULL, wcsPath, MAX_PATH);
		return CFLString<wchar_t>(wcsPath);
	}

	// 캐시가 기준 파일보다 최신인지 확인 // Check whether the cache is newer than the reference file
	bool IsCacheUpToDate(const CFLString<wchar_t>& flsCache, const CFLString<wchar_t>& flsReference)
	{
		struct _stat64 stCache;
		if(_wstat64(flsCache.GetString(), &stCache) != 0)
			return false;

		struct _stat64 stReference;
		if(_wstat64(flsReference.GetString(), &stReference) != 0)
			return false;

		return stCache.st_mtime > stReference.st_mtime;
	}

	// 동일 사양으로 알고리즘 구성 및 학습 // Configure and learn with identical specs
	CResult ConfigureAndLearn(CSpacePlanningStaticSP& alg, const SpacePlanning::SBinSpec<float>& binSpec,
	                          const std::vector<SpacePlanning::SItemSpec<float>>& vctItemSpecs, const std::vector<int32_t>& vctItemCounts)
	{
		CResult res = EResult_UnknownError;

		do
		{
			// 재학습 시 사양 중복 추가 방지 // Avoid duplicated specs on re-learn
			alg.Clear();

			if((res = alg.AddBinSpec(binSpec)).IsFail())
				break;

			bool bFailed = false;
			for(size_t i = 0; i < vctItemSpecs.size(); ++i)
			{
				if((res = alg.AddItemSpec(vctItemSpecs[i])).IsFail())
				{
					bFailed = true;
					break;
				}
			}

			if(bFailed)
				break;

			SpacePlanning::SStaticListParameters params;
			params.itemCounts.Resize(static_cast<int64_t>(vctItemCounts.size()));
			for(size_t i = 0; i < vctItemCounts.size(); ++i)
				params.itemCounts[static_cast<int64_t>(i)] = vctItemCounts[i];

			if((res = alg.SetStaticListParameters(params)).IsFail())
				break;

			res = alg.Learn();
		}
		while(false);

		return res;
	}

	// 캐시가 유효하면 Load, 아니면 Learn 후 Save // Load a valid cache, otherwise learn and save
	CResult LearnOrLoadModel(CSpacePlanningStaticSP& alg, const CFLString<wchar_t>& flsCache, const CFLString<wchar_t>& flsSource,
	                         const SpacePlanning::SBinSpec<float>& binSpec, const std::vector<SpacePlanning::SItemSpec<float>>& vctItemSpecs,
	                         const std::vector<int32_t>& vctItemCounts)
	{
		CResult res = EResult_UnknownError;

		// 캐시가 소스보다 최신이면 학습 결과를 불러옴 // Load the learned result from a newer cache
		if(IsCacheUpToDate(flsCache, flsSource))
		{
			res = alg.Load(flsCache);

			// PartialOK 는 파라미터만 로드된 상태이므로 재학습 필요 // PartialOK means parameters were loaded but learning is required
			if(res.IsOK() && alg.IsLearned())
			{
				wprintf(L"Loaded cached model: %s\n", flsCache.GetString());
				return res;
			}
		}

		// ConfigureAndLearn 이 Clear 하므로 부분 로드 상태여도 안전 // ConfigureAndLearn clears any partial-load state
		if((res = ConfigureAndLearn(alg, binSpec, vctItemSpecs, vctItemCounts)).IsFail())
			return res;

		// 저장 실패는 예제 진행에 치명적이지 않으므로 경고만 출력 // Save failure is non-fatal for this example
		const CResult resSave = alg.Save(flsCache);
		if(resSave.IsFail())
			wprintf(L"Warning: failed to cache model (%s): %s\n", flsCache.GetString(), resSave.GetString());
		else
			wprintf(L"Learned and cached model: %s\n", flsCache.GetString());

		return res;
	}

	// 학습된 알고리즘에서 world 좌표 변환기 구성 // Build a world-space coordinate converter from a learned algorithm
	CResult SetupConverter(const CSpacePlanningBaseSP& alg, CSpacePlanningCoordinateConverterSP& converter, float f32WorldXOffset)
	{
		CResult res = EResult_UnknownError;

		do
		{
			if((res = alg.GetCoordinateConverter(converter)).IsFail())
				break;

			const Base::TPoint3<float> tpWorldPivot(f32WorldXOffset, 0.f, 0.f);
			const Base::TPoint3<float> tpBinPivot(0.f, 0.f, 0.f);
			const Base::TPoint3<float> tpDirectionZ(0.f, 0.f, 1.f);
			const Base::TPoint3<float> tpUpY(0.f, 1.f, 0.f);

			if((res = converter.SetBinTransform(0, tpWorldPivot, tpBinPivot, tpDirectionZ, tpUpY)).IsFail())
				break;

			const int32_t i32ItemCount = alg.GetItemSpecCount();
			bool bFailed = false;
			for(int32_t i = 0; i < i32ItemCount; ++i)
			{
				if((res = converter.SetItemPivotNormalized(i, Base::TPoint3<float>(0.5f, 0.5f, 0.5f))).IsFail())
				{
					bFailed = true;
					break;
				}
			}

			if(bFailed)
				break;

			res = converter.Learn();
		}
		while(false);

		return res;
	}

	// 상자 상태를 렌더링용 배치 배열로 변환 // Convert bin state into a placement array for rendering
	void BuildPlacementArray(const SBinState& bin, SpacePlanning::CFLPlacementInfoArray& flpaOut)
	{
		flpaOut.Clear();
		for(size_t i = 0; i < bin.vctItems.size(); ++i)
		{
			SpacePlanning::SPlacementInfo placement;
			placement.i32BinIndex = 0;
			placement.i32ItemIndex = bin.vctItems[i].i32ItemSpecIndex;
			placement.eRotation = bin.vctItems[i].eRotation;
			placement.tpPosition = bin.vctItems[i].tpMin;
			flpaOut.PushBack(placement);
		}
	}

	// 지정한 상자 상태를 뷰에 그림 // Draw a bin state into the view
	CResult PushBinToView(CGUIView3DWrap& view3D, CSpacePlanningCoordinateConverterSP& converter, const SBinState& bin)
	{
		CResult res = EResult_UnknownError;

		do
		{
			CFL3DObjectGroup flogBin;
			if((res = converter.MakeBinObjectGroup(flogBin)).IsFail())
				break;

			int32_t i32BinObjIndex = 0;
			view3D.PushObject(*flogBin.GetObjectByIndex(0), &i32BinObjIndex);
			((CGUIView3DObject*)view3D.GetView3DObject(i32BinObjIndex))->SetOpacity(0.15f);

			if(bin.vctItems.empty())
			{
				res = EResult_OK;
				break;
			}

			SpacePlanning::CFLPlacementInfoArray flpaItems;
			BuildPlacementArray(bin, flpaItems);

			CFL3DObjectGroup flogItems;
			if((res = converter.MakeItemObjectGroup(flpaItems, flogItems)).IsFail())
				break;

			for(size_t i = 0; i < bin.vctItems.size(); ++i)
			{
				int32_t i32ObjIndex = 0;
				view3D.PushObject(*flogItems.GetObjectByIndex(static_cast<int32_t>(i)), &i32ObjIndex);
				((CGUIView3DObject*)view3D.GetView3DObject(i32ObjIndex))->SetOpacity(0.6f);
			}

			res = EResult_OK;
		}
		while(false);

		return res;
	}

	// source 의 상단 물품을 dstAlg 추천 위치로 모두 이송 // Move all top-pickable source items to destination recommendations
	CResult TransferAllItems(SBinState& srcBin, CSpacePlanningStaticSP& dstAlg, SBinState& dstBin,
	                         const std::vector<SpacePlanning::SItemSpec<float>>& vctItemSpecs, const wchar_t* pwcsLabel,
	                         const std::function<void()>& fnOnStep)
	{
		CResult res = EResult_UnknownError;

		do
		{
			if((res = dstAlg.ClearInteractiveStates()).IsFail())
				break;

			if((res = dstAlg.Execute()).IsFail())
				break;

			// 현재 상단 물품을 목적지 대기열에 추가 // Push currently top-pickable items into the destination queue
			bool bFailed = false;
			for(size_t i = 0; i < srcBin.vctItems.size(); ++i)
			{
				if(srcBin.vctCountAbove[i] != 0)
					continue;

				if((res = dstAlg.PushItem(srcBin.vctItems[i].i32ItemSpecIndex)).IsFail())
				{
					bFailed = true;
					break;
				}
			}

			if(bFailed)
				break;

			int32_t i32Step = 0;
			while(!srcBin.vctItems.empty())
			{
				// 목적지 추천 물품/위치 조회 // Get the next destination recommendation
				SpacePlanning::SPlacementInfo placement;
				if((res = dstAlg.GetRecommendedNextPlacement(placement)).IsFail())
					break;

				// 같은 타입의 상단 물품 제거 // Remove a top item of the recommended type
				std::vector<int32_t> vctNewlyPickable;
				if((res = srcBin.RemovePickableOfType(placement.i32ItemIndex, vctNewlyPickable)).IsFail())
					break;

				for(size_t k = 0; k < vctNewlyPickable.size(); ++k)
				{
					const int32_t i32Idx = vctNewlyPickable[k];
					if((res = dstAlg.PushItem(srcBin.vctItems[i32Idx].i32ItemSpecIndex)).IsFail())
					{
						bFailed = true;
						break;
					}
				}

				if(bFailed)
					break;

				// 목적지에 추천 위치 그대로 배치 // Place into the destination at the recommended position
				if((res = dstAlg.AddPlacement(placement)).IsFail())
					break;

				dstBin.AddInstance(MakeItemInstance(vctItemSpecs, placement));

				++i32Step;
				wprintf(L"[%s] step %2d: picked item type %d -> placed at bin %d [%.1f, %.1f, %.1f]  (source left: %d)\n",
					pwcsLabel, i32Step, placement.i32ItemIndex, placement.i32BinIndex,
					placement.tpPosition.x, placement.tpPosition.y, placement.tpPosition.z,
					static_cast<int32_t>(srcBin.vctItems.size()));

				if(fnOnStep)
					fnOnStep();
			}
		}
		while(false);

		return res;
	}
}

int main()
{
	// FLImaging(R) 라이브러리 사용 전 반드시 한 번 호출
	// Must be called once before using any FLImaging(R) features.
	CLibraryUtilities::Initialize();

	// 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap view3DResult;

	CResult res = EResult_UnknownError;

	do
	{
		// 물품 사양과 개수 설정 // Set item specs and counts
		std::vector<SpacePlanning::SItemSpec<float>> vctItemSpecs;
		vctItemSpecs.push_back(SpacePlanning::SItemSpec<float>{ 3.0f, 3.0f, 4.0f, 1.f, SpacePlanning::ERotationAllowance_VerticalAxisOnly });
		vctItemSpecs.push_back(SpacePlanning::SItemSpec<float>{ 2.0f, 4.3f, 5.9f, 1.f, SpacePlanning::ERotationAllowance_VerticalAxisOnly });
		vctItemSpecs.push_back(SpacePlanning::SItemSpec<float>{ 4.0f, 3.0f, 3.5f, 1.f, SpacePlanning::ERotationAllowance_VerticalAxisOnly });
		vctItemSpecs.push_back(SpacePlanning::SItemSpec<float>{ 5.0f, 3.0f, 2.5f, 1.f, SpacePlanning::ERotationAllowance_FullRotation });

		std::vector<int32_t> vctItemCounts;
		vctItemCounts.push_back(4);
		vctItemCounts.push_back(3);
		vctItemCounts.push_back(3);
		vctItemCounts.push_back(3);

		const SpacePlanning::SBinSpec<float> binSpecA = { 9.f, 12.f, 10.f };
		const SpacePlanning::SBinSpec<float> binSpecB = { 10.f, 11.f, 9.f };

		// A/B 상자를 서로 다른 사양으로 학습 // Learn bins A/B with different specs
		CSpacePlanningStaticSP algA;
		CSpacePlanningStaticSP algB;

		// 모델 캐시 파일 설정 // Set model cache files
		const CFLString<wchar_t> flsSource = GetExecutablePath();
		CFLString<wchar_t> flsCacheA, flsCacheB;
		flsCacheA.Format(L"SpacePlanningStaticRoundTrip_A.%s", algA.GetFileExtension());
		flsCacheB.Format(L"SpacePlanningStaticRoundTrip_B.%s", algB.GetFileExtension());

		if((res = LearnOrLoadModel(algA, flsCacheA, flsSource, binSpecA, vctItemSpecs, vctItemCounts)).IsFail())
		{
			ErrorPrint(res, "Failed to prepare model for bin A.\n");
			break;
		}

		if((res = LearnOrLoadModel(algB, flsCacheB, flsSource, binSpecB, vctItemSpecs, vctItemCounts)).IsFail())
		{
			ErrorPrint(res, "Failed to prepare model for bin B.\n");
			break;
		}

		// A/B 를 world 좌표계에서 나란히 배치 // Place A/B side by side in world coordinates
		CSpacePlanningCoordinateConverterSP converterA;
		CSpacePlanningCoordinateConverterSP converterB;

		if((res = SetupConverter(algA, converterA, 0.f)).IsFail() ||
		   (res = SetupConverter(algB, converterB, 18.f)).IsFail())
		{
			ErrorPrint(res, "Failed to set up coordinate converters.\n");
			break;
		}

		// 학습 결과로 A 의 초기 적재 상태 구성 // Build initial bin A state from learned placements
		SBinState binA;
		SBinState binB;

		SpacePlanning::SLearnedPlacementResults learnedA;
		if((res = algA.GetLearnedPlacements(learnedA)).IsFail())
		{
			ErrorPrint(res, "Failed to get learned placements for bin A.\n");
			break;
		}

		for(int64_t i = 0; i < learnedA.placements.GetCount(); ++i)
		{
			const SpacePlanning::SPlacementInfo& p = learnedA.placements[i];
			binA.AddInstance(MakeItemInstance(vctItemSpecs, p));
		}

		// 3D 뷰 생성 // Create 3D view
		if((res = view3DResult.Create(600, 0, 1300, 650)).IsFail())
		{
			ErrorPrint(res, "Failed to create the 3D view.\n");
			break;
		}

		view3DResult.SetRenderingTransparencyMode(ERenderingTransparencyMode_DepthPeelingOIT);
		view3DResult.SetRenderingResolutionScale(2);

		view3DResult.GetLayer(0).DrawTextCanvas(CFLPoint<double>(0, 0), L"Static SP Round Trip - pick top items only", YELLOW, BLACK, 20);

		bool bZoomFitted = false;

		// 매 스텝 두 상자 다시 그리기 // Redraw both bins on each step
		std::function<void()> fnRender = [&]() -> void
		{
			if(!view3DResult.IsAvailable())
				return;

			view3DResult.Lock();
			view3DResult.ClearObjects();

			PushBinToView(view3DResult, converterA, binA);
			PushBinToView(view3DResult, converterB, binB);

			if(!bZoomFitted)
			{
				view3DResult.ZoomFit();
				bZoomFitted = true;
			}

			view3DResult.Unlock();
			view3DResult.Invalidate(true);

			CThreadUtilities::Sleep(500);
		};

		// 초기 상태 표시 // Show the initial state
		fnRender();

		// A -> B 이송 // Transfer A -> B
		wprintf(L"Starting transfer A->B.\n");
		if((res = TransferAllItems(binA, algB, binB, vctItemSpecs, L"A->B", fnRender)).IsFail())
		{
			if(view3DResult.IsAvailable())
				ErrorPrint(res, "Transfer A->B failed.\n");
			break;
		}

		wprintf(L"A->B complete. Switching to B->A after 2 seconds.\n");
		CThreadUtilities::Sleep(2000);

		// B -> A 이송 (왕복) // Transfer B -> A (round trip)
		wprintf(L"Starting transfer B->A.\n");
		if((res = TransferAllItems(binB, algA, binA, vctItemSpecs, L"B->A", fnRender)).IsFail())
		{
			if(view3DResult.IsAvailable())
				ErrorPrint(res, "Transfer B->A failed.\n");
			break;
		}

		wprintf(L"Round trip complete.\n");

		// 3D 뷰가 종료될 때 까지 기다림 // Wait for the 3D view to close
		while(view3DResult.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}
