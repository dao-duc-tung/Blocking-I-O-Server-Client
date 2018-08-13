// BlockingTCP_Client-Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
//#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

SOCKET c;		//socket cho client

DWORD WINAPI ReceiverThread(LPVOID lpParam){
	char szXau[1024];
	int len;
	while (1)
	{
		len = recv(c, szXau, 1024, 0);	//nhan data tu socket
		if (len<=0)
		{
			closesocket(c);
			break;
		}
		szXau[len] = 0;
		printf("%s\n", szXau);
	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//B1 - khoi tao winsock
	WSAData wsaData;
	int ret = WSAStartup(0x0202, &wsaData);
	if (ret != 0)
	{
		printf("khong the tao winsock");
		return 0;
	}

	//B2 - dinh nghia socket
	c = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//B3 - dinh nghia SOCKADDR
	SOCKADDR_IN sAddr;
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(8888);
	sAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//B4 - connect
	ret = connect(c, (sockaddr*)&sAddr, sizeof(sAddr));
	if (ret == SOCKET_ERROR){
		printf("Loi ket noi: %d", WSAGetLastError());
		return 0;
	}

	//B5 - xu ly Thread recv va send
	//tao ngay Thread
	CreateThread(NULL, NULL, ReceiverThread, NULL, NULL, NULL);

	char szXau[1024];
	while (1)
	{
		gets(szXau);
		send(c, szXau, strlen(szXau), 0);
	}

	//B6 - giai phong
	closesocket(c);
	WSACleanup();

	return 0;
}

