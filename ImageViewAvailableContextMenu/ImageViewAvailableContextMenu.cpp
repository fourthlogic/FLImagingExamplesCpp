#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage[2];

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

		// 뷰의 시점을 동기화 한다. // Synchronizes the view point.
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
		
		// 이미지 뷰의 모든 컨텍스트 메뉴 사용 여부를 설정합니다. 
		// true로 설정하면 모든 메뉴가 활성화되고, false로 설정하면 모든 메뉴가 비활성화됩니다.
		// Sets whether to enable all context menus in the image view.
		// Setting it to true enables all context menu items, and setting it to false disables all context menu items.
		viewImage[0].EnableAvailableViewImageContextMenuAll(false);

		// 이미지뷰의 0번 레이어 가져오기 // Retrieves layer 0 from the image view.
		CGUIViewImageLayerWrap layer = viewImage[0].GetLayer(0);

		// 기존에 Layer 에 그려진 도형들을 삭제 // Clear all figures previously drawn on the layer.
		layer.Clear();

		// 안내 문자열 생성 // Creates a guidance message.
		CFLString<wchar_t> flsInformation = L"RIGHT BUTTON CLICK ON MOUSE AND SEE THE CONTEXT MENU";
		CFLString<wchar_t> flsInformation2 = L"Option : None";

		// 아래 함수 DrawTextCanvas는 스크린 좌표를 기준으로 문자열을 뷰어에 출력한다.
		// The function DrawTextCanvas displays a string on the viewer using screen coordinates.
		// 파라미터 순서 : 기준 좌표 Figure 객체 -> 문자열 -> 텍스트 색 -> 텍스트 테두리 색 -> 폰트 크기 -> 실제 크기로 출력 유무 -> 각도 -> 정렬 -> 폰트 이름 -> 텍스트 알파값(불투명도) -> 텍스트 테두리 알파값 (불투명도) -> 폰트 두께 -> 폰트 이탤릭 여부
		// Parameter order: reference coordinate (Figure object) -> text string -> text color -> text outline color -> font size -> render in real-world size (bool) -> angle -> alignment -> font name -> text alpha (opacity) -> text outline alpha (opacity) -> font thickness -> italic font (bool)
		layer.DrawTextCanvas(CFLPoint<double>(10, 10), flsInformation, LIME, BLACK, 15);
		layer.DrawTextCanvas(CFLPoint<double>(10, 30), flsInformation2, CYAN, BLACK, 15);


		// 이미지 뷰에서 이용 가능한 컨텍스트 메뉴를 설정합니다.
		// 기본값은 모든 메뉴가 사용 가능한 상태이며, EnableAvailableViewImageContextMenuAll(true) 로 언제든 전체 활성화할 수 있습니다.
		// 아래와 같이 EMenuItem 배열을 만들어 RemoveAvailableViewImageContextMenu 를 호출하면
		// 전달한 항목들만 비활성화됩니다.
		// 아래 예제에서는 파일 열기, 닫기, 저장, 이미지 생성 관련 메뉴가 비활성화됩니다.
		// Sets the context menus available in the image view.
		// By default all context menu items are enabled; call EnableAvailableViewImageContextMenuAll(true) to enable them all at any time.
		// As shown below, build an EMenuItem array and pass it to RemoveAvailableViewImageContextMenu
		// to disable only the items you list.
		// The example below disables the Open File, Close File, Save, and Create Image menu items.
		EMenuItem arrRemoveMenu[] =
		{
			// Load
			EMenuItem_MenuGroup_Load,

			// ClearFile
			EMenuItem_ClearFile,
			EMenuItem_ClearSelectedPage,

			// Save
			EMenuItem_MenuGroup_Save,

			// CreateImage
			EMenuItem_CreateImage,
			EMenuItem_InsertPage,
			EMenuItem_AppendPage,
		};

		viewImage[1].EnableAvailableViewImageContextMenuAll(true); // 전체 메뉴 활성화 / Enable all menu items
		viewImage[1].RemoveAvailableViewImageContextMenu(arrRemoveMenu, sizeof(arrRemoveMenu) / sizeof(arrRemoveMenu[0]));

		// 이미지뷰의 0번 레이어 가져오기 // Retrieves layer 0 from the image view.
		layer = viewImage[1].GetLayer(0);

		// 기존에 Layer 에 그려진 도형들을 삭제 // Clear all figures previously drawn on the layer.
		layer.Clear();

		// 안내 문자열 지정 // Sets the guidance message.
		flsInformation = L"RIGHT BUTTON CLICK ON MOUSE AND SEE THE CONTEXT MENU";
		flsInformation2 = L"Option: RemoveAvailableViewImageContextMenu\n           (LoadFile, LoadFile_Raw, LoadFolder\n           ClearFile, Save, CreateImage, ...)";

		// 아래 함수 DrawTextCanvas는 스크린 좌표를 기준으로 문자열을 뷰어에 출력한다.
		// The function DrawTextCanvas displays a string on the viewer using screen coordinates.
		// 파라미터 순서 : 기준 좌표 Figure 객체 -> 문자열 -> 텍스트 색 -> 텍스트 테두리 색 -> 폰트 크기 -> 실제 크기로 출력 유무 -> 각도 -> 정렬 -> 폰트 이름 -> 텍스트 알파값(불투명도) -> 텍스트 테두리 알파값 (불투명도) -> 폰트 두께 -> 폰트 이탤릭 여부
		// Parameter order: reference coordinate (Figure object) -> text string -> text color -> text outline color -> font size -> render in real-world size (bool) -> angle -> alignment -> font name -> text alpha (opacity) -> text outline alpha (opacity) -> font thickness -> italic font (bool)
		layer.DrawTextCanvas(CFLPoint<double>(10, 10), flsInformation, LIME, BLACK, 15);
		layer.DrawTextCanvas(CFLPoint<double>(10, 30), flsInformation2, CYAN, BLACK, 15);

		for(int32_t i = 0; i < 2; ++i)
			viewImage[i].Invalidate();

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage[0].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}