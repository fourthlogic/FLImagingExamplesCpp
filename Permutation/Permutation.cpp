#include <cstdio>

#include <FLImaging.h>


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 순열 객체 선언 // Declare permutation object
	CPermutation permutation;

	while(true)
	{
		CFLString<char> flstrResult = "";

		do
		{
			char arrInput[4096] = { 0, };

			printf("Please input n and k as n, k.\n");
			printf("Permutation : k objects are selected from a set of n objects to produce subsets with ordering\n");
			printf("ex) 6, 2\n");
			printf("Input: ");

			// n, k 문자열을 입력 받는다. // Receive n, k strings.
			fgets(arrInput, 4096, stdin);

			CFLString<char> flstrInput(arrInput);
			if(flstrInput.IsEmpty())
				break;

			// 계수 값을 담기위해 CFLArray<int> 생성 // Create CFLArray<int> to hold coefficient values
			CFLArray<int> flaNK;
			int64_t i64TokenIdx = 0;

			// 입력 받은 문자열을 ',' 으로 구분하여 int 값으로 변환한다. // Separates the input string with ',' and converts it to an int value.
			while(true)
			{
				CFLString<char> flstrToken = flstrInput.Tokenize(",", i64TokenIdx);
				if(flstrToken.IsEmpty())
					break;

				if(flstrToken == "\n")
					break;

				flaNK.PushBack(flstrToken.ToInt32());

				if(flaNK.GetCount() == 2)
					break;
			}

			if(flaNK.GetCount() < 2)
				break;

			int32_t n = flaNK.GetAt(0);
			int32_t k = flaNK.GetAt(1);

			if(k == 0 || n < k)
			{
				flstrResult = "\nCount : 0";
				break;
			}

			// nPk, n 개에서 k 개를 선택하는 순열 // nPk, a permutation of selecting k objects from n objects
			permutation.SetMax(n);
			permutation.SetSelection(k);

			// 순열을 계산 // Calculate the permutation
			permutation.Calculate();

			// 순열 결과값 얻기 // Get permutation result
			CFLArray<CFLArrayL> flaPermutation = permutation.GetResult();

			CFLString<char> flstrPermutation = "";
			int64_t i64PermutationCnt = 0;

			for(int64_t i = 0; i < flaPermutation.GetCount(); i++)
			{
				flstrPermutation += "(";

				for(int64_t j = 0; j < flaPermutation.GetAt(i).GetCount(); j++)
					flstrPermutation += CFLString<char>().Format(" %d ", flaPermutation.GetAt(i).GetAt(j));

				flstrPermutation += ")\n";

				i64PermutationCnt++;
			}

			CFLString<char> flstrCnt = CFLString<char>().Format("\nCount : %d", i64PermutationCnt);

			flstrResult = flstrPermutation + flstrCnt;
		}
		while(false);

		if(flstrResult.IsEmpty())
			flstrResult = "Please check the input.\n";

		flstrResult += "\n\n";

		printf(flstrResult);
	}

	return 0;
}