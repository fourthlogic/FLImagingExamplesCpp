#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 뷰 선언 // Declare image view
	CGUIView3DWrap view3DSrc0;
	CGUIView3DWrap view3DSrc1;
	CGUIView3DWrap view3DWorld;
	CGUIView3DWrap view3DDst;
	CFL3DObject floSource0;
	CFL3DObject floSource1;
	CFL3DObject floWorld;
	CFL3DObject floDst;

	CResult res = EResult_UnknownError;

	do
	{
		// 데이터 로드 // Load data
		floSource0.Load(L"../../ExampleImages/CoordinateFrameUnification3D/Office_mosaicked(Left).ply");
		floWorld.Load(L"../../ExampleImages/CoordinateFrameUnification3D/Office_mosaicked(Middle).ply");
		floSource1.Load(L"../../ExampleImages/CoordinateFrameUnification3D/Office_mosaicked(Right).ply");

		CCoordinateFrameUnification3D coordinateFrameUnification3D;

		// Scene 0와 World 좌표 간 점 대응을 추가
		// Add point correpondence between Scene 0 & World

		CFLArray<TPoint3<float>> flaWorld0, flaScene0;

		flaWorld0.Resize(4);
		flaScene0.Resize(4);

		flaWorld0[0] = TPoint3<float>(0.316194f, 0.089235f, -0.955000f);
		flaScene0[0] = TPoint3<float>(0.048920f, 0.131229f, -0.824725f);
		flaWorld0[1] = TPoint3<float>(0.328092f, 0.086743f, -0.952000f);
		flaScene0[1] = TPoint3<float>(0.062442f, 0.128631f, -0.826201f);
		flaWorld0[2] = TPoint3<float>(0.465690f, 0.065212f, -0.920000f);
		flaScene0[2] = TPoint3<float>(0.202130f, 0.117711f, -0.854954f);
		flaWorld0[3] = TPoint3<float>(0.339934f, -0.020669f, -0.646000f);
		flaScene0[3] = TPoint3<float>(0.189541f, -0.046209f, -0.589000f);

		coordinateFrameUnification3D.AddSourceObject(floSource0, flaWorld0, flaScene0);

		// Scene 1과 World 좌표 간 점 대응을 추가
		// Add point correpondence between Scene 1 & World
		CFLArray<TPoint3<float>> flaWorld1, flaScene1;

		flaWorld1.Resize(6);
		flaScene1.Resize(6);

		flaWorld1[0] = TPoint3<float>(-0.553926f, 0.204508f, -1.155000f);
		flaScene1[0] = TPoint3<float>(0.202496f, 0.448916f, -0.853000f);
		flaWorld1[1] = TPoint3<float>(-0.552240f, 0.189193f, -1.160931f);
		flaScene1[1] = TPoint3<float>(0.208646f, 0.434687f, -0.859625f);
		flaWorld1[2] = TPoint3<float>(-0.479978f, 0.192098f, -1.145000f);
		flaScene1[2] = TPoint3<float>(0.251620f, 0.415887f, -0.796545f);
		flaWorld1[3] = TPoint3<float>(-0.477483f, 0.173172f, -1.146783f);
		flaScene1[3] = TPoint3<float>(0.258778f, 0.401190f, -0.810000f);
		flaWorld1[4] = TPoint3<float>(-0.406276f, -0.267226f, -0.835000f);
		flaScene1[4] = TPoint3<float>(0.138451f, -0.120545f, -0.677569f);
		flaWorld1[5] = TPoint3<float>(-0.016503f, -0.275241f, -1.050700f);
		flaScene1[5] = TPoint3<float>(0.568925f, -0.122618f, -0.588000f);

		coordinateFrameUnification3D.AddSourceObject(floSource1, flaWorld1, flaScene1);

		coordinateFrameUnification3D.SetDestinationObject(floDst);


		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = coordinateFrameUnification3D.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute.\n");
			break;
		}

		// 3D 뷰 생성 // Create the 3D views
		if((res = view3DSrc0.Create(100, 250, 600, 750)).IsFail() ||
		   (res = view3DWorld.Create(600, 0, 1100, 500)).IsFail() ||
		   (res = view3DSrc1.Create(1100, 250, 1600, 750)).IsFail() ||
		   (res = view3DDst.Create(600, 500, 1100, 1000)).IsFail())
		{
			ErrorPrint(res, "Failed to create the 3d view.\n");
			break;
		}

		view3DDst.SynchronizeWindow(&view3DSrc0);
		view3DDst.SynchronizeWindow(&view3DWorld);
		view3DDst.SynchronizeWindow(&view3DSrc1);

		view3DSrc0.PushObject(floSource0);
		view3DWorld.PushObject(floWorld);
		view3DSrc1.PushObject(floSource1);
		view3DDst.PushObject(floDst);

		view3DDst.SynchronizePointOfView(&view3DWorld);
		
		view3DDst.SynchronizePointOfView(&view3DSrc0);
		view3DDst.SynchronizePointOfView(&view3DSrc1);

		// 3D 뷰에 카메라 위치 직접 세팅 // Set 3d view camera pose manually
		CFL3DCamera cam;

		cam.SetProjectionType(E3DCameraProjectionType_Perspective);
		cam.SetDirection(CFLPoint3<float>(0.337466, -0.125061, -0.932993));
		cam.SetDirectionUp(CFLPoint3<float>(0.139977, 0.986837, -0.080987));
		cam.SetPosition(CFLPoint3<float>(-0.70, 0.16, 1.0));
		cam.SetAngleOfViewY(45);

		view3DWorld.SetCamera(cam);

		view3DDst.DesynchronizePointOfView(&view3DSrc0);
		view3DDst.DesynchronizePointOfView(&view3DSrc1);
		
		CGUIView3DLayerWrap layer3DSrc0 = view3DSrc0.GetLayer(0);
		CGUIView3DLayerWrap layer3DSrc1 = view3DSrc1.GetLayer(0);
		CGUIView3DLayerWrap layer3DWorld = view3DWorld.GetLayer(0);
		CGUIView3DLayerWrap layer3DDst = view3DDst.GetLayer(0);

		CFLPoint<double> flpTopLeft(0, 0);

		if((res = layer3DSrc0.DrawTextCanvas(flpTopLeft, L"Scene 0", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DWorld.DrawTextCanvas(flpTopLeft, L"World(Reference)", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DSrc1.DrawTextCanvas(flpTopLeft, L"Scene 1", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DDst.DrawTextCanvas(flpTopLeft, L"Result", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text.\n");
			break;
		}
		
		// 뷰를 갱신 // Update view
		view3DSrc0.Invalidate(true);
		view3DWorld.Invalidate(true);
		view3DSrc1.Invalidate(true);
		view3DDst.Invalidate(true);

		// 뷰가 종료될 때까지 기다림 // Wait for the view to close
		while(view3DSrc0.IsAvailable() && view3DSrc1.IsAvailable() && view3DWorld.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}