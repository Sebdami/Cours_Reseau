// TD1 Reseau Server.cpp : définit le point d'entrée pour l'application console.
//
#include "stdafx.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib") // Indicates to the linker that this file is needed

#define DEFAULT_PORT "54323"
#define DEFAULT_BUFLEN 512 //Default length of the data transfer buffer 

int main()
{
	// Winsock Sartup
	WSAData wsaData;
	int i = WSAStartup(MAKEWORD(2, 1), &wsaData);
	if (i != 0) // If it returns anything but zero, something has failed 
	{
		std::cout << "Winsock Startup failed : " << i << std::endl;
		return 1;
	}


	struct addrinfo *result = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM; // This socket will be a stream socket
	hints.ai_protocol = IPPROTO_TCP; // We will be using TCP
	hints.ai_flags = AI_PASSIVE; 

	// Determine the local address and port the server will be using
	i = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (i != 0) {
		std::cout << "getaddrinfo failed: " << i << std::endl;
		WSACleanup();
		return 1;
	}
	// Create a Socket with the gathered information 
	// This socket will be used to listen for client connections
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	
	// Check for errors once again
	if (ListenSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		freeaddrinfo(result); //Free the reserved addr
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	i = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (i == SOCKET_ERROR) {
		std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	//The socket is bound, we don't need the info from the previous getadddrinfo() anymore
	freeaddrinfo(result);

	// We now need to listen on the socket
	// We pass the created socket and the number for the maximum length of the queue of pending connections to accept
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "Listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET ClientSocket; // Temporary socket to accept connection
	ClientSocket = INVALID_SOCKET;

	// Accept a client socket, only work for one client since we're not using multi threads or any technique to handle several connections
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		std::cout << "accept failed: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// We no longer need the server socket since the connection has been established
	closesocket(ListenSocket);

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) { // If iResult > 0 we received data
			std::cout << "Bytes received: " << iResult << std::endl;
			std::cout << "Message received: ";
			std::cout.write(recvbuf, iResult);
			std::cout << std::endl;

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				std::cout << "send failed: " << WSAGetLastError() << std::endl;
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			std::cout << "Bytes sent: " << iSendResult << std::endl;
			std::cout << "Message sent: ";
			std::cout.write(recvbuf, iResult);
			std::cout << std::endl;
		}
		else if (iResult == 0) // If it is equal to 0 there is nothing more to get, close the connection
			std::cout << "Connection closing..." << std::endl;
		else { // If iResult is negative, something went wrong
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// Shutdown the connection 
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// Cleanup
	closesocket(ClientSocket);
	WSACleanup();

    return 0;
}

