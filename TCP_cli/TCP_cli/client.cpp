/*비주얼 스튜디오 상위 버전에서 error c4996 : inet_addr의 보안 오류가 발생하면
* 아래 define을 추가해 해결할 수 있다.
* 또는 <WS2tcpip.h> 헤더를 추가 후 inet_addr 대신에 inet_pton API를 사용하면 된다.
*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include <WinSock2.h>
#include <iostream>
#include <thread> //멀티 쓰레드를 사용하기 위해 추가

using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define PORT 7890	//서버와 통신할 포트 번호
#define PACKET_LENGTH 1024	//패킷 크기

/*서버쪽에서 INADDR_ANY로 잡아준 컴퓨터 IP에
* 접속하기 위해 "127.0.0.1"의 IP를 지정한다.
* (같은 컴퓨터의 IP 주소)
*/
#define LOCAL_IP "127.0.0.1"	//로컬 호스트 IP(서버와 클라이언트가 같은 컴퓨터에 경우)

/* 메시지 수신 함수 (별도의 쓰레드에서 동작한다.)
* 서버로부터 지속적으로 메시지를 받는 역할
* recv는 블로킹 함수이기 때문에 이 함수는 계속 서버로부터 데이터를 기다린다.
*/

void ReceiveMessages(SOCKET hSocket) {
	char Packet[PACKET_LENGTH] = {};


	while (true) {
		//서버로부터 메시지 수신
		int recvSize = recv(hSocket, Packet, PACKET_LENGTH, 0);
		if (recvSize == SOCKET_ERROR || recvSize == 0) {//에러가 발생하거나 서버에서 연결을 종료했을 때
			cout << "서버 연결 종료" << endl;
			break; // 수신 루프를 종료하고 연결을 끊음
		}

		Packet[recvSize] = '\0';//받은 메시지 끝에 null문자를 추가하여 문자열을 종료
		cout << "Server : " << Packet << endl;//서버로부터 받은 메시지를 출력

		//만약 서버가 "extI" 메시지를 보냈다면, 연결 종료
		if (strcmp(Packet, "exit") == 0) {
			cout << "서버가 종료를 요청했습니다." << endl;
			break;
		}

		memset(Packet, 0, sizeof(Packet)); //패킷 버퍼를 초기화함

	}
}



int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	/* create Socket
	* 서버에 접속 요청을 위한 소켓을 생성한다.
	* TCP 전용 소켓을 생성했으므로 똑같이 맞춰준다.
	* 마지막 세 번째 인자는 IPPROTO_HOPOPTS(0)으로 전달해도 된다.
	*/
	SOCKET hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tAddr = {};
	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(PORT);
	tAddr.sin_addr.s_addr = inet_addr(LOCAL_IP);

	/* conect 요청
	* 운영체제를 통해 포트 번호와 IP 주소가 바인딩 된 서버 소켓에 접속을 요청한다.
	* 이 함수도 블로킹 방식으로 처리된다.
	* 서버 소켓이 클라이언트의 연결 요청에 대한 성공, 거절, 또는 시간 초과등에 대한
	* 결정이 되기 전까지 이 함수에서 빠져나올 수 없다.
	* (이 함수가 실행되면 바로 리턴되지 않을 수 있다.)
	*/
	connect(hSocket, (SOCKADDR*)&tAddr, sizeof(tAddr));
	/* recv
	* 연결 요청이 성공되었을 때 서버로부터 연결이 완료 되었다는 확인용 패킷을 받는다.
	* recv API는 패킷을 받을 때, send API는 패킷을 전송할 때 쓰는 함수다.
	* send와 recv 모두 블로킹 방식으로 처리된다.
	* 성공, 실패, 종료가 결정되기 전까지 함수에서 빠져나올 수 없다는 것
	*
	* 보통 recv는 send와 달리 별도의 스레드에서 처리하는 방식을 활용한다.
	* send는 데이터를 보내는 주체가 자가 자신이기 때문에 얼마만큼의 패킷을 전송할 지
	* 언제 전송해야 할 지 알 수 있지만 recv는 대상이 어떤 데이터를 언제 수신할 지에 대한
	* 특정한 시점을 알 수 없다. 이를 해결하기 위해 데이터 수신을 위한 recv API는 별도의 스레드에서 해결한다.
	* 소켓과 연결이 완료되면 새로운 스레드를 생성하고 그곳에서 recv를 통해
	* 데이터가 수신되길 기다리는 방식으로 처리할 수 있다.
	*/
	char Packet[PACKET_LENGTH] = {};
	recv(hSocket, Packet, PACKET_LENGTH, 0);

	// 서버 소켓으로부터 받은 패킷 메시지를 출력한다.
	cout << "Server : " << Packet << endl;

	thread recvThread(ReceiveMessages, hSocket);
	recvThread.detach();
	while (true) {//메인 쓰레드에서는 사용자 입력을 받아서 서버로 전송하는 역할을 수행
		cout << "Message To Server : ";
		cin.getline(Packet, PACKET_LENGTH);//사용자 입력을 받음

		//"exit" 입력 시 서버에 메시지를 보내고 종료
		if (strcmp(Packet, "exit") == 0) {
			send(hSocket, Packet, strlen(Packet), 0);
			break;
		}

		send(hSocket, Packet, strlen(Packet), 0);//서버에 메시지를 전송
	}

	closesocket(hSocket);//사용이 끝난 소켓 리소스 반환

	//윈도우 소켓 해제
	WSACleanup();

	return 0;
}