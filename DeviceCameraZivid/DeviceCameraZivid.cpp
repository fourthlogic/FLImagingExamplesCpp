#include <cstdio>

#include <FLImaging.h>


// 카메라에서 데이터 취득 이벤트를 받기 위해 CDeviceEventImageBase 를 상속 받아서 구현
// Implemented by inheriting CDeviceEventImageBase to receive data acquisition events from the camera
class CDeviceEventImageEx : public CDeviceEventImageBase
{
public:
	// CDeviceEventImageEx 생성자
	// Constructor for CDeviceEventImageEx
	CDeviceEventImageEx()
	{
		m_pView3D = nullptr;
	}

	// CDeviceEventImageEx 소멸자
	// Destructor for CDeviceEventImageEx
	virtual ~CDeviceEventImageEx()
	{
	}

	// 취득한 데이터를 표시할 3D 뷰를 설정하는 함수
	// Function to set the 3D view for displaying the acquired data
	void SetView3D(CGUIView3DWrap* pView3D)
	{
		if(pView3D)
			m_pView3D = pView3D;
	}

	// 카메라에서 데이터 취득 시 호출 되는 함수
	// Function called when data is acquired from the camera
	virtual void OnAcquisition(const CDeviceImageBase* pDeviceImage)
	{
		do
		{
			if(!m_pView3D)
				break;
			// 3D 뷰의 유효성을 확인한다.
			// Check whether the 3D view is valid
			if(!m_pView3D->IsAvailable())
				break;

			const CDeviceCameraZivid_2_17_1* pCamera = dynamic_cast<const CDeviceCameraZivid_2_17_1*>(pDeviceImage);

			if(!pCamera)
				break;

			// 데이터 객체 선언
			// Declare a data object
			CFL3DObject floData;

			// 카메라에서 취득 한 데이터를 얻어온다.
			// Retrieve the acquired 3D data from the camera
			pCamera->GetAcquired3DData(floData);

			// 3D 뷰의 업데이트를 막습니다.
			// Lock the 3D view to prevent updates
			m_pView3D->LockUpdate();

			// 3D 뷰의 유효성을 확인한다.
			// Check whether the 3D view is valid
			if(!m_pView3D->IsAvailable())
				break;

			// 3D 뷰의 객체 개수를 얻어옵니다.
			// Get the number of objects currently in the 3D view
			int32_t i32ObjectCount = m_pView3D->GetObjectCount();

			// 3D 뷰의 객체들을 모두 클리어합니다.
			// Clear all objects in the 3D view
			m_pView3D->ClearObjects();

			// 3D 뷰의 유효성을 확인한다.
			// Check whether the 3D view is valid
			if(!m_pView3D->IsAvailable())
				break;

			// 3D 뷰에 객체를 추가합니다.
			// Add the acquired 3D object to the view
			m_pView3D->PushObject(floData);

			// 3D 뷰의 유효성을 확인한다.
			// Check whether the 3D view is valid
			if(!m_pView3D->IsAvailable())
				break;

			// 3D 뷰의 업데이트 막은 것을 해제합니다.
			// Unlock the 3D view to allow updates again
			m_pView3D->UnlockUpdate();

			// 3D 뷰의 스케일을 조정합니다.
			// Adjust the 3D view scale if this is the first object
			if(!i32ObjectCount)
				m_pView3D->ZoomFit();
		}
		while(false);
	}

protected:
	CGUIView3DWrap* m_pView3D;
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CResult drReturn = EResult_UnknownError;

	// 3D 뷰 선언 // Declare the 3D view
	CGUIView3DWrap view3D;

	// Zivid 카메라 선언 // Declare the Zivid camera
	CDeviceCameraZivid_2_17_1 camZivid;

	do
	{
		wchar_t arrInput[4096] = { 0, };
		bool bAutoDetect = false;
		int32_t i32SelectDevice = 0;
		CFLString<wchar_t> flsConnection = L"";
		CFLString<wchar_t> flsCamfilePath = L"";
		CFLString<wchar_t> flsInput = L"";

		// Cam file 의 전체 경로를 입력합니다. // Enter the full path of the cam file.
		printf("Enter camfile full path (e.g. C:/Sample.yml): ");
		fgetws(arrInput, 4096, stdin);
		flsCamfilePath = arrInput;
		flsCamfilePath.Replace(L"\r", L"");
		flsCamfilePath.Replace(L"\n", L"");

		if((drReturn = camZivid.SetCamFilePath(flsCamfilePath)).IsFail())
			break;

		// 장치 찾기 방법을 선택합니다. // Select Detection Method
		while(true)
		{
			printf("1. Auto Detect\n");
			printf("2. Manual\n");
			printf("Select Detection Method: ");
			fgetws(arrInput, 4096, stdin);

			CFLString<wchar_t> flsInput(arrInput);
			int32_t i32Select = flsInput.ToInt32();
			bool bSelected = true;

			switch(i32Select)
			{
			case 1:
				bAutoDetect = true;
				break;

			case 2:
				bAutoDetect = false;
				break;

			default:
				bSelected = false;
				break;
			}

			if(bSelected)
				break;

			printf("Incorrect input. Please select again.\n\n");
		}

		printf("\n");

		if(bAutoDetect)
		{
			CFLArray<CFLString<wchar_t>> flarrSerialNumbers;

			// 연결되어 있는 카메라의 시리얼 번호를 얻는다. // Get serial numbers of connected cameras
			drReturn = camZivid.GetAutoDetectCameraSerialNumbers(flarrSerialNumbers);

			if(drReturn.IsFail() || !flarrSerialNumbers.GetCount())
			{
				drReturn = EResult_FailedToRead;
				printf("Not Found Device.\n");
				break;
			}

			// 연결 할 카메라를 선택합니다. // Select camera to be connected.
			while(true)
			{
				for(int64_t i = 0; i < flarrSerialNumbers.GetCount(); ++i)
				{
					CFLString<wchar_t> flsElement;
					flsElement.Format(L"%d. ", i + 1);

					flsElement += flarrSerialNumbers.GetAt(i);
					flsElement += L"\n";

					wprintf_s(flsElement);
				}

				printf("Select Device: ");
				fgetws(arrInput, 4096, stdin);

				CFLString<wchar_t> flsInput(arrInput);
				int32_t i32Select = flsInput.ToInt32() - 1;

				if(i32Select < 0 || i32Select >= flarrSerialNumbers.GetCount())
				{
					printf("Incorrect input. Please select again.\n\n");
					continue;
				}

				i32SelectDevice = i32Select;

				break;
			}
		}
		else
		{
			// 시리얼 번호를 입력 받는다. // Enter the serial number.
			printf("Input Serial Number: ");
			fgetws(arrInput, 4096, stdin);

			flsConnection = arrInput;
			flsConnection.Replace(L"\n", L"");
		}

		// 이벤트를 받을 객체 선언 // Declare the object that receives events
		CDeviceEventImageEx eventImage;

		// 카메라에 이벤트 객체 설정 // Set event object on Camera 
		camZivid.RegisterDeviceEvent(&eventImage);

		if(bAutoDetect)
			// 인덱스에 해당하는 카메라로 연결을 설정합니다. // Set the camera corresponding to the index to be connected.
			drReturn = camZivid.AutoDetectCamera(i32SelectDevice);
		else
			// 시리얼 번호를 설정합니다. // Set the serial number to camera
			camZivid.SetSerialNumber(flsConnection);

		// 카메라를 초기화 합니다. // Initialize the camera
		drReturn = camZivid.Initialize();

		if(drReturn.IsFail())
		{
			printf("Failed to initialize the camera.\n");
			break;
		}

		// 3D 뷰 생성 // Create 3D view
		if(view3D.Create(0, 0, 1000, 1000).IsFail())
		{
			drReturn = EResult_FailedToCreateObject;

			printf("Failed to create the image view.\n");
			break;
		}

		eventImage.SetView3D(&view3D);

		// 카메라를 Live 합니다. // Live the camera
		drReturn = camZivid.Live();

		if(drReturn.IsFail())
		{
			printf("Failed to live the camera.\n");
			break;
		}

		// 3D 뷰가 종료될 때 까지 기다림 // Wait for the 3D view to close.
		while(view3D.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	// 카메라의 초기화를 해제합니다.
	camZivid.Terminate();
	camZivid.ClearDeviceEvents();

	if(drReturn.IsFail())
		getchar();

	return 0;
}