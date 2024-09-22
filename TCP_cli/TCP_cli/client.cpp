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

#define PORT 7890
#define PACKET_LENGTH 1024

/*�����ʿ��� INADDR_ANY�� ����� ��ǻ�� IP��
* �����ϱ� ���� "127.0.0.1"�� IP�� �����Ѵ�.
* (���� ��ǻ���� IP �ּ�)
*/
#define LOCAL_IP "127.0.0.1"

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

	//���� ���Ͽ� ��Ŷ�� ������ �޽��� �Է�
	strcpy_s(Packet, "���� ����");

	/* send
	* ���� ���Ͽ� ��Ŷ "���� ����" �޽����� �����Ѵ�.
	*/
	send(hSocket, Packet, strlen(Packet), 0);

	//����� ����� ���� ���� ���ҽ��� ��ȯ�Ѵ�.
	closesocket(hSocket);

	//������ ���� ����
	WSACleanup();

	return 0;
}