#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


enum EType
{
	EType_Src = 0,
	EType_Dst1,
	EType_Dst2,
	EType_Dst3,
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

	bool bError = false;

	do
	{
		CResult res = EResult_UnknownError;
		// Source 이미지 로드 // Load the source image
		if(IsFail(res = arrFliImage[EType_Src].Load(L"../../ExampleImages/Filter/Sun.flif")))
		{
			ErrorPrint(res, "Failed to load the image file.\n");
			break;
		}

		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			// Destination 이미지를 Src 이미지와 동일한 이미지로 생성
			if(i != EType_Src)
			{
				if(IsFail(res = arrFliImage[i].Assign(arrFliImage[EType_Src])))
				{
					ErrorPrint(res, "Failed to assign the image file.\n");
					bError = true;
					break;
				}
			}

			int32_t i32X = i % 2;
			int32_t i32Y = i / 2;

			// 이미지 뷰 생성 // Create image view
			if(IsFail(res = arrViewImage[i].Create(i32X * 400 + 400, i32Y * 400, i32X * 400 + 400 + 400, i32Y * 400 + 400)))
			{
				ErrorPrint(res, "Failed to create the image view.\n");
				bError = true;
				break;
			}

			// 이미지 뷰에 이미지를 디스플레이 // Display an image in an image view
			if(IsFail(res = arrViewImage[i].SetImagePtr(&arrFliImage[i])))
			{
				ErrorPrint(res, "Failed to set image object on the image view.\n");
				bError = true;
				break;
			}

			// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
			if(i != EType_Src)
			{
				if(IsFail(res = arrViewImage[EType_Src].SynchronizePointOfView(&arrViewImage[i])))
				{
					ErrorPrint(res, "Failed to synchronize view\n");
					bError = true;
					break;
				}

				// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
				if(IsFail(res = arrViewImage[EType_Src].SynchronizeWindow(&arrViewImage[i])))
				{
					ErrorPrint(res, "Failed to synchronize window.\n");
					bError = true;
					break;
				}
			}
		}

		if(bError)
			break;

		// Convolution UserDefinedKernel 객체 생성 // Create Convolution UserDefinedKernel object
		CConvolutionFilter convolution;

		// Source 이미지 설정 // Set the source image
		convolution.SetSourceImage(arrFliImage[EType_Src]);

		// 커널을 설정하기 위해 FLArray 생성
		CFLArray<CFLArray<float>> flarrKernel;
		CFLArray<float> flarrKernelElement;

		// 커널 생성 (Gaussian blur)
		// 1.f, 2.f, 1.f
		// 2.f, 4.f, 2.f
		// 1.f, 2.f, 1.f
		flarrKernel.Clear();

		flarrKernelElement.Clear();
		flarrKernelElement.Append(1.f);
		flarrKernelElement.Append(2.f);
		flarrKernelElement.Append(1.f);
		flarrKernel.Append(flarrKernelElement);

		flarrKernelElement.Clear();
		flarrKernelElement.Append(2.f);
		flarrKernelElement.Append(4.f);
		flarrKernelElement.Append(2.f);
		flarrKernel.Append(flarrKernelElement);

		flarrKernelElement.Clear();
		flarrKernelElement.Append(1.f);
		flarrKernelElement.Append(2.f);
		flarrKernelElement.Append(1.f);
		flarrKernel.Append(flarrKernelElement);

		// 커널 설정
		convolution.SetKernel(flarrKernel);

		// Destination 이미지 설정 // Set the destination image
		convolution.SetDestinationImage(arrFliImage[EType_Dst1]);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = convolution.Execute()))
		{
			ErrorPrint(res, "Failed to execute convolution.");
			break;
		}

		// 커널 생성 (Sharpen)
		// 0.f, -1.f, 0.f
		// -1.f, 5.f, -1.f
		// 0.f, -1.f, 0.f
		flarrKernel.Clear();

		flarrKernelElement.Clear();
		flarrKernelElement.Append(0.f);
		flarrKernelElement.Append(-1.f);
		flarrKernelElement.Append(0.f);
		flarrKernel.Append(flarrKernelElement);

		flarrKernelElement.Clear();
		flarrKernelElement.Append(-1.f);
		flarrKernelElement.Append(5.f);
		flarrKernelElement.Append(-1.f);
		flarrKernel.Append(flarrKernelElement);

		flarrKernelElement.Clear();
		flarrKernelElement.Append(0.f);
		flarrKernelElement.Append(-1.f);
		flarrKernelElement.Append(0.f);
		flarrKernel.Append(flarrKernelElement);

		// 커널 설정
		convolution.SetKernel(flarrKernel);

		// Destination 이미지 설정 // Set the destination image
		convolution.SetDestinationImage(arrFliImage[EType_Dst2]);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = convolution.Execute()))
		{
			ErrorPrint(res, "Failed to execute convolution.");
			break;
		}

		// 커널 생성 (Edge detection)
		// -1.f, -1.f, -1.f
		// -1.f, 8.f, -1.f
		// -1.f, -1.f, -1.f
		flarrKernel.Clear();

		flarrKernelElement.Clear();
		flarrKernelElement.Append(-1.f);
		flarrKernelElement.Append(-1.f);
		flarrKernelElement.Append(-1.f);
		flarrKernel.Append(flarrKernelElement);

		flarrKernelElement.Clear();
		flarrKernelElement.Append(-1.f);
		flarrKernelElement.Append(8.f);
		flarrKernelElement.Append(-1.f);
		flarrKernel.Append(flarrKernelElement);

		flarrKernelElement.Clear();
		flarrKernelElement.Append(-1.f);
		flarrKernelElement.Append(-1.f);
		flarrKernelElement.Append(-1.f);
		flarrKernel.Append(flarrKernelElement);

		// 커널 설정
		convolution.SetKernel(flarrKernel);

		// Destination 이미지 설정 // Set the destination image
		convolution.SetDestinationImage(arrFliImage[EType_Dst3]);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if(IsFail(res = convolution.Execute()))
		{
			ErrorPrint(res, "Failed to execute convolution.");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap arrLayer[ETypeCount];
		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			arrLayer[i] = arrViewImage[i].GetLayer(0);
		}

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if(IsFail(res = arrLayer[EType_Src].DrawTextCanvas(CFLPoint<double>(0, 0), L"Source Image", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_Dst1].DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image1", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_Dst2].DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image2", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = arrLayer[EType_Dst3].DrawTextCanvas(CFLPoint<double>(0, 0), L"Destination Image3", YELLOW, BLACK, 20)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}


		// 이미지 뷰를 갱신 합니다. // Update image view
		for(int32_t i = 0; i < ETypeCount; ++i)
		{
			arrViewImage[i].Invalidate(true);
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		bool bRun = true;
		while(bRun)
		{
			for(int32_t i = 0; i < ETypeCount; ++i)
			{
				bRun &= arrViewImage[i].IsAvailable();
			}

			CThreadUtilities::Sleep(1);
		}
	}
	while(false);

	return 0;
}