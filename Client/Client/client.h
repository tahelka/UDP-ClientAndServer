#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>
#include<windows.h>


#define TIME_PORT	27015
#define BUFFER_SIZE 255

void showMenu();
int getClientToServerDelayEstimation(char* sendBuff, char* recvBuff, SOCKET* connSocket, sockaddr_in* server);
int getAnswerFromServer(char* recvBuff, SOCKET* connSocket);
int get100AnswersFromServer(char* recvBuff, SOCKET* connSocket, DWORD* tickCounts);
int sendRequestToServer(char* sendBuff, SOCKET* connSocket, sockaddr_in* server);
int send100RequestsToServer(char* sendBuff, SOCKET* connSocket, sockaddr_in* server);
void sendRequestToServerAndPrintItsAnswerToUser(char* sendBuff, char* recvBuff, SOCKET* connSocket, sockaddr_in* server);
int measureRTT(char* sendBuff, char* recvBuff, SOCKET* connSocket, sockaddr_in* server);
void citiesMenu();
int getTimeWithoutDateInCity(char* sendBuff, SOCKET* connSocket, sockaddr_in* server);
double calculateDelayAvg(DWORD* tickCounts);
