#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 객체 선언 // Declare 3D object
	CFL3DObject floSrcObject;
	CFL3DObject floDstObject;

	// 이미지 뷰 선언 // Declare image view
	CGUIView3DWrap view3DSrc;
	CGUIView3DWrap view3DDst;

	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res = floSrcObject.Load(L"../../ExampleImages/PlaneDetector3D/Source.ply")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the object file.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if((res = view3DSrc.Create(0, 0, 500, 500)).IsFail() ||
		   (res = view3DDst.Create(500, 0, 1000, 500)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((res = view3DSrc.SynchronizePointOfView(&view3DDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view. \n");
			break;
		}

		// Source Object 3D 뷰 생성 // Create the source object 3D view
		if((res = view3DSrc.PushObject(floSrcObject)).IsFail())
		{
			ErrorPrint(res, L"Failed to display the 3D object.\n");
			break;
		}

		// 알고리즘 객체 생성 // Create algorithm object
		CPlaneDetector3D planeDetector3D;

		if((res = planeDetector3D.SetSourceObject(floSrcObject)).IsFail())
			break;
		if((res = planeDetector3D.SetDestinationObject(floDstObject)).IsFail())
			break;

		if((res = planeDetector3D.SetPlaneFittingTarget(CPlaneDetector3D::EPlaneFittingTarget_VertexNormal)).IsFail())
			break;
		if((res = planeDetector3D.SetMaximumParallelPlaneCount(1)).IsFail())
			break;
		if((res = planeDetector3D.SetNormalVectorResolution(Base::TPoint3<int32_t>(19, 19, 19))).IsFail())
			break;
		if((res = planeDetector3D.SetInlierDistance(0.5)).IsFail())
			break;
		if((res = planeDetector3D.SetMinimumInlierCount(500)).IsFail())
			break;
		if((res = planeDetector3D.SetFitCosineSimilarity(0.995)).IsFail())
			break;
		if((res = planeDetector3D.SetExpandCosineSimilarity(0.99)).IsFail())
			break;
		if((res = planeDetector3D.SetMergeCosineSimilarity(0.99)).IsFail())
			break;

		if((res = planeDetector3D.SetOutliersFilteringMethod(CPlaneDetector3D::EOutliersFilteringMethod_LeastSquare)).IsFail())
			break;
		if((res = planeDetector3D.SetOutliersThresholdCount(1)).IsFail())
			break;
		if((res = planeDetector3D.SetOutliersThreshold(3.00)).IsFail())
			break;
		if((res = planeDetector3D.SetPlaneFittingMethod(CPlaneDetector3D::EPlaneFittingMethod_LeastSquare)).IsFail())
			break;

		// 알고리즘 수행 // Execute the algorithm
		if((res = planeDetector3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute the algorithm.");
			break;
		}

		// 3D 이미지 뷰에 3d object 를 디스플레이 // Display the 3d object on the 3D image view
		CGUIView3DObject gvoDst;
		if((res = ((CFL3DObject*)gvoDst.Get3DObject())->Swap(floDstObject)).IsFail())
			break;
		if((res = gvoDst.SetTopologyType((ETopologyType3D)0x1f)).IsFail())
			break;
		if((res = gvoDst.SetPointSize(2)).IsFail())
			break;
		if((res = view3DDst.PushObject(gvoDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIView3DLayerWrap layer3DSrc = view3DSrc.GetLayer(0);
		CGUIView3DLayerWrap layer3DDst = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer3DSrc.Clear();
		layer3DDst.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		int32_t i32PlaneCount = planeDetector3D.GetResultPlaneCount();
		CFLPoint<double> flp(0, 0);
		CFLString<wchar_t> flsDstLayerString;
		flsDstLayerString.Format(L"Plane Count %d", i32PlaneCount);
		if((res = layer3DSrc.DrawTextCanvas(flp, L"Source Object", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DDst.DrawTextCanvas(flp, L"Destination Object", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DDst.DrawTextCanvas(CFLPoint<double>(0, 30), flsDstLayerString, YELLOW, BLACK, 12)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		bool bDrawTextIndex = true;
		bool bDrawTextCount = true;
		bool bDrawTextArea = false;
		// text
		if(bDrawTextIndex || bDrawTextCount || bDrawTextArea)
		{
			CPlaneDetector3D::EPlaneFittingTarget ePlaneFittingTarget = planeDetector3D.GetPlaneFittingTarget();
			bool bVertexNormal = ePlaneFittingTarget == CPlaneDetector3D::EPlaneFittingTarget_VertexNormal;
			CFLArray<TPoint3<float>> flaResultPlaneCentroids;
			CFLArray<int32_t> flaResultPlaneTargetCounts;
			CFLArray<float> flaResultPlaneTargetAreas;
			if((res = planeDetector3D.GetResultPlaneCentroids(flaResultPlaneCentroids)).IsFail())
				break;
			if((res = planeDetector3D.GetResultPlaneTargetCounts(flaResultPlaneTargetCounts)).IsFail())
				break;
			if((res = planeDetector3D.GetResultPlaneAreas(flaResultPlaneTargetAreas)).IsFail())
				break;

			for(int32_t i32PlaneIndex = 0; i32PlaneIndex < i32PlaneCount; i32PlaneIndex++)
			{
				CFLString<wchar_t> flsTotal;
				CFLString<wchar_t> fls;
				TPoint3<double> tp3Centroid;
				tp3Centroid.x = flaResultPlaneCentroids[i32PlaneIndex].x;
				tp3Centroid.y = flaResultPlaneCentroids[i32PlaneIndex].y;
				tp3Centroid.z = flaResultPlaneCentroids[i32PlaneIndex].z;
				int32_t i32PlaneTargetCount = flaResultPlaneTargetCounts[i32PlaneIndex];
				float f32PlaneArea = flaResultPlaneTargetAreas[i32PlaneIndex];

				if(bDrawTextIndex)
				{
					fls.Format(L"[%d]\n", i32PlaneIndex);
					flsTotal += fls;
				}
				if(bDrawTextCount)
				{
					fls.Format(L"Count %d\n", i32PlaneTargetCount);
					flsTotal += fls;
				}
				if(bDrawTextArea)
				{
					fls.Format(L"Area %.3f\n", f32PlaneArea);
					flsTotal += fls;
				}
				layer3DDst.DrawText3D(tp3Centroid, flsTotal, YELLOW, 0, 9);
			}
		}

		// Zoom Fit
		view3DSrc.ZoomFit();
		view3DDst.ZoomFit();

		// 이미지 뷰를 갱신 합니다. // Update image view
		view3DSrc.Invalidate(true);
		view3DDst.Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림
		while(view3DSrc.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}