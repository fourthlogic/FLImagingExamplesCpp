#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"
#include <initializer_list>

namespace SpacePlanning = FLImaging::ThreeDim::SpacePlanning;

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


	// 3D 뷰 선언 // Declare 3D views
	CGUIView3DWrap arrView3DResults[3];

	CResult res = EResult_UnknownError;

	do
	{
		// 알고리즘 객체 선언 // Declare algorithm object
		CSpacePlanningStaticSP alg;

		// Bin spec 설정 // Set the bin spec
		SpacePlanning::SBinSpec<float> binSpec = { 8, 6, 8 };

		if((res = alg.AddBinSpec(binSpec)).IsFail())
		{
			ErrorPrint(res, "Failed to add bin spec.\n");
			break;
		}

		// Item spec 설정 (회전 없음) // Set the item specs (no rotation)
		SpacePlanning::SItemSpec<float> itemSpec1 = { 3, 3, 4, 1, SpacePlanning::ERotationAllowance_NoRotation };
		SpacePlanning::SItemSpec<float> itemSpec2 = { 4, 3, 3, 1, SpacePlanning::ERotationAllowance_NoRotation };
		SpacePlanning::SItemSpec<float> itemSpec3 = { 5, 3, 2, 1, SpacePlanning::ERotationAllowance_NoRotation };

		if((res = alg.AddItemSpec(itemSpec1)).IsFail() ||
		   (res = alg.AddItemSpec(itemSpec2)).IsFail() ||
		   (res = alg.AddItemSpec(itemSpec3)).IsFail())
		{
			ErrorPrint(res, "Failed to add item spec.\n");
			break;
		}

		// Static list 파라미터 설정 // Set the static list parameters
		SpacePlanning::SStaticListParameters params;
		params.itemCounts.Resize(3);
		params.itemCounts[0] = 4;
		params.itemCounts[1] = 3;
		params.itemCounts[2] = 2;

		if((res = alg.SetStaticListParameters(params)).IsFail())
		{
			ErrorPrint(res, "Failed to set static list parameters.\n");
			break;
		}

		// 같은 item 수량을 직접 순서, 순서 무관, seed shuffle의 세 방식으로 평가
		// Evaluate the same item counts as a direct sequence, order-free supply, and seeded shuffle
		CFLArray<int32_t> flaSequenceA;
		int32_t arrSequenceA[] = { 0, 1, 2, 0, 1, 2, 0, 1, 0 };

		for(int32_t i = 0; i < 9; ++i)
			flaSequenceA.Append(arrSequenceA[i]);

		SpacePlanning::CScoreEvaluationCaseSequenceSP evaluationCaseA(flaSequenceA, 1);
		SpacePlanning::CScoreEvaluationCaseOrderFreeSP evaluationCaseB(params.itemCounts);
		SpacePlanning::CScoreEvaluationCaseShuffledSP evaluationCaseC(params.itemCounts, 2, 20260729ULL);

		if((res = alg.AddScoreEvaluationCase(evaluationCaseA, CFLString<wchar_t>(L"Alternating sequence"))).IsFail() ||
		   (res = alg.AddScoreEvaluationCase(evaluationCaseB, CFLString<wchar_t>(L"Order-free counts"))).IsFail() ||
		   (res = alg.AddScoreEvaluationCase(evaluationCaseC, CFLString<wchar_t>(L"Seeded shuffle"))).IsFail() ||
		   (res = alg.EnableImmediateScoreEvaluation(false)).IsFail())
		{
			ErrorPrint(res, "Failed to configure score-evaluation cases.\n");
			break;
		}

		// Learn은 전략을 준비하고, EvaluateScore mode의 Execute가 같은 case들을 평가
		// Learn prepares strategies; Execute in EvaluateScore mode evaluates the same cases
		if((res = alg.Learn()).IsFail())
		{
			ErrorPrint(res, "Failed to learn.\n");
			break;
		}

		if((res = alg.SetExecutionMode(SpacePlanning::EExecutionMode_EvaluateScore)).IsFail() ||
		   (res = alg.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to evaluate scores.\n");
			break;
		}

		if(!alg.HasValidScoreEvaluation())
		{
			ErrorPrint(EResult_NoResult, "Score evaluation did not commit results.\n");
			break;
		}

		// 한 고정 strategy에 대해 case별 요약과 실제 배치 순서를 보관
		// Keep each case summary and actual placement order for one fixed strategy
		const SpacePlanning::SSpacePlanningStrategyId sEvaluationStrategyId(SpacePlanning::EStrategyGroup_Search, 0);
		const int32_t i32EvaluationCaseCount = alg.GetScoreEvaluationCaseCount();

		if(i32EvaluationCaseCount != 3)
		{
			ErrorPrint(EResult_NoResult, "Expected three committed score-evaluation cases.\n");
			break;
		}

		SpacePlanning::SScoreEvaluationCaseInfo arrCaseInfos[3];
		SpacePlanning::SScoreEvaluationResult arrEvaluationResults[3];

		for(int32_t i = 0; i < i32EvaluationCaseCount; ++i)
		{
			SpacePlanning::SScoreEvaluationCaseInfo& info = arrCaseInfos[i];
			SpacePlanning::SScoreEvaluationResult& evaluationResult = arrEvaluationResults[i];

			if((res = alg.GetScoreEvaluationCaseInfo(i, info)).IsFail() ||
			   (res = alg.GetScoreEvaluationResult(
				   sEvaluationStrategyId,
				   info.u64CaseId,
				   evaluationResult)).IsFail())
			{
				ErrorPrint(res, "Failed to get score-evaluation results.\n");
				break;
			}

			wprintf(
				L"Case %c - %ls: placed %d/%d, utilization %.2f%%\n",
				L'A' + i,
				info.flsName.GetString(),
				evaluationResult.i32PlacedItemCount,
				info.i32TotalItemCount,
				evaluationResult.f64VolumeUtilization * 100.0);
		}

		if(res.IsFail())
			break;

		CSpacePlanningCoordinateConverterSP converter;
		if((res = InitializeCoordinateConverter(alg, converter)).IsFail())
		{
			ErrorPrint(res, "Failed to initialize the coordinate converter.\n");
			break;
		}

		CFL3DObjectGroup flogBins;
		CFL3DObjectGroup arrFlogItems[3];
		if((res = converter.MakeBinObjectGroup(flogBins)).IsFail())
		{
			ErrorPrint(res, "Failed to build world-space bin objects.\n");
			break;
		}

		for(int32_t i = 0; i < i32EvaluationCaseCount; ++i)
		{
			if((res = converter.MakeItemObjectGroup(arrEvaluationResults[i].flpaPlacements, arrFlogItems[i])).IsFail())
			{
				ErrorPrint(res, "Failed to build world-space item objects.\n");
				break;
			}

			const SpacePlanning::CFLPlacementInfoArray& flpaPlacements = arrEvaluationResults[i].flpaPlacements;
			wprintf(L"\nCase %c placements:\n", L'A' + i);
			for(int64_t j = 0; j < flpaPlacements.GetCount(); ++j)
			{
				Base::TPoint3<float> tpWorldPosition;
				if((res = converter.Convert(flpaPlacements[j], tpWorldPosition)).IsFail())
				{
					ErrorPrint(res, "Failed to convert placement coordinates.\n");
					break;
				}

				wprintf(L"  %d: bin %d, item %d -> world center [%.1f, %.1f, %.1f]\n",
						(int32_t)j,
						flpaPlacements[j].i32BinIndex,
						flpaPlacements[j].i32ItemIndex,
						tpWorldPosition.x, tpWorldPosition.y, tpWorldPosition.z);
			}

			if(res.IsFail())
				break;
		}

		if(res.IsFail())
			break;

		const int32_t i32BinCount = alg.GetBinSpecCount();
		const int32_t i32ViewWidth = 600;
		const int32_t i32ViewHeight = 500;

		for(int32_t i = 0; i < i32EvaluationCaseCount; ++i)
		{
			CGUIView3DWrap& view3DResult = arrView3DResults[i];
			const SpacePlanning::SScoreEvaluationCaseInfo& info = arrCaseInfos[i];
			const SpacePlanning::SScoreEvaluationResult& evaluationResult = arrEvaluationResults[i];
			const int32_t i32PlacedCount = static_cast<int32_t>(evaluationResult.flpaPlacements.GetCount());

			if((res = view3DResult.Create(i32ViewWidth * i, 0, i32ViewWidth * (i + 1), i32ViewHeight)).IsFail())
			{
				ErrorPrint(res, "Failed to create a 3D view.\n");
				break;
			}

			view3DResult.SetRenderingTransparencyMode(ERenderingTransparencyMode_DepthPeelingOIT);
			view3DResult.SetRenderingResolutionScale(2);

			// 결과 뷰에 해당 case의 world-space 아이템 및 bin 오브젝트 추가
			// Push this case's world-space item and bin objects to its result view
			for(int32_t j = 0; j < i32PlacedCount; ++j)
			{
				int32_t i32ObjIndex = 0;
				view3DResult.PushObject(*arrFlogItems[i].GetObjectByIndex(j), &i32ObjIndex);
				((CGUIView3DObject*)view3DResult.GetView3DObject(i32ObjIndex))->SetOpacity(0.6f);
			}

			for(int32_t j = 0; j < i32BinCount; ++j)
			{
				int32_t i32ObjIndex = 0;
				view3DResult.PushObject(*flogBins.GetObjectByIndex(j), &i32ObjIndex);
				((CGUIView3DObject*)view3DResult.GetView3DObject(i32ObjIndex))->SetOpacity(0.2f);
			}

			// 화면에 출력하기 위해 3D 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 from the 3D view for display
			// 이 객체는 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to the view and does not need to be released separately
			CGUIView3DLayerWrap layer3DResult = view3DResult.GetLayer(0);

			CFLString<wchar_t> flsTitle;
			flsTitle.Format(L"Case %c - %s", L'A' + i, info.flsName.GetString());
			layer3DResult.DrawTextCanvas(CFLPoint<double>(0, 0), flsTitle, YELLOW, BLACK, 20);

			// 결과 정보를 3D 뷰에 텍스트로 표시 // Draw result summary text on the 3D view
			CFLString<wchar_t> flsResultInfo;
			flsResultInfo.Format(
				L"Evaluation strategy: group=%d, id=%d\n"
				L"Placed items: %d/%d\n"
				L"Volume utilization: %.1f%%\n"
				L"Coordinate converter: world-space center pivot",
				(int32_t)sEvaluationStrategyId.eGroup,
				sEvaluationStrategyId.i32IDInStrategy,
				evaluationResult.i32PlacedItemCount,
				info.i32TotalItemCount,
				evaluationResult.f64VolumeUtilization * 100.0);

			layer3DResult.DrawTextCanvas(CFLPoint<double>(0, 25), flsResultInfo, YELLOW, BLACK, 16);

			// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다.
			// With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
			view3DResult.ZoomFit();

			// 이미지 뷰를 갱신 합니다. // Update image view
			view3DResult.Invalidate(true);
		}

		if(res.IsFail())
			break;

		for(int32_t i = 1; i < i32EvaluationCaseCount; ++i)
		{
			arrView3DResults[0].SynchronizePointOfView(&arrView3DResults[i]);
			arrView3DResults[0].SynchronizeWindow(&arrView3DResults[i]);
		}

		// 3D 뷰 중 하나가 종료될 때까지 기다림 // Wait until any of the three 3D views is closed
		bool bAllViewsAvailable = false;
		do
		{
			bAllViewsAvailable = true;
			for(int32_t i = 0; i < i32EvaluationCaseCount; ++i)
				bAllViewsAvailable = bAllViewsAvailable && arrView3DResults[i].IsAvailable();

			if(bAllViewsAvailable)
				CThreadUtilities::Sleep(1);
		}
		while(bAllViewsAvailable);
	}
	while(false);

	return 0;
}
