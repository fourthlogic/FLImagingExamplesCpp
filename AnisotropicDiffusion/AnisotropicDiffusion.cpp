#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

enum EType
{
	EType_Source = 0,
	EType_Destination1,
	EType_Destination2,
	EType_Destination3,
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
	CGUIViewImageWrap viewImage[ETypeCount];

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res = arrFliImage[EType_Source].Load(L"../../ExampleImages/NoiseImage/NoiseImage1.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		bool bError = false;

		for(int32_t i = EType_Destination1; i < ETypeCount; ++i)
		{
			// Destination 이미지를 Source 이미지와 동일한 이미지로 생성 // Create destination image as same as source image
			if(IsFail(res = arrFliImage[i].Assign(arrFliImage[EType_Source])))
			{
				ErrorPrint(res, "Failed to assign the image file.\n");
				bError = true;
				break;
			}
		}

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			int32_t x = i % 2;
			int32_t y = i / 2;

			// 이미지 뷰 생성 // Create image view
			if(IsFail(res = viewImage[i].Create(x * 400 + 400, y * 400, x * 400 + 400 + 400, y * 400 + 400)))
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				bError = true;
				break;
			}

			// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
			if(IsFail(res = viewImage[i].SetImagePtr(&arrFliImage[i])))
			{
				ErrorPrint(res, "Failed to set image object on the image view.\n");
				bError = true;
				break;
			}

			if(i == EType_Source)
				continue;

			// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
			if(IsFail(res = viewImage[EType_Source].SynchronizePointOfView(&viewImage[i])))
			{
				ErrorPrint(res, "Failed to synchronize view\n");
				bError = true;
				break;
			}

			// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
			if(IsFail(res = viewImage[EType_Source].SynchronizeWindow(&viewImage[i])))
			{
				ErrorPrint(res, "Failed to synchronize window.\n");
				bError = true;
				break;
			}
		}

		// AnisotropicDiffusion 객체 생성 // Create AnisotropicDiffusion object
		CAnisotropicDiffusion anisotropicDiffusion;

		// Source 이미지 설정 // Set the source image
		anisotropicDiffusion.SetSourceImage(arrFliImage[EType_Source]);

		// Destination1 이미지 설정 // Set the destination image
		anisotropicDiffusion.SetDestinationImage(arrFliImage[EType_Destination1]);

		// diffusion coefficient 모드 설정(Parabolic) // Set the diffusion coefficient mode
		anisotropicDiffusion.SetDiffusionMode(CAnisotropicDiffusion::EDiffusionCoefficientMode_Parabolic);

		// Contrast = 3 설정 // Set the Contrast = 3
		anisotropicDiffusion.SetContrast(3.0);

		// Theta = 3 설정 // Set the Theta = 3
		anisotropicDiffusion.SetTheta(3.0);

		// Iteration = 5 설정 // Set the Iteration = 5
		anisotropicDiffusion.SetIteration(5);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = anisotropicDiffusion.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute AnisotropicDiffusion.\n");
			wprintf(res.GetString());
			break;
		}

		// Destination2 이미지 설정 // Set the destination image
		anisotropicDiffusion.SetDestinationImage(arrFliImage[EType_Destination2]);

		// diffusion coefficient 모드 설정(Perona-Malik) // Set the diffusion coefficient mode
		anisotropicDiffusion.SetDiffusionMode(CAnisotropicDiffusion::EDiffusionCoefficientMode_PeronaMalik);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = anisotropicDiffusion.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute AnisotropicDiffusion.\n");
			wprintf(res.GetString());
			break;
		}

		// Destination3 이미지 설정 // Set the destination image
		anisotropicDiffusion.SetDestinationImage(arrFliImage[EType_Destination3]);

		// diffusion coefficient 모드 설정(Weickert) // Set the diffusion coefficient mode
		anisotropicDiffusion.SetDiffusionMode(CAnisotropicDiffusion::EDiffusionCoefficientMode_Weickert);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = anisotropicDiffusion.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute AnisotropicDiffusion.\n");
			wprintf(res.GetString());
			break;
		}

		CGUIViewImageLayerWrap arrLayer[ETypeCount];

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
			// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately

			arrLayer[i] = viewImage[i].GetLayer(0);
			arrLayer[i].Clear();
		}

		// 이미지 뷰 정보 표시 // Display image view information
		if((res = arrLayer[EType_Source].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if((res = arrLayer[EType_Destination1].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination1 Image (Parabolic Mode)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if((res = arrLayer[EType_Destination2].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination2 Image (PeronaMalik Mode)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if((res = arrLayer[EType_Destination3].DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination3 Image (Weickert Mode)", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		for(int32_t i = 0; i < ETypeCount; ++i)
			viewImage[i].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		bool bAvailable = true;
		while(bAvailable)
		{
			for(int32_t i = 0; i < ETypeCount; ++i)
			{
				bAvailable = viewImage[i].IsAvailable();
				if(!bAvailable)
					break;
			}

			CThreadUtilities::Sleep(1);
		}
	}
	while(false);

	return 0;
}