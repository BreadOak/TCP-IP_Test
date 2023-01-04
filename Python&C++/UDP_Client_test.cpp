#include <stdio.h> 
#include <stdlib.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <iostream>
#define PORT 8000

using namespace std;

int main(int argc, char const *argv[]) 
{ 
    int sock = 0; 
    struct sockaddr_in serv_addr; 

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 

    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "Your IP address", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
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

    send(sock, (char *)&Data, sizeof(Data), 0);

    cout << "Data1:" << Data.data1 << endl;
    cout << "Data2:" << Data.data2 << endl;
    cout << "Data3:" << Data.data3 << endl;

    close(sock);
    printf("Message sent\n"); 

    return 0; 
} 
