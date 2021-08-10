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

#ifndef _tcpcomm_h
#define _tcpcomm_h

#include "MyForm.h"
#include <winsock2.h>
//MyForm aa;

namespace Project1
{
	ref class TCPcomm {
	public:
		TCPcomm(MyForm^ myForm);
		void maintcp();
	private:
		MyForm^ myForm = gcnew MyForm;
		ref class BitArray2D;
		typedef ref struct motorspeed;
		int Connect(SOCKET *ConnectSocket);
		bool tcpchangebuffer(char msgsearch[], SOCKET client);
		void updaterecvbuffer(SOCKET client, int offset);
		void CheckpacketValidity(SOCKET client);
		int tcpmsgsend(char message[], int length, SOCKET client);
		int tcpmsgreceive(SOCKET client, int offset);
		//void setMotorspeeds(array<motorspeed^>^ ms);
		void setMotorspeeds(array<motorspeed^>^ ms);
	};
}

#endif