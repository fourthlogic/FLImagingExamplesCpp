#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

// 프로파일 데이터 취득 이벤트를 받기 위해 CDeviceEventProfileBase 를 상속 받아서 구현
// Inherit and implement CDeviceEventProfileBase to receive profile data acquisition events
class CDeviceEventProfileEx : public CDeviceEventProfileBase
{
public:
	// CDeviceEventProfileEx 생성자 // CDeviceEventProfileEx Constructor
	CDeviceEventProfileEx()
	{
		m_pViewHeightImage = nullptr;
		m_pViewLuminanceImage = nullptr;

		// 높이 데이터 프로파일 이미지를 받을 객체 생성
		// Create an object to receive the height data profile image
		m_pHeightImage = new CFLImage;
		// 휘도 데이터 프로파일 이미지를 받을 객체 생성
		// Create an object to receive the luminance data profile image
		m_pLuminanceImage = new CFLImage;
	}

	// CDeviceEventProfileEx 소멸자 // CDeviceEventProfileEx destructor
	virtual ~CDeviceEventProfileEx()
	{
		// 생성한 높이 이미지 객체 해제 // Release the created height image object
		if(m_pHeightImage)
		{
			delete m_pHeightImage;
			m_pHeightImage = nullptr;
		}

		// 생성한 휘도 이미지 객체 해제 // Release the created luminance image object
		if(m_pLuminanceImage)
		{
			delete m_pLuminanceImage;
			m_pLuminanceImage = nullptr;
		}
	}

	// 취득한 높이 이미지를 표시할 이미지 뷰를 설정하는 함수
	// Function to set the image view to display the acquired height image
	void SetViewHeightImage(CGUIViewImageWrap* pViewImage)
	{
		if(pViewImage)
		{
			m_pViewHeightImage = pViewImage;

			// 이미지 뷰에 이미지 포인터 설정 // Set image pointer in image view
			m_pViewHeightImage->SetImagePtr(m_pHeightImage);
		}
	}

	// 취득한 휘도 이미지를 표시할 이미지 뷰를 설정하는 함수
	// Function to set the image view to display the acquired luminance image
	void SetViewLuminanceImage(CGUIViewImageWrap* pViewImage)
	{
		if(pViewImage)
		{
			m_pViewLuminanceImage = pViewImage;

			// 이미지 뷰에 이미지 포인터 설정 // Set image pointer in image view
			m_pViewLuminanceImage->SetImagePtr(m_pLuminanceImage);
		}
	}

	// 센서에서 프로파일 이미지 취득 시 호출 되는 함수 // Function called when acquiring a profile image from the sensor
	virtual void OnAcquisition(const CDeviceProfileBase* pDeviceProfile)
	{
		// 이미지 뷰의 유효성을 확인한다. // Check the validity of the image view.
		if(m_pViewHeightImage && m_pViewHeightImage->IsAvailable())
		{
			// 센서에서 취득 한 프로파일 이미지를 얻어온다. // Retrieves the profile image acquired from the sensor.
			m_pHeightImage->Lock();
			pDeviceProfile->GetAcquiredHeightProfile(m_pHeightImage);
			m_pHeightImage->Unlock();

			// 이미지 뷰를 재갱신 한다. // Invalidate the image view.
			m_pViewHeightImage->Invalidate();
		}

		// 이미지 뷰의 유효성을 확인한다. // Check the validity of the image view.
		if(m_pViewLuminanceImage && m_pViewLuminanceImage->IsAvailable())
		{
			// 센서에서 취득 한 프로파일 이미지를 얻어온다. // Retrieves the profile image acquired from the sensor.
			m_pLuminanceImage->Lock();
			pDeviceProfile->GetAcquiredLuminanceProfile(m_pLuminanceImage);
			m_pLuminanceImage->Unlock();

			// 이미지 뷰를 재갱신 한다. // Invalidate the image view.
			m_pViewLuminanceImage->Invalidate();
		}
	}

protected:
	CGUIViewImageWrap* m_pViewHeightImage;
	CGUIViewImageWrap* m_pViewLuminanceImage;
	CFLImage* m_pHeightImage;
	CFLImage* m_pLuminanceImage;
};


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CResult res;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewHeightImage;
	CGUIViewImageWrap viewLuminanceImage;

	// Laser Profile Sensor 선언 // Laser Profile Sensor Declaration
	CDeviceLaserProfileSensorKeyence devLaserProfile;

	// 이벤트를 받을 객체 선언 // Declare an object to receive events
	CDeviceEventProfileEx eventProfile;

	// 센서에 이벤트 객체 설정 // Set event object on sensor
	devLaserProfile.RegisterDeviceEvent(&eventProfile);

	do
	{
		wchar_t arrInput[4096] = { 0, };

		int32_t i32SelectDevice = -1;
		CFLString<wchar_t> flsConnection = L"";

		// IP 주소를 입력 받습니다. // Enter the IP address.
 		while(true)
 		{
 			printf("Input IP Address: ");
 
 			fgetws(arrInput, 4096, stdin);
 
 			flsConnection = arrInput;
 			flsConnection.Replace(L"\n", L"");
 
 			CFLString<char> flsIPAddress(flsConnection);
 
 			// 입력 받은 IP 주소를 설정합니다. // Set the entered IP address.
 			if((res = devLaserProfile.SetConnectionIPAddress(flsIPAddress)).IsOK())
 				break;
 
 			ErrorPrint(res, "Failed to set IP Address.\n");
 		}
 
 		// 포트 번호를 입력 받습니다. // Enter the port number.
 		while(true)
 		{
 			printf("Input Port Number: ");
 
 			fgetws(arrInput, 4096, stdin);
 
 			flsConnection = arrInput;
 			flsConnection.Replace(L"\n", L"");
 
 			// 입력 받은 포트 번호를 설정합니다. // Set the entered port number.
 			if((res = devLaserProfile.SetPortNumber((uint16_t)flsConnection.ToUint32())).IsOK())
 				break;
 
 			ErrorPrint(res, "Failed to set port number.\n");
 		}
 
 		// 고속 통신용 포트 번호를 입력 받습니다. // Enter the port number for high-speed communication.
 		while(true)
 		{
 			printf("Input High-speed Port Number: ");
 
 			fgetws(arrInput, 4096, stdin);
 
 			flsConnection = arrInput;
 			flsConnection.Replace(L"\n", L"");
 
 			// 입력 받은 고속 통신용 포트 번호를 설정합니다. // Set the input port number for high-speed communication.
 			if((res = devLaserProfile.SetHighSpeedPort((uint16_t)flsConnection.ToUint32())).IsOK())
 				break;
 
 			ErrorPrint(res, "Failed to set high-speed port number.\n");
 		}

		// Profile 수를 입력 받습니다. // Enter the profile number.
		while(true)
		{
			printf("Input Profile Count: ");

			fgetws(arrInput, 4096, stdin);

			flsConnection = arrInput;
			flsConnection.Replace(L"\n", L"");

			// 입력 받은 Profile 수를 설정합니다. // Set the number of profiles entered.
			if((res = devLaserProfile.SetProfileCount(flsConnection.ToInt32())).IsOK())
				break;

			ErrorPrint(res, "Failed to set profile count.\n");
		}

		// 프로파일 센서를 초기화 합니다. // Initialize the profile sensor.
		if((res = devLaserProfile.Initialize()).IsFail())
		{
			ErrorPrint(res, "Failed to initialize device.\n");
			break;
		}

		// 높이 이미지 뷰 생성 // Create height image view
		if((res = viewHeightImage.Create(400, 0, 812, 384)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 휘도 이미지 뷰 생성 // Create luminance image view
		if((res = viewLuminanceImage.Create(812, 0, 1224, 384)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 각 이미지 뷰의 시점을 동기화 한다. // Synchronize the viewpoint of each image view.
		if((res = viewHeightImage.SynchronizePointOfView(&viewLuminanceImage)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}
		// 각 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the position of each image view window
		if((res = viewHeightImage.SynchronizeWindow(&viewLuminanceImage)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 이벤트 객체에 View를 설정합니다. // Set a View on the event object.
		eventProfile.SetViewHeightImage(&viewHeightImage);
		eventProfile.SetViewLuminanceImage(&viewLuminanceImage);

		// 활성 프로그램 No. 를 전환합니다. // Change active program number.
		devLaserProfile.SetProgramNumber(0);
		// 설정 함수의 적용 범위를 설정합니다. // Set the setting depth of the setting function.
		devLaserProfile.SetSettingDepth(CDeviceLaserProfileSensorKeyence::ESettingDepth_Running);


		// 파라미터 설정 함수 - Keyence 사의 LJ X Navigator 설치 후 C:\Program Files\KEYENCE\LJ-X Navigator\lib\Manual 경로의 매뉴얼 11.3 참고
		// Parameter setting function - Refer to section 11.3 of the manual located at C:\Program Files\KEYENCE\LJ-X Navigator\lib\Manual after installing Keyence's LJ-X Navigator.

		// Trigger 모드 설정(0 : 연속 트리거, 1 : 외부 트리거, 2 : 인코더 트리거)
		// Trigger mode setting (0: continuous trigger, 1: external trigger, 2: encoder trigger)
		{
			int64_t i64DataSize = 4;
			std::vector<uint8_t> vctData(i64DataSize, 0);

			vctData[0] = 0;

			devLaserProfile.SetTriggerSetting(CDeviceLaserProfileSensorKeyence::ETriggerSettingItem_TriggerMode, vctData.data(), i64DataSize);
		}

		// 배치 측정 설정(0 : 배치 OFF, 1 : 배치 ON)
		// Batch measurement settings (0: batch OFF, 1: batch ON)
		{
			int64_t i64DataSize = 4;
			std::vector<uint8_t> vctData(i64DataSize, 0);

			vctData[0] = 0;

			devLaserProfile.SetTriggerSetting(CDeviceLaserProfileSensorKeyence::ETriggerSettingItem_BatchMeasurement, vctData.data(), i64DataSize);
		}

		// 휘도 출력 설정(0 : 높이 데이터만, 1 : 높이 + 휘도 데이터)
		// Luminance output settings (0: height data only, 1: height + luminance data)
		{
			int64_t i64DataSize = 4;
			std::vector<uint8_t> vctData(i64DataSize, 0);

			vctData[0] = 1;

			devLaserProfile.SetCommonSetting(CDeviceLaserProfileSensorKeyence::ECommonSettingItem_LuminanceOutput, vctData.data(), i64DataSize);
		}

		// 프로파일 센서를 Start 합니다. // Start the profile sensor.
		if((res = devLaserProfile.Start()).IsFail())
		{
			ErrorPrint(res, "Failed to start the profile sensor.\n");
			break;
		}

		
		Sleep(100);
		viewHeightImage.ZoomFit();

		// 화면에 출력하기 위해 Image View 에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layerHeight;
		CGUIViewImageLayerWrap layerLuminance;

		// 각각의 image View 에서 0번 레이어 가져오기 // Get Layer 0 from each image view 
		layerHeight = viewHeightImage.GetLayer(0);
		layerLuminance = viewLuminanceImage.GetLayer(0);

		// 각 레이어 캔버스에 텍스트 그리기 // Draw text to each Layer Canvas
		layerHeight.DrawTextCanvas(&CFLPoint<int32_t>(0, 0), L"Height", YELLOW, BLACK, 30);
		layerLuminance.DrawTextCanvas(&CFLPoint<int32_t>(0, 0), L"Luminance", YELLOW, BLACK, 30);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewHeightImage.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	// 프로파일 센서의 초기화를 해제합니다. // Uninitialize the profile sensor.
	devLaserProfile.Terminate();
	devLaserProfile.ClearDeviceEvents();

	if(res.IsFail())
		getchar();

	return 0;
}