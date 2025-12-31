#include <cstdio>
#include "../CommonHeader/ErrorPrint.h"
#include <FLImaging.h>


enum EType
{
	EType_Source = 0,
	EType_DistanceTransform1,
	EType_DistanceTransform2,
	EType_DistanceTransform3,
	EType_DistanceTransform4,
	ETypeCount,
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage arrFliImage[ETypeCount];

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap arrViewImage[ETypeCount];

	do
	{
		// 알고리즘 동작 결과 // Algorithmic executation result
		CResult res = EResult_UnknownError;

		// Source 이미지 로드 // Load the source image
		if(IsFail(res = arrFliImage[EType_Source].Load(L"../../ExampleImages/DistanceTransform/circle.flif")))
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// Destination1 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination1 image as same as source image
		if(IsFail(res = arrFliImage[EType_DistanceTransform1].Assign(arrFliImage[EType_Source])))
		{
			ErrorPrint(res, L"Failed to assign the image file.\n");
			break;
		}

		// Destination2 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination2 image as same as source image
		if(IsFail(res = arrFliImage[EType_DistanceTransform2].Assign(arrFliImage[EType_Source])))
		{
			ErrorPrint(res, L"Failed to assign the image file.\n");
			break;
		}

		// Destination3 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination3 image as same as source image
		if(IsFail(res = arrFliImage[EType_DistanceTransform3].Assign(arrFliImage[EType_Source])))
		{
			ErrorPrint(res, L"Failed to assign the image file.\n");
			break;
		}

		// Destination4 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination4 image as same as source image
		if(IsFail(res = arrFliImage[EType_DistanceTransform4].Assign(arrFliImage[EType_Source])))
		{
			ErrorPrint(res, L"Failed to assign the image file.\n");
			break;
		}

		// Source 이미지 뷰 생성 // Create Source image view
		if(IsFail(res = arrViewImage[EType_Source].Create(100, 0, 612, 512)))
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination1 이미지 뷰 생성 // Create destination1 image view
		if(IsFail(res = arrViewImage[EType_DistanceTransform1].Create(612, 0, 1124, 512)))
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination2 이미지 뷰 생성 // Create destination2 image view
		if(IsFail(res = arrViewImage[EType_DistanceTransform2].Create(100, 512, 612, 1024)))
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination3 이미지 뷰 생성 // Create the destination3 image view
		if(IsFail(res = arrViewImage[EType_DistanceTransform3].Create(612, 512, 1124, 1024)))
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination4 이미지 뷰 생성 // Create the destination4 image view
		if(IsFail(res = arrViewImage[EType_DistanceTransform4].Create(1124, 512, 1636, 1024)))
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}



		bool bError = false;

		// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			if(IsFail(res = arrViewImage[i].SetImagePtr(&arrFliImage[i])))
			{
				ErrorPrint(res, L"Failed to set image object on the image view.\n");
				bError = true;
				break;
			}
		}

		if(bError)
			break;

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = arrViewImage[EType_Source].SynchronizePointOfView(&arrViewImage[EType_DistanceTransform1])))
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = arrViewImage[EType_Source].SynchronizePointOfView(&arrViewImage[EType_DistanceTransform2])))
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = arrViewImage[EType_Source].SynchronizePointOfView(&arrViewImage[EType_DistanceTransform3])))
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = arrViewImage[EType_Source].SynchronizePointOfView(&arrViewImage[EType_DistanceTransform4])))
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if(IsFail(res = arrViewImage[EType_Source].SynchronizeWindow(&arrViewImage[EType_DistanceTransform1])))
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if(IsFail(res = arrViewImage[EType_Source].SynchronizeWindow(&arrViewImage[EType_DistanceTransform2])))
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if(IsFail(res = arrViewImage[EType_Source].SynchronizeWindow(&arrViewImage[EType_DistanceTransform3])))
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if(IsFail(res = arrViewImage[EType_Source].SynchronizeWindow(&arrViewImage[EType_DistanceTransform4])))
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// Utility DistanceTransform 객체 생성 // Create Utility DistanceTransform object
		CDistanceTransform distance;
		// Source 이미지 설정 // Set the source image
		distance.SetSourceImage(arrFliImage[EType_Source]);
		// Destination 이미지 설정 // Set the destination image
		distance.SetDestinationImage(arrFliImage[EType_DistanceTransform1]);
		// Image DistanceTransform 측정 방식을 Euclid로 설정
		distance.SetMeasurementMethod(CDistanceTransform::EMeasurementMethod_Euclid);
		
		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = distance.Execute()))
		{
			ErrorPrint(res, L"Failed to execute distance.");
			break;
		}

		// Destination 이미지 설정 // Set the destination image
		distance.SetDestinationImage(arrFliImage[EType_DistanceTransform2]);
		// Image DistanceTransform 측정 방식을 CityBlock로 설정
		distance.SetMeasurementMethod(CDistanceTransform::EMeasurementMethod_CityBlock);
		
		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = distance.Execute()))
		{
			ErrorPrint(res, L"Failed to execute distance.");
			break;
		}

		// Destination 이미지 설정 // Set the destination image
		distance.SetDestinationImage(arrFliImage[EType_DistanceTransform3]);
		// Image DistanceTransform 측정 방식을 Chessboard로 설정
		distance.SetMeasurementMethod(CDistanceTransform::EMeasurementMethod_Chessboard);
		
		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = distance.Execute()))
		{
			ErrorPrint(res, L"Failed to execute distance.");
			break;
		}

		// Destination 이미지 설정 // Set the destination image
		distance.SetDestinationImage(arrFliImage[EType_DistanceTransform4]);
		// Image DistanceTransform 측정 방식을 QuasiEuclid로 설정
		distance.SetMeasurementMethod(CDistanceTransform::EMeasurementMethod_QuasiEuclid);
		
		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = distance.Execute()))
		{
			ErrorPrint(res, L"Failed to execute distance.");
			break;
		}

		CGUIViewImageLayerWrap arrLayer[ETypeCount];

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
			// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
			arrLayer[i] = arrViewImage[i].GetLayer(0);

			// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
			arrLayer[i].Clear();
		}

		// 이미지 뷰 정보 표시 // Display image view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = arrLayer[EType_Source].DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_DistanceTransform1].DrawTextCanvas(CFLPoint<double>(0, 0), L"DistanceTransform1 Euclid", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_DistanceTransform2].DrawTextCanvas(CFLPoint<double>(0, 0), L"DistanceTransform2 CityBlock", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_DistanceTransform3].DrawTextCanvas(CFLPoint<double>(0, 0), L"DistanceTransform3 Chessboard", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_DistanceTransform4].DrawTextCanvas(CFLPoint<double>(0, 0), L"DistanceTransform4 Qusai-Euclid", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 // Update image view
		arrViewImage[EType_Source].Invalidate(true);
		arrViewImage[EType_DistanceTransform1].Invalidate(true);
		arrViewImage[EType_DistanceTransform2].Invalidate(true);
		arrViewImage[EType_DistanceTransform3].Invalidate(true);
		arrViewImage[EType_DistanceTransform4].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(arrViewImage[EType_Source].IsAvailable()
			  && arrViewImage[EType_DistanceTransform1].IsAvailable()
			  && arrViewImage[EType_DistanceTransform2].IsAvailable()
			  && arrViewImage[EType_DistanceTransform3].IsAvailable()
			  && arrViewImage[EType_DistanceTransform4].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}