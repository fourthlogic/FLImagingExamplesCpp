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
		m_pViewImage = nullptr;
		m_pView3D = nullptr;

		// 이미지를 받을 객체 생성
		// Create an object to receive the image
		m_pFliImage = new CFLImage;
	}

	// CDeviceEventProfileEx 소멸자 // CDeviceEventProfileEx destructor
	virtual ~CDeviceEventProfileEx()
	{
		// 생성한 이미지 객체 해제 // Release the created image object
		if(m_pFliImage)
		{
			delete m_pFliImage;
			m_pFliImage = nullptr;
		}
	}

	// 취득한 이미지를 표시할 이미지 뷰를 설정하는 함수
	// Function to set the image view to display the acquired image
	void SetViewImage(CGUIViewImageWrap* pViewImage)
	{
		if(pViewImage)
		{
			m_pViewImage = pViewImage;

			// 이미지 뷰에 이미지 포인터 설정 // Set image pointer in image view
			m_pViewImage->SetImagePtr(m_pFliImage);
		}
	}

	// 취득한 3D 데이터를 표시할 3D 뷰를 설정하는 함수
	// Function to set the 3D view to display the acquired 3D data
	void SetView3D(CGUIView3DWrap* pView3D)
	{
		if(pView3D)
			m_pView3D = pView3D;
	}

	// 센서에서 프로파일 데이터 취득 시 호출 되는 함수 // Function called when acquiring a profile data from the sensor
	virtual void OnAcquisition(const CDeviceProfileBase* pDeviceProfile)
	{
		do
		{
			const CDeviceLaserProfileSensorLMI* pDeviceLMI = dynamic_cast<const CDeviceLaserProfileSensorLMI*>(pDeviceProfile);

			if(!pDeviceLMI)
				break;

			if(!m_pViewImage)
				break;

			if(!m_pView3D)
				break;

			// 스캔 모드를 얻어옵니다. // Get the scan mode.
			CDeviceLaserProfileSensorLMI::EScanMode eScanMode = CDeviceLaserProfileSensorLMI::EScanMode_Image;

			pDeviceLMI->GetScanMode(eScanMode);

			switch(eScanMode)
			{
			case FLImaging::Devices::CDeviceLaserProfileSensorLMI::EScanMode_Image:
				{
					// 이미지 뷰의 유효성을 확인합니다. // Validate the image view.
					if(!m_pViewImage->IsAvailable())
						break;

					if(!m_pFliImage)
						break;

					// 기존 이미지 정보를 얻어옵니다. // Retrieve the existing image information.
					int64_t i64Width = m_pFliImage->GetWidth();
					int64_t i64Height = m_pFliImage->GetHeight();
					int32_t i32SelectedPageIndex = m_pFliImage->GetSelectedPageIndex();
					bool bZoomFit = false;

					m_pFliImage->Lock();

					// 취득 한 이미지를 얻어온다. //Retrieve the acquired image.
					pDeviceLMI->GetAcquiredImage(m_pFliImage);

					// 기존 선택된 페이지 인덱스로 선택합니다. // Select the page using the existing selected page index.
					m_pFliImage->SelectPage(i32SelectedPageIndex);

					// 기존 이미지 정보와 비교합니다. // Compare with the existing image information.
					if(i64Width != m_pFliImage->GetWidth() || i64Height != m_pFliImage->GetHeight())
						bZoomFit = true;

					m_pFliImage->Unlock();

					// 뷰를 Zoom fit 합니다. // Fit the view to the window.
					if(bZoomFit)
						m_pViewImage->ZoomFit();

					// 이미지 뷰를 재갱신 한다. // Invalidate the image view.
					m_pViewImage->Invalidate();
				}
				break;

			case FLImaging::Devices::CDeviceLaserProfileSensorLMI::EScanMode_Profile:
				{
					// 프로파일 데이터를 얻어올 배열을 선언합니다. // Declare an array to store profile data.
					CFLArray<CFLArray<CFLArray<double>>> flaProfile;

					// 취득한 프로파일 데이터를 얻어온다. // Retrieve the acquired profile data.
					pDeviceLMI->GetAcquiredProfile(flaProfile);

					// 프로파일 데이터를 표시합니다. // Display the profile data.
					CFLString<wchar_t> flsProfile;

					for(int64_t i = 0; i < flaProfile.GetCount(); ++i)
					{
						if(i != 0)
							flsProfile.Append(L"\n");

						flsProfile.Append(L"[");

						for(int64_t j = 0; j < flaProfile[i].GetCount(); ++j)
						{
							if(j != 0)
								flsProfile.Append(L"\n");

							flsProfile.Append(L"{");

							for(int64_t k = 0; k < flaProfile[i][j].GetCount(); ++k)
							{
								if(k != 0)
									flsProfile.Append(L", ");

								flsProfile.AppendFormat(L"(%0.3lf,%0.3lf,%0.3lf)", flaProfile[i][j][CDeviceLaserProfileSensorLMI::EProfileDataElement_PositionX], flaProfile[i][j][CDeviceLaserProfileSensorLMI::EProfileDataElement_PositionZ], flaProfile[i][j][CDeviceLaserProfileSensorLMI::EProfileDataElement_Intensity]);
							}

							flsProfile.Append(L"}");
						}

						flsProfile.Append(L"]");
					}

					wprintf(flsProfile);
				}
				break;

			case FLImaging::Devices::CDeviceLaserProfileSensorLMI::EScanMode_Surface:
				{
					// 3D 뷰의 유효성을 확인합니다. // Validate the 3D view.
					if(!m_pView3D->IsAvailable())
						break;

					// 3D 데이터를 얻어올 객체를 선언합니다. // Declare an object to store 3D data.
					CFL3DObjectGroup flogData;

					// 취득한 3D 데이터를 얻어온다. // Retrieve the acquired 3D data.
					pDeviceLMI->GetAcquired3DData(flogData);

					// 3D 뷰의 유효성을 확인합니다. // Validate the 3D view.
					if(!m_pView3D->IsAvailable())
						break;

					// 3D 데이터를 3D 뷰에 표시합니다. // Display the 3D data in the 3D view.
					m_pView3D->LockUpdate();

					int32_t i32ObjectCount = m_pView3D->GetObjectCount();

					m_pView3D->ClearObjects();

					for(int64_t i = 0; i < flogData.GetObjectCount(); ++i)
						m_pView3D->PushObject(*flogData.GetObjectByIndex(i));

					m_pView3D->UnlockUpdate();

					if(i32ObjectCount == 0)
						m_pView3D->ZoomFit();
				}
				break;
			}
		}
		while(false);
	}

protected:
	CGUIViewImageWrap* m_pViewImage;
	CGUIView3DWrap* m_pView3D;
	CFLImage* m_pFliImage;
};


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CResult res;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage;

	// 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap view3D;

	// Laser Profile Sensor 선언 // Laser Profile Sensor Declaration
	CDeviceLaserProfileSensorLMI devLaserProfile;

	// 이벤트를 받을 객체 선언 // Declare an object to receive events
	CDeviceEventProfileEx eventProfile;

	// 센서에 이벤트 객체 설정 // Set event object on sensor
	devLaserProfile.RegisterDeviceEvent(&eventProfile);

	do
	{
		wchar_t arrInput[4096] = { 0, };
		CFLString<wchar_t> flsInput = L"";

		// IP 주소를 입력 받습니다. // Enter the IP address.
		while(true)
		{
			printf("Input IP Address: ");

			fgetws(arrInput, 4096, stdin);

			flsInput = arrInput;
			flsInput.Replace(L"\n", L"");

			// 입력 받은 IP 주소를 설정합니다. // Set the entered IP address.
			if((res = devLaserProfile.SetIPAddress(flsInput)).IsOK())
				break;

			ErrorPrint(res, "Failed to set IP Address.\n");
		}

		// 포트 번호를 입력 받습니다. // Enter the port number.
		while(true)
		{
			printf("Input Port Number: ");

			fgetws(arrInput, 4096, stdin);

			flsInput = arrInput;
			flsInput.Replace(L"\n", L"");

			// 입력 받은 포트 번호를 설정합니다. // Set the entered port number.
			if((res = devLaserProfile.SetPortNumber((uint16_t)flsInput.ToUint32())).IsOK())
				break;

			ErrorPrint(res, "Failed to set port number.\n");
		}

		// 프로파일 센서를 초기화 합니다. // Initialize the profile sensor.
		if((res = devLaserProfile.Initialize()).IsFail())
		{
			ErrorPrint(res, "Failed to initialize device.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if((res = viewImage.Create(400, 0, 812, 384)).IsFail())
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 3D 뷰 생성 // Create 3D view
		if((res = view3D.Create(812, 0, 1224, 384)).IsFail())
		{
			ErrorPrint(res, "Failed to create the 3D view.\n");
			break;
		}

		// 각 뷰 윈도우의 위치를 동기화 한다 // Synchronize the position of each view window
		if((res = viewImage.SynchronizeWindow(&view3D)).IsFail())
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 이벤트 객체에 view를 설정합니다. // Set a View on the event object.
		eventProfile.SetViewImage(&viewImage);
		eventProfile.SetView3D(&view3D);

		// 서피스 컬러라이제이션 기능을 설정합니다. Set the surface colorization feature.
		devLaserProfile.EnableSurfaceColorization(true);
		devLaserProfile.SetSurfaceColorizationRange(-5, 5);

		// 프로파일 센서를 Start 합니다. // Start the profile sensor.
		if((res = devLaserProfile.Start()).IsFail())
		{
			ErrorPrint(res, "Failed to start the profile sensor.\n");
			break;
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage.IsAvailable())
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