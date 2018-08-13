// BlockingTCP_Server-Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
//#include <WS2tcpip.h>
#include <WinSock2.h>
#include <string.h>

#pragma comment (lib, "ws2_32.lib")

SOCKET s;			//socket ket noi tu server
SOCKET c[1024];		//cac socket cho clients
SOCKADDR_IN sAddr;	//luu thong tin server
SOCKADDR_IN cAddrs[1024];	//luu thong tin cac client
int nClients = 0;	//so luong client MAX (da ket noi, co the da close)

//Thread xu ly Recv
DWORD WINAPI ReceiverThread(LPVOID lParam){
	int i = (int)lParam;	//ep sang so'

	char szXau[1024];		//luu recv data
	int len = 0;

	char szStr[1024];		//luu temp data

	while (1){
		len = recv(c[i], szXau, 1024, 0);	//recv data

		if (len <= 0){		//ngat ket noi socket neu k nhan dc data
			printf("Socket o vi tri %d ngat ket noi!\n", i);
			closesocket(c[i]);
			c[i] = 0;
			break;
		}

		szXau[len] = 0;		//neu len>0, xu ly data nhan dc
		printf("Client %d: %s\n", i, szXau);
		sprintf(szStr, "Client %d: %s", i, szXau);	//copy vao szStr

		//gui cho tat ca cac client khac chinh' n, va phai dang hoat dong
		for (int j = 0; j < nClients; j++)
		{
			if (i != j && c[j] != 0)
			{
				send(c[j], szStr, strlen(szStr), 0);
			}
		}
	}
	return 0;
}

//Thread Send: gui data cho tat ca cac Client
DWORD WINAPI SenderThread(LPVOID lParam){
	char szXau[1024];	//luu send data
	char szStr[1024];	//luu temp data
	printf("Bat dau Thread Sender\n");

	while (1)
	{
		gets(szXau);	//nhap xau dau vao

		//neu nhap exit thi close tat ca socket
		if (strcmp(szXau, "exit") == 0)
		{
			closesocket(s);		//close server
			
			//close tat ca clients
			for (int j = 0; j < nClients; j++)
			{
				if (c[j] != 0)
				{
					closesocket(c[j]);
					c[j] = 0;
				}
			}
			Sleep(10);
			break;
		}

		//gui cho tat ca cac client dang ket noi den
		sprintf(szStr, "Server: %s", szXau);
		for (int i = 0; i < nClients; i++)
		{
			if (c[i] != 0)
			{
				send(c[i], szStr, strlen(szStr), 0);
			}
		}
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

	//B2 - xac dinh socket (server)
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//B3 - xac dinh IP, PORT cho server
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(8888);
	sAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//B4 - bind
	ret = bind(s, (sockaddr*)&sAddr, sizeof(sAddr));
	if (ret == SOCKET_ERROR)
	{
		printf("loi bind: %d", WSAGetLastError());
		return 0;
	}

	//B5 - listen
	ret = listen(s, 16);

	memset(c, 0, sizeof(SOCKET)* 1024);	//set 1024 vi tri = 0, ke tu (c+0)

	//B6 - accept -> createThread -> send/recv
	int i;
	int cAddrLen = sizeof(SOCKADDR_IN);

	while (1){
		//tim kiem gia tri i dau tien de gan thu tu cho Client ket noi den
		for (i = 0; i < nClients; i++)
		{
			if (c[i] == 0)
			{
				break;
			}
		}

		//chap nhan ket noi voi Client vua dc gan thu tu i
		c[i] = accept(s, (sockaddr*) &cAddrs[i], &cAddrLen);
		if (c[i] == SOCKET_ERROR)
		{
			break;
		}

		//tao SenderThread vao thoi diem ban dau khoi dong server
		if (nClients == 0)
		{
			CreateThread(NULL, NULL, SenderThread, 0, 0, 0);
		}

		//thong bao co ket noi tu Client[i]
		printf("Co ket noi moi tu %s:%d, cat vao vi tri %d\n", inet_ntoa(cAddrs[i].sin_addr), ntohs(cAddrs[i].sin_port), i);

		//tang nClients de quan ly so luong Clients
		//nClients chi tang chu k giam, vi co the close bat ki 1 vi tri nao
		//nen k the dung nClients de dem so luong Clients
		if (i == nClients)
		{
			nClients++;
		}

		//tao Thread de Recv, voi Parameter i
		CreateThread(NULL, NULL, ReceiverThread, (LPVOID)i, NULL, NULL);
	}

	//B7 - giai phong
	printf("Xong");
	WSACleanup();

	return 0;
}

