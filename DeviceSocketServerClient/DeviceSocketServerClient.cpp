# include <cstdio>

# include <FLImaging.h>
# include "../CommonHeader/ErrorPrint.h"

// 서버 소켓 이벤트 핸들러 클래스
// Server socket event class
// 소켓 이벤트를 수신하기 위해 CDeviceEventSocketBase에서 상속받아 구현
// Inherit from CDeviceEventSocketBase to receive socket events
class CDeviceEventSocketServerEx : public CDeviceEventSocketBase
{
public:

	// 생성자 // Constructor
	CDeviceEventSocketServerEx(CDeviceSocketServer* pSocketServer)
	{
		m_pSocketServer = pSocketServer;
	}

	// 소멸자 // Destructor
	virtual ~CDeviceEventSocketServerEx()
	{
		m_pSocketServer = nullptr;
	}

	// 수신 이벤트 함수 재정의 // Override receive event function
	virtual void OnReceived(const Devices::CDeviceSocketBase* pDeviceSocketClient, const Devices::CDeviceSocketPacket* pSocketPacket) override
	{
		// 받은 데이터를 문자열로 출력 // Print received data
		if(pSocketPacket)
		{
			CFLString<char> flsData((char*)pSocketPacket->GetBuffer(), pSocketPacket->GetSize());
			printf("[Server] Recv %s\n", flsData.GetBuffer());
		}

		// 재전송(echo) // Send string(echo)
		if(pDeviceSocketClient)
		{
			CDeviceSocketClient* pClient = (CDeviceSocketClient*)(pDeviceSocketClient);

			if(pClient)
			{
				// 클라이언트 매니저 가져오기 // Obtain Client Manager Objects
				CDeviceSocketClientManager* pDeviceSocketClientManager = m_pSocketServer->GetSocketClientManager();

				if(pDeviceSocketClientManager && pSocketPacket)
				{
					CFLString<char> flsData((char*)pSocketPacket->GetBuffer(), pSocketPacket->GetSize());

					printf("[Server] Send %s\n", flsData.GetBuffer());

					// 연결이 살아있고 연결 상태라면 데이터 전송 // Send if connection is alive
					if(pDeviceSocketClientManager->IsClientAlive(pClient))
						pClient->Send(pSocketPacket);

					CThreadUtilities::Sleep(500);
				}
			}
		}
	}

protected:

	Devices::CDeviceSocketServer* m_pSocketServer;
};

// 클라이언트 소켓 이벤트 클래스
// Client socket event class
class CDeviceEventSocketClientEx : public CDeviceEventSocketBase
{
public:
	// 생성자 // Constructor
	CDeviceEventSocketClientEx()
	{
		m_bConnect = false;
	}

	// 소멸자 // Destructor
	virtual ~CDeviceEventSocketClientEx()
	{

	}
	
	// 연결 이벤트 함수 재정의 // Override connection event functions
	virtual void OnConnected(const Devices::CDeviceSocketBase* pDeviceSocketClient) override
	{
		m_bConnect = true;
	}

	// 연결 해제 이벤트 함수 재정의 // Override disconnection event functions
	virtual void OnDisconnected(const Devices::CDeviceSocketBase* pDeviceSocketClient) override
	{
		m_bConnect = false;
	}

	// 수신 이벤트 함수 재정의 // Override receive event function
	virtual void OnReceived(const Devices::CDeviceSocketBase* pDeviceSocketClient, const Devices::CDeviceSocketPacket* pSocketPacket) override
	{
		// 받은 데이터를 문자열로 출력 // Print received data
		if(pSocketPacket)
		{
			CFLString<char> flsData((char*)pSocketPacket->GetBuffer(), pSocketPacket->GetSize());
			printf("[Client] Recv %s\n", flsData.GetBuffer());
		}

		// 재전송(echo) // Send string(echo)
		if(pDeviceSocketClient && pSocketPacket)
		{
			CDeviceSocketClient* pClient = (CDeviceSocketClient*)(pDeviceSocketClient);

			if(pClient)
			{
				CFLString<char> flsData((char*)pSocketPacket->GetBuffer(), pSocketPacket->GetSize());

				if(m_bConnect)
				{
					printf("[Client] Send %s\n", flsData.GetBuffer());
					pClient->Send(pSocketPacket);
					CThreadUtilities::Sleep(500);
				}
			}
		}
	}

protected:

	bool m_bConnect; // 연결 여부 // Connection state
};


int main()
{
	// FLImaging 라이브러리 초기화 (필수 호출) 
	// Initialize the FLImaging library (must be called once)
	CLibraryUtilities::Initialize();

	CResult res;

	// 소켓 서버, 클라이언트 선언 // Declare socket server and client
	CDeviceSocketServer deviceSocketServer;
	CDeviceSocketClient deviceSocketClient;

	// 이벤트 객체 생성 및 등록 // Create and register event handlers
	CDeviceEventSocketServerEx deviceEventSocketServer(&deviceSocketServer);
	CDeviceEventSocketClientEx deviceEventSocketClient;

	deviceSocketServer.RegisterDeviceEvent(&deviceEventSocketServer);
	deviceSocketClient.RegisterDeviceEvent(&deviceEventSocketClient);

	do
	{
		// 소켓 모드 설정 (Passive)
		// Set socket mode (Passive)
		deviceSocketServer.SetSocketMode(ESocketMode_NoProtocol_Passive);
		deviceSocketClient.SetSocketMode(ESocketMode_NoProtocol_Passive);

		// IP 주소와 포트 설정 // Set IP and port
		CFLString<char> flsIPAddress = "127.0.0.1";
		uint16_t u16Port = 4444;
		deviceSocketServer.SetConnectionIPAddress(flsIPAddress, u16Port);
		deviceSocketClient.SetConnectionIPAddress(flsIPAddress, u16Port);

		// 소켓 서버 초기화 // Initialize socket server
		if((res = deviceSocketServer.Initialize()).IsFail())
		{
			ErrorPrint(res, "Failed to initialize server.\n");
			break;
		}

		// 소켓 클라이언트 초기화 (서버에 연결) // Initialize client (connect to server)
		if((res = deviceSocketClient.Initialize()).IsFail())
		{
			ErrorPrint(res, "Failed to initialize client.\n");
			break;
		}

		// 테스트용 데이터 생성 // Create test data
		CFLString<char> flsData("Socket echo test. [Enter any key if you want to exit]");
		CDeviceSocketPacket socketPacket;
		socketPacket.Assign((uint8_t*)flsData.GetBuffer(), flsData.GetLength());

		// 클라이언트에서 서버로 데이터 송신 // Send data from client to server
		if((res = deviceSocketClient.Send(socketPacket)).IsFail())
		{
			ErrorPrint(res, "Failed to send.\n");
			break;
		}

		getchar();

		// 소켓 해제 및 이벤트 해제 // Terminate sockets and clear events
		deviceSocketServer.Terminate();
		deviceSocketClient.Terminate();
	}
	while(false);

	return 0;
}