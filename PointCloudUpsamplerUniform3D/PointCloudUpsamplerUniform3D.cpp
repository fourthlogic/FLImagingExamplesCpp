#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 뷰 선언 // Declare image view
	CGUIView3DWrap view3DSrc;
	CGUIView3DWrap view3DDst;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult res = EResult_UnknownError;

		// Destination 3D 뷰 생성 // Create the destination 3D view
		if((res = view3DSrc.Create(100, 0, 600, 500)).IsFail() ||
		   (res = view3DDst.Create(600, 0, 1100, 500)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}
		
		view3DSrc.SetTopologyType(ETopologyType3D_PointCloud);
		view3DDst.SetTopologyType(ETopologyType3D_PointCloud);

		view3DDst.PushObject(CFL3DObject());
		CGUIView3DObject* pViewObjectDst = (CGUIView3DObject*)view3DDst.GetView3DObject(0);
		CFL3DObject& floDst = *(CFL3DObject*)pViewObjectDst->Get3DObject();

		view3DSrc.PushObject(CFL3DObject());
		CGUIView3DObject* pViewObjectSrc = (CGUIView3DObject*)view3DSrc.GetView3DObject(0);
		CFL3DObject& floSrc = *(CFL3DObject*)pViewObjectSrc->Get3DObject();

		if((res = floSrc.Load(L"../../ExampleImages/CoordinateFrameUnification3D/Office_mosaicked(Middle).ply")).IsFail())
		{
			ErrorPrint(res, L"Failed to load source object.");
			break;
		}

		// Uniform Upsampler 객체 생성 // Create Uniform Upsampler object
		CPointCloudUpsamplerUniform3D pointCloudUpsamplerUniform3D;

		// 파라미터 설정 // Set parameter
		pointCloudUpsamplerUniform3D.SetSourceObject(floSrc);
		pointCloudUpsamplerUniform3D.SetDestinationObject(floDst);
		pointCloudUpsamplerUniform3D.SetColoringMode(CPointCloudUpsampler3DBase::EColoringMode_Interpolate);
		pointCloudUpsamplerUniform3D.EnableNormalInterpolation(true);
		pointCloudUpsamplerUniform3D.SetSamplingSize((int64_t)1e7);
		pointCloudUpsamplerUniform3D.EnableCopyVertex(true);
		pointCloudUpsamplerUniform3D.EnableFaceReconstruction(false);
		pointCloudUpsamplerUniform3D.EnableFaceRetainment(false);
		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIView3DLayerWrap layer3DSrc = view3DSrc.GetLayer(0);
		CGUIView3DLayerWrap layer3DDst = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer3DSrc.Clear();
		layer3DDst.Clear();

		CFLPoint<double> flpTopLeft(0, 0);

		if((res = layer3DSrc.DrawTextCanvas(flpTopLeft, L"Source Object", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DDst.DrawTextCanvas(flpTopLeft, L"Destination Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		if((res = pointCloudUpsamplerUniform3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute.");
			break;
		}

		view3DSrc.UpdateObject(0);
		view3DDst.UpdateObject(0);

		pViewObjectSrc->UpdateAll();
		pViewObjectDst->UpdateAll();

		view3DSrc.SynchronizePointOfView(&view3DDst);
		view3DSrc.SynchronizeWindow(&view3DDst);

		CFL3DCamera cam;

		cam.SetProjectionType(E3DCameraProjectionType_Perspective);
		cam.SetDirection(CFLPoint3<float>(0.327705, 0.066764, -0.942418));
		cam.SetDirectionUp(CFLPoint3<float>(0.311277, 0.839746, -0.444896));
		cam.SetPosition(CFLPoint3<float>(-1.825832, 0.425620, 1.548716));
		cam.SetAngleOfViewY(45);

		view3DDst.SetCamera(cam);

		// 뷰가 종료될 때까지 계속 실행
		while(view3DSrc.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}