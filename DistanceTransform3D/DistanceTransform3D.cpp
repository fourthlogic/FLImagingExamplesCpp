#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

const CResult DrawResult(GUI::CGUIView3DWrap* pView3D, const Base::CFLArray<Base::TPoint3<float>>* pFlaPlyData, Base::CFLArray<Base::TPoint3<float>>& arrResult, Base::CFLString<wchar_t> strDirection)
{
	CResult er;

	do
	{
		float arr2F32DataRange[3][2] =
		{
			{ std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity() },
			{ std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity() },
			{ std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity() },
		};

		for(int32_t i = 0; i < (int32_t)arrResult.GetCount(); ++i)
		{
			float* pF32Value = (float*)&arrResult[i];

			for(int32_t j = 0; j < 3; ++j)
			{
				arr2F32DataRange[j][0] = std::min<float>(arr2F32DataRange[j][0], *(pF32Value + j));
				arr2F32DataRange[j][1] = std::max<float>(arr2F32DataRange[j][1], *(pF32Value + j));
			}
		}

		if(!pFlaPlyData)
		{
			er = EResult_NoData;
			break;
		}

		if(pFlaPlyData->GetCount() != arrResult.GetCount())
		{
			er = EResult_NoResult;
			break;
		}

		CFLString<wchar_t> strRangeX = CFLString<wchar_t>().Format(L"X : [%.4f, %.4f]", arr2F32DataRange[0][0], arr2F32DataRange[0][1]);
		CFLString<wchar_t> strRangeY = CFLString<wchar_t>().Format(L"Y : [%.4f, %.4f]", arr2F32DataRange[1][0], arr2F32DataRange[1][1]);
		CFLString<wchar_t> strRangeZ = CFLString<wchar_t>().Format(L"Z : [%.4f, %.4f]", arr2F32DataRange[2][0], arr2F32DataRange[2][1]);

		pView3D->GetLayer(0).DrawTextCanvas(CFLPoint<double>(10, 20), L"Data Ranges", RGB(255, 255, 128), BLACK, 13);
		pView3D->GetLayer(0).DrawTextCanvas(CFLPoint<double>(10, 35), strRangeX, RGB(255, 128, 128), BLACK, 13);
		pView3D->GetLayer(0).DrawTextCanvas(CFLPoint<double>(10, 50), strRangeY, RGB(128, 255, 128), BLACK, 13);
		pView3D->GetLayer(0).DrawTextCanvas(CFLPoint<double>(10, 65), strRangeZ, RGB(128, 128, 255), BLACK, 13);

		int32_t i32SelectedAxis;

		if(strDirection == L"Delta X")
			i32SelectedAxis = 0;
		else if(strDirection == L"Delta Y")
			i32SelectedAxis = 1;
		else // dZ
			i32SelectedAxis = 2;

		auto lGetColor = [](float f32Intensity, uint8_t arrF32Color[3]) -> void
			{
				float f32Temp;
				float f32Segment = (1.f / 6.f);

				for(int32_t i = 0; i < 3; ++i)
				{
					float f32Value;
					f32Value = (f32Intensity - (i * 2 - 1) * f32Segment) / f32Segment;
					f32Value = f32Value > 1 ? 1 : f32Value;
					f32Value = f32Value < 0 ? 0 : f32Value;
					f32Temp = (f32Intensity - (i * 2 + 1) * f32Segment) / f32Segment;
					f32Temp = f32Temp > 1 ? 1 : f32Temp;
					f32Temp = f32Temp < 0 ? 0 : f32Temp;
					f32Value -= f32Temp;
					arrF32Color[i] = uint8_t(f32Value * 255);
				}
			};


		CFLArray<TPoint3<uint8_t> > flaColors;
		flaColors.Reserve(arrResult.GetCount());

		for(int64_t i = 0; i < arrResult.GetCount(); ++i)
		{
			float f32Intensity;
			TPoint3<uint8_t> tpColor;

			f32Intensity = (*((float*)&arrResult[i] + i32SelectedAxis) - arr2F32DataRange[i32SelectedAxis][0]) / (arr2F32DataRange[i32SelectedAxis][1] - arr2F32DataRange[i32SelectedAxis][0]);

			if(f32Intensity < 0) f32Intensity = 0;
			if(f32Intensity > 1) f32Intensity = 1;

			lGetColor(f32Intensity, (uint8_t*)&tpColor);
			flaColors.PushBack(tpColor);
		}

		pView3D->PushObject(CFL3DObject(*pFlaPlyData, flaColors));

		er = EResult_OK;
	}
	while(false);

	return er;
}

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 뷰 선언 // Declare image view
	CGUIView3DWrap view3DSrc;
	CGUIView3DWrap view3DDst;
	CFL3DObject fl3DObject;

	do
	{
		// 알고리즘 동작 결과 // Algorithm execution result
		CResult res = EResult_UnknownError;

		// Source 3D 뷰 생성 // Create the Source 3D view
		if((res = view3DSrc.Create(100, 0, 600, 500)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		// Destination 3D 뷰 생성 // Create the destination 3D view
		if((res = view3DDst.Create(600, 0, 1100, 500)).IsFail())
		{
			ErrorPrint(res, L"Failed to create the image view.\n");
			break;
		}

		fl3DObject.Load(L"../../ExampleImages/DistanceTransform3D/binary-vertex.ply");

		// Distance Transform 3D 객체 생성 // Create Distance Transform 3D object
		CDistanceTransform3D distanceTransform;

		TPoint3<float> tpPosition(0.0f, 0.0f, 0.0f);
		TPoint3<float> tpDirection(-0.1f, 0.0f, -1.0f);
		TPoint3<float> tpUpVector(0.0f, 1.0f, 0.0f);

		// Source 객체 설정 // Set the source object
		distanceTransform.SetSourceObject(&fl3DObject);
		// 카메라 위치 설정 // Set the camera position
		distanceTransform.SetPosition(tpPosition);
		// 카메라 방향 설정 // Set the camera direction
		distanceTransform.SetDirection(tpDirection);
		// 카메라 업 벡터 설정 // Set the camera up vector
		distanceTransform.SetUpVector(tpUpVector);

		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((res = distanceTransform.Execute()).IsFail())
		{
			ErrorPrint(res, L"Failed to execute.");
			break;
		}

		CFLArray<TPoint3<float>> arrResult;
		// 거리 결과 가져오기 // Get the distance
		distanceTransform.GetResultDistanceAxis(arrResult);

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIView3DLayerWrap layer3DSrc = view3DSrc.GetLayer(0);
		CGUIView3DLayerWrap layer3DDst = view3DDst.GetLayer(0);

		// 기존에 Layer에 그려진 도형들을 삭제 // Clear the figures drawn on the existing layer
		layer3DSrc.Clear();
		layer3DDst.Clear();

		// 거리 결과를 그려준다 // Draw distance result
		DrawResult(&view3DDst, fl3DObject.GetVertices(), arrResult, L"Delta Z");

		// Destination 이미지가 새로 생성됨으로 Zoom fit 을 통해 디스플레이 되는 이미지 배율을 화면에 맞춰준다. // With the newly created Destination image, the image magnification displayed through Zoom fit is adjusted to the screen.
		if((res = view3DDst.ZoomFit()).IsFail())
		{
			ErrorPrint(res, L"Failed to zoom fit of the image view.\n");
			break;
		}

		view3DSrc.PushObject(fl3DObject);
		view3DSrc.ZoomFit();

		CFLPoint<double> flpTopLeft(0, 0);

		if((res = layer3DSrc.DrawTextCanvas(flpTopLeft, L"Source Object", YELLOW, BLACK, 20)).IsFail() ||
		   (res = layer3DDst.DrawTextCanvas(flpTopLeft, L"Destination Object", YELLOW, BLACK, 20)).IsFail())
		{
			ErrorPrint(res, L"Failed to draw text.\n");
			break;
		}

		// 이미지 뷰를 갱신 합니다. // Update image view
		view3DSrc.Invalidate(true);
		view3DDst.Invalidate(true);

		// 이미지 뷰, 3D 뷰가 종료될 때 까지 기다림
		while(view3DSrc.IsAvailable() || view3DDst.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}