#include<stdio.h>
#include<stdlib.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

int main(){
    //create socket
    int srv_sock = socket(AF_INET, SOCK_STREAM, 0);
    //server address
    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(9002);
    srv_addr.sin_addr.s_addr = INADDR_ANY;

    //bind address to socket
    if (bind(srv_sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr))==-1){
        printf("Error binding socket to address");
        return -1;
    }

    //listen for connections
    if (listen(srv_sock, 5)==-1){
        printf("Error listening to connections");
        return -1;
    }

    //accept connections
    int client_sock = accept(srv_sock, NULL, NULL);

    char data_to_client[256] = "From server, with love";
    //send data to client
    send(client_sock, data_to_client, sizeof(data_to_client), 0);

    //close socket
    close(srv_sock);
    return 0;
}