#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CFLImage fliSrcImage;
	CFLImage fliDstImage;
	CFL3DObject floDstObject;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageDst;
	CGUIView3DWrap view3DDst;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult res = EResult_UnknownError;

		// Source 이미지 로드 // Load the source image
		if((res = fliSrcImage.Load(L"../../ExampleImages/StationaryConeBeamRotationCT/")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		fliSrcImage.SelectPage(0);

		// Source 이미지 뷰 생성 // Create the source image view
		if((res = viewImageSrc.Create(100, 0, 548, 448)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Source 이미지 뷰에 이미지를 디스플레이 // Display the image in the source image view
		if((res = viewImageSrc.SetImagePtr(&fliSrcImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create the destination image view
		if((res = viewImageDst.Create(548, 0, 996, 448)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if((res = viewImageDst.SetImagePtr(&fliDstImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// Destination 3D 이미지 뷰 생성 // Create the destination 3D image view
		if((res = view3DDst.Create(548, 448, 996, 896)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		viewImageSrc.SetFixThumbnailView(true);

		// 알고리즘 객체 생성 // Create algorithm object
		CStationaryConeBeamRotationCT stationaryConeBeamRotationCT;

		if((res = stationaryConeBeamRotationCT.SetSourceImage(&fliSrcImage)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetDestinationImage(&fliDstImage)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetDestinationObject(&floDstObject)).IsFail())
			break;

		if((res = stationaryConeBeamRotationCT.SetOptimizationOption(CStationaryConeBeamRotationCT::EOptimizationOption_Faster)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetDetectorCellXSizeUnit(0.248046875)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetDetectorCellYSizeUnit(0.248046875)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetSourceDetectorDistanceUnit(597.00)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetPrincipalDeltaXPixel(0.00)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetPrincipalDeltaYPixel(0.00)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetObjectRotationAngleDistanceDegree(360.00)).IsFail())
			break;

		if((res = stationaryConeBeamRotationCT.SetMergeXCoefficient(1.00)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetMergeYCoefficient(1.00)).IsFail())
			break;

		Base::TPoint3<double> tpObjectRotationPivotUnit;
		tpObjectRotationPivotUnit.x = 0.00;
		tpObjectRotationPivotUnit.y = 0.00;
		tpObjectRotationPivotUnit.z = 28.6333;
		if((res = stationaryConeBeamRotationCT.SetObjectRotationPivotUnit(tpObjectRotationPivotUnit)).IsFail())
			break;
		Base::TPoint3<double> tpObjectRotationAxis;
		tpObjectRotationAxis.x = 0.00;
		tpObjectRotationAxis.y = -0.8191520442889918;
		tpObjectRotationAxis.z = -0.573576436351046;
		if((res = stationaryConeBeamRotationCT.SetObjectRotationAxis(tpObjectRotationAxis)).IsFail())
			break;
		Base::TPoint3<double> tpObjectVoxelSizeUnit;
		tpObjectVoxelSizeUnit.x = 0.01;
		tpObjectVoxelSizeUnit.y = 0.02;
		tpObjectVoxelSizeUnit.z = 0.01;
		if((res = stationaryConeBeamRotationCT.SetObjectVoxelSizeUnit(tpObjectVoxelSizeUnit)).IsFail())
			break;
		Base::TPoint3<int32_t> tpObjectVoxelCount;
		tpObjectVoxelCount.x = 200;
		tpObjectVoxelCount.y = 30;
		tpObjectVoxelCount.z = 200;
		if((res = stationaryConeBeamRotationCT.SetObjectVoxelCount(tpObjectVoxelCount)).IsFail())
			break;

		if((res = stationaryConeBeamRotationCT.SetNormalizedAirThreshold(0.60)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.EnableFrequencyRampFilter(true)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetFrequencyWindow(CStationaryConeBeamRotationCT::EFrequencyWindow_Gaussian)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetSigma(0.50)).IsFail())
			break;

		if((res = stationaryConeBeamRotationCT.EnableCircularMask(true)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetCircularMaskRadiusUnit(1.00)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetOutputFormat(CStationaryConeBeamRotationCT::EOutputFormat_U8)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetSigmoidB(4000.00)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetSigmoidM(0.00)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetIntensityThreshold(210)).IsFail())
			break;
		if((res = stationaryConeBeamRotationCT.SetSlicingPlane(CStationaryConeBeamRotationCT::ESlicingPlane_Transverse)).IsFail())
			break;

		// 알고리즘 수행 // Execute the algorithm
		if((res = stationaryConeBeamRotationCT.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute algorithm.");
			break;
		}


		// 3D 이미지 뷰에 Destination Object 를 디스플레이
		if((res = view3DDst.PushObject(floDstObject)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerDst = viewImageDst.GetLayer(0);
		CGUIView3DLayerWrap layer3D = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerDst.Clear();
		layer3D.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		CFLPoint<double> flp = new CFLPoint<double>();
		if((res = layerSrc.DrawTextCanvas(flp, L"Source Image", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layerDst.DrawTextCanvas(flp, L"Destination Image", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3D.DrawTextCanvas(flp, L"Destination Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		viewImageSrc.SetLayerAutoClearMode(ELayerAutoClearMode_PageChanged, false);
		viewImageDst.SetLayerAutoClearMode(ELayerAutoClearMode_PageChanged, false);

		// Zoom Fit
		viewImageSrc.ZoomFit();
		viewImageDst.ZoomFit();
		view3DDst.ZoomFit();
		
		// 3D 뷰 카메라 수정 // Modify 3D view camera
		CGUIView3DCamera cameraNew(*view3DDst.GetCamera());
		CFLPoint3<float> flpPositionOld = cameraNew.GetPosition();
		flpPositionOld.y = flpPositionOld.z;
		cameraNew.SetPosition(flpPositionOld, false);
		view3DDst.SetCamera(cameraNew);

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc.Invalidate(true);
		viewImageDst.Invalidate(true);
		view3DDst.Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림
		while(viewImageSrc.IsAvailable() && viewImageDst.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}