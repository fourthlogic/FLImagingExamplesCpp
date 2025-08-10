#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CResult res = EResult_UnknownError;

	do
	{
		// 3D 뷰 선언
		// Declare 3D views.
		CGUIView3DWrap view3D[2];

		// 3D 뷰 생성
		// Create 3D views.
		if(IsFail(res = view3D[0].Create(100, 0, 612, 512)))
		{
			ErrorPrint(res, "Failed to create the 3D view.\n");
			break;
		}

		if(IsFail(res = view3D[1].Create(612, 0, 1124, 512)))
		{
			ErrorPrint(res, "Failed to create the 3D view.\n");
			break;
		}

		view3D[0].SynchronizeWindow(&view3D[1]);
		view3D[0].SetTopologyType(ETopologyType3D_Wireframe);
		view3D[1].SetTopologyType(ETopologyType3D_Wireframe);

		// 곡선의 접선에서 코드(Chord, 곡선의 두 점을 직선으로 연결한 선분)가 벗어날 수 있는 최대 거리를 나타냅니다. 
		// 이 값이 클수록 분할된 삼각형의 수가 적어지며, 
		// 반대로 값이 작을수록 더 많은 삼각형이 형성되어 곡선을 더 정밀하게 근사합니다. 
		// 기본값 0을 입력하면 step 모델에서 적절한 chordal deviation 값을 자동으로 계산합니다.
		// It represents the maximum distance that a chord (a straight line segment connecting two points on the curve) can deviate from the tangent of the curve. 
		// A larger value results in fewer triangles being formed, 
		// while a smaller value results in more triangles, providing a more precise approximation of the curve. 
		// The default value of 0 automatically calculates the appropriate chordal deviation value based on the imported step model.
		double f64ChordalDeviation = 0.;

		// 방법 1. CFL3DObject 에서 Step 파일 로드
		// Method 1. Load the STEP file directly into a CFL3DObject
		CFL3DObject fl3DObj0;

		if((res = fl3DObj0.LoadSTEP("../../ExampleImages/StepReaderConvertTo3DObject/Cylinder.step", f64ChordalDeviation)).IsFail())
		{
			ErrorPrint(res, "Failed to load step file.\n");
			break;
		}

		if((res = view3D[0].PushObject(fl3DObj0)).IsFail())
		{
			ErrorPrint(res, "Failed to display 3D object.\n");
			break;
		}


		// 방법 2. CStepReader 에서 Step 파일 로드 후 GetResult3DObject() 로 CFL3DObject 에 할당
		// Method 2. Load the STEP file using CStepReader and then assign it to CFL3DObject using GetResult3DObject().
		CFL3DObject fl3DObj1;
		CStepReader stepReader;
		f64ChordalDeviation = 0.00001;

		if((res = stepReader.Load("../../ExampleImages/StepReaderConvertTo3DObject\\Cylinder.step")).IsFail())
		{
			ErrorPrint(res, "Failed to load step file.\n");
			break;
		}

		if((res = stepReader.GetResult3DObject(fl3DObj1, f64ChordalDeviation)).IsFail())
		{
			ErrorPrint(res, "Failed to get 3D object from the StepReader.\n");
			break;
		}

		if((res = view3D[1].PushObject(fl3DObj1)).IsFail())
		{
			ErrorPrint(res, "Failed to display 3D object.\n");
			break;
		}

		view3D[0].ZoomFit();
		view3D[1].ZoomFit();

		// 아래 함수 DrawTextCanvas는 스크린 좌표를 기준으로 문자열을 뷰어에 출력한다.
		// The function DrawTextCanvas displays a string on the viewer using screen coordinates.
		// 파라미터 순서 : 기준 좌표 Figure 객체 -> 문자열 -> 텍스트 색 -> 텍스트 테두리 색 -> 폰트 크기 -> 실제 크기로 출력 유무 -> 각도 -> 정렬 -> 폰트 이름 -> 텍스트 알파값(불투명도) -> 텍스트 테두리 알파값 (불투명도) -> 폰트 두께 -> 폰트 이탤릭 여부
		// Parameter order: reference coordinate (Figure object) -> text string -> text color -> text outline color -> font size -> render in real-world size (bool) -> angle -> alignment -> font name -> text alpha (opacity) -> text outline alpha (opacity) -> font thickness -> italic font (bool)
		CFLPoint<double> flp(0, 0);
		if(IsFail(res = view3D[0].GetLayer(2).DrawTextCanvas(&flp, L"Chordal Deviation = 0.0", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		if(IsFail(res = view3D[1].GetLayer(2).DrawTextCanvas(&flp, L"Chordal Deviation = 0.00001", YELLOW, BLACK, 30)))
		{
			ErrorPrint(res, "Failed to draw text\n");
			break;
		}

		view3D[0].SetCanvasColor(WHITE);
		view3D[1].SetCanvasColor(WHITE);

		// 3D 뷰를 갱신 // Update 3D view
		view3D[0].UpdateScreen();
		view3D[1].UpdateScreen();

		// 3D 뷰가 종료될 때 까지 기다림 // Wait for the 3D view 
		while(view3D[0].IsAvailable() && view3D[1].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}