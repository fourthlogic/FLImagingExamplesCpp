#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 


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
			m_pViewImage = pViewImage;
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

			if(m_pViewImage->GetImagePtr() != m_pImage)
			{
				// 이미지 뷰에 이미지 포인터 설정 // Set the image pointer for the image view.
				m_pViewImage->SetImagePtr(m_pImage);
				m_pViewImage->ZoomFit();
			}

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

	CResult res = EResult_UnknownError;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage;

	// Matrox 카메라 선언 // Declare Matrox camera
	CDeviceCameraMatrox camMatrox;

	do
	{
		wchar_t arrInput[4096] = { 0, };

		CFLString<wchar_t> flsCamfilePath = L"";
		CDeviceCameraMatrox::EDeviceType eDeviceType = CDeviceCameraMatrox::EDeviceType_Unknown;
		int32_t i32DeviceIndex = 0;
		int32_t i32ModuleIndex = 0;

		CFLString<wchar_t> flsInput = L"";

		// Cam file 의 전체 경로를 입력합니다. // Enter the full path of the CAM file.
		printf("Enter camfile full path (e.g. C:/Camfile/AnyCamfile.cam): ");
		fgetws(arrInput, 4096, stdin);
		flsCamfilePath = arrInput;
		flsCamfilePath.Replace(L"\r", L"");
		flsCamfilePath.Replace(L"\n", L"");

		// 장치 타입을 입력합니다. // Enter the device type.
		printf("Device type\n");
		printf("1.Clarity UHD\t\t2.Concord POE\t\t3.GenTL\n");
		printf("4.GevIQ\t\t\t5.GigE\t\t\t6.Host\n");
		printf("7.Indio\t\t\t8.Iris GTX\t\t9.Morphis\n");
		printf("10.Radient eV-CXP\t11.Radient eV-CL\t12.Rapixo Pro CL\n");
		printf("13.Rapixo CXP\t\t14.Solios\t\t15.USB3\n");
		printf("Enter device type: ");
		fgetws(arrInput, 4096, stdin);
		flsInput = arrInput;
		eDeviceType = (CDeviceCameraMatrox::EDeviceType)flsInput.ToInt32();

		// 장치의 인덱스를 입력합니다. // Enter the device index.
		printf("Enter device index: ");
		fgetws(arrInput, 4096, stdin);
		flsInput = arrInput;
		i32DeviceIndex = flsInput.ToInt32();

		// 모듈의 인덱스를 입력합니다. // Enter the module index.
		printf("Enter module index: ");
		fgetws(arrInput, 4096, stdin);
		flsInput = arrInput;
		i32ModuleIndex = flsInput.ToInt32();

		printf("\n");

		// 이벤트를 받을 객체 선언 // Declare an event object.
		CDeviceEventImageEx eventImage;

		// 카메라에 이벤트 객체 설정 // Set the event object for the camera.
		camMatrox.RegisterDeviceEvent(&eventImage);

		// 카메라에 장치 설정
		camMatrox.SetCamFilePath(flsCamfilePath);
		camMatrox.SetDeviceType(eDeviceType);
		camMatrox.SetDeviceIndex(i32DeviceIndex);
		camMatrox.SetModuleIndex(i32ModuleIndex);

		// 카메라를 초기화 합니다. // Initialize the camera.
		if(IsFail(res = camMatrox.Initialize()))
		{
			ErrorPrint(res, "Failed to initialize the camera.\n");
			break;
		}

		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage.Create(0, 0, 1000, 1000)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		eventImage.SetViewImage(&viewImage);

		// 카메라를 Live 합니다. // Start live acquisition.
		if(IsFail(res = camMatrox.Live()))
		{
			ErrorPrint(res, "Failed to live the camera.\n");
			break;
		}

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	// 카메라의 초기화를 해제합니다. // Terminate the camera.
	camMatrox.Terminate();
	camMatrox.ClearDeviceEvents();

	return 0;
}