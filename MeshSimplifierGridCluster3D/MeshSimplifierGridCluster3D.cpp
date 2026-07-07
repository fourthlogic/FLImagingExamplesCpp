#include <cstdio>
#include <array>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap view3DDst[4];

	CGUIView3DWrap view3DSource;

	// 알고리즘 동작 결과 // Algorithm execution result
	CResult res = EResult_UnknownError;

	do
	{

		if((res = view3DSource.Create(0, 0, 500, 500)).IsFail() ||
		   (res = view3DDst[0].Create(500, 0, 1000, 500)).IsFail() ||
		   (res = view3DDst[1].Create(500, 500, 1000, 1000)).IsFail() ||
		   (res = view3DDst[2].Create(1000, 0, 1500, 500)).IsFail() ||
		   (res = view3DDst[3].Create(1000, 500, 1500, 1000)).IsFail())
		{
			ErrorPrint(res, L"Failed To Create Views");
			break;
		}

		for(int32_t i = 0; i < 4; ++i)
		{
			view3DDst[i].SynchronizePointOfView(&view3DSource);
			view3DDst[i].SynchronizeWindow(&view3DSource);
		}

		// 소스 오브젝트 생성 // Make Source Object
		CFL3DObject floPointCloud;
		const int32_t i32Count = static_cast<int32_t>(1e6);
		{
			CXorshiroRandomGenerator rng;
			rng.Seed(42);

			CFLArray<TPoint3<float>>& flaVertices = *const_cast<CFLArray<TPoint3<float>>*>(floPointCloud.GetVertices());
			flaVertices.Resize(i32Count);

			for(int32_t i = 0; i < i32Count; ++i)
			{
				auto tp = rng.GenerateUniformRandomPointOnUnitSphereF32();
				flaVertices[i] = TPoint3F(tp.x * 2, tp.y * 3, tp.z * -3.5f);
			}
		}

		CFL3DObject floMesh;
		CConvexHull3D convexHull3D;
		convexHull3D.SetSourceObject(floPointCloud);
		convexHull3D.SetDestinationObject(floMesh);
		convexHull3D.EnableNormalRecalculation(true);
		convexHull3D.EnablePreserveVertex(false);
		
		if((res = convexHull3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed To Execute Convex Hull.");
			break;
		}

		view3DSource.PushObject(floMesh);
		view3DSource.ZoomFit();

		// 소스 뷰에 메시지 출력 // Display message on the source view
		CGUIView3DLayerWrap layer3DSource = view3DSource.GetLayer(0);
		layer3DSource.Clear();
		layer3DSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source View", YELLOW, BLACK, 20);

		// CMeshSimplifierGridCluster3D 실행 // Execute CMeshSimplifierGridCluster3D
		CMeshSimplifierGridCluster3D meshSimplifierGridCluster3D;

		std::array<int32_t, 4> arrI32SampleSize = { 3333, 1000, 350, 100 };

		for(int32_t i = 0; i < 4; ++i)
		{
			CFL3DObject floResult;

			meshSimplifierGridCluster3D.SetSourceObject(floMesh);
			meshSimplifierGridCluster3D.SetDestinationObject(floResult);
			meshSimplifierGridCluster3D.SetSamplingSize(arrI32SampleSize[i]);
			meshSimplifierGridCluster3D.SetClusteringMethod(CMeshSimplifierGridCluster3D::EClusteringMethod_FastGrid);

			if((res = meshSimplifierGridCluster3D.Execute()).IsFail())
			{
				ErrorPrint(res, L"Failed To Execute Mesh Simplifier Grid Cluster.");
				break;
			}
			view3DDst[i].PushObject(floResult);

			// 목적지 뷰에 메시지 출력 // Display message on the destination view
			CGUIView3DLayerWrap layer3DDst = view3DDst[i].GetLayer(0);
			layer3DDst.Clear();
			layer3DDst.DrawTextCanvas(CFLPoint<double>(0, 0), CFLString<wchar_t>().Format(L"Destination(Count: %d)", arrI32SampleSize[i]), YELLOW, BLACK, 20);
		}

		while(view3DSource.IsAvailable() && view3DDst[0].IsAvailable() && view3DDst[1].IsAvailable() && view3DDst[2].IsAvailable() && view3DDst[3].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}
