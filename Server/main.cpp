#include<stdio.h>
#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>

#pragma comment (lib,"ws2_32.lib")

#define PORT "5150"

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
	printf("Aguardando por conexoes\n");

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Erro ao escutar por conexoes\n");
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//to receive client connections we need an extra socket
	sockaddr clientAddr;
	SOCKET clientSocket = INVALID_SOCKET;
	int clientSize = sizeof(clientAddr);

	clientSocket = accept(listenSocket, &clientAddr, &clientSize);

	if (clientSocket == INVALID_SOCKET) {
		printf("Falha ao aceitar conexao\n");
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	//at this point connections to the ip and port specified on the listenSocket should be accepted
	//here i will add code to discover connected client's name and send messages between the server and the client
	char buf[255];
	char clientName[255];
	ZeroMemory(&clientName, 255);
	ZeroMemory(&buf, 255);

	if (getnameinfo(&clientAddr, clientSize, buf, 255, clientName, 255, 0) != 0) {
		closesocket(listenSocket);
		closesocket(clientSocket);
		WSACleanup();
		printf("Nao pode resolver nome do client conectado\n");
		return 1;
	}
	std::cout << buf << " se conectou ao servidor\n";


	char inputBuffer[255];
	char recvBuffer[255];
	char in;
	ZeroMemory(&inputBuffer, 255);
	ZeroMemory(&recvBuffer, 255);

	printf("Digite uma mensagem para o cliente\n");

	std::cin >> inputBuffer;
	send(clientSocket, inputBuffer, 255, 0);


	printf("Aguardando resposta do cliente\n");
	recv(clientSocket, recvBuffer, 255, 0);
	std::cout << recvBuffer;

	std::cin >> in;

	return 0;
}