#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do 
	{
		// 스냅 뷰 객체 선언 // Declare SNAP View
		CGUIViewSNAPWrap viewSNAP;

		// 스냅 뷰 생성 // Create SNAP view
		if((res = viewSNAP.Create(0, 0, 600, 600)).IsFail())
		{
			ErrorPrint(res, "Failed to create the SNAP view.\n");
			break;
		}

		// 스냅 파일 로드 // Load SNAP file
		if((res = viewSNAP.Load(L"..\\..\\FLImagingExamplesSNAP\\Advanced Functions\\Object\\Blob\\Blob.flsf")).IsFail())
		{
			ErrorPrint(res, "Failed to load the SNAP file.\n");
			break;
		}

		// 스냅 실행 // Run SNAP
		res = viewSNAP.Run();

		// 스냅이 종료될 때 까지 기다림 // Wait for the SNAP to close
		while(viewSNAP.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}