#pragma once

namespace FLImaging
{
	namespace GUI
	{
		typedef struct sProgressThreadContext
		{
			sProgressThreadContext();
			virtual ~sProgressThreadContext();

			/**
			* \~English
			* \brief Terminates and releases the progress worker thread safely.
			* \~Korean
			* \brief 프로그레스 워커 스레드를 안전하게 종료하고 해제합니다.
			*/
			void TerminateProgressThread();

			/**
			* \~English
			* \brief Resets the thread parameters.
			* \~Korean
			* \brief 스레드 파라미터를 초기화합니다.
			*/
			void ResetThreadParams();

			/**
			* \~English
			* \brief Closes the progress display dialog window.
			* \return The result of the operation.
			* \~Korean
			* \brief 진행률 표시 창 다이얼로그를 닫습니다.
			* \return 작업 결과입니다.
			*/
			const CResult CloseProgressDialog();

			// 다이얼로그 객체 포인터
			// Dialog object pointer
			CGUIDialogProgress* pDlgProgress;

			// 스레드 포인터
			// Thread pointer
			std::future<void>* pThread;

			// 스레드 완료 플래그
			// Thread completion flag
			volatile bool bThreadDone;

			// 스레드 중지 요청 플래그
			// Thread stop request flag
			volatile bool bThreadStopRequested;

			// 현재 진행량
			// Current progress amount
			volatile int32_t i32CurrentProgress;

			// 전체 양
			// Total amount
			volatile int32_t i32TotalProgress;

			// 중지 진행 시 현재 진행량
			// Current progress amount during stop processing
			volatile int32_t i32CurrentStopProgress;

			// 중지 진행 시 전체 양
			// Total amount for stop processing
			volatile int32_t i32TotalStopProgress;

			// 중지 버튼 클릭 횟수
			// Stop button click count
			int32_t i32BtnStopClickCount;

			// 프로그레스 바 객체 포인터
			// Progress bar object pointer
			CGUIProgressCtrlFloatingPoint* pProgressCtrlFP;

			// 중지 버튼 객체 포인터
			// Stop button object pointer
			CGUIButton* pStopButton;

			// 메세지, 프로그레스 바, 시간 등이 표시되는 그리드
			// Grid layout displaying messages, progress bar, time, etc.
			CGUIGridLayout* pGridMain;

			// 버튼이 위치한 그리드
			// Grid layout where buttons are located
			CGUIGridLayout* pGridButtons;

			// 상세 창 그리드
			// Details window grid layout
			CGUIGridLayout* pGridDetails;

			// CFLSheet 가 포함된 레이아웃
			// Layout containing CFLSheet
			CGUILayoutBase* pLayoutForSheet;
		}SProgressThreadContext;

		class CPropertyMenuDialogProgressExample : public CGUIPropertyMenuBase
		{
			SupportToDuplicateObject(CPropertyMenuDialogProgressExample);

		public:
			CPropertyMenuDialogProgressExample();
			virtual ~CPropertyMenuDialogProgressExample();

			virtual const CResult ConfigureMenu() override;

		protected:
			// 작업 스레드 통합 (컨텍스트 기반)
			// Unified worker threads (Context-based)
			static void AlgorithmThreadForComplexDialog(CPropertyMenuDialogProgressExample* pInstance, SProgressThreadContext* pContext);
			static void AlgorithmThreadForSimpleDialog(CPropertyMenuDialogProgressExample* pInstance, SProgressThreadContext* pContext, bool bWithSettings);

			// 간단한 진행률 표시 창 다이얼로그 생성 함수
			// Function to create a simple progress display dialog
			const CResult CreateSimpleDialog();

			// Resize Pivot 등 설정이 반영된 간단한 진행률 표시 창 다이얼로그 생성 함수
			// Function to create a simple progress display dialog with settings like Resize Pivot
			const CResult CreateSimpleDialogWithSettings();

			// 복잡한 진행률 표시 창 다이얼로그 생성 함수
			// Function to create a complex progress display dialog
			const CResult CreateComplexProgressDialog();

			// Modal 복잡한 진행률 표시 창 다이얼로그 생성 함수
			// Function to create a modal complex progress display dialog
			const CResult CreateComplexProgressDialogModal();

			// CFLSheet 가 포함된 복잡한 진행률 표시 창 다이얼로그 생성 함수
			// Function to create a complex progress display dialog with CFLSheet
			const CResult CreateComplexProgressDialogWithSheet(bool bBoxLayout, SProgressThreadContext* pContext);

			// 간단한 진행률 표시 창 업데이트 함수
			// Function to update the simple progress display dialog
			const CResult UpdateSimpleDialog(SProgressThreadContext* pSContext, bool bWithSettings);

			// 복잡한 진행률 표시 창 통합 업데이트 함수
			// Unified function to update the complex progress display dialog
			const CResult UpdateComplexDialog(SProgressThreadContext* pSContext, int32_t i32StopClickCount);

			// 다이얼로그 설정 값(UI Properties)을 얻어오는 공통 함수
			// Common function to get dialog setting values (UI Properties)
			void GetDialogSettings(EGUIAlignment& eTextAlignment, EGUIAlignment& eResizePivot, bool& bKeepMaxWidth, bool& bKeepMaxHeight) const;

			// 정렬 옵션을 얻어 오는 함수
			// Function to get the alignment option
			EGUIAlignment GetAlignment(const Base::CFLString<wchar_t>& strVal) const;

			// 버튼 클릭 이벤트 핸들러
			// Button click event handler
			CPropertyButtonClickProcedure* ButtonCreateSimpleDialog();
			CPropertyButtonClickProcedure* ButtonCreateSimpleDialogWithoutMembers();
			CPropertyButtonClickProcedure* ButtonCreateComplexDialog();
			CPropertyButtonClickProcedure* ButtonCreateComplexDialogModal();
			CPropertyButtonClickProcedure* ButtonCreateComplexDialogWithSheet1();
			CPropertyButtonClickProcedure* ButtonCreateComplexDialogWithSheet2();
			CPropertyButtonClickProcedure* ButtonCreateSimpleDialogWithSettings();

		protected:
			// 다이얼로그 종류별 독립 컨텍스트 할당
			// Independent contexts allocated for each dialog type
			SProgressThreadContext* m_pSContext1; // Simple
			SProgressThreadContext* m_pSContext2; // Complex
			SProgressThreadContext* m_pSContext3; // ComplexModal
			SProgressThreadContext* m_pSContext4; // SimpleWithSettings
			SProgressThreadContext* m_pSContext5; // ComplexSheet1
			SProgressThreadContext* m_pSContext6; // ComplexSheet2

			// 여러 길이의 문자열 벡터
			// Vector of strings with various lengths
			std::vector<Base::CFLString<wchar_t>> m_vctVariousLengthMessages;
		};
	}
}