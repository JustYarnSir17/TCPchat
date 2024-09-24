/*���־� ��Ʃ��� ���� �������� error c4996 : inet_addr�� ���� ������ �߻��ϸ�
* �Ʒ� define�� �߰��� �ذ��� �� �ִ�.
* �Ǵ� <WS2tcpip.h> ����� �߰� �� inet_addr ��ſ� inet_pton API�� ����ϸ� �ȴ�.
*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include <WinSock2.h>
#include <iostream>
#include <thread> //��Ƽ �����带 ����ϱ� ���� �߰�

using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define PORT 7890	//������ ����� ��Ʈ ��ȣ
#define PACKET_LENGTH 1024	//��Ŷ ũ��

/*�����ʿ��� INADDR_ANY�� ����� ��ǻ�� IP��
* �����ϱ� ���� "127.0.0.1"�� IP�� �����Ѵ�.
* (���� ��ǻ���� IP �ּ�)
*/
#define LOCAL_IP "127.0.0.1"	//���� ȣ��Ʈ IP(������ Ŭ���̾�Ʈ�� ���� ��ǻ�Ϳ� ���)

/* �޽��� ���� �Լ� (������ �����忡�� �����Ѵ�.)
* �����κ��� ���������� �޽����� �޴� ����
* recv�� ���ŷ �Լ��̱� ������ �� �Լ��� ��� �����κ��� �����͸� ��ٸ���.
*/

void ReceiveMessages(SOCKET hSocket) {
	char Packet[PACKET_LENGTH] = {};


	while (true) {
		//�����κ��� �޽��� ����
		int recvSize = recv(hSocket, Packet, PACKET_LENGTH, 0);
		if (recvSize == SOCKET_ERROR || recvSize == 0) {//������ �߻��ϰų� �������� ������ �������� ��
			cout << "���� ���� ����" << endl;
			break; // ���� ������ �����ϰ� ������ ����
		}

		Packet[recvSize] = '\0';//���� �޽��� ���� null���ڸ� �߰��Ͽ� ���ڿ��� ����
		cout << "Server : " << Packet << endl;//�����κ��� ���� �޽����� ���

		//���� ������ "extI" �޽����� ���´ٸ�, ���� ����
		if (strcmp(Packet, "exit") == 0) {
			cout << "������ ���Ḧ ��û�߽��ϴ�." << endl;
			break;
		}

		memset(Packet, 0, sizeof(Packet)); //��Ŷ ���۸� �ʱ�ȭ��

	}
}



int main() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	/* create Socket
	* ������ ���� ��û�� ���� ������ �����Ѵ�.
	* TCP ���� ������ ���������Ƿ� �Ȱ��� �����ش�.
	* ������ �� ��° ���ڴ� IPPROTO_HOPOPTS(0)���� �����ص� �ȴ�.
	*/
	SOCKET hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tAddr = {};
	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(PORT);
	tAddr.sin_addr.s_addr = inet_addr(LOCAL_IP);

	/* conect ��û
	* �ü���� ���� ��Ʈ ��ȣ�� IP �ּҰ� ���ε� �� ���� ���Ͽ� ������ ��û�Ѵ�.
	* �� �Լ��� ���ŷ ������� ó���ȴ�.
	* ���� ������ Ŭ���̾�Ʈ�� ���� ��û�� ���� ����, ����, �Ǵ� �ð� �ʰ�� ����
	* ������ �Ǳ� ������ �� �Լ����� �������� �� ����.
	* (�� �Լ��� ����Ǹ� �ٷ� ���ϵ��� ���� �� �ִ�.)
	*/
	connect(hSocket, (SOCKADDR*)&tAddr, sizeof(tAddr));
	/* recv
	* ���� ��û�� �����Ǿ��� �� �����κ��� ������ �Ϸ� �Ǿ��ٴ� Ȯ�ο� ��Ŷ�� �޴´�.
	* recv API�� ��Ŷ�� ���� ��, send API�� ��Ŷ�� ������ �� ���� �Լ���.
	* send�� recv ��� ���ŷ ������� ó���ȴ�.
	* ����, ����, ���ᰡ �����Ǳ� ������ �Լ����� �������� �� ���ٴ� ��
	*
	* ���� recv�� send�� �޸� ������ �����忡�� ó���ϴ� ����� Ȱ���Ѵ�.
	* send�� �����͸� ������ ��ü�� �ڰ� �ڽ��̱� ������ �󸶸�ŭ�� ��Ŷ�� ������ ��
	* ���� �����ؾ� �� �� �� �� ������ recv�� ����� � �����͸� ���� ������ ���� ����
	* Ư���� ������ �� �� ����. �̸� �ذ��ϱ� ���� ������ ������ ���� recv API�� ������ �����忡�� �ذ��Ѵ�.
	* ���ϰ� ������ �Ϸ�Ǹ� ���ο� �����带 �����ϰ� �װ����� recv�� ����
	* �����Ͱ� ���ŵǱ� ��ٸ��� ������� ó���� �� �ִ�.
	*/
	char Packet[PACKET_LENGTH] = {};
	recv(hSocket, Packet, PACKET_LENGTH, 0);

	// ���� �������κ��� ���� ��Ŷ �޽����� ����Ѵ�.
	cout << "Server : " << Packet << endl;

	thread recvThread(ReceiveMessages, hSocket);
	recvThread.detach();
	while (true) {//���� �����忡���� ����� �Է��� �޾Ƽ� ������ �����ϴ� ������ ����
		cout << "Message To Server : ";
		cin.getline(Packet, PACKET_LENGTH);//����� �Է��� ����

		//"exit" �Է� �� ������ �޽����� ������ ����
		if (strcmp(Packet, "exit") == 0) {
			send(hSocket, Packet, strlen(Packet), 0);
			break;
		}

		send(hSocket, Packet, strlen(Packet), 0);//������ �޽����� ����
	}

	closesocket(hSocket);//����� ���� ���� ���ҽ� ��ȯ

	//������ ���� ����
	WSACleanup();

	return 0;
}