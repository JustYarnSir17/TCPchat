#include <iostream>
#include <WinSock2.h>
#include <thread> // ��Ƽ �����带 ����ϱ� ���� �߰�


using namespace std;

// ������ ������ ����� ���� ���� ���̺귯���� �߰��Ѵ�.
#pragma comment(lib, "ws2_32.lib")

#define PORT 7890	//����� ��Ʈ ��ȣ�� 7890���� ����
#define PACKET_LENGTH 1024	//�� ���� �ְ� ���� ��Ŷ�� �ִ� ���̸� 1024 ����Ʈ�� ����

//Ŭ���̾�Ʈ ó�� �Լ�(�����忡�� ����)
//Ŭ���̾�Ʈ�� �޽����� �ְ� �޴� ����� �����忡�� ������ �� �ְ� �Լ��� �и�
void ClientHandler(SOCKET clientSocket) {
	char Packet[PACKET_LENGTH] = {};//������ ������ ��Ŷ ���� �ʱ�ȭ

	while (true) {//���� ������ ���ؼ� ���������� Ŭ���̾�Ʈ �޽����� �ְ� ����
		//Ŭ���̾�Ʈ�κ��� �޽����� ����
		int recvSize = recv(clientSocket, Packet, PACKET_LENGTH, 0);
		if (recvSize == SOCKET_ERROR || recvSize == 0) {//���� ���� �Ǵ� Ŭ���̾�Ʈ�� ���� ���� ��
			cout << "Ŭ���̾�Ʈ ���� ����" << endl;
			break;
		}

		Packet[recvSize] = '\0';//���� �޽��� ���� null ���ڸ� �߰��ؼ� ���ڿ��� ������
		cout << "Client : " << Packet << endl;//Ŭ���̾�Ʈ�κ��� ���� �޽����� �ֿܼ� ���

		//Ŭ���̾�Ʈ�� "exit"��� �޽����� ������ �������� ������ �����Ѵ�.
		if (strcmp(Packet, "exit") == 0) {
			cout << "Ŭ���̾�Ʈ�� ���� ��û�� ���½��ϴ�." << endl;
			break;
		}


		//��Ŷ ���۸� ���� �����͸� ���� �����ϰ� �ʱ�ȭ
		memset(Packet, 0, sizeof(Packet));



	}
		//Ŭ���̾�Ʈ ������ �ݾƼ� ������ �����Ѵ�.
		closesocket(clientSocket);

}


bool chatClient(SOCKET hClientSocket,char *Packet) {
	cin.getline(Packet, PACKET_LENGTH);//����� �Է��� ����

	//"exit" �Է� �� ������ �޽����� ������ ����
	if (strcmp(Packet, "exit") == 0) {
		send(hClientSocket, Packet, strlen(Packet), 0);
		return false;
	}

	send(hClientSocket, Packet, strlen(Packet), 0);//������ �޽����� ����
	return true;
}

int main() {
	/*
	* struct WSAData{
	* WORD wVersion;	//������ ������ ����
	* WORD wHighVersion;	//�����Ǵ� ������ ���� �������� wVersion�� ��ġ
	* char szDescription[WSADESCRIPTION_LEN+1];		//NULL�� ������ �ƽ�Ű ��Ʈ�� ��, ������ ���� ����
	* char szSystemStatus[WSASYS_STATUS_LEN+1];		//NULL�� ������ �ƽ�Ű ��Ʈ�� ��, ���� ���ڿ�
	* unsigned short iMaxSockets;	//����� ������ �ִ� ���� ��, version 2 ���ʹ� ����
	* unsigned short iMaxUdpDg;		//������ �� �ִ� ������ �׷��� �ִ� ũ��, version 2 ���ʹ� ����
	* char FAR * lpVendorinfo;		//���� ����(�ǹ̾���)
	*/
	WSADATA wsaData; //������ ���� �ʱ�ȭ ������ ������ �ִ� ����ü

	/*
		WSAStartup(������ ���� �ʱ�ȭ) ~ WSACleanup(������ ���� ����) ���̿�
		������ ���� �ڵ带 �ۼ��Ѵ�.
	*/

	/*
	* ������ ���� �ʱ�ȭ (WSAStartup() �Լ�)
	* int WSAStartup(
	* WORD wVersionRequired,
	* LPWSADATA lpWSAData
	* );
	* 
	* arg 1 : WORD  *wVersionRequested
	* ���α׷��� �䱸�ϴ� winsock ���� �� �ֻ��� ����.
	* MAKEWORD ��ũ�θ� ���ؼ� 2.2������ ���ڴٰ� �����Ѵ�.
	* 
	* arg 2 : LPWSADATA *lpWSAData
	* WSADATA ����ü �����Ͱ� ��
	* �Լ��� ��ȯ�Ǹ� ������ ������ ���� ���� ������ ����ü�� ���Եȴ�.
	*/
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	/* ���� ���� (SOCKET WSAAPI socket() �Լ�
	* SOCKET WSAAPI socket(
	* int af, //address family�� ���, AF_INET �̿�
	* int type, //������ �������� SOCKET_STREAM
	* int protocol 
	* )
	* 
	* PF_INET : Protocol Family�� ����(IPv4)
	* SOCK_STREAM : ���� ������ ������ Ÿ���� ���Ѵ�(TCP).
	* IPPROTO_TCP : ������ ����� ���������� ���������� �����Ѵ�(TCP).
	* �� ��° ���ڷ� IPPROTO_HOPOPTS�� �����ص� �ȴ�.
	* �� ���� ����Ʈ 0���� �����ִ�. �� ���� ���� �ڵ����� TCP ���� �������� UDP ���� �������� �������ش�. 
	*/
	SOCKET hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	/*
	* ������ ������ ������ ��Ʈ ��ȣ�� IP �ּ�, Address Family ���ؾ� ��
	* �̸� ���� SOCKADDR_IN ����ü�� ���� (bind�� �ϱ� ���� �غ�)
	* 
	* struct sockaddr_in{ ������ ������ ����� �ּҸ� ���� ����ü
	* short sin_family;		//ipv4���� ����� �ּ� ü��: AF_INET
	* u_short sin_port;		//��Ʈ ��ȣ
	* struct in_addr sin_addr;		//32��Ʈ IP�ּ�
	* char sin_zero[8];		//��ü ũ�⸦ 16��Ʈ�� ���߱� ���� dummy, ������ ����
	* };
	* 
	* struct in_addr{
	* u_long s_addr;		//32��Ʈ IP�ּҸ� ������ ����ü
	* };
	*/
	SOCKADDR_IN tListenAddr = {};

	//AF_INET : IPv4 Address Family�� ��
	tListenAddr.sin_family = AF_INET;

	/*htons(PORT) : ������ ��ũ�η� ������ ��Ʈ ��ȣ�� �����Ѵ�.
	* htons : Host To Network Short�� ���ڷ� 
	* Network�� short �� �����͸� ������ ����Ʈ ������ ���ִ� �Լ�
	* ��Ʈ��ũ �ּҴ� CPU ����Ʈ ������� ����Ǵµ�, ����Ʈ ���� ����� 2������ �ִ�.
	* 
	* [�� �����]
	* SPARC, RISC CPU �迭���� ����ϴ� ����Ʈ ���� ���.
	* �� ����� ����� ����Ʈ ������ ū �������� �޸𸮿� ���´�.
	* ex) 0x12345678�� ���´ٰ� �ϸ� ū ������ 1234 �ϱ� 
	* 12, 34, 56, 78 ������ �޸𸮸� ���´�.
	* ��ó�� �ְ� ������ �տ� �����Ƿ� ��ȣ�� ���� ��Ʈ Ȯ���� ����
	* ��Ʈ��ũ ��ſ��� ����ϴ� ����̶�� �Ѵ�.
	* 
	* [��Ʋ �����]
	* ����, AMD �迭���� ����ϴ� ����Ʈ ���� ���.
	* ��κ��� ��ǻ�ʹ� �� �迭�� CPU�� ����ϹǷ�
	* ��Ʈ��ũ ����� ���� htons�� �� ����� ����� ����Ʈ ������ ��ȯ�ؾ� �Ѵ�.
	* �� ����� ���� �������� �޸𸮿� ���� ����̴�.
	* ex) 0x12345678�� ���´ٰ� �ϸ� ���� ������ 5678 �̴ϱ� 
	* 78, 56, 34, 12 ������ �޸𸮸� ���´�.
	* ������ �� �� ���� �� �ڸ����� ����ϹǷ� ������ �����ٴ� ������ �ִ�.
	*/
	tListenAddr.sin_port = htons(PORT); //WinSock2 ����� ����ִ� �Լ�

	/* bind �غ�
	* htonl(INADDR_ANY) : Host To Network Long
	* long Ÿ�� �������� ����Ʈ ������ �� ����� ����Ʈ ���� ������� ���� ��
	* INADDR_ANY : ��ǻ�Ϳ��� �����ϴ� ��ī�� �߿��� ��� ������ IP �ּҸ� ã�Ƽ�
	* �ڵ����� �������ش�. ���� ���忡�� ���� ���۵Ǵ� ��ǻ�Ͱ� ������ IP �ּ��̹Ƿ�
	* INADDR_ANY�� �������ش�.
	*/
	tListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* bind ����
	* ������ ������ ��Ʈ ��ȣ, IP �ּҸ� ������ Listen Socket�� ���ε��Ѵ�.
	* �̷��� ���ε��ϴ� ������ �ü���� ��Ʈ ��ȣ�� �����ϱ� �����̴�. 
	* ���� ���� ������ ��Ʈ ��ȣ�� �ٸ� ���ϰ� �ߺ��ȴٰ� �Ѵٸ� 
	* �ü���� �̸� �����Ͽ� ������ �����ϰ� �ȴ�.
	*/
	bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));

	/* Listen ����
	* ������ ��Ʈ ��ȣ�� IP �ּҸ� ���ε��� ���� ������ ���ؼ�
	* Ŭ���̾�Ʈ�� ���� ��û�� ���ŵ� ������ ��ٸ���.
	* �� ��° ���ڴ� Ŭ���̾�Ʈ ���� ��⿭�� ũ���̴�. ���� Ŭ���̾�Ʈ�� �� ���� ���� �� �ִ�.
	* ������ ����2���� SOMAXCONN�̶�� ������� ������ �� �ִ�.
	* �̸� ���� ���� �����ڰ� �˾Ƽ� ���� ��⿭�� ũ�⸦ ������ �� �ְ� �Ǿ���.
	* �׸��� listen api�� ���������� �����ߴٰ� �ؼ� ������ ����Ǿ� �ִ� ���°� �ƴϴ�.
	* Ŭ���̾�Ʈ�� �������� ���� ��û�� �޾Ƶ��̴� �Լ��� �Ʒ����� ���� accept �Լ��̴�.
	*/
	listen(hListen, SOMAXCONN);

	char Packet[PACKET_LENGTH] = {};
	
	//���� ���� �޽��� ���
	cout << "������ ���۵Ǿ����ϴ�. Ŭ���̾�Ʈ�� ��ٸ��� ��..." << endl;

	while (true) {//Ŭ���̾�Ʈ�� ���� ��û�� ����ؼ� ��ٸ��� ����

		/* accept �غ�
		* Ŭ���̾�Ʈ�� connect�� accept �ϱ� ����
		* �̸� ���Ӱ� ������ ������ ����ü ������ �����صд�.
		* Ŭ���̾�Ʈ�� ������ �����Ǹ� �� ����ü�� ������ �̿��ؼ�
		* Ŭ���̾�Ʈ�� �ּ� ������ �˾Ƴ� �� �ִ�.
		*/
		SOCKADDR_IN tClntAddr = {}; //Ŭ���̾�Ʈ �ּ� ������ ���� ����ü
		int iSize = sizeof(tClntAddr);//Ŭ���̾�Ʈ �ּ� ���� ����ü�� ũ��

		/* accept ����
		* accept�� ���������� ���Ͽ��� ����ϴ� �Լ�
		* (�񿬰������� UDP������ ������� ����)
		* accept�� ���ŷ ������� ó���ȴ�.
		* ���ŷ ����̶� ��û�� �� ������ ����ϴ� ��.
		* (��û�� ���� ������ �� �Լ����� ���� ���� �� ����.)
		* Ŭ���̾�Ʈ�� ������ �Ǹ� ���� ����� ���ο� ������ ��ȯ�Ѵ�.
		* �� ������ �������� ���� ����� �� �� �ְ� �ȴ�.
		* ������ �߻��ϸ� -1 (SOCKET_ERROR)�� �����ϰ�
		* �����ϸ� 0���� ū ����������ȣ�� ���ϵȴ�.
		*/
		SOCKET hClientSocket = accept(hListen, (SOCKADDR*)&tClntAddr, &iSize);//Ŭ���̾�Ʈ ���� ��û�� �����ϰ�, ����� Ŭ���̾�Ʈ�� ������ ��ȯ�Ѵ�.

		if (hClientSocket == SOCKET_ERROR) {
			cout << "Ŭ���̾�Ʈ ���� ����" << endl;
			continue;
		}

		cout << "Ŭ���̾�Ʈ�� ����Ǿ����ϴ�." << endl;//Ŭ���̾�Ʈ ���� ���� �޽��� ���
		strcpy_s(Packet, "������ ����Ǿ����ϴ�.");
		send(hClientSocket, Packet, PACKET_LENGTH, 0);
		thread clientThread(ClientHandler, hClientSocket);//Ŭ���̾�Ʈ���� ������ �����带 �����Ͽ� Ŭ���̾�Ʈ���� ����� ó��
		/*
		* thread �����ڴ� ���ο� �����带 ������ �� �����ڿ� ù ��° ���ڷ� ������ �Լ�, �� ��° ���ں��ʹ� �� �Լ��� ������ �Ķ���͸� �޴´�.
		* ��, thread ��ü�� ���ο� �����带 �����ϰ�, �� �����尡 ClientHandler �Լ��� hClientSocket�� ���ڷ� �Ͽ� �����ϵ��� �����Ѵ�.
		* ���ο� �����尡 ���۵Ǹ鼭 ClientHandler(hClientSocket)�� ȣ��ǰ�, �� �Լ� ������ Ŭ���̾�Ʈ���� ����� ó���ϰ� �ȴ�.
		*/
		


		clientThread.detach();//�����带 �и�(detach)�Ͽ� ���������� �����ϵ��� ��. �����尡 ������ �ڵ����� �ڿ� ����
		//�����尡 ���������� ����Ǹ鼭 ���� ������� �ٸ� Ŭ���̾�Ʈ�� ó���� �� �ְ� �ȴ�.

		
		while (chatClient(hClientSocket, Packet));
	}



	closesocket(hListen);//������ ���� ���� (�� �̻� ������ ���� ����)

	//������ ������ �����Ѵ�.
	WSACleanup();

	return 0;
}