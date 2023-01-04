#include <stdlib.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#define BUFF_SIZE 24
#define BUFFER_SIZE 4096

using namespace std;

int main(int argc, char *argv[])
{
        char readBuff[BUFFER_SIZE];
        char sendBuff[BUFFER_SIZE];

        struct sockaddr_in serverAddress, clientAddress;

        int server_fd, client_fd;
        int client_addr_size;

        ssize_t recsizeBytes;
        ssize_t sentBytes;

        socklen_t clientAddressLength = 0;

        memset(&serverAddress, 0, sizeof(serverAddress));
        memset(&clientAddress, 0, sizeof(clientAddress));

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr("192.168.0.79");
        serverAddress.sin_port = htons(8000);

        if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
                printf("Server : can not Open Socket\n");
                exit(0);
        }

        if (bind(server_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        {
                printf("Server : can not bind local address");
        }

        printf("Server : waiting connection request. \n");

        // Data structure 
        struct Data{
        double data1;
        double data2;
        double data3;
        }Data;  

        while(1)
        {
                struct sockaddr_in connectSocket;
                socklen_t connectSocketLength = sizeof(connectSocket);
                getpeername(client_fd, (struct sockaddr*)&clientAddress, &connectSocketLength);
                char clientIP[sizeof(clientAddress.sin_addr) + 1] = {0};
                sprintf(clientIP,"%s",inet_ntoa(clientAddress.sin_addr));
                recvfrom(server_fd, (char *)&Data, sizeof(Data), 0, (struct sockaddr*)&clientAddress, (socklen_t*)&client_addr_size);
                cout << Data.data1 << endl;
                cout << Data.data2 << endl;
                cout << Data.data3 << endl;
        }

        close(server_fd);

        return 0;
}