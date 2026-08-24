#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"



enum EImageType
{
	EImageType_Source = 0,
	EImageType_DestinationXY,
	EImageType_DestinationXZ,
	EImageType_DestinationYX,
	EImageType_DestinationYZ,
	EImageType_DestinationZX,
	EImageType_DestinationZY,
	EImageType_Count,
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare the image object
	CFLImage arrFliImage[EImageType_Count];

	// 이미지 뷰 선언 // Declare the image view
	CGUIViewImageWrap arrViewImage[EImageType_Count];

	// 알고리즘 동작 결과 // Algorithm execution result
	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res = arrFliImage[EImageType_Source].Load(L"../../ExampleImages/ImageTranspose/Gradation.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file. \n");
			break;
		}

		// Source 이미지 뷰 생성 // Create source image view
		if((res = arrViewImage[EImageType_Source].Create(100, 0, 700, 600)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view. \n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create destination image view
		if((res = arrViewImage[EImageType_DestinationXY].Create(700, 0, 1000, 300)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view. \n");
			break;
		}

		if((res = arrViewImage[EImageType_DestinationXZ].Create(1000, 0, 1300, 300)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view. \n");
			break;
		}

		if((res = arrViewImage[EImageType_DestinationYX].Create(1300, 0, 1600, 300)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view. \n");
			break;
		}

		if((res = arrViewImage[EImageType_DestinationYZ].Create(700, 300, 1000, 600)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view. \n");
			break;
		}

		if((res = arrViewImage[EImageType_DestinationZX].Create(1000, 300, 1300, 600)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view. \n");
			break;
		}

		if((res = arrViewImage[EImageType_DestinationZY].Create(1300, 300, 1600, 600)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view. \n");
			break;
		}

		// Source와 Desitination 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the source and destination image view windows
		if((res = arrViewImage[EImageType_Source].SynchronizeWindow(&arrViewImage[EImageType_DestinationXY])).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window. \n");
			break;
		}

		if((res = arrViewImage[EImageType_Source].SynchronizeWindow(&arrViewImage[EImageType_DestinationXZ])).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window. \n");
			break;
		}

		if((res = arrViewImage[EImageType_Source].SynchronizeWindow(&arrViewImage[EImageType_DestinationYX])).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window. \n");
			break;
		}

		if((res = arrViewImage[EImageType_Source].SynchronizeWindow(&arrViewImage[EImageType_DestinationYZ])).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window. \n");
			break;
		}

		if((res = arrViewImage[EImageType_Source].SynchronizeWindow(&arrViewImage[EImageType_DestinationZX])).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window. \n");
			break;
		}

		if((res = arrViewImage[EImageType_Source].SynchronizeWindow(&arrViewImage[EImageType_DestinationZY])).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window. \n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		for(int32_t i = 0; i < EImageType_Count; ++i)
		{
			if((res = arrViewImage[i].SetImagePtr(&arrFliImage[i])).IsFail())
			{
				ErrorPrint(res, L"Failed to set image object on the image view.\n");
				break;
			}
		}

		if(res.IsFail())
			break;

		// Image Transpose 객체 생성 // Create Image Transpose object
		CImageTranspose imageTranspose;

		// Source 이미지 설정 // Set the source image
		// 모든 Source 이미지는 동일한 사이즈와 포맷을 가져야합니다. // All Source images must have the same size and format.
		imageTranspose.SetSourceImage(arrFliImage[EImageType_Source]);

		// Destination 이미지 설정 // Set the destination image
		imageTranspose.SetDestinationImage(&arrFliImage[EImageType_DestinationXY]);

		// Transpose 후 사용자에게 보일 평면을 XY 평면으로 설정 // Set the plane to the XY plane to be visible to the user after Transpose
		imageTranspose.SetResultPlane(CImageTranspose::EPlane_XY);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = imageTranspose.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Image Transpose.\n");
			break;
		}

		// Destination 이미지 설정 // Set the destination image
		imageTranspose.SetDestinationImage(&arrFliImage[EImageType_DestinationXZ]);

		// Transpose 후 사용자에게 보일 평면을 XZ 평면으로 설정 // Set the plane to the XZ plane to be visible to the user after Transpose
		imageTranspose.SetResultPlane(CImageTranspose::EPlane_XZ);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = imageTranspose.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Image Transpose.\n");
			break;
		}

		// Destination 이미지 설정 // Set the destination image
		imageTranspose.SetDestinationImage(&arrFliImage[EImageType_DestinationYX]);

		// Transpose 후 사용자에게 보일 평면을 YX 평면으로 설정 // Set the plane to the YX plane to be visible to the user after Transpose
		imageTranspose.SetResultPlane(CImageTranspose::EPlane_YX);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = imageTranspose.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Image Transpose.\n");
			break;
		}

		// Destination 이미지 설정 // Set the destination image
		imageTranspose.SetDestinationImage(&arrFliImage[EImageType_DestinationYZ]);

		// Transpose 후 사용자에게 보일 평면을 YZ 평면으로 설정 // Set the plane to the YZ plane to be visible to the user after Transpose
		imageTranspose.SetResultPlane(CImageTranspose::EPlane_YZ);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = imageTranspose.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Image Transpose.\n");
			break;
		}

		// Destination 이미지 설정 // Set the destination image
		imageTranspose.SetDestinationImage(&arrFliImage[EImageType_DestinationZX]);

		// Transpose 후 사용자에게 보일 평면을 ZX 평면으로 설정 // Set the plane to the ZX plane to be visible to the user after Transpose
		imageTranspose.SetResultPlane(CImageTranspose::EPlane_ZX);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = imageTranspose.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Image Transpose.\n");
			break;
		}

		// Destination 이미지 설정 // Set the destination image
		imageTranspose.SetDestinationImage(&arrFliImage[EImageType_DestinationZY]);

		// Transpose 후 사용자에게 보일 평면을 ZY 평면으로 설정 // Set the plane to the ZY plane to be visible to the user after Transpose
		imageTranspose.SetResultPlane(CImageTranspose::EPlane_ZY);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = imageTranspose.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute Image Transpose.\n");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다.	// With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		arrViewImage[EImageType_DestinationXY].ZoomFit();
		arrViewImage[EImageType_DestinationXZ].ZoomFit();
		arrViewImage[EImageType_DestinationYX].ZoomFit();
		arrViewImage[EImageType_DestinationYZ].ZoomFit();
		arrViewImage[EImageType_DestinationZX].ZoomFit();
		arrViewImage[EImageType_DestinationZY].ZoomFit();

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSource = arrViewImage[EImageType_Source].GetLayer(0);
		CGUIViewImageLayerWrap layerDestinationXY = arrViewImage[EImageType_DestinationXY].GetLayer(0);
		CGUIViewImageLayerWrap layerDestinationXZ = arrViewImage[EImageType_DestinationXZ].GetLayer(0);
		CGUIViewImageLayerWrap layerDestinationYX = arrViewImage[EImageType_DestinationYX].GetLayer(0);
		CGUIViewImageLayerWrap layerDestinationYZ = arrViewImage[EImageType_DestinationYZ].GetLayer(0);
		CGUIViewImageLayerWrap layerDestinationZX = arrViewImage[EImageType_DestinationZX].GetLayer(0);
		CGUIViewImageLayerWrap layerDestinationZY = arrViewImage[EImageType_DestinationZY].GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSource.Clear();
		layerDestinationXY.Clear();
		layerDestinationXZ.Clear();
		layerDestinationYX.Clear();
		layerDestinationYZ.Clear();
		layerDestinationZX.Clear();
		layerDestinationZY.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if((res = layerSource.DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerDestinationXY.DrawTextCanvas(CFLPoint<double>(0, 0), L"XY Plane Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerDestinationXZ.DrawTextCanvas(CFLPoint<double>(0, 0), L"XZ Plane Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerDestinationYX.DrawTextCanvas(CFLPoint<double>(0, 0), L"YX Plane Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerDestinationYZ.DrawTextCanvas(CFLPoint<double>(0, 0), L"YZ Plane Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerDestinationZX.DrawTextCanvas(CFLPoint<double>(0, 0), L"ZX Plane Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerDestinationZY.DrawTextCanvas(CFLPoint<double>(0, 0), L"ZY Plane Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		arrViewImage[EImageType_Source].Invalidate(true);
		arrViewImage[EImageType_DestinationXY].Invalidate(true);
		arrViewImage[EImageType_DestinationXZ].Invalidate(true);
		arrViewImage[EImageType_DestinationYX].Invalidate(true);
		arrViewImage[EImageType_DestinationYZ].Invalidate(true);
		arrViewImage[EImageType_DestinationZX].Invalidate(true);
		arrViewImage[EImageType_DestinationZY].Invalidate(true);

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = arrViewImage[EImageType_DestinationXY].ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit\n");
			break;
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(arrViewImage[EImageType_Source].IsAvailable() && arrViewImage[EImageType_DestinationXY].IsAvailable() && arrViewImage[EImageType_DestinationXZ].IsAvailable() && arrViewImage[EImageType_DestinationYX].IsAvailable() && arrViewImage[EImageType_DestinationYZ].IsAvailable() && arrViewImage[EImageType_DestinationZX].IsAvailable() && arrViewImage[EImageType_DestinationZY].IsAvailable())
			CThreadUtilities::Sleep(1);

		for(int32_t i = 0; i < EImageType_Count; ++i)
			arrViewImage[i].Destroy();
	}
	while(false);

	return 0;
}