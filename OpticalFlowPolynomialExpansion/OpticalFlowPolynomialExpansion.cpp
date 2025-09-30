#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

class CMessageReceiver : public CFLBase
{
public:

	// CMessageReceiver 생성자
	CMessageReceiver(CGUIViewImageWrap* pViewImageSrc, CGUIViewImageWrap* pViewImageDst) : m_pViewImageSrc(pViewImageSrc), m_pViewImageDst(pViewImageDst), m_bViewImageAvailable(true)
	{
		// 메세지를 전달 받기 위해 CBroadcastManager 에 구독 등록
		CBroadcastManager::Subscribe(this);
	}

	// CMessageReceiver 소멸자
	virtual ~CMessageReceiver()
	{
		// 객체가 소멸할때 메세지 수신을 중단하기 위해 구독을 해제한다.
		CBroadcastManager::Unsubscribe(this);
	}

	DeclareGetClassType();

	// 메세지가 들어오면 호출되는 함수 OnReceiveBroadcast 오버라이드 하여 구현
	void OnReceiveBroadcast(const CBroadcastMessage* pMessage) override
	{
		do
		{
			// pMessage 가 nullptr 인지 확인
			if(pMessage == nullptr)
				break;

			// 메세지의 채널을 확인
			switch(pMessage->GetChannel())
			{
			case EGUIBroadcast_ViewImage_PreDestroy:
				{
					if(!m_bViewImageAvailable)
						break;

					// GetCaller() 가 등록한 이미지뷰인지 확인
					if(m_pViewImageSrc && pMessage->GetCaller() == (const CFLBase*)m_pViewImageSrc->GetMessageCallerPtr())
					{
						m_bViewImageAvailable = false;
						m_pViewImageSrc = nullptr;
					}

					if(m_pViewImageDst && pMessage->GetCaller() == (const CFLBase*)m_pViewImageDst->GetMessageCallerPtr())
					{
						m_bViewImageAvailable = false;
						m_pViewImageDst = nullptr;
					}
				}
				break;
			}
		}
		while(false);
	}

	bool IsViewImageAvailable() const
	{
		return m_bViewImageAvailable;
	}

protected:
	CGUIViewImageWrap* m_pViewImageSrc;
	CGUIViewImageWrap* m_pViewImageDst;
	bool m_bViewImageAvailable;
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliSrcImage;
	CFLImage fliDstImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImageSrc;
	CGUIViewImageWrap viewImageDst;

	// 메세지를 전달 받을 CMessageReceiver 객체 생성 // Create 메세지를 전달 받을 CMessageReceiver object
	CMessageReceiver msgReceiver(&viewImageSrc, &viewImageDst);

	// 알고리즘 동작 결과 // Algorithm execution result
	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res = fliSrcImage.Load(L"../../ExampleImages/OpticalFlowPolynomialExpansion/Highway.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		// OpticalFlowPolynomialExpansion 객체 생성 // Create OpticalFlowPolynomialExpansion object
		COpticalFlowPolynomialExpansion opticalFlowPolynomialExpansion;

		// Source 이미지 설정 // Set the source image
		opticalFlowPolynomialExpansion.SetSourceImage(fliSrcImage);
		// Destination 이미지 설정 // Set the destination image
		opticalFlowPolynomialExpansion.SetDestinationImage(fliDstImage);
		// Pyramid Level 설정 // Set Pyramid Level
		opticalFlowPolynomialExpansion.SetPyramidLevel(2);
		// Iteration 설정 // Set Iteration
		opticalFlowPolynomialExpansion.SetIteration(3);
		// Window Size 설정 // Set Window Size
		opticalFlowPolynomialExpansion.SetWindowSize(15);
		// Binning Size 설정 // Set Binning Size
		opticalFlowPolynomialExpansion.SetBinningSize(8);
		// Minimum Vector Size 설정 // Set  Minimum Vector Size
		opticalFlowPolynomialExpansion.SetMinimumVectorSize(5.000000);

		printf("Processing.....\n");

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = opticalFlowPolynomialExpansion.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute algorithm.");
			break;
		}

		// Source 이미지 뷰 생성 // Create the source image view
		if((res = viewImageSrc.Create(400, 0, 1012, 512)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 이미지 뷰 생성 // Create the destination image view
		if((res = viewImageDst.Create(1012, 0, 1624, 512)).IsFail())
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

		// Destination 이미지 뷰에 이미지를 디스플레이 // Display the image in the destination image view
		if((res = viewImageDst.SetImagePtr(&fliDstImage)).IsFail())
		{
			ErrorPrint(res, L"Failed to set image object on the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		//if((res = viewImageSrc.SynchronizePointOfView(&viewImageDst)).IsFail())
		//{
		//	ErrorPrint(res, L"Failed to synchronize view\n");
		//	break;
		//}

		// 두 이미지 뷰의 페이지를 동기화 한다
		if((res = viewImageSrc.SynchronizePageIndex(&viewImageDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if((res = viewImageSrc.SynchronizeWindow(&viewImageDst)).IsFail())
		{
			ErrorPrint(res, L"Failed to synchronize window.\n");
			break;
		}

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = viewImageDst.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit of the image view.\n");
			break;
		}

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerDst = viewImageDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerDst.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic
		if((res = layerSrc.DrawTextCanvas(&CFLPoint<double>(0, 30), L"Source Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		if((res = layerDst.DrawTextCanvas(&CFLPoint<double>(0, 30), L"Destination Image", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		viewImageSrc.Invalidate(true);
		viewImageDst.Invalidate(true);

		// 이미지 페이지 변경으로 인한 Auto Clear Mode 비활성화
		viewImageSrc.SetLayerAutoClearMode(0, ELayerAutoClearMode_PageChanged, false);
		viewImageDst.SetLayerAutoClearMode(0, ELayerAutoClearMode_PageChanged, false);
		viewImageSrc.SetLayerAutoClearMode(1, ELayerAutoClearMode_PageChanged, false);

		// 첫번째 이미지 페이지 선택
		viewImageSrc.MoveToPage(0);
		viewImageDst.MoveToPage(0);

		viewImageSrc.GetLayer(1).SetLayerDrawingMethod(ELayerDrawingMethod_Manual);

		int32_t i32PageIndex = 0;
		CFLFigureArray flfaResultArrow;
		CPerformanceCounter performanceCounter;

		opticalFlowPolynomialExpansion.GetResultMotionVectorsArrowShapeAllScenes(flfaResultArrow);
		performanceCounter.Start();

		// 이미지 뷰에 Optical Flow 출력
		while(msgReceiver.IsViewImageAvailable())
		{
			if(fliSrcImage.GetPageCount() - 1 == fliSrcImage.GetSelectedPageIndex())
			{
				viewImageSrc.MoveToPage(0);
				viewImageDst.MoveToPage(0);
				i32PageIndex = 0;
				continue;
			}

			viewImageSrc.MoveToPage(i32PageIndex);
			viewImageDst.MoveToPage(i32PageIndex);
			viewImageDst.GetLayer(1).Clear();
			viewImageDst.GetLayer(1).DrawFigureImage(flfaResultArrow.GetAt(i32PageIndex), BLACK, 3);
			viewImageDst.GetLayer(1).DrawFigureImage(flfaResultArrow.GetAt(i32PageIndex), YELLOW, 1);

			viewImageSrc.GetLayer(1).Update();
			viewImageSrc.RedrawWindow();

			if(!msgReceiver.IsViewImageAvailable())
				break;

			while(performanceCounter.GetElapsedTimeFromStartInMilliSecond() <= 40.0)
				CThreadUtilities::Sleep(1);

			performanceCounter.Start();
			i32PageIndex++;
		}
	}
	while(false);

	return 0;
}