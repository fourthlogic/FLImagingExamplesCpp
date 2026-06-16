#pragma once

namespace FLImaging
{
	namespace GUI
	{
		class CPropertyMenuDialogProgressExample : public CGUIPropertyMenuBase
		{
			SupportToDuplicateObject(CPropertyMenuDialogProgressExample);

		public:
			CPropertyMenuDialogProgressExample();
			virtual ~CPropertyMenuDialogProgressExample();

			virtual const CResult ConfigureMenu() override;

		protected:
			// 가상의 작업 스레드
			static void AlgorithmThreadForComplexDialog(CPropertyMenuDialogProgressExample* pInstance, std::future<void>** pContext);
			
			// 가상의 작업 스레드
			static void AlgorithmThreadForComplexDialogModal(CPropertyMenuDialogProgressExample* pInstance, std::future<void>** pContext);

			// 가상의 작업 스레드
			static void AlgorithmThreadForSimpleDialog(CPropertyMenuDialogProgressExample* pInstance, std::future<void>** pContext);

			// 간단한 진행률 표시 창 다이얼로그 생성 함수
			const CResult CreateSimpleDialog();

			// Resize Pivot 등 설정이 반영된 간단한 진행률 표시 창 다이얼로그 생성 함수
			const CResult CreateSimpleDialogWithSettings();

			// CFLSheet 가 포함된 복잡한 진행률 표시 창 다이얼로그 생성 함수
			const CResult CreateComplexProgressDialogWithSheet(bool bBoxLayout);

			// 복잡한 진행률 표시 창 다이얼로그 생성 함수
			const CResult CreateComplexProgressDialog();

			// Modal Dialog 생성 함수
			const CResult CreateComplexProgressDialogModal();

			// 간단한 진행률 표시 창 업데이트 함수
			const CResult UpdateSimpleDialog();

			// 간단한 진행률 표시 창 업데이트 함수
			const CResult UpdateSimpleDialogWithSettings();

			// 복잡한 진행률 표시 창 업데이트 함수
			const CResult UpdateComplexDialog();

			// CFLSheet 가 포함된 복잡한 진행률 표시 창 업데이트 함수
			const CResult UpdateComplexDialogWithSheet();

			// 진행률 표시 창 다이얼로그 닫기 함수
			const CResult CloseProgressDialog();

			// 정렬 옵션을 얻어 오는 함수
			EGUIAlignment GetAlignment(const Base::CFLString<wchar_t>& strVal) const;

			// 스레드 파라미터를 초기화 하는 함수
			void ResetThreadParams();

			// 버튼 클릭 이벤트 핸들러
			CPropertyButtonClickProcedure* ButtonCreateSimpleDialog();
			CPropertyButtonClickProcedure* ButtonCreateComplexDialog();
			CPropertyButtonClickProcedure* ButtonCreateComplexDialogWithSheetBoxLayout();
			CPropertyButtonClickProcedure* ButtonCreateComplexDialogWithSheetGridLayout();
			CPropertyButtonClickProcedure* ButtonCreateSimpleDialogWithSettings();
			CPropertyButtonClickProcedure* ButtonCreateComplexDialogModal();

		protected:
			// 스레드 완료 플래그
			volatile bool m_bThreadDone;

			// 현재 진행량
			volatile int32_t m_i32CurrentProgress;

			// 전체 양
			volatile int32_t m_i32TotalProgress;

			// 중지 진행 시 현재 진행량
			volatile int32_t m_i32CurrentStopProgress;

			// 중지 진행 시 전체 양
			volatile int32_t m_i32TotalStopProgress;

			// 스레드 포인터
			std::future<void>* m_pThread;

			// 다이얼로그 객체 포인터
			CGUIDialogProgress* m_pDlgProgress;

			// 프로그레스 바 객체 포인터
			CGUIProgressCtrlFloatingPoint* m_pProgressCtrlFP;

			// 중지 버튼 객체 포인터
			CGUIButton* m_pStopButton;

			// 여러 길이의 문자열 벡터
			std::vector<Base::CFLString<wchar_t>> m_vctVariousLengthMessages;

			// 메세지, 프로그레스 바, 시간 등이 표시되는 그리드
			CGUIGridLayout* m_pGridMain;

			// 버튼이 위치한 그리드
			CGUIGridLayout* m_pGridButtons;

			// 상세 창 그리드
			CGUIGridLayout* m_pGridDetails;

			// CFLSheet 가 포함된 레이아웃
			CGUILayoutBase* m_pLayoutForSheet;

			// 중지 버튼 클릭 횟수
			int32_t m_i32BtnStopClickCount;
		};
	}
}