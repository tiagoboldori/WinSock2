#include<stdio.h>
#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>

#pragma comment (lib,"ws2_32.lib")

#define PORT "5150"
SOCKET socketPool[100];
int cont = 0;

static void socketHandler(SOCKET clientSocket, sockaddr clientAddr) {
	while (true) {
		char receiveBuf[255];
		ZeroMemory(&receiveBuf,255);
		int bytesReceived = recv(clientSocket, receiveBuf, 255, 0);
		if (bytesReceived <= 0) {
			printf("Erro ao receber dados ou conexao fechada\n");
			closesocket(clientSocket);
			return;
		}
		char bufClient[255];
		char clientName[255];
		int clientSize = sizeof(clientAddr);
		getnameinfo(&clientAddr, clientSize, bufClient, 255, clientName, 255, 0);

		printf("Mensagem de %s: %s\n", bufClient, receiveBuf);

		/*
		montando os bytes para troca de mensagens:
		bytes:
		[0,1,2,3,4,5, ...,n]

		nome do usuario ate 20 bytes
		*/

		int userNameSize = 20;
		char sendBuf[255];
		ZeroMemory(&sendBuf, 255);
		for(int i =0 ; i < userNameSize; i++) {
			if (bufClient[i] == '\0') {
				userNameSize = i;
				break;
			}
			sendBuf[i] = bufClient[i];
		}
		
		sendBuf[userNameSize] = ':';
		userNameSize++;

		for(int i = userNameSize; i < 255; i++) {
			sendBuf[i] = receiveBuf[i-userNameSize];
		}
		std::cout << sendBuf << std::endl;

		for(int i=0; i < cont; i++) {
			if(socketPool[i] != clientSocket) {
				send(socketPool[i], sendBuf, bytesReceived, 0);
			}
		}
	}	
}

int main() {
	//initializing Winsock
	WSAData wsaData;
	WORD version = MAKEWORD(2, 2);
	int isOk = WSAStartup(version, &wsaData);

	if (isOk != 0) {
		printf("Erro ao initializar winsock \n");
		return 1;
	}
	printf("Winsock inicializado!\n");

	//Creating SOCKET
	SOCKET listenSocket = INVALID_SOCKET;

	addrinfo* info = NULL, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	int iResult = getaddrinfo(NULL, PORT, &hints, &info);
	if (iResult != 0) {
		printf("Getaddrinfo failed!\n");
		WSACleanup();
		return 1;
	}

	listenSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

	if (listenSocket == INVALID_SOCKET) {
		printf("Erro ao criar socket\n");
		freeaddrinfo(info);
		WSACleanup();
		return 1;
	}
	printf("Socket criada!\n");

	//binding socket
	iResult = bind(listenSocket, info->ai_addr, (int)info->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("Erro ao bindar socket");
		freeaddrinfo(info);
		WSACleanup();
	}
	printf("Socket bindada\n");

	//listening for connections
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Erro ao escutar por conexoes\n");
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}	
	printf("Socket escutando por conexoes\n");


	//to receive client connections we need an extra socket
	sockaddr clientAddr;
	SOCKET clientSocket = INVALID_SOCKET;
	int clientSize = sizeof(clientAddr);

	//accept func stops the main program execution and waits for incoming connections

	while (true) {
		clientSocket = accept(listenSocket, &clientAddr, &clientSize);

		if (clientSocket == INVALID_SOCKET) {
			printf("Falha ao aceitar conexao\n");
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}
		else {
			socketPool[cont++] = clientSocket;
			std::thread t(socketHandler,clientSocket,clientAddr);
			t.detach();
		}
		clientSocket = INVALID_SOCKET;
	}
	
	return 0;
}