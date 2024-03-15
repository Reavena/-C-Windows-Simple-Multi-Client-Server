#include <stdio.h>
#include <winsock2.h>

#define PORT 4444

int main() {
    WSADATA wsa;
    SOCKET clientSocket; //Client socket descriptor
    struct sockaddr_in serverAddr;
    char buffer[1024];

    // Initialize Winsock v2.2
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[-]Failed to initialize Winsock.\n");
        return 1;
    }

    // Create a socket          iPv4          TCP type          default
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        printf("[-]Error in connection.\n");
        WSACleanup();
        return 1;
    }
    printf("[+]Client Socket is created.\n");

    // Set up server address structure
    memset(&serverAddr, 0, sizeof(serverAddr)); // //initialize all serverAddr bytes to 0
    serverAddr.sin_family = AF_INET;                        //set to iPv4
    serverAddr.sin_port = htons(PORT);              //set port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //loopback adress for local machine

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {//(struct sockaddr*)&serverAddr: A pointer to the server's address structure, cast to a generic socket address structure
        printf("[-]Error in connection.\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    printf("[+]Connected to Server.\n");

    // Client interaction loop
    while (1) {
        printf("Client: "); // Prompt the user to enter a message
       //scanf("%s", buffer);
       fgets(buffer,sizeof(buffer),stdin );// Reads a string from the user and stores it in the buffer variable
        send(clientSocket, buffer, strlen(buffer), 0); //Send client's message to server

        if (strncmp(buffer, "/exit",strlen("/exit")) == 0)   {
            closesocket(clientSocket);
            printf("[-]Disconnected from server.\n");
            break;
        }

        // Receive and display the server's response
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0); //Receive data from server
        if (bytesReceived <= 0) {
            printf("[-]Error in receiving data.\n");
            break;
        } else {
            buffer[bytesReceived-1] = '\0';// Null-terminate the received data to become string and remove enter
            printf("Server: %s\n", buffer);
        }
    }

    // Close the client socket and clean up
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
