#include <cstdio>

#include <FLImaging.h>


// 카메라에서 이미지 취득 이벤트를 받기 위해 CDeviceEventImageBase 를 상속 받아서 구현
// Inherit from CDeviceEventImageBase to receive image acquisition events from the camera.
class CDeviceEventImageEx : public CDeviceEventImageBase
{
public:
	// CDeviceEventImageEx 생성자 // Constructor of CDeviceEventImageEx
	CDeviceEventImageEx()
	{
		m_pViewImage = nullptr;
		// 이미지를 받을 객체 생성 // Create an object to receive images.
		m_pImage = new CFLImage;
	}

	// CDeviceEventImageEx 소멸자 // Destructor of CDeviceEventImageEx
	virtual ~CDeviceEventImageEx()
	{
		// 생성한 이미지 객체 해제 // Delete the created image object.
		if(m_pImage)
		{
			delete m_pImage;
			m_pImage = nullptr;
		}
	}

	// 취득한 이미지를 표시할 이미지 뷰를 설정하는 함수 // Sets the image view to display acquired images.
	void SetViewImage(CGUIViewImageWrap* pViewImage)
	{
		if(pViewImage)
		{
			m_pViewImage = pViewImage;

			// 이미지 뷰에 이미지 포인터 설정 // Set the image pointer for the image view.
			m_pViewImage->SetImagePtr(m_pImage);
		}
	}

	// 카메라에서 이미지 취득 시 호출 되는 함수 // Callback function called when an image is acquired from the camera.
	virtual void OnAcquisition(const CDeviceImageBase* pDeviceImage)
	{
		// 이미지 뷰의 유효성을 확인한다. // Check the validity of the image view.
		if(m_pViewImage && m_pViewImage->IsAvailable())
		{
			// 카메라에서 취득 한 이미지를 얻어온다. // Get the image acquired from the camera.
			m_pImage->Lock();
			pDeviceImage->GetAcquiredImage(m_pImage);
			m_pImage->Unlock();

			// 이미지 뷰를 재갱신 한다. // Invalidate the image view.
			m_pViewImage->Invalidate();
		}
	}

protected:
	CGUIViewImageWrap* m_pViewImage;
	CFLImage* m_pImage;
};


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CResult drReturn = EResult_UnknownError;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage;

	// ProntoCam 카메라 선언 // Declare ProntoCam camera.
	CDeviceCameraProntoCam camProntoCam;

	// 이벤트를 받을 객체 선언 // Declare an event object.
	CDeviceEventImageEx eventImage;

	// 카메라에 이벤트 객체 설정 // Set the event object for the camera.
	camProntoCam.RegisterDeviceEvent(&eventImage);

	do
	{
		wchar_t arrInput[4096] = { 0, };

		bool bAutoDetect = false;
		int32_t i32SelectDevice = -1;
		CFLString<wchar_t> flsConnection = L"";

		// 장치 찾기 방법을 선택합니다. // Select the detection method
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

			// 연결되어 있는 카메라의 시리얼 번호를 얻는다. // Get the serial number of the connected camera.
			drReturn = camProntoCam.GetAutoDetectCameraSerialNumbers(flarrSerialNumbers);
			
			if(drReturn.IsFail() || !flarrSerialNumbers.GetCount())
			{
				drReturn = EResult_FailedToRead;
				printf("Not Found Device.\n");
				break;
			}

			// 연결 할 카메라를 선택합니다. // Select the camera to connect to.
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
			// 시리얼 번호를 입력 받습니다. // Enter the serial number.
			printf("Input Serial Number: ");

			fgetws(arrInput, 4096, stdin);

			flsConnection = arrInput;
			flsConnection.Replace(L"\n", L"");
		}

		if(bAutoDetect)
		{
			// 인덱스에 해당하는 카메라로 연결을 설정합니다. // Set the camera specified by the index for connection.
			drReturn = camProntoCam.AutoDetectCamera(i32SelectDevice);
		}
		else
		{
			// 시리얼 번호를 설정합니다. // Set the serial number.
			camProntoCam.SetSerialNumber(flsConnection);
		}

		// 카메라를 초기화 합니다. // Initialize the camera.
		drReturn = camProntoCam.Initialize();

		if(drReturn.IsFail())
		{
			printf("Failed to initialize the camera.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(viewImage.Create(0, 0, 1000, 1000).IsFail())
		{
			drReturn = EResult_FailedToCreateObject;

			printf("Failed to create the image view.\n");
			break;
		}

		eventImage.SetViewImage(&viewImage);

		// 카메라를 Live 합니다. // Start live acquisition.
		drReturn = camProntoCam.Live();

		if(drReturn.IsFail())
		{
			printf("Failed to live the camera.\n");
			break;
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	// 카메라의 초기화를 해제합니다. // Terminate the camera.
	camProntoCam.Terminate();
	camProntoCam.ClearDeviceEvents();

	if(drReturn.IsFail())
		getchar();

	return 0;
}