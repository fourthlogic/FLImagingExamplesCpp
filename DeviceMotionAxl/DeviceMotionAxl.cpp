#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h" 

// 모션 기능 enum // motion feature enum
enum EMotionFeature
{
	EMotionFeature_SearchOriginPosition = 0,
	EMotionFeature_MoveAbsolutePosition,
	EMotionFeature_MoveIncrementalPosition,
};

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CResult res = EResult_UnknownError;

	// Axl Motion 장치를 선언 // Declare Axl Motion device
	CDeviceMotionAxl motionAxl;

	do
	{
		wchar_t arrInput[4096] = { 0, };
		CFLString<wchar_t> flsInput = L"";

		// 모션 파일의 전체 경로를 입력합니다. // Enter the full path of the motion file.
		printf("Enter motion file full path (e.g. C:/Motion/Any.mot): ");
		fgetws(arrInput, 4096, stdin);

		CFLString<wchar_t> flsMotionPath = arrInput;
		flsMotionPath.Replace(L"\r", L"");
		flsMotionPath.Replace(L"\n", L"");

		// 모션 파일의 경로를 설정합니다. // Sets the path to the motion file.
		if((res = motionAxl.SetMotionFilePath(flsMotionPath)).IsFail())
		{
			ErrorPrint(res, L"Failed to set motion file path.");
			break;
		}

		// 연결할 축 개수를 설정합니다. // Sets the number of axes to connect to.
		if((res = motionAxl.SetAxisCount(1)).IsFail())
		{
			ErrorPrint(res, L"Failed to set axis count.");
			break;
		}

		// 모션 장치를 초기화 합니다. // Initialize the motion device.
		if((res = motionAxl.Initialize()).IsFail())
		{
			ErrorPrint(res, L"Failed to initialize the device.");
			break;
		}

		// 모션 축 객체를 얻어옵니다. // Obtain motion axis objects.
		const CDeviceMotionAxlAxis* pMotionAxis = dynamic_cast<const CDeviceMotionAxlAxis*>(motionAxl.GetMotionAxis(0));

		if(!pMotionAxis)
		{
			printf("Failed to get motion axis.\n");
			break;
		}

		// 서보를 켭니다. // Turn on the servo.
		if((res = pMotionAxis->SetServoOn(true)).IsFail())
		{
			ErrorPrint(res, "Failed to servo on.\n");
			break;
		}

		do
		{
			CThreadUtilities::Sleep(100);
		}
		while(!pMotionAxis->IsServoOn());

		while(true)
		{
			EMotionFeature eMotionFeature = EMotionFeature_MoveAbsolutePosition;
			bool bExit = false;

			while(true)
			{
				// 사용할 모션 기능을 선택합니다. // Select the motion feature you want to use.
				printf("\n");
				printf("1. Search Origin Position\n");
				printf("2. Move Absolute Position\n");
				printf("3. Move Incremental Position\n");
				printf("0. Exit\n");
				printf("Select: ");
				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;

				int32_t i32Select = flsInput.ToInt32();

				bool bSelected = true;

				switch(i32Select)
				{
				case 0:
					bExit = true;
					break;
				case 1:
					eMotionFeature = EMotionFeature_SearchOriginPosition;
					break;
				case 2:
					eMotionFeature = EMotionFeature_MoveAbsolutePosition;
					break;
				case 3:
					eMotionFeature = EMotionFeature_MoveIncrementalPosition;
					break;

				default:
					bSelected = false;
					break;
				}

				if(bSelected)
					break;

				printf("Incorrect input. Please select again.\n\n");
			}

			if(bExit)
				break;

			if(eMotionFeature == EMotionFeature_SearchOriginPosition)
			{
				// 원점 복귀 동작을 진행합니다. // Proceed with the return-to-origin action.
				if((res = pMotionAxis->SearchOriginPosition()).IsFail())
				{
					ErrorPrint(res, "Failed to search origin position.\n");
					break;
				}

				// 원점 복귀 동작이 완료 될때까지 대기합니다. // Wait until the return to origin action is complete.
				do
				{
					CThreadUtilities::Sleep(100);
				}
				while(pMotionAxis->GetSearchOriginStatus() == CDeviceMotionAxlAxis::ESearchOriginStatus_Searching);

				if(pMotionAxis->GetSearchOriginStatus() == CDeviceMotionAxlAxis::ESearchOriginStatus_Error)
					printf("Failed to search origin position.\n");
				else
					printf("Successed to search origin position.\n");
			}
			else
			{
				double f64MoveVelocity = 0.;
				double f64MoveAccelAndDecel = 0.;
				double f64MovePosition = 0.;

				// 이동 속도를 입력합니다. // Enter the velocity of movement.
				printf("Enter Axis Velocity(mm/s): ");
				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;
				f64MoveVelocity = flsInput.ToDouble();

				// 가감속을 입력합니다. // Enter acceleration and deceleration.
				printf("Enter Axis Acceleration(mm/s^2): ");
				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;
				f64MoveAccelAndDecel = flsInput.ToDouble();

				// 이동거리나 절대위치를 입력합니다. // Enter the distance or absolute position.
				printf("Enter Axis Position(mm): ");
				fgetws(arrInput, 4096, stdin);
				flsInput = arrInput;
				f64MovePosition = flsInput.ToDouble();

				if(eMotionFeature == EMotionFeature_MoveAbsolutePosition)
				{
					// 절대 좌표로 이동합니다. // Move to absolute coordinates.
					if((res = pMotionAxis->MovePosition(f64MovePosition, f64MoveVelocity, f64MoveAccelAndDecel, f64MoveAccelAndDecel, false)).IsFail())
					{
						ErrorPrint(res, "Failed to move position.\n");
						break;
					}

					// 모션이 정지 될때까지 대기합니다. // Wait until motion stops.
					do
					{
						CThreadUtilities::Sleep(100);
					}
					while(!pMotionAxis->IsMotionDone());
				}
				else if(eMotionFeature == EMotionFeature_MoveIncrementalPosition)
				{
					// 상대 좌표로 이동합니다. // Move to relative coordinates.
					if((res = pMotionAxis->MoveDistance(f64MovePosition, f64MoveVelocity, f64MoveAccelAndDecel, f64MoveAccelAndDecel, false)).IsFail())
					{
						ErrorPrint(res, "Failed to move distance.\n");
						break;
					}

					// 모션이 정지 될때까지 대기합니다. // Wait until motion stops.
					do
					{
						CThreadUtilities::Sleep(100);
					}
					while(!pMotionAxis->IsMotionDone());
				}
			}
		}
	}
	while(false);

	// Motion 장치의 초기화를 해제합니다. // Terminate the Motion device.
	motionAxl.Terminate();

	if(res.IsFail())
		getchar();

	return 0;
}