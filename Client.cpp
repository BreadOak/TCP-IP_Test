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

#define BUFF_SIZE 100
#define BUFFER_SIZE 4096

using namespace std;

int main(int argc, char *argv[])
{
        int client_socket;
        struct sockaddr_in serverAddress;
        int server_addr_size;
        char sendBuff[BUFF_SIZE];
        char readBuff[BUFF_SIZE];     

        ssize_t receivedBytes;
        ssize_t sentBytes;

        memset(&serverAddress, 0, sizeof(serverAddress));

        serverAddress.sin_family = AF_INET;
        inet_aton("0.0.0.0", (struct in_addr*) &serverAddress.sin_addr.s_addr); // Your Addr Name
        serverAddress.sin_port = htons(Portname); // Your Port Name

        // 소켓 생성
        if ((client_socket = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
        {
                printf("socket 생성 실패\n");
                exit(0);
        }

        // Data structure 
        struct Data{
        double data1;
        double data2;
        double data3;
        }Data;  

        Data.data1 = 0;
        Data.data2 = 5.5;
        Data.data3 = 39;

        while(1)
        {
                server_addr_size = sizeof(serverAddress);   
                sendto(client_socket, (char *)&Data, sizeof(Data), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
                cout << Data.data1 << endl;
                cout << Data.data2 << endl;
                cout << Data.data3 << endl;

        }

}
