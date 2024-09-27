#include <iostream>
#include <WinSock2.h>
#include <thread> // 멀티 쓰레드를 사용하기 위해 추가


using namespace std;

// 윈도우 소켓의 기능을 쓰기 위해 라이브러리를 추가한다.
#pragma comment(lib, "ws2_32.lib")

#define PORT 7890	//사용할 포트 번호를 7890으로 설정
#define PACKET_LENGTH 1024	//한 번에 주고 받을 패킷의 최대 길이를 1024 바이트로 설정

//클라이언트 처리 함수(쓰레드에서 동작)
//클라이언트와 메시지를 주고 받는 기능을 쓰레드에서 실행할 수 있게 함수로 분리
void ClientHandler(SOCKET clientSocket) {
	char Packet[PACKET_LENGTH] = {};//데이터 저장할 패킷 버퍼 초기화

	while (true) {//무한 루프를 통해서 지속적으로 클라이언트 메시지를 주고 받음
		//클라이언트로부터 메시지를 수신
		int recvSize = recv(clientSocket, Packet, PACKET_LENGTH, 0);
		if (recvSize == SOCKET_ERROR || recvSize == 0) {//수신 에러 또는 클라이언트가 연결 종료 시
			cout << "클라이언트 연결 종료" << endl;
			break;
		}

		Packet[recvSize] = '\0';//받은 메시지 끝에 null 문자를 추가해서 문자열을 마무리
		cout << "Client : " << Packet << endl;//클라이언트로부터 받은 메시지를 콘솔에 출력

		//클라이언트가 "exit"라는 메시지를 보내면 서버에서 연결을 종료한다.
		if (strcmp(Packet, "exit") == 0) {
			cout << "클라이언트가 종료 요청을 보냈습니다." << endl;
			break;
		}


		//패킷 버퍼를 다음 데이터를 위해 깨끗하게 초기화
		memset(Packet, 0, sizeof(Packet));



	}
		//클라이언트 소켓을 닫아서 연결을 종료한다.
		closesocket(clientSocket);

}


bool chatClient(SOCKET hClientSocket,char *Packet) {
	cin.getline(Packet, PACKET_LENGTH);//사용자 입력을 받음

	//"exit" 입력 시 서버에 메시지를 보내고 종료
	if (strcmp(Packet, "exit") == 0) {
		send(hClientSocket, Packet, strlen(Packet), 0);
		return false;
	}

	send(hClientSocket, Packet, strlen(Packet), 0);//서버에 메시지를 전송
	return true;
}

int main() {
	/*
	* struct WSAData{
	* WORD wVersion;	//윈도우 소켓의 버전
	* WORD wHighVersion;	//지원되는 소켓의 상위 버전으로 wVersion과 일치
	* char szDescription[WSADESCRIPTION_LEN+1];		//NULL로 끝나는 아스키 스트링 값, 윈도우 소켓 설명
	* char szSystemStatus[WSASYS_STATUS_LEN+1];		//NULL로 끝나는 아스키 스트링 값, 상태 문자열
	* unsigned short iMaxSockets;	//사용할 소켓의 최대 소켓 수, version 2 부터는 무시
	* unsigned short iMaxUdpDg;		//전송할 수 있는 데이터 그램의 최대 크기, version 2 부터는 무시
	* char FAR * lpVendorinfo;		//벤더 정보(의미없음)
	*/
	WSADATA wsaData; //윈도우 소켓 초기화 정보를 가지고 있는 구조체

	/*
		WSAStartup(윈도우 소켓 초기화) ~ WSACleanup(윈도우 소켓 종료) 사이에
		윈도우 소켓 코드를 작성한다.
	*/

	/*
	* 윈도우 소켓 초기화 (WSAStartup() 함수)
	* int WSAStartup(
	* WORD wVersionRequired,
	* LPWSADATA lpWSAData
	* );
	* 
	* arg 1 : WORD  *wVersionRequested
	* 프로그램이 요구하는 winsock 버전 중 최상위 버전.
	* MAKEWORD 매크로를 통해서 2.2버전을 쓰겠다고 선언한다.
	* 
	* arg 2 : LPWSADATA *lpWSAData
	* WSADATA 구조체 포인터가 들어감
	* 함수가 반환되면 윈도우 소켓을 위한 세부 정보가 구조체에 포함된다.
	*/
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	/* 소켓 생성 (SOCKET WSAAPI socket() 함수
	* SOCKET WSAAPI socket(
	* int af, //address family를 명시, AF_INET 이용
	* int type, //소켓의 유형으로 SOCKET_STREAM
	* int protocol 
	* )
	* 
	* PF_INET : Protocol Family를 설정(IPv4)
	* SOCK_STREAM : 새로 생생헐 소켓의 타입을 정한다(TCP).
	* IPPROTO_TCP : 실제로 사용할 프로토콜을 최정적으로 결정한다(TCP).
	* 세 번째 인자로 IPPROTO_HOPOPTS를 전달해도 된다.
	* 이 값은 디폴트 0으로 잡혀있다. 이 값을 쓰면 자동으로 TCP 전용 소켓인지 UDP 전용 소켓인지 결정해준다. 
	*/
	SOCKET hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	/*
	* 위에서 생성한 소켓의 포트 번호와 IP 주소, Address Family 정해야 함
	* 이를 위해 SOCKADDR_IN 구조체를 생성 (bind를 하기 위한 준비)
	* 
	* struct sockaddr_in{ 소켓을 연결할 대상의 주소를 쓰는 구조체
	* short sin_family;		//ipv4에서 사용할 주소 체계: AF_INET
	* u_short sin_port;		//포트 번호
	* struct in_addr sin_addr;		//32비트 IP주소
	* char sin_zero[8];		//전체 크기를 16비트로 맞추기 위한 dummy, 사용되지 않음
	* };
	* 
	* struct in_addr{
	* u_long s_addr;		//32비트 IP주소를 저장할 구조체
	* };
	*/
	SOCKADDR_IN tListenAddr = {};

	//AF_INET : IPv4 Address Family를 씀
	tListenAddr.sin_family = AF_INET;

	/*htons(PORT) : 위에서 매크로로 정의한 포트 번호를 설정한다.
	* htons : Host To Network Short의 약자로 
	* Network로 short 형 데이터를 보낼때 바이트 오더를 해주는 함수
	* 네트워크 주소는 CPU 바이트 순서대로 저장되는데, 바이트 오더 방식은 2가지가 있다.
	* 
	* [빅 엔디안]
	* SPARC, RISC CPU 계열에서 사용하는 바이트 오더 방식.
	* 빅 엔디안 방식의 바이트 오더는 큰 단위부터 메모리에 적는다.
	* ex) 0x12345678을 적는다고 하면 큰 단위가 1234 니까 
	* 12, 34, 56, 78 순으로 메모리를 적는다.
	* 이처럼 최고 단위가 앞에 있으므로 부호에 대한 비트 확인이 빨라서
	* 네트워크 통신에서 사용하는 방식이라고 한다.
	* 
	* [리틀 엔디안]
	* 인텔, AMD 계열에서 사용하는 바이트 오더 방식.
	* 대부분의 컴퓨터는 이 계열의 CPU를 사용하므로
	* 네트워크 통신을 위해 htons로 빅 엔디안 방식의 바이트 오더로 전환해야 한다.
	* 이 방식은 적은 단위부터 메모리에 적는 방식이다.
	* ex) 0x12345678을 적는다고 하면 작은 단위가 5678 이니까 
	* 78, 56, 34, 12 순으로 메모리를 적는다.
	* 연산을 할 때 가장 뒷 자리부터 계산하므로 연산이 빠르다는 장점이 있다.
	*/
	tListenAddr.sin_port = htons(PORT); //WinSock2 헤더에 들어있는 함수

	/* bind 준비
	* htonl(INADDR_ANY) : Host To Network Long
	* long 타입 데이터의 바이트 오더를 빅 엔디안 바이트 오더 방식으로 바쿼 줌
	* INADDR_ANY : 컴퓨터에서 존재하는 랜카드 중에서 사용 가능한 IP 주소를 찾아서
	* 자동으로 대입해준다. 서버 입장에서 현재 동작되는 컴퓨터가 서버의 IP 주소이므로
	* INADDR_ANY로 설정해준다.
	*/
	tListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* bind 실행
	* 위에서 지정한 포트 번호, IP 주소를 생성한 Listen Socket에 바인딩한다.
	* 이렇게 바인딩하는 이유는 운영체제가 포트 번호를 관리하기 위함이다. 
	* 만약 서버 소켓의 포트 번호가 다른 소켓과 중복된다고 한다면 
	* 운영체제는 이를 감지하여 에러를 리턴하게 된다.
	*/
	bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));

	/* Listen 실행
	* 위에서 포트 번호와 IP 주소를 바인딩한 서버 소켓을 통해서
	* 클라이언트의 연결 요청이 수신될 때까지 기다린다.
	* 두 번째 인자는 클라이언트 연결 대기열의 크기이다. 여러 클라이언트가 한 번에 들어올 수 있다.
	* 윈도우 소켓2부터 SOMAXCONN이라는 상수값을 지정할 수 있다.
	* 이를 통해 소켓 지정자가 알아서 연결 대기열의 크기를 결정할 수 있게 되었다.
	* 그리고 listen api가 성공적으로 리턴했다고 해서 무조건 연결되어 있는 상태가 아니다.
	* 클라이언트의 최종적인 연결 요청을 받아들이는 함수는 아래에서 사용될 accept 함수이다.
	*/
	listen(hListen, SOMAXCONN);

	char Packet[PACKET_LENGTH] = {};
	
	//서버 시작 메시지 출력
	cout << "서버가 시작되었습니다. 클라이언트를 기다리는 중..." << endl;

	while (true) {//클라이언트의 연결 요청을 계속해서 기다리는 루프

		/* accept 준비
		* 클라이언트의 connect를 accept 하기 전에
		* 미리 새롭게 생성될 소켓의 구조체 정보를 생성해둔다.
		* 클라이언트와 연결이 성공되면 이 구조체의 정보를 이용해서
		* 클라이언트의 주소 정보를 알아낼 수 있다.
		*/
		SOCKADDR_IN tClntAddr = {}; //클라이언트 주소 정보를 담을 구조체
		int iSize = sizeof(tClntAddr);//클라이언트 주소 정보 구조체의 크기

		/* accept 실행
		* accept는 연결지향형 소켓에서 사용하는 함수
		* (비연결지향형 UDP에서는 사용하지 않음)
		* accept는 블로킹 방식으로 처리된다.
		* 블로킹 방식이란 요청이 올 때까지 대기하는 것.
		* (요청이 오기 전까지 이 함수에서 빠져 나올 수 없다.)
		* 클라이언트와 연결이 되면 서로 통신할 새로운 소켓을 반환한다.
		* 이 소켓을 바탕으로 서로 통신을 할 수 있게 된다.
		* 에러가 발생하면 -1 (SOCKET_ERROR)을 리턴하고
		* 성공하면 0보다 큰 파일지정번호가 리턴된다.
		*/
		SOCKET hClientSocket = accept(hListen, (SOCKADDR*)&tClntAddr, &iSize);//클라이언트 연결 요청을 수락하고, 연결된 클라이언트의 소켓을 반환한다.

		if (hClientSocket == SOCKET_ERROR) {
			cout << "클라이언트 연결 실패" << endl;
			continue;
		}

		cout << "클라이언트가 연결되었습니다." << endl;//클라이언트 연결 성공 메시지 출력
		strcpy_s(Packet, "서버와 연결되었습니다.");
		send(hClientSocket, Packet, PACKET_LENGTH, 0);
		thread clientThread(ClientHandler, hClientSocket);//클라이언트마다 별도의 스레드를 생성하여 클라이언트와의 통신을 처리
		/*
		* thread 생성자는 새로운 스레드를 생성할 때 생성자에 첫 번째 인자로 실행할 함수, 두 번째 인자부터는 그 함수에 전달할 파라미터를 받는다.
		* 즉, thread 객체는 새로운 스레드를 생성하고, 그 스레드가 ClientHandler 함수를 hClientSocket을 인자로 하여 실행하도록 설정한다.
		* 새로운 스레드가 시작되면서 ClientHandler(hClientSocket)가 호출되고, 이 함수 내에서 클라이언트와의 통신을 처리하게 된다.
		*/
		


		clientThread.detach();//스레드를 분리(detach)하여 독립적으로 동작하도록 함. 스레드가 끝나면 자동으로 자원 해제
		//스레드가 독립적으로 실행되면서 메인 스레드는 다른 클라이언트를 처리할 수 있게 된다.

		
		while (chatClient(hClientSocket, Packet));
	}



	closesocket(hListen);//리스닝 소켓 종료 (더 이상 연락을 받지 않음)

	//윈도우 소켓을 종료한다.
	WSACleanup();

	return 0;
}