#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


class CMessageReceiver : public CFLBase
{
public:

	// CMessageReceiver 생성자 // CMessageReceiver constructor.
	CMessageReceiver(CGUIViewImageWrap* pViewImage) : m_pViewImage(pViewImage)
	{
		// 메세지를 전달 받기 위해 CBroadcastManager 에 구독 등록 // Subscribe to CBroadcastManager to receive messages.
		CBroadcastManager::Subscribe(this);
	}

	// CMessageReceiver 소멸자 // CMessageReceiver destructor.
	virtual ~CMessageReceiver()
	{
		// 객체가 소멸할때 메세지 수신을 중단하기 위해 구독을 해제한다. // Unsubscribe to stop receiving messages when the object is destroyed.
		CBroadcastManager::Unsubscribe(this);
	}

	DeclareGetClassType();

	// 메세지가 들어오면 호출되는 함수 OnReceiveBroadcast 오버라이드 하여 구현 // Override OnReceiveBroadcast, which is called when a message is received.
	void OnReceiveBroadcast(const CBroadcastMessage* pMessage) override
	{
		do
		{
			// pMessage 가 nullptr 인지 확인 // Check whether pMessage is nullptr.
			if(pMessage == nullptr)
				break;

			// 메세지의 채널을 확인 // Check the message channel.
			switch(pMessage->GetChannel())
			{
			case EGUIBroadcast_ViewImage_PostMouseMove:
				{
					// GetCaller() 가 등록한 이미지뷰인지 확인 // Check whether GetCaller() is a registered image view.
					if(pMessage->GetCaller() != (const CFLBase*)m_pViewImage->GetMessageCallerPtr())
						break;

					// pMessage 객체를 CBroadcastMessage_GUI_ViewImage_MouseEvent 로 캐스팅 // Cast pMessage to CBroadcastMessage_GUI_ViewImage_MouseEvent.
					CBroadcastMessage_GUI_ViewImage_MouseEvent* pMsgMouseEvent = dynamic_cast<CBroadcastMessage_GUI_ViewImage_MouseEvent*>((CBroadcastMessage*)pMessage);

					// pMsgMouseEvent 가 nullptr 인지 확인 // Check whether pMsgMouseEvent is nullptr.
					if(pMsgMouseEvent == nullptr)
						break;

					// 이미지뷰의 0번 레이어 가져오기 // Get layer 0 of the image view.
					CGUIViewImageLayerWrap layer = m_pViewImage->GetLayer(0);

					// 기존에 Layer 에 그려진 도형들을 삭제 // Clear all figures from the layer.
					layer.Clear();

					// 마우스 좌표를 표시할 문자열 생성 // Create a string to display the mouse coordinates.
					CFLString<wchar_t> flsPosition = L"";
					flsPosition.Format(L"Move X: %d, Y: %d", pMsgMouseEvent->GetCursor()->x, pMsgMouseEvent->GetCursor()->y);

					// 아래 함수 DrawTextCanvas는 스크린 좌표를 기준으로 문자열을 뷰어에 출력한다.
					// The function DrawTextCanvas displays a string on the viewer using screen coordinates.
					// 파라미터 순서 : 기준 좌표 Figure 객체 -> 문자열 -> 텍스트 색 -> 텍스트 테두리 색 -> 폰트 크기 -> 실제 크기로 출력 유무 -> 각도 -> 정렬 -> 폰트 이름 -> 텍스트 알파값(불투명도) -> 텍스트 테두리 알파값 (불투명도) -> 폰트 두께 -> 폰트 이탤릭 여부
					// Parameter order: reference coordinate (Figure object) -> text string -> text color -> text outline color -> font size -> render in real-world size (bool) -> angle -> alignment -> font name -> text alpha (opacity) -> text outline alpha (opacity) -> font thickness -> italic font (bool)
					layer.DrawTextCanvas(CFLPoint<double>(10, 10), flsPosition, LIME, BLACK);

					// 이미지뷰를 갱신 // Invalidate the image view.
					m_pViewImage->Invalidate();
				}
				break;

			case EGUIBroadcast_ViewImage_PostLButtonDown:
				{
					// GetCaller() 가 등록한 이미지뷰인지 확인 // Check whether GetCaller() is a registered image view.
					if(pMessage->GetCaller() != (const CFLBase*)m_pViewImage->GetMessageCallerPtr())
						break;

					// pMessage 객체를 CBroadcastMessage_GUI_ViewImage_MouseEvent 로 캐스팅 // Cast pMessage to CBroadcastMessage_GUI_ViewImage_MouseEvent.
					CBroadcastMessage_GUI_ViewImage_MouseEvent* pMsgMouseEvent = dynamic_cast<CBroadcastMessage_GUI_ViewImage_MouseEvent*>((CBroadcastMessage*)pMessage);

					// pMsgMouseEvent 가 nullptr 인지 확인 // Check whether pMsgMouseEvent is nullptr.
					if(pMsgMouseEvent == nullptr)
						break;

					// 이미지뷰의 0번 레이어 가져오기 // Get layer 0 of the image view.
					CGUIViewImageLayerWrap layer = m_pViewImage->GetLayer(0);

					// 기존에 Layer 에 그려진 도형들을 삭제 // Clear all figures from the layer.
					layer.Clear();

					// 마우스 좌표를 표시할 문자열 생성 // Create a string to display the mouse coordinates.
					CFLString<wchar_t> flsPosition = L"";
					flsPosition.Format(L"LButtonDown X: %d, Y: %d", pMsgMouseEvent->GetCursor()->x, pMsgMouseEvent->GetCursor()->y);

					// 아래 함수 DrawTextCanvas는 스크린 좌표를 기준으로 문자열을 뷰어에 출력한다.
					// The function DrawTextCanvas displays a string on the viewer using screen coordinates.
					// 파라미터 순서 : 기준 좌표 Figure 객체 -> 문자열 -> 텍스트 색 -> 텍스트 테두리 색 -> 폰트 크기 -> 실제 크기로 출력 유무 -> 각도 -> 정렬 -> 폰트 이름 -> 텍스트 알파값(불투명도) -> 텍스트 테두리 알파값 (불투명도) -> 폰트 두께 -> 폰트 이탤릭 여부
					// Parameter order: reference coordinate (Figure object) -> text string -> text color -> text outline color -> font size -> render in real-world size (bool) -> angle -> alignment -> font name -> text alpha (opacity) -> text outline alpha (opacity) -> font thickness -> italic font (bool)
					layer.DrawTextCanvas(CFLPoint<double>(10, 10), flsPosition, RED, BLACK);

					// 이미지뷰를 갱신 // Invalidate the image view.
					m_pViewImage->Invalidate();
				}
				break;

			case EGUIBroadcast_ViewImage_PostLButtonUp:
				{
					// GetCaller() 가 등록한 이미지뷰인지 확인 // Check whether GetCaller() is a registered image view.
					if(pMessage->GetCaller() != (const CFLBase*)m_pViewImage->GetMessageCallerPtr())
						break;

					// pMessage 객체를 CBroadcastMessage_GUI_ViewImage_MouseEvent 로 캐스팅 // Cast pMessage to CBroadcastMessage_GUI_ViewImage_MouseEvent.
					CBroadcastMessage_GUI_ViewImage_MouseEvent* pMsgMouseEvent = dynamic_cast<CBroadcastMessage_GUI_ViewImage_MouseEvent*>((CBroadcastMessage*)pMessage);

					// pMsgMouseEvent 가 nullptr 인지 확인 // Check whether pMsgMouseEvent is nullptr.
					if(pMsgMouseEvent == nullptr)
						break;

					// 이미지뷰의 0번 레이어 가져오기 // Get layer 0 of the image view.
					CGUIViewImageLayerWrap layer = m_pViewImage->GetLayer(0);

					// 기존에 Layer 에 그려진 도형들을 삭제 // Clear all figures from the layer.
					layer.Clear();

					// 마우스 좌표를 표시할 문자열 생성 // Create a string to display the mouse coordinates.
					CFLString<wchar_t> flsPosition = L"";
					flsPosition.Format(L"LButtonUp X: %d, Y: %d", pMsgMouseEvent->GetCursor()->x, pMsgMouseEvent->GetCursor()->y);

					// 아래 함수 DrawTextCanvas는 스크린 좌표를 기준으로 문자열을 뷰어에 출력한다.
					// The function DrawTextCanvas displays a string on the viewer using screen coordinates.
					// 파라미터 순서 : 기준 좌표 Figure 객체 -> 문자열 -> 텍스트 색 -> 텍스트 테두리 색 -> 폰트 크기 -> 실제 크기로 출력 유무 -> 각도 -> 정렬 -> 폰트 이름 -> 텍스트 알파값(불투명도) -> 텍스트 테두리 알파값 (불투명도) -> 폰트 두께 -> 폰트 이탤릭 여부
					// Parameter order: reference coordinate (Figure object) -> text string -> text color -> text outline color -> font size -> render in real-world size (bool) -> angle -> alignment -> font name -> text alpha (opacity) -> text outline alpha (opacity) -> font thickness -> italic font (bool)
					layer.DrawTextCanvas(CFLPoint<double>(10, 10), flsPosition, BLUE, BLACK);

					// 이미지뷰를 갱신 // Invalidate the image view.
					m_pViewImage->Invalidate();
				}
				break;
			}
		}
		while(false);
	}

	protected:
		CGUIViewImageWrap* m_pViewImage;
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage[2];

	// 메세지를 전달 받을 CMessageReceiver 객체 생성 // Create a CMessageReceiver object to receive messages.
	CMessageReceiver msgReceiver(&viewImage[0]);

	CResult res = EResult_UnknownError;

	do
	{
		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage[0].Create(300, 0, 300 + 520, 430)))
		{
			ErrorPrint(res,"Failed to create the image view.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage[1].Create(300 + 520, 0, 300 + 520 * 2, 430)))
		{
			ErrorPrint(res,"Failed to create the image view.\n");
			break;
		}

		// 뷰의 시점을 동기화 한다 // Synchronize the view point.
		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[1])))
		{
			ErrorPrint(res,"Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[1])))
		{
			ErrorPrint(res,"Failed to synchronize window\n");
			break;
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage[0].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}