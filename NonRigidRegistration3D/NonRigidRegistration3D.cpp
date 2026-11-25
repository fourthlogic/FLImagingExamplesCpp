#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main() 
{
	// FLImaging(R) 라이브러리의 기능을 사용하기 전에 아래 함수를 반드시 한 번 호출해야 합니다.
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 객체 선언 // Declare 3D object
	CFL3DObject floDestinationObject;
	CFL3DObject floLearnObject;
	CFL3DObject floSourceObject;

	// 3D 뷰 선언 // Declare 3D view
	CGUIView3DWrap view3DDst;
	CGUIView3DWrap view3DLearn;
	CGUIView3DWrap view3DSource;


	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult eResult;

		// Learn Object 로드 // Load the Learn object
		if((eResult = floLearnObject.Load(L"../../ExampleImages/NonRigidRegistration3D/hand0Sample.fl3do")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the object file.\n");
			break;
		}

		// Source Object 로드 // Load the Source object
		if((eResult = floSourceObject.Load(L"../../ExampleImages/NonRigidRegistration3D/hand1Sample.fl3do")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the object file.\n");
			break;
		}

		// Learn 3D 뷰 생성 // Create the Learn 3D view
		if((eResult = view3DLearn.Create(0, 0, 500, 500)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to create the Learn 3D view.\n");
			break;
		}

		// Source 3D 뷰 생성 // Create the Source 3D view
		if((eResult = view3DSource.Create(500, 0, 1000, 512)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to create the Source 3D view.\n");
			break;
		}

		// Destination 3D 뷰 생성 // Create the Destination 3D view
		if((eResult = view3DDst.Create(0, 500, 500, 1000)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to create the Destination 3D view.\n");
			break;
		}

		// Learn 3D 뷰에 Learn Object 디스플레이 // Display the Learn object in the Learn 3D view
		if((eResult = view3DLearn.PushObject(floLearnObject)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to display the 3D object.\n");
			break;
		}

		// Source 3D 뷰에 Source Object 디스플레이 // Display the Source object in the Source 3D view
		if((eResult = view3DSource.PushObject(floSourceObject)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to display the 3D object.\n");
			break;
		}

		// CNonRigidRegistration3D 객체 생성 // Create CNonRigidRegistration3D object
		CNonRigidRegistration3D nonRigidRegistration3D;

		// Learn object 설정 // Set the learn object
		nonRigidRegistration3D.SetLearnObject(floLearnObject);

		// Source object 설정 // Set the source object
		nonRigidRegistration3D.SetSourceObject(floSourceObject);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((eResult = nonRigidRegistration3D.Execute()).IsFail())
		{
			ErrorPrint(eResult, L"Failed to execute Non-Rigid Registration 3D.");
			break;
		}

		// 3D 뷰의 시점을 객체에 맞춤 // Fit the 3D views to the objects
		view3DLearn.ZoomFit();
		view3DSource.ZoomFit();
		view3DDst.ZoomFit();

		// 화면에 출력하기 위해 3D 뷰에서 레이어 0번을 얻어옴 // Obtain layer 0 from the 3D view for display
		// 이 객체는 3D 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to a 3D view and does not need to be released separately
		CGUIView3DLayerWrap layer3DDst = view3DDst.GetLayer(0);
		CGUIView3DLayerWrap layer3DLearn = view3DLearn.GetLayer(0);
		CGUIView3DLayerWrap layer3DSource = view3DSource.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer3DDst.Clear();
		layer3DLearn.Clear();
		layer3DSource.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Area Alpha Value (Opaqueness) -> Font Thickness -> Font Italic

		CFLPoint<double> flp(0, 0);

		if((eResult = layer3DLearn.DrawTextCanvas(flp, L"Learn Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		if((eResult = layer3DSource.DrawTextCanvas(flp, L"Source Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		if((eResult = layer3DDst.DrawTextCanvas(flp, L"Destination Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		// 정합 결과 3D 객체 얻기 // Get the registration result 3D object
		CFL3DObject floResult;

		nonRigidRegistration3D.GetResult3DObject(floResult);

		// 결과 객체를 3D 뷰에 표시하기 위한 객체 생성 // Create an object to display the result object in the 3D view
		CGUIView3DObject view3DObject(floResult);

		int32_t i32ResultObjectIndex = 0;

		// Destination 3D 뷰에 결과 객체와 Source 객체 디스플레이 // Display the result object and the Source object in the Destination 3D view
		if(view3DDst.PushObject(view3DObject, &i32ResultObjectIndex).IsFail() ||
		   view3DDst.PushObject(floSourceObject).IsFail())
		{
			ErrorPrint(eResult, L"Failed to set object on the 3d view.\n");
			break;
		}

		// Learn 객체와 결과 객체의 정점 얻기 // Get the vertices of the Learn object and the result object
		CFLArray<TPoint3<float>> flaVertices = floLearnObject.GetVertices();
		CFLArray<TPoint3<float>> flaResultVertices = floResult.GetVertices();

		// 기준 점으로 Learn 객체의 첫 번째 정점 선택 // Select the first vertex of the Learn object as the reference point
		TPoint3<float> tp3Point = flaVertices[0];
		TPoint3<float> tp3ResultTranslationVector;
		TPoint3<float> tp3ResultEulerAngle;

		// 정합 전 원본 Learn 객체 기준으로 기준 점과 가장 가까운 결과 노드의 인덱스 얻기
		// Get the index of the result node nearest to the reference point, searched on the original Learn object before registration
		int32_t i32ResultNodeIndex = nonRigidRegistration3D.GetNearestResultNodeIndex(tp3Point);

		// 결과 객체에서 동일한 인덱스의 정점이 기준 점의 정합 결과 위치 // The vertex with the same index in the result object is the registered position of the reference point
		TPoint3<float> tp3ResultPoint = flaResultVertices[i32ResultNodeIndex];

		// 해당 노드의 결과 이동 벡터와 오일러 각 얻기 // Get the result translation vector and euler angle of the node
		nonRigidRegistration3D.GetResultTranslationVector(i32ResultNodeIndex, tp3ResultTranslationVector);
		nonRigidRegistration3D.GetResultEulerAngle(i32ResultNodeIndex, EEulerSequence_Extrinsic_XYZ, tp3ResultEulerAngle);

		TPoint3<double> tp3ResultPosition;
		TPoint3<double> tp3LearnPosition;

		// Learn 3D 뷰에 기준 점 위치 표시 // Display the reference point position in the Learn 3D view
		CFLString<wchar_t> flsLearnPosition = L"Source Point";

		tp3LearnPosition.x = (double)tp3Point.x;
		tp3LearnPosition.y = (double)tp3Point.y;
		tp3LearnPosition.z = (double)tp3Point.z;

		layer3DLearn.DrawText3D(tp3LearnPosition, flsLearnPosition, YELLOW, BLACK, 15);

		// Destination 3D 뷰의 결과 위치에 해당 노드의 결과 포즈 표시 // Display the result pose of the node at the result position in the Destination 3D view
		tp3ResultPosition.x = (double)tp3ResultPoint.x;
		tp3ResultPosition.y = (double)tp3ResultPoint.y;
		tp3ResultPosition.z = (double)tp3ResultPoint.z;

		CFLString<wchar_t> flsResultPose;
		flsResultPose.Format(L"Source -> Target\nT(%.3f, %.3f, %.3f)\nR(%.3f, %.3f, %.3f)", tp3ResultTranslationVector.x, tp3ResultTranslationVector.y, tp3ResultTranslationVector.z, tp3ResultEulerAngle.x, tp3ResultEulerAngle.y, tp3ResultEulerAngle.z);

		layer3DDst.DrawText3D(tp3ResultPosition, flsResultPose, YELLOW, BLACK, 15);

		// 결과 객체를 빨간색으로 강조 표시 // Highlight the result object in red
		((CGUIView3DObject*)view3DDst.GetView3DObject(i32ResultObjectIndex))->HighlightObject(RED);

		// 3D 뷰의 시점을 객체에 맞춤 // Fit the 3D views to the objects
		view3DLearn.ZoomFit();
		view3DSource.ZoomFit();
		view3DDst.ZoomFit();

		// 3D 뷰들의 시점을 동기화 // Synchronize the point of view of the 3D views
		view3DDst.SynchronizePointOfView(&view3DLearn);
		view3DDst.SynchronizePointOfView(&view3DSource);

		// 3D 뷰를 갱신 합니다. // Update 3d view
		view3DSource.Invalidate(true);
		view3DLearn.Invalidate(true);
		view3DDst.Invalidate(true);
		view3DDst.UpdateObject(i32ResultObjectIndex);

		// 3D 뷰가 종료될 때 까지 기다림 // Wait for the 3D views to close
		while(view3DLearn.IsAvailable() && view3DSource.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);

		view3DLearn.Destroy();
		view3DSource.Destroy();
		view3DDst.Destroy();
	}
	while(false);
}