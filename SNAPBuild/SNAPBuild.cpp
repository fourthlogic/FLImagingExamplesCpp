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
		// 스냅 빌드 객체 선언 // Declare SNAP Build
		CSNAPBuild snapBuild;

		// 스냅 파일 로드 // Load SNAP file
		if((res = snapBuild.Load(L"Example.flsf")).IsFail())
		{
			ErrorPrint(res, "Failed to load the SNAP file.\n");
			break;
		}

		// 소스 이미지 노드를 찾습니다. // Finds the source image node.
		CSNAPObjectNode* pNodeSourceImage = snapBuild.FindNode(L"Image", L"Source Image");

		if(!pNodeSourceImage)
		{
			res = EResult_FailedToFind;
			ErrorPrint(res, "Failed to find the node.\n");
			break;
		}

		// 임계값 처리 결과 노드를 찾습니다. // Finds the threshold result node.
		CSNAPObjectNode* pNodeThresholdResult = snapBuild.FindNode(L"MultiVar<Double>", L"Threshold Result");

		if(!pNodeThresholdResult)
		{
			res = EResult_FailedToFind;
			ErrorPrint(res, "Failed to find the node.\n");
			break;
		}

		CFLImage fliSource;

		// 소스 이미지를 로드합니다. // Loads the source image.
		if((res = fliSource.Load(L"..\\..\\ExampleImages\\Blob\\Ball.flif")).IsFail())
		{
			ErrorPrint(res, "Failed to load the image.\n");
			break;
		}

		// 노드에 소스 이미지를 설정합니다. // Sets the source image to the node.
		if((res = pNodeSourceImage->SetParameter(L"Image", &fliSource)).IsFail())
		{
			ErrorPrint(res, "Failed to set the parameter.\n");
			break;
		}

		// 스냅 실행 // Run SNAP
		res = snapBuild.Run();
		
		// 스냅 실행이 종료 될때까지 대기합니다. // Waits until the SNAP run is complete.
		snapBuild.WaitStop();

		CMultiVar<double> mvThresholdResult;

		// 노드에서 임계값 처리 결과를 얻어옵니다. // Gets the threshold result from the node.
		res = pNodeThresholdResult->GetParameter(L"Multi Variable", &mvThresholdResult);

		// 스냅이 종료될 때 까지 기다림 // Wait for the SNAP to close
		while(snapBuild.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}