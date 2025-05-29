
#include<iostream>
#include<stdio.h>
#include<thread>
#include<WinSock2.h>
#include<WS2tcpip.h>
#pragma comment (lib,"ws2_32.lib")


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "5150"
void receiver(SOCKET sock) {
	while (true) {
		char buf[255];
		recv(sock, buf, 255, 0);

		printf("%s \n", buf);

	}
}


int main() {
	
	//initializing Winsock2
	WSADATA wsaData;
	WORD version;
	version = MAKEWORD(2, 2);
	int isOk = WSAStartup(version, &wsaData);

	if (isOk != 0) {
		printf("Erro ao inicializar winsock\n");
		return 1;
	}
	printf("Winsock inicializado!\n");



	//creating client socket so the client can connect to the server
	addrinfo hints, * serverInfo;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	//getting server info so we can connect
	getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &serverInfo);


	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);

	if (clientSocket == INVALID_SOCKET) {
		printf("Erro ao criar socket\n");
		return 1;
	}

	//connecting to the server with the info we got through getaddrinfo()
	isOk = connect(clientSocket, serverInfo->ai_addr, (int)serverInfo->ai_addrlen);
	if (isOk != 0) {
		printf("Erro ao conectar\n");
		return 1;
	}
	printf("Conectado ao servidor!\n");



	char rcvBuffer[255];
	char inputBuffer[255];
	char in;

	ZeroMemory(&rcvBuffer, 255);
	ZeroMemory(&inputBuffer, 255);

	/*
	printf("Esperando resposta do servidor\n");
	recv(clientSocket, rcvBuffer, 255, 0);
	std::cout << rcvBuffer << "\n";
	*/
	
	while(true){
		std::thread t(receiver, clientSocket);
		t.detach();
		printf("Envie uma mensagem ao servidor\n");
		std::cin >> inputBuffer;
		send(clientSocket, inputBuffer, 255, 0);
	}
	

	std::cin >> in;
	return 0;
}