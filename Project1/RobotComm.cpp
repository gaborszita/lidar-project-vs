#include "RobotComm.h"
#include <simpleTCP.h>
#include <BitArrays.h>
#include <OpenCLInit.h>
#include <PointMover.h>

#define _USE_MATH_DEFINES
#include <math.h>
#define mapRamX 10000
#define mapRamY 10000

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

//#define scrladd (keyboard->ScrollLock ? 9 : 0)
#define scrladd 9
#define STANDARDMOTORSPEED 90
#define turnSub 10

#define LIST_SIZE 360
#define ALL_ERROR_DEBUG
using namespace simpleTCPLibrary;
using namespace BitArrays;
using namespace System;

bool picrefav = false;

int cmpfunc(const void * a, const void * b) {
	return (*(int*)a - *(int*)b);
}

namespace Project1
{
	RobotComm::RobotComm(MyForm^ myForm)
	{
		this->myForm = myForm;
	}
	void RobotComm::maintcp()
	{
		//RobotComm RobotComm(this);
		Keyboard ^keyboard = gcnew Keyboard;
		char sendbuffer[50000];
		char * sendbufpointer = sendbuffer;
		int laserdataX[2][LIST_SIZE];
		int laserdataY[2][LIST_SIZE];
		memset(laserdataX, 0, sizeof(laserdataX));
		memset(laserdataY, 0, sizeof(laserdataY));
		int laserDataArrayActive = 0;
		int robotx=0, roboty=0, heading=0;
		int lastkeystate = 0;
		BitArray2D mapdata(mapRamX, mapRamY);
		//mapdata.getbit(mapRamX-1, mapRamY-1);
		//TCHAR NPath[MAX_PATH];
		//GetCurrentDirectory(MAX_PATH, NPath);
		//std::cout << "directory: " << NPath << "\n";
		//FILE* program_handle;
		//int err = fopen_s(&program_handle, "Project1.exe.metagen", "r");
		MotorSpeeds ms[18];
		MotorSpeeds::setMotorSpeeds(ms);
		OpenCLInit openCLInit;
		PointMover pointMover(&openCLInit);
		bool isFirstRun = true;

		//connect to the robot
		simpleTCPClient robotTcp;
		int ret;
		do
		{
			ret = robotTcp.Connect("lidarraspberry", "19984");
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
		} while (ret != 0);

		myForm->tcpstatus->Text = "connected\r\n";
		myForm->robotbox->Text = "raspberry pi agent information\r\n";
		Bitmap ^ image = gcnew Bitmap(700, 700);
		Graphics ^imgrap = Graphics::FromImage(image);
		myForm->pictureBox1->Image = image;

		while (true)
		{
			if (robotTcp.tcpchangebuffer("robotbox"))
			{
				robotTcp.CheckpacketValidity();
				myForm->robotbox->AppendText("\r\n" + gcnew String(robotTcp.recvbufpointer));
				robotTcp.recvbufpointer += strlen(robotTcp.recvbufpointer) + 1;
			}
			else if (robotTcp.tcpchangebuffer("lidardrawing"))
			{
				memset(laserdataX[laserDataArrayActive], -1, LIST_SIZE * sizeof(int));
				memset(laserdataY[laserDataArrayActive], -1, LIST_SIZE * sizeof(int));
				char* bsptr = robotTcp.recvbufpointer;
				for (int i = 0; i < LIST_SIZE; i++)
				{
					robotTcp.CheckpacketValidity();
					int test = sscanf(robotTcp.recvbufpointer, "%d", &laserdataX[laserDataArrayActive][i]);
					robotTcp.recvbufpointer += strlen(robotTcp.recvbufpointer) + 1;
				}
				for (int i = 0; i < LIST_SIZE; i++)
				{
					robotTcp.CheckpacketValidity();
					int test = sscanf(robotTcp.recvbufpointer, "%d", &laserdataY[laserDataArrayActive][i]);
					robotTcp.recvbufpointer += strlen(robotTcp.recvbufpointer) + 1;
					//laserdataY[laserDataArrayActive][i] -= 1000;
				}

				if (myForm->mapEnabled)
				{
					//save reading data to map
					for (int i = 0; i < LIST_SIZE; i++) {
						int x = laserdataX[laserDataArrayActive][i];
						int y = laserdataY[laserDataArrayActive][i];
						int mapX = mapRamX / 2 + (int)(x * cos(-(heading*M_PI / 180))) - (int)(y * sin(-(heading*M_PI / 180))) + robotx;
						int mapY = mapRamY / 2 + (int)(y * cos(-(heading*M_PI / 180))) + (int)(x * sin(-(heading*M_PI / 180))) - roboty;
						if ((mapX > 0 || mapX < mapRamX) && (mapY > 0 || mapY < mapRamX)) {
							mapdata.setbit(mapX, mapY, 1);
						}
					}

					//display map data
					int mapXorigin = mapRamX / 2 - robotx;
					int mapYorigin = mapRamY / 2 - roboty;
					for (int i = 0; i < 700; i++) {
						for (int j = 0; j < 700; j++) {
							int xcorr = (int)((i - 350) * cos(-(heading * M_PI / 180))) - (int)((j - 350) * sin(-(heading * M_PI / 180)));
							int ycorr = (int)((j - 350) * cos(-(heading * M_PI / 180))) + (int)((i - 350) * sin(-(heading * M_PI / 180)));
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
				}

				//display current reading data
				for (int i = 0; i < LIST_SIZE; i++)
				{
					int x = laserdataX[laserDataArrayActive][i] / 1 + 350;
					int y = laserdataY[laserDataArrayActive][i] / 1 + 350;
					if (y < 700 && y > 0 && x < 700 && x > 0) {
						image->SetPixel(x, y, Color::FromArgb(0, 255, 0));
					}
				}
				//printf("%d\n", laserdataY[laserDataArrayActive][180]);
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

				/*memcpy(laserdataX[laserDataArrayActive == 0 ? 1 : 0], laserdataX[laserDataArrayActive], sizeof(laserdataX[laserDataArrayActive]));
				memcpy(laserdataY[laserDataArrayActive == 0 ? 1 : 0], laserdataY[laserDataArrayActive], sizeof(laserdataY[laserDataArrayActive]));
				for (int i = 0; i < 360; i++)
				{
					laserdataX[laserDataArrayActive][i] = i;
					laserdataY[laserDataArrayActive][i] = i;
					laserdataX[laserDataArrayActive == 0 ? 1 : 0][i] = i;
					laserdataY[laserDataArrayActive == 0 ? 1 : 0][i] = i;
				}*/
				/*for (int i = 0; i < LIST_SIZE; i++)
				{
					laserdataX[laserDataArrayActive][i] = 0;
					laserdataY[laserDataArrayActive][i] = 0;
					int degangle = 1;
					float radangle = degangle * (float)M_PI / 180;
					laserdataX[laserDataArrayActive == 0 ? 1 : 0][i] = (int)(laserdataX[laserDataArrayActive][i] * cos(radangle) - laserdataY[laserDataArrayActive][i] * sin(radangle));
					laserdataY[laserDataArrayActive == 0 ? 1 : 0][i] = (int)(laserdataX[laserDataArrayActive][i] * sin(radangle) + laserdataY[laserDataArrayActive][i] * cos(radangle));
				}*/
				if (!isFirstRun)
				{
					pointMover.enqueuCalculation(laserdataX[laserDataArrayActive == 0 ? 1 : 0], laserdataY[laserDataArrayActive == 0 ? 1 : 0], laserdataX[laserDataArrayActive], laserdataY[laserDataArrayActive], 0);
					int Xmv;
					int Ymv;
					int rotmv;
					pointMover.getCalculationData(&Xmv, &Ymv, &rotmv);
					float radheading = heading * (float)M_PI / 180;
					Xmv = (int)round(Xmv * cos(radheading) - Ymv * sin(radheading));
					Ymv = (int)round(Ymv * sin(radheading) + Ymv * cos(radheading));
					robotx += Xmv;
					roboty += Ymv;
					heading += rotmv;
				}
				else
				{
					isFirstRun = false;
				}
				myForm->xLabel->Text = "x: " + gcnew String("" + robotx);
				myForm->yLabel->Text = "y: " + gcnew String("" + roboty);
				myForm->heading->Text = "heading: " + gcnew String("" + heading);
				if (laserDataArrayActive == 0)
				{
					laserDataArrayActive = 1;
				}
				else
				{
					laserDataArrayActive = 0;
				}
			}
			else if (robotTcp.tcpchangebuffer("xydebugcorners")) {
				robotTcp.CheckpacketValidity();
				int num_pairs;
				sscanf(robotTcp.recvbufpointer, "%d", &num_pairs);
				robotTcp.recvbufpointer += strlen(robotTcp.recvbufpointer) + 1;
				int corners[LIST_SIZE][2];
				for (int j = 0; j < 2; j++) {
					for (int i = 0; i < num_pairs; i++) {
						robotTcp.CheckpacketValidity();
						sscanf(robotTcp.recvbufpointer, "%d", &corners[i][0]);
						robotTcp.recvbufpointer += strlen(robotTcp.recvbufpointer) + 1;
						robotTcp.CheckpacketValidity();
						sscanf(robotTcp.recvbufpointer, "%d", &corners[i][1]);
						robotTcp.recvbufpointer += strlen(robotTcp.recvbufpointer) + 1;
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
			else if (robotTcp.tcpchangebuffer("rpm"))
			{
				robotTcp.CheckpacketValidity();
				myForm->rpmLabel->Text = "rpm: " + gcnew String(robotTcp.recvbufpointer);
				robotTcp.recvbufpointer += strlen(robotTcp.recvbufpointer) + 1;
			}
			else if (robotTcp.tcpchangebuffer("xyupdate"))

			{
				robotTcp.CheckpacketValidity();
				//sscanf(robotTcp.recvbufpointer, "%d", &robotx);
				//myForm->xLabel->Text = "x: " + gcnew String(robotTcp.recvbufpointer);
				robotTcp.recvbufpointer += strlen(robotTcp.recvbufpointer) + 1;
				robotTcp.CheckpacketValidity();
				//sscanf(robotTcp.recvbufpointer, "%d", &roboty);
				//myForm->yLabel->Text = "y: " + gcnew String(robotTcp.recvbufpointer);
				robotTcp.recvbufpointer += strlen(robotTcp.recvbufpointer) + 1;
				robotTcp.CheckpacketValidity();
				//sscanf(robotTcp.recvbufpointer, "%d", &heading);
				//myForm->heading->Text = "heading: " + gcnew String(robotTcp.recvbufpointer);
				robotTcp.recvbufpointer += strlen(robotTcp.recvbufpointer) + 1;
			}
			else {
#ifdef ALL_ERROR_DEBUG
				printf("\nerror tcp: %s\n", robotTcp.recvbufpointer);
				printf("other\n");
#endif
				robotTcp.tcpmsgreceive(0);
				robotTcp.recvbufpointer = robotTcp.recvbuf;
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
						sprintf(sendbufpointer, "%d", ms[i - 0x61 + scrladd].m1);
						sendbufpointer += strlen(sendbufpointer) + 1;
						sprintf(sendbufpointer, "%d", ms[i - 0x61 + scrladd].m2);
						sendbufpointer += strlen(sendbufpointer) + 1;
						sprintf(sendbufpointer, "%d", ms[i - 0x61 + scrladd].m3);
						sendbufpointer += strlen(sendbufpointer) + 1;
						sprintf(sendbufpointer, "%d", ms[i - 0x61 + scrladd].m4);
						sendbufpointer += strlen(sendbufpointer) + 1;
						robotTcp.tcpmsgsend(sendbuffer, (int)(sendbufpointer - sendbuffer));
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
					robotTcp.tcpmsgsend(sendbuffer, (int)(sendbufpointer - sendbuffer));
					lastkeystate = 0;
				}
			}
		}
		return;
	}

	void RobotComm::MotorSpeeds::setMotorSpeeds(MotorSpeeds* ms)
	{
		//Forward speeds
		ms[FORWARD - 1].m1 = STANDARDMOTORSPEED;
		ms[FORWARD - 1].m2 = STANDARDMOTORSPEED;
		ms[FORWARD - 1].m3 = STANDARDMOTORSPEED;
		ms[FORWARD - 1].m4 = STANDARDMOTORSPEED;
		//Forward left speeds
		ms[FORWARDLEFT - 1].m1 = 0;
		ms[FORWARDLEFT - 1].m2 = STANDARDMOTORSPEED;
		ms[FORWARDLEFT - 1].m3 = 0;
		ms[FORWARDLEFT - 1].m4 = STANDARDMOTORSPEED;
		//Forward right speeds
		ms[FORWARDRIGHT - 1].m1 = STANDARDMOTORSPEED;
		ms[FORWARDRIGHT - 1].m2 = 0;
		ms[FORWARDRIGHT - 1].m3 = STANDARDMOTORSPEED;
		ms[FORWARDRIGHT - 1].m4 = 0;
		//Left speeds
		ms[LEFT - 1].m1 = -STANDARDMOTORSPEED + turnSub;
		ms[LEFT - 1].m2 = STANDARDMOTORSPEED - turnSub;
		ms[LEFT - 1].m3 = -STANDARDMOTORSPEED + turnSub;
		ms[LEFT - 1].m4 = STANDARDMOTORSPEED - turnSub;
		//Right speeds
		ms[RIGHT - 1].m1 = STANDARDMOTORSPEED - turnSub;
		ms[RIGHT - 1].m2 = -STANDARDMOTORSPEED + turnSub;
		ms[RIGHT - 1].m3 = STANDARDMOTORSPEED - turnSub;
		ms[RIGHT - 1].m4 = -STANDARDMOTORSPEED + turnSub;
		//Backward speeds
		ms[BACKWARD - 1].m1 = -STANDARDMOTORSPEED;
		ms[BACKWARD - 1].m2 = -STANDARDMOTORSPEED;
		ms[BACKWARD - 1].m3 = -STANDARDMOTORSPEED;
		ms[BACKWARD - 1].m4 = -STANDARDMOTORSPEED;
		//Backward right speeds
		ms[BACKWARDRIGHT - 1].m1 = -STANDARDMOTORSPEED;
		ms[BACKWARDRIGHT - 1].m2 = 0;
		ms[BACKWARDRIGHT - 1].m3 = -STANDARDMOTORSPEED;
		ms[BACKWARDRIGHT - 1].m4 = 0;
		//Backward left speeds
		ms[BACKWARDLEFT - 1].m1 = 0;
		ms[BACKWARDLEFT - 1].m2 = -STANDARDMOTORSPEED;
		ms[BACKWARDLEFT - 1].m3 = 0;
		ms[BACKWARDLEFT - 1].m4 = -STANDARDMOTORSPEED;

		//using mecanum wheel special ability

		//Forward speeds
		ms[FORWARD + 8].m1 = -STANDARDMOTORSPEED;
		ms[FORWARD + 8].m2 = -STANDARDMOTORSPEED;
		ms[FORWARD + 8].m3 = STANDARDMOTORSPEED;
		ms[FORWARD + 8].m4 = STANDARDMOTORSPEED;
		//Forward left speeds
		ms[FORWARDLEFT + 8].m1 = -STANDARDMOTORSPEED;
		ms[FORWARDLEFT + 8].m2 = 0;
		ms[FORWARDLEFT + 8].m3 = STANDARDMOTORSPEED;
		ms[FORWARDLEFT + 8].m4 = 0;
		//Forward right speeds
		ms[FORWARDRIGHT + 8].m1 = 0;
		ms[FORWARDRIGHT + 8].m2 = -STANDARDMOTORSPEED;
		ms[FORWARDRIGHT + 8].m3 = 0;
		ms[FORWARDRIGHT + 8].m4 = STANDARDMOTORSPEED;
		//Left speeds
		ms[LEFT + 8].m1 = -STANDARDMOTORSPEED;
		ms[LEFT + 8].m2 = STANDARDMOTORSPEED;
		ms[LEFT + 8].m3 = STANDARDMOTORSPEED;
		ms[LEFT + 8].m4 = -STANDARDMOTORSPEED;
		//Right speeds
		ms[RIGHT + 8].m1 = STANDARDMOTORSPEED;
		ms[RIGHT + 8].m2 = -STANDARDMOTORSPEED;
		ms[RIGHT + 8].m3 = -STANDARDMOTORSPEED;
		ms[RIGHT + 8].m4 = STANDARDMOTORSPEED;
		//Backward speeds
		ms[BACKWARD + 8].m1 = STANDARDMOTORSPEED;
		ms[BACKWARD + 8].m2 = STANDARDMOTORSPEED;
		ms[BACKWARD + 8].m3 = -STANDARDMOTORSPEED;
		ms[BACKWARD + 8].m4 = -STANDARDMOTORSPEED;
		//Backward right speeds
		ms[BACKWARDRIGHT + 8].m1 = STANDARDMOTORSPEED;
		ms[BACKWARDRIGHT + 8].m2 = 0;
		ms[BACKWARDRIGHT + 8].m3 = -STANDARDMOTORSPEED;
		ms[BACKWARDRIGHT + 8].m4 = 0;
		//Backward left speeds
		ms[BACKWARDLEFT + 8].m1 = 0;
		ms[BACKWARDLEFT + 8].m2 = STANDARDMOTORSPEED;
		ms[BACKWARDLEFT + 8].m3 = 0;
		ms[BACKWARDLEFT + 8].m4 = -STANDARDMOTORSPEED;
		return;
	}
}