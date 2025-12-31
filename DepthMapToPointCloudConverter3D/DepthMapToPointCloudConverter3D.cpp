#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"
#include <map>

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CFLImage fliSource, fliTexture;
	CFL3DObject floDestination;
	CGUIView3DWrap view3D;
	CGUIViewImageWrap viewDepthImage;
	CGUIViewImageWrap viewTextureImage;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult eResult = EResult_UnknownError;

		// 이미지 로드 // Load the image
		if((eResult = fliSource.Load(L"../../ExampleImages/DepthMapToPointCloudConverter3D/Depth.flif")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the image.\n");
			break;
		}

		if((eResult = fliTexture.Load(L"../../ExampleImages/DepthMapToPointCloudConverter3D/Texture.flif")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the image.\n");
			break;
		}

		// DepthMapToPointCloudConverter3D 객체 생성 // Create DepthMapToPointCloudConverter3D object
		CDepthMapToPointCloudConverter3D depthMapToPointCloudConverter3D;

		// SourceROI 설정 // Set the source roi.
		CFLQuad<double> flfSourceROI(236.258951, 137.611646, 247.605353, 273.171284, 355.694756, 268.393852, 339.570922, 130.445497);
		depthMapToPointCloudConverter3D.SetSourceROI(flfSourceROI);


		// Source 이미지 설정 // Set the source image.
		depthMapToPointCloudConverter3D.SetSourceImage(fliSource);

		// Texture 이미지 설정 // Set the texture image.
		depthMapToPointCloudConverter3D.SetTextureImage(fliTexture);

		// Camera Matrix 설정 // Set the camera matrix
		CFLPoint<float> flpFocalLength;
		CFLPoint<float> flpPrincipalPoint;

		flpFocalLength.x = 582.2f;
		flpFocalLength.y = 582.7f;
		flpPrincipalPoint.x = 247;
		flpPrincipalPoint.y = 187.5f;
		
		depthMapToPointCloudConverter3D.SetIntrinsicParameter(flpFocalLength, flpPrincipalPoint);

		//왜곡 계수 설정 // Set the distortion coefficient
		CFLArray<double> flaDistortionCoefficient;

		flaDistortionCoefficient.PushBack(-0.2333453150000);
		flaDistortionCoefficient.PushBack(0.1352355330000);
		flaDistortionCoefficient.PushBack(0.0005843197580);
		flaDistortionCoefficient.PushBack(-0.0005675755000);
		flaDistortionCoefficient.PushBack(-0.0246060137000);

		depthMapToPointCloudConverter3D.SetDistortionCoefficient(flaDistortionCoefficient);

		// 법선 벡터 추정 여부 설정 // Set whether to estimate the normal vector
		depthMapToPointCloudConverter3D.EnableIncludingNormalVector(false);

		// Z축 방향 설정 // Set z-axis direction.
		depthMapToPointCloudConverter3D.SetDirectionType(EDirectionType_Increment);

		// Destination 3D Object 설정 // Set the destination 3D object
		depthMapToPointCloudConverter3D.SetDestinationObject(floDestination);

		// 이미지 뷰 생성 // Create image view
		if(IsFail(eResult = viewDepthImage.Create(100, 0, 612, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Source image view.\n");
			break;
		}

		if(IsFail(eResult = viewTextureImage.Create(612, 0, 1124, 512)))
		{
			ErrorPrint(eResult, "Failed to create the Texture image view.\n");
			break;
		}

		// 결과 3D 뷰 생성 // Create result 3D view
		if(IsFail(eResult = view3D.Create(100, 512, 612, 1024)))
		{
			ErrorPrint(eResult, "Failed to create the Result 3D view.\n");
			break;
		}

		// 이미지 포인터 설정 // Set image pointer
		viewDepthImage.SetImagePtr(&fliSource);
		viewTextureImage.SetImagePtr(&fliTexture);

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately		
		CGUIViewImageLayerWrap layerViewDepth = viewDepthImage.GetLayer(0);
		CGUIViewImageLayerWrap layerViewTexture = viewTextureImage.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerViewDepth.Clear();
		layerViewTexture.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if((eResult = layerViewDepth.DrawTextCanvas(CFLPoint<double>(0, 0), L"Depth Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		if((eResult = layerViewTexture.DrawTextCanvas(CFLPoint<double>(0, 0), L"Texture Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((eResult = depthMapToPointCloudConverter3D.Execute()).IsFail())
		{
			ErrorPrint(eResult, L"Failed to execute Depth Map To Point Cloud Converter 3D.");
			break;
		}

		view3D.PushObject(floDestination);
		view3D.UpdateObject();
		view3D.UpdateScreen();
		view3D.ZoomFit();

		viewDepthImage.ZoomFit();
		viewTextureImage.ZoomFit();

		viewDepthImage.Invalidate();
		viewTextureImage.Invalidate();

		// 이미지 뷰가 종료될 때 까지 기다림
		while(viewDepthImage.IsAvailable() && viewTextureImage.IsAvailable() && view3D.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}