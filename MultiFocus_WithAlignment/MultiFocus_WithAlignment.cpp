#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

class CMessageReceiver : public CFLBase
{
public:

	// CMessageReceiver 생성자
	CMessageReceiver(CGUIViewImageWrap* pViewImage) : m_pViewImage(pViewImage)
	{
		// 메세지를 전달 받기 위해 CBroadcastManager 에 구독 등록 //Subscribe to CBroadcast Manager to receive messages
		CBroadcastManager::Subscribe(this);

		m_pFliFirstPageAlignment = nullptr;
		m_pFliLastPageAlignment = nullptr;
	}

	// CMessageReceiver 소멸자
	virtual ~CMessageReceiver()
	{
		// 객체가 소멸할때 메세지 수신을 중단하기 위해 구독을 해제한다. // Unsubscribe to stop receiving messages when the object disappears.
		CBroadcastManager::Unsubscribe(this);
	}

	DeclareGetClassType();

	// 메세지가 들어오면 호출되는 함수 OnReceiveBroadcast 오버라이드 하여 구현
	void OnReceiveBroadcast(const CBroadcastMessage* pMessage) override
	{
		do
		{
			// pMessage 가 nullptr 인지 확인 // Verify message is nullptr
			if(pMessage == nullptr)
				break;

			// GetCaller() 가 등록한 이미지뷰인지 확인 // Verify that GetCaller() is a registered image view
			if(pMessage->GetCaller() != (const CFLBase*)m_pViewImage->GetMessageCallerPtr())
				break;

			// 메세지의 채널을 확인
			switch(pMessage->GetChannel())
			{
			case EGUIBroadcast_ViewImage_PostPageChange:
				{
					CFLImage* pFliSrcImage = (CFLImage*)m_pViewImage->GetImagePtr();
					int64_t i64CurPage = pFliSrcImage->GetSelectedPageIndex();

					if(i64CurPage == 0)
					{
					    // 이미지뷰의 3번 레이어 가져오기 // Get layer 3rd of image view
						CGUIViewImageLayerWrap wrapImageLayer = m_pViewImage->GetLayer((int32_t)(3));
						wrapImageLayer.DrawFigureImage(m_pFliFirstPageAlignment, LIME, 1);

						CFLQuad<double>* pFlqAlignment = dynamic_cast<CFLQuad<double>*>(m_pFliFirstPageAlignment);

						if(!pFlqAlignment)
							break;

						wrapImageLayer.DrawTextImage(Base::TPoint<double>(pFlqAlignment->flpPoints[0].x, pFlqAlignment->flpPoints[0].y), L"First Page Alignment", CYAN, 1);
					}
					else if(i64CurPage == pFliSrcImage->GetPageCount() - 1)
					{
						// 이미지뷰의 4번 레이어 가져오기 // Get layer 4th of image view
						CGUIViewImageLayerWrap wrapImageLayer = m_pViewImage->GetLayer((int32_t)(4));
						wrapImageLayer.DrawFigureImage(m_pFliLastPageAlignment, LIME, 1);

						CFLQuad<double>* pFlqAlignment = dynamic_cast<CFLQuad<double>*>(m_pFliLastPageAlignment);
						
						if (!pFlqAlignment)
							break;

						wrapImageLayer.DrawTextImage(Base::TPoint<double>(pFlqAlignment->flpPoints[0].x, pFlqAlignment->flpPoints[0].y), L"Last Page Alignment", CYAN, 1);
					}

					// 이미지뷰를 갱신 // Update the image view.
					m_pViewImage->Invalidate();
				}
				break;
			}
		}
		while(false);
	}

	CFLFigure* m_pFliFirstPageAlignment;
	CFLFigure* m_pFliLastPageAlignment;

protected:
	CGUIViewImageWrap* m_pViewImage;
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

	// Message Reciever 객체 생성 // Create Message Reciever object
	CMessageReceiver msgReceiver(&viewImageSrc);

	CResult res = EResult_UnknownError;

	do
	{
		// Source 이미지 로드 // Load the source image
		if((res = fliSrcImage.Load(L"../../ExampleImages/MultiFocus/SourceAlignment.flif")).IsFail())
		{
			ErrorPrint(res, L"Failed to load the image file.\n");
			break;
		}

		CFLQuad<double> flqFirstPageAlignment;
		CFLQuad<double> flqLastPageAlignment;

		if((res = flqFirstPageAlignment.Load(L"../../ExampleImages/MultiFocus/FirstPageAlignment.fig")).IsFail())
		{
			ErrorPrint(res, L"Failed to load Source Projection Figure.");
			break;
		}

		if((res = flqLastPageAlignment.Load(L"../../ExampleImages/MultiFocus/LastPageAlignment.fig")).IsFail())
		{
			ErrorPrint(res, L"Failed to load Destination Projection Figure.");
			break;
		}
		
		// 메시지 리시버에 Figure Pointer 설정 // Set Figure Point to message receiver
		msgReceiver.m_pFliFirstPageAlignment = &flqFirstPageAlignment;
		msgReceiver.m_pFliLastPageAlignment = &flqLastPageAlignment;

		// 이미지 뷰 생성 // Create image view
		if((res = viewImageSrc.Create(100, 0, 600, 500)).IsFail() ||
		   (res = viewImageDst.Create(600, 0, 1100, 500)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views. 
		if(IsFail(res = viewImageSrc.SynchronizePointOfView(&viewImageDst)))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImageSrc.SynchronizeWindow(&viewImageDst)))
		{
			ErrorPrint(res, "Failed to synchronize window\n");
			break;
		}

		// 이미지 뷰에 이미지를 디스플레이 // Display the image in the image view
		if((res = viewImageSrc.SetImagePtr(&fliSrcImage)).IsFail() ||
		   (res = viewImageDst.SetImagePtr(&fliDstImage)).IsFail())
		{
			ErrorPrint(res, "Failed to set image object on the image view. \n");
			break;
		}

		viewImageSrc.SetFixThumbnailView(true);


		// 알고리즘 객체 생성 // Create algorithm object
		CMultiFocus multiFocus;

		if((res = multiFocus.SetSourceImage(fliSrcImage)).IsFail())
			break;
		if((res = multiFocus.SetDestinationImage(fliDstImage)).IsFail())
			break;
		if((res = multiFocus.SetKernel(23)).IsFail())
			break;
		if((res = multiFocus.SetFirstPageAlignment(flqFirstPageAlignment)).IsFail())
			break;
		if((res = multiFocus.SetLastPageAlignment(flqLastPageAlignment)).IsFail())
			break;

		// 알고리즘 수행 // Execute the algorithm
		if((res = multiFocus.Execute()).IsFail())
		{
			ErrorPrint(res, "Failed to execute the algorithm.");
			break;
		}


		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerSrc = viewImageSrc.GetLayer(0);
		CGUIViewImageLayerWrap layerDst = viewImageDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layerSrc.Clear();
		layerDst.Clear();

		layerSrc.SetAutoClearMode(ELayerAutoClearMode_PageChanged, false);

		// View 정보를 디스플레이 합니다. // Display View information.
		CFLPoint<double> flpPoint = new CFLPoint<double>(0, 0);
		if((res = (layerSrc.DrawTextCanvas(flpPoint, L"Source Image", YELLOW, BLACK, 30))).IsFail() ||
		   (res = layerDst.DrawTextCanvas(flpPoint, L"Destination Image", YELLOW, BLACK, 30)).IsFail())
		{
			ErrorPrint(res, "Failed to draw text. \n");
			break;
		}

		// Zoom Fit
		viewImageSrc.ZoomFit();
		viewImageDst.ZoomFit();

		// 이미지 뷰를 갱신 합니다. // Update the image view.
		viewImageSrc.Invalidate(true);
		viewImageDst.Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImageSrc.IsAvailable() && viewImageDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}