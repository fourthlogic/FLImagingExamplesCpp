#pragma once

namespace FLImaging
{
	namespace GUI
	{
		class CPropertyMenuPaneViewGraphExample : public CGUIPropertyMenuBase
		{
			SupportToDuplicateObject(CPropertyMenuPaneViewGraphExample);

		public:
			CPropertyMenuPaneViewGraphExample();
			virtual ~CPropertyMenuPaneViewGraphExample();

			virtual const CResult ConfigureMenu() override;

			CPropertyItemButtonClickProcedure* MakeButton1Implement();
			CPropertyItemButtonClickProcedure* MakeButton2Implement();
			CPropertyItemButtonClickProcedure* MakeButton3Implement();
			CPropertyItemButtonClickProcedure* MakeButton4Implement();

			DeclareGetClassType();
		};
	}
}