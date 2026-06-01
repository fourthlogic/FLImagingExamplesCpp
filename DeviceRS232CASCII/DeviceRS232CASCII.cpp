#include <cstdio>

#include <FLImaging.h>

#include "../CommonHeader/ErrorPrint.h" 

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CResult res = EResult_UnknownError;

	// RS232C ASCII 선언 // Declare the RS232C ASCII
	CDeviceRS232CASCII rs232CASCII;

	do
	{
		wchar_t arrInput[4096] = { 0, };

		CFLString<wchar_t> flsInput = L"";
		bool bExit = false;

		// 페시브 모드 설정 false // Set passive mode to false
		rs232CASCII.SetPassive(false);

		{
			// 컴포트 번호를 입력합니다. // Enter the COM port number.
			int32_t i32ComPortNumber = 0;
			printf("Port Number: ");
			fgetws(arrInput, 4096, stdin);
			flsInput = arrInput;
			i32ComPortNumber = flsInput.ToInt32();
			system("cls");
			// 컴포트 번호 설정 // Set the COM port number.
			rs232CASCII.SetComPortNumber(i32ComPortNumber);
		}

		{
			int32_t i32MenuSelection = 0;
			int32_t i32BaudRate = 0;
			bool bValidInput = false;

			// 보드레이트를 입력합니다. // Enter the Baud Rate.
			while(!bValidInput)
			{
				printf("=== Select Baud Rate ===\n");
				printf("1. 9600\n");
				printf("2. 19200\n");
				printf("3. 38400\n");
				printf("4. 57600\n");
				printf("5. 115200\n");
				printf("Select Number: ");

				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;
				i32MenuSelection = flsInput.ToInt32();
				system("cls");

				// 선택한 번호에 따라 보드 레이트 매핑 // Map the Baud Rate according to the selected number
				switch(i32MenuSelection)
				{
				case 1:
					i32BaudRate = 9600;
					bValidInput = true;
					break;
				case 2:
					i32BaudRate = 19200;
					bValidInput = true;
					break;
				case 3:
					i32BaudRate = 38400;
					bValidInput = true;
					break;
				case 4:
					i32BaudRate = 57600;
					bValidInput = true;
					break;
				case 5:
					i32BaudRate = 115200;
					bValidInput = true;
					break;
				default:
					printf("[Error] Invalid selection. Please try again.\n\n");
					bValidInput = false;
					break;
				}
			}

			// 보드 레이트 설정 // Set the Baud Rate.
			rs232CASCII.SetBaudRate(i32BaudRate);
		}

		if(IsFail(res = rs232CASCII.Initialize()))
		{
			ErrorPrint(res, "Failed to initialize the light controller.\n");
			break;
		}

		while(true)
		{
			int32_t i32SelectMode = 0;

			// 작업 모드를 선택합니다. // Select the operation mode.
			printf("1. Send\n");
			printf("2. Recv\n");
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

			if(i32SelectMode == 1)
			{
				// 텍스트를 입력합니다. // Input text.
				printf("Input Text: ");
				fgetws(arrInput, 4096, stdin);
				printf("\n");
				system("cls");

				CFLString<char> flsCharInput(arrInput);

				if(flsCharInput.GetLength() > 0 && flsCharInput.GetAt(flsCharInput.GetLength() - 1) == L'\n')
					flsCharInput.Delete(flsCharInput.GetLength() - 1, 1);

				rs232CASCII.Send(flsCharInput);
			}
			else if(i32SelectMode == 2)
			{
				CFLString<char> packet;

				// 데이터 수신 // Receive data
				rs232CASCII.Recv(packet);

				// 수신 데이터 출력 // Print the received data
				if(packet.GetLength())
					printf("Recv Text: %s\n", packet.GetBuffer());
				else
					printf("No Recv Data.\n");
			}
		}

		if(bExit)
			break;
	}
	while(false);

	// RS232C ASCII 연결을 종료합니다. // Terminate the connection to the RS232C ASCII.
	if(IsFail(res = rs232CASCII.Terminate()))
	{
		// [오류 수정] 기존 "Failed to terminate the motion." 주석 및 문자열을 통신 종료에 맞게 변경
		ErrorPrint(res, "Failed to terminate the communication.\n");
	}

	return 0;
}