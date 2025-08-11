#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 객체 선언 // Declare image object
	CFLImage fliImage;

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage[3];

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// Source Coordinate View 생성 // Create Source Coordinate View
		if(IsFail(res = viewImage[0].Create(200, 0, 700, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Destination Coordinate View 생성 // Create Destination Coordinate View
		if(IsFail(res = viewImage[1].Create(700, 0, 1200, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// Restore Coordinate View 생성 // Create Restore Coordinate View
		if(IsFail(res = viewImage[2].Create(1200, 0, 1700, 500)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// 각 이미지 뷰의 시점을 동기화 한다. // Synchronize the viewpoint of each image view.
		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[1])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}
		if(IsFail(res = viewImage[1].SynchronizePointOfView(&viewImage[2])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 각 이미지 뷰 윈도우의 위치를 동기화 한다 // Synchronize the position of each image view window
		if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[1])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}
		if(IsFail(res = viewImage[1].SynchronizeWindow(&viewImage[2])))
		{
			ErrorPrint(res, "Failed to synchronize window.\n");
			break;
		}

		// 화면상에 잘 보이도록 좌표 90배율을 적용 // Apply 90 magnification to the coordinates so that they can be seen clearly on the screen
		double f64Scale = 90;
		// 화면상에 잘 보이도록 시점 Offset 조정 // Adjust the viewpoint offset so that it can be seen clearly on the screen
		double f64CenterCoordX = 2.007;
		double f64CenterCoordY = 1.865;
		viewImage[0].SetViewCenterAndScale(CFLPoint<double>(f64CenterCoordX, f64CenterCoordY), f64Scale);

		// 화면에 출력하기 위해 Image View에서 레이어 0번을 얻어옴 // Obtain layer 0 number from image view for display
		// 이 객체는 이미지 뷰에 속해있기 때문에 따로 해제할 필요가 없음 // This object belongs to an image view and does not need to be released separately
		CGUIViewImageLayerWrap layer[3];

		for(int32_t i = 0; i < 3; ++i)
			layer[i] = viewImage[i].GetLayer(0);

		// 화면상 좌표(고정 좌표)에 Source 좌표 View 임을 표시
		// Indicate that the source coordinates are View at the coordinates (fixed coordinates) on the screen
		layer[0].DrawTextCanvas(&CFLPoint<int32_t>(0, 0), L"Source Coordinate", YELLOW, BLACK, 30);
		// 화면상 좌표(고정 좌표)에 Destination 좌표 View 임을 표시
		// Indicate that it is the destination coordinate view on the screen coordinates (fixed coordinates)
		layer[1].DrawTextCanvas(&CFLPoint<int32_t>(0, 0), L"Destination Coordinate", YELLOW, BLACK, 30);
		// 화면상 좌표(고정 좌표)에 Restore 좌표 View 임을 표시
		// Indicate Restore Coordinates View on the screen coordinates (fixed coordinates)
		layer[2].DrawTextCanvas(&CFLPoint<int32_t>(0, 0), L"Restore Coordinate (from Destination)", YELLOW, BLACK, 30);

		// 좌표 매핑용 클래스 선언 // Class declaration for coordinate mapping
		CBicubicSplineMapping bicubicSplineMapping;

		// 보간 방식은 데이터 설정 이후에도 아래 함수를 통해 변경 가능합니다.
		// The interpolation method can be changed through the function below even after setting the data.

		// 소스의 좌표 보간 형태를 Cubic Spline으로 설정 // Set the source coordinate interpolation type to Cubic Spline
		bicubicSplineMapping.SetSourceInterpolationMethod(ESourceInterpolationMethod_CubicSpline);
		// 대상의 좌표 보간 형태를 Cubic Spline으로 설정 // Set the coordinate interpolation type of the destination to Cubic Spline
		bicubicSplineMapping.SetDestinationInterpolationMethod(EDestinationInterpolationMethod_CubicSpline);

		// 만약 기존 저장된 매핑 데이터가 있다면 해당 데이터를 로드합니다. // If there is previously saved mapping data, load the data.
		// 두번째 실행부터는 파일이 생성될 것이기 때문에 아래 세팅과정을 수행하지 않고 지나가게 됩니다. // Since the file will be created from the second execution, the setting process below will be skipped.
		// 계속 새로 데이터를 생성하는것을 테스트 하려 한다면 아래 Load함수와 관련된 if문 1줄을 삭제하면 됩니다. // If you want to test continuously creating new data, you can delete one line of the if statement related to the Load function below.
		if(IsFail(res = bicubicSplineMapping.Load(L"MappingData.flbcs")))
		{
			// 그리드를 (5,5)로 초기화하면서, 가상 확장 영역을 3으로 지정
			// 확장영역 밖으로 값을 변환하려고 하면 값이 나오지 않기때문에 적절한 크기로 확장을 해야 하며,
			// 가능하면 정확한 매핑을 위해 큰 범위의 매핑을 하는것을 추천합니다.
			// Set the virtual extension to 3, initializing the grid to (5,5)
			// If you try to convert the value outside the extended range, the value does not come out,
			// so you need to expand it to an appropriate size, and if possible, it is recommended to map a large range for accurate mapping.
			const int32_t i32Extension = 3;
			CFLPoint<int32_t> flpGridSize(5, 5);
			bicubicSplineMapping.Initialize(flpGridSize, i32Extension);

			CFLPoint<int32_t> flpGridIndex;
			for(int y = 0; y < flpGridSize.y; ++y)
			{
				flpGridIndex.y = y;

				for(int x = 0; x < flpGridSize.x; ++x)
				{
					flpGridIndex.x = x;

					// Grid Index와 같은 좌표로 Source 좌표를 설정 // Set the source coordinates to the same coordinates as the Grid Index
					CFLPoint<double> flpSource(flpGridIndex.x, flpGridIndex.y);
					// Grid Index와 같은 좌표에서 미세한 랜덤 값을 부여해서 좌표를 왜곡 // Distort the coordinates by giving fine random values at the same coordinates as the Grid Index
					CFLPoint<double> flpDistortion((flpGridIndex.x + CRandomGenerator::Double(-.1, .1)), (flpGridIndex.y + CRandomGenerator::Double(-.1, .1)));

					// 위에서 설정한 좌표들을 바탕으로 BicubicSplineMapping 클래스에 하나의 Vertex를 설정
					// Set one vertex in the BicubicSplineMapping class based on the coordinates set above
					bicubicSplineMapping.SetControlPoint(flpGridIndex, flpSource, flpDistortion);
				}
			}

			// 설정한 데이터를 매핑 가능하도록 마무리 작업을 진행합니다.
			// 반드시 이 함수를 호출해서 결과가 OK가 나와야 매핑 사용이 가능합니다.
			// We proceed with the finishing work so that the set data can be mapped.
			// You must call this function and the result must be OK to use the mapping.
			if(IsFail(res = bicubicSplineMapping.Finish()))
			{
				ErrorPrint(res, "Failed to finalize\n");
				break;
			}

			// Finalize 까지 완료된 상태라면 Save를 통해 파일에 저장이 가능하며,
			// 추후 Load함수를 통해 로드 시 위의 Initialize -> Set -> Finalize 과정을 생략할 수 있습니다.
			// If Finalize is completed, it can be saved to a file through Save.
			// When loading through the Load function later, the above Initialize -> Set -> Finalize process can be omitted.
			if(IsFail(res = bicubicSplineMapping.Save(L"MappingData.flbcs")))
			{
				ErrorPrint(res, "Failed to save mapping data\n");
				break;
			}
		}

		// 세팅이 완료된 BicubicSplineMapping 클래스를 이용해 변환을 하는 단계입니다.
		// This step is to convert using the BicubicSplineMapping class that has been set.

		// BicubicSplineMapping 클래스에 설정된 Vertex 정보를 화면에 Display
		// Display the vertex information set in the BicubicSplineMapping class on the screen
		for(int32_t y = 0; y < bicubicSplineMapping.GetRow(); ++y)
		{
			for(int32_t x = 0; x < bicubicSplineMapping.GetColumn(); ++x)
			{
				const CBicubicSplineMapping::CBicubicSplineMappingVertexInfo* pVertex = bicubicSplineMapping.GetControlPoint(CFLPoint<int32_t>(x, y));

				// 유효한 좌표가 아닌 경우 nullptr이 리턴된다. // If not valid coordinates, nullptr is returned.
				if(pVertex)
				{
					CFLPoint<double> flpSource(pVertex->tpSource.x, pVertex->tpSource.y);
					CFLPoint<double> flpDestination(pVertex->tpDestination.x, pVertex->tpDestination.y);

					for(int32_t i = 0; i < 3; ++i)
					{
						// Destination Vertex를 각 View Layer에 Drawing // Drawing the destination vertex to each view layer
						if(IsFail(res = layer[i].DrawFigureImage(&flpDestination, BLUE, 3)))
						{
							ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
							break;
						}

						// Source Vertex를 각 View Layer에 Drawing // Drawing source vertex to each view layer
						if(IsFail(res = layer[i].DrawFigureImage(&flpSource, RED, 3)))
						{
							ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
							break;
						}
					}

					printf("Source Vertex (%d,%d) : (%.03lf,%.03lf)\n", x, y, pVertex->tpSource.x, pVertex->tpSource.y);
					printf("Destination Vertex (%d,%d) : (%.03lf,%.03lf)\n", x, y, pVertex->tpDestination.x, pVertex->tpDestination.y);
				}
			}
		}


		// 정점 사이를 대략 10등분, 즉 하나의 영역에 100개의 보간영역을 테스트
		// Divide the vertices into approximately 10 equal parts, that is, test 100 interpolation areas in one area
		double f64Slice = 10;

		CFLPoint<double> flpSource; // Source 좌표 // Source coordinates
		CFLPoint<double> flpDestination; // Destination 좌표 // Destination coordinates
		CFLPoint<double> flpConvertedSource; // Destination 좌표를 다시 Source로 변환, 검증 용도의 좌표 // Convert destination coordinates back to source, coordinates for verification purposes

		for(int y = 0; y <= (bicubicSplineMapping.GetRow() - 1) * f64Slice; ++y)
		{
			flpSource.y = (y / f64Slice);

			for(int x = 0; x <= (bicubicSplineMapping.GetColumn() - 1) * f64Slice; ++x)
			{
				flpSource.x = (x / f64Slice);

				// 연산을 수행할 Source 좌표를 View에 Display // Display the source coordinates to perform the operation on the View
				if(IsFail(res = layer[0].DrawFigureImage(&flpSource, YELLOW)))
				{
					ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
					break;
				}

				// Source 좌표의 공간을 Destination 좌표 공간으로 변환 // Convert the space of source coordinates to destination coordinate space
				if(IsOK(bicubicSplineMapping.ConvertSourceToDestination(flpSource, flpDestination)))
				{
					// Source 좌표에서 Destination 좌표로 변환된 좌표를 View에 Display // Display coordinates converted from source coordinates to destination coordinates on the View
					if(IsFail(res = layer[1].DrawFigureImage(&flpDestination, LIME)))
					{
						ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
						break;
					}

					// 변환된 Destination 좌표를 그대로 Source 좌표로 변환해서 자신의 위치로 제대로 돌아오는지 검증
					// Verify that the converted destination coordinates are converted to source coordinates as they are and return to their own position properly
					if(IsOK(bicubicSplineMapping.ConvertDestinationToSource(flpDestination, flpConvertedSource)))
					{
						printf("Source (%.03lf,%.03lf) -> Destination (%.03lf,%.03lf) -> Source (%.03lf,%.03lf)\n", flpSource.x, flpSource.y, flpDestination.x, flpDestination.y, flpConvertedSource.x, flpConvertedSource.y);

						// 변환된 좌표를 View에 Display // Display the converted coordinates in the View
						if(IsFail(res = layer[2].DrawFigureImage(&flpConvertedSource, CYAN)))
						{
							ErrorPrint(res, "Failed to draw figure objects on the image view.\n");
							break;
						}
					}
				}
			}
		}

		// 이미지 뷰들을 갱신 합니다. // Update the image views.
		for(int32_t i = 0; i < 3; ++i)
			viewImage[i].Invalidate(true);


		// 이미지 뷰가 셋중에 하나라도 꺼지면 종료로 간주 // Consider closed when any of the three image views are turned off
		while(viewImage[0].IsAvailable() && viewImage[1].IsAvailable() && viewImage[2].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}