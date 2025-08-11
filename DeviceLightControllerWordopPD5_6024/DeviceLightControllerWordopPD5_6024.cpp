#include <cstdio>

#include <FLImaging.h>

#include "../CommonHeader/ErrorPrint.h" 

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CResult res = EResult_UnknownError;

	// 조명 컨트롤러 WordopPD5_6024 선언 // Declare the WordopPD5_6024 Light Controller
	CDeviceLightControllerWordopPD5_6024 lightControllerWordopPD56024;

	do
	{
		wchar_t arrInput[4096] = { 0, };

		CFLString<wchar_t> flsInput = L"";
		int32_t i32ConnectionType = 0;
		bool bExit = false;

		do
		{
			// 조명 컨트롤러 연결 방식을 선택합니다. // Select the connection method for the light controller.
			printf("1. RS232C\n");
			printf("2. TCP Server\n");
			printf("3. TCP Client\n");
			printf("4. UDP\n");
			printf("0. Exit\n");
			printf("\n");
			printf("Connection Type: ");
			fgetws(arrInput, 4096, stdin);
			printf("\n");
			system("cls");
			flsInput = arrInput;

			int32_t i32Select = flsInput.ToInt32();

			if(i32Select == 0)
				bExit = true;
			else if(i32Select == 1 || i32Select == 2 || i32Select == 3 || i32Select == 4)
			{
				i32ConnectionType = i32Select;
				break;
			}

			printf("Incorrect input. Please select again.\n\n");
		}
		while(true);

		if(bExit)
			break;

		CDeviceLightControllerWordopPD5_6024::EConnectionMethod eConnectionMethod = CDeviceLightControllerWordopPD5_6024::EConnectionMethod_RS232C;

		if(i32ConnectionType == 2)
			eConnectionMethod = CDeviceLightControllerWordopPD5_6024::EConnectionMethod_TCPServer;
		else if(i32ConnectionType == 3)
			eConnectionMethod = CDeviceLightControllerWordopPD5_6024::EConnectionMethod_TCPClient;
		else if(i32ConnectionType == 4)
			eConnectionMethod = CDeviceLightControllerWordopPD5_6024::EConnectionMethod_UDP;

		// 연결 방식을 설정합니다. // Set the connection method.
		lightControllerWordopPD56024.SetConnectionMethod(eConnectionMethod);

		if(i32ConnectionType == 1) // RS232C
		{
			// 컴포트 번호를 입력합니다. // Enter the COM port number.
			int32_t i32ComPortNumber = 0;
			printf("Port Number: ");
			fgetws(arrInput, 4096, stdin);
			flsInput = arrInput;
			i32ComPortNumber = flsInput.ToInt32();
			system("cls");
			// 컴포트 번호 설정 // Set the COM port number.
			lightControllerWordopPD56024.SetConnectionComPortNumber(i32ComPortNumber);
		}
		else
		{
			CFLString<wchar_t> flsIPAddress;
			uint16_t u16Port = 0;

			// IP 번호를 입력합니다. // Enter the IP address.
			printf("Input IP Address: ");
			fgetws(arrInput, 4096, stdin);

			flsIPAddress = arrInput;

			// Port 번호를 입력합니다. // Enter the port number.
			printf("Port Number: ");
			fgetws(arrInput, 4096, stdin);
			system("cls");
			flsInput = arrInput;
			u16Port = flsInput.ToInt32();

			// IP 주소, Port 설정 // Set the IP address and port.
			lightControllerWordopPD56024.SetConnectionIPAddress(flsIPAddress);
			lightControllerWordopPD56024.SetConnectionPort(u16Port);
		}

		if(IsFail(res = lightControllerWordopPD56024.Initialize()))
		{
			ErrorPrint(res, "Failed to initialize the light controller.\n");
			break;
		}

		int32_t i32Channel = 0;

		do
		{
			// 채널 갯수를 선택합니다. // Select the number of channels.
			printf("1. Channel 4\n");
			printf("2. Channel 8\n");
			printf("0. Exit\n");
			printf("\n");
			printf("Input Channel Count: ");
			fgetws(arrInput, 4096, stdin);
			printf("\n");
			system("cls");
			flsInput = arrInput;

			int32_t i32Select = flsInput.ToInt32();

			if(i32Select == 0)
				bExit = true;
			else if(i32Select == 1 || i32Select == 2)
			{
				i32Channel = i32Select;
				break;
			}

			printf("Incorrect input. Please select again.\n\n");
		}
		while(true);

		if(bExit)
			break;

		CDeviceLightControllerWordopPD5_6024::ELightChannel eLightChannel = CDeviceLightControllerWordopPD5_6024::ELightChannel_Port_4;

		if(i32Channel == 1)
			eLightChannel = CDeviceLightControllerWordopPD5_6024::ELightChannel_Port_8;

		// 채널 갯수 설정 // Set the number of channels.
		lightControllerWordopPD56024.SetLightChannel(eLightChannel);

		int32_t i32CommuticationType = 0;

		do
		{
			// 통신 방식을 선택합니다. // Select the communication method.
			printf("1. ASCII Code\n");
			printf("2. Hexadecimal\n");
			printf("0. Exit\n");
			printf("\n");
			printf("Input Communication Type: ");
			fgetws(arrInput, 4096, stdin);
			printf("\n");
			system("cls");
			flsInput = arrInput;

			int32_t i32Select = flsInput.ToInt32();

			if(i32Select == 0)
				bExit = true;
			else if(i32Select == 1 || i32Select == 2)
			{
				i32CommuticationType = i32Select;
				break;
			}

			printf("Incorrect input. Please select again.\n\n");
		}
		while(true);

		if(bExit)
			break;

		CDeviceLightControllerWordopPD5_6024::ECommunicationType eCommunicationType = CDeviceLightControllerWordopPD5_6024::ECommunicationType_ASCIICode;

		if(i32CommuticationType == 2)
			eCommunicationType = CDeviceLightControllerWordopPD5_6024::ECommunicationType_Hexadecimal;

		// 통신 방식을 설정합니다. // Set the communication type.
		lightControllerWordopPD56024.SetCommunicationType(eCommunicationType);

		int32_t i32TriggerMethod = 0;

		while(true)
		{
			int32_t i32SelectMode = 0;

			// 작업 모드를 선택합니다. // Select the operation mode.
			printf("1. Light On/Off\n");
			printf("2. Light Value\n");
			printf("3. Strobe Time\n");
			printf("4. Trigger Method\n");
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

			if(i32SelectMode == 4)
			{
				do
				{
					// 트리거 방식을 선택합니다. // Select the trigger method.
					printf("1. Low Level\n");
					printf("2. High Level\n");
					printf("3. Falling Edge\n");
					printf("4. Rising Edge\n");
					printf("0. Exit\n");
					printf("\n");
					printf("Input Trigger Method: ");
					fgetws(arrInput, 4096, stdin);
					printf("\n");
					system("cls");
					flsInput = arrInput;

					int32_t i32Select = flsInput.ToInt32();

					if(i32Select == 0)
						bExit = true;
					else if(i32Select == 1 || i32Select == 2 || i32Select == 3 || i32Select == 4)
					{
						i32TriggerMethod = i32Select;
						break;
					}

					printf("Incorrect input. Please select again.\n\n");
				}
				while(true);

				CDeviceLightControllerWordopPD5_6024::ETriggerMethod eTriggerMethod = CDeviceLightControllerWordopPD5_6024::ETriggerMethod_LowLevel;

				if(i32TriggerMethod == 2)
					eTriggerMethod = CDeviceLightControllerWordopPD5_6024::ETriggerMethod_HighLevel;
				else if(i32TriggerMethod == 3)
					eTriggerMethod = CDeviceLightControllerWordopPD5_6024::ETriggerMethod_FallingEdge;
				else if(i32TriggerMethod == 4)
					eTriggerMethod = CDeviceLightControllerWordopPD5_6024::ETriggerMethod_RisingEdge;

				// 트리거 방식을 설정합니다. // Set the trigger method.
				lightControllerWordopPD56024.SetTriggerMethod(eTriggerMethod);
			}
			else
			{
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
						lightControllerWordopPD56024.SetChannelState(i32Channel, true);
					else if(i32OnOff == 1)
						lightControllerWordopPD56024.SetChannelState(i32Channel, false);
				}
				else if(i32SelectMode == 2)
				{
					// 조명 값을 설정합니다. // Set the light value.
					printf("Input Light Value (0 ~ 255): ");
					fgetws(arrInput, 4096, stdin);
					flsInput = arrInput;
					system("cls");
					uint8_t u8Value = (uint8_t)flsInput.ToInt32();

					lightControllerWordopPD56024.SetLightValue(i32Channel, u8Value);
				}
				else if(i32SelectMode == 3)
				{
					// 스토로브 타임을 설정합니다. // Set the strobe time.
					printf("Input Strobe Time (1 ~ 999 ms): ");
					fgetws(arrInput, 4096, stdin);
					flsInput = arrInput;
					system("cls");
					uint16_t u16StrobeTime = (uint16_t)flsInput.ToInt32();

					lightControllerWordopPD56024.SetStrobeTime(i32Channel, u16StrobeTime);
				}
			}

			// 입력된 파라미터를 적용합니다. // Apply the configured parameters.
			lightControllerWordopPD56024.Apply();
		}

		if(bExit)
			break;
	}
	while(false);

	// 조명 컨트롤러에 연결을 종료합니다. // Terminate the connection to the light controller.
	if(IsFail(res = lightControllerWordopPD56024.Terminate()))
	{
		ErrorPrint(res, "Failed to terminate the motion.\n");
	}

	return 0;
}