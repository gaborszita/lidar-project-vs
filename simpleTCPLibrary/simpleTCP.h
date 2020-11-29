#ifndef _SIMPLETCP_H
#define _SIMPLETCP_H
#include <winsock2.h>
#include <ws2tcpip.h>
#define DEFAULT_BUFLEN 100000
//#define DEFAULT_PORT "19984"
//#define ROBOT_IP_HOSTNAME "lidarraspberry"

#define SOCK_WSASTARTUP_ERROR -1
#define SOCK_CANNOT_RESOLVE_HOST -2
#define SOCK_CANNOT_CONNECT -3
#define SOCK_CONNECT_SUCCESSFUL 0

namespace simpleTCPLibrary
{
	class simpleTCPClient
	{
	private:
		char tcpbuffer[DEFAULT_BUFLEN];
		SOCKET client;

	public:
		char * recvbufpointer = recvbuf;
		char recvbuf[DEFAULT_BUFLEN];
		int recvbuflen;

		int Connect(char hostname[], char port[]);
		bool tcpchangebuffer(char msgsearch[]);
		void updaterecvbuffer(int offset);
		void CheckpacketValidity();
		int tcpmsgsend(char message[], int length);
		int tcpmsgreceive(int offset);

	};
}
#endif
