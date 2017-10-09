// TD1 Reseau.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib") // Indicates to the linker that this file is needed

#define DEFAULT_PORT "54323"
#define DEFAULT_BUFLEN 512

int main(int argc, char* argv[])
{
	// Winsock Sartup
	WSAData wsaData;
	int i = WSAStartup(MAKEWORD(2, 1), &wsaData);
	if (i != 0) // If it returns anything but zero, something has failed 
	{
		std::cout << "Winsock Startup failed : " << i << std::endl;
		return 1;
	}


	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Unspecified family (IPv4, IPv6...)
	hints.ai_socktype = SOCK_STREAM; // This socket will be a stream socket
	hints.ai_protocol = IPPROTO_TCP; // We will be using TCP

	// Determine the local address and port the server will be using
	i = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (i != 0) {
		std::cout << "getaddrinfo failed: " << i << std::endl;
		WSACleanup();
		return 1;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;

	// Attempt to connect to an address until one succeeds
	ptr = result;
	bool connected = false;
	while ( ptr != NULL && !connected) // This loop is useless if we only want to try to connect to the first adress returned by getaddrinfo
	{

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return 1;
		}

		// Connect to server.
		i = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (i == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			//Couldn't connect to this address, try the next one
			ptr = ptr->ai_next;
		}
		else
		{
			connected = true;
		}
	}

	freeaddrinfo(result); // We don't need this info anymore

	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Unable to connect to server!" << std::endl;
		WSACleanup();
		return 1;
	}

	// Sending data

	char *sendbuf = "Hello, it's me."; // The data to send to the server
	char recvbuf[DEFAULT_BUFLEN]; // To receive the answer from the server

	i = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (i == SOCKET_ERROR) {
		std::cout << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	std::cout << "Bytes Sent: " << i << std::endl;
	std::cout << "Message sent: ";
	std::cout.write(sendbuf, i);
	std::cout << std::endl;

	// Shutdown the connection for sending since no more data will be sent
	// We can still use the ConnectSocket for receiving data
	i = shutdown(ConnectSocket, SD_SEND);
	if (i == SOCKET_ERROR) {
		std::cout << "shutdown failed: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive data until the server closes the connection
	do {
		i = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (i > 0)
		{
			std::cout << "Bytes received: " << i << std::endl;
			std::cout << "Message received: ";
			std::cout.write(recvbuf, i);
			std::cout << std::endl;
		}
			
		else if (i == 0)
			std::cout << "Connection closed" << std::endl;
		else
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
	} while (i > 0);

	// Cleanup
	closesocket(ConnectSocket);
	WSACleanup();

    return 0;
}

