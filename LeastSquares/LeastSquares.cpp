#include <cstdio>
#include <locale.h>
#include <time.h>

#include <FLImaging.h>


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	char* astrEquation[] = {
		"Linear equation",
		"Quadratic equation",
		"Cubic equation",
		"Quartic equation",
		"Quintic equation"
	};

	while(true)
	{
		double* pF64DataX = nullptr;
		double* pF64DataY = nullptr;

		do 
		{
			char arrInput[4096] = { 0, };

			printf("Please input generate sample data count: ");

			// 계수 문자열을 입력 받는다. // Receive the count string.
			fgets(arrInput, 4096, stdin);

			CFLString<char> flstrInput(arrInput);
			if(flstrInput.IsEmpty())
			{
				printf("Please check the input.\n");
				break;
			}

			int32_t i32DataCount = flstrInput.ToInt32();
			if(i32DataCount <= 0)
			{
				printf("Please check the input.\n");
				break;
			}

			// 입력 받은 개수만큼 데이터를 생성한다. // Generate data as many as the input number.
			pF64DataX = new double[i32DataCount];
			pF64DataY = new double[i32DataCount];

			if(!pF64DataX || !pF64DataY)
				break;

			srand((unsigned int)time(nullptr));

			CFLString<char> flstrSampleData = "";

			double f64PrevX = 0.;
			double f64PrevY = 0.;

			for(int32_t i = 0; i < i32DataCount; ++i)
			{
				if(!flstrSampleData.IsEmpty())
					flstrSampleData += ", ";

				pF64DataX[i] = f64PrevX + ((rand() % i32DataCount) / 10.);
				if(rand() % 2)
					pF64DataY[i] = f64PrevY + ((rand() % i32DataCount) / 10.);
				else
					pF64DataY[i] = f64PrevY - ((rand() % i32DataCount) / 10.);

				f64PrevX = pF64DataX[i];
				f64PrevY = pF64DataY[i];

				CFLString<char> flstrFormat = "";
				flstrFormat.Format("(%.1lf, %.1lf)", pF64DataX[i], pF64DataY[i]);

				flstrSampleData += flstrFormat;
			}

			printf("Sample Data\n");
			printf(flstrSampleData);
			printf("\n\n");

			// LeastSquares<double> 객체 생성 // Create LeastSquares<double> object
			CLeastSquares<double> leastSqaures;
			// 데이터를 할당 // Assign data
			leastSqaures.Assign(pF64DataX, pF64DataY, i32DataCount);

			for(int32_t i = 1; i <= 5; ++i)
			{
				// 계수 값을 받기 위해 FLArray 생성 // Create FLArray to receive coefficient values
				CFLArray<double> vctOutput;
				// R square 값을 받기 위해 double 선언 // Declare double to receive R square value
				double f64TRSqr = 0.;

				// 다항식 계수를 얻는다. // Get polynomial coefficients
				leastSqaures.GetPoly(i, &vctOutput, &f64TRSqr);

				CFLString<char> flstrEquation = "";

				int64_t i64Count = vctOutput.GetCount();
				if(!i64Count)
					continue;

				CFLArray<std::complex<double>> flaCoef;

				// 얻어온 계수로 다항식을 만든다. // Create a polynomial with the obtained coefficients.
				for(int64_t j = 0; j < i64Count; ++j)
				{
					double f64Coef = vctOutput.GetAt(j);
					flaCoef.PushBack(f64Coef);
					if(f64Coef == 0.)
						continue;

					if(!flstrEquation.IsEmpty() && f64Coef > 0.)
						flstrEquation += " + ";

					CFLString<char> flstrFormat = "";
					if(j == (i64Count - 2))
					{
						flstrFormat.Format("%lf*x", f64Coef);
					}
					else if(j == (i64Count - 1))
					{
						flstrFormat.Format("%lf", f64Coef);
					}
					else
					{
						flstrFormat.Format("%lf*x^%d", f64Coef, i64Count - 1 - j);
					}

					flstrEquation += flstrFormat;
				}

				if(flstrEquation.IsEmpty())
					continue;

				flstrEquation += "\n";

				CFLString<char> flstrDegree = astrEquation[i-1];
				flstrDegree += "\n";

				CFLString<char> flstrR = "";
				flstrR.Format("R square value: %lf\n", f64TRSqr);

				printf(flstrDegree);
				printf(flstrR);
				printf(flstrEquation);

				// 방정식의 해를 얻기위해 CFLArray<std::complex<double>> 생성 // Create CFLArray<std::complex<double>> to get solution of equation
				CFLArray<std::complex<double>> flaEquationResult;

				// 방정식의 해를 얻어온다. // Get the solution of the equation.
				CEquation::Solve(flaCoef, &flaEquationResult);

				// 방정식의 해를 표시한다. // Display the solution of the equation.
				CFLString<char> flstrResult = "Result \n";

				for(int64_t i = 0; i < flaEquationResult.GetCount(); ++i)
				{
					std::complex<double> cp = flaEquationResult.GetAt(i);
					CFLString<char> flstrCP = "";
					if(cp.imag() == 0.)
						flstrCP.Format("%lf", cp.real());
					else if(cp.imag() > 0.)
						flstrCP.Format("%.15lf+%.15lfi", cp.real(), cp.imag());
					else
						flstrCP.Format("%.15lf%.15lfi", cp.real(), cp.imag());

					flstrResult += flstrCP + "\n";
				}

				flstrResult += "\n\n";

				printf(flstrResult);
			}
		}
		while(false);

		if(pF64DataX)
		{
			delete [] pF64DataX;
			pF64DataX = nullptr;
		}

		if(pF64DataY)
		{
			delete [] pF64DataY;
			pF64DataY = nullptr;
		}
	}

	return 0;
}