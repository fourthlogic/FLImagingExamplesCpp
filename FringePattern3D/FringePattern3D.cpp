#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CFLArray<CFLImage*> flaFliLrnImage;
	CFLArray<CFLImage*> flaFliSrcImage;
	CFLImage fliLearnImage[2];
	CFLImage fliSourceImage[2];
	CFLImage fliDstImage;
	CFLImage fliTxtImage;
	CFL3DObjectHeightMap floDstObject;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageLrn[2];
	CGUIViewImageWrap viewImageSrc[2];
	CGUIViewImageWrap viewImageDst;
	CGUIView3DWrap view3DDst;

	CResult res = EResult_UnknownError;

	do
	{
		// Learn 이미지 로드 // Load the reference plane image for calibration
		if((res = fliLearnImage[0].Load(L"../../ExampleImages/fringePattern3D/Learn0/")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 로드 // Load the source image
		if((res = fliSourceImage[0].Load(L"../../ExampleImages/fringePattern3D/Object0/")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Learn 이미지 로드 // Load the reference plane image for calibration
		if((res = fliLearnImage[1].Load(L"../../ExampleImages/fringePattern3D/Learn1/")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Source 이미지 로드 // Load the source image
		if((res = fliSourceImage[1].Load(L"../../ExampleImages/fringePattern3D/Object1/")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Texture 이미지 로드 // Load the texture image
		if((res = fliTxtImage.Load(L"../../ExampleImages/fringePattern3D/text.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// CFLArray<CFLImage*> 객체에 CFImage* 추가 // Push back CFImage* to CFLArray<CFLImage*> 
		flaFliLrnImage.PushBack(&fliLearnImage[0]);
		flaFliLrnImage.PushBack(&fliLearnImage[1]);
		flaFliSrcImage.PushBack(&fliSourceImage[0]);
		flaFliSrcImage.PushBack(&fliSourceImage[1]);

		// 이미지 뷰 생성 // Create the image view
		if((res = viewImageLrn[0].Create(100, 0, 600, 400)).IsFail() ||
		   (res = viewImageLrn[1].Create(600, 0, 1100, 400)).IsFail() ||
		   (res = viewImageSrc[0].Create(100, 400, 600, 800)).IsFail() ||
		   (res = viewImageSrc[1].Create(600, 400, 1100, 800)).IsFail() ||
		   (res = viewImageDst.Create(200, 200, 700, 600)).IsFail() ||
		   (res = view3DDst.Create(400, 100, 1150, 550)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if((res = viewImageSrc[0].SynchronizePointOfView(&viewImageSrc[1])).IsFail() ||
		   (res = viewImageSrc[0].SynchronizePointOfView(&viewImageLrn[0])).IsFail() ||
		   (res = viewImageSrc[0].SynchronizePointOfView(&viewImageLrn[1])).IsFail() ||
		   (res = viewImageSrc[0].SynchronizePointOfView(&viewImageDst)).IsFail())
		{

			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰의 페이지 인덱스를 동기화 한다 // Synchronize the page index of the two image views
		if((res = viewImageSrc[0].SynchronizePageIndex(&viewImageSrc[1])).IsFail() ||
		   (res = viewImageSrc[0].SynchronizePageIndex(&viewImageLrn[0])).IsFail() ||
		   (res = viewImageSrc[0].SynchronizePageIndex(&viewImageLrn[1])).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc[0].SynchronizeWindow(&viewImageSrc[1])).IsFail() ||
		   (res = viewImageSrc[0].SynchronizeWindow(&viewImageLrn[0])).IsFail() ||
		   (res = viewImageSrc[0].SynchronizeWindow(&viewImageLrn[1])).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if((res = viewImageLrn[0].SetImagePtr(flaFliLrnImage[0])).IsFail() ||
		   (res = viewImageLrn[1].SetImagePtr(flaFliLrnImage[1])).IsFail() ||
		   (res = viewImageSrc[0].SetImagePtr(flaFliSrcImage[0])).IsFail() ||
		   (res = viewImageSrc[1].SetImagePtr(flaFliSrcImage[1])).IsFail() ||
		   (res = viewImageDst.SetImagePtr(&fliDstImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}


		// 알고리즘 객체 생성 // Create algorithm object
		CFringePattern3D fringePattern3D;

		if((res = fringePattern3D.SetWorkingDistance(330)).IsFail())
			break;
		if((res = fringePattern3D.SetFieldOfView(400)).IsFail())
			break;
		if((res = fringePattern3D.SetAngleOfProjector(CMultiVar<double>(73, 105))).IsFail())
			break;
		if((res = fringePattern3D.SetBinInterval(CMultiVar<double>(1, 1))).IsFail())
			break;
		if((res = fringePattern3D.SetPatternType(CFringePattern3D::EPatternType_SquareWave)).IsFail())
			break;
		if((res = fringePattern3D.EnableNoiseReduction(true)).IsFail())
			break;

		if((res = fringePattern3D.SetLearnImage(flaFliLrnImage)).IsFail())
			break;

		// 알고리즘 Calibrate // Calibrate the algorithm
		if((res = fringePattern3D.Calibrate()).IsFail())
		{
			ErrorPrint(res, L"Failed to calibrate the algorithm.");
			break;
		}

		if((res = fringePattern3D.SetSourceImage(flaFliSrcImage)).IsFail())
			break;
		if((res = fringePattern3D.SetDestinationHeightMapImage(fliDstImage)).IsFail())
			break;
		if((res = fringePattern3D.SetDestinationObject(floDstObject)).IsFail())
			break;

		// 알고리즘 수행 // Execute the algorithm
		if((res = fringePattern3D.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute the algorithm.");
			break;
		}


		floDstObject.SetTextureImage(fliTxtImage);
		floDstObject.ActivateVertexColorTexture(true);

		// 3D 이미지 뷰에 Height Map (Destination Image) 이미지를 디스플레이 // Display the Height Map (Destination Image) on the 3D image view
		if((res = view3DDst.PushObject(floDstObject)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc0 = viewImageSrc[0].GetLayer(0);
		CGUIViewImageLayerWrap layerSrc1 = viewImageSrc[1].GetLayer(0);
		CGUIViewImageLayerWrap layerLrn0 = viewImageLrn[0].GetLayer(0);
		CGUIViewImageLayerWrap layerLrn1 = viewImageLrn[1].GetLayer(0);
		CGUIViewImageLayerWrap layerDst = viewImageDst.GetLayer(0);
		CGUIView3DLayerWrap layer3D = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc0.Clear();
		layerSrc1.Clear();
		layerLrn0.Clear();
		layerLrn1.Clear();
		layerDst.Clear();

		// 이미지 뷰 정보 표시 // Display image view information
		CFLPoint<double> flp = new CFLPoint<double>();
		if((res = layerLrn0.DrawTextCanvas(flp, L"Learn Image[0]", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layerLrn1.DrawTextCanvas(flp, L"Learn Image[1]", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layerSrc0.DrawTextCanvas(flp, L"Source Image[0]", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layerSrc1.DrawTextCanvas(flp, L"Source Image[1]", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3D.DrawTextCanvas(flp, L"Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		viewImageLrn[0].SetLayerAutoClearMode(ELayerAutoClearMode_PageChanged, false);
		viewImageLrn[1].SetLayerAutoClearMode(ELayerAutoClearMode_PageChanged, false);
		viewImageSrc[0].SetLayerAutoClearMode(ELayerAutoClearMode_PageChanged, false);
		viewImageSrc[1].SetLayerAutoClearMode(ELayerAutoClearMode_PageChanged, false);

		// Zoom Fit
		viewImageLrn[0].ZoomFit();
		viewImageLrn[1].ZoomFit();
		viewImageSrc[0].ZoomFit();
		viewImageSrc[1].ZoomFit();
		viewImageDst.ZoomFit();
		view3DDst.ZoomFit();

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageLrn[0].Invalidate(true);
		viewImageLrn[1].Invalidate(true);
		viewImageSrc[0].Invalidate(true);
		viewImageSrc[1].Invalidate(true);
		viewImageDst.Invalidate(true);
		view3DDst.Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림
		while(viewImageLrn[0].IsAvailable() && viewImageLrn[1].IsAvailable() && viewImageSrc[0].IsAvailable() && viewImageSrc[1].IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}