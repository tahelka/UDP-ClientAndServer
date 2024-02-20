# UDP Client and Server in C/C++

## Introduction

This project demonstrates the implementation of a UDP (User Datagram Protocol) client and server in C++. UDP is a connectionless protocol that operates on top of IP (Internet Protocol) and provides a simple interface between hosts to send and receive datagrams.

## Features

### UDP Client

- **Client Interaction**: Allows users to interact with the server by sending requests and receiving responses.
- **Menu System**: Provides a menu interface for users to choose various options for sending requests to the server.
- **Delay Estimation**: Estimates the delay between the client and server by sending multiple requests and measuring response times.
- **Round-Trip Time (RTT) Measurement**: Measures the Round-Trip Time (RTT) by sending requests and measuring the time taken for responses.
- **City Time Query**: Enables users to query the server for the current time in a specific city.

### UDP Server

- **Server Response Handling**: Listens for incoming UDP datagrams from clients and responds to client requests accordingly.
- **Socket Creation**: Creates a UDP socket and binds it to a specified IP address and port to accept client connections.
- **Request Processing**: Processes client requests and generates appropriate responses based on the requested operation.
- **City Time Service**: Provides time information for various cities upon client request.

## Usage

### UDP Server

1. **Compile**: Compile the server code (`server.sln`) using a C++ compiler.
2. **Run**: Run the compiled executable.
3. **Server Startup**: The server will start listening for incoming requests on a specified port.
4. **Client Requests**: Process incoming requests from clients and generate appropriate responses.

### UDP Client

1. **Compile**: Compile the client code (`client.sln`) using a C++ compiler.
2. **Run**: Run the compiled executable.
3. **Interaction**: Choose options from the menu to send requests to the server and receive responses.
4. **Exit**: Select the exit option to terminate the client.

## Dependencies

- **Windows Sockets (Winsock)**: Both the client and server code utilize the Winsock library for socket programming on Windows.
