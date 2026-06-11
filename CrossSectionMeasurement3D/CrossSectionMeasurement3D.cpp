#include <cstdio>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

static void AddSegment(CFL3DObject& floObject, const TPoint3<float>& tp3Start, const TPoint3<float>& tp3End, const TPoint3<uint8_t>& tp3Color)
{
	CFLArray<TPoint3<float>>* pFlaVertices = (CFLArray<TPoint3<float>>*)floObject.GetVertices();
	CFLArray<int32_t>* pFlaSegmentIndices = (CFLArray<int32_t>*)floObject.GetSegmentIndices();
	CFLArray<int32_t>* pFlaSegmentElementCount = (CFLArray<int32_t>*)floObject.GetSegmentElementCountInformation();
	CFLArray<TPoint3<uint8_t>>* pFlaSegmentColors = (CFLArray<TPoint3<uint8_t>>*)floObject.GetSegmentColors();

	int32_t i32VertexIndex = (int32_t)pFlaVertices->GetCount();
	pFlaVertices->PushBack(tp3Start);
	pFlaVertices->PushBack(tp3End);
	pFlaSegmentIndices->PushBack(i32VertexIndex);
	pFlaSegmentIndices->PushBack(i32VertexIndex + 1);
	pFlaSegmentElementCount->PushBack(2);
	pFlaSegmentColors->PushBack(tp3Color);
}

static TPoint3<uint8_t> ColorRefToPoint3(COLORREF color)
{
	return TPoint3<uint8_t>((uint8_t)GetRValue(color), (uint8_t)GetGValue(color), (uint8_t)GetBValue(color));
}

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	CFL3DObject floSourceObject;
	CFL3DObject floResultObject;

	CGUIView3DWrap view3D;

	do
	{
		CResult eResult = EResult_UnknownError;

		// 3D Object 로드 // Load the 3D object
		if((eResult = floSourceObject.Load(L"../../ExampleImages/SurfaceMatch3D/Car example.ply")).IsFail())
		{
			ErrorPrint(eResult, "Failed to load the object file.\n");
			break;
		}

		// 3D 뷰 생성 // Create the 3D view
		if((eResult = view3D.Create(0, 0, 1024, 768)).IsFail())
		{
			ErrorPrint(eResult, "Failed to create the 3D view.\n");
			break;
		}

		if((eResult = view3D.PushObject(floSourceObject)).IsFail())
		{
			ErrorPrint(eResult, "Failed to display the 3D object.\n");
			break;
		}

		// CrossSectionMeasurement3D 객체 생성 // Create CrossSectionMeasurement3D object
		CCrossSectionMeasurement3D crossSectionMeasurement3D;

		crossSectionMeasurement3D.SetSourceObject(floSourceObject);

		TPoint3<float> tp3CrossSectionCenter(-122.0f, -48.0f, -342.0f);
		TPoint3<float> tp3CrossSectionNormal(0.0f, 1.0f, 1.0f);
		crossSectionMeasurement3D.SetCrossSection(tp3CrossSectionCenter, tp3CrossSectionNormal);
		crossSectionMeasurement3D.SetMeasurementPlane(CCrossSectionMeasurement3D::EMeasurementDirectionFromSection_Vertical);
		crossSectionMeasurement3D.SetSectionMeasurementMode(CCrossSectionMeasurement3D::ESectionMeasurementMode_GlobalSpan);
		crossSectionMeasurement3D.SetMeasurementInterval(3.0f);
		crossSectionMeasurement3D.SetMinLength(0.1f);
		crossSectionMeasurement3D.SetMaxLength(1e+09f);
		crossSectionMeasurement3D.Execute();


		// 앞서 설정된 파라미터 대로 알고리즘 수행 // Execute algorithm according to previously set parameters
		if((eResult = crossSectionMeasurement3D.Execute()).IsFail())
		{
			ErrorPrint(eResult, L"Failed to execute Cross Section Measurement 3D.");
			break;
		}

		CFLArray<CFLArray<TPoint3<float>>> flaIntersectionLines;
		CFLArray<CFLArray<TPoint3<float>>> flaMeasurementPoints;
		CFLArray<CFLArray<float>> flaMeasurementDistances;

		crossSectionMeasurement3D.GetResultIntersectionLines(flaIntersectionLines);
		crossSectionMeasurement3D.GetResultMeasurementPoints(flaMeasurementPoints);
		crossSectionMeasurement3D.GetResultMeasurementDistances(flaMeasurementDistances);

		const TPoint3<uint8_t> tp3IntersectionColor = ColorRefToPoint3(LIGHTGREEN);
		const TPoint3<uint8_t> tp3MeasurementColor = ColorRefToPoint3(CYAN);

		for(int64_t i = 0; i < flaIntersectionLines.GetCount(); ++i)
		{
			const CFLArray<TPoint3<float>>& flaLine = flaIntersectionLines[i];

			for(int64_t j = 0; j + 1 < flaLine.GetCount(); ++j)
				AddSegment(floResultObject, flaLine[j], flaLine[j + 1], tp3IntersectionColor);
		}

		int64_t i64MeasurementCount = 0;
		for(int64_t i = 0; i < flaMeasurementPoints.GetCount(); ++i)
		{
			const CFLArray<TPoint3<float>>& flaPoints = flaMeasurementPoints[i];

			for(int64_t j = 0; j + 1 < flaPoints.GetCount(); j += 2)
			{
				AddSegment(floResultObject, flaPoints[j], flaPoints[j + 1], tp3MeasurementColor);
				++i64MeasurementCount;
			}
		}

		CGUIView3DObject viewResultObject(floResultObject);
		viewResultObject.SetTopologyType(ETopologyType3D_Segment);

		if((eResult = view3D.PushObject(viewResultObject)).IsFail())
		{
			ErrorPrint(eResult, "Failed to display the result object.\n");
			break;
		}

		CGUIView3DLayerWrap layer3D = view3D.GetLayer(0);
		layer3D.Clear();
		layer3D.DrawTextCanvas(CFLPoint<double>(0, 0), L"Cross Section Measurement 3D", YELLOW, BLACK, 20);
		layer3D.DrawTextCanvas(CFLPoint<double>(0, 30), CFLString<wchar_t>().Format(L"Intersection Lines : %lld\nMeasurement Planes : %lld\nMeasurements : %lld", flaIntersectionLines.GetCount(), flaMeasurementPoints.GetCount(), i64MeasurementCount), YELLOW, BLACK, 15);
		for(int64_t i = 0; i < flaMeasurementPoints.GetCount() && i < flaMeasurementDistances.GetCount(); ++i)
		{
			const CFLArray<TPoint3<float>>& flaPoints = flaMeasurementPoints[i];
			const CFLArray<float>& flaDistances = flaMeasurementDistances[i];

			for(int64_t j = 0, d = 0; j + 1 < flaPoints.GetCount() && d < flaDistances.GetCount(); j += 2, ++d)
			{
				const TPoint3<double> tp3Text((flaPoints[j].x + flaPoints[j + 1].x) * .5, (flaPoints[j].y + flaPoints[j + 1].y) * .5, (flaPoints[j].z + flaPoints[j + 1].z) * .5);
				layer3D.DrawText3D(tp3Text, CFLString<wchar_t>().Format(L"%.3f", flaDistances[d]), DEEPPINK, BLACK, 10);
			}
		}

		view3D.ZoomFit();
		view3D.Invalidate(true);

		while(view3D.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}
