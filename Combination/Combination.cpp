#include <cstdio>

#include <FLImaging.h>


int main()
{
	// You must call the following function once
	// before using any features of the FLImaging(R) library
	CLibraryUtilities::Initialize();

	// 조합 객체 선언 // Declare a combination object
	CCombination combination;

	while(true)
	{
		CFLString<char> flstrResult = "";

		do
		{
			char arrInput[4096] = { 0, };

			printf("Please input n and k as n, k.\n");
			printf("Combination : k objects are selected from a set of n objects to produce subsets without ordering\n");
			printf("ex) 6, 2\n");
			printf("Input: ");

			// 계수 문자열을 입력 받는다. // Receive the count string.
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

			// nCk, n 개에서 k 개를 선택하는 조합 // nCk, a combination of selecting k objects from n objects
			combination.SetMax(n);
			combination.SetSelection(k);

			// 조합을 계산 // Calculate combinations
			combination.Calculate();

			// 조합 결과값 얻기 // Get combination result
			CFLArray<CFLArrayL> flaCombination = combination.GetResult();

			CFLString<char> flstrCombination = "";
			int64_t i64CombinationCnt = 0;

			for(int64_t i = 0; i < flaCombination.GetCount(); i++)
			{
				flstrCombination += "(";

				for(int64_t j = 0; j < flaCombination.GetAt(i).GetCount(); j++)
				{
					flstrCombination += CFLString<char>().Format(" %d ", flaCombination.GetAt(i).GetAt(j));
				}
				flstrCombination += ")\n";
				i64CombinationCnt++;
			}

			CFLString<char> flstrCnt = CFLString<char>().Format("\nCount : %d", i64CombinationCnt);

			flstrResult = flstrCombination + flstrCnt;
		} while(false);

		if(flstrResult.IsEmpty())
			flstrResult = "Please check the input.\n";

		flstrResult += "\n\n";

		printf(flstrResult);
	}

	return 0;
}