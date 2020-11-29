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