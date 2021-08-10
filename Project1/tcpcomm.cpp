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

#include "tcpcomm.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <process.h>
#include <iostream>

#define mapRamX 100000
#define mapRamY 100000

#define FORWARD  8
#define FORWARDLEFT 7
#define FORWARDRIGHT 9
#define LEFT 4
#define RIGHT 6
#define BACKWARD 2
#define BACKWARDRIGHT 3
#define BACKWARDLEFT 1

#define Robotwidthleft 10
#define Robotwidthright 10
#define Robotlengthfront 7
#define Robotlengthback 15

#define CornerDebugPtsSize 4

#define scrladd 9
#define STANDARDMOTORSPEED 90
#define turnSub 10

#define SOCK_WSASTARTUP_ERROR 1
#define SOCK_CANNOT_RESOLVE_HOST 2
#define SOCK_CANNOT_CONNECT 3
#define SOCK_CONNECT_SUCCESSFUL 0

using namespace System;
using namespace Microsoft::VisualBasic::Devices;
using namespace Project1;

TCPcomm::TCPcomm(MyForm^ myForm) {
	this->myForm = myForm;
}

void TCPcomm::maintcp()
{
	Keyboard ^keyboard = gcnew Keyboard;
	char sendbuffer[50000];
	char * sendbufpointer = sendbuffer;
	int laserdataX[360];
	int laserdataY[360];
	int robotx, roboty, heading;
	int lastkeystate = 0;
	BitArray2D mapdata(mapRamX, mapRamY);
	array<motorspeed^>^ ms = gcnew array<motorspeed^>(18);
	setMotorspeeds(ms);

	//connect to the robot
	SOCKET client;
	int ret = Connect(&client);
	while (ret != 0)
	{
		int ret = Connect(&client);
		switch (ret)
		{
		case SOCK_WSASTARTUP_ERROR:
			myForm->tcpstatus->Text = "disconnected, attemting to reconnect \r\n Cause of faliure: WSAStartup failed. Try enabling ALL_ERROR_DEBUG.";
			break;

		case SOCK_CANNOT_RESOLVE_HOST:
			myForm->tcpstatus->Text = "disconnected, attemting to reconnect \r\n Cause of faliure: Couldn't resolve hostname. Check if the hostname is correct and/or the robot is connected to the network.";
			break;

		case SOCK_CANNOT_CONNECT:
			myForm->tcpstatus->Text = "disconnected, attemting to reconnect \r\n Cause of faliure: Couldn't resolve hostname. Check if the hostname is correct and/or the robot is connected to the network.";
			break;

		default:
			myForm->tcpstatus->Text = "disconnected, attemting to reconnect \r\n Cause of faliure unknown, try enabling ALL_ERROR_DEBUG.";
		}
	}
	do { //attempt to connect until successfully connected
		int ret = Connect(&client);
		myForm->tcpstatus->Text = "disconnected, attemting to reconnect";
	} while (client != 0);

	myForm->tcpstatus->Text = "connected";
	myForm->robotbox->Text = "raspberry pi agent information\r\n";
	Bitmap ^ image = gcnew Bitmap(700, 700);
	Graphics ^imgrap = Graphics::FromImage(image);
	myForm->pictureBox1->Image = image;

	while (true)
	{
		if (tcpchangebuffer("robotbox", client))
		{
			CheckpacketValidity(client);
			myForm->robotbox->AppendText("\r\n" + gcnew String(recvbufpointer));
			recvbufpointer += strlen(recvbufpointer) + 1;
		}
		else if (tcpchangebuffer("lidardrawing", client))
		{
			memset(laserdataX, -1, 360 * sizeof(int));
			memset(laserdataY, -1, 360 * sizeof(int));
			char* bsptr = recvbufpointer;
			for (int i = 0; i < 360; i++)
			{
				CheckpacketValidity(client);
				int test = sscanf(recvbufpointer, "%d", &laserdataX[i]);
				recvbufpointer += strlen(recvbufpointer) + 1;
			}
			for (int i = 0; i < 360; i++)
			{
				CheckpacketValidity(client);
				int test = sscanf(recvbufpointer, "%d", &laserdataY[i]);
				recvbufpointer += strlen(recvbufpointer) + 1;
			}

			//save reading data to map
			for (int i = 0; i < 360; i++) {
				int x = laserdataX[i];
				int y = laserdataY[i];
				int mapX = mapRamX / 2 + (int)(x * cos(-(heading*pi / 180))) - (int)(y * sin(-(heading*pi / 180))) + robotx;
				int mapY = mapRamY / 2 + (int)(y * cos(-(heading*pi / 180))) + (int)(x * sin(-(heading*pi / 180))) - roboty;
				if ((mapX > 0 || mapX < mapRamX) && (mapY > 0 || mapY < mapRamX)) {
					mapdata.setbit(mapX, mapY, 1);
				}
			}

			//display map data
			int mapXorigin = mapRamX / 2 - robotx;
			int mapYorigin = mapRamY / 2 - roboty;
			for (int i = 0; i < 700; i++) {
				for (int j = 0; j < 700; j++) {
					int xcorr = (int)((i - 350) * cos(-(heading * pi / 180))) - (int)((j - 350) * sin(-(heading * pi / 180)));
					int ycorr = (int)((j - 350) * cos(-(heading * pi / 180))) + (int)((i - 350) * sin(-(heading * pi / 180)));
					int x = mapXorigin + xcorr;
					int y = mapYorigin + ycorr;
					int res = 0;
					if ((x > 0 || x < mapRamX) && (y > 0 || y < mapRamX)) {
						res = mapdata.getbit(x, y);
					}
					if (res == 1) {
						image->SetPixel(i, j, Color::FromArgb(255, 255, 255));
					}
				}

			}

			//display current reading data
			for (int i = 0; i < 360; i++)
			{
				int x = laserdataX[i] / 1 + 350;
				int y = laserdataY[i] / 1 + 350;
				if (y < 700 && y > 0 && x < 700 && x > 0) {
					image->SetPixel(x, y, Color::FromArgb(0, 255, 0));
				}
			}

			//display the yellow rectangle (the robot)
			for (int i = 350 - Robotwidthleft; i <= 350 + Robotwidthright; i++)
			{
				for (int x = 350 - Robotlengthfront; x <= 350 + Robotlengthback; x++)
				{
					image->SetPixel(i, x, Color::FromArgb(255, 255, 51));
				}
			}


			myForm->pictureBox1->Refresh();
			imgrap->Clear(Color::FromArgb(0, 0, 0));
		}
		else if (tcpchangebuffer("xydebugcorners", client)) {
			CheckpacketValidity(client);
			int num_pairs;
			sscanf(recvbufpointer, "%d", &num_pairs);
			recvbufpointer += strlen(recvbufpointer) + 1;
			int corners[360][2];
			for (int j = 0; j < 2; j++) {
				for (int i = 0; i < num_pairs; i++) {
					CheckpacketValidity(client);
					sscanf(recvbufpointer, "%d", &corners[i][0]);
					recvbufpointer += strlen(recvbufpointer) + 1;
					CheckpacketValidity(client);
					sscanf(recvbufpointer, "%d", &corners[i][1]);
					recvbufpointer += strlen(recvbufpointer) + 1;
				}
				printf("%d\n", num_pairs);
				for (int i = 0; i < num_pairs; i++)
				{
					int x = corners[i][0] / 1 + 350;
					int y = corners[i][1] / 1 + 350;
					printf("%d %d\n", x, y);
					for (int xD = x - CornerDebugPtsSize; xD <= x + CornerDebugPtsSize; xD++)
					{
						for (int yD = y - CornerDebugPtsSize; yD <= y + CornerDebugPtsSize; yD++)
						{
							if (yD < 700 && yD > 0 && xD < 700 && xD > 0) {
								if (j == 0) {
									image->SetPixel(xD, yD, Color::FromArgb(255, 51, 0));
								}
								else {
									if ((xD <= x - 2 || xD >= x + 2) || (yD <= y - 2 || yD >= y + 2)) {
										image->SetPixel(xD, yD, Color::FromArgb(0, 0, 255));
									}
								}
							}
						}
					}
				}
			}
			myForm->pictureBox1->Refresh();
			imgrap->Clear(Color::FromArgb(0, 0, 0));
			//free(corners);
		}
		else if (tcpchangebuffer("rpm", client))
		{
			CheckpacketValidity(client);
			myForm->rpmLabel->Text = "rpm: " + gcnew String(recvbufpointer);
			recvbufpointer += strlen(recvbufpointer) + 1;
		}
		else if (tcpchangebuffer("xyupdate", client))

		{
			CheckpacketValidity(client);
			sscanf(recvbufpointer, "%d", &robotx);
			myForm->xLabel->Text = "x: " + gcnew String(recvbufpointer);
			recvbufpointer += strlen(recvbufpointer) + 1;
			CheckpacketValidity(client);
			sscanf(recvbufpointer, "%d", &roboty);
			myForm->yLabel->Text = "y: " + gcnew String(recvbufpointer);
			recvbufpointer += strlen(recvbufpointer) + 1;
			CheckpacketValidity(client);
			sscanf(recvbufpointer, "%d", &heading);
			myForm->heading->Text = "heading: " + gcnew String(recvbufpointer);
			recvbufpointer += strlen(recvbufpointer) + 1;
		}
		else {
#ifdef ALL_ERROR_DEBUG
			printf("\nerror tcp: %s\n", recvbufpointer);
			printf("other\n");
#endif
			tcpmsgreceive(client, 0);
			recvbufpointer = recvbuf;
			myForm->tcpstatus->AppendText("packet error detected\r\n");
		}

		//send motor states
		for (int i = 0x60; i <= 0x6F; i++)
		{
			if (i == 0x65) i++;
			if (GetKeyState(i) & 0x8000)
			{

				if (i != lastkeystate)
				{
					lastkeystate = i;
					if (i == 0x60) {
						i = 0x5B;
					}
					else if (i == 0x6E) {
						i = 0x66 - 9;
					}
					sendbufpointer = sendbuffer;
					sprintf(sendbufpointer, "motorsrun");
					sendbufpointer += strlen(sendbufpointer) + 1;
					sprintf(sendbufpointer, "%d", ms[i - 0x61 + scrladd]->m1);
					sendbufpointer += strlen(sendbufpointer) + 1;
					sprintf(sendbufpointer, "%d", ms[i - 0x61 + scrladd]->m2);
					sendbufpointer += strlen(sendbufpointer) + 1;
					sprintf(sendbufpointer, "%d", ms[i - 0x61 + scrladd]->m3);
					sendbufpointer += strlen(sendbufpointer) + 1;
					sprintf(sendbufpointer, "%d", ms[i - 0x61 + scrladd]->m4);
					sendbufpointer += strlen(sendbufpointer) + 1;
					tcpmsgsend(sendbuffer, (int)(sendbufpointer - sendbuffer), client);
				}
				break;
			}
			else if (lastkeystate != 0 && i == 0x6F)
			{
				sendbufpointer = sendbuffer;
				sprintf(sendbufpointer, "motorsrun");
				sendbufpointer += strlen(sendbufpointer) + 1;
				sprintf(sendbufpointer, "0");
				sendbufpointer += strlen(sendbufpointer) + 1;
				sprintf(sendbufpointer, "0");
				sendbufpointer += strlen(sendbufpointer) + 1;
				sprintf(sendbufpointer, "0");
				sendbufpointer += strlen(sendbufpointer) + 1;
				sprintf(sendbufpointer, "0");
				sendbufpointer += strlen(sendbufpointer) + 1;
				tcpmsgsend(sendbuffer, (int)(sendbufpointer - sendbuffer), client);
				lastkeystate = 0;
			}
		}
	}
	return;
}

int TCPcomm::Connect(SOCKET *ConnectSocket)
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
	iResult = getaddrinfo(ROBOT_IP_HOSTNAME, DEFAULT_PORT, &hints, &result);
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
		*ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (*ConnectSocket == INVALID_SOCKET) {
#ifdef ALL_ERROR_DEBUG
			printf("socket failed with error: %ld\n", WSAGetLastError());
#endif
			WSACleanup();
			return SOCK_CANNOT_CONNECT;
		}

		// Connect to server.
		iResult = connect(*ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(*ConnectSocket);
			*ConnectSocket = INVALID_SOCKET;
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

bool TCPcomm::tcpchangebuffer(char msgsearch[], SOCKET client)
{
	updaterecvbuffer(client, 0);
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

void TCPcomm::updaterecvbuffer(SOCKET client, int offset) {
	if (recvbufpointer >= recvbuf + recvbuflen)
	{

		tcpmsgreceive(client, offset);
		recvbufpointer = recvbuf;
	}
}

void TCPcomm::CheckpacketValidity(SOCKET client) { //check if a packet ready to read (socket is non-blocking because CASyncSocket)
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
			tcpmsgreceive(client, (int)(recvbuf + recvbuflen - recvbufpointer));
			recvbufpointer = recvbuf;
		}
		else {
			break;
		}
	}
}

int TCPcomm::tcpmsgsend(char message[], int length, SOCKET client)
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

int TCPcomm::tcpmsgreceive(SOCKET client, int offset)
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

void TCPcomm::setMotorspeeds(array<motorspeed^>^ ms)
{
	//Forward speeds
	ms[FORWARD - 1]->m1 = STANDARDMOTORSPEED;
	ms[FORWARD - 1]->m2 = STANDARDMOTORSPEED;
	ms[FORWARD - 1]->m3 = STANDARDMOTORSPEED;
	ms[FORWARD - 1]->m4 = STANDARDMOTORSPEED;
	//Forward left speeds
	ms[FORWARDLEFT - 1]->m1 = 0;
	ms[FORWARDLEFT - 1]->m2 = STANDARDMOTORSPEED;
	ms[FORWARDLEFT - 1]->m3 = 0;
	ms[FORWARDLEFT - 1]->m4 = STANDARDMOTORSPEED;
	//Forward right speeds
	ms[FORWARDRIGHT - 1]->m1 = STANDARDMOTORSPEED;
	ms[FORWARDRIGHT - 1]->m2 = 0;
	ms[FORWARDRIGHT - 1]->m3 = STANDARDMOTORSPEED;
	ms[FORWARDRIGHT - 1]->m4 = 0;
	//Left speeds
	ms[LEFT - 1]->m1 = -STANDARDMOTORSPEED + turnSub;
	ms[LEFT - 1]->m2 = STANDARDMOTORSPEED - turnSub;
	ms[LEFT - 1]->m3 = -STANDARDMOTORSPEED + turnSub;
	ms[LEFT - 1]->m4 = STANDARDMOTORSPEED - turnSub;
	//Right speeds
	ms[RIGHT - 1]->m1 = STANDARDMOTORSPEED - turnSub;
	ms[RIGHT - 1]->m2 = -STANDARDMOTORSPEED + turnSub;
	ms[RIGHT - 1]->m3 = STANDARDMOTORSPEED - turnSub;
	ms[RIGHT - 1]->m4 = -STANDARDMOTORSPEED + turnSub;
	//Backward speeds
	ms[BACKWARD - 1]->m1 = -STANDARDMOTORSPEED;
	ms[BACKWARD - 1]->m2 = -STANDARDMOTORSPEED;
	ms[BACKWARD - 1]->m3 = -STANDARDMOTORSPEED;
	ms[BACKWARD - 1]->m4 = -STANDARDMOTORSPEED;
	//Backward right speeds
	ms[BACKWARDRIGHT - 1]->m1 = -STANDARDMOTORSPEED;
	ms[BACKWARDRIGHT - 1]->m2 = 0;
	ms[BACKWARDRIGHT - 1]->m3 = -STANDARDMOTORSPEED;
	ms[BACKWARDRIGHT - 1]->m4 = 0;
	//Backward left speeds
	ms[BACKWARDLEFT - 1]->m1 = 0;
	ms[BACKWARDLEFT - 1]->m2 = -STANDARDMOTORSPEED;
	ms[BACKWARDLEFT - 1]->m3 = 0;
	ms[BACKWARDLEFT - 1]->m4 = -STANDARDMOTORSPEED;

	//using mecanum wheel special ability

	//Forward speeds
	ms[FORWARD + 8]->m1 = -STANDARDMOTORSPEED;
	ms[FORWARD + 8]->m2 = -STANDARDMOTORSPEED;
	ms[FORWARD + 8]->m3 = STANDARDMOTORSPEED;
	ms[FORWARD + 8]->m4 = STANDARDMOTORSPEED;
	//Forward left speeds
	ms[FORWARDLEFT + 8]->m1 = -STANDARDMOTORSPEED;
	ms[FORWARDLEFT + 8]->m2 = 0;
	ms[FORWARDLEFT + 8]->m3 = STANDARDMOTORSPEED;
	ms[FORWARDLEFT + 8]->m4 = 0;
	//Forward right speeds
	ms[FORWARDRIGHT + 8]->m1 = 0;
	ms[FORWARDRIGHT + 8]->m2 = -STANDARDMOTORSPEED;
	ms[FORWARDRIGHT + 8]->m3 = 0;
	ms[FORWARDRIGHT + 8]->m4 = STANDARDMOTORSPEED;
	//Left speeds
	ms[LEFT + 8]->m1 = -STANDARDMOTORSPEED;
	ms[LEFT + 8]->m2 = STANDARDMOTORSPEED;
	ms[LEFT + 8]->m3 = STANDARDMOTORSPEED;
	ms[LEFT + 8]->m4 = -STANDARDMOTORSPEED;
	//Right speeds
	ms[RIGHT + 8]->m1 = STANDARDMOTORSPEED;
	ms[RIGHT + 8]->m2 = -STANDARDMOTORSPEED;
	ms[RIGHT + 8]->m3 = -STANDARDMOTORSPEED;
	ms[RIGHT + 8]->m4 = STANDARDMOTORSPEED;
	//Backward speeds
	ms[BACKWARD + 8]->m1 = STANDARDMOTORSPEED;
	ms[BACKWARD + 8]->m2 = STANDARDMOTORSPEED;
	ms[BACKWARD + 8]->m3 = -STANDARDMOTORSPEED;
	ms[BACKWARD + 8]->m4 = -STANDARDMOTORSPEED;
	//Backward right speeds
	ms[BACKWARDRIGHT + 8]->m1 = STANDARDMOTORSPEED;
	ms[BACKWARDRIGHT + 8]->m2 = 0;
	ms[BACKWARDRIGHT + 8]->m3 = -STANDARDMOTORSPEED;
	ms[BACKWARDRIGHT + 8]->m4 = 0;
	//Backward left speeds
	ms[BACKWARDLEFT + 8]->m1 = 0;
	ms[BACKWARDLEFT + 8]->m2 = STANDARDMOTORSPEED;
	ms[BACKWARDLEFT + 8]->m3 = 0;
	ms[BACKWARDLEFT + 8]->m4 = -STANDARDMOTORSPEED;
	return;
}


typedef struct TCPcomm::motorspeed
{
	int m1;
	int m2;
	int m3;
	int m4;
};

class TCPcomm::BitArray2D {
private:
	long int* arr;
	int spY;

public:
	BitArray2D(int dimX, int dimY) {
		arr = (long int*)malloc((dimX * dimY + 32 - 1) / 32 * sizeof(long int));
		memset(arr, 0, (dimX * dimY + 32 - 1) / 32 * sizeof(long int));
		spY = dimY;
	}
	int getbit(int bitreqX, int bitreqY) {
		int bitreq = bitreqX * spY + bitreqY;
		int index = bitreq / 32;
		int bit = bitreq % 32;
		//printf("bitreq: %d index:%d bit:%d\n", bitreq, index, bit);
		//return 0;
		int elem = (arr[index] >> bit) & 1;
		return elem;
	}
	void setbit(int bitreqX, int bitreqY, int bitwrite) {
		int bitreq = bitreqX * spY + bitreqY;
		int index = bitreq / 32;
		int bit = bitreq % 32;
		//printf("index:%d bit7:%d\n", index, bit);
		//return;
		if (bitwrite == 0) {
			arr[index] &= ~(1 << bit);
		}
		else {
			arr[index] |= (1 << bit);
		}
	}
};