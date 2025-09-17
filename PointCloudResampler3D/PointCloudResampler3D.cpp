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
	CGUIView3DWrap view3DUpsample;
	CGUIView3DWrap view3DDownsample;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult res = EResult_UnknownError;

		// Destination 3D 뷰 생성 // Create the destination 3D view
		if((res = view3DSrc.Create(100, 0, 600, 500)).IsFail() ||
		   (res = view3DUpsample.Create(600, 0, 1100, 500)).IsFail() ||
		   (res = view3DDownsample.Create(1100, 0, 1600, 500)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}
		
		view3DSrc.SetTopologyType(ETopologyType3D_PointCloud);
		view3DUpsample.SetTopologyType(ETopologyType3D_PointCloud);
		view3DDownsample.SetTopologyType(ETopologyType3D_PointCloud);

		view3DUpsample.PushObject(CFL3DObject());
		CGUIView3DObject* pViewObjectUpsample = (CGUIView3DObject*)view3DUpsample.GetView3DObject(0);
		CFL3DObject& floUpsample = *(CFL3DObject*)pViewObjectUpsample->Get3DObject();

		view3DDownsample.PushObject(CFL3DObject());
		CGUIView3DObject* pViewObjectDownsample = (CGUIView3DObject*)view3DDownsample.GetView3DObject(0);
		CFL3DObject& floDownsample = *(CFL3DObject*)pViewObjectDownsample->Get3DObject();

		view3DSrc.PushObject(CFL3DObject());
		CGUIView3DObject* pViewObjectSrc = (CGUIView3DObject*)view3DSrc.GetView3DObject(0);
		CFL3DObject& floSrc = *(CFL3DObject*)pViewObjectSrc->Get3DObject();

		if((res = floSrc.Load(L"../../ExampleImages/CoordinateFrameUnification3D/Office_mosaicked(Middle).ply")).IsFail())
		{
			ErrorPrint(res, L"Failed to load source object.");
			break;
		}

		// Uniform Upsampler 객체 생성 // Create Uniform Upsampler object
		CPointCloudResampler3D pointCloudResampler3D;

		// 파라미터 설정 // Set parameter
		pointCloudResampler3D.SetSourceObject(floSrc);
		pointCloudResampler3D.SetColoringMode(EColoringMode_Interpolate);
		pointCloudResampler3D.EnableNormalInterpolation(true);
		pointCloudResampler3D.SetSamplingMode(CPointCloudResampler3D::ESamplingMode_Ratio_Strict);
		pointCloudResampler3D.EnableFaceReconstruction(false);
		pointCloudResampler3D.EnableFaceRetainment(false);

		pointCloudResampler3D.SetDestinationObject(floUpsample);
		pointCloudResampler3D.SetSampleRatio(20);

		if((res = pointCloudResampler3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute.");
			break;
		}

		pointCloudResampler3D.SetDestinationObject(floDownsample);
		pointCloudResampler3D.SetSampleRatio(0.15);


		if((res = pointCloudResampler3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIView3DLayerWrap layer3DSrc = view3DSrc.GetLayer(0);
		CGUIView3DLayerWrap layer3DUpsample = view3DUpsample.GetLayer(0);
		CGUIView3DLayerWrap layer3DDownsample = view3DDownsample.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer3DSrc.Clear();
		layer3DUpsample.Clear();
		layer3DDownsample.Clear();

		CFLPoint<double> flpTopLeft(0, 0);

		if((res = layer3DSrc.DrawTextCanvas(flpTopLeft, L"Source Object", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DUpsample.DrawTextCanvas(flpTopLeft, L"Destination Object(Upsample)", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DDownsample.DrawTextCanvas(flpTopLeft, L"Destination Object(Downsample)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		view3DSrc.UpdateObject(0);
		view3DUpsample.UpdateObject(0);
		view3DDownsample.UpdateObject(0);

		pViewObjectSrc->UpdateAll();
		pViewObjectUpsample->UpdateAll();
		pViewObjectDownsample->UpdateAll();

		view3DSrc.SynchronizePointOfView(&view3DUpsample);
		view3DSrc.SynchronizeWindow(&view3DUpsample);
		view3DSrc.SynchronizePointOfView(&view3DDownsample);
		view3DSrc.SynchronizeWindow(&view3DDownsample);

		CFL3DCamera cam;

		cam.SetProjectionType(E3DCameraProjectionType_Perspective);
		cam.SetDirection(CFLPoint3<float>(0.327705, 0.066764, -0.942418));
		cam.SetDirectionUp(CFLPoint3<float>(0.311277, 0.839746, -0.444896));
		cam.SetPosition(CFLPoint3<float>(-1.825832, 0.425620, 1.548716));
		cam.SetAngleOfViewY(45);

		view3DUpsample.SetCamera(cam);

		// 뷰가 종료될 때까지 계속 실행
		while(view3DSrc.IsAvailable() && view3DUpsample.IsAvailable() || view3DDownsample.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}