#include "pch.h"
//#include "pch2.h"
#undef UNICODE
//#define _DEBUG

#define WIN32_LEAN_AND_MEAN
#define _STDC_WANT_LIB_EXT1__ 1
#define __STDC_LIB_EXT1__

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "8889"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <thread>
#include <vector>


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

using namespace std;
using std::thread;
using std::string;
//functions
void getInput();
void recvNsent(int iResult2, SOCKET ClientSocket, char recvbuf[], int recvbuflen);
char* convertToStr(int n);
void strca(char p[], char q[]);
//variables
string message;
char datab[DEFAULT_BUFLEN];
 char key;
 char*  str1, *str2, *str3, *str4, *str5, *str6;
 char str[13] = "GET_ALL_JNT";
 char st[69] = "0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0";
int iSendResult;
int iResult2;
int joint1 = 0;
int joint2 = 0;
int joint3 = 0;
int  joint4 = 0;
int joint5 = 0;
int joint6 = 0;
int delta = 5;


std::thread t2(getInput);

int __cdecl main(void)
{

	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	
	std::thread t(main, iResult, ClientSocket, recvbuf, recvbuflen);
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	//startPort = DEFAULT_PORT
	int startPort = 8889; //the starting and also main listening port which will accept all trafic and sends out the ports for connecting
	int numServerPorts = 1;//number of ports which is server listening on
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult =bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//cout << "Thread with id " << GetCurrentThreadId() << "is on port: " << startPort << endl;

	int clientNum = 0;
	
	while (1) {
		cout << "waiting for a connection\n";

		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		cout << "Client: " << clientNum << "on port " << startPort << endl;

	
		while (1) {

			//receive and send data
			recvNsent(iResult, ClientSocket, recvbuf, recvbuflen);
			
			// synchronize threads:
			t.join();                // pauses until first finishes
			t2.join();               // pauses until second finishes
		}		
		break;
	}

	
	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
	return 0;
	
}

//Receive and send server data 
void recvNsent(int iResult2, SOCKET ClientSocket, char recvbuf[], int recvbuflen) {

	while (1) {

		iResult2 = recv(ClientSocket, recvbuf, recvbuflen, 0);
		cout << "recebendo data from client\n";

		//verify if IResult has data
		if (iResult2 > 0) { //if IResult == ">"
			printf("Bytes received: %d\n", iResult2);

			int i = 0;
			while (i <= DEFAULT_BUFLEN) {
				//datab has data from client 
				datab[i] = recvbuf[i];

				if (datab[i] == '>') {
					message = datab;
					cout << "message = " << message;
					break;
				}

				i++;
			}

			datab[0] = '\0';
			if (message == "<CONNECTION_OPEN>") {
				//datab[0] = '\0';
				strcat_s(datab, 40, "CONNECTION_OPEN;;");
				cout << " datab =  " << datab;
				iSendResult = send(ClientSocket, datab, iResult2, 0);
			}
			if (message == "<GET_NUM_ARMS>N;;") {
				//datab[0] = '\0';
				strcat_s(datab, 40, "GET_NUM_ARMS;1;");
				iSendResult = send(ClientSocket, datab, iResult2, 0);
			}
			if (message.compare(0, 19, "<GET_ARM_CODNAME;1>") == 0) {
				//datab[0] = '\0';
				strcat_s(datab, 40, "GET_ARM_CODNAME;1;NS_00101");
				iSendResult = send(ClientSocket, datab, iResult2, 0);
			}
			if (message.compare(0, 16, "<GET_ARM_AXES;1>") == 0) {
				// datab[0] = '\0';
				strcat_s(datab, 40, "GET_ARM_AXES;1;111111----");
				iSendResult = send(ClientSocket, datab, iResult2, 0);
			}
			if (message.compare(0, 15, "<GET_ARM_AUX;1>") == 0) {
				//datab[0] = '\0';
				strcat_s(datab, 40, "GET_ARM_AUX;1;----------");
				iSendResult = send(ClientSocket, datab, iResult2, 0);
			}
			if (message.compare(0, 16, "<GET_ARM_BASE;1>") == 0) {
				//datab[0] = '\0';
				strcat_s(datab, 40, "GET_ARM_BASE;1;0;0;0;0;0;0");
				iSendResult = send(ClientSocket, datab, iResult2, 0);
			}
			if (message.compare(0, 16, "<GET_TOOL_RMT;1>") == 0) {
				//datab[0] = '\0';
				strcat_s(datab, 40, "GET_TOOL_RMT;1;False");
				iSendResult = send(ClientSocket, datab, iResult2, 0);
			}
			if (message.compare(0, 22, "<GET_ARM_ALL_FRAMES;1>") == 0) {
				//datab[0] = '\0';
				strcat_s(datab, 22, "GET_ARM_ALL_FRAMES;1;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0");
				iSendResult = send(ClientSocket, datab, iResult2, 0);
			}
			if (message.compare(0, 16, "<GET_AUX_BASE;1>") == 0) {
				//datab[0] = '\0';
				strcat_s(datab, 65, "GET_AUX_BASE;1;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0");
				iSendResult = send(ClientSocket, datab, iResult2, 0);
			}
			if (message.compare(0, 13, "<GET_ALL_JNT>") == 0) {
				//datab[0] = '\0';

				str1 = convertToStr(joint1);
				str2 = convertToStr(joint2);
				str3 = convertToStr(joint3);
				str4 = convertToStr(joint4);
				str5 = convertToStr(joint5);
				str6 = convertToStr(joint6);
				strca(str, str1);
				strca(str, str2);
				strca(str, str3);
				strca(str, str4);
				strca(str, str5);
				strca(str, str6);
				strca(str, st);

				iSendResult = send(ClientSocket, str, iResult2, 0);
			}
			if (message.compare(0, 14, "<GET_IR_TYPES>") == 0) {
				//datab[0] = '\0';
				strcat_s(datab, 45, "GET_IR_TYPES;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0");
				iSendResult = send(ClientSocket, datab, iResult2, 0);
			}
		}
		else {
			cout << "iResult eh menor que zero\n";
			break;
		}
	}
}

//Read inputs from keyboard ###########
void getInput()
{
	while (1) {
		cout << "print your key";
		cin >> key;

		if (key == 'q')
			joint1 += delta;
		if (key == 'a')
			joint1 -= delta;
		if (key == 'w')
			joint2 += delta;
		if (key == 's')
			joint2 -= delta;
		if (key == 'e')
			joint3 += delta;
		if (key == 'd')
			joint3 -= delta;
		if (key == 'r')
			joint4 += delta;
		if (key == 'f')
			joint4 -= delta;
		if (key == 't')
			joint5 += delta;
		if (key == 'g')
			joint5 -= delta;
		if (key == 'y')
			joint6 += delta;
		if (key == 'h')
			joint6 -= delta;
		if (key == 27) //ESC
			break;
	}
}

//Concatena strings e coloca ';' no final da primeira string ###
void strca(char p[], char q[]) {
	int c, d;

	c = 0;

	while (p[c] != '\0') {
		c++;
	}
	// adicionando ; no final da string, antes de concatenar 
	p[c] = ';';

	d = 0;

	while (q[d] != '\0') {
		p[c + 1] = q[d];
		d++;
		c++;
	}

	p[c] = '\0';
}

//Function int -> str  #############
char* convertToStr(int n) {
	char* st_aux = '\0';
	_ltoa_s(n, st_aux, 10, 10);

	return st_aux;
}



