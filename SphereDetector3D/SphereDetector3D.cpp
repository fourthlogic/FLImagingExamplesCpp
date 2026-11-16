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

	// 뷰 선언 // Declare view
	CGUIView3DWrap view3DSrc;
	CGUIView3DWrap view3DResult;

	CResult res = EResult_UnknownError;

	do
	{
		// 법선과 색상이 포함된 원본 점군 로드 // Load the source point cloud with normals and colors
		if((res = floSrcObject.Load(L"../../ExampleImages/SphereDetector3D/Source.fl3do")).IsFail())
		{
			ErrorPrint(res, L"Failed to load ../../ExampleImages/SphereDetector3D/Source.fl3do.\n");
			break;
		}

		// 뷰 생성 // Create view
		if((res = view3DSrc.Create(0, 0, 640, 640)).IsFail() ||
		   (res = view3DResult.Create(640, 0, 1280, 640)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the source and result 3D views.\n");
			break;
		}

		// 원본 장면을 두 뷰에 표시 // Display the source scene on both views
		if((res = view3DSrc.PushObject(floSrcObject)).IsFail() ||
		   (res = view3DResult.PushObject(floSrcObject)).IsFail())
		{
			ErrorPrint(res, L"Failed to display the source point cloud on the 3D views.\n");
			break;
		}

		// 알고리즘 객체 생성 // Create algorithm object
		CSphereDetector3D sphereDetector3D;

		// 구의 대략적인 크기와 개수를 설정 // Set the approximate sphere size and count
		// 작은 구의 관측 점도 포함하도록 최소 inlier 개수를 설정
		// Set the minimum inlier count to include the observed points of the small sphere
		if((res = sphereDetector3D.SetSourceObject(floSrcObject)).IsFail() ||
		   (res = sphereDetector3D.SetRadiusRange(0.55f, 1.65f)).IsFail() ||
		   (res = sphereDetector3D.SetMaxCount(4)).IsFail() ||
		   (res = sphereDetector3D.SetMinimumInlierCount(40)).IsFail() ||
		   (res = sphereDetector3D.SetDistanceThreshold(0.03f)).IsFail() ||
		   (res = sphereDetector3D.SetFitCosineSimilarity(0.98)).IsFail() ||
		   (res = sphereDetector3D.SetMinimumSurfaceCoverage(0.25f)).IsFail() ||
		   (res = sphereDetector3D.SetScoreItem(CSphereDetector3D::EScoreItem_SurfaceCoverage)).IsFail() ||
		   (res = sphereDetector3D.SetIterationCount(5000)).IsFail() ||
		   (res = sphereDetector3D.SetRefinementIterationCount(10)).IsFail())
		{
			ErrorPrint(res, L"Failed to set the Sphere Detector 3D source object or detection parameters.\n");
			break;
		}

		// 알고리즘 수행 // Execute the algorithm
		if((res = sphereDetector3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute Sphere Detector 3D.\n");
			break;
		}

		// 결과 개수, inlier 개수 및 구면 Coverage 가져오기
		// Get the result count, inlier counts and surface coverages
		const int64_t i64SphereCount = sphereDetector3D.GetResultCount();
		CFLArray<int64_t> flaInlierCounts;
		CFLArray<float> flaSurfaceCoverages;

		if((res = sphereDetector3D.GetResultInlierCounts(flaInlierCounts)).IsFail() ||
		   (res = sphereDetector3D.GetResultSurfaceCoverages(flaSurfaceCoverages)).IsFail())
		{
			ErrorPrint(res, L"Failed to retrieve the result inlier counts or surface coverages.\n");
			break;
		}

		// 화면에 출력하기 위해 3D View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from 3D view for display
		// 이 객체는 3D 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to a 3D view and does not need to be released separately
		CGUIView3DLayerWrap layer3DSrc = view3DSrc.GetLayer(0);
		CGUIView3DLayerWrap layer3DResult = view3DResult.GetLayer(0);
		CFLString<wchar_t> flsCount;
		flsCount.Format(L"Sphere Count %lld", i64SphereCount);

		if((res = layer3DSrc.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Object", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DResult.DrawTextCanvas(CFLPoint<double>(0, 0), L"Sphere Detection Result", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DResult.DrawTextCanvas(CFLPoint<double>(0, 30), flsCount, YELLOW, BLACK, 12)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw the view titles or sphere count.\n");
			break;
		}

		wprintf(L"Sphere Count: %lld\n", i64SphereCount);

		for(int64_t i = 0; i < i64SphereCount; ++i)
		{
			// 결과 구를 3D ROI로 표시 // Display the result sphere as a 3D ROI
			CFLSphere3<double> flsp;

			if((res = sphereDetector3D.GetResultSphere(flsp, i)).IsFail() ||
			   (res = view3DResult.PushBackROI(flsp)).IsFail())
			{
				ErrorPrint(res, L"Failed to retrieve a result sphere or display it as a 3D ROI.\n");
				break;
			}

			const CFLPoint3<double> flpCenter = flsp.GetCenter3D();
			const TPoint3<double> tpCenter(flpCenter.x, flpCenter.y, flpCenter.z);
			CFLString<wchar_t> flsText;
			// 구 위에는 번호를, 화면 상단에는 상세 결과를 표시
			// Display the index on the sphere and the detailed results at the top of the view
			flsText.Format(L"[%lld]", i);

			if((res = layer3DResult.DrawText3D(tpCenter, flsText, YELLOW, BLACK, 12)).IsFail())
			{
				ErrorPrint(res, L"Failed to draw the result sphere index in 3D.\n");
				break;
			}

			flsText.Format(L"[%lld]  Radius: %.3f  Inliers: %lld  Coverage: %.1f%%",
				i, flsp.GetRadius(), flaInlierCounts[i], flaSurfaceCoverages[i] * 100.f);

			if((res = layer3DResult.DrawTextCanvas(CFLPoint<double>(0, 50 + 18 * i), flsText, YELLOW, BLACK, 11)).IsFail())
			{
				ErrorPrint(res, L"Failed to draw the result sphere radius, inlier count and coverage.\n");
				break;
			}

			wprintf(L"[%lld] Center: (%.3f, %.3f, %.3f), Radius: %.3f, Inliers: %lld, Coverage: %.1f%%\n",
				i, tpCenter.x, tpCenter.y, tpCenter.z, flsp.GetRadius(), flaInlierCounts[i], flaSurfaceCoverages[i] * 100.f);
		}

		if(res.IsFail())
			break;

		// 원본 장면을 관측 방향에서 바라보기 // View the source scene from the observation direction
		if((res = view3DSrc.GetCamera()->SetPosition(CFLPoint3<float>(13.f, -20.f, 39.f))).IsFail() ||
		   (res = view3DSrc.GetCamera()->SetTarget(CFLPoint3<float>(0.f, 0.f, 0.f))).IsFail() ||
		   (res = view3DSrc.GetCamera()->SetDirectionUp(CFLPoint3<float>(0.f, 0.f, 1.f))).IsFail() ||
		   (res = view3DSrc.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to set the source camera viewpoint or fit the source scene.\n");
			break;
		}

		// 결과 뷰는 원본 뷰와 동일한 시점으로 표시 // Display the result view from the same viewpoint as the source view
		if((res = view3DResult.SetCamera(*view3DSrc.GetCamera())).IsFail() ||
		   (res = view3DSrc.SynchronizePointOfView(&view3DResult)).IsFail())
		{
			ErrorPrint(res, L"Failed to copy and synchronize the source and result camera viewpoints.\n");
			break;
		}

		// 뷰 갱신 // Update view
		view3DSrc.Invalidate(true);
		view3DResult.Invalidate(true);

		// 뷰가 종료될 때까지 기다림 // Wait for the views to close
		while(view3DSrc.IsAvailable() && view3DResult.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}
