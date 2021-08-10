/* 
* Copyright (C) 2017-2020 Gabor Szita
* 
* This file is part of lidar project.
*
* Lidar project is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 3 as published by
* the Free Software Foundation.
*
* Lidar project is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with lidar project.  If not, see https://www.gnu.org/licenses/ 
*/ 

#include "simpleTCP.h"

namespace simpleTCPLibrary
{
	int simpleTCPClient::Connect(char hostname_ip[], char port[])
	{
		WSADATA wsaData;
		//SOCKET ConnectSocket = INVALID_SOCKET;
		struct addrinfo *result = NULL,
			*ptr = NULL,
			hints;
		int iResult;
		int recvbuflen = DEFAULT_BUFLEN;

		// Validate the parameters

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
#ifdef ALL_ERROR_DEBUG
			printf("WSAStartup failed with error: %d\n", iResult);
#endif
			return SOCK_WSASTARTUP_ERROR;
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		iResult = getaddrinfo(hostname_ip, port, &hints, &result);
		if (iResult != 0) {
#ifdef ALL_ERROR_DEBUG
			printf("getaddrinfo failed with error: %d\n", iResult);
#endif
			WSACleanup();
			return SOCK_CANNOT_RESOLVE_HOST;
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
			// Create a SOCKET for connecting to server
			client = socket(ptr->ai_family, ptr->ai_socktype,
				ptr->ai_protocol);
			if (client == INVALID_SOCKET) {
#ifdef ALL_ERROR_DEBUG
				printf("socket failed with error: %ld\n", WSAGetLastError());
#endif
				WSACleanup();
				return SOCK_CANNOT_CONNECT;
			}

			// Connect to server.
			iResult = connect(client, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(client);
				client = INVALID_SOCKET;
				continue;
			}
			break;
		}
		/*u_long ioctlsocketMode = 0;
		iResult = ioctlsocket(ConnectSocket, FIONBIO, &ioctlsocketMode);
		if (iResult != NO_ERROR)
			printf("ioctlsocket failed with error: %ld\n", WSAGetLastError());*/

		freeaddrinfo(result);
		return SOCK_CONNECT_SUCCESSFUL;
	}

	bool simpleTCPClient::tcpchangebuffer(char msgsearch[])
	{
		updaterecvbuffer(0);
		if (strcmp(recvbufpointer, msgsearch) == 0 && recvbufpointer < recvbuf + recvbuflen)
		{
			recvbufpointer += strlen(msgsearch) + 1;
			return true;
		}
		else
		{
			return false;
		}
	}

	void simpleTCPClient::updaterecvbuffer(int offset) {
		if (recvbufpointer >= recvbuf + recvbuflen)
		{

			tcpmsgreceive(offset);
			recvbufpointer = recvbuf;
		}
	}
	
	void simpleTCPClient::CheckpacketValidity() { //check if a packet ready to read (socket is non-blocking because CASyncSocket)
		bool valid = false;
		while (true) {
			for (int i = (int)(recvbufpointer - recvbuf); i < recvbuflen; i++) {
				if (recvbuf[i] == '\0') {
					valid = true;
					break;
				}
			}
			if (valid == false) {
				memcpy(&recvbuf, recvbufpointer, recvbuf + recvbuflen - recvbufpointer);
				tcpmsgreceive((int)(recvbuf + recvbuflen - recvbufpointer));
				recvbufpointer = recvbuf;
			}
			else {
				break;
			}
		}
	}

	int simpleTCPClient::tcpmsgsend(char message[], int length)
	{
		int iResult;

		iResult = send(client, message, length, 0);
		if (iResult == SOCKET_ERROR) {
			closesocket(client);
			WSACleanup();
			return 1;
		}
		return 0;

	}

	int simpleTCPClient::tcpmsgreceive(int offset)
	{
		int iResult;
		iResult = recv(client, recvbuf + offset, DEFAULT_BUFLEN, 0);
		if (iResult == SOCKET_ERROR) {
#ifdef ALL_ERROR_DEBUG
			printf("SOCKET_ERROR%d", WSAGetLastError());
#endif
			closesocket(client);
			WSACleanup();
			return 1;
		}
		recvbuflen = iResult + offset;
		return 0;
	}
}