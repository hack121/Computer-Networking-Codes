#include<stdio.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(int argc, char *argv[]){
    char *address;
    address = argv[1];
    //create socket
    int client = socket(AF_INET, SOCK_STREAM, 0);
    //address
    struct sockaddr_in skt_addr;
    skt_addr.sin_family = AF_INET;
    skt_addr.sin_port = htons(80);
    inet_aton(address, &skt_addr.sin_addr.s_addr);
    //connect
    connect(client, (struct sockaddr*)&skt_addr, sizeof(skt_addr));
    //request
    char request[] = "GET / HTTP/1.0\r\n\r\n";
    //response
    char response[1<<16];
    //send
    send(client, request, sizeof(request), 0);
    //recieve and print
    printf("Data from server %ld B:\n %s\n", recv(client, &response, sizeof(response), 0), response);
    //close
    close(client);
    return 0;
}