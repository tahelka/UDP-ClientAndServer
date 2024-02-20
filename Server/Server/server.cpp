
#include "server.h"

void main()
{
	// Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows 
	// Sockets implementation.
	WSAData wsaData;

	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}

	// Server side:
	// Create and bind a socket to an internet address.

	// After initialization, a SOCKET object is ready to be instantiated.

	// Create a SOCKET object called m_socket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							datagram sockets (SOCK_DGRAM), 
	//							and the UDP/IP protocol (IPPROTO_UDP).
	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The "if" statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a server to communicate on a network, it must first bind the socket to 
	// a network address.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;
	// Address family (must be AF_INET - Internet address family).
	serverService.sin_family = AF_INET;
	// IP address. The sin_addr is a union (s_addr is a unsigdned long (4 bytes) data type).
	// INADDR_ANY means to listen on all interfaces.
	// inet_addr (Internet address) is used to convert a string (char *) into unsigned int.
	// inet_ntoa (Internet address) is the reverse function (converts unsigned int to char *)
	// The IP address 127.0.0.1 is the host itself, it's actually a loop-back.
	serverService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");
	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order (which is big-endian).
	serverService.sin_port = htons(TIME_PORT);

	// Bind the socket for client's requests.

	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	// Waits for incoming requests from clients.

	// Send and receive data.
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[BUFFER_SIZE];
	char recvBuff[BUFFER_SIZE];
	int isSocketError = 0;
	int didMeasure = FALSE;
	int measuredSeconds = 0;

	// Get client's requests and answer them.
	// The recvfrom function receives a datagram and stores the source address.
	// The buffer for data to be received and its available size are 
	// returned by recvfrom. The fourth argument is an idicator 
	// specifying the way in which the call is made (0 for default).
	// The two last arguments are optional and will hold the details of the client for further communication. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
	cout << "Time Server: Wait for clients' requests.\n";

	while (true)
	{
		bytesRecv = recvfrom(m_socket, recvBuff, BUFFER_SIZE, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

		initMeasureIfNeeded(&didMeasure, &measuredSeconds);
		isSocketError = executeClientsRequest(recvBuff, sendBuff, &m_socket, &client_addr, 
			&client_addr_len, &didMeasure, &measuredSeconds);

		if (isSocketError == SOCKET_ERROR)
		{
			return;
		}
		
		// Sends the answer to the client, using the client address gathered
		// by recvfrom. 
		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}

int executeClientsRequest(char* recvBuff, char* sendBuff, SOCKET* m_socket, sockaddr* client_addr, 
	int* client_addr_len, int* didMeasure, int* measuredSeconds)
{
	int isSocketError = 0;

	switch (atoi(recvBuff))
	{
	case 1:
		// Answer client's request by the current time.
		getTime(sendBuff);
		break;
	case 2:
		getTimeWithoutDate(sendBuff);
		break;
	case 3:
		getTimeSinceEpoch(sendBuff);
		break;
	case 4:
	case 5:
		getTickCountToSendBuff(sendBuff);
		break;
	case 6:
		getTimeWithoutDateOrSeconds(sendBuff);
		break;
	case 7:
		getYear(sendBuff);
		break;
	case 8:
		getMonthAndDay(sendBuff);
		break;
	case 9:
		getSecondsSinceBeginningOfMonth(sendBuff);
		break;
	case 10:
		getWeekOfYear(sendBuff);
		break;
	case 11:
		getDaylightSavings(sendBuff);
		break;
	case 12:
		isSocketError = getTimeWithoutDateInCity(sendBuff, m_socket, client_addr, client_addr_len);
		break;
	case 13:
		measureTimeLap(sendBuff, didMeasure, measuredSeconds);
		break;
	default:
		strcpy(sendBuff, "request is not supported");
	}
	
	return isSocketError;
}

void getTime(char* sendBuff)
{
	// Get the current time.
	time_t timer;

	time(&timer);
	// Parse the current time to printable string.
	strcpy(sendBuff, "The time is: ");
	strcat(sendBuff, ctime(&timer));
	sendBuff[strlen(sendBuff) - 1] = '\0'; //to remove the new-line from the created string
}

void getTimeWithoutDate(char* sendBuff)
{
	time_t timer;

	time(&timer);
	tm* localTime = localtime(&timer);
	strftime(sendBuff, BUFFER_SIZE, "The Time without date is: %H:%M:%S", localTime);
}

void getTimeSinceEpoch(char* sendBuff)
{
	// Get the current time.
	time_t timer;

	time(&timer);
	snprintf(sendBuff, BUFFER_SIZE, "The time since epoch is: %ld", timer);
}

void getTickCountToSendBuff(char* sendBuff)
{
	DWORD count = GetTickCount();
	snprintf(sendBuff, BUFFER_SIZE, "%lu", count);
}

void getTimeWithoutDateOrSeconds(char* sendBuff)
{
	time_t timer;
	time(&timer);
	tm* currTime = localtime(&timer);
	strftime(sendBuff, BUFFER_SIZE, "The time without date or seconds is: %H:%M", currTime);
}

void getYear(char* sendBuff)
{
	time_t timer;
	time(&timer);
	tm* currTime = localtime(&timer);
	strftime(sendBuff, BUFFER_SIZE, "The current year is: %Y", currTime);
}

void getMonthAndDay(char* sendBuff)
{
	time_t timer;
	time(&timer);
	tm* currTime = localtime(&timer);
	strftime(sendBuff, BUFFER_SIZE, "The month and day are: %B %d", currTime);
}

void getSecondsSinceBeginningOfMonth(char* sendBuff)
{
	time_t timer;
	int totalSecondsOnOtherDays; // total secs on other days of this months in total
	int totalSecondsToday;
	int totalSecondsSinceBeginningOfMonth;

	time(&timer);
	tm* currTime = localtime(&timer);
	totalSecondsOnOtherDays = (currTime->tm_mday - 1) * 60 * 60 * 24;
	totalSecondsToday = currTime->tm_sec + (currTime->tm_min * 60) + (currTime->tm_hour * 60 * 60);
	totalSecondsSinceBeginningOfMonth = totalSecondsToday + totalSecondsOnOtherDays;

	snprintf(sendBuff, BUFFER_SIZE, "The seconds since beginning of month is: %d", totalSecondsSinceBeginningOfMonth);
}

void getWeekOfYear(char* sendBuff)
{
	time_t timer = time(0);
	tm* currTime = localtime(&timer);
	strftime(sendBuff, BUFFER_SIZE, "The week of year is %U.", currTime);
}

void getDaylightSavings(char* sendBuff)
{
	snprintf(sendBuff, BUFFER_SIZE, "%d", isDaylightSavings());
}

int isDaylightSavings()
{
	time_t timer = time(0);
	tm* currTime = localtime(&timer);
	return (currTime->tm_isdst ? 1 : 0);
}

int getTimeWithoutDateInCity(char* sendBuff, SOCKET* m_socket, sockaddr* client_addr, int* client_addr_len)
{
	char chosenCityBuffer[BUFFER_SIZE];
	int isSummerTime;
	int addHours = 0;
	int isSocketError;
	int chosenCityIndex;
	SYSTEMTIME systemTime;
	const char* cityName;

	isSocketError = recvFromClient(m_socket, chosenCityBuffer, client_addr, client_addr_len);
	if (isSocketError != SOCKET_ERROR)
	{
		chosenCityIndex = atoi(chosenCityBuffer);
		GetSystemTime(&systemTime);
		isSummerTime = isDaylightSavings();

		switch (chosenCityIndex) {
		case 1:
			cityName = "Doha, Qatar";
			addHours = 3;
			break;
		case 2:
			cityName = "Prague, Czechia";
			if (isSummerTime)
			{
				addHours = 2;
			}
			else
			{
				addHours = 1;
			}

			break;
		case 3:
			cityName = "New York, USA";
			if (isSummerTime)
			{
				addHours = -4;
			}
			else
			{
				addHours = -5;
			}
			break;
		case 4:
			cityName = "Berlin, Germany";
			if (isSummerTime)
			{
				addHours = 2;
			}
			else
			{
				addHours = 1;
			}
			break;
		default:
			cityName = "UTC";
			break;
		}
		systemTime.wHour += addHours;
		snprintf(sendBuff, BUFFER_SIZE, "%s %02d:%02d:%02d", cityName, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
	}
	
	return isSocketError;
}

int recvFromClient(SOCKET* m_socket, char* recvBuff, sockaddr* client_addr, int* client_addr_len)
{
	int bytesRecv = 0;

	bytesRecv = recvfrom(*m_socket, recvBuff, BUFFER_SIZE, 0, client_addr, client_addr_len);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
		closesocket(*m_socket);
		WSACleanup();
	}
	else
	{
		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
	}

	return bytesRecv;
}

void measureTimeLap(char* sendBuff, int* didMeasure, int* measuredSeconds)
{
	time_t timer;
	time(&timer);

	if (*didMeasure)
	{
		*didMeasure = FALSE;
		int measuredTimeLap = static_cast<int>(timer) - *measuredSeconds;
		sprintf(sendBuff, "The measured time lap is: %d seconds", measuredTimeLap);
	}
	else
	{
		*didMeasure = TRUE;
		*measuredSeconds = static_cast<int>(timer);
		strcpy(sendBuff, "Measuring has started!");
	}
}

void initMeasureIfNeeded(int* didMeasure, int* measuredSeconds)
{
	time_t timeLap;

	time(&timeLap);
	if (*didMeasure)
	{
		if (static_cast<int>(timeLap) - *measuredSeconds >= 180)
			*didMeasure = FALSE;
	}
}