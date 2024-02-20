#include "client.h"

void main()
{

	// Initialize Winsock (Windows Sockets).

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return;
	}

	// Client side:
	// Create a socket and connect to an internet address.

	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a client to communicate on a network, it must connect to a server.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);

	// Send and receive data.
	
	char sendBuff[BUFFER_SIZE];
	char recvBuff[BUFFER_SIZE];
	bool toContinue = true;
	int userChoice;
	int isSocketError;

	while (toContinue)
	{
		showMenu();

		cin >> userChoice;
		snprintf(sendBuff, BUFFER_SIZE, "%d", userChoice);

		switch (userChoice)
		{
		case 1:
		case 2:
		case 3:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 13:
			sendRequestToServerAndPrintItsAnswerToUser(sendBuff, recvBuff, &connSocket, &server);
			break;
		case 4:
			isSocketError = getClientToServerDelayEstimation(sendBuff, recvBuff, &connSocket, &server);
			if (isSocketError == SOCKET_ERROR)
			{
				return;
			}
			break;
		case 5:
			isSocketError = measureRTT(sendBuff, recvBuff, &connSocket, &server);
			if (isSocketError == SOCKET_ERROR)
			{
				return;
			}
			break;
		case 12:
			isSocketError = getTimeWithoutDateInCity(sendBuff, &connSocket, &server);
			if (isSocketError == SOCKET_ERROR)
			{
				return;
			}
			break;
		case 14:
			toContinue = false;
			break;
		default:
			cout << "Please enter your choice from 1 to 14" << endl;
		}

		system("pause");		
		system("cls");
		/*if (toContinue)
		{
			
		}*/
	}

	// Closing connections and Winsock.
	cout << "Time Client: Closing Connection.\n";
	closesocket(connSocket);

	//system("pause");
}

void sendRequestToServerAndPrintItsAnswerToUser(char* sendBuff, char* recvBuff, SOCKET* connSocket, sockaddr_in* server)
{
	int bytesSent = 0;
	int bytesRecv = 0;


	// The send function sends data on a connected socket.
	// The buffer to be sent and its size are needed.
	// The fourth argument is an idicator specifying the way in which the call is made (0 for default).
	// The two last arguments hold the details of the server to communicate with. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).

	bytesSent = sendRequestToServer(sendBuff, connSocket, server);

	if (bytesSent == SOCKET_ERROR)
	{
		return;
	}
	/*else
	{
		cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}*/

	bytesRecv = getAnswerFromServer(recvBuff, connSocket);

	if (bytesSent == SOCKET_ERROR)
	{
		return;
	}
	else
	{
		//cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
		cout << recvBuff << endl << endl;
	}
}

void showMenu()
{
	cout << "Please choose your option. type your choice from 1 to 14" << endl;
	cout << "1  - get time" << endl;
	cout << "2  - get time without date" << endl;
	cout << "3  - get time since epoch" << endl;
	cout << "4  - get client to server delay estimation." << endl;
	cout << "5  - get measured RTT" << endl;
	cout << "6  - get time without date or seconds" << endl;
	cout << "7  - get year" << endl;
	cout << "8  - get month and day" << endl;
	cout << "9  - get seconds since beginning of month " << endl;
	cout << "10 - get week of year" << endl;
	cout << "11 - get daylight savings" << endl;
	cout << "12 - get time without date in city" << endl;
	cout << "13 - get measured time lap" << endl;
	cout << "14 - Exit" << endl;
}

int measureRTT(char* sendBuff, char* recvBuff, SOCKET* connSocket, sockaddr_in* server)
{
	int isSocketError = FALSE;
	DWORD timeOfRequest = 0;
	DWORD timeOfResponse = 0;
	DWORD allRttTime = 0;

	for (int i = 0; i < 100; i++)
	{
		timeOfRequest = GetTickCount();
		isSocketError = sendRequestToServer(sendBuff, connSocket, server);

		if (isSocketError != SOCKET_ERROR)
		{
			isSocketError = getAnswerFromServer(recvBuff, connSocket);
			timeOfResponse = GetTickCount();
		}

		if (isSocketError == SOCKET_ERROR)
		{
			break;
		}

		allRttTime += (timeOfResponse - timeOfRequest);
	}

	cout << "The avg measured RTT is: " << (double)allRttTime/100 << " milliseconds." << endl;

	return isSocketError;
}

int getClientToServerDelayEstimation(char* sendBuff, char* recvBuff, SOCKET* connSocket, sockaddr_in* server)
{
	int isSocketError = 0;
	double avg;
	DWORD tickCounts[100];

	isSocketError = send100RequestsToServer(sendBuff, connSocket, server);

	if (isSocketError != SOCKET_ERROR)
	{
		isSocketError = get100AnswersFromServer(recvBuff, connSocket, tickCounts);
		if (isSocketError != SOCKET_ERROR)
		{
			avg = calculateDelayAvg(tickCounts);
			cout << "The client to server delay estimation is: " << avg << " milliseconds." << endl;
		}
	}

	return isSocketError;
}

double calculateDelayAvg(DWORD* tickCounts)
{
	double avg = 0;

	for (int i = 0; i < 99; i++)
	{		
		avg += (tickCounts[i + 1] - tickCounts[i]);
	}

	return avg / 99;
}

int getAnswerFromServer(char* recvBuff, SOCKET* connSocket)
{
	int bytesRecv = 0;

	// Gets the server's answer using simple recieve (no need to hold the server's address).
	bytesRecv = recv(*connSocket, recvBuff, BUFFER_SIZE, 0);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(*connSocket);
		WSACleanup();
	}
	else 
	{
		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
	}

	return bytesRecv;
}

int get100AnswersFromServer(char* recvBuff, SOCKET* connSocket, DWORD* tickCounts)
{
	int isSocketError = FALSE;

	for (int i = 0; i < 100; i++)
	{
		isSocketError = getAnswerFromServer(recvBuff, connSocket);
		if (isSocketError == SOCKET_ERROR)
		{
			break;
		}

		tickCounts[i] = atoi(recvBuff);
	}

	return isSocketError;
}

int sendRequestToServer(char* sendBuff, SOCKET* connSocket, sockaddr_in* server)
{
	int bytesSent = 0;

	bytesSent = sendto(*connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)server, sizeof(*server));
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
		closesocket(*connSocket);
		WSACleanup();
	}

	return bytesSent;
}

int send100RequestsToServer(char* sendBuff, SOCKET* connSocket, sockaddr_in* server)
{
	int isSocketError = FALSE;

	for (int i = 0; i < 100; i++)
	{
		if (sendRequestToServer(sendBuff, connSocket, server) == SOCKET_ERROR)
		{
			isSocketError = SOCKET_ERROR;
			break;
		}
	}

	return isSocketError;
}

void citiesMenu()
{
	system("cls");
	cout << "Please choose a city: " << endl;
	cout << "1. Qatar" << endl;
	cout << "2. Prague" << endl;
	cout << "3. New York" << endl;
	cout << "4. Berlin" << endl;
}

int getTimeWithoutDateInCity(char* sendBuff, SOCKET* connSocket, sockaddr_in* server) 
{
	int isSocketError = 0;
	char recvBuff[BUFFER_SIZE];
	char cityUserChoiceBuff[BUFFER_SIZE];

	isSocketError = sendRequestToServer(sendBuff, connSocket, server);
	if (isSocketError != SOCKET_ERROR)
	{
		citiesMenu();
		cin >> cityUserChoiceBuff;
		isSocketError = sendRequestToServer(cityUserChoiceBuff, connSocket, server);
		if (isSocketError != SOCKET_ERROR)
		{
			isSocketError = getAnswerFromServer(recvBuff, connSocket);

			if (isSocketError != SOCKET_ERROR)
			{
				cout << recvBuff << endl;
			}
		}	
	}
	
	return isSocketError;
}