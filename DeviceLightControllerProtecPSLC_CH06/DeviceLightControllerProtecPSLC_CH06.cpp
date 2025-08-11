#include <cstdio>

#include <FLImaging.h>

#include "../CommonHeader/ErrorPrint.h" 

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CResult res = EResult_UnknownError;

	// 조명 컨트롤러 ProtecPSLC_CH06 선언 // Declare the ProtecPSLC_CH06 Light Controller
	CDeviceLightControllerProtecPSLC_CH06 lightControllerProtecPSLCCH06;

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
			lightControllerProtecPSLCCH06.SetComPortNumber(i32ComPortNumber);
		}

		if(IsFail(res = lightControllerProtecPSLCCH06.Initialize()))
		{
			ErrorPrint(res, "Failed to initialize the light controller.\n");
			break;
		}

		while(true)
		{
			int32_t i32SelectMode = 0;

			// 작업 모드를 선택합니다. // Select the operation mode.
			printf("1. Live Mode\n");
			printf("2. Strobe Mode\n");
			printf("3. Multi Shot Strobe\n");
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

			int32_t i32TriggerIndex = 0;

			if(i32SelectMode == 1)
			{
				// On/Off 상태를 설정합니다. // Set the On/Off state.
				printf("1. Live On\n");
				printf("2. Live Off\n");
				printf("\n");
				printf("Select Number: ");
				fgetws(arrInput, 4096, stdin);
				printf("\n");
				system("cls");
				flsInput = arrInput;

				int32_t i32OnOff = flsInput.ToInt32();

				lightControllerProtecPSLCCH06.SetOperationMode(CDeviceLightControllerProtecPSLC_CH06::EOperationMode_Live);

				if(i32OnOff == 1)
				{
					lightControllerProtecPSLCCH06.EnableLiveTurnOn(true);

					// 설정할 채널 값을 입력합니다. // Enter the channel value to configure.
					printf("Select Channel Index: ");
					fgetws(arrInput, 4096, stdin);
					flsInput = arrInput;

					int32_t i32Channel = flsInput.ToInt32();

					// 조명 값을 설정합니다. // Set the light value.
					printf("Input Light Value (0 ~ 255): ");
					fgetws(arrInput, 4096, stdin);
					flsInput = arrInput;
					system("cls");
					uint8_t u8Value = (uint8_t)flsInput.ToInt32();

					lightControllerProtecPSLCCH06.SetLightValue(i32Channel, u8Value);
				}
				else if(i32OnOff == 2)
					lightControllerProtecPSLCCH06.EnableLiveTurnOn(false);
			}
			else if(i32SelectMode == 2)
			{
				lightControllerProtecPSLCCH06.SetOperationMode(CDeviceLightControllerProtecPSLC_CH06::EOperationMode_Strobe);

				// 설정할 트리거 Index을 입력합니다. // Enter the trigger index value to configure.
				printf("Select Trigger Index: ");
				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;

				i32TriggerIndex = flsInput.ToInt32();

				// 설정할 채널 Index 을 입력합니다. // Enter the channel index to configure.
				printf("Select Channel Index: ");
				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;

				int32_t i32Channel = flsInput.ToInt32();

				// 스토로브 값을 설정합니다. // Set the strobe value.
				printf("Input Strobe Value (0 ~ 4000 us): ");
				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;
				system("cls");
				uint16_t u16StrobeTime = (uint16_t)flsInput.ToInt32();

				lightControllerProtecPSLCCH06.SetStrobe(i32TriggerIndex, i32Channel, u16StrobeTime);
			}
			else if(i32SelectMode == 3)
			{
				lightControllerProtecPSLCCH06.SetOperationMode(CDeviceLightControllerProtecPSLC_CH06::EOperationMode_MultiShotStrobe);

				// 설정할 트리거 Index을 입력합니다. // Enter the trigger index value to configure.
				printf("Select Trigger Index: ");
				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;

				i32TriggerIndex = flsInput.ToInt32();

				// 설정할 메모리 index을 입력합니다. // Enter the memory index value to configure.
				printf("Select Memory Index: ");
				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;

				int32_t i32MemoryIndex = flsInput.ToInt32();

				// 설정할 채널 index을 입력합니다. // Enter the channel index value to configure.
				printf("Select Channel Index: ");
				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;

				int32_t i32Channel = flsInput.ToInt32();

				// 스토로브 값을 설정합니다. // Set the strobe value.
				printf("Input Strobe Value (0 ~ 4000 us): ");
				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;
				system("cls");
				uint16_t u16StrobeTime = (uint16_t)flsInput.ToInt32();

				lightControllerProtecPSLCCH06.SetMultiShotStrobe(i32TriggerIndex, i32MemoryIndex, i32Channel, u16StrobeTime);
			}

			// 입력된 파라미터를 적용합니다. // Apply the configured parameters.
			if(IsFail(res = lightControllerProtecPSLCCH06.Apply(i32TriggerIndex)))
			{
				ErrorPrint(res, "Failed to apply the light controller.\n");
				break;
			}
		}

		if(bExit)
			break;
	}
	while(false);

	// 조명 컨트롤러에 연결을 종료합니다. // Terminate the connection to the light controller.
	if(IsFail(res = lightControllerProtecPSLCCH06.Terminate()))
	{
		ErrorPrint(res, "Failed to terminate the motion.\n");
	}

	return 0;
}