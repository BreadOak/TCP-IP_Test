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

using namespace std;

int main(int argc, char *argv[])
{
        struct sockaddr_in serverAddress, clientAddress;

        int server_fd, client_fd;
        int client_addr_size;

        ssize_t recsizeBytes;
        ssize_t sentBytes;

        socklen_t clientAddressLength = 0;

        memset(&serverAddress, 0, sizeof(serverAddress));
        memset(&clientAddress, 0, sizeof(clientAddress));

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr("0.0.0.0");
        serverAddress.sin_port = htons(0000);

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
        struct SendData{
        double sdata1;
        double sdata2;
        double sdata3;
        }sData;  

        sData.sdata1 = 1.1;
        sData.sdata2 = 2.2;
        sData.sdata3 = 3.3; 

        struct RecvData{
        double rdata1;
        double rdata2;
        double rdata3;
        }rData;  

        while(1)
        {
                struct sockaddr_in connectSocket;
                socklen_t connectSocketLength = sizeof(connectSocket);
                getpeername(client_fd, (struct sockaddr*)&clientAddress, &connectSocketLength);
                char clientIP[sizeof(clientAddress.sin_addr) + 1] = {0};
                sprintf(clientIP,"%s",inet_ntoa(clientAddress.sin_addr));

                client_addr_size = sizeof(clientAddress);

                recvfrom(server_fd, (char *)&rData, sizeof(rData), 0, (struct sockaddr*)&clientAddress, (socklen_t*)&client_addr_size);

                cout << rData.rdata1 << endl;
                cout << rData.rdata2 << endl;
                cout << rData.rdata3 << endl;

                sendto(server_fd, (char *)&sData, sizeof(sData), 0, (struct sockaddr*)&clientAddress, sizeof(clientAddress));

        }

        close(server_fd);

        return 0;
}
