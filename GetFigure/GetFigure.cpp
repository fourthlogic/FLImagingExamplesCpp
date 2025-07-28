#include <cstdio>

#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 이미지 뷰 선언 // Declare image view
	CGUIViewImageWrap viewImage[4];

	// 수행 결과 객체 선언 // Declare the execution result object
	CResult res;

	do
	{
		// 이미지 뷰 생성 // Create image view
		if(IsFail(res = viewImage[0].Create(400, 0, 812, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[1].Create(812, 0, 1224, 384)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[2].Create(400, 384, 812, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		if(IsFail(res = viewImage[3].Create(812, 384, 1224, 768)))
		{
			ErrorPrint(res, "Failed to create the image view.\n");
			break;
		}

		// SourceView, DstView 의 0번 레이어 가져오기 // Get Layer 0 of SourceView, DstView
		CGUIViewImageLayerWrap layerSrc1 = viewImage[0].GetLayer(0);
		CGUIViewImageLayerWrap layerDst1 = viewImage[1].GetLayer(0);
		CGUIViewImageLayerWrap layerSrc2 = viewImage[2].GetLayer(0);
		CGUIViewImageLayerWrap layerDst2 = viewImage[3].GetLayer(0);

		layerSrc1.DrawTextCanvas(TPoint<double>(0, 0), L"Source Figure 1", YELLOW, BLACK, 15);
		layerSrc2.DrawTextCanvas(TPoint<double>(0, 0), L"Source Figure 2", YELLOW, BLACK, 15);
		layerDst1.DrawTextCanvas(TPoint<double>(0, 0), L"Result Figure 1", YELLOW, BLACK, 15);
		layerDst2.DrawTextCanvas(TPoint<double>(0, 0), L"Result Figure 2", YELLOW, BLACK, 15);

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImage[0].SynchronizePointOfView(&viewImage[1])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰의 시점을 동기화 한다 // Synchronize the viewpoints of the two image views
		if(IsFail(res = viewImage[2].SynchronizePointOfView(&viewImage[3])))
		{
			ErrorPrint(res, "Failed to synchronize view\n");
			break;
		}

		// 두 이미지 뷰 윈도우의 위치를 맞춤 // Synchronize the positions of the two image view windows
		for(int32_t i = 1; i < 4; ++i)
		{
			if(IsFail(res = viewImage[0].SynchronizeWindow(&viewImage[i])))
			{
				ErrorPrint(res, "Failed to synchronize window.\n");
				break;
			}
		}

		CFLFigureArray flfaSource1;
		CFLFigureArray flfaSource2;

		// Source Figure 불러오기 // Load source figure
		if(IsFail(res = flfaSource1.Load(L"../../ExampleImages/Figure/various shapes.fig")))
		{
			ErrorPrint(res, "Failed to load the figure file.\n");
			break;
		}

		// Source Figure 불러오기 // Load source figure
		if(IsFail(res = flfaSource2.Load(L"../../ExampleImages/Figure/Circles.fig")))
		{
			ErrorPrint(res, "Failed to load the figure file.\n");
			break;
		}

		// 지원되는 연산자 : [, ] , { , }, (, ), +, -, *, / , ^, <, >, <= , =<, >= , =>, != , =!, =, == , &, &&, and, | , || , or
		// 
		// 	연산자 설명
		// 	2 - 1. 괄호 연산자 : [, { , (,] , }, )
		// 	2 - 2. 덧셈 연산자 : +
		// 	2 - 3. 뺄셈 연산자 : -
		// 	2 - 4. 곱셈 연산자 : *
		// 	2 - 5. 나눗셈 연산자 : /
		// 	2 - 6. 거듭제곱 연산자 : ^
		// 	2 - 7. 관계 연산자 : <, >, <= , =<, >= , =>, != , =!, =, ==
		// 	2 - 8. 논리 곱 : &, &&, and
		// 	2 - 9. 논리 합 : | , || , or
		// 
		// 	같은 역할을 하는 연산자
		// 	3 - 1.[, { , (
		// 	3 - 2.], }, )
		// 	3 - 3. <= , = <
		// 	3 - 3. >= , =>
		// 	3 - 4. != , =!
		// 	3 - 5. =, ==
		// 	3 - 6. &, &&, and
		// 	3 - 7. | , || , or
		// 
		// 지원되는 Figure 조건식
		//  넓이                                  : Area
		//	둘레                                  : Perimeter
		//	길이                                  : Length
		//	폭                                    : Width
		//	높이                                  : Height
		//	각도                                  : Angle
		//	중심의 x 좌표                         : Center.x
		//	중심의 y 좌표                         : Center.y
		//	무게중심의 x 좌표                     : CenterOfGravity.x
		//	무게중심의 y 좌표                     : CenterOfGravity.y
		//	BoundaryRect의 left                   : BoundaryRect.left 혹은 left
		//	BoundaryRect의 top                    : BoundaryRect.top 혹은 top
		//	BoundaryRect의 right                  : BoundaryRect.right 혹은 right
		//	BoundaryRect의 bottom                 : BoundaryRect.bottom 혹은 bottom
		//	최소 둘레 직사각형의 넓이             : MinimumEnclosingRectangleArea 혹은 MerArea
		//	최소 둘레 직사각형의 둘레             : MinimumEnclosingRectanglePerimeter 혹은 MerPerimeter
		//	최소 둘레 직사각형의 폭               : MinimumEnclosingRectangleWidth 혹은 MerWidth
		//	최소 둘레 직사각형의 높이             : MinimumEnclosingRectangleHeight 혹은 MerHeight
		//	최소 둘레 직사각형의 장변             : MinimumEnclosingRectangleLongSideLength 혹은 MerLongSideLength
		//	최소 둘레 직사각형의 단변             : MinimumEnclosingRectangleShortSideLength 혹은 MerShortSideLength
		//	최소 둘레 직사각형의 단변/장변 비율   : MinimumEnclosingRectangleShortLongRatio 혹은 MerShortLongRatio
		//	최소 둘레 직사각형의 각도             : MinimumEnclosingRectangleAngle 혹은 MerAngle
		//	최소 둘레 직사각형의 중심의 x 좌표    : MinimumEnclosingRectangleCenter.x 혹은 MerCenter.x
		//	최소 둘레 직사각형의 중심의 y 좌표    : MinimumEnclosingRectangleCenter.y 혹은 MerCenter.y
		//	최소 둘레 직사각형의 point0의 x 좌표  : MinimumEnclosingRectangle.point0.x 혹은 MER.point0.x
		//	최소 둘레 직사각형의 point0의 y 좌표  : MinimumEnclosingRectangle.point0.y 혹은 MER.point0.y
		//	최소 둘레 직사각형의 point1의 x 좌표  : MinimumEnclosingRectangle.point1.x 혹은 MER.point1.x
		//	최소 둘레 직사각형의 point1의 y 좌표  : MinimumEnclosingRectangle.point1.y 혹은 MER.point1.y
		//	최소 둘레 직사각형의 point2의 x 좌표  : MinimumEnclosingRectangle.point2.x 혹은 MER.point2.x
		//	최소 둘레 직사각형의 point2의 y 좌표  : MinimumEnclosingRectangle.point2.y 혹은 MER.point2.y
		//	최소 둘레 직사각형의 point3의 x 좌표  : MinimumEnclosingRectangle.point3.x 혹은 MER.point3.x
		//	최소 둘레 직사각형의 point3의 y 좌표  : MinimumEnclosingRectangle.point3.y 혹은 MER.point3.y
		//	내부의 모든 정점의 개수               : VertexCount
		//  도형 타입                             : Type
		//  이름                                  : Name
		//  인덱스                                : Index
		//										  
		// 지원되는 함수						    
		//  최대값                                : Max 혹은 Maximum
		//	최소값                                : Min 혹은 Minimum
		//	평균값                                : Mean 혹은 Average 혹은 Avg
		//	중간값                                : Median 혹은 Med
		//	합                                    : Sum
		//	제곱합                                : Sqrsum 혹은 Sumsqr
		//	제곱근합                              : Sqrtsum 혹은 Sumsqrt
		//	분산                                  : Variance 혹은 Var
		//	표준편차                              : Stdev 혹은 Stddev

		// Supported Operators : [, ] , { , }, (, ), +, -, *, / , ^, <, >, <= , =<, >= , =>, != , =!, =, == , &, &&, and, | , || , or
		// 
		// 	Operator description
		// 	2 - 1. parenthesis operator : [, { , (,] , }, )
		// 	2 - 2. addition operator : +
		// 	2 - 3. subtraction operator : -
		// 	2 - 4. multiplication operator : *
		// 	2 - 5. division operator : /
		// 	2 - 6. power operator : ^
		// 	2 - 7. relational operator : <, >, <= , =<, >= , =>, != , =!, =, ==
		// 	2 - 8. logical product : &, &&, and
		// 	2 - 9. logical sum : | , || , or
		// 
		// 	Operators that do the same thing
		// 	3 - 1.[, { , (
		// 	3 - 2.], }, )
		// 	3 - 3. <= , = <
		// 	3 - 3. >= , =>
		// 	3 - 4. != , =!
		// 	3 - 5. =, ==
		// 	3 - 6. &, &&, and
		// 	3 - 7. | , || , or
		// 
		//  Supported figure conditional expressions
		//  area                                                   : Area
		//	perimeter                                              : Perimeter
		//	length                                                 : Length
		//	width                                                  : Width
		//	height                                                 : Height
		//	angle                                                  : Angle
		//	x-coordinate of center                                 : Center.x
		//	y-coordinate of center                                 : Center.y
		//	x-coordinate of the center of gravity                  : CenterOfGravity.x
		//	y-coordinate of the center of gravity                  : CenterOfGravity.y
		//	left of BoundaryRect                                   : BoundaryRect.left or left
		//	top of BoundaryRect                                    : BoundaryRect.top or top
		//	right of BoundaryRect                                  : BoundaryRect.right or right
		//	bottom of BoundaryRect                                 : BoundaryRect.bottom or bottom
		//	Area of minimum enclosing rectangle                    : MinimumEnclosingRectangleArea or MerArea
		//	perimeter of minimum enclosing rectangle               : MinimumEnclosingRectanglePerimeter or MerPerimeter
		//	width of minimum enclosing rectangle                   : MinimumEnclosingRectangleWidth or MerWidth
		//	height of minimum enclosing rectangle                  : MinimumEnclosingRectangleHeight or MerHeight
		//	long side length of minimum enclosing rectangle        : MinimumEnclosingRectangleLongSideLength or MerLongSideLength
		//	short side length of minimum enclosing rectangle       : MinimumEnclosingRectangleShortSideLength or MerShortSideLength
		//	short/long ratio of minimum enclosing rectangle        : MinimumEnclosingRectangleShortLongRatio or MerShortLongRatio
		//	angle of minimum enclosing rectangle                   : MinimumEnclosingRectangleAngle or MerAngle
		//	x-coordinate of center of minimum enclosing rectangle  : MinimumEnclosingRectangleCenter.x or MerCenter.x
		//	y-coordinate of center of minimum enclosing rectangle  : MinimumEnclosingRectangleCenter.y or MerCenter.y
		//	x-coordinate of point0 of minimum enclosing rectangle  : MinimumEnclosingRectangle.point0.x or MER.point0.x
		//	y-coordinate of point0 of minimum enclosing rectangle  : MinimumEnclosingRectangle.point0.y or MER.point0.y
		//	x-coordinate of point1 of minimum enclosing rectangle  : MinimumEnclosingRectangle.point1.x or MER.point1.x
		//	y-coordinate of point1 of minimum enclosing rectangle  : MinimumEnclosingRectangle.point1.y or MER.point1.y
		//	x-coordinate of point2 of minimum enclosing rectangle  : MinimumEnclosingRectangle.point2.x or MER.point2.x
		//	y-coordinate of point2 of minimum enclosing rectangle  : MinimumEnclosingRectangle.point2.y or MER.point2.y
		//	x-coordinate of point3 of minimum enclosing rectangle  : MinimumEnclosingRectangle.point3.x or MER.point3.x
		//	y-coordinate of point3 of minimum enclosing rectangle  : MinimumEnclosingRectangle.point3.y or MER.point3.y
		//	the number of all vertices in the interior             : VertexCount
		//  type of figure                                         : Type
		//  name                                                   : Name
		//  index                                                  : Index
		//														   
		//  Supported functions									   
		//  maximum value                                          : Max or Maximum
		//	minimum value                                          : Min or Minimum
		//	mean value                                             : Mean or Average or Avg
		//	median value                                           : Median or Med
		//	sum                                                    : Sum
		//	sum of squares                                         : Sqrsum or Sumsqr
		//	sum of square roots                                    : Sqrtsum or Sumsqrt
		//	variance                                               : Variance or Var
		//	standard deviation                                     : Stdev or Stddev


		// 조건식 문자열 // Condition string
		CFLString<wchar_t> strExpression1;
		strExpression1.Format(L"area > 400 and center.y < 160 or vertexcount = 3");

		// 조건식을 View에 표기 // Draw the conditional expression in the View
		layerDst1.DrawTextCanvas(TPoint<double>(0, 20), strExpression1, YELLOW, BLACK, 13);

		// 조건식을 만족하는 Figure를 flfaResult1에 추출 // Extract the figure that satisfies the conditional expression to flfaResult1
		CFLFigureArray flfaResult1;

		if(IsFail(res = flfaSource1.GetFigure(strExpression1, &flfaResult1)))
		{
			ErrorPrint(res, "Failed to process.\n");
			break;
		}

		// 조건식 문자열 // Condition string
		CFLString<wchar_t> strExpression2;
		strExpression2.Format(L"area >= mean('area')");

		// 조건식을 View에 표기 // Draw the conditional expression in the View
		layerDst2.DrawTextCanvas(TPoint<double>(0, 20), strExpression2, YELLOW, BLACK, 13);

		// 조건식을 만족하는 Figure를 flfaResult2에 추출 // Get the figure that satisfies the conditional expression to flfaResult2
		CFLFigureArray flfaResult2;

		if(IsFail(res = flfaSource2.GetFigure(strExpression2, &flfaResult2)))
		{
			ErrorPrint(res, "Failed to process.\n");
			break;
		}

		// SourceView의 0번 레이어에 Source Figure 그리기 // Draw the Source Figure on Layer 0 of the SourceView
		layerSrc1.DrawFigureImage(flfaSource1, CYAN);
		layerSrc2.DrawFigureImage(flfaSource2, CYAN);

		// DstView1의 0번 레이어에 결과 그리기 // Draw the result on layer 0 of DstView1
		layerDst1.DrawFigureImage(flfaSource1, CYAN);
		layerDst1.DrawFigureImage(flfaResult1, LIME, 3, LIME, EGUIViewImagePenStyle_Solid, 1, 0.2f);

		// DstView2의 0번 레이어에 결과 그리기 // Draw the result on layer 0 of DstView2
		layerDst2.DrawFigureImage(flfaSource2, CYAN);
		layerDst2.DrawFigureImage(flfaResult2, LIME, 3, LIME, EGUIViewImagePenStyle_Solid, 1, 0.2f);

		// Console 출력 // Console output
		wprintf(L"Source1 Figure Array\n");
		wprintf(L"%s\n\n", CFigureUtilities::ConvertFigureObjectToString(&flfaSource1).GetString());

		wprintf(L"Result1 Figure that area > 400 and center.y < 160 or vertexcount = 3\n");
		wprintf(L"%s\n\n", CFigureUtilities::ConvertFigureObjectToString(&flfaResult1).GetString());

		wprintf(L"Source2 Figure Array\n");
		wprintf(L"%s\n\n", CFigureUtilities::ConvertFigureObjectToString(&flfaSource2).GetString());

		wprintf(L"Result2 Figure that area >= mean('area')\n");
		wprintf(L"%s\n\n", CFigureUtilities::ConvertFigureObjectToString(&flfaResult2).GetString());

		// 이미지 뷰를 갱신 합니다. // Update image view
		for(int32_t i = 0; i < 4; ++i)
			viewImage[i].Invalidate(true);

		// 이미지 뷰가 종료될 때 까지 기다림 // Wait for the image view to close
		while(viewImage[0].IsAvailable() && viewImage[1].IsAvailable() && viewImage[2].IsAvailable() && viewImage[3].IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}