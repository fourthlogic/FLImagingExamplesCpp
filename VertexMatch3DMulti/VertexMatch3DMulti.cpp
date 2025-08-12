#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 3D 객체 선언 // Declare 3D object
	CFL3DObject arrLearnObject[3];
	CFL3DObject floSourceObject;

	// 3D 뷰 선언 // Declare 3D view	
	CGUIView3DWrap view3DDst;
	CGUIView3DWrap arrView3DLearn[3];
	CGUIView3DWrap view3DSource;


	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult eResult = EResult_UnknownError;

		const CFLString<wchar_t> flsBasePath = L"../../ExampleImages/SurfaceMatch3DMulti/";
		CFLString<wchar_t> arrPath[3] = { flsBasePath + L"Box1.ply", flsBasePath + L"Box2.ply", flsBasePath + L"Cylinder.ply" };
		CFLString<wchar_t> arrClassName[3] = { L"Box1", L"Box2", L"Cylinder" };
		int32_t arrMaxObject[3] = { 1, 1, 3 };

		// VertexMatch3DMulti 객체 생성 // Create VertexMatch3DMulti object
		CVertexMatch3DMulti vertexMatch3DMulti;

		// Source 3D 뷰 생성
		if(IsFail(eResult = view3DSource.Create(100, 500, 500, 900)))
		{
			ErrorPrint(eResult, "Failed to create the Source 3D view.\n");
			break;
		}

		// Dst 3D 뷰 생성
		if(IsFail(eResult = view3DDst.Create(500, 500, 900, 900)))
		{
			ErrorPrint(eResult, "Failed to create the Destination 3D view.\n");
			break;
		}

		for(int32_t i = 0; i < 3; ++i)
		{
			// Learn Object 로드 // Load the learn object
			if((eResult = arrLearnObject[i].Load(arrPath[i])).IsFail())
			{
				ErrorPrint(eResult, L"Failed to load the object file.\n");
				break;
			}

			// Learn 3D 뷰 생성
			if((eResult = arrView3DLearn[i].Create(100 + 400 * i, 100, 100 + 400 * (i + 1), 500)).IsFail())
			{
				ErrorPrint(eResult, "Failed to create the Source 3D view.\n");
				break;
			}

			if((eResult = arrView3DLearn[i].PushObject(arrLearnObject[i])).IsFail())
			{
				ErrorPrint(eResult, L"Failed to display the 3D object.\n");
				break;
			}

			arrView3DLearn[i].SetShadingType(EShadingType3D_Flat);

			// Learn object 설정 // Set the learn object
			vertexMatch3DMulti.SetLearnObject(arrLearnObject[i]);

			// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
			if((eResult = vertexMatch3DMulti.Learn(arrClassName[i])).IsFail())
			{
				ErrorPrint(eResult, L"Failed to learn Vertex Match 3D Multi.");
				break;
			}

			arrView3DLearn[i].SynchronizeWindow(&view3DDst);
			arrView3DLearn[i].SynchronizeWindow(&view3DSource);
		}

		// Source Object 로드 // Load the Source object
		if((eResult = floSourceObject.Load(L"../../ExampleImages/SurfaceMatch3DMulti/Source.ply")).IsFail())
		{
			ErrorPrint(eResult, L"Failed to load the object file.\n");
			break;
		}

		if((eResult = view3DSource.PushObject(floSourceObject)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to display the 3D object.\n");
			break;
		}

		// Source object 설정 // Set the source object
		vertexMatch3DMulti.SetSourceObject(floSourceObject);
		// Min score 설정 // Set the min score
		vertexMatch3DMulti.SetMinScore(0.3);
		// 학습 샘플링 거리 설정 // Set the learn sampling distance
		vertexMatch3DMulti.SetLearnSamplingDistance(0.03);
		// 장면 샘플링 거리 설정 // Set the scene sampling distance
		vertexMatch3DMulti.SetSceneSamplingDistance(0.03);
		// 키포인트 비율 설정 // Set the keypoint ratio.
		vertexMatch3DMulti.SetKeypointRatio(0.5);
		// 엣지 학습 여부 설정 // Set the edge train
		vertexMatch3DMulti.EnableTrainEdge(false);
		// 클러스터링 범위 설정 // Set the clustering range
		vertexMatch3DMulti.SetClusterRange(0.02);
		// 포즈 조정 반복 횟수 설정 // Set the iteration value of pose refinement
		vertexMatch3DMulti.SetIteration(5);

		// 검출 시 사용될 탐색 방식을 설정합니다. // Set the search method to be used for detection.
		vertexMatch3DMulti.SetMaxObjectMode(CMatchBase3DMulti::EMaxObjectMode_Class);

		// 최대 결과 개수 설정 // Set the max count of match result
		for(int64_t i = 0; i < 3; ++i)
			vertexMatch3DMulti.SetMaxObject(arrClassName[i], arrMaxObject[i]);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((eResult = vertexMatch3DMulti.Execute()).IsFail())
		{
			ErrorPrint(eResult, L"Failed to execute Vertex Match 3D Multi.");
			break;
		}

		view3DSource.SetShadingType(EShadingType3D_Flat);
		view3DDst.SetShadingType(EShadingType3D_Flat);

		view3DSource.ZoomFit();
		view3DDst.ZoomFit();

		view3DSource.SynchronizePointOfView(&view3DDst);

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately		
		CGUIView3DLayerWrap layer3DDst = view3DDst.GetLayer(0);
		CGUIView3DLayerWrap layer3DSource = view3DSource.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer3DDst.Clear();
		layer3DSource.Clear();

		// View 정보를 디스플레이 한다. // Display view information
		// 아래 함수 DrawTextCanvas 는 Screen좌표를 기준으로 하는 String을 Drawing 한다. // The function DrawTextCanvas below draws a String based on the screen coordinates.
		// 색상 파라미터를 EGUIViewImageLayerTransparencyColor 으로 넣어주게되면 배경색으로 처리함으로 불투명도를 0으로 한것과 같은 효과가 있다. // If the color parameter is added as EGUIViewImageLayerTransparencyColor, it has the same effect as setting the opacity to 0 by processing it as a background color.
		// 파라미터 순서 : 레이어 -> 기준 좌표 Figure 객체 -> 문자열 -> 폰트 색 -> 면 색 -> 폰트 크기 -> 실제 크기 유무 -> 각도 ->
		//                 얼라인 -> 폰트 이름 -> 폰트 알파값(불투명도) -> 면 알파값 (불투명도) -> 폰트 두께 -> 폰트 이텔릭
		// Parameter order: layer -> reference coordinate Figure object -> string -> font color -> Area color -> font size -> actual size -> angle ->
		//                  Align -> Font Name -> Font Alpha Value (Opaqueness) -> Cotton Alpha Value (Opaqueness) -> Font Thickness -> Font Italic		
		if((eResult = layer3DSource.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Source Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		if((eResult = layer3DDst.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Destination Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to draw text.\n");
			break;
		}

		for(int32_t i = 0; i < 3; ++i)
		{
			arrView3DLearn[i].ZoomFit();

			CGUIView3DLayerWrap layer3DLearn = arrView3DLearn[i].GetLayer(0);

			layer3DLearn.Clear();

			if((eResult = layer3DLearn.DrawTextCanvas(&CFLPoint<double>(0, 0), L"Class Name : " + arrClassName[i], YELLOW, BLACK, 20)).IsFail())
			{
				ErrorPrint(eResult, L"Failed to draw text.\n");
				break;
			}
		}

		// 3D 오브젝트 뷰에 결과 Object와 비교를 위한 Source 오브젝트 디스플레이		
		CFLArray<TPoint3<uint8_t>>* pFlaColors = (CFLArray<TPoint3<uint8_t>>*)floSourceObject.GetVertexColors();

		for(int32_t i = 0; i < (int32_t)pFlaColors->GetCount(); ++i)
		{
			pFlaColors->GetAt(i).x = 127;
			pFlaColors->GetAt(i).y = 127;
			pFlaColors->GetAt(i).z = 127;
		}

		if((eResult = view3DDst.PushObject(floSourceObject)).IsFail())
		{
			ErrorPrint(eResult, L"Failed to set object on the 3d view.\n");
			break;
		}

		ThreeDim::SPoseMatrixParametersMulti sResult;
		int64_t i64ResultCount = vertexMatch3DMulti.GetResultCount();
		TPoint3<double> tp3F64Rotation, tp3F64RotVec;
		CFLString<wchar_t> strText;
		CFLPoint3<double> flp3F64Translation;
		double f64Score, f64Residual;

		if(i64ResultCount == 0)
		{
			ErrorPrint(eResult, L"Failed to estimate pose matrix.\n");
			break;
		}

		for(int64_t i = 0; i < i64ResultCount; i++)
		{
			CFL3DObject floLearnTransform;
			Base::TPoint3<double> tp3Center;

			// 추정된 포즈 행렬 가져오기
			if((eResult = vertexMatch3DMulti.GetResultPoseMatrix(i, sResult)).IsFail())
			{
				ErrorPrint(eResult, L"Failed to estimate pose matrix.\n");
				break;
			}

			// 추정된 포즈 행렬이 적용된 결과 오브젝트 가져오기
			if((eResult = vertexMatch3DMulti.GetResultObject(i, floLearnTransform, tp3Center)).IsFail())
			{
				ErrorPrint(eResult, L"Failed to estimate pose matrix.\n");
				break;
			}

			f64Residual = sResult.f64Residual;
			f64Score = sResult.f64Score;
			tp3F64Rotation = sResult.tp3Angle;
			tp3F64RotVec = sResult.tp3RotationVector;
			flp3F64Translation.x = sResult.tp3TranslationVector.x;
			flp3F64Translation.y = sResult.tp3TranslationVector.y;
			flp3F64Translation.z = sResult.tp3TranslationVector.z;

			// 추정한 포즈 결과를 Console창에 출력한다 // Print the estimated pose matrix to the console window
			printf(" ▷ Pose Matrix %d\n", (int32_t)i);
			printf("  1. R : Rotation, T : Translation\n");
			printf("    Class Name : %ls", (wchar_t*)sResult.strClassName);
			printf("    Rx   : %.3lf\n", tp3F64Rotation.x);
			printf("    Ry   : %.3lf\n", tp3F64Rotation.y);
			printf("    Rz   : %.3lf\n", tp3F64Rotation.z);
			printf("    Rotation Vector X   : %.3lf\n", tp3F64RotVec.x);
			printf("    Rotation Vector Y   : %.3lf\n", tp3F64RotVec.y);
			printf("    Rotation Vector Z   : %.3lf\n", tp3F64RotVec.z);
			printf("    Tx   : %.3lf\n", flp3F64Translation.x);
			printf("    Ty   : %.3lf\n", flp3F64Translation.y);
			printf("    Tz   : %.3lf\n", flp3F64Translation.z);
			printf("    Score : %.3lf\n", f64Score);
			printf("    Residual : %.3lf\n", f64Residual);
			printf("\n");

			if((eResult = view3DDst.PushObject(floLearnTransform)).IsFail())
			{
				ErrorPrint(eResult, L"Failed to set object on the 3d view.\n");
				break;
			}

			strText.Format(L"Class Name : %ls\nR(%.04lf,%.04lf,%.04lf)\nRVec(%.04lf,%.04lf,%.04lf)\nT(%.04lf,%.04lf,%.04lf)\nScore : %lf\nResidual : %lf",
						   (wchar_t*)sResult.strClassName, tp3F64Rotation.x, tp3F64Rotation.y, tp3F64Rotation.z, tp3F64RotVec.x, tp3F64RotVec.y, tp3F64RotVec.z, flp3F64Translation.x, flp3F64Translation.y, flp3F64Translation.z, f64Score, f64Residual);

			// 추정된 포즈 행렬 및 score 출력
			if((eResult = layer3DDst.DrawText3D(tp3Center, strText, YELLOW, 0, 9)).IsFail())
			{
				ErrorPrint(eResult, L"Failed to draw text.\n");
				break;
			}
		}

		view3DSource.ZoomFit();
		view3DDst.ZoomFit();

		// 이미지 뷰를 갱신 합니다. // Update image view
		view3DSource.Invalidate(true);

		view3DDst.UpdateObject();
		view3DDst.UpdateScreen();
		view3DDst.Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림
		while(arrView3DLearn[0].IsAvailable() && arrView3DLearn[1].IsAvailable() && arrView3DLearn[2].IsAvailable() && view3DSource.IsAvailable() && view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}