#include "stdafx.h"
#include "PropertyMenuFullyConnected_XOR.h"

using namespace FLImaging;
using namespace FLImaging::Base;
using namespace FLImaging::Foundation;
using namespace FLImaging::GUI;

namespace FLImaging
{
	namespace GUI
	{
		// 복잡한 다이얼로그의 레이아웃의 row 인덱스
		enum class ERowLayout : int32_t
		{
			Main = 0,           // Main 레이아웃
			Button = 4,         // Button 레이아웃
			Details = 0,        // Details 레이아웃

			Count = 5,          // 행 개수
		};

		// 복잡한 다이얼로그의 레이아웃의 col 인덱스
		enum class EColLayout : int32_t
		{
			Main = 0,           // Main 레이아웃
			Button = 0,         // Button 레이아웃
			Details = 1,        // Details 레이아웃

			Count = 2,          // 컬럼 개수
		};

		// 단순한 다이얼로그의 메인 그리드 내부 컨트롤의 row 인덱스
		enum class ERowMainSimple : int32_t
		{
			Message = 0,       // 메세지
			ProgressCtrl = 2,  // 프로그레스 바
			ElapsedTime = 3,   // 시간

			Count = 4,         // 행 개수
		};

		// 단순한 다이얼로그의 메인 그리드 내부 컨트롤의 col 인덱스
		enum class EColMainSimple : int32_t
		{
			Message = 0,       // 메세지
			ProgressCtrl = 0,  // 프로그레스 바
			ElapsedTime = 0,   // 시간

			Count = 1,         // 컬럼 개수
		};

		// 단순한 다이얼로그의 버튼 그리드의 row 인덱스
		enum class ERowButtonSimple : int32_t
		{
			Stop = 0,          // 중지 버튼

			Count = 1,         // 행 개수
		};

		// 다이얼로그의 버튼 그리드의 col 인덱스
		enum class EColButton : int32_t
		{
			Button = 2,        // 버튼

			Count = 3,         // 컬럼 개수
		};
	}
}

FLImaging::GUI::CPropertyMenuFullyConnectedXOR::CPropertyMenuFullyConnectedXOR()
	: CGUIPropertyMenuBase()
	, m_pDlgProgress(nullptr)
	, m_pThread(nullptr)
	, m_pProgressCtrlFP(nullptr)
	, m_pStopButton(nullptr)
	, m_pGridMain(nullptr)
	, m_pGridButtons(nullptr)
	, m_pGridDetails(nullptr)
	, m_i32BtnStopClickCount(0)
	, m_tsrEvaluation(*new CTensor<float>)
	, m_tsrCost(*new CTensor<float>)
	, m_tsrAnswer(*new CTensor<float>)
	, m_pOpt(nullptr)
	, m_pGfCost(nullptr)
	, m_pGfEvaluation(nullptr)
	, m_pPane(nullptr)
	, m_pViewGraph(nullptr)
{
}

FLImaging::GUI::CPropertyMenuFullyConnectedXOR::~CPropertyMenuFullyConnectedXOR()
{
	CloseProgressDialog();

	if(m_pGfCost)
	{
		CComputationalGraphUtilities<float>::Delete((CComputationalBase<float>*)m_pGfCost, false);

		m_pGfCost = nullptr;
		m_pGfEvaluation = nullptr;
	}

	if(m_pThread)
	{
		delete m_pThread;
		m_pThread = nullptr;
	}

	delete& m_tsrCost;
	delete& m_tsrEvaluation;
	delete& m_tsrAnswer;
}

const CResult FLImaging::GUI::CPropertyMenuFullyConnectedXOR::ConfigureMenu()
{
	CResult cReturn = EResult_UnknownError;

	do
	{
		CGUIPropertyButton* pBtn = new CGUIPropertyButton;
		pBtn->SetName(L"XOR Test");
		pBtn->SetPropertyButtonClickProcedure(TrainXOR());
		pBtn->SetButtonWidthRatio(1.);
		AddButton(pBtn);
		SetShortcut(EKeyType_F6, pBtn);

		cReturn = EResult_OK;
	}
	while(false);

	return cReturn;
}

CPropertyButtonClickProcedure* FLImaging::GUI::CPropertyMenuFullyConnectedXOR::TrainXOR()
{
	CPropertyButtonClickProcedure* pProcedure = new CPropertyButtonClickProcedure;
	*pProcedure = MakePropertyButtonClickProcedure
	{
		CResult res = EResult_UnknownError;

		CGUIPaneProperties* pPane = GetPaneProperties();
		pPane->EnableWindow(false);

		do
		{
			CreateSimpleDialog();
			// 다이얼로그 창 생성
			m_pDlgProgress->OnInitDialog();
			m_pDlgProgress->SetWindowPos(nullptr, 850, 0, 300, 500, SWP_NOZORDER);

			float f32LearningRate = 1e-1f;
			float f32Beta1 = 0.9f;
			float f32Beta2 = 0.999f;
			float f32Epsilon = 1e-8f;

			//XOR Input
			CTensor<float> tsrX;

			tsrX =
			{
				{ 0.f, 0.f },
				{ 0.f, 1.f },
				{ 1.f, 0.f },
				{ 1.f, 1.f },
			};

			//XOR Output
			CTensor<float> tsrY;

			tsrY =
			{
				{ 0.f },
				{ 1.f },
				{ 1.f },
				{ 0.f },
			};

			CComputationalGraphPlaceholderF gfPlaceHolderX, gfPlaceHolderY;
			gfPlaceHolderX.SetID(L"X");
			gfPlaceHolderX.SetValueAttribute(EValueAttribute_Input);
			gfPlaceHolderY.SetID(L"Y");
			gfPlaceHolderY.SetValueAttribute(EValueAttribute_Input);

			gfPlaceHolderX.SetOperand(tsrX);
			gfPlaceHolderY.SetOperand(tsrY);

			m_tsrAnswer = tsrY;

			//Layer에 필요한 Weight Tensor 선언
			CTensor<float> tsrW1;
			tsrW1.Create({ 2, 10 });
			tsrW1.FillHe();
			tsrW1.EnableReference(false);
			tsrW1.SetValueAttribute(EValueAttribute_Variable);
			CTensor<float> tsrB1;
			tsrB1.Create({ 10 });
			tsrB1.FillZero();
			tsrB1.EnableReference(false);
			tsrB1.SetValueAttribute(EValueAttribute_Variable);

			CTensor<float> tsrWH1;
			tsrWH1.Create({ 10, 10 });
			tsrWH1.FillHe();
			tsrWH1.EnableReference(false);
			tsrWH1.SetValueAttribute(EValueAttribute_Variable);
			CTensor<float> tsrBH1;
			tsrBH1.Create({ 10 });
			tsrBH1.FillZero();
			tsrBH1.EnableReference(false);
			tsrBH1.SetValueAttribute(EValueAttribute_Variable);

			CTensor<float> tsrWH2;
			tsrWH2.Create({ 10, 10 });
			tsrWH2.FillHe();
			tsrWH2.EnableReference(false);
			tsrWH2.SetValueAttribute(EValueAttribute_Variable);
			CTensor<float> tsrBH2;
			tsrBH2.Create({ 10 });
			tsrBH2.FillZero();
			tsrBH2.EnableReference(false);
			tsrBH2.SetValueAttribute(EValueAttribute_Variable);

			CTensor<float> tsrW2;
			tsrW2.Create({ 10, 1 });
			tsrW2.FillHe();
			tsrW2.EnableReference(false);
			tsrW2.SetValueAttribute(EValueAttribute_Variable);
			CTensor<float> tsrB2;
			tsrB2.Create({ 1 });
			tsrB2.FillZero();
			tsrB2.EnableReference(false);
			tsrB2.SetValueAttribute(EValueAttribute_Variable);

			// XOR Graph Layer 생성
			CCGFNodeVar(gfLayer0, CCGFSigmoid(CCGFAffine(gfPlaceHolderX, tsrW1, tsrB1)));
			CCGFNodeVar(gfLayerH1, CCGFSigmoid(CCGFAffine(gfLayer0, tsrWH1, tsrBH1)));
			CCGFNodeVar(gfLayerH2, CCGFSigmoid(CCGFAffine(gfLayerH1, tsrWH2, tsrBH2)));
			CCGFNodeVar(gfHypothesis, CCGFSigmoid(CCGFAffine(gfLayerH2, tsrW2, tsrB2)));
			CCGFNodeVar(gfCost, CCGFMultiply(CCGFReduceMean(CCGFAdd(CCGFMultiply(gfPlaceHolderY, CCGFLog(gfHypothesis)), CCGFMultiply(CCGFSubtract(1, gfPlaceHolderY), CCGFLog(CCGFSubtract(1, gfHypothesis)))), {0}), -1));

			COptimizer<float> optm;
			CLRSConstant<float> lrs;
			CValidatorForClassifier<float> validator;

			//Optimizer parameter 설정
			lrs.SetLearningRate(f32LearningRate);

			m_pGfCost = &gfCost;
			m_pGfEvaluation = &gfHypothesis;
			m_pGfEvaluation->SetID(L"Valid");
			validator.SetValidationFunctions(&gfPlaceHolderY, m_pGfEvaluation);

			optm.SetLearningRateScheduler(lrs);
			optm.SetFunction(*m_pGfCost);
			optm.SetValidator(validator);
			optm.Initialize();
			m_pOpt = &optm;

			// 작업 스레드 생성
			m_pThread = new std::future<void>(std::async(CPropertyMenuFullyConnectedXOR::AlgorithmThreadForSimpleDialog, this, &m_pThread));
			SetThreadPriority(m_pThread, THREAD_PRIORITY_LOWEST);

			// 스레드 파라미터 초기화
			ResetThreadParams();

			// 작업 스레드에서 업데이트 한 값을 반영하여 다이얼로그 창 업데이트
			UpdateSimpleDialog();

			// 다이얼로그 닫기
			CloseProgressDialog();

			res = EResult_OK;
		}
		while(false);

		if(m_pThread)
		{
			delete m_pThread;
			m_pThread = nullptr;
		}

		pPane->EnableWindow(true);
	};

	return pProcedure;
}

const CResult FLImaging::GUI::CPropertyMenuFullyConnectedXOR::CreateSimpleDialog()
{
	CResult cr;

	do
	{
		CloseProgressDialog();

		// 다이얼로그 생성
		if(!m_pDlgProgress)
			m_pDlgProgress = CGUIDialogProgress::CreateModelessDialog(AfxGetApp()->m_pMainWnd);

		CFLString<wchar_t> strMessage;

		// 메시지, 프로그레스 바, 시간 등이 표시되는 메인 그리드 레이아웃 생성
		m_pGridMain = new CGUIGridLayout((int32_t)ERowMainSimple::Count, (int32_t)EColMainSimple::Count);
		{
			strMessage.Format
			(
				L"Fully-Connected XOR Network Training...(%d / %d)\n",
				m_i32Epoch, m_i32MaxEpoch
			);

			// 메시지를 그리드 레이아웃에 추가
			m_pGridMain->AddCtrl((int32_t)ERowMainSimple::Message, (int32_t)EColMainSimple::Message, 2, 1, strMessage);
			// 프로그레스 바를 그리드 레이아웃에 추가
			m_pGridMain->AddCtrl((int32_t)ERowMainSimple::ProgressCtrl, (int32_t)EColMainSimple::ProgressCtrl, EControl_ProgressBarFloatingPoint);
			// 소요 시간을 그리드 레이아웃에 추가
			m_pGridMain->AddCtrl((int32_t)ERowMainSimple::ElapsedTime, (int32_t)EColMainSimple::ElapsedTime, L"Elapsed Time      00 : 00 : 00");
			// 소요 시간의 텍스트 정렬 옵션을 수평, 수직 모두 중간 정렬로 설정
			m_pGridMain->SetCtrlAlignment((int32_t)ERowMainSimple::ElapsedTime, (int32_t)EColMainSimple::ElapsedTime, EGUIAlignment_CENTER_CENTER);
			// 소요 시간이 m_pGridMain 레이아웃의 (int32_t)ERowMainSimple::ElapsedTime, (int32_t)EColMainSimple::ElapsedTime 위치에 있다는 것을 m_pDlgProgress 에 알려줌
			m_pDlgProgress->SetElapsedTimePosition(m_pGridMain, (int32_t)ERowMainSimple::ElapsedTime, (int32_t)EColMainSimple::ElapsedTime);
			// 진행률 업데이트를 위해 프로그레스 바 객체 포인터를 얻어 오기
			m_pProgressCtrlFP = (CGUIProgressCtrlFloatingPoint*)m_pGridMain->GetCtrl((int32_t)ERowMainSimple::ProgressCtrl, (int32_t)EColMainSimple::ProgressCtrl);
		}

		// 버튼이 표시되는 버튼 그리드 레이아웃 생성
		m_pGridButtons = new CGUIGridLayout((int32_t)ERowButtonSimple::Count, (int32_t)EColButton::Count);
		{
			// 중지 버튼을 그리드 레이아웃에 추가
			m_pGridButtons->AddCtrl((int32_t)ERowButtonSimple::Stop, (int32_t)EColButton::Button, EControl_Button, L"Stop");
			// 중지 버튼 객체 포인터를 얻어 오기
			m_pStopButton = (CGUIButton*)m_pGridButtons->GetCtrl((int32_t)ERowButtonSimple::Stop, (int32_t)EColButton::Button);
		}

		m_pPane = CGUIManagerPane::GetPaneViewGraph();

		if(m_pPane)
		{
			m_pViewGraph = m_pPane->GetViewGraph();

			if(m_pViewGraph)
			{
				m_pViewGraph->ClearAll();
				m_pViewGraph->Invalidate();
			}
		}

		// 다이얼로그에 메인 그리드 레이아웃 추가
		m_pDlgProgress->Add(m_pGridMain);
		// 다이얼로그에 버튼 그리드 레이아웃 추가
		m_pDlgProgress->Add(m_pGridButtons);

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuFullyConnectedXOR::UpdateSimpleDialog()
{
	CResult cr;

	do
	{
		while(true)
		{
			// 스레드가 완료되면 break
			if(m_bThreadDone)
			{
				if(m_tsrCost.IsDeviceActivated())
				{
					m_tsrCost.CopyDeviceToHost();
					m_tsrEvaluation.CopyDeviceToHost();
				}

				// 중지 버튼 내 문자열을 "Continue.." 로 설정
				m_pGridButtons->SetCtrlValue((int32_t)ERowButtonSimple::Stop, (int32_t)EColButton::Button, L"Close");

				if(m_pStopButton->GetClickCount() == 1)
				{
					if(m_tsrCost.IsDeviceActivated())
						m_tsrCost.CopyDeviceToHost();

					break;
				}

				const float* pF32Cost = m_tsrCost.GetMemory();
				const float* pF32Evaluate = m_tsrEvaluation.GetMemory();

				// 가변 길이의 문자열을 설정
				CFLString<wchar_t> strMessage;
				CFLString<wchar_t> strEpochMessage;

				strEpochMessage.Format
				(
					L"Fully-Connected XOR Network Training Finished!\n\n",
					m_i32Epoch, m_i32MaxEpoch
				);

				CFLString<wchar_t> strTrainMessage =
					CFLString<wchar_t>(L"<Cost>\n%f\n\n") +
					CFLString<wchar_t>(L"<Validation>\n") +
					CFLString<wchar_t>(L"[ %f %f %f %f ]\n\n");

				strMessage = strEpochMessage + strTrainMessage;
				strMessage.Format
				(
					strMessage, pF32Cost[0], pF32Evaluate[0], pF32Evaluate[1], pF32Evaluate[2], pF32Evaluate[3]
				);

				// m_pGridMain 의 ERowMainSimple::Message, ERowMainSimple::Message 위치에 문자열 값 설정하여 다이얼로그 상에서 보이는 메세지 설정
				m_pGridMain->SetCtrlValue((int32_t)ERowMainSimple::Message, (int32_t)ERowMainSimple::Message, strMessage);
				// 프로그레스 바가 100% 로 채워지도록 설정
				m_pProgressCtrlFP->SetPos(1.);
				// 다이얼로그 화면 갱신
				m_pDlgProgress->Invalidate();
				// 메시지 펌프
				CGUIManager::PeekAndPump();
				Sleep(1);
				continue;
			}
			else
			{
				// 중지 버튼 클릭 횟수가 변경된 경우
				if(m_pStopButton->GetClickCount() != m_i32BtnStopClickCount)
				{
					// 중지 버튼을 한 번 클릭한 경우
					if(m_pStopButton->GetClickCount() == 1)
					{
						// 중지 버튼 내 문자열을 "Close" 로 설정
						m_pGridButtons->SetCtrlValue((int32_t)ERowButtonSimple::Stop, (int32_t)EColButton::Button, L"Close");
						// 프로그레스 바가 100% 로 채워지도록 설정
						m_pProgressCtrlFP->SetPos(1.);
						// 다이얼로그 화면 갱신
						m_pDlgProgress->Invalidate();
					}
					// 중지 버튼을 두 번 클릭한 경우 ("Continue.." 버튼을 클릭한 경우)
					else if(m_pStopButton->GetClickCount() == 2)
					{
						// 중지 버튼 숨기기
						m_pGridButtons->SetCtrlVisible((int32_t)ERowButtonSimple::Stop, (int32_t)EColButton::Button, false);
					}

					// 중지 버튼 클릭 횟수 업데이트
					m_i32BtnStopClickCount = m_pStopButton->GetClickCount();

					// 다이얼로그의 레이아웃 업데이트
					m_pDlgProgress->AdjustLayout();
				}

				// 중지 버튼을 1회 이상 클릭 시 break
				if(m_pStopButton->GetClickCount() >= 2)
					break;
				else if(m_i32Epoch > 0)
				{
					if(m_tsrCost.IsDeviceActivated())
					{
						m_tsrCost.CopyDeviceToHost();
						m_tsrEvaluation.CopyDeviceToHost();
					}

					const float* pF32Cost = m_tsrCost.GetMemory();
					const float* pF32Evaluate = m_tsrEvaluation.GetMemory();

					// 가변 길이의 문자열을 설정
					CFLString<wchar_t> strMessage;
					CFLString<wchar_t> strEpochMessage;

					if(m_pStopButton->GetClickCount() == 0)
					{
						strEpochMessage.Format
						(
							L"Fully-Connected XOR Network Training...(%d / %d)\n\n",
							m_i32Epoch, m_i32MaxEpoch
						);
					}
					else
					{
						strEpochMessage.Format
						(
							L"Fully-Connected XOR Network Terminated!\n\nFinal Epoch : %d\n\n",
							m_i32Epoch
						);
					}

					CFLString<wchar_t> strTrainMessage =
						CFLString<wchar_t>(L"<Cost>\n%f\n\n") +
						CFLString<wchar_t>(L"<Validation>\n") +
						CFLString<wchar_t>(L"[ %f %f %f %f ]\n\n");

					strMessage = strEpochMessage + strTrainMessage;
					strMessage.Format
					(
						strMessage, pF32Cost[0], pF32Evaluate[0], pF32Evaluate[1], pF32Evaluate[2], pF32Evaluate[3]
					);

					// m_pGridMain 의 ERowMainSimple::Message, ERowMainSimple::Message 위치에 문자열 값 설정하여 다이얼로그 상에서 보이는 메세지 설정
					m_pGridMain->SetCtrlValue((int32_t)ERowMainSimple::Message, (int32_t)ERowMainSimple::Message, strMessage);

					// 프로그레스 바에 진행률 표시
					m_pProgressCtrlFP->SetPos((double)m_i32Epoch / (double)m_i32MaxEpoch);
					// 다이얼로그 창 크기 업데이트
					m_pDlgProgress->AdjustLayout();
				}
			}


			// 메시지 펌프
			CGUIManager::PeekAndPump();
			// Sleep 시간을 1로 설정
			Sleep(1);
		}

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

const CResult FLImaging::GUI::CPropertyMenuFullyConnectedXOR::CloseProgressDialog()
{
	CResult cr;

	do
	{
		if(m_pDlgProgress)
		{
			m_pDlgProgress->DestroyWindow();
			m_pDlgProgress = nullptr;
		}

		cr = EResult_OK;
	}
	while(false);

	return cr;
}

void FLImaging::GUI::CPropertyMenuFullyConnectedXOR::AlgorithmThreadForSimpleDialog(CPropertyMenuFullyConnectedXOR* pInstance, std::future<void>** pContext)
{
	// 단순한 다이얼로그에서 사용되는 스레드

	if(pInstance && pInstance->m_pDlgProgress)
	{
		// 학습 결과 비용과 검증 결과 기록을 받아 그래프 뷰에 출력  
		// Get the history of cost and validation and print it at graph view
		CFLArray<float> flaCosts;
		CFLArray<float> flaValidationFF;
		CFLArray<float> flaValidationFT;
		CFLArray<float> flaValidationTF;
		CFLArray<float> flaValidationTT;

		while(true)
		{
			if(pInstance->m_i32Epoch >= pInstance->m_i32MaxEpoch)
			{
				pInstance->m_bThreadDone = true;
				break;
			}

			if(pInstance->m_pStopButton->GetClickCount() == 1)
			{
				pInstance->m_bThreadDone = false;
				break;
			}

			pInstance->m_pViewGraph->LockUpdate();
			// 이전 그래프의 데이터를 삭제
			pInstance->m_pViewGraph->Clear();

			pInstance->m_tsrCost = pInstance->m_pGfCost->Evaluate();
			pInstance->m_tsrEvaluation = pInstance->m_pGfEvaluation->Evaluate();

			//GPU 연산일 경우 호스트 메모리로 데이터 복사
			if(pInstance->m_tsrCost.IsDeviceActivated())
			{
				pInstance->m_tsrCost.CopyDeviceToHost();
				pInstance->m_tsrEvaluation.CopyDeviceToHost();
			}

			if(pInstance->m_tsrCost.GetMemory() != nullptr)
			{

				flaCosts.PushBack(pInstance->m_tsrCost.GetMemory()[0]);
				flaValidationFF.PushBack(pInstance->m_tsrEvaluation.GetMemory()[0]);
				flaValidationFT.PushBack(pInstance->m_tsrEvaluation.GetMemory()[1]);
				flaValidationTF.PushBack(pInstance->m_tsrEvaluation.GetMemory()[2]);
				flaValidationTT.PushBack(pInstance->m_tsrEvaluation.GetMemory()[3]);
			}

			// XOR 결과와 완벽하게 일치하면 종료
			if(pInstance->m_tsrAnswer.Equals(pInstance->m_tsrEvaluation))
			{
				pInstance->m_bThreadDone = true;
				break;
			}

			pInstance->m_pViewGraph->Plot(flaCosts, EChartType_Line, RED, L"Cost");
			pInstance->m_pViewGraph->Plot(flaValidationFF, EChartType_Line, PINK, L"FF");
			pInstance->m_pViewGraph->Plot(flaValidationFT, EChartType_Line, ORANGE, L"FT");
			pInstance->m_pViewGraph->Plot(flaValidationTF, EChartType_Line, SKYBLUE, L"TF");
			pInstance->m_pViewGraph->Plot(flaValidationTT, EChartType_Line, GREEN, L"TT");

			pInstance->m_pViewGraph->UnlockUpdate();
			pInstance->m_pViewGraph->UpdateWindow();
			pInstance->m_pViewGraph->RedrawWindow();
			pInstance->m_pViewGraph->Invalidate();

			pInstance->m_pOpt->Fit();
			pInstance->m_i32Epoch++;

			Sleep(1);
		}
	}
}

void FLImaging::GUI::CPropertyMenuFullyConnectedXOR::ResetThreadParams()
{
	// 스레드 파라미터 값을 초기화 합니다.
	m_bThreadDone = false;
	m_i32CurrentStopProgress = 0;
	m_i32CurrentProgress = 0;
	m_i32TotalProgress = 2000;
	m_i32TotalStopProgress = 20;
	m_i32BtnStopClickCount = 0;
	m_i32Epoch = 0;
}
