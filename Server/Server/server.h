#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include<windows.h>

#define TIME_PORT	27015
#define BUFFER_SIZE 255

int executeClientsRequest(char* recvBuff, char* sendBuff, SOCKET* m_socket, sockaddr* client_addr, 
	int* client_addr_len, int* didMeasure, int* measuredSeconds);
void getTime(char* sendBuff);
void getTimeWithoutDate(char* sendBuff);
void getTimeSinceEpoch(char* sendBuff);
void getTickCountToSendBuff(char* sendBuff);
void getTimeWithoutDateOrSeconds(char* sendBuff);
void getYear(char* sendBuff);
void getMonthAndDay(char* sendBuff);
void getSecondsSinceBeginningOfMonth(char* sendBuff);
void getWeekOfYear(char* sendBuff);
void getDaylightSavings(char* sendBuff);
int isDaylightSavings();
int getTimeWithoutDateInCity(char* sendBuff, SOCKET* m_socket, sockaddr* client_addr, int* client_addr_len);
int recvFromClient(SOCKET* m_socket, char* recvBuff, sockaddr* client_addr, int* client_addr_len);
void measureTimeLap(char* sendBuff, int* didMeasure, int* measuredSeconds);
void initMeasureIfNeeded(int* didMeasure, int* measuredSeconds);
