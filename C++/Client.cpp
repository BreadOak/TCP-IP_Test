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
        int client_socket;
        struct sockaddr_in serverAddress;
        int server_addr_size;    

        ssize_t receivedBytes;
        ssize_t sentBytes;

        memset(&serverAddress, 0, sizeof(serverAddress));

        serverAddress.sin_family = AF_INET;
        inet_aton("192.168.0.6", (struct in_addr*) &serverAddress.sin_addr.s_addr);
        serverAddress.sin_port = htons(8000);

        // 소켓 생성
        if ((client_socket = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
        {
                printf("socket 생성 실패\n");
                exit(0);
        }

        // Data structure 
        struct SendData{
        double sdata1;
        double sdata2;
        double sdata3;
        }sData;  

        sData.sdata1 = 10.10;
        sData.sdata2 = 9.9;
        sData.sdata3 = 8.8;

        struct RecvData{
        double rdata1;
        double rdata2;
        double rdata3;
        }rData;

        while(1)
        {
                server_addr_size = sizeof(serverAddress);   

                sendto(client_socket, (char *)&sData, sizeof(sData), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

                recvfrom(client_socket, (char *)&rData, sizeof(rData), 0, (struct sockaddr*)&serverAddress, (socklen_t*)&server_addr_size);

                cout << rData.rdata1 << endl;
                cout << rData.rdata2 << endl;
                cout << rData.rdata3 << endl;

        }

        close(client_socket);

        return 0;
}