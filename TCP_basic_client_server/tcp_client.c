#include<stdio.h>
#include<stdlib.h>

#include<sys/socket.h>
#include<sys/types.h>

#include<netinet/in.h>

int main(){
    //create socket descriptor
    int nw_sock = socket(AF_INET, SOCK_STREAM, 0);

    //server address specifier
    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(9002);
    srv_addr.sin_addr.s_addr = INADDR_ANY;

    //estb. connection
    int conn = connect(nw_sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
    if (conn==-1){
        printf("Connection Failed!!!\n");
        return -1;
    }

    //recieve data from server
    char data_from_server[512];
    int recd = recv(nw_sock, &data_from_server, sizeof(data_from_server), 0);
    
    if (recd==-1){
        printf("Error occured while capturing pakets\n");
        return -1;
    }
    if (recd==0){
        printf("Seems ilike connection was closed\n");
        return 0;
    }

    //data read from socket
    printf("No. of bytes read: %d\n", recd);
    printf("Data read from socket :\n%s\n", data_from_server);

    //close socket connection
    close(nw_sock);

    return 0;
}