// TD1 Reseau Server.cpp : définit le point d'entrée pour l'application console.
//

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib") // Indicates to the linker that this file is needed

#define DEFAULT_PORT "54323"

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


	struct addrinfo *result = NULL, *ptr = NULL, hints;

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

    return 0;
}

