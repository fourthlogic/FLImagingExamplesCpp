#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// ------------------------------------------------------------------
		// 1단계: 데이터 준비 및 KDTree 구축 (Build)
		// Step 1: Prepare Data and Build KDTree
		// ------------------------------------------------------------------
		wprintf(L"[Step 1] Prepare Data and Build KDTree\n");

		CFLArray<TPoint<double>> flaVertices;
		flaVertices.PushBack(TPoint<double>(0.0, 0.0));   // Index 0
		flaVertices.PushBack(TPoint<double>(1.0, 1.0));   // Index 1
		flaVertices.PushBack(TPoint<double>(2.0, 2.0));   // Index 2
		flaVertices.PushBack(TPoint<double>(3.0, 1.0));   // Index 3
		flaVertices.PushBack(TPoint<double>(5.0, 5.0));   // Index 4
		flaVertices.PushBack(TPoint<double>(10.0, 10.0)); // Index 5

		// 입력받은 정점 목록 및 인덱스 출력 // Output the input list of vertices and indices
		wprintf(L" - Input Vertices:\n");

		for(int64_t i = 0; i < flaVertices.GetCount(); ++i)
			wprintf(L"   Index %lld : (%.1f, %.1f)\n", i, flaVertices[i].x, flaVertices[i].y);

		// CKDTree 선언 (2D Point, double 타입) // Declare CKDTree (2D Point, double type)
		CKDTree<TPoint, double> kdtree;

		// 트리 구축 // Build tree
		if((res = kdtree.Build(flaVertices)).IsFail())
		{
			ErrorPrint(res, "Failed to build KDTree.\n");
			break;
		}

		wprintf(L" - The number of elements in the constructed data : %lld\n\n", kdtree.GetCount());

		// ------------------------------------------------------------------
		// 2단계: 단일 최근접 정점 탐색 (GetNearestPointAndIndex)
		// Step 2: Single Nearest Neighbor Search
		// ------------------------------------------------------------------
		wprintf(L"[Step 2] Single Nearest Neighbor Search\n");

		TPoint<double> tpQuery1(1.2, 0.9);
		wprintf(L" - Target Query: (%.1f, %.1f)\n", tpQuery1.x, tpQuery1.y);

		// 좌표와 인덱스를 동시에 구함 // Get both coordinates and index
		std::pair<TPoint<double>, size_t> pairNearestResult = kdtree.GetNearestPointAndIndex(tpQuery1);

		wprintf(L" - Nearest Point: (%.1f, %.1f)\n", pairNearestResult.first.x, pairNearestResult.first.y);
		wprintf(L" - Nearest Index: %llu\n\n", (uint64_t)pairNearestResult.second);

		// ------------------------------------------------------------------
		// 3단계: K-최근접 정점 탐색 (GetNearestNeighborsPointsAndIndices)
		// Step 3: K-Nearest Neighbors Search
		// ------------------------------------------------------------------
		wprintf(L"[Step 3] K-Nearest Neighbors Search (K = 3)\n");

		TPoint<double> tpQuery2(1.5, 1.5);
		int64_t i64K = 3;
		wprintf(L" - Target Query: (%.1f, %.1f)\n", tpQuery2.x, tpQuery2.y);

		CFLArray<TPoint<double>> flaKNNPoints;
		CFLArray<uint64_t> flaKNNIndices;

		kdtree.GetNearestNeighborsPointsAndIndices(tpQuery2, i64K, flaKNNPoints, flaKNNIndices);

		for(int64_t i = 0; i < flaKNNPoints.GetCount(); ++i)
			wprintf(L"   [%lld] Index: %llu -> (%.1f, %.1f)\n", i, flaKNNIndices[i], flaKNNPoints[i].x, flaKNNPoints[i].y);

		wprintf(L"\n");


		// ------------------------------------------------------------------
		// 4단계: 영역/범위 탐색 (GetPointsAndIndicesInRange)
		// Step 4: Bounding Box Range Search
		// ------------------------------------------------------------------
		wprintf(L"[Step 4] Bounding Box Range Search\n");

		TPoint<double> tpLowerBound(0.5, 0.5);
		TPoint<double> tpUpperBound(3.5, 2.5);
		wprintf(L" - Range Lower Bound: (%.1f, %.1f)\n", tpLowerBound.x, tpLowerBound.y);
		wprintf(L" - Range Upper Bound: (%.1f, %.1f)\n", tpUpperBound.x, tpUpperBound.y);

		CFLArray<TPoint<double>> flaRangePoints;
		CFLArray<uint64_t> flaRangeIndices;

		// 모든 매칭 점을 찾으려면 i64Count에 -1 전달 // Pass -1 to i64Count to find all matching points
		kdtree.GetPointsAndIndicesInRange(tpLowerBound, tpUpperBound, flaRangePoints, flaRangeIndices, -1);

		wprintf(L" - Number of points found in range: %lld\n", flaRangePoints.GetCount());

		for(int64_t i = 0; i < flaRangePoints.GetCount(); ++i)
			wprintf(L"   - Index: %llu -> (%.1f, %.1f)\n", flaRangeIndices[i], flaRangePoints[i].x, flaRangePoints[i].y);

		wprintf(L"\n");


		// ------------------------------------------------------------------
		// 5단계: 반경 탐색 (GetPointsAndIndicesInRadius)
		// Step 5: Radius Search
		// ------------------------------------------------------------------
		wprintf(L"[Step 5] Radius Search\n");

		TPoint<double> tpCenter(0.0, 0.0);
		double f64Radius = 3.0;
		wprintf(L" - Center Point: (%.1f, %.1f)\n", tpCenter.x, tpCenter.y);
		wprintf(L" - Radius: %.1f\n", f64Radius);

		CFLArray<TPoint<double>> flaRadiusPoints;
		CFLArray<uint64_t> flaRadiusIndices;

		kdtree.GetPointsAndIndicesInRadius(tpCenter, f64Radius, flaRadiusPoints, flaRadiusIndices);

		wprintf(L" - Number of points found in radius: %lld\n", flaRadiusPoints.GetCount());

		for(int64_t i = 0; i < flaRadiusPoints.GetCount(); ++i)
			wprintf(L"   - Index: %llu -> (%.1f, %.1f)\n", flaRadiusIndices[i], flaRadiusPoints[i].x, flaRadiusPoints[i].y);

		wprintf(L"\n");


		// ------------------------------------------------------------------
		// 6단계: 기하 연산 (OperateAdd)
		// Step 6: Geometric Operation (OperateAdd)
		// ------------------------------------------------------------------
		wprintf(L"[Step 6] Translate All Nodes (OperateAdd)\n");

		TPoint<double> tpOffset(10.0, 10.0);
		wprintf(L" - Offset Vector: (%.1f, %.1f)\n", tpOffset.x, tpOffset.y);

		// 모든 점에 (10, 10) 이동 적용 // Apply (10, 10) offset to all points
		kdtree.OperateAdd(tpOffset);

		// 이동 후 동일한 Query 점(1.2, 0.9)으로 다시 탐색해 확인 // Re-query near the same query point (1.2, 0.9) to check result
		wprintf(L" - Re-querying near: (%.1f, %.1f)\n", tpQuery1.x, tpQuery1.y);
		std::pair<TPoint<double>, size_t> pairShiftedResult = kdtree.GetNearestPointAndIndex(tpQuery1);

		wprintf(L" - Shifted Nearest Point: (%.1f, %.1f)\n", pairShiftedResult.first.x, pairShiftedResult.first.y);
		wprintf(L" - Index: %llu\n", (uint64_t)pairShiftedResult.second);

		wprintf(L"\n========================================\n");

		wprintf(L"Press Enter to exit...");
		getchar();
	}
	while(false);

	return 0;
}