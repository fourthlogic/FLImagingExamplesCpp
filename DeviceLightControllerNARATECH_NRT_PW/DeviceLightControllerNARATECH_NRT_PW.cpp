#include <cstdio>
#include <FLImaging.h>

#include "../CommonHeader/ErrorPrint.h" 

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CResult res;

	// 조명 컨트롤러 NARATECH_NRT_PW 선언 // Declare the NARATECH_NRT_PW Light Controller
	CDeviceLightControllerNARATECH_NRT_PW lightControllerNARATECH_NRT_PW;

	do
	{
		wchar_t arrInput[4096] = { 0, };

		CFLString<wchar_t> flsInput = L"";
		bool bExit = false;

		{
			// 컴포트 번호를 입력합니다. // Enter the COM port number.
			int32_t i32ComPortNumber = 0;
			printf("Port Number: ");
			fgetws(arrInput, 4096, stdin);
			flsInput = arrInput;
			i32ComPortNumber = flsInput.ToInt32();
			system("cls");
			// 컴포트 번호 설정 // Set the COM port number.
			lightControllerNARATECH_NRT_PW.SetComPortNumber(i32ComPortNumber);

			// 보드레이트 입력합니다. // Enter the baud rate.
			int32_t i32BaudRate = 0;
			printf("BaudRate(Switch ON = 9600, OFF = 19200]: ");
			fgetws(arrInput, 4096, stdin);
			flsInput = arrInput;
			i32BaudRate = flsInput.ToInt32();
			system("cls");
			// 컴포트 번호 설정 // Set the COM port number.
			lightControllerNARATECH_NRT_PW.SetBaudRate(i32BaudRate);
		}

		if(IsFail(res = lightControllerNARATECH_NRT_PW.Initialize()))
		{
			ErrorPrint(res, "Failed to initialize the light controller.\n");
			break;
		}

		while(true)
		{
			int32_t i32SelectMode = 0;

			// 작업 모드를 선택합니다. // Select the operation mode.
			printf("1. Light On/Off\n");
			printf("2. Light Value\n");
			printf("0. Exit\n");
			printf("\n");
			printf("Select Number: ");
			fgetws(arrInput, 4096, stdin);
			printf("\n");
			system("cls");
			flsInput = arrInput;
			i32SelectMode = flsInput.ToInt32();

			if(i32SelectMode == 0)
			{
				bExit = true;
				break;
			}

			// 설정할 채널 값을 입력합니다. // Enter the channel value to configure.
			printf("Select Channel: ");
			fgetws(arrInput, 4096, stdin);
			flsInput = arrInput;

			int32_t i32Channel = flsInput.ToInt32();

			if(i32SelectMode == 1)
			{
				printf("0. On\n");
				printf("1. Off\n");
				printf("\n");
				printf("Enter On/Off: ");
				fgetws(arrInput, 4096, stdin);
				printf("\n");
				system("cls");
				flsInput = arrInput;

				int32_t i32OnOff = flsInput.ToInt32();

				// 채널별 On/Off 상태를 설정합니다. // Set the On/Off state for the channel.
				if(i32OnOff == 0)
					lightControllerNARATECH_NRT_PW.SetChannelState(i32Channel, true);
				else if(i32OnOff == 1)
					lightControllerNARATECH_NRT_PW.SetChannelState(i32Channel, false);
			}
			else if(i32SelectMode == 2)
			{
				// 조명 값을 설정합니다. // Set the light value.
				printf("Input Light Value (0 ~ 255): ");
				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;
				system("cls");
				uint8_t u8Value = (uint8_t)flsInput.ToInt32();

				lightControllerNARATECH_NRT_PW.SetLightValue(i32Channel, u8Value);
			}

			// 입력된 파라미터를 적용합니다. // Apply the configured parameters.
			lightControllerNARATECH_NRT_PW.Apply();
		}

		if(bExit)
			break;
	}
	while(false);

	// 조명 컨트롤러에 연결을 종료합니다. // Terminate the connection to the light controller.
	if(IsFail(res = lightControllerNARATECH_NRT_PW.Terminate()))
	{
		ErrorPrint(res, "Failed to terminate the motion.\n");
	}

	return 0;
}